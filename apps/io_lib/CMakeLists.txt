cmake_minimum_required(VERSION 2.6.4)
project(CFS_IO_LIB C)

include_directories(fsw/public_inc)

aux_source_directory(fsw/src LIB_SRC_FILES)
aux_source_directory(fsw/src/formats LIB_FORMATS_FILES)
aux_source_directory(fsw/src/services LIB_SERVICES_FILES)

#add_library(io_lib_formats ${LIB_FORMATS_FILES})
#add_library(io_lib_services ${LIB_SERVICES_FILES})

# Create the app module
add_cfe_app(io_lib ${LIB_SRC_FILES} ${LIB_SERVICES_FILES} ${LIB_FORMATS_FILES})
#target_link_libraries(io_lib io_lib_services io_lib_formats)
