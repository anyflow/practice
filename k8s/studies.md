# Studies

- `IP` 종류 in the point of view of `kubectl`
  - **IP**
    - TODO
  - **CLUSTER-IP**
    - TODO
  - **EXTERNAL-IP**
    - TODO
- **3rd party tools**
  - Prometheus
  - Istio
  - Helm
  - Envoy

# Commands

- 특정 pod bash에 접근

  ```bash
  kubectl exec -it <pod name> -- bash # kubectl exec --stdin --tty <pod name> -- /bin/bash
  ```

- network util pod 생성 및 shell 로그인

  ```bash
  kubectl run <netutil name> --rm -i --tty --image praqma/network-multitool -- bash
  ```

# DNS 서비스 validation

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

- **관련 reference** : `https://kubernetes.io/ko/docs/tasks/access-application-cluster/web-ui-dashboard/`

- **dashboard UI 배포**

  ```bash
  > kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.4.0/aio/deploy/recommended.yaml
  ...
  namespace/kubernetes-dashboard create
  serviceaccount/kubernetes-dashboard created
  service/kubernetes-dashboard created
  secret/kubernetes-dashboard-certs created
  secret/kubernetes-dashboard-csrf created
  secret/kubernetes-dashboard-key-holder created
  configmap/kubernetes-dashboard-settings created
  role.rbac.authorization.k8s.io/kubernetes-dashboard created
  clusterrole.rbac.authorization.k8s.io/kubernetes-dashboard created
  rolebinding.rbac.authorization.k8s.io/kubernetes-dashboard created
  clusterrolebinding.rbac.authorization.k8s.io/kubernetes-dashboard created
  deployment.apps/kubernetes-dashboard created
  service/dashboard-metrics-scraper created
  deployment.apps/dashboard-metrics-scraper created
  ```

- **정상 배포 확인**

  ```bash
  > kubectl get pod -n kubernetes-dashboard
  ...
  NAME                                        READY   STATUS    RESTARTS   AGE
  dashboard-metrics-scraper-c45b7869d-hq2cq   1/1     Running   0          45s
  kubernetes-dashboard-576cb95f94-56gxg       1/1     Running   0          45s
  ```

- **`ClusterRoleBinding` 생성 for admin 권한으로 접근**

  ```bash
  > kubectl create clusterrolebinding default-admin --clusterrole cluster-admin --serviceaccount=default:default
  ...
  clusterrolebinding.rbac.authorization.k8s.io/default-admin created
  ```

- **로그인 용 token 생성**

  ```bash
  > token=$(kubectl get secrets -o jsonpath="{.items[?(@.metadata.annotations['kubernetes\.io/service-account\.name']=='default')].data.token}"|base64 --decode)
  ...
  > echo $token
  ...
  eyJhbGciOiJSUzI1NiIsImtpZCI6InB0OHNlTWlTdGI5dUg1bzF6eGtSSEY4RjRQc2FMOTBLU3VZcGZEZVhpRUUifQ.eyJpc3MiOiJrdWJlcm5ldGVzL3NlcnZpY2VhY2NvdW50Iiwia3ViZXJuZXRlcy5pby9zZXJ2aWNlYWNjb3VudC9uYW1lc3BhY2UiOiJkZWZhdWx0Iiwia3ViZXJuZXRlcy5pby9zZXJ2aWNlYWNjb3VudC9zZWNyZXQubmFtZSI6ImRlZmF1bHQtdG9rZW4tNGdwdDIiLCJrdWJlcm5ldGVzLmlvL3NlcnZpY2VhY2NvdW50L3NlcnZpY2UtYWNjb3VudC5uYW1lIjoiZGVmYXVsdCIsImt1YmVybmV0ZXMuaW8vc2VydmljZWFjY291bnQvc2VydmljZS1hY2NvdW50LnVpZCI6IjU2Mzg3NWIxLTRlZWEtNGQ5ZS1hOTU5LWJmNWQ0OTIxODIzOCIsInN1YiI6InN5c3RlbTpzZXJ2aWNlYWNjb3VudDpkZWZhdWx0OmRlZmF1bHQifQ.ZlAkVuFa9y9ugtV1LoqTjmcjwB-s0i65n6Wcf4VzN0oXrhFmPTAqb0n1hmcFGWZzzNgI5G2TQeGndMyey0rFkT4zznQUFtH23Ibe9KI19mmV81urghtjFALinfnq7qHlxbO-0IwpOuTEIOy3Zz1mqbeGFhkeYUtocU1kM_dbSID1wEvIV_TSdDW52DB6N3qIJgEECfCZKhd4APvY39B8SedgJQ5LAQMXLVFaa7BaoVFrEQ4pVD6mkjpfCfndgU6PqnZvTKU1IbYkvqaOvSe4OEFNRwMr7LbM1o4LJDJz4xQdhlKldAeSdC4iGO6T112l2Zw2btqF8rUV_lEwFLmlqw
  ```

- **dashboard 접속 활성화**

  ```bash
  > kubectl proxy
  ```

- **dashboard URL (접근)**

  ```bash
  http://localhost:8001/api/v1/namespaces/kubernetes-dashboard/services/https:kubernetes-dashboard:/proxy/
  ```