#
# Top Level Mission Makefile
#
.PHONY: help

BUILDTYPE ?= debug
INSTALLPREFIX ?= exe
FSWBUILDDIR ?= $(CURDIR)/fsw/build
GSWBUILDDIR ?= $(CURDIR)/gsw/build
SIMBUILDDIR ?= $(CURDIR)/sims/build
COVERAGEDIR ?= $(CURDIR)/fsw/build/amd64-posix/default_cpu1

SC1_CFG ?= 

export SYSTEM_TEST_FILE_PATH = /scripts/gsw/system_test.rb

export CFS_APP_PATH = ../components
export MISSION_DEFS = ../cfg/build/
export MISSIONCONFIG = ../cfg/build/nos3


# The "prep" step requires extra options that are specified via environment variables.
# Certain special ones should be passed via cache (-D) options to CMake.
# These are only needed for the "prep" target but they are computed globally anyway.
PREP_OPTS :=

ifneq ($(INSTALLPREFIX),)
PREP_OPTS += -DCMAKE_INSTALL_PREFIX=$(INSTALLPREFIX)
endif

ifneq ($(VERBOSE),)
PREP_OPTS += --trace
endif

ifneq ($(BUILDTYPE),)
PREP_OPTS += -DCMAKE_BUILD_TYPE=$(BUILDTYPE)
endif


# The "LOCALTGTS" defines the top-level targets that are implemented in this makefile
# Any other target may also be given, in that case it will simply be passed through.
LOCALTGTS := all checkout clean clean-fsw clean-sim clean-gsw code-coverage config debug fsw gcov gsw help help-all launch log prep sim stop stop-gsw uninstall
OTHERTGTS := $(filter-out $(LOCALTGTS),$(MAKECMDGOALS))

# As this makefile does not build any real files, treat everything as a PHONY target
# This ensures that the rule gets executed even if a file by that name does exist
.PHONY: $(LOCALTGTS) $(OTHERTGTS)

#
# Commands
#
all: ## Build everything: config, fsw, sim, gsw
	@if [ ! -f ./cfg/build/current_config_path.txt ]; then \
		echo "Running make config (initial)..."; \
		$(MAKE) config; \
	else \
		echo "Skipping make config (already configured)"; \
	fi
	$(MAKE) fsw
	$(MAKE) sim
	$(MAKE) gsw

build-cryptolib: ## Build CryptoLib Component, ## -DSTANDALONE_TCP=0 if using udp for cryptolib in the loop
	mkdir -p $(GSWBUILDDIR)
	cd $(GSWBUILDDIR) && cmake $(PREP_OPTS) -DSTANDALONE_TCP=1 -DCRYPTO_RX_GROUND_PORT=$(CRYPTO_RX_GROUND_PORT) -DCRYPTO_TX_GROUND_PORT=$(CRYPTO_TX_GROUND_PORT) -DCRYPTO_TX_RADIO_PORT=$(CRYPTO_TX_RADIO_PORT) -DCRYPTO_RX_RADIO_PORT=$(CRYPTO_RX_RADIO_PORT) -DSA_FILE=OFF -DSUPPORT=1 -DCRYPTO_LIBGCRYPT=1 -DSA_INTERNAL=1 -DMC_INTERNAL=1 -DKEY_INTERNAL=1 ../../components/cryptolib
	$(MAKE) --no-print-directory -C $(GSWBUILDDIR)

build-fsw: ## Build the flight software (cFS or F')
ifeq ($(FLIGHT_SOFTWARE), fprime)
	cd fsw/fprime/fprime-nos3 && fprime-util generate && fprime-util build
else
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) ../cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install
endif

build-sim: ## Build Simulator
	mkdir -p $(SIMBUILDDIR)
	cd $(SIMBUILDDIR) && cmake -DCMAKE_INSTALL_PREFIX=$(SIMBUILDDIR) ..
	$(MAKE) --no-print-directory -C $(SIMBUILDDIR) install

build-test: ## Build unit tests
ifeq ($(FLIGHT_SOFTWARE), fprime)
	# TODO
else
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) -DENABLE_UNIT_TESTS=true ../cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install
endif

checkout: ## Run a checkout application (may require reconfiguration)
	./scripts/checkout.sh

#This could currently break if not using COSMOS in the config.
ci-launch: ## Headless Launch for System Testing
	@export SYSTEM_TEST_FILE_PATH=$(SYSTEM_TEST_FILE_PATH) && \
	./scripts/ci_launch.sh && \
	./scripts/system_tests.sh && \
	./scripts/stop.sh

#This could currently break if not using COSMOS in the config.
system-tests: ## System Testing with GUI
	@export SYSTEM_TEST_FILE_PATH=../..$(SYSTEM_TEST_FILE_PATH) && \
	./cfg/build/launch.sh && \
	./scripts/system_tests.sh

#Be sure that your nos3-mission.xml has been set to YAMCS
yamcs-operator:  ## Launch as a YAMCS operator viewpoint
	@export SYSTEM_TEST_FILE_PATH=$(SYSTEM_TEST_FILE_PATH) && \
	./scripts/ci_launch.sh --use-yamcs

#Be sure that your nos3-mission.xml has been set to COSMOS
cosmos-operator: ## Launch as a COSMOS operator viewpoint
	@export SYSTEM_TEST_FILE_PATH=../..$(SYSTEM_TEST_FILE_PATH) && \
	./scripts/ci_launch.sh --use-cosmos-gui 

clean: ## Clean all build files and configurations
	$(MAKE) clean-fsw
	$(MAKE) clean-sim
	$(MAKE) clean-gsw
	rm -rf cfg/build

clean-fsw: ## Clean only flight software build artifacts
	rm -rf cfg/build/nos3_defs
	rm -rf docs/coverage/coverage_report.*
	rm -rf fsw/build
	rm -rf fsw/fprime/fprime-nos3/build-artifacts
	rm -rf fsw/fprime/fprime-nos3/build-fprime-automatic-native
	rm -rf fsw/fprime/fprime-nos3/fprime-venv
	rm -rf fsw/fprime/fprime-nos3/logs

clean-sim: ## Clean only simulator build artifacts
	rm -rf sims/build

clean-gsw: ## Clean only GSW build artifacts
	rm -rf gsw/build
	rm -rf gsw/cosmos/build
	rm -rf /tmp/nos3
	./scripts/gsw/gsw_openc3_clean.sh

config: ## Run configuration setup
	@if [ -n "$(SC1_CFG)" ]; then \
		SC1_CFG="$(SC1_CFG)" ./scripts/cfg/config.sh; \
	else \
		./scripts/cfg/config.sh; \
	fi

debug: ## Launch the debug container terminal
	./scripts/debug.sh

fsw:  ## Build Flight Software binaries
	./cfg/build/fsw_build.sh

gcov: ## Build Code Coverage results
	cd $(COVERAGEDIR) && ctest -O ctest.log
	lcov -c --directory . --output-file $(COVERAGEDIR)/coverage.info
	genhtml $(COVERAGEDIR)/coverage.info --output-directory $(COVERAGEDIR)/results

code-coverage: ## Build code coverage file, does not work via shared folders in VirtualBox
	$(MAKE) build-test 
	$(MAKE) test-fsw
	mkdir -p docs/coverage
	gcovr --gcov-ignore-parse-errors --merge-mode-functions=merge-use-line-0 --xml-pretty -o docs/coverage/coverage_report.xml
	gcovr --gcov-ignore-parse-errors --merge-mode-functions=merge-use-line-0 --html --html-details -o docs/coverage/coverage_report.html
	chmod 777 ./docs/coverage/coverage_report.*

gsw: ## Build Ground Software binaries
	./scripts/gsw/build_cryptolib.sh
	./cfg/build/gsw_build.sh

help: ## Display this help message
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@printf "%-20s %s\n" "all"           "Build everything: config, fsw, sim, gsw"
	@printf "%-20s %s\n" "clean"         "Clean all build files and configurations"
	@printf "%-20s %s\n" "help"          "Display this help message"
	@printf "%-20s %s\n" "help-all"      "Display advanced help information"
	@printf "%-20s %s\n" "launch"        "Launch NOS3 System"
	@printf "%-20s %s\n" "prep"          "Prepare full development environment"
	@printf "%-20s %s\n" "stop"          "Stop entire system"
	@printf "%-20s %s\n" "uninstall"     "Remove all build artifacts and containers"
	@echo ""

help-all: ## Displays advanced help information
	@echo ""
	@echo "Usage: make <target>"
	@echo ""

	@echo "Setup:"
	@printf "\t%-20s %s\n" "prep"           "Prepare full development environment"
	@printf "\t%-20s %s\n" "prep-gsw"       "Prepare Ground Software"
	@printf "\t%-20s %s\n" "prep-sat"       "Prepare Satellite IP Routes"
	@printf "\t%-20s %s\n" "start-gsw"      "Launch Ground Software"
	@printf "\t%-20s %s\n" "start-sat"      "Satellite Launch"

	@echo ""
	@echo "Building:"
	@printf "\t%-20s %s\n" "build-cryptolib" "Build CryptoLib Component"
	@printf "\t%-20s %s\n" "build-fsw"      "Build the Flight Software (cFS or F')"
	@printf "\t%-20s %s\n" "build-sim"      "Build Simulator"
	@printf "\t%-20s %s\n" "build-test"     "Build Unit Tests"
	@printf "\t%-20s %s\n" "config"			"Configuration Setup.  Optional: [SC1_CFG=spacecraft/sc-minimal-config.xml]"
	@printf "\t%-20s %s\n" "igniter"        "Launch Configuration GUI Igniter"

	@echo ""
	@echo "Running:"
	@printf "\t%-20s %s\n" "checkout"       "Run a checkout application (may require reconfiguration)"
	@printf "\t%-20s %s\n" "debug"          "Launch the debug container terminal"
	@printf "\t%-20s %s\n" "fsw"            "Build Flight Software Binaries"
	@printf "\t%-20s %s\n" "gcov"           "Build Code Coverage Results"
	@printf "\t%-20s %s\n" "gsw"            "Build Ground Software Binaries"
	@printf "\t%-20s %s\n" "sim"            "Build Simulation Binaries"
	@printf "\t%-20s %s\n" "cosmos-operator" "Launch as COSMOS operator viewpoint - requires configuration"
	@printf "\t%-20s %s\n" "yamcs-operator" "Launch as YAMCS operator viewpoint - requires configuration"

	@echo ""
	@echo "Testing:"
	@printf "\t%-20s %s\n" "code-coverage"  "Produce code coverage report (does not work via shared folders)"
	@printf "\t%-20s %s\n" "ci-launch"      "Headless Launch for System Testing"
	@printf "\t%-20s %s\n" "system-tests"   "GUI Launch for System Testing"
	@printf "\t%-20s %s\n" "test-fsw"       "Test Flight Software"

	@echo ""
	@echo "Cleanup:"
	@printf "\t%-20s %s\n" "clean"          "Clean all build files and configurations"
	@printf "\t%-20s %s\n" "clean-fsw"      "Clean only Flight Software build artifacts"
	@printf "\t%-20s %s\n" "clean-gsw"      "Clean only Ground Software build artifacts"
	@printf "\t%-20s %s\n" "clean-sim"      "Clean only Simulation build artifacts"
	@printf "\t%-20s %s\n" "log"            "Log Outputs"
	@printf "\t%-20s %s\n" "stop"           "Stop entire System"
	@printf "\t%-20s %s\n" "stop-gsw"       "Stop Ground Software"
	@printf "\t%-20s %s\n" "uninstall"      "Remove all build artifacts and containers"
	@echo ""

igniter: ## Launch Configuration GUI Igniter
	./scripts/cfg/igniter_launch.sh

launch: ## Launch NOS3 System
	./cfg/build/launch.sh

log: ## Log outputs
	./scripts/log.sh

prep: ## Prepare full development enviornment
	./scripts/cfg/prepare.sh

prep-gsw: ## Prepare Ground Software
	./scripts/cfg/prep_gsw.sh

prep-sat: ## Prepare Satelite IP Routes
	./scripts/cfg/prep_sat.sh

sim: ## Build Sims
	./scripts/build_sim.sh

start-gsw: ## Launch Ground Software
	./scripts/gsw/launch_gsw.sh

start-sat: ## Satellite Launch
	./scripts/fsw/launch_sat.sh

stop: ## Stop entire system
	./scripts/stop.sh
	@if [ -f ./cfg/build/current_config_path.txt ]; then \
	  echo "Cleaning up temporary config file..."; \
	  CONFIG_FILE=$$(cat ./cfg/build/current_config_path.txt | tr -d '\n'); \
	  if [ "$$(basename $$CONFIG_FILE)" != "nos3-mission.xml" ]; then \
	    echo "Removing $$CONFIG_FILE"; \
	    rm -f "$$CONFIG_FILE"; \
	  fi; \
	  rm -f ./cfg/build/current_config_path.txt; \
	fi

stop-gsw: ## Stop Ground Sfotware
	./scripts/gsw/stop_gsw.sh

test-fsw: ## Test Flight Software 
	cd $(COVERAGEDIR) && ctest --output-on-failure -O ctest.log

uninstall: ## Remove all build artifacts
	$(MAKE) clean
	./scripts/cfg/uninstall.sh