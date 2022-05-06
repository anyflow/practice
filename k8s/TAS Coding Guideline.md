# TAS Coding Guideline

- [Description](#description)
- [Coding Guideline General](#coding-guideline-general)
- [Naming Guideline](#naming-guideline)
- [Guideline](#guideline)
- [아키텍처](#아키텍처)
    - [아키텍처링 주안점](#아키텍처링-주안점)
    - [`ThePackage` 아키텍처](#thepackage-아키텍처)
- [`ThePackage` 기본 구성요소 설명](#thepackage-기본-구성요소-설명)
    - [`endpoint`](#endpoint)
    - [`sanitize`](#sanitize)
    - [`bizlogic`](#bizlogic)
    - [`database`, `external`, `storage`](#database-external-storage)
- [Appendix : 왜 Guideline을? 결국 가독성(Readability) 땜시](#appendix--왜-guideline을-결국-가독성readability-땜시)

## Description

ThinQ App service Server(TAS)의 Coding Guideline이다.

## Coding Guideline General

- **나중의 '나'가 알아보도록 작성**
- 다른 이가 알아보도록 하는건 둘째치고, 내가 알아보도록. 단, 지금의 '나'가 아닌 나중의 '나'가.
- **의미론(Semantic) 상시 고려**
  - 주석 제거하고 코드로 설명하라도 결국 이 말임.
- **모든 코드는 refactoring이 완료된 상태로**
  - 코딩 마지막은 항상 refactoring으로(가독성은 사실 refactoring 과정에서 생긴다).
  - [임시] 각 파일은 200 line을 넘기지 않도록(상수 값을 이렇게 못박으면 안좋긴 하지만... 일단 현 시점에선)
- `public`, `private` 구분 (`protected` 사용 X)
- 외부 모듈(파일 포함)에서 참조하지 않는 모든 object는 `private`(`__` prefix)으로
- `protected`(단일 `_`)는 사실 상 무의미. 현 TAS 소스코드 구조 상 사실 상 use case가 없음. 혼선만 증가됨(의미론 상)
- 아래의 **아키텍처**, **Naming Guideline** 따르기

## Naming Guideline

- **Naming Convention** 존재 의의 역시 알고보면 가독성 때문
- 목표는 **'짧고도' 충분한 의미 전달** : modern javascript에서 채용한 명명법 👍

### Guideline

- **return 값 기준**으로 함수 명명
- (의미론적) **namespace 적극 이용**
  - 👍 sales_model_name.validity.of(sales_model_name)
  - 😔 get_sales_model_name_validity(sales_model_name)
- **`get_` prefix 제거** : 어짜피 함수 대부분이 조회성임
  - 함수명이 명사면 조회로 간주
  - 👍 `sales_model_name()`
  - 😔 `get_sales_model_name()`
- **단축어 지양** : 못 알아봄. 단축어는 autocomplete이 없던, 화면 작던 시절에 쓰던 practice.
  - 길어서 단축어 썼다? 먼저 naming 제대로 했나 살펴보자.
- **`data`, `info` 등의 무의미한 단어 사용 지양**
  - 알고보면 아무런 정보를 제공하지 않음(Software에서 data, info 아닌게 무엇이 있는가)
- **`bizlogic` 내 모든 명명은 biz 관점으로(not 기술 관점)**
  - `bizlogic` 안에 `Database`, `External`, `Storage` interface를, 게다가 `Bizlogic` 동적생성까지 동원한 유일한 이유
  - 이렇게 함으로 clean architecture의 핵심 practice를 가져온 것은 덤...

### 아키텍처

#### 아키텍처링 주안점

- **표준화**(반복요소 일원화)를 통한 가독성 극대화
  - grouping 표준화 : (HTTP) resource 기준 grouping
  - `ThePackage` 구분 표준화 : (HTTP) method 기준 packaging
  - `ThePackage` 기본 구성요소 표준화 : `endpoint`, `bizlogic`, `sanitize`, `database`, `external`, `storage`
- **bizlogic 중심 구성**
  - 결국 관심은 business logic에 있기에 `bizlogic` 중심으로 구성(clean architecturing...?)

#### `ThePackage` 아키텍처

- API 별 packaging
- `ThePackage` 명명법 : (http) method 사용
  - `/get`, `/post`, `/delete`, `/put`, `/list`(http method는 아니지만 `/get`과 구분을 위해 `{id}`가 없는 GET에 사용)
`ThePackage` grouping 기준 : 각 package는 (HTTP) resource로 grouping. group명은 resource와 동일하게 (e.g. `/jsonstore/get`, `/jsonstore/list`)
`ThePackage` 기본 구성요소 : `endpoint`, `bizlogic`, `sanitize`, `database`, `external`, `storage`
  - 이외의 모듈도 추가 가능. 하지만 이들은 `ThePackage` 내 기본 구성요소'만' 사용해야

```plantuml
package shared.aspect {}
package endpoint {}

endpoint -down-> shared.aspect.endpoint
endpoint -right-> bizlogic.Bizlogic
endpoint -down-> sanitize

package bizlogic {
    class Bizlogic {
        execute()
    }

    interface Database
    interface Storage
    interface External

    Bizlogic *-down-> bizlogic.Storage
    Bizlogic *-down-> bizlogic.Database
    Bizlogic *-right-> bizlogic.External
}

package storage {
    package Jsonstore {}
    package S3 {}

    storage.Storage -up-|> bizlogic.Storage
    storage.Storage -down-> Jsonstore
    storage.Storage -down-> S3
}

package database {
    package DynamoDB {}
    package DocumentDB {}
    package RDS {}

    database.Database -up-|> bizlogic.Database
    database.Database -down-> DynamoDB
    database.Database -down-> DocumentDB
    database.Database -down-> RDS
}

package external {
    external.External -left-|> bizlogic.External
    external.External -right-> shared.sqs
    external.External -down-> shared.external
}

package shared.sqs {}
package shared.external {
    package GSFS,SMILE,etc... {}
    package Common,Emp,etc... {}
}
```

### `ThePackage` 기본 구성요소 설명

#### `endpoint`

- **역할**
  - request 수집
  - 정제된 request의 `bizlogic`으로의 전달
  - output에 대한 response로의 변환
- **코딩 시 유의 사항**
  - 타 모듈에서 담당할 로직이 들어가지 않도록

#### `sanitize`

- **역할**
request 정제(cleansing, filtering)
- **코딩 시 유의 사항**
  - 빠른 처리속도가 필요. esp. `database` 접근이 요구되는 로직이 들어가면 안됨(속도 이슈)
  - `database`, `external` 접근 필요 등의 처리 속도가 느린 sanitize 로직은 `bizlogic`에서 처리.

#### `bizlogic`

- **역할**
  - (bizlogic 관점으로 정제된) input에 대한 output 생성
- **코딩 시 유의 사항**
  - bizlogic 이외의 모듈에 비즈니스 로직이 담겨서는 안됨
  - 함수명 만으로도 비즈니스 로직이 이해되도록
  - `endpoint`가 호출하는 단일 함수 필요(`execute()`)
  - `execute()` 내의 코드만 갖고도 비즈니스 로직이 이해되도록 : 상세 로직은 별도 함수로 격리하여 설명

#### `database`, `external`, `storage`

- **역할**
  - `bizlogic`을 위한 request 외 데이터 수집 및 정제
  - output에 대한 (target 관점으로) 정제 및 전달
- **코딩 시 유의 사항**
  - TODO


## Appendix : 왜 Guideline을? 결국 가독성(Readability) 땜시

- 바빠 죽겠는데 버그 FIX가 많다? 가독성 문제다.
- 바빠 죽겠는데 간단한 로직 추가조차 시간이 많이 든다? 가독성 문제다.
- 바빠 죽겠는데 성능이 발목 잡는다? 가독성 문제다.
- 업무 인수인계가 안된다? 가독성 문제다.
- 결국 생산성이 낮다? 가독성 문제다.

사실,

- 가독성 땜시 발목 잡혀본적 있으면 말 안해도 안다.
- 사실 발목 수준 정도면 이런거 안쓴다. 필자 관점에선 귀찮고, 아는척으로 몰려 평판 깎힐 위험마저(나아가 틀리기라도 한다면... 😭😭). 독자 관점에선 자유도 - 코딩 재미를 깎아먹고(코딩 싫어지기까지도? 😭😭)
- (필자 관점에선) 아키텍처 잘 잡으려는 이유도 가독성 때문이다.
