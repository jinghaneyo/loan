# 전송 전략 패턴

## 1. 개요

`Session_Pool` 인터페이스를 구현한 세 가지 전략과 파일 저장 모드를 제공한다.
`TCP_ClientMgr`이 `Session_Pool*`로 참조하기 때문에 전략을 런타임에 교체할 수 있다.

```
Session_Pool (인터페이스)
  ├─ Session_RoundRobin   부하 분산
  ├─ Session_FailOver     단방향 Active → Standby 전환
  └─ Session_FailBack     양방향 Active ↔ Standby 자동 전환
```

---

## 2. Session_Pool 인터페이스

```cpp
class Session_Pool {
public:
    // 연결 성공 시 호출
    virtual bool Add_SessionPool_Connected(Rigi_ClientTCP* session, bool bActive) = 0;
    // 연결 끊김 시 호출
    virtual bool Add_SessionPool_DisConnected(Rigi_ClientTCP* session, bool bActive) = 0;
    // 연결 풀에서 제거
    virtual bool Del_SessionPool_Connected(Rigi_ClientTCP* session, bool bActive) = 0;
    // 미연결 풀에서 제거
    virtual bool Del_SessionPool_DisConnected(Rigi_ClientTCP* session, bool bActive) = 0;
    // 다음에 데이터를 보낼 세션 반환
    virtual Rigi_ClientTCP* Get_Send_Session() = 0;
    // 끊긴 세션들 재접속 시도
    virtual bool Reconnect_DisConnect_Pool() = 0;
    virtual void Clear() = 0;
};
```

---

## 3. Round-Robin

### 동작 방식

연결된 세션 목록을 순환하며 하나씩 선택한다.
서버가 3대라면 메시지 1 → 서버A, 메시지 2 → 서버B, 메시지 3 → 서버C, 메시지 4 → 서버A 순서다.

```
[연결 풀]   서버A ── 서버B ── 서버C
                  ↑ 라운드로빈 순환
```

### 세션 관리

```
연결 성공 → Add_SessionPool_Connected()  → 연결 풀로 이동
연결 끊김 → Add_SessionPool_DisConnected() → 미연결 풀로 이동
재접속 성공 → Add_SessionPool_Connected() → 연결 풀로 복귀
```

### 설정 예시

```yaml
send_rule: round-robin
destination:
  - ip: 192.168.1.10
    port: 5555
  - ip: 192.168.1.11
    port: 5555
  - ip: 192.168.1.12
    port: 5555
```

```cpp
clientMgr.Add_Eng_RoundRobin("192.168.1.10", "5555");
clientMgr.Add_Eng_RoundRobin("192.168.1.11", "5555");
clientMgr.Add_Eng_RoundRobin("192.168.1.12", "5555");
```

---

## 4. Fail-Over

### 동작 방식

평소에는 Active 그룹만 사용한다.
Active 그룹 전체가 연결 불가 상태가 되면 Standby 그룹으로 전환한다.
Active가 복구되어도 Standby를 계속 사용한다 (단방향).

```
[정상]   데이터 → Active 서버 그룹 (Round-Robin)

[Active 장애]
         데이터 → Standby 서버 그룹 (자동 전환)
         ※ Active 복구 후에도 Standby 유지
```

### 구조

```
Session_FailOver
  ├─ Session_RoundRobin (Active 그룹)
  │     서버A, 서버B
  └─ Session_RoundRobin (Standby 그룹)
        서버C, 서버D
```

### 전환 조건

`Get_Send_Session()` 호출 시 Active 풀이 비어있으면 Standby로 전환.
`m_bActive` 플래그로 현재 상태를 추적한다.

### 설정 예시

```yaml
send_rule: fail-over
destination:
  active:
    - ip: 192.168.1.10
      port: 5555
  stand-by:
    - ip: 192.168.1.20
      port: 5555
```

```cpp
clientMgr.Add_Eng_FailOver_Active("192.168.1.10", "5555");
clientMgr.Add_Eng_FailOver_Standby("192.168.1.20", "5555");
```

---

## 5. Fail-Back

### 동작 방식

Fail-Over와 동일하게 Active 장애 시 Standby로 전환한다.
추가로 Active가 복구되면 자동으로 다시 Active로 복귀한다 (양방향).

```
[정상]        데이터 → Active 서버 그룹

[Active 장애] 데이터 → Standby 서버 그룹 (자동 전환)

[Active 복구] 데이터 → Active 서버 그룹  (자동 복귀)
```

### Fail-Over vs Fail-Back 비교

| 항목 | Fail-Over | Fail-Back |
|------|---------|---------|
| Active 장애 시 | Standby 전환 | Standby 전환 |
| Active 복구 시 | Standby 유지 | Active 복귀 |
| 사용 사례 | 수동 복구 환경 | 자동 복구 환경 |

### 설정 예시

```yaml
send_rule: fail-back
destination:
  active:
    - ip: 192.168.1.10
      port: 5555
  stand-by:
    - ip: 192.168.1.20
      port: 5555
```

```cpp
clientMgr.Add_Eng_FailBack_Active("192.168.1.10", "5555");
clientMgr.Add_Eng_FailBack_Standby("192.168.1.20", "5555");
```

---

## 6. Save-File (파일 저장)

전송 대신 로컬 파일에 기록한다.
네트워크 전송이 불가한 환경이나 로컬 백업용으로 사용한다.

```yaml
send_rule: save-file
save_path: "/data/logs/[LOCAL_IP]/[CURRENT_TIME:%Y%m%d].log"
```

```cpp
clientMgr.Set_SaveFile("/data/logs/backup.log");
```

경로에서 `[LOCAL_IP]`와 `[CURRENT_TIME:format]` 매크로를 사용할 수 있다.

---

## 7. 전략 선택 가이드

| 상황 | 권장 전략 |
|------|---------|
| 여러 서버에 부하 분산 | Round-Robin |
| Active/Standby 이중화, 수동 전환 | Fail-Over |
| Active/Standby 이중화, 자동 복구 | Fail-Back |
| 오프라인 환경, 로컬 저장 | Save-File |
| 분산 + 이중화 | Round-Robin(Active) + Fail-Over |

---

## 8. 재접속 동작

모든 전략에서 끊긴 세션은 `미연결 풀`로 이동하며,
`m_thr_conn` 스레드가 100ms마다 재접속을 시도한다.

```
재접속 스레드 루프:
  while (m_bRun_Thread) {
      sleep(100ms)
      pool->Reconnect_DisConnect_Pool()
        → 미연결 풀의 각 TCP_Client에 Connect() 시도
        → 성공 시 연결 풀로 이동
  }
```

재접속 주기는 `POLICY::Get_Retry_Time()`으로 조정할 수 있다.
