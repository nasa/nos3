project(nos_time_driver)

find_package(Boost REQUIRED QUIET COMPONENTS program_options filesystem) # system program_options filesystem)
find_package(ITC_Common REQUIRED COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED COMPONENTS common transport client server)

include_directories(
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(nos_time_driver_src
    src/nos_time_driver.cpp
)

set(nos_time_driver_libs
    sim_common
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_executable(nos-time-driver ${nos_time_driver_src})
target_link_libraries(nos-time-driver ${nos_time_driver_libs})
install(TARGETS nos-time-driver RUNTIME DESTINATION bin)
