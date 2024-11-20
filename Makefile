#
# Top Level Mission Makefile
#
BUILDTYPE ?= debug
INSTALLPREFIX ?= exe
NOS3BUILDDIR ?= /tmp/nos3
FSWBUILDDIR ?= $(NOS3BUILDDIR)/fsw
GSWBUILDDIR ?= $(NOS3BUILDDIR)/gsw
SIMBUILDDIR ?= $(NOS3BUILDDIR)/sims

export CFS_APP_PATH = ../components
export MISSION_DEFS = /tmp/nos3/cfg/nos3_defs
export MISSIONCONFIG = nos3

# The "prep" step requires extra options that are specified via enviroment variables.
# Certain special ones should be passed via cache (-D) options to CMake.
# These are only needed for the "prep" target but they are computed globally anyway.
PREP_OPTS := -DMISSION_DEFS=$(MISSION_DEFS)

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

build-cryptolib:
	mkdir -p $(GSWBUILDDIR)
	cd $(GSWBUILDDIR) && cmake -DSUPPORT=1 $(CURDIR)/components/cryptolib
	$(MAKE) --no-print-directory -C $(GSWBUILDDIR)

build-fsw:
ifeq ($(FLIGHT_SOFTWARE), fprime)
	cd fsw/fprime/fprime-nos3 && fprime-util generate && fprime-util build
else
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) $(CURDIR)/fsw/cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install
endif

build-sim:
	mkdir -p $(SIMBUILDDIR)
	cd $(SIMBUILDDIR) && cmake -DCMAKE_INSTALL_PREFIX=$(SIMBUILDDIR) $(CURDIR)/sims
	$(MAKE) --no-print-directory -C $(SIMBUILDDIR) install

checkout:
	./scripts/checkout.sh

clean:
	$(MAKE) clean-fsw
	$(MAKE) clean-sim
	$(MAKE) clean-gsw
	./scripts/clean.sh

clean-fsw:
	rm -rf fsw/fprime/fprime-nos3/build-artifacts
	rm -rf fsw/fprime/fprime-nos3/build-fprime-automatic-native
	rm -rf fsw/fprime/fprime-nos3/fprime-venv
	./scripts/fsw/fsw_clean.sh

clean-sim:
	./scripts/sim/sim_clean.sh

clean-gsw:
	./scripts/gsw/gsw_clean.sh

config:
	./scripts/cfg/config.sh

debug:
	./scripts/debug.sh

fsw: 
	$(NOS3BUILDDIR)/cfg/fsw_build.sh $(CURDIR)

gsw:
	./scripts/gsw/build_cryptolib.sh
	$(NOS3BUILDDIR)/cfg/gsw_build.sh $(CURDIR)

igniter:
	./scripts/igniter_launch.sh

launch:
	$(NOS3BUILDDIR)/cfg/launch.sh $(CURDIR)

log:
	./scripts/log.sh

prep:
	./scripts/cfg/prepare.sh

prep-gsw:
	./scripts/cfg/prep_gsw.sh

prep-sat:
	./scripts/cfg/prep_sat.sh

sim:
	./scripts/sim/build_sim.sh

start-gsw:
	./scripts/gsw/launch_gsw.sh

start-sat:
	./scripts/fsw/launch_sat.sh

stop:
	./scripts/stop.sh

stop-gsw:
	./scripts/gsw/stop_gsw.sh

uninstall:
	$(MAKE) clean
	./scripts/cfg/uninstall.sh
