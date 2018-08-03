#ifndef _serial_queue_h_
#define _serial_queue_h_

#include "cfe.h"
#include "serial_sbn_if_struct.h"
#include "sbn_interfaces.h"

/* Function declarations */
int Serial_QueueGetMsg(uint32 queue, uint32 semId, SBN_MsgType_t *MsgTypePtr,
     SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *Msg);
int Serial_QueueAddNode(uint32 queue, uint32 semId, SBN_MsgType_t MsgType,
    SBN_MsgSize_t MsgSize, SBN_CpuId_t CpuId, void *Msg);
int Serial_QueueRemoveNode(uint32 queue);

#endif /* _serial_queue_h_ */
