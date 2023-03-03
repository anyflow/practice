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

delete_cluster: delete_jenkins
delete_cluster:
	kubectl delete -f ./jenkins/volume-claim.yaml
	kubectl delete -f ./jenkins/namespace.yaml

	kind delete cluster -n my-cluster

create_ingress_controller:
	kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

	@echo ${Yellow}"Waiting for ingress controller to be ready while maximum 300s ..."${Reset}
	kubectl wait pods -n ingress-nginx -l app.kubernetes.io/component=controller --for condition=Ready --timeout=300s

delete_ingress_controller:
	kubectl delete -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml

create_jenkins:
	kubectl apply -f ./jenkins/namespace.yaml
	kubectl apply -f ./jenkins/volume-claim.yaml
	kubectl apply -f ./jenkins/resources.yaml
	@echo ${Yellow}"Waiting for jenkins to be ready while maximum 300s ..."${Reset}
	kubectl wait pods -n jenkins -l app=jenkins --for condition=Ready --timeout=300s

delete_jenkins:
	kubectl delete -f ./jenkins/resources.yaml
	# do not delete namespace and volume claim