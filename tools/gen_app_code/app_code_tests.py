#========================================================================================
# File:    app_code_tests.py
# Author:  Tam Ngo/JSC
# Date:    2012-01-25
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_Date  = "na"
g_Owner = "na"

g_OutDir = "."

#========================================================================================

def inits():
    global g_OutDir, g_Date, g_Owner

    g_Date = app_code_configs.get_date()
    g_OutDir = app_code_configs.get_outdir()
    g_Owner = app_code_configs.get_owner()

#========================================================================================

def construct_makefile_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    content = """\
#######################################################################################
#
# File:    Linux Makefile for Unit Testing of %s Application
# Author:  %s
# Date:    %s
#
#######################################################################################

#
# Compiler/linker definitions
#

#
# Search path definitions to modify
#
export CFS_MISSION=$(YOUR_PROJ_HOME)
CFS_APP_SRC=$(CFS_MISSION)/apps/
APPTARGET=ut_%s

# Set build type to CFE_APP. This allows us to 
# define different compiler flags for the cFE Core and Apps.
# 
BUILD_TYPE = CFE_APP

# 
# Include all necessary cFE make rules
# Any of these can be copied to a local file and changed if needed.
#
#     cfe-config.mak contains PSP and OS selection
#
include $(CFS_MISSION)/build/linux/cfe/cfe-config.mak

#
# Setup the source path for this build
#
VPATH := .

#
# Setup the include path for this build
# The OS specific includes are in the build-rules.make file
#
# If this build needs include files from another app, add the path here.
#
INC_PATH := -I. \\
            -I../platform_inc \\
            -I$(CFS_MISSION)/osal/src/os/inc \\
            -I$(CFS_MISSION)/osal/build/inc  \\
            -I$(CFS_MISSION)/psp/fsw/inc \\
            -I$(CFS_MISSION)/psp/fsw/$(PSP)/inc \\
            -I$(CFS_MISSION)/cfe/fsw/cfe-core/src/inc 

#
# The default "make" target 
# 
all:: ut_%s.bin

clean::
	-rm -f *.o
	-rm -f *.bin

ut_%s.bin: ut_%s.c
	gcc $(LOCAL_COPTS) $(INC_PATH) $(COPTS) $(DEBUG_OPTS) \\
            -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -m32 $^ \\
            -o ut_%s.bin

#######################################################################################
    """ % (lcApp, g_Owner, g_Date, ucApp, lcApp, lcApp, lcApp, lcApp)

    return content
    
#========================================================================================

def construct_ut_main_content(tgtApp):
    global g_Owner, g_Date

    lcApp = tgtApp.lower()
    ucApp = tgtApp.upper()

    head = """\
/*=======================================================================================
** File Name:  ut_%s.c
**
** Title:  Main source file for ut_%s
**
** $Author:    %s
** $Revision: 1.1 $
** $Date:      %s
**
** Purpose:  This file contains all the necessary function definitions to perform unit
**           testing of %s application.
**
** Functions Contained:
**    Function name - Brief purpose of function
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute
**    3. List known limitations that apply to the funcs in this file
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   %s | %s | Build #: Code Started
**
**=====================================================================================*/
    """ % (lcApp, g_Owner, g_Date, ucApp, g_Date, g_Owner)

    body = """
int main(int argc, char* argv[])
{
    /* TODO:  Add code to unit test %s application here */

    return (0);
}
    """ % (ucApp)

    content = head + body

    return content

#========================================================================================

def generate_makefile(tgtApp):
    global g_OutDir

    # Construct file path
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "unit_test", "Makefile")
    fileExist_p = os.path.exists(filePath)
    # Open file
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_makefile_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================

def generate_ut_main(tgtApp):
    global g_OutDir

    # Open file
    name = "ut_" + tgtApp.lower() + ".c"
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "unit_test", name)
    fileExist_p = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_ut_main_content(tgtApp))

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, fileExist_p)

#========================================================================================
# End of app_code_tests.py
#========================================================================================

