/**
 * @file
 * 
 * This file contains all functions relevant adding or removing data from the
 * OS queues. 
 * 
 * @author Jaclyn Beck
 * @date 2015/06/24 15:30:00
 */
#include "cfe.h"
#include "serial_queue.h"
#include "serial_events.h"
#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include <arpa/inet.h>

/**
 * Checks the given queue to see if there is data available. If so, it copies 
 * the data into the message buffer. 
 *
 * @param queue      Queue ID (data or protocol) to check
 * @param semId      Semaphore ID to lock/unlock
 * @param MsgBuf     Pointer to the buffer to copy the message into
 *
 * @return SBN_IF_EMPTY if no data available
 * @return MsgSize (number of bytes read from the queue)
 * @return SBN_ERROR on error
 */
int Serial_QueueGetMsg(uint32 queue, uint32 semId, SBN_MsgType_t *MsgTypePtr,
     SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *Msg)
{
    void *data = NULL, *dataoffset = NULL;
    uint32 size = 0;
    int32 status = 0;
 
    if(queue == 0)
    {
        return SBN_IF_EMPTY; 
    }/* end if */

    /* Take the semaphore */
    status = OS_BinSemTake(semId); 
    if(status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: Error taking semaphore %d. Returned %d\n", semId, status); 
        return SBN_ERROR; 
    }/* end if */

    /* Try and get a message from the queue (non-blocking) */
    status = OS_QueueGet(queue, &data, sizeof(uint32), &size, OS_CHECK); 
    
    if(status == OS_SUCCESS && data != NULL)
    {
        dataoffset = data;
        CFE_PSP_MemCpy(MsgSizePtr, dataoffset, sizeof(*MsgSizePtr));
        *MsgSizePtr = ntohs(*MsgSizePtr);
        dataoffset += sizeof(*MsgSizePtr);

        CFE_PSP_MemCpy(MsgTypePtr, dataoffset, sizeof(*MsgTypePtr));
        dataoffset += sizeof(*MsgTypePtr);

        CFE_PSP_MemCpy(CpuIdPtr, dataoffset, sizeof(*CpuIdPtr));
        *CpuIdPtr = ntohl(*CpuIdPtr);
        dataoffset += sizeof(*CpuIdPtr);

        CFE_PSP_MemCpy(Msg, dataoffset, *MsgSizePtr);
        free(data);
    }/* end if */

    /* Give up the semaphore */
    status = OS_BinSemGive(semId); 
    if(status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: Error giving semaphore %d. Returned %d\n", semId, status);
        return SBN_ERROR; 
    }/* end if */

    return SBN_OK; 
}/* end Serial_QueueGetMsg */

/**
 * Adds a message to the OS queue. The message is allocated and then copied
 * from MsgBuf to the allocated character buffer. 
 *
 * @param queue      Queue ID (data or protocol) to add to
 * @param semId      Semaphore ID to lock/unlock
 * @param MsgBuf     Pointer to the buffer to copy the message from
 * 
 * @return SBN_OK on success
 * @return SBN_ERROR on error
 */
int Serial_QueueAddNode(uint32 queue, uint32 semId, SBN_MsgType_t MsgType,
    SBN_MsgSize_t MsgSize, SBN_CpuId_t CpuId, void *Msg)
{
    int32 status = 0;
    void *data = malloc(MsgSize + SBN_PACKED_HDR_SIZE), *dataoffset = NULL;

    if(data == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: QueueAddNode: Error allocating message\n"); 
        return SBN_ERROR; 
    }/* end if */

    dataoffset = data;

    MsgSize = htons(MsgSize);
    CFE_PSP_MemCpy(dataoffset, &MsgSize, sizeof(MsgSize));
    MsgSize = ntohs(MsgSize);
    dataoffset += sizeof(MsgSize);

    CFE_PSP_MemCpy(dataoffset, &MsgType, sizeof(MsgType));
    dataoffset += sizeof(MsgType);

    CpuId = htons(CpuId);
    CFE_PSP_MemCpy(dataoffset, &CpuId, sizeof(CpuId));
    CpuId = ntohs(CpuId);
    dataoffset += sizeof(CpuId);

    CFE_PSP_MemCpy(dataoffset, Msg, MsgSize);

    /* Take the semaphore */
    status = OS_BinSemTake(semId); 
    if(status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: Error taking semaphore %d. Returned %d\n", semId, status); 
        return SBN_ERROR; 
    }/* end if */

    /* Try adding the message to the queue */
    status = OS_QueuePut(queue, &data, sizeof(uint32), 0); 
    if(status == OS_QUEUE_FULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_INFORMATION,
            "Serial: Queue %d is full. Old messages will be lost.\n", queue); 

        /* Remove the oldest message to make room for the new message and try 
           adding the new message again */
        Serial_QueueRemoveNode(queue); 
        status = OS_QueuePut(queue, &data, sizeof(uint32), 0); 
    }/* end if */

    if(status != OS_SUCCESS)
    {
        /* TODO: should we free message? */
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: Error adding message to queue %d. Returned %d\n",
            queue, status); 
    }/* end if */

    /* Give up the semaphore */
    status = OS_BinSemGive(semId); 
    if(status != OS_SUCCESS)
    {
        /* TODO: should we free message? */
        CFE_EVS_SendEvent(SBN_SERIAL_QUEUE_EID, CFE_EVS_ERROR,
            "Serial: Error giving semaphore %d. Returned %d\n", semId, status); 
        return SBN_ERROR; 
    }/* end if */

    return SBN_OK; 
}/* end Serial_QueueAddNode */


/**
 * Removes a message from the queue and discards the data. The removed message
 * is then freed to avoid memory leaks. 
 *
 * @param queue     Queue ID (data or protocol) to remove from
 *
 * @return SBN_OK on success or SBN_ERROR if there's an issue.
 */
int32 Serial_QueueRemoveNode(uint32 queue)
{
    uint8 *data = NULL; 
    uint32 size = 0;

    if(OS_QueueGet(queue, &data, sizeof(uint32), &size, OS_CHECK) != OS_SUCCESS)
    {
        return SBN_ERROR;
    }/* end if */

    if(data != NULL)
    {
        free(data); 
    }/* end if */
    return SBN_OK;
}/* end Serial_QueueRemoveNode */
