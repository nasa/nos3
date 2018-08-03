###############################################################################
#
# File: CFE Core Subsystem Unit Test Makefile
#
# $Id: Makefile 1.11 2014/10/07 09:36:23GMT-05:00 acudmore Exp  $
#
# $Log: Makefile  $
# Revision 1.11 2014/10/07 09:36:23GMT-05:00 acudmore 
# Added $(ARCH_OPTS) to unit test link to allow it to link on 64 bit systems
# Revision 1.10 2014/09/05 14:27:28GMT-05:00 rmcgraw 
# DCR22107:21 Re-added gcov processing coverage to ccsds.c
# Revision 1.9 2014/06/12 14:41:50EDT wmoleski
# Updated the build/cpux/Makefile to contain comments about the Path variables and restored the
# Revision history to build/cpux/cfe/unit-test/xx/Makefiles.
# Revision 1.8 2014/05/28 07:45:34EDT wmoleski
# Checking in the files that were modified in the mission tree.
# Revision 1.7 2012/10/01 18:41:54EDT aschoeni
# Added includes for all cfe task directories to eliminate relative include paths in files
# Revision 1.6 2009/07/09 12:04:32EDT rmcgraw
# DCR8291:1 Changed CFE_MISSION_INC to CFS_MISSION_INC and added log if needed
#
###############################################################################

##
## Identify the cFE Core Subsystem here.
##
SUBSYS = sb

##
## The Unit Test target to build
##
TARGET = sb_UT

##
## Specify extra C Flags needed to build this subsystem
##
LOCAL_COPTS = -pg -fprofile-arcs -ftest-coverage $(OSCFLAGS)

##
## The Unit Test Objects needed
##
UT_OBJS = ut_stubs.o \
ut_bsp_stubs.o \
ut_osprintf_stubs.o \
ut_es_stubs.o \
ut_evs_stubs.o \
ut_fs_stubs.o \
ut_tbl_stubs.o \
ut_time_stubs.o \
sb_UT.o

##
## The cFE Subsystem Objects needed
##
SUBSYS_OBJS =  cfe_sb_task.o cfe_sb_api.o cfe_sb_priv.o cfe_sb_init.o \
cfe_sb_buf.o cfe_sb_util.o ccsds.o  cfe_sb_msg_id_util.o


##
## The cFE unit test object libraries needed
##
SUBSYS_LIBS = \
		../libut_osapi_stubs/libut_osapi_stubs.a \
		../libut_bsp/libut_bsp.a \
		../libut_assert/libut_assert.a

##
## Include all necessary cFE make rules
## Any of these can be copied to a local file and
## changed if needed.
##
##
##       cfe-config.mak contians arch, PSP, and OS selection
##
include ../../cfe-config.mak
##
##       debug-opts.mak contains debug switches
##
include ../../debug-opts.mak
##
##       compiler-opts.mak contains compiler definitions and switches/defines
##
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

##
## Define the OBJS macro for the compile and make clean rules
##
OBJS = $(UT_OBJS) $(SUBSYS_OBJS)

##
## Setup the include path for this subsystem
## The OS specific includes are in the compiler-opts.mak file
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(OSAL)/ut_assert/inc \
-I$(CFE_CORE_SRC)/$(SUBSYS) \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_CORE_SRC)/es \
-I$(CFE_CORE_SRC)/evs \
-I$(CFE_CORE_SRC)/fs \
-I$(CFE_CORE_SRC)/tbl \
-I$(CFE_CORE_SRC)/time \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFS_MISSION_INC) \
-I$(CFE_UNIT_TEST) \
-I../../../inc
#-I$(CFE_CORE_SRC)/../unit-test \

##
## Define the VPATH make variable.
## This can be modified to include source from another directory.
##
VPATH = $(CFE_CORE_SRC)/$(SUBSYS):$(OSAL_SRC)/$(OS):$(CFE_UNIT_TEST)

##
## Include the common make rules for the cFE Core subsystems
##
include $(CFE_CORE_SRC)/make/make-rules.mak

##
## Subsystem Link rule -- Overrides the default rule
##
$(TARGET): $(UT_OBJS) $(SUBSYS_OBJS) $(SUBSYS_LIBS)
	$(COMPILER) -o $(TARGET) $(UT_OBJS) $(SUBSYS_OBJS) -pg -fprofile-arcs -ftest-coverage $(ARCH_OPTS) $(SUBSYS_LIBS)

##
## gcov rule
##
gcov:
	@echo
	@echo "Running unit test: $@"
	@echo
	./$(TARGET)
	@rm -f gmon.out
	@echo
	@gcov ccsds.gcda | sed 'N;s/\n/ /' | \
			    sed -n '/File/p' | sed '/ads/d'  | \
		      sed 's/ Lines executed:/ /; s/File/gcov:/; s/of//'
	@gcov cfe_$(SUBSYS)*.gcda | sed 'N;s/\n/ /' | \
			    sed -n '/File/p' | sed '/ads/d'  | \
		      sed 's/ Lines executed:/ /; s/File/gcov:/; s/of//'
	@echo

