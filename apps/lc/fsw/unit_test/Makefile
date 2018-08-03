##############################################################################
## File:
##   $Id: Makefile 1.1.1.1 2012/10/01 18:44:15EDT lwalling Exp  $
##
## Purpose:
##   Makefile for building the CFS Limit Checker (LC) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.1.1.1 2012/10/01 18:44:15EDT lwalling 
##   Apply revision 1.3 changes to branch
##   Revision 1.3 2012/10/01 13:32:39PDT lwalling 
##   Remove LC/LCX folder name dependencies from Makefile
##   Revision 1.2 2012/10/01 07:58:29PDT lwalling 
##   Changed makefile folder references from LC to LCX
##   Revision 1.1 2012/07/31 13:53:42PDT nschweis 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/unit_test/project.pj
##   Revision 1.4 2012/05/24 11:33:50EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.3 2012/01/04 16:03:58EST jmdagost 
##   Added gcov rule to LC unit test makefile.
##   Revision 1.2 2009/12/28 14:19:06EST lwalling 
##   Update unit tests per latest version of CFE tools
##   Revision 1.1 2009/01/15 15:23:36EST dahardis 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/unit_test/project.pj
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
VPATH += ../src
VPATH += ../tables
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
INCLUDES += -I../src
INCLUDES += -I../platform_inc
INCLUDES += -I../mission_inc
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
# LC object files
#
LC_OBJS := lc_app.o
LC_OBJS += lc_cmds.o
LC_OBJS += lc_action.o
LC_OBJS += lc_watch.o
LC_OBJS += lc_custom.o
LC_OBJS += lc_def_adt.o
LC_OBJS += lc_def_wdt.o
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
all: lc_utest.exe 
#
lc_utest.exe: lc_utest.o lc_utest_adt.o lc_utest_wdt.o \
$(UTF_OBJS) $(CFE_OBJS) $(PSP_OBJS) $(CFSLIB_OBJS) $(LC_OBJS)
#
clean:
	rm -f *.o *.exe *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
#
gcov ::
	@echo
	@gcov $(LC_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo
#
###########################################################################
#
# end of file
