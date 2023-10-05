project(generic_star_tracker_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(generic_star_tracker_sim_src
    src/generic_star_tracker_hardware_model.cpp
    src/generic_star_tracker_42_data_provider.cpp
    src/generic_star_tracker_data_provider.cpp
    src/generic_star_tracker_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB generic_star_tracker_sim_inc inc/*.hpp)

set(generic_star_tracker_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(generic_star_tracker_sim SHARED ${generic_star_tracker_sim_src} ${generic_star_tracker_sim_inc})
target_link_libraries(generic_star_tracker_sim ${generic_star_tracker_sim_libs})
install(TARGETS generic_star_tracker_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
