#
# Top Level Mission Makefile
#
BUILDTYPE ?= debug
INSTALLPREFIX ?= exe
FSWBUILDDIR ?= $(CURDIR)/fsw/build
GSWBUILDDIR ?= $(CURDIR)/gsw/build
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
LOCALTGTS := all checkout clean clean-fsw clean-sim clean-gsw config debug fsw gsw launch log prep sim stop stop-gsw uninstall
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
	cd $(GSWBUILDDIR) && cmake $(PREP_OPTS) -DSUPPORT=1 ../../components/cryptolib
	$(MAKE) --no-print-directory -C $(GSWBUILDDIR)

build-fsw:
ifeq ($(FLIGHT_SOFTWARE), fprime)
	cd fsw/fprime/fprime-nos3 && fprime-util generate && fprime-util build
else
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) ../cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install
endif

build-sim:
	mkdir -p $(SIMBUILDDIR)
	cd $(SIMBUILDDIR) && cmake -DCMAKE_INSTALL_PREFIX=$(SIMBUILDDIR) ..
	$(MAKE) --no-print-directory -C $(SIMBUILDDIR) install

build-test:
ifeq ($(FLIGHT_SOFTWARE), fprime)
	# TODO
else
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) -DENABLE_UNIT_TESTS=true ../cfe
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install
endif

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
	rm -rf fsw/fprime/fprime-nos3/build-artifacts
	rm -rf fsw/fprime/fprime-nos3/build-fprime-automatic-native
	rm -rf fsw/fprime/fprime-nos3/fprime-venv

clean-sim:
	rm -rf sims/build

clean-gsw:
	rm -rf gsw/build
	rm -rf gsw/cosmos/build
	rm -rf /tmp/nos3

config:
	./scripts/cfg/config.sh

debug:
	./scripts/debug.sh

fsw: 
	./cfg/build/fsw_build.sh

gsw:
	./scripts/gsw/build_cryptolib.sh
	./cfg/build/gsw_build.sh

igniter:
	./scripts/igniter_launch.sh

launch:
	./cfg/build/launch.sh

log:
	./scripts/log.sh

prep:
	./scripts/cfg/prepare.sh

prep-gsw:
	./scripts/cfg/prep_gsw.sh

prep-sat:
	./scripts/cfg/prep_sat.sh

sim:
	./scripts/build_sim.sh

start-gsw:
	./scripts/gsw/launch_gsw.sh

start-sat:
	./scripts/fsw/launch_sat.sh

stop:
	./scripts/stop.sh

stop-gsw:
	./scripts/gsw/stop_gsw.sh

test-fsw:
	cd $(FSWBUILDDIR)/amd64-posix/default_cpu1 && ctest -O ctest.log

uninstall:
	$(MAKE) clean
	./scripts/cfg/uninstall.sh
