cmake_minimum_required(VERSION 2.6.4)
project(CFS_CI C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(../inc)
include_directories(${to_MISSION_DIR}/fsw/platform_inc)
include_directories(${to_MISSION_DIR}/fsw/src)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

set(CI_TRANSPORT udp)
set(MISSION_NAME CFS_TST)
execute_process(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/fsw/examples/${CI_TRANSPORT}/MISSION_ci_types.h ${CMAKE_CURRENT_SOURCE_DIR}/../inc/${MISSION_NAME}_ci_types.h
                RESULT_VARIABLE rv)
message("NOTE: CI APP Transport selected ${CI_TRANSPORT}, copied to ${CMAKE_CURRENT_SOURCE_DIR}/../inc/${MISSION_NAME}_ci_types.h")

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(ci ${APP_SRC_FILES})