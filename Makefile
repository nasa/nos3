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
LOCALTGTS := all fsw fsw-prep pack sim sim-prep clean clean-fsw clean-sim checkout gsw gsm-prep launch log real-clean stop sc-launch
OTHERTGTS := $(filter-out $(LOCALTGTS),$(MAKECMDGOALS))

# As this makefile does not build any real files, treat everything as a PHONY target
# This ensures that the rule gets executed even if a file by that name does exist
.PHONY: $(LOCALTGTS) $(OTHERTGTS)

#
# Generic Commands
#
all:
	$(MAKE) fsw
	$(MAKE) sim
	$(MAKE) gsw

#
# FSW
#
fsw:
	$(MAKE) fsw-prep
	$(MAKE) --no-print-directory -C $(FSWBUILDDIR) mission-install

fsw-prep:
	mkdir -p $(FSWBUILDDIR)
	cd $(FSWBUILDDIR) && cmake $(PREP_OPTS) ../cfe

#
# Sims
#
sim:
	$(MAKE) sim-prep
	$(MAKE) --no-print-directory -C $(SIMBUILDDIR) install

sim-prep:
	mkdir -p $(SIMBUILDDIR)
	cd $(SIMBUILDDIR) && cmake -DCMAKE_INSTALL_PREFIX=$(SIMBUILDDIR) ..

#
# GSW
#
gsw:
	$(MAKE) gsw-prep
	./gsw/scripts/create_cosmos_gem.sh

gsw-prep:
	mkdir -p ./gsw/cosmos/build

#
# Clean
#
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

#
# Script Calls
#
checkout:
	./gsw/scripts/checkout.sh

gsw-launch:
	./gsw/scripts/gsw.sh

launch:
	./gsw/scripts/launch.sh

reboot:
	./gsw/scripts/reboot.sh

log:
	./gsw/scripts/log.sh

real-clean:
	$(MAKE) clean
	./gsw/scripts/real_clean.sh

sc-launch:
	./gsw/scripts/sc_launch.sh

stop:
	./gsw/scripts/stop.sh
