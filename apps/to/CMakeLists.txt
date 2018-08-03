cmake_minimum_required(VERSION 2.6.4)
project(CFS_TO C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(../inc)
include_directories(${ci_MISSION_DIR}/fsw/platform_inc
                    ${ci_MISSION_DIR}/fsw/src
                    ${io_lib_MISSION_DIR}/fsw/public_inc
                    ${cf_MISSION_DIR}/fsw/platform_inc
                    ${sch_MISSION_DIR}/fsw/platform_inc
                    ${hs_MISSION_DIR}/fsw/platform_inc
                    ${hk_MISSION_DIR}/fsw/platform_inc)


set(TO_TRANSPORT udp)
set(MISSION_NAME CFS_TST)
execute_process(COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/fsw/examples/${TO_TRANSPORT}/MISSION_to_types.h ${CMAKE_CURRENT_SOURCE_DIR}/../inc/${MISSION_NAME}_to_types.h
                RESULT_VARIABLE rv)
message("NOTE: TO APP Transport selected ${TO_TRANSPORT}, copied to ${CMAKE_CURRENT_SOURCE_DIR}/../inc/${MISSION_NAME}_to_types.h")

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(to ${APP_SRC_FILES})

# Create the table object files 
include_directories(fsw/tables) 

# Add the tables
add_cfe_tables(to fsw/tables/to_config.c)
