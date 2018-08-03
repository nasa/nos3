##############################################################################
## File:
##   $Id: Makefile 1.7 2012/05/24 11:15:22EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Checksum (SC) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.7 2012/05/24 11:15:22EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.6 2012/01/04 11:21:09EST jmdagost 
##   Added gcov rule.
##   Revision 1.5 2010/04/22 15:05:51EDT lwalling 
##   Change unit test makefile to reference rts001 and rts002
##   Revision 1.4 2010/04/15 16:49:41EDT lwalling 
##   Add default table files to unit test makefile
##   Revision 1.3 2010/03/11 10:22:06EST lwalling 
##   Change makefile per current developers environment
##   Revision 1.2 2009/02/19 10:06:50EST nyanchik 
##   Update SC To work with cFE 5.2 Config parameters
##   Revision 1.1 2009/01/26 14:20:12EST nyanchik 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/unit_test/project.pj
##   Revision 1.2 2008/09/08 10:39:37EDT njyanchik 
##   Checking in of Updated unit test
##   Revision 1.1 2008/07/23 15:28:18BST njyanchik 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/unit_test/project.pj
##
##############################################################################
#
# Compiler/linker definitions
#
COMPILER=gcc
LINKER=gcc
#
# Compiler/linker options
#
DEFAULT_COPT= -m32 -fPIC -Wall -Wstrict-prototypes -Wunreachable-code -ansi -g 
COPT= -D_ix86_ -DSOFTWARE_LITTLE_BIT_ORDER -D_EL
LOPT= -m32
#
# Options for gcov
#
GCOV_COPT= -fprofile-arcs -ftest-coverage
GCOV_LOPT= -pg -fprofile-arcs -ftest-coverage
#
###########################################################################
#
# Search path definitions
#
#
###########################################################################
#
# Source file path definitions
#
VPATH := .
VPATH += ..
VPATH += $(CFS_APP_SRC)/sc/fsw/src
VPATH += $(CFS_APP_SRC)/sc/fsw/tables
VPATH += $(CFS_APP_SRC)/cfs_lib/fsw/src
VPATH += $(UTF_SRC)/src
VPATH += $(CFE_CORE_SRC)/sb
VPATH += $(CFE_CORE_SRC)/time
VPATH += $(CFE_CORE_SRC)/es
VPATH += $(CFE_CORE_SRC)/evs
VPATH += $(CFE_CORE_SRC)/fs
VPATH += $(CFE_CORE_SRC)/tbl
VPATH += $(OSAL_SRC)/posix
VPATH += $(OSAL_SRC)/../bsp/pc-linux/src
VPATH += $(CFE_PSP_SRC)/pc-linux/src
#
###########################################################################
#
# Header file path definitions
#
INCLUDES := -I.
INCLUDES += -I..
INCLUDES += -I$(CFS_APP_SRC)/sc/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/sc/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/sc/fsw/mission_inc
INCLUDES += -I$(OSAL_SRC)/../../build/inc
INCLUDES += -I$(OSAL_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/pc-linux/inc
INCLUDES += -I$(UTF_SRC)/inc
INCLUDES += -I$(CFS_APP_SRC)/cfs_lib/fsw/public_inc
INCLUDES += -I$(CFE_CORE_SRC)/../../mission_inc
INCLUDES += -I$(CFE_CORE_SRC)/../../platform_inc/cpu1
INCLUDES += -I$(CFE_CORE_SRC)/inc
INCLUDES += -I$(CFE_CORE_SRC)/time
INCLUDES += -I$(CFE_CORE_SRC)/sb
INCLUDES += -I$(CFE_CORE_SRC)/es
INCLUDES += -I$(CFE_CORE_SRC)/evs
INCLUDES += -I$(CFE_CORE_SRC)/fs
INCLUDES += -I$(CFE_CORE_SRC)/tbl
#
###########################################################################
#
# UTF object files
#
UTF_OBJS := utf_cfe_es_api.o
UTF_OBJS += utf_cfe_es_cds.o
UTF_OBJS += utf_cfe_esmempool.o
UTF_OBJS += utf_cfe_evs.o
UTF_OBJS += utf_cfe_psp_eeprom.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_port.o
UTF_OBJS += utf_cfe_psp_ram.o
UTF_OBJS += utf_cfe_psp_support.o
UTF_OBJS += utf_cfe_sb.o
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_tbl_api.o
UTF_OBJS += utf_cfe_tbl_internal.o
UTF_OBJS += utf_cfe_time_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_osapi.o
UTF_OBJS += utf_osfileapi.o
UTF_OBJS += utf_osfilesys.o
UTF_OBJS += utf_osloader.o
UTF_OBJS += utf_ostimer.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_sim.o
#
###########################################################################
#
# cFE object files
#
CFE_OBJS := ccsds.o
CFE_OBJS += cfe_sb_util.o
CFE_OBJS += cfe_es_cds_mempool.o
CFE_OBJS += cfe_fs_api.o
#CFE_OBJS += cfe_time_api.o
#CFE_OBJS += bsp_timer.o
#
###########################################################################
#
# PSP object files
#
PSP_OBJS := cfe_psp_timer.o
#
###########################################################################
#
# CFSLIB object files
#
CFSLIB_OBJS :=
#
###########################################################################
#
# SC object files
#
SC_OBJS := sc_app.o
SC_OBJS += sc_atsrq.o
SC_OBJS += sc_cmds.o
SC_OBJS += sc_loads.o
SC_OBJS += sc_rtsrq.o
SC_OBJS += sc_state.o
SC_OBJS += sc_utils.o
SC_OBJS += sc_ats1.o
SC_OBJS += sc_rts001.o
SC_OBJS += sc_rts002.o

#
###########################################################################
#
# Rules to make the specified targets
#   
%.o: %.c
	$(COMPILER) -c $(COPT) $(DEFAULT_COPT) $(GCOV_COPT) $(INCLUDES) $<
#
%.exe: %.o
	$(LINKER) $(LOPT) $(GCOV_LOPT) $^ -o $*.exe 
#
all: utf_test_sc.exe
#
utf_test_sc.exe: utf_test_sc.o \
$(UTF_OBJS) $(CFE_OBJS) $(PSP_OBJS) $(CFSLIB_OBJS) $(SC_OBJS)
#
clean ::
	rm -f *.o *.exe *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
#
gcov ::
	@echo
	@gcov $(SC_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo
#
###########################################################################
#
# end of file
