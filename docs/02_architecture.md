# 아키텍처 설계

## 1. 전체 데이터 흐름

```
┌─────────────────────────────────────────────────────────────────┐
│                         로그 소스                                 │
│   /var/log/app.log   /var/log/nginx.log   /var/log/syslog ...   │
└────────────────────────────┬────────────────────────────────────┘
                             │ tail (실시간 파일 모니터링)
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                       loan_gether                                │
│   파일 tail → MsgLog (Protobuf) 직렬화 → MsgLog_Q               │
│   전송 스레드: Q에서 꺼내 TCP 전송                                │
└────────────────────────────┬────────────────────────────────────┘
                             │ TCP (Protobuf + 0x17 0x17 구분자)
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                       loan_buffer  (선택)                        │
│   TCP 서버로 수신 → MsgLog_Q → TCP 클라이언트로 재전송             │
└────────────────────────────┬────────────────────────────────────┘
                             │ TCP
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                      loan_analyzer                               │
│   TCP 서버로 수신 → 필터링/분석 → Event_Write                     │
│                                     ↓                           │
│                          JSON 파일 저장 또는 외부 서버 전송        │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. 서비스 내부 구조

모든 서비스는 동일한 패턴을 따른다.

```
┌──────────────────────────────────────────────────────┐
│  서비스 (loan_gether / loan_buffer / loan_analyzer)   │
│                                                       │
│  ┌─────────────┐    ┌───────────┐    ┌─────────────┐ │
│  │ TCP_ServerMgr│    │ MsgLog_Q  │    │TCP_ClientMgr│ │
│  │ (수신)       │───▶│ (버퍼 큐) │───▶│ (전송)      │ │
│  └─────────────┘    └───────────┘    └─────────────┘ │
│         │                                   │         │
│   Rigitaeda                          Session_Pool     │
│   TCP 세션                      (RoundRobin/FailOver) │
└──────────────────────────────────────────────────────┘
```

### 스레드 구성

```
메인 스레드
  └─ io_context.run()  (Rigitaeda, 비동기 I/O)

TCP_ClientMgr 내부
  ├─ m_thr_conn   재연결 스레드 (100ms 주기로 끊긴 세션 재접속 시도)
  └─ m_thr_send   전송 스레드  (MsgLog_Q에서 꺼내 Session_Pool로 전송)

loan_gether 추가
  └─ Run_Tail 스레드들  (로그 파일별 1개씩, tail 방식 읽기)
```

---

## 3. 컴포넌트 관계도

```
POLICY ◄─── Conf_Yaml::Load_yaml()
  │
  ├─▶ TCP_ServerMgr<TCP_Session>      Rigitaeda::Rigi_TCPServerMgr 상속
  │       └─ TCP_Session               Rigitaeda::Rigi_TCPSession 상속
  │             └─ Split_Protobuf()    0x17 0x17 구분자로 메시지 분리
  │                   └─ MsgLog_Q::Push_back()
  │
  └─▶ TCP_ClientMgr
          ├─ Session_Pool (인터페이스)
          │     ├─ Session_RoundRobin
          │     ├─ Session_FailOver
          │     │     ├─ Session_RoundRobin (Active)
          │     │     └─ Session_RoundRobin (Standby)
          │     └─ Session_FailBack
          │           ├─ Session_RoundRobin (Active)
          │           └─ Session_RoundRobin (Standby)
          └─ MsgLog_Q::Pop_front() → SendPacket()
```

---

## 4. Protobuf 메시지 프레이밍

TCP는 스트림 프로토콜이므로 메시지 경계를 별도로 표시해야 한다.
loan은 **0x17 0x17** (두 바이트)를 구분자로 사용한다.

```
[Protobuf 직렬화 바이트...] [0x17][0x17] [다음 메시지...] [0x17][0x17]
```

`TCP_Session::Split_Protobuf()`가 수신 스트림을 이 구분자로 분리하고,
각 메시지를 역직렬화하여 `MsgLog_Q`에 넣는다.

> **참고:** Rigitaeda의 `Rigi_Codec_Delimiter({'\x17','\x17'})`와 동일한 방식이다.
> 향후 `Set_Codec`으로 교체하면 `Split_Protobuf()` 수동 버퍼 관리를 제거할 수 있다.

---

## 5. 고가용성 구조

```
[정상 상태]
  loan_gether ──▶ Active 서버 그룹 (Round-Robin)

[Active 전체 장애 — Fail-Over]
  loan_gether ──▶ Standby 서버 그룹 (자동 전환)
  ※ Active 복구되어도 Standby 유지

[Active 전체 장애 — Fail-Back]
  loan_gether ──▶ Standby 서버 그룹 (자동 전환)
  ※ Active 복구되면 다시 Active로 자동 복귀
```

---

## 6. 파일 구조

```
loan/
├── engine/
│   ├── eng/                      각 서비스 진입점
│   │   ├── common/
│   │   │   ├── Run.hpp/cpp       서비스 실행 조정자
│   │   │   ├── Conf_Yaml.hpp/cpp YAML 설정 파서
│   │   │   └── Data_Policy.hpp   확장 정책 구조체
│   │   ├── loan_analyzer/src/main.cpp
│   │   ├── loan_buffer/src/main.cpp
│   │   ├── loan_gether/src/main.cpp
│   │   └── loan_watchdog/
│   └── lib/                      공유 라이브러리
│       ├── Common/
│       │   ├── Policy.hpp        기본 정책 (IP, Port, 전송 규칙)
│       │   ├── MsgLog_Q.hpp      스레드 안전 메시지 큐
│       │   ├── loan_util.hpp/cpp 범용 유틸리티
│       │   └── protobuf/
│       │       ├── loan.proto    MsgLog 메시지 정의
│       │       └── loan.pb.h/cc  컴파일된 Protobuf
│       ├── Protobuf_Server/
│       │   ├── include/TCP_ServerMgr.hpp
│       │   ├── include/TCP_Session.hpp
│       │   └── src/TCP_Session.cpp
│       ├── Protobuf_Client/
│       │   ├── include/TCP_ClientMgr.hpp
│       │   ├── include/TCP_Client.hpp
│       │   └── src/TCP_ClientMgr.cpp
│       └── Send_to/
│           ├── include/Session_Pool.hpp      (추상)
│           ├── include/Session_RoundRobin.hpp
│           ├── include/Session_FailOver.hpp
│           ├── include/Session_FailBack.hpp
│           └── src/
└── docs/
```
