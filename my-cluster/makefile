# Reset
Reset="\033[0m"       # Text Reset

# Regular Colors
Black="\033[0;30m"        # Black
Red="\033[0;31m"          # Red
Green="\033[0;32m"        # Green
Yellow="\033[0;33m"       # Yellow
Blue="\033[0;34m"         # Blue
Purple="\033[0;35m"       # Purple
Cyan="\033[0;36m"         # Cyan
White="\033[0;37m"        # White

default: create_cluster create_ingress_controller

create_cluster:
	kind create cluster --config ./kind-config.yaml

delete_cluster:
	kind delete cluster -n my-cluster

create_ingress_controller:
	kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

	@echo ${Yellow}"Waiting for ingress controller to be ready while maximum 300s ..."${Reset}
	kubectl wait pods -n ingress-nginx -l app.kubernetes.io/component=controller -l app.kubernetes.io/instance=ingress-nginx --for condition=Ready --timeout=300s

delete_ingress_controller:
	kubectl delete -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

create_jenkins:
	kubectl apply -f ./jenkins/resources.yaml
	helm repo add jenkinsci https://charts.jenkins.io
	helm repo update
	helm search repo jenkinsci
	helm install jenkins -n jenkins -f ./jenkins/values.yaml jenkinsci/jenkins

	@echo ${Yellow}"Waiting for jenkins to be ready while maximum 300s ..."${Reset}
	kubectl wait pods -n jenkins -l app.kubernetes.io/name=jenkins --for condition=Ready --timeout=300s

delete_jenkins:
	helm uninstall jenkins -n jenkins
	kubectl delete -f ./jenkins/resources.yaml
