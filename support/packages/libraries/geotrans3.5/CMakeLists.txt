cmake_minimum_required(VERSION 2.8)

project(geotrans3.5)

# FLAGS?  Try without:  posic = -shared -fPIC -m32 -march=pentium4  -pthread -std=gnu++98 -Wno-deprecated  -D_GNU_SOURCE -DREDHAT
#set(CMAKE_CXX_FLAGS "-shared -fPIC -m32 -march=pentium4  -pthread -std=gnu++98 -Wno-deprecated  -D_GNU_SOURCE -DREDHAT ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")

include_directories(inc/dtcc inc/ccservice inc/ccs)

file(GLOB dtcc_src src/dtcc/*.cpp)
file(GLOB ccservice_src src/ccservice/*.cpp)
file(GLOB ccs_src src/ccs/*.cpp)

# For Code::Blocks and other IDEs
file(GLOB dtcc_inc inc/dtcc/*.h)
file(GLOB ccservice_inc inc/ccservice/*.h)
file(GLOB ccs_inc inc/ccs/*.h)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(MSPdtcc SHARED ${dtcc_src} ${dtcc_inc} ${ccservice_src} ${ccservice_inc})
target_link_libraries(MSPdtcc)
install(TARGETS MSPdtcc LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(MSPCoordinateConversionService SHARED ${ccs_src} ${ccs_inc})
target_link_libraries(MSPCoordinateConversionService)
install(TARGETS MSPCoordinateConversionService LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_executable(geotrans-test-coordinate-conversion sampleCode/testCoordinateConversionSample.cpp)
target_link_libraries(geotrans-test-coordinate-conversion MSPCoordinateConversionService MSPdtcc)
install(TARGETS geotrans-test-coordinate-conversion RUNTIME DESTINATION bin)

set(geotrans_data
    data/3_param.dat
    data/7_param.dat
    data/default.xml
    data/egm84.grd
    data/egm96.grd
    data/ellips.dat
    data/wwgrid.bin
)    
install(FILES ${geotrans_data} DESTINATION bin COMPONENT config)
