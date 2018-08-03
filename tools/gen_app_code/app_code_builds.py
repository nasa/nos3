#========================================================================================
# File:    app_code_builds.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_Date   = "na"
g_Owner = "na"

g_OutDir = "."

g_Tbls = []

#========================================================================================

def inits():
    global g_OutDir, g_Owner, g_Date, g_Tbls

    g_OutDir = app_code_configs.get_outdir()
    g_Owner = app_code_configs.get_owner()
    g_Date = app_code_configs.get_date()

    g_Tbls.extend(app_code_configs.get_tables())

#========================================================================================

def construct_makefile_content(tgtApp):
    global g_Owner, g_Date, g_Tbls

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    part1 = """\
#######################################################################################
#
# File:    CFS Application Makefile 
# Author:  GSFC/Flight Software Branch/Code 582
# Date:    2008-2010
#
#######################################################################################

#
# Subsystem produced by this makefile
#
APPTARGET = %s

# 
# Entry Point for task
# 
ENTRY_PT = %s_AppMain

#
# Object files required to build subsystem.
#
OBJS = %s_app.o """ % (lcApp, ucApp, lcApp)

    part2 = ""
    if "iload" in g_Tbls:
        part2 = """\
%s_iload_utils.o """ % (lcApp)

    part3 = ""
    if "cds" in g_Tbls:
        part3 = """\
%s_cds_utils.o """ % (lcApp)

    part4 = """

#
# Source files required to build subsystem; used to generate dependencies.
# As long as there are no assembly files this can be automated.
#
SOURCES = $(OBJS:.o=.c)

#
# Specify extra C Flags needed to build this subsystem
#
LOCAL_COPTS = 

#
# EXEDIR is defined here, just in case it needs to be different for a custom build
#
EXEDIR=../exe

#
# Certain OSs and Application Loaders require the following option for shared libraries. 
# Currently only needed for vxWorks 5.5 and RTEMS.
# For each shared library that this app depends on, you need to have an entry like the
# following:
#    -R../tst_lib/tst_lib.elf
#
SHARED_LIB_LINK = 

#======================================================================================
# Should not have to change below this line, except for customized mission and cFE
# directory structures
#======================================================================================
#
# Set build type to CFE_APP. This allows us to 
# define different compiler flags for the cFE Core and Apps.
# 
BUILD_TYPE = CFE_APP

# 
# Include all necessary cFE make rules
# Any of these can be copied to a local file and changed if needed.
#
#       cfe-config.mak contains PSP and OS selection
#
include ../cfe/cfe-config.mak
#
#       debug-opts.mak contains debug switches
#
include ../cfe/debug-opts.mak
#
#       compiler-opts.mak contains compiler definitions and switches/defines
#
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

#
# Setup the include path for this subsystem
# The OS specific includes are in the build-rules.make file
#
# If this subsystem needs include files from another app, add the path here.
#
INCLUDE_PATH = -I$(OSAL_SRC)/inc \\
               -I$(CFE_CORE_SRC)/inc \\
               -I$(CFE_PSP_SRC)/inc \\
               -I$(CFE_PSP_SRC)/$(PSP)/inc \\
               -I$(CFS_APP_SRC)/inc \\
               -I$(CFS_APP_SRC)/$(APPTARGET)/fsw/src \\
               -I$(CFS_APP_SRC)/$(APPTARGET)/fsw/tables \\
               -I$(CFS_MISSION_INC) \\
               -I../cfe/inc \\
               -I../inc 

#
# Define the VPATH make variable. 
# This can be modified to include source from another directory.
# If there is no corresponding app in the cfs-apps directory, then this can be discarded,
# or if the mission chooses to put the src in another directory such as "src", then that
# can be added here as well.
#
VPATH = $(CFS_APP_SRC)/$(APPTARGET)/fsw/src \\
        $(CFS_APP_SRC)/$(APPTARGET)/fsw/tables 

#
# Include the common make rules for building a cFE Application
#
include $(CFE_CORE_SRC)/make/app-rules.mak

#######################################################################################
    """
    
    return part1 + part2 + part3 + part4

#========================================================================================

def construct_table_makefile_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
#######################################################################################
#
# File:    CFS Application Table Makefile 
# Author:  GSFC/Flight Software Branch/Code 582
# Date:    2008-2010
#
#######################################################################################

#
# The Application needs to be specified here
#
PARENTAPP = %s

#
# List the tables that are generated here.
# Restrictions:
# 1. The table file name must be the same as the C source file name.
# 2. There must be a single C source file for each table.
#
TABLES = %s_iloads.tbl

#======================================================================================
# Normally, nothing has to be changed below this line.
# The following are changes that may have to be made for a custom app environment:
# 1. INCLUDE_PATH - This may be customized to tailor the include path for an app.
# 2. VPATH - This may be customized to tailor the location of the table sources.
#            For example: if the tables were stored in a "tables" subdirectory
#                         (build/cpu1/sch/tables)
#======================================================================================

#
# Object files required for tables
#
OBJS = $(TABLES:.tbl=.o)

#
# Source files required to build tables.
#
SOURCES = $(OBJS:.o=.c)

#
# Specify extra C Flags needed to build this subsystem
#
LOCAL_COPTS =

#
# EXEDIR is defined here, just in case it needs to be different for a custom build.
#
EXEDIR = ../exe

#======================================================================================
# Should not have to change below this line, except for customized mission and cFE
# directory structures.
#======================================================================================

#
# Set build type to CFE_APP. This allows us to define different compiler flags for the
# cFE Core and Apps.
# 
BUILD_TYPE = CFE_TABLE

# 
# Include all necessary cFE make rules
# Any of these can be copied to a local file and changed if needed.
#
#
#       cfe-config.mak contains arch, BSP, and OS selection.
#
include ../cfe/cfe-config.mak

#
#       debug-opts.mak contains debug switches.
#       -- Note that the table must be built with -g for the elf2tbl utility to work.
#
include ../cfe/debug-opts.mak

##
##       compiler-opts.mak contains compiler definitions and switches/defines.
##
include $(CFE_PSP_SRC)/$(PSP)/make/compiler-opts.mak

#
# Setup the include path for this subsystem.
# The OS specific includes are in the build-rules.make file.
#
# If this subsystem needs include files from another app, add the path here.
#
INCLUDE_PATH = -I$(OSAL_SRC)/inc \\
               -I$(CFE_CORE_SRC)/inc \\
               -I$(CFE_PSP_SRC)/$(PSP)/inc \\
               -I$(CFE_PSP_SRC)/inc \\
               -I$(CFS_APP_SRC)/inc \\
               -I$(CFS_APP_SRC)/$(PARENTAPP)/fsw/src \\
               -I$(CFS_APP_SRC)/$(PARENTAPP)/fsw/tables \\
               -I$(CFS_MISSION_INC) \\
               -I../cfe/inc \\
               -I../inc 

#
# Define the VPATH make variable. 
# This can be modified to include source from another directory.
# If there is no corresponding app in the cfe-apps directory, then this can be discarded,
# or if the mission chooses to put the src in another directory such as "src", then that
# can be added here as well.
#
VPATH = $(CFS_APP_SRC)/$(PARENTAPP)/fsw/tables

#
# Include the common make rules for building a cFE Application
#
include $(CFE_CORE_SRC)/make/table-rules.mak

#######################################################################################
    """ % (lcApp, lcApp)

    return content

#========================================================================================

def generate_makefile(tgtApp):
    global g_OutDir

    # Construct file path
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "for_build", "Makefile")
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_makefile_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_table_makefile(tgtApp):
    global g_OutDir, g_Tbls

    # Construct file path
    name = tgtApp.lower() + "_tables.mak"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "for_build", name)

    if not g_Tbls:
        # Remove existing file, if exists
        app_utils.remove_file(filePath)
    elif "iload" in g_Tbls or "cds" in g_Tbls:
        fileExist_p = os.path.exists(filePath)
        # Open file
        fileHdl = app_utils.open_file_for_writing(filePath, False)

        # Write to file
        fileHdl.write(construct_table_makefile_content(tgtApp))

        # Close file
        app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)
    else:
        pass

#========================================================================================
# End of app_code_builds.py
#========================================================================================

