#ifndef _shmem_sbn_if_struct_h_
#define _shmem_sbn_if_struct_h_

#include <pthread.h>

/**
 * Struct that holds variables from a file entry. All addresses are physical
 * addresses. 
 */
typedef struct {
    uint32 DataRcvPhysAddr;
    uint32 DataRcvPhysSize;
    uint32 DataSendPhysAddr;
    uint32 DataSendPhysSize;
    uint32 ProtoRcvPhysAddr;
    uint32 ProtoRcvPhysSize;
    uint32 ProtoSendPhysAddr;
    uint32 ProtoSendPhysSize;
} ShMem_SBNEntry_t;


/**
 * Struct that holds information about a memory segment, including its ioremapped
 * base address, the address where the queue actually starts after the header,
 * and where the pointers are. 
 */
typedef struct {
    uint32 BaseAddr;
    uint32 QueueAddr;
    uint32 QueueSize;
    uint32 ReadPtrAddr;
    uint32 WritePtrAddr;
    pthread_mutex_t* Mutex;
} ShMem_SBNMemSeg_t;


/**
 * Host data that contains the data receive, data send, and protocol send memory
 * segments. 
 */
typedef struct {
    ShMem_SBNMemSeg_t DataRcv;
    ShMem_SBNMemSeg_t DataSend;
    ShMem_SBNMemSeg_t ProtoSend;
} ShMem_SBNHostData_t;


/**
 * Peer data that contains the protocol send memory segment write to. This is
 * the only segment the peer needs to access. The "protocol send" segment for
 * peer data is actually the protocol receive segment on this CPU. 
 */
typedef struct {
    ShMem_SBNMemSeg_t ProtoSend;
} ShMem_SBNPeerData_t;


/**
 * \brief Peer Status
 *
 * Status info for a shared memory peer, or, more accurately, 
 * shared memory host.  Most of the information is from the 
 * ShMem_SBNHostData_t struct.
 */
typedef struct {
    uint32 DataRcvBaseAddr;
    uint32 DataRcvQueueAddr;
    uint32 DataRcvQueueSize;
    uint32 DataRcvReadPtr;
    uint32 DataRcvWritePtr;
    uint32 DataSendBaseAddr;
    uint32 DataSendQueueAddr;
    uint32 DataSendQueueSize;
    uint32 DataSendReadPtr;
    uint32 DataSendWritePtr;
    uint32 ProtoSendBaseAddr;
    uint32 ProtoSendQueueAddr;
    uint32 ProtoSendQueueSize;
    uint32 ProtoSendReadPtr;
    uint32 ProtoSendWritePtr;
    uint32 ProtoRcvBaseAddr;
    uint32 ProtoRcvQueueAddr;
    uint32 ProtoRcvQueueSize;
    uint32 ProtoRcvReadPtr;
    uint32 ProtoRcvWritePtr;
} ShMem_PeerStatus_t;


#endif
