## File:
##   $Id: makefile 1.7 2012/05/24 11:30:58EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Health and Safety (HS) UTF unit tests
##
##   $Log: makefile  $
##   Revision 1.7 2012/05/24 11:30:58EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.6 2012/01/09 19:28:18EST aschoeni 
##   Updated to support newest versions
##   Revision 1.5 2012/01/04 16:06:25EST jmdagost 
##   Corrected and updated gcov rule in MD unittest makefile.
##   Revision 1.4 2009/10/09 17:13:13EDT aschoeni 
##   Updated unit tests to use new tables
##   Revision 1.4 2009/08/20 16:01:24EDT aschoeni 
##   Added cfe_psp watchdog file to build
##   Revision 1.3 2009/06/24 16:20:38EDT aschoeni 
##   Changed bsp_timer.o to cfe_psp_timer.o
##   Revision 1.2 2009/06/04 18:48:31EDT aschoeni 
##   Updated Makefiles based on new directory structure
##   Revision 1.1 2009/05/22 17:56:18EDT aschoeni 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/unit_test/project.pj
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
COPT= -D__PPC__ -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -DCLOCKS_PER_SEC=1000000 -DS_IFDIR=16384
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
VPATH += $(CFS_APP_SRC)/md/fsw/src
VPATH += $(CFS_APP_SRC)/md/fsw/tables
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
INCLUDES += -I$(CFS_APP_SRC)/md/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/md/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/md/fsw/mission_inc
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
UTF_OBJS := utf_osapi.o
UTF_OBJS += utf_osfilesys.o
UTF_OBJS += utf_osfileapi.o
UTF_OBJS += utf_osloader.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_ram.o
UTF_OBJS += utf_cfe_psp_eeprom.o
UTF_OBJS += utf_cfe_psp_support.o
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_cfe_sb.o
UTF_OBJS += utf_sim.o
UTF_OBJS += utf_cfe_es_api.o
UTF_OBJS += utf_cfe_es_cds.o
UTF_OBJS += utf_cfe_esmempool.o
UTF_OBJS += utf_cfe_evs.o
UTF_OBJS += utf_cfe_tbl_api.o
UTF_OBJS += utf_cfe_tbl_internal.o
#
###########################################################################
#
# cFE object files
#
UTF_OBJS += ccsds.o
UTF_OBJS += cfe_sb_util.o
UTF_OBJS += cfe_time_api.o
UTF_OBJS += cfe_es_cds_mempool.o
UTF_OBJS += cfe_fs_api.o
UTF_OBJS += cfe_psp_timer.o
UTF_OBJS += cfe_psp_watchdog.o
#
###########################################################################
#
# MD object files
#
MD_OBJS := md_app.o
MD_OBJS += md_dwell_pkt.o
MD_OBJS += md_dwell_tbl.o
MD_OBJS += md_cmds.o
MD_OBJS += md_utils.o
MD_OBJS += cfs_utils.o

# Unit Test object files
UNIT_OBJS := UT_BuildLoadfiles.o

#
###########################################################################
#
# Rules to make the specified targets
#   
%.o: %.c
	$(COMPILER) -c $(COPT) $(DEFAULT_COPT) $(GCOV_COPT) $(INCLUDES) $<

%.exe: %.o
	$(LINKER) $(LOPT) $(GCOV_LOPT) $^ -o $*.exe


all:md_unit_test1.exe md_unit_test2.exe md_unit_test3.exe md_unit_test4.exe

md_unit_test1.exe: md_unit_test1.o $(UTF_OBJS) $(MD_OBJS)

md_unit_test2.exe: md_unit_test2.o $(UTF_OBJS) $(MD_OBJS)

md_unit_test3.exe: md_unit_test3.o $(UTF_OBJS) $(MD_OBJS)

md_unit_test4.exe: md_unit_test4.o $(UTF_OBJS) $(MD_OBJS)


clean ::
	rm -f *.o *.bin *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
	
gcov ::
	@echo
	@gcov $(MD_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo
#
###########################################################################
#
# end of file
