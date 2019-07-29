project(CFEPSP C)

include(CMakeParseArguments)

# find itc cmake module path
find_path(_ITC_CMAKE_MODULES_
          NAMES FindITC_Common.cmake
          PATHS ${ITC_CMAKE_MODULES}
                ${ITC_DEV_ROOT}/cmake/modules
                $ENV{ITC_DEV_ROOT}/cmake/modules
                /usr/local/cmake/modules
                /usr/cmake/modules)
if(NOT _ITC_CMAKE_MODULES_)
    message(WARNING "Unable to find ITC CMake Modules")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${_ITC_CMAKE_MODULES_})

if (NOT CFE_SYSTEM_PSPNAME)
  message(FATAL_ERROR "CFE_SYSTEM_PSPNAME is not defined - do not know which to build")
endif()

if (CFE_SYSTEM_PSPNAME STREQUAL "nos-linux")
    # ITC Logger required for NOS Engine
    set(CXX_11 ON)
    find_package(NOSENGINE REQUIRED COMPONENTS client)
    include_directories(include
                        ${NOSENGINE_INCLUDE_DIRS}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../components/hwlib/sim/inc)
    message(STATUS "Set NOS Engine Includes")
endif()

if (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fsw/${CFE_SYSTEM_PSPNAME})
  message(FATAL_ERROR "PSP ${CFE_SYSTEM_PSPNAME} is not implemented")
endif()

add_definitions(-D_CFE_PSP_)
include_directories(fsw/${CFE_SYSTEM_PSPNAME}/inc)
include_directories(fsw/shared)
  
# Use all source files under the shared code directory (always)
# as well as all the files for the selected PSP
set(PSPFILES)
aux_source_directory(fsw/shared PSPFILES)
aux_source_directory(fsw/${CFE_SYSTEM_PSPNAME}/src PSPFILES)

add_library(psp-${CFE_SYSTEM_PSPNAME} STATIC ${PSPFILES})

if (CFE_SYSTEM_PSPNAME STREQUAL "nos-linux")
    target_link_libraries(psp-${CFE_SYSTEM_PSPNAME} ${NOSENGINE_LIBRARIES} noslink)
    message(STATUS "Set NOS Engine Libraries")
endif()
