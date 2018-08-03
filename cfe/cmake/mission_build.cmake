#
# cFS mission project top-level makefile
# This will build cFE/CFS for each target machine
#
# Note that the target CPUs may use different architectures, therefore each
# architecture must be done as a separate sub-build since none of the binaries 
# can be shared.
#
# This implements the "top-level" script which divides the overall build by architecture
#

##################################################################
#
# FUNCTION: add_static_dependencies
#
# Adds an entry to the depedency list during app search
# (Note apps can depend on other apps/libs)
#
function(add_static_dependencies TGT_DEPS)

  set(FULLDEPS ${MISSION_DEPS} ${FIND_DEP_LIST})
  set(NEWDEPS)    
  foreach(DEP ${TGT_DEPS} ${ARGN})
    list(FIND FULLDEPS ${DEP} DEPIDX)
    if (DEPIDX LESS 0)
      list(APPEND NEWDEPS ${DEP})
      list(APPEND FULLDEPS ${DEP})
    endif()  
  endforeach()
  
  set(FIND_DEP_LIST ${FIND_DEP_LIST} ${NEWDEPS} PARENT_SCOPE)
  
endfunction(add_static_dependencies)



##################################################################
#
# FUNCTION: prepare
#
# Called by the top-level CMakeLists.txt to set up prerequisites
#
function(prepare)

  # Generate the cfe_mission_cfg.h wrapper file
  generate_config_includefile("inc/cfe_mission_cfg.h" mission_cfg.h ${MISSIONCONFIG} )
  
  # Create custom targets for building and cleaning all architectures
  # This is required particularly for doing extra stuff in the clean step
  add_custom_target(mission-all COMMAND $(MAKE) all)
  add_custom_target(mission-install COMMAND $(MAKE) install)
  add_custom_target(mission-clean COMMAND $(MAKE) clean)
  add_custom_target(mission-genfiles)
  
  # Locate the source location for all the apps found within the target file
  # Each of those may in turn have a "mission_build" file that calls out additional dependencies for that app,
  # so this is run in a loop until the list of unfound apps is empty
  string(REPLACE ":" ";" CFS_APP_PATH "$ENV{CFS_APP_PATH}")
  list(APPEND CFS_APP_PATH "apps" "apps/CFS" "cfe/fsw" "libs")
  set(MISSION_DEPS "cfe-core")
  if (ENABLE_FUNCTIONAL_TESTS)
    list(APPEND MISSION_DEPS "utexec" "ftctrl")
  endif()
  set(APP_MISSING_COUNT 0)

  # OSAL library must exist at ${MISSION_SOURCE_DIR}/osal
  # Set an explicit "osal_MISSION_DIR" variable so it can be referenced like any other dependency
  set(osal_MISSION_DIR ${MISSION_SOURCE_DIR}/osal)
  set(osal_MISSION_DIR ${MISSION_SOURCE_DIR}/osal PARENT_SCOPE)
  
  # First search for PSP modules - these should exist under psp/fsw/modules (only)
  # and they should be self-contained entities that do not depend on other things
  foreach(PSPMOD ${MISSION_PSPMODULES})
    if(IS_DIRECTORY ${MISSION_SOURCE_DIR}/psp/fsw/modules/${PSPMOD})
      set(MODFOUND ${MISSION_SOURCE_DIR}/psp/fsw/modules/${PSPMOD})
      include(${MODFOUND}/mission_build.cmake OPTIONAL)
      message(STATUS "PSP module '${PSPMOD}' found at ${MODFOUND}")
      set(${PSPMOD}_MISSION_DIR ${MODFOUND})
      set(${PSPMOD}_MISSION_DIR ${MODFOUND} PARENT_SCOPE)
    else()
      message("** PSP module ${PSPMOD} NOT found **")
      math(EXPR APP_MISSING_COUNT "${APP_MISSING_COUNT} + 1")
    endif()    
  endforeach(PSPMOD ${MISSION_PSPMODULES})
  
  # First search for the rest of CFS applications/libraries - these may exist in
  # a variety of places and also may in turn add more dependencies onto the list.
  # Repeat this loop until the list becomes empty.
  set(FIND_DEP_LIST ${MISSION_APPS} ${MISSION_DEPS})
  while(1)
    set(FIND_DEP_CURRSET ${FIND_DEP_LIST})
    set(FIND_DEP_LIST)
    foreach(APP ${FIND_DEP_CURRSET})
      set (APPFOUND)
      foreach(APPSRC ${CFS_APP_PATH})
        if(IS_DIRECTORY ${MISSION_SOURCE_DIR}/${APPSRC}/${APP})
          set(APPFOUND ${MISSION_SOURCE_DIR}/${APPSRC}/${APP})
          break()
        endif()
      endforeach(APPSRC ${CFS_APP_PATH})
      if (APPFOUND)
        set(${APP}_MISSION_DIR ${APPFOUND})
        set(${APP}_MISSION_DIR ${APPFOUND} PARENT_SCOPE)
        include(${APPFOUND}/mission_build.cmake OPTIONAL)
        message(STATUS "Module '${APP}' found at ${APPFOUND}")
      else()
        message("** Module ${APP} NOT found **")
        math(EXPR APP_MISSING_COUNT "${APP_MISSING_COUNT} + 1")
      endif()
    endforeach(APP ${FIND_DEP_CURRSET})
    if (NOT FIND_DEP_LIST)
      break()
    endif (NOT FIND_DEP_LIST)
    list(APPEND MISSION_DEPS ${FIND_DEP_LIST})
  endwhile(1)
  
  if (APP_MISSING_COUNT GREATER 0)
    message(FATAL_ERROR "Target build incomplete, source for ${APP_MISSING_COUNT} module(s) not found.")
  endif (APP_MISSING_COUNT GREATER 0)
  
  set(MISSION_DEPS ${MISSION_DEPS} PARENT_SCOPE)
  
  # Generate the tools for the native (host) arch
  add_subdirectory(${MISSION_SOURCE_DIR}/tools ../tools)

  # Add a dependency on "elf2cfetbl" as this is required for table builds
  # The "elf2cfetbl" target should have been added by the "tools" above
  if (NOT ${TGTNAME} STREQUAL "avr")
  	add_dependencies(mission-genfiles elf2cfetbl) 
  endif() 
  
  if (ENABLE_DATA_DICTIONARY)
    add_subdirectory(cmake/tools buildtools)  
  
    # Now that a complete list of mission dependencies is generated, 
    # check for the presence of additional special files under each app subdir:
    #  - The "functional-test" subdir will be installed to the host functional test dir
    #  - The "ui" subdir will be installed to the UI pages install dir
    #  - Any "*.datadict.xml" files will be used to generate the mission-wide data dictionary.
    set(DDXMLFILES)
    set(MISSION_DD_APPS)  
    foreach(APP ${MISSION_APPS} ${MISSION_DEPS})
      set(APPSRC ${${APP}_MISSION_DIR})
      if (IS_DIRECTORY ${APPSRC}/functional-test AND DEFINED FT_INSTALL_SUBDIR)    
        install(DIRECTORY ${APPSRC}/functional-test/ DESTINATION ${FT_INSTALL_SUBDIR})
      endif (IS_DIRECTORY ${APPSRC}/functional-test)
  
      if (IS_DIRECTORY ${APPSRC}/ui AND DEFINED UI_INSTALL_SUBDIR)
        install(DIRECTORY ${APPSRC}/ui/ DESTINATION ${UI_INSTALL_SUBDIR}/pages)
      endif()
  
      file(GLOB_RECURSE APPXML FOLLOW_SYMLINKS ${APPSRC}/datadict/*.xml)
      if(APPXML)
        message(STATUS "Module '${APP}' data dictionary found at ${APPXML}")
        list(APPEND DDXMLFILES ${APPXML})
        list(LENGTH APPXML XMLCOUNT)
        if (XMLCOUNT EQUAL 1)
          list(APPEND MISSION_DD_APPS ${APP})
        endif()
      endif(APPXML)
    endforeach(APP ${MISSION_DEPS})
  
    set(MISSION_DD_APPS ${MISSION_DD_APPS} PARENT_SCOPE)

    # Generate the target that converts the list of XML data dictionary files into the
    # corresponding C header and source files.  The "mission-cdecl" top level target is created for this.
    add_custom_command(
      OUTPUT 
        ${CMAKE_BINARY_DIR}/.gencdecl 
      COMMAND 
        gendatadict -O ${CMAKE_BINARY_DIR} ${DDXMLFILES}
      COMMAND 
        updatefiles ${CMAKE_BINARY_DIR}/inc ${CMAKE_BINARY_DIR}/src 
      COMMAND 
        ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/.gencdecl 
      DEPENDS 
        gendatadict updatefiles ${DDXMLFILES}
    )

    add_custom_target(mission-cdecl DEPENDS ${CMAKE_BINARY_DIR}/.gencdecl) 
    add_dependencies(mission-genfiles mission-cdecl) 
    if (DEFINED EDS_INSTALL_SUBDIR)
      install(FILES ${DDXMLFILES} DESTINATION ${EDS_INSTALL_SUBDIR})
    endif()

    # Repeat the data dictionary generation for the msglib test XML files
    # These are custom-crafted data dictionary files that are used during the unit tests of msglib
    file(GLOB_RECURSE UTXMLFILES FOLLOW_SYMLINKS ${msglib_MISSION_DIR}/unit-test/*.datadict.xml)
  
    # The list of XML files is sorted to ensure a consistent order of processing after each run
    # this is because the order of processing determines the order which apps are resgisted in the
    # global index files which in turn affects the runtime structure IDs.  This is a possible
    # issue for unit test that rely on structure IDs being the same from build to build. 
    list(SORT UTXMLFILES)
    add_custom_command(
      OUTPUT 
        ${CMAKE_BINARY_DIR}/unit-test/.gencdecl 
      COMMAND 
        gendatadict -O ${CMAKE_BINARY_DIR}/unit-test ${UTXMLFILES}
      COMMAND 
        updatefiles ${CMAKE_BINARY_DIR}/unit-test/inc ${CMAKE_BINARY_DIR}/unit-test/src 
      COMMAND 
        ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/unit-test/.gencdecl 
      DEPENDS 
        gendatadict updatefiles ${UTXMLFILES}
    )
    add_custom_target(mission-ut-cdecl DEPENDS ${CMAKE_BINARY_DIR}/unit-test/.gencdecl) 
    add_dependencies(mission-genfiles mission-ut-cdecl)
  endif(ENABLE_DATA_DICTIONARY) 
  
  # Generate a customized Doxyfile file for the Doxygen docs.  
  #  (This just sets the project name to identify the build where it came from.)
  # Also a "mission-doc" top level target is created for the doc generation  
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/doc")
  file(WRITE ${CMAKE_BINARY_DIR}/doc/Doxyfile
        "PROJECT_NAME           = ${MISSION_NAME}-${MISSIONCONFIG}\n"
        "PROJECT_BRIEF          = \"${MISSION_NAME} Core Flight Software (cFS) based project built using \\\"${MISSIONCONFIG}\\\" configuration\"\n"
        "@INCLUDE               = \$(CMAKE_SOURCE_DIR)/doc/Doxyfile\n")
  
  add_custom_target(mission-doc CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} MISSION_BUILD_DIR=${CMAKE_BINARY_DIR} doxygen WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc) 

  # Certain runtime variables need to be "exported" to the subordinate build, such as
  # the specific arch settings and the location of all the apps.  This is done by creating
  # a temporary file within the dir and then the subprocess will read that file and re-create
  # variables out of them.  The alternative to this is to specify many "-D" parameters to the
  # subordinate build but that would not scale well to many vars.
  set(VARLIST "MISSION_NAME" "SIMULATION" "TARGETSYSTEM" "ARCH_BINARY_DIR" "MISSION_DEFS" "MISSION_SOURCE_DIR" 
    "MISSION_BINARY_DIR" "MISSIONCONFIG" "MISSION_APPS" "MISSION_DD_APPS" "MISSION_PSPMODULES" "MISSION_DEPS"
    "SKIP_VERSION" "BUILD_SIMULATOR" "BUILD_TESTS")
  foreach(APP "osal" ${MISSION_APPS} ${MISSION_PSPMODULES} ${MISSION_DEPS})
    set(VARLIST ${VARLIST} "${APP}_MISSION_DIR")
  endforeach(APP ${MISSION_APPS})
  
  set(EXPORT_VARLIST ${VARLIST} PARENT_SCOPE)

  file(WRITE "${CMAKE_BINARY_DIR}/mission_vars.cache")
  
  foreach(VARL ${VARLIST})
    if (DEFINED ${VARL})
      file(APPEND "${CMAKE_BINARY_DIR}/mission_vars.cache" 
          "${VARL}\n"
          "${${VARL}}\n")
    endif()
  endforeach(VARL ${VARLIST})
  
  # Generate version information for the executable file.  This is done by executing a small CMAKE
  # at _build_ time  (not at prep time since it might change between now and then) that collects
  # the info out of the version control system in use (git is currently assumed). 
  add_custom_target(
    version
    COMMAND 
        ${CMAKE_COMMAND} -D BIN=${CMAKE_BINARY_DIR}
                         -P ${CMAKE_SOURCE_DIR}/cfe/cmake/version.cmake
    WORKING_DIRECTORY 
      ${CMAKE_SOURCE_DIR}                             
  )
  
  # Install the functional test code into the host directory
  if (IS_DIRECTORY ${MISSION_DEFS}/functional-test AND DEFINED FT_INSTALL_SUBDIR)
    install(DIRECTORY ${MISSION_DEFS}/functional-test/ DESTINATION ${FT_INSTALL_SUBDIR})
  endif()
  
  set(ENABLE_UNIT_TESTS $ENV{ENABLE_UNIT_TESTS} CACHE BOOL "Enable build of unit tests")
  
endfunction(prepare)

##################################################################
#
# FUNCTION: process_arch
#
# Called by the top-level CMakeLists.txt to set up targets for this arch
# This is where the real work is done
#
function(process_arch TARGETSYSTEM)
  set(CURRSYS "${SYSID_${TARGETSYSTEM}}")
  set(ARCH_BINARY_DIR "${CMAKE_BINARY_DIR}/${CURRSYS}")
  file(MAKE_DIRECTORY "${ARCH_BINARY_DIR}")

  message(STATUS "Configuring for system arch: ${CURRSYS}")
  
  # Note for parsing purposes it is easier to store each as a pair of lines; i.e. var name
  # on first line and value on second line.  This makes the recovery step easier.
  file(WRITE "${ARCH_BINARY_DIR}/mission_archbuild_vars.cache")
  
  if (ENABLE_UNIT_TESTS)
    file(APPEND "${ARCH_BINARY_DIR}/mission_archbuild_vars.cache" 
      "ENABLE_UNIT_TESTS\n"
      "TRUE\n")
  endif()
  
  foreach(VARL ${EXPORT_VARLIST})
    if (DEFINED ${VARL})
      file(APPEND "${ARCH_BINARY_DIR}/mission_archbuild_vars.cache" 
          "${VARL}\n"
          "${${VARL}}\n")
    endif()
  endforeach(VARL ${EXPORT_VARLIST})
  
  # Certain CMAKE cache variables _must_  be set using -D.  Those are added here.
  # Note - A warning is issued if you pass CMAKE_TOOLCHAIN_FILE to an already-configured build area
  # so an extra check is added to see if this is an initial run or a re-run by checking for a CMakeCache file.
  set(CURRSYS_CMAKE_OPTS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
  if (NOT CURRSYS STREQUAL "native" AND NOT EXISTS "${ARCH_BINARY_DIR}/CMakeCache.txt")
    # Find the toolchain file - allow a file in the mission defs dir to supercede one in the compile dir
    if (EXISTS ${MISSION_DEFS}/toolchain-${CURRSYS}.cmake)
      set(TOOLCHAIN_FILE ${MISSION_DEFS}/toolchain-${CURRSYS}.cmake)
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/cmake/toolchain-${CURRSYS}.cmake)
      set(TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-${CURRSYS}.cmake)
    else()
      message(FATAL_ERROR "Unable to find toolchain file for ${CURRSYS}")
    endif()
    list(APPEND CURRSYS_CMAKE_OPTS "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}")
  endif ()
  
  # Execute CMake subprocess to create a binary build tree for the specific CPU architecture
  execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} ${CURRSYS_CMAKE_OPTS} 
    WORKING_DIRECTORY "${ARCH_BINARY_DIR}" 
    RESULT_VARIABLE RESULT)
  if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to configure ${CURRSYS}")
  endif (NOT RESULT EQUAL 0)  
  
  # Hook the "make all", "make clean", and "make install" targets for the subordinate build
  # to top-level build targets prefixed by the CPU architecture.
  add_custom_target(${CURRSYS}-all 
   COMMAND 
      $(MAKE) all 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${CURRSYS}-clean 
   COMMAND 
      $(MAKE) clean 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )
  add_custom_target(${CURRSYS}-install 
   COMMAND 
      $(MAKE) install 
   WORKING_DIRECTORY 
      "${ARCH_BINARY_DIR}"
  )

  # All subordinate builds depend on the generated files and the version info being present first
  add_dependencies(${CURRSYS}-install mission-genfiles)
  add_dependencies(${CURRSYS}-all mission-genfiles) 
  add_dependencies(${CURRSYS}-install version)  
  add_dependencies(${CURRSYS}-all version)  

  add_dependencies(mission-all ${CURRSYS}-all)
  add_dependencies(mission-clean ${CURRSYS}-clean)
  add_dependencies(mission-install ${CURRSYS}-install)
  
endfunction(process_arch TARGETSYSTEM)


