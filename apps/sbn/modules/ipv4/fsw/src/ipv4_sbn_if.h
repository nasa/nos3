#ifndef _sbn_ipv4_if_h_
#define _sbn_ipv4_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "cfe.h"

#ifdef _osapi_confloader_

int SBN_LoadIPv4Entry(const char **, int, void *);

#else /* ! _osapi_confloader_ */

int SBN_ParseIPv4FileEntry(char *, uint32, void *);

#endif /* _osapi_confloader_ */

int SBN_InitIPv4IF(SBN_InterfaceData* data);

int SBN_SendIPv4NetMsg(SBN_InterfaceData *HostList[], int NumHosts,
    SBN_InterfaceData *IfData, SBN_MsgType_t MsgType,
    SBN_MsgSize_t MsgSize, void *Msg);

int SBN_RcvIPv4Msg(SBN_InterfaceData *Data, SBN_MsgType_t *MsgTypePtr,
    SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *MsgBuf);

int IPv4_VerifyPeerInterface(SBN_InterfaceData *Peer,
    SBN_InterfaceData *HostList[], int NumHosts);

int IPv4_VerifyHostInterface(SBN_InterfaceData *Host,
    SBN_PeerData_t *PeerList, int NumPeers);

int IPv4_ReportModuleStatus(SBN_ModuleStatusPacket_t *Packet,
    SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[],
    int NumHosts);

int IPv4_ResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[],
    int NumHosts);

SBN_InterfaceOperations IPv4Ops =
{
#ifdef _osapi_confloader_
    SBN_LoadIPv4Entry,
#else /* ! _osapi_confloader_ */
    SBN_ParseIPv4FileEntry,
#endif /* _osapi_confloader_ */
    SBN_InitIPv4IF,
    SBN_SendIPv4NetMsg,
    SBN_RcvIPv4Msg,
    IPv4_VerifyPeerInterface,
    IPv4_VerifyHostInterface,
    IPv4_ReportModuleStatus,
    IPv4_ResetPeer
};

#endif /* _sbn_ipv4_if_h_ */
