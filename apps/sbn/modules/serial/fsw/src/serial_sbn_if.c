/**
 * @file
 *
 * This file contains all functions that SBN calls, plus some helper functions.
 *
 * @author Jaclyn Beck, Jonathan Urriste, Chris KNight
 */
#include "serial_sbn_if_struct.h"
#include "serial_sbn_if.h"
#include "serial_platform_cfg.h"
#include "serial_queue.h"
#include "serial_events.h"
#include <string.h>
#include <arpa/inet.h> /* htonl/ntohl/etc. */

int Serial_SbnReceiveMsg(SBN_InterfaceData *Data, SBN_MsgType_t *MsgTypePtr,
    SBN_MsgSize_t *MsgSizePtr, SBN_CpuId_t *CpuIdPtr, void *Msg)
{
    Serial_SBNHostData_t *Host = NULL;

    if(Data == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_RECEIVE_EID, CFE_EVS_ERROR,
            "Serial: Error in SerialRcvMsg: Data is NULL.\n");
        return SBN_ERROR;
    }/* end if */
    if(Msg == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_RECEIVE_EID, CFE_EVS_ERROR,
            "Serial: Error in SerialRcvMsg: Msg is NULL.\n");
        return SBN_ERROR;
    }/* end if */

    Host = (Serial_SBNHostData_t *)Data->InterfacePvt;
    Serial_QueueGetMsg(Host->Queue, Host->SemId, MsgTypePtr, MsgSizePtr,
        CpuIdPtr, Msg);
    return SBN_OK;
}/* end Serial_SbnReceiveMsg */

#ifdef _osapi_confloader_

int SBN_LoadSerialEntry(const char **row, int fieldcount, void *entryptr)
{   
    Serial_SBNEntry_t *entry = (Serial_SBNEntry_t *)entryptr;

    if(fieldcount != SBN_SERIAL_ITEMS_PER_FILE_LINE)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Invalid SBN peer file line, exp %d items,found %d",
            SBN_SERIAL_ITEMS_PER_FILE_LINE, fieldcount);
        return SBN_ERROR;
    }/* end if */

    entry->PairNum = atoi(row[0]);
    strncpy(entry->DevNameHost, row[1], sizeof(entry->DevNameHost));
    entry->BaudRate = atoi(row[2]);

    return SBN_OK;
}/* end SBN_LoadSerialEntry */

#else /* ! _osapi_confloader_ */

int Serial_SbnParseInterfaceFileEntry(char *FileEntry, uint32 LineNum,
    void *entryptr)
{
    Serial_SBNEntry_t *entry = (Serial_SBNEntry_t *)entryptr;
    int ScanfStatus = 0;
    unsigned int BaudRate = 0, PairNum = 0;
    char DevNameHost[SBN_SERIAL_MAX_CHAR_NAME];

    /*
    ** Using sscanf to parse the string.
    ** Currently no error handling
    */
    ScanfStatus = sscanf(FileEntry, "%u %s %u", &PairNum, DevNameHost,
        &BaudRate);
    DevNameHost[SBN_SERIAL_MAX_CHAR_NAME-1] = 0;

    /* Check to see if the correct number of items were parsed */
    if(ScanfStatus != SBN_SERIAL_ITEMS_PER_FILE_LINE)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Invalid SBN peer file line, exp %d items,found %d",
            SBN_SERIAL_ITEMS_PER_FILE_LINE, ScanfStatus);
        return SBN_ERROR;
    }/* end if */

    entry->PairNum  = PairNum;
    entry->BaudRate = BaudRate;
    strncpy(entry->DevNameHost, DevNameHost,
        sizeof(entry->DevNameHost));

    return SBN_OK;
}/* end Serial_SbnParseInterfaceFileEntry */

#endif /* _osapi_confloader_ */

int Serial_SbnInitPeerInterface(SBN_InterfaceData *Data)
{
    Serial_SBNEntry_t *entry = NULL;
    char name[20];
    int32 Status = 0;

    if(Data == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Serial: Cannot initialize interface! Interface data is null.\n");
        return SBN_ERROR;
    }/* end if */

    entry = (Serial_SBNEntry_t *)(Data->InterfacePvt);

    /* CPU names match - this is host data. */
    if(strncmp(Data->Name, CFE_CPU_NAME, SBN_MAX_PEERNAME_LENGTH) == 0)
    {
        /* create, fill, and store Serial-specific host data structure */
        Serial_SBNHostData_t Host;

        CFE_PSP_MemSet(&Host, 0, sizeof(Host));

        /* open serial device and set options */
        Status = Serial_IoOpenPort(entry->DevNameHost, entry->BaudRate,
            &Host.Fd);
        if(Status != SBN_OK)
        {
            return Status;
        }/* end if */

        strncpy(Host.DevName, entry->DevNameHost, sizeof(Host.DevName));
        Host.PairNum  = entry->PairNum;
        Host.BaudRate = entry->BaudRate;

        /* Create data queue semaphor */
        snprintf(name, sizeof(name), "Sem%d", Host.PairNum);
        if(OS_BinSemCreate(&Host.SemId, name, OS_SEM_FULL, 0) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
                "Serial: Error creating data semaphore for host %d.\n",
                Host.PairNum);
        }/* end if */

        /* Create data queue */
        snprintf(name, sizeof(name), "SerialQueue%d", Host.PairNum);
        if(OS_QueueCreate(&Host.Queue, name, SBN_SERIAL_QUEUE_DEPTH,
            sizeof(uint32), 0) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
                "Serial: Error creating data queue for host %d.\n",
                Host.PairNum);
        }/* end if */

        memcpy(Data->InterfacePvt, &Host, sizeof(Host));

        return SBN_HOST;
    }
    else /* CPU names do not match - this is peer data. */
    {
        /* create, fill, and store peer data structure */
        Serial_SBNPeerData_t peer;

        CFE_PSP_MemSet(&peer, 0, sizeof(peer));

        peer.PairNum  = entry->PairNum;
        peer.BaudRate = entry->BaudRate;

        memcpy(Data->InterfacePvt, &peer, sizeof(peer));

        return SBN_PEER;
    }/* end if */
}/* end Serial_SbnInitPeerInterface */

static int IsHostPeerMatch(Serial_SBNEntry_t *Host, Serial_SBNEntry_t *Peer)
{
    if(Host == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Error in IsHostPeerMatch: Host is NULL.\n");
        return 0;
    }/* end if */

    if(Peer == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Error in IsHostPeerMatch: Peer is NULL.\n");
        return 0;
    }/* end if */

    return (Host->PairNum == Peer->PairNum)
        && (Host->BaudRate == Peer->BaudRate);
}/* end IsHostPeerMatch */

int Serial_SbnSendMsg(SBN_InterfaceData *HostList[], int NumHosts,
    SBN_InterfaceData *IfData, SBN_MsgType_t MsgType, SBN_MsgSize_t MsgSize,
    void *Msg)
{
    Serial_SBNEntry_t *Peer = NULL;
    Serial_SBNHostData_t *Host = NULL;
    uint32 HostIdx;
    SBN_CpuId_t CpuId = htonl(CFE_CPU_ID);

    /* Check pointer arguments used for all cases for null */
    if(HostList == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_SEND_EID, CFE_EVS_ERROR,
            "Serial: Error in SendSerialNetMsg: HostList is NULL.\n");
        return SBN_ERROR;
    }/* end if */

    if(IfData == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_SEND_EID, CFE_EVS_ERROR,
            "Serial: Error in SendSerialNetMsg: IfData is NULL.\n");
        return SBN_ERROR;
    }/* end if */

    if(Msg == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_SEND_EID, CFE_EVS_ERROR,
            "Serial: Error in SendSerialNetMsg: MsgBuf is NULL.\n");
        return SBN_ERROR;
    }/* end if */

    /* Find the host that goes with this peer. */
    Peer = (Serial_SBNEntry_t *)(IfData->InterfacePvt);
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++)
    {
        if(HostList[HostIdx]->ProtocolId == SBN_SERIAL)
        {
            Host = (Serial_SBNHostData_t *)(HostList[HostIdx]->InterfacePvt);
            if(IsHostPeerMatch((Serial_SBNEntry_t *)Host, Peer))
            {
                break;
            }
            else
            {
                Host = NULL;
            }/* end if */
        }/* end if */
    }/* end for */

    if(!Host)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_SEND_EID, CFE_EVS_ERROR,
            "Serial: No Serial Host Found!\n");
        return SBN_ERROR;
    }/* end if */

    MsgSize = htons(MsgSize);
    OS_write(Host->Fd, &MsgSize, sizeof(MsgSize));
    MsgSize = ntohs(MsgSize);

    OS_write(Host->Fd, &MsgType, sizeof(MsgType));

    OS_write(Host->Fd, &CpuId, sizeof(MsgType));

    OS_write(Host->Fd, &Msg, MsgSize);

    return SBN_OK;
}/* end SBN_SendMsg */


int Serial_SbnVerifyPeerInterface(SBN_InterfaceData *Peer,
    SBN_InterfaceData *HostList[], int NumHosts)
{
    int HostIdx = 0;
    Serial_SBNEntry_t *HostEntry = NULL;
    Serial_SBNEntry_t *PeerEntry = NULL;

    if(Peer == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Serial: Error in Serial_VerifyPeerInterface: Peer is NULL.\n");
        return FALSE;
    }/* end if */

    if(HostList == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Serial: Error in Serial_VerifyPeerInterface: Hosts is NULL.\n");
        return FALSE;
    }/* end if */

    PeerEntry = (Serial_SBNEntry_t *)Peer->InterfacePvt;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++)
    {
        if(HostList[HostIdx]->ProtocolId == SBN_SERIAL)
        {
            HostEntry = (Serial_SBNEntry_t *)HostList[HostIdx]->InterfacePvt;

            if(IsHostPeerMatch(HostEntry, PeerEntry))
            {
                return TRUE;
            }/* end if */
        }/* end if */
    }/* end for */

    return FALSE;
}/* end Serial_SbnVerifyPeerInterface */

int Serial_SbnVerifyHostInterface(SBN_InterfaceData *Data,
    SBN_PeerData_t *PeerList, int NumPeers)
{
    int PeerIdx;
    Serial_SBNEntry_t *PeerEntry = NULL;
    Serial_SBNEntry_t *HostEntry = NULL;

    if(Data == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Serial: Error in Serial_VerifyHostInterface: Data is NULL.\n");
        return FALSE;
    }/* end if */

    if(PeerList == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
            "Serial: Error in Serial_VerifyHostInterface: PeerList is NULL.\n");
        return FALSE;
    }/* end if */

    HostEntry = (Serial_SBNEntry_t *)Data->InterfacePvt;

    /* Find the peer that goes with this host. */
    for(PeerIdx = 0; PeerIdx < NumPeers; PeerIdx++)
    {
        if(PeerList[PeerIdx].ProtocolId == SBN_SERIAL)
        {
            PeerEntry = (Serial_SBNEntry_t *)(PeerList[PeerIdx].IfData)
                ->InterfacePvt;

            if(IsHostPeerMatch(HostEntry, PeerEntry))
            {
                /* Start serial read task. */
                return Serial_IoStartReadTask(
                    (Serial_SBNHostData_t *)Data->InterfacePvt);
            }/* end if */
        }/* end if */
    }/* end for */

    return FALSE;
}/* end Serial_SbnVerifyHostInterface */

static int GetHostPeerMatchData(SBN_InterfaceData *Data,
    SBN_InterfaceData *HostList[], Serial_SBNHostData_t **HostData,
    Serial_SBNPeerData_t **PeerData, int NumHosts)
{
    Serial_SBNEntry_t *HostEntry = NULL;
    int HostIdx = 0;

    if(Data->InterfacePvt == 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: InterfaceData entry has no interface private.\n");
        return SBN_ERROR;
    }/* end if */

    *PeerData = (Serial_SBNPeerData_t *)Data->InterfacePvt;

    /* Find the host that goes with this peer. */
    for(HostIdx = 0; HostIdx < NumHosts; HostIdx++)
    {
        if(HostList[HostIdx]->ProtocolId == SBN_SERIAL)
        {
            HostEntry = (Serial_SBNEntry_t *)HostList[HostIdx]->InterfacePvt;

            if(IsHostPeerMatch(HostEntry,
                (Serial_SBNEntry_t *)Data->InterfacePvt))
            {
                if(HostList[HostIdx]->InterfacePvt != 0)
                {
                    *HostData = (Serial_SBNHostData_t *)HostList[HostIdx]
                        ->InterfacePvt;
                    return SBN_OK;
                }/* end if */
            }/* end if */
        }/* end if */
    }/* end for */

    /* If the code reaches here, no match was found */
    CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_INFORMATION,
        "Serial: Could not find matching host for peer %d.\n",
        (*PeerData)->PairNum);

    return SBN_ERROR;
}/* end GetHostPeerMatchData */

int Serial_SbnReportModuleStatus(SBN_ModuleStatusPacket_t *StatusPkt,
    SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[], int NumHosts)
{
    int Status = 0;
    Serial_SBNModuleStatus_t *ModuleStatus = NULL;
    Serial_SBNPeerData_t *PeerData = NULL;
    Serial_SBNHostData_t *HostData = NULL;

    /* Error check */
    if(StatusPkt == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not report module status: StatusPkt is null.\n");
        return SBN_ERROR;
    }/* end if */

    if(Peer == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not report module status: Peer is null.\n");
        return SBN_ERROR;
    }/* end if */

    if(HostList == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not report module status: HostList is null.\n");
        return SBN_ERROR;
    }/* end if */

    /* Cast to the serial module's packet format to make it clearer
     * where things are going
     */
    ModuleStatus = (Serial_SBNModuleStatus_t*)&StatusPkt->ModuleStatus;

    /* Find the matching host for this peer */
    Status = GetHostPeerMatchData(Peer, HostList, &HostData, &PeerData, NumHosts);
    if(Status != SBN_OK)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not report module status for peer.\n");
        return Status;
    }/* end if */

    /* Copy data into the module status packet */
    CFE_PSP_MemCpy(&ModuleStatus->HostData, HostData,
        sizeof(Serial_SBNHostData_t));
    CFE_PSP_MemCpy(&ModuleStatus->PeerData, PeerData,
        sizeof(Serial_SBNPeerData_t));

    return SBN_OK;
}/* end Serial_SbnReportModuleStatus */

int Serial_SbnResetPeer(SBN_InterfaceData *Peer, SBN_InterfaceData *HostList[],
    int NumHosts)
{
    Serial_SBNPeerData_t *PeerData = NULL;
    Serial_SBNHostData_t *HostData = NULL;
    int Status = 0;

    /* Error check */
    if(Peer == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not reset peer: Peer is null.\n");
        return SBN_ERROR; 
    }/* end if */

    if(HostList == NULL)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not reset peer: HostList is null.\n");
        return SBN_ERROR; 
    }/* end if */

    /* Find the matching host for this peer */
    Status = GetHostPeerMatchData(Peer, HostList, &HostData, &PeerData,
        NumHosts);
    if(Status != SBN_OK)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not reset peer because no matching host "
            "was found.\n");
        return Status; 
    }/* end if */

    /* Stop the read task if it's running */
    if(HostData->TaskHandle > 0)
    {
        Status = CFE_ES_DeleteChildTask(HostData->TaskHandle);
        if(Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
                "Serial: Could not stop read task for host/peer %d.\n",
                HostData->PairNum);
            return SBN_ERROR;
        }/* end if */
    }/* end if */

    /* Close serial port */
    OS_close(HostData->Fd);

    /* Empty the queues. These are just while loops that loop until the method
     * returns OS_QUEUE_EMPTY
     */
    while(Serial_QueueRemoveNode(HostData->Queue) == OS_SUCCESS)
    {
        /* do nothing */
    }/* end while */

    /* Re-open serial port */
    Status = Serial_IoOpenPort(HostData->DevName, HostData->BaudRate,
        &HostData->Fd);
    if(Status != SBN_OK)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not re-open host %d's serial port.\n",
            HostData->PairNum);
        return Status;
    }/* end if */

    /* Re-start read task */
    Status = Serial_IoStartReadTask(HostData);
    if(!Status)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_EID, CFE_EVS_ERROR,
            "Serial: Could not restart the read task for host %d.\n",
            HostData->PairNum);
        return Status;
    }/* end if */

    return SBN_OK;
}/* end Serial_SbnResetPeer */
