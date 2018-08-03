##############################################################################
## File:
##   $Id: Makefile 1.6 2012/05/24 11:10:02EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Memory Manager (MM) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.6 2012/05/24 11:10:02EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.5 2012/01/04 11:35:20EST jmdagost 
##   Replaced gcov rule with new standard one. (Kept previous rule in a comment for future reference.)
##   Revision 1.4 2011/11/30 16:10:11EST jmdagost 
##   Added new unit test files.  Also modified gcov test formatting.
##   Revision 1.3 2010/05/27 15:16:51EDT jmdagost 
##   Updated paths and support filenames.
##   Revision 1.2 2008/09/22 14:29:29EDT dahardison 
##   Updated for cFE 5.2/OSAL 2.12 and MM v1.0.0.0
##   Revision 1.1 2008/05/19 15:27:49EDT dahardison 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/unit_test/project.pj
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
DEFAULT_COPT := -m32 -fPIC -Wall -Wstrict-prototypes -Wunreachable-code -ansi -g 
COPT = -D_ix86_ -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -DCLOCKS_PER_SEC=1000000 -DS_IFDIR=16384
## LOPT = -m elf_i386 -o
LOPT = -m32
#
# Options for gcov
#
GCOV_COPT= -fprofile-arcs -ftest-coverage
GCOV_LOPT= -pg -fprofile-arcs -ftest-coverage
#
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
VPATH += $(CFS_APP_SRC)/mm/fsw/src
#VPATH += $(CFS_APP_SRC)/mm/fsw/tables
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
INCLUDES += -I$(CFS_APP_SRC)/mm/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/mm/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/mm/fsw/mission_inc
INCLUDES += -I$(OSAL_SRC)/../../build/inc
INCLUDES += -I$(OSAL_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/inc
INCLUDES += -I$(CFE_PSP_SRC)/pc-linux/inc
#INCLUDES += -I$(CFE_PSP_SRC)/mac-osx/inc
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
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
UTF_OBJS += utf_cfe_psp_eeprom.o
UTF_OBJS += utf_cfe_psp_ram.o
UTF_OBJS += utf_cfe_psp_support.o
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
CFE_OBJS := ccsds.o
CFE_OBJS += cfe_sb_util.o
CFE_OBJS += cfe_time_api.o
CFE_OBJS += cfe_es_cds_mempool.o
CFE_OBJS += cfe_fs_api.o
CFE_OBJS += cfe_psp_timer.o
#
###########################################################################
#
# CFS Library object files
#
CFSLIB_OBJS := cfs_utils.o
#
###########################################################################
#
# MM object files
#
MM_OBJS := mm_app.o
MM_OBJS += mm_load.o
MM_OBJS += mm_dump.o
MM_OBJS += mm_mem32.o
MM_OBJS += mm_mem16.o
MM_OBJS += mm_mem8.o
MM_OBJS += mm_utils.o
#
###########################################################################
#
# MM unit test object files
#
UT_MM_OBJS := ut_mm_test_pokes.o
UT_MM_OBJS += ut_mm_test_peeks.o
UT_MM_OBJS += ut_mm_test_dumpinevent.o
UT_MM_OBJS += ut_mm_test_loadwid.o
UT_MM_OBJS += ut_mm_test_loadfromfile.o
UT_MM_OBJS += ut_mm_test_dumptofile.o
UT_MM_OBJS += ut_mm_test_fill.o
UT_MM_OBJS += ut_mm_test_symlookup.o
UT_MM_OBJS += ut_mm_test_symtbldump.o
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
all: utf_test_mm.exe 
#
utf_test_mm.exe: utf_test_mm.o $(UTF_OBJS) $(CFE_OBJS) $(CFSLIB_OBJS) $(MM_OBJS) $(UT_MM_OBJS)
#
clean ::
	rm -f *.o *.exe *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
#
## Note that the following gcov sequence uses two awk scripts that are not universally
## applied in the CFS unit test makefiles.  If you have these awk scripts, and have
## added the CFS_TOOLS environment variable to your shell script you can uncomment
## this gcov definition (and comment out the one below it).
## Note that you should also include the compiler option '-Wunreachable-code' to the
## DEFAULT_COPTS variable near the beginning of this makefile.	
#gcov ::
#	@echo
#	@gcov -b $(MM_OBJS:.o=.gcda) | awk -f $(CFS_TOOLS)/gcov_parser.awk
#	@awk -f $(CFS_TOOLS)/gcov_warning.awk $(CS_OBJS:.o=.c.gcov)
#	@rm -f *.gcda *.gcno
#	@echo	

gcov ::
	@echo
	@gcov $(MM_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo
##########################################################################
#
# end of file