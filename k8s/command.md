# 유용한 명령어들

## Access pod

- 특정 pod bash에 접근

  ```bash
  kubectl exec -it <pod name> -- bash
  #or
  kubectl exec --stdin --tty <pod name> -- /bin/bash
  ```

- network util pod 생성 및 shell 로그인

  ```bash
  kubectl run <netutil name> --rm -i --tty --image praqma/network-multitool -- bash
  ```

## DNS 서비스 validation

- References : <https://kubernetes.io/docs/tasks/administer-cluster/dns-debugging-resolution/>

- dnsutils 설치

  ```bash
  > kubectl apply -f https://k8s.io/examples/admin/dns/dnsutils.yaml
  ```

- coreDNS가 정상 동작하는지 확인

  ```bash
  > kubectl exec -i -t dnsutils -- nslookup google.com
  ...
  Server:         10.96.0.10
  Address:        10.96.0.10#53

  Non-authoritative answer:
  Name:   google.com
  Address: 142.251.42.206

  > kubectl exec -i -t dnsutils -- nslookup kubernetes.default
  ...
  Server:         10.96.0.10
  Address:        10.96.0.10#53

  Name:   kubernetes.default.svc.cluster.local
  Address: 10.96.0.1
  ```
# Dashboard

- *관련 reference* : `https://kubernetes.io/ko/docs/tasks/access-application-cluster/web-ui-dashboard/`

- dashboard UI 배포

  ```bash
  kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.4.0/aio/deploy/recommended.yaml
  ```

- dashboard 접속 활성화

  ```bash
  kubectl proxy
  ```

- dashboard URL (접근)

  ```
  http://localhost:8001/api/v1/namespaces/kubernetes-dashboard/services/https:kubernetes-dashboard:/proxy/
  ```

- dashboard UI에 접근에 사용할 token 얻기

  ```bash
  kubectl -n kube-system describe secret $(kubectl -n kube-system get secret | awk '/^deployment-controller-token-/{print $1}') | awk '$1=="token:"{print $2}'
  ```

# TODO

- dashboard에 service, ingress, node 등의  항목이 나타나지 않는데, 이를 해결