# Access pod

- 특정 pod bash에 접근

  ```bash
  kubectl exec --stdin --tty <pod name> -- /bin/bash
  #or
  kubectl exec -it <pod name> -- bash
  ```

- network util pod 생성 및 shell 로그인

  ```bash
  kubectl run <netutil name> --rm -i --tty --image praqma/network-multitool -- bash
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