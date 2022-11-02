# API Gateway와 Service Mesh에 대한 자료, 단상

## Introduction

MSA(MicroService Architecture)와 Kubernetes 관점에서 API Gateway와 Service Mesh 모두 Cross cutting concerns 처리를 위한 모듈인 듯 한데, 이들 둘을 단일 아키텍처 내에서 어떻게 배치해야 하는지에 대한 단상이다. 구체적으로는 아래와 같은 질문에 대한 생각이 되겠다.

- 역할이 동일하면 둘 중 하나만 사용하면 안되는가? - 단순성 측면
- 둘 중 하나만 사용하면 무엇을 사용하는 것이 좋을까? - 효율성 측면
- 둘 모두가 필요한 경우는 무엇일까? - 실제 외부에서 그렇게 많이들 사용하는 듯 보여서리.
- Reference Architecture는?

## 차이점

### API Gateway

- 시스템 외부 - North South Traffic 관점

# Service Mesh

- 시스템 내부 - East West Traffic 관점



​

# API Gateway 제품 목록

​

# 자료s

​

- [Do I Need an API Gateway if I Use a Service Mesh?](<https://blog.christianposta.com/microservices/do-i-need-an-api-gateway-if-i-have-a-service-mesh/>)
- [API Gateway Identity Crisis](https://www.solo.io/blog/api-gateways-are-going-through-an-identity-crisis)
- [Challenges of Adopting Service Mesh in Enterprise Organizations](https://blog.christianposta.com/challenges-of-adopting-service-mesh-in-enterprise-organizations/)
- [Service mesh에 적합한 Ingress Gateway는 무엇일까 ?](https://binux.tistory.com/63)
- [https://blog.apilayer.com/service-mesh-vs-api-gateway/](https://blog.apilayer.com/service-mesh-vs-api-gateway)
- [How Is A Service Mesh Different From An API Gateway?](https://medium.com/code-factory-berlin/how-is-a-service-mesh-different-from-an-api-gateway-254397f39954)
