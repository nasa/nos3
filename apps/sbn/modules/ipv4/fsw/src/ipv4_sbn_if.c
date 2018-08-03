#include "ipv4_sbn_if_struct.h"
#include "ipv4_sbn_if.h"
#include "ipv4_events.h"
#include "cfe.h"
#include "sbn_lib_utils.h"
#include <network_includes.h>
#include <string.h>
#include <strings.h> /* for bzero */
#include <errno.h>

static void ClearSocket(int SockId)
{
    struct sockaddr_in  s_addr;
    socklen_t           addr_len = 0;
    int                 i = 0;
    int                 status = 0;
    char                DiscardData[SBN_MAX_MSG_SIZE];

    addr_len = sizeof(s_addr);
    bzero((char *) &s_addr, sizeof(s_addr));

    CFE_EVS_SendEvent(SBN_IPV4_SOCK_EID, CFE_EVS_DEBUG,
        "Clearing socket %d", SockId);

    /* change to while loop */
    for(i = 0; i <= 50; i++)
    {
        status = recvfrom(SockId, DiscardData, sizeof(DiscardData),
            MSG_DONTWAIT,(struct sockaddr *) &s_addr, &addr_len);
        if((status < 0) && (errno == EWOULDBLOCK)) // TODO: add EAGAIN?
            break; /* no (more) messages */
    }/* end for */
}/* end ClearSocket */

static int CreateSocket(char *Addr, int Port)
{
    static struct sockaddr_in   my_addr;
    int                         SockId = 0;

    CFE_EVS_SendEvent(SBN_IPV4_SOCK_EID, CFE_EVS_DEBUG,
        "Creating socket for %s:%d", Addr, Port);

    if((SockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        CFE_EVS_SendEvent(SBN_IPV4_SOCK_EID, CFE_EVS_ERROR,
            "%s:socket call failed,line %d,rtn val %d,errno=%d",
            CFE_CPU_NAME, __LINE__, SockId, errno);
        return SockId;
    }/* end if */

    my_addr.sin_addr.s_addr = inet_addr(Addr);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(Port);

    if(bind(SockId, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0 )
    {
        CFE_EVS_SendEvent(SBN_IPV4_SOCK_EID, CFE_EVS_ERROR,
            "%s:bind call failed,line %d,rtn val %d,errno=%d",
            CFE_CPU_NAME, __LINE__, SockId, errno);
        return SockId;
    }/* end if */

    #ifdef _HAVE_FCNTL_
        /*
        ** Set the socket to non-blocking
        ** This is not available to vxWorks, so it has to be
        ** Conditionally compiled in
        */
        fcntl(SockId, F_SETFL, O_NONBLOCK);
    #endif

    ClearSocket(SockId);
    return SockId;
}/* end CreateSocket */

#ifdef _osapi_confloader_

int SBN_LoadIPv4Entry(const char **row, int fieldcount, void *entryptr)
{
    IPv4_SBNEntry_t *entry = (IPv4_SBNEntry_t *)entryptr;
    if(fieldcount < IPV4_ITEMS_PER_FILE_LINE)
    {
        return SBN_ERROR;
    }/* end if */

    strncpy(entry->Addr, row[0], 16);
    entry->Port = atoi(row[1]);

    return SBN_OK;
}/* end SBN_LoadIPv4Entry */

#else /* ! _osapi_confloader_ */

int SBN_ParseIPv4FileEntry(char *FileEntry, uint32 LineNum, void *entryptr)
{
    IPv4_SBNEntry_t *entry = (IPv4_SBNEntry_t *)entryptr;

    int     ScanfStatus = 0;
    char    Addr[16];
    int     Port = 0;

    /*
     * Using sscanf to parse the string.
     * Currently no error handling
     */
    ScanfStatus = sscanf(FileEntry, "%s %d", Addr, &Port);

    /*
     * Check to see if the correct number of items were parsed
     */
    if(ScanfStatus != IPV4_ITEMS_PER_FILE_LINE)
    {
        CFE_EVS_SendEvent(SBN_IPV4_CONFIG_EID,CFE_EVS_ERROR,
                "%s:Invalid SBN peer file line,exp %d items,found %d",
                CFE_CPU_NAME, IPV4_ITEMS_PER_FILE_LINE, ScanfStatus);
        return SBN_ERROR;
    }/* end if */

    strncpy(entry->Addr, Addr, 16);
    entry->Port = Port;

    return SBN_OK;
}/* end SBN_ParseIPv4FileEntry */

#endif /* _osapi_confloader_ */

/**
 * Initializes an IPv4 host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_OK on success, error code otherwise
 */
int SBN_InitIPv4IF(SBN_InterfaceData *Data)
{
    IPv4_SBNEntry_t *entry = (IPv4_SBNEntry_t *)Data->InterfacePvt;

    if(Data->ProcessorId == CFE_CPU_ID)
    {
        /* CPU names match - this is host data.
         * Create msg interface when we find entry matching its own name
         * because the self entry has port info needed to bind this interface.
         */
        /* create, fill, and store an IPv4-specific host data structure */
        IPv4_SBNHostData_t Host;

        CFE_PSP_MemSet(&Host, 0, sizeof(Host));

        strncpy(Host.Addr, entry->Addr, sizeof(entry->Addr));
        Host.Port = entry->Port;
        Host.SockId = CreateSocket(Host.Addr, Host.Port);
        if(Host.SockId == SBN_ERROR){
            return SBN_ERROR;
        }/* end if */

        memcpy(Data->InterfacePvt, &Host, sizeof(Host));

        return SBN_HOST;
    }
    else
    {
        /* CPU names do not match - this is peer data. */
        /* create, fill, and store an IPv4-specific host data structure */
        IPv4_SBNPeerData_t Peer;

        CFE_PSP_MemSet(&Peer, 0, sizeof(Peer));

        strncpy(Peer.Addr, entry->Addr, sizeof(entry->Addr));
        Peer.Port = entry->Port;

        memcpy(Data->InterfacePvt, &Peer, sizeof(Peer));

        return SBN_PEER;
    }/* end if */
}/* end SBN_InitIPv4IF */

int SBN_SendIPv4NetMsg(SBN_InterfaceData *HostList[], int NumHosts,
    SBN_InterfaceData *IfData, SBN_MsgType_t MsgType,
    SBN_MsgSize_t MsgSize, void *Msg)
{
    static struct sockaddr_in s_addr;
    IPv4_SBNPeerData_t *Peer = NULL;
    IPv4_SBNHostData_t *Host = NULL;
    uint32 HostIdx = 0;
    uint8 Buf[SBN_MAX_MSG_SIZE]; /* it all needs to go into one packet */
    void *BufOffset = Buf;
    SBN_CpuId_t CpuId = CFE_CPU_ID;

    /* Find the host that goes with this peer.  There should only be one
       ethernet host */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++)
    {
        if(HostList[HostIdx]->ProtocolId == SBN_IPv4)
        {
            Host = (IPv4_SBNHostData_t *)HostList[HostIdx]->InterfacePvt;
        }/* end if */
    }/* end for */

    if(!Host)
    {
        OS_printf("No IPv4 Host Found!\n");
        return SBN_ERROR;
    }/* end if */

    Peer = (IPv4_SBNPeerData_t *)IfData->InterfacePvt;
    bzero((char *) &s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Peer->Addr);
    s_addr.sin_port = htons(Peer->Port);

    SBN_EndianMemCpy(BufOffset, &MsgSize, sizeof(MsgSize));
    BufOffset += sizeof(MsgSize);
    CFE_PSP_MemCpy(BufOffset, &MsgType, sizeof(MsgType));
    BufOffset += sizeof(MsgType);
    SBN_EndianMemCpy(BufOffset, &CpuId, sizeof(CpuId));
    BufOffset += sizeof(CpuId);

    if(Msg && MsgSize)
    {
        CFE_PSP_MemCpy(BufOffset, Msg, MsgSize);
    }/* end if */

    sendto(Host->SockId, Buf, MsgSize + SBN_PACKED_HDR_SIZE, 0,
        (struct sockaddr *) &s_addr, sizeof(s_addr));

    return SBN_OK;
}/* end SBN_SendIPv4NetMsg */

int SBN_RcvIPv4Msg(SBN_InterfaceData *Data, SBN_MsgType_t *MsgTypePtr,
    SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *MsgBuf)
{
    ssize_t             Received = 0, TotalReceived = 0;
    struct sockaddr_in  s_addr;
    socklen_t           addr_len = sizeof(s_addr);
    IPv4_SBNHostData_t  *Host = (IPv4_SBNHostData_t *)Data->InterfacePvt;
    void *MsgBufPtr = MsgBuf;

    bzero((char *) &s_addr, sizeof(s_addr));
    *MsgSizePtr = 0;

    while(1)
    {
        addr_len = sizeof(s_addr);
        Received = recvfrom(Host->SockId, (char *)MsgBuf + TotalReceived,
            SBN_MAX_MSG_SIZE - TotalReceived,
            MSG_DONTWAIT, (struct sockaddr *) &s_addr, &addr_len);

        if(Received == 0) return SBN_IF_EMPTY;

        if((Received < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN)))
            return SBN_ERROR;

        TotalReceived += Received;
        if(TotalReceived < sizeof(SBN_MsgSize_t))
        {
            continue;
        }/* end if */

        if(MsgBufPtr == MsgBuf) /* we haven't read the size yet */
        {
            SBN_EndianMemCpy(MsgSizePtr, MsgBufPtr, sizeof(*MsgSizePtr));
            MsgBufPtr += sizeof(*MsgSizePtr);
        }/* end if */

        if(TotalReceived >= SBN_PACKED_HDR_SIZE + *MsgSizePtr)
        {
            break;
        }/* end if */
    }/* end while */

    CFE_PSP_MemCpy(MsgTypePtr, MsgBufPtr, sizeof(*MsgTypePtr));
    MsgBufPtr += sizeof(*MsgTypePtr);

    SBN_EndianMemCpy(CpuIdPtr, MsgBufPtr, sizeof(*CpuIdPtr));
    MsgBufPtr += sizeof(*CpuIdPtr);

    if(TotalReceived > SBN_PACKED_HDR_SIZE)
    {
        memmove(MsgBuf, MsgBufPtr, TotalReceived - SBN_PACKED_HDR_SIZE);
    }/* end if */

    return SBN_OK;
}/* end SBN_IPv4RcvMsg */

int IPv4_VerifyPeerInterface(SBN_InterfaceData *Peer,
        SBN_InterfaceData *HostList[], int NumHosts)
{
    int     HostIdx = 0;

    /* Find the host that goes with this peer.  There should only be one
       ethernet host */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++)
    {
        if(HostList[HostIdx]->ProtocolId == SBN_IPv4)
        {
            return TRUE;
        }/* end if */
    }/* end for */

    return FALSE;
}/* end IPv4_VerifyPeerInterface */

int IPv4_VerifyHostInterface(SBN_InterfaceData *Host,
        SBN_PeerData_t *PeerList, int NumPeers)
{
    return TRUE;
}/* end IPv4_VerifyHostInterface */

int IPv4_ReportModuleStatus(SBN_ModuleStatusPacket_t *Packet,
        SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int NumHosts)
{
    return SBN_NOT_IMPLEMENTED;
}/* end IPv4_ReportModuleStatus */

int IPv4_ResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[],
        int NumHosts)
{
    return SBN_NOT_IMPLEMENTED;
}/* end IPv4_ResetPeer */
