# 프로젝트 개요

## 1. 시스템이란?

loan은 **분산 로그 수집 · 버퍼링 · 분석 파이프라인**이다.

여러 서버에 흩어진 로그 파일을 실시간으로 수집하여 Protobuf로 직렬화하고,
중간 버퍼 계층을 거쳐 분석 엔진으로 전달한다.
전송 실패 시 Round-Robin / Fail-Over / Fail-Back 전략으로 고가용성을 보장한다.

---

## 2. 구성 서비스

| 서비스 | 바이너리 | 역할 |
|--------|---------|------|
| **loan_gether** | `loan_gether` | 로그 파일 tail → Protobuf 변환 → 전송 |
| **loan_buffer** | `loan_buffer` | 중간 버퍼링 계층 (수신 → 재전송) |
| **loan_analyzer** | `loan_analyzer` | 로그 수신 → 분석 → 파일/외부 서버 저장 |
| **loan_watchdog** | `loan_watchdog` | 서비스 상태 모니터링 |

---

## 3. 핵심 기능

### 실시간 로그 수집
`loan_gether`가 지정된 로그 파일을 tail 방식으로 모니터링한다.
새 줄이 추가되면 즉시 Protobuf `MsgLog` 메시지로 변환하여 전송한다.

### 고가용성 전송
세 가지 전송 전략으로 서버 장애에 대응한다.

```
Round-Robin  → 여러 서버에 순차 분배 (부하 분산)
Fail-Over    → Active 실패 시 Standby로 단방향 전환
Fail-Back    → Active 복구 시 Standby → Active 자동 복귀
Save-File    → 전송 실패 시 로컬 파일에 저장
```

### Protobuf 기반 통신
모든 서비스 간 통신은 `loan.proto`로 정의된 `MsgLog` 메시지를 사용한다.
바이너리 직렬화로 네트워크 오버헤드를 최소화한다.

### YAML 기반 설정
서버 주소, 전송 규칙, 로그 경로 등 모든 설정을 `conf.yaml`로 관리한다.
코드 수정 없이 운영 환경을 변경할 수 있다.

---

## 4. 전송 계층

Rigitaeda 라이브러리를 기반으로 동작한다.
TCP 소켓 관리, 비동기 수신, 세션 풀은 Rigitaeda가 담당하고,
loan은 그 위에서 프로토콜(Protobuf 0x17 구분자)과 비즈니스 로직을 구현한다.

```
loan (응용)
  └─ Rigitaeda (전송)
       └─ Boost.ASIO (epoll / IOCP)
```

---

## 5. 의존성

| 라이브러리 | 용도 |
|-----------|------|
| **Rigitaeda** | TCP 전송 계층 |
| **Boost.ASIO** | 비동기 I/O (Rigitaeda 내부) |
| **Protobuf** | 메시지 직렬화 |
| **yaml-cpp** | YAML 설정 파싱 |
| **glog** | 로깅 |
| **gflags** | 커맨드라인 인자 |
| **pthread** | 멀티스레딩 |
