/************************************************************************
** File:
**   $Id: hs_app.c 1.7 2016/09/07 18:49:19EDT mdeschu Exp  $
** 
**   Copyright � 2007-2016 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   The CFS Health and Safety (HS) provides several utilities including
**   application monitoring, event monitoring, cpu utilization monitoring,
**   aliveness indication, and watchdog servicing.
**
**   $Log: hs_app.c  $
**   Revision 1.7 2016/09/07 18:49:19EDT mdeschu 
**   All CFE_EVS_SendEvents with format warning arguments were explicitly cast
**   Revision 1.6 2016/08/29 23:40:08EDT mdeschu 
**   Checkin missing change to hs_app.c from babelfish ticket #26
**   Revision 1.5 2016/08/16 13:26:58EDT mdeschu 
**   Remove initialization of Status in HS_AppInit as the value is never used
**   Revision 1.4 2016/08/05 09:43:39EDT mdeschu 
**   Ticket #26: Fix HS build errors with strict settings
**   
**       Fix minor issues causing build to fail:
**   
**           Extra argument in CFE_SendEvent() call
**           Unused variable in HS_CustomCleanup()
**   Revision 1.3 2016/05/16 17:28:39EDT czogby 
**   Move function prototype from hs_app.c file to hs_app.h file
**   Revision 1.2 2015/11/12 14:25:27EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.22 2015/05/04 11:59:10EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.21 2015/05/04 10:59:55EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.20 2015/05/01 16:48:31EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.19 2015/03/03 12:16:03EST sstrege 
**   Added copyright information
**   Revision 1.18 2011/10/13 18:47:00EDT aschoeni 
**   updated for hs utilization calibration changes
**   Revision 1.17 2011/08/15 18:46:04EDT aschoeni 
**   HS Unsubscibes when eventmon is disabled
**   Revision 1.16 2010/11/23 16:05:28EST aschoeni 
**   Fixed CALLS_PER_MARK and CYCLES_PER_INTERVAL issue
**   Revision 1.15 2010/11/19 17:58:26EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.14 2010/10/14 17:45:28EDT aschoeni 
**   Removed assumptions of rate of utilization measurement
**   Revision 1.13 2010/10/01 15:18:40EDT aschoeni 
**   Added Telemetry point to track message actions
**   Revision 1.12 2010/09/29 18:26:48EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.11 2010/05/25 18:55:34EDT aschoeni 
**   Updated to increase message limit from event msgid on event pipe
**   Revision 1.10 2010/05/25 16:23:40EDT aschoeni 
**   Removed out of date watchdog comment
**   Revision 1.9 2009/08/20 16:02:18EDT aschoeni 
**   Updated Watchdog API to match current design
**   Revision 1.8 2009/06/12 15:15:28EDT rmcgraw 
**   DCR8291:1 Put back API changes after tag (OS_BSP* to CFE_PSP_*)
**   Revision 1.7 2009/06/11 15:36:27EDT rmcgraw 
**   DCR8291:1 Revert to be compatible with cFE5.2 for tag
**   Revision 1.6 2009/06/10 14:08:39EDT rmcgraw 
**   DCR82191:1 Changed OS_BSP* function calls to CFE_PSP_*
**   Revision 1.5 2009/06/02 16:38:45EDT aschoeni 
**   Updated telemetry and internal status to support HS Internal Status bit flags
**   Revision 1.4 2009/05/22 17:40:32EDT aschoeni 
**   Updated CDS related events
**   Revision 1.3 2009/05/21 14:48:40EDT aschoeni 
**   Added casting of inverted data for CDS validation check
**   Revision 1.2 2009/05/04 17:44:28EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:24EDT aschoeni 
**   Initial revision
**   Member added to CFS project
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "hs_app.h"
#include "hs_events.h"
#include "hs_msgids.h"
#include "hs_perfids.h"
#include "hs_monitors.h"
#include "hs_custom.h"
#include "hs_version.h"
#include "hs_cmds.h"
#include "hs_verify.h"

/************************************************************************
** Macro Definitions
*************************************************************************/

/************************************************************************
** HS global data
*************************************************************************/
HS_AppData_t     HS_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_AppMain(void)
{
    int32   Status      = CFE_SUCCESS;
    uint32  RunStatus   = CFE_ES_APP_RUN;

    /*
    ** Performance Log, Start
    */
    CFE_ES_PerfLogEntry(HS_APPMAIN_PERF_ID);

    /*
    ** Register this application with Executive Services
    */
    Status = CFE_ES_RegisterApp();

    /*
    ** Perform application specific initialization
    */
    if (Status == CFE_SUCCESS)
    {
        Status = HS_AppInit();
    }

    /*
    ** If no errors were detected during initialization, then wait for everyone to start
    */
    if (Status == CFE_SUCCESS)
    {
       CFE_ES_WaitForStartupSync(HS_STARTUP_SYNC_TIMEOUT);

       /*
       ** Enable and set the watchdog timer
       */
       CFE_PSP_WatchdogSet(HS_WATCHDOG_TIMEOUT_VALUE);
       CFE_PSP_WatchdogService();
       CFE_PSP_WatchdogEnable();
       CFE_PSP_WatchdogService();

       /*
       ** Subscribe to Event Messages
       */
       if (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED)
       {
          Status = CFE_SB_SubscribeEx(CFE_EVS_EVENT_MSG_MID,
                                      HS_AppData.EventPipe,
                                      CFE_SB_Default_Qos,
                                      HS_EVENT_PIPE_DEPTH);
          if (Status != CFE_SUCCESS)
          {
             CFE_EVS_SendEvent(HS_SUB_EVS_ERR_EID, CFE_EVS_ERROR,
                 "Error Subscribing to Events,RC=0x%08X",(unsigned int)Status);
          }
       }
    }

    if (Status != CFE_SUCCESS)
    {
       /*
       ** Set run status to terminate main loop
       */
       RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Application main loop
    */
    while(CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        /*
        ** Performance Log, Stop
        */
        CFE_ES_PerfLogExit(HS_APPMAIN_PERF_ID);

        /*
        ** Task Delay for a configured timeout
        */
#if HS_POST_PROCESSING_DELAY != 0
        OS_TaskDelay(HS_POST_PROCESSING_DELAY);
#endif

        /*
        ** Task Delay for a configured timeout
        */
        Status = CFE_SB_RcvMsg(&HS_AppData.MsgPtr, HS_AppData.WakeupPipe, HS_WAKEUP_TIMEOUT);

        /*
        ** Performance Log, Start
        */
        CFE_ES_PerfLogEntry(HS_APPMAIN_PERF_ID);

        /*
        ** Process the software bus message
        */
        if ((Status == CFE_SUCCESS) ||
            (Status == CFE_SB_NO_MESSAGE) ||
            (Status == CFE_SB_TIME_OUT))
        {
            Status = HS_ProcessMain();
        }

        /*
        ** Note: If there were some reason to exit the task
        **       normally (without error) then we would set
        **       RunStatus = CFE_ES_APP_EXIT
        */
        if (Status != CFE_SUCCESS)
        {
            /*
            ** Set request to terminate main loop
            */
            RunStatus = CFE_ES_APP_ERROR;
        }

    } /* end CFS_ES_RunLoop while */

    /*
    ** Check for "fatal" process error...
    */
    if (Status != CFE_SUCCESS)
    {
        /*
        ** Send an event describing the reason for the termination
        */
        CFE_EVS_SendEvent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL,
                          "Application Terminating, err = 0x%08X", (unsigned int)Status);

        /*
        ** In case cFE Event Services is not working
        */
        CFE_ES_WriteToSysLog("HS App: Application Terminating, ERR = 0x%08X\n", (unsigned int)Status);
    }

    HS_CustomCleanup();

    /*
    ** Performance Log, Stop
    */
    CFE_ES_PerfLogExit(HS_APPMAIN_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(RunStatus);

} /* end HS_AppMain */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HS initialization                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_AppInit(void)
{
    int32       Status;

    /* 
    ** Initialize operating data to default states...
    */
    HS_AppData.ServiceWatchdogFlag = HS_STATE_ENABLED;
    HS_AppData.AlivenessCounter = 0;
    HS_AppData.RunStatus = CFE_ES_APP_RUN;
    HS_AppData.EventsMonitoredCount = 0;
    HS_AppData.MsgActExec = 0;

    HS_AppData.CurrentAppMonState = HS_APPMON_DEFAULT_STATE;
    HS_AppData.CurrentEventMonState = HS_EVENTMON_DEFAULT_STATE;
    HS_AppData.CurrentAlivenessState = HS_ALIVENESS_DEFAULT_STATE;
    HS_AppData.CurrentCPUHogState = HS_CPUHOG_DEFAULT_STATE;

#if HS_MAX_EXEC_CNT_SLOTS != 0
    HS_AppData.ExeCountState = HS_STATE_ENABLED;
#else
    HS_AppData.ExeCountState = HS_STATE_DISABLED;
#endif

    HS_AppData.MsgActsState = HS_STATE_ENABLED;
    HS_AppData.AppMonLoaded = HS_STATE_ENABLED;
    HS_AppData.EventMonLoaded = HS_STATE_ENABLED;
    HS_AppData.CDSState = HS_STATE_ENABLED;

    HS_AppData.CurrentCPUUtilIndex = 0;
    HS_AppData.CurrentCPUHoggingTime = 0;
    HS_AppData.MaxCPUHoggingTime = HS_UTIL_HOGGING_TIMEOUT;
    
    HS_AppData.UtilCpuAvg = 0;
    HS_AppData.UtilCpuPeak = 0;

    /* 
    ** Register for event services...
    */
    Status = CFE_EVS_Register (NULL, 0, CFE_EVS_BINARY_FILTER);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("HS App: Error Registering For Event Services, RC = 0x%08X\n", (unsigned int)Status);
        return (Status);
    }


    /* 
    ** Create Critical Data Store
    */
    Status = CFE_ES_RegisterCDS(&HS_AppData.MyCDSHandle, 
                                 sizeof(HS_CDSData_t), 
                                 HS_CDSNAME);
                            
    if (Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* 
        ** Critical Data Store already existed, we need to get a 
        ** copy of its current contents to see if we can use it
        */
        Status = CFE_ES_RestoreFromCDS(&HS_AppData.CDSData, HS_AppData.MyCDSHandle);
        
        if (Status == CFE_SUCCESS)
        {
            if((HS_AppData.CDSData.ResetsPerformed != (uint16) ~HS_AppData.CDSData.ResetsPerformedNot) ||
               (HS_AppData.CDSData.MaxResets != (uint16) ~HS_AppData.CDSData.MaxResetsNot))
            {
                /* 
                ** Report error restoring data
                */
                CFE_EVS_SendEvent(HS_CDS_CORRUPT_ERR_EID, CFE_EVS_ERROR, 
                                  "Data in CDS was corrupt, initializing resets data");
                /* 
                ** If data was corrupt, initialize data
                */
                HS_SetCDSData(0, HS_MAX_RESTART_ACTIONS);
            }

        }
        else
        {
            /* 
            ** Report error restoring data
            */
            CFE_EVS_SendEvent(HS_CDS_RESTORE_ERR_EID, CFE_EVS_ERROR, 
                              "Failed to restore data from CDS (Err=0x%08x), initializing resets data", (unsigned int)Status);
            /* 
            ** If data could not be retrieved, initialize data
            */
            HS_SetCDSData(0, HS_MAX_RESTART_ACTIONS);
        }

        Status = CFE_SUCCESS;

    } 
    else if (Status == CFE_SUCCESS)
    {
        /* 
        ** If CDS did not previously exist, initialize data
        */
        HS_SetCDSData(0, HS_MAX_RESTART_ACTIONS);
    }
    else
    {
        /* 
        ** Disable saving to CDS
        */
        HS_AppData.CDSState = HS_STATE_DISABLED;

        /* 
        ** Initialize values anyway (they will not be saved)
        */
        HS_SetCDSData(0, HS_MAX_RESTART_ACTIONS);
    }

    /* 
    ** Set up the HS Software Bus
    */
    Status = HS_SbInit();
    if(Status != CFE_SUCCESS)
    {
        return (Status);
    }

    /*
    ** Register The HS Tables
    */
    Status = HS_TblInit();
    if(Status != CFE_SUCCESS)
    {
        return (Status);
    }

    /*
    ** Perform custom initialization (for cpu utilization monitoring)
    */
    Status = HS_CustomInit();

    /*
    ** Application initialization event
    */
    CFE_EVS_SendEvent (HS_INIT_EID, CFE_EVS_INFORMATION,
               "HS Initialized.  Version %d.%d.%d.%d",
                HS_MAJOR_VERSION,
                HS_MINOR_VERSION,
                HS_REVISION,
                HS_MISSION_REV);

    return CFE_SUCCESS;

} /* end HS_AppInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize the software bus interface                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_SbInit(void)
{
    int32 Status = CFE_SUCCESS;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) NULL;
    HS_AppData.CmdPipe = 0;
    HS_AppData.EventPipe = 0;
    HS_AppData.WakeupPipe = 0;

    /* Initialize housekeeping packet  */
    CFE_SB_InitMsg(&HS_AppData.HkPacket,HS_HK_TLM_MID,sizeof(HS_HkPacket_t),TRUE);

    /* Create Command Pipe */
    Status = CFE_SB_CreatePipe (&HS_AppData.CmdPipe,HS_CMD_PIPE_DEPTH,HS_CMD_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_CR_CMD_PIPE_ERR_EID, CFE_EVS_ERROR,
              "Error Creating SB Command Pipe,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Create Event Pipe */
    Status = CFE_SB_CreatePipe (&HS_AppData.EventPipe,HS_EVENT_PIPE_DEPTH,HS_EVENT_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_CR_EVENT_PIPE_ERR_EID, CFE_EVS_ERROR,
              "Error Creating SB Event Pipe,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Create Wakeup Pipe */
    Status = CFE_SB_CreatePipe (&HS_AppData.WakeupPipe,HS_WAKEUP_PIPE_DEPTH,HS_WAKEUP_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_CR_WAKEUP_PIPE_ERR_EID, CFE_EVS_ERROR,
              "Error Creating SB Wakeup Pipe,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Subscribe to Housekeeping Request */
    Status = CFE_SB_Subscribe(HS_SEND_HK_MID,HS_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_SUB_REQ_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Request,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Subscribe to HS ground commands */
    Status = CFE_SB_Subscribe(HS_CMD_MID,HS_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_SUB_CMD_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to Gnd Cmds,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Subscribe to HS Wakeup Message */
    Status = CFE_SB_Subscribe(HS_WAKEUP_MID,HS_AppData.WakeupPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_SUB_WAKEUP_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to Wakeup,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /*
    ** Event message subscription delayed until after startup synch
    */

    return(Status);

} /* End of HS_SbInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize the table interface                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_TblInit(void)
{
    uint32      TableSize;
    int32       Status = CFE_SUCCESS;

    /* Register The HS Applications Monitor Table */
    TableSize = HS_MAX_MONITORED_APPS * sizeof (HS_AMTEntry_t);
    Status = CFE_TBL_Register (&HS_AppData.AMTableHandle,
                                HS_AMT_TABLENAME,
                                TableSize,
                                CFE_TBL_OPT_DEFAULT,
                                HS_ValidateAMTable);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_AMT_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering AppMon Table,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Register The HS Events Monitor Table */
    TableSize = HS_MAX_MONITORED_EVENTS * sizeof (HS_EMTEntry_t);
    Status = CFE_TBL_Register (&HS_AppData.EMTableHandle,
                                HS_EMT_TABLENAME,
                                TableSize,
                                CFE_TBL_OPT_DEFAULT,
                                HS_ValidateEMTable);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_EMT_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering EventMon Table,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Register The HS Message Actions Table */
    TableSize = HS_MAX_MSG_ACT_TYPES * sizeof (HS_MATEntry_t);
    Status = CFE_TBL_Register (&HS_AppData.MATableHandle,
                                HS_MAT_TABLENAME,
                                TableSize,
                                CFE_TBL_OPT_DEFAULT,
                                HS_ValidateMATable);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_MAT_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering MsgActs Table,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

#if HS_MAX_EXEC_CNT_SLOTS != 0
    /* Register The HS Execution Counters Table */
    TableSize = HS_MAX_EXEC_CNT_SLOTS * sizeof (HS_XCTEntry_t);
    Status = CFE_TBL_Register (&HS_AppData.XCTableHandle,
                                HS_XCT_TABLENAME,
                                TableSize,
                                CFE_TBL_OPT_DEFAULT,
                                HS_ValidateXCTable);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_XCT_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering ExeCount Table,RC=0x%08X",(unsigned int)Status);
        return (Status);
    }

    /* Load the HS Execution Counters Table */
    Status = CFE_TBL_Load (HS_AppData.XCTableHandle,
                           CFE_TBL_SRC_FILE,
                           (const void *) HS_XCT_FILENAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_XCT_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading ExeCount Table,RC=0x%08X",(unsigned int)Status);
        HS_AppData.ExeCountState = HS_STATE_DISABLED;
    }
#endif

    /* Load the HS Applications Monitor Table */
    Status = CFE_TBL_Load (HS_AppData.AMTableHandle,
                           CFE_TBL_SRC_FILE,
                           (const void *) HS_AMT_FILENAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_AMT_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading AppMon Table,RC=0x%08X",(unsigned int)Status);
        HS_AppData.CurrentAppMonState = HS_STATE_DISABLED;
        CFE_EVS_SendEvent (HS_DISABLE_APPMON_ERR_EID, CFE_EVS_ERROR,
                           "Application Monitoring Disabled due to Table Load Failure");
        HS_AppData.AppMonLoaded = HS_STATE_DISABLED;
    }

    /* Load the HS Events Monitor Table */
    Status = CFE_TBL_Load (HS_AppData.EMTableHandle,
                           CFE_TBL_SRC_FILE,
                           (const void *) HS_EMT_FILENAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_EMT_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading EventMon Table,RC=0x%08X",(unsigned int)Status);
        HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;
        CFE_EVS_SendEvent (HS_DISABLE_EVENTMON_ERR_EID, CFE_EVS_ERROR,
                           "Event Monitoring Disabled due to Table Load Failure");
        HS_AppData.EventMonLoaded = HS_STATE_DISABLED;
    }

    /* Load the HS Message Actions Table */
    Status = CFE_TBL_Load (HS_AppData.MATableHandle,
                           CFE_TBL_SRC_FILE,
                           (const void *) HS_MAT_FILENAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HS_MAT_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading MsgActs Table,RC=0x%08X",(unsigned int)Status);
        HS_AppData.MsgActsState = HS_STATE_DISABLED;
    }

    /*
    ** Get pointers to table data
    */
    HS_AcquirePointers();

    return CFE_SUCCESS;

} /* End of HS_TblInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Main Processing Loop                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ProcessMain(void)
{
    int32 Status = CFE_SUCCESS;
    char *AliveString = HS_CPU_ALIVE_STRING;
    uint32 i;

    /* 
    ** Get Tables
    */
    HS_AcquirePointers();

    /* 
    ** Decrement Cooldowns for Message Actions
    */
    for(i = 0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        if(HS_AppData.MsgActCooldown[i] != 0)
        {
            HS_AppData.MsgActCooldown[i]--;
        }
    }

    /* 
    ** Monitor Applications
    */
    if (HS_AppData.CurrentAppMonState == HS_STATE_ENABLED)
    {
        HS_MonitorApplications();
    }

    /* 
    ** Monitor CPU Utilization
    */
    HS_CustomMonitorUtilization();

    /* 
    ** Output Aliveness
    */
    if (HS_AppData.CurrentAlivenessState == HS_STATE_ENABLED)
    {
        HS_AppData.AlivenessCounter++;

        if (HS_AppData.AlivenessCounter >= HS_CPU_ALIVE_PERIOD)
        {
            OS_printf("%s", AliveString);
            HS_AppData.AlivenessCounter = 0;
        }

    }

    /* 
    ** Check for Commands, Events, and HK Requests
    */
    Status = HS_ProcessCommands();

    /* 
    ** Service the Watchdog
    */
    if (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED)
    {
        CFE_PSP_WatchdogService();
    }

    return(Status);

} /* End of HS_ProcessMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process any Commands and Event Messages received this cycle     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ProcessCommands(void)
{
    int32 Status = CFE_SUCCESS;

    /*
    ** Event Message Pipe (done first so EventMon does not get enabled without table checking)
    */
    if (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED)
    {
        while (Status == CFE_SUCCESS)
        {
            Status = CFE_SB_RcvMsg(&HS_AppData.MsgPtr, HS_AppData.EventPipe, CFE_SB_POLL);

            if (Status == CFE_SUCCESS)
            {
                /*
                ** Pass Events to Event Monitor
                */
                HS_AppData.EventsMonitoredCount++;
                HS_MonitorEvent(HS_AppData.MsgPtr);
            }
        }
    }

    if (Status == CFE_SB_NO_MESSAGE)
    {
        /*
        ** It's Good to not get a message -- we are polling
        */
        Status = CFE_SUCCESS;
    }

    /*
    ** Command and HK Requests Pipe
    */
    while (Status == CFE_SUCCESS)
    {
        /*
        ** Process pending Commands or HK Reqs
        */
        Status = CFE_SB_RcvMsg(&HS_AppData.MsgPtr, HS_AppData.CmdPipe, CFE_SB_POLL);

        if (Status == CFE_SUCCESS)
        {
            /*
            ** Pass Commands/HK Req to AppPipe Processing
            */
            HS_AppPipe(HS_AppData.MsgPtr);
        }
    }

    if (Status == CFE_SB_NO_MESSAGE)
    {
        /*
        ** It's Good to not get a message -- we are polling
        */
        Status = CFE_SUCCESS;
    }

    return(Status);

} /* End of HS_ProcessCommands() */

/************************/
/*  End of File Comment */
/************************/
