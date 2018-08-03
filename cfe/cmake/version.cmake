
# This is a small cmake script to generate the version.h file for identification within the final EXE

set(GIT_EXECUTABLE git)

function(get_version DEP)
    if (DEP STREQUAL "MISSION")
      set(NAME ${MISSION_NAME})
      set(DIR ${MISSION_SOURCE_DIR})
    else()
      if(EXISTS ${${DEP}_MISSION_DIR}/version_info.cmake)
        include(${${DEP}_MISSION_DIR}/version_info.cmake)
      else()
        set(NAME ${DEP})
      endif()
      set(DIR ${${DEP}_MISSION_DIR})
    endif()
    if(NOT SKIP_VERSION)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty --match "${NAME}*"
            WORKING_DIRECTORY ${DIR} 
            OUTPUT_VARIABLE GIT_DESC_OUTPUT 
            RESULT_VARIABLE GIT_RESULT
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else ()
        set(GIT_RESULT 0)
        set(GIT_DESC_OUTPUT "UNVERSIONED")
    endif (NOT SKIP_VERSION)  
    
    if (NOT GIT_RESULT EQUAL 0)
      set(GIT_DESC_OUTPUT "error/unknown")
    endif()
    set(${DEP}_NAME "${NAME}" PARENT_SCOPE)
    set(${DEP}_VERSION "${GIT_DESC_OUTPUT}" PARENT_SCOPE)
endfunction()


# First read in any variables that are passed in from the parent process
# There may be many of these and they may not all be passable via -D options
file(STRINGS "${BIN}/mission_vars.cache" PARENTVARS)
set(VARNAME)
foreach(PV ${PARENTVARS})
  if (VARNAME)
    set(${VARNAME} ${PV})
    set(VARNAME)
  else()
    set(VARNAME ${PV})
  endif()
endforeach(PV ${PARENTVARS})

# Set other well-known locations
set(OSAL_MISSION_DIR ${MISSION_SOURCE_DIR}/osal)

# Set CMAKE to use old policy until cFS received update
cmake_policy(SET CMP0053 OLD)
# Start the template for configure_file() -- 
# see below why it is done this way and not written directly
file(WRITE ${BIN}/cmake_version.h.in 
    "/* Auto-generated version information file */\n"
    "#define MISSION_CONFIG      \"@MISSIONCONFIG@\"\n" 
    "#define MISSION_NAME        \"@MISSION_NAME@\"\n") 
    
# Get version for all mission apps/dependencies (they may be different)
foreach(DEP "MISSION" "osal" ${MISSION_APPS} ${MISSION_DEPS} ${MISSION_PSPMODULES})
  get_version(${DEP})
  string(TOUPPER ${${DEP}_NAME} MACRONAME)
  string(REGEX REPLACE "[^A-Z0-9_]+" "_" MACRONAME "${MACRONAME}")
  file(APPEND ${BIN}/cmake_version.h.in "#define ${MACRONAME}_VERSION \"@${DEP}_VERSION@\"\n") 
endforeach()

string(TOUPPER ${MISSION_NAME} MACRONAME)
file(APPEND ${BIN}/cmake_version.h.in "\n#define MISSION_VERSION ${MACRONAME}_VERSION\n\n /* end */\n")
   
# Same for build information -
# note this is done separately because the build time might change often -
# only the final target EXE should include build.h (nothing else in the mission) to minimize rebuilds
file(WRITE ${BIN}/build.h.in 
    "/* Auto-generated build information file */\n"
    "#define MISSION_BUILDDATE   \"@BUILDDATE@\"\n"
    "#define MISSION_BUILDUSER   \"@USERNAME@\"\n"
    "#define MISSION_BUILDHOST   \"@HOSTNAME@\"\n\n")

# OVERRIDE NOTE: 
# All 3 of these may be passed via environment variables to force a particular
# date, user, or hostname i.e. if hoping to reproduce an exact binary of a prior build
# They are actually free-form strings so they can be anything up to 64 chars.

# Get the current date and time
set(BUILDDATE $ENV{BUILDDATE})
if (NOT BUILDDATE)
    execute_process(
        COMMAND date "+%Y%m%d%H%M"
        OUTPUT_VARIABLE BUILDDATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif(NOT BUILDDATE)

# Get the build host
set(HOSTNAME $ENV{HOSTNAME})
if (NOT HOSTNAME)
    execute_process(
        COMMAND hostname
        OUTPUT_VARIABLE HOSTNAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT HOSTNAME)

# Get the user ID
set(USERNAME $ENV{USER})
if (NOT USERNAME)
    execute_process(
        COMMAND whoami
        OUTPUT_VARIABLE USERNAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT USERNAME)

# Use configure_file() command to generate the final output file because this can detect
# and only update it if it changes.
configure_file(${BIN}/cmake_version.h.in ${BIN}/inc/cmake_version.h @ONLY)
configure_file(${BIN}/build.h.in ${BIN}/inc/build.h @ONLY)


