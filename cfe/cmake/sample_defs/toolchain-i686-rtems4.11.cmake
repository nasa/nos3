# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

# In this sample application, the cross toolchain is configured to
# use a compiler for the RTEMS operating system targeting the "pc686" BSP

# Note that to use this, the "RTEMS" platform module may need to be added 
# to the system-wide CMake installation as a default CMake does not yet 
# recognize RTEMS as a system name.  An example of this is distributed with
# the pc-rtems PSP.

# Basic cross system configuration
SET(RTEMS_TOOLS_TOP "/opt/rtems-4.11")
SET(RTEMS_BSP_TOP "/opt/x-tools/rtems-4.11" CACHE PATH "Rtems install directory")

SET(CMAKE_SYSTEM_NAME         RTEMS)
SET(CMAKE_SYSTEM_VERSION      1)
SET(CMAKE_SYSTEM_PROCESSOR    i686)

# specify the cross compiler
SET(CMAKE_C_COMPILER          ${RTEMS_TOOLS_TOP}/bin/i386-rtems4.11-gcc)
SET(CMAKE_CXX_COMPILER        ${RTEMS_TOOLS_TOP}/bin/i386-rtems4.11-g++)
SET(CMAKE_LINKER              ${RTEMS_TOOLS_TOP}/bin/i386-rtems4.11-ld)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH      ${RTEMS_TOOLS_TOP}/i386-rtems4.11 
                              ${RTEMS_TOOLS_TOP}/lib/gcc/i386-rtems4.11/4.8.2 ${RTEMS_BSP_TOP}/i386-rtems4.11/pc686)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)

SET(CMAKE_PREFIX_PATH                   /)

# these settings are specific to cFE/OSAL and determines which
# abstraction layers are built when using this toolchain
SET(CFE_SYSTEM_PSPNAME                  pc-rtems)
SET(OSAL_SYSTEM_BSPTYPE                 pc-rtems)
SET(OSAL_SYSTEM_OSTYPE                  rtems)

# Forces the "Init" symbol to be undefined so it can be defined in a library
SET(CFE_ENTRY_SYM Init)

SET(CMAKE_C_FLAGS_INIT "-march=i686 -B${RTEMS_BSP_TOP}/i386-rtems4.11/pc686/lib" 
    CACHE STRING "C Flags required by platform")

# For RTEMS also install rtems-grub.cfg that allows booting using QEMU
macro(target_add_hook TGTNAME)
  install(FILES ${MISSION_DEFS}/${TGTNAME}_rtems-grub.cfg DESTINATION ${TGTNAME} RENAME rtems-grub.cfg)
endmacro()

