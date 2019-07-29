project(example_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(example_sim_src
    src/foo_hardware_model.cpp
    src/bar_data_provider.cpp
    src/bar_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB example_sim_inc inc/*.hpp)

set(example_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(example_sim SHARED ${example_sim_src} ${example_sim_inc})
target_link_libraries(example_sim ${example_sim_libs})
install(TARGETS example_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_executable(nos3-example-simulator src/foosim.cpp)
target_link_libraries(nos3-example-simulator example_sim ${example_sim_libs})
install(TARGETS nos3-example-simulator RUNTIME DESTINATION bin)
install(TARGETS example_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
