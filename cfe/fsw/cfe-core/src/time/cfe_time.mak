###############################################################################
#
# $Id: cfe_time.mak 1.1 2008/04/17 07:05:38GMT-05:00 ruperera Exp  $
#
# Purpose:  cFE Time Services (TIME) make file
#
# Author:   S.Walling/Microtel
#
# Notes:    Creates a combination of the Time Services task and API library
#
# $Log: cfe_time.mak  $
# Revision 1.1 2008/04/17 07:05:38GMT-05:00 ruperera 
# Initial revision
# Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
# Revision 1.3 2007/08/24 13:04:51EDT apcudmore 
# Updated cfe-core makefile for new build system
# Revision 1.2 2006/01/17 13:49:39EST apcudmore 
# Updated makefiles for new directory structure
# Revision 1.1 2005/06/09 14:57:55GMT rperera 
# Initial revision
# Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/time/project.pj
# Revision 1.2  2005/03/18 20:45:33  swalling
# created new source file cfe_time_tone.c from portion of cfe_time_utils.c
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = time.o

#==============================================================================
# Object files required to build subsystem.

OBJS =\
  cfe_time_task.o cfe_time_api.o cfe_time_utils.o cfe_time_tone.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

