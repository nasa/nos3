##############################################################################
## File:
##   $Id: Makefile 1.1.1.8 2012/05/24 11:08:20EDT jmdagost Exp  $
##
## Purpose:
##   Makefile for building the CFS Housekeeping (HK) UTF unit tests
##
##   $Log: Makefile  $
##   Revision 1.1.1.8 2012/05/24 11:08:20EDT jmdagost 
##   Updated makefile to reference environment variables.
##   Revision 1.1.1.7 2012/01/04 16:01:26EST jmdagost 
##   Replaced gcov rule in HK unit test makefile.  Left previous rule as comment for future reference.
##   Revision 1.1.1.6 2011/09/19 17:24:23EDT jmdagost 
##   Updated unit test and results files.
##   Revision 1.1.1.5 2010/05/21 14:17:07EDT jmdagost 
##   Updated for new OSAL paths
##   Revision 1.1.1.4 2010/05/11 16:19:59EDT jmdagost 
##   Major revisions to unit test and associated files, plus results.
##   Revision 1.1.1.3 2009/12/07 11:59:01EST jmdagost 
##   Added public_inc path, and tables path.
##   Modified options for 64-bit CentOS (Linux).
##   Changed paths to be relative from unit_test directory.
##   Updated for cFE 6.0 definitions and paths.
##   Revision 1.1.1.2 2008/10/17 15:46:52EDT rjmcgraw 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/project.pj
##   Revision 1.1 2008/10/17 15:29:34EDT rjmcgraw 
##   Initial revision
##   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/project.pj
##
##############################################################################
#
# Compiler/linker definitions
#
COMPILER=gcc
LINKER=gcc
ELF2TBL=$(CFE_CORE_SRC)/../../../tools/elf2cfetbl/elf2cfetbl
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
VPATH += $(CFS_APP_SRC)/hk/fsw/src
VPATH += $(CFS_APP_SRC)/hk/fsw/tables
#VPATH += $(CFS_APP_SRC)/cfs_lib/fsw/src
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
INCLUDES += -I$(CFS_APP_SRC)/hk/fsw/src
INCLUDES += -I$(CFS_APP_SRC)/hk/fsw/platform_inc
INCLUDES += -I$(CFS_APP_SRC)/hk/fsw/mission_inc
INCLUDES += -I$(CFS_APP_SRC)/hk/fsw/public_inc
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
UTF_OBJS += utf_cfe_sb_api.o
UTF_OBJS += utf_cfe_time_utils.o
UTF_OBJS += utf_cfe_psp_memutils.o
UTF_OBJS += utf_cfe_psp_memory.o
UTF_OBJS += utf_cfe_psp_memrange.o
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
# HK object files
#
HK_OBJS := hk_app.o
HK_OBJS += hk_utils.o
HK_OBJS += hk_cpy_tbl.o

#
###########################################################################
#
# Rules to make the specified targets
#   
%.o: %.c
	$(COMPILER) -c $(COPT) $(DEFAULT_COPT) $(GCOV_COPT) $(INCLUDES) $<

%.bin: %.o
	$(LINKER) $(LOPT) $(GCOV_LOPT) $^ -o $*.bin

all: utf_test_hk.bin defaulttbl

utf_test_hk.bin: utf_test_hk.o $(UTF_OBJS) $(CFE_OBJS) $(HK_OBJS)

defaulttbl ::
	$(ELF2TBL) hk_cpy_tbl.o ./cf/apps/
	cp ./cf/apps/hk_cpy_tbl.tbl ./cf/apps/hk_cpy_tbl2.tbl

clean ::
	rm -f *.o *.bin *.da *.out *.bbg *.bb *.gcda *.gcno *.gcov
	rm -f ./cf/apps/*.o
	rm -f ./cf/apps/*.tbl
	
## Note that the following gcov sequence uses two awk scripts that are not universally
## applied in the CFS unit test makefiles.  If you have these awk scripts, and have
## added the CFS_TOOLS environment variable to your shell script you can uncomment
## this gcov definition (and comment out the one below it).
## Note that you should also include the compiler option '-Wunreachable-code' to the
## DEFAULT_COPTS variable near the beginning of this makefile.	
#gcov ::
#	@echo
#	@gcov -b $(HK_OBJS:.o=.gcda) | awk -f $(CFS_TOOLS)/gcov_parser.awk
#	@awk -f $(CFS_TOOLS)/gcov_warning.awk $(CS_OBJS:.o=.c.gcov)
#	@rm -f *.gcda *.gcno
#	@echo	

gcov ::
	@echo
	@gcov $(HK_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

#
###########################################################################
#
# end of file
