##
##
## CFE Core Subsystem Makefile
##
##  

##
## Identify the cFE Core Subsystem here. For almost all cases, this subsystem
## variable is the only thing that has to change in the makefile.
##
SUBSYS = psp

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
include $(CFE_PSP_SRC)/$(PSP)/src/$(SUBSYS).mak

##
## Setup the include path for this subsystem
## The OS specific includes are in the compiler-opts.mak file
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFS_MISSION_INC) \
-I../../inc
##
## Define the VPATH make variable. 
## This can be modified to include source from another directory.
##
VPATH = $(CFE_PSP_SRC)/$(PSP)/src:$(CFE_PSP_SRC)/shared 

##
## Include the common make rules for the cFE Core subsystems
##
include $(CFE_CORE_SRC)/make/make-rules.mak
