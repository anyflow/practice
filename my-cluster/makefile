# Reset
Reset="\033[0m"

# Regular Colors
Black="\033[0;30m"
Red="\033[0;31m"
Green="\033[0;32m"
Yellow="\033[0;33m"
Blue="\033[0;34m"
Purple="\033[0;35m"
Cyan="\033[0;36m"
White="\033[0;37m"

default: create_cluster create_ingress_controller

create_cluster:
	kind create cluster --config ./kind-config.yaml

delete_cluster:
	kind delete cluster -n my-cluster

create_ingress_controller:
	kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

	@echo ${Yellow}"Waiting maximum 300s for ingress controller to be ready ..."${Reset}
	kubectl wait pods -n ingress-nginx -l app.kubernetes.io/component=controller --for condition=Ready --timeout=300s

delete_ingress_controller:
	kubectl delete -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

create_jenkins:
	kubectl apply -f ./jenkins/persistent.yaml
	kubectl apply -f ./jenkins/volatile.yaml

	@echo ${Yellow}"Waiting maximum 300s for jenkins to be ready ..."${Reset}
	kubectl wait pods -n jenkins -l app=jenkins --for condition=Ready --timeout=300s

	@echo ${Cyan}"Initial Administrator password is a50f4d1f69a74ed69125c4277cddbd09"${Reset}
	@echo ${Cyan}"Opening Jenkins in Safari..."${Reset}

	open -a safari http://jenkins.local/login

delete_jenkins:
	kubectl delete -f ./jenkins/volatile.yaml
