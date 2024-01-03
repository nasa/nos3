#
# Top Level Mission Makefile
#
BUILDTYPE ?= debug
INSTALLPREFIX ?= exe
FSWBUILDDIR ?= $(CURDIR)/fsw/build
SIMBUILDDIR ?= $(CURDIR)/sims/build

export CFS_APP_PATH = ../components
export MISSION_DEFS = ../cfg/build/
export MISSIONCONFIG = ../cfg/build/nos3

# The "prep" step requires extra options that are specified via enviroment variables.
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
LOCALTGTS := all checkout clean clean-fsw clean-sim clean-gsw config debug fsw gsw launch log prep real-clean sim stop stop-gsw
OTHERTGTS := $(filter-out $(LOCALTGTS),$(MAKECMDGOALS))

# As this makefile does not build any real files, treat everything as a PHONY target
# This ensures that the rule gets executed even if a file by that name does exist
.PHONY: $(LOCALTGTS) $(OTHERTGTS)

#
# Commands
#
all:
	$(MAKE) config
	$(MAKE) fsw
	$(MAKE) sim
	$(MAKE) gsw

build-fsw:
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) ../cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install

build-sim:
	mkdir -p $(SIMBUILDDIR)
	cd $(SIMBUILDDIR) && cmake -DCMAKE_INSTALL_PREFIX=$(SIMBUILDDIR) ..
	$(MAKE) --no-print-directory -C $(SIMBUILDDIR) install

checkout:
	./scripts/checkout.sh

clean:
	$(MAKE) clean-fsw
	$(MAKE) clean-sim
	$(MAKE) clean-gsw
	rm -rf cfg/build

clean-fsw:
	rm -rf cfg/build/nos3_defs
	rm -rf fsw/build

clean-sim:
	rm -rf sims/build

clean-gsw:
	rm -rf gsw/cosmos/build

config:
	./scripts/config.sh

debug:
	./scripts/docker_debug.sh

fsw: 
	./scripts/docker_build_fsw.sh

gsw:
	./scripts/create_cosmos_gem.sh

launch:
	./scripts/docker_launch.sh

log:
	./scripts/log.sh

prep:
	./scripts/prepare.sh

real-clean:
	$(MAKE) clean
	./scripts/real_clean.sh

sim:
	./scripts/docker_build_sim.sh

stop:
	./scripts/docker_stop.sh
	./scripts/stop.sh

stop-gsw:
	./scripts/stop_gsw.sh
