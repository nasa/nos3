################################################################################
# File: Makefile
#
# Purpose:
#   Main Makefile for cFE core build and all apps
#
## $Id: Makefile 1.22 2014/10/14 15:55:12GMT-05:00 acudmore Exp  $
#
# Purpose: Main Makefile for cFE core build and all apps
#
# $Log: Makefile  $
# Revision 1.22 2014/10/14 15:55:12GMT-05:00 acudmore 
# Added CFE_PLATFORM_INC_SRC and removed CPUNAME. File installs are now relative and do not depend on the CPUNAME.
# Revision 1.21 2014/08/22 15:23:30GMT-05:00 acudmore 
# Moved tool build rules to cfs.mak.
# Added macros to define executable file extensions.
# Revision 1.20 2014/07/10 07:35:12GMT-05:00 acudmore 
# Updated clean rules, added config and distclean rules
# Revision 1.19 2014/07/07 08:09:37GMT-05:00 acudmore 
# Updated elf2cfetbl rule
# Revision 1.18 2014/06/19 11:40:58GMT-05:00 acudmore 
# New location for cfs.mak and cfs_tst.mak.
# Rules for elf2cfetbl in build directory
# Revision 1.17 2014/06/12 13:41:49GMT-05:00 wmoleski 
# Updated the build/cpux/Makefile to contain comments about the Path variables and restored the 
# Revision history to build/cpux/cfe/unit-test/xx/Makefiles.
# Revision 1.16 2014/06/09 07:24:36EDT wmoleski 
# Moved the MISSION environment variable from Makefile to setenvs.sh, modified the Apps and 
# Tbls to be null and modifed the BUILD_PATH in the Makefile.
# Revision 1.15 2014/05/28 07:45:35EDT wmoleski 
# Checking in the files that were modified in the mission tree.
# Revision 1.14 2009/07/31 16:28:32EDT rmcgraw
# DCR8291:8 Removed CF entries and added log
# 
################################################################################

#===============================================================================
# Build-specific environment variables
#===============================================================================

# List of apps/libs to include in the build
THE_APPS := sample_lib sample_app
THE_APPS += ci_lab to_lab sch_lab
# THE_APPS += cfs_lib
# THE_APPS += cf cs ds fm hk hs lc md mm sc sch
export THE_APPS

# List of apps that include tables for the build
THE_TBLS := 
#THE_TBLS += ds fm hk lc sc sch
#THE_TBLS += cf hs 
export THE_TBLS

#===============================================================================
# Platform-specific environment variables
#===============================================================================

#===============================================================================
# The CFE_CORE_PLATFORM_INC_SRC variable determines which set of cFE platform
# config header files is used. 
# The cFE currently has cpu1, cpu2, and cpu3 platform config directories.
#===============================================================================
export CFE_CORE_PLATFORM_INC_SRC:=cpu1

#===============================================================================
# The BUILD_PATH variable is not used in the current cFE baseline. However, this
# variable is used in the makefiles included below for copying files. By
# default, the CFS build will put all of the build products (.o's (or .so's for
# linux), .tbls, etc) in /exe. This default location may not work in a shared
# lab machine. The BUILD_PATH was added so that users can specify where they
# want the build products to go.
#===============================================================================
BUILD_PATH:=.

#===============================================================================
# The VOLTAB_PATH variable is not used in the current cFE baseline.  The PSP
# defines where the CFS build will load/run from, which may not necessarily be 
# the same as the BUILD_PATH directory.  To make things simple and flexible for
# all users, the VOLTAB_PATH is defined here and can be used for convenience. 
# This variable can be used in the cfe_psp_voltab.c file when the PSP source is
# shared amonst multiple platforms.  An example cfe_psp_voltab.c entry is 
# provided below:
#
# {"/cf",      VOLTAB_PATH, FS_BASED, FALSE, FALSE, TRUE,  "CF", "/cf", 1024 },
#  
#===============================================================================
#export TARGET_DEFS:=-DVOLTAB_PATH='"$(HOME)/tmp"'

export CFS_EXE:=$(BUILD_PATH)/exe
export CFS_PROM:=$(CFS_EXE)/cf
export CFS_PROM_APPS:=$(CFS_PROM)/apps
export CFS_RAM:=$(CFS_EXE)/ram
#export CFS_STATIC_STARTUP_TBL:=$(BUILD_PATH)

#
# Define the Executable file extensions for copies
# This should match the type of system being compiled
# 
export CFE_CORE_EXE_TYPE:=bin
export CFS_APP_EXE_TYPE:=so

#===============================================================================
# Top-level build rules
#===============================================================================

all:: cfs_envs cfs_inbuild cfs_postbuild

config:: cfs_envs cfs_prebuild cfs_tool

clean:: cfs_clean

distclean:: cfs_realclean cfs_cleantool

realclean:: cfs_realclean cfs_cleantool

#===============================================================================
# Supplemental makefile(s)
#===============================================================================
# cfs.mak file should not be modified by general users.
include ../cfs.mak

#===============================================================================
# User-defined build rules
#===============================================================================
# Users can add their own build rules here.
include ../cfs_tst.mak

################################################################################
# End of top-level CFS makefile for Linux platform
################################################################################
