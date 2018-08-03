##############################################################################
## File:
##   $Id: Makefile 1.8 2012/05/24 11:11:29EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Data Storage (DS) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.8 2012/05/24 11:11:29EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.7 2012/01/04 15:57:14EST jmdagost 
##   Added gcov rule to unit test makefile.
##   Revision 1.6 2010/04/09 17:14:02EDT lwalling 
##   Change local path definitions from Windows Cygwin to MAC
##   Revision 1.5 2009/12/07 13:40:37EST lwalling 
##   Update DS unit tests, add unit test results files to MKS
##   Revision 1.4 2009/10/05 13:52:40EDT lwalling 
##   Change unit test makefile to reference table source files from tables folder
##   Revision 1.3 2009/08/07 16:25:01EDT lwalling 
##   Update cmd tests, create table tests, modify makefile
##   Revision 1.2 2009/06/16 17:04:38EDT lwalling 
##   Add references to sample table source files
##   Revision 1.1 2009/05/26 13:37:50EDT lwalling 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
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
VPATH += $(CFS_APP_SRC)/ds/fsw/src
VPATH += $(CFS_APP_SRC)/ds/fsw/tables
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
INCLUDES += -I$(CFS_APP_SRC)/ds/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/ds/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/ds/fsw/mission_inc
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
CFSLIB_OBJS := cfs_utils.o
#
###########################################################################
#
# DS object files
#
DS_OBJS := ds_app.o
DS_OBJS += ds_cmds.o
DS_OBJS += ds_file.o
DS_OBJS += ds_table.o
DS_OBJS += ds_file_tbl.o
DS_OBJS += ds_filter_tbl.o
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
all: ds_utest.exe 
#
ds_utest.exe: ds_utest.o ds_utest_app.o ds_utest_cmds.o ds_utest_file.o ds_utest_table.o \
$(UTF_OBJS) $(CFE_OBJS) $(PSP_OBJS) $(CFSLIB_OBJS) $(DS_OBJS)
#
clean:
	rm -f *.o *.exe *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
#
gcov ::
	@echo
	@gcov $(DS_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo
#
###########################################################################
#
# end of file