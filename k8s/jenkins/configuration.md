
## 다루는 내용

- Jenkins에서 자체 source code 기반의 APISIX를 설치하는 방법

## 배포 대상 Source Code

- APISIX : <http://mod.lge.com/code/projects/MCA/repos/tcn-apisix/browse>
- APISIX Helm Chart : <http://mod.lge.com/code/projects/MCA/repos/tcn-apisix-helm-chart/browse>

## k8s용 주요 Jenkins Plugin

- [Docker Pipeline Plugin](https://github.com/jenkinsci/docker-workflow-plugin) : Jenkins plugin which allows building, testing, and using Docker images from Jenkins Pipeline projects.
- [Kubernetes Continuous Deploy Plugin](<https://github.com/jenkinsci/kubernetes-cd-plugin>) : A Jenkins plugin to deploy resource configurations to a Kubernetes cluster.

## References

- N/A
- [Jenkins에서 Kubernetes 플러그인을 이용해 보다 쉽고 효율적으로 성능 테스트하기](https://engineering.linecorp.com/ko/blog/performance-test-in-jenkins-run-dynamic-pod-executors-in-kubernetes-parallelly/)
- [[ Kube 49.1 ] Deploy to Kubernetes cluster using Jenkins CI/CD pipeline | Building with Kaniko tool](https://www.youtube.com/watch?v=YnZQJAMK6JI)
- [[FINDA] MSA를 위한 Kubernetes 세팅과 CI/CD Pipeline 구성, 그리고 Monitoring 시스템 구축 — 1](https://medium.com/finda-tech/finda-msa를-위한-kubernetes-세팅과-ci-cd-pipeline-구성-그리고-monitoring-시스템-구축-1-783bf49af15b)
- [[FINDA] MSA를 위한 Kubernetes 세팅과 CI/CD Pipeline 구성, 그리고 Monitoring 시스템 구축 — 2](https://medium.com/@bsc0227/finda-msa를-위한-kubernetes-세팅과-ci-cd-pipeline-구성-그리고-monitoring-시스템-구축-2-ef29380ec474)
