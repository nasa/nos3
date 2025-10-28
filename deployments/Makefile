.DEFAULT_GOAL := help
.ONESHELL:

SHELL=bash

# Containerization Parameters
export CONTAINER_BIN=docker
export CONTAINER_COMPOSE_BIN=${CONTAINER_BIN} compose
export CONTAINER_DIR=./docker
export COMPOSE_FILE=${CONTAINER_DIR}/compose.yaml

export ENVIRO_SCRIPT="./env.sh"
export ENVIRO_FILE="${CONTAINER_DIR}/.env"

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

build-fortytwo: | env-create set-permissions ## Build fortytwo (42)
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-fortytwo

build-yamcs: | env-create set-permissions ## Build yamcs
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-gsw

build-openmct: | env-create set-permissions ## Build openmct
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-openmct

build: | env-create set-permissions ## Build all containers
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-fortytwo nos3-gsw nos3-openmct

set-permissions: ## set filesystem permissions, needed for restricted accounts
	find . -name '*.sh'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.py'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.json' -exec chmod 664 {} \; # Make sure all .json files are world readable
	find . -name '*.xml'  -exec chmod 664 {} \; # Make sure all .xml files are world readable
	find ${HOME}/.nos3/   -name '*.xml' -exec chmod 666 {} \; # Make sure all .xml files are world readable

up: | env-create build ## Bring up nos3 in detached state
	${CONTAINER_COMPOSE_BIN} --env-file ${ENVIRO_FILE} -f ${COMPOSE_FILE} up -d --remove-orphans

down: ## Bring down nos3
	${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} down --remove-orphans || true

up-minimum: | down build ## Bring up nos3 in detached state
	export MINIMAL_PATH=nos3; \
	${CONTAINER_COMPOSE_BIN} \
		--env-file ${ENVIRO_FILE} \
		-f ${COMPOSE_FILE} up -d \
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

sidecar: ## Run sidecar to enable output
	./scripts/sidecar/sidecar.sh -c "/CFS/CMD/TO_ENABLE_OUTPUT"

doitall: ## Do it all for nos3
	@$(MAKE) down; \
	sleep 5; \
	$(MAKE) up; \
	sleep 15; \
	${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true && \
	$(MAKE) --silent sidecar
	$(MAKE) info-services

clean: | down down-new ## clean images, cache, volumes, and networks
	yes | ${CONTAINER_BIN} image prune --filter "dangling=true"
	yes | ${CONTAINER_BIN} system prune -a
	yes | ${CONTAINER_BIN} volume prune
	yes | ${CONTAINER_BIN} network prune

setup-maven: ## Setup maven settings.xml with proxy info-services
	echo "<settings><proxies></proxies></settings>" > ${CONTAINER_DIR}/yamcs/settings.xml

purge-containers: ## WARNING: purge docker/podman images, cache, volumes, and networks
	@$(MAKE) all-down || true
	@yes | ${CONTAINER_BIN} image prune --filter "dangling=true" || true
	@yes | ${CONTAINER_BIN} system prune -a || true
	@yes | ${CONTAINER_BIN} volume prune || true
	@yes | ${CONTAINER_BIN} network prune || true
	@echo

info-services: ## Information on accessing various services
	@echo; echo "accessing various services:"; echo
	@echo "open http://localhost:8090 to access yamcs/gsw"
	@echo "open http://localhost:30090 to access fortytwo (42)"
	@echo "open http://localhost:9000 to access openmct"
	@echo

env-create: ## create ./docker/.env file
	${ENVIRO_SCRIPT} > ${ENVIRO_FILE}

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
