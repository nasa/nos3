.DEFAULT_GOAL := help
.ONESHELL:

SHELL=bash

# Containerization Parameters
export TARGETS_DIR=./targets
export SERVICES_DIR=./services
export SCRIPTS_DIR=./scripts

export CONTAINER_BIN=docker
export CONTAINER_COMPOSE_BIN=${CONTAINER_BIN} compose

export CONTAINER_DIR=${TARGETS_DIR}/docker
export COMPOSE_FILE=${CONTAINER_DIR}/compose.yaml

export ENVIRO_SCRIPT="${SCRIPTS_DIR}/env.sh"
export ENVIRO_FILE="${CONTAINER_DIR}/.env"

ifeq (${CONTAINER_BIN},podman)
export CONTAINER_COMPOSE_BIN=${CONTAINER_BIN}-compose
endif

# To set default settings.xml and essentially empty for mvn, unless in vmmoc
export MAVEN_HTTPS_PROXY=-s./settings.xml

ifdef (NOS3_CFG_PATH)
export NOS3_CFG_PATH=${NOS3_CFG_PATH}
else
export NOS3_CFG_PATH=..
endif

build-fortytwo: | env-create set-permissions ## Build fortytwo (42)
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-fortytwo

build-yamcs: | env-create setup-maven set-permissions ## Build yamcs
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-gsw

build-openmct: | env-create set-permissions ## Build openmct
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-openmct

build: | env-create setup-maven set-permissions ## Build all containers
	source ${ENVIRO_FILE} && \
	  ${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} build --pull nos3-fortytwo nos3-gsw nos3-openmct

set-permissions: ## set filesystem permissions, needed for restricted accounts
	find . -name '*.sh'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.py'   -exec chmod 775 {} \; # Make sure all scripts are world executable
	find . -name '*.json' -exec chmod 664 {} \; # Make sure all .json files are world readable
	find . -name '*.xml'  -exec chmod 664 {} \; # Make sure all .xml files are world readable
	find ${HOME}/.nos3/   -name '*.xml' -exec chmod 666 {} \; # Make sure all .xml files are world readable

up: | down env-create build ## Bring up nos3 in detached state
	sleep 5
	@source ${ENVIRO_FILE} && \
	  export NOS3_CFG_PATH=${NOS3_CFG_PATH}; \
	  ${CONTAINER_COMPOSE_BIN} --env-file ${ENVIRO_FILE} -f ${COMPOSE_FILE} up -d --force-recreate --remove-orphans
	sleep 15
	@$(MAKE) --silent sidecar
	@$(MAKE) --silent info-services
	@${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true

down: ## Bring down nos3
	${CONTAINER_COMPOSE_BIN} -f ${COMPOSE_FILE} down --remove-orphans || true

up-minimum: | down env-create build ## Bring up nos3 in detached state
	@sleep 5
	@source ${ENVIRO_FILE} && \
	  export NOS3_CFG_PATH=${NOS3_CFG_PATH}; \
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
	sleep 15
	@$(MAKE) --silent sidecar
	@$(MAKE) --silent info-services
	@${CONTAINER_BIN} ps -a | egrep -i 'exited|unhealthy|created|error' | grep -v STATUS || true

sidecar: ## Run sidecar to enable output
	@${SCRIPTS_DIR}/sidecar/sidecar.sh -c "/CFS/CMD/TO_ENABLE_OUTPUT"

clean: | down down-new ## clean images, cache, volumes, and networks
	yes | ${CONTAINER_BIN} image prune --filter "dangling=true"
	yes | ${CONTAINER_BIN} system prune -a
	yes | ${CONTAINER_BIN} volume prune
	yes | ${CONTAINER_BIN} network prune

setup-maven: ## Setup maven settings.xml with proxy info-services
	echo "<settings><proxies></proxies></settings>" > $${SERVICES_DIR}/yamcs/settings.xml
	if [ "$${DEPLOYMENT_ENVIRO}" == "vmmoc" ]; then \
	  cp -f $${MAVEN_SETTINGS_FILE} $${HOME}/.nos3/yamcs/ || true; \
	  cp -f $${MAVEN_SETTINGS_FILE} $${SERVICES_DIR}/yamcs/ || true; \
    fi

purge-containers: ## WARNING: purge docker/podman images, cache, volumes, and networks
	@$(MAKE) down || true
	@yes | ${CONTAINER_BIN} image prune --filter "dangling=true" || true
	@yes | ${CONTAINER_BIN} system prune -a || true
	@yes | ${CONTAINER_BIN} volume prune || true
	@yes | ${CONTAINER_BIN} network prune || true
	@echo

info-services: ## info on accessing various services
	@echo; echo "How to access various services:"; echo
	@echo "open http://localhost:8090 to access yamcs/gsw"
	@echo "open http://localhost:30090/vnc.html to access fortytwo (42)"
	@echo "open http://localhost:9000 to access openmct"
	@echo

env-create: ## Create .env file from env.sh script
	${ENVIRO_SCRIPT} > ${ENVIRO_FILE}

install-taskfile: ## Install Taskfile CLI tool
	@sh -c "$$(curl --location https://taskfile.dev/install.sh)" -- -d -b ~/.local/bin && \
	  chmod +x ~/.local/bin/task && export PATH="${HOME}/.local/bin:${PATH}"
	@echo; echo "add: ${HOME}/.local/bin:${PATH} to your ~/.bashrc or ~/.zshrc"; echo

install-lazydocker: ## Install lazydocker CLI tool (to view logs, etc)
	@curl --silent https://raw.githubusercontent.com/jesseduffield/lazydocker/master/scripts/install_update_linux.sh | bash
	@echo; echo "don't forget to add: ${HOME}/.local/bin:${PATH} to your ~/.bashrc or ~/.zshrc"; echo
	@echo "watch a Lazydocker demo here: https://www.youtube.com/watch?v=NICqQPxwJWw"; echo

lazydocker: | install-lazydocker ## Run lazydocker CLI tool
	@lazydocker

config-nos3-mission: | env-create ## configure nos3-mission.xml to set start-time
	@source ${ENVIRO_FILE} && \
	  sed -Ei 's/(\s*)(\<start\-time\>).*(\<start\-time\>)/\1\2>'$${START_TIME}'<\/\3/g' ../cfg/nos3-mission.xml && \
	  sed -Ei 's/(\s*)(\<gsw\>).*(\<gsw\>)/\1\2>'$${GSW_SOFTWARE}'<\/\3/g' ../cfg/nos3-mission.xml && \
	  sed -Ei 's/(\s*)(\<fsw\>).*(\<fsw\>)/\1\2>'$${FSW_SOFTWARE}'<\/\3/g' ../cfg/nos3-mission.xml

config: ## configure nos3 by running config.sh
	@cd ../ && ./scripts/cfg/config.sh

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
