/*
**  File:
**  cfe_es_task.c
**  $Id: cfe_es_task.c 1.16 2014/07/07 10:56:17GMT-05:00 acudmore Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:
**  cFE Executive Services (ES) task
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
** $Log: cfe_es_task.c  $
** Revision 1.16 2014/07/07 10:56:17GMT-05:00 acudmore 
** Added comment to note that Mempool info has an unused parameter
** Revision 1.15 2012/01/18 16:27:14GMT-05:00 jmdagost 
** Modified init and no-op event messages to include OSAL revision and mission revision numbers, also added
** those numbers to the ES HK tlm pkt.
** Revision 1.14 2012/01/13 11:50:05EST acudmore 
** Changed license text to reflect open source
** Revision 1.13 2012/01/10 13:35:47EST lwalling 
** Add output filename to shell command packet structure
** Revision 1.12 2011/11/30 14:11:19EST jmdagost 
** Corrected CFE_MISSION_REV to CFE_PSP_MISSION_REV, as it should be.
** Revision 1.11 2010/11/23 15:30:54EST jmdagost 
** Added cFE Mission Rev to init message and telemetry, added PSP Mission Rev to init message.
** Revision 1.10 2009/08/04 13:34:05EDT aschoeni 
** Added heap information to ES telemetry
** Revision 1.9 2009/07/28 16:40:19EDT jmdagost 
** Replaced reference to ComputeChecksum with CalculateCRC.
** Revision 1.8 2009/06/10 09:09:02EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.7 2009/06/04 16:45:36EDT rmcgraw 
** Fixed compiler error in version event
** Revision 1.6 2009/06/04 11:00:29EDT rmcgraw 
** DCR8290:1 Added PSP Version to event
** Revision 1.5 2008/07/31 15:41:36EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.4 2008/07/28 14:11:29EDT apcudmore 
** Changed cFE Kernel Text segment checksum to cFE Core checksum in housekeeping.
** Revision 1.3 2008/06/26 15:41:52EDT apcudmore 
** Added public CFE_ES_GetAppInfo API.
** Moved the old API to CFE_ES_GetAppInfoInternal
** -- May merge these two in the near future.
** Revision 1.2 2008/06/20 15:40:29EDT apcudmore 
** Added support for OSAL based module loader
**  - Removed calls and references to the BSP based module loader
** Revision 1.1 2008/04/17 08:05:09EDT ruperera 
** Member added to tlserver3 project /MKS-CFE-PROJECT/fsw/cfe-core/src/es
** Revision 1.107 2008/01/24 09:06:21EST njyanchik 
** Removed comments
** Revision 1.106 2008/01/24 08:54:07EST njyanchik 
** I added this:
** 
**         CFE_ES_ResetDataPtr->ERLogEntries = 0;
** 
** to CFE_ES_ClearERlogCmd so that the number of entries are cleared.
** Revision 1.105 2007/09/25 10:04:31EDT apcudmore 
** Created new config parameter for default filename for Query All Tasks cmd.
** Revision 1.104 2007/09/20 10:52:58EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.103 2007/09/18 16:39:10EDT apcudmore 
** Updated ES invalid command length event message to make it clear.
** Revision 1.102 2007/08/31 10:57:10EDT rjmcgraw 
** Changes to eliminate perl script
** Revision 1.101 2007/06/01 11:01:12EDT dlkobe 
** Corrected unassigned Pool Handle in Mem Pool Stats Telemetry Packet
** Revision 1.100 2007/05/25 11:55:42EDT apcudmore 
** Error Handling updates
**    - CFE_ES_GetAppInfo returns void
**    - added app ID range check in RestartApp
**  
** Revision 1.99 2007/05/15 11:11:12EDT rjmcgraw 
** Changed syslog msgs to be consistent with other tasks
** Revision 1.98 2007/05/14 15:58:45EDT apcudmore 
** Added runtime error detection back in to the ES main task.
** Revision 1.97 2007/05/14 15:43:46EDT apcudmore 
** Cleaned up events and disabled debug code for application restart.
** Revision 1.95 2007/05/11 10:30:13EDT rjmcgraw 
** DCR2987:4 Added return checking on init event and version event
** Revision 1.94 2007/05/09 12:41:02EDT dlkobe 
** Added processing for Dump CDS Registry Command
** Revision 1.92 2007/05/07 11:59:37EDT apcudmore 
** Removed SB Pipe error telemetry point
** Handle SB error with sys log message
** Revision 1.91 2007/05/04 20:22:23EDT dlkobe 
** Corrected prototype errors and added missing event message IDs
** Revision 1.90 2007/05/04 20:07:52EDT dlkobe 
** Added command to telemeter memory pool statistics
** Revision 1.89 2007/05/04 15:52:40EDT dlkobe 
** Modified CFE_ES_DeleteCDSCmd to make use of new CFE_ES_DeleteCDS function
** Revision 1.88 2007/05/03 14:34:59EDT dlkobe 
** Added test to prevent Delete CDS command from deleting a Critical Table Image from CDS
** Revision 1.87 2007/04/28 15:57:00EDT dlkobe 
** Added "cfe_es_cds.h" includes to resolve sudden errors.
** Revision 1.86 2007/04/27 15:55:15EDT rjmcgraw 
** DCR2987 Moved call to CFE_ES_RegisterApp into TaskInit and checked return for err
** Revision 1.85 2007/04/27 15:41:13EDT rjmcgraw 
** DCR2987:4 Standardize task init error code
** Revision 1.84 2007/04/25 11:57:05EDT apcudmore 
** Implemented "Back End" of Application Restart and Delete.
** Changes include all ES Cleanup code to delete App and Task Resources.
** Revision 1.83 2007/04/17 11:00:27EDT rjmcgraw 
** Copy DataToWrite into ES HK Pkt
** Revision 1.82 2007/04/13 16:26:00EDT apcudmore 
** Updated how ES scans its App table. It will now scan after processing a 
** ground/external command.
** Revision 1.81 2007/04/12 13:40:58EDT apcudmore 
** Added system log messages to ES init failures.
** Revision 1.80 2007/04/12 10:37:18EDT apcudmore 
** Added support for ES task to call CFE_ES_ExitApp on initialization and 
** runtime failure.
**
*/


/*
** Includes
*/
#include "private/cfe_private.h"
#include "cfe_platform_cfg.h"
#include "cfe_version.h"
#include "cfe_es_global.h"
#include "cfe_es_apps.h"
#include "cfe_es_events.h"
#include "cfe_es_verify.h"
#include "cfe_es_task.h"
#include "cfe_es_shell.h"
#include "cfe_es_log.h"
#include "cfe_es_cds.h"
#include "cfe_fs.h"
#include "cfe_psp.h"

#include <string.h>

/*
** Defines
*/
#define CFE_ES_PERF_TRIGGERMASK_INT_SIZE    (sizeof(CFE_ES_ResetDataPtr->Perf.MetaData.TriggerMask) / sizeof(uint32))
#define CFE_ES_PERF_TRIGGERMASK_EXT_SIZE    (sizeof(CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask) / sizeof(uint32))
#define CFE_ES_PERF_FILTERMASK_INT_SIZE     (sizeof(CFE_ES_ResetDataPtr->Perf.MetaData.FilterMask) / sizeof(uint32))
#define CFE_ES_PERF_FILTERMASK_EXT_SIZE     (sizeof(CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask) / sizeof(uint32))

/* 
** This define should be put in the OS API headers -- Right now it matches what the OS API uses 
*/
#define OS_MAX_PRIORITY 255 

/*
** Executive Services (ES) task global data.
*/
CFE_ES_TaskData_t CFE_ES_TaskData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskMain() -- Task entry point and main process loop     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_TaskMain(void)
{
    int32   Status;
    uint32  TimeOut = CFE_ES_APP_SCAN_RATE;
    uint32  AppRunStatus = CFE_ES_RUNSTATUS_APP_RUN;

    
    /*
    ** Performance Time Stamp Entry
    */
    CFE_ES_PerfLogEntry(CFE_ES_MAIN_PERF_ID);

    /*
    ** Perform task specific initialization.
    */
    Status = CFE_ES_TaskInit();
    if ( Status != CFE_SUCCESS )
    {
       /*
       ** Create a syslog entry
       */
       CFE_ES_WriteToSysLog("ES:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      

       /*
       ** Allow Core App to Exit 
       */
       AppRunStatus = CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR;
       
    } /* end if */

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForStartupSync(CFE_CORE_MAX_STARTUP_MSEC);

    /*
    ** Main process loop
    */
    while (AppRunStatus == CFE_ES_RUNSTATUS_APP_RUN)
    {
        /*
        ** Increment the main task execution counter
        **  This is normally done in the CFE_ES_RunLoop call, but
        **  currently CFE Child tasks and the cFE core tasks do not
        **  use the RunLoop call.
        */
        CFE_ES_IncrementTaskCounter();
        
        /*
        ** Performance Time Stamp Exit
        */
        CFE_ES_PerfLogExit(CFE_ES_MAIN_PERF_ID);

        /*
        ** Wait for the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&CFE_ES_TaskData.MsgPtr,
                                  CFE_ES_TaskData.CmdPipe,
                                  TimeOut);

        /*
        ** Performance Time Stamp Entry
        */
        CFE_ES_PerfLogEntry(CFE_ES_MAIN_PERF_ID);

        /*
        ** Scan the App table for Application Deletion requests
        */
        if ( Status == CFE_SB_TIME_OUT )
        {
           CFE_ES_ScanAppTable();
        }
        else if (Status == CFE_SUCCESS)
        {
           /*
           ** Process Software Bus message.
           */
           CFE_ES_TaskPipe(CFE_ES_TaskData.MsgPtr);
           
           /*
           ** Scan the App Table for changes after processing a command
           */
           CFE_ES_ScanAppTable();
        }
        else
        {
            /*
            ** SB Error: Write a SysLog Message 
            */
            CFE_ES_WriteToSysLog("ES:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);

            /*
            ** Allow Core App to Exit 
            */
            AppRunStatus = CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR;

        }  /* end if */
        
    } /* end while */    
    
    /*
    ** Performance Time Stamp Exit
    */
    CFE_ES_PerfLogExit(CFE_ES_MAIN_PERF_ID);
       
    /*
    ** Exit the application, CFE_ES_ExitApp will not return.
    */
    CFE_ES_ExitApp(AppRunStatus);

} /* End of CFE_ES_TaskMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskInit() -- ES task initialization                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_TaskInit(void)
{
    int32   Status;
    uint32  SizeofCfeSegment;
    cpuaddr CfeSegmentAddr;

    /*
    ** Register the Application
    */
    Status = CFE_ES_RegisterApp();
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Call to CFE_ES_RegisterApp Failed, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }    

    /*
    ** Initialize task command execution counters
    */
    CFE_ES_TaskData.CmdCounter = 0;
    CFE_ES_TaskData.ErrCounter = 0;

    /*
    ** Initialize task configuration data
    */
    strcpy(CFE_ES_TaskData.PipeName, "ES_CMD_PIPE");
    CFE_ES_TaskData.PipeDepth = 12;

    CFE_ES_TaskData.LimitHK   = 2;
    CFE_ES_TaskData.LimitCmd  = 4;
    
    /*
    ** Initialize systemlog to default mode
    */
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_DEFAULT_SYSLOG_MODE;

    /*
    ** Register event filter table.
    */
    Status = CFE_EVS_Register(NULL,0,CFE_EVS_BINARY_FILTER);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Call to CFE_EVS_Register Failed, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area)
    */
    CFE_SB_InitMsg(&CFE_ES_TaskData.HkPacket, CFE_ES_HK_TLM_MID, sizeof(CFE_ES_HkPacket_t), TRUE);

    /*
    ** Initialize shell output packet (clear user data area)
    */
    CFE_SB_InitMsg(&CFE_ES_TaskData.ShellPacket, CFE_ES_SHELL_TLM_MID, sizeof(CFE_ES_ShellPacket_t), TRUE);

    /*
    ** Initialize single application telemetry packet
    */
    CFE_SB_InitMsg(&CFE_ES_TaskData.OneAppPacket, CFE_ES_APP_TLM_MID, sizeof(CFE_ES_OneAppTlm_t), TRUE);

    /*
    ** Initialize memory pool statistics telemetry packet
    */
    CFE_SB_InitMsg(&CFE_ES_TaskData.MemStatsPacket, CFE_ES_MEMSTATS_TLM_MID, 
                   sizeof(CFE_ES_PoolStatsTlm_t), TRUE);

    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_ES_TaskData.CmdPipe, CFE_ES_TaskData.PipeDepth, CFE_ES_TaskData.PipeName);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Create SB Pipe, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }
    
    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_SubscribeEx(CFE_ES_SEND_HK_MID, CFE_ES_TaskData.CmdPipe,
                                CFE_SB_Default_Qos, CFE_ES_TaskData.LimitHK);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Subscribe to HK packet, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Subscribe to ES task ground command packets
    */
    Status = CFE_SB_SubscribeEx(CFE_ES_CMD_MID, CFE_ES_TaskData.CmdPipe,
                       CFE_SB_Default_Qos, CFE_ES_TaskData.LimitCmd);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Cannot Subscribe to ES ground commands, RC = 0x%08X\n", (unsigned int)Status);
        return(Status);
    }

    /*
    ** Compute the CRC for the cfe core code segment and place
    ** in ES Housekeeping pkt.   
    */
    Status = CFE_PSP_GetCFETextSegmentInfo( &CfeSegmentAddr, &SizeofCfeSegment);
   
    if ( Status == CFE_PSP_SUCCESS )
    { 
       CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum = CFE_ES_CalculateCRC(
                                  (void *)(CfeSegmentAddr), SizeofCfeSegment, 0, CFE_ES_DEFAULT_CRC);
    }
    else
    {
       CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum = 0xFFFF;
    }

    /*
    ** Initialize the version numbers in the ES Housekeeping pkt
    */
    CFE_ES_TaskData.HkPacket.Payload.CFEMajorVersion     = CFE_MAJOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.CFEMinorVersion     = CFE_MINOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.CFERevision         = CFE_REVISION;
    CFE_ES_TaskData.HkPacket.Payload.CFEMissionRevision  = CFE_MISSION_REV;
    CFE_ES_TaskData.HkPacket.Payload.OSALMajorVersion    = OS_MAJOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.OSALMinorVersion    = OS_MINOR_VERSION;
    CFE_ES_TaskData.HkPacket.Payload.OSALRevision        = OS_REVISION;
    CFE_ES_TaskData.HkPacket.Payload.OSALMissionRevision = OS_MISSION_REV;

    /*
    ** Task startup event message.
    */
    Status = CFE_EVS_SendEvent(CFE_ES_INIT_INF_EID,
                      CFE_EVS_INFORMATION,
                      "cFE ES Initialized");
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Error sending init event:RC=0x%08X\n", (unsigned int)Status);
        return(Status);
    }                      

    Status = CFE_EVS_SendEvent(CFE_ES_INITSTATS_INF_EID,
                      CFE_EVS_INFORMATION,
                      "Versions:cFE %d.%d.%d.%d, OSAL %d.%d.%d.%d, PSP %d.%d.%d.%d, chksm %d",
                      CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV,                      
                      OS_MAJOR_VERSION,OS_MINOR_VERSION,OS_REVISION,OS_MISSION_REV,
                      CFE_PSP_MAJOR_VERSION,CFE_PSP_MINOR_VERSION,CFE_PSP_REVISION,CFE_PSP_MISSION_REV,
                      (int)CFE_ES_TaskData.HkPacket.Payload.CFECoreChecksum);
    if ( Status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("ES:Error sending version event:RC=0x%08X\n", (unsigned int)Status);
        return(Status);
    }

#ifdef _ENHANCED_BUILD_
    /*
     ** Advertise the build and version information at start up
     */
    Status = CFE_EVS_SendEvent(CFE_ES_VERSION_INF_EID,
          CFE_EVS_INFORMATION,
          "Mission %s.%s, %s, %s",
          GLOBAL_CONFIGDATA.MissionVersion, GLOBAL_CONFIGDATA.Config,
          GLOBAL_CONFIGDATA.CfeVersion, GLOBAL_CONFIGDATA.OsalVersion);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("ES:Error sending version event:RC=0x%08X\n", (unsigned int)Status);
       return(Status);
    }

    Status = CFE_EVS_SendEvent(CFE_ES_BUILD_INF_EID,
          CFE_EVS_INFORMATION,
          "Build %s %s",
          GLOBAL_CONFIGDATA.Date, GLOBAL_CONFIGDATA.User);
    if ( Status != CFE_SUCCESS )
    {
       CFE_ES_WriteToSysLog("ES:Error sending build info event:RC=0x%08X\n", (unsigned int)Status);
       return(Status);
    }
#endif


   return(CFE_SUCCESS);
   
} /* End of CFE_ES_TaskInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_TaskPipe() -- Process command pipe message               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_TaskPipe(CFE_SB_MsgPtr_t Msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16         CommandCode;

    MessageID = CFE_SB_GetMsgId(Msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request
        */
        case CFE_ES_SEND_HK_MID:
            CFE_ES_HousekeepingCmd(Msg);
            break;

        /*
        ** ES task ground commands
        */
        case CFE_ES_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(Msg);
            switch (CommandCode)
            {
                case CFE_ES_NOOP_CC:
                    CFE_ES_NoopCmd(Msg);
                    break;

                case CFE_ES_RESET_CC:
                    CFE_ES_ResetCmd(Msg);
                    break;

                case CFE_ES_RESTART_CC:
                    CFE_ES_RestartCmd(Msg);
                    break;

                case CFE_ES_SHELL_CMD_CC:
                    CFE_ES_ShellCmd(Msg);
                    break;

                case CFE_ES_START_APP_CC:
                    CFE_ES_StartAppCmd(Msg);
                    break;

                case CFE_ES_STOP_APP_CC:
                    CFE_ES_StopAppCmd(Msg);
                    break;

                case CFE_ES_RESTART_APP_CC:
                    CFE_ES_RestartAppCmd(Msg);
                    break;

                case CFE_ES_RELOAD_APP_CC:
                    CFE_ES_ReloadAppCmd(Msg);
                    break;

                case CFE_ES_QUERY_ONE_CC:
                    CFE_ES_QueryOneCmd(Msg);
                    break;

                case CFE_ES_QUERY_ALL_CC:
                    CFE_ES_QueryAllCmd(Msg);
                    break;

                case CFE_ES_QUERY_ALL_TASKS_CC:
                    CFE_ES_QueryAllTasksCmd(Msg);
                    break;

                case CFE_ES_CLEAR_SYSLOG_CC:
                    CFE_ES_ClearSyslogCmd(Msg);
                    break;

                case CFE_ES_WRITE_SYSLOG_CC:
                    CFE_ES_WriteSyslogCmd(Msg);
                    break;

                case CFE_ES_OVERWRITE_SYSLOG_CC:
                    CFE_ES_OverWriteSyslogCmd(Msg);
                    break;

                case CFE_ES_CLEAR_ERLOG_CC:
                    CFE_ES_ClearERlogCmd(Msg);
                    break;

                case CFE_ES_WRITE_ERLOG_CC:
                    CFE_ES_WriteERlogCmd(Msg);
                    break;

                case CFE_ES_PERF_STARTDATA_CC:
                    CFE_ES_PerfStartDataCmd(Msg);
                    break;

                case CFE_ES_PERF_STOPDATA_CC:
                    CFE_ES_PerfStopDataCmd(Msg);
                    break;

                case CFE_ES_PERF_SETFILTERMASK_CC:
                    CFE_ES_PerfSetFilterMaskCmd(Msg);
                    break;

                case CFE_ES_PERF_SETTRIGMASK_CC:
                    CFE_ES_PerfSetTriggerMaskCmd(Msg);
                    break;

                case CFE_ES_RESET_PR_COUNT_CC:
                    CFE_ES_ResetPRCountCmd(Msg);
                    break;

                case CFE_ES_SET_MAX_PR_COUNT_CC:
                    CFE_ES_SetMaxPRCountCmd(Msg);
                    break;

                case CFE_ES_DELETE_CDS_CC:
                    CFE_ES_DeleteCDSCmd(Msg);
                    break;

                case CFE_ES_TLM_POOL_STATS_CC:
                    CFE_ES_TlmPoolStatsCmd(Msg);
                    break;

                case CFE_ES_DUMP_CDS_REG_CC:
                    CFE_ES_DumpCDSRegCmd(Msg);
                    break;

                default:
                    CFE_EVS_SendEvent(CFE_ES_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    CFE_ES_TaskData.ErrCounter++;
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(CFE_ES_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            CFE_ES_TaskData.ErrCounter++;
            break;
    }

} /* End of CFE_ES_TaskPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_HousekeepingCmd() -- On-board command (HK request)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_HousekeepingCmd(CFE_SB_MsgPtr_t Msg)
{
    OS_heap_prop_t HeapProp;
    int32          stat;
    uint32         PerfIdx;

    /*
    ** Get command execution counters, system log entry count & bytes used.
    */
    CFE_ES_TaskData.HkPacket.Payload.CmdCounter = CFE_ES_TaskData.CmdCounter;
    CFE_ES_TaskData.HkPacket.Payload.ErrCounter = CFE_ES_TaskData.ErrCounter;

    CFE_ES_TaskData.HkPacket.Payload.SysLogBytesUsed = CFE_ES_ResetDataPtr->SystemLogIndex;
    CFE_ES_TaskData.HkPacket.Payload.SysLogSize = CFE_ES_SYSTEM_LOG_SIZE;
    CFE_ES_TaskData.HkPacket.Payload.SysLogEntries   = CFE_ES_ResetDataPtr->SystemLogEntryNum;
    CFE_ES_TaskData.HkPacket.Payload.SysLogMode = CFE_ES_ResetDataPtr->SystemLogMode;

    CFE_ES_TaskData.HkPacket.Payload.ERLogIndex      = CFE_ES_ResetDataPtr->ERLogIndex;
    CFE_ES_TaskData.HkPacket.Payload.ERLogEntries    = CFE_ES_ResetDataPtr->ERLogEntries;
    
    CFE_ES_TaskData.HkPacket.Payload.RegisteredCoreApps      = CFE_ES_Global.RegisteredCoreApps;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredExternalApps  = CFE_ES_Global.RegisteredExternalApps;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredTasks         = CFE_ES_Global.RegisteredTasks;
    CFE_ES_TaskData.HkPacket.Payload.RegisteredLibs          = CFE_ES_Global.RegisteredLibs;

    CFE_ES_TaskData.HkPacket.Payload.ResetType = CFE_ES_ResetDataPtr->ResetVars.ResetType;
    CFE_ES_TaskData.HkPacket.Payload.ResetSubtype = CFE_ES_ResetDataPtr->ResetVars.ResetSubtype;
    CFE_ES_TaskData.HkPacket.Payload.ProcessorResets = CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount;
    CFE_ES_TaskData.HkPacket.Payload.MaxProcessorResets = CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;
    CFE_ES_TaskData.HkPacket.Payload.BootSource = CFE_ES_ResetDataPtr->ResetVars.BootSource;

    CFE_ES_TaskData.HkPacket.Payload.PerfState = CFE_ES_ResetDataPtr->Perf.MetaData.State;
    CFE_ES_TaskData.HkPacket.Payload.PerfMode = CFE_ES_ResetDataPtr->Perf.MetaData.Mode;
    CFE_ES_TaskData.HkPacket.Payload.PerfTriggerCount = CFE_ES_ResetDataPtr->Perf.MetaData.TriggerCount;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataStart = CFE_ES_ResetDataPtr->Perf.MetaData.DataStart;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataEnd = CFE_ES_ResetDataPtr->Perf.MetaData.DataEnd;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataCount = CFE_ES_ResetDataPtr->Perf.MetaData.DataCount;
    CFE_ES_TaskData.HkPacket.Payload.PerfDataToWrite = CFE_ES_PerfLogDumpStatus.DataToWrite;

    /*
     * Fill out the perf trigger/filter mask objects
     * The entire array in the HK payload object (external size) must be filled,
     * to avoid sending garbage data.
     *
     * If it is larger than what the platform supports (internal size), it will
     * be padded with 0's
     *
     * If it is smaller than what the platform supports, then truncate.
     */
    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_TRIGGERMASK_EXT_SIZE; ++PerfIdx)
    {
        if (PerfIdx < CFE_ES_PERF_TRIGGERMASK_INT_SIZE)
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] =
                    CFE_ES_ResetDataPtr->Perf.MetaData.TriggerMask[PerfIdx];
        }
        else
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] = 0;
        }
    }

    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_FILTERMASK_EXT_SIZE; ++PerfIdx)
    {
        if (PerfIdx < CFE_ES_PERF_FILTERMASK_INT_SIZE)
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] =
                    CFE_ES_ResetDataPtr->Perf.MetaData.FilterMask[PerfIdx];
        }
        else
        {
            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] = 0;
        }
    }

    stat = OS_HeapGetInfo(&HeapProp);

    if(stat == OS_SUCCESS)
    {
        CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree = HeapProp.free_bytes;
        CFE_ES_TaskData.HkPacket.Payload.HeapBlocksFree = HeapProp.free_blocks;
        CFE_ES_TaskData.HkPacket.Payload.HeapMaxBlockSize = HeapProp.largest_free_block;
    }
    else
    {
        CFE_ES_TaskData.HkPacket.Payload.HeapBytesFree = 0;
        CFE_ES_TaskData.HkPacket.Payload.HeapBlocksFree = 0;
        CFE_ES_TaskData.HkPacket.Payload.HeapMaxBlockSize = 0;
    }

    /*
    ** Send housekeeping telemetry packet.
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.HkPacket);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.HkPacket);

    /*
    ** This command does not affect the command execution counter.
    */

} /* End of CFE_ES_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_NoopCmd() -- ES task ground command (NO-OP)              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_NoopCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_NoArgsCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        /*
         ** Advertise the build and version information with the no-op command
         ** For unit testing purposes, it helps to put this first - the UT
         ** is checking for the last event sent to be NOOP_INF_EID.
         */
#ifdef _ENHANCED_BUILD_
        CFE_EVS_SendEvent(CFE_ES_BUILD_INF_EID,
              CFE_EVS_INFORMATION,
              "Build %s %s",
              GLOBAL_CONFIGDATA.Date, GLOBAL_CONFIGDATA.User);
#endif

        /*
        ** This command will always succeed.
        */
        CFE_ES_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_ES_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "No-op command. Versions:cFE %d.%d.%d.%d, OSAL %d.%d.%d.%d, PSP %d.%d.%d.%d",
                         CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV,                      
                         OS_MAJOR_VERSION,OS_MINOR_VERSION,OS_REVISION,OS_MISSION_REV,
                         CFE_PSP_MAJOR_VERSION,CFE_PSP_MINOR_VERSION,CFE_PSP_REVISION,CFE_PSP_MISSION_REV);

    }

        
} /* End of CFE_ES_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ResetCmd() -- ES task ground command (reset counters)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_ResetCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_NoArgsCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        CFE_ES_TaskData.CmdCounter = 0;
        CFE_ES_TaskData.ErrCounter = 0;

        /*
        ** This command will always succeed.
        */
        CFE_EVS_SendEvent(CFE_ES_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "Reset Counters command");
    }

} /* End of CFE_ES_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_RestartCmd() -- Restart cFE (may reset processor)        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_RestartCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_RestartCmd_Payload_t *cmd = (CFE_ES_RestartCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_RestartCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {

        if ((cmd->RestartType != CFE_PSP_RST_TYPE_PROCESSOR) &&
            (cmd->RestartType != CFE_PSP_RST_TYPE_POWERON))
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_BOOT_ERR_EID, CFE_EVS_ERROR,
                             "Invalid cFE restart type: %d",
                              (int)cmd->RestartType);
        }
        else
        {
            /*
            ** This function will not return.
            */
            CFE_ES_ResetCFE(cmd->RestartType);
        }
    }

} /* End of CFE_ES_RestartCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ShellCmd() -- Pass thru string to O/S shell              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_ShellCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_ShellCmd_t);
    int32 Result;
    CFE_ES_ShellCmd_Payload_t *cmd = (CFE_ES_ShellCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    char LocalCmd[CFE_ES_MAX_SHELL_CMD];
    char LocalFile[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        /* Create local copies of both input strings and ensure null termination */
        CFE_SB_MessageStringGet(LocalCmd, (char *)cmd->CmdString, NULL,
                CFE_ES_MAX_SHELL_CMD, sizeof(cmd->CmdString));

        CFE_SB_MessageStringGet(LocalFile, (char *)cmd->OutputFilename, NULL,
                OS_MAX_PATH_LEN, sizeof(cmd->OutputFilename));

        /*
        ** Call the Shell command API
        */
        Result = CFE_ES_ShellOutputCommand(LocalCmd, LocalFile);
        /*
        ** Send appropriate event message.
        */
        if (Result == CFE_SUCCESS)
        {
            CFE_ES_TaskData.CmdCounter++;
            CFE_EVS_SendEvent(CFE_ES_SHELL_INF_EID, CFE_EVS_INFORMATION,
                             "Invoked shell command: '%s'",
                              LocalCmd);
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_SHELL_ERR_EID, CFE_EVS_ERROR,
                    "Failed to invoke shell command: '%s', RC = 0x%08X",
                    LocalCmd, (unsigned int)Result);
        }
    }

} /* End of CFE_ES_ShellCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_StartAppCmd() -- Load (and start) single application     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_StartAppCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16                ExpectedLength = sizeof(CFE_ES_StartAppCmd_t);
    CFE_ES_StartAppCmd_Payload_t *cmd = (CFE_ES_StartAppCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint32                AppID          = 0;
    int32                 Result;
    int32                 FilenameLen;
    int32                 AppEntryLen;
    int32                 AppNameLen;
    char                  LocalFile[OS_MAX_PATH_LEN];
    char                  LocalEntryPt[OS_MAX_API_NAME];
    char                  LocalAppName[OS_MAX_API_NAME];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        /* Create local copies of all input strings and ensure null termination */
        FilenameLen = CFE_SB_MessageStringGet(LocalFile, (char *)cmd->AppFileName, NULL,
                OS_MAX_PATH_LEN, sizeof(cmd->AppFileName));

        AppEntryLen = CFE_SB_MessageStringGet(LocalEntryPt, (char *)cmd->AppEntryPoint, NULL,
                OS_MAX_API_NAME, sizeof(cmd->AppEntryPoint));

        AppNameLen = CFE_SB_MessageStringGet(LocalAppName, (char *)cmd->Application, NULL,
                OS_MAX_API_NAME, sizeof(cmd->Application));

        /*
        ** Verify command parameters
        */
        if (FilenameLen < 4)
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_INVALID_FILENAME_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: invalid filename: %s",
                           LocalFile);
       }
       else if (AppEntryLen <= 0)
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: App Entry Point is NULL.");
       }
       else if (AppNameLen <= 0)
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_NULL_APP_NAME_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: App Name is NULL.");
       }
       else if (cmd->StackSize < CFE_ES_DEFAULT_STACK_SIZE)
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_STACK_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: Stack size is less than system Minimum: %d.",
                           CFE_ES_DEFAULT_STACK_SIZE);
       }
       else if (cmd->Priority > OS_MAX_PRIORITY)
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_PRIORITY_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: Priority is too large: %d.",
                           cmd->Priority);
       }
       else if ((cmd->ExceptionAction != CFE_ES_APP_EXCEPTION_RESTART_APP) &&
                (cmd->ExceptionAction != CFE_ES_APP_EXCEPTION_PROC_RESTART ))
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_START_EXC_ACTION_ERR_EID, CFE_EVS_ERROR,
                           "CFE_ES_StartAppCmd: Invalid Exception Action: %d.",
                           cmd->ExceptionAction);
       }
       else
       {
          /*
          ** Invoke application loader/startup function.
          */
          Result = CFE_ES_AppCreate(&AppID, LocalFile,
                      LocalEntryPt,
                      LocalAppName,
                      (uint32) cmd->Priority, 
                      (uint32) cmd->StackSize,
                      (uint32) cmd->ExceptionAction);

          /*
          ** Send appropriate event message
          */
          if (Result == CFE_SUCCESS)
          {
             CFE_ES_TaskData.CmdCounter++;
             CFE_EVS_SendEvent(CFE_ES_START_INF_EID, CFE_EVS_INFORMATION,
                                "Started %s from %s, AppID = %d",
                                LocalAppName, LocalFile, (int)AppID);
           }
           else
           {
               CFE_ES_TaskData.ErrCounter++;
               CFE_EVS_SendEvent(CFE_ES_START_ERR_EID, CFE_EVS_ERROR,
                             "Failed to start %s from %s, RC = 0x%08X",
                             LocalAppName, LocalFile, (unsigned int)Result);
           }
           
       } /* End if -- command parameter validation */

   } /* End if - command length verification */
     
} /* End of CFE_ES_StartAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_StopAppCmd() -- Stop single application                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_StopAppCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_AppNameCmd_Payload_t *cmd = (CFE_ES_AppNameCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_AppNameCmd_t);
    char LocalApp[OS_MAX_API_NAME];


    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        uint32 AppID;
        int32 Result;
        
        CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, OS_MAX_API_NAME, sizeof(cmd->Application));

        Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

        if (Result == CFE_SUCCESS)
        {            
            /*
            ** Delete the App
            */
            Result = CFE_ES_DeleteApp(AppID);

            /*
            ** Send appropriate event message.
            */
            if (Result == CFE_SUCCESS)
            {
                CFE_ES_TaskData.CmdCounter++;
                CFE_EVS_SendEvent(CFE_ES_STOP_DBG_EID, CFE_EVS_DEBUG,
                             "Stop Application %s Initiated.", LocalApp);
            }
            else
            {
                CFE_ES_TaskData.ErrCounter++;
                CFE_EVS_SendEvent(CFE_ES_STOP_ERR1_EID, CFE_EVS_ERROR,
                             "Stop Application %s Failed, RC = 0x%08X",
                             LocalApp, (unsigned int)Result);
            }
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_STOP_ERR2_EID, CFE_EVS_ERROR,
                              "Stop Application %s, GetAppIDByName failed. RC = 0x%08X.",
                              LocalApp, (unsigned int)Result);
        }

    }

} /* End of CFE_ES_StopAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_RestartAppCmd() -- Restart a single application            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_RestartAppCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_AppNameCmd_Payload_t *cmd = (CFE_ES_AppNameCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_AppNameCmd_t);
    char LocalApp[OS_MAX_API_NAME];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        uint32 AppID;
        int32 Result;

        CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, OS_MAX_API_NAME, sizeof(cmd->Application));

        Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

        if (Result == CFE_SUCCESS)
        {
            Result = CFE_ES_RestartApp(AppID);

            /*
            ** Send appropriate event message.
            */
            if (Result == CFE_SUCCESS)
            {
                CFE_ES_TaskData.CmdCounter++;
                CFE_EVS_SendEvent(CFE_ES_RESTART_APP_DBG_EID, CFE_EVS_DEBUG,
                             "Restart Application %s Initiated.", LocalApp);
            }
            else
            {
                CFE_ES_TaskData.ErrCounter++;
                CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR1_EID, CFE_EVS_ERROR,
                             "Restart Application %s Failed, RC = 0x%08X",
                             LocalApp, (unsigned int)Result);
            }
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR2_EID, CFE_EVS_ERROR,
                              "Restart Application %s, GetAppIDByName failed. RC = 0x%08X.",
                              LocalApp, (unsigned int)Result);
        }
    }

} /* End of CFE_ES_ResetAppCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ReloadAppCmd() -- Reload a single application            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_ReloadAppCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_AppReloadCmd_Payload_t *cmd = (CFE_ES_AppReloadCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_AppReloadCmd_t);
    char LocalApp[OS_MAX_API_NAME];
    char LocalFileName[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        uint32  AppID;
        int32   Result;

        CFE_SB_MessageStringGet(LocalFileName, (char *)cmd->AppFileName, NULL, sizeof(LocalFileName), sizeof(cmd->AppFileName));
        CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, sizeof(LocalApp), sizeof(cmd->Application));

        Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

        if (Result == CFE_SUCCESS)
        {
           Result = CFE_ES_ReloadApp(AppID, LocalFileName);

            /*
            ** Send appropriate event message.
            */
            if (Result == CFE_SUCCESS)
            {
                CFE_ES_TaskData.CmdCounter++;
                CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_DBG_EID, CFE_EVS_DEBUG,
                             "Reload Application %s Initiated.", LocalApp);
            }
            else
            {
                CFE_ES_TaskData.ErrCounter++;
                CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR1_EID, CFE_EVS_ERROR,
                             "Reload Application %s Failed, RC = 0x%08X",
                             LocalApp, (unsigned int)Result);
            }
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR2_EID, CFE_EVS_ERROR,
                              "Reload Application %s, GetAppIDByName failed. RC = 0x%08X.",
                              LocalApp, (unsigned int)Result);
        }
    }

} /* End of CFE_ES_ReloadAppCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryOneCmd() -- Request tlm packet with single app data */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_QueryOneCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_AppNameCmd_Payload_t *cmd = (CFE_ES_AppNameCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_AppNameCmd_t);
    char LocalApp[OS_MAX_API_NAME];

    /*
    ** Verify command packet length
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        uint32 AppID;
        int32 Result;

        CFE_SB_MessageStringGet(LocalApp, (char *)cmd->Application, NULL, OS_MAX_API_NAME, sizeof(cmd->Application));

        Result = CFE_ES_GetAppIDByName(&AppID, LocalApp);

        /*
        ** Send appropriate event message...
        */
        if (Result == CFE_SUCCESS)
        {
        
           CFE_ES_GetAppInfoInternal(AppID, &(CFE_ES_TaskData.OneAppPacket.Payload.AppInfo));
           /*
           ** Send application status telemetry packet.
           */
           CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.OneAppPacket);
           Result = CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.OneAppPacket);
           if ( Result == CFE_SUCCESS )
           {
               CFE_ES_TaskData.CmdCounter++;
               CFE_EVS_SendEvent(CFE_ES_ONE_APP_EID, CFE_EVS_DEBUG,
                       "Sent %s application data", LocalApp);
           }
           else
           {
               CFE_ES_TaskData.ErrCounter++;
               CFE_EVS_SendEvent(CFE_ES_ONE_ERR_EID, CFE_EVS_ERROR,
                          "Failed to send %s application data, RC = 0x%08X",
                          LocalApp, (unsigned int)Result);
           }
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;
            CFE_EVS_SendEvent(CFE_ES_ONE_APPID_ERR_EID, CFE_EVS_ERROR,
                       "Failed to send %s application data: GetAppIDByName Failed, RC = 0x%08X",
                       LocalApp, (unsigned int)Result);
        }
    }

} /* End of CFE_ES_QueryOneCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryAllCmd() -- Write all app data to file              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_QueryAllCmd(CFE_SB_MsgPtr_t Msg)
{

    uint16                ExpectedLength = sizeof(CFE_ES_QueryAllCmd_t);
    CFE_FS_Header_t       FileHeader;
    int32                 FileDescriptor;
    uint32                i;
    uint32                EntryCount = 0;
    uint32                FileSize = 0;
    int32                 Result;
    CFE_ES_AppInfo_t      AppInfo;
    CFE_ES_QueryAllCmd_Payload_t *CmdPtr = (CFE_ES_QueryAllCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    char                  QueryAllFilename[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
       /* 
       ** Copy the commanded filename into local buffer to ensure size limitation and to allow for modification 
       */
       CFE_SB_MessageStringGet(QueryAllFilename, (char *)CmdPtr->QueryAllFileName,
                CFE_ES_DEFAULT_APP_LOG_FILE, OS_MAX_PATH_LEN, sizeof(CmdPtr->QueryAllFileName));

       /* 
       ** Check to see if the file already exists 
       */
       FileDescriptor = OS_open(QueryAllFilename, OS_READ_ONLY, 0);
       if (FileDescriptor >= 0)
       {
           OS_close(FileDescriptor);
           OS_remove(QueryAllFilename);
       }

       /*
       ** Create ES task log data file
       */
       FileDescriptor = OS_creat(QueryAllFilename, OS_WRITE_ONLY);
       if (FileDescriptor >= 0)
       {
          /*
          ** Initialize cFE file header
          */
          CFE_FS_InitHeader(&FileHeader, CFE_ES_APP_LOG_DESC, CFE_FS_ES_QUERYALL_SUBTYPE);

          /*
          ** Output the Standard cFE File Header to the App File
          */
          Result = CFE_FS_WriteHeader(FileDescriptor, &FileHeader);

          if (Result != sizeof(CFE_FS_Header_t))
          {
              OS_close(FileDescriptor);
              CFE_ES_TaskData.ErrCounter++;
              CFE_EVS_SendEvent(CFE_ES_WRHDR_ERR_EID, CFE_EVS_ERROR,
                 "Failed to write App Info file, WriteHdr RC = 0x%08X, exp %d",
              (unsigned int)Result,(int)sizeof(CFE_FS_Header_t));
              return;
          }/* end if */

          /* 
          ** Maintain statistics of amount of data written to file 
          */
          FileSize += Result;

          /*
          ** Loop through the ES AppTable for main applications
          */
          for(i=0;i<CFE_ES_MAX_APPLICATIONS;i++)
          {
             if(CFE_ES_Global.AppTable[i].RecordUsed != FALSE)
             {
                /* 
                ** zero out the local entry 
                */
                CFE_PSP_MemSet(&AppInfo,0,sizeof(CFE_ES_AppInfo_t));

                /*
                ** Populate the AppInfo entry 
                */
                CFE_ES_GetAppInfoInternal(i, &AppInfo);

                /*
                ** Write the local entry to file 
                */
                Result = OS_write(FileDescriptor, &AppInfo, sizeof(CFE_ES_AppInfo_t));
                if (Result !=  sizeof(CFE_ES_AppInfo_t))
                {
                   OS_close(FileDescriptor);
                   CFE_ES_TaskData.ErrCounter++;
                   CFE_EVS_SendEvent(CFE_ES_TASKWR_ERR_EID, CFE_EVS_ERROR,
                      "Failed to write App Info file, Task write RC = 0x%08X, exp %d",
                      (unsigned int)Result,(int)sizeof(CFE_ES_AppInfo_t));
                   return;
                }/* end if */

                FileSize += Result;
                EntryCount ++;
             }
          } /* end for */

          OS_close(FileDescriptor);
          CFE_ES_TaskData.CmdCounter++;
          CFE_EVS_SendEvent(CFE_ES_ALL_APPS_EID, CFE_EVS_DEBUG,
                        "App Info file written to %s, Entries=%d, FileSize=%d",
                      QueryAllFilename,(int)EntryCount,(int)FileSize);
       }
       else
       {
           CFE_ES_TaskData.ErrCounter++;
           CFE_EVS_SendEvent(CFE_ES_OSCREATE_ERR_EID, CFE_EVS_ERROR,
           "Failed to write App Info file, OS_creat RC = 0x%08X",(unsigned int)FileDescriptor);
       }
       
    } /* end if verify command length */
    return;

} /* End of CFE_ES_QueryAllCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_QueryAllTasksCmd() -- Write all Task Data to a file      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_QueryAllTasksCmd(CFE_SB_MsgPtr_t Msg)
{

    uint16                     ExpectedLength = sizeof(CFE_ES_QueryAllTasksCmd_t);
    CFE_FS_Header_t            FileHeader;
    int32                      FileDescriptor;
    uint32                     i;
    uint32                     EntryCount = 0;
    uint32                     FileSize = 0;
    int32                      Result;
    CFE_ES_TaskInfo_t          TaskInfo;
    CFE_ES_QueryAllTasksCmd_Payload_t *CmdPtr = (CFE_ES_QueryAllTasksCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    char                       QueryAllFilename[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
       /* 
       ** Copy the commanded filename into local buffer to ensure size limitation and to allow for modification 
       */
       CFE_SB_MessageStringGet(QueryAllFilename, (char *)CmdPtr->QueryAllFileName,
               CFE_ES_DEFAULT_TASK_LOG_FILE, OS_MAX_PATH_LEN, sizeof(CmdPtr->QueryAllFileName));

       /* 
       ** Check to see if the file already exists 
       */
       FileDescriptor = OS_open(QueryAllFilename, OS_READ_ONLY, 0);
       if (FileDescriptor >= 0)
       {
           OS_close(FileDescriptor);
           OS_remove(QueryAllFilename);
       }

       /*
       ** Create ES task log data file
       */
       FileDescriptor = OS_creat(QueryAllFilename, OS_WRITE_ONLY);
       if (FileDescriptor >= 0)
       {
          /*
          ** Initialize cFE file header
          */
          CFE_FS_InitHeader(&FileHeader, CFE_ES_TASK_LOG_DESC, CFE_FS_ES_QUERYALLTASKS_SUBTYPE);

          /*
          ** Output the Standard cFE File Header to the App File
          */
          Result = CFE_FS_WriteHeader(FileDescriptor, &FileHeader);

          if (Result != sizeof(CFE_FS_Header_t))
          {
              OS_close(FileDescriptor);
              CFE_ES_TaskData.ErrCounter++;
              CFE_EVS_SendEvent(CFE_ES_TASKINFO_WRHDR_ERR_EID, CFE_EVS_ERROR,
                 "Failed to write Task Info file, WriteHdr RC = 0x%08X, exp %d",
              (unsigned int)Result,(int)sizeof(CFE_FS_Header_t));
              return;
          }/* end if */

          /* 
          ** Maintain statistics of amount of data written to file 
          */
          FileSize += Result;

          /*
          ** Loop through the ES AppTable for main applications
          */
          for(i=0;i<OS_MAX_TASKS;i++)
          {
             if(CFE_ES_Global.TaskTable[i].RecordUsed != FALSE)
             {
                /* 
                ** zero out the local entry 
                */
                CFE_PSP_MemSet(&TaskInfo,0,sizeof(CFE_ES_TaskInfo_t));

                /*
                ** Populate the AppInfo entry 
                */
                CFE_ES_GetTaskInfo(&TaskInfo,CFE_ES_Global.TaskTable[i].TaskId);

                /*
                ** Write the local entry to file 
                */
                Result = OS_write(FileDescriptor, &TaskInfo, sizeof(CFE_ES_TaskInfo_t));
                if (Result !=  sizeof(CFE_ES_TaskInfo_t))
                {
                   OS_close(FileDescriptor);
                   CFE_ES_TaskData.ErrCounter++;
                   CFE_EVS_SendEvent(CFE_ES_TASKINFO_WR_ERR_EID, CFE_EVS_ERROR,
                      "Failed to write Task Info file, Task write RC = 0x%08X, exp %d",
                      (unsigned int)Result,(int)sizeof(CFE_ES_TaskInfo_t));
                   return;
                }/* end if */

                FileSize += Result;
                EntryCount ++;
             }
            
          } /* end for */

          OS_close(FileDescriptor);
          CFE_ES_TaskData.CmdCounter++;
          CFE_EVS_SendEvent(CFE_ES_TASKINFO_EID, CFE_EVS_DEBUG,
                        "Task Info file written to %s, Entries=%d, FileSize=%d",
                      QueryAllFilename,(int)EntryCount,(int)FileSize);
       }
       else
       {
           CFE_ES_TaskData.ErrCounter++;
           CFE_EVS_SendEvent(CFE_ES_TASKINFO_OSCREATE_ERR_EID, CFE_EVS_ERROR,
           "Failed to write Task Info file, OS_creat RC = 0x%08X",(unsigned int)FileDescriptor);
       }
       
    } /* end if verify command length */
    return;

} /* End of CFE_ES_QueryAllTasksCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ClearSyslogCmd() -- Clear executive services system log  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_ClearSyslogCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_NoArgsCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {

        /*
        ** Clear syslog index and memory area
        */
        CFE_ES_SysLogClear();

        /*
        ** This command will always succeed...
        */
        CFE_ES_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_ES_SYSLOG1_INF_EID, CFE_EVS_INFORMATION,
                         "Cleared Executive Services log data");
    }

} /* End of CFE_ES_ClearSyslogCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_OverWriteSyslogCmd() -- set syslog mode                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_OverWriteSyslogCmd(CFE_SB_MsgPtr_t Msg)
{
   uint32                        Status = 0;
   CFE_ES_OverWriteSysLogCmd_Payload_t *CmdPtr = (CFE_ES_OverWriteSysLogCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
   uint16                        ExpectedLength = sizeof(CFE_ES_OverWriteSysLogCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {

      Status = CFE_ES_SysLogOverwrite(CmdPtr->Mode);

      if (Status == FALSE)
      {
         CFE_EVS_SendEvent(CFE_ES_ERR_SYSLOGMODE_EID,
                           CFE_EVS_ERROR,
                        "Set OverWriteSysLog Command: Invalid Mode setting = %d", (int)CmdPtr->Mode);
                           
         CFE_ES_TaskData.ErrCounter++;      
      }
      else
      {
         CFE_EVS_SendEvent(CFE_ES_SYSLOGMODE_EID,
                           CFE_EVS_DEBUG,
                        "Set OverWriteSysLog Command Received with Mode setting = %d", (int)CmdPtr->Mode);
                           
         CFE_ES_TaskData.CmdCounter++;
      }

   } /* end if verify command length */
   
} /* End CFE_ES_OverWriteSyslogCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_SysLogClear -- Clear system log & index                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_SysLogClear(void)
{
    /*
    ** Clear SysLog data buffer
    */
    CFE_PSP_MemSet(CFE_ES_ResetDataPtr->SystemLog, 0, CFE_ES_SYSTEM_LOG_SIZE);

    /*
    ** Initialize SysLog index & Entry Count
    */
    CFE_ES_ResetDataPtr->SystemLogIndex = 0;
    CFE_ES_ResetDataPtr->SystemLogEntryNum = 0;

} /* End of CFE_ES_SysLogClear() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                       */
/* CFE_ES_WriteSyslogCmd() -- Process Cmd to write ES System Log to file */
/*                                                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_WriteSyslogCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_WriteSyslogCmd_Payload_t *CmdPtr = (CFE_ES_WriteSyslogCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    int32                     Stat;
    uint16                    ExpectedLength = sizeof(CFE_ES_WriteSyslogCmd_t);
    char                      LogFilename[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
       CFE_SB_MessageStringGet(LogFilename, (char *)CmdPtr->SysLogFileName,
               CFE_ES_DEFAULT_SYSLOG_FILE, OS_MAX_PATH_LEN, sizeof(CmdPtr->SysLogFileName));

       Stat = CFE_ES_SysLogDump(LogFilename);
         
       if(Stat == CFE_SUCCESS)
       {
           CFE_ES_TaskData.CmdCounter++;
       }
       else
       {
           CFE_ES_TaskData.ErrCounter++;
       }/* end if */

   } /* end if command length verify */
   
}/* end CFE_ES_WriteSyslogCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_SysLogDump() -- Write ES System Log to a file.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_SysLogDump(const char *Filename)
{
    int32   fd;
    int32   WriteStat;
    uint32  FileSize;
    CFE_FS_Header_t FileHdr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < 0)
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOG2_ERR_EID,CFE_EVS_ERROR,
                      "Error creating file %s, RC = 0x%08X",
                      Filename,(unsigned int)fd);
        return CFE_ES_FILE_IO_ERR;
    }/* end if */

    CFE_FS_InitHeader(&FileHdr, CFE_ES_SYS_LOG_DESC, CFE_FS_ES_SYSLOG_SUBTYPE);

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t))
    {
        CFE_ES_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_ES_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* need to take semaphore here */

    WriteStat = OS_write(fd,CFE_ES_ResetDataPtr->SystemLog,CFE_ES_SYSTEM_LOG_SIZE);
    if(WriteStat != CFE_ES_SYSTEM_LOG_SIZE)
    {
        CFE_ES_FileWriteByteCntErr(Filename,CFE_ES_SYSTEM_LOG_SIZE,WriteStat);
        OS_close(fd);
        return CFE_ES_FILE_IO_ERR;
    }/* end if */

    /* need to release the semaphore here */

    FileSize += WriteStat;    
    
    OS_close(fd);

    CFE_EVS_SendEvent(CFE_ES_SYSLOG2_EID, CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",Filename,(int)FileSize,
                      (int)CFE_ES_TaskData.HkPacket.Payload.SysLogEntries);

    return CFE_SUCCESS;

} /* End of CFE_ES_SysLogDump() */



/*
** Function: CFE_ES_SysLogOverwrite
**
** Purpose:  Set syslog write mode
**
*/
int32 CFE_ES_SysLogOverwrite(uint32 Mode )
{
    uint32 Status = TRUE;

    if((Mode == CFE_ES_LOG_OVERWRITE) || (Mode == CFE_ES_LOG_DISCARD))
    {
        CFE_ES_ResetDataPtr->SystemLogMode = Mode;
    }
    else
    {
        Status = FALSE;
    }

    return Status;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ClearERlogCmd() -- Clear The exception and reset log.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_ClearERlogCmd(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_NoArgsCmd_t);

    /*
    ** Verify command packet length
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        /*
        ** Clear ER log data buffer
        */

        CFE_PSP_MemSet(CFE_ES_ResetDataPtr->ERLog, 0, sizeof(CFE_ES_ResetDataPtr->ERLog));

        /*
        ** Reset ER log buffer index
        */

        CFE_ES_ResetDataPtr->ERLogIndex = 0;
        
        /* 
        ** Set Number of Entries in ER log buffer back to zero
        */
        CFE_ES_ResetDataPtr->ERLogEntries = 0;

        /*
        ** This command will always succeed
        */
        CFE_ES_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_ES_ERLOG1_INF_EID, CFE_EVS_INFORMATION,
                         "Cleared ES Exception and Reset Log data");
    }

} /* End of CFE_ES_ClearERlogCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_WriteERlogCmd() -- Process Cmd to write exception & reset*/
/*                           log to a file.                        */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_ES_WriteERlogCmd(CFE_SB_MsgPtr_t Msg)
{
    CFE_ES_WriteERlogCmd_Payload_t *CmdPtr = (CFE_ES_WriteERlogCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    int32                    Stat;
    uint16                   ExpectedLength = sizeof(CFE_ES_WriteERlogCmd_t);
    char                     LogFilename[OS_MAX_PATH_LEN];

    /*
    ** Verify command packet length
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
       CFE_SB_MessageStringGet(LogFilename, (char *)CmdPtr->ERLogFileName,
               CFE_ES_DEFAULT_ER_LOG_FILE, OS_MAX_PATH_LEN, sizeof(CmdPtr->ERLogFileName));

       Stat = CFE_ES_ERLogDump(LogFilename);
         
       if(Stat == CFE_SUCCESS)
       {
           CFE_ES_TaskData.CmdCounter++;
       }
       else
       {
           CFE_ES_TaskData.ErrCounter++;
       }/* end if */
       
   } /* end if verify command length */
   
}/* end CFE_ES_WriteERlogCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ERLogDump() -- Write exception & reset log to a file.    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ERLogDump(const char *Filename)
{

    int32   fd;
    int32   WriteStat,BspStat;
    uint32  FileSize,i,ResetAreaSize;
    CFE_FS_Header_t FileHdr;
    cpuaddr ResetDataAddr;
    
    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < 0)
    {
        CFE_EVS_SendEvent(CFE_ES_ERLOG2_ERR_EID,CFE_EVS_ERROR,
                      "Error creating file %s, RC = 0x%08X",
                      Filename,(unsigned int)fd);
        return CFE_ES_FILE_IO_ERR;
    }/* end if */

    CFE_FS_InitHeader(&FileHdr, CFE_ES_ER_LOG_DESC, CFE_FS_ES_ERLOG_SUBTYPE);

    /* write the cFE header to the file */
    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);    
    if(WriteStat != sizeof(CFE_FS_Header_t))
    {
        CFE_ES_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_ES_FILE_IO_ERR;        
    }/* end if */
    FileSize = WriteStat;

    /* Get the pointer to the Reset Log from the BSP */
    BspStat = CFE_PSP_GetResetArea (&ResetDataAddr, &ResetAreaSize);
    if(BspStat != CFE_PSP_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_RST_ACCESS_EID, CFE_EVS_ERROR,
              "Error accessing ER Log,%s not written. RC = 0x%08X",Filename,(unsigned int)BspStat);
        OS_close(fd);
        return CFE_ES_RST_ACCESS_ERR;        
    }/* end if */

    /* write a single ER log entry on each pass */
    for(i=0;i<CFE_ES_ER_LOG_ENTRIES;i++)
    {
        WriteStat = OS_write(fd,(uint8 *)ResetDataAddr,sizeof(CFE_ES_ERLog_t));
        if(WriteStat != sizeof(CFE_ES_ERLog_t))
        {
            CFE_ES_FileWriteByteCntErr(Filename,sizeof(CFE_ES_ERLog_t),WriteStat);
            OS_close(fd);
            return CFE_ES_FILE_IO_ERR;        
        }/* end if */        
        FileSize += WriteStat;
        ResetDataAddr+=sizeof(CFE_ES_ERLog_t);
    }/* end for */   
    
    OS_close(fd);

    CFE_EVS_SendEvent(CFE_ES_ERLOG2_EID, CFE_EVS_DEBUG,
                      "%s written:Size=%d",Filename,(int)FileSize);

    return CFE_SUCCESS;

} /* end CFE_ES_ERLogDump() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_VerifyCmdLength() -- Verify command packet length        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean CFE_ES_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
{
    boolean result       = TRUE;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(Msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(CFE_ES_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid cmd length: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
                          MessageID, CommandCode, ExpectedLength, ActualLength);
        result = FALSE;
        CFE_ES_TaskData.ErrCounter++;
    }

    return(result);

} /* End of CFE_ES_VerifyCmdLength() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_ResetPRCountCmd() -- ES task ground command              */
/*                              (Processor Reset Count)            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
                
void CFE_ES_ResetPRCountCmd(CFE_SB_MsgPtr_t Msg)
{                   
    uint16 ExpectedLength = sizeof(CFE_ES_NoArgsCmd_t);
                
    /*              
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {               
                     
        /*                            
        ** Reset the processor reset count
        */          
        CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
            
        /*
        ** This command will always succeed.                                       
        */                                                                         
        CFE_EVS_SendEvent(CFE_ES_RESET_PR_COUNT_EID, CFE_EVS_INFORMATION,          
                         "Set Processor Reset Count to Zero");  
                         
        CFE_ES_TaskData.CmdCounter++;
    }                                                                              
                                                                                   
} /* End of CFE_ES_ResetPRCountCmd() */                                            

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
/*                                                                 */              
/* CFE_ES_SetMaxPRCountCmd() -- Set Maximum Processor reset count  */              
/*                                                                 */              
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
                                                                                   
void CFE_ES_SetMaxPRCountCmd(CFE_SB_MsgPtr_t Msg)                                  
{                                                                                  
    CFE_ES_SetMaxPRCountCmd_Payload_t *cmd = (CFE_ES_SetMaxPRCountCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_SetMaxPRCountCmd_t);                     
                                                                                   
    /*                                                                             
    ** Verify command packet length.                                               
    */                                                                             
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))                               
    {                                                                              
                                                                                   
        /*                                                                         
        ** Set the MAX Processor reset count                                       
        */                                                                         
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = cmd->MaxPRCount;   
                                                                                   
        /*                                                                         
        ** This command will always succeed.                                       
        */                                                                         
        CFE_EVS_SendEvent(CFE_ES_SET_MAX_PR_COUNT_EID, CFE_EVS_INFORMATION,        
                         "Maximum Processor Reset Count set to: %d", cmd->MaxPRCount);
                         
        CFE_ES_TaskData.CmdCounter++;
        
    }                                                                              
                                                                                   
} /* End of CFE_ES_RestartCmd() */ 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
/*                                                                 */              
/* CFE_ES_DeleteCDSCmd() -- Delete Specified Critical Data Store   */              
/*                                                                 */              
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
                                                                                   
void CFE_ES_DeleteCDSCmd(CFE_SB_MsgPtr_t Msg)                                  
{                                                                                  
    uint16 ExpectedLength = sizeof(CFE_ES_DeleteCDSCmd_t);                     
    int32   Status;
    CFE_ES_DeleteCDSCmd_Payload_t *cmd = (CFE_ES_DeleteCDSCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    char LocalCdsName[CFE_ES_CDS_MAX_FULL_NAME_LEN];
                                                                                   
    /*                                                                             
    ** Verify command packet length.                                               
    */                                                                             
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))                               
    {
        CFE_SB_MessageStringGet(LocalCdsName, (char *)cmd->CdsName, NULL,
                CFE_ES_CDS_MAX_FULL_NAME_LEN, sizeof(cmd->CdsName));

        Status = CFE_ES_DeleteCDS(LocalCdsName, FALSE);
        
        if (Status == CFE_ES_CDS_WRONG_TYPE_ERR)
        {
            CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_TBL_ERR_EID, CFE_EVS_ERROR,
                              "CDS '%s' is a Critical Table CDS. Must be deleted via TBL Command", 
                              LocalCdsName);

            CFE_ES_TaskData.ErrCounter++;      
        }
        else if (Status == CFE_ES_CDS_OWNER_ACTIVE_ERR)
        {
            CFE_EVS_SendEvent(CFE_ES_CDS_OWNER_ACTIVE_EID, CFE_EVS_ERROR,        
                             "CDS '%s' not deleted because owning app is active", LocalCdsName);
                             
            CFE_ES_TaskData.ErrCounter++;
        }
        else if (Status == CFE_ES_CDS_NOT_FOUND_ERR)
        {
            CFE_EVS_SendEvent(CFE_ES_CDS_NAME_ERR_EID, CFE_EVS_ERROR,        
                             "Unable to locate '%s' in CDS Registry", LocalCdsName);
                             
            CFE_ES_TaskData.ErrCounter++;
        }
        else if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_ES_CDS_DELETE_ERR_EID, CFE_EVS_ERROR,
                             "Error while deleting '%s' from CDS, See SysLog.(Err=0x%08X)",
                             LocalCdsName, (unsigned int)Status);

            CFE_ES_TaskData.ErrCounter++;      
        }
        else
        {
            CFE_EVS_SendEvent(CFE_ES_CDS_DELETED_INFO_EID, CFE_EVS_INFORMATION,
                              "Successfully removed '%s' from CDS", LocalCdsName);

            CFE_ES_TaskData.CmdCounter++;      
        }
    }

} /* End of CFE_ES_DeleteCDSCmd() */ 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
/*                                                                 */              
/* CFE_ES_TlmPoolStatsCmd() -- Telemeter Memory Pool Statistics    */              
/*                                                                 */
/* Note: The "Application" parameter of the                        */ 
/*       CFE_ES_TlmPoolSatatsCmd_t structure is not used.          */
/*                                                                 */              
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
                                                                                   
void CFE_ES_TlmPoolStatsCmd(CFE_SB_MsgPtr_t Msg)                                  
{                                                                                  
    uint16                    ExpectedLength = sizeof(CFE_ES_TlmPoolStatsCmd_t);                     
    CFE_ES_TlmPoolStatsCmd_Payload_t *Cmd;
    CFE_ES_MemHandle_t        MemHandle;
    boolean                   ValidHandle;
                                                                                   
    /*                                                                             
    ** Verify command packet length.                                               
    */                                                                             
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))                               
    {                                                                              
        Cmd = (CFE_ES_TlmPoolStatsCmd_Payload_t *) &Msg->Byte[CFE_SB_CMD_HDR_SIZE];
        
        /* Verify the handle to make sure it is legit */
        MemHandle = CFE_SB_GET_MEMADDR(Cmd->PoolHandle);
        ValidHandle = CFE_ES_ValidateHandle(MemHandle);
        
        if (ValidHandle)
        {
            /* Extract the memory statistics from the memory pool */
            CFE_ES_GetMemPoolStats(&CFE_ES_TaskData.MemStatsPacket.Payload.PoolStats, MemHandle);
            
            /* Echo the specified pool handle in the telemetry packet */
            CFE_SB_SET_MEMADDR(CFE_ES_TaskData.MemStatsPacket.Payload.PoolHandle, MemHandle);
            
            /*
            ** Send memory statistics telemetry packet.
            */
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.MemStatsPacket);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_ES_TaskData.MemStatsPacket);

            CFE_ES_TaskData.CmdCounter++;      
            CFE_EVS_SendEvent(CFE_ES_TLM_POOL_STATS_INFO_EID, CFE_EVS_DEBUG,
                        "Successfully telemetered memory pool stats for 0x%08lX", (unsigned long)Cmd->PoolHandle);
        }
        else
        {
            CFE_ES_TaskData.ErrCounter++;      
            CFE_EVS_SendEvent(CFE_ES_INVALID_POOL_HANDLE_ERR_EID, CFE_EVS_ERROR,
                              "Cannot telemeter memory pool stats. Illegal Handle (0x%08lX)", 
                              (unsigned long)Cmd->PoolHandle);
        }
    }

} /* End of CFE_ES_TlmPoolStatsCmd() */ 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
/*                                                                 */              
/* CFE_ES_DumpCDSRegCmd() -- Dump CDS Registry to a file           */              
/*                                                                 */              
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */              
                                                                                   
void CFE_ES_DumpCDSRegCmd( const CFE_SB_Msg_t *MessagePtr )
{
    CFE_FS_Header_t               StdFileHeader;
    int32                         FileDescriptor;
    int32                         Status;
    int16                         RegIndex=0;
    const CFE_ES_DumpCDSRegCmd_Payload_t *CmdPtr = (const CFE_ES_DumpCDSRegCmd_Payload_t *) &MessagePtr->Byte[CFE_SB_CMD_HDR_SIZE];
    char                          DumpFilename[OS_MAX_PATH_LEN];
    CFE_ES_CDS_RegRec_t          *RegRecPtr;
    CFE_ES_CDSRegDumpRec_t        DumpRecord;
    int32                         FileSize=0;
    int32                         NumEntries=0;

    /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
    CFE_SB_MessageStringGet(DumpFilename, CmdPtr->DumpFilename, CFE_ES_DEFAULT_CDS_REG_DUMP_FILE,
            OS_MAX_PATH_LEN, sizeof(CmdPtr->DumpFilename));

    /* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat(DumpFilename, OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "CDS_Registry", CFE_FS_ES_CDS_REG_SUBTYPE);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);
        
        /* Maintain statistics of amount of data written to file */
        FileSize += Status;

        if (Status == sizeof(CFE_FS_Header_t))
        {
            Status = sizeof(CFE_ES_CDSRegDumpRec_t);
            while ((RegIndex < CFE_ES_CDS_MAX_NUM_ENTRIES) && (Status == sizeof(CFE_ES_CDSRegDumpRec_t)))
            {
                /* Make a pointer to simplify code look and to remove redundant indexing into registry */
                RegRecPtr = &CFE_ES_Global.CDSVars.Registry[RegIndex];

                /* Check to see if the Registry entry is empty */
                if (RegRecPtr->Taken == TRUE)
                {
                    /* Fill CDS Registry Dump Record with relevant information */
                    DumpRecord.Size             = RegRecPtr->Size;
                    DumpRecord.Handle           = RegRecPtr->MemHandle;
                    DumpRecord.Table            = RegRecPtr->Table;
                    DumpRecord.ByteAlignSpare1  = 0;
                    
                    /* strncpy will zero out any unused buffer - memset not necessary */
                    strncpy(DumpRecord.Name, RegRecPtr->Name, sizeof(DumpRecord.Name));

                    /* Output Registry Dump Record to Registry Dump File */
                    Status = OS_write(FileDescriptor,
                                      &DumpRecord,
                                      sizeof(CFE_ES_CDSRegDumpRec_t));
                    
                    FileSize += Status;
                    NumEntries++;      
                }

                /* Look at the next entry in the Registry */
                RegIndex++;
            }

            if (Status == sizeof(CFE_ES_CDSRegDumpRec_t))
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_REG_DUMP_INF_EID,
                                  CFE_EVS_DEBUG,
                                  "Successfully dumped CDS Registry to '%s':Size=%d,Entries=%d",
                                  DumpFilename, (int)FileSize, (int)NumEntries);

                /* Increment Successful Command Counter */
                CFE_ES_TaskData.CmdCounter++;      
            }
            else
            {
                CFE_EVS_SendEvent(CFE_ES_CDS_DUMP_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Error writing CDS Registry to '%s', Status=0x%08X",
                                  DumpFilename, (unsigned int)Status);
                            
                /* Increment Command Error Counter */      
                CFE_ES_TaskData.ErrCounter++;      
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_ES_WRITE_CFE_HDR_ERR_EID,
                              CFE_EVS_ERROR,
                              "Error writing cFE File Header to '%s', Status=0x%08X",
                              DumpFilename, (unsigned int)Status);
                            
            /* Increment Command Error Counter */      
            CFE_ES_TaskData.ErrCounter++;      
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_CREATING_CDS_DUMP_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating CDS dump file '%s', Status=0x%08X",
                          DumpFilename, (unsigned int)FileDescriptor);
                            
        /* Increment Command Error Counter */      
        CFE_ES_TaskData.ErrCounter++;      
    }
    
} /* End of CFE_ES_DumpCDSRegCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_ES_FileWriteByteCntErr() -- Send event to inform ground that*/
/*                                a byte count discrepancy has been*/
/*                                detected during the file write   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual)
{

    CFE_EVS_SendEvent(CFE_ES_FILEWRITE_ERR_EID,CFE_EVS_ERROR,
                      "File write,byte cnt err,file %s,request=%d,actual=%d",
                       Filename,(int)Requested,(int)Actual);


}/* End of CFE_ES_FileWriteByteCntErr() */                                                

/************************/
/*  End of File Comment */
/************************/


