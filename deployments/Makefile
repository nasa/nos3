.DEFAULT_GOAL := help

.PHONY: clean delete

SHELL=bash
export DIFF_PROGRAM:=vimdiff

# Containerization Parameters
export CONTAINER_BIN=docker
export CONTAINER_COMPOSE=${CONTAINER_BIN} compose
export DEFAULT_IMAGE_REGISTRY=docker.io/library/

export DOCKERFILE=./Dockerfile

GIT_BRANCH=$(shell git rev-parse --abbrev-ref HEAD)

DOCKER_DIR=./docker
K8S_DIR=./kubernetes

# Generally this is empty unless in specific environments
ENVIRO_FILE=./.env

#---
ifndef DEPLOYMENT_ENVIRO
$(warning DEPLOYMENT_ENVIRO is not set. Therefore, assuming not in vmmoc, and proceeding without vmmoc specific definitions)
else

ifeq (${DEPLOYMENT_ENVIRO},vmmoc)
$(info DEPLOYMENT_ENVIRO is set to vmmoc. Proceeding with vmmoc specific configurations)
export ENVIRO_FILE=/opt/neo/etc/environment
include ${ENVIRO_FILE}
else
$(error DEPLOYMENT_ENVIRO is set to ${DEPLOYMENT_ENVIRO}, which is not expected. Exiting!)
endif
endif
#---

down: ## Bring down nos3
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose down --remove-orphans || true

build-fsw-uart: ## build fsw-uart
	docker rmi -f ghcr.io/haisamido/nos3-base-fsw-uart:dev || true
	docker pull ghcr.io/haisamido/nos3-base:dev || true
	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
	cd ${DOCKER_DIR}/fsw && ${CONTAINER_BIN} build \
		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
		--build-arg HTTP_PROXY=${HTTP_PROXY} \
		--build-arg NO_PROXY=${NO_PROXY} \
		-t ghcr.io/haisamido/nos3-base-fsw-uart:dev  .

build-fortytwo: ## build-fortytwo
	docker rmi -f ghcr.io/haisamido/nos3-base-42:dev || true
	docker pull ghcr.io/haisamido/nos3-base:dev || true
	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
	cd ${DOCKER_DIR}/42 && ${CONTAINER_BIN} build \
		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
		--build-arg HTTP_PROXY=${HTTP_PROXY} \
		--build-arg NO_PROXY=${NO_PROXY} \
		-t ghcr.io/haisamido/nos3-base-42:dev  .

up: ## Bring up nos3 in detached state (attached state doesn't work for some reason)
	$(MAKE) down || true
	$(MAKE) build-fsw-uart
	$(MAKE) build-fortytwo
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose --env-file ${ENVIRO_FILE} up -d --remove-orphans 

up-attached: down
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose --env-file ${ENVIRO_FILE} --remove-orphans

up-kubernetes:
	cd ${K8S_DIR} && ./kubernetes.sh

#---
RESET  = \033[0m
PURPLE = \033[0;35m
GREEN  = \033[0;32m
LINE   = $(PURPLE)----------------------------------------------------------------------------------------$(RESET)

help:
	@echo
	@printf "\033[37m%-30s\033[0m %s\n" "#----------------------------------------------------------------------------------------"
	@printf "\033[37m%-30s\033[0m %s\n" "# Makefile targets                                                                       "
	@printf "\033[37m%-30s\033[0m %s\n" "#----------------------------------------------------------------------------------------"
	@echo 
	@printf "\033[37m%-30s\033[0m %s\n" "#-target-----------------------description-----------------------------------------------"
	@grep -E '^[a-zA-Z_-].+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
	@echo 

print-%  : ; @echo $* = $($*)
