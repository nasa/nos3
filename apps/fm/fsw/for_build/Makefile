###############################################################################
# File: fm.mak
#
# Purpose:
#   Compile the CFS File Manager (FM) Application.
#
# $Log: Makefile  $
# Revision 1.7 2009/10/30 14:02:30EDT lwalling 
# Remove trailing white space from all lines
# Revision 1.6 2009/10/23 14:38:47EDT lwalling
# Add fm_child.o to build list
# Revision 1.5 2009/07/09 11:41:24EDT rmcgraw
# DCR8291:1 Changed CFE_MISSION_INC to CFS_MISSION_INC and added log if needed
# Revision 1.4 2009/06/11 11:51:36EDT rmcgraw
# DCR82191:1 Changed BSP to PSP
# Revision 1.3 2009/06/03 12:35:21EDT rmcgraw
# DCR8291:1 Environment variable changes for new repository structure
# Revision 1.2 2009/04/20 17:35:31EDT dkobe
# Removed device table source file that magically re-appeared during doxygen update
# Revision 1.1 2009/04/16 17:30:46EDT dkobe
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/for_build/project.pj
# Revision 1.3 2008/10/09 08:43:27EDT sstrege
# Moved device table from the source section into the object section.
# Revision 1.2 2008/06/26 16:47:30EDT slstrege
# Member moved from fsw/src/Makefile in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to Makefile in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
# Revision 1.1 2008/06/26 15:47:30ACT slstrege
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
#
###############################################################################
#
# Subsystem produced by this makefile.
#
export APPTARGET = fm

#
# Entry Point for task
#
ENTRY_PT = FM_AppMain

#
# Object files required to build subsystem.
#
OBJS := fm_app.o
OBJS += fm_cmds.o
OBJS += fm_cmd_utils.o
OBJS += fm_tbl.o
OBJS += fm_child.o

#
# Source files required to build subsystem; used to generate dependencies.
# As long as there are no assembly files this can be automated.
#
SOURCES := $(OBJS:.o=.c)

##
## Specify extra C Flags needed to build this subsystem
##
LOCAL_COPTS =


##
## EXEDIR is defined here, just in case it needs to be different for a custom
## build
##
EXEDIR=../exe

##
## Certain OSs and Application Loaders require the following option for
## Shared libraries. Currently only needed for vxWorks 5.5 and RTEMS.
## For each shared library that this app depends on, you need to have an
## entry like the following:
##  -R../tst_lib/tst_lib.elf
##
## For builds under cygwin when you need the cfs_lib.dll linked in
## set this variables as follows:
##
## SHARED_LIB_LINK =../cfs_lib/cfs_lib.dll
##
SHARED_LIB_LINK =

########################################################################
# Should not have to change below this line, except for customized
# Mission and cFE directory structures
########################################################################

#
# Set build type to CFE_APP. This allows us to
# define different compiler flags for the cFE Core and Apps.
#
BUILD_TYPE = CFE_APP

##
## Include all necessary cFE make rules
## Any of these can be copied to a local file and
## changed if needed.
##
##
##       cfe-config.mak contains PSP and OS selection
##
include ../cfe/cfe-config.mak
##
##       debug-opts.mak contains debug switches
##
include ../cfe/debug-opts.mak
##
##       compiler-opts.mak contains compiler definitions and switches/defines
##
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

##
## Setup the include path for this subsystem
## The OS specific includes are in the build-rules.make file
##
## If this subsystem needs include files from another app, add the path here.
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFS_APP_SRC)/inc \
-I$(CFS_APP_SRC)/$(APPTARGET)/fsw/src \
-I$(CFS_MISSION_INC) \
-I../cfe/inc \
-I../inc

##
## Define the VPATH make variable.
## This can be modified to include source from another directory.
## If there is no corresponding app in the cfs-apps directory, then this can be discarded, or
## if the mission chooses to put the src in another directory such as "src", then that can be
## added here as well.
##
VPATH = $(CFS_APP_SRC)/$(APPTARGET)/fsw/src

##
## Include the common make rules for building a cFE Application
##
include $(CFE_CORE_SRC)/make/app-rules.mak
