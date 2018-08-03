#
# CPST project top-level makefile
# This will build cFE/CFS for each CPST target machine
#
# Note that the target CPUs may use different architectures, therefore each
# architecture must be done as a separate sub-build since none of the binaries 
# can be shared.
#
# This implements the architecture-specific build that generates the binaries 
# 

# Enable testing at the top level so test files will be generated here
# This is run at the global scope (not in a function)
enable_testing()

##################################################################
#
# FUNCTION: add_psp_module
#
# Simplified routine to add a driver to the PSP in use on this arch
# Called by module listfiles
#
function(add_psp_module MOD_NAME MOD_SRC_FILES)

  # Include the PSP shared directory so it can get to cfe_psp_module.h
  include_directories(${MISSION_SOURCE_DIR}/psp/fsw/shared)
  
  # Create the module
  add_library(${MOD_NAME} STATIC ${MOD_SRC_FILES} ${ARGN})

endfunction(add_psp_module)

##################################################################
#
# FUNCTION: add_cfe_app
#
# Simplified routine to add a CFS app or lib this arch
# Called by module listfiles
#
function(add_cfe_app APP_NAME APP_SRC_FILES)

  # currently this will build an app with either static linkage or shared/module linkage,
  # but this does not currently support both for a single arch (could be revised if that is needed)
  if (APP_INSTALL_LIST)
     set(APPTYPE "MODULE")
  else()
     set(APPTYPE "STATIC")
  endif()
    
  # Create the app module
  add_library(${APP_NAME} ${APPTYPE} ${APP_SRC_FILES} ${ARGN})
  
  # If the app has an associated data dictionary, then link with it
  # by convention the name of the data dictionary will be UPCASE_datadict
  string(TOUPPER "${APP_NAME}" APP_DD_NAME)
  set(APP_DD_NAME "${APP_DD_NAME}_datadict")
  if (TARGET ${APP_DD_NAME})
    target_link_libraries(${APP_NAME} ${APP_DD_NAME})
  endif()
    
  if (APP_INSTALL_LIST)
    # Override the default behavior of attaching a "lib" prefix
    set_target_properties(${APP_NAME} PROPERTIES PREFIX "")

    # Create the install targets for this shared/modular app
    foreach(TGT ${APP_INSTALL_LIST})
      install(TARGETS ${APP_NAME} DESTINATION ${TGT}/${INSTALL_SUBDIR})
    endforeach()
  else()
    # Set a compile definition to enable static linkage macros
    set_target_properties(${APP_NAME} PROPERTIES COMPILE_DEFINITIONS "CFS_STATIC_MODULE")
  endif()
  
endfunction(add_cfe_app)

##################################################################
#
# FUNCTION: add_cfe_tables
#
# Simplified routine to add CFS tables to be built with an app
#
function(add_cfe_tables APP_NAME TBL_SRC_FILES)

  # The table source must be compiled using the same "include_directories"
  # as any other target, but it uses the "add_custom_command" so there is
  # no automatic way to do this (at least in the older cmakes)
  get_current_cflags(TBL_CFLAGS ${CMAKE_C_FLAGS})

  # Create the intermediate table objects using the target compiler,
  # then use "elf2cfetbl" to convert to a .tbl file
  set(TBL_LIST)
  foreach(TBL ${TBL_SRC_FILES} ${ARGN})
  
    # Get name without extension (NAME_WE) and append to list of tables
    get_filename_component(TBLWE ${TBL} NAME_WE)
    list(APPEND TBL_LIST "${TBLWE}.tbl")
    
    # Check if an override exists at the mission level (recommended practice)
    # This allows a mission to implement a customized table without modifying
    # the original - this also makes for easier merging/updating if needed.
    if (EXISTS "${MISSION_DEFS}/tables/${TBLWE}.c")
      set(TBL_SRC "${MISSION_DEFS}/tables/${TBLWE}.c")
    elseif (EXISTS "${MISSION_SOURCE_DIR}/tables/${TBLWE}.c")
      set(TBL_SRC "${MISSION_SOURCE_DIR}/tables/${TBLWE}.c")
    elseif (IS_ABSOLUTE "${TBL}")
      set(TBL_SRC "${TBL}")
    else()
      set(TBL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${TBL}")
    endif()
    
    message("NOTE: Selected ${TBL_SRC} as source for ${TBLWE}")    
    
    # IMPORTANT: This rule assumes that the output filename of elf2cfetbl matches
    # the input file name but with a different extension (.o -> .tbl)
    # The actual output filename is embedded in the source file (.c), however
    # this must match and if it does not the build will break.  That's just the
    # way it is, because NO make system supports changing rules based on the
    # current content of a dependency (rightfully so).
    add_custom_command(
      OUTPUT ${TBLWE}.tbl
      COMMAND ${CMAKE_C_COMPILER} ${TBL_CFLAGS} -c -o ${TBLWE}.o ${TBL_SRC}
      COMMAND ${MISSION_BINARY_DIR}/tools/elf2cfetbl/elf2cfetbl ${TBLWE}.o
      DEPENDS ${MISSION_BINARY_DIR}/tools/elf2cfetbl/elf2cfetbl ${TBL_SRC}
    )
    
  endforeach(TBL ${TBL_SRC_FILES} ${ARGN})

  # Make a custom target that depends on all the tables  
  add_custom_target(${APP_NAME}_tables ALL DEPENDS ${TBL_LIST})
  
  # Create the install targets for all the tables
  # The full set of tables is installed for every target using this app 
  foreach(TGT ${APP_INSTALL_LIST})
    foreach(TBL ${TBL_LIST})
      install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TBL} DESTINATION ${TGT}/${INSTALL_SUBDIR})
    endforeach(TBL ${TBL_LIST})
  endforeach(TGT ${APP_INSTALL_LIST})
  
endfunction(add_cfe_tables)

##################################################################
#
# FUNCTION: add_static_cfe_tables
#
# Simplified routine to add CFS tables to be built with an app
#
function(add_static_cfe_tables APP_NAME TBL_SRC_FILES)

  # The table source must be compiled using the same "include_directories"
  # as any other target, but it uses the "add_custom_command" so there is
  # no automatic way to do this (at least in the older cmakes)
  get_current_cflags(TBL_CFLAGS ${CMAKE_C_FLAGS})

  #set(STATIC_TBL_LIST)
  foreach(TBL ${TBL_SRC_FILES} ${ARGN})
  
    # Get name without extension (NAME_WE) and append to list of tables
    get_filename_component(TBLWE ${TBL} NAME_WE)

    # Check if an override exists at the mission level (recommended practice)
    # This allows a mission to implement a customized table without modifying
    # the original - this also makes for easier merging/updating if needed.
    if (EXISTS "${MISSION_DEFS}/tables/${TBLWE}.c")
      set(TBL_SRC "${MISSION_DEFS}/tables/${TBLWE}.c")
    elseif (EXISTS "${MISSION_SOURCE_DIR}/tables/${TBLWE}.c")
      set(TBL_SRC "${MISSION_SOURCE_DIR}/tables/${TBLWE}.c")
    elseif (IS_ABSOLUTE "${TBL}")
      set(TBL_SRC "${TBL}")
    else()
      set(TBL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${TBL}")
    endif()
    
    message("NOTE: Selected ${TBL_SRC} as source for static table ${TBLWE}")

    add_library(LIB_${TBLWE} ${TBL_SRC})
    target_link_libraries(${APP_NAME} LIB_${TBLWE})

  endforeach(TBL ${TBL_SRC_FILES} ${ARGN})

  # Make a custom target that depends on all the tables  
  #add_custom_target(${APP_NAME}_tables ALL DEPENDS ${STATIC_TBL_LIST})
  #add_dependencies(${APP_NAME} ${APP_NAME}_tables)

endfunction(add_static_cfe_tables)

##################################################################
#
# FUNCTION: add_unit_test_lib
#
# Add a library for unit testing.  This is basically the same as the 
# normal CMake "add_library" but enables the code coverage compiler options.
#
function(add_unit_test_lib UT_NAME UT_SRCS)
    add_library(utl_${UT_NAME} STATIC  ${UT_SRCS} ${ARGN})
    set_target_properties(utl_${UT_NAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -pg --coverage")
endfunction(add_unit_test_lib)

##################################################################
#
# FUNCTION: add_unit_test_exe
#
# Create unit test executable.  This links the UT main executive with
# a library that is placed under test (created via add_unit_test_lib)
# It also registers the final executable target with ctest so it will
# be run during the "make test" target or when ctest is run.
#
function(add_unit_test_exe UT_NAME UT_SRCS)
    add_executable(${UT_NAME} ${utexec_MISSION_DIR}/src/utexec.c ${UT_SRCS} ${ARGN})
    
    get_target_property(UTCDEFS ${UT_NAME} COMPILE_DEFINITIONS)
    list(APPEND UTCDEFS "DEFAULT_REF_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
    
    get_target_property(UTCFLAGS ${UT_NAME} COMPILE_FLAGS)
    if (UTCFLAGS STREQUAL "UTCFLAGS-NOTFOUND")
        set(UTCFLAGS)
    endif()
    set(UTCFLAGS "${UTCFLAGS} -I${utexec_MISSION_DIR}/inc -I${CMAKE_CURRENT_SOURCE_DIR}")
    
    get_target_property(UTLFLAGS ${UT_NAME} LINK_FLAGS)
    if (UTLFLAGS STREQUAL "UTLFLAGS-NOTFOUND")
        set(UTLFLAGS)
    endif()
    set(UTLFLAGS "${UTLFLAGS} -pg --coverage")
    
    set_target_properties(${UT_NAME} PROPERTIES LINK_FLAGS "${UTLFLAGS}" COMPILE_DEFINITIONS "${UTCDEFS}" COMPILE_FLAGS "${UTCFLAGS}")
    target_link_libraries(${UT_NAME} utl_${UT_NAME}) 
    add_test(${UT_NAME} ${UT_NAME})
endfunction(add_unit_test_exe)

##################################################################
#
# FUNCTION: prepare
#
# Called by the top-level CMakeLists.txt to set up prerequisites
#
function(prepare)

  # Generate the "osconfig.h" wrapper file as indicated by the configuration
  # If specific system config options were not specified, use defaults
  if (NOT OSAL_SYSTEM_OSCONFIG)
    set(OSAL_SYSTEM_OSCONFIG default)
  endif (NOT OSAL_SYSTEM_OSCONFIG)    
  generate_config_includefile("inc/osconfig.h" osconfig.h ${OSAL_SYSTEM_OSCONFIG} ${TARGETSYSTEM})

  # If there are mission-specific or target-specific include directories, use them first
  # This allows overriding distribution files with custom versions if necessary
  # Doing this here keeps it consistent among all subdirectories
  get_filename_component(GLOBALINC "${CMAKE_BINARY_DIR}/../inc" ABSOLUTE)
  include_directories(${GLOBALINC})
  include_directories(${CMAKE_BINARY_DIR}/inc)
  
  # Allow sources to "ifdef" certain things if running on simulated hardware
  # This should be used sparingly, typically to fake access to hardware that is not present
  if (SIMULATION)
    add_definitions(-DSIMULATION=${SIMULATION})
  endif (SIMULATION)
  
  # Check that PSPNAME, BSPTYPE, and OSTYPE are set properly for this arch
  if (NOT CFE_SYSTEM_PSPNAME OR NOT OSAL_SYSTEM_OSTYPE)
    if (CMAKE_CROSSCOMPILING)
      message(FATAL_ERROR "Cross-compile toolchain ${CMAKE_TOOLCHAIN_FILE} must define CFE_SYSTEM_PSPNAME and OSAL_SYSTEM_OSTYPE")
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR 
            "${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
      # Export the variables determined here up to the parent scope
      SET(CFE_SYSTEM_PSPNAME      "pc-linux" PARENT_SCOPE)
      SET(OSAL_SYSTEM_OSTYPE      "posix-ng"    PARENT_SCOPE)
    else ()
      # Not cross compiling and host system is not recognized
      message(FATAL_ERROR "Do not know how to set CFE_SYSTEM_PSPNAME and OSAL_SYSTEM_OSTYPE on ${CMAKE_SYSTEM_NAME} system")
    endif()
  endif (NOT CFE_SYSTEM_PSPNAME OR NOT OSAL_SYSTEM_OSTYPE)
  
  # Truncate the global TGTSYS_LIST to be only the target architecture
  set(TGTSYS_LIST ${TARGETSYSTEM} PARENT_SCOPE)
  
  # Need to set ARCH_BINARY_DIR that is usually inherited from the mission build
  set(ARCH_BINARY_DIR ${CMAKE_BINARY_DIR} PARENT_SCOPE)
  
endfunction(prepare)


##################################################################
#
# FUNCTION: process_arch
#
# Called by the top-level CMakeLists.txt to set up targets for this arch
# This is where the real work is done
#
function(process_arch SYSVAR)

  # Check if something actually uses this arch; 
  # if this list is empty then do nothing, skip building osal/psp
  if (NOT DEFINED TGTSYS_${SYSVAR})
    return()
  endif()
  
  # Generate a list of targets that share this system architecture
  set(INSTALL_TARGET_LIST)
  foreach(TGTID ${TGTSYS_${SYSVAR}})
    set(TGTNAME ${TGT${TGTID}_NAME})
    if(NOT TGTNAME)
      set(TGTNAME "cpu${TGTID}")
    endif(NOT TGTNAME)
    list(APPEND INSTALL_TARGET_LIST ${TGTNAME})
  endforeach()
       
  # Include any specific compiler flags or config from the selected PSP
  include(${MISSION_SOURCE_DIR}/psp/fsw/${CFE_SYSTEM_PSPNAME}/make/build_options.cmake)
    
  # Configure OSAL target first, as it also determines important compiler flags
  add_subdirectory(${MISSION_SOURCE_DIR}/osal osal)
  
  # The OSAL displays its selected OS, so it is logical to display the selected PSP
  # This can help with debugging if things go wrong.
  message(STATUS "PSP Selection: ${CFE_SYSTEM_PSPNAME}")

  # Add all widely-used public headers to the include path chain
  # The "inc" directory in the binary dir contains the generated wrappers, if any
  include_directories(${MISSION_BINARY_DIR}/inc)
  include_directories(${ARCH_BINARY_DIR}/inc)
  include_directories(${MISSION_SOURCE_DIR}/osal/src/os/inc)
  include_directories(${MISSION_SOURCE_DIR}/psp/fsw/inc)  
  include_directories(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/inc)
  include_directories(${MISSION_SOURCE_DIR}/cfe/cmake/target/inc)
  # INCLUDE_REFACTOR: this is so the specific PSP config file can be found
  # This will become unnecessary when dependency refactoring is merged in 
  include_directories(${MISSION_SOURCE_DIR}/psp/fsw/${CFE_SYSTEM_PSPNAME}/inc)
    
  # The PSP and/or OSAL should have defined where to install the binaries.
  # If not, just install them in /cf as a default (this can be modified 
  # by the packaging script if it is wrong for the target)
  if (NOT INSTALL_SUBDIR)
    set(INSTALL_SUBDIR cf)
  endif (NOT INSTALL_SUBDIR)
      
  # Add any dependencies which MIGHT be required for subsequent apps/libs/tools
  # The cfe-core is handled explicitly later since it has special extra config
  foreach(DEP ${MISSION_DEPS})
    if (NOT DEP STREQUAL "cfe-core" AND EXISTS "${${DEP}_MISSION_DIR}/CMakeLists.txt")
      add_subdirectory(${${DEP}_MISSION_DIR} ${DEP})
    endif()
  endforeach(DEP ${MISSION_DEPS})
    
  # Clear the app lists
  set(ARCH_APP_SRCS)
  foreach(APP ${TGTSYS_${SYSVAR}_APPS})
    set(TGTLIST_${APP})
  endforeach()
  foreach(DRV ${TGTSYS_${SYSVAR}_DRIVERS})
    set(TGTLIST_DRV_${DRV})
  endforeach()
      
  # Process each target that shares this system architecture
  foreach(TGTID ${TGTSYS_${SYSVAR}})
  endforeach(TGTID ${TGTSYS_${SYSVAR}})

  # INCLUDE_REFACTOR: apps and the PSP like to #include cfe_platform_cfg.h -- they shouldn't
  # This will become unnecessary when dependency refactoring is merged in, but for now
  # they need to be able to find it.  Remove the next line once refactoring is merged.
  # Also do not do this if more than one CPU shares this architecture - this hack can only
  # be done if a 1:1 mapping between cpus and architectures (so all apps are rebuilt per-cpu)
  list(LENGTH TGTSYS_${SYSVAR} ARCHLEN)
  if (ARCHLEN EQUAL 1)
    include_directories(${ARCH_BINARY_DIR}/cfe_core_default_${TGT${TGTSYS_${SYSVAR}}_NAME}/inc)
  endif (ARCHLEN EQUAL 1)
        
  # Process each PSP module that is referenced on this system architecture (any cpu)
  foreach(PSPMOD ${TGTSYS_${SYSVAR}_PSPMODULES}) 
    message(STATUS "Building PSP Module: ${PSPMOD}")
    add_subdirectory(${${PSPMOD}_MISSION_DIR} psp/${PSPMOD})
  endforeach()
  
  # Process each app that is used on this system architecture
  set(APP_INSTALL_LIST)
  foreach(APP ${TGTSYS_${SYSVAR}_STATICAPPS})
    message(STATUS "Building Static App: ${APP}")
    add_subdirectory(${${APP}_MISSION_DIR} apps/${APP})
  endforeach()

  # Process each target that shares this system architecture
  foreach(TGTID ${TGTSYS_${SYSVAR}})
      
    set(TGTNAME ${TGT${TGTID}_NAME})
    if(NOT TGTNAME)
      set(TGTNAME "cpu${TGTID}")
    endif(NOT TGTNAME)
       
    set(TGTPLATFORM ${TGT${TGTID}_PLATFORM})
    if(NOT TGTPLATFORM)
      set(TGTPLATFORM "default" ${TGTNAME})
    endif(NOT TGTPLATFORM)

    string(REPLACE ";" "_" CFE_CORE_TARGET "cfe_core_${TGTPLATFORM}")
    if (NOT TARGET ${CFE_CORE_TARGET})

      # Generate wrapper file for the requisite cfe_platform_cfg.h file
      generate_config_includefile("${CFE_CORE_TARGET}/inc/cfe_msgids.h" msgids.h ${TGTPLATFORM})
      generate_config_includefile("${CFE_CORE_TARGET}/inc/cfe_platform_cfg.h" platform_cfg.h ${TGTPLATFORM})
      
      # Actual core library is a subdirectory
      add_subdirectory(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core ${CFE_CORE_TARGET})
      
      # Generate the UTF library
      # UTF is being deprecated in favor of utassert
      if (ENABLE_DEPRECATED)
        add_subdirectory(${MISSION_SOURCE_DIR}/tools/utf utf-${CFE_CORE_TARGET})
      endif (ENABLE_DEPRECATED)
      # utassert library
      add_subdirectory(${MISSION_SOURCE_DIR}/tools/ut_assert ${ARCH_BINARY_DIR}/utassert)
      # setup code coverage target
      setup_mission_coverage_target(${ARCH_BINARY_DIR})
    endif (NOT TARGET ${CFE_CORE_TARGET})

    # Append to the app install list for this CPU
    foreach(APP ${TGT${TGTID}_APPLIST})
      set(TGTLIST_${APP} ${TGTLIST_${APP}} ${TGTNAME})
    endforeach(APP ${TGT${TGTID}_APPLIST})
  
    # Target to generate the actual executable file
    add_subdirectory(cmake/target ${TGTNAME})
    
    foreach(INSTFILE ${TGT${TGTID}_FILELIST})
      if(EXISTS ${MISSION_DEFS}/${TGTNAME}_${INSTFILE})
        set(FILESRC ${MISSION_DEFS}/${TGTNAME}_${INSTFILE})
      elseif(EXISTS ${MISSION_DEFS}/${INSTFILE})
        set(FILESRC ${MISSION_DEFS}/${INSTFILE})
      else()
        set(FILESRC)
      endif()
      if (FILESRC)
        install(FILES ${FILESRC} DESTINATION ${TGTNAME}/${INSTALL_SUBDIR} RENAME ${INSTFILE})
      else(FILESRC)
        message("WARNING: Install file ${INSTFILE} for ${TGTNAME} not found")
      endif (FILESRC)
    endforeach(INSTFILE ${TGT${TGTID}_FILELIST})
  endforeach(TGTID ${TGTSYS_${SYSVAR}})
 
  # Configure the selected PSP
  # The naming convention allows more than one PSP per arch,
  # however in practice this gets too complicated so it is
  # currently a 1:1 relationship.  This may change at some point.
  add_subdirectory(${MISSION_SOURCE_DIR}/psp psp/${CFE_SYSTEM_PSPNAME})
        
  # Process each app that is used on this system architecture
  foreach(APP ${TGTSYS_${SYSVAR}_APPS})
    set(APP_INSTALL_LIST ${TGTLIST_${APP}})
    message(STATUS "Building App: ${APP} install=${APP_INSTALL_LIST}")
    add_subdirectory(${${APP}_MISSION_DIR} apps/${APP})
  endforeach()

endfunction(process_arch SYSVAR)

