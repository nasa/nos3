# CMake snippet for building cmdUtil 

if (ENABLE_DATA_DICTIONARY)
  include_directories(${msglib_MISSION_DIR}/inc)
endif()

add_executable(cmdutil cmdUtil.c sendUdp.c)

if (ENABLE_DATA_DICTIONARY)
  target_link_libraries(cmdutil cfe_mission_dictionary msglib_full)
endif()

# ITC - suppress build warnings
disable_warnings(CMAKE_C_FLAGS ITC_C_FLAGS "format")
set_target_properties(cmdutil PROPERTIES COMPILE_FLAGS "${ITC_C_FLAGS}")

install(TARGETS cmdutil DESTINATION host)


