/******************************************************************************
** File: cfe_sb_buf.c
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
**      This file contains the source code for the SB memory management
**      functions.
**
** Author:   R.McGraw/SSI
**
**
** $Log: cfe_sb_buf.c  $
** Revision 1.17 2014/04/24 09:57:05GMT-05:00 rmcgraw 
** DCR19487:1 - Remove size argument in CFE_SB_GetBufferFromCaller
** Revision 1.16 2012/01/13 12:15:12EST acudmore
** Changed license text to reflect open source
** Revision 1.15 2011/12/20 10:26:04EST rmcgraw
** DCR15187:1 Removed function CFE_SB_DecrMsgLimCnt
** Revision 1.14 2011/09/09 14:25:44EDT aschoeni
** Added fix for ZeroCopy issues
** Revision 1.13 2010/10/25 16:02:56EDT aschoeni
** Allocation for sb message buffer now includes descriptor instead of separate allocation for descriptor
** Revision 1.12 2009/07/29 12:02:50EDT aschoeni
** Updated GetBufferFromPool to deallocate the first buffer if the second buffer creation fails
** Revision 1.11 2009/07/29 11:51:52EDT aschoeni
** Updated GetBufferFromCaller to deallocate the message buffer if the descriptor buffer fails to be created (otherwise it is never reclaimed)
** Revision 1.10 2009/07/24 18:23:57EDT aschoeni
** Added Zero Copy Mode
** Revision 1.9 2009/07/20 14:09:30EDT aschoeni
** Made GetAppTskName reentrant
** Revision 1.8 2009/06/26 17:02:04EDT aschoeni
** Updated SB to use __func__ instead of __FILE__ for lock and unlock errors
** Revision 1.7 2009/04/08 13:25:26EDT rmcgraw
** DCR5802:4 Change data type int to int32
** Revision 1.6 2009/02/11 14:19:50EST rmcgraw
** DCR6269:1 Removed the 'Buf' in mem pool names
** Revision 1.5 2009/02/06 11:29:04EST rmcgraw
** DCR5801:2 General Cleanup
** Revision 1.4 2009/02/03 11:06:58EST rmcgraw
** DCR5801:2 Changed destination desciptors from array based to linked list
** Revision 1.3 2009/01/30 11:13:08EST rmcgraw
** DCR5801:6 Moved semaphore unlock to above GetAppid call in DecrMsgLimCnt
** Revision 1.2 2009/01/23 15:00:16EST rmcgraw
** DCR5802:1 Removed redundant events in cfe_sb_buf.c
** Revision 1.1 2008/04/17 08:05:31EDT ruperera
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.34 2007/09/19 17:03:57EDT rjmcgraw
** Fixed compiler error
** Revision 1.33 2007/09/19 14:39:25EDT rjmcgraw
** DCR4421 Removed use count error processing
** Revision 1.32 2007/08/17 15:59:51EDT rjmcgraw
** Changes to free semaphore before calling SendEventWithAppId
** Revision 1.31 2007/08/07 12:52:40EDT dlkobe
** Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
** Revision 1.30 2007/07/12 17:05:05EDT rjmcgraw
** DCR4680:1 Replaced calls to CFE_SB_LogEvent with SendEventWithAppid
** Revision 1.29 2007/03/27 08:57:03EST rjmcgraw
** Added #include cfe_sb_events.h
** Revision 1.28 2007/03/22 13:05:45EST rjmcgraw
** DCR246:Replace SB stat Descriptors in use to Peak Buffers In Use
** Revision 1.27 2006/10/16 14:30:43EDT rjmcgraw
** Minor changes to comply with MISRA standard
** Revision 1.26 2006/09/11 16:38:03EDT rjmcgraw
** Added condition to update statistics only if 'put' is successful
** Revision 1.25 2006/09/01 11:07:26EDT kkaudra
** IV&V:Removed cfe_evs.h
**
******************************************************************************/

/*
**  Include Files
*/

#include "cfe_sb_priv.h"
#include "cfe_sb_events.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_error.h"

/******************************************************************************
**  Function:   CFE_SB_GetBufferFromPool()
**
**  Purpose:
**    Request a buffer from the SB buffer pool. The SB buffer pool is a
**    pre-allocated block of memory of size CFE_SB_BUF_MEMORY_BYTES. It is used
**    by the SB to dynamically allocate memory to hold the message and a buffer
**    descriptor associated with the message during the sending of a message.
**
**  Arguments:
**    msgId        : Message ID
**    size         : Size of the buffer in bytes.
**
**  Return:
**    Pointer to the buffer descriptor for the new buffer, or NULL if the buffer
**    could not be allocated.
*/

CFE_SB_BufferD_t * CFE_SB_GetBufferFromPool(uint16 MsgId, uint16 Size) {
   int32                stat1;
   uint8               *address = NULL;
   CFE_SB_BufferD_t    *bd = NULL;

    /* Allocate a new buffer descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf((uint32 **)&bd, CFE_SB.Mem.PoolHdl,  Size + sizeof(CFE_SB_BufferD_t));
    if(stat1 < 0){
        return NULL;
    }

    /* increment the number of buffers in use and adjust the high water mark if needed */
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

    /* Initialize the buffer descriptor structure. */
    bd->MsgId     = MsgId;
    bd->UseCount  = 1;
    bd->Size      = Size;
    bd->Buffer    = (void *)address;

    return bd;

}/* CFE_SB_GetBufferFromPool */


/******************************************************************************
**  Function:   CFE_SB_GetBufferFromCaller()
**
**  Purpose:
**    Request a buffer from the SB buffer pool to use as a buffer descriptor
**    for and already created SB buffer.
**
**  Arguments:
**    msgId        : Message ID
**    Address      : Address of the buffer
**
**  Return:
**    Pointer to the buffer descriptor for the suplied buffer, or NULL if the
**    descriptor could not be allocated.
*/

CFE_SB_BufferD_t * CFE_SB_GetBufferFromCaller(uint16 MsgId,
                                              void *Address) {
   CFE_SB_BufferD_t    *bd = (CFE_SB_BufferD_t *)(((uint8 *)Address) - sizeof(CFE_SB_BufferD_t));

    /* Initialize the MsgId in the buffer descriptor (the rest has already been initialized in this case). */
    bd->MsgId     = MsgId;

    return bd;

}/* CFE_SB_GetBufferFromCaller */


/******************************************************************************
**  Function:   CFE_SB_ReturnBufferToPool()
**
**  Purpose:
**    This function will return two blocks of memory back to the memory pool.
**    One block is the memory used to store the actual message, the other block
**    was used to store the buffer descriptor for the message.
**
**  Arguments:
**    bd     : Pointer to the buffer descriptor.
**
**  Return:
**    SB status
*/
int32 CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd){
    int32    Stat;

    /* give the buf descriptor back to the buf descriptor pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)bd);
    if(Stat > 0){
        CFE_SB.StatTlmMsg.Payload.SBBuffersInUse--;
        /* Substract the size of a buffer descriptor from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    return CFE_SUCCESS;

}/* end CFE_SB_ReturnBufferToPool */



/******************************************************************************
**  Function:   CFE_SB_DecrBufUseCnt()
**
**  Purpose:
**    This function will decrement the UseCount of a particular buffer. If the
**    the UseCount is decremented to zero, it will return the buffer to the
**    memory pool.
**
**  Note:
**    UseCount is a variable in the CFE_SB_BufferD_t and is used only to
**    determine when a buffer may be returned to the memory pool.
**
**  Arguments:
**    bd : Pointer to the buffer descriptor.
**
**  Return:
**    CFE_SUCCESS for normal operation.
*/
int32 CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd){

    /* range check the UseCount variable */
    if(bd->UseCount > 0){

        bd->UseCount--;

        if (bd->UseCount == 0) {
           CFE_SB_ReturnBufferToPool(bd);
        }/* end if */

    }

    return CFE_SUCCESS;

}/* end CFE_SB_DecrBufUseCnt */



/******************************************************************************
**  Function:   CFE_SB_GetDestinationBlk()
**
**  Purpose:
**    This function gets a destination descriptor from the SB memory pool.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the destination descriptor
*/
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void)
{
    int32 Stat;
    CFE_SB_DestinationD_t *Dest = NULL;

    /* Allocate a new destination descriptor from the SB memory pool.*/
    Stat = CFE_ES_GetPoolBuf((uint32 **)&Dest, CFE_SB.Mem.PoolHdl,  sizeof(CFE_SB_DestinationD_t));
    if(Stat < 0){
        return NULL;
    }

    /* Add the size of a destination descriptor to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=Stat;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    return Dest;

}/* end CFE_SB_GetDestinationBlk */


/******************************************************************************
**  Function:   CFE_SB_PutDestinationBlk()
**
**  Purpose:
**    This function returns a destination descriptor to the SB memory pool.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the destination descriptor
*/
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest)
{
    int32 Stat;

    if(Dest==NULL){
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* give the destination block back to the SB memory pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)Dest);
    if(Stat > 0){
        /* Substract the size of the destination block from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    return CFE_SUCCESS;

}/* end CFE_SB_PutDestinationBlk */


/*****************************************************************************/
