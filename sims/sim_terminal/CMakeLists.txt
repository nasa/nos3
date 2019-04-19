project(sim_terminal)

find_package(Boost REQUIRED QUIET COMPONENTS system program_options filesystem)
find_package(ITC_Common REQUIRED COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED COMPONENTS common transport client server i2c spi uart)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS}
                    inc
)

set(sim_terminal_src
    src/simulator_terminal.cpp
    src/bus_connections.cpp
)

# For Code::Blocks and other IDEs
#file(GLOB sim_terminal_inc inc/*.hpp)

set(sim_terminal_libs
    sim_common
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

# add_executable(nos3-simulator-terminal src/simulator_terminal.cpp)
# target_link_libraries(nos3-simulator-terminal ${sim_terminal_libs})
# install(TARGETS nos3-simulator-terminal RUNTIME DESTINATION bin)

add_library(sim_terminal SHARED ${sim_terminal_src} ${sim_terminal_inc})
target_link_libraries(sim_terminal ${sim_terminal_libs})
install(TARGETS sim_terminal LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_executable(nos3-simulator-terminal src/simulator_terminal.cpp)
target_link_libraries(nos3-simulator-terminal sim_terminal ${sim_terminal_libs})
install(TARGETS nos3-simulator-terminal RUNTIME DESTINATION bin)
# install(TARGETS sim_terminal LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)