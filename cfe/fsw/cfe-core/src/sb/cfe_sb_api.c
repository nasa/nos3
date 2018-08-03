/******************************************************************************
** File: cfe_sb_api.c
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
**      This file contains the source code for the SB API's.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_api.c  $
** Revision 1.40 2014/08/22 16:41:25GMT-05:00 lwalling 
** Change signed loop counters to unsigned
** Revision 1.39 2014/06/17 13:42:34EDT rmcgraw 
** DCR18686:1 Initialized all AppId and CallerID's with 0xFFFFFFFF
** Revision 1.38 2014/05/05 14:47:16EDT rmcgraw
** DCR19498:1 Fixed buffers in use count, was going negative for zero copy transfers
** Revision 1.37 2014/05/05 13:53:09EDT rmcgraw
** DCR19578:1 - Add UnlockSharedData for errors in ZeroCopyReleaseDesc
** Revision 1.36 2014/04/24 10:57:07EDT rmcgraw
** DCR19487:1 - Remove size argument in CFE_SB_GetBufferFromCaller
** Revision 1.35 2012/07/09 15:00:18EDT rmcgraw
** DCR19413:1 Release buf mem for zero cpy send, no subscribers and error cases
** Revision 1.34 2012/01/13 12:15:11EST acudmore
** Changed license text to reflect open source
** Revision 1.33 2012/01/06 09:27:19EST rmcgraw
** DCR16100:1 Added perf markers for msg lim and pipe overflow errors
** Revision 1.32 2011/12/20 10:25:30GMT-05:00 rmcgraw
** DCR15187:1 Removed function CFE_SB_DecrMsgLimCnt and MsgLim Counter error event
** Revision 1.31 2011/12/09 12:22:56EST rmcgraw
** DCR15763:1 Added call to LockSharedData for case of invalid pipe id rcvd in RcvMsg API
** Revision 1.30 2011/09/09 14:25:33EDT aschoeni
** Added fix for ZeroCopy issues
** Revision 1.29 2011/04/29 10:04:48EDT rmcgraw
** Add check for NULL ptr in send loop
** Revision 1.28 2011/04/28 10:37:31EDT rmcgraw
** Fixed compiler error
** Revision 1.27 2011/04/28 10:10:15EDT rmcgraw
** DCR14592:1 Fix for the SB Send semaphore problem
** Revision 1.26 2010/11/04 16:41:53EDT aschoeni
** Added optional sender information storage
** Revision 1.25 2010/11/04 14:27:15EDT aschoeni
** Optimized rcv function
** Revision 1.24 2010/11/03 15:07:01EDT jmdagost
** Added cfe.h include file.
** Revision 1.23 2009/07/29 19:23:26EDT aschoeni
** Updated for ZeroCopyHandle_t and GetPoolBufInfo
** Revision 1.22 2009/07/24 18:27:10EDT aschoeni
** Added Zero Copy Mode
** Revision 1.21 2009/07/20 14:10:54EDT aschoeni
** Made GetAppTskName reentrant
** Revision 1.20 2009/07/17 19:43:01EDT aschoeni
** Added PassMsg API to sb to support sequence count preservation
** Revision 1.19 2009/06/26 17:02:07EDT aschoeni
** Updated SB to use __func__ instead of __FILE__ for lock and unlock errors
** Revision 1.18 2009/06/10 09:17:57EDT acudmore
** Updated OS_Mem* and OS_BSP* to CFE_PSP_*
** Revision 1.17 2009/05/08 11:27:08EDT rmcgraw
** DCR7631:1 Removed index checks in Subscribe and Unsubscribe - dead code and cannot be tested
** Revision 1.16 2009/04/29 10:04:33EDT rmcgraw
** DCR5801:11 Changed subscription return value when buf allocation fails
** Revision 1.15 2009/03/30 10:45:01EDT rmcgraw
** DCR5801:2 Fixed infinite loop when deleting a pipe with subscriptions
** Revision 1.14 2009/02/26 17:57:39EST rmcgraw
** DCR6805:1 Corrected note under SendMsg API prolog regarding seq count
** Revision 1.13 2009/02/10 16:53:23EST rmcgraw
** DCR1700:1 Decrement pipes-in-use counter in DeletePipe API
** Revision 1.12 2009/02/10 12:29:07EST rmcgraw
** DCR6956:1 In Create Pipe API, set user's pipe id to invalid for error cases
** Revision 1.11 2009/02/10 11:25:56EST rmcgraw
** DCR1699:1 Removed pipe owner in five events
** Revision 1.10 2009/02/06 15:35:08EST rmcgraw
** DCR5801:3 Moved Total Msg Size test before the lock in the send api
** Revision 1.9 2009/02/06 11:29:04EST rmcgraw
** DCR5801:2 General Cleanup
** Revision 1.8 2009/02/03 16:33:29EST rmcgraw
** DCR5801:2 Fixed DestPtr errors in Send API
** Revision 1.7 2009/02/03 11:06:58EST rmcgraw
** DCR5801:2 Changed destination desciptors from array based to linked list
** Revision 1.6 2009/01/30 12:27:07EST rmcgraw
** DCR5801:5 Removed call to GetPipePtr in send API inside loop
** Revision 1.5 2009/01/30 11:50:07EST rmcgraw
** DCR5801:8 Set users ptr to null only when readqueue fails in rcv API
** Revision 1.4 2009/01/30 11:45:06EST rmcgraw
** DCR5801:9 Removed unused calls to GetAppID in send and rcv APIs
** Revision 1.3 2009/01/30 11:07:03EST rmcgraw
** DCR5801:3 Moved the semaphore lock in the send and rcv API's
** Revision 1.2 2009/01/30 10:34:57EST rmcgraw
** DCR5801:1 Began tracking subsribers with 'destinations' counter.
** Revision 1.1 2008/04/17 08:05:30EDT ruperera
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.74 2007/09/19 14:38:48EDT rjmcgraw
** DCR4421 Removed use count err events and reworded buf allocation error event
** Revision 1.73 2007/09/19 12:01:46EDT rjmcgraw
** Return code in unsubscribe changed from no subscribers to cfe success
** Revision 1.72 2007/09/13 09:34:09EDT rjmcgraw
** DCR4861:2 Added check in SendMsg to prevent recursive event problem
** Revision 1.71 2007/08/17 15:59:51EDT rjmcgraw
** Changes to free semaphore before calling SendEventWithAppId
** Revision 1.70 2007/07/12 16:58:22EDT rjmcgraw
** DCR4680:1 Removed SB event log related items
** Revision 1.69 2007/07/06 13:34:17EDT rjmcgraw
** DCR469:1 Added GetLastSender code
** Revision 1.68 2007/04/24 11:07:18EDT rjmcgraw
** Added PipeId to PipeDepthStats
** Revision 1.67 2007/04/24 09:34:04EDT rjmcgraw
** DCR3294 - Added pipe depth stats to several SB api's
** Revision 1.66 2007/03/28 14:17:37EST rjmcgraw
** Sending TskId instead of AppId in call to LogSBEvent - DCR2654
** Revision 1.65 2007/03/27 13:42:28EST rjmcgraw
** Added step to increment new duplicate subscription counter
**
******************************************************************************/

/*
** Include Files
*/
#include "common_types.h"
#include "private/cfe_private.h"
#include "cfe_sb_events.h"
#include "cfe_sb_priv.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_error.h"
#include <string.h>

/* 
 * Macro to reflect size of PipeDepthStats Telemetry array - 
 * this may or may not be the same as CFE_SB_MSG_MAX_PIPES
 */
#define CFE_SB_TLM_PIPEDEPTHSTATS_SIZE     (sizeof(CFE_SB.StatTlmMsg.Payload.PipeDepthStats) / sizeof(CFE_SB.StatTlmMsg.Payload.PipeDepthStats[0]))
/******************************************************************************
** Name:    CFE_SB_CreatePipe
**
** Purpose: API to create a pipe for receiving messages
**
** Assumptions, External Events, and Notes:
**
**          Note: Zero is a valid PipeId
**
** Date Written:
**          04/25/2005
**
** Inputs:
**          PipeIdPtr - Ptr to users empty PipeId variable, to be filled by
**                      this function.
**          Depth     - The depth of the pipe, synonymous to the max number
**                      of messages the pipe can hold at any time.
**          PipeName  - The name of the pipe displayed in event messages
**
** Outputs:
**          PipeId    - The handle of the pipe to be used when receiving
**                      messages.
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, const char *PipeName)
{
    uint32          AppId = 0xFFFFFFFF;
    uint32          TskId = 0;
    uint32          SysQueueId = 0;
    int32           Status;
    CFE_SB_PipeId_t PipeTblIdx;
    char            AppName[OS_MAX_API_NAME] = {'\0'};
    char            FullName[(OS_MAX_API_NAME * 2)];

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    TskId = OS_TaskGetId();

    /* set user's pipe id value to 'invalid' for error cases below */
    if(PipeIdPtr != NULL){
        *PipeIdPtr = CFE_SB_INVALID_PIPE;
    }/* end if */

    /* check input parameters */
    if((PipeIdPtr == NULL)||(Depth > CFE_SB_MAX_PIPE_DEPTH)||(Depth == 0)){
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_BAD_ARG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "CreatePipeErr:Bad Input Arg:app=%s,ptr=0x%lx,depth=%d,maxdepth=%d",
                CFE_SB_GetAppTskName(TskId,FullName),(unsigned long)PipeIdPtr,Depth,CFE_SB_MAX_PIPE_DEPTH);
        return CFE_SB_BAD_ARGUMENT;
    }/*end if*/

    /* get first available entry in pipe table */
    PipeTblIdx = CFE_SB_GetAvailPipeIdx();

    /* if pipe table is full, send event and return error */
    if(PipeTblIdx == CFE_SB_INVALID_PIPE){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MAX_PIPES_MET_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "CreatePipeErr:Max Pipes(%d)In Use.app %s",
          CFE_SB_MAX_PIPES,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_MAX_PIPES_MET;
    }/* end if */

    /* create the queue */
    Status = OS_QueueCreate(&SysQueueId,PipeName,Depth,sizeof(CFE_SB_BufferD_t *),0);
    if (Status != OS_SUCCESS) {
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
                "CreatePipeErr:OS_QueueCreate returned %d,app %s",
                (int)Status,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_PIPE_CR_ERR;
    }/* end if */

    /* get callers AppId */
    CFE_ES_GetAppID(&AppId);

    /* get callers name */
    CFE_ES_GetAppName(AppName, AppId, OS_MAX_API_NAME);

    /* Hardcode a NULL terminator, in case rcvd name was too long */
    AppName[OS_MAX_API_NAME-1]= '\0';

    /* fill in the pipe table fields */
    CFE_SB.PipeTbl[PipeTblIdx].InUse       = CFE_SB_IN_USE;
    CFE_SB.PipeTbl[PipeTblIdx].SysQueueId  = SysQueueId;
    CFE_SB.PipeTbl[PipeTblIdx].PipeId      = PipeTblIdx;
    CFE_SB.PipeTbl[PipeTblIdx].QueueDepth  = Depth;
    CFE_SB.PipeTbl[PipeTblIdx].AppId       = AppId;
    CFE_SB.PipeTbl[PipeTblIdx].SendErrors  = 0;
    CFE_SB.PipeTbl[PipeTblIdx].CurrentBuff = NULL;
    CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff = NULL;
    strcpy(&CFE_SB.PipeTbl[PipeTblIdx].AppName[0],&AppName[0]);
    strncpy(&CFE_SB.PipeTbl[PipeTblIdx].PipeName[0],PipeName,OS_MAX_API_NAME);
    CFE_SB.PipeTbl[PipeTblIdx].PipeName[OS_MAX_API_NAME-1]='\0';

    /* Increment the Pipes in use ctr and if it's > the high water mark,*/
    /* adjust the high water mark */
    CFE_SB.StatTlmMsg.Payload.PipesInUse++;
    if(CFE_SB.StatTlmMsg.Payload.PipesInUse > CFE_SB.StatTlmMsg.Payload.PeakPipesInUse){
       CFE_SB.StatTlmMsg.Payload.PeakPipesInUse = CFE_SB.StatTlmMsg.Payload.PipesInUse;
    }/* end if */

    /* Reset the pipe depth parameters in the statistics pkt */
    if (PipeTblIdx < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
    {
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PipeId = PipeTblIdx;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].Depth = Depth;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].InUse = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PeakInUse = 0;
    }

    /* give the pipe handle to the caller */
    *PipeIdPtr = PipeTblIdx;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /* send debug event */
    CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_ADDED_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
          "Pipe Created:name %s,id %d,app %s",
          CFE_SB_GetPipeName(CFE_SB.PipeTbl[PipeTblIdx].PipeId),
          (int)CFE_SB.PipeTbl[PipeTblIdx].PipeId,
          CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_CreatePipe */


/******************************************************************************
**  Function:  CFE_SB_DeletePipe()
**
**  Purpose:
**    Will unsubscribe to all routes associated with the given pipe id, then remove
**    pipe from the pipe table.
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_DeletePipeFull(PipeId,CallerId);

    return Status;

}/* end CFE_SB_DeletePipe */



/******************************************************************************
**  Function:  CFE_SB_DeletePipeWithAppId()
**
**  Purpose:
**
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, uint32 AppId)
{
    int32   Status = 0;

    Status = CFE_SB_DeletePipeFull(PipeId,AppId);

    return Status;

}/* end CFE_SB_DeletePipeWithAppId */



/******************************************************************************
**  Function:  CFE_SB_DeletePipeFull()
**
**  Purpose:
**    Will unsubscribe to all routes associated with the given pipe id, then remove
**    pipe from the pipe table.
**
**  NOTE:This function cannot be called directly, it would not be semaphore protected
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,uint32 AppId)
{
    uint8         PipeTblIdx;
    int32         RtnFromVal,Stat;
    uint32        Owner,i;
    uint32        TskId = 0;
    CFE_SB_Msg_t  *PipeMsgPtr;
    CFE_SB_DestinationD_t *DestPtr = NULL;
    char          FullName[(OS_MAX_API_NAME * 2)];

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get TaskId of caller for events */
    TskId = OS_TaskGetId();

    /* check input parameter */
    PipeTblIdx = CFE_SB_GetPipeIdx(PipeId);
    RtnFromVal = CFE_SB_ValidatePipeId(PipeId);
    if((RtnFromVal != CFE_SUCCESS)||(PipeTblIdx == CFE_SB_INVALID_PIPE))
    {
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR1_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Bad Argument,PipedId %d,Requestor %s,Idx %d,Stat %d",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)PipeTblIdx,(int)RtnFromVal);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    Owner = CFE_SB.PipeTbl[PipeTblIdx].AppId;

    /* check that the given AppId is the owner of the pipe */
    if(AppId != Owner){
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR2_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Caller(%s) is not the owner of pipe %d",
          CFE_SB_GetAppTskName(TskId,FullName),PipeId);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check destination list of every in-use MsgId, for the given pipeid. */
    /* when found, remove the pipe ID from the destination list via 'unsubscribe' */
    for(i=0;i<CFE_SB_MAX_MSG_IDS;i++){
        if(CFE_SB.RoutingTbl[i].MsgId != CFE_SB_INVALID_MSG_ID){

            DestPtr = CFE_SB.RoutingTbl[i].ListHeadPtr;

            while(DestPtr != NULL){

                if(DestPtr -> PipeId == PipeId){
                    /* release the semaphore, unsubscribe will need to take it */
                    CFE_SB_UnlockSharedData(__func__,__LINE__);
                    CFE_SB_UnsubscribeWithAppId(CFE_SB.RoutingTbl[i].MsgId,
                                       PipeId,AppId);
                    CFE_SB_LockSharedData(__func__,__LINE__);
                }/* end if */

                DestPtr = DestPtr -> Next;

            }/* end while */

        }/* end if */
    }/* end for */

    if (CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff != NULL) {

        /* Decrement the Buffer Use Count and Free buffer if cnt=0) */
        CFE_SB_DecrBufUseCnt(CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff);
        CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff = NULL;

    }/* end if */


    /* remove any messages that might be on the pipe */
    /* this step will free the memory used to store the message */
    do{
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      Stat = CFE_SB_RcvMsg(&PipeMsgPtr,PipeId,CFE_SB_POLL);
      CFE_SB_LockSharedData(__func__,__LINE__);
    }while(Stat == CFE_SUCCESS);

    /* Delete the underlying OS queue */
    OS_QueueDelete(CFE_SB.PipeTbl[PipeTblIdx].SysQueueId);

    /* remove the pipe from the pipe table */
    CFE_SB.PipeTbl[PipeTblIdx].InUse         = CFE_SB_NOT_IN_USE;
    CFE_SB.PipeTbl[PipeTblIdx].SysQueueId    = CFE_SB_UNUSED_QUEUE;
    CFE_SB.PipeTbl[PipeTblIdx].PipeId        = CFE_SB_INVALID_PIPE;
    CFE_SB.PipeTbl[PipeTblIdx].CurrentBuff   = NULL;
    CFE_PSP_MemSet(&CFE_SB.PipeTbl[PipeTblIdx].PipeName[0],0,OS_MAX_API_NAME);

    /* zero out the pipe depth stats */
    if (PipeTblIdx < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
    {
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PipeId = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].Depth = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].InUse = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PeakInUse = 0;
    }

    CFE_SB.StatTlmMsg.Payload.PipesInUse--;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_DELETED_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
          "Pipe Deleted:id %d,owner %s",PipeId, CFE_SB_GetAppTskName(Owner,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_DeletePipeFull */


/******************************************************************************
** Name:    CFE_SB_SubscribeEx
**
** Purpose: API to globally subscribe to a message when QOS and MsgLim defaults
**          are insufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**          Note: MsgLim default: 4, see Input Arguments below for more detail
**          Note: Qos parameter is currently unused by the fsw. It is recommended
**                to give a value of zero for Quality.Priority and Quality.Reliability
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          Quality - Quality of Service (Qos) - priority and reliability,
**                    see note above for recommended settings
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SubscribeEx(CFE_SB_MsgId_t   MsgId,
                          CFE_SB_PipeId_t  PipeId,
                          CFE_SB_Qos_t     Quality,
                          uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,Quality,MsgLim,(uint8)CFE_SB_GLOBAL);

}/* end CFE_SB_SubscribeEx */




/******************************************************************************
** Name:    CFE_SB_SubscribeLocal
**
** Purpose: CFE Internal API to locally subscribe to a message when QOS and
**          MsgLim defaults are insufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**                MsgLim default: 4 - meaning at most 4 messages of given
**                                    MsgId allowed on the given PipeId.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t   MsgId,
                            CFE_SB_PipeId_t  PipeId,
                            uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,CFE_SB_Default_Qos,MsgLim,
                                (uint8)CFE_SB_LOCAL);

}/* end CFE_SB_SubscribeLocal */




/******************************************************************************
** Name:    CFE_SB_Subscribe
**
** Purpose: API to locally subscribe to a message when QOS and MsgLim defaults
**          are sufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**                MsgLim default: 4 - meaning at most 4 messages of given
**                                    MsgId allowed on the given PipeId.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t   MsgId,
                       CFE_SB_PipeId_t  PipeId)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,CFE_SB_Default_Qos,
                               (uint16)CFE_SB_DEFAULT_MSG_LIMIT,
                               (uint8)CFE_SB_GLOBAL);

}/* end CFE_SB_Subscribe */


/******************************************************************************
** Name:    CFE_SB_SubscribeFull
**
** Purpose: CFE Internal API used to subscribe to a message. Contains an input
**          parameter for all possible subscription choices. This function is
**          called by CFE_SB_SubscribeEx, CFE_SB_Subscribe and
**          CFE_SB_SubscribeLocal.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          Quality - Quality of Service (Qos) - priority and reliability
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**          Scope   - Local subscription or broadcasted to peers
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SubscribeFull(CFE_SB_MsgId_t   MsgId,
                            CFE_SB_PipeId_t  PipeId,
                            CFE_SB_Qos_t     Quality,
                            uint16           MsgLim,
                            uint8            Scope)
{
    uint16 Idx;
    int32  Stat;
    uint32 TskId = 0;
    uint32 AppId = 0xFFFFFFFF;
    uint8  PipeIdx;
    CFE_SB_DestinationD_t *DestBlkPtr = NULL;
    char   FullName[(OS_MAX_API_NAME * 2)];

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get task id for events */
    TskId = OS_TaskGetId();

    /* get the callers Application Id */
    CFE_ES_GetAppID(&AppId);

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrCnt++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_PIPE_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "Subscribe Err:Invalid Pipe Id,Msg=0x%x,PipeId=%d,App %s",MsgId,PipeId,
          CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check that the requestor is the owner of the pipe */
    if(CFE_SB.PipeTbl[PipeIdx].AppId != AppId){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrCnt++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_CALLER_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "Subscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
          CFE_SB_GetAppTskName(TskId,FullName),PipeId,MsgId);
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check message id and scope */
    if((CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS)||(Scope > 1))
    {
        CFE_SB.HKTlmMsg.Payload.SubscribeErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SUB_ARG_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "Subscribe Err:Bad Arg,MsgId 0x%x,PipeId %d,app %s,scope %d",
          MsgId,PipeId,CFE_SB_GetAppTskName(TskId,FullName),Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check for duplicate subscription */
    if(CFE_SB_DuplicateSubscribeCheck(MsgId,PipeId)==CFE_SB_DUPLICATE){
        CFE_SB.HKTlmMsg.Payload.DupSubscriptionsCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DUP_SUBSCRIP_EID,CFE_EVS_INFORMATION,CFE_SB.AppId,
          "Duplicate Subscription,MsgId 0x%x on %s pipe,app %s",
           MsgId,CFE_SB_GetPipeName(PipeId),CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SUCCESS;
    }/* end if */

    /*
    ** If there has been a subscription for this message id earlier,
    ** get the element number in the routing table.
    */
    Idx = CFE_SB_GetRoutingTblIdx(MsgId);

    /* if first subscription for this message... */
    if(Idx==CFE_SB_AVAILABLE){

        /* Get the index to the first available element in the routing table */
        Idx = CFE_SB_GetAvailRoutingIdx();

        /* if all routing table elements are used, send event */
        if(Idx >= CFE_SB_MAX_MSG_IDS){
            CFE_SB_UnlockSharedData(__func__,__LINE__);
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_MSGS_MET_EID,CFE_EVS_ERROR,CFE_SB.AppId,
              "Subscribe Err:Max Msgs(%d)In Use,MsgId 0x%x,pipe %s,app %s",
              CFE_SB_MAX_MSG_IDS,MsgId,CFE_SB_GetPipeName(PipeId),CFE_SB_GetAppTskName(TskId,FullName));
            return CFE_SB_MAX_MSGS_MET;
        }/* end if */

        /* Increment the MsgIds in use ctr and if it's > the high water mark,*/
        /* adjust the high water mark */
        CFE_SB.StatTlmMsg.Payload.MsgIdsInUse++;
        if(CFE_SB.StatTlmMsg.Payload.MsgIdsInUse > CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse){
           CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse = CFE_SB.StatTlmMsg.Payload.MsgIdsInUse;
        }/* end if */

        /* populate the look up table with the routing table index */
        CFE_SB_SetRoutingTblIdx(MsgId,Idx);

        /* label the new routing block with the message identifier */
        CFE_SB.RoutingTbl[Idx].MsgId = MsgId;

    }/* end if */

    if(CFE_SB.RoutingTbl[Idx].Destinations >= CFE_SB_MAX_DEST_PER_PKT){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MAX_DESTS_MET_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Subscribe Err:Max Dests(%d)In Use For Msg 0x%x,pipe %s,app %s",
             CFE_SB_MAX_DEST_PER_PKT,MsgId,CFE_SB_GetPipeName(PipeId),
             CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_MAX_DESTS_MET;
    }/* end if */

    DestBlkPtr = CFE_SB_GetDestinationBlk();
    if(DestBlkPtr == NULL){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEST_BLK_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Subscribe Err:Request for Destination Blk failed for Msg 0x%x", (unsigned int)MsgId);
        return CFE_SB_BUF_ALOC_ERR;
    }/* end if */

    /* initialize destination block */
    DestBlkPtr -> PipeId = PipeId;
    DestBlkPtr -> MsgId2PipeLim = (uint16)MsgLim;
    DestBlkPtr -> Active = CFE_SB_ACTIVE;
    DestBlkPtr -> BuffCount = 0;
    DestBlkPtr -> DestCnt = 0;
    DestBlkPtr -> Scope = Scope;
    DestBlkPtr -> Prev = NULL;
    DestBlkPtr -> Next = NULL;

    /* add destination block to head of list */
    CFE_SB_AddDest(Idx, DestBlkPtr);

    CFE_SB.RoutingTbl[Idx].Destinations++;

    CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse++;
    if(CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse > CFE_SB.StatTlmMsg.Payload.PeakSubscriptionsInUse)
    {
       CFE_SB.StatTlmMsg.Payload.PeakSubscriptionsInUse = CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse;
    }/* end if */

    if((CFE_SB.SubscriptionReporting == CFE_SB_ENABLE)&&(Scope==CFE_SB_GLOBAL)){
      CFE_SB.SubRprtMsg.Payload.MsgId = MsgId;
      CFE_SB.SubRprtMsg.Payload.Pipe = PipeId;
      CFE_SB.SubRprtMsg.Payload.Qos.Priority = Quality.Priority;
      CFE_SB.SubRprtMsg.Payload.Qos.Reliability = Quality.Reliability;
      CFE_SB.SubRprtMsg.Payload.SubType = CFE_SB_SUBSCRIPTION;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.SubRprtMsg);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RPT_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
            "Sending Subscription Report Msg=0x%x,Pipe=%d,Stat=0x%x",
            (unsigned int)MsgId,(int)PipeId,(unsigned int)Stat);
      CFE_SB_LockSharedData(__func__,__LINE__);/* to prevent back-to-back unlock */
    }/* end if */

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RCVD_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
        "Subscription Rcvd:MsgId 0x%x on %s(%d),app %s",
         (unsigned int)MsgId,CFE_SB_GetPipeName(PipeId),(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_SubscribeFull */


/******************************************************************************
** Name:    CFE_SB_Unsubscribe
**
** Purpose: API used to unsubscribe to a message.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_GLOBAL,CallerId);

    return Status;

}/* end CFE_SB_Unsubscribe */


/******************************************************************************
** Name:    CFE_SB_UnsubscribeLocal
**
** Purpose: CFE Internal API used to locally unsubscribe to a message. This
**          request to cancel a subscription will not be sent to peers.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_LOCAL,CallerId);

    return Status;

}/* end CFE_SB_UnsubscribeLocal */


/******************************************************************************
** Name:    CFE_SB_UnsubscribeAppId
**
** Purpose: CFE Internal API intented to be called by CFE_ES when an applications
**          SB resources need to be freed. The regular unsibscribe api won't work
**          because it does a check to ensure the caller is the owner of the pipe.
**
** Date Written:
**          03/15/2007
**
** Input Arguments:
**          MsgId
**          PipeId
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId,
                              CFE_SB_PipeId_t PipeId,
                              uint32 AppId)
{
    int32   Status = 0;

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_LOCAL, AppId);

    return Status;

}/* end CFE_SB_UnsubscribeWithAppId */


/******************************************************************************
** Name:    CFE_SB_UnsubscribeFull
**
** Purpose: CFE Internal API used to unsubscribe to a message.
**
** Assumptions, External Events, and Notes:
**
**
** Notes:This function cannot be called directly,it would not be semaphore protected.
**       Also,if more than one subscription is found, this function will remove all
**       entries that match.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**          Scope
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId,CFE_SB_PipeId_t PipeId,
                             uint8 Scope,uint32 AppId)
{
    uint16  Idx;
    uint32  PipeIdx;
    uint32  TskId = 0;
    uint32  MatchFound = FALSE;
    int32   Stat;
    CFE_SB_DestinationD_t   *DestPtr = NULL;
    char    FullName[(OS_MAX_API_NAME * 2)];


    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get task id for events */
    TskId = OS_TaskGetId();

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_PIPE_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Invalid Pipe Id Msg=0x%x,Pipe=%d,app=%s",
            (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* if given 'AppId' is not the owner of the pipe, send error event and return */
    if(CFE_SB.PipeTbl[PipeIdx].AppId != AppId){
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_CALLER_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
            CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId,MsgId);
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check input parameters */
    if((CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS)||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS)||
       (Scope > 1))
    {
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_ARG_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "UnSubscribe Err:Bad Arg,MsgId 0x%x,PipeId %d,app %s,scope %d",
            (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* get index into routing table */
    Idx = CFE_SB_GetRoutingTblIdx(MsgId);

    /* if there are no subscriptions for this message id... */
    if(Idx == CFE_SB_AVAILABLE){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_NO_SUBS_EID,CFE_EVS_INFORMATION,CFE_SB.AppId,
            "Unsubscribe Err:No subs for Msg 0x%x on %s,app %s",
            (unsigned int)MsgId,CFE_SB_GetPipeName(PipeId),CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SUCCESS;
    }/* end if */

    /* At this point, there must be at least one destination. */
    /* So the value of 'ListHeadPtr' will not be NULL by design */

    /* search the list for a matching pipe id */
    DestPtr = CFE_SB.RoutingTbl[Idx].ListHeadPtr;

    do{

        if(DestPtr->PipeId == PipeId){
            /* match found, remove node from list */
            CFE_SB_RemoveDest(Idx,DestPtr);

            /* return node to memory pool */
            CFE_SB_PutDestinationBlk(DestPtr);

            CFE_SB.RoutingTbl[Idx].Destinations--;
            CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse--;

            MatchFound = TRUE;

        }/* end if */

        DestPtr = DestPtr->Next;

    }while((MatchFound == FALSE)&&(DestPtr != NULL));

    /* if 'Destinations' was decremented to zero above... */
    if(CFE_SB.RoutingTbl[Idx].Destinations==0){
        CFE_SB.StatTlmMsg.Payload.MsgIdsInUse--;
        CFE_SB.RoutingTbl[Idx].MsgId = CFE_SB_INVALID_MSG_ID;
        CFE_SB_SetRoutingTblIdx(MsgId,CFE_SB_AVAILABLE);

        /* Send unsubscribe report only if there are zero requests for this pkt */
        if((CFE_SB.SubscriptionReporting == CFE_SB_ENABLE)&&
          (Scope == CFE_SB_GLOBAL))
        {
          CFE_SB.SubRprtMsg.Payload.MsgId = MsgId;
          CFE_SB.SubRprtMsg.Payload.Pipe = PipeId;
          CFE_SB.SubRprtMsg.Payload.Qos.Priority = 0;
          CFE_SB.SubRprtMsg.Payload.Qos.Reliability = 0;
          CFE_SB.SubRprtMsg.Payload.SubType = CFE_SB_UNSUBSCRIPTION;
          CFE_SB_UnlockSharedData(__func__,__LINE__);
          Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.SubRprtMsg);
          CFE_EVS_SendEventWithAppID(CFE_SB_UNSUBSCRIPTION_RPT_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
            "Sending Unsubscription Report Msg=0x%x,Pipe=%d,Stat=0x%x",
            (unsigned int)MsgId,(int)PipeId,(unsigned int)Stat);
          CFE_SB_LockSharedData(__func__,__LINE__);
        }/* end if */

    }/* end if */

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_REMOVED_EID,CFE_EVS_DEBUG,CFE_SB.AppId,
            "Subscription Removed:Msg 0x%x on pipe %d,app %s",
            MsgId,PipeId,CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_UnsubscribeFull */



/******************************************************************************
** Name:    CFE_SB_SendMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function increments and tracks the source sequence
**                counter for all telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SendMsg(CFE_SB_Msg_t    *MsgPtr)
{
    int32   Status = 0;

    Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_INCREMENT_TLM,CFE_SB_SEND_ONECOPY);

    return Status;

}/* end CFE_SB_SendMsg */



/******************************************************************************
** Name:    CFE_SB_PassMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function does NOT increment and track the source
**                sequence counter for telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_PassMsg(CFE_SB_Msg_t    *MsgPtr)
{
    int32   Status = 0;

    Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_DO_NOT_INCREMENT,CFE_SB_SEND_ONECOPY);

    return Status;

}/* end CFE_SB_PassMsg */



/******************************************************************************
** Name:    CFE_SB_SendMsgFull
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function increments and tracks the source sequence
**                counter for all telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**          TlmCntIncrements
**          CopyMode
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SendMsgFull(CFE_SB_Msg_t    *MsgPtr,
                          uint32           TlmCntIncrements,
                          uint32           CopyMode)
{
    CFE_SB_MsgId_t          MsgId;
    int32                   Status;
    CFE_SB_DestinationD_t   *DestPtr = NULL;
    CFE_SB_PipeD_t          *PipeDscPtr;
    CFE_SB_RouteEntry_t     *RtgTblPtr;
    CFE_SB_BufferD_t        *BufDscPtr;
    uint16                  TotalMsgSize;
    uint16                  RtgTblIdx;
    uint32                  TskId = 0;
    uint16                  i;
    char                    FullName[(OS_MAX_API_NAME * 2)];
    CFE_SB_EventBuf_t       SBSndErr;

    SBSndErr.EvtsToSnd = 0;

    /* get task id for events and Sender Info*/
    TskId = OS_TaskGetId();

    /* check input parameter */
    if(MsgPtr == NULL){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_BAD_ARG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Send Err:Bad input argument,Arg 0x%lx,App %s",
            (unsigned long)MsgPtr,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    MsgId = CFE_SB_GetMsgId(MsgPtr);

    /* validate the msgid in the message */
    if(CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrCnt++;
        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        {
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Send Err:Invalid MsgId(0x%x)in msg,App %s",
            MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);

    /* Verify the size of the pkt is < or = the mission defined max */
    if(TotalMsgSize > CFE_SB_MAX_SB_MSG_SIZE){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrCnt++;
        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        {
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
            MsgId,CFE_SB_GetAppTskName(TskId,FullName),TotalMsgSize,CFE_SB_MAX_SB_MSG_SIZE);
        return CFE_SB_MSG_TOO_BIG;
    }/* end if */

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    RtgTblIdx = CFE_SB_GetRoutingTblIdx(MsgId);

    /* if there have been no subscriptions for this pkt, */
    /* increment the dropped pkt cnt, send event and return success */
    if(RtgTblIdx == CFE_SB_AVAILABLE){

        CFE_SB.HKTlmMsg.Payload.NoSubscribersCnt++;

        if (CopyMode == CFE_SB_SEND_ZEROCOPY){
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }

        CFE_SB_UnlockSharedData(__func__,__LINE__);

        /* Determine if event can be sent without causing recursive event problem */
        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT) == CFE_SB_GRANTED){

           CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID,CFE_EVS_INFORMATION,CFE_SB.AppId,
              "No subscribers for MsgId 0x%x,sender %s",
              MsgId,CFE_SB_GetAppTskName(TskId,FullName));

           /* clear the bit so the task may send this event again */
           CFE_SB_FinishSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT);
        }/* end if */

        return CFE_SUCCESS;
    }/* end if */

    /* Allocate a new buffer. */
    if (CopyMode == CFE_SB_SEND_ZEROCOPY){
        BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
    }
    else{
        BufDscPtr = CFE_SB_GetBufferFromPool(MsgId, TotalMsgSize);
    }
    if (BufDscPtr == NULL){
        CFE_SB.HKTlmMsg.Payload.MsgSendErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);

        /* Determine if event can be sent without causing recursive event problem */
        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT) == CFE_SB_GRANTED){

            CFE_EVS_SendEventWithAppID(CFE_SB_GET_BUF_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
              "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
              MsgId,CFE_SB_GetAppTskName(TskId,FullName),TotalMsgSize);

            /* clear the bit so the task may send this event again */
            CFE_SB_FinishSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT);
        }/* end if */

        return CFE_SB_BUF_ALOC_ERR;
    }/* end if */

    /* Copy the packet into the SB memory space */
    if (CopyMode != CFE_SB_SEND_ZEROCOPY){
        /* Copy the packet into the SB memory space */
        CFE_PSP_MemCpy( BufDscPtr->Buffer, MsgPtr, (uint16)TotalMsgSize );
    }

    /* For Tlm packets, increment the seq count if requested */
    if((CFE_SB_GetPktType(MsgId)==CFE_SB_TLM) &&
       (TlmCntIncrements==CFE_SB_INCREMENT_TLM)){
        CFE_SB.RoutingTbl[RtgTblIdx].SeqCnt++;
        CFE_SB_SetMsgSeqCnt((CFE_SB_Msg_t *)BufDscPtr->Buffer,
                              CFE_SB.RoutingTbl[RtgTblIdx].SeqCnt);
    }/* end if */

    /* store the sender information */
    if(CFE_SB.SenderReporting != 0)
    {
       BufDscPtr->Sender.ProcessorId = CFE_PSP_GetProcessorId();
       strncpy(&BufDscPtr->Sender.AppName[0],CFE_SB_GetAppTskName(TskId,FullName),OS_MAX_API_NAME);
    }

    RtgTblPtr = &CFE_SB.RoutingTbl[RtgTblIdx];

    /* At this point there must be at least one destination for pkt */

    DestPtr = RtgTblPtr -> ListHeadPtr;

    /* Send the packet to all destinations  */
    for (i=0; i < RtgTblPtr -> Destinations; i++) {

        /* The DestPtr should never be NULL in this loop, this is just extra
           protection in case of the unforseen */
        if(DestPtr == NULL){
          break;
        }

        if (DestPtr->Active != CFE_SB_INACTIVE)    /* destination is active */
        {

        PipeDscPtr = &CFE_SB.PipeTbl[DestPtr->PipeId];

        /* if Msg limit exceeded, log event, increment counter */
        /* and go to next destination */
        if(DestPtr->BuffCount >= DestPtr->MsgId2PipeLim){

            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_MSGID_LIM_ERR_EID;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.MsgLimErrCnt++;
            PipeDscPtr->SendErrors++;

            }else{
        /*
        ** Write the buffer descriptor to the queue of the pipe.  If the write
        ** failed, log info and increment the pipe's error counter.
        */
                Status = OS_QueuePut(PipeDscPtr->SysQueueId,(void *)&BufDscPtr,
                                     sizeof(CFE_SB_BufferD_t *),0);

        if (Status == OS_SUCCESS) {
            BufDscPtr->UseCount++;    /* used for releasing buffer  */
            DestPtr->BuffCount++; /* used for checking MsgId2PipeLimit */
            DestPtr->DestCnt++;   /* used for statistics */
            if (DestPtr->PipeId < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
            {
                CFE_SB_PipeDepthStats_t *StatObj =
                        &CFE_SB.StatTlmMsg.Payload.PipeDepthStats[DestPtr->PipeId];
                StatObj->InUse++;
                if(StatObj->InUse > StatObj->PeakInUse){
                    StatObj->PeakInUse = StatObj->InUse;
            }/* end if */
            }

        }else if(Status == OS_QUEUE_FULL) {

            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_FULL_ERR_EID;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.PipeOverflowErrCnt++;
            PipeDscPtr->SendErrors++;


        }else{ /* Unexpected error while writing to queue. */

            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_WR_ERR_EID;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].ErrStat = Status;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.InternalErrCnt++;
            PipeDscPtr->SendErrors++;

                }/*end if */
            }/*end if */
        }/*end if */

        DestPtr = DestPtr -> Next;

    } /* end loop over destinations */

    /*
    ** Decrement the buffer UseCount and free buffer if cnt=0. This decrement is done
    ** because the use cnt is initialized to 1 in CFE_SB_GetBufferFromPool.
    ** Initializing the count to 1 (as opposed to zero) and decrementing it here are
    ** done to ensure the buffer gets released when there are destinations that have
    ** been disabled via ground command.
    */
    CFE_SB_DecrBufUseCnt(BufDscPtr);

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);


    /* send an event for each pipe write error that may have occurred */
    for(i=0;i < SBSndErr.EvtsToSnd; i++)
    {
        if(SBSndErr.EvtBuf[i].EventId == CFE_SB_MSGID_LIM_ERR_EID)
        {
            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_MSGID_LIM_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_ES_PerfLogEntry(CFE_SB_MSG_LIM_PERF_ID);
              CFE_ES_PerfLogExit(CFE_SB_MSG_LIM_PERF_ID);

              CFE_EVS_SendEventWithAppID(CFE_SB_MSGID_LIM_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
                "Msg Limit Err,MsgId 0x%x,pipe %s,sender %s",
                (unsigned int)RtgTblPtr->MsgId,
                CFE_SB_GetPipeName(SBSndErr.EvtBuf[i].PipeId),
                CFE_SB_GetAppTskName(TskId,FullName));

              /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_MSGID_LIM_ERR_EID_BIT);
            }/* end if */

        }else if(SBSndErr.EvtBuf[i].EventId == CFE_SB_Q_FULL_ERR_EID){

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_Q_FULL_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_ES_PerfLogEntry(CFE_SB_PIPE_OFLOW_PERF_ID);
              CFE_ES_PerfLogExit(CFE_SB_PIPE_OFLOW_PERF_ID);

              CFE_EVS_SendEventWithAppID(CFE_SB_Q_FULL_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
                  "Pipe Overflow,MsgId 0x%x,pipe %s,sender %s",
                  (unsigned int)RtgTblPtr->MsgId,
                  CFE_SB_GetPipeName(SBSndErr.EvtBuf[i].PipeId),
                  CFE_SB_GetAppTskName(TskId,FullName));

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_FULL_ERR_EID_BIT);
            }/* end if */

        }else{

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_EVS_SendEventWithAppID(CFE_SB_Q_WR_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
                "Pipe Write Err,MsgId 0x%x,pipe %s,sender %s,stat 0x%x",
                (unsigned int)RtgTblPtr->MsgId,
                CFE_SB_GetPipeName(SBSndErr.EvtBuf[i].PipeId),
                CFE_SB_GetAppTskName(TskId,FullName),
                (unsigned int)SBSndErr.EvtBuf[i].ErrStat);

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT);
            }/* end if */

        }/* end if */
    }


    return CFE_SUCCESS;

}/* end CFE_SB_SendMsgFull */



/******************************************************************************
** Name:    CFE_SB_RcvMsg
**
** Purpose: API used to receive a message from the software bus.
**
** Assumptions, External Events, and Notes:
**          Notes:
**
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          PipeId
**          BufPtr
**          timeOut - CFE_SB_PEND, CFE_SB_POLL or millisecond timeout
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_RcvMsg(CFE_SB_MsgPtr_t    *BufPtr,
                     CFE_SB_PipeId_t    PipeId,
                     int32              TimeOut)
{
    int32                  Status;
    CFE_SB_BufferD_t       *Message;
    CFE_SB_PipeD_t         *PipeDscPtr;
    CFE_SB_DestinationD_t  *DestPtr = NULL;
    uint32                 TskId = 0;
    char                   FullName[(OS_MAX_API_NAME * 2)];

    /* get task id for events */
    TskId = OS_TaskGetId();

    /* Check input parameters */
    if((BufPtr == NULL)||(TimeOut < (-1))){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgReceiveErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_RCV_BAD_ARG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Rcv Err:Bad Input Arg:BufPtr 0x%lx,pipe %d,t/o %d,app %s",
            (unsigned long)BufPtr,(int)PipeId,(int)TimeOut,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    PipeDscPtr = CFE_SB_GetPipePtr(PipeId);
    /* If the pipe does not exist or PipeId is out of range... */
    if (PipeDscPtr == NULL) {
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgReceiveErrCnt++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_BAD_PIPEID_EID,CFE_EVS_ERROR,CFE_SB.AppId,
            "Rcv Err:PipeId %d does not exist,app %s",
            (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /*
    ** Save off any currently in use buffer to free later; this allows
    ** one fewer shared data locks
    */

    PipeDscPtr->ToTrashBuff = PipeDscPtr->CurrentBuff;
    PipeDscPtr->CurrentBuff = NULL;

    /*
    ** Read a new buffer from the queue of the pipe, using the
    ** specified timeout option.  If a buffer was obtained, return the
    ** packet to the task according to mode.  Otherwise, return a status
    ** code indicating that no buffer was read.
    */
    Status = CFE_SB_ReadQueue(PipeDscPtr, TskId, TimeOut, &Message);

    /* take semaphore again to protect the remaining code in this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* free any pending trash buffer */
    if (PipeDscPtr->ToTrashBuff != NULL) {

        /* Decrement the Buffer Use Count and Free buffer if cnt=0) */
        CFE_SB_DecrBufUseCnt(PipeDscPtr->ToTrashBuff);

        PipeDscPtr->ToTrashBuff = NULL;

    }/* end if */

    if (Status == CFE_SUCCESS) {

        /*
        ** Load the pipe tables 'CurrentBuff' with the buffer descriptor
        ** ptr corresponding to the message just read. This is done so that
        ** the buffer can be released on the next RcvMsg call for this pipe.
        */
        PipeDscPtr->CurrentBuff = Message;

        /* Set the Receivers pointer to the address of the actual message */
        *BufPtr = (CFE_SB_MsgPtr_t) Message->Buffer;

        /* get pointer to destination to be used in decrementing msg limit cnt*/
        DestPtr = CFE_SB_GetDestPtr(PipeDscPtr->CurrentBuff->MsgId, PipeDscPtr->PipeId);

        /*
        ** DestPtr would be NULL if the msg is unsubscribed to while it is on
        ** the pipe. The BuffCount may be zero if the msg is unsubscribed to and
        ** then resubscribed to while it is on the pipe. Both of these cases are
        ** considered nominal and are handled by the code below.
        */
        if(DestPtr != NULL){

            if (DestPtr->BuffCount > 0){
                DestPtr->BuffCount--;
            }

        }/* end if DestPtr != NULL */

        if (PipeDscPtr->PipeId < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
        {
        CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeDscPtr->PipeId].InUse--;
        }

    }else{

        /* Set the users pointer to NULL indicating the CFE_SB_ReadQueue failed */
        *BufPtr = NULL;

    }/* end if */

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /*
    ** If status is not CFE_SUCCESS, then no packet was received.  If this was
    ** caused by an unexpected error, then CFE_SB_ReadQueue() will report the
    ** error.
    */
    return Status;

}/* end CFE_SB_RcvMsg */


/******************************************************************************
** Name:    CFE_SB_GetLastSenderId
**
** Purpose: API used for receiving sender Information of the last message received on
**          the given pipe.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          06/13/2007
**
** Input Arguments:
**          Ptr -     caller declares a ptr of type CFE_SB_SenderId_t then gives the
**                    address of that pointer.
**          PipeId -  the Pipe Id that the message was received on.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
uint32  CFE_SB_GetLastSenderId(CFE_SB_SenderId_t **Ptr,CFE_SB_PipeId_t  PipeId)
{

    CFE_SB_BufferD_t *Ptr2BufDescriptor;
    uint32            TskId = 0;
    uint32            AppId = 0xFFFFFFFF;
    char              FullName[(OS_MAX_API_NAME * 2)];

    TskId = OS_TaskGetId();

    /* validate ptr  - note: must validate ptr before pipe id validation  */
    /* because an invalid pipe id sets the callers pointer to NULL */
    if(Ptr == NULL){
      CFE_EVS_SendEventWithAppID(CFE_SB_LSTSNDER_ERR1_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Rcvd Null Ptr,Pipe=%d,App=%s",
          PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* validate pipe id */
    if(CFE_SB_ValidatePipeId(PipeId)!=CFE_SUCCESS){
      *Ptr = NULL;
      CFE_EVS_SendEventWithAppID(CFE_SB_LSTSNDER_ERR2_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Rcvd Invalid Pipe=%d,App=%s",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    CFE_ES_GetAppID(&AppId);

    CFE_SB_LockSharedData(__func__,__LINE__);

    /* verify requestor is owner of pipe */
    if(CFE_SB.PipeTbl[PipeId].AppId != AppId){
      *Ptr = NULL;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_GLS_INV_CALLER_EID,CFE_EVS_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Caller(%s) is not the owner of pipe %d",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId);
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* Get ptr to buffer descriptor for the last msg received on the given pipe */
    Ptr2BufDescriptor = CFE_SB.PipeTbl[PipeId].CurrentBuff;

    /* Set the receivers pointer to the adr of 'Sender' struct in buf descriptor */
    *Ptr = (CFE_SB_SenderId_t *) &Ptr2BufDescriptor -> Sender;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;

}/* end CFE_SB_GetLastSenderId */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyGetPtr
**
** Purpose: API used for for getting a pointer to a buffer (for zero copy mode
**          only)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgSize
**
** Output Arguments:
**          BufferHandle
**
** Return Values:
**          Pointer to an empty buffer in SB Memory space, or NULL if the buffer
**          could not be allocated.
**
******************************************************************************/

CFE_SB_Msg_t  *CFE_SB_ZeroCopyGetPtr(uint16 MsgSize,
                                     CFE_SB_ZeroCopyHandle_t *BufferHandle)
{
   int32                stat1;
   uint32               AppId = 0xFFFFFFFF;
   uint8               *address = NULL;
   CFE_SB_ZeroCopyD_t  *zcd = NULL;
   CFE_SB_BufferD_t    *bd = NULL;

    CFE_SB_LockSharedData(__func__,__LINE__);

    /* Allocate a new zero copy descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf((uint32 **)&zcd, CFE_SB.Mem.PoolHdl,  sizeof(CFE_SB_ZeroCopyD_t));
    if(stat1 < 0){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return NULL;
    }

    /* Add the size of a zero copy descriptor to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=stat1;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    /* Allocate a new buffer (from the SB memory pool) to hold the message  */
    stat1 = CFE_ES_GetPoolBuf((uint32 **)&bd, CFE_SB.Mem.PoolHdl, MsgSize + sizeof(CFE_SB_BufferD_t));
    if((stat1 < 0)||(bd==NULL)){
        /*deallocate the first buffer if the second buffer creation fails*/
        stat1 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);
        if(stat1 > 0){
            CFE_SB.StatTlmMsg.Payload.MemInUse-=stat1;
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return NULL;
    }

    /* Increment the number of buffers in use by one even though two buffers */
    /* were allocated. SBBuffersInUse increments on a per-message basis */
    CFE_SB.StatTlmMsg.Payload.SBBuffersInUse++;
    if(CFE_SB.StatTlmMsg.Payload.SBBuffersInUse > CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse){
        CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse = CFE_SB.StatTlmMsg.Payload.SBBuffersInUse;
    }/* end if */

    /* Add the size of the actual buffer to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=stat1;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    /* first set ptr to actual msg buffer the same as ptr to descriptor */
    address = (uint8 *)bd;

    /* increment actual msg buffer ptr beyond the descriptor */
    address += sizeof(CFE_SB_BufferD_t);

    /* Initialize the zero copy descriptor structure. */
    zcd->Size      = MsgSize;
    zcd->Buffer    = (void *)address;
    zcd->Next      = NULL;

    /* Add this Zero Copy Descriptor to the end of the chain */
    if(CFE_SB.ZeroCopyTail != NULL){
        ((CFE_SB_ZeroCopyD_t *) CFE_SB.ZeroCopyTail)->Next = (void *)zcd;
    }
    zcd->Prev = CFE_SB.ZeroCopyTail;
    CFE_SB.ZeroCopyTail = (void *)zcd;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /* get callers AppId */
    CFE_ES_GetAppID(&AppId);
    zcd->AppID     = AppId;

    (*BufferHandle) = (CFE_SB_ZeroCopyHandle_t) zcd;

    /* Initialize the buffer descriptor structure. */
    bd->UseCount  = 1;
    bd->Size      = MsgSize;
    bd->Buffer    = (void *)address;

    return (CFE_SB_Msg_t *)address;

}/* CFE_SB_ZeroCopyGetPtr */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleasePtr
**
** Purpose: API used for releasing a pointer to a buffer (for zero copy mode
**          only) This function is typically not needed for zero copy transfers.
**          This function is needed only when a ptr is received via
**          CFE_SB_ZeroCopyGetPtr, but never used in a send.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          Ptr2Release
**          BufferHandle
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleasePtr(CFE_SB_Msg_t  *Ptr2Release,
                                CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32    Status;
    int32    Stat2;

    Status = CFE_SB_ZeroCopyReleaseDesc(Ptr2Release, BufferHandle);

    CFE_SB_LockSharedData(__func__,__LINE__);

    if(Status == CFE_SUCCESS){
        /* give the buffer back to the buffer pool */
        Stat2 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl,
                                  (uint32 *) (((uint8 *)Ptr2Release) - sizeof(CFE_SB_BufferD_t)));
        if(Stat2 > 0){
             /* Substract the size of the actual buffer from the Memory in use ctr */
            CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat2;
            CFE_SB.StatTlmMsg.Payload.SBBuffersInUse--;
        }/* end if */
    }

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return Status;

}/* end CFE_SB_ZeroCopyReleasePtr */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleaseDesc
**
** Purpose: API used for releasing a zero copy descriptor (for zero copy mode
**          only).
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          Ptr2Release
**          BufferHandle
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Msg_t  *Ptr2Release,
                                 CFE_SB_ZeroCopyHandle_t  BufferHandle)
{
    int32    Stat;
    CFE_SB_ZeroCopyD_t *zcd = (CFE_SB_ZeroCopyD_t *) BufferHandle;

    CFE_SB_LockSharedData(__func__,__LINE__);

    Stat = CFE_ES_GetPoolBufInfo(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);

    if((Ptr2Release == NULL) || (Stat < 0) || (zcd->Buffer != (void *)Ptr2Release)){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return CFE_SB_BUFFER_INVALID;
    }

    /* delink the descriptor */
    if(zcd->Prev != NULL){
        ((CFE_SB_ZeroCopyD_t *) (zcd->Prev))->Next = zcd->Next;
    }
    if(zcd->Next != NULL){
        ((CFE_SB_ZeroCopyD_t *) (zcd->Next))->Prev = zcd->Prev;
    }
    if(CFE_SB.ZeroCopyTail == (void *)zcd){
        CFE_SB.ZeroCopyTail = zcd->Prev;
    }

    /* give the descriptor back to the buffer pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);
    if(Stat > 0){
        /* Substract the size of the actual buffer from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;

}/* end CFE_SB_ZeroCopyReleaseDesc */


/******************************************************************************
** Name:    CFE_SB_ZeroCopySend
**
** Purpose: API for sending messages in zero copy mode (with telemetry source
**          sequence count incrementing)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopySend(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32   Status = 0;

    Status = CFE_SB_ZeroCopyReleaseDesc(MsgPtr, BufferHandle);

    if(Status == CFE_SUCCESS){
        Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_INCREMENT_TLM,CFE_SB_SEND_ZEROCOPY);
    }

    return Status;

}/* end CFE_SB_ZeroCopySend */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyPass
**
** Purpose: API for sending messages in zero copy mode (telemetry source
**          sequence count is preserved)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyPass(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32   Status = 0;

    Status = CFE_SB_ZeroCopyReleaseDesc(MsgPtr, BufferHandle);

    if(Status == CFE_SUCCESS){
        Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_DO_NOT_INCREMENT,CFE_SB_SEND_ZEROCOPY);
    }

    return Status;

}/* end CFE_SB_ZeroCopyPass */


/******************************************************************************
**  Function:  CFE_SB_ReadQueue()
**
**  Purpose:
**    Read an SB message from the system queue.  The message is represented
**    by a pointer to the buffer descriptor of the message.  Several options
**    are available for the timeout, as described below.
**
**  Arguments:
**    PipeDscPtr: Pointer to pipe descriptor.
**    AppId     : Application ID of the receiving task (used for error messages).
**    Time_Out  : Timeout option; one of the following:
**                  CFE_SB_PEND_FOREVER  = wait forever until a packet arrives
**                  CFE_SB_POLL = check the pipe for packets but don't wait
**                  value in milliseconds = wait up to a specified time
**    Message   : Pointer to a variable that will receive the buffer
**                descriptor of the message.
**
**  Return:
**    CFE_SB status code indicating the result of the operation:
**      CFE_SUCCESS         = message was successfully read
**      CFE_SB_NO_MESSAGE   = no message is present (for CFE_SB_POLL option)
**      CFE_SB_TIME_OUT     = timeout expired (for timeout option)
**      CFE_SB_PIPE_RD_ERR  = an unexpected queue read error occurred
*/

int32  CFE_SB_ReadQueue (CFE_SB_PipeD_t         *PipeDscPtr,
                         uint32                 TskId,
                         CFE_SB_TimeOut_t       Time_Out,
                         CFE_SB_BufferD_t       **Message)
{
    int32              Status,TimeOut;
    uint32             Nbytes;
    char               FullName[(OS_MAX_API_NAME * 2)];

    /* translate the given Time_Out value */
    switch(Time_Out){

      case CFE_SB_PEND_FOREVER:
          TimeOut = OS_PEND;
          break;

      case CFE_SB_POLL:
          TimeOut = OS_CHECK;
          break;

      default:
          TimeOut = Time_Out;
          break;

    }/* end switch */

    /* Read the buffer descriptor address from the queue.  */
    Status = OS_QueueGet(PipeDscPtr->SysQueueId,
                        (void *)Message,
                        sizeof(CFE_SB_BufferD_t *),
                        &Nbytes,
                        TimeOut);

    /* translate the return value */
    switch(Status){

      case OS_SUCCESS:
          Status = CFE_SUCCESS;
          break;

      case OS_QUEUE_EMPTY:
          Status = CFE_SB_NO_MESSAGE;
          break;

      case OS_QUEUE_TIMEOUT:
          Status = CFE_SB_TIME_OUT;
          break;

      default:
          CFE_SB_LockSharedData(__func__,__LINE__);
          CFE_SB.HKTlmMsg.Payload.InternalErrCnt++;
          CFE_SB_UnlockSharedData(__func__,__LINE__);
          /* Unexpected error while reading the queue. */
          CFE_EVS_SendEventWithAppID(CFE_SB_Q_RD_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
              "Pipe Read Err,pipe %s,app %s,stat 0x%x",
              CFE_SB_GetPipeName(PipeDscPtr->PipeId),CFE_SB_GetAppTskName(TskId,FullName),(unsigned int)Status);

          Status = CFE_SB_PIPE_RD_ERR;
          break;

    }/* end switch */

    return (Status);
}/* end CFE_SB_ReadQueue */

/*****************************************************************************/

