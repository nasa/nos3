project(noslink C)
cmake_minimum_required(VERSION 2.6.4)

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

set(CXX_11 ON)

find_package(NOSENGINE REQUIRED COMPONENTS uart i2c spi)

include_directories(inc
                    ${NOSENGINE_INCLUDE_DIRS}
                    ${MISSION_SOURCE_DIR}/components/hwlib/fsw/public_inc
                    ${MISSION_SOURCE_DIR}/osal/src/os/inc
                    ${MISSION_SOURCE_DIR}/psp/fsw/inc)

aux_source_directory(src NOSLINK_SRC)

add_library(noslink STATIC ${NOSLINK_SRC})
target_link_libraries(noslink ${NOSENGINE_LIBRARIES})

