#!/bin/sh

Green='\033[0;32m'        # Green
NC='\033[0m'              # No Color

echo "${Green}Creating k8s cluster...${NC}"
kind create cluster --config './kind-config.yaml'

echo
echo "${Green}Deploying NGINX ingress controller...${NC}"
kubectl apply -f 'https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml'

echo
echo "${Green}Waiting for NGINX setting completion... (timeout : 90s)${NC}"
kubectl wait --namespace ingress-nginx \
  --for=condition=ready pod \
  --selector=app.kubernetes.io/component=controller \
  --timeout=90s

echo
echo "${Green}Deploying deployment/service/ingress echo server...${NC}"
kubectl apply -f './echo-server.yaml'

echo
echo "${Green}Deploying APISIX etcd...${NC}"
kubectl apply -f './etcd.yaml'

echo
echo "${Green}Deploying APISIX...${NC}"
kubectl apply -f './apisix.yaml'

echo
echo "${Green}Waiting for APISIX setting completion... (timeout : 240s)${NC}"
kubectl wait --for=condition=ready pod \
  --selector=app.kubernetes.io/instance=apisix \
  --timeout=240s

echo
echo "${Green}Deploying APISIX Dashboard...${NC}"
kubectl apply -f './apisix-dashboard.yaml'
