
# Install Jenkins in Kubernetes

## Why?

- **Why research Jenkins installation?**
  - 추후 T20 또는 인프라운영팀의 Jenkins를 사용할 수도 있지만, 예외 상황 발생 시 자체 Jenkins 필요
- **Why in Kubernetes?**
  - 빠른 Jenkins 설치를 위하여(EC2 등으로는 자동화를 위한 기술 습득 및 재사용면에서 상대 열위임)

## Constraints

- `kind` 기반의 local Kubernetes에서 테스트한 내용이나, 아래 `kind` 특화 사항을 제외한 EKS 등에서도 유효하리라 예상함
- **`kind` 특화 사항** (esp. in `jenkins-values.yaml`)
    - `serviceType: NodePort` (131 line) : EKS 등에서는 `LoadBalancer` 또는 ingress 사용
  	- `nodePort: 31593` (132 line) : `NodePort` 사용 시에만 사용

## 파일 설명

- **`kind-config.yaml`**
  - kind` k8s 클러스터 생성 시 사용하는 config 파일.
  - `localhost`의 31593 port와 Jenkins Service의 `NodePort` 31593이 매핑됨
  - node의 `hostroot`와 localhost의 `hostroot_in_node`가 매핑됨(for jenkins Persistent Volume)
- **`jenkins-volume.yaml`**
  - Jenkins Persistent Volume configuration
  - node의 `/hostroot/jenkins-volume/`에 jenkins 파일이 위치
- **`jenkins-sa.yaml`**
  - Service Account 및 RBAC 설정(role 생성 및 binding)
- **`jenkins-values.yaml`**
  - Helm install시 사용되는 values.

## Installation

```bash
# namespace 생성
> kubectl create namespace jenkins
...
namespace/jenkins created

# Configure jenkins Helm
> helm repo add jenkinsci https://charts.jenkins.io
...
> helm repo update
...
> helm search repo jenkinsci # helm 정상 configurating 확인
...
NAME                    CHART VERSION   APP VERSION     DESCRIPTION
jenkinsci/jenkins       4.2.5           2.361.1         Jenkins - Build great things at any scale! The ...

# jenkins용 Persistent Volume 생성
> kubectl apply -f ./jenkins-volume.yaml
...
persistentvolume/jenkins-pv created

# Service Account 생성
> kubectl apply -f ./jenkins-sa.yaml
...
serviceaccount/jenkins created
clusterrole.rbac.authorization.k8s.io/jenkins created
clusterrolebinding.rbac.authorization.k8s.io/jenkins created

# Jenkins 설치
> chart=jenkinsci/jenkins
> helm install jenkins -n jenkins -f ./jenkins-values.yaml $chart
...
NAME: jenkins
LAST DEPLOYED: Mon Sep 26 13:20:55 2022
NAMESPACE: jenkins
STATUS: deployed
REVISION: 1
...

```

## Jenkins 로그인하기

### `admin` 계정 password 얻기

```bash
# 마지막의 '; echo'는 zsh 사용시 발생하는 PROMPT_EOL_MARK(%) 제거 용도임
> kubectl get secret -n jenkins jenkins -o jsonpath="{.data.jenkins-admin-password}" | base64 --decode; echo
...
m780cFwRlqNmDKrboT7VkA
```
### Web Browser에서 Jenkins 진입

- **ID**: admin
- **PASSWORD** : `m780cFwRlqNmDKrboT7VkA` (상기 과정에서 얻은 password)

```bash
# macOS - Safari의 경우. Web Browser에서 localhost:31593/login으로 직접 들어가도 됨.
> open -a safari localhost:31593/login
```

## References

- [Installing Jenkins in Kubernetes](https://www.jenkins.io/doc/book/installing/kubernetes/)

## Attached Files

- [`kind-config.yaml`](./kind-config.yaml)
- [`jenkins-volume.yaml`](./jenkins-volume.yaml)
- [`jenkins-sa.yaml`](./jenkins-sa.yaml)
- [`jenkins-values.yaml`](./jenkins-values.yaml)
