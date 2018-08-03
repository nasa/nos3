#ifndef _sbn_platform_cfg_h_
#define _sbn_platform_cfg_h_

#define SBN_SUB_PIPE_DEPTH            256
#define SBN_MAX_ONESUB_PKTS_ON_PIPE   256
#define SBN_MAX_ALLSUBS_PKTS_ON_PIPE  64

#define SBN_VOL_PEER_FILENAME         "/ram/SbnPeerData.dat"
#define SBN_NONVOL_PEER_FILENAME      "/cf/SbnPeerData.dat"
#define SBN_PEER_FILE_LINE_SIZE       128
#define SBN_MAX_NETWORK_PEERS         4
#define SBN_NETWORK_MSG_MARGIN        SBN_MAX_NETWORK_PEERS * 2  /* Double it to handle clock skews where a peer */
                                                             /* can send two messages in my single cycle */
#define SBN_VOL_MODULE_FILENAME       "/ram/SbnModuleData.dat"
#define SBN_NONVOL_MODULE_FILENAME    "/cf/SbnModuleData.dat"
#define SBN_MODULE_FILE_LINE_SIZE     128
#define SBN_MAX_INTERFACE_TYPES       6

#define SBN_SCH_PIPE_DEPTH            10

#define SBN_MOD_STATUS_MSG_SIZE       128 /* bytes */

#define SBN_MAX_MSG_RETRANSMISSIONS   3

#endif /* _sbn_platform_cfg_h_ */
