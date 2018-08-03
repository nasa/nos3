#ifndef _sbn_spw_if_h_
#define _sbn_spw_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "cfe.h"

#define SBN_OK 0
#define SBN_ERROR 1
#define SBN_SPW_ITEMS_PER_FILE_LINE 2
#define SBN_SPW_MAX_CHAR_NAME 32

/* for string allocation, unless you want it dynamic... */
#define SBN_SPW_MAX_PATH_LENGTH 4096
/* sprintf format for Sysfs path to spacewire status files */
#define SBN_SPW_SYSFS_PATH "/sys/class/%s/%s/device/"
/*sprintf format for dev path to spacewire device file */
#define SBN_SPW_DEV_PATH "/dev/%s"

/* SpaceWire status file names in Sysfs */
#define SBN_SPW_LINK_STATUS "link_status"
#define SBN_SPW_RX_DATA_AVAILABLE "rxqueue_len"

#define SPW_FREAD_NO_ERROR 0

/* Interface-specific functions */
void  SBN_ShowSPWPeerData(int i);

/* SBN_InterfaceOperations functions */
int32 SBN_CheckForSPWNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf);
int   SBN_SPWRcvMsg(SBN_InterfaceData *Host, NetDataUnion *DataMsgBuf);
int32 SBN_ParseSPWFileEntry(char *FileEntry, uint32 LineNum, void** EntryAddr);
int32 SBN_InitSPWIF(SBN_InterfaceData* data);
int32 SBN_SendSPWNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[], int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData, SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf);
int32 SPW_VerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts);
int32 SPW_VerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *PeerList, int32 NumPeers);

SBN_InterfaceOperations SPWOps = {
    SBN_ParseSPWFileEntry,
    SBN_InitSPWIF,
    SBN_SendSPWNetMsg,
    SBN_CheckForSPWNetProtoMsg,
    SBN_SPWRcvMsg,
    SPW_VerifyPeerInterface,
    SPW_VerifyHostInterface
};


#endif /* _sbn_spw_if_h_ */
