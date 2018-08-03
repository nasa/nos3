#ifndef _sbn_interfaces_h_
#define _sbn_interfaces_h_

#include "cfe.h"
#include "sbn_constants.h"
#include "sbn_msg.h"
#include "sbn_lib_utils.h" /* in sbn_lib */

/*
** Other Structures
*/

/* used in local and peer subscription tables */
typedef struct {
  uint32            InUseCtr;
  CFE_SB_MsgId_t    MsgId;
  CFE_SB_Qos_t      Qos;
} SBN_Subs_t;

typedef uint16 SBN_MsgSize_t;
typedef uint8 SBN_MsgType_t;
typedef uint32 SBN_CpuId_t;

/* note that the packed size is likely smaller than an in-memory's struct
 * size, as the CPU will align objects.
 * SBN headers are MsgSize + MsgType + CpuId
 * SBN subscription messages are MsgId + Qos
 */
#define SBN_PACKED_HDR_SIZE (sizeof(SBN_MsgSize_t) + sizeof(SBN_MsgType_t) + sizeof(SBN_CpuId_t))

#define SBN_PACKED_SUB_SIZE (sizeof(CFE_SB_MsgId_t) + sizeof(CFE_SB_Qos_t))

typedef struct {
    char   Name[SBN_MAX_PEERNAME_LENGTH];
    int    ProtocolId;      /* from SbnPeerData.dat file */
    uint32 ProcessorId;     /* from SbnPeerData.dat file */
    uint32 SpaceCraftId;    /* from SbnPeerData.dat file */
    uint8  QoS;             /* from SbnPeerData.dat file */
    uint8  IsValid;         /* used by interfaces that require a match - 1 if match exists, 0 if not */
    uint8  InterfacePvt[128]; /* generic blob of bytes, interface-specific */
} SBN_InterfaceData;


typedef struct {
    uint8             InUse;
    CFE_SB_PipeId_t   Pipe;
    char              PipeName[OS_MAX_API_NAME];
    char              Name[SBN_MAX_PEERNAME_LENGTH];
    uint8             QoS;       /* Quality of Service */
    uint32            ProcessorId;
    int               ProtocolId;
    uint32            SpaceCraftId;
    uint32            State;
    OS_time_t         last_sent, last_received;
    uint32            SubCnt;
    SBN_Subs_t        Sub[SBN_MAX_SUBS_PER_PEER + 1]; /* trailing empty */
    SBN_InterfaceData *IfData;
} SBN_PeerData_t;

/**
 * This structure contains function pointers to interface-specific versions
 * of the key SBN functions.  Every interface module must have an equivalent
 * structure that points to the approprate functions for that interface.
 */
typedef struct {
    /**
     * Parses a peer data file line into an interface-specific entry structure.
     * Information that is common to all interface types is captured in the SBN
     * and may be captured here or not, at the discretion of the interface
     * developer.
     *
     * @param char*   Interface description line as read from file
     * @param uint32  The line number in the peer file
     * @param void*   The address of the entry struct to be loaded
     * @return SBN_OK if entry is parsed correctly, SBN_ERROR otherwise
     */
#ifdef _osapi_confloader_
    int (*LoadInterfaceEntry)(const char **, int, void *);
#else /* ! _osapi_confloader_ */
    int (*ParseInterfaceFileEntry)(char *, uint32, void *);
#endif /* _osapi_confloader_ */

    /**
     * Initializes the interface, classifies each interface based as a host
     * or a peer.  Hosts are those interfaces that are on the current CPU.
     *
     * @param SBN_InterfaceData* Struct pointer describing a single interface
     * @return SBN_HOST if the interface is a host, SBN_PEER if a peer,
     *         SBN_ERROR otherwise
     */
    int (*InitPeerInterface)(SBN_InterfaceData*);

    /**
     * Sends a message to a peer over the specified interface.
     * Both protocol and data message buffers are included in the parameters,
     * but only one is used at a time.  The data message buffer is used for
     * un/subscriptions and app messages.  The protocol message buffer is used
     * for announce and heartbeat messages/acks.
     *
     * @param SBN_InterfaceData *[] Array of all host interfaces in the SBN
     * @param int                   Number of host interfaces in the SBN
     * @param SBN_InterfaceData *   Interface data describing the
     *                              intended peer recipient
     * @param SBN_MsgType_t         The SBN message type
     * @param SBN_MsgSize_t         The size of the SBN message payload
     * @param void *                The SBN message payload
     * @return  Number of bytes sent on success, SBN_ERROR on error
     */
    int (*SendNetMsg)(SBN_InterfaceData *[], int, SBN_InterfaceData *,
        SBN_MsgType_t, SBN_MsgSize_t, void *);


    /**
     * Receives a data message from the specified interface.
     *
     * @param SBN_InterfaceData *   Host interface from which to receive a
     *                              message
     * @param SBN_MsgType_t *       SBN message type received.
     * @param SBN_MsgSize_t *       Payload size received.
     * @param SBN_CpuId_t *         CpuId of the sender.
     * @param void *                Message buf
     *                              (pass in a buffer of SBN_MAX_MSG_SIZE)
     * @return SBN_OK on success, SBN_ERROR on failure
     */
    int (*ReceiveMsg)(SBN_InterfaceData *, SBN_MsgType_t *,
        SBN_MsgSize_t *, SBN_CpuId_t *, void *);

    /**
     * Iterates through the list of all host interfaces to see if there is a
     * match for the specified peer interface.  This function must be present,
     * but can return SBN_VALID for interfaces that don't require a match.
     *
     * @param SBN_InterfaceData *    Peer to verify
     * @param SBN_InterfaceData *[]  List of hosts to check against the peer
     * @param int                    Number of hosts in the SBN
     * @return SBN_VALID if the required match exists, SBN_NOT_VALID if not
     */
    int (*VerifyPeerInterface)(SBN_InterfaceData *, SBN_InterfaceData *[],
        int);

    /**
     * Iterates through the list of all peer interfaces to see if there is a
     * match for the specified host interface.  This function must be present,
     * but can return SBN_VALID for interfaces that don't require a match.
     *
     * @param SBN_InterfaceData *    Host to verify
     * @param SBN_PeerData_t *       List of peers to check against the host
     * @param int                    Number of peers in the SBN
     * @return SBN_VALID if the required match exists, SBN_NOT_VALID if not
     */
    int (*VerifyHostInterface)(SBN_InterfaceData *, SBN_PeerData_t *, int);

    /**
     * Reports the status of the module.  The status can be in a module-specific
     * format but must be no larger than SBN_MOD_STATUS_MSG_SIZE bytes (as
     * defined in sbn_platform_cfg.h).  The status packet is passed in
     * initialized (with message ID and size), the module fills it, and upon
     * return the SBN application sends the message over the software bus.
     *
     * @param SBN_ModuleStatusPacket_t *  Status packet to fill
     * @param SBN_InterfaceData *         Peer to report status
     * @param SBN_InterfaceData *[]       List of hosts that may match with peer
     * @param int                         Number of hosts in the SBN
     * @return SBN_NOT_IMPLEMENTED if the module does not implement this
     *         function
     *         SBN_OK otherwise
     */
    int (*ReportModuleStatus)(SBN_ModuleStatusPacket_t *,
        SBN_InterfaceData *, SBN_InterfaceData *[], int);

    /**
     * Resets a specific peer.
     * This function must be present, but can simply return SBN_NOT_IMPLEMENTED
     * if it is not used by or not applicable to a module.
     *
     * @param SBN_InterfaceData *         Peer to report status
     * @param SBN_InterfaceData *[]       List of hosts that may match with peer
     * @param int                         Number of hosts in the SBN
     * @return  SBN_OK when the peer is reset correcly
     *          SBN_ERROR if the peer cannot be reset
     *          SBN_NOT_IMPLEMENTED if the module does not implement this
     *          function
     */
    int (*ResetPeer)(SBN_InterfaceData *, SBN_InterfaceData *[], int);

} SBN_InterfaceOperations;

#endif /* _sbn_interfaces_h_ */
