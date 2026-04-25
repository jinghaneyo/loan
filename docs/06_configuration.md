# YAML 설정 레퍼런스

모든 서비스는 `conf.yaml` 파일 하나로 동작을 제어한다.
`Conf_Yaml::Load_yaml()`이 파일을 파싱하여 `POLICY` / `DATA_POLICY` 구조체에 채운다.

---

## 1. 서버 설정

서비스가 Listen할 포트와 프로토콜을 지정한다.

```yaml
server:
  protocol: tcp    # tcp (현재 지원)
  port: 5555       # Listen 포트
```

---

## 2. 전송 규칙 (`send_rule`)

데이터를 어떻게 전송할지 결정한다.

```yaml
send_rule: round-robin   # round-robin | fail-over | fail-back | save-file
```

### round-robin

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

### fail-over

```yaml
send_rule: fail-over
destination:
  active:
    - ip: 192.168.1.10
      port: 5555
    - ip: 192.168.1.11
      port: 5555
  stand-by:
    - ip: 192.168.1.20
      port: 5555
```

### fail-back

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

### save-file

```yaml
send_rule: save-file
save_path: "/data/logs/[LOCAL_IP]/[CURRENT_TIME:%Y%m%d].log"
```

---

## 3. 재접속 주기

끊긴 서버에 재접속을 시도하는 주기를 설정한다.

```yaml
retry_connect_time: 5    # 단위: 초 (기본값 5)
```

---

## 4. 로그 서비스 설정 (`loan_gether` 전용)

수집할 로그 파일 경로를 서비스명으로 그룹화한다.

```yaml
log_service:
  nginx:                              # 서비스명 (MsgLog.service_name)
    - path: /var/log/nginx/access.log
    - path: /var/log/nginx/error.log
  application:
    - path: /var/log/myapp/app.log
  system:
    - path: /var/log/syslog
```

---

## 5. 로그 필터링 (`loan_analyzer` 전용)

### 정규식 필터

정규식에 매칭되는 로그만 처리한다.

```yaml
regex:
  - "ERROR.*database"
  - "WARN.*timeout"
```

### 서비스 필터

지정한 서비스의 로그만 처리한다.

```yaml
service_filter:
  - nginx
  - application
```

### 문자열 치환

로그 내용에서 특정 문자열을 치환한다.

```yaml
replace:
  "password=\\w+": "password=***"
  "token=[A-Za-z0-9]+": "token=***"
```

---

## 6. 파일 저장 경로 매크로

`save_path`에서 다음 매크로를 사용할 수 있다.

| 매크로 | 치환 결과 | 예시 |
|--------|---------|------|
| `[LOCAL_IP]` | 현재 호스트 IP | `192.168.1.10` |
| `[CURRENT_TIME:format]` | 현재 시각 (strftime 형식) | `[CURRENT_TIME:%Y%m%d]` → `20241025` |

```yaml
# 호스트별 날짜별 파일 분리
save_path: "/data/[LOCAL_IP]/[CURRENT_TIME:%Y]/[CURRENT_TIME:%m%d].log"
```

---

## 7. 전체 예시

### loan_gether 설정

```yaml
# loan_gether/conf.yaml
send_rule: round-robin
retry_connect_time: 3

destination:
  - ip: 192.168.1.10
    port: 3333
  - ip: 192.168.1.11
    port: 3333

log_service:
  nginx:
    - path: /var/log/nginx/access.log
    - path: /var/log/nginx/error.log
  myapp:
    - path: /var/log/myapp/app.log
```

### loan_buffer 설정

```yaml
# loan_buffer/conf.yaml
server:
  protocol: tcp
  port: 3333

send_rule: fail-over
retry_connect_time: 5

destination:
  active:
    - ip: 192.168.1.100
      port: 5555
  stand-by:
    - ip: 192.168.1.101
      port: 5555
```

### loan_analyzer 설정

```yaml
# loan_analyzer/conf.yaml
server:
  protocol: tcp
  port: 5555

send_rule: save-file
save_path: "/data/logs/[LOCAL_IP]/[CURRENT_TIME:%Y%m%d].log"

regex:
  - "ERROR"
  - "CRITICAL"

service_filter:
  - nginx
  - myapp
```

---

## 8. 설정 파싱 흐름

```
conf.yaml 파일
      ↓
Conf_Yaml::Load_yaml(path, policy)
  ├─ Parse_Server()      → policy.m_ServerInfo
  ├─ Parse_SendRule()    → policy.m_SendRule
  ├─ Parse_Destination() → policy.m_vecRoundRobin
  │                         policy.m_mapFailOver_IP_Port
  │                         policy.m_mapFailBack_IP_Port
  ├─ Parse_LogService()  → policy.m_mapLogService  (DATA_POLICY만)
  └─ Parse_Group()       → policy.m_vecFailOver_Change_Limit 등
```

파싱 결과는 `POLICY` 참조로 `TCP_ServerMgr`, `TCP_ClientMgr`, `Run`에 주입된다.
