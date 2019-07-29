# CMake snippet for building elf2cfetbl
#
# WARNING: There are numerous hacks in here to get this to work,
# however when data dictionary / electronic data sheet functionality
# gets merged in then this all becomes moot, as tool becomes obsolete.
# (you can build tables from text files)

# cfe core, PSP, and OSAL public header files
include_directories(${MISSION_SOURCE_DIR}/osal/src/os/inc)
include_directories(${MISSION_SOURCE_DIR}/psp/fsw/inc)
include_directories(${cfe-core_MISSION_DIR}/src/inc)

# Hack: This needs to use tbl header files that are normally private to the cFE table app 
include_directories(${cfe-core_MISSION_DIR}/src/tbl)

# This uses the OSAL and PSP header files although it does not actually use the OSAL library
# or PSP.  Therefore in order for this to work there must be at least these files to satisfy
# the compiler when it needs to find them:
#  osconfig.h
#  cfe_psp_config.h
#  psp_version.h
include_directories(${MISSION_BINARY_DIR}/editor_inc)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/inc_stubs)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/inc_stubs/cfe_psp_config.h 
   "/* Empty placeholder cfe_psp_config.h file */\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/inc_stubs/psp_version.h
   "/* Empty placeholder psp_version.h file */\n")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/inc_stubs)

# force build as 32-bit
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

add_executable(elf2cfetbl elf2cfetbl.c)

install(TARGETS elf2cfetbl DESTINATION host)


