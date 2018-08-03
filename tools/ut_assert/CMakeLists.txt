cmake_minimum_required(VERSION 2.6.4)
project(utassert C)

include_directories(inc
                    ${ARCH_BINARY_DIR}/inc
                    ${MISSION_BINARY_DIR}/inc
                    ${MISSION_SOURCE_DIR}/osal/src/os/inc
                    ${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/inc
                    ${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/time)

aux_source_directory(src UTASSERT_SRC_FILES)

add_library(utassert STATIC ${UTASSERT_SRC_FILES})

# update utassert compile flags (suppress warnings)
disable_warnings(CMAKE_C_FLAGS UTASSERT_C_FLAGS "format")
set_target_properties(utassert PROPERTIES COMPILE_FLAGS "${UTASSERT_C_FLAGS}")

