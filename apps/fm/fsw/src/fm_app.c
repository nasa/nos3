/*
** $Id: fm_app.c 1.33 2015/02/28 17:50:50EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: Core Flight System (CFS) File Manager (FM) Application
**
** Purpose: The File Manager (FM) Application provides onboard file system
**          management services by processing commands for copying and moving
**          files, decompressing files, concatenating files, creating directories,
**          deleting files and directories, and providing file and directory status.
**          When the File Manager application receives a housekeeping request
**          (scheduled within the scheduler application), FM  reports it's housekeeping
**          status values via telemetry messaging.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** $Log: fm_app.c  $
** Revision 1.33 2015/02/28 17:50:50EST sstrege 
** Added copyright information
** Revision 1.32 2014/12/18 14:32:54EST lwalling 
** Added mutex semaphore protection when accessing FM_GlobalData.ChildQueueCount
** Revision 1.31 2014/12/04 17:52:09EST lwalling 
** Removed unused CommandWarnCounter
** Revision 1.30 2014/10/22 17:50:57EDT lwalling 
** Allow zero as a valid semaphore ID, use FM_CHILD_SEM_INVALID instead
** Revision 1.29 2011/08/29 15:11:58EDT lwalling 
** Remove unused argument to function FM_DeleteFileCmd()
** Revision 1.28 2011/06/09 15:48:24EDT lwalling 
** Fixed typo that used message ID instead of command code
** Revision 1.27 2011/05/31 17:14:04EDT lwalling 
** Added entry for delete file internal, optimized call to get messageID
** Revision 1.26 2011/04/15 15:18:10EDT lwalling 
** Added current and previous child task command code to global data and housekeeping packet
** Revision 1.25 2011/01/12 14:39:44EST lwalling 
** Move mission revision number to platform config header file
** Revision 1.24 2010/01/12 15:07:12EST lwalling 
** Remove references to fm_mission_cfg.h
** Revision 1.23 2009/11/17 13:40:48EST lwalling 
** Remove global open files list data structure
** Revision 1.22 2009/11/13 16:26:34EST lwalling 
** Modify macro names, add SetTableEntryState cmd
** Revision 1.21 2009/11/09 16:57:53EST lwalling 
** Move value definitions to fm_defs.h, move prototypes to fm_app.h, cleanup source indents
** Revision 1.20 2009/10/30 16:01:09EDT lwalling 
** Include fm_msgdefs.h, add HK request command packet definition
** Revision 1.19 2009/10/30 14:02:31EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.18 2009/10/30 10:46:46EDT lwalling
** Remove detail from function prologs, leave detail in function prototypes
** Revision 1.17 2009/10/29 11:42:23EDT lwalling
** Make common structure for open files list and open file telemetry packet, change open file to open files
** Revision 1.16 2009/10/27 17:38:35EDT lwalling
** Fix typo during creation of new child task warning counter
** Revision 1.15 2009/10/27 17:26:42EDT lwalling
** Add child task warning counter to housekeeping telemetry structure
** Revision 1.14 2009/10/26 16:40:41EDT lwalling
** Rename funcs, make global data local, change struct/var names, add child vars to HK
** Revision 1.13 2009/10/26 11:30:58EDT lwalling
** Remove Close File command from FM application
** Revision 1.12 2009/10/23 14:49:06EDT lwalling
** Update event text and descriptions of event text
** Revision 1.11 2009/10/16 15:47:59EDT lwalling
** Update event text, command code names, function names, add warning counter
** Revision 1.10 2009/10/09 17:23:51EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.9 2009/10/08 16:20:22EDT lwalling
** Remove disk free space from HK telemetry
** Revision 1.8 2009/10/07 15:54:16EDT lwalling
** Fix startup w/o table load, change some data types from int8 to int32
** Revision 1.7 2009/09/28 15:29:54EDT lwalling
** Review and modify event text
** Revision 1.6 2009/09/28 14:15:27EDT lwalling
** Create common filename verification functions
** Revision 1.5 2009/06/12 14:16:27EDT rmcgraw
** DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
** Revision 1.4 2008/10/03 15:53:22EDT sstrege
** Added include to new fm_version.h header files
** Added version information to application initialization event message
** Revision 1.3 2008/09/30 16:38:01EDT sstrege
** Removed platform_cfg.h include and replaced with include to perfids.h
** Revision 1.2 2008/06/20 16:21:20EDT slstrege
** Member moved from fsw/src/fm_app.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_app.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:20ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#include "cfe.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_app.h"
#include "fm_tbl.h"
#include "fm_child.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_events.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application global data                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

FM_GlobalData_t  FM_GlobalData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- entry point and main loop processor           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_AppMain(void)
{
    uint32 RunStatus = CFE_ES_APP_RUN;
    CFE_SB_MsgPtr_t MsgPtr;
    int32  Result;

    /* Register application */
    Result = CFE_ES_RegisterApp();

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(FM_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization...
    */
    if (Result == CFE_SUCCESS)
    {
        Result = FM_AppInit();
    }

    /*
    ** Check for start-up error...
    */
    if (Result != CFE_SUCCESS)
    {
        /*
        ** Set request to terminate main loop...
        */
        RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Main process loop...
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(FM_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_RcvMsg(&MsgPtr, FM_GlobalData.CmdPipe, CFE_SB_PEND_FOREVER);

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(FM_APPMAIN_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /* Process Software Bus message */
            FM_ProcessPkt(MsgPtr);
        }
        else
        {
            /* Process Software Bus error */
            CFE_EVS_SendEvent(FM_SB_RECEIVE_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: SB receive: result = 0x%08X", Result);

            /* Set request to terminate main loop */
            RunStatus = CFE_ES_APP_ERROR;
        }
    }

    /*
    ** Send an event describing the reason for the termination...
    */
    CFE_EVS_SendEvent(FM_EXIT_ERR_EID, CFE_EVS_ERROR,
       "Application terminating: result = 0x%08X", Result);

    /*
    ** In case cFE Event Services is not working...
    */
    CFE_ES_WriteToSysLog("FM application terminating: result = 0x%08X\n", Result);

    /*
    ** Performance Log (stop time counter)...
    */
    CFE_ES_PerfLogExit(FM_APPMAIN_PERF_ID);

    /*
    ** Let cFE kill the task (and any child tasks)...
    */
    CFE_ES_ExitApp(RunStatus);

} /* End FM_AppMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- startup initialization processor              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_AppInit(void)
{
    char *ErrText = "Initialization error:";
    int32 Result;

    /* Initialize global data  */
    CFE_PSP_MemSet(&FM_GlobalData, 0, sizeof(FM_GlobalData_t));

    /* Initialize child task semaphores */
    FM_GlobalData.ChildSemaphore = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCountSem = FM_CHILD_SEM_INVALID;

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(FM_STARTUP_EVENTS_ERR_EID, CFE_EVS_ERROR,
           "%s register for event services: result = 0x%08X", ErrText, Result);
    }
    else
    {
        /* Create Software Bus message pipe */
        Result = CFE_SB_CreatePipe(&FM_GlobalData.CmdPipe,
                                    FM_APP_PIPE_DEPTH, FM_APP_PIPE_NAME);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(FM_STARTUP_CREAT_PIPE_ERR_EID, CFE_EVS_ERROR,
               "%s create SB input pipe: result = 0x%08X", ErrText, Result);
        }
        else
        {
            /* Subscribe to Housekeeping request commands */
            Result = CFE_SB_Subscribe(FM_SEND_HK_MID, FM_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(FM_STARTUP_SUBSCRIB_HK_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to HK request: result = 0x%08X", ErrText, Result);
            }
        }
    }

    /* Keep indentation from getting too deep */
    if (Result == CFE_SUCCESS)
    {
        /* Subscribe to FM ground command packets */
        Result = CFE_SB_Subscribe(FM_CMD_MID, FM_GlobalData.CmdPipe);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(FM_STARTUP_SUBSCRIB_GCMD_ERR_EID, CFE_EVS_ERROR,
               "%s subscribe to FM commands: result = 0x%08X", ErrText, Result);
        }
        else
        {
            /* Initialize FM tables */
            Result = FM_TableInit();

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(FM_STARTUP_TABLE_INIT_ERR_EID, CFE_EVS_ERROR,
                   "%s register free space table: result = 0x%08X", ErrText, Result);
            }
            else
            {
                /* Create low priority child task */
                FM_ChildInit();

                /* Application startup event message */
                CFE_EVS_SendEvent(FM_STARTUP_EID, CFE_EVS_INFORMATION,
                   "Initialization complete: version %d.%d.%d.%d",
                    FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
            }
        }
    }

    return(Result);

} /* End of FM_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- input packet processor                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t MessageID;

    MessageID = CFE_SB_GetMsgId(MessagePtr);

    switch(MessageID)
    {
        /* Housekeeping request */
        case FM_SEND_HK_MID:
            FM_ReportHK(MessagePtr);
            break;

        /* FM ground commands */
        case FM_CMD_MID:
            FM_ProcessCmd(MessagePtr);
            break;

        default:
            CFE_EVS_SendEvent(FM_MID_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid message ID: mid = 0x%04X", MessageID);
            break;

    }

    return;

} /* End of FM_ProcessPkt */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- command packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    boolean Result = TRUE;
    uint16 CommandCode = CFE_SB_GetCmdCode(MessagePtr);

    /* Invoke specific command handler */
    switch (CommandCode)
    {
        case FM_NOOP_CC:
            Result = FM_NoopCmd(MessagePtr);
            break;

        case FM_RESET_CC:
            Result = FM_ResetCountersCmd(MessagePtr);
            break;

        case FM_COPY_CC:
            Result = FM_CopyFileCmd(MessagePtr);
            break;

        case FM_MOVE_CC:
            Result = FM_MoveFileCmd(MessagePtr);
            break;

        case FM_RENAME_CC:
            Result = FM_RenameFileCmd(MessagePtr);
            break;

        case FM_DELETE_CC:
            Result = FM_DeleteFileCmd(MessagePtr);
            break;

        case FM_DELETE_ALL_CC:
            Result = FM_DeleteAllFilesCmd(MessagePtr);
            break;

        case FM_DECOMPRESS_CC:
            Result = FM_DecompressFileCmd(MessagePtr);
            break;

        case FM_CONCAT_CC:
            Result = FM_ConcatFilesCmd(MessagePtr);
            break;

        case FM_GET_FILE_INFO_CC:
            Result = FM_GetFileInfoCmd(MessagePtr);
            break;

        case FM_GET_OPEN_FILES_CC:
            Result = FM_GetOpenFilesCmd(MessagePtr);
            break;

        case FM_CREATE_DIR_CC:
            Result = FM_CreateDirectoryCmd(MessagePtr);
            break;

        case FM_DELETE_DIR_CC:
            Result = FM_DeleteDirectoryCmd(MessagePtr);
            break;

        case FM_GET_DIR_FILE_CC:
            Result = FM_GetDirListFileCmd(MessagePtr);
            break;

        case FM_GET_DIR_PKT_CC:
            Result = FM_GetDirListPktCmd(MessagePtr);
            break;

        case FM_GET_FREE_SPACE_CC:
            Result = FM_GetFreeSpaceCmd(MessagePtr);
            break;

        case FM_SET_TABLE_STATE_CC:
            Result = FM_SetTableStateCmd(MessagePtr);
            break;

        case FM_DELETE_INT_CC:
            Result = FM_DeleteFileCmd(MessagePtr);
            break;

        default:
            Result = FALSE;
            CFE_EVS_SendEvent(FM_CC_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid command code: cc = %d", CommandCode);
            break;
    }

    if (Result == TRUE)
    {
        /* Increment command success counter */
        if ((CommandCode != FM_RESET_CC) && (CommandCode != FM_DELETE_INT_CC))
        {
            FM_GlobalData.CommandCounter++;
        }
    }
    else
    {
        /* Increment command error counter */
        FM_GlobalData.CommandErrCounter++;
    }

    return;

} /* End of FM_ProcessCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- housekeeping request packet processor         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ReportHK(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "HK Request";
    boolean Result;

    /* Verify command packet length */
    Result = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_HousekeepingCmd_t),
                                    FM_HK_REQ_ERR_EID, CmdText);

    /* Report FM housekeeping telemetry data */
    if (Result == TRUE)
    {
        /* Release table pointers */
        FM_ReleaseTablePointers();

        /* Allow cFE chance to dump, update, etc. */
        FM_AcquireTablePointers();

        /* Initialize housekeeping telemetry message */
        CFE_SB_InitMsg(&FM_GlobalData.HousekeepingPkt, FM_HK_TLM_MID,
                       sizeof(FM_HousekeepingPkt_t), TRUE);

        /* Report application command counters */
        FM_GlobalData.HousekeepingPkt.CommandCounter = FM_GlobalData.CommandCounter;
        FM_GlobalData.HousekeepingPkt.CommandErrCounter = FM_GlobalData.CommandErrCounter;

        /* Report current number of open files */
        FM_GlobalData.HousekeepingPkt.NumOpenFiles = FM_GetOpenFilesData(NULL);

        /* Report child task command counters */
        FM_GlobalData.HousekeepingPkt.ChildCmdCounter = FM_GlobalData.ChildCmdCounter;
        FM_GlobalData.HousekeepingPkt.ChildCmdErrCounter = FM_GlobalData.ChildCmdErrCounter;
        FM_GlobalData.HousekeepingPkt.ChildCmdWarnCounter = FM_GlobalData.ChildCmdWarnCounter;

        /* Report number of commands in child task queue */
        FM_GlobalData.HousekeepingPkt.ChildQueueCount = FM_GlobalData.ChildQueueCount;

        /* Report current and previous commands executed by the child task */
        FM_GlobalData.HousekeepingPkt.ChildCurrentCC = FM_GlobalData.ChildCurrentCC;
        FM_GlobalData.HousekeepingPkt.ChildPreviousCC = FM_GlobalData.ChildPreviousCC;

        /* Timestamp and send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.HousekeepingPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.HousekeepingPkt);
    }

    return;

} /* End of FM_ReportHK */


/************************/
/*  End of File Comment */
/************************/

