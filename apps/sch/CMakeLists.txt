cmake_minimum_required(VERSION 2.6.4)
project(CFS_SCH C)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

# include app messages
include_directories(${ci_lab_MISSION_DIR}/fsw/platform_inc
                    ${to_lab_MISSION_DIR}/fsw/platform_inc
                    ${hk_MISSION_DIR}/fsw/platform_inc
                    ${sc_MISSION_DIR}/fsw/platform_inc
					          ${novatel_oem615_MISSION_DIR}/fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Swap out the sch_custom written for avr
if (NOT ${TGTNAME} STREQUAL "avr")
   list(REMOVE_ITEM APP_SRC_FILES fsw/src/sch_custom.c)
else ()
   list(REMOVE_ITEM APP_SRC_FILES fsw/src/sch_custom_linux.c)
endif ()

# Create the app module
add_cfe_app(sch ${APP_SRC_FILES})

# Create table object files
include_directories(fsw/src)

# Add the tables
add_static_cfe_tables(sch fsw/tables/sch_def_msgtbl.c fsw/tables/sch_def_schtbl.c)