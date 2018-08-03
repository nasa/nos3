/******************************************************************************
** File: cfe_sb_priv.c
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
**   This header file contains prototypes for private functions and type
**   definitions for cFE internal use.
**
** Author:   R.McGraw/SSI
**
** Notes:
**
** $Log: cfe_sb_priv.c  $
** Revision 1.18 2014/06/17 12:42:34GMT-05:00 rmcgraw 
** DCR18686:1 Initialized all AppId and CallerID's with 0xFFFFFFFF
** Revision 1.17 2012/02/29 17:21:50EST lwalling
** Check result of call to CFE_SB_GetAppTskName()
** Revision 1.16 2012/01/13 12:15:13EST acudmore
** Changed license text to reflect open source
** Revision 1.15 2011/12/07 19:19:00EST aschoeni
** Removed returns for TIME and SB for cleaning up apps
** Revision 1.14 2011/09/08 12:17:12EDT aschoeni
** Added newline to syslog calls on mutex failures
** Revision 1.13 2010/11/03 15:08:24EDT jmdagost
** Added cfe.h include file.
** Revision 1.12 2009/07/24 18:24:48EDT aschoeni
** Added Zero Copy Mode
** Revision 1.11 2009/07/20 14:10:14EDT aschoeni
** Made GetAppTskName reentrant
** Revision 1.10 2009/07/17 17:58:22EDT aschoeni
** Updated MsgMap (and associated variables) from a uint16 to an CFE_SB_MsgId_t
** Revision 1.9 2009/06/26 17:02:05EDT aschoeni
** Updated SB to use __func__ instead of __FILE__ for lock and unlock errors
** Revision 1.8 2009/05/08 11:28:07EDT rmcgraw
** DCR7631:1 Removed commented out utility to print routing info
** Revision 1.7 2009/05/06 09:34:09EDT rmcgraw
** DCR5801:12 Removed unused function GetRoutingPtr
** Revision 1.6 2009/04/06 10:21:09EDT rmcgraw
** DCR5801:2 Fixed problem with list, unsubscribing to all, then subscribing again
** Revision 1.5 2009/03/31 09:25:22EDT rmcgraw
** DCR5801:2 Fixed problem with removing a node in linked list
** Revision 1.4 2009/02/06 11:29:05EST rmcgraw
** DCR5801:2 General Cleanup
** Revision 1.3 2009/02/03 11:06:59EST rmcgraw
** DCR5801:2 Changed destination descriptors from array based to linked list
** Revision 1.2 2009/01/30 10:36:37EST rmcgraw
** DCR5801:1 Removed function CFE_SB_GetNumberOfSubscribers
** Revision 1.1 2008/04/17 08:05:31EDT ruperera
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.40 2007/09/13 09:39:58EDT rjmcgraw
** DCR4861 New function def for CFE_SB_RequestToSendEvent
** Revision 1.39 2007/07/12 16:59:38EDT rjmcgraw
** DCR4680:1 Removed SB event log related items
** Revision 1.38 2007/05/21 14:34:40EDT rjmcgraw
** Fixed compiler warnings in CFE_SB_GetAppTskName
** Revision 1.37 2007/03/28 14:22:02EST rjmcgraw
** DCR2654:displaying app.tsk name in events
** Revision 1.36 2007/03/19 14:38:03EST rjmcgraw
** Removed duplicate pipename check, it exists in OS_QueueCreate
** Revision 1.35 2007/03/16 15:29:29EST rjmcgraw
** Added code for duplicate pipe name check
** Revision 1.34 2007/03/16 10:06:18EST rjmcgraw
** Changed DeletePipe call to DeletePipeWithAppId in cleanup function
** Revision 1.33 2007/03/13 14:10:19EST rjmcgraw
** Added CFE_SB_CleanUpApp
** Revision 1.32 2006/10/16 14:30:56EDT rjmcgraw
** Minor changes to comply with MISRA standard
** Revision 1.31 2006/09/01 11:09:51EDT kkaudra
** IV&V:Removed cfe_evs.h,cfe_fs.h,cfe_sbp.h
**
******************************************************************************/

/*
** Include Files
*/

#include "common_types.h"
#include "osapi.h"
#include "private/cfe_private.h"
#include "cfe_sb_priv.h"
#include "cfe_sb.h"
#include "ccsds.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include <string.h>


/******************************************************************************
**  Function:  CFE_SB_CleanUpApp()
**
**  Purpose:
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_CleanUpApp(uint32 AppId){

  uint32 i;

  /* loop through the pipe table looking for pipes owned by AppId */
  for(i=0;i<CFE_SB_MAX_PIPES;i++){
    if((CFE_SB.PipeTbl[i].InUse == CFE_SB_IN_USE)&&
       (CFE_SB.PipeTbl[i].AppId == AppId))
    {
      CFE_SB_DeletePipeWithAppId(CFE_SB.PipeTbl[i].PipeId,AppId);
    }/* end if */
  }/* end for */

  /* Release any zero copy buffers */
  CFE_SB_ZeroCopyReleaseAppId(AppId);

  return CFE_SUCCESS;

}/* end CFE_SB_CleanUpApp */


/******************************************************************************
**  Function:  CFE_SB_GetAvailPipeIdx()
**
**  Purpose:
**    SB internal function to get the next available Pipe descriptor. Typically
**    called when a pipe is being created.
**
**  Arguments:
**    None
**
**  Return:
**    Returns the index of an empty pipe descriptor (which is also the PipeId)
**    or CFE_SB_INVALID_PIPE if there are no pipe descriptors available.
*/
CFE_SB_PipeId_t CFE_SB_GetAvailPipeIdx(void){

    uint8 i;

    /* search for next available pipe entry */
    for(i=0;i<CFE_SB_MAX_PIPES;i++){

        if(CFE_SB.PipeTbl[i].InUse == CFE_SB_NOT_IN_USE){
            return i;
        }/* end if */

    }/* end for */

    return CFE_SB_INVALID_PIPE;

}/* end CFE_SB_GetAvailPipeIdx */


/******************************************************************************
**  Function:  CFE_SB_GetAvailRoutingIdx()
**
**  Purpose:
**    SB internal function to get the next available Routing Table element
**    (CFE_SB_RouteEntry_t). Typically called when an application subscribes
**    to a message.
**
**  Arguments:
**    None
**
**  Return:
**    Returns the index of an empty Routing Table element or
**    CFE_SB_NO_ROUTING_IDX if there are no more elements available.
*/
uint16 CFE_SB_GetAvailRoutingIdx(void){

    uint16 i;

    /* search for next available routing table entry */
    for(i=0;i<CFE_SB_MAX_MSG_IDS;i++){

        if(CFE_SB.RoutingTbl[i].MsgId == CFE_SB_INVALID_MSG_ID){
            return i;
        }/* end if */

    }/* end for */

    /* error event indicating no more entries available */
    return CFE_SB_NO_ROUTING_IDX;

}/* end CFE_SB_GetAvailRoutingIdx */


/******************************************************************************
**  Function:  CFE_SB_GetPipeIdx()
**
**  Purpose:
**    SB internal function to get the pipe table index for the given pipe id.
**
**  Arguments:
**    PipeId
**
**  Return:
**    Returns the pipe table index of the given pipe id or CFE_SB_INVALID_PIPE if
*     there was not an entry for the given pipe id.
*/
uint8 CFE_SB_GetPipeIdx(CFE_SB_PipeId_t PipeId){

    uint8  i;

    /* search the pipe table for the for the given pipe id */
    for(i=0;i<CFE_SB_MAX_PIPES;i++){

        if((CFE_SB.PipeTbl[i].PipeId == PipeId)&&(CFE_SB.PipeTbl[i].InUse == 1)){
            return i;
        }/* end if */

    }/* end for */

    return CFE_SB_INVALID_PIPE;

}/* end CFE_SB_GetPipeIdx */


/******************************************************************************
**  Function:  CFE_SB_LockSharedData()
**
**  Purpose:
**    SB internal function to handle a semaphore take failure for the Shared
**    Data Mutex
**
**  Arguments:
**    FuncName   - the function name containing the code that generated the error.
**    LineNumber - the line number in the file of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber){

    int32   Status;
    uint32  AppId = 0xFFFFFFFF;

    Status = OS_MutSemTake(CFE_SB.SharedDataMutexId);
    if (Status != OS_SUCCESS) {

        CFE_ES_GetAppID(&AppId);

        CFE_ES_WriteToSysLog("SB SharedData Mutex Take Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FuncName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_SB_LockSharedData */



/******************************************************************************
**  Function:  CFE_SB_UnlockSharedData()
**
**  Purpose:
**    SB internal function to handle a semaphore give failure  for the Shared
**    Data Mutex
**
**  Arguments:
**    FuncName   - the function name containing the code that generated the error.
**    LineNumber - the line number in the file of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber){

   int32   Status;
   uint32  AppId = 0xFFFFFFFF;

    Status = OS_MutSemGive(CFE_SB.SharedDataMutexId);
    if (Status != OS_SUCCESS) {

        CFE_ES_GetAppID(&AppId);

        CFE_ES_WriteToSysLog("SB SharedData Mutex Give Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FuncName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_SB_UnlockSharedData */


/******************************************************************************
**  Function:  CFE_SB_GetPipePtr()
**
**  Purpose:
**    SB internal function to get a pointer to the pipe descriptor associated
**    with the given pipe id.
**
**  Arguments:
**    PipeId
**
**  Return:
**    Pointer to the descriptor for the pipe.  If the pipe ID is not valid,
**    a NULL pointer is returned.
*/

CFE_SB_PipeD_t *CFE_SB_GetPipePtr(CFE_SB_PipeId_t PipeId) {

   /*
   ** Verify that the pipeId is in the valid range and being used.
   ** If so, return the pointer to the pipe descriptor.
   */

    if(CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS){
        return NULL;
    }else{
        return &CFE_SB.PipeTbl[PipeId];
    }/* end if */

}/* end CFE_SB_GetPipePtr */



/******************************************************************************
**  Function:  CFE_SB_GetDestPtr()
**
**  Purpose:
**    SB internal function to get a pointer to the destination descriptor
**    associated with the given message id/pipe id combination.
**
**  Arguments:
**    MsgId  : ID of the message
**    PipeId : Pipe ID for the destination.
**
**  Return:
**    Pointer to the destination descriptor that corresponds to the msg/pipe
**    combination. If the destination does not exist, return NULL.
*/
CFE_SB_DestinationD_t  *CFE_SB_GetDestPtr(CFE_SB_MsgId_t MsgId,
                                          CFE_SB_PipeId_t PipeId){

    uint16                  Idx;
    CFE_SB_DestinationD_t   *DestPtr;

    Idx = CFE_SB_GetRoutingTblIdx(MsgId);

    if(Idx==CFE_SB_AVAILABLE){
        return NULL;
    }/* end if */

    DestPtr = CFE_SB.RoutingTbl[Idx].ListHeadPtr;

    while(DestPtr != NULL){

        if(DestPtr -> PipeId == PipeId){
            return DestPtr;
        }/* end if */

        DestPtr = DestPtr->Next;

    }/* end while */

    return NULL;

}/* end CFE_SB_GetDestPtr */



/******************************************************************************
**  Function:  CFE_SB_GetRoutingTblIdx()
**
**  Purpose:
**    SB internal function to get the index of the routing table element
**    associated with the given message id.
**
**  Arguments:
**    MsgId  : ID of the message
**    PipeId : Pipe ID for the destination.
**
**  Return:
**    Will return the index of the routing table element for the given message ID
**    or 0xFFFF if message does not exist.
*/
CFE_SB_MsgId_t CFE_SB_GetRoutingTblIdx(CFE_SB_MsgId_t MsgId){

#ifdef MESSAGE_FORMAT_IS_CCSDS

    /* mask out the ccsds version number to   */
    /* ensure we don't read beyond end of array */
    MsgId &= 0x1FFF;

    return CFE_SB.MsgMap[MsgId];
#else
    return 0;
#endif

}/* end CFE_SB_GetRoutingTblIdx */



/******************************************************************************
**  Function:  CFE_SB_SetRoutingTblIdx()
**
**  Purpose:
**    SB internal function to set a value in the message map. The "Value" is
**    the routing table index of the given message ID. The message map is used
**    for quick routing table index lookups of a given message ID. The cost of
**    this quick lookup is 8K bytes of memory(for CCSDS).
**
**  Arguments:
**    MsgId  : ID of the message
**    Value  : value to set.
**
**  Return:
**    Will return CFE_SUCCESS or CFE_INVALID_MSGID if MsgId is invalid
*/
int32 CFE_SB_SetRoutingTblIdx(CFE_SB_MsgId_t MsgId, CFE_SB_MsgId_t Value){

#ifdef MESSAGE_FORMAT_IS_CCSDS

    /* mask out the ccsds version number to   */
    /* ensure we don't write beyond end of array */
    MsgId &= 0x1FFF;

    CFE_SB.MsgMap[MsgId] = Value;

#endif

    return CFE_SUCCESS;

}/* end CFE_SB_SetRoutingTblIdx */


/******************************************************************************
**  Function:  CFE_SB_GetPipeName()
**
**  Purpose:
**    SB internal function to get the string of the pipe name of the given
**    Pipe ID.
**
**  Arguments:
**    MsgId  : ID of the message
**
**  Return:
**    Will return a pointer to the PipeName array in the pipe table if the the pipeid
**    is in range. Otherwise this function returns a pointer to the
**    PipeName4ErrCase[0], which is initialized with a null terminator.
**
*/
char *CFE_SB_GetPipeName(CFE_SB_PipeId_t PipeId){

    static char PipeName4ErrCase[1] = {'\0'};

    if(PipeId >= CFE_SB_MAX_PIPES){
        return &PipeName4ErrCase[0];
    }else{
        return &CFE_SB.PipeTbl[PipeId].PipeName[0];
    }/* end if */

}/* end CFE_SB_GetPipeName */



/******************************************************************************
**  Function:  CFE_SB_DuplicateSubscribeCheck()
**
**  Purpose:
**    SB internal function to check for a duplicate subscription.
**
**  Arguments:
**    MsgId  : ID of the message
**    PipeId : ID of the pipe
**
**  Return:
**    Will return CFE_SB_DUPLICATE if the given MsgId/PipeId subscription
**    exists in SB routing tables, otherwise will return CFE_SB_NO_DUPLICATE.
*/
int32 CFE_SB_DuplicateSubscribeCheck(CFE_SB_MsgId_t MsgId,
                                       CFE_SB_PipeId_t PipeId){

    uint16                  Idx;
    CFE_SB_DestinationD_t   *DestPtr;

    Idx = CFE_SB_GetRoutingTblIdx(MsgId);

    if(Idx==CFE_SB_AVAILABLE){
        return CFE_SB_NO_DUPLICATE;
    }/* end if */

    DestPtr = CFE_SB.RoutingTbl[Idx].ListHeadPtr;

    while(DestPtr != NULL){

        if(DestPtr -> PipeId == PipeId){
            return CFE_SB_DUPLICATE;
        }/* end if */

        DestPtr = DestPtr->Next;

    }/* end while */

    return CFE_SB_NO_DUPLICATE;

}/* end CFE_SB_DuplicateSubscribeCheck */



/******************************************************************************
**  Function:  CFE_SB_SetMsgSeqCnt()
**
**  Purpose:
**    SB internal function to set the sequence count of a message to a
**    particular value.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**    Count   : sets the sequence count to this value
**
**  Return:
**    None
*/
void CFE_SB_SetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr,uint32 Count){

#ifdef MESSAGE_FORMAT_IS_CCSDS
    CCSDS_WR_SEQ(MsgPtr->Hdr,Count);
#endif

}/* end CFE_SB_SetMsgSeqCnt */


/******************************************************************************
**  Function:  CFE_SB_ValidateMsgId()
**
**  Purpose:
**    SB internal function to validate a given MsgId.
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId){

    /* ensure the ccsds version number in MsgId is 0 */
    /* cppcheck-suppress redundantCondition */
    if((MsgId > CFE_SB_HIGHEST_VALID_MSGID)||
       (MsgId == CFE_SB_INVALID_MSG_ID))
    {
        return CFE_SB_FAILED;
    }else{
        return CFE_SUCCESS;
    }/* end if */

}/* end CFE_SB_ValidateMsgId */


/******************************************************************************
**  Function:  CFE_SB_ValidatePipeId()
**
**  Purpose:
**    This function checks that the pipe id does not have an index larger than the
**    array and that the pipe is in use.
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId){

    if((PipeId >= CFE_SB_MAX_PIPES)||
       (CFE_SB.PipeTbl[PipeId].InUse == CFE_SB_NOT_IN_USE))
    {
        return CFE_SB_FAILED;
    }else{
        return CFE_SUCCESS;
    }/* end if */

}/* end CFE_SB_ValidatePipeId */


/******************************************************************************
**  Function:  CFE_SB_GetAppTskName()
**
**  Purpose:
**    This function returns a pointer to the app.tsk name string
**
**  Arguments:
**    TaskId - the task id of the app.task name desired
**    FullName - string buffer to store name
**
**  Return:
**    Pointer to App.Tsk Name
**
**  Note: With taskId, Parent App name and Child Task name can be queried from ES
**
*/
char *CFE_SB_GetAppTskName(uint32 TaskId,char *FullName){

    CFE_ES_TaskInfo_t  TaskInfo;
    CFE_ES_TaskInfo_t  *ptr = &TaskInfo;
    char               AppName[OS_MAX_API_NAME];
    char               TskName[OS_MAX_API_NAME];

    if(CFE_ES_GetTaskInfo(ptr, TaskId) != CFE_SUCCESS){

      /* unlikely, but possible if TaskId is bogus */
      strncpy(FullName,"Unknown",OS_MAX_API_NAME-1);
      FullName[OS_MAX_API_NAME-1] = '\0';

    }else if(strncmp((char *)ptr->AppName,(char *)ptr->TaskName,OS_MAX_API_NAME-1) == 0){

      /* if app name and task name are the same */
      strncpy(FullName,(char *)ptr->AppName,OS_MAX_API_NAME-1);
      FullName[OS_MAX_API_NAME-1] = '\0';

    }else{

      /* AppName and TskName buffers and strncpy are needed to limit string sizes */
      strncpy(AppName,(char *)ptr->AppName,OS_MAX_API_NAME-1);
      AppName[OS_MAX_API_NAME-1] = '\0';
      strncpy(TskName,(char *)ptr->TaskName,OS_MAX_API_NAME-1);
      TskName[OS_MAX_API_NAME-1] = '\0';

      sprintf(FullName,"%s.%s",AppName,TskName);

    }/* end if */

    return FullName;

}/* end CFE_SB_GetAppTskName */


/******************************************************************************
**  Function:  CFE_SB_GetPktType()
**
**  Purpose:
**    For CCSDS packets, this function returns the state of the cmd/tlm bit(12).
**    For cmd pkts, the state is 1. For tlm pkts, the state is 0.
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId){

#ifdef MESSAGE_FORMAT_IS_CCSDS

        return CFE_TST(MsgId,12);

#endif

}/* end CFE_SB_GetPktType */


/******************************************************************************
**  Function:  CFE_SB_RequestToSendEvent()
**
**  Purpose:
**    This function will test the given bit for the given task. If the bit is set
**    this function will return CFE_SB_DENIED. If bit is not set, this function set
**    the bit and return CFE_SB_GRANTED. This will prevent recursive events from
**    occurring.
**
**  Arguments:
**
**  Return:
**    If the bit is set this function will return CFE_SB_DENIED.
**    If bit is not set, this function set the bit and return CFE_SB_GRANTED.
*/
uint32 CFE_SB_RequestToSendEvent(uint32 TaskId, uint32 Bit){

    OS_ConvertToArrayIndex(TaskId, &TaskId);

    /* if bit is set... */
    if(CFE_TST(CFE_SB.StopRecurseFlags[TaskId],Bit)==TRUE)
    {

      return CFE_SB_DENIED;

    }else{

      CFE_SET(CFE_SB.StopRecurseFlags[TaskId],Bit);
      return CFE_SB_GRANTED;

    }/* end if */

}/* end CFE_SB_RequestToSendEvent */

/******************************************************************************
**  Function:  CFE_SB_FinishSendEvent()
**
**  Purpose:
**    This function will clear the given bit for the given task. Called after
**    a successful CFE_SB_RequestToSendEvent()
**
**  Arguments:
**
**  Return:
**    If the bit is set this function will return CFE_SB_DENIED.
**    If bit is not set, this function set the bit and return CFE_SB_GRANTED.
*/
void CFE_SB_FinishSendEvent(uint32 TaskId, uint32 Bit){

    OS_ConvertToArrayIndex(TaskId, &TaskId);

    /* clear the bit so the task may send this event again */
    CFE_CLR(CFE_SB.StopRecurseFlags[TaskId],Bit);
}/* end CFE_SB_RequestToSendEvent */



/******************************************************************************
**  Function:  CFE_SB_AddDest()
**
**  Purpose:
**      This function will add the given node to the head of the list.
**
**  Arguments:
**      RtgTblIdx - Routing table index
**      Dest - Pointer to the destination block to add to the list
**
**  Return:
**
*/
int32 CFE_SB_AddDest(uint16 RtgTblIdx, CFE_SB_DestinationD_t *NewNode){

    CFE_SB_DestinationD_t *WBS;/* Will Be Second (WBS) node */

    /* if first node in list */
    if(CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr == NULL){

        /* initialize the new node */
        NewNode->Next = NULL;
        NewNode->Prev = NULL;

        /* insert the new node */
        CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr = NewNode;

    }else{

        WBS = CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr;

        /* initialize the new node */
        NewNode->Next = WBS;
        NewNode->Prev = NULL;

        /* insert the new node */
        WBS -> Prev = NewNode;
        CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr = NewNode;

    }/* end if */

    return CFE_SUCCESS;

}/* CFE_SB_AddDest */



/******************************************************************************
**  Function:  CFE_SB_RemoveDest()
**
**  Purpose:
**      This function will remove the given node from the list.
**      This function assumes there is at least one node in the list.
**
**  Arguments:
**      RtgTblIdx - Routing table index
**      Dest - Pointer to the destination block to remove from the list
**
**  Return:
**
*/
int32 CFE_SB_RemoveDest(uint16 RtgTblIdx, CFE_SB_DestinationD_t *NodeToRemove){

    CFE_SB_DestinationD_t *PrevNode;
    CFE_SB_DestinationD_t *NextNode;

    /* if this is the only node in the list */
    if((NodeToRemove->Prev == NULL) && (NodeToRemove->Next == NULL)){

        CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr = NULL;

    /* if first node in the list and list has more than one */
    }else if(NodeToRemove->Prev == NULL){

        NextNode = NodeToRemove->Next;

        NextNode -> Prev = NULL;

        CFE_SB.RoutingTbl[RtgTblIdx].ListHeadPtr = NextNode;

    /* if last node in the list and list has more than one */
    }else if(NodeToRemove->Next == NULL){

        PrevNode = NodeToRemove->Prev;

        PrevNode -> Next = NULL;

    /* NodeToRemove has node(s) before and node(s) after */
    }else{

        PrevNode = NodeToRemove->Prev;
        NextNode = NodeToRemove->Next;

        PrevNode -> Next = NextNode;
        NextNode -> Prev = PrevNode;

    }/* end if */


    /* initialize the node before returning it to the heap */
    NodeToRemove -> Next = NULL;
    NodeToRemove -> Prev = NULL;

    return CFE_SUCCESS;

}/* CFE_SB_RemoveDest */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleaseAppId
**
** Purpose: API used for releasing all pointers to a buffers (for zero copy mode
**          only) for a specific Application. This function is used for cleaning
**          up when an application crashes.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          07/23/2009
**
** Input Arguments:
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleaseAppId(uint32         AppId)
{
    CFE_SB_ZeroCopyD_t *prev = NULL;
    CFE_SB_ZeroCopyD_t *zcd = (CFE_SB_ZeroCopyD_t *) (CFE_SB.ZeroCopyTail);

    while(zcd != NULL){
        prev = (CFE_SB_ZeroCopyD_t *) (zcd->Prev);
        if(zcd->AppID == AppId){
            CFE_SB_ZeroCopyReleasePtr((CFE_SB_Msg_t *) zcd->Buffer, (CFE_SB_ZeroCopyHandle_t) zcd);
        }
        zcd = prev;
    }

    return CFE_SUCCESS;

}/* end CFE_SB_ZeroCopyReleasePtr */

/*****************************************************************************/

