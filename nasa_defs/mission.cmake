enable_testing()

# default to more verbose unit test output (add -DUT_VERBOSE to enable UtPrintf)
add_definitions(-DUT_VERBOSE_TEST_NAME)

# enable arg parsing
include(CMakeParseArguments)

# coverage pre-requisites
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

##################################################################
#
# FUNCTION: setup_mission_coverage_target
#
# Setup a code coverage target for the arch build.
##################################################################
function(setup_mission_coverage_target ARCH_BINARY_DIR)
    if(NOT BUILD_TESTS)
        return()
    endif()
    # currently limit to linux
    if(${TGTNAME} STREQUAL linux)
        set(COV_INFO stf1.info)
        set(COV_IDIR cfe/apps)
        set(COV_ODIR coverage)
        # setup code coverage reset target
        add_custom_target(coverage-clean
                          ${LCOV_PATH} --directory ${COV_IDIR} --zerocounters
                          COMMAND ${CMAKE_COMMAND} -E remove_directory ${COV_ODIR} 
                          COMMAND ${CMAKE_COMMAND} -E remove ${COV_INFO}
                          WORKING_DIRECTORY ${ARCH_BINARY_DIR})
        # setup code coverage target
        add_custom_target(coverage
                          ${CMAKE_CTEST_COMMAND} --quiet || true
                          COMMAND ${LCOV_PATH} --directory ${COV_IDIR} --capture --output-file ${COV_INFO}
                          COMMAND ${LCOV_PATH} --remove ${COV_INFO} unit_test/* --output-file ${COV_INFO}
                          COMMAND ${GENHTML_PATH} ${COV_INFO} --output-directory ${COV_ODIR}
                          DEPENDS coverage-clean
                          WORKING_DIRECTORY ${ARCH_BINARY_DIR})
        # show info target
        add_custom_command(TARGET coverage POST_BUILD
                           COMMENT "open ./${COV_ODIR}/index.html in your browser to view the coverage report")
    endif()
endfunction(setup_mission_coverage_target)

##################################################################
#
# FUNCTION: add_mission_unit_test
#
# Create an STF-1 unit test executable using the ut-assert library.
# The executable target is registered with ctest so it is run during
# the "make test" target or when ctest is run.
##################################################################
function(add_mission_unit_test UT_NAME UT_SRCS)
    if(NOT BUILD_TESTS)
    return()
    endif()

    set(options LINK_HWLIB)
    cmake_parse_arguments(UT "${options}" "" "" ${UT_SRCS} ${ARGN})

    include_directories(${MISSION_DEFS}
                    ${MISSION_SOURCE_DIR}/tools/ut_assert/inc
                    ${asfstub_SOURCE_DIR}/inc
                    ${MISSION_SOURCE_DIR}/stf_apps/hwlib/fsw/public_inc
                    ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src)
    if(UT_LINK_HWLIB)
    set(${UT_NAME}_LIBS utassert hwlibtest)
    endif()

    # unit test executable
    add_executable(${UT_NAME} ${UT_UNPARSED_ARGUMENTS})
    target_link_libraries(${UT_NAME} ${${UT_NAME}_LIBS})
    install(TARGETS ${UT_NAME} DESTINATION test)

    # unit test compile/link flags
    set(UT_COPT "${CMAKE_C_FLAGS} -g -O0 --coverage -pg -p -Wall -Wextra -Wno-error -Wno-redundant-decls")
    set(UT_LOPT "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set_target_properties(${UT_NAME} PROPERTIES COMPILE_FLAGS "${UT_COPT}" LINK_FLAGS "${UT_LOPT}")

    # generate avr binary image
    if(${TGTNAME} STREQUAL "avr")
    add_custom_command(TARGET ${UT_NAME}
                    POST_BUILD
                    COMMAND ${AVR_ROOT}/bin/avr32-objcopy -O binary -j .text -j .exception -j .data ${UT_NAME} ${UT_NAME}.bin)
    add_custom_target(${UT_NAME}.bin ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${UT_NAME})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${UT_NAME}.bin DESTINATION test/bin)
    endif()

    # add test
    add_test(${UT_NAME} ${UT_NAME})
endfunction(add_mission_unit_test)

##################################################################
#
# FUNCTION: add_mission_unit_test_lib
#
# Create unit test library using the ut-assert library.
##################################################################
function(add_mission_unit_test_lib UT_NAME UT_SRCS)
    if(NOT BUILD_TESTS)
        return()
    endif()

    # unit test library
    add_library(${UT_NAME} STATIC ${UT_SRCS} ${ARGN})
    target_link_libraries(${UT_NAME} a3200stub asfstub)

    # unit test compile/link flags
    set(UT_COPT "${CMAKE_C_FLAGS} -g -O0 --coverage -pg -p -Wall -Wextra -Wno-error -Wno-redundant-decls")
    set(UT_LOPT "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set_target_properties(${UT_NAME} PROPERTIES COMPILE_FLAGS "${UT_COPT}" LINK_FLAGS "${UT_LOPT}")
endfunction(add_mission_unit_test_lib)

##################################################################
#
# FUNCTION: disable_warnings
#
# Attempt to smartly remove list of warning types. If the warning
# exists in the string, replace with the negated flag
# (i.e.; -Wformat becomes -Wno-format), otherwise, just add the
# appropriate flag directly. This was chosen over simply adding
# the -w flag to inhibit all warning messages in favor of requiring
# explicit warning lists.
##################################################################
function(disable_warnings OLD_FLAGS NEW_FLAGS WARNINGS)
    # flag list
    set(_FLAGS ${${OLD_FLAGS}})
    if(NOT DEFINED _FLAGS)
        set(_FLAGS "")
    endif()
    separate_arguments(_FLAGS)

    # warning list
    set(_WARNINGS "${WARNINGS} ${ARGN}")
    separate_arguments(_WARNINGS)

    # iterate warnings
    foreach(_WARNING ${_WARNINGS})
        # remove warning flags
        list(REMOVE_ITEM _FLAGS "-W${_WARNING}" "-Wno-${_WARNING}")
        # append negated version
        list(APPEND _FLAGS "-Wno-${_WARNING}")
    endforeach()

    # convert flag list to string
    string(REPLACE ";" " " _FLAGS "${_FLAGS}")
    set(${NEW_FLAGS} ${_FLAGS} PARENT_SCOPE)
endfunction(disable_warnings)

