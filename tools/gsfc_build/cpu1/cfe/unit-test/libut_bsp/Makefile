# Build an intermediate UT-BSP static library
# to be used to link unit tests and applications,
# rather than forcing everyone to recompile all of
# the OSAL sources separately and repeatedly.

SUBSYS = libut_bsp
TARGET = libut_bsp.a
SUBSYS_OBJS = bsp_ut.o bsp_ut_voltab.o

include ../../cfe-config.mak
include ../../debug-opts.mak
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

ifeq ($(BSP),)
${error "BSP was not set. Please set it in your cfe-config.mak file."}
endif

OBJS = $(SUBSYS_OBJS) $(UT_OBJS)

INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(OSAL)/ut_assert/inc \
-I$(OSAL)/src/bsp/$(BSP)/ut-src \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_CORE_SRC)/evs \
-I$(CFE_CORE_SRC)/fs \
-I$(CFE_CORE_SRC)/sb \
-I$(CFE_CORE_SRC)/tbl \
-I$(CFE_CORE_SRC)/time \
-I$(CFE_PSP_SRC)/inc \
-I$(CFE_PSP_SRC)/$(PSP)/inc \
-I$(CFS_MISSION_INC) \
-I$(CFE_UNIT_TEST) \
-I../../../inc

VPATH = $(OSAL)/src/bsp/$(BSP)/ut-src

include $(CFE_CORE_SRC)/make/make-rules.mak

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
