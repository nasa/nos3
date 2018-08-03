#include "spw_sbn_if_struct.h"
#include "spw_sbn_if_struct.h"
#include "spw_sbn_if.h"
#include "cfe.h"
#include "cfe_sb_msg.h"
#include "cfe_sb.h"
#include <network_includes.h>
#include <string.h>
#include <errno.h>

/**
 * Displays peer data for the given Spacewire peer. Since the current Spacewire core is only point-to-point, function provides link status
 *
 * @param idx   Index of Spacewire peer
 */
void SBN_ShowIPv4PeerData(int idx) {
	/* TODO: print link status */
}


/**
 * Returns status for a given status name by reading the corresponding file in Sysfs
 * Note: the implementation of SpaceWire driver targeted by this SBN plugin has a single number in each status file (see fscanf statement)
 *
 * @param spwEntry Entry associated with this SpaceWire interface
 * @param statusName Status file name to be read
 * @return the value from the status name
 */
int SPW_GetStatus(SPW_SBNEntry_t spwEntry, char *statusName) {
 /* TODO: Use OSAL functions for file IO */
	FILE *fp;
	int status;
	char filePath[SBN_SPW_MAX_PATH_LENGTH];

	/* Prepares a string for opening a status file */
	sprintf(filePath, SBN_SPW_SYSFS_PATH,
		spwEntry->DevClass, spwEntry->DevInstance, statusName);
	filePath[SBN_SPW_MAX_PATH_LENGTH-1] = '\0';

	fp = fopen(filePath, "r");
	if (fp == NULL) {
		return SBN_ERROR;
	}

	/* Read from the status file, target implementation contains a single number */
	fscanf(fp, "%d", &status);
	fclose(fp);
	return status;
}

/**
 * Attempts to copy the data in the SpaceWire character device to the provided buffer
 *
 * @param spwEntry Entry associated with this SpaceWire interface
 * @param ProtoMsgBuf Pointer to the target data buffer
 * @param dataSize Size of the data buffer in bytes
 * @param error Pointer to store error status from the copy operation
 * @return dataRead Size in bytes of the copied data if successful, -1 if unsuccessful
 */

int SPW_GetData(SPW_SBNEntry_t spwEntry, void *dataBuffer, int dataSize, int *error) {
 /* TODO: this needs to 'packet-ize' the incoming data, it currently just takes everything in the incoming buffer */
 /* TODO: Use OSAL functions for file IO */
	FILE *fp;
	char filePath[SBN_SPW_MAX_PATH_LENGTH];
	int dataRead = -1;

	/* Prepares a string for opening device file */
	sprintf(filePath, SBN_SPW_DEV_PATH, spwEntry->DevInstance);
	filePath[SBN_SPW_MAX_PATH_LENGTH-1] = '\0';

	fp = fopen(filePath, "r");
	if (fp == NULL) {
		return -1;
	}

	/* Copy data from the device file to the buffer, get error */
	dataRead = fread(dataBuffer, 1, dataSize, fp);
	&error = ferror(fp);

	if (error == SPW_FREAD_NO_ERROR) {
		return dataRead;
	}
	else return -1;
}

/**
 * Attempts to copy the data from the provided buffer to the SpaceWire character device
 *
 * @param spwEntry Entry associated with this SpaceWire interface
 * @param ProtoMsgBuf Pointer to the target data buffer
 * @param dataSize Size of the data buffer in bytes
 * @param error Pointer to store error status from the copy operation
 * @return dataRead Size in bytes of the copied data if successful, -1 if unsuccessful
 */

int SPW_SendData(SPW_SBNEntry_t spwEntry, void *dataBuffer, int dataSize, int *error) {
 /* TODO: this needs to 'packet-ize' the incoming data, it currently just takes everything in the incoming buffer */
 /* TODO: Use OSAL functions for file IO */
	FILE *fp;
	char filePath[SBN_SPW_MAX_PATH_LENGTH];
	int dataRead = -1;

	/* Prepares a string for opening device file */
	sprintf(filePath, SBN_SPW_DEV_PATH, spwEntry->DevInstance);
	filePath[SBN_SPW_MAX_PATH_LENGTH-1] = '\0';

	fp = fopen(filePath, "w");
	if (fp == NULL) {
		return -1;
	}

	/* Copy data from the buffer to the device file, get error */
	dataRead = fwrite(dataBuffer, 1, dataSize, fp);
	&error = ferror(fp);

	if (error == SPW_FREAD_NO_ERROR) {
		return dataRead;
	}
	else return -1;
}

/**
 * Checks for a single protocol message.
 *
 * @param Peer         Structure of interface data for a peer
 * @param ProtoMsgBuf  Pointer to the SBN's protocol message buffer
 * @return 1 for message available and 0 for no messages or an error due to link being down or data copy error
 */
int32 SBN_CheckForSPWNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf) {
    SPW_SBNPeerData_t *peer = Peer->PeerData;
	int linkStatus, dataRead;
	int error;

	dataRead = SPW_GetData(peer->spwEntry, (*void)ProtoMsgBuf, sizeof(SBN_NetProtoMsg_t), &error);
    if (dataRead > 0 && error == SPW_FREAD_NO_ERROR) { /* Positive number indicates byte length of message */
        return SBN_TRUE; /* Message available and no errors */
	}

	linkStatus = SPW_GetStatus(peer->spwEntry, SBN_SPW_LINK_STATUS);
    if (!linkStatus | error != SPW_FREAD_NO_ERROR) {
		CFE_EVS_SendEvent(SBN_NET_RCV_PROTO_ERR_EID,CFE_EVS_ERROR,
						  "%s:Recv err in CheckForNetProtoMsgs linkStatus=%d ferror=%d"
						  CFE_CPU_NAME, linkStatus, error);
		ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
		return SBN_ERROR;
	}

    /* dataRead = 0, so no messages and no errors */
    ProtoMsgBuf->Hdr.Type = SBN_NO_MSG;
    return SBN_NO_MSG;
}/* end SBN_CheckForNetProtoMsg */

/**
 * Receives a message from a peer over the appropriate interface.
 *
 * @param Peer        Structure of interface data for a peer
 * @param DataMsgBuf  Pointer to the SBN's protocol message buffer
 * @return Bytes received on success, SBN_IF_EMPTY if empty, -1 on error
 */
int SBN_SPWRcvMsg(SBN_InterfaceData *Peer, NetDataUnion *DataMsgBuf) {
    SPW_SBNPeerData_t *peer = Peer->PeerData;
	int dataRead;
	int error;

	dataRead = SPW_GetData(peer->spwEntry, (*void)ProtoMsgBuf, SBN_MAX_MSG_SIZE, &error);

	/* TODO: determine whether to only check for empty buffer on return, or include other error checks */
    if (dataRead <= 0) { /* Positive number indicates byte length of message */
        return SBN_IF_EMPTY;
    else
        return dataRead;
}

/**
 * Parses the peer data file into SBN_FileEntry_t structures.
 * Parses information that is common to all interface types and
 * allows individual interface modules to parse out interface-
 * specfic information.
 *
 * @param FileEntry  Interface description line as read from file
 * @param LineNum    The line number in the peer file
 * @param EntryAddr  Address in which to return the filled entry struct
 * @return SBN_OK if entry is parsed correctly, SBN_ERROR otherwise
 */
int32 SBN_ParseSPWFileEntry(char *FileEntry, uint32 LineNum, void** EntryAddr) {
    int     ScanfStatus;
    char    ProtoDev[SBN_SPW_MAX_CHAR_NAME];
    char    DevInstance[SBN_SPW_MAX_CHAR_NAME];
    int     DataPort;

    /*
    ** Using sscanf to parse the string.
    ** Currently no error handling
    */
    ScanfStatus = sscanf(FileEntry, "%s %s", &ProtoDev, &DevInstance);
    ProtoDev[SBN_SPW_MAX_CHAR_NAME-1] = '\0';
    DevInstance[SBN_SPW_MAX_CHAR_NAME-1] = '\0';

    /*
    ** Check to see if the correct number of items were parsed
    */
    if (ScanfStatus != SBN_SPW_ITEMS_PER_FILE_LINE) {
        CFE_EVS_SendEvent(SBN_INV_LINE_EID,CFE_EVS_ERROR,
                "%s:Invalid SBN peer file line,exp %d items,found %d",
                CFE_CPU_NAME, SBN_SPW_ITEMS_PER_FILE_LINE, ScanfStatus);
        return SBN_ERROR;
    }

    SPW_SBNEntry_t *entry = malloc(sizeof(SPW_SBNEntry_t));
    *EntryAddr = entry;

    strncpy(entry->ProtoDev, &ProtoDev, SBN_SPW_MAX_CHAR_NAME);
    strncpy(entry->DevInstance, &DevInstance, SBN_SPW_MAX_CHAR_NAME);
    entry->ProtoDev = ProtoDev;

    return SBN_OK;
}

/**
 * Initializes an SPW host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_OK on success, error code otherwise
 */
int32 SBN_InitSPWIF(SBN_InterfaceData *Data) {
    int32 Stat;
    SPW_SBNEntry_t *entry = Data->EntryData;
    /* CPU names match - this is host data.
       Create msg interface when we find entry matching its own name
       because the self entry has port info needed to bind this interface. */
	if(strncmp(Data->Name, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH) == 0) {
        /* create, fill, and store SPW-specific host data structure */
        SPW_SBNHostData_t *host = malloc(sizeof(SPW_SBNHostData_t));
        host->spwEntry = entry;
		Data->HostData = host;
        return SBN_HOST;
    }
    /* CPU names do not match - this is peer data. */
    else {
        /* create, fill, and store an IPv4-specific host data structure */
        SPW_SBNPeerData_t *peer = malloc(sizeof(SPW_SBNPeerData_t));
        peer->spwEntry = spwEntry;
        Data->PeerData = peer;
        return SBN_PEER;
    }
}

/**
 * Sends a message to a peer over a SPW interface.
 *
 * @param MsgType      Type of Message
 * @param MsgSize      Size of Message
 * @param HostList     The array of SBN_InterfaceData structs that describes the host
 * @param SenderPtr    Sender information
 * @param IfData       The SBN_InterfaceData struct describing this peer
 * @param ProtoMsgBuf  Protocol message
 * @param DataMsgBuf   Data message
 */
int32 SBN_SendSPWNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[], int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData, SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf) {
    int    status, found = 0;
    SPW_SBNPeerData_t *peer;
    SPW_SBNHostData_t *host;
    uint32 HostIdx;

    peer = IfData->PeerData;

    switch(MsgType) {
        case SBN_APP_MSG: /* If my peer sent this message, don't send it back to them, avoids loops */
            if (CFE_PSP_GetProcessorId() != SenderPtr->ProcessorId)
                break;
            /* Then no break, so fill in the sender application information */
            strncpy((char *)&(DataMsgBuf->Hdr.MsgSender.AppName), &SenderPtr->AppName[0], OS_MAX_API_NAME);
            DataMsgBuf->Hdr.MsgSender.ProcessorId = SenderPtr->ProcessorId;
        case SBN_SUBSCRIBE_MSG:
        case SBN_UN_SUBSCRIBE_MSG:


            /* Initialize the SBN hdr of the outgoing network message */
            strncpy((char *)&DataMsgBuf->Hdr.SrcCpuName,CFE_CPU_NAME,SBN_MAX_PEERNAME_LENGTH);

            DataMsgBuf->Hdr.Type = MsgType;
            status = SPW_SendData(peer->spwEntry, (*void)ProtoMsgBuf, MsgSize, &error);

            break;

        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:

            ProtoMsgBuf->Hdr.Type = MsgType;
            strncpy(ProtoMsgBuf->Hdr.SrcCpuName, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH);

            status = SPW_SendData(peer->spwEntry, (*void)ProtoMsgBuf, MsgSize, &error);
            break;

        default:
            OS_printf("Unexpected msg type\n");
            /* send event to indicate unexpected msgtype */
            status = (-1);
            break;
    } /* end switch */

    return (status);
}/* end SBN_SendNetMsg */

int32 SPW_VerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts) {
    int32 HostIdx;
    int32 found;

    /* Find the host that goes with this peer.  There should only be one
       ethernet host */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++) {
        if(HostList[HostIdx]->ProtocolId == SBN_SPW) {
            found = 1;
            break;
        }
    }
    if(found == 1) {
        return SBN_VALID;
    }
    else {
        return SBN_NOT_VALID;
    }
}

/**
 * An SPW host doesn't necessarily need a peer, so this always returns true.
 */
int32 SPW_VerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *PeerList, int32 NumPeers) {
    return SBN_VALID;
}
