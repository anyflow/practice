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

## 참고

- 특정 pod bash에 접근 : `kubectl exec --stdin --tty <pod name> -- /bin/bash`
- network util pod 생성 및 shell 로그인 `kubectl run my-shell --rm -i --tty --image praqma/network-multitool -- bash`
