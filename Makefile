#
# Top Level Mission Makefile
#
BUILDTYPE ?= debug
INSTALLPREFIX ?= exe
FSWBUILDDIR ?= $(CURDIR)/fsw/build
SIMBUILDDIR ?= $(CURDIR)/sims/build

export CFS_APP_PATH = ../components

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
LOCALTGTS := all build-fsw build-sim checkout clean clean-fsw clean-sim clean-gsw fsw gsw launch log prep real-clean sim stop
OTHERTGTS := $(filter-out $(LOCALTGTS),$(MAKECMDGOALS))

# As this makefile does not build any real files, treat everything as a PHONY target
# This ensures that the rule gets executed even if a file by that name does exist
.PHONY: $(LOCALTGTS) $(OTHERTGTS)

#
# Commands
#
all:
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
	./gsw/scripts/checkout.sh

clean:
	$(MAKE) clean-fsw
	$(MAKE) clean-sim
	$(MAKE) clean-gsw

clean-fsw:
	rm -rf fsw/build

clean-sim:
	rm -rf sims/build

clean-gsw:
	rm -rf gsw/cosmos/build

fsw: 
	./gsw/scripts/docker_build_fsw.sh

gsw:
	./gsw/scripts/create_cosmos_gem.sh

launch:
	./gsw/scripts/docker_launch.sh

log:
	./gsw/scripts/log.sh

prep:
	./gsw/scripts/prepare.sh

real-clean:
	$(MAKE) clean
	./gsw/scripts/real_clean.sh

sim:
	./gsw/scripts/docker_build_sim.sh

stop:
	./gsw/scripts/docker_stop.sh
	./gsw/scripts/stop.sh
