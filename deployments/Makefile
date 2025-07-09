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
VOLUME_MAVEN_SETTINGS_PATH=""

# To set default settings.xml and essentially empty for mvn
$(shell echo "<settings><proxies></proxies></settings>" > ${DOCKER_DIR}/yamcs/settings.xml)

#---
ifndef DEPLOYMENT_ENVIRO
$(warning DEPLOYMENT_ENVIRO is not set. Therefore, assuming not in vmmoc, and proceeding without vmmoc specific definitions)
else

ifeq (${DEPLOYMENT_ENVIRO},vmmoc)
$(info DEPLOYMENT_ENVIRO is set to vmmoc. Proceeding with vmmoc specific configurations)
export ENVIRO_FILE=/opt/neo/etc/environment
include ${ENVIRO_FILE}
export VOLUME_MAVEN_SETTINGS_PATH=-v ${MAVEN_SETTINGS_FILE}:${MAVEN_SETTINGS_FILE}
$(shell cp -f ${MAVEN_SETTINGS_FILE} ${DOCKER_DIR}/yamcs/settings.xml)
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
	docker rmi -f ghcr.io/haisamido/nos3-base-fortytwo:dev || true
	docker pull ghcr.io/haisamido/nos3-base:dev || true
	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
	cd ${DOCKER_DIR}/42 && ${CONTAINER_BIN} build \
		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
		--build-arg HTTP_PROXY=${HTTP_PROXY} \
		--build-arg NO_PROXY=${NO_PROXY} \
		-t ghcr.io/haisamido/nos3-base-fortytwo:dev  .

build-yamcs: ## build-yamcs
	docker rmi -f ghcr.io/haisamido/nos3-base-yamcs:dev || true
	docker pull ghcr.io/haisamido/nos3-base:dev || true
	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
	cd ${DOCKER_DIR}/yamcs && ${CONTAINER_BIN} build \
		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
		--build-arg HTTP_PROXY=${HTTP_PROXY} \
		--build-arg NO_PROXY=${NO_PROXY} \
		--build-arg MAVEN_SETTINGS_FILE=${MAVEN_SETTINGS_FILE} \
		--build-arg MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY} \
		-t ghcr.io/haisamido/nos3-base-yamcs:dev  .

build: ## build essentials
	$(MAKE) build-fsw-uart
	$(MAKE) build-fortytwo
	$(MAKE) build-yamcs

up: ## Bring up nos3 in detached state (attached state doesn't work for some reason)
	$(MAKE) down || true
	$(MAKE) build
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
