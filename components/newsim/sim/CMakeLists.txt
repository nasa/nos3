project(newsim_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(newsim_sim_src
    src/newsim_hardware_model.cpp
    src/newsim_42_data_provider.cpp
    src/newsim_data_provider.cpp
    src/newsim_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB newsim_sim_inc inc/*.hpp)

set(newsim_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(newsim_sim SHARED ${newsim_sim_src} ${newsim_sim_inc})
target_link_libraries(newsim_sim ${newsim_sim_libs})
install(TARGETS newsim_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
