###############################################################################
#
# $Id: cfe_fs.mak 1.3 2008/07/08 14:40:39GMT-05:00 apcudmore Exp  $
#
# Purpose:  cFE File Services (FS) make file
#
# Author:   S.Walling/Microtel
#
# Notes:    Creates a combination of the File Services task and API library
#
# $Log: cfe_fs.mak  $
# Revision 1.3 2008/07/08 14:40:39GMT-05:00 apcudmore 
# Added CFE_FS global data, lock/unlock functions, log messages for decompress API.
# Revision 1.2 2008/06/20 15:40:29EDT apcudmore 
# Added support for OSAL based module loader
#  - Removed calls and references to the BSP based module loader
# Revision 1.1 2008/04/17 08:05:16EDT ruperera 
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/fs/project.pj
# Revision 1.3 2007/08/24 13:04:50EDT apcudmore 
# Updated cfe-core makefile for new build system
# Revision 1.2 2006/01/17 13:49:39EST apcudmore 
# Updated makefiles for new directory structure
# Revision 1.1 2005/06/09 14:57:49GMT rperera 
# Initial revision
# Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/fs/project.pj
# Revision 1.1  2005/04/08 22:21:18  swalling
# added new File Services task (mostly stub)
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = fs.o

#==============================================================================
# Object files required to build subsystem.

OBJS = cfe_fs_priv.o cfe_fs_api.o cfe_fs_decompress.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

