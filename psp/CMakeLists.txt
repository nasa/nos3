project(CFEPSP C)

if (NOT CFE_SYSTEM_PSPNAME)
  message(FATAL_ERROR "CFE_SYSTEM_PSPNAME is not defined - do not know which to build")
endif()

if (CFE_SYSTEM_PSPNAME STREQUAL "nos-linux")
    # ITC Logger required for NOS Engine
    set(CXX_11 ON)
    find_package(NOSENGINE REQUIRED COMPONENTS client)
    include_directories(include
                        ${NOSENGINE_INCLUDE_DIRS}
                        ${noslink_SOURCE_DIR}/inc)
    message(STATUS "Set NOS Engine Includes")
endif()

if (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fsw/${CFE_SYSTEM_PSPNAME})
  message(FATAL_ERROR "PSP ${CFE_SYSTEM_PSPNAME} is not implemented")
endif()

add_definitions(-D_CFE_PSP_)
include_directories(fsw/${CFE_SYSTEM_PSPNAME}/inc)
include_directories(fsw/shared)
  
# Use all source files under the shared code directory (always)
# as well as all the files for the selected PSP
set(PSPFILES)
aux_source_directory(fsw/shared PSPFILES)
aux_source_directory(fsw/${CFE_SYSTEM_PSPNAME}/src PSPFILES)

add_library(psp-${CFE_SYSTEM_PSPNAME} STATIC ${PSPFILES})

if (CFE_SYSTEM_PSPNAME STREQUAL "nos-linux")
    target_link_libraries(psp-${CFE_SYSTEM_PSPNAME} ${NOSENGINE_LIBRARIES})
    message(STATUS "Set NOS Engine Libraries")
endif()
