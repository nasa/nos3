###############################################################################
# File: evs.mak
# $Id: cfe_evs.mak 1.1 2008/04/17 07:05:12GMT-05:00 ruperera Exp  $
#
# Purpose:
#   Compile the Event Services (EVS) task.
#
# $Date: 2008/04/17 07:05:12GMT-05:00 $
# $Revision: 1.1 $
# $Log: cfe_evs.mak  $
# Revision 1.1 2008/04/17 07:05:12GMT-05:00 ruperera 
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
# Revision 1.4 2007/08/24 13:04:49EDT apcudmore 
# Updated cfe-core makefile for new build system
# Revision 1.3 2006/01/17 13:49:41EST apcudmore 
# Updated makefiles for new directory structure
# Revision 1.2 2005/07/29 23:51:46GMT sstrege 
# EVS Code Review Action Items
# Revision 1.1 2005/06/09 10:57:48EDT rperera 
# Initial revision
# Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/evs/project.pj
# Revision 1.4  2005/04/20 16:58:53  sstrege
# Added cvs headers
#
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = evs.o

#==============================================================================
# Object files required to build subsystem.

OBJS=cfe_evs.o cfe_evs_task.o cfe_evs_log.o cfe_evs_utils.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

