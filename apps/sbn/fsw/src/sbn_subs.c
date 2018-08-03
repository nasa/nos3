/******************************************************************************
 ** \file sbn_subs.c
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
 **            E. Timmons/GSFC Code587
 */

#include <string.h>
#include "sbn_subs.h"
#include "sbn_main_events.h"
#include <arpa/inet.h>

void SBN_SendSubsRequests(void)
{
    CFE_SB_CmdHdr_t     SBCmdMsg;

    DEBUG_START();

    /* Turn on SB subscription reporting */
    CFE_SB_InitMsg(&SBCmdMsg.Pri, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t),
        TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &SBCmdMsg,
        CFE_SB_ENABLE_SUB_REPORTING_CC);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &SBCmdMsg);

    /* Request a list of previous subscriptions from SB */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &SBCmdMsg, CFE_SB_SEND_PREV_SUBS_CC);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &SBCmdMsg);
}/* end SBN_SendSubsRequests */

static void PackSub(void *SubMsg, CFE_SB_MsgId_t MsgId, CFE_SB_Qos_t Qos)
{
    SBN_EndianMemCpy(SubMsg, &MsgId, sizeof(MsgId));
    CFE_PSP_MemCpy(SubMsg + sizeof(MsgId), &Qos, sizeof(Qos));

}

static void UnPackSub(void *SubMsg, CFE_SB_MsgId_t *MsgIdPtr,
    CFE_SB_Qos_t *QosPtr)
{
    SBN_EndianMemCpy(MsgIdPtr, SubMsg, sizeof(*MsgIdPtr));
    CFE_PSP_MemCpy(QosPtr, SubMsg + sizeof(*MsgIdPtr), sizeof(*QosPtr));
}

static void SendLocalSubToPeer(int SubFlag, CFE_SB_MsgId_t MsgId,
    CFE_SB_Qos_t Qos, int PeerIdx)
{
    uint8 SubMsg[SBN_PACKED_SUB_SIZE];
    CFE_PSP_MemSet(SubMsg, 0, sizeof(SubMsg));
    PackSub(SubMsg, MsgId, Qos);
    SBN_SendNetMsg(SubFlag, sizeof(SubMsg), SubMsg, PeerIdx);
}/* end SendLocalSubToPeer */

void SBN_SendLocalSubsToPeer(int PeerIdx)
{
    int i = 0;

    DEBUG_START();

    for(i = 0; i < SBN.LocalSubCnt; i++)
    {
        SendLocalSubToPeer(SBN_SUBSCRIBE_MSG, SBN.LocalSubs[i].MsgId, SBN.LocalSubs[i].Qos, PeerIdx);
    }/* end for */
}/* end SBN_SendLocalSubsToPeer */

static int IsMsgIdSub(int *IdxPtr, CFE_SB_MsgId_t MsgId)
{
    int     i = 0;

    DEBUG_START();

    for(i = 0; i < SBN.LocalSubCnt; i++)
    {
        if(SBN.LocalSubs[i].MsgId == MsgId)
        {
            if (IdxPtr)
            {
                *IdxPtr = i;
            }/* end if */

            return TRUE;
        }/* end if */
    }/* end for */

    return FALSE;
}/* end IsMsgIdSub */

static int IsPeerSubMsgId(int *SubIdxPtr, CFE_SB_MsgId_t MsgId,
    int PeerIdx)
{
    int     i = 0;

    DEBUG_START();

    for(i = 0; i < SBN.Peer[PeerIdx].SubCnt; i++)
    {
        if(SBN.Peer[PeerIdx].Sub[i].MsgId == MsgId)
        {
            *SubIdxPtr = i;
            return TRUE;
        }/* end if */
    }/* end for */

    return FALSE;

}/* end IsPeerSubMsgId */

static void ProcessLocalSub(CFE_SB_MsgId_t MsgId, CFE_SB_Qos_t Qos)
{
    int SubIdx = 0, PeerIdx = 0;

    DEBUG_START();

    /* don't subscribe to event messages */
    if(MsgId == CFE_EVS_EVENT_MSG_MID) return;

    if(SBN.LocalSubCnt >= SBN_MAX_SUBS_PER_PEER)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                "%s:Local Subscription Ignored for MsgId 0x%04X,max(%d)met",
                CFE_CPU_NAME, ntohs(MsgId), SBN_MAX_SUBS_PER_PEER);
        return;
    }/* end if */

    /* if there is already an entry for this msg id,just incr InUseCtr */
    if(IsMsgIdSub(&SubIdx, MsgId))
    {
        SBN.LocalSubs[SubIdx].InUseCtr++;
        /* does not send to peers, as they already know */
        return;
    }/* end if */

    /* log new entry into LocalSubs array */
    SBN.LocalSubs[SBN.LocalSubCnt].InUseCtr = 1;
    SBN.LocalSubs[SBN.LocalSubCnt].MsgId = MsgId;
    SBN.LocalSubs[SBN.LocalSubCnt].Qos = Qos;
    SBN.LocalSubCnt++;

    for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
    {
        if(!SBN.Peer[PeerIdx].InUse
            || SBN.Peer[PeerIdx].State != SBN_HEARTBEATING)
        {
            continue;
        }/* end if */
        SendLocalSubToPeer(SBN_SUBSCRIBE_MSG, MsgId, Qos, PeerIdx);
    }/* end for */
}/* end ProcessLocalSub */

static void ProcessLocalUnsub(CFE_SB_MsgId_t MsgId)
{
    int SubIdx = 0, PeerIdx = 0, i = 0;

    DEBUG_START();

    /* find idx of matching subscription */
    if(!IsMsgIdSub(&SubIdx, MsgId))
    {
        return;
    }/* end if */

    SBN.LocalSubs[SubIdx].InUseCtr--;

    /* do not modify the array and tell peers
    ** until the # of local subscriptions = 0
    */
    if(SBN.LocalSubs[SubIdx].InUseCtr > 0)
    {
        return;
    }/* end if */

    /* remove sub from array for and
    ** shift all subscriptions in higher elements to fill the gap
    ** note that the LocalSubs[] array has one extra element to allow for an
    ** unsub from a full table.
    */
    for(i = SubIdx; i < SBN.LocalSubCnt; i++)
    {
        CFE_PSP_MemCpy(&SBN.LocalSubs[i], &SBN.LocalSubs[i + 1],
            sizeof(SBN_Subs_t));
    }/* end for */

    SBN.LocalSubCnt--;

    /* send unsubscription to all peers if peer state is heartbeating and */
    /* only if no more local subs (InUseCtr = 0)  */
    for(PeerIdx = 0; PeerIdx < SBN_MAX_NETWORK_PEERS; PeerIdx++)
    {
        if(!SBN.Peer[PeerIdx].InUse
            || SBN.Peer[PeerIdx].State != SBN_HEARTBEATING)
        {
            continue;
        }/* end if */
        SendLocalSubToPeer(SBN_UN_SUBSCRIBE_MSG, SBN.LocalSubs[PeerIdx].MsgId, SBN.LocalSubs[PeerIdx].Qos, PeerIdx);
    }/* end for */
}/* end ProcessLocalUnsub */

int32 SBN_CheckSubscriptionPipe(void)
{
    CFE_SB_MsgPtr_t SBMsgPtr;
    CFE_SB_SubRprtMsg_t *SubRprtMsgPtr;

    /* DEBUG_START(); chatty */

    while(CFE_SB_RcvMsg(&SBMsgPtr, SBN.SubPipe, CFE_SB_POLL) == CFE_SUCCESS)
    {
        SubRprtMsgPtr = (CFE_SB_SubRprtMsg_t *)SBMsgPtr;

        switch(CFE_SB_GetMsgId(SBMsgPtr))
        {
            case CFE_SB_ONESUB_TLM_MID:
#ifdef SBN_PAYLOAD
                switch(SubRprtMsgPtr->Payload.SubType)
                {
                    case CFE_SB_SUBSCRIPTION:
                        ProcessLocalSub(SubRprtMsgPtr->Payload.MsgId,
                            SubRprtMsgPtr->Payload.Qos);
                        break;
                    case CFE_SB_UNSUBSCRIPTION:
                        ProcessLocalUnsub(SubRprtMsgPtr->Payload.MsgId);
                        break;
                    default:
                        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                            "%s:Unexpected SubType %d in "
                            "SBN_CheckSubscriptionPipe",
                            CFE_CPU_NAME, SubRprtMsgPtr->Payload.SubType);
                }/* end switch */

                return TRUE;
#else /* !SBN_PAYLOAD */
                switch(SubRprtMsgPtr->SubType)
                {
                    case CFE_SB_SUBSCRIPTION:
                        ProcessLocalSub(SubRprtMsgPtr->MsgId,
                            SubRprtMsgPtr->Qos);
                        break;
                    case CFE_SB_UNSUBSCRIPTION:
                        ProcessLocalUnsub(SubRprtMsgPtr->MsgId);
                        break;
                    default:
                        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                            "%s:Unexpected SubType %d in "
                            "SBN_CheckSubscriptionPipe",
                            CFE_CPU_NAME, SubRprtMsgPtr->SubType);
                }/* end switch */


                return TRUE;
#endif /* SBN_PAYLOAD */

            case CFE_SB_ALLSUBS_TLM_MID:
                SBN_ProcessAllSubscriptions((CFE_SB_PrevSubMsg_t *) SBMsgPtr);
                return TRUE;

            default:
                CFE_EVS_SendEvent(SBN_MSG_EID, CFE_EVS_ERROR,
                        "%s:Unexpected MsgId 0x%04X on SBN.SubPipe",
                        CFE_CPU_NAME, ntohs(CFE_SB_GetMsgId(SBMsgPtr)));

        }/* end switch */
    }/* end while */

    return FALSE;
}/* end SBN_CheckSubscriptionPipe */

void SBN_ProcessSubFromPeer(int PeerIdx, void *Msg)
{
    int FirstOpenSlot = 0, idx = 0;
    CFE_SB_MsgId_t MsgId;
    CFE_SB_Qos_t Qos;
    uint32 Status = CFE_SUCCESS;

    DEBUG_START();

    if(PeerIdx == SBN_ERROR)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "%s:Cannot process Subscription,PeerIdx(%d)OutOfRange",
             CFE_CPU_NAME, PeerIdx);
        return;
    }/* end if */

    if(SBN.Peer[PeerIdx].SubCnt >= SBN_MAX_SUBS_PER_PEER)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "%s:Cannot process subscription from %s,max(%d)met.",
            CFE_CPU_NAME, SBN.Peer[PeerIdx].Name, SBN_MAX_SUBS_PER_PEER);
        return;
    }/* end if */

    UnPackSub(Msg, &MsgId, &Qos);

    /* if msg id already in the list, ignore */
    if(IsPeerSubMsgId(&idx, MsgId, PeerIdx))
    {
        return;
    }/* end if */

    /* SubscribeLocal suppresses the subscription report */
    Status = CFE_SB_SubscribeLocal(MsgId, SBN.Peer[PeerIdx].Pipe,
            SBN_DEFAULT_MSG_LIM);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "Cannot subscribe to peer msgid 0x%04X %d",
            htons(MsgId), (int)Status);
        return;
    }/* end if */
    FirstOpenSlot = SBN.Peer[PeerIdx].SubCnt;

    /* log the subscription in the peer table */
    SBN.Peer[PeerIdx].Sub[FirstOpenSlot].MsgId = MsgId;
    SBN.Peer[PeerIdx].Sub[FirstOpenSlot].Qos = Qos;

    SBN.Peer[PeerIdx].SubCnt++;
}/* SBN_ProcessSubFromPeer */

void SBN_ProcessUnsubFromPeer(int PeerIdx, void *Msg)
{
    int i = 0, idx = 0;
    CFE_SB_MsgId_t MsgId = 0x0000;
    CFE_SB_Qos_t Qos;
    int32 Status = CFE_SUCCESS;

    DEBUG_START();

    UnPackSub(Msg, &MsgId, &Qos);

    if(!IsPeerSubMsgId(&idx, MsgId, PeerIdx))
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_INFORMATION,
            "%s:Cannot process unsubscription from %s,msg 0x%04X not found",
            CFE_CPU_NAME, SBN.Peer[PeerIdx].Name, htons(MsgId));
        return;
    }/* end if */

    /* remove sub from array for that peer and
    ** shift all subscriptions in higher elements to fill the gap
    ** note that the Sub[] array has one extra element to allow for an
    ** unsub from a full table.
    */
    for(i = idx; i < SBN.Peer[PeerIdx].SubCnt; i++)
    {
        CFE_PSP_MemCpy(&SBN.Peer[PeerIdx].Sub[i], &SBN.Peer[PeerIdx].Sub[i + 1],
            sizeof(SBN_Subs_t));
    }/* end for */

    /* decrement sub cnt */
    SBN.Peer[PeerIdx].SubCnt--;

    /* unsubscribe to the msg id on the peer pipe */
    Status = CFE_SB_UnsubscribeLocal(MsgId, SBN.Peer[PeerIdx].Pipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_INFORMATION,
            "Cannot process unsubscription from 0x%04X",
            htons(MsgId));
    }/* end if */
}/* SBN_ProcessUnsubFromPeer */

void SBN_ProcessAllSubscriptions(CFE_SB_PrevSubMsg_t *Ptr)
{
    int                     i = 0;

    DEBUG_START();

#ifdef SBN_PAYLOAD
    if(Ptr->Payload.Entries > CFE_SB_SUB_ENTRIES_PER_PKT)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "%s:Entries value %d in SB PrevSubMsg exceeds max %d, aborting",
            CFE_CPU_NAME, (int)Ptr->Payload.Entries,
            CFE_SB_SUB_ENTRIES_PER_PKT);
        return;
    }/* end if */

    for(i = 0; i < Ptr->Payload.Entries; i++)
    {
        ProcessLocalSub(Ptr->Payload.Entry[i].MsgId, Ptr->Payload.Entry[i].Qos);
    }/* end for */
#else /* !SBN_PAYLOAD */
    if(Ptr->Entries > CFE_SB_SUB_ENTRIES_PER_PKT)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "%s:Entries value %d in SB PrevSubMsg exceeds max %d, aborting",
            CFE_CPU_NAME, Ptr->Entries, CFE_SB_SUB_ENTRIES_PER_PKT);
        return;
    }/* end if */

    for(i = 0; i < Ptr->Entries; i++)
    {
        ProcessLocalSub(Ptr->Entry[i].MsgId, Ptr->Entry[i].Qos);
    }/* end for */
#endif /* SBN_PAYLOAD */
}/* end SBN_ProcessAllSubscriptions */

void SBN_RemoveAllSubsFromPeer(int PeerIdx)
{
    int     i = 0;
    uint32 Status = CFE_SUCCESS;

    DEBUG_START();

    if(PeerIdx == SBN_ERROR)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "%s:Cannot remove all subs from peer,PeerIdx(%d)OutOfRange",
            CFE_CPU_NAME, PeerIdx);
        return;
    }/* end if */

    for(i = 0; i < SBN.Peer[PeerIdx].SubCnt; i++)
    {
        Status = CFE_SB_UnsubscribeLocal(SBN.Peer[PeerIdx].Sub[i].MsgId,
            SBN.Peer[PeerIdx].Pipe);
        if(Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                "Unable to unsub from MID 0x%04X",
                    htons(SBN.Peer[PeerIdx].Sub[i].MsgId));
        }/* end if */
    }/* end for */

    CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_INFORMATION,
        "%s:UnSubscribed %d MsgIds from %s", CFE_CPU_NAME,
        (int)SBN.Peer[PeerIdx].SubCnt, SBN.Peer[PeerIdx].Name);

    SBN.Peer[PeerIdx].SubCnt = 0;
}/* end SBN_RemoveAllSubsFromPeer */
