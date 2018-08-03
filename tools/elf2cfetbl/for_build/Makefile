
##
## Setup the include paths for this tool
##
INCLUDE_PATH = \
-I$(OSAL_SRC)/inc \
-I$(CFE_CORE_SRC)/tbl \
-I$(CFE_TOOLS)/elf2cfetbl \
-I$(CFE_CORE_SRC)/inc \
-I$(CFE_PSP_SRC)/pc-linux/inc \
-I$(CFE_PSP_SRC)/inc \
-I$(CFS_MISSION_INC) \
-I$ ../inc


##
## Define the C compiler
##
COMPILER = gcc

##
## These defines are different than the cross compiler settings since they are 
## used to compile a host build tool. 
## 
## These settings are assuming that the tool is being run on an x86 processor
## with a linux (or linux-like) operating system
## 
## They will need to be changed if the build host is a different processor, such as a Raspberry Pi, etc
## 
TARGET_DEFS = -D__ix86__ -D_ix86_ -D_LINUX_OS_ -DHWARCH=x86 -Dx86 -Dlinux -DX86PC -DBUILD=$(BUILD) -D_REENTRANT -D _EMBED_  

##
## Architecture / debug defines
## 
ARCH_DEFS = -m32 -g

## 
## Endian Defines
##
ENDIAN_DEFS=-D_EL -DENDIAN=_EL -DSOFTWARE_LITTLE_BIT_ORDER 

#
# The compile rule. Again, it is not based on the cross compiler for the target, but
# the compiler for the build host.
#
default: elf2cfetbl

elf2cfetbl:
	$(COMPILER) $(TARGET_DEFS) $(ENDIAN_DEFS) $(INCLUDE_PATH) $(ARCH_DEFS) -o elf2cfetbl \
            $(CFE_TOOLS)/elf2cfetbl/elf2cfetbl.c

clean:
	rm -f elf2cfetbl
