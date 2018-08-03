###############################################################################
# File: cfe_sb.mak
#
# Purpose:
#   Compile the Software Bus task.
#
# History:
###############################################################################

# Subsystem produced by this makefile.
TARGET = sb.o

#==============================================================================
# Object files required to build subsystem.

OBJS=\
 cfe_sb_task.o cfe_sb_api.o cfe_sb_priv.o cfe_sb_init.o cfe_sb_buf.o \
 cfe_sb_util.o ccsds.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

