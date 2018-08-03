#
# $Id: setvars.sh 1.29 2014/10/07 12:38:17GMT-05:00 sstrege Exp  $
#
# This is a shell script that sets the environment variables for
# a CFS mission build. The makefiles depend on the variables being
# set correctly.
#
# This script requires the "bash" shell to run. It should work on Linux,
# Mac OS X, and Cygwin.
#
# $Log: setvars.sh  $
# Revision 1.29 2014/10/07 12:38:17GMT-05:00 sstrege 
# Updated version number from 3.0.0 to 3.0.1
# Revision 1.28 2014/10/02 17:53:17EDT sstrege 
# Setting MISSION_TEMPLATE_VERSION to 3.0.0
# Revision 1.27 2014/07/25 11:58:46EDT lwalling 
# Changed INCLUDE_CFE_TBL to EXCLUDE_CFE_TBL
# Revision 1.26 2014/07/23 11:22:25EDT lwalling 
# Added definition of INCLUDE_CFE_TBL to control inclusion of Table Services in the CFE-CORE
# Revision 1.25 2014/06/17 14:28:51EDT rmcgraw 
# DCR22706:1 Added _DIR for APP,CFE,OSAL,PSP
# Revision 1.24 2014/06/09 07:24:37EDT wmoleski
# Moved the MISSION environment variable from Makefile to setenvs.sh, modified the Apps and
# Tbls to be null and modifed the BUILD_PATH in the Makefile.
# Revision 1.23 2014/05/28 07:45:36EDT wmoleski
# Checking in the files that were modified in the mission tree.
# Revision 1.22 2011/12/05 10:07:52EST lwalling
# Restore improperly changed file
# Revision 1.21 2011/11/25 16:02:23EST lwalling
# Move test zip file from mission build folder to cfe unit test folder
# Revision 1.20 2009/10/13 14:53:25EDT wmoleski
# Adding the changes to the setvars.sh file in the CFS Mission Tree to contain a definition for the
# location of the UTF source.
# Revision 1.19 2009/09/28 10:12:43EDT rmcgraw
# Error in previous release, setting version back to 0.0.0
# Revision 1.18 2009/09/28 10:06:27EDT rmcgraw
# Set version number to 2.0.0 for release
# Revision 1.17 2009/09/28 09:59:54EDT rmcgraw
# Reset version number back to post release value of 0.0.0
# Revision 1.16 2009/07/08 14:02:07EDT rmcgraw
# DCR8291:1 Changed variables to the correct defaults
# Revision 1.15 2009/07/08 13:49:21EDT rmcgraw
# DCR8291:1 Changed line endings from windows to unix
# Revision 1.14 2009/07/08 10:53:30EDT rmcgraw
# DCR8291:1 Renamed CFE_MISSION_INC to CFS_MISSION_INC
# Revision 1.13 2009/06/16 13:49:49EDT rmcgraw
# DCR8291:1 Added MISSION_TEMPLATE_VERSION and change history log
#

#
# Variable: EXCLUDE_CFE_TBL
#
# EXCLUDE_CFE_TBL controls whether, or not, the CFE Table Services 
#   application is included in the CFE-CORE. The value of EXCLUDE_CFE_TBL
#   must be set equal to TRUE to cause Table Services to be excluded from
#   the CFE-CORE.  Any definition of EXCLUDE_CFE_TBL that does not set the
#   value equal to TRUE (or no definition at all) will result in the
#   inclusion of Table Services.
#
## export EXCLUDE_CFE_TBL=TRUE

#
# Variable: CFS_MISSION
#
# CFS_MISSION is the top level directory of the Mission project tree.
# It is set here using the 'pwd' command so the entire directory can be moved to
# a different place without editing this script.
# It can also be set to an absolute path like the following examples:
#   export CFS_MISSION=/opt/missions/mission-xyz
#   export CFS_MISSION=C:/Projects/mission-xyz
# Having an absolute path would be necessary for including this file in a
# shared login script.
#
export CFS_MISSION=`pwd`

#
# Variable: APP_DIR
#
# APP_DIR is the location of the system applications
# This variable is typically set to $CFS_MISSION/apps
#
export APP_DIR=$CFS_MISSION/apps

#
# Variable: CFE_DIR
#
# CFE_DIR is the location of the core flight executive (cFE)
# This variable is typically set to $CFS_MISSION/cfe
#
export CFE_DIR=$CFS_MISSION/cfe

#
# Variable: OSAL_DIR
#
# OSAL_DIR is the location of the OS Abstraction Layer (OSAL)
# This variable is typically set to $CFS_MISSION/osal
#
export OSAL_DIR=$CFS_MISSION/osal

#
# Variable: PSP_DIR
#
# PSP_DIR is the location of the Platform Support Package (PSP)
# This variable is typically set to $CFS_MISSION/psp
#
export PSP_DIR=$CFS_MISSION/psp


###############################################################################
#     Typically there is no need to change anything below this line
###############################################################################

#
# Variable: CFS_MISSION_INC
#
# CFS_MISSION_INC is the location of the CFS mission include directory. This
# variable is separate because the entire "build" subdirectory is portable
# with the exception of this CFS_MISSION_INC directory.
# This variable is typically set to $CFS_MISSION/build/mission_inc
#
export CFS_MISSION_INC=$CFS_MISSION/build/mission_inc

#
# Variable: CFS_APP_SRC
#
# CFS_APPS is the location of the CFS Application source directory
# Normally, the this is located in the "apps" subdirectory of the mission
# directory, but it can be located anywhere like the following examples:
#   export CFS_APP_SRC=/opt/cfs-app-repository/
#   export CFS_APP_SRC=E:/projects/cfs/apps
#
export CFS_APP_SRC=$APP_DIR

#
# Variable: CFE_FSW
#
# CFE_FSW is the location of the root directory for the cFE flight s/w.
# This variable is used in the build makefiles to locate and copy the necessary
# include files required for building the cFE and cFE Unit Tests
export CFE_FSW=$CFE_DIR/fsw

#
# Variable: CFE_CORE_SRC
#
# CFE_CORE_SRC is the location of the cFE core source code.
# The variable must point to the location of the "fsw/cfe-core/src" subdirectory
# of the cFE distribution.
# within the cFE distribution. For example: cfe-5.1/fsw/cfe-core/src
# Normally, the cFE source is located in the top level of the mission build
# directory. If the cFE source is stored somewhere else in the file system, it
# can be set to an absolute path like the following examples:
#   export CFE_CORE_SRC=/opt/cfs/cfe-5.1/fsw/cfe-core/src
#   export CFE_CORE_SRC=F:/cfe-archive/cfe-5.1/fsw/cfe-core/src
#
export CFE_CORE_SRC=$CFE_DIR/fsw/cfe-core/src

#
# Variable: OSAL
#
# OSAL is the location of the OS Abstraction layer.
# This variable must point to the root directory of the OS Abstraction Layer
# distribution. This is needed by some makefiles to find required OSAL files.
# Normally, the OSAL source is located in the top level of the mission build
# directory. If the OSAL source is stored somewhere else in the file system,
# it can be set to an absolute path like the following examples:
#    export OSAL_SRC=/opt/cfs/osal-2.11
#    export OSAL_SRC=F:/osal-archive/osal-2.11
#
# export OSAL=$HOME/OSAL_JSC
export OSAL=$OSAL_DIR

#
# Variable: OSAL_SRC
#
# OSAL_SRC is the location of the OS Abstraction layer source code.
# This variable must point to the location of the "src/os" subdirectory of
# the OS Abstraction Layer distribution. The OSAL variable defined above is
# used to define this variable.
# Normally, the OSAL source is located in the top level of the mission build
# directory. If the OSAL source is stored somewhere else in the file system,
# it can be set to an absolute path like the following examples:
#    export OSAL_SRC=/opt/cfs/osal-2.11/src/os
#    export OSAL_SRC=F:/osal-archive/osal-2.11/src/os
#
export OSAL_SRC=$OSAL_DIR/src/os

#
# Variable: CFE_PSP_SRC
#
# CFE_PSP_SRC is the location of the cFE Platform Support Package (PSP) source
# code.
# For example:
#    export CFE_PSP_SRC=$CFS_MISSION/custom-psp
#
export CFE_PSP_SRC=$PSP_DIR/fsw

#
# Variable: MISSION
#
# MISSION is used in the Unit Test makefiles as the name of the current mission
# or product name. This is only used for display purposes when printing all the
# environment variables.
#
export MISSION=CFE_TST

#
# Variable: CFE_UNIT_TEST
#
# CFE_UNIT_TEST is the location of the cFE Unit Test source code.
# This variable is used in the platform-specific cfe/unit-test makefiles to
# locate and build the cFE Unit Tests
#
# Add this for the Unit Test stuff
# export CFE_UNIT_TEST=$HOME/JSC_UT/cFE_UT
export CFE_UNIT_TEST=$CFE_DIR/fsw/cfe-core/unit-test

#
# Variable: CFE_TOOLS
#
# CFE_TOOLS is the location of the root directory for the cFE tools.
# This variable is used in the platform-specific Makefiles to build the table
# generation tool (elf2cfetbl) executable
export CFE_TOOLS=$CFS_MISSION/tools

#
# Variable: UTF_SRC
#
# UTF_SRC is the location of the cFE Unit Test Framework source code.
# This variable must point to the location of the "cfe/tools/utf/" subdirectory.
#
export UTF_SRC=$CFE_DIR/tools/utf

#
###############################################################################
#             Please do not change anything below this line
###############################################################################
#
# Variable: MISSION_TEMPLATE_VERSION
#
# MISSION_TEMPLATE_VERSION shows the version number of the mission template
# directory tree.
#
export MISSION_TEMPLATE_VERSION=3.0.1
