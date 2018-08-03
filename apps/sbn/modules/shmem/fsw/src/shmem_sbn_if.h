#ifndef _shmem_sbn_if_h_
#define _shmem_sbn_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "shmem_sbn_if_struct.h"
#include "cfe.h"

#define SHMEM_SBN_ITEMS_PER_FILE_LINE      8
#define SHMEM_MUTEX_SIZE  sizeof(pthread_mutex_t)
#define SHMEM_HDR_SIZE    (sizeof(pthread_mutex_t) + 2*sizeof(uint32))

/* SBN Module functions */
int32 SBN_ShMemParseFileEntry(char *FileEntry, uint32 LineNum, int *EntryAddr);
int32 SBN_ShMemInitIF(SBN_InterfaceData* data);
int32 SBN_ShMemSendNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[],
                          int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData,
                          SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf);
int32 SBN_ShMemCheckForNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf);
int32 SBN_ShMemRcvMsg(SBN_InterfaceData *Host, NetDataUnion *DataMsgBuf);
int32 SBN_ShMemVerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts);
int32 SBN_ShMemVerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *Peers, int32 NumPeers);
int32 SBN_ShMemReportModuleStatus(SBN_ModuleStatusPacket_t *, SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts);
int32 SBN_ShMemResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts);

/* Internal functions */
int32 ShMem_IsHostPeerMatch(ShMem_SBNEntry_t *Host, ShMem_SBNEntry_t *Peer);
int32 ShMem_MapMemory(uint32 PhysAddr, uint32 MemSize, ShMem_SBNMemSeg_t *MemSegPtr);
int32 ShMem_CreateMutex(uint32 MutAddr, pthread_mutex_t **MutPtr);
int32 ShMem_InitReadWritePointers(ShMem_SBNMemSeg_t *MemSeg);
int32 ShMem_ReadSharedMem(ShMem_SBNMemSeg_t MemSeg, char *MsgBuf);
int32 ShMem_WriteSharedMem(ShMem_SBNMemSeg_t MemSeg, uint16 MsgSize, char *MsgBuf);
int32 ShMem_CheckReadPtr(ShMem_SBNMemSeg_t MemSeg);
void  ShMem_CheckWritePtr(ShMem_SBNMemSeg_t MemSeg, uint16 MsgSize);

/**
 * Interface operations struct representing functions that are called by SBN
 */
SBN_InterfaceOperations ShMemOps = {
    SBN_ShMemParseFileEntry,
    SBN_ShMemInitIF,
    SBN_ShMemSendNetMsg,
    SBN_ShMemCheckForNetProtoMsg,
    SBN_ShMemRcvMsg,
    SBN_ShMemVerifyPeerInterface,
    SBN_ShMemVerifyHostInterface,
    SBN_ShMemReportModuleStatus,
    SBN_ShMemResetPeer
};

#endif

