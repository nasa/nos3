cmake_minimum_required(VERSION 2.6.4)
project(NOS3)
include(CMakeParseArguments)

# find itc cmake module path
find_path(_ITC_CMAKE_MODULES_
          NAMES FindITC_Common.cmake
          PATHS ${ITC_CMAKE_MODULES}
                ${ITC_DEV_ROOT}/cmake/modules
                $ENV{ITC_DEV_ROOT}/cmake/modules
                /usr/local/cmake/modules
                /usr/cmake/modules
                /usr/lib/)
if(NOT _ITC_CMAKE_MODULES_)
    message(WARNING "Unable to find ITC CMake Modules")
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${_ITC_CMAKE_MODULES_})

# note: not sure why this needs to be re-configured for sim but cmake is resetting option values
set(CXX_11 ON)
 if(CXX_11)
    set(ITC_CXX_FLAGS "")   #Used for C++
	if(ITC_CXX_FLAGS)
		STRING(REGEX REPLACE "--std=c\\+\\+98" "" ITC_CXX_FLAGS ${ITC_CXX_FLAGS}) #Remove the C++98 Flag if it exists
		endif(ITC_CXX_FLAGS)		
    set(ITC_CXX_FLAGS "${ITC_CXX_FLAGS} --std=c++11")    
    set(CMAKE_CXX_FLAGS "-m32 -Wl,--disable-new-dtags -Werror -Wall -Wextra -pedantic -Wno-vla -Wwrite-strings -Wpointer-arith -Wcast-align")
    set(CMAKE_SHARED_LINKER_FLAGS "-m32 -Wl,--disable-new-dtags")
endif(CXX_11)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
include(MissionSettings.cmake)

# NOS3 Sim Core
add_subdirectory(sim_common)
add_subdirectory(nos_time_driver)
add_subdirectory(sim_terminal)
add_subdirectory(truth_42_sim)

# Add Component Sims
FILE(GLOB _ALL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../components/*)
FOREACH(_FILE ${_ALL_FILES})
  IF(EXISTS ${_FILE}/sim/CMakeLists.txt)
    get_filename_component(SIM_NAME ${_FILE} NAME)
    add_subdirectory(${_FILE}/sim ./${SIM_NAME})
  ENDIF()
ENDFOREACH()

# Install configuration files to bin
set(sim_cfg
    ../cfg/build/sims/nos_engine_server_config.json
    ../cfg/build/sims/nos3-simulator.xml
    ../cfg/build/sims/sim_log_config.xml
)
install(FILES ${sim_cfg} DESTINATION bin COMPONENT config)
