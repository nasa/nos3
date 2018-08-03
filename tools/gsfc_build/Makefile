###############################################################################
# File: makefile
#
# Purpose:
#   Main makefile for Mission build and all apps
#
# $Id: Makefile 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw Exp  $
#
# $Log: Makefile  $
# Revision 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw 
# DCR22713:1 Removed extra tabs and inserted history log
#
###############################################################################
##
## Top Level rules:
## If you want to build the testapps every time, just add the testapps targets
## to the primary targets below ( or uncomment the testapps versions )
##
export MISSIONNAME:=Mission

##############################################################################
## Build rules

## Note: if you add "cpuN" directories, you can simplify your
## build procedure by adding appropriate "make -C" lines
## in this file.

all ::
	$(MAKE) -C cpu1 all

clean ::
	$(MAKE) -C cpu1 clean

install ::
	$(MAKE) -C cpu1 install

depend ::
	$(MAKE) -C cpu1 depend

#-------------------------------------------------------------------------------
# Doxygen rules


usersguide ::
	$(MAKE) -C cpu1  usersguide

detaileddocs ::
	$(MAKE) -C cpu1  detaileddocs

installdocs ::
	$(MAKE) -C cpu1  installdocs

cleandocs ::
	$(MAKE) -C cpu1  cleandocs

