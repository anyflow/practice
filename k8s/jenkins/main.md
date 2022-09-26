
# Install Jenkins in Kubernetes

## Why?

- **Why research Jenkins installation?**
  - 추후 T20 또는 인프라운영팀의 Jenkins를 사용할 수도 있지만, 예외 상황 발생 시 자체 Jenkins 필요
- **Why in Kubernetes?**
  - 빠른 Jenkins 설치를 위하여(EC2 등으로는 자동화를 위한 기술 습득 및 재사용면에서 상대 열위임)

## Constraints

- `kind` 기반의 local Kubernetes에서 테스트한 내용이나, 아래 `kind` 특화 사항을 제외한 EKS 등에서도 유효하리라 예상함

### `kind` 특화 사항

- **`jenkins-values.yaml`**
  - `serviceType: NodePort` (131 line) : EKS 등에서는 `LoadBalancer` 또는 ingress 사용
	- `nodePort: 30001` (132 line) : `NodePort` 사용 시에만 사용
	- (참고) `NodePort`를 그대로 유지하여도 ingress는 정상 동작함(따라서 ingress 사용 시 `ClusterIP` 등으로 변경 불필요)
- **`jenkins-ingress.yaml`**
    - `- host: "jenkins.local"` : localhost에서 호출 시, 동일 ingress 내에 타 domain 기반의 service와 분리를 위해 사용
    - `/etc/hosts` 파일에 `127.0.0.1	jenkins.local` 추가 필요

## 파일 설명

- **`kind-config.yaml`**
  - kind` k8s 클러스터 생성 시 사용하는 config 파일.
  - `localhost`의 30001 port와 Jenkins Service의 `NodePort` 30001이 매핑됨
  - node의 `hostroot`와 localhost의 `hostroot_in_node`가 매핑됨(for jenkins Persistent Volume)
- **`jenkins-volume.yaml`**
  - Jenkins Persistent Volume configuration
  - node의 `/hostroot/jenkins-volume/`에 jenkins 파일이 위치
- **`jenkins-sa.yaml`**
  - Service Account 및 RBAC 설정(role 생성 및 binding)
- **`jenkins-values.yaml`**
  - Helm install시 사용되는 values.
- **`jenkins-ingress.yaml`**
  - ingress 사용 시의 ingress resource

## Installation

### 기본 : `NodePort` 기반으로 jenkins를 호출할 경우(`localhost:30001`)

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
> helm install jenkins -n jenkins -f ./jenkins-values.yaml jenkinsci/jenkins
...
NAME: jenkins
LAST DEPLOYED: Mon Sep 26 13:20:55 2022
NAMESPACE: jenkins
STATUS: deployed
REVISION: 1
...
```

### 추가 : hostname(`jenkins.local`)으로 호출할 경우

```bash
# NGINX ingress controller 설치

> kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml
...
namespace/ingress-nginx created
serviceaccount/ingress-nginx created
serviceaccount/ingress-nginx-admission created
role.rbac.authorization.k8s.io/ingress-nginx created
role.rbac.authorization.k8s.io/ingress-nginx-admission created
clusterrole.rbac.authorization.k8s.io/ingress-nginx created
clusterrole.rbac.authorization.k8s.io/ingress-nginx-admission created
rolebinding.rbac.authorization.k8s.io/ingress-nginx created
rolebinding.rbac.authorization.k8s.io/ingress-nginx-admission created
clusterrolebinding.rbac.authorization.k8s.io/ingress-nginx created
clusterrolebinding.rbac.authorization.k8s.io/ingress-nginx-admission created
configmap/ingress-nginx-controller created
service/ingress-nginx-controller created
service/ingress-nginx-controller-admission created
deployment.apps/ingress-nginx-controller created
job.batch/ingress-nginx-admission-create created
job.batch/ingress-nginx-admission-patch created
ingressclass.networking.k8s.io/nginx created
validatingwebhookconfiguration.admissionregistration.k8s.io/ingress-nginx-admission created

# jenkins ingress resource 설치
> kubectl apply -f jenkins-ingress.yaml
...
ingress.networking.k8s.io/ingress-jenkins created
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
# NodePort 기반의 호출
> open -a safari localhost:31593/login

# ingress 기반 hostname으로 호출
> open -a safari jenkins.local/login
```

## References

- [Installing Jenkins in Kubernetes](https://www.jenkins.io/doc/book/installing/kubernetes/)
- [NGINX Ingress Controller - Rewrite](https://kubernetes.github.io/ingress-nginx/examples/rewrite/)