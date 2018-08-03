#ifndef _sbn_TEMPLATE_if_h_
#define _sbn_TEMPLATE_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "cfe.h"

#define TEMPLATE_ITEMS_PER_FILE_LINE 4 /* TODO this number should match how many items are in the module's line in SbnPeerData */

int32 TEMPLATE_SbnParseInterfaceFileEntry(char *FileEntry, uint32 LineNum, int *EntryAddr);
int32 TEMPLATE_SbnInitPeerInterface(SBN_InterfaceData* data);
int32 TEMPLATE_SbnSendNetMsg(uint32 MsgType, uint32 MsgSize, SBN_InterfaceData *HostList[], int32 NumHosts, CFE_SB_SenderId_t *SenderPtr, SBN_InterfaceData *IfData, SBN_NetProtoMsg_t *ProtoMsgBuf, NetDataUnion *DataMsgBuf);
int32 TEMPLATE_SbnCheckForNetProtoMsg(SBN_InterfaceData *Peer, SBN_NetProtoMsg_t *ProtoMsgBuf);
int32 TEMPLATE_SbnReceiveMsg(SBN_InterfaceData *Host, NetDataUnion *DataMsgBuf);
int32 TEMPLATE_SbnVerifyPeerInterface(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts);
int32 TEMPLATE_SbnVerifyHostInterface(SBN_InterfaceData *Host, SBN_PeerData_t *PeerList, int32 NumPeers);
int32 TEMPLATE_SbnReportModuleStatus(SBN_ModuleStatusPacket_t *Packet, SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts);
int32 TEMPLATE_SbnResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int32 NumHosts);
int32 TEMPLATE_SbnDeleteHostResources(SBN_InterfaceData *Host); 
int32 TEMPLATE_SbnDeletePeerResources(SBN_InterfaceData *Peer); 

SBN_InterfaceOperations TEMPLATEOps = {
    TEMPLATE_SbnParseInterfaceFileEntry,
    TEMPLATE_SbnInitPeerInterface,
    TEMPLATE_SbnSendNetMsg,
    TEMPLATE_SbnCheckForNetProtoMsg,
    TEMPLATE_SbnReceiveMsg,
    TEMPLATE_SbnVerifyPeerInterface,
    TEMPLATE_SbnVerifyHostInterface,
    TEMPLATE_SbnReportModuleStatus,
    TEMPLATE_SbnResetPeer,
    TEMPLATE_SbnDeleteHostResources,
    TEMPLATE_SbnDeletePeerResources
};

#endif /* _sbn_TEMPLATE_if_h_ */
