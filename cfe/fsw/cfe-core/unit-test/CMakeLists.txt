# CMake snippet for CFE unit tests

enable_testing()

add_definitions(-DCFE_LINUX)

# The parent build may have specified extra C flags for use when unit testing
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${UT_C_FLAGS}")

# Because the object code is dependent on settings in cfe_platform_cfg.h,
# we must include the LIBNAME in all binary targets, as they must be rebuilt
# for each different platform config.
set (STUBNAME stub) 
if (CFE_CORE_TARGET)
  set(STUBNAME "${CFE_CORE_TARGET}_${STUBNAME}")
endif (CFE_CORE_TARGET)

set(STUBFILES bsp)

if (TARGET ut_osapi_stubs)
  # Enable a preprocessor flag to indicate we are using the stubs implemented in OSAL
  add_definitions("-DOSAL_UT_OSAPI_STUBS")
else()
  # Compile our own local version of the OSAPI stubs
  # Deprecated - this matched OSAL 4.1 but is easy to get out of sync with newer OSAL APIs
  list(APPEND STUBFILES osapi)
endif()

list(APPEND STUBFILES osprintf ${CFE_CORE_MODULES})

if (ENABLE_DATA_DICTIONARY)
  list(APPEND STUBFILES msglib)
endif()

# Allow direct inclusion of all private CFE header files
foreach(MODULE ${CFE_CORE_MODULES})
  get_filename_component(CFEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/${MODULE} ABSOLUTE)
  if (IS_DIRECTORY "${CFEDIR}")
    include_directories(${CFEDIR})
  endif()
endforeach()

# For each core module, generate the associated unit test
# This is done by linking the stubs of every OTHER module with the
# UT version of the real module (compiled with coverage flags)
foreach(MODULE ${CFE_CORE_MODULES})
  set(CFE_MODULE_FILES)
  set(CFE_UT_SRCS ut_stubs.c)
  aux_source_directory(../src/${MODULE} CFE_MODULE_FILES)
  aux_source_directory(../src/shared CFE_MODULE_FILES)
  foreach(STUB ${STUBFILES})
    if (NOT "${STUB}" STREQUAL "${MODULE}")
      list(APPEND CFE_UT_SRCS ut_${STUB}_stubs.c)
    endif()
  endforeach()  
  add_executable(${CFE_CORE_TARGET}_${MODULE}_UT ${MODULE}_UT.c ${CFE_UT_SRCS} ${CFE_MODULE_FILES})
  if (TARGET ut_osapi_stubs)
    target_link_libraries(${CFE_CORE_TARGET}_${MODULE}_UT ut_osapi_stubs ut_bsp)
  endif()
  add_test(${CFE_CORE_TARGET}_${MODULE}_UT ${CFE_CORE_TARGET}_${MODULE}_UT)
  install(TARGETS ${CFE_CORE_TARGET}_${MODULE}_UT DESTINATION ${TGTNAME}/${UT_INSTALL_SUBDIR})
endforeach(MODULE ${CFE_CORE_MODULES})

# Generate the FS test input files
# As these are just arbitrary data, they only have to be present - they do not need to be updated 
execute_process(COMMAND gzip -c ${CMAKE_CURRENT_SOURCE_DIR}/fs_UT.c OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.gz)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/fs_test.nongz "This is an ordinary file\n")


