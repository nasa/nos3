#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "sbn_msgids.h"
#include "sbn_main_events.h"
#include "sbn_cmds.h"
#include "sbn_app.h"
#include "cfe.h"

/************************************************************************/
/** \brief Housekeeping request
**
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter

**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
*************************************************************************/
void SBN_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Noop command
**  
**  \par Description
**       Processes a noop ground command.
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_RESET_CC
**
*************************************************************************/
void SBN_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset counters command
**
**  \par Description
**       Processes a reset counters command which will reset the
**       following SBN application counters to zero:
**         - Command counter
**         - Command error counter
**         - App messages sent counter for each peer
**         - App message send error counter for each peer
**         - App messages received counter for each peer
**         - App message receive error counter for each peer
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_RESET_CC
**
*************************************************************************/
void SBN_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Get Peer List Command
**
**  \par Description
**       Gets a list of all the peers recognized by the SBN.  The list
**       includes all available identifying information about the peer.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_GET_PEER_LIST_CC
**  \sa #SBN_PeerSummary_t
**
*************************************************************************/
void SBN_GetPeerList(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Get Peer Status Command
**
**  \par Description
**       Get status information on the specified peer.  The interface 
**       module fills up to a maximum number of bytes with status 
**       information in a module-defined format.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_GET_PEER_STATUS_CC
**
*************************************************************************/
void SBN_GetPeerStatus(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset Peer
**
**  \par Description
**       Reset a specified peer.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message
**
**  \sa #SBN_RESET_PEER_CC
**
*************************************************************************/
void SBN_ResetPeer(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Verify message length 
**
**  \par Description
**       Checks if the actual length of a software bus message matches
**       the expected length and sends an error event if a mismatch
**       occurs.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that 
**                                 references the software bus message
**
**  \param [in]   ExpectedLength   The expected length of the message
**                                 based upon the command code.
**
**  \returns
**  \retstmt Returns TRUE if the length is as expected      \endcode
**  \retstmt Returns FALSE if the length is not as expected \endcode
**  \endreturns
**
**  \sa #SBN_LEN_EID
**
*************************************************************************/
boolean SBN_VerifyMsgLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

/*******************************************************************/
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/*******************************************************************/
void SBN_AppPipe(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t  MsgId = 0;
    uint16          CommandCode = 0;

    DEBUG_START();

    MsgId = CFE_SB_GetMsgId(MessagePtr);
    switch(MsgId)
    {
        /* Housekeeping Telemetry Requests */
        case SBN_SEND_HK_MID:
            SBN_HousekeepingReq(MessagePtr);
            break;

        case SBN_CMD_MID:
            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch(CommandCode)
            {
                case SBN_NOOP_CC:
                    SBN_NoopCmd(MessagePtr);
                    break;
                case SBN_RESET_CC:
                    SBN_ResetCountersCmd(MessagePtr);
                    break;
                case SBN_GET_PEER_LIST_CC:
                    SBN_GetPeerList(MessagePtr);
                    break;
                case SBN_GET_PEER_STATUS_CC:
                    SBN_GetPeerStatus(MessagePtr);
                    break;
                case SBN_RESET_PEER_CC:
                    SBN_ResetPeer(MessagePtr);
                    break;
                default:
                    SBN.HkPkt.CmdErrCount++;
                    CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_ERROR,
                        "Invalid command code: ID = 0x%04X, CC = %d", 
                        MsgId, CommandCode);
                    break;
            }/* end switch */
            break;

        default:
            SBN.HkPkt.CmdErrCount++;
            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_ERROR,
                "Invalid command pipe message ID: 0x%04X",
                MsgId);
            break;
    }/* end switch */
}/* end SBN_AppPipe */

/*******************************************************************/
/*                                                                 */
/* Reset telemetry counters                                        */
/*                                                                 */
/*******************************************************************/
void SBN_InitializeCounters(void)
{
    int32   i = 0;

    DEBUG_START();

    SBN.HkPkt.CmdCount = 0;
    SBN.HkPkt.CmdErrCount = 0;

    for(i = 0; i < SBN_MAX_NETWORK_PEERS; i++)
    {
        SBN.HkPkt.PeerAppMsgRecvCount[i] = 0;
        SBN.HkPkt.PeerAppMsgSendCount[i] = 0;
        SBN.HkPkt.PeerAppMsgRecvErrCount[i] = 0;
        SBN.HkPkt.PeerAppMsgSendErrCount[i] = 0;
    }/* end for */
}/* end SBN_InitializeCounters */

/*******************************************************************/
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/*******************************************************************/
void SBN_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr)
{
    int32   i = 0;
    uint16  ExpectedLength = sizeof(SBN_NoArgsCmd_t);

    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /* 
        ** Update with the latest subscription counts 
        */
        for(i = 0; i < SBN_MAX_NETWORK_PEERS; i++)
            SBN.HkPkt.PeerSubsCount[i] = SBN.Peer[i].SubCnt;

        /* 
        ** Timestamp and send packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &SBN.HkPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &SBN.HkPkt);
    }/* end if */
}/* end SBN_HousekeepingReq */

/*******************************************************************/
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/*******************************************************************/
void SBN_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16  ExpectedLength = sizeof(SBN_NoArgsCmd_t);

    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        SBN.HkPkt.CmdCount++;

        CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_INFORMATION,
            "No-op command");
    }/* end if */
}/* end SBN_NoopCmd */

/*******************************************************************/
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/*******************************************************************/
void SBN_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16  ExpectedLength = sizeof(SBN_NoArgsCmd_t);

    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /*
        ** Don't increment counter because we're resetting anyway
        */
        SBN_InitializeCounters();

        CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_DEBUG,
            "Reset counters command");
    }/* end if */
}/* end SBN_ResetCountersCmd */

/*******************************************************************/
/*                                                                 */
/* Get list of peers                                               */
/*                                                                 */
/*******************************************************************/
void SBN_GetPeerList(CFE_SB_MsgPtr_t MessagePtr)
{
    SBN_PeerListResponsePacket_t    response;
    int32                           i = 0;
    uint16                          ExpectedLength = sizeof(SBN_NoArgsCmd_t);

    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        CFE_SB_InitMsg(&response, SBN_GET_PEER_LIST_RSP_MID,
            sizeof(SBN_PeerListResponsePacket_t), TRUE);
        response.NumPeers = SBN.NumPeers;

        for(i = 0; i < response.NumPeers; i++)
        {
            strncpy(response.PeerList[i].Name, SBN.Peer[i].Name,
                SBN_MAX_PEERNAME_LENGTH);
            response.PeerList[i].ProcessorId = SBN.Peer[i].ProcessorId;
            response.PeerList[i].ProtocolId = SBN.Peer[i].ProtocolId;
            response.PeerList[i].SpaceCraftId = SBN.Peer[i].SpaceCraftId;
            response.PeerList[i].State = SBN.Peer[i].State;
            response.PeerList[i].SubCnt = SBN.Peer[i].SubCnt;
        }/* end for */

        SBN.HkPkt.CmdCount++;
        /* 
        ** Timestamp and send packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &response);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &response);

        CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_DEBUG,
            "Peer list retrieved (%d peers)", (int)response.NumPeers);
    }/* end if */
}/* end SBN_GetPeerList */

/*******************************************************************/
/*                                                                 */
/* Get status of a specified peer                                  */
/*                                                                 */
/*******************************************************************/
void SBN_GetPeerStatus(CFE_SB_MsgPtr_t MessagePtr)
{
    SBN_ModuleStatusPacket_t    response;
    int32                       PeerIdx = 0, Status = 0;
    SBN_PeerData_t              Peer;

    uint16                    ExpectedLength = sizeof(SBN_GetPeerStatusCmd_t);
    
    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        SBN_GetPeerStatusCmd_t *Command = (SBN_GetPeerStatusCmd_t*)MessagePtr;
        PeerIdx = Command->PeerIdx;
        Peer = SBN.Peer[PeerIdx];
        
        SBN.HkPkt.CmdCount++;
        CFE_SB_InitMsg(&response, SBN_GET_PEER_STATUS_RSP_MID,
            sizeof(SBN_ModuleStatusPacket_t), TRUE);
        response.ProtocolId = Peer.ProtocolId;
        Status = SBN.IfOps[Peer.ProtocolId]->ReportModuleStatus(&response,
            Peer.IfData, SBN.Host, SBN.NumHosts);
        
        if(Status == SBN_NOT_IMPLEMENTED)
        {
            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_INFORMATION,
                "Peer status command not implemented for peer %d of type %d",
                (int)PeerIdx, (int)response.ProtocolId);
        }
        else
        {
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &response);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &response);

            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_DEBUG,
                "Peer status retrieved for peer %d", (int)PeerIdx);
        }/* end if */
    }/* end if */
}/* end SBN_GetPeerStatus */

/*******************************************************************/
/*                                                                 */
/* Reset peer                                                      */
/*                                                                 */
/*******************************************************************/
void SBN_ResetPeer(CFE_SB_MsgPtr_t MessagePtr)
{
    int                 PeerIdx = 0, Status = 0;
    SBN_PeerData_t      Peer;
    SBN_ResetPeerCmd_t *Command = NULL;

    uint16              ExpectedLength = sizeof(SBN_ResetPeerCmd_t);

    DEBUG_START();

    if(SBN_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        SBN.HkPkt.CmdCount++;
        Command = (SBN_ResetPeerCmd_t*)MessagePtr;
        PeerIdx = Command->PeerIdx;
        Peer = SBN.Peer[PeerIdx];
        
        Status = SBN.IfOps[Peer.ProtocolId]->ResetPeer(
            Peer.IfData, SBN.Host, SBN.NumHosts);
        
        if(Status == SBN_NOT_IMPLEMENTED)
        {
            CFE_EVS_SendEvent(SBN_CMD_EID,
                CFE_EVS_INFORMATION,
                "Reset peer not implemented for peer %d of type %d",
                PeerIdx, Peer.ProtocolId);
        }
        else
        {
            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_DEBUG,
                "Reset peer %d", PeerIdx);
        }/* end if */
    }/* end if */
}/* end SBN_ResetPeer */

/*******************************************************************/
/*                                                                 */
/* Verify message packet length                                    */
/*                                                                 */
/*******************************************************************/
boolean SBN_VerifyMsgLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean         result = TRUE;
    uint16          CommandCode = 0;
    uint16          ActualLength = 0;
    CFE_SB_MsgId_t  MsgId;

    DEBUG_START();

    ActualLength = CFE_SB_GetTotalMsgLength(msg);
    if(ExpectedLength != ActualLength)
    {
        MsgId   = CFE_SB_GetMsgId(msg);
        CommandCode = CFE_SB_GetCmdCode(msg);

        if(MsgId == SBN_SEND_HK_MID)
        {
            /*
            ** For a bad HK request, just send the event.  We only increment
            ** the error counter for ground commands and not internal messages.
            */
            CFE_EVS_SendEvent(SBN_HK_EID, CFE_EVS_ERROR,
                "Invalid HK request msg length: ID = 0x%04X, "
                "CC = %d, Len = %d, Expected = %d",
                MsgId, CommandCode, ActualLength, ExpectedLength);
        }
        else
        {
            /*
            ** All other cases, increment error counter
            */
            CFE_EVS_SendEvent(SBN_CMD_EID, CFE_EVS_ERROR,
                "Invalid msg length: ID = 0x%04X, "
                "CC = %d, Len = %d, Expected = %d",
                MsgId, CommandCode, ActualLength, ExpectedLength);

            SBN.HkPkt.CmdErrCount++;
        }/* end if */

        return FALSE;
    }/* end if */

    return result;
}/* end SBN_VerifyMsgLength */
