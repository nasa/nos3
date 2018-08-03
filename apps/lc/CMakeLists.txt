cmake_minimum_required(VERSION 2.6.4)
project(CFS_LC C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(lc ${APP_SRC_FILES})

# Create table object files
include_directories(fsw/src)

# Add the tables
add_cfe_tables(lc fsw/tables/lc_def_adt.c fsw/tables/lc_def_wdt.c)