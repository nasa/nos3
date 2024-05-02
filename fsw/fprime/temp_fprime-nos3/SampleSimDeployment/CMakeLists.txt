#####
# 'SampleSimDeployment' Deployment:
#
# This registers the 'SampleSimDeployment' deployment to the build system. 
# Custom components that have not been added at the project-level should be added to 
# the list below.
#
#####

###
# Topology and Components
###
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

# Add custom components to this specific deployment here
# add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MyComponent/")

#ITC SFTUFF:
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

find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart can i2c spi)

include_directories("../../../../components/sample/fsw/src")
include_directories("../.././") #device_cfg.h
include_directories("../../../../fsw/apps/hwlib/fsw/public_inc")
include_directories("../../../../components/sample/fsw/platform_inc")
include_directories("../../../../fsw/apps/hwlib/sim/inc")

set(SOURCE_FILES 
"${CMAKE_CURRENT_LIST_DIR}/Main.cpp"

#ITC STUFF
"${CMAKE_CURRENT_LIST_DIR}/../../../../components/sample/fsw/src/sample_device.h"
"${CMAKE_CURRENT_LIST_DIR}/../../../../components/sample/fsw/src/sample_device.c"
"${CMAKE_CURRENT_LIST_DIR}/../../device_cfg.h"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/fsw/public_inc/hwlib.h"
"${CMAKE_CURRENT_LIST_DIR}/../../../../components/sample/fsw/platform_inc/sample_platform_cfg.h"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/sim/src/libuart.c"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/sim/src/libcan.c"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/sim/src/libi2c.c"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/sim/src/libspi.c"
"${CMAKE_CURRENT_LIST_DIR}/../../../../fsw/apps/hwlib/sim/src/nos_link.c"
)



set(MOD_DEPS 
    ${FPRIME_CURRENT_MODULE}/Top
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)


register_fprime_deployment()
