# Description

- mongodb replication cluster를 k8s kind cluster로 설정하

# Prerequisite

- kind cluster 생성 : `./readme.md` 참고

# mongodb replicaset 생성

1. 3개의 mongodb instance 생성

   ```bash
   > kubectl apply -f ./deployment-mongo.yaml
   ```

2. 특정 mongodb pod에 접근

   ```bash
   > kubectl exec --stdin --tty <mongodb pod name> -- /bin/bash
   ```

3. mongoshell로 해당 mongodb에 접근

   ```bash
   > mongosh --host localhost:27017
   ```

4. mongodb localhost exception 설정

   ```bash
   > rs.initiate() #initiate replica set
   > use admin # admin db 사용
   > db.createUser({ #admin 생성
      user: "m103-admin",
      pwd: "m103-pass",
      roles: [{role: "root", db: "admin"}]
     })
    > exit #localhost exception 빠져나오기
   ```

5. replica set cluster 구성

   ```bash
   > mongosh --host localhost:27017 -u admin -p admin --authenticationDatabase admin #admin으로 login
   > rs.add('<other mongodb pod ip #1>:27017') #첫 번째 replica를 cluster에 추
   > rs.add('<other mongodb pod ip #2>:27017') #두 번째 replica를 cluster에 추
   > cfg = rs.conf() # cfg = rs.config()와 동일
   > cfg.members[0].host = '<current mongodb pod ip>:27017' # 현재 current node가 pod name으로 host가 설정되어 있는데, 타 노드가 pod name으로는 접근할 수 없기에, ip로 변경
   > rs.reconfig(cfg)
   ...

   # 기타 replicaset 관련 명령어
   > rs.status()
   > rs.isMaster()
   > rs.stepDown()
   ```

# 검토 TODO

### Kind Persistent Volumes

- URL : <https://mauilion.dev/posts/kind-pvc/>

1. **default storage class**: I want there to be a built in storage class so that I can deploy applications that request persistent volume claims.

2. **pod restart**: If my pod restarts I want that pod to be scheduled such that the persistent volume claim is available to my pod. This ensures that if I have to restart and my pod will always come back with access to the same data.

3. **restore volumes**: I want to be able to bring up a kind cluster and regain access to a previously provisioned persistent volume claim.

4. **volume mobility**: I want to be able to schedule my pod to multiple nodes and have it access the same persistent volume claim. This requires that the peristent volume be made available to all nodes.

### 쿠버네티스 볼륨 개념 정리

- URL : <https://blog.eunsukim.me/posts/kubernetes-volume-overview>

1. Persistent Volume
2. Persistent Volume Claim
3. Storage Class
4. 상기 항목의 용도 : 볼륨 관리자와 볼륨 사용자 관점에서 설명