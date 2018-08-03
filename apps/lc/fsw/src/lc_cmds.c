/************************************************************************
** File:
**   $Id: lc_cmds.c 1.6 2015/03/04 16:09:55EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   CFS Limit Checker (LC) command handling routines
**
**   $Log: lc_cmds.c  $
**   Revision 1.6 2015/03/04 16:09:55EST sstrege 
**   Added copyright information
**   Revision 1.5 2012/08/22 17:17:02EDT lwalling 
**   Modified true to false transition monitor to also accept stale to false
**   Revision 1.4 2012/08/01 14:03:03PDT lwalling 
**   Add age WP results option to AP sample command
**   Revision 1.3 2012/08/01 12:40:48PDT lwalling 
**   Add STALE counters to watchpoint definition and result tables
**   Revision 1.2 2012/08/01 11:20:12PDT lwalling 
**   Change NOT_MEASURED to STALE
**   Revision 1.1 2012/07/31 13:53:37PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.15 2011/06/08 16:11:29EDT lwalling 
**   Changed call from LC_SubscribeWP() to LC_CreateHashTable(), removed function LC_SubscribeWP()
**   Revision 1.14 2011/03/02 10:53:35EST lwalling 
**   Explicitly state return value when known to be CFE_SUCCESS
**   Revision 1.13 2011/03/01 15:42:08EST lwalling 
**   Fix typo in manage function, move LC_SubscribeWP() and LC_UpdateTaskCDS() to lc_cmds.c
**   Revision 1.12 2011/03/01 09:37:34EST lwalling 
**   Modified table management logic and updates to CDS
**   Revision 1.11 2011/02/14 16:53:21EST lwalling 
**   Created LC_ResetResultsAP() and LC_ResetResultsWP(), modified reset stats cmd handlers to call them
**   Revision 1.10 2011/02/07 17:58:12EST lwalling 
**   Modify sample AP commands to target groups of AP's
**   Revision 1.9 2011/01/19 11:32:07EST jmdagost 
**   Moved mission revision number from lc_version.h to lc_platform_cfg.h.
**   Revision 1.8 2010/03/01 11:12:10EST lwalling 
**   Set data saved state flag whenever critical data is stored
**   Revision 1.7 2010/02/23 12:12:01EST lwalling 
**   Add PassiveAPCount to list of AP results cleared by command
**   Revision 1.6 2010/01/04 14:10:03EST lwalling 
**   Update CDS when report housekeeping
**   Revision 1.5 2009/01/15 15:36:14EST dahardis 
**   Unit test fixes
**   Revision 1.4 2009/01/09 11:34:53EST dahardis 
**   Fixed call to CFE_TBL_GetAddress for the Actionpoint Definition Table that was 
**   passing in the wrong table handle, causing the Actionpoint Results Table to be
**   initialized incorrectly.
**   Revision 1.3 2008/12/10 09:38:36EST dahardis 
**   Fixed calls to CFE_TBL_GetAddress (DCR #4699)
**   Revision 1.2 2008/12/03 13:59:34EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:03EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_cmds.h"
#include "lc_msgids.h"
#include "lc_events.h"
#include "lc_version.h"
#include "lc_action.h"
#include "lc_watch.h"
#include "lc_platform_cfg.h"

/************************************************************************
** Local function prototypes
*************************************************************************/
/************************************************************************/
/** \brief Sample actionpoints request
**  
**  \par Description
**       Processes an on-board sample actionpoints request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
*************************************************************************/
void LC_SampleAPReq(CFE_SB_MsgPtr_t MessagePtr);
   
/************************************************************************/
/** \brief Housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This message does not affect the command execution counter
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_AcquirePointers     \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr);

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
**  \sa #LC_NOOP_CC
**
*************************************************************************/
void LC_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the following LC application counters to zero:
**         - Command counter
**         - Command error counter
**         - Actionpoint sample counter
**         - Monitored message counter 
**         - RTS execution counter
**         - Passive RTS execution counter
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_CC
**
*************************************************************************/
void LC_ResetCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set LC state command
**  
**  \par Description
**       Processes a set LC application state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_LC_STATE_CC
**
*************************************************************************/
void LC_SetLCStateCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set AP state command
**  
**  \par Description
**       Processes a set actionpoint state ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_AP_STATE_CC
**
*************************************************************************/
void LC_SetAPStateCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Set AP permanently off command
**  
**  \par Description
**       Processes a set actionpoint permanently off ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_SET_AP_PERMOFF_CC
**
*************************************************************************/
void LC_SetAPPermOffCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset AP statistics command
**  
**  \par Description
**       Processes a reset actionpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_AP_STATS_CC
**
*************************************************************************/
void LC_ResetAPStatsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Reset WP statistics command
**  
**  \par Description
**       Processes a reset watchpoint statistics ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #LC_RESET_WP_STATS_CC
**
*************************************************************************/
void LC_ResetWPStatsCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Verify message length
**  
**  \par Description
**       Checks if the actual length of a software bus message matches 
**       the expected length and sends an error event if a mismatch
**       occures
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
**                                 references the software bus message 
**
**  \param [in]   ExpectedLength   The expected length of the message
**                                 based upon the command code
**
**  \returns
**  \retstmt Returns TRUE if the length is as expected      \endcode
**  \retstmt Returns FALSE if the length is not as expected \endcode
**  \endreturns
**
**  \sa #LC_LEN_ERR_EID
**
*************************************************************************/
boolean LC_VerifyMsgLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength);

int32 LC_ManageTables(void);


 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 LC_AppPipe(CFE_SB_MsgPtr_t MessagePtr)
{
    int32           Status      = CFE_SUCCESS;
    CFE_SB_MsgId_t  MessageID   = 0;
    uint16          CommandCode = 0;

    MessageID = CFE_SB_GetMsgId(MessagePtr);
    switch (MessageID)
    {
        /*
        ** Sample actionpoints request
        */
        case LC_SAMPLE_AP_MID:
            LC_SampleAPReq(MessagePtr);
            break;
    
        /*
        ** Housekeeping telemetry request
        ** (only routine that can return a critical error indicator)
        */
        case LC_SEND_HK_MID:
            Status = LC_HousekeepingReq(MessagePtr);
            break;

        /*
        ** LC application commands...
        */
        case LC_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
            {
                case LC_NOOP_CC:
                    LC_NoopCmd(MessagePtr);
                    break;

                case LC_RESET_CC:
                    LC_ResetCmd(MessagePtr);
                    break;

                case LC_SET_LC_STATE_CC:
                    LC_SetLCStateCmd(MessagePtr);
                    break;
             
                case LC_SET_AP_STATE_CC:
                    LC_SetAPStateCmd(MessagePtr);
                    break;
                     
                case LC_SET_AP_PERMOFF_CC:
                    LC_SetAPPermOffCmd(MessagePtr);
                    break;

                case LC_RESET_AP_STATS_CC:
                    LC_ResetAPStatsCmd(MessagePtr);
                    break;

                case LC_RESET_WP_STATS_CC:
                    LC_ResetWPStatsCmd(MessagePtr);
                    break;

                default:
                    CFE_EVS_SendEvent(LC_CC_ERR_EID, CFE_EVS_ERROR,
                                      "Invalid command code: ID = 0x%04X, CC = %d",
                                      MessageID, CommandCode);
                    
                    LC_AppData.CmdErrCount++;
                    break;
            
            } /* end CommandCode switch */
            break;
            
            /*
            ** All other message ID's should be monitor
            ** packets
            */
            default:
                LC_CheckMsgForWPs(MessageID, MessagePtr);
                break;
            
    } /* end MessageID switch */
    
    return (Status);

} /* End LC_AppPipe */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Sample Actionpoints Request                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SampleAPReq(CFE_SB_MsgPtr_t MessagePtr)
{
    LC_SampleAP_t *LC_SampleAP = (LC_SampleAP_t *) MessagePtr;
    uint16 ExpectedLength = sizeof(LC_SampleAP_t);
    uint16 WatchIndex;
    boolean ValidSampleCmd = FALSE;    

    /* 
    ** Verify message packet length 
    */
    if (LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /*
        ** Ignore AP sample requests if disabled at the application level
        */
        if (LC_AppData.CurrentLCState != LC_STATE_DISABLED)
        {
            /*
            ** Range check the actionpoint array index arguments
            */
            if ((LC_SampleAP->StartIndex == LC_ALL_ACTIONPOINTS) &&
                (LC_SampleAP->EndIndex   == LC_ALL_ACTIONPOINTS))
            {
                /*
                ** Allow special "sample all" heritage values
                */
                LC_SampleAPs(0, LC_MAX_ACTIONPOINTS - 1);
                ValidSampleCmd = TRUE;
            }
            else if ((LC_SampleAP->StartIndex <= LC_SampleAP->EndIndex) &&
                     (LC_SampleAP->EndIndex < LC_MAX_ACTIONPOINTS))
            {
                /*
                ** Start is less or equal to end, and end is within the array
                */
                LC_SampleAPs(LC_SampleAP->StartIndex, LC_SampleAP->EndIndex);
                ValidSampleCmd = TRUE;
            }
            else
            {
                /*
                ** At least one actionpoint array index is out of range
                */
                CFE_EVS_SendEvent(LC_APSAMPLE_APNUM_ERR_EID, CFE_EVS_ERROR,
                   "Sample AP error: invalid AP number, start = %d, end = %d", 
                    LC_SampleAP->StartIndex, LC_SampleAP->EndIndex);
            }

            /*
            ** Optionally update the age of watchpoint results
            */
            if ((LC_SampleAP->UpdateAge != 0) && (ValidSampleCmd))
            {
                for (WatchIndex = 0; WatchIndex < LC_MAX_WATCHPOINTS; WatchIndex++)
                {
                    if (LC_OperData.WRTPtr[WatchIndex].CountdownToStale != 0)
                    {
                        LC_OperData.WRTPtr[WatchIndex].CountdownToStale--;

                        if (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0)
                        {
                            LC_OperData.WRTPtr[WatchIndex].WatchResult = LC_WATCH_STALE;
                        }
                    }
                }
            }
        }
    }

    return;

} /* end LC_SampleAPReq */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 LC_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr)
{
    int32  Result;
    uint16 ExpectedLength = sizeof(LC_NoArgsCmd_t);
    uint16 TableIndex;
    uint16 HKIndex;
    uint8  ByteData;
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /*
        ** Update HK variables
        */
        LC_OperData.HkPacket.CmdCount             = LC_AppData.CmdCount;
        LC_OperData.HkPacket.CmdErrCount          = LC_AppData.CmdErrCount;
        LC_OperData.HkPacket.APSampleCount        = LC_AppData.APSampleCount;
        LC_OperData.HkPacket.MonitoredMsgCount    = LC_AppData.MonitoredMsgCount;
        LC_OperData.HkPacket.RTSExecCount         = LC_AppData.RTSExecCount;
        LC_OperData.HkPacket.PassiveRTSExecCount  = LC_AppData.PassiveRTSExecCount;
        LC_OperData.HkPacket.CurrentLCState       = LC_AppData.CurrentLCState;
        LC_OperData.HkPacket.WPsInUse             = LC_OperData.WatchpointCount;
        
        /*
        ** Clear out the active actionpoint count, it will get
        ** recomputed below
        */
        LC_OperData.HkPacket.ActiveAPs = 0;
        
        /*
        ** Update packed watch results
        ** (4 watch results in one 8-bit byte)
        */
        for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex += 4)
        {
            HKIndex = TableIndex / 4;

            /*
            ** Pack in first result
            */
            switch (LC_OperData.WRTPtr[TableIndex + 3].WatchResult)
            {
                case LC_WATCH_STALE:
                    ByteData = LC_HKWR_STALE << 6;
                    break;
                    
                case LC_WATCH_FALSE:
                    ByteData = LC_HKWR_FALSE << 6;
                    break;
                    
                case LC_WATCH_TRUE:
                    ByteData = LC_HKWR_TRUE  << 6;
                    break;
                    
                /*
                ** We should never get an undefined watch result,
                ** but we'll set an error result if we do
                */    
                case LC_WATCH_ERROR:
                default:   
                    ByteData = LC_HKWR_ERROR << 6;
                    break;
            }
           
            /*
            ** Pack in second result
            */
            switch (LC_OperData.WRTPtr[TableIndex + 2].WatchResult)
            {
                case LC_WATCH_STALE:
                    ByteData = (ByteData | (LC_HKWR_STALE << 4));
                    break;
                    
                case LC_WATCH_FALSE:
                    ByteData = (ByteData | (LC_HKWR_FALSE << 4));
                    break;
                    
                case LC_WATCH_TRUE:
                    ByteData = (ByteData | (LC_HKWR_TRUE  << 4));
                    break;
                    
                case LC_WATCH_ERROR:
                default:   
                    ByteData = (ByteData | (LC_HKWR_ERROR << 4));
                    break;
            }
           
            /*
            ** Pack in third result
            */
            switch (LC_OperData.WRTPtr[TableIndex + 1].WatchResult)
            {
                case LC_WATCH_STALE:
                    ByteData = (ByteData | (LC_HKWR_STALE << 2));
                    break;
                    
                case LC_WATCH_FALSE:
                    ByteData = (ByteData | (LC_HKWR_FALSE << 2));
                    break;
                    
                case LC_WATCH_TRUE:
                    ByteData = (ByteData | (LC_HKWR_TRUE  << 2));
                    break;
                    
                case LC_WATCH_ERROR:
                default:   
                    ByteData = (ByteData | (LC_HKWR_ERROR << 2));
                    break;
            }
            
            /*
            ** Pack in fourth and last result
            */
            switch (LC_OperData.WRTPtr[TableIndex].WatchResult)
            {
                case LC_WATCH_STALE:
                    ByteData = (ByteData | LC_HKWR_STALE);
                    break;
                    
                case LC_WATCH_FALSE:
                    ByteData = (ByteData | LC_HKWR_FALSE);
                    break;
                    
                case LC_WATCH_TRUE:
                    ByteData = (ByteData | LC_HKWR_TRUE);
                    break;
                    
                case LC_WATCH_ERROR:
                default:   
                    ByteData = (ByteData | LC_HKWR_ERROR);
                    break;
            }

            /*
            ** Update houskeeping watch results array
            */
            LC_OperData.HkPacket.WPResults[HKIndex] = ByteData;
            
        } /* end watch results for loop */

        /*
        ** Update packed action results
        ** (2 action state/result pairs (4 bits each) in one 8-bit byte)
        */
        for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex += 2)
        {
            HKIndex = TableIndex / 2;
 
            /*
            ** Pack in first actionpoint, current state
            */
            switch (LC_OperData.ARTPtr[TableIndex + 1].CurrentState)
            {
                case LC_ACTION_NOT_USED:
                    ByteData = LC_HKAR_STATE_NOT_USED << 6;
                    break;
                    
                case LC_APSTATE_ACTIVE:
                    ByteData = LC_HKAR_STATE_ACTIVE  << 6;
                    LC_OperData.HkPacket.ActiveAPs++;
                    break;
                    
                case LC_APSTATE_PASSIVE:
                    ByteData = LC_HKAR_STATE_PASSIVE << 6;
                    break;

                case LC_APSTATE_DISABLED:
                    ByteData = LC_HKAR_STATE_DISABLED << 6;
                    break;

                /*
                ** Permanantly disabled actionpoints get reported
                ** as unused. We should never get an undefined 
                ** action state, but we'll set to not used if we do.
                */    
                case LC_APSTATE_PERMOFF:
                default:
                    ByteData = LC_HKAR_STATE_NOT_USED << 6;
                    break;
            }

            /*
            ** Pack in first actionpoint, action result
            */
            switch (LC_OperData.ARTPtr[TableIndex + 1].ActionResult)
            {
                case LC_ACTION_STALE:
                    ByteData = (ByteData | (LC_HKAR_STALE << 4));
                    break;
                    
                case LC_ACTION_PASS:
                    ByteData = (ByteData | (LC_HKAR_PASS << 4));
                    break;
                    
                case LC_ACTION_FAIL:
                    ByteData = (ByteData | (LC_HKAR_FAIL  << 4));
                    break;
                    
                /*
                ** We should never get an undefined action result,
                ** but we'll set an error result if we do
                */    
                case LC_ACTION_ERROR:
                default:   
                    ByteData = (ByteData | (LC_HKAR_ERROR << 4));
                    break;
            }

            /*
            ** Pack in second actionpoint, current state
            */
            switch (LC_OperData.ARTPtr[TableIndex].CurrentState)
            {
                case LC_ACTION_NOT_USED:
                    ByteData = (ByteData | (LC_HKAR_STATE_NOT_USED << 2));
                    break;
                    
                case LC_APSTATE_ACTIVE:
                    ByteData = (ByteData | (LC_HKAR_STATE_ACTIVE  << 2));
                    LC_OperData.HkPacket.ActiveAPs++;
                    break;
                    
                case LC_APSTATE_PASSIVE:
                    ByteData = (ByteData | (LC_HKAR_STATE_PASSIVE << 2));
                    break;

                case LC_APSTATE_DISABLED:
                    ByteData = (ByteData | (LC_HKAR_STATE_DISABLED << 2));
                    break;

                case LC_APSTATE_PERMOFF:
                default:
                    ByteData = (ByteData | (LC_HKAR_STATE_NOT_USED << 2));
                    break;
            }

            /*
            ** Pack in second actionpoint, action result
            */
            switch (LC_OperData.ARTPtr[TableIndex].ActionResult)
            {
                case LC_ACTION_STALE:
                    ByteData = (ByteData | LC_HKAR_STALE);
                    break;
                    
                case LC_ACTION_PASS:
                    ByteData = (ByteData | LC_HKAR_PASS);
                    break;
                    
                case LC_ACTION_FAIL:
                    ByteData = (ByteData | LC_HKAR_FAIL);
                    break;
                    
                case LC_ACTION_ERROR:
                default:   
                    ByteData = (ByteData | LC_HKAR_ERROR);
                    break;
            }
            
            /*
            ** Update houskeeping action results array
            */
            LC_OperData.HkPacket.APResults[HKIndex] = ByteData;
        
        } /* end action results for loop */
        
        /*
        ** Timestamp and send housekeeping packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &LC_OperData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &LC_OperData.HkPacket);
        
    } /* end LC_VerifyMsgLength if */
    
    /*
    ** Manage tables - allow cFE to perform dump, update, etc.
    **  (an error here is fatal - LC must be able to access its tables)
    */ 
    if ((Result = LC_ManageTables()) != CFE_SUCCESS)
    {
        return(Result);
    }

    /*
    ** A somewhat arbitrary decision was made to update the CDS only
    **  as often as we report housekeeping, thus we do it here.  One
    **  alternative was to do the update every time the results tables
    **  were modified - but that would result in the update occurring
    **  several times per second.  By doing the update now we cut down
    **  on the update frequency at the cost of the stored data being
    **  a couple of seconds old when a processor reset does occur.
    */
    if (LC_OperData.HaveActiveCDS)
    {
        /*
        ** If CDS is enabled - update the 3 CDS areas managed by LC
        **  (continue, but disable CDS if unable to update all 3)
        */
        if (LC_UpdateTaskCDS() != CFE_SUCCESS)
        {
            LC_OperData.HaveActiveCDS = FALSE;
        }
    }
    
    return(CFE_SUCCESS);

} /* end LC_HousekeepingCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16 ExpectedLength = sizeof(LC_NoArgsCmd_t);
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
       LC_AppData.CmdCount++;
       
       CFE_EVS_SendEvent(LC_NOOP_INF_EID, CFE_EVS_INFORMATION,
                        "No-op command: Version %d.%d.%d.%d",
                         LC_MAJOR_VERSION,
                         LC_MINOR_VERSION,
                         LC_REVISION,
                         LC_MISSION_REV);
    }

    return;

} /* end LC_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16 ExpectedLength = sizeof(LC_NoArgsCmd_t);
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        LC_ResetCounters();
        
        CFE_EVS_SendEvent(LC_RESET_DBG_EID, CFE_EVS_DEBUG,
                          "Reset counters command");
    }
    
    return;
   
} /* end LC_ResetCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset housekeeping counters                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetCounters(void)
{
    LC_AppData.CmdCount     = 0;               
    LC_AppData.CmdErrCount  = 0;         

    LC_AppData.APSampleCount        = 0;          
    LC_AppData.MonitoredMsgCount    = 0;  
    LC_AppData.RTSExecCount         = 0;          
    LC_AppData.PassiveRTSExecCount  = 0; 
   
    return;
    
} /* end LC_ResetCounters */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set LC state command                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetLCStateCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(LC_SetLCState_t);
    LC_SetLCState_t  *CmdPtr;
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        CmdPtr = ((LC_SetLCState_t *)MessagePtr);
        
        switch (CmdPtr -> NewLCState)
        {
            case LC_STATE_ACTIVE:
            case LC_STATE_PASSIVE:
            case LC_STATE_DISABLED:
                LC_AppData.CurrentLCState = CmdPtr -> NewLCState;
                LC_AppData.CmdCount++;
                
                CFE_EVS_SendEvent(LC_LCSTATE_INF_EID, CFE_EVS_INFORMATION,
                                  "Set LC state command: new state = %d", 
                                  CmdPtr -> NewLCState);
                break;
                
            default:
                CFE_EVS_SendEvent(LC_LCSTATE_ERR_EID, CFE_EVS_ERROR,
                                  "Set LC state error: invalid state = %d", 
                                  CmdPtr -> NewLCState);
                
                LC_AppData.CmdErrCount++;
                break;
        }
    }
    
    return;
   
} /* end LC_SetLCStateCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint state command                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPStateCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16           ExpectedLength = sizeof(LC_SetAPState_t);
    LC_SetAPState_t  *CmdPtr;
    uint32           TableIndex;
    uint8            CurrentAPState;
    boolean          ValidState = TRUE;
    boolean          CmdSuccess = FALSE;
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        CmdPtr = ((LC_SetAPState_t *)MessagePtr);

        /*
        ** Do a sanity check on the new actionpoint state 
        ** specified.
        */        
        switch (CmdPtr -> NewAPState)
        {
            case LC_APSTATE_ACTIVE:
            case LC_APSTATE_PASSIVE:
            case LC_APSTATE_DISABLED:
                break;
                
            default:
                ValidState = FALSE;
                CFE_EVS_SendEvent(LC_APSTATE_NEW_ERR_EID, CFE_EVS_ERROR,
                                  "Set AP state error: AP = %d, Invalid new state = %d", 
                                  CmdPtr -> APNumber, CmdPtr -> NewAPState);
                
                LC_AppData.CmdErrCount++;
                break;
        }
        
        /*
        ** Do the rest based on the actionpoint ID we were given
        */ 
        if (ValidState == TRUE)
        {
            if ((CmdPtr -> APNumber) == LC_ALL_ACTIONPOINTS)
            {
                /*
                ** Set all actionpoints to the new state except those that are not
                ** used or set permanently off
                */
                for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
                {
                    CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;
                
                    if ((CurrentAPState != LC_ACTION_NOT_USED) &&
                        (CurrentAPState != LC_APSTATE_PERMOFF))
                    {
                        LC_OperData.ARTPtr[TableIndex].CurrentState = CmdPtr -> NewAPState;
                    }
                }
                
                /*
                ** Set flag that we succeeded
                */
                CmdSuccess = TRUE;
            }
            else
            {
                if ((CmdPtr -> APNumber) < LC_MAX_ACTIONPOINTS)
                {
                    TableIndex = CmdPtr -> APNumber;
                    CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;
                
                    if ((CurrentAPState != LC_ACTION_NOT_USED) &&
                        (CurrentAPState != LC_APSTATE_PERMOFF))
                    {
                        /* 
                        ** Update state for single actionpoint specified
                        */
                        LC_OperData.ARTPtr[TableIndex].CurrentState = CmdPtr -> NewAPState;

                        CmdSuccess = TRUE;
                    }
                    else
                    {
                        /* 
                        ** Actionpoints that are not used or set permanently
                        ** off can only be changed by a table load 
                        */
                        CFE_EVS_SendEvent(LC_APSTATE_CURR_ERR_EID, CFE_EVS_ERROR,
                                          "Set AP state error: AP = %d, Invalid current AP state = %d", 
                                          CmdPtr -> APNumber, CurrentAPState);
                        
                        LC_AppData.CmdErrCount++;
                    }
                }
                else
                {
                    /*
                    **  Actionpoint number is out of range
                    **  (it's zero based, since it's a table index) 
                    */
                    CFE_EVS_SendEvent(LC_APSTATE_APNUM_ERR_EID, CFE_EVS_ERROR,
                                      "Set AP state error: Invalid AP number = %d", 
                                      CmdPtr -> APNumber);
                    
                    LC_AppData.CmdErrCount++;
                }
            }    

            /*
            ** Update the command counter and send out event if command
            ** executed
            */
            if (CmdSuccess == TRUE)
            {
                LC_AppData.CmdCount++;
            
                CFE_EVS_SendEvent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION,
                                  "Set AP state command: AP = %d, New state = %d", 
                                  CmdPtr -> APNumber, CmdPtr -> NewAPState);
            }
            
        } /* end ValidState if */
        
    } /* end LC_VerifyMsgLength if */
    
    return;
   
} /* end LC_SetAPStateCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set actionpoint permanently off command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_SetAPPermOffCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16             ExpectedLength = sizeof(LC_SetAPPermOff_t);
    LC_SetAPPermOff_t  *CmdPtr;
    uint32             TableIndex;
    uint8              CurrentAPState;
    
    /* 
    ** Verify message packet length 
    */
    if(LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        CmdPtr = ((LC_SetAPPermOff_t *)MessagePtr);

        if (((CmdPtr -> APNumber) == LC_ALL_ACTIONPOINTS) ||
            ((CmdPtr -> APNumber) >= LC_MAX_ACTIONPOINTS))
        {
            /*
            **  Invalid actionpoint number
            **  (This command can't be invoked for all actionpoints) 
            */
            CFE_EVS_SendEvent(LC_APOFF_APNUM_ERR_EID, CFE_EVS_ERROR,
                              "Set AP perm off error: Invalid AP number = %d", 
                              CmdPtr -> APNumber);
            
            LC_AppData.CmdErrCount++;
                
        }
        else
        {
            TableIndex = CmdPtr -> APNumber;
            CurrentAPState = LC_OperData.ARTPtr[TableIndex].CurrentState;
        
            if (CurrentAPState != LC_APSTATE_DISABLED)
            {
                /* 
                ** Actionpoints can only be turned permanently off if
                ** they are currently disabled
                */
                CFE_EVS_SendEvent(LC_APOFF_CURR_ERR_EID, CFE_EVS_ERROR,
                                  "Set AP perm off error, AP NOT Disabled: AP = %d, Current state = %d", 
                                  CmdPtr -> APNumber, CurrentAPState);
                
                LC_AppData.CmdErrCount++;
            }
            else
            {
                /* 
                ** Update state for actionpoint specified
                */
                LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PERMOFF;

                LC_AppData.CmdCount++;
            
                CFE_EVS_SendEvent(LC_APOFF_INF_EID, CFE_EVS_INFORMATION,
                                  "Set AP permanently off command: AP = %d", 
                                  CmdPtr -> APNumber);
            }
            
        } /* end CmdPtr -> APNumber else */
        
    } /* end LC_VerifyMsgLength if */
    
    return;
   
} /* end LC_SetAPPermOffCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset actionpoint statistics command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetAPStatsCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16             ExpectedLength = sizeof(LC_ResetAPStats_t);
    LC_ResetAPStats_t *CmdPtr = (LC_ResetAPStats_t *) MessagePtr;
    boolean            CmdSuccess = FALSE;
    
    /* verify message packet length */
    if (LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /* arg may be one or all AP's */
        if (CmdPtr->APNumber == LC_ALL_ACTIONPOINTS)
        {
            LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, TRUE);
            CmdSuccess = TRUE;
        }
        else if (CmdPtr->APNumber < LC_MAX_ACTIONPOINTS)
        {
            LC_ResetResultsAP(CmdPtr->APNumber, CmdPtr->APNumber, TRUE);
            CmdSuccess = TRUE;
        }
        else
        {
            /* arg is out of range (zero based table index) */
            LC_AppData.CmdErrCount++;

            CFE_EVS_SendEvent(LC_APSTATS_APNUM_ERR_EID, CFE_EVS_ERROR,
               "Reset AP stats error: invalid AP number = %d", CmdPtr->APNumber);
        }    

        if (CmdSuccess == TRUE)
        {
            LC_AppData.CmdCount++;
        
            CFE_EVS_SendEvent(LC_APSTATS_INF_EID, CFE_EVS_INFORMATION,
               "Reset AP stats command: AP = %d", CmdPtr->APNumber);
        }
    }
    
    return;
   
} /* end LC_ResetAPStatsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset selected AP statistics (utility function)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetResultsAP(uint32 StartIndex, uint32 EndIndex, boolean ResetStatsCmd)
{
    uint32 TableIndex;

    /* reset selected entries in actionpoint results table */
    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        if (!ResetStatsCmd)
        {
            /* reset AP stats command does not modify AP state or most recent test result */
            LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_STALE;
            LC_OperData.ARTPtr[TableIndex].CurrentState = LC_OperData.ADTPtr[TableIndex].DefaultState;
        }

        LC_OperData.ARTPtr[TableIndex].PassiveAPCount          = 0;
        LC_OperData.ARTPtr[TableIndex].FailToPassCount         = 0;
        LC_OperData.ARTPtr[TableIndex].PassToFailCount         = 0;

        LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount    = 0;
        LC_OperData.ARTPtr[TableIndex].CumulativeFailCount     = 0;
        LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount  = 0;
    }

    return;
   
} /* end LC_ResetResultsAP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset watchpoint statistics command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetWPStatsCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16             ExpectedLength = sizeof(LC_ResetWPStats_t);
    LC_ResetWPStats_t *CmdPtr = (LC_ResetWPStats_t *) MessagePtr;
    boolean            CmdSuccess = FALSE;
    
    /* verify message packet length */
    if (LC_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /* arg may be one or all WP's */
        if (CmdPtr->WPNumber == LC_ALL_WATCHPOINTS)
        {
            LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, TRUE);
            CmdSuccess = TRUE;
        }
        else if (CmdPtr->WPNumber < LC_MAX_WATCHPOINTS)
        {
            LC_ResetResultsWP(CmdPtr->WPNumber, CmdPtr->WPNumber, TRUE);
            CmdSuccess = TRUE;
        }
        else
        {
            /* arg is out of range (zero based table index) */
            LC_AppData.CmdErrCount++;

            CFE_EVS_SendEvent(LC_WPSTATS_WPNUM_ERR_EID, CFE_EVS_ERROR,
               "Reset WP stats error: invalid WP number = %d", CmdPtr->WPNumber);
        }    
        
        if (CmdSuccess == TRUE)
        {
            LC_AppData.CmdCount++;
        
            CFE_EVS_SendEvent(LC_WPSTATS_INF_EID, CFE_EVS_INFORMATION,
               "Reset WP stats command: WP = %d", CmdPtr->WPNumber);
        }
    }
    
    return;
   
} /* end LC_ResetWPStatsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset selected WP statistics (utility function)                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void LC_ResetResultsWP(uint32 StartIndex, uint32 EndIndex, boolean ResetStatsCmd)
{
    uint32 TableIndex;

    /* reset selected entries in watchoint results table */
    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        if (!ResetStatsCmd)
        {
            /* reset WP stats command does not modify most recent test result */
            LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_STALE;
            LC_OperData.WRTPtr[TableIndex].CountdownToStale = 0;
        }

        LC_OperData.WRTPtr[TableIndex].EvaluationCount      = 0;
        LC_OperData.WRTPtr[TableIndex].FalseToTrueCount     = 0;
        LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount = 0;
        LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount  = 0;

        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value                = 0;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds    = 0;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds = 0;

        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value                = 0;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds    = 0;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds = 0;
    }

    return;
   
} /* end LC_ResetResultsWP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify message packet length                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean LC_VerifyMsgLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength)
{
   boolean result = TRUE;
   uint16  CommandCode;  
   uint16  ActualLength;
   CFE_SB_MsgId_t MessageID;
   
   /*
   ** Verify the message packet length...
   */
   ActualLength = CFE_SB_GetTotalMsgLength(msg);
   if (ExpectedLength != ActualLength)
   {
      MessageID   = CFE_SB_GetMsgId(msg);
      CommandCode = CFE_SB_GetCmdCode(msg);

      if (MessageID == LC_SEND_HK_MID)
      {
          /*
          ** For a bad HK request, just send the event. We only increment
          ** the error counter for ground commands and not internal messages.
          */
          CFE_EVS_SendEvent(LC_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR,
                  "Invalid HK request msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                  MessageID, CommandCode, ActualLength, ExpectedLength);
      }
      else if (MessageID == LC_SAMPLE_AP_MID)
      {
          /*
          ** Same thing as previous for a bad actionpoint sample request
          */
          CFE_EVS_SendEvent(LC_APSAMPLE_LEN_ERR_EID, CFE_EVS_ERROR,
                  "Invalid AP sample msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                  MessageID, CommandCode, ActualLength, ExpectedLength);
      }
      else
      {
          /*
          ** All other cases, increment error counter
          */
          CFE_EVS_SendEvent(LC_LEN_ERR_EID, CFE_EVS_ERROR,
                  "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                  MessageID, CommandCode, ActualLength, ExpectedLength);
          LC_AppData.CmdErrCount++;          
      }

      result = FALSE;
   }

   return(result);

} /* End of LC_VerifyMsgLength */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Manage tables - chance to be dumped, reloaded, etc.             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_ManageTables(void)
{
    int32  Result;

    /*
    ** It is not necessary to release dump only table pointers before
    **  calling cFE Table Services to manage the table
    */
    CFE_TBL_Manage(LC_OperData.WRTHandle);
    CFE_TBL_Manage(LC_OperData.ARTHandle);

    /*
    ** Must release loadable table pointers before allowing updates
    */
    CFE_TBL_ReleaseAddress(LC_OperData.WDTHandle);
    CFE_TBL_ReleaseAddress(LC_OperData.ADTHandle);

    CFE_TBL_Manage(LC_OperData.WDTHandle);
    CFE_TBL_Manage(LC_OperData.ADTHandle);

    /*
    ** Re-acquire the pointers and check for new table data
    */
    Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /*
        ** Clear watchpoint results for previous table
        */
        LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, FALSE);

        /*
        ** Create watchpoint hash tables -- also subscribes to watchpoint packets
        */
        LC_CreateHashTable();
    }
    else if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting WDT address, RC=0x%08X", Result);
        return(Result);
    }

    Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

    if (Result == CFE_TBL_INFO_UPDATED)
    {
        /*
        ** Clear actionpoint results for previous table
        */
        LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, FALSE);
    }
    else if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting ADT address, RC=0x%08X", Result);
        return(Result);
    }

    return(CFE_SUCCESS);

} /* LC_ManageTables() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Update Critical Data Store (CDS)                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_UpdateTaskCDS(void)
{
    int32 Result;

    /*
    ** Copy the watchpoint results table (WRT) data to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.WRTDataCDSHandle, LC_OperData.WRTPtr);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WRT_NO_SAVE_ERR_EID, CFE_EVS_ERROR, 
                          "Unable to update watchpoint results in CDS, RC=0x%08X", Result);
        return(Result);
    }

    /*
    ** Copy the actionpoint results table (ART) data to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.ARTDataCDSHandle, LC_OperData.ARTPtr);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ART_NO_SAVE_ERR_EID, CFE_EVS_ERROR, 
                          "Unable to update actionpoint results in CDS, RC=0x%08X", Result);
        return(Result);
    }

    /*
    ** Set the "data has been saved" indicator
    */
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /*
    ** Copy the global application data structure to CDS
    */
    Result = CFE_ES_CopyToCDS(LC_OperData.AppDataCDSHandle, &LC_AppData);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_APP_NO_SAVE_START_ERR_EID, CFE_EVS_ERROR, 
                          "Unable to update application data in CDS, RC=0x%08X", Result);
        return(Result);
    }

    return(CFE_SUCCESS);

} /* LC_UpdateTaskCDS() */


/************************/
/*  End of File Comment */
/************************/
