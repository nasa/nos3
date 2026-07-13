project(CFS_CFDP_APP C)

set(APP_SRC_FILES
  fsw/src/cfdp_app.c
  fsw/src/cfdp_cfdp.c
  fsw/src/cfdp_cmd.c
)

# Create the app module
add_cfe_app(cfdp ${APP_SRC_FILES})

target_include_directories(cfdp PUBLIC fsw/inc)
