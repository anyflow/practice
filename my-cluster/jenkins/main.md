
# Install Jenkins in Kubernetes

## Why?

- **Why research Jenkins installation?**
  - 추후 T20 또는 인프라운영팀의 Jenkins를 사용할 수도 있지만, 예외 상황 발생 시 자체 Jenkins 필요
- **Why in Kubernetes?**
  - 빠른 Jenkins 설치를 위하여(EC2 등으로는 자동화를 위한 기술 습득 및 재사용면에서 상대 열위임)

## Constraints

- `kind` 기반의 local Kubernetes에서 테스트한 내용이나, 아래 localhost 특화 사항을 제외한 EKS 등에서도 유효하리라 예상함

## 파일 설명

- **`../kind-config.yaml`**
  - kind k8s 클러스터 생성 시 사용하는 config 파일.
- **`manifest.yaml`**
  - **`namespace`**
    - `jenkins` namespace 생성
  - **`volume claim`**
    - Jenkins Persistent Volume claim
    - `kind-config.yaml`의 `extraMounts`의 `local-path-provisioner` 참고
- **`values.yaml`** 수정 사항
  - `ingress.enabled` : `true`
  - `ingress.hostName`: `jenkins.local`
  - `persistence.enabled` : `true`
  - `persistence.existingClaim` : `jenkins`
  - `persistence.storageClass` : `standard`
    - `local-path-provisioner`에 해당
  - `csrf.defaultCrumbIssuer.enabled`: `false`
    - 안하면 `java.lang.AssertionError: class hudson.security.csrf.DefaultCrumbIssuer is missing its descriptor` 오류가 container initiation에서 발생하며, (반복된 실패로 인해) 로딩이 오래 걸림.

## Installation

```bash
# namespace 생성
> kubectl apply -f ./jenkins/manifest.yaml
...

# Configure jenkins Helm
> helm repo add jenkinsci https://charts.jenkins.io
...
> helm repo update
...
> helm search repo jenkinsci # helm 정상 configurating 확인
...
NAME                    CHART VERSION   APP VERSION     DESCRIPTION
jenkinsci/jenkins       4.2.5           2.361.1         Jenkins - Build great things at any scale! The ...

# Jenkins 설치
> helm install jenkins -n jenkins -f ./values.yaml jenkinsci/jenkins
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
# ingress 기반 hostname으로 호출
> open -a safari http://jenkins.local/login
```
## k8s용 주요 Jenkins Plugin

- [Docker Pipeline Plugin](https://github.com/jenkinsci/docker-workflow-plugin) : Jenkins plugin which allows building, testing, and using Docker images from Jenkins Pipeline projects.
- [Kubernetes Continuous Deploy Plugin](<https://github.com/jenkinsci/kubernetes-cd-plugin>) : A Jenkins plugin to deploy resource configurations to a Kubernetes cluster.

## References

- [Installing Jenkins in Kubernetes](https://www.jenkins.io/doc/book/installing/kubernetes/)
- [NGINX Ingress Controller - Rewrite](https://kubernetes.github.io/ingress-nginx/examples/rewrite/)
- [Jenkins에서 Kubernetes 플러그인을 이용해 보다 쉽고 효율적으로 성능 테스트하기](https://engineering.linecorp.com/ko/blog/performance-test-in-jenkins-run-dynamic-pod-executors-in-kubernetes-parallelly/)
- [[ Kube 49.1 ] Deploy to Kubernetes cluster using Jenkins CI/CD pipeline | Building with Kaniko tool](https://www.youtube.com/watch?v=YnZQJAMK6JI)
- [[FINDA] MSA를 위한 Kubernetes 세팅과 CI/CD Pipeline 구성, 그리고 Monitoring 시스템 구축 — 1](https://medium.com/finda-tech/finda-msa를-위한-kubernetes-세팅과-ci-cd-pipeline-구성-그리고-monitoring-시스템-구축-1-783bf49af15b)
- [[FINDA] MSA를 위한 Kubernetes 세팅과 CI/CD Pipeline 구성, 그리고 Monitoring 시스템 구축 — 2](https://medium.com/@bsc0227/finda-msa를-위한-kubernetes-세팅과-ci-cd-pipeline-구성-그리고-monitoring-시스템-구축-2-ef29380ec474)
