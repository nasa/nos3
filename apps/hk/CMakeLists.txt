cmake_minimum_required(VERSION 2.6.4)
project(CFS_HK C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(hk ${APP_SRC_FILES})

# Create table object files
include_directories(fsw/src)

# Add the tables
add_cfe_tables(hk fsw/tables/hk_cpy_tbl.c)