cmake_minimum_required(VERSION 2.6.4)
project(CFS_NEWSIM_APP C)

include(../ComponentSettings.cmake)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(newsim ${APP_SRC_FILES})
