# StatefulSet으로 MongoDB Replica Set 구성하기


## 사전 준비

- 동적 생성된 volume 저장용 directory, mongoDB 공유 `keyfile` 저장용 directory 생성

  ```bash
  > mkdir ./pvc ./hostroot_in_node
  ```

- mongoDB용 `keyfile` 생성

  ```bash
  > openssl rand -base64 741 > ./hostroot_in_node/mongo/keyfile
  ```

## 설치
### k8s 클러스터 생성

- k8s 클러스터 생성 (w/ 성공 output)

  ```bash
  > kind create cluster --config ./statefulset-mongo-kind-config.yaml
  ...
  Creating cluster "kind" ...

  ✓ Ensuring node image (kindest/node:v1.21.1) 🖼
  ✓ Preparing nodes 📦 📦 📦 📦
  ✓ Writing configuration 📜
  ✓ Starting control-plane 🕹️
  ✓ Installing CNI 🔌
  ✓ Installing StorageClass 💾
  ✓ Joining worker nodes 🚜
  Set kubectl context to "kind-kind"
  You can now use your cluster with:

  kubectl cluster-info --context kind-kind

  Thanks for using kind! 😊
  ```

### mongoDB 인스턴스 생성

- secret에 `keyfile` 저장

  ```bash
  > kubectl create secret generic shared-bootstrap-data --from-file=mongo-keyfile=./hostroot_in_node/mongo/keyfile
  ```

- StatefulSet 기반으로 mongoDB 배포 (w/ 성공 output)

  ```bash
  > kubectl apply -f ./statefulset-mongo.yaml
  ...
  service/mongodb-service created
  statefulset.apps/mongo created
  ```

- 정상 배포 확인 (w/ 성공 output)

  ```bash
  > kubectl get all
  ...
  NAME          READY   STATUS              RESTARTS   AGE
  pod/mongo-0   1/1     Running   0          6m3s
  pod/mongo-1   1/1     Running   0          5m21s
  pod/mongo-2   1/1     Running   0          4m40s

  NAME                      TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)     AGE
  service/kubernetes        ClusterIP   10.96.0.1    <none>        443/TCP     11m
  service/mongodb-service   ClusterIP   None         <none>        27017/TCP   6m3s

  NAME                     READY   AGE
  statefulset.apps/mongo   3/3     6m3s

  > kubectl get pv,pvc
  ...
  NAME                                                        CAPACITY   ACCESS MODES   RECLAIM POLICY   STATUS   CLAIM                                           STORAGECLASS   REASON   AGE
  persistentvolume/pvc-0acb895f-5c83-4344-a465-2ffc30751282   1Gi        RWO            Delete           Bound    default/mongo-persistent-volume-claim-mongo-2   standard                10m
  persistentvolume/pvc-73ddbf20-76ea-4d64-b96d-4e6151f04cdd   1Gi        RWO            Delete           Bound    default/mongo-persistent-volume-claim-mongo-1   standard                11m
  persistentvolume/pvc-77266513-3c08-494e-ad57-731e7c42d823   1Gi        RWO            Delete           Bound    default/mongo-persistent-volume-claim-mongo-0   standard                11m

  NAME                                                          STATUS   VOLUME                                     CAPACITY   ACCESS MODES   STORAGECLASS   AGE
  persistentvolumeclaim/mongo-persistent-volume-claim-mongo-0   Bound    pvc-77266513-3c08-494e-ad57-731e7c42d823   1Gi        RWO            standard       11m
  persistentvolumeclaim/mongo-persistent-volume-claim-mongo-1   Bound    pvc-73ddbf20-76ea-4d64-b96d-4e6151f04cdd   1Gi        RWO            standard       11m
  persistentvolumeclaim/mongo-persistent-volume-claim-mongo-2   Bound    pvc-0acb895f-5c83-4344-a465-2ffc30751282   1Gi        RWO            standard       10m
  ```

- DNS에 mongodb-service 및 개별 pod가 정상 등록되었는지 확인

  ```bash
  > kubectl apply -f /k8s.io/examples/admin/dns/dnsutils.yaml # cluster내에서 nslookup 실행을 위한 dnsutils pod 설치
  ...
  pod/dnsutils created

  > kubectl exec -i -t dnsutils -- nslookup mongodb-service # mongodb-service가 nslookup 되는지 확인
  ...
  Server:         10.96.0.10
  Address:        10.96.0.10#53
  Name:   mongodb-service.default.svc.cluster.local
  Address: 10.244.2.3
  Name:   mongodb-service.default.svc.cluster.local
  Address: 10.244.1.3
  Name:   mongodb-service.default.svc.cluster.local
  Address: 10.244.3.3

   # 개별 pod가 nslookup 되는지 확인
  > kubectl exec -i -t dnsutils -- nslookup mongo-0.mongodb-service # mongo-1, mongo-2에 대해서도 각기 수행
  ...
  Server:         10.96.0.10
  Address:        10.96.0.10#53

  Name:   mongo-0.mongodb-service.default.svc.cluster.local
  Address: 10.244.2.3
  ```
### MongoDB Replica Set 설정

  ```bash
  > kubectl exec -it mongo-0 -- bash # mongo-0의 shell에 로그인
  ...
  root@mongo-0:/#

  > mongosh # mongo shell에 로그인
  ...
  Current Mongosh Log ID: 62738f0686b5b44653c0329f
  Connecting to:          mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000appName=mongosh+1.3.1
  Using MongoDB:          5.0.7
  Using Mongosh:          1.3.1

  For mongosh info see: https://docs.mongodb.com/mongodb-shell/


  To help improve our products, anonymous usage data is collected and sent to MongoDB periodically (https:/www.mongodb.com/legal/privacy-policy).
  You can opt-out by running the disableTelemetry() command.

  test>

  # mongoDB Replica Set 구성을 위해 각 node 연결 (위에서 확인한 Domain을 hostname으로 사용 중)
  > rs.initiate({ _id: "anyflow-replset", version: 1, members: [
  ... {_id: 0, host: "mongo-0.mongodb-service:27017" },
  ... { _id: 1, host : "mongo-1.mongodb-service:27017" },
  ... {_id: 2, host: "mongo-2.mongodb-service:27017" }] });
  ...
  { ok: 1 }

  # 정상적으로 MongoDB Replica Set이 생성되었는지 확인
  > rs.status()
  ...
  {
    set: 'anyflow-replset',
    ...
    members: [
        {
            _id: 0,
            name: 'mongo-0.mongodb-service:27017',
            ...
            stateStr: 'PRIMARY',
            ...
        },
        {
            _id: 1,
            name: 'mongo-1.mongodb-service:27017',
            ...
            stateStr: 'SECONDARY', # 정상 연결이 안되면 STARTUP 등 타 값이 나타남
            ...
        },
        {
            _id: 2,
            name: 'mongo-2.mongodb-service:27017',
            ...
            stateStr: 'SECONDARY', # 정상 연결이 안되면 STARTUP 등 타 값이 나타남
            ...
        }
    ],
    ok: 1,
    ...
  }

  # admin 계정 생성 mongoDB의 Localhost Exception 모드 제거
  > db.getSiblingDB('admin').createUser({
    ... user: 'mongo-admin',
    ... pwd: 'mongo-pass',
    ... roles: [{ role: 'root', db: 'admin' }]
    ... });
  ...
  {
    ok: 1,
    '$clusterTime': {
        clusterTime: Timestamp({ t: 1651741430, i: 4 }),
        signature: {
            hash: Binary(Buffer.from("ba58556a10615fe5795debf1f0a5859a5761fcab", "hex"), 0),
            keyId: Long("7094171789755940868")
        }
    },
    operationTime: Timestamp({ t: 1651741430, i: 4 })
  }

  > exit
  ```

## 테스트

- mongoDB Replica Set 정상 동작 확인


## References

- [Running MongoDB on Kubernetes with StatefulSets](<https://kubernetes.io/blog/2017/01/running-mongodb-on-kubernetes-with-statefulsets/>)
- [Mongodb Replica Set on Kubernetes](<https://maruftuhin.com/blog/mongodb-replica-set-on-kubernetes/>)
