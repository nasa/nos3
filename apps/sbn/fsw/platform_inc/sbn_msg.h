#ifndef _sbn_msg_h_
#define _sbn_msg_h_

#include "sbn_msgdefs.h"
#include "sbn_platform_cfg.h"
#include "sbn_constants.h"
#include "cfe.h"

/**
 * \brief No Arguments Command
 * For command details see #SBN_NOOP_CC, #SBN_RESET_CC
 * Also see #SBN_SEND_HK_CC
 */
typedef struct {
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
} SBN_NoArgsCmd_t;

/**
 * \brief Get Peer Status Command
 * For command details see #SBN_GET_PEER_STATUS_CC
 */
typedef struct {
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8 PeerIdx;
} SBN_GetPeerStatusCmd_t;

/**
 * \brief Reset Peer Command
 * For command details see #SBN_RESET_PEER_CC
 */
typedef struct {
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8 PeerIdx;
} SBN_ResetPeerCmd_t;

/**
 * \brief Housekeeping packet structure
 */
typedef struct {

    uint8  TlmHeader[CFE_SB_TLM_HDR_SIZE];

    /* SBN App Telemetry */
    uint16 CmdCount;
    uint16 CmdErrCount;

    /* SBN Module Stats */
    uint16 PeerAppMsgRecvCount[SBN_MAX_NETWORK_PEERS];
    uint16 PeerAppMsgSendCount[SBN_MAX_NETWORK_PEERS];
    uint16 PeerAppMsgRecvErrCount[SBN_MAX_NETWORK_PEERS];
    uint16 PeerAppMsgSendErrCount[SBN_MAX_NETWORK_PEERS];

    uint32 PeerSubsCount[SBN_MAX_NETWORK_PEERS];

} OS_PACK SBN_HkPacket_t;

/**
 * \brief Summary of peer information that would be useful on the ground
 */
typedef struct {
    char    Name[SBN_MAX_PEERNAME_LENGTH];
    uint32  ProcessorId;
    uint32  ProtocolId;
    uint32  SpaceCraftId;
    uint32  State;
    uint32  SubCnt;
} SBN_PeerSummary_t;

/**
 * \brief Peer List response packet structure
 */
typedef struct {
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    int32              NumPeers;
    SBN_PeerSummary_t  PeerList[SBN_MAX_NETWORK_PEERS];

} SBN_PeerListResponsePacket_t;

/**
 * \brief Module status response packet structure
 */
typedef struct {
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  ProtocolId;
    uint8   ModuleStatus[SBN_MOD_STATUS_MSG_SIZE];
} SBN_ModuleStatusPacket_t;

#endif /* _sbn_msg_h_ */
