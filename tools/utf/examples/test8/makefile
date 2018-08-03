##############################################################################
## GNU Makefile for building UTF unit test utf_test8.
##     ** $Date: 2009/10/20 12:36:56GMT-05:00 $
##     ** $Revision: 1.4 $
##     ** $Log: makefile  $
##     ** Revision 1.4 2009/10/20 12:36:56GMT-05:00 wmoleski 
##     ** Updates to each example makefile to include the new cfe_psp filenames and remove the old utf_psp_support.c
##     ** file. Update to utf_test15.c to correct a typo. Updates to ActualOutput and ExpectedOutput where needed.
##     ** Revision 1.3 2009/10/13 15:23:38EDT wmoleski 
##     ** 
##     ** Revision 1.2 2008/08/20 15:08:03EDT wfmoleski 
##     ** Added the include of utf_cfe_time.h to utf_cfe.h and added the utf_cfe_time_api.o specification to each
##     ** example's makefile
##     ** Revision 1.1 2008/04/17 08:09:25EDT ruperera 
##     ** Initial revision

# path to source files not in build directory
# uncomment for Windows PATH:
VPATH := $(UTF_SRC)/src
VPATH += $(CFE_CORE_SRC)/sb
VPATH += $(CFE_CORE_SRC)/time
VPATH += $(CFE_CORE_SRC)/es
VPATH += $(CFE_CORE_SRC)/fs
VPATH += $(CFE_PSP_SRC)/pc-linux/src

# include paths
INCLUDES := -I.
INCLUDES += -I$(UTF_SRC)/inc
INCLUDES += -I$(CFE_CORE_SRC)/inc
INCLUDES += -I$(CFE_CORE_SRC)/sb
INCLUDES += -I$(CFE_CORE_SRC)/time
INCLUDES += -I$(CFE_CORE_SRC)/es
INCLUDES += -I$(CFE_CORE_SRC)/evs
INCLUDES += -I$(CFE_CORE_SRC)/tbl
INCLUDES += -I$(OSAL_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/pc-linux/inc
INCLUDES += -I$(CFS_MISSION_INC)
INCLUDES += -I../inc

# UTF object files
UTF_OBJS := utf_osapi.o
UTF_OBJS += utf_cfe_psp_eeprom.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_port.o
UTF_OBJS += utf_cfe_psp_ram.o
UTF_OBJS += utf_cfe_psp_support.o
UTF_OBJS += utf_osfilesys.o
UTF_OBJS += utf_osfileapi.o
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_time_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_cfe_sb.o
UTF_OBJS += utf_sim.o
UTF_OBJS += utf_cfe_es_api.o
UTF_OBJS += utf_cfe_esmempool.o
UTF_OBJS += utf_cfe_es_cds.o
UTF_OBJS += utf_cfe_tbl_api.o
UTF_OBJS += utf_cfe_tbl_internal.o
UTF_OBJS += utf_cfe_evs.o

# flight code object files
UTF_OBJS += ccsds.o
UTF_OBJS += cfe_sb_util.o
UTF_OBJS += cfe_es_cds_mempool.o
UTF_OBJS += cfe_fs_api.o
UTF_OBJS += cfe_psp_timer.o
UTF_OBJS += cfe_psp_watchdog.o

###############################################################################

COMPILER=gcc
LINKER=gcc

DEFAULT_COPT = -Wall -Wstrict-prototypes -ansi -g

COPT = -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_

LOPT =

echo "Building src in = "$INCLUDE

###############################################################################
## Rule to make the specified TARGET
##
%.exe: %.o
	$(LINKER) $(LOPT) $^ -o $*.exe

###############################################################################
##  "C" COMPILER RULE
##
%.o: %.c
	$(COMPILER) -c $(COPT) $(DEFAULT_COPT) $(INCLUDES) $<

##############################################################################
##

all:utf_test8.exe

utf_test8.exe: utf_test8.o $(UTF_OBJS)

clean ::
	rm -f *.o *.exe

# end of file
