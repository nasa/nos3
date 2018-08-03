cmake_minimum_required(VERSION 2.6.4)
project(SBN_APP C)

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)

if(BIG_ENDIAN)
    add_definitions(-DBIG_ENDIAN)
else(BIG_ENDIAN)
    add_definitions(-DLITTLE_ENDIAN)
endif(BIG_ENDIAN)

# workaround until these definitions are exposed by cfe_sb
include_directories(${CFECORE_SOURCE_DIR}/src/sb)

include_directories(fsw/platform_inc)

execute_process(
    COMMAND grep CFE_SB_SubRprtMsg_Payload_t ${CFECORE_SOURCE_DIR}/src/inc/cfe_sb_msg.h
    RESULT_VARIABLE subrprtpayload
)

if(NOT(${subrprtpayload}))
    add_definitions(-DSBN_PAYLOAD)
endif()

aux_source_directory(fsw/src APP_SRC_FILES)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(sbn ${APP_SRC_FILES})
