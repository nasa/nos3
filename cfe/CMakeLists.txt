#
# Core Flight Software Mission top-level CMake build script
# This will build cFS for all target machine(s) defined by the mission
#
# Note that the target CPUs may use different architectures, therefore each
# architecture must be done as a separate sub-build since none of the binaries 
# can be shared.
#
# This is actually two build scripts in one:
#   - A "top-level" script which divides the overall build by architecture
#       (This is run when TARGETSYSTEM is unset)
#   - An architecture-specific build that generates the binaries 
#       (This is run when TARGETSYSTEM is set)
# 


# Squelch a warning when building on Win32/Cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# The minimum CMake version is chosen because 2.6.4 is what is
# included by default with RHEL/Centos 5.x
cmake_minimum_required(VERSION 2.6.4)

# This top-level tier does not define ANY targets but we know
# that the subprojects will at least use the "C" language, so
# indicate that now.  Doing this early initializes the CFLAGS
# so they won't change later.
project(CFETOP C)

# Include the global routines
include("cmake/global_functions.cmake")

# call the setup_globals() function which determines
# the correct mission definitions path (among other things)
setup_globals()

# Load the target configuration information (used by all builds)
# This is at the top level so all vars set in here will become globals.
include(${MISSION_DEFS}/targets.cmake)
    
# Scan the list of targets and organize by target system type.
read_targetconfig()

# load a sub-script that defines the other functions, depending on whether
# the TARGETSYSTEM is defined or not
if(TARGETSYSTEM)
  include("cmake/arch_build.cmake")
else(TARGETSYSTEM)
  include("cmake/mission_build.cmake")
endif(TARGETSYSTEM)

# The prepare function should set up any necessary prerequisites for the build
if (COMMAND prepare)
  prepare()
endif (COMMAND prepare)

# Call the process_arch macro for each architecture
foreach(SYSVAR ${TGTSYS_LIST})
  process_arch(${SYSVAR})
endforeach(SYSVAR IN LISTS TGTSYS_LIST)

# The complete macro will do whatever is needed to finalize the build targets
if (COMMAND complete)
  complete()
endif (COMMAND complete)

