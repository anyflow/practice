#!/bin/sh

Green='\033[0;32m'        # Green
NC='\033[0m'              # No Color

echo "${Green}[startup] Creating k8s cluster...${NC}"
kind create cluster --config './kind-config.yaml'

echo
echo "${Green}[startup] Deploying NGINX ingress controller...${NC}"
kubectl apply -f './ingress-controller.yaml'

echo
echo "${Green}[startup] Waiting for NGINX setting completion...${NC}"
kubectl wait --namespace ingress-nginx \
  --for=condition=ready pod \
  --selector=app.kubernetes.io/component=controller \
  --timeout=90s

echo
echo "${Green}[startup] Deploying deployment htto echo...${NC}"
kubectl apply -f './deployment-http-echo.yaml'

echo
echo "${Green}[startup] Deploying deployment/service htto echo...${NC}"
kubectl apply -f './deployment-http-echo.yaml'

echo
echo "${Green}[startup] Deploying ingress rules for htto echo...${NC}"
kubectl apply -f './ingress.yaml'

echo
echo "${Green}[startup] Deploying APISIX etcd...${NC}"
kubectl apply -f './etcd.yaml'

echo
echo "${Green}[startup] Deploying APISIX...${NC}"
kubectl apply -f './apisix.yaml'

echo
echo "${Green}[startup] Deploying APISIX Dashboard...${NC}"
kubectl apply -f './apisix-dashboard.yaml'
