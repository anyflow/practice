# My-Cluster

## Jenkins

- `worker0/app/jenkins` : Jenkins persistent storage in host
  - `/app/jenkins` : node container path of Jenkins
- static provisioning : cluster 삭제해도 해당 directory 삭제되지 않음.

- `./jenkins/persistent.yaml`
  - `kubectl delete...` 하면 해당 저장된 Jenkins 세팅(초기 설치 설정 - plugins 등)이 삭제됨. 다시 세팅하려면 매우 오래 걸림(1시간?)
  - `storage : 2Gi` 값은 cluster 내리고 수정해도 삭제되거나 오류발생하지 않음