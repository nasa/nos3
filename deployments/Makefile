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

# ./.env file content would be something like:
# # DO NOT PUT ANY SENSITIVE CONFIGURATIONS HERE
# MAVEN_SETTINGS_FILE=
# MAVEN_HTTPS_PROXY=
# HTTPS_PROXY=
# HTTP_PROXY=
# NO_PROXY=
# HEALTHCHECK_PORT=60000

VOLUME_MAVEN_SETTINGS_PATH=""

# To set default settings.xml and essentially empty for mvn, unless in vmmoc
$(shell echo "<settings><proxies></proxies></settings>" > ${DOCKER_DIR}/yamcs/settings.xml)
export MAVEN_HTTPS_PROXY=-s./settings.xml

#---
ifndef DEPLOYMENT_ENVIRO
$(warning DEPLOYMENT_ENVIRO is not set. Therefore, assuming not in vmmoc, and proceeding without vmmoc specific definitions)
else
ifeq (${DEPLOYMENT_ENVIRO},vmmoc)
$(info DEPLOYMENT_ENVIRO is set to vmmoc. Proceeding with vmmoc specific configurations)
export ENVIRO_FILE=/opt/neo/etc/environment
$(shell cp -f ${MAVEN_SETTINGS_FILE} ${DOCKER_DIR}/yamcs/settings.xml)
else
$(error DEPLOYMENT_ENVIRO is set to ${DEPLOYMENT_ENVIRO}, which is not expected. Exiting!)
endif
endif
#---

env_create: ## create ./docker/.env file
	${DOCKER_DIR}/env.sh > ${DOCKER_DIR}/.env

down: ## Bring down nos3
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose down --remove-orphans || true

down-new: ## Bring down nos3
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose -f docker-compose_new.yaml down --remove-orphans || true

build-fortytwo: | env_create set-permissions ## build-fortytwo
	cd ${DOCKER_DIR}/ ; \
		${CONTAINER_BIN} compose build --pull nos3-fortytwo

build-yamcs: | env_create set-permissions ## build-yamcs
	cd ${DOCKER_DIR}/ ; \
		${CONTAINER_BIN} compose build --pull nos3-gsw

build: | set-permissions## build essentials
	$(MAKE) build-fortytwo
	$(MAKE) build-yamcs

set-permissions: ## set filesystem permissions, needed for restricted accounts
	find . -name '*.sh'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.py'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.json' -exec chmod 664 {} \; # Make sure all .json files are world readable
	find . -name '*.xml'  -exec chmod 664 {} \; # Make sure all .xml files are world readable
	find ${HOME}/.nos3/ -name '*.xml' -exec chmod 666 {} \; # Make sure all .xml files are world readable

up: | build ## Bring up nos3 in detached state (attached state doesn't work for some reason)
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose \
		--env-file ./.env \
		-f ./docker-compose.yaml up -d --remove-orphans 

up-new: | build ## Bring up nos3 in detached state (attached state doesn't work for some reason)
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose \
		--env-file ./.env \
		-f ./docker-compose_new.yaml up -d --remove-orphans 

doitall-new: ## do it all
	$(MAKE) down-new down; sleep 5; $(MAKE) up-new; sleep 15; ${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true && \
		$(MAKE) --silent sidecar

sidecar: ## sidecar
	cd ${DOCKER_DIR}/ && ./sidecar/sidecar.sh

doitall: ## do it all
	$(MAKE) down down-new; sleep 5; $(MAKE) up; sleep 15; ${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true && \
		$(MAKE) --silent sidecar

#	$(MAKE) down; sleep 5; $(MAKE) up; sleep 15; ${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS

up-attached: down
	cd ${DOCKER_DIR} && ${CONTAINER_BIN} compose --env-file ./.env --remove-orphans

up-kubernetes:
	cd ${K8S_DIR} && ./kubernetes.sh

# build-fortytwo-orig-delete-me: env_create ## build-fortytwo
# 	docker rmi -f ghcr.io/haisamido/nos3-base-fortytwo:dev || true
# 	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
# 	cd ${DOCKER_DIR}/42 && ${CONTAINER_BIN} build \
# 		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
# 		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
# 		--build-arg HTTP_PROXY=${HTTP_PROXY} \
# 		--build-arg NO_PROXY=${NO_PROXY} \
# 		--build-arg MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY} \
# 		-t ghcr.io/haisamido/nos3-base-fortytwo:dev  .

# build-yamcs-orig-delete-me: env_create ## build-yamcs
# 	docker rmi -f ghcr.io/haisamido/nos3-base-yamcs:dev || true
# 	touch ${ENVIRO_FILE} && source ${ENVIRO_FILE} ; \
# 	cd ${DOCKER_DIR}/yamcs && ${CONTAINER_BIN} build \
# 		--build-arg DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO} \
# 		--build-arg HTTPS_PROXY=${HTTPS_PROXY} \
# 		--build-arg HTTP_PROXY=${HTTP_PROXY} \
# 		--build-arg NO_PROXY=${NO_PROXY} \
# 		--build-arg MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY} \
# 		--build-arg GSW_SOFTWARE=yamcs \
# 		--build-arg GIT_URL=https://github.com/nasa-itc/yamcs-nos3.git\
# 		--build-arg GIT_BRANCH=dev \
# 		-t ghcr.io/haisamido/nos3-base-yamcs:dev  .

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
