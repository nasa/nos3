###############################################################################
# File: cfe_es.mak
#
# Purpose:
#   Compile the Executive Services (ES) startup/runtime code.
#    The ES task is built with cfe_estask.mak
#
# History:
#   2004/11/19  S. Walling   : Initial revision for cFE
#
###############################################################################

# Subsystem produced by this makefile.
TARGET = es.o

#==============================================================================
# Object files required to build subsystem.

OBJS=cfe_es_start.o cfe_es_api.o cfe_es_objtab.o cfe_es_apps.o cfe_esmempool.o \
cfe_es_perf.o cfe_es_shell.o cfe_es_cds_mempool.o cfe_es_cds.o cfe_es_erlog.o \
cfe_es_task.o

#==============================================================================
# Source files required to build subsystem; used to generate dependencies.

SOURCES = $(OBJS:.o=.c)

