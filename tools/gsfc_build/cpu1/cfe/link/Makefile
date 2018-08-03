##
##
## CFE Core  Link Makefile -- This makefile will link the cFE executable / Loadable Module
##
## 

##
## Define the EXE_DIR where the 'make install' rule will copy the cFE Core exe file.
##
EXE_DIR=../../exe

## 
## Include all necessary cFE make rules
## Any of these can be copied to a local file and 
## changed if needed.
##
##
##       cfe-config.mak contians arch, PSP, and OS selection
##
include ../cfe-config.mak
##
##       debug-opts.mak contains debug switches
##
include ../debug-opts.mak
##
##       compiler-opts.mak contains compiler definitions and switches/defines
##
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

##
## Define the link object for the cFE core
## This list should not change unless a cFE Core subsystem is added ( not likely )
##
CORE_OBJS = \
../psp/psp.o \
../osal/osal.o \
../es/es.o \
../sb/sb.o \
../time/time.o \
../evs/evs.o \
../fs/fs.o 

ifneq ($(EXCLUDE_CFE_TBL), TRUE)
CORE_OBJS+=../tbl/tbl.o
endif

##
## Include the Arch/Toolchain specific Link rules for the
## cFE Core.
##	
include $(CFE_PSP_SRC)/$(PSP)/make/link-rules.mak

#
# Default rule for building app
#
default: $(EXE_TARGET)

##
##  Install Rule
## 
install:
	$(CP) $(CORE_INSTALL_FILES) $(EXE_DIR)


##
## rule to clean the executable
##
clean:
	-$(RM) *.o
	-$(RM) *.c
	-$(RM) *.s
	-$(RM) vxWorks*
	-$(RM) *.bin

   
