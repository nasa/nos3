project(cam_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client i2c spi)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(cam_sim_src
    src/cam_hardware_model.cpp
    src/cam_data_provider.cpp
    src/cam_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB cam_sim_inc inc/*.hpp)

set(cam_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(cam_sim SHARED ${cam_sim_src} ${cam_sim_inc})
target_link_libraries(cam_sim ${cam_sim_libs})
install(TARGETS cam_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_executable(nos3-cam-simulator src/camsim.cpp)
target_link_libraries(nos3-cam-simulator cam_sim ${cam_sim_libs})
install(TARGETS nos3-cam-simulator RUNTIME DESTINATION bin)
install(TARGETS cam_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES src/cam.bin DESTINATION bin)
