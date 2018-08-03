/**
 * @file
 * 
 * This file contains all functions that SBN calls, plus some helper functions. 
 * 
 * @author AUTHOR
 * @date DATE
 */

#include "TEMPLATE_sbn_if_struct.h"
#include "TEMPLATE_sbn_if.h"
#include "cfe.h"
#include <string.h>


/**
 * Parses the peer data file into SBN_FileEntry_t structures.  
 * Parses information that is common to all interface types and 
 * allows individual interface modules to parse out interface-
 * specfic information.
 *
 * @param FileEntry  Interface description line as read from file
 * @param LineNum    The line number in the peer file
 * @param EntryAddr  Address in which to return the filled entry struct
 *
 * @return SBN_OK if entry is parsed correctly
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnParseInterfaceFileEntry(char *FileEntry, uint32 LineNum, int *EntryAddr) {
    char    Name[50];
    uint32  ProcessorId; 
    uint32  ProtocolId;
    uint32  SpaceCraftId;
    int     ScanfStatus;
    TEMPLATE_SBNEntry_t *entry;

    /*
    ** Using sscanf to parse the string.
    ** Currently no error handling
    */
    ScanfStatus = sscanf(FileEntry, "%s %d %d %d", Name, &ProcessorId, &ProtocolId, &SpaceCraftId); /* TODO add fields for this module */

    /* Fixme - 1) sscanf needs to be made safe. Use discrete sub functions to safely parse the file 
               3) Need check for my cpu name not found 
    */

    /*
    ** Check to see if the correct number of items were parsed
    */
    if (ScanfStatus != TEMPLATE_ITEMS_PER_FILE_LINE) { 

        CFE_EVS_SendEvent(SBN_INV_LINE_EID,CFE_EVS_ERROR,
                          "%s:Invalid SBN peer file line,exp %d items,found %d",
                          CFE_CPU_NAME, TEMPLATE_ITEMS_PER_FILE_LINE, ScanfStatus);
        return SBN_ERROR;                        
    }/* end if */

    entry = malloc(sizeof(TEMPLATE_SBNEntry_t));
    if (entry == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE cannot allocate memory to store a host/peer file entry.\n");
        return SBN_ERROR;
    }

    *EntryAddr = (int)entry;

    /* TODO other module-specific initialization of the entry */

    return SBN_OK;
}


/**
 * Initializes host or peer data struct depending on the CPU name.
 *
 * @param  Interface data structure containing the file entry
 * 
 * @return SBN_HOST if this entry is for a host (this CPU)
 * @return SBN_PEER if this entry is for a peer (a different CPU)
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnInitPeerInterface(SBN_InterfaceData *Data) {
    TEMPLATE_SBNEntry_t *entry;

    if (Data == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Cannot initialize interface! Interface data is null.\n");
        return SBN_ERROR;
    }

    entry = (TEMPLATE_SBNEntry_t *)Data->EntryData;

    /* CPU names match - this is host data.
       Create msg interface when we find entry matching its own name 
       because the self entry has port info needed to bind this interface. */
    if(strncmp(Data->Name, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH) == 0){

        /* create, fill, and store a TEMPLATE-specific host data structure */
        TEMPLATE_SBNHostData_t *host = malloc(sizeof(TEMPLATE_SBNHostData_t));
        if (host == NULL) {
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                            "TEMPLATE: Cannot allocate host interface data.\n");
			return SBN_ERROR;
		}

        /* TODO host-specific variable assignments/initializations/open ports */

        Data->HostData = (int32)host;
        return SBN_HOST;
    }
    /* CPU names do not match - this is peer data. */
    else {
        /* create, fill, and store a TEMPLATE-specific peer data structure */
        TEMPLATE_SBNPeerData_t *peer = malloc(sizeof(TEMPLATE_SBNPeerData_t));
        if (peer == NULL) {
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Cannot allocate peer interface data.\n");
			return SBN_ERROR;
		}

        /* TODO peer-specific variable assignments/initializations/open ports */

        Data->PeerData = (int32)peer;
        return SBN_PEER;
    }
}


/**
 * Sends a message to a peer over an TEMPLATE interface.
 *
 * @param MsgType      Type of Message
 * @param MsgSize      Size of Message
 * @param HostList     The array of SBN_InterfaceData structs that describes the host
 * @param SenderPtr    Sender information
 * @param IfData       The SBN_InterfaceData struct describing this peer
 * @param ProtoMsgBuf  Protocol message
 * @param DataMsgBuf   Data message
 *
 * @return number of bytes written on success
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnSendNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[], int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData, SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf) {
    int status, found = 0;
    TEMPLATE_SBNEntry_t *peer;
    TEMPLATE_SBNHostData_t *host;
    uint32 HostIdx;

    /* Check pointer arguments used for all cases for null */
    if (HostList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in SendNetMsg: HostList is NULL.\n");
        return SBN_ERROR;
    }
    if (IfData == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in SendNetMsg: IfData is NULL.\n");
        return SBN_ERROR;
    }

    /* Find the host that goes with this peer */
    peer = (TEMPLATE_SBNEntry_t *)(IfData->EntryData);
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        /* TODO uncomment, change "SBN_TEMPLATE" to "SBN_<YourProtocol>" and 
            make your own conditions for determining whether this host matches
            the peer */
        /*if(HostList[HostIdx]->ProtocolId == SBN_TEMPLATE) {
            found = 1;
            host = (TEMPLATE_SBNHostData_t *)HostList[HostIdx]->HostData;
            break;
        }*/
    }
    if(found != 1) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
            "No TEMPLATE Host Found!\n");
        return SBN_ERROR;
    }
    
    switch(MsgType) {
 
        /* Data messages */
        case SBN_APP_MSG: /* If my peer sent this message, don't send it back to them, avoids loops */
            if (SenderPtr == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "TEMPLATE: Error in SendNetMsg: SenderPtr is NULL.\n");
                return SBN_ERROR;
            }

            if (CFE_PSP_GetProcessorId() != SenderPtr->ProcessorId)
                break;

            /* Then no break, so fill in the sender application infomation */
            if (DataMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "TEMPLATE: Error in SendNetMsg: DataMsgBuf is NULL.\n");
                return SBN_ERROR;
            }

            strncpy((char *)&(DataMsgBuf->Hdr.MsgSender.AppName), &SenderPtr->AppName[0], OS_MAX_API_NAME);
            DataMsgBuf->Hdr.MsgSender.ProcessorId = SenderPtr->ProcessorId;
            /* Fall through to the next case */

        case SBN_SUBSCRIBE_MSG:
        case SBN_UN_SUBSCRIBE_MSG:
            if (DataMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "TEMPLATE: Error in SendNetMsg: DataMsgBuf is NULL.\n");
                return SBN_ERROR;
            }

            /* Initialize the SBN hdr of the outgoing network message */
            strncpy((char *)&DataMsgBuf->Hdr.SrcCpuName,CFE_CPU_NAME,SBN_MAX_PEERNAME_LENGTH);
            DataMsgBuf->Hdr.Type = MsgType;
            
            /* TODO module-specific send function. Send data to the host's data send
                port. Set "status" equal to the number of bytes written. */
 
            break;
      
        /* Protocol Messages */
        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:
            if (ProtoMsgBuf == NULL) {
                CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                                  "TEMPLATE: Error in SendNetMsg: ProtoMsgBuf is NULL.\n");
                  return SBN_ERROR;
            }

            ProtoMsgBuf->Hdr.Type = MsgType;
            strncpy(ProtoMsgBuf->Hdr.SrcCpuName, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH);

            /* TODO module-specific send function. Send data to the host's protocol 
                send port. Set "status" equal to the number of bytes written. */
            break;
      
        default:
            CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,"Unexpected msg type\n");
            /* send event to indicate unexpected msgtype */
            status = (-1);
            break;
    } /* end switch */
 
    return (status);
}/* end SBN_SendNetMsg */


/**
 * Checks for a single protocol message.
 *
 * @param Peer         Structure of interface data for a peer
 * @param ProtoMsgBuf  Pointer to the SBN's protocol message buffer
 *
 * @return SBN_TRUE for message available
 * @return SBN_NO_MSG for no messages or an error due to link being down or data copy error
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnCheckForNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf) {
    TEMPLATE_SBNPeerData_t *peer;
    int32 status;

    if (ProtoMsgBuf == NULL) {
        CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in CheckForNetProtoMsg: ProtoMsgBuf is NULL.\n");
        return SBN_ERROR;
    }
    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in CheckForNetProtoMsg: Peer is NULL.\n");
        ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
        return SBN_ERROR;
    }
    
    peer = (TEMPLATE_SBNPeerData_t *)Peer->PeerData;
    
    /* TODO module-specific read function. Read from the peer's protocol send
        port. Set status equal to the number of bytes read. */

    if (status > 0) /* Positive number indicates byte length of message */
        return SBN_TRUE; /* Message available and no errors */
    
    if ( status == SBN_ERROR ) {
        CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
                "TEMPLATE: Error in CheckForNetProtoMsg. Status=%d",
                status);
        ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
        return SBN_ERROR;
    }

    /* status = 0, so no messages and no errors */
    ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
    return SBN_NO_MSG; 
}/* end SBN_CheckForNetProtoMsg */


/**
 * Receives a message from a peer over the appropriate interface.
 *
 * @param Host       Structure of interface data for the host
 * @param DataMsgBuf Pointer to the SBN's protocol message buffer
 *
 * @return Bytes received on success, 
 * @return SBN_IF_EMPTY if no data, 
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnReceiveMsg(SBN_InterfaceData *Host, NetDataUnion *DataMsgBuf) {
    TEMPLATE_SBNHostData_t *host; 
    int32 status;

    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in RcvMsg: Host is NULL.\n");
        return SBN_ERROR;
    }
    if (DataMsgBuf == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in RcvMsg: DataMsgBuf is NULL.\n");
        return SBN_ERROR;
    }

    host = (TEMPLATE_SBNHostData_t *)Host->HostData;
    
    /* TODO module-specific read function. Read from the host's data receive
        port. Set status equal to the number of bytes read. */

    if ( status == 0 )
        return SBN_IF_EMPTY;
    else
        return status;
}


/**
 * Iterates through the list of all host interfaces to see if there is a 
 * match for the specified peer interface.
 *
 * @param SBN_InterfaceData *Peer    Peer to verify
 * @param SBN_InterfaceData *Hosts[] List of hosts to check against the peer
 * @param int32 NumHosts             Number of hosts in the SBN
 * 
 * @return SBN_VALID     if the required match exists, else
 * @return SBN_NOT_VALID if not 
 */
int32 TEMPLATE_SbnVerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts) {
    int32 HostIdx;
    TEMPLATE_SBNEntry_t *PeerEntry;

    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in VerifyPeerInterface: Peer is NULL.\n");
        return SBN_NOT_VALID;
    }
    if (HostList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in VerifyPeerInterface: Hosts is NULL.\n");
        return SBN_NOT_VALID;
    }

    PeerEntry = (TEMPLATE_SBNEntry_t *)Peer->EntryData;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        /* TODO uncomment, change "SBN_TEMPLATE" to "SBN_<YourProtocol>", and
            make your own conditions for verifying that this peer has a
            valid host interface to go with it */
        /*if(HostList[HostIdx]->ProtocolId == SBN_TEMPLATE) {
            return SBN_VALID;
        }*/
    }
    
    return SBN_NOT_VALID;
}


/**
 * Iterates through the list of all peer interfaces to see if there is a 
 * match for the specified host interface. 
 *
 * @param SBN_InterfaceData *Host    Host to verify
 * @param SBN_PeerData_t *Peers      List of peers to check against the host
 * @param int32 NumPeers             Number of peers in the SBN
 *
 * @return SBN_VALID     if the required match exists, else
 * @return SBN_NOT_VALID if not 
 */
int32 TEMPLATE_SbnVerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *PeerList, int32 NumPeers) {
    int PeerIdx;
    TEMPLATE_SBNEntry_t *HostEntry; 

    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in VerifyHostInterface: Host is NULL.\n");
        return SBN_NOT_VALID;
    }
    if (PeerList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in VerifyHostInterface: PeerList is NULL.\n");
        return SBN_NOT_VALID;
    }

    HostEntry = (TEMPLATE_SBNEntry_t *)Host->EntryData;
    
    /* Find the peer that goes with this host. */
    for(PeerIdx = 0; PeerIdx < NumPeers; PeerIdx++) {
        /* TODO uncomment, change "SBN_TEMPLATE" to "SBN_<YourProtocol>", and
            make your own conditions for verifying that this host has a
            valid peer interface to go with it */
        /*if(PeerList[PeerIdx].ProtocolId == SBN_TEMPLATE) {
            return SBN_VALID;
        }*/
    }

    return SBN_NOT_VALID;
}


/**
 * Reports the status of the module. The status packet is passed in
 * initialized (with message ID and size), the module fills it, and upon
 * return the SBN application sends the message over the software bus.
 *
 * @param StatusPkt Status packet to fill
 * @param Peer      Peer to report status
 * @param HostList  List of hosts that may match with peer
 * @param NumHosts  Number of hosts in the SBN
 *
 * @return SBN_OK on success
 * @return SBN_ERROR if the necessary data can't be found
 */
int32 TEMPLATE_SbnReportModuleStatus(SBN_ModuleStatusPacket_t *Packet, SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts) {
    /* Error check */
    if (Packet == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Could not report module status: StatusPkt is null.\n");
        return SBN_ERROR; 
    }

    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Could not report module status: Peer is null.\n");
        return SBN_ERROR; 
    }

    if (HostList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Could not report module status: HostList is null.\n");
        return SBN_ERROR; 
    }

    /* TODO fill in the packet with module-specific data and return SBN_OK */
    return SBN_NOT_IMPLEMENTED;
}


/**
 * Resets a specific peer.  
 *
 * @param Peer      Peer to reset
 * @param HostList  List of hosts that may match with peer
 * @param NumHosts  Number of hosts in the SBN
 *
 * @return SBN_OK when the peer is reset correcly
 * @return SBN_ERROR if the peer cannot be reset
 */
int32 TEMPLATE_SbnResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts) {
    /* Error check */
    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Could not reset peer: Peer is null.\n");
        return SBN_ERROR; 
    }

    if (HostList == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "Serial: Could not reset peer: HostList is null.\n");
        return SBN_ERROR; 
    }

    /* TODO module-specific resets. Return SBN_OK */
    return SBN_NOT_IMPLEMENTED;
}


/**
 * Deletes a host interface. Modules should use this function to clean up
 * all resources allocated to their host data, close ports, etc, such that
 * there are no memory leaks or stray processes running for the host. 
 *
 * @param SBN_InterfaceData *   The host to delete
 * @return SBN_OK on success
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnDeleteHostResources(SBN_InterfaceData *Host) {
    if (Host == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in DeleteHostResources: Host is NULL.\n");
        return SBN_ERROR; 
    }

    if (Host->EntryData != 0) {
        free((void*)Host->EntryData); 
    }

    if (Host->HostData != 0) {
        /* TODO any other frees, closing of ports, etc that need to be done
            before freeing HostData */
        free((void*)Host->HostData); 
    }

    return SBN_OK;  
}


/**
 * Deletes a peer interface. Modules should use this function to clean up
 * all resources allocated to their peer data, close ports, etc, such that
 * there are no memory leaks or stray processes running for the peer. 
 *
 * @param SBN_InterfaceData *   The peer to delete
 * @return SBN_OK on success
 * @return SBN_ERROR on error
 */
int32 TEMPLATE_SbnDeletePeerResources(SBN_InterfaceData *Peer) {
    if (Peer == NULL) {
        CFE_EVS_SendEvent(SBN_INIT_EID,CFE_EVS_ERROR,
                          "TEMPLATE: Error in DeletePeerResources: Peer is NULL.\n");
        return SBN_ERROR; 
    }

    if (Peer->EntryData != 0) {
        free((void*)Peer->EntryData); 
    }

    if (Peer->PeerData != 0) {
        /* TODO any other frees, closing of ports, etc that need to be done
            before freeing PeerData */
        free((void*)Peer->PeerData); 
    }

    return SBN_OK; 
}

