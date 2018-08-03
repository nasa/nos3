/*
**
**  File Name: cfe_evs_task.c
**  $Id: cfe_evs_task.c 1.19 2014/08/22 16:53:24GMT-05:00 lwalling Exp  $
**
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
**
**
**
**  Title: Event Service API Management Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           cFE Event Service task defining the control, command,
**           and telemetry interfaces
**
**  $Date: 2014/08/22 16:53:24GMT-05:00 $
**  $Revision: 1.19 $
**  $Log: cfe_evs_task.c  $
**  Revision 1.19 2014/08/22 16:53:24GMT-05:00 lwalling 
**  Change signed loop counters to unsigned
**  Revision 1.18 2012/10/01 17:37:11EDT aschoeni 
**  removed relative path in include (handle in makefile)
**  Revision 1.17 2012/01/18 16:28:03EST jmdagost 
**  Update init and no-op event msgs to include OSAL revision and mission revision numbers.
**  --- Added comments ---  jmdagost [2012/01/18 21:31:18Z]
**  Updated init and no-op event msgs to include cFE version numbers.
**  Revision 1.16 2012/01/13 12:00:55EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.15 2011/09/30 14:52:08EDT lwalling 
**  Fix use of config file setting for default event log mode
**  Revision 1.14 2011/09/28 13:18:12EDT lwalling 
**  Verify reset area size is at least as big as CFE_ES_ResetData_t
**  Revision 1.13 2011/06/02 18:08:56EDT lwalling 
**  Modified error handling for Write App Data to File command handler, updated event text
**  Revision 1.12 2011/06/01 10:36:39EDT lwalling 
**  Include internal ES header file, fix typo in calls to CFE_ES_WriteToSysLog()
**  Revision 1.11 2011/05/23 15:57:01EDT lwalling 
**  Change startup to not fail if unable to create event log, verify reset area size and event log contents
**  Revision 1.10 2011/04/07 11:24:26EDT lwalling 
**  Fix typo in data definition
**  Revision 1.9 2011/04/07 11:00:39EDT lwalling 
**  Restored deleted app cleanup function -- CFE_EVS_CleanUpApp()
**  Revision 1.8 2011/04/05 16:33:44EDT lwalling 
**  Optimize EVS use of string functions, plus other performance improvements
**  Revision 1.7 2011/03/17 15:55:39EDT lwalling 
**  Fix use of return value from function CFE_ES_GetResetType()
**  Revision 1.6 2010/10/27 16:37:17EDT jmdagost 
**  Forced LogMode to default value on reset, added platform config and evs msg include files.
**  Revision 1.5 2010/09/21 16:12:13EDT jmdagost 
**  Added conditional statements around conditionally-used local variables.
**  Revision 1.4 2009/07/31 19:54:57EDT aschoeni 
**  Added length checking and fixed several comments and events
**  Revision 1.3 2009/06/10 09:13:01EDT acudmore 
**  Converted OS_Mem* and OS_BSP* calls to CFE_PSP_*
**  Revision 1.2 2008/07/31 15:41:38EDT apcudmore 
**  Added execution counter API:
**    -- Added execution counter to ES internal task data
**    -- Added code to increment counter in RunLoop
**    -- Added code to report counter in GetInfo APIs
**    -- Added API to increment counter for child tasks
**    -- Added code to cFE Core apps to increment counters.
**  Revision 1.1 2008/04/17 08:05:13EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
**  Revision 1.1.2.72 2007/08/27 12:01:13EDT njyanchik 
**  I mustv'e accidentally overwritten DAve's change of adding an event ID to WriteAppData, so I
**  am fixing the mistake
**  Revision 1.1.2.71 2007/08/24 14:27:38EDT apcudmore 
**  Keep EVS from clearing out the log on a Processor reset
**  Revision 1.1.2.70 2007/07/18 15:48:00EDT njyanchik 
**  Jonathan added text into each error event in the two functions that write files (WriteLog and WriteAppData), as well as updated the doxygen comments in the header file
**  Revision 1.1.2.68 2007/06/05 15:37:36EDT njyanchik 
**  The log mode was not being set correctly in the telemetry. It was because the logmode variable
**  was assigned to the logfullflag variable in telemetry. I reassigned it to the appropriate variable.
**  Revision 1.1.2.67 2007/05/16 15:13:42EDT dlkobe 
**  Added MemPoolHandle members to HK packet
**  Revision 1.1.2.66 2007/05/16 08:44:36EDT njyanchik 
**  I re added the AppData Subtype to the AppData File header
**  Revision 1.1.2.65 2007/05/15 11:01:10EDT rjmcgraw 
**  DCR78:11 Exit main loop for CFE_SB_RcvMsg errors
**  Revision 1.1.2.64 2007/05/14 10:26:04EDT apcudmore 
**  Preserve the EVS Local event log on a processor restart:
**  - moved the EVS Log to the ES reset data structure
**  - Added a couple of EVS Log related variables into the EVS_Log_t structure
**  - Added a routine to the EVS early init to get a pointer to the EVS Log 
**  - Added code to the EVS housekeeping service routine to copy Log variables into TLM
**  Revision 1.1.2.63 2007/04/27 16:58:19EDT rjmcgraw 
**  Checked return from init event
**  Revision 1.1.2.62 2007/04/27 16:13:15EDT rjmcgraw 
**  DCR2987:4 Standardize task init error code
**  Revision 1.1.2.61 2007/04/27 12:31:06EDT njyanchik 
**  I changed EVS and TIME to not have a telemetry counter for SB pipe reads in the  
**  main loop. Instead, I have added a syslog message to state the problem
**  Revision 1.1.2.60 2007/04/26 14:45:39EDT rjmcgraw 
**  Added pool create and sem create error handling
**  Revision 1.1.2.59 2007/04/13 16:23:16EDT apcudmore 
**  Changed EarlyInit function prototype to return int32 status
**  Revision 1.1.2.58 2007/04/11 10:59:56EDT njyanchik 
**  Remove the TaskDelay in the main loop for EVS. I also addeda counterin telemtry 
**  that will notify the ground of an error in reading the command pipe.
**  Revision 1.1.2.57 2007/04/09 14:45:08EDT rjmcgraw 
**  Changed names of performance ids to include CFE_
**
*/

/* Include Files */
#include "cfe_evs_task.h"       /* EVS internal definitions */
#include "cfe_evs_log.h"        /* EVS log file definitions */
#include "cfe_evs_utils.h"      /* EVS utility function definitions */
#include "cfe_evs.h"            /* EVS API definitions */

#include <string.h>

#include "cfe_version.h"      /* cFE version definitions */
#include "cfe_error.h"        /* cFE error code definitions */
#include "cfe_es.h"           /* Executive Service definitions */
#include "cfe_fs.h"           /* File Service definitions */
#include "cfe_psp.h"          /* cFE Platform Support Package definitions */
#include "osapi.h"            /* OS API file system definitions */

#include "private/cfe_es_resetdata_typedef.h"  /* Definition of CFE_ES_ResetData_t */

/* Global Data */
CFE_EVS_GlobalData_t CFE_EVS_GlobalData;


/*
** Local function prototypes.
*/
void  CFE_EVS_ProcessGroundCommand ( CFE_SB_MsgPtr_t EVS_MsgPtr );
void  CFE_EVS_ReportHousekeepingCmd ( void );
void  CFE_EVS_ResetCountersCmd ( void );
boolean CFE_EVS_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

/* Function Definitions */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EarlyInit
**
** Purpose:  This routine provides initialization for the EVS API.
**
** Assumptions and Notes: This routine must be called before the EVS
**      application is started.  CFE_EVS_EarlyInit performs initialization
**      necessary to support EVS API calls that might occur before
**      the EVS application has completed its startup initialization.
*/
int32 CFE_EVS_EarlyInit ( void )
{

#ifdef CFE_EVS_LOG_ON

   int32                Status;
   uint32               resetAreaSize = 0;
   cpuaddr              resetAreaAddr;
   CFE_ES_ResetData_t  *CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *) NULL;

#endif

   CFE_PSP_MemSet(&CFE_EVS_GlobalData, 0, sizeof(CFE_EVS_GlobalData_t));

   CFE_EVS_GlobalData.EVS_AppID = CFE_EVS_UNDEF_APPID;

   /* Initialize housekeeping packet */
   CFE_SB_InitMsg(&CFE_EVS_GlobalData.EVS_TlmPkt, CFE_EVS_HK_TLM_MID,
                   CFE_EVS_HK_TLM_LNGTH, FALSE);
  
   /* Elements stored in the hk packet that have non-zero default values */
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_DEFAULT_MSG_FORMAT_MODE;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort = CFE_EVS_PORT_DEFAULT;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogFullFlag = FALSE;
   CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogMode = CFE_EVS_DEFAULT_LOG_MODE;

#ifdef CFE_EVS_LOG_ON

   /* Get a pointer to the CFE reset area from the BSP */
   Status = CFE_PSP_GetResetArea(&resetAreaAddr, &resetAreaSize);

   if (Status != CFE_PSP_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS call to CFE_PSP_GetResetArea failed, RC=0x%08x\n", (unsigned int)Status);
   }
   else if (resetAreaSize < sizeof(CFE_ES_ResetData_t))
   {
      /* Got the pointer but the size is wrong */
      Status = CFE_EVS_RESET_AREA_POINTER;
      CFE_ES_WriteToSysLog("Unexpected size from CFE_PSP_GetResetArea: expected = 0x%08lX, actual = 0x%08lX\n",
                            (unsigned long)sizeof(CFE_ES_ResetData_t), (unsigned long)resetAreaSize);
   }
   else
   {
      CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)resetAreaAddr;
      /* Save pointer to the EVS portion of the CFE reset area */
      CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

      /* Create semaphore to serialize access to event log */
      Status = OS_MutSemCreate(&CFE_EVS_GlobalData.EVS_SharedDataMutexID, "CFE_EVS_DataMutex", 0);

      if (Status != OS_SUCCESS)
      {
         CFE_ES_WriteToSysLog("EVS call to OS_MutSemCreate failed, RC=0x%08x\n", (unsigned int)Status);
      }
      else
      {
         /* Enable access to the EVS event log */                                                            
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = TRUE;

         /* Clear event log if power-on reset or bad contents */                                                            
         if (CFE_ES_GetResetType(NULL) == CFE_PSP_RST_TYPE_POWERON)                                                                   
         {
            CFE_ES_WriteToSysLog("Event Log cleared following power-on reset\n");
            EVS_ClearLog();                                                                                         
            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_DEFAULT_LOG_MODE;
         }
         else if (((CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LOG_OVERWRITE) &&
                   (CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LOG_DISCARD))  ||
                  ((CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != FALSE)   &&
                   (CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != TRUE))   ||
                   (CFE_EVS_GlobalData.EVS_LogPtr->Next >= CFE_EVS_LOG_MAX))
         {
            CFE_ES_WriteToSysLog("Event Log cleared, n=%d, c=%d, f=%d, m=%d, o=%d\n",
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->Next,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogMode,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter);
            EVS_ClearLog();                                                                                         
            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_DEFAULT_LOG_MODE;
         }
         else
         {
            CFE_ES_WriteToSysLog("Event Log restored, n=%d, c=%d, f=%d, m=%d, o=%d\n",
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->Next,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogMode,
                                  (int)CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter);
         }
      }
   }

#endif

   return(CFE_SUCCESS);

} /* End CFE_EVS_EarlyInit */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_CleanUpApp
**
** Purpose:  ES calls this routine when an app is being terminated.
**
** Assumptions and Notes:
*/
int32 CFE_EVS_CleanUpApp(uint32 AppID)
{
   int32  Status = CFE_SUCCESS;

   if (AppID >= CFE_ES_MAX_APPLICATIONS)
   {
      Status = CFE_EVS_APP_ILLEGAL_APP_ID;
   }
   else if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == TRUE)
   {
      /* Same cleanup as CFE_EVS_Unregister() */
      CFE_PSP_MemSet(&CFE_EVS_GlobalData.AppData[AppID], 0, sizeof(EVS_AppData_t));
   }
    
   return(Status);
}


/*
**             Function Prologue
**
** Function Name:      EVS_TaskMain
**
** Purpose:  This is the main EVS task process loop.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_TaskMain(void)
{
    int32 Status;    
    CFE_SB_MsgPtr_t    EVS_MsgPtr; /* Pointer to SB message */

    CFE_ES_PerfLogEntry(CFE_EVS_MAIN_PERF_ID);    
   
    Status = CFE_EVS_TaskInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("EVS:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_EVS_MAIN_PERF_ID);
      /* Note: CFE_ES_ExitApp will not return */
      CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR);
    }/* end if */      

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForStartupSync(CFE_CORE_MAX_STARTUP_MSEC);

    /* Main loop */
    while (Status == CFE_SUCCESS)
    {
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();
    
        CFE_ES_PerfLogExit(CFE_EVS_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_RcvMsg(&EVS_MsgPtr, 
                               CFE_EVS_GlobalData.EVS_CommandPipe, 
                               CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_EVS_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_EVS_ProcessCommandPacket(EVS_MsgPtr);
        }else{            
            CFE_ES_WriteToSysLog("EVS:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */
    
    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);

} /* end CFE_EVS_TaskMain */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_TaskInit
**
** Purpose:  This function performs any necessary EVS task initialization.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_TaskInit ( void )
{
   int32 Status;
   uint32 AppID;
 
   /* Register EVS application */
   Status = CFE_ES_RegisterApp();
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }

   /* Query and verify the EVS AppID */
   Status = EVS_GetAppID(&AppID);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_ES_GetAppID Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }

   /* Register EVS task for event services */
   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
    
   /* Create software bus command pipe */
   Status = CFE_SB_CreatePipe(&CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_EVS_PIPE_DEPTH, CFE_EVS_PIPE_NAME);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
      
   /* Subscribe to command and telemetry requests coming in on the command pipe */
   Status = CFE_SB_SubscribeEx(CFE_EVS_CMD_MID, CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_SB_Default_Qos, CFE_EVS_MSG_LIMIT);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Subscribing to Cmds Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
  
   Status = CFE_SB_SubscribeEx(CFE_EVS_SEND_HK_MID, CFE_EVS_GlobalData.EVS_CommandPipe,
                               CFE_SB_Default_Qos, CFE_EVS_MSG_LIMIT);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("EVS:Subscribing to HK Request Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
   }
  
   /* Write the AppID to the global location, now that the rest of initialization is done */
   CFE_EVS_GlobalData.EVS_AppID = AppID;
   EVS_SendEvent(CFE_EVS_STARTUP_EID, CFE_EVS_INFORMATION, "cFE EVS Initialized. cFE Version %d.%d.%d.%d",
                 CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

   return CFE_SUCCESS;

} /* End CFE_EVS_TaskInit */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ProcessCommandPacket
**
** Purpose:  This function processes packets received on the EVS command pipe.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ProcessCommandPacket ( CFE_SB_MsgPtr_t EVS_MsgPtr )
{
    /* Process all SB messages */
    switch (CFE_SB_GetMsgId(EVS_MsgPtr))
    {
        case CFE_EVS_CMD_MID:
            /* EVS task specific command */
            CFE_EVS_ProcessGroundCommand(EVS_MsgPtr);
            break;

        case CFE_EVS_SEND_HK_MID:
            /* Housekeeping request */
            CFE_EVS_ReportHousekeepingCmd();
            break;

        default:
            /* Unknown command -- should never occur */
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrCounter++;
            EVS_SendEvent(CFE_EVS_ERR_MSGID_EID, CFE_EVS_ERROR,
                         "Invalid command packet, Message ID = 0x%08X",
                          (unsigned int)CFE_SB_GetMsgId(EVS_MsgPtr));
            break;
    }

    return;

} /* End CFE_EVS_ProcessCommandPacket */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ProcessGroundCommand
**
** Purpose:  This function processes a command, verifying that it is valid and of
**           proper length.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ProcessGroundCommand ( CFE_SB_MsgPtr_t EVS_MsgPtr )
{
   boolean Valid = FALSE;
   CFE_SB_MsgPayloadPtr_t Payload;

   Payload = &EVS_MsgPtr->Byte[CFE_SB_CMD_HDR_SIZE];
   /* Process "known" EVS task ground commands */
   switch (CFE_SB_GetCmdCode(EVS_MsgPtr))
   {
      case CFE_EVS_NO_OPERATION_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_SB_CmdHdr_t))) == TRUE)
         {
            EVS_SendEvent(CFE_EVS_NOOP_EID, CFE_EVS_INFORMATION,"No-op command. cFE Version %d.%d.%d.%d",
                          CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);
         }
         break;

      case CFE_EVS_RESET_COUNTERS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_SB_CmdHdr_t))) == TRUE)
         {
            CFE_EVS_ResetCountersCmd();
            /* Return avoids incrementing command counter */
            return;
         }
         break;

      case CFE_EVS_ENABLE_EVENT_TYPE_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_BitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_EnableEventTypesCmd(Payload);
         }
         break;

      case CFE_EVS_DISABLE_EVENT_TYPE_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_BitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_DisableEventTypesCmd(Payload);
         }
         break;

      case CFE_EVS_SET_EVENT_FORMAT_MODE_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ModeCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_SetEventFormatModeCmd(Payload);
         }
         break;

      case CFE_EVS_ENABLE_APP_EVENT_TYPE_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameBitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_EnableAppEventTypesCmd(Payload);
         }
         break;

      case CFE_EVS_DISABLE_APP_EVENT_TYPE_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameBitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_DisableAppEventTypesCmd(Payload);
         }
         break;

      case CFE_EVS_ENABLE_APP_EVENTS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_EnableAppEventsCmd(Payload);
         }
         break;

      case CFE_EVS_DISABLE_APP_EVENTS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_DisableAppEventsCmd(Payload);
         }
         break;

      case CFE_EVS_RESET_APP_COUNTER_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_ResetAppEventCounterCmd(Payload);
         }
         break;

      case CFE_EVS_SET_FILTER_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, (uint16) sizeof(CFE_EVS_AppNameEventIDMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_SetFilterMaskCmd(Payload);
         }
         break;

      case CFE_EVS_ENABLE_PORTS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_BitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_EnablePortsCmd(Payload);
         }
         break;

      case CFE_EVS_DISABLE_PORTS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_BitMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_DisablePortsCmd(Payload);
         }
         break;

      case CFE_EVS_RESET_FILTER_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameEventIDCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_ResetFilterCmd(Payload);
         }
         break;

      case CFE_EVS_RESET_ALL_FILTERS_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_ResetAllFiltersCmd(Payload);
         }
         break;

      case CFE_EVS_ADD_EVENT_FILTER_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameEventIDMaskCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_AddEventFilterCmd(Payload);
         }
         break;

      case CFE_EVS_DELETE_EVENT_FILTER_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppNameEventIDCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_DeleteEventFilterCmd(Payload);
         }
         break;

      case CFE_EVS_FILE_WRITE_APP_DATA_CC:

         if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_AppDataCmd_t))) == TRUE)
         {
            Valid = CFE_EVS_WriteAppDataCmd(Payload);
         }
         break;

      case CFE_EVS_SET_LOG_MODE_CC:

         if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == TRUE)
         {   
            if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_ModeCmd_t))) == TRUE)
            {
               Valid = CFE_EVS_SetLoggingModeCmd(Payload);
            }
         }
         else
         {
            EVS_SendEvent(CFE_EVS_NO_LOGSET_EID, CFE_EVS_ERROR,
               "Set Log Mode Command: Event Log is Disabled");
         }
         break;

      case CFE_EVS_CLEAR_LOG_CC:

         if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == TRUE)
         {   
            if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_SB_CmdHdr_t))) == TRUE)
            {
               EVS_ClearLog();
            }
         }
         else
         {
            EVS_SendEvent(CFE_EVS_NO_LOGCLR_EID, CFE_EVS_ERROR,
                         "Clear Log Command: Event Log is Disabled");
         }
         break;

      case CFE_EVS_FILE_WRITE_LOG_DATA_CC:

         if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == TRUE)
         {   
            if ((Valid = CFE_EVS_VerifyCmdLength(EVS_MsgPtr, sizeof(CFE_EVS_LogFileCmd_t))) == TRUE)
            {
               Valid = CFE_EVS_WriteLogFileCmd(Payload);
            }
         }
         else
         {
            EVS_SendEvent(CFE_EVS_NO_LOGWR_EID, CFE_EVS_ERROR,
                         "Write Log Command: Event Log is Disabled");
         }
         break;

       /* default is a bad command code as it was not found above */
       default:

          EVS_SendEvent(CFE_EVS_ERR_CC_EID, CFE_EVS_ERROR,
                       "Invalid command code -- ID = 0x%08x, CC = %d",
                        (unsigned int)CFE_SB_GetMsgId(EVS_MsgPtr), (int)CFE_SB_GetCmdCode(EVS_MsgPtr));
          break;
   }

   if (Valid == TRUE)
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandCounter++;
   }
   else
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrCounter++;
   }

   return;

} /* End of EVS_ProcessGroundCommand() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_VerifyCmdLength
**
** Purpose:  This function validates the length of incoming commands.
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
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

        EVS_SendEvent(CFE_EVS_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid cmd length: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
                          (unsigned int)MessageID, (int)CommandCode, (int)ExpectedLength, (int)ActualLength);
        result = FALSE;
    }

    return(result);

} /* End of CFE_EVS_VerifyCmdLength() */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ReportHousekeepingCmd
**
** Purpose:  Request for housekeeping status telemetry packet.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ReportHousekeepingCmd ( void )
{
   uint32 i, j;


   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == TRUE)
   {   
      /* Copy hk variables that are maintained in the event log */
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogFullFlag = CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogMode = CFE_EVS_GlobalData.EVS_LogPtr->LogMode;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogOverflowCounter = CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter;
   }

   /* Write event state data for registered apps to telemetry packet */
   for (i = 0, j = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
   {
      if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == TRUE)
      {
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppID = i;
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppEnableStatus = CFE_EVS_GlobalData.AppData[i].ActiveFlag;
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[j].AppMessageSentCounter = CFE_EVS_GlobalData.AppData[i].EventCount;
         j++;
      }
   }

   /* Clear unused portion of event state data in telemetry packet */
   for (i = j; i < CFE_ES_MAX_APPLICATIONS; i++)
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppID = 0;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppEnableStatus = FALSE;
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.AppData[i].AppMessageSentCounter = 0;
   }

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_EVS_GlobalData.EVS_TlmPkt);

   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_EVS_GlobalData.EVS_TlmPkt);

} /* End of CFE_EVS_ReportHousekeepingCmd() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetCountersCmd
**
** Purpose:  This function resets all the global counter variables that are
**           part of the task telemetry.
**
** Assumptions and Notes:
**
*/
void CFE_EVS_ResetCountersCmd ( void )
{
   /* Status of commands processed by EVS task */
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandCounter  = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.CommandErrCounter   =  0;

   /* EVS telemetry counters */
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter = 0;
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.UnregisteredAppCounter = 0;

   EVS_SendEvent(CFE_EVS_RSTCNT_EID, CFE_EVS_DEBUG, "Reset Counters Command Received");

} /* End of CFE_EVS_ResetCountersCmd() */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetEventFilterMaskCmd
**
** Purpose:  This routine sets the filter mask for the given event_id in the
**           calling task's filter array
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_SetFilterMaskCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *)Payload;
   EVS_BinFilter_t    *FilterPtr;
   uint32              AppID = CFE_EVS_UNDEF_APPID;
   int32               Status;
   EVS_AppData_t      *AppDataPtr;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if (Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         /* Set application filter mask */
         FilterPtr->Mask = CmdPtr->Mask;

         EVS_SendEvent(CFE_EVS_SETFILTERMSK_EID, CFE_EVS_DEBUG,
                           "Set Filter Mask Command Received with AppName=%s, EventID=0x%08x, Mask=0x%04x",
                           LocalName, CmdPtr->EventID, CmdPtr->Mask);

         Status = TRUE;
      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu ",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_SET_FILTER_CC);

         Status = FALSE;
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_SET_FILTER_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End CFE_EVS_SetFilterMaskCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnablePortsCmd
**
** Purpose:  This routine sets the command given ports to an enabled state
**
** Assumptions and Notes:
** Shifting is done so the value not masked off is placed in the ones spot:
** necessary for comparing with TRUE.
*/
boolean CFE_EVS_EnablePortsCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
    const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_BitMaskCmd_Payload_t *)Payload;
    boolean ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu",
                          (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_PORTS_CC);
        ReturnCode = FALSE;  
    }
    else
    {

        /* Process command data */
        if(((CmdPtr->BitMask & CFE_EVS_PORT1_BIT) >> 0) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT1_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT2_BIT) >>1) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT2_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT3_BIT) >> 2) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT3_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT4_BIT) >>3) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort |= CFE_EVS_PORT4_BIT;
        }

        EVS_SendEvent(CFE_EVS_ENAPORT_EID, CFE_EVS_DEBUG,
                        "Enable Ports Command Received with Port Bit Mask = 0x%02x",
                        (unsigned int)CmdPtr->BitMask);
        ReturnCode = TRUE;
    }

    return ReturnCode;

} /* End CFE_EVS_EnablePortsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisablePortsCmd
**
** Purpose:  This routine sets the command given ports to a disabled state
**
** Assumptions and Notes:
** Shifting is done so the value not masked off is placed in the ones spot:
** necessary for comparing with TRUE.
*/
boolean CFE_EVS_DisablePortsCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
    
   const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_BitMaskCmd_Payload_t *)Payload;
    boolean ReturnCode;

    /* Need to check for an out of range bitmask, since oue bit masks are only 4 bits */
    if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
    {
        EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                          "Bit Mask = 0x%08x out of range: CC = %lu",
                          (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_PORTS_CC);
        ReturnCode = FALSE;  
    }
    else
    {

        /* Process command data */
        if(((CmdPtr->BitMask & CFE_EVS_PORT1_BIT) >>0) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT1_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT2_BIT) >> 1) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT2_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT3_BIT) >> 2) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT3_BIT;
        }
        if(((CmdPtr->BitMask & CFE_EVS_PORT4_BIT) >>3) == TRUE)
        {
            CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort &= ~CFE_EVS_PORT4_BIT;
        }

        EVS_SendEvent(CFE_EVS_DISPORT_EID, CFE_EVS_DEBUG,
                         "Disable Ports Command Received with Port Bit Mask = 0x%02x",
                         CmdPtr->BitMask);
        
        ReturnCode = TRUE;
    }
   
    return ReturnCode;

} /* End CFE_EVS_DisablePortsCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableEventTypesCmd
**
** Purpose:  This routine sets the given event types to an enabled state across all
**           registered applications
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_EnableEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32      i;
   const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_BitMaskCmd_Payload_t *)Payload;
   boolean  ReturnCode; 

   /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
   if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
   {
       EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                         "Bit Mask = 0x%08x out of range: CC = %lu",
                         (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_EVENT_TYPE_CC);
    ReturnCode = FALSE;  
   }
   else
   {

        

        for (i = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
        {
            /* Make sure application is registered for event services */
            if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == TRUE)
            {
                EVS_EnableTypes(CmdPtr->BitMask, i);
            }
        }

        EVS_SendEvent(CFE_EVS_ENAEVTTYPE_EID, CFE_EVS_DEBUG,
                        "Enable Event Type Command Received with Event Type Bit Mask = 0x%02x",
                         CmdPtr->BitMask);

       ReturnCode = TRUE;
   }

   return ReturnCode;

} /* End CFE_EVS_EnableEventTypesCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableEventTypesCmd
**
** Purpose:  This routine sets the given event types to a disabled state across all
**           registered applications
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_DisableEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32   i;
   const CFE_EVS_BitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_BitMaskCmd_Payload_t *)Payload;
   boolean  ReturnCode; 

   /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
   if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
   {
       EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                         "Bit Mask = 0x%08x out of range: CC = %lu",
                         (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_EVENT_TYPE_CC);
    ReturnCode = FALSE;  
   }

   else
   {

        for (i = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
        {
            /* Make sure application is registered for event services */
            if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == TRUE)
            {
                EVS_DisableTypes(CmdPtr->BitMask, i);
            }
         }

        EVS_SendEvent(CFE_EVS_DISEVTTYPE_EID, CFE_EVS_DEBUG,
                         "Disable Event Type Command Received with Event Type Bit Mask = 0x%02x",
                         CmdPtr->BitMask);

        ReturnCode = TRUE;
   }

   return ReturnCode;

} /* End CFE_EVS_DisableEventTypesCmd */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetEventFormatModeCmd
**
** Purpose:  This routine sets the Event Format Mode
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_SetEventFormatModeCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_ModeCmd_Payload_t *CmdPtr = (const CFE_EVS_ModeCmd_Payload_t *)Payload;
   boolean Status = TRUE;

   if((CmdPtr->Mode == CFE_EVS_SHORT_FORMAT) || (CmdPtr->Mode == CFE_EVS_LONG_FORMAT))
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode = CmdPtr->Mode;

      EVS_SendEvent(CFE_EVS_SETEVTFMTMOD_EID, CFE_EVS_DEBUG,
                        "Set Event Format Mode Command Received with Mode = 0x%02x",
                        (unsigned int)CmdPtr->Mode);
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLEGALFMTMOD_EID, CFE_EVS_ERROR,
                        "Set Event Format Mode Command: Invalid Event Format Mode = 0x%02x", CmdPtr->Mode);
      Status = FALSE;
   }

   return Status;

} /* End CFE_EVS_SetEventFormatModeCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableAppEventTypesCmd
**
** Purpose:  This routine sets the given event type for the given application identifier to an
**           enabled state
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_EnableAppEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameBitMaskCmd_Payload_t *)Payload;
   uint32  AppID = CFE_EVS_UNDEF_APPID;
   int32   Status;
   char    LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retrieve application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {

        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                              "Bit Mask = 0x%08x out of range: CC = %lu",
                              (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
            Status = FALSE;  
        }
        else
        {
            EVS_EnableTypes(CmdPtr->BitMask, AppID);
            Status = TRUE;
        }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }

   if(Status == TRUE)
   {
      EVS_SendEvent(CFE_EVS_ENAAPPEVTTYPE_EID, CFE_EVS_DEBUG,
                        "Enable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                        LocalName, CmdPtr->BitMask);
   }

   return (boolean)Status;

} /* End CFE_EVS_EnableAppEventTypesCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableAppEventTypesCmd
**
** Purpose:  This routine sets the given event type for the given application identifier to a
**           disabled state
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_DisableAppEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameBitMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameBitMaskCmd_Payload_t *)Payload;
   int32  Status;
   char    LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {

        /* Need to check for an out of range bitmask, since our bit masks are only 4 bits */
        if (CmdPtr->BitMask == 0x0 || CmdPtr->BitMask > 0x0F)
        {
            EVS_SendEvent(CFE_EVS_ERR_INVALID_BITMASK_EID, CFE_EVS_ERROR,
                              "Bit Mask = 0x%08x out of range: CC = %lu",
                              (unsigned int)CmdPtr->BitMask, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
            Status = FALSE;  
        }
        else
        {
            EVS_DisableTypes(CmdPtr->BitMask, AppID);
            Status = TRUE;
        }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS,: CC = %lu",
                        LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
      Status = FALSE;
   }

   if(Status == TRUE)
   {
      EVS_SendEvent(CFE_EVS_DISAPPENTTYPE_EID, CFE_EVS_DEBUG,
                        "Disable App Event Type Command Received with AppName = %s, EventType Bit Mask = 0x%02x",
                        LocalName, CmdPtr->BitMask);
   }

   return (boolean)Status;

} /* End CFE_EVS_DisableAppEventTypes */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_EnableAppEventsCmd
**
** Purpose:  This routine enables application events for the given application identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_EnableAppEventsCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameCmd_Payload_t *)Payload;
   int32  Status;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retrieve application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
        CFE_EVS_GlobalData.AppData[AppID].ActiveFlag = TRUE;

        EVS_SendEvent(CFE_EVS_ENAAPPEVT_EID, CFE_EVS_DEBUG,
                          "Enable App Events Command Received with AppName = %s",
                          LocalName);

        Status = TRUE;
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ENABLE_APP_EVENTS_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End EVS_EnableAppEventsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DisableAppEventsCmd
**
** Purpose:  This routine disables application events for the given application identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_DisableAppEventsCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameCmd_Payload_t *)Payload;
   int32  Status = TRUE;
   char                LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
        CFE_EVS_GlobalData.AppData[AppID].ActiveFlag = FALSE;

        EVS_SendEvent(CFE_EVS_DISAPPEVT_EID, CFE_EVS_DEBUG,
                          "Disable App Events Command Received with AppName = %s",
                          LocalName);

        Status = TRUE;
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName,(long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Disable App Events Command: Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DISABLE_APP_EVENTS_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End CFE_EVS_DisableAppEventsCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetAppEventCounterCmd
**
** Purpose:  This routine sets the application event counter to zero for the given
**           application identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_ResetAppEventCounterCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32 AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameCmd_Payload_t *) Payload;
   int32  Status;
   char   LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
        CFE_EVS_GlobalData.AppData[AppID].EventCount = 0;

        EVS_SendEvent(CFE_EVS_RSTEVTCNT_EID, CFE_EVS_DEBUG,
                          "Reset Event Counter Command Received with AppName = %s",
                          LocalName);

        Status = TRUE;
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName,(long unsigned int) CFE_EVS_RESET_APP_COUNTER_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Reset Event Counter Command: Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_APP_COUNTER_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End CFE_EVS_ResetAppEventCounterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetFilterCmd
**
** Purpose:  This routine sets the application event filter counter to zero for the given
**           application identifier and event identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_ResetFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameEventIDCmd_Payload_t *)Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         FilterPtr->Count = 0;

         EVS_SendEvent(CFE_EVS_RSTFILTER_EID, CFE_EVS_DEBUG,
                           "Reset Filter Command Received with AppName = %s, EventID = 0x%08x",
                           LocalName, CmdPtr->EventID);

         Status = TRUE;
      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_FILTER_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End CFE_EVS_ResetFilterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetAllFiltersCmd
**
** Purpose:  This routine sets all application event filter counters to zero for the given
**           application identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_ResetAllFiltersCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   uint32                    AppID  = CFE_EVS_UNDEF_APPID;
   const CFE_EVS_AppNameCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameCmd_Payload_t *)Payload;
   int32                     Status;
   uint32                    i;
   char                      LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      for(i=0; i<CFE_EVS_MAX_EVENT_FILTERS; i++)
      {
         CFE_EVS_GlobalData.AppData[AppID].BinFilters[i].Count = 0;
      }

      EVS_SendEvent(CFE_EVS_RSTALLFILTER_EID, CFE_EVS_DEBUG,
                        "Reset All Filters Command Received with AppName = %s",
                        LocalName);

      Status = TRUE;
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_RESET_ALL_FILTERS_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End CFE_EVS_ResetAllFiltersCmd */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_AddEventFilterCmd
**
** Purpose:  This routine adds the given event filter for the given application
**           identifier and event identifier.
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_AddEventFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameEventIDMaskCmd_Payload_t *)Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      /* Check to see if this event is already registered for filtering */
      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      /* FilterPtr != NULL means that this Event ID was found as already being registered */
      if (FilterPtr != NULL)
      {
          EVS_SendEvent(CFE_EVS_EVT_FILTERED_EID, CFE_EVS_ERROR,
                      "Add Filter Command:AppName = %s, EventID = 0x%08x is already registered for filtering",
                      LocalName, CmdPtr->EventID);
          Status = FALSE;
      }
      else
      {
          /* now check to see if there is a free slot */
           FilterPtr = EVS_FindEventID(CFE_EVS_FREE_SLOT, AppDataPtr->BinFilters);

            if (FilterPtr != NULL)
            {
               /* Add Filter Contents */
               FilterPtr->EventID = CmdPtr->EventID;
               FilterPtr->Mask = CmdPtr->Mask;
               FilterPtr->Count = 0;

               EVS_SendEvent(CFE_EVS_ADDFILTER_EID, CFE_EVS_DEBUG,
                                 "Add Filter Command Received with AppName = %s, EventID = 0x%08x, Mask = 0x%04x",
                                 LocalName, CmdPtr->EventID, CmdPtr->Mask);

               Status = TRUE;
            }
            else
            {
               EVS_SendEvent(CFE_EVS_ERR_MAXREGSFILTER_EID, CFE_EVS_ERROR,
                                 "Add Filter Command: number of registered filters has reached max = %d",
                                 CFE_EVS_MAX_EVENT_FILTERS);

               Status = FALSE;
            }
      }/* end else*/
   } /* end if (Status == CFE_SUCCESS) */

   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName,(long unsigned int)CFE_EVS_ADD_EVENT_FILTER_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* CFE_End EVS_AddEventFilterCmd */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_DeleteEventFilterCmd
**
** Purpose:  This routine deletes the event filter for the given application
**           identifer and event identifier
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_DeleteEventFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_AppNameEventIDCmd_Payload_t *CmdPtr = (const CFE_EVS_AppNameEventIDCmd_Payload_t *)Payload;
   EVS_BinFilter_t     *FilterPtr;
   uint32               AppID = CFE_EVS_UNDEF_APPID;
   int32                Status;
   EVS_AppData_t       *AppDataPtr;
   char                 LocalName[OS_MAX_API_NAME];

   /*
    * Althgouh EVS_GetApplicationInfo() does not require a null terminated argument,
    * the value is passed to EVS_SendEvent which does require termination (normal C string)
    */
   CFE_SB_MessageStringGet(LocalName, (char *)CmdPtr->AppName, NULL, OS_MAX_API_NAME, sizeof(CmdPtr->AppName));

   /* Retreive application data */
   Status = EVS_GetApplicationInfo(&AppID, LocalName);

   if(Status == CFE_SUCCESS)
   {
      AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

      FilterPtr = EVS_FindEventID(CmdPtr->EventID, AppDataPtr->BinFilters);

      if(FilterPtr != NULL)
      {
         /* Clear Filter Contents */
         FilterPtr->EventID = CFE_EVS_FREE_SLOT;
         FilterPtr->Mask = CFE_EVS_NO_MASK;
         FilterPtr->Count = 0;

         EVS_SendEvent(CFE_EVS_DELFILTER_EID, CFE_EVS_DEBUG,
                           "Delete Filter Command Received with AppName = %s, EventID = 0x%08x",
                           LocalName, CmdPtr->EventID);

         Status = TRUE;
      }
      else
      {
         EVS_SendEvent(CFE_EVS_ERR_EVTIDNOREGS_EID, CFE_EVS_ERROR,
                           "%s Event ID %d not registered for filtering: CC = %lu",
                           LocalName, (int)CmdPtr->EventID, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
         Status = FALSE;
      }
   }
   else if(Status == CFE_EVS_APP_NOT_REGISTERED)
   {
      EVS_SendEvent(CFE_EVS_ERR_APPNOREGS_EID, CFE_EVS_ERROR,
                        "%s not registered with EVS: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
      Status = FALSE;
   }
   else if(Status == CFE_EVS_APP_ILLEGAL_APP_ID)
   {
      EVS_SendEvent(CFE_EVS_ERR_ILLAPPIDRANGE_EID, CFE_EVS_ERROR,
                        "Illegal application ID %d retrieved for %s: CC = %lu",
                        (int)AppID, LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
      Status = FALSE;
   }
   else
   {
      EVS_SendEvent(CFE_EVS_ERR_NOAPPIDFOUND_EID, CFE_EVS_ERROR,
                        "Unable to retrieve application ID for %s: CC = %lu",
                        LocalName, (long unsigned int)CFE_EVS_DELETE_EVENT_FILTER_CC);
      Status = FALSE;
   }

   return (boolean)Status;

} /* End EVS_DeleteEventFilterCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_WriteAppDataCmd
**
** Purpose:  This routine writes all application data to a file for all applications that
**           have registered with the EVS.  The application data includes the Application ID,
**           Active Flag, Event Count, Event Types Active Flag, and Filter Data.
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_WriteAppDataCmd ( const CFE_SB_MsgPayloadPtr_t Payload )
{
   boolean                           Result = FALSE;
   int32                             FileHandle;
   int32                             BytesWritten;
   uint32                            EntryCount = 0;
   uint32                            i;
   static CFE_EVS_AppDataFile_t      AppDataFile;
   CFE_FS_Header_t                   FileHdr;
   EVS_AppData_t                    *AppDataPtr;
   const CFE_EVS_AppDataCmd_Payload_t *CmdPtr = (const CFE_EVS_AppDataCmd_Payload_t *)Payload;
   char                              LocalName[OS_MAX_PATH_LEN];

   /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
   CFE_SB_MessageStringGet(LocalName, CmdPtr->AppDataFilename, CFE_EVS_DEFAULT_APP_DATA_FILE,
           OS_MAX_PATH_LEN, sizeof(CmdPtr->AppDataFilename));

   /* Create Application Data File */
   FileHandle = OS_creat(LocalName, OS_WRITE_ONLY);

   if (FileHandle < OS_FS_SUCCESS)
   {
      EVS_SendEvent(CFE_EVS_ERR_CRDATFILE_EID, CFE_EVS_ERROR,
                   "Write App Data Command Error: OS_creat = 0x%08X, filename = %s",
                    (unsigned int)FileHandle, LocalName);
   }
   else
   {
      /* Initialize cFE file header */
      CFE_FS_InitHeader(&FileHdr, "EVS Application Data File", CFE_FS_EVS_APPDATA_SUBTYPE);

      /* Write cFE file header to the App File */
      BytesWritten = CFE_FS_WriteHeader(FileHandle, &FileHdr);

      if (BytesWritten == sizeof(CFE_FS_Header_t))
      {
         for (i = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
         {
            /* Only have data for apps that are registered */
            if (CFE_EVS_GlobalData.AppData[i].RegisterFlag == TRUE)
            {
               AppDataPtr = &CFE_EVS_GlobalData.AppData[i];

               /* Clear application file data record */
               CFE_PSP_MemSet(&AppDataFile, 0, sizeof(CFE_EVS_AppDataFile_t));

               /* Copy application data to application file data record */
               CFE_ES_GetAppName(AppDataFile.AppName, i, OS_MAX_API_NAME);
               AppDataFile.ActiveFlag = AppDataPtr->ActiveFlag;
               AppDataFile.EventCount = AppDataPtr->EventCount;
               AppDataFile.EventTypesActiveFlag = AppDataPtr->EventTypesActiveFlag;

               /* Copy application filter data to application file data record */
               CFE_PSP_MemCpy(AppDataFile.Filters, AppDataPtr->BinFilters,
                              CFE_EVS_MAX_EVENT_FILTERS * sizeof(EVS_BinFilter_t));

               /* Write application data record to file */
               BytesWritten = OS_write(FileHandle, &AppDataFile, sizeof(CFE_EVS_AppDataFile_t));

               if (BytesWritten == sizeof(CFE_EVS_AppDataFile_t))
               {
                  EntryCount++;
               }
               else
               {
                  EVS_SendEvent(CFE_EVS_ERR_WRDATFILE_EID, CFE_EVS_ERROR,
                               "Write App Data Command Error: OS_write = 0x%08X, filename = %s",
                                (unsigned int)BytesWritten, LocalName);
                  break;
               }
            }
         }

         /* Process command handler success result */
         if (i == CFE_ES_MAX_APPLICATIONS)
         {
            EVS_SendEvent(CFE_EVS_WRDAT_EID, CFE_EVS_DEBUG,
                         "Write App Data Command: %d application data entries written to %s",
                          EntryCount, LocalName);
            Result = TRUE;
         }
      }

      OS_close(FileHandle);
   }

   return(Result);

} /* CFE_End EVS_WriteAppDataCmd */


/* End cfe_evs_task */
