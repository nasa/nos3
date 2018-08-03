project(gps_sim)

find_package(Boost REQUIRED QUIET COMPONENTS system program_options filesystem)
find_package(ITC_Common REQUIRED COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED COMPONENTS common transport client uart)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS}
)
include_directories(/usr/local/lib/sofa_c-20150209_a/sofa/20150209_a/c/src /usr/local/lib/sofa_c-20150209_a/build)
include_directories(/usr/local/lib/geotrans3.5/inc/dtcc /usr/local/lib/geotrans3.5/inc/ccservice /usr/local/lib/geotrans3.5/inc/ccs /usr/local/lib/geotrans3.5/build)

set(gps_sim_src
    src/gps_sim_hardware_model_OEM615.cpp
    src/gps_sim_data_file_provider.cpp
    src/gps_sim_data_42socket_provider.cpp
    src/gps_sim_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB gps_sim_inc inc/*.hpp)

set(gps_sim_libs
    sim_common
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
    sofa_c-20150209_a
    MSPCoordinateConversionService
    MSPdtcc
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(gps_sim SHARED ${gps_sim_src} ${gps_sim_inc})

add_library(sofa_c-20150209_a SHARED IMPORTED)
set_target_properties(sofa_c-20150209_a PROPERTIES IMPORTED_LOCATION /usr/local/lib/sofa_c-20150209_a/build/libsofa_c-20150209_a.so)
file(COPY /usr/local/lib/sofa_c-20150209_a/build/libsofa_c-20150209_a.so
     DESTINATION ${CMAKE_BINARY_DIR}/lib)

add_library(MSPCoordinateConversionService SHARED IMPORTED)
set_target_properties(MSPCoordinateConversionService PROPERTIES IMPORTED_LOCATION /usr/local/lib/geotrans3.5/build/libMSPCoordinateConversionService.so)
file(COPY /usr/local/lib/geotrans3.5/build/libMSPCoordinateConversionService.so
     DESTINATION ${CMAKE_BINARY_DIR}/lib)

add_library(MSPdtcc SHARED IMPORTED)
set_target_properties(MSPdtcc PROPERTIES IMPORTED_LOCATION /usr/local/lib/geotrans3.5/build/libMSPdtcc.so)
file(COPY /usr/local/lib/geotrans3.5/build/libMSPdtcc.so
     DESTINATION ${CMAKE_BINARY_DIR}/lib)

# geotrans_data
file(COPY /usr/local/lib/geotrans3.5/data/3_param.dat
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/7_param.dat
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/default.xml
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/egm84.grd
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/egm96.grd
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/ellips.dat
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY /usr/local/lib/geotrans3.5/data/wwgrid.bin
     DESTINATION ${CMAKE_BINARY_DIR}/bin)

set_target_properties(gps_sim PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "")
target_link_libraries(gps_sim ${gps_sim_libs})
install(TARGETS gps_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_executable(nos3-gps-simulator src/gps_simulator.cpp)
set_target_properties(nos3-gps-simulator PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "")
target_link_libraries(nos3-gps-simulator ${gps_sim_libs})
install(TARGETS nos3-gps-simulator RUNTIME DESTINATION bin)
