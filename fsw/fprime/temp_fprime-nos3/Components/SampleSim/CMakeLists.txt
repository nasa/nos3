####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
include_directories("../../../../../components/sample/fsw/src")
include_directories("../../.././") #device_cfg.h
include_directories("../../../../../fsw/apps/hwlib/fsw/public_inc")
include_directories("../../../../../components/sample/fsw/platform_inc")


set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SampleSim.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SampleSim.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../../components/sample/fsw/src/sample_device.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../../device_cfg.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../../fsw/apps/hwlib/fsw/public_inc/hwlib.h"
  "${CMAKE_CURRENT_LIST_DIR}/../../../../../components/sample/fsw/platform_inc/sample_platform_cfg.h"
)



# include_directories("/home/jstar/dev/nos3/components/sample/fsw/src")



# Uncomment and add any modules that this component depends on, else
# they might not be available when cmake tries to build this component.

# set(MOD_DEPS
#     Add your dependencies here
# )

set(MOD_DEPS
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)


register_fprime_module()
