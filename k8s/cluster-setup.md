

# Cluster operations with ingress NGINX

## Create a cluster
```bash
# Create cluster of this config
> kind create cluster --config ./kind-ingress.yaml

# Install ingree NGINX
> kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

# Successful output of the above command
# > namespace/ingress-nginx created
# > serviceaccount/ingress-nginx created
# > serviceaccount/ingress-nginx-admission created
# > role.rbac.authorization.k8s.io/ingress-nginx created
# > role.rbac.authorization.k8s.io/ingress-nginx-admission created
# > clusterrole.rbac.authorization.k8s.io/ingress-nginx created
# > clusterrole.rbac.authorization.k8s.io/ingress-nginx-admission created
# > rolebinding.rbac.authorization.k8s.io/ingress-nginx created
# > rolebinding.rbac.authorization.k8s.io/ingress-nginx-admission created
# > clusterrolebinding.rbac.authorization.k8s.io/ingress-nginx created
# > clusterrolebinding.rbac.authorization.k8s.io/ingress-nginx-admission created
# > configmap/ingress-nginx-controller created
# > service/ingress-nginx-controller created
# > service/ingress-nginx-controller-admission created
# > deployment.apps/ingress-nginx-controller created
# > job.batch/ingress-nginx-admission-create created
# > job.batch/ingress-nginx-admission-patch created
# > ingressclass.networking.k8s.io/nginx created
# > validatingwebhookconfiguration.admissionregistration.k8s.io/ingress-nginx-admission created

# Wait for Ingress is all setup
> kubectl wait --namespace ingress-nginx \
  --for=condition=ready pod \
  --selector=app.kubernetes.io/component=controller \
  --timeout=90s

# Successful output of the above command
# > pod/ingress-nginx-controller-bdf7bf984-k6qqt condition met
```



## Installing metallb using default manifests

```bash
# Create the metallb namespace
> kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/namespace.yaml

# Successful output of the above command
# > namespace/metallb-system created

#Apply metallb manifest
> kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/metallb.yaml

# Successful output of the above command
# > Warning: policy/v1beta1 PodSecurityPolicy is deprecated in v1.21+, unavailable in v1.25+
# > podsecuritypolicy.policy/controller created
# > podsecuritypolicy.policy/speaker created
# > serviceaccount/controller created
# > serviceaccount/speaker created
# > clusterrole.rbac.authorization.k8s.io/metallb-system:controller created
# > clusterrole.rbac.authorization.k8s.io/metallb-system:speaker created
# > role.rbac.authorization.k8s.io/config-watcher created
# > role.rbac.authorization.k8s.io/pod-lister created
# > role.rbac.authorization.k8s.io/controller created
# > clusterrolebinding.rbac.authorization.k8s.io/metallb-system:controller created
# > clusterrolebinding.rbac.authorization.k8s.io/metallb-system:speaker created
# > rolebinding.rbac.authorization.k8s.io/config-watcher created
# > rolebinding.rbac.authorization.k8s.io/pod-lister created
# > rolebinding.rbac.authorization.k8s.io/controller created
# > daemonset.apps/speaker created
# > deployment.apps/controller created

# Wait for metallb pods to have a status of Running
> kubectl get pods -n metallb-system --watch

# Successful output of the above command
# > controller-66445f859d-kxkgh   1/1     Running   0          75s
# > speaker-2x4gm                 1/1     Running   0          75s
# > speaker-4d2b5                 1/1     Running   0          75s
# > speaker-dp6m7                 1/1     Running   0          75s

# Setup address pool used by loadbalancers
> docker network inspect -f '{{.IPAM.Config}}' kind

# Successful output of the above command
# > [{172.18.0.0/16  172.18.0.1 map[]} {fc00:f853:ccd:e793::/64  fc00:f853:ccd:e793::1 map[]}]

# Update metallb-configmap.yaml address section with the above result
      # addresses:
      # - 172.18.255.200-172.18.255.250

# Apply metallb-configmap.yaml
> kubectl apply -f ./metallb-configmap.yaml

# Successful output of the above command
# > configmap/config created
```


## Delete the cluster

```bash
# delete cluster of this config
kind delete cluster # --name anyflow-cluster
```
