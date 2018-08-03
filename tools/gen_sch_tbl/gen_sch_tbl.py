#!/usr/bin/env python

#===============================================================================
# File:    gen_sch_tbl.py
# Author:  Steve Duran
# Date:    2014-06-23
# 
# This script will convert a simple input file of the format:
# The first two lines define the number of minor frames per major frame and
# the max number of activity messages that the scheduler can send per minor
# frame time slot
# SCH_MAX_MINOR_FRAMES max_num_minor_frames
# MSGS_PER_MINOR_FRAMES messages_per_minor_frame
#
# The rest of the file is one line per app of the following form
# APP_ID PRIO   RATE_Hz    INITIAL_MF MSG_TYPE(WU_HK)
# 
# where:
#    APP_ID: app ID or prefix (i.e. SCH, DS, GNC, etc)
#    PRIO: priority in the range of 1 to 255 (high to low)
#    RATE_Hz: cycle rate for the app
#    INITIAL_MF: initial minor frame number to start in (basically
#                start offset into major frame)
#    MSG_TYPE(WU_HK): activity message type to send, currently supported are
#                     either WU for a wakeup message or HK for a housekeeping
#                     message
#
# USAGE:
#    gen_sch_tbl.py sch_input_config_file sch_def_file.c
#    ex:
#        gen_sch_tbl.py sch.in sch_def_schtbl.c
#
#===============================================================================

import os, sys

#===============================================================================

# Global Variables - only global to this file

gSchConfig = []
gSchCfgFileContent = []
gSchTbl = []

# line and within line index values for parsing 'sch.in' file
# NOTE: If the sch.in file format changes these may have to change too
SCH_MAX_MINOR_FRAMES_LINE = 0
SCH_MAX_MINOR_FRAMES_IDX = 1  
MSGS_PER_MINOR_FRAMES_LINE = 1
MSGS_PER_MINOR_FRAMES_IDX = 1 
APP_PARAM_LIST_START_LINE = 3 # app sch params list start on line 3
APP_ID_IDX = 0
APP_PRIORITY_IDX = 1
APP_RATE_Hz_IDX = 2
APP_INITIAL_MF_IDX = 3
APP_MSG_TYPE_IDX = 4

#===============================================================================

def Init():
    #print str(sys.argv)
    if(len(sys.argv) < 3):
        print "usage: gen_sch_tbl.py schCfgFile schTblFile\n"
    return

#===============================================================================
def LoadSchConfigFile(cfgFileName):

    fp = open(cfgFileName, "r")
    
    gSchCfgFileContent = list(fp)
   
    for line in gSchCfgFileContent:     
        gSchConfig.append(line.split())
   
    fp.close()
    return
    
#===============================================================================
def GenerateSchTable():

    numMinorFrames = int(gSchConfig[SCH_MAX_MINOR_FRAMES_LINE][SCH_MAX_MINOR_FRAMES_IDX], 10)
    numMsgsPerMinorFrame = int(gSchConfig[MSGS_PER_MINOR_FRAMES_LINE][MSGS_PER_MINOR_FRAMES_IDX], 10)
    
    # initialize schedule table with all empty entries
	# "{ SCH_UNUSED, 0, 0, 0, 0, SCH_GROUP_NONE },"
    for i in range(0, numMinorFrames):
        for j in range(0, numMsgsPerMinorFrame):
            gSchTbl.append("{ SCH_UNUSED, 0, 0, 0, 0, SCH_GROUP_NONE },")
    
#    print gSchTbl
    
    	# for each app entry in priority order (so that activity messages
	# are filled in priority order), scan through sch table and fill in its
	# entries based on the app rate and initial minor frame
    for appIdx in range(APP_PARAM_LIST_START_LINE, len(gSchConfig)):
        # determine which minor frames this app needs to be
		# messaged from the scheduler, starting on its initial minor frame
        mfIncr = numMinorFrames / int(gSchConfig[appIdx][APP_RATE_Hz_IDX], 10)
        
        # starting at initial minor frame from sch.in, add a msg entry
        # for the current app, every 
        mfIdx = int(gSchConfig[appIdx][APP_INITIAL_MF_IDX], 10)
        while mfIdx < numMinorFrames: 
#            print appIdx, gSchConfig[appIdx][APP_ID_IDX], mfIdx, mfIncr
            for msgIdx in range(0, numMsgsPerMinorFrame):
                idx = (mfIdx * numMsgsPerMinorFrame) + msgIdx
                # Look for SCH_UNUSED activity slots at the current
                # minor frame index for this app
                if(gSchTbl[idx].find("SCH_UNUSED") > -1):
                     # add either wakeup or HK activity message to sch table
                    if(gSchConfig[appIdx][APP_MSG_TYPE_IDX] == "WU"):
                        gSchTbl[idx] = "{ SCH_ENABLED, SCH_ACTIVITY_SEND_MSG, 1, 0, " + \
                        gSchConfig[appIdx][APP_ID_IDX] + "_WAKEUP_MIDX, SCH_GROUP_MISC },"
                    elif(gSchConfig[appIdx][APP_MSG_TYPE_IDX] == "HK"):
                        gSchTbl[idx] = "{ SCH_ENABLED, SCH_ACTIVITY_SEND_MSG, 1, 0, " + \
                        gSchConfig[appIdx][APP_ID_IDX] + "_SEND_HK_MIDX, SCH_GROUP_MISC },"
                    #else
                        # WU and HK are the only options, anything else would
                        # be a user input error, so just leave default table
                        # entry
                            
#                    print mfIdx, msgIdx, idx, gSchTbl[idx]
                    
                    # found the next available msg slot open, now exit msgs
                    # per minor frame loop
                    break;
            # increment minor frame index for next minor frame that this 
            # app should be scheduled
            mfIdx += mfIncr       
    return
    
#===============================================================================
def SaveSchTbl(schTblFileName):

    numMinorFrames = int(gSchConfig[SCH_MAX_MINOR_FRAMES_LINE][SCH_MAX_MINOR_FRAMES_IDX], 10)
    numMsgsPerMinorFrame = int(gSchConfig[MSGS_PER_MINOR_FRAMES_LINE][MSGS_PER_MINOR_FRAMES_IDX], 10)

    fp = open(schTblFileName, "w")
    
    # write sch table file header and top level constant struct defs
    fp.write("""
/*==============================================================================
** File Name:  sch_def_schtbl.c
**
** Title:  CFS scheduler table
**
** $Author:    Auto-generated by gen_sch_tbl
** $Revision: 1.1 $
** $Date: 2014/08/28 12:53:20GMT-05:00 $
**
** Purpose:  
**\n""")           
    fp.write("**  Schedule table configuration parameters:\n")
    fp.write("**    %s %s\n" %(gSchConfig[SCH_MAX_MINOR_FRAMES_LINE][0], \
             gSchConfig[SCH_MAX_MINOR_FRAMES_LINE][SCH_MAX_MINOR_FRAMES_IDX]))
    fp.write("**    %s %s\n" %(gSchConfig[MSGS_PER_MINOR_FRAMES_LINE][0], \
             gSchConfig[MSGS_PER_MINOR_FRAMES_LINE][MSGS_PER_MINOR_FRAMES_LINE]))
    for i in range(2, len(gSchConfig)):
        fp.write("**    %s    %s    %s    %s    %s\n" %(gSchConfig[i][0],\
                gSchConfig[i][1],gSchConfig[i][2],gSchConfig[i][3],gSchConfig[i][4]))
        
    fp.write("""
**============================================================================*/
/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "sch_platform_cfg.h"
#include "sch_msgdefs.h"
#include "sch_tbldefs.h"
#include "sch_grpids.h"
#include "sch_tblidx.h"
/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/* Table file header */
static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "SCH_DefaultScheduleTable", "SCH.SCHED_DEF", "SCH schedule table",
    "sch_def_schtbl.tbl", (sizeof(SCH_ScheduleEntry_t) * SCH_TABLE_ENTRIES)
};

/* Default schedule table data */
SCH_ScheduleEntry_t SCH_DefaultScheduleTable[SCH_TABLE_ENTRIES] =
{
    /*
    ** Structure definition...
    **
    **    uint8    EnableState  -- SCH_UNUSED, SCH_ENABLED, SCH_DISABLED
    **    uint8    Type         -- 0 or SCH_ACTIVITY_SEND_MSG
    **    uint16   Frequency    -- how many seconds between Activity execution
    **    uint16   Remainder    -- seconds offset to perform Activity
    **    uint16   MessageIndex -- Message Index into Message Definition table
    **    uint32   GroupData    -- Group and Multi-Group membership definitions
    */
    """)
    
    # write table content
    for i in range(0, numMinorFrames):
        fp.write("\n    /* Slot #%d */\n" %(i))
        for j in range(0, numMsgsPerMinorFrame):
            fp.write("    %s\n" %(gSchTbl[(i * numMsgsPerMinorFrame) + j]))


    #  write sch table struct closing bracket
    fp.write("};\n")
    
    fp.write(
    """
/*==============================================================================
** End of file %s
**============================================================================*/
    """ %(schTblFileName)
    )
    
    fp.close()
    return
    

#===============================================================================
# "main" - init, load sch table input file, generate sch table .c file,
# save sch table file to sch tbl.c
Init()

LoadSchConfigFile(sys.argv[1])

GenerateSchTable()

SaveSchTbl(sys.argv[2])

#===============================================================================
# End of gen_sch_tbl.py
#===============================================================================
