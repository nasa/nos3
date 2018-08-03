#ifndef _sbn_serial_if_h_
#define _sbn_serial_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "cfe.h"
#include "serial_sbn_if_struct.h"
#include "serial_io.h"

/* SBN_InterfaceOperations functions */

#ifdef _osapi_confloader_
int SBN_LoadSerialEntry(const char **row, int fieldcount, void *entryptr);
#else /* ! _osapi_confloader_*/
int Serial_SbnParseInterfaceFileEntry(char *, uint32, void*);
#endif /* _osapi_confloader_ */
int Serial_SbnInitPeerInterface(SBN_InterfaceData* data);

int Serial_SbnSendMsg(SBN_InterfaceData *HostList[], int NumHosts,
    SBN_InterfaceData *IfData, SBN_MsgType_t MsgType, SBN_MsgSize_t MsgSize,
    void *Msg);

int Serial_SbnReceiveMsg(SBN_InterfaceData *Data, SBN_MsgType_t *MsgTypePtr,
    SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *MsgBuf);

int Serial_SbnVerifyPeerInterface(SBN_InterfaceData *Peer,
    SBN_InterfaceData *HostList[], int NumHosts);

int Serial_SbnVerifyHostInterface(SBN_InterfaceData *Host,
    SBN_PeerData_t *PeerList, int NumPeers);

int Serial_SbnReportModuleStatus(SBN_ModuleStatusPacket_t *StatusPkt,
    SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int NumHosts);

int Serial_SbnResetPeer(SBN_InterfaceData *Peer,
    SBN_InterfaceData *HostList[], int NumHosts);

/* Interface operations called by SBN */
SBN_InterfaceOperations SerialOps = {
#ifdef _osapi_confloader_
    SBN_LoadSerialEntry,
#else /* ! _osapi_confloader_ */
    Serial_SbnParseInterfaceFileEntry,
#endif /* _osapi_confloader_ */
    Serial_SbnInitPeerInterface,
    Serial_SbnSendMsg,
    Serial_SbnReceiveMsg,
    Serial_SbnVerifyPeerInterface,
    Serial_SbnVerifyHostInterface,
    Serial_SbnReportModuleStatus,
    Serial_SbnResetPeer
};

#endif /* _sbn_serial_if_h_ */
