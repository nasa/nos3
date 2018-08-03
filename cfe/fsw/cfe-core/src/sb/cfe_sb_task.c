/******************************************************************************
** File: cfe_sb_task.c
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
** Purpose:
**      This file contains the source code for the SB task.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_task.c  $
** Revision 1.18 2012/01/18 16:30:29GMT-05:00 jmdagost 
** Updated no-op event message to include cFE version numbers.
** Revision 1.17 2012/01/13 12:15:13EST acudmore 
** Changed license text to reflect open source
** Revision 1.16 2011/12/20 10:28:02EST rmcgraw 
** DCR15187:1 Changed MsgIdToPipeErrCnt to MsgLimErrCnt
** Revision 1.15 2009/07/17 18:07:33EDT aschoeni 
** Updated MsgMap and associated variables to be CFE_SB_MsgId_t
** Revision 1.14 2009/06/26 17:02:06EDT aschoeni 
** Updated SB to use __func__ instead of __FILE__ for lock and unlock errors
** Revision 1.13 2009/06/10 09:18:03EDT acudmore 
** Updated OS_Mem* and OS_BSP* to CFE_PSP_*
** Revision 1.12 2009/04/29 10:05:37EDT rmcgraw 
** DCR5801:11 Removed old code that was commented out
** Revision 1.11 2009/04/24 10:30:52EDT rmcgraw 
** DCR6772:1 Added #include cfe_es_msg.h
** Revision 1.10 2009/04/24 10:24:48EDT rmcgraw 
** DCR6772:1 Changed the Get/PutPoolBuf check in SB_AppInit
** Revision 1.9 2009/04/08 13:20:10EDT rmcgraw 
** DCR6772:1 Added insurance that a gnd cmded reset can get to ES
** Revision 1.8 2009/04/01 15:23:28EDT rmcgraw 
** DCR6296:4 Changed hk tlm point from pk-mem-inuse to unmarked mem
** Revision 1.7 2009/02/12 14:52:46EST rmcgraw 
** DCR6296:3 Remove 'Buf' in memory pool names
** Revision 1.6 2009/02/06 12:56:07EST rmcgraw 
** DCR5802:3 Removed two of four new telemetry items
** Revision 1.5 2009/02/05 16:19:53EST rmcgraw 
** DCR5802:2 Added SB statistics to hk pkt
** Revision 1.4 2009/02/03 11:07:01EST rmcgraw 
** DCR5801:2 Changed destination desciptors from array based to linked list
** Revision 1.3 2008/08/08 14:11:56EDT rjmcgraw 
** DCR4208:1 Added #include cfe_sb_verify.h to cfe_sb_task.c
** Revision 1.2 2008/07/31 15:41:34EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.1 2008/04/17 08:05:32EDT ruperera 
** Initial revision
** Member added to tlserver3
** Revision 1.62 2007/09/20 11:59:44EDT rjmcgraw 
** Fixed compiler error related to event log variable
** Revision 1.61 2007/09/04 16:22:50EDT rjmcgraw 
** Added code to detect number of active event filters in platform cfg file
** Revision 1.60 2007/07/12 17:01:37EDT rjmcgraw 
** Added code to process new SB event filtering params from platform cfg file
** Revision 1.59 2007/06/04 13:23:39EDT rjmcgraw 
** Moved SB stat initialization from task init to early init
** Revision 1.58 2007/05/18 16:12:11EDT rjmcgraw 
** DCR3052:9 Moved initialization of subscription report state from app init to 
** early init
** Revision 1.57 2007/05/16 15:13:40EDT dlkobe 
** Added MemPoolHandle members to HK packet
** Revision 1.56 2007/05/15 11:01:32EDT rjmcgraw 
** DCR78:11 Exit main loop for CFE_SB_RcvMsg errors
** Revision 1.55 2007/05/10 15:44:04EDT rjmcgraw 
** Added check of GetPipePtr return in SendRtgInfo
** Revision 1.54 2007/05/01 09:42:13EDT rjmcgraw 
** DCR3052:7 Removed function SendNetworkInfo
** Revision 1.53 2007/05/01 09:30:42EDT rjmcgraw 
** DCR3314:3 Populate subtype fields for SB files
** Revision 1.52 2007/04/30 15:20:34EDT rjmcgraw 
** DCR78:5 Removed delay, added sys log msg for cmd pipe errors
**
******************************************************************************/

/* Include Files */

#include "cfe_sb.h"
#include "cfe_sb_events.h"
#include "cfe_evs.h"
#include "cfe_sb_priv.h"
#include "osapi.h"
#include "cfe_version.h"
#include "cfe_msgids.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_es_msg.h"
#include "cfe_sb_verify.h"
#include <string.h>

/*  Task Globals */
cfe_sb_t                CFE_SB;
CFE_SB_Qos_t            CFE_SB_Default_Qos;


/******************************************************************************
**  Function:  CFE_SB_TaskMain()
**
**  Purpose:
**    Main loop for Software Bus task, used to process SB commands.
**
**  Arguments:
**    none
**
**  Return:
**    none
*/
void CFE_SB_TaskMain(void)
{
    int32  Status;

    CFE_ES_PerfLogEntry(CFE_SB_MAIN_PERF_ID);

    Status = CFE_SB_AppInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("SB:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_SB_MAIN_PERF_ID);
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

        CFE_ES_PerfLogExit(CFE_SB_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
                                CFE_SB.CmdPipe,
                                CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_SB_MAIN_PERF_ID);

        if(Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_SB_ProcessCmdPipePkt();
        }else{
            CFE_ES_WriteToSysLog("SB:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */

    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR);

}/* end CFE_SB_TaskMain */



/******************************************************************************
**  Function:  CFE_SB_AppInit()
**
**  Purpose:
**    Initialization routine for SB application. This routine is executed when
**    the SB application is started by Executive Services.
**
**  Arguments:
**    none
**
**  Return:
**    CFE_SUCCESS if no errors, otherwise this function returns the error code
**    that was received from the function that detected the error.
**    
*/
int32 CFE_SB_AppInit(void){

    uint32 CfgFileEventsToFilter = 0;    
    uint32 *TmpPtr = NULL;
    int32  Status = CFE_SUCCESS;
    
    Status = CFE_ES_RegisterApp();

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /* Get the assigned Application ID for the SB Task */
    CFE_ES_GetAppID(&CFE_SB.AppId);

    /* Process the platform cfg file events to be filtered */
    if(CFE_SB_FILTERED_EVENT1 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT1;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0xFFFF;      
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 4;
      CfgFileEventsToFilter++;
    }/* end if */           

    if(CFE_SB_FILTERED_EVENT2 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT2;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0xFFFF;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 4;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT3 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT3;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0xFFFF;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 16;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT4 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT4;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0xFFFF;      
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 16;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT5 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT5;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 0;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT6 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT6;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 0;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT7 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT7;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 0;
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_SB_FILTERED_EVENT8 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT8;
      CFE_SB.EventFilters[CfgFileEventsToFilter].RateLimit = 0;
      CFE_SB.EventFilters[CfgFileEventsToFilter].BurstLimit = 0;
      CfgFileEventsToFilter++;
    }/* end if */
    
    /* Be sure the number of events to register for filtering   
    ** does not exceed CFE_EVS_MAX_EVENT_FILTERS */
    if(CFE_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
      CfgFileEventsToFilter = CFE_EVS_MAX_EVENT_FILTERS;
    }/* end if */


    /* Register event filter table... */
    Status = CFE_EVS_Register(CFE_SB.EventFilters,
                              CfgFileEventsToFilter,
                              CFE_EVS_BINARY_FILTER);
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    CFE_ES_WriteToSysLog("SB:Registered %d events for filtering\n",(int)CfgFileEventsToFilter);

    CFE_SB_InitMsg(&CFE_SB.HKTlmMsg.Hdr.Pri,
                   CFE_SB_HK_TLM_MID,
                   sizeof(CFE_SB_HKMsg_t),
                   TRUE);

    CFE_SB_InitMsg(&CFE_SB.PrevSubMsg.Hdr.Pri,
                   CFE_SB_ALLSUBS_TLM_MID,
                   sizeof(CFE_SB_PrevSubMsg_t),
                   TRUE);

    CFE_SB_InitMsg(&CFE_SB.SubRprtMsg.Hdr.Pri,
                   CFE_SB_ONESUB_TLM_MID,
                   sizeof(CFE_SB_SubRprtMsg_t),
                   TRUE);    

    /* Populate the fixed fields in the HK Tlm Msg */
    CFE_SB_SET_MEMADDR(CFE_SB.HKTlmMsg.Payload.MemPoolHandle, CFE_SB.Mem.PoolHdl);
    
    /* Populate the fixed fields in the Stat Tlm Msg */
    CFE_SB.StatTlmMsg.Payload.MaxMsgIdsAllowed = CFE_SB_MAX_MSG_IDS;
    CFE_SB.StatTlmMsg.Payload.MaxPipesAllowed  = CFE_SB_MAX_PIPES;
    CFE_SB.StatTlmMsg.Payload.MaxMemAllowed    = CFE_SB_BUF_MEMORY_BYTES;
    CFE_SB.StatTlmMsg.Payload.MaxPipeDepthAllowed = CFE_SB_MAX_PIPE_DEPTH;
    CFE_SB.StatTlmMsg.Payload.MaxSubscriptionsAllowed =
                    ((CFE_SB_MAX_MSG_IDS)*(CFE_SB_MAX_DEST_PER_PKT));
    
    Status = CFE_SB_CreatePipe(&CFE_SB.CmdPipe,
                                CFE_SB_CMD_PIPE_DEPTH,
                                CFE_SB_CMD_PIPE_NAME);
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */                                

    Status = CFE_SB_Subscribe(CFE_SB_CMD_MID,CFE_SB.CmdPipe);

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Subscribe to Cmds Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
        
    Status = CFE_SB_Subscribe(CFE_SB_SEND_HK_MID,CFE_SB.CmdPipe);

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Subscribe to HK Request Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
     
    /* Ensure a ground commanded reset does not get blocked if SB mem pool  */
    /* becomes fully configured (DCR6772) */
    Status = CFE_ES_GetPoolBuf((uint32 **)&TmpPtr, CFE_SB.Mem.PoolHdl,
                                        sizeof(CFE_ES_RestartCmd_t));    

    if(Status < 0){
      CFE_ES_WriteToSysLog("SB:Init error, GetPool Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /* Return mem block used on previous call,the actual memory is not needed.*/
    /* The SB mem pool is now configured with a block size for the reset cmd. */
    Status = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)TmpPtr);

    if(Status < 0){
      CFE_ES_WriteToSysLog("SB:Init error, PutPool Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */    
    
    Status = CFE_EVS_SendEvent(CFE_SB_INIT_EID,
                              CFE_EVS_INFORMATION,
                              "cFE SB Initialized");
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Error sending init event:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
                      
    return CFE_SUCCESS;                      

}/* end CFE_SB_AppInit */




/******************************************************************************
**  Function:  CFE_SB_ProcessCmdPipePkt()
**
**  Purpose:
**    Function to control actions when an SB command is received.
**
**  Arguments:
**    none
**
**  Return:
**    none
*/
void CFE_SB_ProcessCmdPipePkt(void){
   CFE_SB_MsgPayloadPtr_t Payload;
   Payload = &CFE_SB.CmdPipePktPtr->Byte[CFE_SB_CMD_HDR_SIZE];
   switch(CFE_SB_GetMsgId(CFE_SB.CmdPipePktPtr)){

      case CFE_SB_SEND_HK_MID:
         /* Note: Command counter not incremented for this command */
         CFE_SB_SendHKTlmPkt();
         break;

      case CFE_SB_CMD_MID:
         switch (CFE_SB_GetCmdCode(CFE_SB.CmdPipePktPtr)) {
            case CFE_SB_NOOP_CC:
               CFE_EVS_SendEvent(CFE_SB_CMD0_RCVD_EID,CFE_EVS_INFORMATION,
                                 "No-op Cmd Rcvd. cFE Version %d.%d.%d.%d",
                                 CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);
               CFE_SB.HKTlmMsg.Payload.CommandCnt++;
               break;

            case CFE_SB_RESET_CTRS_CC:
               /* Note: Command counter not incremented for this command */
               CFE_EVS_SendEvent(CFE_SB_CMD1_RCVD_EID,CFE_EVS_DEBUG,
                                 "Reset Counters Cmd Rcvd");
               CFE_SB_ResetCounters();
               break;

            case CFE_SB_SEND_SB_STATS_CC:
                CFE_SB_SendStats();
                break;

            case CFE_SB_SEND_ROUTING_INFO_CC:
                CFE_SB_ProcessSendRtgInfoCmd(Payload);
                break;

            case CFE_SB_ENABLE_ROUTE_CC:
                CFE_SB_EnableRoute(Payload);
                break;

            case CFE_SB_DISABLE_ROUTE_CC:
                CFE_SB_DisableRoute(Payload);
                break;

            case CFE_SB_SEND_PIPE_INFO_CC:
                CFE_SB_ProcessSendPipeInfoCmd(Payload);
                break;

            case CFE_SB_SEND_MAP_INFO_CC:
                CFE_SB_ProcessSendMapInfoCmd(Payload);
                break;

            case CFE_SB_SEND_PREV_SUBS_CC:
                CFE_SB_SendPrevSubs();
                break;

            case CFE_SB_ENABLE_SUB_REPORTING_CC:
                CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);
                break;

            case CFE_SB_DISABLE_SUB_REPORTING_CC:
                CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);
                break;

            default:
               CFE_EVS_SendEvent(CFE_SB_BAD_CMD_CODE_EID,CFE_EVS_ERROR,
                     "Invalid Cmd, Unexpected Command Code %d",
                     (int)CFE_SB_GetCmdCode(CFE_SB.CmdPipePktPtr));
               CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
               break;
         } /* end switch on cmd code */
         break;

         default:
            CFE_EVS_SendEvent(CFE_SB_BAD_MSGID_EID,CFE_EVS_ERROR,
                  "Invalid Cmd, Unexpected Msg Id: 0x%04x",
                  (unsigned int)CFE_SB_GetMsgId(CFE_SB.CmdPipePktPtr));
            CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
            break;

   } /* end switch on MsgId */

} /* end CFE_SB_ProcessCmdPipePkt */



/******************************************************************************
**  Function:  CFE_SB_SendHKTlmPkt()
**
**  Purpose:
**    Function to send the SB housekeeping packet.
**
**  Arguments:
**    none
**
**  Notes:
**    Command counter not incremented for this command
**
**  Return:
**    none
*/
void CFE_SB_SendHKTlmPkt(void){
    
    CFE_SB.HKTlmMsg.Payload.MemInUse        = CFE_SB.StatTlmMsg.Payload.MemInUse;
    CFE_SB.HKTlmMsg.Payload.UnmarkedMem     = CFE_SB_BUF_MEMORY_BYTES - CFE_SB.StatTlmMsg.Payload.PeakMemInUse;
    
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_SB.HKTlmMsg);
    CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.HKTlmMsg);

}/* end CFE_SB_SendHKTlmPkt */


/******************************************************************************
**  Function:  CFE_SB_ResetCounters()
**
**  Purpose:
**    Function to reset the SB housekeeping counters.
**
**  Arguments:
**    none
**
**  Notes:
**    Command counter not incremented for this command
**
**  Return:
**    none
*/
void CFE_SB_ResetCounters(void){

   CFE_SB.HKTlmMsg.Payload.CommandCnt          = 0;
   CFE_SB.HKTlmMsg.Payload.CmdErrCnt           = 0;
   CFE_SB.HKTlmMsg.Payload.NoSubscribersCnt    = 0;
   CFE_SB.HKTlmMsg.Payload.DupSubscriptionsCnt = 0;
   CFE_SB.HKTlmMsg.Payload.MsgSendErrCnt       = 0;
   CFE_SB.HKTlmMsg.Payload.MsgReceiveErrCnt    = 0;
   CFE_SB.HKTlmMsg.Payload.InternalErrCnt      = 0;
   CFE_SB.HKTlmMsg.Payload.CreatePipeErrCnt    = 0;
   CFE_SB.HKTlmMsg.Payload.SubscribeErrCnt     = 0;
   CFE_SB.HKTlmMsg.Payload.PipeOverflowErrCnt  = 0;
   CFE_SB.HKTlmMsg.Payload.MsgLimErrCnt        = 0;

}/* end CFE_SB_ResetCounters */


/******************************************************************************
**  Function:  CFE_SB_EnableRoute()
**
**  Purpose:
**    SB internal function to enable a specific route. A route is defined as a
**    MsgId/PipeId combination.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**
**  Return:
**    None
*/
void CFE_SB_EnableRoute(CFE_SB_MsgPayloadPtr_t Payload){

    CFE_SB_MsgId_t          MsgId;
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_DestinationD_t   *DestPtr;
    CFE_SB_EnRoutCmd_Payload_t      *CmdPtr;

    CmdPtr = (CFE_SB_EnRoutCmd_Payload_t *)Payload;

    MsgId  = CmdPtr->MsgId;
    PipeId = CmdPtr->Pipe;

    /* check cmd parameters */
    if((CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS)||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS))
    {
        CFE_EVS_SendEvent(CFE_SB_ENBL_RTE3_EID,CFE_EVS_ERROR,
                      "Enbl Route Cmd:Invalid Param.Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
        return;
    }/* end if */

    DestPtr = CFE_SB_GetDestPtr(MsgId, PipeId);
    if(DestPtr == NULL){
        CFE_EVS_SendEvent(CFE_SB_ENBL_RTE1_EID,CFE_EVS_ERROR,
                "Enbl Route Cmd:Route does not exist.Msg 0x%x,Pipe %d",MsgId,PipeId);
        CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
        return;
    }/* end if */

    DestPtr->Active = CFE_SB_ACTIVE;
    CFE_EVS_SendEvent(CFE_SB_ENBL_RTE2_EID,CFE_EVS_DEBUG,
                      "Enabling Route,Msg 0x%x,Pipe %d",MsgId,PipeId);

    CFE_SB.HKTlmMsg.Payload.CommandCnt++;

}/* end CFE_SB_EnableRoute */



/******************************************************************************
**  Function:  CFE_SB_DisableRoute()
**
**  Purpose:
**    SB internal function to disable a specific route. A route is defined as a
**    MsgId/PipeId combination.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**
**  Return:
**    None
*/
void CFE_SB_DisableRoute(CFE_SB_MsgPayloadPtr_t Payload){

    CFE_SB_MsgId_t          MsgId;
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_DestinationD_t   *DestPtr;
    CFE_SB_EnRoutCmd_Payload_t      *CmdPtr;

    CmdPtr = (CFE_SB_EnRoutCmd_Payload_t *)Payload;

    MsgId  = CmdPtr->MsgId;
    PipeId = CmdPtr->Pipe;

    /* check cmd parameters */
    if((CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS)||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS)){
        CFE_EVS_SendEvent(CFE_SB_DSBL_RTE3_EID,CFE_EVS_ERROR,
                   "Disable Route Cmd:Invalid Param.Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
        return;
    }/* end if */

    DestPtr = CFE_SB_GetDestPtr(MsgId, PipeId);
    if(DestPtr == NULL){
        CFE_EVS_SendEvent(CFE_SB_DSBL_RTE1_EID,CFE_EVS_ERROR,
            "Disable Route Cmd:Route does not exist,Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
        return;
    }/* end if */

    DestPtr->Active = CFE_SB_INACTIVE;

    CFE_EVS_SendEvent(CFE_SB_DSBL_RTE2_EID,CFE_EVS_DEBUG,
                      "Route Disabled,Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
    CFE_SB.HKTlmMsg.Payload.CommandCnt++;

}/* end CFE_SB_DisableRoute */


/******************************************************************************
**  Function:  CFE_SB_SendStats()
**
**  Purpose:
**    SB internal function to send a Software Bus statistics packet
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_SendStats(void){

    CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.StatTlmMsg);

    CFE_EVS_SendEvent(CFE_SB_SND_STATS_EID,CFE_EVS_DEBUG,
                      "Software Bus Statistics packet sent");

    CFE_SB.HKTlmMsg.Payload.CommandCnt++;

}/* CFE_SB_SendStats */


/******************************************************************************
**  Function:  CFE_SB_ProcessSendRtgInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Routing Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_ProcessSendRtgInfoCmd(CFE_SB_MsgPayloadPtr_t Payload){

    CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = (CFE_SB_WriteFileInfoCmd_Payload_t *)Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_SB_DEFAULT_ROUTING_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendRtgInfo(LocalFilename);
    CFE_SB_IncrCmdCtr(Stat);

}/* end CFE_SB_ProcessSendRtgInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_ProcessSendPipeInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Pipe Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_ProcessSendPipeInfoCmd(CFE_SB_MsgPayloadPtr_t Payload){

    CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = (CFE_SB_WriteFileInfoCmd_Payload_t *)Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_SB_DEFAULT_PIPE_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendPipeInfo(LocalFilename);
    CFE_SB_IncrCmdCtr(Stat);

}/* end CFE_SB_ProcessSendPipeInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_ProcessSendMapInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Map Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_ProcessSendMapInfoCmd(CFE_SB_MsgPayloadPtr_t Payload){

    CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = (CFE_SB_WriteFileInfoCmd_Payload_t *)Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_SB_DEFAULT_MAP_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendMapInfo(LocalFilename);

    CFE_SB_IncrCmdCtr(Stat);

}/* end CFE_SB_ProcessSendMapInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_SendRoutingInfo()
**
**  Purpose:
**    SB internal function to write the routing information to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendRtgInfo(const char *Filename){

    CFE_SB_MsgId_t              RtgTblIdx = 0;
    CFE_SB_MsgId_t              i;
    int32                       fd = 0;
    int32                       WriteStat;
    uint32                      FileSize = 0;
    uint32                      EntryCount = 0;
    CFE_SB_RoutingFileEntry_t   Entry;
    CFE_FS_Header_t             FileHdr;
    CFE_SB_PipeD_t              *pd; 
    CFE_SB_DestinationD_t       *DestPtr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_ERROR,
                      "Error creating file %s, stat=0x%x",
                      Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Routing Information", CFE_FS_SB_ROUTEDATA_SUBTYPE);

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the entire MsgMap */
    for(i=0;i<CFE_SB_HIGHEST_VALID_MSGID;i++){

        RtgTblIdx = CFE_SB.MsgMap[i];

        /* Only process table entry if it is used. */
        if(RtgTblIdx == CFE_SB_AVAILABLE){
            DestPtr = NULL;
        } else {
            DestPtr = CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr;
        }

        while(DestPtr != NULL){

            pd = CFE_SB_GetPipePtr(DestPtr -> PipeId);
            /* If invalid id, continue on to next entry */
            if (pd != NULL) {
            
                Entry.MsgId     = CFE_SB.RoutingTbl[RtgTblIdx].MsgId;
                Entry.PipeId    = DestPtr -> PipeId;
                Entry.State     = DestPtr -> Active;
                Entry.MsgCnt    = DestPtr -> DestCnt;               
            
                Entry.AppName[0] = 0;
                /* 
                 * NOTE: as long as CFE_ES_GetAppName() returns success, then it 
                 * guarantees null termination of the output.  Return code is not
                 * checked here (bad) but in case of error it does not seem to touch
                 * the buffer, therefore the initialization above will protect for now 
                 */
                CFE_ES_GetAppName(&Entry.AppName[0], pd->AppId, sizeof(Entry.AppName));
                strncpy(&Entry.PipeName[0],CFE_SB_GetPipeName(Entry.PipeId),sizeof(Entry.PipeName));

                WriteStat = OS_write (fd, &Entry, sizeof(CFE_SB_RoutingFileEntry_t));
                if(WriteStat != sizeof(CFE_SB_RoutingFileEntry_t)){
                    CFE_SB_FileWriteByteCntErr(Filename,
                                           sizeof(CFE_SB_RoutingFileEntry_t),
                                           WriteStat);
                    OS_close(fd);
                    return CFE_SB_FILE_IO_ERR;
                }/* end if */

                FileSize += WriteStat;
                EntryCount ++;
            }
            
            DestPtr = DestPtr->Next;

        }/* end while */

    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendRtgInfo */


/******************************************************************************
**  Function:  CFE_SB_SendPipeInfo()
**
**  Purpose:
**    SB internal function to write the Pipe table to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendPipeInfo(const char *Filename){

    uint16 i;
    int32  fd = 0;
    int32  WriteStat;
    uint32 FileSize = 0;
    uint32 EntryCount = 0;
    CFE_FS_Header_t FileHdr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);

    if(fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_ERROR,
                          "Error creating file %s, stat=0x%x",
                           Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Pipe Information", CFE_FS_SB_PIPEDATA_SUBTYPE);
    
    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the pipe table */
    for(i=0;i<CFE_SB_MAX_PIPES;i++){

        if(CFE_SB.PipeTbl[i].InUse==CFE_SB_IN_USE){

            WriteStat = OS_write (fd, &(CFE_SB.PipeTbl[i]), sizeof(CFE_SB_PipeD_t));
            if(WriteStat != sizeof(CFE_SB_PipeD_t)){
                CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_SB_PipeD_t),WriteStat);
                OS_close(fd);
                return CFE_SB_FILE_IO_ERR;
            }/* end if */

            FileSize += WriteStat;
            EntryCount ++;

        }/* end if */

    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                       Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendPipeInfo */


/******************************************************************************
**  Function:  CFE_SB_SendMapInfo()
**
**  Purpose:
**    SB internal function to write the Message Map to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendMapInfo(const char *Filename){

    uint16 i;
    int32  fd = 0;
    int32  WriteStat;
    uint32 FileSize = 0;
    uint32 EntryCount = 0;
    CFE_SB_MsgMapFileEntry_t Entry;
    CFE_FS_Header_t FileHdr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);

    if (fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_ERROR,
                          "Error creating file %s, stat=0x%x",
                           Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Message Map Information", CFE_FS_SB_MAPDATA_SUBTYPE);

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the entire MsgMap */
    for(i=0;i<CFE_SB_HIGHEST_VALID_MSGID;i++){

        if(CFE_SB.MsgMap[i] != CFE_SB_AVAILABLE){

            Entry.MsgId = i;
            Entry.Index = CFE_SB.MsgMap[i];

            WriteStat = OS_write (fd, &Entry, sizeof(CFE_SB_MsgMapFileEntry_t));
            if(WriteStat != sizeof(CFE_SB_MsgMapFileEntry_t)){
                CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_SB_MsgMapFileEntry_t),WriteStat);
                OS_close(fd);
                return CFE_SB_FILE_IO_ERR;
            }/* end if */

            FileSize += WriteStat;
            EntryCount ++;

        }/* end for */
    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendMapInfo */



/******************************************************************************
**  Function:  CFE_SB_SendPrevSubs()
**
**  Purpose:
**    SB function to build and send an SB packet containing a complete list of
**    current subscriptions.Intended to be used primarily for the Software Bus
**    Networking Application (SBN).
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_SendPrevSubs(void){

  uint32 i;
  uint32 EntryNum = 0;
  uint32 SegNum = 1;
  int32  Stat;
  CFE_SB_DestinationD_t *DestPtr = NULL;

  /* Take semaphore to ensure data does not change during this function */
  CFE_SB_LockSharedData(__func__,__LINE__);

  /* seek msgids that are in use */
  for(i=0;i<CFE_SB_MAX_MSG_IDS;i++){

        if(CFE_SB.RoutingTbl[i].MsgId == CFE_SB_INVALID_MSG_ID) {
            DestPtr = NULL;
        } else {
            DestPtr = CFE_SB.RoutingTbl[i].ListHeadPtr;
        }
        
        while(DestPtr != NULL){

            if(DestPtr->Scope == CFE_SB_GLOBAL){
            
                /* ...add entry into pkt */
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].MsgId = CFE_SB.RoutingTbl[i].MsgId;
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].Qos.Priority = 0;
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].Qos.Reliability = 0;
                EntryNum++;
        
                /* send pkt if full */
                if(EntryNum >= CFE_SB_SUB_ENTRIES_PER_PKT){
                  CFE_SB.PrevSubMsg.Payload.PktSegment = SegNum;
                  CFE_SB.PrevSubMsg.Payload.Entries = EntryNum;
                  CFE_SB_UnlockSharedData(__func__,__LINE__);
                  Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.PrevSubMsg);
                  CFE_SB_LockSharedData(__func__,__LINE__);
                  CFE_EVS_SendEvent(CFE_SB_FULL_SUB_PKT_EID,CFE_EVS_DEBUG,
                      "Full Sub Pkt %d Sent,Entries=%d,Stat=0x%x\n",(int)SegNum,(int)EntryNum,(unsigned int)Stat);
                  EntryNum = 0;
                  SegNum++;
                }/* end if */
        
                /* break while loop through destinations, onto next CFE_SB.RoutingTbl index */
                /* This is done because we want only one network subscription per msgid */
                /* Later when Qos is used, we may want to take just the highest priority */
                /* subscription if there are more than one */
                break;
                
            }/* end if */
            
            /* Check next destination (if another exists) for global scope */
            DestPtr = DestPtr -> Next;
        
        }/* end while */
  
  }/* end for */ 

  /* if pkt has any number of entries, send it as a partial pkt */
  if(EntryNum > 0){
    CFE_SB.PrevSubMsg.Payload.PktSegment = SegNum;
    CFE_SB.PrevSubMsg.Payload.Entries = EntryNum;
    CFE_SB_UnlockSharedData(__func__,__LINE__);
    Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.PrevSubMsg);
    CFE_SB_LockSharedData(__func__,__LINE__);
    CFE_EVS_SendEvent(CFE_SB_PART_SUB_PKT_EID,CFE_EVS_DEBUG,
        "Partial Sub Pkt %d Sent,Entries=%d,Stat=0x%x",(int)SegNum,(int)EntryNum,(unsigned int)Stat);
  }/* end if */

  CFE_SB_UnlockSharedData(__func__,__LINE__);
  return;

}/* end CFE_SB_SendPrevSubs */


/******************************************************************************
**  Function:  CFE_SB_FindGlobalMsgIdCnt()
**
**  Purpose:
**    SB internal function to get a count of the global message ids in use.
**
**  Notes:
**    Subscriptions made with CFE_SB_SubscribeLocal would not be counted.
**    Subscription made with a subscribe API other than CFE_SB_SubscribeLocal are
**    considerd to be global subscriptions. MsgIds with both global and local
**    subscriptions would be counted.
**
**  Arguments:
**
**  Return:
**    None
*/
uint32 CFE_SB_FindGlobalMsgIdCnt(void){

    uint32 i;
    uint32 cnt = 0;
    CFE_SB_DestinationD_t *DestPtr = NULL;
    
    for(i=0;i<CFE_SB_MAX_MSG_IDS;i++){

        if(CFE_SB.RoutingTbl[i].MsgId == CFE_SB_INVALID_MSG_ID) {
            DestPtr = NULL;
        } else {
            DestPtr = CFE_SB.RoutingTbl[i].ListHeadPtr;
        }
        
        while(DestPtr != NULL){
    
            if(DestPtr->Scope == CFE_SB_GLOBAL){

                cnt++;
                break;

            }/* end if */
            
            /* Check next destination (if another exists) for global scope */
            DestPtr = DestPtr -> Next;
            
        }/* end while */

    }/* end for */

  return cnt;

}/* end CFE_SB_FindGlobalMsgIdCnt */




/******************************************************************************
**  Function:  CFE_SB_IncrCmdCtr()
**
**  Purpose:
**    SB internal function to increment the proper cmd counter based on the
**    status input. This small utility was written to eliminate duplicate code.
**
**  Arguments:
**    status - typically CFE_SUCCESS or an SB error code
**
**  Return:
**    None
*/
void CFE_SB_IncrCmdCtr(int32 status){

    if(status==CFE_SUCCESS){
      CFE_SB.HKTlmMsg.Payload.CommandCnt++;
    }else{
      CFE_SB.HKTlmMsg.Payload.CmdErrCnt++;
    }/* end if */

}/* end CFE_SB_IncrCmdCtr */



/******************************************************************************
**  Function:  CFE_SB_FileWriteByteCntErr()
**
**  Purpose:
**    SB internal function to report a file write error
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CFE_SB_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual){

    CFE_EVS_SendEvent(CFE_SB_FILEWRITE_ERR_EID,CFE_EVS_ERROR,
                      "File write,byte cnt err,file %s,request=%d,actual=%d",
                       Filename,(int)Requested,(int)Actual);

}/* end CFE_SB_FileWriteByteCntErr() */


/******************************************************************************
**  Function:  CFE_SB_SetSubscriptionReporting()
**
**  Purpose:
**    SB internal function to enable and disable subscription reporting.
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CFE_SB_SetSubscriptionReporting(uint32 state){

    CFE_SB.SubscriptionReporting = state;

}/* end CFE_SB_SetSubscriptionReporting */





