cmake_minimum_required(VERSION 2.6.4)
project(CFS_ARDUCAM C)

include(../ComponentSettings.cmake)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)
aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(arducam ${APP_SRC_FILES})
target_link_libraries(arducam hwlib)

# Add HWIL libraries for communication
if (HWIL)
	include_directories(/usr/local/include/)
	target_link_libraries(arducam wiringPi)
	add_definitions(-DHWIL)
	message(STATUS "Loading HWIL libraries")
else ()
	message(STATUS "Ignoring HWIL libraries")
endif (HWIL)

# Unit Tests
aux_source_directory(fsw/unit_test UT_SRC_FILES)
add_mission_unit_test(test_cam ${UT_SRC_FILES} ${APP_SRC_FILES} LINK_HWLIB)
