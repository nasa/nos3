#!/usr/bin/env python
#========================================================================================
# File:    gen_app_code.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#
# Modification History:
#   Date | Author | Description
#   ---------------------------
#   10/7/15 | Susanne Strege | Added calls to generate perfids, message ids, and message
#                              definition header files
#   03/31/16 | Mike Rosburg  | Commented out call to generate global types header file
#========================================================================================

import os, sys, app_utils, app_code_configs, app_code_builds, app_code_incs
import app_code_src, app_code_tables, app_code_tests, app_code_readme

#========================================================================================

# Parse for the app's settings
app_code_configs.parse_configs()
app_code_configs.print_configs()

print ""
# Call the rest of the modules' inits() to grab configuration values
app_code_builds.inits()
app_code_incs.inits()
app_code_src.inits()
app_code_tables.inits()
app_code_tests.inits()
app_code_readme.inits()

g_Apps = []
g_Apps.extend(app_code_configs.get_apps())

# Generate code for a list of application names
for itm in g_Apps:
    print ""
    # Create the necessary directory and sub-directories
    app_utils.create_cfs_dirs(itm)
    print ""
    # Generate the app's build files
    app_code_builds.generate_makefile(itm)
    app_code_builds.generate_table_makefile(itm)
    print ""
    # Generate the app's mission_inc header file(s)
    app_code_incs.generate_mission_cfg(itm)
    app_code_incs.generate_ids(itm)
    app_code_incs.generate_private_types(itm)
    # app_code_incs.generate_global_types(itm)
    print ""
    # Generate the app's platform_inc header file(s)
    app_code_incs.generate_platform_cfg(itm)
    app_code_incs.generate_perfids(itm)
    app_code_incs.generate_msgids(itm)
    print ""
    # Generate the app's main source & header file(s)
    app_code_src.generate_msgheader(itm)
    app_code_src.generate_header(itm)
    app_code_src.generate_source(itm)
    # Generate ReadMe file
    app_code_readme.generate_readme(itm)
    print ""
    # Generate the app's ILoad tables' source & header files
    app_code_tables.generate_header(itm)
    app_code_tables.generate_default_iload_source(itm)
    app_code_tables.generate_iload_utils_header(itm)
    app_code_tables.generate_iload_utils_source(itm)
    app_code_tables.generate_cds_utils_header(itm)
    app_code_tables.generate_cds_utils_source(itm)
    print ""
    # Generate the app's source & header file(s) for unit testing
    app_code_tests.generate_makefile(itm)
    print ""

#========================================================================================
# End of gen_app_code.py
#========================================================================================

