##############################################################################
## GNU Makefile for building UT unit tests

#
# Supported MAKEFILE targets:
#   clean - deletes object files, executables, output files, and gcov files
#   all   - makes utf_test_runner.exe
#   run   - runs utf_test_runner.exe
#   gcov  - prints a GCOV coverage report (make all, make run, make gcov)
#
# GCOV is disabled by default.  If you are using the source level debugger you will want to 
# disable GCOV.  To enable GCOV you can override the ENABLE_GCOV variable on the command line 
# by setting it to TRUE.  For example "make ENABLE_GCOV=TRUE".
#

CFE_PATH=../../../../cFE611/fsw/cfe-core
OSAL_PATH=../../../../OSAL31
PSP_PATH=../../../../CFE_PSP11

#
# VPATH specifies the search paths for source files outside of the current directory.  Note that
# all object files will be created in the current directory even if the source file is not in the 
# current directory.
#
VPATH := ../src
VPATH += ../src/PRI
VPATH += ../src/PUB
VPATH += ./ut-assert/src

#
# INCLUDES specifies the search paths for include files outside of the current directory.  
# Note that the -I is required. 
#
INCLUDES := -I.
INCLUDES += -I..
INCLUDES += -I../src
INCLUDES += -I../src/PRI
INCLUDES += -I../src/PUB
#INCLUDES += -I../platform_inc
INCLUDES += -I../mission_inc
INCLUDES += -I./ut-assert/inc
INCLUDES += -I$(CFE_PATH)/os/inc
INCLUDES += -I$(CFE_PATH)/src/inc
INCLUDES += -I$(CFE_PATH)/src/time
INCLUDES += -I$(CFE_PATH)/src/sb
INCLUDES += -I$(CFE_PATH)/src/es
INCLUDES += -I$(CFE_PATH)/src/evs
INCLUDES += -I$(CFE_PATH)/src/fs
INCLUDES += -I$(CFE_PATH)/src/tbl
INCLUDES += -I$(CFE_PATH)/../mission_inc
INCLUDES += -I$(CFE_PATH)/../platform_inc/cpu1
INCLUDES += -I$(OSAL_PATH)/src/os/inc
INCLUDES += -I$(OSAL_PATH)/build/inc
INCLUDES += -I$(PSP_PATH)/fsw/inc
INCLUDES += -I$(PSP_PATH)/fsw/mac-osx/inc

#
# CF_OBJS specifies flight software object files.
#
CF_OBJS := cf_app.o
CF_OBJS += cf_utils.o
CF_OBJS += cf_callbacks.o
CF_OBJS += cf_cmds.o
CF_OBJS += cf_playback.o
CF_OBJS += cf_cfgtable.o

ENG_OBJS := aaa.o
ENG_OBJS += callbacks.o
ENG_OBJS += cfdp_lib.o
ENG_OBJS += default_filestore.o
ENG_OBJS += default_indication.o
ENG_OBJS += event.o
ENG_OBJS += machine_list.o
ENG_OBJS += message_class.o
ENG_OBJS += mib.o
ENG_OBJS += misc.o
ENG_OBJS += nak_mem.o
ENG_OBJS += nak.o
ENG_OBJS += pdu_as_string.o
ENG_OBJS += pdu.o
ENG_OBJS += r1.o
ENG_OBJS += r2.o
ENG_OBJS += s1.o
ENG_OBJS += s2.o
ENG_OBJS += timer.o
ENG_OBJS += utils.o

#
# UT_OBJS specifies unit test object files.
#
UT_OBJS := ut_osapi_stubs.o
UT_OBJS += ut_osfileapi_stubs.o
UT_OBJS += ut_cfe_psp_memutils_stubs.o
UT_OBJS += ut_cfe_sb_stubs.o
UT_OBJS += ut_cfe_sb_hooks.o
UT_OBJS += ut_cfe_es_stubs.o
UT_OBJS += ut_cfe_es_hooks.o
UT_OBJS += ut_cfe_evs_stubs.o
UT_OBJS += ut_cfe_evs_hooks.o
UT_OBJS += ut_cfe_tbl_stubs.o
UT_OBJS += ut_cfe_tbl_hooks.o
UT_OBJS += ut_cfe_fs_stubs.o
UT_OBJS += utassert.o
UT_OBJS += utlist.o
UT_OBJS += uttest.o
UT_OBJS += uttools.o
UT_OBJS += cf_testcase.o

###############################################################################

COMPILER=gcc
LINKER=gcc

#
# Compiler and Linker Options
#
ENABLE_GCOV = TRUE
ifeq ($(ENABLE_GCOV), TRUE)
GCOV_COPT = -fprofile-arcs -ftest-coverage -pg -p
GCOV_LOPT = -pg -p -fprofile-arcs -ftest-coverage -lgcov
endif

#WARNINGS = -Wall -W -ansi -Werror -Wstrict-prototypes -Wundef
WARNINGS = -Wall -Wstrict-prototypes
DEBUGGER = -g

COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_ -DUT_VERBOSE 
#COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_ 

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

all:cf_testrunner.exe

cf_testrunner.exe: cf_testrunner.o $(UT_OBJS) $(CF_OBJS) $(ENG_OBJS)

clean ::
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out

run ::
	./cf_testrunner.exe

#gcov ::
#	@echo
#	@gcov $(CF_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
#		sed -n '/File/p' | sed '/ads/d'  | \
#		sed 's/ Lines executed:/ /; s/File/gcov:/; s/of//'
#	@rm -f *.gcda *.gcno
#	@echo

gcov ::
	@echo
	@gcov $(CF_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
         sed -n '/File/p' | sed '/ads/d' | sed -e '/\.h/d'  | \
         sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

# end of file
