######################################################################
# 
# Master config file for cFS target boards
#
# This file indicates the architecture and configuration of the
# target boards that will run core flight software.
#
# The following variables are defined per board, where <x> is the 
# CPU number starting with 1:
#
#  TGT<x>_NAME : the user-friendly name of the cpu.  Should be simple
#       word with no punctuation
#  TGT<x>_APPLIST : list of applications to build and install on the CPU
#  TGT<x>_PSP_MODULELIST : list of PSP modules to build and statically link into 
#       the CFE executable for this CPU.  These are initialized by the PSP if supported
#       by the PSP library in use.
#  TGT<x>_STATIC_APPLIST : list of modules (CFS applications/libraries) to build
#       and statically link into the CFE executable for this CPU.
#  TGT<x>_FILELIST : list of extra files to copy onto the target.  No
#       modifications of the file will be made.  In order to differentiate
#       between different versions of files with the same name, priority
#       will be given to a file named <cpuname>_<filename> to be installed
#       as simply <filename> on that cpu (prefix will be removed). 
#  TGT<x>_SYSTEM : the toolchain to use for building all code.  This
#       will map to a CMake toolchain file called "toolchain-<ZZZ>"
#       If not specified then it will default to "cpu<x>" so that
#       each CPU will have a dedicated toolchain file and no objects
#       will be shared across CPUs.  
#       Otherwise any code built using the same toolchain may be 
#       copied to multiple CPUs for more efficient builds.
#  TGT<x>_PLATFORM : configuration for the CFE core to use for this
#       cpu.  This determines the cfe_platform_cfg.h to use during the
#       build.  Multiple files/components may be concatenated together
#       allowing the config to be generated in a modular fashion.  If 
#       not specified then it will be assumed as "default <cpuname>".
# 

# The MISSION_NAME will be compiled into the target build data structure
# as well as being passed to "git describe" to filter the tags when building
# the version string.
SET(MISSION_NAME "SampleMission")

# SPACECRAFT_ID gets compiled into the build data structure and the PSP may use it.
# should be an integer.
SET(SPACECRAFT_ID 42)

# UI_INSTALL_SUBDIR indicates where the UI data files (included in some apps) should
# be copied during the install process.
SET(UI_INSTALL_SUBDIR "host/ui")

# FT_INSTALL_SUBDIR indicates where the black box test data files (lua scripts) should
# be copied during the install process.
SET(FT_INSTALL_SUBDIR "host/functional-test")

# common apps for all arch builds
# NOTE: libstfhw to_lab intentionally left off of this list because the order it is included is
# critical and differs between linux and avr builds.
SET(MISSION_APPLIST
         sch
         cfs_lib
         arducam
         novatel_oem615
         sample
         )

# either generic linux or nos3 linux depending on if building sim
SET(TGT1_NAME linux)
SET(TGT1_SYSTEM linux)

SET(TGT1_STATIC_APPLIST
         ci_lab
         hwlib
         to_lab
         ${MISSION_APPLIST}
         )

# dynamically generate toolchain file
SET(AUTO_LINUX_PSPNAME "pc-linux")
IF(BUILD_SIMULATOR)
    SET(AUTO_LINUX_PSPNAME "nos-linux")
ENDIF(BUILD_SIMULATOR)
CONFIGURE_FILE(${MISSION_DEFS}/toolchain-linux.cmake.in ${MISSION_DEFS}/toolchain-linux.cmake NEWLINE_STYLE UNIX)
 