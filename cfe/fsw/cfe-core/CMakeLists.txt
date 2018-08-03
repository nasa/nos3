cmake_minimum_required(VERSION 2.6.4)
project(CFECORE C)

# The CFE core target to generate MUST be given
# this allows more than one cfe core target per architecture 
if (NOT CFE_CORE_TARGET)
  message(FATAL_ERROR "CFE_CORE_TARGET must be defined to build cFE core")
endif (NOT CFE_CORE_TARGET)

# The cmake CURRENT binary include dir will have the generated platform config file (cfe_platform_cfg.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/inc)

# If the data dictionary/eds is present then the msglib library should be used to make use of it
if(ENABLE_DATA_DICTIONARY)
  include_directories(${msglib_MISSION_DIR}/inc)
endif(ENABLE_DATA_DICTIONARY)

add_definitions(-D_CFE_CORE_)

set(CFE_CORE_MODULES es sb evs tbl time fs)
set(CFE_ALL_MODULE_SRCS)

foreach(MODULE ${CFE_CORE_MODULES} config shared)
  aux_source_directory(src/${MODULE} CFE_ALL_MODULE_SRCS)
endforeach(MODULE ${CFE_CORE_MODULES})

add_library(${CFE_CORE_TARGET} STATIC ${CFE_ALL_MODULE_SRCS})

if(ENABLE_DATA_DICTIONARY)
  target_link_libraries(${CFE_CORE_TARGET} msglib cfecore_datadict)
endif(ENABLE_DATA_DICTIONARY)

if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)


