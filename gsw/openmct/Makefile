.DEFAULT_GOAL := help
SHELL=bash

# Containerization Parameters
CONTAINER_BIN=docker
CONTAINER_COMPOSE_BIN=${CONTAINER_BIN} compose
CONTAINER_FILE=./compose.yaml
TM_SIMULATOR_RATE=1

ifeq (${CONTAINER_BIN},podman)
export CONTAINER_COMPOSE=${CONTAINER_BIN}-compose
endif

ifdef (${DEPLOYMENT_ENVIRO})
export DEPLOYMENT_ENVIRO := ${DEPLOYMENT_ENVIRO}
endif

ifdef (${HTTP_PROXY})
export HTTP_PROXY := ${HTTP_PROXY}
endif

ifdef (${HTTPS_PROXY})
export HTTPS_PROXY := ${HTTPS_PROXY}
endif

ifdef (${NO_PROXY})
export NO_PROXY := ${NO_PROXY}
endif

ifdef (${MAVEN_HTTPS_PROXY})
export MAVEN_HTTPS_PROXY := ${MAVEN_HTTPS_PROXY}
else
export MAVEN_HTTPS_PROXY=-s./settings.xml
endif

ifdef (${MAVEN_SETTINGS_FILE})
export MAVEN_SETTINGS_FILE := ${MAVEN_SETTINGS_FILE}
endif

ifdef (${MAVEN_IMAGE})
export MAVEN_IMAGE := ${MAVEN_IMAGE}
else
export MAVEN_IMAGE := maven:3.9.9-eclipse-temurin-17
endif

ifdef (${IMAGE_URI})
export IMAGE_URI := ${IMAGE_URI}
else
export IMAGE_URI := docker.io/library/ubuntu:25.04
endif

openmct-pull: | env-create ## Pull ubuntu image for openmct, because in proxied enviro dockerfile cannot find docker.io!
	@source ./.env &&  ${CONTAINER_BIN} pull ${IMAGE_URI}

openmct-build: | env-create ## Build openmct
	@source ./.env && ${CONTAINER_COMPOSE_BIN} -f ${CONTAINER_FILE} build openmct

openmct-up: | openmct-build ## Bring up openmct
	@source ./.env && ${CONTAINER_COMPOSE_BIN} -f ${CONTAINER_FILE} up -d --build openmct --remove-orphans

openmct-down: ## Bring down openmct
	@source ./.env && ${CONTAINER_COMPOSE_BIN} -f ${CONTAINER_FILE} down openmct --remove-orphans

purge-containers: ## WARNING: purge docker/podman images, cache, volumes, and networks
	@$(MAKE) all-down || true
	@yes | ${CONTAINER_BIN} image prune --filter "dangling=true" || true
	@yes | ${CONTAINER_BIN} system prune -a || true
	@yes | ${CONTAINER_BIN} volume prune || true
	@yes | ${CONTAINER_BIN} network prune || true
	@echo

env-create: ## Create .env file 
	@./env.sh > ./.env

install-taskfile: ## Install Taskfile CLI tool
	@sh -c "$$(curl --location https://taskfile.dev/install.sh)" -- -d -b ~/.local/bin && \
	  chmod +x ~/.local/bin/task && export PATH="${HOME}/.local/bin:${PATH}"
	@echo; echo "add: ${HOME}/.local/bin:${PATH} to your ~/.bashrc or ~/.zshrc"; echo

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
