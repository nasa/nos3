##############################################################################
## File:
##   $Id: makefile 1.2 2016/07/26 13:19:04EDT czogby Exp  $
##
## Purpose:
##   Makefile for building the CFS Housekeeping (HS) UT Assert unit tests
##
##   $Log: makefile  $
##   Revision 1.2 2016/07/26 13:19:04EDT czogby 
##   Replace hard-coded paths with environment variables set in setvars.sh
##   Revision 1.1 2016/06/24 14:31:55EDT czogby 
##   Initial revision
##   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
##
##############################################################################
## GNU Makefile for building UT unit tests

#
# Supported MAKEFILE targets:
#   clean - deletes object files, executables, output files, and gcov files
#   all   - makes hs_testrunner.exe
#   run   - runs hs_testrunner.exe
#   gcov  - prints a GCOV coverage report (make all, make run, make gcov)
#
# GCOV is disabled by default.  If you are using the source level debugger you will want to 
# disable GCOV.  To enable GCOV you can override the ENABLE_GCOV variable on the command line 
# by setting it to TRUE.  For example "make ENABLE_GCOV=TRUE".
#

#
# VPATH specifies the search paths for source files outside of the current directory.  Note that
# all object files will be created in the current directory even if the source file is not in the 
# current directory.
#
VPATH := .
VPATH += $(UT_ASSERT_SRC)/src
VPATH += $(CFS_MISSION)/apps/cfs_lib/fsw/src
VPATH += ../src
VPATH += ../for_build

HS_PATH=../../

#
# INCLUDES specifies the search paths for include files outside of the current directory.  
# Note that the -I is required. 
#
INCLUDES := -I.
INCLUDES += -I$(UT_ASSERT_SRC)/inc
INCLUDES += -I$(CFS_MISSION)/build/pc-linux/inc
INCLUDES += -I$(APP_DIR)/cfs_lib/fsw/public_inc
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/inc
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/time
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/sb
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/es
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/evs
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/fs
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/tbl
INCLUDES += -I$(CFE_DIR)/fsw/mission_inc
INCLUDES += -I$(CFE_DIR)/fsw/platform_inc/cpu1
INCLUDES += -I$(OSAL_DIR)/src/os/inc
INCLUDES += -I$(OSAL_DIR)/build/inc
INCLUDES += -I$(PSP_DIR)/fsw/inc
INCLUDES += -I$(PSP_DIR)/fsw/pc-linux/inc
INCLUDES += -I$(PSP_DIR)/fsw/pc-linux/inc
INCLUDES += -I$(HS_PATH)/fsw/src
INCLUDES += -I$(HS_PATH)/fsw/mission_inc
INCLUDES += -I$(HS_PATH)/fsw/platform_inc

#
# FLIGHT_OBJS specifies flight software object files.
#
FLIGHT_OBJS := hs_app.o hs_cmds.o hs_custom.o hs_monitors.o

#
# UT_OBJS specifies unit test object files.
#
UT_OBJS := ut_osapi_stubs.o
UT_OBJS += ut_osfileapi_stubs.o
UT_OBJS += ut_cfe_sb_stubs.o
UT_OBJS += ut_cfe_es_stubs.o
UT_OBJS += ut_cfe_evs_stubs.o
UT_OBJS += ut_cfe_time_stubs.o
UT_OBJS += ut_cfe_fs_stubs.o
UT_OBJS += ut_cfe_psp_memutils_stubs.o
UT_OBJS += ut_cfe_psp_eeprom_stubs.o
UT_OBJS += ut_cfe_psp_ram_stubs.o
UT_OBJS += ut_cfe_psp_memrange_stubs.o
UT_OBJS += ut_cfe_psp_watchdog_stubs.o
UT_OBJS += ut_cfe_psp_timer_stubs.o
UT_OBJS += ut_cfe_tbl_stubs.o
UT_OBJS += ut_cfe_sb_hooks.o
UT_OBJS += ut_cfe_es_hooks.o
UT_OBJS += ut_cfe_evs_hooks.o
UT_OBJS += ut_cfe_tbl_hooks.o
UT_OBJS += ut_cfe_time_hooks.o
UT_OBJS += utassert.o
UT_OBJS += utlist.o
UT_OBJS += uttest.o
UT_OBJS += uttools.o
UT_OBJS += hs_app_test.o
UT_OBJS += hs_cmds_test.o
UT_OBJS += hs_custom_test.o
UT_OBJS += hs_monitors_test.o
UT_OBJS += hs_testrunner.o
UT_OBJS += hs_test_utils.o
UT_OBJS += cfs_utils.o

#
# UT_TEST_RUNNER specifies the name of the test runner.
#
UT_TEST_RUNNER = hs_testrunner.exe

###############################################################################

COMPILER=gcc
LINKER=gcc

#
# Compiler and Linker Options
#
GCOV_COPT = -fprofile-arcs -ftest-coverage -pg -p
GCOV_LOPT = -pg -p -fprofile-arcs -ftest-coverage -lgcov -lm

#WARNINGS = -Wall -W -ansi -Werror -Wstrict-prototypes -Wundef
WARNINGS = -Wall -Wstrict-prototypes -Wundef
DEBUGGER = -g

COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_ -DUT_VERBOSE -DOSAPI_NO_SPECIAL_ATTRIBS

LOPT = $(GCOV_LOPT)

###############################################################################
## Rule to make the specified TARGET
##
%.exe: %.o
	$(LINKER) $(LOPT) $^ -o $*.exe

###############################################################################
##  "C" COMPILER RULE
##
%.o: %.c
	$(COMPILER) -c $(COPT) $(INCLUDES) $<

##############################################################################
##

all:$(UT_TEST_RUNNER)

$(UT_TEST_RUNNER): $(UT_OBJS) $(FLIGHT_OBJS)

clean ::
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out

run ::
	./$(UT_TEST_RUNNER)

gcov ::
	@echo
	@gcov $(FLIGHT_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
		sed -n '/File/p' | sed '/ads/d' | sed -e '/\.h/d'  | \
		sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

# end of file
