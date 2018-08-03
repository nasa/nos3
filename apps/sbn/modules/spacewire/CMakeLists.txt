cmake_minimum_required(VERSION 2.6.4)
project(SBN_SPACEWIRE_MODULE C)

if(NOT(IS_DIRECTORY ${SBN_APP_SOURCE_DIR}))
    message(FATAL_ERROR "SBN_APP_SOURCE_DIR not defined, is sbn in the target list before this module?")
endif()

include_directories(${SBN_APP_SOURCE_DIR}/fsw/platform_inc)

if(NOT(IS_DIRECTORY ${SBN_LIB_SOURCE_DIR}))
    message(FATAL_ERROR "SBN_LIB_SOURCE_DIR not defined, is sbn_lib in the target list before this module?")
endif()

include_directories(${SBN_LIB_SOURCE_DIR}/fsw/public_inc)


aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(sbn_module_spacewire ${LIB_SRC_FILES})
