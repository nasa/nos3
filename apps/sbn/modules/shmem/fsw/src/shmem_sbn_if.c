/**
 * @file
 *
 * This file contains all functions that are called by SBN, plus some utility
 * functions.
 *
 * @author Jaclyn Beck
 * @date 2015/06/25 18:00:00
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "cfe.h"
#include "shmem_sbn_if.h"
#include "shmem_sbn_if_struct.h"

/**
 * Parses the peer data file into SBN_FileEntry_t structures.
 * Parses information that is common to all interface types and
 * allows individual interface modules to parse out interface-
 * specfic information.
 *
 * @param FileEntry  Interface description line as read from file
 * @param LineNum    The line number in the peer file
 * @param EntryAddr  Address in which to return the filled entry struct
 * @return SBN_OK    if entry is parsed correctly, else
 * @return SBN_ERROR otherwise
 *
 */
int32 SBN_ShMemParseFileEntry(char *FileEntry, uint32 LineNum, int *EntryAddr) {
    int  ScanfStatus;
    unsigned int DataRcvAddr;
    unsigned int DataRcvSize;
    unsigned int DataSendAddr;
    unsigned int DataSendSize;
    unsigned int ProtoRcvAddr;
    unsigned int ProtoRcvSize;
    unsigned int ProtoSendAddr;
    unsigned int ProtoSendSize;
    ShMem_SBNEntry_t *entry;

    if (FileEntry == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                "%s: cannot parse file entry because FileEntry is null.\n",
                CFE_CPU_NAME);
        return SBN_ERROR;
    }

    /*
    ** Using sscanf to parse the string.
    ** Currently no error handling
    */
    ScanfStatus = sscanf(FileEntry, "%x %x %x %x %x %x %x %x",
                         &DataRcvAddr, &DataRcvSize, &DataSendAddr, 
                         &DataSendSize, &ProtoRcvAddr, &ProtoRcvSize, 
                         &ProtoSendAddr, &ProtoSendSize);

    /*
    ** Check to see if the correct number of items were parsed
    */
    if (ScanfStatus != SHMEM_SBN_ITEMS_PER_FILE_LINE) {
        CFE_EVS_SendEvent(SBN_INV_LINE_EID,CFE_EVS_ERROR,
                "%s:Invalid SBN peer file line,exp %d items,found %d",
                CFE_CPU_NAME, SHMEM_SBN_ITEMS_PER_FILE_LINE, ScanfStatus);
        return SBN_ERROR;
    }

    entry = malloc(sizeof(ShMem_SBNEntry_t));
    if (entry == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                "%s: Failed to allocate memory for host/peer file entry.\n",
                CFE_CPU_NAME);
        return SBN_ERROR;
    }

    *EntryAddr = (int)entry;

    entry->DataRcvPhysAddr   = DataRcvAddr;
    entry->DataRcvPhysSize   = DataRcvSize;
    entry->DataSendPhysAddr  = DataSendAddr;
    entry->DataSendPhysSize  = DataSendSize;
    entry->ProtoRcvPhysAddr  = ProtoRcvAddr;
    entry->ProtoRcvPhysSize  = ProtoRcvSize;
    entry->ProtoSendPhysAddr = ProtoSendAddr;
    entry->ProtoSendPhysSize = ProtoSendSize;

    return SBN_OK;
}


/**
 * Initializes a shared memory host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_HOST  if a host was just initialized, else
 * @return SBN_PEER  if a peer was just initialized, else
 * @return SBN_ERROR code otherwise
 */
int32 SBN_ShMemInitIF(SBN_InterfaceData *Data) {
    ShMem_SBNEntry_t *entry;

    if (Data == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Cannot initialize interface! Interface data is null.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    entry = (ShMem_SBNEntry_t *)Data->EntryData;

    /* CPU names match - this is host data.
       Create msg interface when we find entry matching its own name
       because the self entry has port info needed to bind this interface. */
    if(strncmp(Data->Name, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH) == 0){

        /* create, fill, and store a shared memory-specific data structure */
        ShMem_SBNHostData_t *host = malloc(sizeof(ShMem_SBNHostData_t));
        if (host == NULL) {
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Cannot allocate host interface data.\n",
                          CFE_CPU_NAME);
            return SBN_ERROR;
        }

        /* mmap memory for data receive, data send, and protocol send */
        if ( ShMem_MapMemory(entry->DataRcvPhysAddr,   entry->DataRcvPhysSize,   &host->DataRcv)   == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_MapMemory(entry->DataSendPhysAddr,  entry->DataSendPhysSize,  &host->DataSend)  == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_MapMemory(entry->ProtoSendPhysAddr, entry->ProtoSendPhysSize, &host->ProtoSend) == SBN_ERROR )
            return SBN_ERROR;

        /* Create mutexes at the addresses of the protocol send and data send/rcv */
        if ( ShMem_CreateMutex(host->DataRcv.BaseAddr,   &host->DataRcv.Mutex)   == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_CreateMutex(host->DataSend.BaseAddr,  &host->DataSend.Mutex)  == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_CreateMutex(host->ProtoSend.BaseAddr, &host->ProtoSend.Mutex) == SBN_ERROR )
            return SBN_ERROR;

        /* Initialize read and write pointers of queues. */
        if ( ShMem_InitReadWritePointers(&host->DataRcv)   == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_InitReadWritePointers(&host->DataSend)  == SBN_ERROR )
            return SBN_ERROR;

        if ( ShMem_InitReadWritePointers(&host->ProtoSend) == SBN_ERROR )
            return SBN_ERROR;

        Data->HostData = (int32)host;
        return SBN_HOST;
    }
    /* CPU names do not match - this is peer data. */
    else {
        ShMem_SBNPeerData_t *peer = malloc(sizeof(ShMem_SBNPeerData_t));
        if (peer == NULL) {
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                              "%s: ShMem: Cannot allocate peer interface data.\n",
                              CFE_CPU_NAME);
            return SBN_ERROR;
        }

        /* mmap memory for peer's protocol send */
        if ( ShMem_MapMemory(entry->ProtoSendPhysAddr, entry->ProtoSendPhysSize, &peer->ProtoSend) == SBN_ERROR )
            return SBN_ERROR;

        /* Create peer's protocol send mutex */
        if ( ShMem_CreateMutex(peer->ProtoSend.BaseAddr, &peer->ProtoSend.Mutex) == SBN_ERROR )
            return SBN_ERROR;

        /* Initialize read and write pointers of queue */
        if ( ShMem_InitReadWritePointers(&peer->ProtoSend) == SBN_ERROR )
            return SBN_ERROR;

        Data->PeerData = (int32)peer;
        return SBN_PEER;
    }
}


/**
 * Sends a message to a peer over a shared memory interface.
 *
 * @param MsgType      Type of Message
 * @param MsgSize      Size of Message
 * @param HostList     The array of SBN_InterfaceData structs that describes the host
 * @param SenderPtr    Sender information
 * @param IfData       The SBN_InterfaceData struct describing this peer
 * @param ProtoMsgBuf  Protocol message
 * @param DataMsgBuf   Data message
 * @return Number of bytes written on success
 * @return SBN_ERROR on error
 */
int32 SBN_ShMemSendNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[], int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData, SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf) {
    int status, found = 0;
    ShMem_SBNHostData_t *host;
    uint32 HostIdx;
    ShMem_SBNEntry_t *host_tmp;
    ShMem_SBNEntry_t *peer;


    /* Check pointer arguments used for all cases for null */
    if (HostList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemSendNetMsg: HostList is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }
    if (IfData == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemSendNetMsg: IfData is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    /* Find the host that goes with this peer. */
    peer = (ShMem_SBNEntry_t *)IfData->EntryData;

    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        if(HostList[HostIdx]->ProtocolId == SBN_SHMEM) {

            host_tmp = (ShMem_SBNEntry_t *)HostList[HostIdx]->EntryData;
            if ( ShMem_IsHostPeerMatch(host_tmp, peer) ) {
                found = 1;
                host = (ShMem_SBNHostData_t*)HostList[HostIdx]->HostData;
                break;
            }
        }
    }
    if(found != 1) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: No Shared Memory Host Found!\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    switch(MsgType) {

        case SBN_APP_MSG: /* If my peer sent this message, don't send it back to them, avoids loops */
            if (SenderPtr == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "%s: ShMem: Error in ShMemSendNetMsg: SenderPtr is NULL.\n",
                                  CFE_CPU_NAME);
                return SBN_ERROR;
            }

            if (CFE_PSP_GetProcessorId() != SenderPtr->ProcessorId)
                break;

            /* Then no break, so fill in the sender application infomation */
            if (DataMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "%s: ShMem: Error in ShMemSendNetMsg: DataMsgBuf is NULL.\n",
                                  CFE_CPU_NAME);
                return SBN_ERROR;
            }
            strncpy((char *)&(DataMsgBuf->Hdr.MsgSender.AppName), &SenderPtr->AppName[0], OS_MAX_API_NAME);
            DataMsgBuf->Hdr.MsgSender.ProcessorId = SenderPtr->ProcessorId;
            /* Fall through to the next case */

        case SBN_SUBSCRIBE_MSG:
        case SBN_UN_SUBSCRIBE_MSG:
            if (DataMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "%s: ShMem: Error in ShMemSendNetMsg: DataMsgBuf is NULL.\n",
                                  CFE_CPU_NAME);
                return SBN_ERROR;
            }

            /* Initialize the SBN hdr of the outgoing network message */
 //           strncpy((char *)&DataMsgBuf->Hdr.SrcCpuName,CFE_CPU_NAME,SBN_MAX_PEERNAME_LENGTH);
            DataMsgBuf->Hdr.Type = MsgType;

            /* Write DataMsgBuf to data send shared memory */
            if(DataMsgBuf->Hdr.SrcCpuName[0] == 0) {
                printf("SrcCpuName send DataMsgBuf is bad\n");
            }
            status = ShMem_WriteSharedMem( host->DataSend, MsgSize, (char *)DataMsgBuf );
            break;

        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:
            if (ProtoMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "%s: ShMem: Error in ShMemSendNetMsg: ProtoMsgBuf is NULL.\n",
                                  CFE_CPU_NAME);
                  return SBN_ERROR;
            }

            ProtoMsgBuf->Hdr.Type = MsgType;
   //         strncpy(ProtoMsgBuf->Hdr.SrcCpuName, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH);

            /* Write ProtoMsgBuf to protocol send shared memory */
            if(ProtoMsgBuf->Hdr.SrcCpuName[0] == 0) {
                printf("SrcCpuName send ProtoMsgBuf is bad\n");
            }
            status = ShMem_WriteSharedMem( host->ProtoSend, MsgSize, (char *)ProtoMsgBuf );
            break;

        default:
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Unexpected message type!\n",
                          CFE_CPU_NAME);
            status = SBN_ERROR;
            break;
    } /* end switch */

    return (status);
} /* end SBN_SendNetMsg */


/**
 * Checks for a single protocol message.
 *
 * @param Peer         Structure of interface data for a peer
 * @param ProtoMsgBuf  Pointer to the SBN's protocol message buffer
 * @return 1 for message available, else
 * @return 0 for no messages or an error
 */
int32 SBN_ShMemCheckForNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf) {
    int status;
    ShMem_SBNPeerData_t *peer;

    if (ProtoMsgBuf == NULL) {
        CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in CheckForNetProtoMsgs: ProtoMsgBuf is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }
    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in CheckForNetProtoMsgs: Peer is NULL.\n",
                          CFE_CPU_NAME);
        ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
        return SBN_ERROR;
    }

    peer = (ShMem_SBNPeerData_t *)Peer->PeerData;

    /* Read from peer's protocol send (our proto receive) shared memory into ProtoMsgBuf */
    status = ShMem_ReadSharedMem( peer->ProtoSend, (char *)ProtoMsgBuf );

    if (status > 0) /* Positive number indicates byte length of message */
        return SBN_TRUE; /* Message available and no errors */

    if ( status == SBN_ERROR ) {
            CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                              "%s: ShMem: Error in CheckForNetProtoMsgs stat=%d",
                              CFE_CPU_NAME, status);
            ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
            return SBN_ERROR;
    }

    /* status = 0, so no messages and no errors */
    ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
    return SBN_NO_MSG;
} /* end SBN_CheckForNetProtoMsg */


/**
 * Receives a message from a peer over the appropriate interface.
 *
 * @param Host        Structure of interface data for a peer
 * @param DataMsgBuf  Pointer to the SBN's protocol message buffer
 * @return Bytes received on success, else
 * @return SBN_IF_EMPTY if empty, else
 * @return SBN_ERROR    on error
 */
int32 SBN_ShMemRcvMsg(SBN_InterfaceData *Host, NetDataUnion *DataMsgBuf) {
    int status;
    ShMem_SBNHostData_t *host;

    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemRcvMsg: Host is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }
    if (DataMsgBuf == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemRcvMsg: DataMsgBuf is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    host = (ShMem_SBNHostData_t *)Host->HostData;

    /* Read from data receive shared memory into DataMsgBuf */
    status = ShMem_ReadSharedMem( host->DataRcv, (char *)DataMsgBuf );
    if(DataMsgBuf->Hdr.SrcCpuName[0] == 0) {
        printf("SrcCpuName rcv is bad\n");
        printf("Status = %d\n", status);
        printf("Msg Type = %x\n", DataMsgBuf->Hdr.Type);
    }
    
    if ( status == 0 )
        return SBN_IF_EMPTY;
    else
        return status;
}


/**
 * Iterates through the list of all host interfaces to see if there is a
 * match for the specified peer interface.  This function must be present,
 * but can return SBN_VALID for interfaces that don't require a match.
 *
 * @param SBN_InterfaceData *Peer    Peer to verify
 * @param SBN_InterfaceData *Hosts[] List of hosts to check against the peer
 * @param int32 NumHosts             Number of hosts in the SBN
 * @return SBN_VALID     if the required match exists, else
 * @return SBN_NOT_VALID if not
 */
int32 SBN_ShMemVerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts) {
    int HostIdx;
    ShMem_SBNEntry_t *HostEntry;
    ShMem_SBNEntry_t *PeerEntry;

    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemVerifyPeerInterface: Peer is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_NOT_VALID;
    }
    if (Hosts == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemVerifyPeerInterface: Hosts is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_NOT_VALID;
    }

    PeerEntry = (ShMem_SBNEntry_t *)Peer->EntryData;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        if(Hosts[HostIdx]->ProtocolId == SBN_SHMEM) {
            HostEntry = (ShMem_SBNEntry_t *)Hosts[HostIdx]->EntryData;

            if ( ShMem_IsHostPeerMatch(HostEntry, PeerEntry) ) {
                return SBN_VALID;
            }
        }
    }

    return SBN_NOT_VALID;
}


/**
 * Iterates through the list of all peer interfaces to see if there is a
 * match for the specified host interface.  This function must be present,
 * but can return SBN_VALID for interfaces that don't require a match.
 *
 * @param SBN_InterfaceData *Host    Host to verify
 * @param SBN_PeerData_t *Peers      List of peers to check against the host
 * @param int32 NumPeers             Number of peers in the SBN
 * @return SBN_VALID     if the required match exists, else
 * @return SBN_NOT_VALID if not
 */
int32 SBN_ShMemVerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *Peers, int32 NumPeers) {
    int PeerIdx;
    ShMem_SBNEntry_t *PeerEntry;
    ShMem_SBNEntry_t *HostEntry;

    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemVerifyHostInterface: Host is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_NOT_VALID;
    }
    if (Peers == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemVerifyHostInterface: Peers is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_NOT_VALID;
    }

    HostEntry = (ShMem_SBNEntry_t *)Host->EntryData;

    /* Find the peer that goes with this host. */
    for(PeerIdx = 0; PeerIdx < NumPeers; PeerIdx++) {
        if(Peers[PeerIdx].ProtocolId == SBN_SHMEM) {
            PeerEntry = (ShMem_SBNEntry_t *)(Peers[PeerIdx].IfData)->EntryData;

            if ( ShMem_IsHostPeerMatch(HostEntry, PeerEntry) ) {
                return SBN_VALID;
            }
        }
    }

    return SBN_NOT_VALID;
}


/**
 * Reports status on the specified peer and the corresponding host.
 *
 * @param SBN_ModuleStatusPacket_t *Packet  The status packet to fill
 * @param SBN_InterfaceData *Peer           The peer on which to report status
 * @param SBN_InterfaceData *Hosts          List of hosts in the SBN
 * @param NumHosts                          Number of hosts in the SBN
 * @return SBN_OK
 */
int32 SBN_ShMemReportModuleStatus(SBN_ModuleStatusPacket_t *Packet, SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts) {
    int32 HostIdx, copySize;
    ShMem_PeerStatus_t PeerStatus;
    ShMem_SBNEntry_t *HostEntry;
    ShMem_SBNEntry_t *PeerEntry;
    ShMem_SBNHostData_t *HostData = NULL;
    ShMem_SBNPeerData_t *PeerData;

    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemReportModuleStatus: Peer is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }
    if (Hosts == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemReportModuleStatus: Hosts is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    PeerEntry = (ShMem_SBNEntry_t *)Peer->EntryData;
    PeerData = (ShMem_SBNPeerData_t *)Peer->PeerData;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        if(Hosts[HostIdx]->ProtocolId == SBN_SHMEM) {
            HostEntry = (ShMem_SBNEntry_t *)Hosts[HostIdx]->EntryData;

            if ( ShMem_IsHostPeerMatch(HostEntry, PeerEntry) ) {
                HostData = (ShMem_SBNHostData_t *)Hosts[HostIdx]->HostData;
                break;
            }
        }
    }

    if(HostData == NULL) {
        return SBN_ERROR;
    }

    CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_DEBUG,
        "ShMem module found host data\n");

    PeerStatus.DataRcvBaseAddr  = HostData->DataRcv.BaseAddr;
    PeerStatus.DataRcvQueueAddr = HostData->DataRcv.QueueAddr;
    PeerStatus.DataRcvQueueSize = HostData->DataRcv.QueueSize;
    PeerStatus.DataRcvReadPtr   = *((uint32*)HostData->DataRcv.ReadPtrAddr);
    PeerStatus.DataRcvWritePtr  = *((uint32*)HostData->DataRcv.WritePtrAddr);
    PeerStatus.DataSendBaseAddr = HostData->DataSend.BaseAddr;
    PeerStatus.DataSendQueueAddr = HostData->DataSend.QueueAddr;
    PeerStatus.DataSendQueueSize = HostData->DataSend.QueueSize;
    PeerStatus.DataSendReadPtr   = *((uint32*)HostData->DataSend.ReadPtrAddr);
    PeerStatus.DataSendWritePtr  = *((uint32*)HostData->DataSend.WritePtrAddr);
    PeerStatus.ProtoSendBaseAddr = HostData->ProtoSend.BaseAddr;
    PeerStatus.ProtoSendQueueAddr = HostData->ProtoSend.QueueAddr;
    PeerStatus.ProtoSendQueueSize = HostData->ProtoSend.QueueSize;
    PeerStatus.ProtoSendReadPtr  = *((uint32*)HostData->ProtoSend.ReadPtrAddr);
    PeerStatus.ProtoSendWritePtr = *((uint32*)HostData->ProtoSend.WritePtrAddr);
    PeerStatus.ProtoRcvBaseAddr  = PeerData->ProtoSend.BaseAddr;
    PeerStatus.ProtoRcvQueueAddr = PeerData->ProtoSend.QueueAddr;
    PeerStatus.ProtoRcvQueueSize = PeerData->ProtoSend.QueueSize;
    PeerStatus.ProtoRcvReadPtr   = *((uint32*)PeerData->ProtoSend.ReadPtrAddr);
    PeerStatus.ProtoRcvWritePtr  = *((uint32*)PeerData->ProtoSend.WritePtrAddr);

    if(sizeof(ShMem_PeerStatus_t) < SBN_MOD_STATUS_MSG_SIZE) {
        copySize = sizeof(ShMem_PeerStatus_t);
    }
    else {
        copySize = SBN_MOD_STATUS_MSG_SIZE;
    }

    memcpy(Packet->ModuleStatus, &PeerStatus, copySize);

    return SBN_OK;
}


/**
 * Resets the specified peer and any relevant counters in the corresponding
 * host. For the shared memory interface, this means erasing everything in
 * the queues and setting the read/write pointers to 0.
 *
 * @param SBN_InterfaceData *Peer           The peer on which to report status
 * @param SBN_InterfaceData *Hosts          List of hosts in the SBN
 * @param NumHosts                          Number of hosts in the SBN
 * @return  SBN_OK if the peer is successfully reset, SBN_ERROR otherwise
 */
int32 SBN_ShMemResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *Hosts[], int32 NumHosts) {
    int32 HostIdx;
    ShMem_SBNEntry_t *HostEntry;
    ShMem_SBNEntry_t *PeerEntry;
    ShMem_SBNHostData_t *HostData = NULL;
    ShMem_SBNPeerData_t *PeerData;

    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemResetPeer: Peer is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }
    if (Hosts == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ShMemResetPeer: Hosts is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    PeerEntry = (ShMem_SBNEntry_t *)Peer->EntryData;
    PeerData = (ShMem_SBNPeerData_t *)Peer->PeerData;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        if(Hosts[HostIdx]->ProtocolId == SBN_SHMEM) {
            HostEntry = (ShMem_SBNEntry_t *)Hosts[HostIdx]->EntryData;

            if ( ShMem_IsHostPeerMatch(HostEntry, PeerEntry) ) {
                HostData = (ShMem_SBNHostData_t *)Hosts[HostIdx]->HostData;
                break;
            }
        }
    }

    if(HostData == NULL) {
        return SBN_ERROR;
    }
OS_printf("Resetting...\n");
    /* This function call erases all queues and sets the read/write pointers to 0 */
    if ( ShMem_InitReadWritePointers(&HostData->DataRcv)   == SBN_ERROR )
        return SBN_ERROR;

    if ( ShMem_InitReadWritePointers(&HostData->DataSend)  == SBN_ERROR )
        return SBN_ERROR;

    if ( ShMem_InitReadWritePointers(&HostData->ProtoSend) == SBN_ERROR )
        return SBN_ERROR;

    if ( ShMem_InitReadWritePointers(&PeerData->ProtoSend) == SBN_ERROR )
        return SBN_ERROR;
OS_printf("Reset\n");
    return SBN_OK;
}


/******************************************************************************/
/* Internal functions                                                         */
/******************************************************************************/


/**
 * Compares the four addresses of a host and a peer entry to see if their
 * crossovers match.
 *
 * @param ShMem_SBNEntry_t *Host    Host to verify
 * @param ShMem_SBNEntry_t *Peer    Peer to verify
 * @return 1 if the two match, else
 * @return 0 if not
 */
int32 ShMem_IsHostPeerMatch(ShMem_SBNEntry_t *Host, ShMem_SBNEntry_t *Peer) {
    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in IsHostPeerMatch: Host is NULL.\n",
                          CFE_CPU_NAME);
        return 0;
    }
    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in IsHostPeerMatch: Peer is NULL.\n",
                          CFE_CPU_NAME);
        return 0;
    }

    if ( Host->DataRcvPhysAddr   == Peer->DataSendPhysAddr &&
         Host->DataSendPhysAddr  == Peer->DataRcvPhysAddr &&
         Host->ProtoRcvPhysAddr  == Peer->ProtoSendPhysAddr &&
         Host->ProtoSendPhysAddr == Peer->ProtoRcvPhysAddr ) {
        return 1;
    }

    return 0;
}


/**
 * Linux-specific: Maps a physical address in shared memory to a virtual
 * address, and adjusts the virtual size to account for the header of the
 * memory segment (24 bytes of mutex, 4 bytes of read pointer, 4 bytes of write
 * ptr).
 *
 * @param uint32            PhysAddr    Physical address in shared memory
 * @param uint32            MemSize     Physical size of this chunk of memory
 * @param ShMem_SBNMemSeg_t *MemSegPtr  Pointer to struct that describes the memory region
 * @return SBN_ERROR if unable to map memory, else
 * @return SB_OK     if success
 */
int32 ShMem_MapMemory(uint32 PhysAddr, uint32 MemSize, ShMem_SBNMemSeg_t *MemSegPtr) {
    int fd;
    char *memptr;

    fd = open("/dev/mem", O_RDWR);

    if (fd <= 0) {
        return SBN_ERROR;
    }

    memptr = mmap(NULL, MemSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PhysAddr);

    if ((int)memptr == -1) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Unable to map physical memory location 0x%08x "
                          "with size 0x%x to a virtual address. Errno=%d\n",
                          CFE_CPU_NAME, PhysAddr, MemSize, errno);
        return SBN_ERROR;
    }

    MemSegPtr->BaseAddr  = (uint32)memptr;
    MemSegPtr->QueueAddr = (uint32)memptr + SHMEM_HDR_SIZE;
    MemSegPtr->QueueSize = MemSize - SHMEM_HDR_SIZE;

    return SBN_OK;
}


/**
 * Linux-specific: Creates a mutex at a specific memory location by changing the
 * mutex pointer to that location. A pointer to the mutex pointer is passed in
 * so that the address of the mutex pointer can be changed.
 *
 * @param uint32 MutAddr            Address to create the mutex at
 * @param pthread_mutex_t **MutPtr  Pointer to mutex pointer
 * @return SBN_ERROR if unable to initialize mutex, else
 * @return SBN_OK    on success
 */
int32 ShMem_CreateMutex(uint32 MutAddr, pthread_mutex_t **MutPtr) {
    int status;

    *MutPtr = (pthread_mutex_t*)MutAddr;
    status = pthread_mutex_init(*MutPtr, NULL);

    if (status < 0) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Unable to create mutex at virtual location 0x%08x."
                          "Return status: %d\n",
                          CFE_CPU_NAME, MutAddr, status);
        return SBN_ERROR;
    }

    return SBN_OK;
}


/**
 * Linux-specific: Initializes the read and write pointers of the queue of
 * messages at this memory location. The read pointer is located at byte 24 and
 * indicates where to start reading a message from. The write pointer is located
 * at byte 28 and indicates where to start writing to. Both pointers are
 * relative to the start of the message queue, not absolute addresses.
 *
 * @param ShMem_SBNMemSeg_t *MemSegPtr   Pointer to struct that describes the memory region
 * @return SBN_ERROR if error locking or unlocking mutex, else
 * @return SBN_OK    on success
 */
int32 ShMem_InitReadWritePointers(ShMem_SBNMemSeg_t *MemSegPtr) {
    uint32 *ReadPtr, *WritePtr, status;

    if (MemSegPtr->Mutex == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in InitReadWritePointers: Mutex is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    /* Get mutex, this is a blocking operation unlike other reads/writes because
       this is a critical operation. */
    status = pthread_mutex_lock(MemSegPtr->Mutex);
    if (status < 0) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Unable to lock mutex while initializing "
                          "read/write queues. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    /* Set ReadPtr/WritePtr pointers to position 0 for the first message */
    MemSegPtr->ReadPtrAddr = MemSegPtr->BaseAddr + SHMEM_MUTEX_SIZE;
    ReadPtr  = (uint32*)MemSegPtr->ReadPtrAddr;
    *ReadPtr = 0;

    MemSegPtr->WritePtrAddr = MemSegPtr->BaseAddr + SHMEM_MUTEX_SIZE + sizeof(uint32);
    WritePtr  = (uint32*)MemSegPtr->WritePtrAddr;
    *WritePtr = 0;

    /* Erase everything in this memory block */
    CFE_PSP_MemSet(MemSegPtr->QueueAddr, 0, MemSegPtr->QueueSize);

    /* Release mutex */
    status = pthread_mutex_unlock(MemSegPtr->Mutex);
    if(status < 0) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Unable to unlock mutex while initializing "
                          "read/write queues. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    return SBN_OK;
}


/**
 * Linux-specific: Reads from shared memory into a message buffer. Each message
 * in memory has 2 bytes indicating the length of the message and then the
 * message itself.
 *
 * @param ShMem_SBNMemSeg_t MemSeg   Struct that describes the memory region
 * @param char *MsgBuf               Character buffer to put the read bytes into
 * @return SBN_ERROR  if error locking or unlocking mutex, else
 * @return 0          if memory is empty or mutex was busy, else
 * @return length of message read from memory
 */
int32 ShMem_ReadSharedMem(ShMem_SBNMemSeg_t MemSeg, char *MsgBuf) {
    int status;
    uint16 length = 0;
    uint32 *ReadPtr = (uint32*)MemSeg.ReadPtrAddr;
    char *shmem;

    if (MemSeg.Mutex == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in ReadSharedMem: Mutex is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    /* Get receive mutex */
    status = pthread_mutex_trylock(MemSeg.Mutex);
    if ( (status == EBUSY) || (status == EAGAIN) ) {
        /* Mutex was busy, return without reading */
        return 0;
    }
    else if ( status != 0 ) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error locking mutex for read. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    /* Make sure ReadPtr is at the right location and doesn't need to wrap.
       Also verify the length of what is about to be read. */
    if ( ShMem_CheckReadPtr(MemSeg) == SBN_ERROR )
        return SBN_ERROR;

    /* Move to where ReadPtr points to, write what's there to MsgBuf */
    shmem = (char*)(MemSeg.QueueAddr + *ReadPtr);
    memcpy(&length, shmem, sizeof(length));           /* Read the length */
    memcpy(MsgBuf, (shmem + sizeof(length)), length); /* Read the message */

    /* Clear memory at this location and move read pointer */
    if (length > 0) {
        CFE_PSP_MemSet(shmem, 0, length+sizeof(length));
        *ReadPtr = *ReadPtr + length + sizeof(length);
    }

    /* Release receive mutex */
    status = pthread_mutex_unlock(MemSeg.Mutex);
    if(status < 0) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error unlocking mutex after read. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    return (int32)length;
}


/**
 * Linux-specific: Writes from a message buffer into shared memory. Two bytes
 * indicating the length of the message are written first, then the message.
 *
 * @param ShMem_SBNMemSeg_t MemSeg   Struct that describes the memory region
 * @param uint16            MsgSize  Size of the message to write
 * @param char              *MsgBuf  Character buffer to write bytes from
 * @return SBN_ERROR  if error locking or unlocking mutex, or message length error, else
 * @return 0          if mutex was busy
 * @return MemSize    if successfully wrote MemSize bytes
 */
int32 ShMem_WriteSharedMem(ShMem_SBNMemSeg_t MemSeg, uint16 MsgSize, char *MsgBuf) {
    int status;
    uint32 *WritePtr = (uint32*)MemSeg.WritePtrAddr;
    char *shmem;

    if (MemSeg.Mutex == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in WriteSharedMem: Mutex is NULL.\n",
                          CFE_CPU_NAME);
        return SBN_ERROR;
    }

    /* If we can't fit this message and its length in memory, give an error */
    if ( MsgSize > (MemSeg.QueueSize - sizeof(MsgSize)) ) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error in WriteSharedMem: MsgSize %d will not fit in memory.\n",
                          CFE_CPU_NAME, MsgSize);
        return SBN_ERROR;
    }

    /* Get send mutex */
    status = pthread_mutex_trylock(MemSeg.Mutex);
    if ( (status == EBUSY) || (status == EAGAIN) ) {
        /* Mutex was busy, return without reading */
        return 0;
    }
    else if ( status != 0 ) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error locking mutex for write. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    /* Check WritePtr. If it is about to overtake ReadPtr, ReadPtr needs to be
       moved over first to avoid the next read being at a bad boundary */
    ShMem_CheckWritePtr(MemSeg, MsgSize);

    /* Move to where WritePtr points to, write MsgBuf to memory */
    shmem = (char*)(MemSeg.QueueAddr + *WritePtr);
    memcpy(shmem, &MsgSize, sizeof(MsgSize));         /* Write size of message */
    memcpy((shmem+sizeof(MsgSize)), MsgBuf, MsgSize); /* Write message */

    /* Update WritePtr. */
    *WritePtr = *WritePtr + MsgSize + sizeof(MsgSize);

    /* Release send mutex */
    status = pthread_mutex_unlock(MemSeg.Mutex);
    if(status < 0) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error unlocking mutex for read. Return status: %d\n",
                          CFE_CPU_NAME, status);
        return SBN_ERROR;
    }

    return MsgSize;
}


/**
 * Linux-specific: Checks several properties of ReadPtr for safety :
 * - 1. Is it in a valid region of memory or does it need to wrap back to 0?
 * - 2. Is the length of the message at ReadPtr a valid number?
 * - 3. If length is valid, is it too long for the message to fit between ReadPtr
 *    and the end of memory?
 *        If so, this is a signal that ReadPtr should wrap back to 0.
 *
 * It is assumed that a mutex has been locked before calling this function.
 *
 * @param ShMem_SBNMemSeg_t MemSeg  Struct describing this memory region
 * @return SBN_ERROR if the length is invalid
 * @return SBN_OK    otherwise
 */
int32 ShMem_CheckReadPtr(ShMem_SBNMemSeg_t MemSeg) {
    uint32 *ReadPtr  = (uint32*)MemSeg.ReadPtrAddr;
    uint32 *WritePtr = (uint32*)MemSeg.WritePtrAddr;
    uint16 length;
    char *shmem;

    if (*ReadPtr > MemSeg.QueueSize - sizeof(uint16) ) {
        *ReadPtr = 0;
    }

    shmem = (char*)(MemSeg.QueueAddr + *ReadPtr);
    memcpy(&length, shmem, sizeof(length));

    /* If length is too long to fit in memory, this is an error and we should
       reset ReadPtr AND WritePtr to the beginning of memory and start the
       queue over. */
    if ( length > (MemSeg.QueueSize - sizeof(length)) ) {
        *ReadPtr  = 0;
        *WritePtr = 0;
        CFE_PSP_MemSet(MemSeg.QueueAddr, 0, MemSeg.QueueSize);

        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "%s: ShMem: Error during read: Invalid length %d read, ReadPtr may be at an invalid location. Restarting message queue.\n",
                          CFE_CPU_NAME, length);
        return SBN_ERROR;
    }

    /* If length is longer than the amount of space left in memory, wrap ReadPtr
       to the beginning. */
    if ( (*ReadPtr + length + sizeof(length)) > MemSeg.QueueSize ) {
        CFE_PSP_MemSet(shmem, 0, sizeof(length));
        *ReadPtr = 0;
    }

    return SBN_OK;
}


/**
 * Linux-specific: Checks two things :
 * - 1. Is there enough space between WritePtr and the end of the memory block to
 *    write the pending message?
 *        If not, write just the length of the message and wrap WritePtr to the
 *        beginning of memory. A length that is too long will signal ReadPtr to
 *        wrap too.
 * - 2. Will the pending write overwrite part or all of what is at ReadPtr?
 *        If it will, ReadPtr is moved over to the next message so that the
 *        write can safely be performed.
 *
 * It is assumed that a mutex has been locked before calling this function.
 *
 * @param ShMem_SBNMemSeg_t MemSeg    Struct describing this memory region
 * @param uint32            MsgSize   Number of bytes of the message to write
 */
void ShMem_CheckWritePtr(ShMem_SBNMemSeg_t MemSeg, uint16 MsgSize) {
    uint32 *WritePtr  = (uint32*)MemSeg.WritePtrAddr;
    uint32 *ReadPtr   = (uint32*)MemSeg.ReadPtrAddr;
    uint8  RoomAtEnd, RoomBetween, HasMessage;
    uint16 length;

    /* Make sure ReadPtr is at a valid location before doing anything to it */
    ShMem_CheckReadPtr(MemSeg);

    RoomAtEnd   = (*WritePtr + MsgSize + sizeof(MsgSize)) < MemSeg.QueueSize;
    RoomBetween = (*ReadPtr - *WritePtr) >= (MsgSize + sizeof(MsgSize));

    /* If there is enough space to write the message, this is safe */
    if ( ((*ReadPtr < *WritePtr) && RoomAtEnd) ||
         ((*ReadPtr > *WritePtr) && RoomBetween) ) {
        return;
    }

    /* Otherwise we need to check to see if there is a message at ReadPtr */
    memcpy(&length, (char*)(MemSeg.QueueAddr + *ReadPtr), sizeof(length));
    HasMessage = (length > 0);

    /* If they both point to the same place, there's no message waiting,
       and there's room to write the new message, this is safe */
    if ( (*ReadPtr == *WritePtr) && !HasMessage && RoomAtEnd ) {
        return;
    }

    /* If ReadPtr won't be overrun but there's not enough space to write the
       message at the end of memory, wrap WritePtr and perform a second check */
    if ( ((*ReadPtr < *WritePtr) && !RoomAtEnd) ||
         ((*ReadPtr == *WritePtr) && !HasMessage && !RoomAtEnd) ) {

        /* If we have room to write the length, write it. */
        if (*WritePtr <= MemSeg.QueueSize - sizeof(MsgSize)) {
            memcpy((MemSeg.QueueAddr + *WritePtr), &MsgSize, sizeof(MsgSize));
        }
        *WritePtr = 0;  /* Wrap to beginning */
        ShMem_CheckWritePtr(MemSeg, MsgSize); /* Re-check to make sure wrapping is ok */
    }

    /* If ReadPtr would be overwritten but there's no message, just set it to
       the same location as WritePtr and perform a second check.
       This case actually should never happen if the queue is performing correctly
       but is here just in case. */
    else if ( (*ReadPtr > *WritePtr) && !HasMessage && !RoomBetween ) {
        *ReadPtr = *WritePtr;
        ShMem_CheckWritePtr(MemSeg, MsgSize); /* Re-check to make sure WritePtr doesn't need to wrap too */
    }

    /* (ReadPtr == WritePtr && HasMessage), or (ReadPtr > WritePtr && HasMessage && !RoomBetween).
       ReadPtr needs to be moved out of the way and a second check performed */
    else {
        /* Otherwise erase the message to be skipped and move ReadPtr to the
               next message */
        printf("ShMem_CheckWritePtr: Erasing message and moving ReadPtr\n");
        CFE_PSP_MemSet((MemSeg.QueueAddr + *ReadPtr), 0,
            length + sizeof(length));
        *ReadPtr = *ReadPtr + length + sizeof(length);
        ShMem_CheckWritePtr(MemSeg, MsgSize); /* Re-check to make sure ReadPtr is out of the way */
    }
}

