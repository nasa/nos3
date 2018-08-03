project(noslink C)
cmake_minimum_required(VERSION 2.6.4)

set(CXX_11 ON)

find_package(NOSENGINE REQUIRED COMPONENTS uart i2c spi)

include_directories(inc
                    ${NOSENGINE_INCLUDE_DIRS}
                    ${MISSION_SOURCE_DIR}/components/hwlib/fsw/public_inc
                    ${MISSION_SOURCE_DIR}/osal/src/os/inc
                    ${MISSION_SOURCE_DIR}/psp/fsw/inc)

aux_source_directory(src NOSLINK_SRC)

add_library(noslink STATIC ${NOSLINK_SRC})
target_link_libraries(noslink ${NOSENGINE_LIBRARIES})

