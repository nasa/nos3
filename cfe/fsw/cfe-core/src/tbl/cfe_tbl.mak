###############################################################################
#
# $Id: cfe_tbl.mak 1.1 2008/04/17 07:05:33GMT-05:00 ruperera Exp  $
#
# Purpose:  cFE Table Services (TBL) make file
#
# Author:   K.Audra(Microtel-LLC)
#
# Notes:    
#
# $Log: cfe_tbl.mak  $
# Revision 1.1 2008/04/17 07:05:33GMT-05:00 ruperera 
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
# Revision 1.4 2007/08/24 13:04:50EDT apcudmore 
# Updated cfe-core makefile for new build system
# Revision 1.3 2006/01/17 13:49:39EST apcudmore 
# Updated makefiles for new directory structure
# Revision 1.2 2005/11/10 20:39:35GMT dlkobe 
# Baseline version of cFE Table Services
# Revision 1.1 2005/09/08 13:54:28GMT-05:00 kkaudra 
# Initial revision
# Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/tbl/project.pj
# 
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = tbl.o

#==============================================================================
# Object files required to build subsystem.

OBJS =\
  cfe_tbl_internal.o cfe_tbl_api.o cfe_tbl_task.o cfe_tbl_task_cmds.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

