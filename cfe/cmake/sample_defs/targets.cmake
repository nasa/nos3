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
#  TGT<x>_DRIVERLIST : list of drivers to build and statically link into 
#       the PSP for this CPU. This requires the PSP iodriver enhancement.
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

# Each target board can have its own HW arch selection and set of included apps
SET(TGT1_NAME cpu1)
SET(TGT1_APPLIST sample_app sample_lib ci_lab to_lab sch_lab)
SET(TGT1_FILELIST cfe_es_startup.scr)

# CPU2/3 are duplicates of CPU1.  These are not built by default anymore but are
# commented out to serve as an example of how one would configure multiple cpus.
#SET(TGT2_NAME cpu2)
#SET(TGT2_APPLIST sample_app ci_lab to_lab sch_lab)
#SET(TGT2_FILELIST cfe_es_startup.scr)

#SET(TGT3_NAME cpu3)
#SET(TGT3_APPLIST sample_app ci_lab to_lab sch_lab)
#SET(TGT3_FILELIST cfe_es_startup.scr)

