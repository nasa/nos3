project(sim_common)

find_package(Boost REQUIRED QUIET COMPONENTS program_options filesystem)
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common)

add_definitions(-D_ENABLE_SOCKETS_)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc 
    ${ITC_Common_INCLUDE_DIRS}
    ${NOSENGINE_INCLUDE_DIRS}
)

set(sim_common_src
    src/sim_config.cpp
    src/sim_hardware_model_factory.cpp
    src/sim_data_provider_factory.cpp
    src/sim_data_42socket_provider.cpp
    src/sim_42data_point.cpp
    src/sim_coordinate_transformations.cpp
)

set(sim_common_libs
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
)

# For Code::Blocks and other IDEs
file(GLOB sim_common_inc inc/*.hpp)

set(sim_common_config
    cfg/nos3-simulator.xml
    cfg/sim_log_config.xml
    cfg/gps_data.42
)

add_library(sim_common SHARED ${sim_common_src} ${sim_common_inc})
target_link_libraries(sim_common ${sim_common_libs})

install(TARGETS sim_common LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ${sim_common_config} DESTINATION bin COMPONENT config)
