cmake_minimum_required(VERSION 2.6.4)
project(CFS_CF C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src/PUB)

# include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/src/PRI PRI_SRC_FILES)

add_library(cfdp ${PRI_SRC_FILES})

# Create the app module
add_cfe_app(cf ${APP_SRC_FILES})
target_link_libraries(cf cfdp)

# Create table object files
include_directories(fsw/src)

# Add the tables
add_cfe_tables(cf fsw/tables/cf_cfgtable.c)