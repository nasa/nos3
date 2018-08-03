cmake_minimum_required(VERSION 2.6.4)
project(CFS_HS C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(hs ${APP_SRC_FILES})

# Create table object files
include_directories(fsw/src)

# Add the tables
add_cfe_tables(hs fsw/tables/hs_amt.c fsw/tables/hs_emt.c fsw/tables/hs_mat.c fsw/tables/hs_xct.c)