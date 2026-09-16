.DEFAULT_GOAL := help
.ONESHELL:

SHELL=bash

install-taskfile: ## USE TASK instead of Makefile: Install Taskfile CLI tool
	@sh -c "$$(curl --location https://taskfile.dev/install.sh)" -- -d -b ~/.local/bin && \
	  chmod +x ~/.local/bin/task && export PATH="${HOME}/.local/bin:${PATH}"
	@echo; echo "add: ${HOME}/.local/bin:${PATH} to your ~/.bashrc or ~/.zshrc"; echo

install-lazydocker: ## Install lazydocker CLI tool (to view logs, etc)
	@curl --silent https://raw.githubusercontent.com/jesseduffield/lazydocker/master/scripts/install_update_linux.sh | bash
	@echo; echo "don't forget to add: ${HOME}/.local/bin:${PATH} to your ~/.bashrc or ~/.zshrc"; echo
	@echo "watch a Lazydocker demo here: https://www.youtube.com/watch?v=NICqQPxwJWw"; echo

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
