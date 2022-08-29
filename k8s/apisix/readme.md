# APISIX in KinD(k8s in Docker)

## 테스트케이스 목록

### APISIX 테스트 환경 구축

- KinD 클러스터 설치
- ingress controller 설치
- APISIX 설치
- mock test service 설치(http echo - foo, bar)
- APISIX를 통하지 않고 ingress로 mock test 정상 동작 확인 (flow : client -> ingress -> mock test)

### APISIX 테스트

- APISIX에 mock service의 route 설정
- APISIX를 통해 mock test 정상 동작 확인 (flow : client -> APSIX -> ingress -> mock test)


### APISIX Route 생성

```bash
curl "http://127.0.0.1:9080/apisix/admin/routes/2" -H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" -X PUT -d '
{
  "methods": ["GET"],
  "uri": "/foo",
  "upstream": {
    "type": "roundrobin",
    "nodes": {
      "foo-service:5000": 1
    }
  }
}'
```

```bash
curl "http://127.0.0.1:9080/apisix/admin/routes/3" -H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" -X PUT -d '
{
  "methods": ["GET"],
  "uri": "/bar",
  "upstream": {
    "type": "roundrobin",
    "nodes": {
      "bar-service:5000": 1
    }
  }
}'
```