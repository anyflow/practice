- [다루는 내용](#다루는-내용)
- [kind란?](#kind란)
- [사전 준비](#사전-준비)
  - [1. Prerequisite : docker 설치](#1-prerequisite--docker-설치)
  - [2. kind 설치](#2-kind-설치)
  - [3. 첨부 파일 설치](#3-첨부-파일-설치)
- [k8s 클러스터, ingress controller, load balancer 설치하기](#k8s-클러스터-ingress-controller-load-balancer-설치하기)
- [k8s 클러스터 생성](#k8s-클러스터-생성)
- [NGINX (ingress controller) 설치하기](#nginx-ingress-controller-설치하기)
- [Metallb (load balancer) 설치하기](#metallb-load-balancer-설치하기)
- [ingress, Load balancer 테스트 준비하기](#ingress-load-balancer-테스트-준비하기)
- [ingress, load balancer 테스트하기](#ingress-load-balancer-테스트하기)
  - [ingress 테스트](#ingress-테스트)
  - [load balancer 테스트](#load-balancer-테스트)
- [클러스터 삭제하기](#클러스터-삭제하기)

## 다루는 내용

- **kind**가 무엇인지, 어떻게 사용하는지
- kind를 이용한 로컬 환경에서의 **multi-node**의 Kubernetes 클러스터 구성
- **LoadBalancer** 생성(**metallb**) 및 LoadBalancer 타입의 service 생성 및 동작 확인
- **ingress controller** 생성(**NGINX**) 및 **ingress** 생성, 동작 확인

## kind란?

<img src="https://d33wubrfki0l68.cloudfront.net/d0c94836ab5b896f29728f3c4798054539303799/9f948/logo/logo.png" alt="drawing" width="400"/>

(아마도) Kubernetes in Docker의 약자인 듯(공식 문서에 약자 풀이가 안보임). docker 컨테이너를 k8s node로 사용하여 docker만으로도 별도의 host machine없이 k8s 클러스터를 구성할 수 있도록 하는 도구.

- multi-node (HA 포함) 클러스터 구성 가능 (*minikube*는 불가능함에도)
- 별도 virtual machine 요구 없음 (오직 **docker**만 prerequisite임)
- 주요 OS 모두 지원 (Linux, macOS, Windows)
- 믿을만 함 (원래 k8s 그 자체를 테스트하기 위해 만들어졌다고)
- 공식 documentation : <https://kind.sigs.k8s.io>
- Github : <https://github.com/kubernetes-sigs/kind>

## 사전 준비

### 1. Prerequisite : docker 설치

- <https://docs.docker.com/get-docker/> 참고

### 2. kind 설치

- <https://github.com/kubernetes-sigs/kind#installation-and-usage> 참고 (Linux, macOS, Windows 모두 다 있음. 무지 간단)

### 3. 첨부 파일 설치

하기 첨부 파일 모두는 working directory에 위치해야 함

- **`kind-config.yaml`**
  - kind 클러스터 생성 시 사용할 configuration 파일
  - k8s가 올라갈 host machine 환경에 대한 설정 - 노드 개수/타입, 네트워크
  - 특히, 로컬에서의 테스트를 위한 전용 포트포워딩 설정이 중요
  - 세부 설정에 대한 설명은 파일 내 comment 참조

- **`deployment-http-echo.yaml`**
  - 2개의 http echo 서버(`foo`, `bar`) 컨테이너 pod 설정
  - http echo 서버는 사전 설정에 따라, 호출 시 단순히 `foo!`, `bar!`를 응답
  - 세부 설정에 대한 설명은 파일 내 comment 참조

- **`services.yaml`**
  - 상기 http echo 서버를 노출하기 위한 설정 (config type : `service`)
  - `foo-service`, `bar-service`
    - `ClusterIP` 타입(default)의 k8s 서비스
    - ***ingress***를 통해 80 port로 외부에 노출됨
  - `http-echo-service`
    - `LoadBalancer` 타입의 k8s 서비스
    - `LoadBalancer` 타입 특성 상 external IP로 접근 가능하지만, macOS, Windows에서는 *docker* network를 host에 노출하지 않으므로 여기서는 `NodePort`를 통해 외부에 노출 (port : 31593.<br/>***참고로 이 땜시 개고생함***. NodePort로 우회하는 전략을 설명하는 문서가 전무. 오직 hint만 kind 공식 문서에 있을 뿐. [macOS의 경우 docker network를 노출하는 방법](<https://www.thehumblelab.com/kind-and-metallb-on-mac/>)이 있긴 한데, 이 문서의 prerequisite인 tuntap 설치가 macOS에서는 이제 불가)
  - 세부 설정에 대한 설명은 파일 내 comment 참조

- **`ingress.yaml`**
  - `ingress` 규칙 설정
  - `/foo` path로 호출하면 `foo-service`가, `/bar` path로 호출하면 `bar-service`가 서비스됨
  - 세부 설정에 대한 설명은 파일 내 comment 참조

- **`metallib-configmap.yaml`**
  - load balancer 설정 : 외부 노출 주소 범위를 설정하는데, linux에서만 해당 주소 범위에 접근 가능.
  - 세부 설정에 대한 설명은 파일 내 comment 참조

## k8s 클러스터, ingress controller, load balancer 설치하기

- ingress controller로는 NGINX 사용
- load balancer로는 metallb 사용

## k8s 클러스터 생성

- `--config` 옵션으로 클러스터 설정 파일 지정하여 생성하기 (w/ 성공 output - 3개 노드 생성됨)

```bash
> kind create cluster --config ./kind-config.yaml
...
Creating cluster "kind" ...
 ✓ Ensuring node image (kindest/node:v1.21.1) 🖼
 ✓ Preparing nodes 📦 📦 📦
 ✓ Writing configuration 📜
 ✓ Starting control-plane 🕹️
 ✓ Installing CNI 🔌
 ✓ Installing StorageClass 💾
 ✓ Joining worker nodes 🚜
Set kubectl context to "kind-kind"
You can now use your cluster with:

kubectl cluster-info --context kind-kind

Not sure what to do next? 😅  Check out https://kind.sigs.k8s.io/docs/user/quick-start/
...
 > kubectl get nodes
...
NAME                 STATUS   ROLES                  AGE   VERSION
kind-control-plane   Ready    control-plane,master   18m   v1.21.1
kind-worker          Ready    <none>                 18m   v1.21.1
kind-worker2         Ready    <none>                 18m   v1.21.1
kind-worker3         Ready    <none>                 18m   v1.21.1
```

## NGINX (ingress controller) 설치하기

ingress controller 설치를 위해서는 클러스터에 관련 설정이 요구됨(`extraPortMappings`, `node-labels`. 여기서는 위에서 이미 설정함)

1. 설치 command (w/ 성공 output)

```bash
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
```

2. 내부 설정 완료 기다리기 (w/ 성공 output)

```bash
> kubectl wait --namespace ingress-nginx \
  --for=condition=ready pod \
  --selector=app.kubernetes.io/component=controller \
  --timeout=90s
...
pod/ingress-nginx-controller-bdf7bf984-vtkkc condition met
```

## Metallb (load balancer) 설치하기

1. metallb namespace 생성하기 (w/ 성공 output)

```bash
> kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/namespace.yaml
...
namespace/metallb-system created
```

2. metallb manifest 적용하기 (w/ 성공 output)

```bash
> kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/metallb.yaml
...
Warning: policy/v1beta1 PodSecurityPolicy is deprecated in v1.21+, unavailable in v1.25+
podsecuritypolicy.policy/controller created
podsecuritypolicy.policy/speaker created
serviceaccount/controller created
serviceaccount/speaker created
clusterrole.rbac.authorization.k8s.io/metallb-system:controller created
clusterrole.rbac.authorization.k8s.io/metallb-system:speaker created
role.rbac.authorization.k8s.io/config-watcher created
role.rbac.authorization.k8s.io/pod-lister created
role.rbac.authorization.k8s.io/controller created
clusterrolebinding.rbac.authorization.k8s.io/metallb-system:controller created
clusterrolebinding.rbac.authorization.k8s.io/metallb-system:speaker created
rolebinding.rbac.authorization.k8s.io/config-watcher created
rolebinding.rbac.authorization.k8s.io/pod-lister created
rolebinding.rbac.authorization.k8s.io/controller created
daemonset.apps/speaker created
deployment.apps/controller created
```

3. 내부 설정 완료 기다리기 (w/ 성공 output)

```bash
> kubectl get pods -n metallb-system --watch
...
NAME                          READY   STATUS    RESTARTS   AGE
controller-66445f859d-6pq8b   1/1     Running   0          2m1s
speaker-4mk7h                 1/1     Running   0          2m1s
speaker-8rjd9                 1/1     Running   0          2m1s
speaker-dptgm                 1/1     Running   0          2m1s
```

아래부터는 Linux에서만 유의미함(macOS, Windows에서는 docker network에 접근 불가하기 때문)

4. load balancer에서 사용하는 주소 풀 설정 (w/ 성공 output)

```bash
> docker network inspect -f '{{.IPAM.Config}}' kind
...
[{172.18.0.0/16  172.18.0.1 map[]} {fc00:f853:ccd:e793::/64  fc00:f853:ccd:e793::1 map[]}]
```

5. 상기 주소 결과로 `Updatemetallb-configmap.yaml`  address 섹션 수정

```yaml
      ...
      addresses:
      - 172.18.255.200-172.18.255.250
      ...
```

6. `metallb-configmap.yaml` 적용 (w/ 성공 output)

```bash
> kubectl apply -f ./metallb-configmap.yaml
...
configmap/config created
```

## ingress, Load balancer 테스트 준비하기

1. http-echo 서버인 4개 pod(foo 2개, bar 2개) 생성하기 (w/ 성공 output)

```bash
> kubectl apply -f ./deployment-http-echo.yaml
...
deployment.apps/foo created
deployment.apps/bar created
...
> kubectl get deployments
...
NAME             READY   UP-TO-DATE   AVAILABLE   AGE
bar   2/2     2            2           13m
foo   2/2     2            2           13m
...
> kubectl get pods
...
NAME                              READY   STATUS    RESTARTS   AGE
bar-565c58bc76-lv9zn   1/1     Running   0          14m
bar-565c58bc76-p2zt4   1/1     Running   0          14m
foo-7d77c84f46-lcwnq   1/1     Running   0          14m
foo-7d77c84f46-lh6qs   1/1     Running   0          14m
```


2. 서비스 생성하기 (w/ 성공 output)

```bash
> kubectl apply -f services.yaml
...
service/http-echo-service created
service/foo-service created
service/bar-service created
...
> kubectl get services
...
NAME                TYPE           CLUSTER-IP     EXTERNAL-IP      PORT(S)          AGE
bar-service         ClusterIP      10.96.130.5    <none>           5000/TCP         6s
foo-service         ClusterIP      10.96.104.8    <none>           5000/TCP         6s
http-echo-service   LoadBalancer   10.96.31.254   172.18.255.200   5678:31593/TCP   6s
kubernetes          ClusterIP      10.96.0.1      <none>           443/TCP          49m
```

3. ingress rule 생성하기 (w/ 성공 output)

```bash
> kubectl apply -f ./ingress.yaml
...
ingress.networking.k8s.io/ingress-foobar created
...
> kubectl get ingresses
...
NAME             CLASS    HOSTS   ADDRESS   PORTS   AGE
ingress-foobar   <none>   *                 80      6s
```

## ingress, load balancer 테스트하기

### ingress 테스트

- **확인 대상** : L7 기반(**http path**)으로 routing
- **테스트 방법** : 테스트 동일 hostname (L4) 임에도 path가 다른 (L7) `/foo`, `/bar`로 각기 호출 및 각기 다른 결과 발생 확인

```bash
> curl localhost/foo
...
foo!
...
> curl localhost/bar
...
bar!
```

### load balancer 테스트

- **확인 대상** : load balancer의 주 미션인 부하 분산 동작
- **테스트 방법** : 동일 hostname:port (L4) 로 반복 호출 시 각기 다른 결과 발생 확인

```bash
for _ in {1..10}; do
  curl localhost:31593
done
...
bar!
foo!
foo!
bar!
bar!
bar!
bar!
foo!
foo!
bar!
```

## 클러스터 삭제하기

- 생성 시 클러스터 name을 지정하지 않으면 default로 `kind`가 지정됨

```bash
> kind delete cluster
...
Deleting cluster "kind" ...
```
