###############################################################################
#
# File: CFE Core Subsystem Makefile 
#
# $Id: Makefile 1.6 2009/07/09 11:04:35GMT-05:00 rmcgraw Exp  $
#
# $Log: Makefile  $
# Revision 1.6 2009/07/09 11:04:35GMT-05:00 rmcgraw 
# DCR8291:1 Changed CFE_MISSION_INC to CFS_MISSION_INC and added log if needed
#
###############################################################################  

##
## Identify the cFE Core Subsystem here. For almost all cases, this subsystem
## variable is the only thing that has to change in the makefile.
##
SUBSYS = tbl

##
## Specify extra C Flags needed to build this subsystem
##
LOCAL_COPTS = 

## 
## Include all necessary cFE make rules
## Any of these can be copied to a local file and 
## changed if needed.
##
##
##       cfe-config.mak contians arch, PSP, and OS selection
##
include ../cfe-config.mak
##
##       debug-opts.mak contains debug switches
##
include ../debug-opts.mak
##
##       compiler-opts.mak contains compiler definitions and switches/defines
##
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak
##
##       SUBSYS.mak file contains the list of objects to compile
##
include $(CFE_CORE_SRC)/$(SUBSYS)/cfe_$(SUBSYS).mak

##
## Setup the include path for this subsystem
## The OS specific includes are in the compiler-opts.mak file
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(CFE_CORE_SRC)/$(SUBSYS) \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFS_MISSION_INC) \
-I../../inc


##
## Define the VPATH make variable. 
## This can be modified to include source from another directory.
##
VPATH = $(CFE_CORE_SRC)/$(SUBSYS)

##
## Include the common make rules for the cFE Core subsystems
##
include $(CFE_CORE_SRC)/make/make-rules.mak
