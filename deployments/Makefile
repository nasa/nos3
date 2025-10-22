.DEFAULT_GOAL := help
.ONESHELL:

SHELL=bash

# Containerization Parameters
export CONTAINER_BIN=docker
export CONTAINER_COMPOSE_BIN=${CONTAINER_BIN} compose
export COMPOSE_FILE=./docker-compose.yaml
export CONTAINER_DIR=./docker

export ENVIRO_FILE=./.env

ifeq (${CONTAINER_BIN},podman)
export CONTAINER_COMPOSE_BIN=${CONTAINER_BIN}-compose
endif

# To set default settings.xml and essentially empty for mvn, unless in vmmoc
$(shell echo "<settings><proxies></proxies></settings>" > ${CONTAINER_DIR}/yamcs/settings.xml)
export MAVEN_HTTPS_PROXY=-s./settings.xml
export MINIMAL_PATH=..

#---
ifndef DEPLOYMENT_ENVIRO
$(warning DEPLOYMENT_ENVIRO is not set. Therefore, assuming not in vmmoc, and proceeding without vmmoc specific definitions)
else
ifeq (${DEPLOYMENT_ENVIRO},vmmoc)
$(info DEPLOYMENT_ENVIRO is set to vmmoc. Proceeding with vmmoc specific configurations)
export ENVIRO_FILE=/opt/neo/etc/environment
$(shell cp -f ${MAVEN_SETTINGS_FILE} ${CONTAINER_DIR}/yamcs/settings.xml)
else
$(error DEPLOYMENT_ENVIRO is set to ${DEPLOYMENT_ENVIRO}, which is not expected. Exiting!)
endif
endif
#---

env_create: ## create ./docker/.env file
	${CONTAINER_DIR}/env.sh > ${CONTAINER_DIR}/.env

env_export: env_create ## export ./docker/.env file variables to shell
	set -o allexport && source ${CONTAINER_DIR}/.env && set +o allexport

down: ## Bring down nos3
	cd ${CONTAINER_DIR} && ${CONTAINER_COMPOSE_BIN} down --remove-orphans || true

build-fortytwo: | env_export set-permissions ## Build fortytwo (42)
	cd ${CONTAINER_DIR}/ ; \
		${CONTAINER_COMPOSE_BIN} build --pull nos3-fortytwo

build-yamcs: | env_export set-permissions ## Build yamcs
	cd ${CONTAINER_DIR}/ ; \
		${CONTAINER_COMPOSE_BIN} build --pull nos3-gsw

build-openmct: | env_export set-permissions ## Build openmct
	cd ${CONTAINER_DIR}/ ; \
		${CONTAINER_COMPOSE_BIN} build --pull nos3-openmct

build: | set-permissions## Build all containers
	cd ${CONTAINER_DIR}/ ; \
		${CONTAINER_COMPOSE_BIN} build --pull nos3-fortytwo nos3-gsw nos3-openmct

set-permissions: ## set filesystem permissions, needed for restricted accounts
	find . -name '*.sh'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.py'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.json' -exec chmod 664 {} \; # Make sure all .json files are world readable
	find . -name '*.xml'  -exec chmod 664 {} \; # Make sure all .xml files are world readable
	find ${HOME}/.nos3/   -name '*.xml' -exec chmod 666 {} \; # Make sure all .xml files are world readable

up: | build ## Bring up nos3 in detached state
	cd ${CONTAINER_DIR} && ${CONTAINER_COMPOSE_BIN} \
		--env-file ./.env \
		-f ./docker-compose.yaml up -d --remove-orphans

up-minimum: | down build ## Bring up nos3 in detached state
	export MINIMAL_PATH=nos3; \
	cd ${CONTAINER_DIR} && ${CONTAINER_COMPOSE_BIN} \
		--env-file ./.env \
		-f ./docker-compose.yaml up -d \
			nos3-fortytwo \
			nos3-nos-engine-server \
			nos3-fsw \
			nos3-gps-sim \
			nos3-time \
			nos3-radio-sim-cryptolib \
			nos3-truth42sim \
			nos3-gsw \
			nos3-openmct \
		--remove-orphans 

up-new: | build ## Bring up nos3 in detached state (attached state doesn't work for some reason)
	cd ${CONTAINER_DIR} && ${CONTAINER_COMPOSE_BIN} \
		--env-file ./.env \
		-f ./docker-compose_new.yaml up -d --remove-orphans 

down-new: ## Bring down nos3
	cd ${CONTAINER_DIR} && ${CONTAINER_COMPOSE_BIN} -f docker-compose_new.yaml down --remove-orphans || true

sidecar: ## sidecar
	cd ${CONTAINER_DIR}/ && ./sidecar/sidecar.sh -c "/CFS/CMD/TO_ENABLE_OUTPUT"

doitall: ## Do it all for nos3
	@$(MAKE) down down-new; sleep 5; $(MAKE) up; sleep 15; ${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true && \
	  $(MAKE) --silent sidecar

clean: | down down-new ## clean images, cache, volumes, and networks
	yes | ${CONTAINER_BIN} image prune --filter "dangling=true"
	yes | ${CONTAINER_BIN} system prune -a
	yes | ${CONTAINER_BIN} volume prune
	yes | ${CONTAINER_BIN} network prune

setup-maven: ## setup maven settings.xml with proxy info
	echo "<settings><proxies></proxies></settings>" > ${CONTAINER_DIR}/yamcs/settings.xml

info-services: ## Information on accessing various services
	@echo; echo "accessing various services:"; echo
	@echo "open http://localhost:8090 to access yamcs/gsw"
	@echo "open http://localhost:30090 to access fortytwo (42)"
	@echo "open http://localhost:9000 to access openmct"
	@echo

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
