# 컴포넌트 상세

## 1. MsgLog_Q — 스레드 안전 메시지 큐

`engine/lib/Common/MsgLog_Q.hpp`

수신 스레드(io_context)와 전송 스레드(m_thr_send) 사이의 버퍼다.
`std::deque<std::string*>` 기반이며 mutex로 보호한다.

```cpp
MsgLog_Q logQ;
logQ.Set_MaxQ(10000);   // 최대 크기 (초과 시 Push_back 거부)

// 수신 측 (io_context 스레드)
auto* msg = new std::string(protobuf_bytes);
logQ.Push_back(msg);    // 큐 뒤에 추가

// 전송 측 (m_thr_send 스레드)
std::string* msg = logQ.Pop_front();
if (msg) {
    send(msg);
    delete msg;
}
```

| 메서드 | 설명 |
|--------|------|
| `Push_back(msg)` | 큐 뒤에 추가. 가득 차면 drop |
| `Push_front(msg)` | 큐 앞에 추가. 전송 실패 시 재삽입용 |
| `Pop_front()` | 앞에서 꺼냄. 비어있으면 nullptr |
| `GetQ_Size()` | 현재 크기 |
| `QEmpty()` | 비어있는지 확인 |

**Push_front 패턴 (재삽입):**
```cpp
// TCP_ClientMgr 전송 스레드 내부
std::string* pLog = m_pLogQ->Pop_front();
bool sent = m_pSendSession->SendData(pLog);
if (!sent) {
    m_pLogQ->Push_front(pLog);  // 전송 실패 → 큐 앞에 돌려놓음
    pLog = nullptr;             // 소유권 이전
}
delete pLog;  // nullptr이면 no-op, 성공이면 해제
```

---

## 2. TCP_Session — 서버 세션

`engine/lib/Protobuf_Server/`

`Rigitaeda::Rigi_TCPSession`을 상속한다.
연결된 클라이언트(loan_gether, loan_buffer 등)로부터 데이터를 수신하고
Protobuf 메시지를 분리하여 `MsgLog_Q`에 넣는다.

```cpp
class TCP_Session : public Rigitaeda::Rigi_TCPSession {
    MsgLog_Q*  m_pLogQ;
    POLICY*    m_pPolicy;
    std::string m_strTempBuff;  // TCP 조각 누적 버퍼
};
```

### Split_Protobuf — 핵심 메서드

수신 버퍼에서 `0x17 0x17` 구분자를 찾아 완성된 Protobuf 메시지를 추출한다.

```
수신 데이터 흐름:

[TCP raw bytes]
      ↓
Split_Protobuf()
  → m_strTempBuff에 누적
  → 0x17 0x17 발견 시 앞부분 추출
  → Protobuf 역직렬화
  → 필터 적용 (Event_Receive_Filter 콜백)
  → MsgLog_Q::Push_back()
```

### 이벤트 오버라이드

```cpp
bool OnEvent_Init() override;   // 세션 초기화 (LogQ, Policy 연결)
void OnEvent_Receive(char* data, size_t len) override;  // 데이터 수신
void OnEvent_Close() override;  // 연결 종료
```

### 수신 필터 콜백

```cpp
// 서버 설정 시 수신 필터 등록
session->Set_Event_Filter([](loan::MsgLog& msg) -> bool {
    // true 반환 시 큐에 추가, false 반환 시 drop
    return msg.service_name() != "ignore_service";
});
```

---

## 3. TCP_ServerMgr — 서버 관리자

`engine/lib/Protobuf_Server/include/TCP_ServerMgr.hpp`

`Rigitaeda::Rigi_TCPServerMgr<T>`를 상속하는 템플릿 클래스다.
세션이 수락될 때 `MsgLog_Q`와 `POLICY`를 세션에 주입한다.

```cpp
TCP_ServerMgr<TCP_Session> serverMgr;
serverMgr.Set_LogQ(&logQ);
serverMgr.Set_Policy(&policy);

Rigi_Server server(4096);
server.Run(5555, 100, &serverMgr);  // 포트 5555, 최대 100 세션
```

---

## 4. TCP_ClientMgr — 클라이언트 관리자

`engine/lib/Protobuf_Client/`

`MsgLog_Q`에서 메시지를 꺼내 `Session_Pool`을 통해 외부 서버로 전송하는 핵심 컴포넌트.

### 스레드 구성

```
m_thr_conn (재연결 스레드)
  └─ 100ms마다 Session_Pool::Reconnect_DisConnect_Pool() 호출
     → 끊긴 TCP_Client들의 재접속 시도

m_thr_send (전송 스레드)
  └─ MsgLog_Q에서 Pop_front()
     → Session_Pool::Get_Send_Session() → ASync_Send()
     → 실패 시 Push_front()로 재삽입
     → 빈 큐면 1ms 대기
```

### 전송 규칙 설정

```cpp
TCP_ClientMgr clientMgr;
clientMgr.Set_LogQ(&logQ);
clientMgr.Set_Policy(&policy);

// Round-Robin
clientMgr.Add_Eng_RoundRobin("192.168.1.10", "5555");
clientMgr.Add_Eng_RoundRobin("192.168.1.11", "5555");

// Fail-Over
clientMgr.Add_Eng_FailOver_Active("192.168.1.10", "5555");
clientMgr.Add_Eng_FailOver_Standby("192.168.1.20", "5555");

// Fail-Back
clientMgr.Add_Eng_FailBack_Active("192.168.1.10", "5555");
clientMgr.Add_Eng_FailBack_Standby("192.168.1.20", "5555");

// 파일 저장
clientMgr.Set_SaveFile("/data/logs/backup.log");

clientMgr.Run();   // 두 스레드 시작
// ...
clientMgr.Stop();  // 스레드 정상 종료 (join)
```

### TCP_Client — Active / Standby 구분

```cpp
class TCP_Client : public Rigitaeda::Rigi_ClientTCP {
    bool m_bActive;   // true = Active, false = Standby
};
```

Fail-Over / Fail-Back에서 어떤 그룹에 속하는지 식별하는 데 사용한다.

---

## 5. POLICY — 정책 구조체

`engine/lib/Common/Policy.hpp`

YAML 설정을 파싱한 결과가 담기는 구조체.
서버 주소, 전송 규칙, 재접속 주기 등을 보관한다.

```cpp
POLICY policy;
// Conf_Yaml이 Load_yaml() 호출 시 자동으로 채워줌

policy.GetSendRule();            // "round-robin" / "fail-over" / "fail-back" / "save-file"
policy.GetRoundRobin_IP_Port();  // vector<pair<string,string>>
policy.GetFailOver_IP_Port();    // map<string, vector<...>> ("active", "stand-by")
policy.GetFailBack_IP_Port();    // map<string, vector<...>>
policy.Get_Retry_Time();         // 재접속 주기 (초)
```

`DATA_POLICY`는 `POLICY`를 상속하며 로그 파일 경로, 정규식 필터, 문자열 치환 규칙 등을 추가로 보관한다.

---

## 6. Protobuf MsgLog 메시지

`engine/lib/Common/protobuf/loan.proto`

모든 서비스 간 통신에서 사용하는 메시지 구조.

```protobuf
message MsgLog {
    string msg_type          = 1;  // 메시지 타입 (e.g. "MSG_TYPE_GEN")
    int32  msg_cmd           = 2;  // 명령 코드 (MsgLog_Cmd_Crolling 참조)
    int64  log_time_seconds  = 3;  // 로그 발생 시각 (Unix timestamp)
    int32  log_time_index    = 4;  // 같은 초 내 순서 번호
    string service_name      = 5;  // 서비스명 (e.g. "nginx")
    string service_path      = 6;  // 로그 파일 경로
    string log_contents      = 7;  // 실제 로그 내용
    string log_option        = 8;  // 추가 옵션
}
```

### 명령 코드 (msg_cmd)

```cpp
enum class MsgLog_Cmd_Crolling : int {
    SENDLING   = 1,  // 로그 전송 중
    STOP_REQU  = 2,  // 수집 중지 요청
    STOP_RESP  = 3,  // 수집 중지 응답
    START_REQU = 4,  // 수집 시작 요청
    START_RESP = 5,  // 수집 시작 응답
};
```

### 직렬화 / 역직렬화

```cpp
// 직렬화 (loan_gether 송신 측)
loan::MsgLog msg;
msg.set_service_name("nginx");
msg.set_log_contents(line);
msg.set_log_time_seconds(time(nullptr));

std::string* raw = new std::string();
msg.SerializeToString(raw);
// 구분자 추가 후 전송
*raw += "\x17\x17";
logQ.Push_back(raw);

// 역직렬화 (TCP_Session 수신 측)
loan::MsgLog msg;
msg.ParseFromString(protobuf_bytes);
```

---

## 7. loan_util — 범용 유틸리티

`engine/lib/Common/loan_util.hpp`

| 함수 | 설명 |
|------|------|
| `Get_LocalHostIP()` | 현재 호스트의 IP 주소 반환 |
| `GetCurrentTime(format)` | 현재 시각 포맷 문자열 반환 |
| `Replace_All(src, old, new)` | 문자열 전체 치환 |
| `Replace_Macro(src)` | `[LOCAL_IP]`, `[CURRENT_TIME:format]` 매크로 치환 |
| `OpenFile(path, locale, ofstream)` | 파일 열기 (UTF-8 로케일 지원) |

### 경로 매크로 예시

```yaml
# conf.yaml 파일 저장 경로에 매크로 사용 가능
save_path: "/data/logs/[LOCAL_IP]/[CURRENT_TIME:%Y%m%d].log"
# → "/data/logs/192.168.1.10/20241025.log"
```
