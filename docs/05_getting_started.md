# 시작하기 및 사용법

## 1. 빌드

### 요구사항

| 항목 | 버전 |
|------|------|
| g++ | 7+ (C++17) |
| CMake | 3.10+ |
| Boost | 1.66+ |
| Protobuf | 3.x |
| yaml-cpp | 0.6+ |
| glog / gflags | 최신 안정 버전 |

### 빌드 방법

각 서비스 디렉토리에서 독립적으로 빌드한다.

```bash
# loan_analyzer 빌드 예시
cd engine/eng/loan_analyzer
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Docker 환경

```bash
# 기본 이미지 빌드
docker build -t loan_base docker/base/loan_base/

# 빌드 이미지
docker build -t loan_crate docker/base/loan_crate/

# Docker Compose 실행
docker-compose -f docker/compose/docker-compose.yml up -d
```

---

## 2. 서비스 실행 순서

서비스 간 의존성에 맞게 다음 순서로 시작한다.

```
1. loan_analyzer   (분석 서버 — 먼저 Listen 상태여야 함)
2. loan_buffer     (버퍼 서버 — analyzer에 연결)
3. loan_gether     (수집기    — buffer 또는 analyzer에 연결)
```

---

## 3. loan_analyzer — 분석 서버

수신한 Protobuf 로그를 분석하여 파일 또는 외부 서버로 저장한다.

### 실행

```bash
./loan_analyzer --port=5555 --conf=conf.yaml
```

### conf.yaml 예시

```yaml
server:
  protocol: tcp
  port: 5555

send_rule: save-file
save_path: "/data/logs/[LOCAL_IP]/[CURRENT_TIME:%Y%m%d].log"
```

### main.cpp 구조 요약

```cpp
// 1. YAML 설정 로드
DATA_POLICY policy;
Conf_Yaml conf;
conf.Load_yaml("conf.yaml", policy);

// 2. 메시지 큐 생성
MsgLog_Q logQ;

// 3. 분석 서버 시작 (수신)
TCP_ServerMgr<TCP_Session> serverMgr;
serverMgr.Set_LogQ(&logQ);
serverMgr.Set_Policy(&policy);

Rigi_Server server(4096);

// 4. 이벤트: 수신 로그를 JSON 파일로 저장
serverMgr.Set_Event_Write([&](loan::MsgLog& msg) {
    std::string json = R"({"reg_dt":")" + GetCurrentTime() +
                       R"(","msg":")" + msg.log_contents() + R"("})";
    // 파일에 쓰기
});

// 5. 실행 (블로킹)
std::thread t([&]{ server.Run(policy.GetPort(), 100, &serverMgr); });
t.join();
```

---

## 4. loan_buffer — 버퍼 서버

수집기에서 받은 로그를 일시 보관 후 분석 서버로 재전송한다.
분석 서버 장애 시 데이터를 잃지 않도록 중간에 배치한다.

### 실행

```bash
./loan_buffer --conf=conf.yaml
```

### conf.yaml 예시

```yaml
server:
  protocol: tcp
  port: 3333          # loan_gether가 접속할 포트

send_rule: fail-over  # 분석 서버로 전송 전략
destination:
  active:
    - ip: 192.168.1.10
      port: 5555
  stand-by:
    - ip: 192.168.1.11
      port: 5555
```

### main.cpp 구조 요약

```cpp
// 수신 서버 (loan_gether → loan_buffer)
TCP_ServerMgr<TCP_Session> serverMgr;
serverMgr.Set_LogQ(&logQ);

// 전송 클라이언트 (loan_buffer → loan_analyzer)
TCP_ClientMgr clientMgr;
clientMgr.Set_LogQ(&logQ);
clientMgr.Add_Eng_FailOver_Active("192.168.1.10", "5555");
clientMgr.Add_Eng_FailOver_Standby("192.168.1.11", "5555");
clientMgr.Run();

Rigi_Server server(4096);
std::thread t([&]{ server.Run(3333, 500, &serverMgr); });
```

---

## 5. loan_gether — 로그 수집기

로그 파일을 실시간으로 tail하며 Protobuf로 변환하여 전송한다.

### 실행

```bash
./loan_gether --conf=conf.yaml
```

### conf.yaml 예시

```yaml
send_rule: round-robin
destination:
  - ip: 192.168.1.10
    port: 3333
  - ip: 192.168.1.11
    port: 3333

log_service:
  nginx:
    - path: /var/log/nginx/access.log
  app:
    - path: /var/log/myapp/app.log
    - path: /var/log/myapp/error.log
```

### main.cpp 구조 요약

```cpp
// 로그 파일별 tail 스레드 생성
for (auto& [service, paths] : policy.GetLogService()) {
    for (auto& path : paths) {
        threads.emplace_back([&] { Run_Tail(path, service, &logQ); });
    }
}

// Read_Tail: 파일 끝부터 새 줄 감지 → MsgLog 생성 → 큐에 삽입
void Read_Tail(const std::string& path, const std::string& service, MsgLog_Q* q) {
    // 파일 오프셋 추적
    // 새 줄 발견 시:
    loan::MsgLog msg;
    msg.set_service_name(service);
    msg.set_service_path(path);
    msg.set_log_contents(line);
    msg.set_log_time_seconds(time(nullptr));

    auto* raw = new std::string();
    msg.SerializeToString(raw);
    *raw += "\x17\x17";   // 구분자 추가
    q->Push_back(raw);
}

// 전송 클라이언트
TCP_ClientMgr clientMgr;
clientMgr.Set_LogQ(&logQ);
clientMgr.Add_Eng_RoundRobin("192.168.1.10", "3333");
clientMgr.Add_Eng_RoundRobin("192.168.1.11", "3333");
clientMgr.Run();
```

---

## 6. 커스텀 확장

### 수신 필터 추가

특정 조건의 로그만 처리하고 싶을 때:

```cpp
// TCP_Session에 필터 콜백 등록
session->Set_Event_Filter([](loan::MsgLog& msg) -> bool {
    // ERROR 레벨 로그만 처리
    return msg.log_contents().find("ERROR") != std::string::npos;
});
```

### 새 전송 전략 추가

`Session_Pool`을 상속하면 새 전략을 추가할 수 있다:

```cpp
class Session_Broadcast : public Session_Pool {
    // 모든 연결된 세션에 동일한 데이터 전송
    Rigi_ClientTCP* Get_Send_Session() override {
        // 브로드캐스트 구현
    }
};
```

### Protobuf 메시지 확장

`loan.proto`에 필드를 추가하고 재컴파일:

```bash
protoc --cpp_out=. loan.proto
```
