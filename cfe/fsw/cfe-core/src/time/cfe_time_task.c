/*
** $Id: cfe_time_task.c 1.8 2014/04/14 10:51:39GMT-05:00 lwalling Exp  $
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
** Subsystem: cFE TIME Task
**
** Author: S. Walling (Microtel)
**
** Notes:
**
** $Log: cfe_time_task.c  $
** Revision 1.8 2014/04/14 10:51:39GMT-05:00 lwalling 
** Created platform config definitions for Time 1HZ and Time TONE task priorities and stack sizes
** Revision 1.7 2012/09/28 16:31:14EDT aschoeni 
** Removed Interrupt Enable fixme
** Revision 1.6 2012/01/18 16:33:35EST jmdagost 
** Updated no-op event msg to include cFE version numbers.
** Revision 1.5 2012/01/13 12:21:35EST acudmore 
** Changed license text to reflect open source
** Revision 1.4 2011/11/30 15:10:04EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.3 2009/06/08 16:13:07EDT lwalling 
** Change 1Hz adjust cmd arg from micro-seconds to sub-seconds
** Revision 1.2 2008/07/31 15:41:26EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.1 2008/04/17 08:05:39EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
** Revision 1.28 2007/09/21 15:40:15EDT David Kobe (dlkobe) 
** Modified pointer type definitions to eliminate Linux gcc compile warnings
** Revision 1.27 2007/07/18 10:23:55EDT njyanchik 
** I changed the 'Result' variable to 'Status in the ifdef.
** Revision 1.26 2007/05/15 11:00:14EDT rjmcgraw 
** DCR78:11 Exit main loop for CFE_SB_RcvMsg errors
** Revision 1.25 2007/04/27 16:59:26EDT rjmcgraw 
** DCR2987:4 Standardize task init error code
** Revision 1.24 2007/04/27 12:31:04EDT njyanchik 
** I changed EVS and TIME to not have a telemetry counter for SB pipe reads in the main loop. 
** Instead, I have added a syslog message to state the problem
** Revision 1.23 2007/04/13 16:22:05EDT apcudmore 
** Changed EarlyInit function prototype to return int32 status.
** Revision 1.22 2007/04/11 11:02:05EDT njyanchik 
** Remove the TaskDelay in the main loop for TIME. I also addeda counterin telemtry that will
** notify the ground of an error in reading the command pipe.
** Revision 1.21 2007/04/09 14:47:52EDT rjmcgraw 
** Changed names of performance ids to include CFE_
** Revision 1.20 2007/02/01 16:10:03EST rjmcgraw 
** Changed time and tone pkt subscriptions to local for server
** Revision 1.19 2006/11/01 11:33:17EST njyanchik 
** This change removes any references to the CDS in TIME. TIME now uses  the ES
** Reset Area to store its critial variables. this was due to an issue of trying to call
** CDS functions before apps were registered (in the EarlyInit call).
** Revision 1.18 2006/07/28 09:17:15GMT-05:00 njyanchik 
** Fix of LDRA errors
** Revision 1.17 2006/06/21 13:24:12EDT kkaudra 
** Removed cfe_es_perf.h
** Revision 1.16 2006/06/13 16:28:07EDT rjmcgraw 
** osperf to perf changes
*/


/*
** Required header files...
*/
#include "cfe_time_utils.h"
#include "cfe_version.h"

/*
** Time task global data...
*/
CFE_TIME_TaskData_t CFE_TIME_TaskData;


/*
** Command handler for "HK request"...
*/
void  CFE_TIME_HousekeepingCmd(void);

/*
** Command handler for "tone signal detected"...
*/
void  CFE_TIME_ToneSignalCmd(void);

/*
** Command handler for "time at the tone"...
*/
void  CFE_TIME_ToneDataCmd(CFE_SB_MsgPayloadPtr_t Payload);

/*
** Command handler for "fake tone"...
*/
#if (CFE_TIME_CFG_FAKE_TONE == TRUE)
void CFE_TIME_FakeToneCmd(void);
#endif

/*
** Command handler for "request time at the tone"...
**
** Note: This command (sent by the scheduler) is used to
**       signal that now is the right time (in relation
**       to the "real" tone signal) for a Time Server to
**       send the "time at the tone" data packet.  We do
**       not need (or want) this command if we are not a
**       Time Server, or if we are configured to operate
**       in "fake tone" mode, or if we are configured to
**       get time data from an external source.
*/
#if (CFE_TIME_CFG_SERVER == TRUE)
  #if (CFE_TIME_CFG_SOURCE != TRUE)
    #if (CFE_TIME_CFG_FAKE_TONE != TRUE)
void CFE_TIME_ToneSendCmd(void);
    #endif
  #endif
#endif

/*
** Ground command handlers...
*/
void CFE_TIME_NoopCmd(void);
void CFE_TIME_ResetCmd(void);
void CFE_TIME_DiagCmd(void);
void CFE_TIME_SetStateCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetSourceCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetSignalCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetDelayCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr, int16 Direction);
void CFE_TIME_SetTimeCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetMETCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetSTCFCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_SetLeapsCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr);
void CFE_TIME_AdjustCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr, int16 Direction);
void CFE_TIME_1HzAdjCmd(CFE_SB_MsgPayloadPtr_t PayloadPtr, int16 Direction);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_EarlyInit() -- API initialization before any tasks     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_TIME_EarlyInit(void)
{
    /*
    ** Initialize global Time Services data...
    */
    CFE_TIME_InitData();

    return ( CFE_SUCCESS);

} /* End of CFE_TIME_EarlyInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_TaskMain() -- Task entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_TaskMain(void)
{
    int32  Status;
  
    CFE_ES_PerfLogEntry(CFE_TIME_MAIN_PERF_ID);
  
    Status = CFE_TIME_TaskInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_TIME_MAIN_PERF_ID);
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

      CFE_ES_PerfLogExit(CFE_TIME_MAIN_PERF_ID);
  
      /* Pend on receipt of packet */
      Status = CFE_SB_RcvMsg(&CFE_TIME_TaskData.MsgPtr,
                              CFE_TIME_TaskData.CmdPipe,
                              CFE_SB_PEND_FOREVER);
  
      CFE_ES_PerfLogEntry(CFE_TIME_MAIN_PERF_ID);
  
      if (Status == CFE_SUCCESS)
      {
          /* Process cmd pipe msg */
          CFE_TIME_TaskPipe(CFE_TIME_TaskData.MsgPtr);
      }else{
          CFE_ES_WriteToSysLog("TIME:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
      }/* end if */
      
    }/* end while */
    
    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);

} /* end CFE_TIME_TaskMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_TaskInit() -- Time task initialization                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_TIME_TaskInit(void)
{
    int32 Status = CFE_SUCCESS;

    Status = CFE_ES_RegisterApp();
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    Status = CFE_EVS_Register(NULL, 0, 0);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    Status = OS_BinSemCreate(&CFE_TIME_TaskData.ToneSemaphore,
                              CFE_TIME_SEM_TONE_NAME,
                              CFE_TIME_SEM_VALUE,
                              CFE_TIME_SEM_OPTIONS);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error creating tone semaphore:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */        
    
    Status = OS_BinSemCreate(&CFE_TIME_TaskData.LocalSemaphore,
                              CFE_TIME_SEM_1HZ_NAME,
                              CFE_TIME_SEM_VALUE,
                              CFE_TIME_SEM_OPTIONS);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error creating local semaphore:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    
    Status = CFE_ES_CreateChildTask(&CFE_TIME_TaskData.ToneTaskID,
                                     CFE_TIME_TASK_TONE_NAME,
                                     CFE_TIME_Tone1HzTask,
                                     CFE_TIME_TASK_STACK_PTR,
                                     CFE_TIME_TONE_TASK_STACK_SIZE,
                                     CFE_TIME_TONE_TASK_PRIORITY,
                                     CFE_TIME_TASK_FLAGS);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error creating tone 1Hz child task:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
        
    Status = CFE_ES_CreateChildTask(&CFE_TIME_TaskData.LocalTaskID,
                                     CFE_TIME_TASK_1HZ_NAME,
                                     CFE_TIME_Local1HzTask,
                                     CFE_TIME_TASK_STACK_PTR,
                                     CFE_TIME_1HZ_TASK_STACK_SIZE,
                                     CFE_TIME_1HZ_TASK_PRIORITY,
                                     CFE_TIME_TASK_FLAGS);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error creating local 1Hz child task:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */


    Status = CFE_SB_CreatePipe(&CFE_TIME_TaskData.CmdPipe,
                                CFE_TIME_TaskData.PipeDepth,
                                CFE_TIME_TaskData.PipeName);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error creating cmd pipe:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */


    Status = CFE_SB_Subscribe(CFE_TIME_SEND_HK_MID,
                              CFE_TIME_TaskData.CmdPipe);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error subscribing to HK Request:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */


    /*
    ** Subscribe to time at the tone "signal" commands...
    */
    #if (CFE_TIME_CFG_CLIENT == TRUE)
    Status = CFE_SB_Subscribe(CFE_TIME_TONE_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe);
    #endif
    
    #if (CFE_TIME_CFG_SERVER == TRUE)
    Status = CFE_SB_SubscribeLocal(CFE_TIME_TONE_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe,4);
    #endif
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error subscribing to tone cmd:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */


    /*
    ** Subscribe to time at the tone "data" commands...
    */
    #if (CFE_TIME_CFG_CLIENT == TRUE)
    Status = CFE_SB_Subscribe(CFE_TIME_DATA_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe);
    #endif
    
    #if (CFE_TIME_CFG_SERVER == TRUE)
    Status = CFE_SB_SubscribeLocal(CFE_TIME_DATA_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe,4);
    #endif
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error subscribing to time data cmd:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    
    /*
    ** Subscribe to "fake" tone signal commands...
    */
    #if (CFE_TIME_CFG_FAKE_TONE == TRUE)

    #if (CFE_TIME_CFG_CLIENT == TRUE)
    Status = CFE_SB_Subscribe(CFE_TIME_FAKE_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe);
    #endif
    
    #if (CFE_TIME_CFG_SERVER == TRUE)
    Status = CFE_SB_SubscribeLocal(CFE_TIME_FAKE_CMD_MID,
                                   CFE_TIME_TaskData.CmdPipe,4);
    #endif
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error subscribing to fake tone signal cmds:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */    
    #endif    
    
    
    /*
    ** Subscribe to time at the tone "request data" commands...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
      #if (CFE_TIME_CFG_SOURCE != TRUE)
        #if (CFE_TIME_CFG_FAKE_TONE != TRUE)

        
   Status = CFE_SB_Subscribe(CFE_TIME_SEND_CMD_MID,
                                  CFE_TIME_TaskData.CmdPipe);
        if(Status != CFE_SUCCESS)
        {
          CFE_ES_WriteToSysLog("TIME:Error subscribing to time at the tone request data cmds:RC=0x%08X\n",(unsigned int)Status);
          return Status;
        }/* end if */

        #endif
      #endif
    #endif

    /*
    ** Subscribe to Time task ground command packets...
    */
    Status = CFE_SB_Subscribe(CFE_TIME_CMD_MID,
                              CFE_TIME_TaskData.CmdPipe);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error subscribing to time task gnd cmds:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */  


    Status = CFE_EVS_SendEvent(CFE_TIME_INIT_EID,
                               CFE_EVS_INFORMATION,
                               "cFE TIME Initialized");
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TIME:Error sending init event:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /*
    ** Select primary vs redundant tone interrupt signal...
    */
    #if (CFE_TIME_CFG_SIGNAL == TRUE)
    OS_SelectTone(CFE_TIME_TaskData.ClockSignal);
    #endif

    /*
     * Check to see if the OSAL in use implements the TimeBase API
     * and if the PSP has set up a system time base.  If so, then create
     * a 1Hz callback based on that system time base.  This call should
     * return OS_ERR_NOT_IMPLEMENTED if the OSAL does not support this,
     * or OS_ERR_NAME_NOT_FOUND if the PSP didn't set this up.  Either
     * way any error here means the PSP must use the "old way" and call
     * the 1hz function directly.
     */
    /* McCarty - Removed for compile. Dellingr missing 
    Status = OS_TimeBaseGetIdByName(&TimeBaseId, "cFS-Master");
    if (Status == OS_SUCCESS)
    {
        // Create the 1Hz callback 
        Status = OS_TimerAdd(&TimerId, "cFS-1Hz", TimeBaseId, CFE_TIME_Local1HzTimerCallback, NULL);
        if (Status == OS_SUCCESS)
        {
            Status = OS_TimerSet(TimerId, 500000, 1000000);
            if (Status != OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("TIME:1Hz OS_TimerSet failed:RC=0x%08X\n",(unsigned int)Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("TIME:1Hz OS_TimerAdd failed:RC=0x%08X\n",(unsigned int)Status);
        }
    } 
    */

    return CFE_SUCCESS;

} /* End of CFE_TIME_TaskInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_TaskPipe() -- Process command pipe message             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_TaskPipe(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t MessageID;
    CFE_SB_MsgPayloadPtr_t Payload;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(MessagePtr);
    Payload = &MessagePtr->Byte[CFE_SB_CMD_HDR_SIZE];
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request...
        */
        case CFE_TIME_SEND_HK_MID:
            CFE_TIME_HousekeepingCmd();
            break;

        /*
        ** Time at the tone "signal"...
        */
        case CFE_TIME_TONE_CMD_MID:
            CFE_TIME_ToneSignalCmd();
            break;

        /*
        ** Time at the tone "data"...
        */
        case CFE_TIME_DATA_CMD_MID:
            CFE_TIME_ToneDataCmd(Payload);
            break;

        /*
        ** Simulate time at the tone "signal"...
        */
        #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
        case CFE_TIME_FAKE_CMD_MID:
            CFE_TIME_FakeToneCmd();
            break;
        #endif

        /*
        ** Request for time at the tone "data"...
        */
        #if (CFE_TIME_CFG_SERVER == TRUE)
          #if (CFE_TIME_CFG_SOURCE != TRUE)
            #if (CFE_TIME_CFG_FAKE_TONE != TRUE)
        case CFE_TIME_SEND_CMD_MID:
            CFE_TIME_ToneSendCmd();
            break;
            #endif
          #endif
        #endif

        /*
        ** Time task ground commands...
        */
        case CFE_TIME_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
            {
                case CFE_TIME_NOOP_CC:
                    CFE_TIME_NoopCmd();
                    break;

                case CFE_TIME_RESET_CC:
                    CFE_TIME_ResetCmd();
                    break;

                case CFE_TIME_DIAG_TLM_CC:
                    CFE_TIME_DiagCmd();
                    break;

                case CFE_TIME_SET_STATE_CC:
                    CFE_TIME_SetStateCmd(Payload);
                    break;

                case CFE_TIME_SET_SOURCE_CC:
                    CFE_TIME_SetSourceCmd(Payload);
                    break;

                case CFE_TIME_SET_SIGNAL_CC:
                    CFE_TIME_SetSignalCmd(Payload);
                    break;

                /*
                ** Time Clients process "tone delay" commands...
                */
                case CFE_TIME_ADD_DELAY_CC:
                    CFE_TIME_SetDelayCmd(Payload, (int16) CFE_TIME_ADD_ADJUST);
                    break;

                case CFE_TIME_SUB_DELAY_CC:
                    CFE_TIME_SetDelayCmd(Payload, (int16) CFE_TIME_SUB_ADJUST);
                    break;

                /*
                ** Time Servers process "set time" commands...
                */
                case CFE_TIME_SET_TIME_CC:
                    CFE_TIME_SetTimeCmd(Payload);
                    break;

                case CFE_TIME_SET_MET_CC:
                    CFE_TIME_SetMETCmd(Payload);
                    break;

                case CFE_TIME_SET_STCF_CC:
                    CFE_TIME_SetSTCFCmd(Payload);
                    break;

                case CFE_TIME_SET_LEAPS_CC:
                    CFE_TIME_SetLeapsCmd(Payload);
                    break;

                case CFE_TIME_ADD_ADJUST_CC:
                    CFE_TIME_AdjustCmd(Payload, (int16) CFE_TIME_ADD_ADJUST);
                    break;

                case CFE_TIME_SUB_ADJUST_CC:
                    CFE_TIME_AdjustCmd(Payload,  (int16) CFE_TIME_SUB_ADJUST);
                    break;

                case CFE_TIME_ADD_1HZADJ_CC:
                    CFE_TIME_1HzAdjCmd(Payload, (int16) CFE_TIME_ADD_ADJUST);
                    break;

                case CFE_TIME_SUB_1HZADJ_CC:
                    CFE_TIME_1HzAdjCmd(Payload, (int16) CFE_TIME_SUB_ADJUST);
                    break;

                default:

                    CFE_TIME_TaskData.ErrCounter++;
                    CFE_EVS_SendEvent(CFE_TIME_CC_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command code -- ID = 0x%X, CC = %d",
                                      (unsigned int)MessageID, (int)CommandCode);
                    break;
            } /* switch (CFE_TIME_CMD_MID -- command code)*/
            break;

        default:

            /*
            ** Note: we only increment the command error counter when
            **    processing CFE_TIME_CMD_MID commands...
            */
            CFE_EVS_SendEvent(CFE_TIME_ID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid message ID -- ID = 0x%X",
                              (unsigned int)MessageID);
            break;

    } /* switch (message ID) */

    return;

} /* End of CFE_TIME_TaskPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_HousekeepingCmd() -- On-board command (HK request)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_HousekeepingCmd(void)
{
    CFE_TIME_Reference_t Reference;

    /*
    ** Get reference time values (local time, time at tone, etc.)...
    */
    CFE_TIME_GetReference(&Reference);

    /*
    ** Update TIME portion of Critical Data Store...
    */
    CFE_TIME_UpdateResetVars(&Reference);

    /*
    ** Collect housekeeping data from Time Services utilities...
    */
    CFE_TIME_GetHkData(&Reference);

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.HkPacket);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.HkPacket);

    /*
    ** Note: we only increment the command execution counter when
    **   processing CFE_TIME_CMD_MID commands...
    */
    return;

} /* End of CFE_TIME_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSignalCmd() -- Time at tone command (signal)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneSignalCmd(void)
{
    /*
    ** Indication that tone signal occurred recently...
    */
    CFE_TIME_ToneSignal();

    #if (CFE_TIME_CFG_SERVER == TRUE)
      #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    /*
    ** If we are simulating the tone signal, and we are the time
    **   server, and we have processed the simulated tone, then
    **   send time at the tone data...
    */
    CFE_TIME_ToneSend();
      #endif
    #endif

    /*
    ** Note: we only increment the command execution counter when
    **   processing CFE_TIME_CMD_MID commands...
    */
    return;

} /* End of CFE_TIME_ToneSignalCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneDataCmd() -- Time at tone command (data)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ToneDataCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    /*
    ** This command packet contains "time at the tone" data...
    */
    CFE_TIME_ToneData((CFE_TIME_ToneDataCmd_Payload_t *) Payload);

    /*
    ** Note: we only increment the command execution counter when
    **   processing CFE_TIME_CMD_MID commands...
    */
    return;

} /* End of CFE_TIME_ToneDataCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_FakeToneCmd() -- Simulated time at tone signal         */
/*                                                                 */
/* If we don't have a distributed tone signal and we are in a      */
/*    lab environment where time accuracy isn't important, then    */
/*    we can use a SB message to signal the tone.                  */
/*                                                                 */
/* Both the time server and clients may be configured to use       */
/*    the arrival time of the SB message as the tone (instead      */
/*    of a discrete interrupt signal).                             */
/*                                                                 */
/* This way the actual time of the tone is closer to being in      */
/*    sync and we can ignore the slight delays due to getting      */
/*    the tone command from the SB message queue since it will     */
/*    be the same for the server and clients.                      */
/*                                                                 */
/* This command is sent by the time server (might be us) while     */
/*    processing its local 1Hz interrupt task.                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_FAKE_TONE == TRUE)
void CFE_TIME_FakeToneCmd(void)
{
    /*
    ** Fake the call-back from the "real" h/w ISR...
    */
    CFE_TIME_Tone1HzISR();

    /*
    ** Note: we only increment the command execution counter when
    **   processing CFE_TIME_CMD_MID commands...
    */
    return;

} /* End of CFE_TIME_FakeToneCmd() */
#endif /* CFE_TIME_CFG_FAKE_TONE */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ToneSendCmd() -- Time at tone command (send data)      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (CFE_TIME_CFG_SERVER == TRUE)
  #if (CFE_TIME_CFG_SOURCE != TRUE)
    #if (CFE_TIME_CFG_FAKE_TONE != TRUE)
void CFE_TIME_ToneSendCmd(void)
{
    /*
    ** Request for "time at tone" data packet (probably scheduler)...
    */
    CFE_TIME_ToneSend();

    /*
    ** Note: we only increment the command execution counter when
    **   processing CFE_TIME_CMD_MID commands...
    */
    return;

} /* End of CFE_TIME_SendCmd() */
    #endif /* CFE_TIME_CFG_SERVER */
  #endif /* not CFE_TIME_CFG_SOURCE */
#endif /* not CFE_TIME_CFG_FAKE_TONE */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_NoopCmd() -- Time task ground command (NO-OP)          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_NoopCmd(void)
{

    CFE_TIME_TaskData.CmdCounter++;

    CFE_EVS_SendEvent(CFE_TIME_NOOP_EID, CFE_EVS_INFORMATION,
                     "No-op command. cFE Version %d.%d.%d.%d",
                     CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

    return;

} /* End of CFE_TIME_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_ResetCmd() -- Time task ground command (reset counters)*/
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_ResetCmd(void)
{

    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;

    CFE_TIME_TaskData.ToneMatchCount  = 0;
    CFE_TIME_TaskData.ToneMatchErrors = 0;

    CFE_TIME_TaskData.ToneSignalCount = 0;
    CFE_TIME_TaskData.ToneDataCount   = 0;

    CFE_TIME_TaskData.ToneIntCount    = 0;
    CFE_TIME_TaskData.ToneIntErrors   = 0;
    CFE_TIME_TaskData.ToneTaskCount   = 0;

    CFE_TIME_TaskData.VersionCount    = 0;

    CFE_TIME_TaskData.LocalIntCount   = 0;
    CFE_TIME_TaskData.LocalTaskCount  = 0;

    CFE_TIME_TaskData.InternalCount   = 0;
    CFE_TIME_TaskData.ExternalCount   = 0;

    CFE_EVS_SendEvent(CFE_TIME_RESET_EID, CFE_EVS_DEBUG,
                     "Reset Counters command");

    return;

} /* End of CFE_TIME_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_DiagCmd() -- Time task ground command (diagnostics)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_DiagCmd(void)
{
    CFE_TIME_TaskData.CmdCounter++;

    /*
    ** Collect housekeeping data from Time Services utilities...
    */
    CFE_TIME_GetDiagData();

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.DiagPacket);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TIME_TaskData.DiagPacket);

    CFE_EVS_SendEvent(CFE_TIME_DIAG_EID, CFE_EVS_DEBUG,
                     "Request diagnostics command");

    return;

} /* End of CFE_TIME_DiagCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetStateCmd() -- Time task command (set clock state)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetStateCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_StateCmd_Payload_t *CommandPtr = (CFE_TIME_StateCmd_Payload_t *) Payload;
    char *ClockStateText;

    /*
    ** Verify command argument value (clock state)...
    */
    if ((CommandPtr->ClockState == CFE_TIME_INVALID) ||
        (CommandPtr->ClockState == CFE_TIME_VALID)   ||
        (CommandPtr->ClockState == CFE_TIME_FLYWHEEL))
    {
        CFE_TIME_SetState(CommandPtr->ClockState);

        /*
        ** Select appropriate text for event message...
        */
        if (CommandPtr->ClockState == CFE_TIME_INVALID)
        {
            ClockStateText = "INVALID";
        }
        else if (CommandPtr->ClockState == CFE_TIME_VALID)
        {
            ClockStateText = "VALID";
        }
        else
        {
            ClockStateText = "FLYWHEEL";
        }

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_STATE_EID, CFE_EVS_INFORMATION,
                         "Set Clock State = %s", ClockStateText);
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_STATE_ERR_EID, CFE_EVS_ERROR,
                         "Invalid Clock State = 0x%X", (unsigned int)CommandPtr->ClockState);
    }

    return;

} /* End of CFE_TIME_SetStateCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSourceCmd() -- Time task command (set time source)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetSourceCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_SourceCmd_Payload_t *CommandPtr = (CFE_TIME_SourceCmd_Payload_t *) Payload;

    #if (CFE_TIME_CFG_SOURCE == TRUE)
    char *TimeSourceText;
    #endif

    /*
    ** Verify command argument value (time data source)...
    */
    if ((CommandPtr->TimeSource == CFE_TIME_USE_INTERN) ||
        (CommandPtr->TimeSource == CFE_TIME_USE_EXTERN))
    {
        #if (CFE_TIME_CFG_SOURCE == TRUE)
        /*
        ** Only systems configured to select source of time data...
        */
        CFE_TIME_TaskData.CmdCounter++;

        CFE_TIME_SetSource(CommandPtr->TimeSource);

        /*
        ** Select appropriate text for event message...
        */
        if (CommandPtr->TimeSource == CFE_TIME_USE_INTERN)
        {
            TimeSourceText = "INTERNAL";
        }
        else
        {
            TimeSourceText = "EXTERNAL";
        }

        CFE_EVS_SendEvent(CFE_TIME_SOURCE_EID,
                          CFE_EVS_INFORMATION,
                         "Set Time Source = %s", TimeSourceText);

        #else /* not CFE_TIME_CFG_SOURCE */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_SOURCE_CFG_EID, CFE_EVS_ERROR,
        "Set Source commands invalid without CFE_TIME_CFG_SOURCE set to TRUE");

        #endif /* CFE_TIME_CFG_SOURCE */
    }
    else
    {
        /*
        ** Ground system database will prevent most of these errors...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_SOURCE_ERR_EID, CFE_EVS_ERROR,
                         "Invalid Time Source = 0x%X", (unsigned int)CommandPtr->TimeSource);
    }

    return;

} /* End of CFE_TIME_SetSourceCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSignalCmd() -- Time task command (set tone source)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetSignalCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_SignalCmd_Payload_t *CommandPtr = (CFE_TIME_SignalCmd_Payload_t *) Payload;

    #if (CFE_TIME_CFG_SIGNAL == TRUE)
    char *ToneSourceText;
    #endif

    /*
    ** Verify command argument value (tone source)...
    */
    if ((CommandPtr->ToneSource == CFE_TIME_TONE_PRI) ||
        (CommandPtr->ToneSource == CFE_TIME_TONE_RED))
    {
        #if (CFE_TIME_CFG_SIGNAL == TRUE)
        /*
        ** Only systems configured to select tone signal...
        */
        CFE_TIME_TaskData.CmdCounter++;

        CFE_TIME_SetSignal(CommandPtr->ToneSource);

        /*
        ** Select appropriate text for event message...
        */
        if (CommandPtr->ToneSource == CFE_TIME_TONE_PRI)
        {
            ToneSourceText = "PRIMARY";
        }
        else
        {
            ToneSourceText = "REDUNDANT";
        }

        CFE_EVS_SendEvent(CFE_TIME_SIGNAL_EID,
                          CFE_EVS_INFORMATION,
                         "Set Tone Source = %s", ToneSourceText);

        #else /* not CFE_TIME_CFG_SIGNAL */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_SIGNAL_CFG_EID, CFE_EVS_ERROR,
           "Set Signal commands invalid without CFE_TIME_CFG_SIGNAL set to TRUE");

        #endif /* CFE_TIME_CFG_SIGNAL */
    }
    else
    {
        /*
        ** Ground system database will prevent most of these errors...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_SIGNAL_ERR_EID, CFE_EVS_ERROR,
                         "Invalid Tone Source = 0x%X", (unsigned int)CommandPtr->ToneSource);
    }

    return;

} /* End of CFE_TIME_SetSignalCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetDelayCmd() -- Time task ground command (tone delay) */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetDelayCmd(CFE_SB_MsgPayloadPtr_t Payload, int16 Direction)
{
    CFE_TIME_TimeCmd_Payload_t *CommandPtr = (CFE_TIME_TimeCmd_Payload_t *) Payload;

    /*
    ** Verify "micro-seconds" command argument...
    */
    if (CommandPtr->MicroSeconds < 1000000)
    {
        #if (CFE_TIME_CFG_CLIENT == TRUE)

        CFE_TIME_SysTime_t Delay;

        Delay.Seconds = CommandPtr->Seconds;
        Delay.Subseconds = CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds);

        CFE_TIME_SetDelay(Delay, Direction);

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_DELAY_EID, CFE_EVS_INFORMATION,
           "Set Tone Delay -- secs = %u, usecs = %u, ssecs = 0x%X, dir = %d",
           (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds,
                          (unsigned int)CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds),
                          (int)Direction);

        #else /* not CFE_TIME_CFG_CLIENT */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_DELAY_CFG_EID, CFE_EVS_ERROR,
        "Set Delay commands invalid without CFE_TIME_CFG_CLIENT set to TRUE");

        #endif /* CFE_TIME_CFG_CLIENT */
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_DELAY_ERR_EID, CFE_EVS_ERROR,
               "Invalid Tone Delay -- secs = %u, usecs = %u",
               (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds);
    }

    return;

} /* End of CFE_TIME_SetDelayCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetTimeCmd() -- Time task ground command (calc STCF)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetTimeCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_TimeCmd_Payload_t *CommandPtr = (CFE_TIME_TimeCmd_Payload_t *) Payload;

    /*
    ** Verify "micro-seconds" command argument...
    */
    if (CommandPtr->MicroSeconds < 1000000)
    {
        #if (CFE_TIME_CFG_SERVER == TRUE)

        CFE_TIME_SysTime_t NewTime;

        NewTime.Seconds = CommandPtr->Seconds;
        NewTime.Subseconds = CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds);

        CFE_TIME_SetTime(NewTime);

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_TIME_EID, CFE_EVS_INFORMATION,
                         "Set Time -- secs = %u, usecs = %u, ssecs = 0x%X",
                          (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds,
                          (unsigned int)CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds));

        #else /* not CFE_TIME_CFG_SERVER */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_TIME_CFG_EID, CFE_EVS_ERROR,
           "Set Time commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

        #endif /* CFE_TIME_CFG_SERVER */
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_TIME_ERR_EID, CFE_EVS_ERROR,
                         "Invalid Time -- secs = %u, usecs = %u",
                         (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds);
    }

    return;

} /* End of CFE_TIME_SetTimeCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetMETCmd() -- Time task ground command (set MET)      */
/*                                                                 */
/* Note: This command will not have lasting effect if configured   */
/*       to get external time of type MET.  Also, there cannot     */
/*       be a local h/w MET and an external MET since both would   */
/*       need to be synchronized to the same tone signal.          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetMETCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_TimeCmd_Payload_t *CommandPtr = (CFE_TIME_TimeCmd_Payload_t *) Payload;

    /*
    ** Verify "micro-seconds" command argument...
    */
    if (CommandPtr->MicroSeconds < 1000000)
    {
        #if (CFE_TIME_CFG_SERVER == TRUE)

        CFE_TIME_SysTime_t NewMET;

        NewMET.Seconds = CommandPtr->Seconds;
        NewMET.Subseconds = CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds);

        CFE_TIME_SetMET(NewMET);

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_MET_EID, CFE_EVS_INFORMATION,
                         "Set MET -- secs = %u, usecs = %u, ssecs = 0x%X",
                          (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds,
                          (unsigned int)CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds));

        #else /* not CFE_TIME_CFG_SERVER */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_MET_CFG_EID, CFE_EVS_ERROR,
           "Set MET commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

        #endif /* CFE_TIME_CFG_SERVER */
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_MET_ERR_EID, CFE_EVS_ERROR,
                         "Invalid MET -- secs = %u, usecs = %u",
                         (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds);
    }

    return;

} /* End of CFE_TIME_SetMETCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetSTCFCmd() -- Time task ground command (set STCF)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetSTCFCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    CFE_TIME_TimeCmd_Payload_t *CommandPtr = (CFE_TIME_TimeCmd_Payload_t *) Payload;

    /*
    ** Verify "micro-seconds" command argument...
    */
    if (CommandPtr->MicroSeconds < 1000000)
    {
        #if (CFE_TIME_CFG_SERVER == TRUE)

        CFE_TIME_SysTime_t NewSTCF;

        NewSTCF.Seconds = CommandPtr->Seconds;
        NewSTCF.Subseconds = CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds);

        CFE_TIME_SetSTCF(NewSTCF);

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_STCF_EID, CFE_EVS_INFORMATION,
                         "Set STCF -- secs = %u, usecs = %u, ssecs = 0x%X",
                         (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds,
                         (unsigned int)CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds));

        #else /* not CFE_TIME_CFG_SERVER */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_STCF_CFG_EID, CFE_EVS_ERROR,
           "Set STCF commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

        #endif /* CFE_TIME_CFG_SERVER */
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_STCF_ERR_EID, CFE_EVS_ERROR,
                         "Invalid STCF -- secs = %u, usecs = %u",
                         (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds);
    }

    return;

} /* End of CFE_TIME_SetSTCFCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_SetLeapsCmd() -- Time task ground command (set leaps)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_SetLeapsCmd(CFE_SB_MsgPayloadPtr_t Payload)
{
    #if (CFE_TIME_CFG_SERVER == TRUE)

    CFE_TIME_LeapsCmd_Payload_t *CommandPtr = (CFE_TIME_LeapsCmd_Payload_t *) Payload;

    /*
    ** No value checking (leaps may be positive or negative)...
    */
    CFE_TIME_SetLeaps(CommandPtr->LeapSeconds);

    CFE_TIME_TaskData.CmdCounter++;

    CFE_EVS_SendEvent(CFE_TIME_LEAPS_EID, CFE_EVS_INFORMATION,
                     "Set Leap Seconds = %d", (int)CommandPtr->LeapSeconds);

    #else /* not CFE_TIME_CFG_SERVER */
    /*
    ** We want to know if disabled commands are being sent...
    */
    CFE_TIME_TaskData.ErrCounter++;

    CFE_EVS_SendEvent(CFE_TIME_LEAPS_CFG_EID, CFE_EVS_ERROR,
       "Set Leaps commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

    #endif /* CFE_TIME_CFG_SERVER */

    return;

} /* End of CFE_TIME_SetLeapsCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_AdjustCmd() -- Time task ground command (adjust STCF)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_AdjustCmd(CFE_SB_MsgPayloadPtr_t Payload, int16 Direction)
{
    CFE_TIME_TimeCmd_Payload_t *CommandPtr = (CFE_TIME_TimeCmd_Payload_t *) Payload;

    /*
    ** Verify command arguments...
    */
    if (CommandPtr->MicroSeconds < 1000000)
    {
        #if (CFE_TIME_CFG_SERVER == TRUE)

        CFE_TIME_SysTime_t Adjust;

        Adjust.Seconds = CommandPtr->Seconds;
        Adjust.Subseconds = CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds);

        CFE_TIME_SetAdjust(Adjust, Direction);

        CFE_TIME_TaskData.CmdCounter++;
        CFE_EVS_SendEvent(CFE_TIME_DELTA_EID, CFE_EVS_INFORMATION,
           "STCF Adjust -- secs = %u, usecs = %u, ssecs = 0x%X, dir[1=Pos, 2=Neg] = %d",
           (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds,
           (unsigned int)CFE_TIME_Micro2SubSecs(CommandPtr->MicroSeconds),
                          (int)Direction);

        #else /* not CFE_TIME_CFG_SERVER */
        /*
        ** We want to know if disabled commands are being sent...
        */
        CFE_TIME_TaskData.ErrCounter++;

        CFE_EVS_SendEvent(CFE_TIME_DELTA_CFG_EID, CFE_EVS_ERROR,
           "STCF Adjust commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

        #endif /* CFE_TIME_CFG_SERVER */
    }
    else
    {
        CFE_TIME_TaskData.ErrCounter++;
        CFE_EVS_SendEvent(CFE_TIME_DELTA_ERR_EID, CFE_EVS_ERROR,
           "Invalid STCF Adjust -- secs = %u, usecs = %u, dir[1=Pos, 2=Neg] = %d",
           (unsigned int)CommandPtr->Seconds, (unsigned int)CommandPtr->MicroSeconds, (int)Direction);
    }

    return;

} /* End of CFE_TIME_AdjustCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFE_TIME_1HzAdjCmd() -- Time task ground command (1Hz adjust)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CFE_TIME_1HzAdjCmd(CFE_SB_MsgPayloadPtr_t Payload, int16 Direction)
{
    /*
    ** 1Hz adjustments are only valid for "Time Servers"...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)

    CFE_TIME_1HzAdjCmd_Payload_t *CommandPtr = (CFE_TIME_1HzAdjCmd_Payload_t *) Payload;
    CFE_TIME_SysTime_t Adjust;

    CFE_TIME_Copy(&Adjust, CommandPtr);

    CFE_TIME_Set1HzAdj(Adjust, Direction);

    CFE_TIME_TaskData.CmdCounter++;
    CFE_EVS_SendEvent(CFE_TIME_1HZ_EID, CFE_EVS_INFORMATION,
       "STCF 1Hz Adjust -- secs = %d, ssecs = 0x%X, dir[1=Pos, 2=Neg] = %d",
                      (int)CommandPtr->Seconds, (unsigned int)CommandPtr->Subseconds, (int)Direction);

    #else /* not CFE_TIME_CFG_SERVER */
    /*
    ** We want to know if disabled commands are being sent...
    */
    CFE_TIME_TaskData.ErrCounter++;

    CFE_EVS_SendEvent(CFE_TIME_1HZ_CFG_EID, CFE_EVS_ERROR,
       "1Hz Adjust commands invalid without CFE_TIME_CFG_SERVER set to TRUE");

    #endif /* CFE_TIME_CFG_SERVER */

    return;

} /* End of CFE_TIME_1HzAdjCmd() */


/************************/
/*  End of File Comment */
/************************/
