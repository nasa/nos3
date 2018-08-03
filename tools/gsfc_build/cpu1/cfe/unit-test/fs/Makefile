###############################################################################
#
# File: CFE Core Subsystem Unit Test Makefile 
#
# $Id: Makefile 1.12 2014/10/07 09:36:15GMT-05:00 acudmore Exp  $
#
# $Log: Makefile  $
# Revision 1.12 2014/10/07 09:36:15GMT-05:00 acudmore 
# Added $(ARCH_OPTS) to unit test link to allow it to link on 64 bit systems
# Revision 1.11 2014/06/12 13:41:51GMT-05:00 wmoleski 
# Updated the build/cpux/Makefile to contain comments about the Path variables and restored the 
# Revision history to build/cpux/cfe/unit-test/xx/Makefiles.
# Revision 1.10 2014/05/28 07:45:33EDT wmoleski 
# Checking in the files that were modified in the mission tree.
# Revision 1.9 2012/10/01 18:42:06EDT aschoeni 
# Added includes for all cfe task directories to eliminate relative include paths in files
# Revision 1.8 2011/11/30 14:20:30EST jmdagost 
# Removed references to unused and empty ut_estask_stubs file.
# Revision 1.7 2011/11/25 16:21:14EST lwalling 
# Added include path to OSAL build/inc folder to match new folder hierarchy
# Revision 1.6 2009/07/09 12:04:33EDT rmcgraw 
# DCR8291:1 Changed CFE_MISSION_INC to CFS_MISSION_INC and added log if needed
#
###############################################################################  

##
## Identify the cFE Core Subsystem here. 
##
SUBSYS = fs

##
## The Unit Test target to build
##
TARGET = fs_UT

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
ut_sb_stubs.o \
ut_time_stubs.o \
ut_tbl_stubs.o \
fs_UT.o

##
## The cFE Subsystem Objects needed
##
SUBSYS_OBJS = cfe_fs_decompress.o cfe_fs_api.o cfe_fs_priv.o
 

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
OBJS = $(SUBSYS_OBJS) $(UT_OBJS)
 
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
-I$(CFE_CORE_SRC)/sb \
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
$(TARGET): $(OBJS) $(SUBSYS_LIBS)
	$(COMPILER) -o $(TARGET) $(OBJS) -pg -fprofile-arcs -ftest-coverage $(ARCH_OPTS) $(SUBSYS_LIBS)

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
#	@gcov *.gcda | sed 'N;s/\n/ /' | 
	@gcov cfe_$(SUBSYS)*.gcda | sed 'N;s/\n/ /' | \
			    sed -n '/File/p' | sed '/ads/d'  | \
		            sed 's/ Lines executed:/ /; s/File/gcov:/; s/of//'
	@echo
   
