# CMake snippet for building small_tools
#
# This directory is intended to be a place to keep
# small tools wanted during development and by the
# automatic build-and-test system.

add_executable(badchar badchar.c)

target_compile_options(badchar PUBLIC -std=c99)

install(TARGETS badchar DESTINATION host)

