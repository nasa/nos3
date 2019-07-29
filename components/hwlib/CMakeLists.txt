cmake_minimum_required(VERSION 2.6.4)
project(CFS_HWLIB C)

# flight/nos psp names (based on CFE_SYSTEM_PSPNAME set by the toolchain from targets.cmake)
set(FLIGHT_PSPNAME "pc-linux")
set(NOS_PSPNAME "nos-linux")

include(../ComponentSettings.cmake)

# hwlib include directories
include_directories(fsw/mission_inc
                    fsw/platform_inc
                    fsw/public_inc)

# freertos include directories for flight configuration
IF(CFE_SYSTEM_PSPNAME STREQUAL FLIGHT_PSPNAME)
    message(STATUS "hwlib: FLIGHT configuration")
    #include_directories(flight needed includes here)
else()
    message(STATUS "hwlib: NON-FLIGHT configuration")
ENDIF()

# create the hwlib app module
aux_source_directory(fsw/src APP_SRC_FILES)
add_cfe_app(hwlib ${APP_SRC_FILES})

# stub out hardware for all non-flight configurations
IF(NOT CFE_SYSTEM_PSPNAME STREQUAL FLIGHT_PSPNAME)
    # nos specific platform
    IF(CFE_SYSTEM_PSPNAME STREQUAL NOS_PSPNAME)
        message(STATUS "NOS configuration")
        add_subdirectory(sim)
        target_link_libraries(hwlib noslink)
    ENDIF()
ENDIF()

