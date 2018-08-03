/******************************************************************************
 ** \file sbn_app.c
 **
 **      Copyright (c) 2004-2006, United States government as represented by the
 **      administrator of the National Aeronautics Space Administration.
 **      All rights reserved. This software(cFE) was created at NASA's Goddard
 **      Space Flight Center pursuant to government contracts.
 **
 **      This software may be used only pursuant to a United States government
 **      sponsored project and the United States government may not be charged
 **      for use thereof.
 **
 ** Purpose:
 **      This file contains source code for the Software Bus Network Application.
 **
 ** Authors:   J. Wilmot/GSFC Code582
 **            R. McGraw/SSI
 **             C. Knight/ARC Code TI
 ******************************************************************************/

/*
 ** Include Files
 */
#include <fcntl.h>
#include <string.h>

#include "cfe.h"
#include "cfe_sb_msg.h"
#include "cfe_sb.h"
#include "sbn_app.h"
#include "sbn_netif.h"
#include "sbn_msgids.h"
#include "sbn_loader.h"
#include "sbn_cmds.h"
#include "sbn_subs.h"
#include "sbn_main_events.h"
#include "sbn_perfids.h"
#include "cfe_sb_events.h" /* For event message IDs */
#include "cfe_sb_priv.h" /* For CFE_SB_SendMsgFull */
#include "cfe_es.h" /* PerfLog */

/*
 **   Task Globals
 */

static void CheckPeerPipes(void)
{
    int PeerIdx = 0, ReceivedFlag = 0, iter = 0;
    CFE_SB_MsgPtr_t SBMsgPtr = 0;
    CFE_SB_SenderId_t * lastSenderPtr = NULL;

    /* DEBUG_START(); chatty */

    /* Process one message per peer, then start again until no peers
     * have pending messages. At max only process SBN_MAX_MSG_PER_WAKEUP
     * per peer per wakeup otherwise I will starve other processing.
     */
    for(iter = 0; iter < SBN_MAX_MSG_PER_WAKEUP; iter++)
    {
        ReceivedFlag = 0;

        for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
        {
            /* if peer data is not in use, go to next peer */
            if(SBN.Peer[PeerIdx].State != SBN_HEARTBEATING)
            {
                continue;
            }

            if(CFE_SB_RcvMsg(&SBMsgPtr, SBN.Peer[PeerIdx].Pipe, CFE_SB_POLL)
                != CFE_SUCCESS)
            {
                continue;
            }/* end if */

            ReceivedFlag = 1;

            /* don't re-send what SBN sent */
            CFE_SB_GetLastSenderId(&lastSenderPtr, SBN.Peer[PeerIdx].Pipe);

            if(strncmp(SBN.App_FullName, lastSenderPtr->AppName,
                OS_MAX_API_NAME))
            {
                SBN_SendNetMsg(SBN_APP_MSG,
                    CFE_SB_GetTotalMsgLength(SBMsgPtr), SBMsgPtr, PeerIdx);
            }/* end if */
        }/* end for */

        if(!ReceivedFlag)
        {
            break;
        }/* end if */
    } /* end for */
}/* end CheckPeerPipes */

static int32 CheckCmdPipe(void)
{
    CFE_SB_MsgPtr_t     SBMsgPtr = 0;
    int                 Status = 0;

    /* DEBUG_START(); */

    /* Command and HK requests pipe */
    while(Status == CFE_SUCCESS)
    {
        Status = CFE_SB_RcvMsg(&SBMsgPtr, SBN.CmdPipe, CFE_SB_POLL);

        if(Status == CFE_SUCCESS) SBN_AppPipe(SBMsgPtr);
    }/* end while */

    if(Status == CFE_SB_NO_MESSAGE) Status = CFE_SUCCESS;

    return Status;
}/* end CheckCmdPipe */

static void RunProtocol(void)
{
    int         PeerIdx = 0;
    OS_time_t   current_time;

    /* DEBUG_START(); chatty */

    CFE_ES_PerfLogEntry(SBN_PERF_SEND_ID);

    for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
    {
        /* if peer data is not in use, go to next peer */
        if(!SBN.Peer[PeerIdx].InUse) continue;

        OS_GetLocalTime(&current_time);

        if(SBN.Peer[PeerIdx].State == SBN_ANNOUNCING)
        {
            if(current_time.seconds - SBN.Peer[PeerIdx].last_sent.seconds
                    > SBN_ANNOUNCE_TIMEOUT)
            {
                SBN_SendNetMsg(SBN_ANNOUNCE_MSG, 0, NULL, PeerIdx);
            }/* end if */
            return;
        }/* end if */
        if(current_time.seconds - SBN.Peer[PeerIdx].last_received.seconds
                > SBN_HEARTBEAT_TIMEOUT)
        {
            /* lost connection, reset */
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
                "peer %d lost connection, resetting\n", PeerIdx);
            SBN_RemoveAllSubsFromPeer(PeerIdx);
            SBN.Peer[PeerIdx].State = SBN_ANNOUNCING;

        }/* end if */
        if(current_time.seconds - SBN.Peer[PeerIdx].last_sent.seconds
                > SBN_HEARTBEAT_SENDTIME)
        {
            SBN_SendNetMsg(SBN_HEARTBEAT_MSG, 0, NULL, PeerIdx);
	}/* end if */
    }/* end for */

    CFE_ES_PerfLogExit(SBN_PERF_SEND_ID);
}/* end RunProtocol */

static int32 WaitForWakeup(int32 iTimeOut)
{
    int32           Status = CFE_SUCCESS;
    CFE_SB_MsgPtr_t SBMsgPtr = 0;

    /* DEBUG_START(); chatty */

    /* Wait for WakeUp messages from scheduler */
    Status = CFE_SB_RcvMsg(&SBMsgPtr, SBN.SchPipeId, iTimeOut);

    /* success or timeout is ok to proceed through main loop */
    if(Status == CFE_SB_TIME_OUT || (Status == CFE_SUCCESS && CFE_SB_GetMsgId(SBMsgPtr) == SBN_WAKEUP_MID))
    {
        /* For sbn, we still want to perform cyclic processing
        ** if the WaitForWakeup time out
        ** cyclic processing at timeout rate
        */
        CFE_ES_PerfLogEntry(SBN_PERF_RECV_ID);

        RunProtocol();
        SBN_CheckForNetAppMsgs();
        SBN_CheckSubscriptionPipe();
        CheckPeerPipes();
        CheckCmdPipe();

        CFE_ES_PerfLogExit(SBN_PERF_RECV_ID);
    }/* end if */

    return Status;
}/* end WaitForWakeup */

/**
 * Waits for either a response to the "get subscriptions" message from SB, OR
 * an event message that says SB has finished initializing. The latter message
 * means that SB was not started at the time SBN sent the "get subscriptions"
 * message, so that message will need to be sent again.
 * @return TRUE if message received was a initialization message and
 *      requests need to be sent again, or
 * @return FALSE if message received was a response
 */
static int WaitForSBStartup(void)
{
    CFE_EVS_Packet_t *EvsPacket = NULL;
    CFE_SB_MsgPtr_t SBMsgPtr = 0;
    uint8 counter = 0;
    CFE_SB_PipeId_t EventPipe = 0;
    uint32 Status = CFE_SUCCESS;

    DEBUG_START();

    /* Create event message pipe */
    Status = CFE_SB_CreatePipe(&EventPipe, 100, "SBNEventPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to create EventPipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    /* Subscribe to event messages temporarily to be notified when SB is done
     * initializing
     */
    Status = CFE_SB_Subscribe(CFE_EVS_EVENT_MSG_MID, EventPipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to sub to EventPipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    while(1)
    {
        /* Check for subscription message from SB */
        if(SBN_CheckSubscriptionPipe())
        {
            /* SBN does not need to re-send request messages to SB */
            break;
        }
        else if(counter % 100 == 0)
        {
            /* Send subscription request messages again. This may cause the SB
             * to respond to duplicate requests but that should be okay
             */
            SBN_SendSubsRequests();
        }/* end if */

        /* Check for event message from SB */
        if(CFE_SB_RcvMsg(&SBMsgPtr, EventPipe, 100) == CFE_SUCCESS)
        {
            if(CFE_SB_GetMsgId(SBMsgPtr) == CFE_EVS_EVENT_MSG_MID)
            {
                EvsPacket = (CFE_EVS_Packet_t *)SBMsgPtr;

                /* If it's an event message from SB, make sure it's the init
                 * message
                 */
#ifdef SBN_PAYLOAD
                if(strcmp(EvsPacket->Payload.PacketID.AppName, "CFE_SB") == 0
                    && EvsPacket->Payload.PacketID.EventID == CFE_SB_INIT_EID)
                {
                    break;
                }/* end if */
#else /* !SBN_PAYLOAD */
                if(strcmp(EvsPacket->PacketID.AppName, "CFE_SB") == 0
                    && EvsPacket->PacketID.EventID == CFE_SB_INIT_EID)
                {
                    break;
                }/* end if */
#endif /* SBN_PAYLOAD */
            }/* end if */
        }/* end if */

        counter++;
    }/* end while */

    /* Unsubscribe from event messages */
    CFE_SB_Unsubscribe(CFE_EVS_EVENT_MSG_MID, EventPipe);

    CFE_SB_DeletePipe(EventPipe);

    /* SBN needs to re-send request messages */
    return TRUE;
}/* end WaitForSBStartup */

/** \brief Initializes SBN */
static int Init(void)
{
    int Status = CFE_SUCCESS;
    int     PeerIdx = 0, j = 0;
    uint32  TskId = 0;

    Status = CFE_ES_RegisterApp();
    if(Status != CFE_SUCCESS) return Status;

    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    if(Status != CFE_SUCCESS) return Status;

    DEBUG_START();

    CFE_PSP_MemSet(&SBN, 0, sizeof(SBN));

    /* load the App_FullName so I can ignore messages I send out to SB */
    TskId = OS_TaskGetId();
    CFE_SB_GetAppTskName(TskId,SBN.App_FullName);

    if(SBN_ReadModuleFile() == SBN_ERROR)
    {
        CFE_EVS_SendEvent(SBN_FILE_EID, CFE_EVS_ERROR,
            "SBN APP Will Terminate, Module File Not Found or Data Invalid!");
        return SBN_ERROR;
    }/* end if */

    if(SBN_GetPeerFileData() == SBN_ERROR)
    {
        CFE_EVS_SendEvent(SBN_FILE_EID, CFE_EVS_ERROR,
            "SBN APP Will Terminate, Peer File Not Found or Data Invalid!");
        return SBN_ERROR;
    }/* end if */

    for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
    {
        SBN.Peer[PeerIdx].InUse = FALSE;
        SBN.Peer[PeerIdx].SubCnt = 0;
        for(j = 0; j < SBN_MAX_SUBS_PER_PEER; j++)
        {
            SBN.Peer[PeerIdx].Sub[j].InUseCtr = 0;
        }/* end for */

        SBN.LocalSubCnt = 0;
    }/* end for */

    SBN_InitPeerInterface();
    SBN_VerifyPeerInterfaces();
    SBN_VerifyHostInterfaces();

    CFE_ES_GetAppID(&SBN.AppId);

    /* Init schedule pipe */
    SBN.usSchPipeDepth = SBN_SCH_PIPE_DEPTH;
    strncpy(SBN.cSchPipeName, "SBN_SCH_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    Status = CFE_SB_CreatePipe(&SBN.SchPipeId, SBN.usSchPipeDepth,
        SBN.cSchPipeName);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to create SCH pipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    Status = CFE_SB_Subscribe(SBN_WAKEUP_MID, SBN.SchPipeId);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to subscribe to SCH wakeup (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    /* Create pipe for subscribes and unsubscribes from SB */
    Status = CFE_SB_CreatePipe(&SBN.SubPipe, SBN_SUB_PIPE_DEPTH,
        "SBNSubPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to create sub pipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    Status = CFE_SB_SubscribeLocal(CFE_SB_ALLSUBS_TLM_MID,
        SBN.SubPipe, SBN_MAX_ALLSUBS_PKTS_ON_PIPE);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to subscribe to allsubs (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    Status = CFE_SB_SubscribeLocal(CFE_SB_ONESUB_TLM_MID,
        SBN.SubPipe, SBN_MAX_ONESUB_PKTS_ON_PIPE);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to subscribe to sub (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    /* Create pipe for HK requests and gnd commands */
    Status = CFE_SB_CreatePipe(&SBN.CmdPipe,20,"SBNCmdPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to create cmdpipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    Status = CFE_SB_Subscribe(SBN_CMD_MID,SBN.CmdPipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to subscribe to cmd (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    Status = CFE_SB_Subscribe(SBN_SEND_HK_MID,SBN.CmdPipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN APP Failed to subscribe to hk (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */


    CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
        "SBN APP Initialized V1.1, AppId=%d", (int)SBN.AppId);

    /* Initialize HK Message */
    CFE_SB_InitMsg(&SBN.HkPkt, SBN_HK_TLM_MID, sizeof(SBN_HkPacket_t), TRUE);
    SBN_InitializeCounters();

    /* Wait for event from SB saying it is initialized OR a response from SB
       to the above messages. TRUE means it needs to re-send subscription
       requests */
    if(WaitForSBStartup()) SBN_SendSubsRequests();

    return SBN_OK;
}/* end Init */

/** \brief SBN Main Routine */
void SBN_AppMain(void)
{
    int     Status = CFE_SUCCESS;
    uint32  RunStatus = CFE_ES_APP_RUN;

    Status = Init();
    if(Status != CFE_SUCCESS) RunStatus = CFE_ES_APP_ERROR;

    /* Loop Forever */
    while(CFE_ES_RunLoop(&RunStatus)) WaitForWakeup(SBN_MAIN_LOOP_DELAY);

    CFE_ES_ExitApp(RunStatus);
}/* end SBN_AppMain */

int SBN_CreatePipe4Peer(int PeerIdx)
{
    int32   Status = 0;
    char    PipeName[OS_MAX_API_NAME];

    DEBUG_START();

    /* create a pipe name string similar to SBN_CPU2_Pipe */
    sprintf(PipeName, "SBN_%s_Pipe", SBN.Peer[PeerIdx].Name);
    Status = CFE_SB_CreatePipe(&SBN.Peer[PeerIdx].Pipe, SBN_PEER_PIPE_DEPTH,
            PipeName);

    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "%s:failed to create pipe %s", CFE_CPU_NAME, PipeName);

        return Status;
    }/* end if */

    strncpy(SBN.Peer[PeerIdx].PipeName, PipeName, OS_MAX_API_NAME);

    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
        "%s: pipe created %s", CFE_CPU_NAME, PipeName);

    return SBN_OK;
}/* end SBN_CreatePipe4Peer */

void SBN_ProcessNetMsg(SBN_MsgType_t MsgType, SBN_CpuId_t CpuId,
    SBN_MsgSize_t MsgSize, void *Msg)
{
    int PeerIdx = 0, Status = 0;

    DEBUG_START();

    PeerIdx = SBN_GetPeerIndex(CpuId);

    if(PeerIdx == SBN_ERROR) return;

    if(SBN.Peer[PeerIdx].State == SBN_ANNOUNCING
        || MsgType == SBN_ANNOUNCE_MSG)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
            "Peer #%d alive, resetting", PeerIdx);
        SBN.Peer[PeerIdx].State = SBN_HEARTBEATING;
        SBN_SendLocalSubsToPeer(PeerIdx);
    }/* end if */

    switch(MsgType)
    {
        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:
            break;

        case SBN_APP_MSG:
            Status = CFE_SB_SendMsgFull(Msg,
                CFE_SB_DO_NOT_INCREMENT, CFE_SB_SEND_ONECOPY);

            if(Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(SBN_SB_EID, CFE_EVS_ERROR,
                    "%s:CFE_SB_SendMsg err %d. type 0x%x",
                    CFE_CPU_NAME, Status,
                    MsgType);
            }/* end if */
            break;

        case SBN_SUBSCRIBE_MSG:
        {
            SBN_ProcessSubFromPeer(PeerIdx, Msg);
            break;
        }

        case SBN_UN_SUBSCRIBE_MSG:
        {
            SBN_ProcessUnsubFromPeer(PeerIdx, Msg);
            break;
        }

        default:
            /* make sure of termination */
            CFE_EVS_SendEvent(SBN_MSG_EID, CFE_EVS_ERROR,
                "%s:Unknown Msg Type 0x%x", CFE_CPU_NAME,
                MsgType);
            break;
    }/* end switch */
}/* end SBN_ProcessNetMsg */

int SBN_GetPeerIndex(uint32 ProcessorId)
{
    int     PeerIdx = 0;

    /* DEBUG_START(); chatty */

    for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
    {
        if(!SBN.Peer[PeerIdx].InUse) continue;

        if(SBN.Peer[PeerIdx].ProcessorId == ProcessorId) return PeerIdx;
    }/* end for */

    return SBN_ERROR;
}/* end SBN_GetPeerIndex */
