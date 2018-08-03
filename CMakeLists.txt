cmake_minimum_required(VERSION 2.8)

project(mission)

include(CMakeParseArguments)

SET(FORCE_32 ON)
SET(BUILD_FLIGHT "NO" CACHE BOOL "Specifies whether to build the AVR flight target")
SET(BUILD_SIMULATOR "NO" CACHE BOOL "Specifies whether to build the NOS3 Simulator")
SET(BUILD_TESTS "NO" CACHE BOOL "Flag to enable building unit tests")
SET(SKIP_VERSION "NO" CACHE BOOL "Specifies whether to skip generating the CFE version.h file (which can be a slow process)")
SET(WIRING_PI "NO" CACHE BOOL "Specifies whether or not to include the wiringPi library")

# test endianess
include(TestBigEndian)
test_big_endian(BYTE_ORDER_BE)
if(BYTE_ORDER_BE)
    add_definitions(-DBYTE_ORDER_BE)
else()
    add_definitions(-DBYTE_ORDER_LE)
endif()

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

set(CMAKE_CXX_FLAGS "-m32")
set(CMAKE_SHARED_LINKER_FLAGS "-m32")

set(ENV{CFS_APP_PATH} "components:$ENV{CFS_APP_PATH}")
set(MISSION_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

include(nasa_defs/mission.cmake)

add_subdirectory(cfe)

if (BUILD_SIMULATOR)
	# add gtest to build
	if (NOT WIRING_PI)
	        include(SetupGTest)
	endif (NOT WIRING_PI)
	add_subdirectory(sims)
endif (BUILD_SIMULATOR)

