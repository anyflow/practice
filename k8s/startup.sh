#!/bin/sh

Green='\033[0;32m'        # Green
NC='\033[0m'              # No Color

echo "${Green}[startup] Creating k8s cluster...${NC}"
kind create cluster --config ./statefulset-mongo-kind-config.yaml

echo
echo "${Green}[startup] Deploying NGINX ingress controller...${NC}"
kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

echo
echo "${Green}[startup] Waiting for NGINX setting completion...${NC}"
kubectl wait --namespace ingress-nginx \
  --for=condition=ready pod \
  --selector=app.kubernetes.io/component=controller \
  --timeout=90s

echo
echo "${Green}[startup] Creating metallb load balancer namespace...${NC}"
kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/namespace.yaml

echo
echo "${Green}[startup] Deploying metallb...${NC}"
kubectl apply -f https://raw.githubusercontent.com/metallb/metallb/v0.12.1/manifests/metallb.yaml

echo
echo "${Green}[startup] Waiting for metallb setting completion...${NC}"
#kubectl get pods -n metallb-system --watch
kubectl wait --namespace metallb-system \
  --for=condition=ready pod \
  --selector=app=metallb,component=speaker \
  --timeout=90s