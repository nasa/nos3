###############################################################################
# File: cFE Application Makefile 
#
#
# History:
#
###############################################################################
#
# Subsystem produced by this makefile.
#

##
## If this subsystem needs include files from another app, add the path here.
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_MISSION_INC) \
-I../cfe/inc \
-I../inc

##
## Define the VPATH make variable. 
## This can be modified to include source from another directory.
## If there is no corresponding app in the cfe-apps directory, then this can be discarded, or
## if the mission chooses to put the src in another directory such as "src", then that can be 
## added here as well.
##
##VPATH = $(CFS_APP_SRC)/$(APPTARGET) 

all:
	gcc $(INCLUDE_PATH) -g -o gen_msgids gen_msgids.c

clean:
	rm gen_msgids
	rm cFEMsgIDs.h

