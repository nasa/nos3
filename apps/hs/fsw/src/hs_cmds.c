/************************************************************************
** File:
**   $Id: hs_cmds.c 1.4 2016/09/07 18:49:19EDT mdeschu Exp  $
**
**   Copyright © 2007-2016 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   CFS Health and Safety (HS) command handling routines
**
**   $Log: hs_cmds.c  $
**   Revision 1.4 2016/09/07 18:49:19EDT mdeschu 
**   All CFE_EVS_SendEvents with format warning arguments were explicitly cast
**   Revision 1.3 2016/05/16 17:33:11EDT czogby 
**   Move function prototype from hs_cmds.c file to hs_cmds.h file
**   Revision 1.2 2015/11/12 14:25:21EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.15 2015/05/04 11:59:12EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.14 2015/05/04 10:59:56EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.13 2015/05/01 16:48:37EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.12 2015/03/03 12:16:24EST sstrege 
**   Added copyright information
**   Revision 1.11 2011/10/13 18:47:16EDT aschoeni 
**   updated for hs utilization calibration changes
**   Revision 1.10 2011/08/16 14:59:37EDT aschoeni 
**   telemetry cmd counters are not 8 bit instead of 16
**   Revision 1.9 2011/08/15 18:49:30EDT aschoeni 
**   HS Unsubscibes when eventmon is disabled
**   Revision 1.8 2010/11/19 17:58:27EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.7 2010/11/16 18:18:57EST aschoeni 
**   Added support for Device Driver and ISR Execution Counters
**   Revision 1.6 2010/10/01 15:18:40EDT aschoeni 
**   Added Telemetry point to track message actions
**   Revision 1.5 2010/09/29 18:27:06EDT aschoeni 
**   Added Utilization Monitoring Telemetry
**   Revision 1.4 2009/06/02 16:38:47EDT aschoeni 
**   Updated telemetry and internal status to support HS Internal Status bit flags
**   Revision 1.3 2009/05/21 16:10:56EDT aschoeni 
**   Updated based on errors found during unit testing
**   Revision 1.2 2009/05/04 17:44:34EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:38EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "hs_app.h"
#include "hs_cmds.h"
#include "hs_custom.h"
#include "hs_monitors.h"
#include "hs_msgids.h"
#include "hs_events.h"
#include "hs_version.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_AppPipe(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t  MessageID   = 0;
    uint16          CommandCode = 0;

    MessageID = CFE_SB_GetMsgId(MessagePtr);
    switch (MessageID)
    {

        /*
        ** Housekeeping telemetry request
        */
        case HS_SEND_HK_MID:
            HS_HousekeepingReq(MessagePtr);
            break;

        /*
        ** HS application commands...
        */
        case HS_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
            {
                case HS_NOOP_CC:
                    HS_NoopCmd(MessagePtr);
                    break;

                case HS_RESET_CC:
                    HS_ResetCmd(MessagePtr);
                    break;

                case HS_ENABLE_APPMON_CC:
                    HS_EnableAppMonCmd(MessagePtr);
                    break;

                case HS_DISABLE_APPMON_CC:
                    HS_DisableAppMonCmd(MessagePtr);
                    break;

                case HS_ENABLE_EVENTMON_CC:
                    HS_EnableEventMonCmd(MessagePtr);
                    break;

                case HS_DISABLE_EVENTMON_CC:
                    HS_DisableEventMonCmd(MessagePtr);
                    break;

                case HS_ENABLE_ALIVENESS_CC:
                    HS_EnableAlivenessCmd(MessagePtr);
                    break;

                case HS_DISABLE_ALIVENESS_CC:
                    HS_DisableAlivenessCmd(MessagePtr);
                    break;

                case HS_RESET_RESETS_PERFORMED_CC:
                    HS_ResetResetsPerformedCmd(MessagePtr);
                    break;

                case HS_SET_MAX_RESETS_CC:
                    HS_SetMaxResetsCmd(MessagePtr);
                    break;

                case HS_ENABLE_CPUHOG_CC:
                    HS_EnableCPUHogCmd(MessagePtr);
                    break;

                case HS_DISABLE_CPUHOG_CC:
                    HS_DisableCPUHogCmd(MessagePtr);
                    break;

                default:
                    if (HS_CustomCommands(MessagePtr) != CFE_SUCCESS)
                    {
                        CFE_EVS_SendEvent(HS_CC_ERR_EID, CFE_EVS_ERROR,
                                          "Invalid command code: ID = 0x%04X, CC = %d",
                                          MessageID, CommandCode);

                        HS_AppData.CmdErrCount++;
                    }
                    break;

            } /* end CommandCode switch */
            break;

      /*
      ** Unrecognized Message ID
      */
      default:
         HS_AppData.CmdErrCount++;
         CFE_EVS_SendEvent(HS_MID_ERR_EID, CFE_EVS_ERROR,
                           "Invalid command pipe message ID: 0x%04X", MessageID);
         break;

    } /* end MessageID switch */

    return;

} /* End HS_AppPipe */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16 ExpectedLength = sizeof(HS_NoArgsCmd_t);
    uint32 AppId;
#if HS_MAX_EXEC_CNT_SLOTS != 0
    uint32 ExeCount;
    uint32 TaskId;
    CFE_ES_TaskInfo_t TaskInfo;
#endif
    int32 Status;
    uint32 TableIndex;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        /*
        ** Update HK variables
        */
        HS_AppData.HkPacket.CmdCount                = (uint8) HS_AppData.CmdCount;
        HS_AppData.HkPacket.CmdErrCount             = (uint8) HS_AppData.CmdErrCount;
        HS_AppData.HkPacket.CurrentAppMonState      = HS_AppData.CurrentAppMonState;
        HS_AppData.HkPacket.CurrentEventMonState    = HS_AppData.CurrentEventMonState;
        HS_AppData.HkPacket.CurrentAlivenessState   = HS_AppData.CurrentAlivenessState;
        HS_AppData.HkPacket.CurrentCPUHogState      = HS_AppData.CurrentCPUHogState;
        HS_AppData.HkPacket.ResetsPerformed         = HS_AppData.CDSData.ResetsPerformed;
        HS_AppData.HkPacket.MaxResets               = HS_AppData.CDSData.MaxResets;
        HS_AppData.HkPacket.EventsMonitoredCount    = HS_AppData.EventsMonitoredCount;
        HS_AppData.HkPacket.MsgActExec              = HS_AppData.MsgActExec;

        /*
        ** Calculate the current number of invalid event monitor entries
        */
        HS_AppData.HkPacket.InvalidEventMonCount    = 0;

        for(TableIndex = 0; TableIndex < HS_MAX_MONITORED_EVENTS; TableIndex++)
        {
            if(HS_AppData.EMTablePtr[TableIndex].ActionType != HS_EMT_ACT_NOACT)
            {
                Status = CFE_ES_GetAppIDByName(&AppId, HS_AppData.EMTablePtr[TableIndex].AppName);

                if (Status == CFE_ES_ERR_APPNAME)
                {
                    HS_AppData.HkPacket.InvalidEventMonCount++;

                }
            }
        }

        /*
        ** Build the HK status flags byte
        */
        HS_AppData.HkPacket.StatusFlags             = 0;
#if HS_MAX_EXEC_CNT_SLOTS != 0
        if(HS_AppData.ExeCountState == HS_STATE_ENABLED)
        {
            HS_AppData.HkPacket.StatusFlags   |= HS_LOADED_XCT;
        }
#endif
        if(HS_AppData.MsgActsState == HS_STATE_ENABLED)
        {
            HS_AppData.HkPacket.StatusFlags   |= HS_LOADED_MAT;
        }
        if(HS_AppData.AppMonLoaded == HS_STATE_ENABLED)
        {
            HS_AppData.HkPacket.StatusFlags   |= HS_LOADED_AMT;
        }
        if(HS_AppData.EventMonLoaded == HS_STATE_ENABLED)
        {
            HS_AppData.HkPacket.StatusFlags   |= HS_LOADED_EMT;
        }
        if(HS_AppData.CDSState == HS_STATE_ENABLED)
        {
            HS_AppData.HkPacket.StatusFlags   |= HS_CDS_IN_USE;
        }

        /*
        ** Update the AppMon Enables
        */
        for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
        {
            HS_AppData.HkPacket.AppMonEnables[TableIndex] = HS_AppData.AppMonEnables[TableIndex];
        }


        HS_AppData.HkPacket.UtilCpuAvg = HS_AppData.UtilCpuAvg;
        HS_AppData.HkPacket.UtilCpuPeak = HS_AppData.UtilCpuPeak;

#if HS_MAX_EXEC_CNT_SLOTS != 0
        /*
        ** Add the execution counters
        */
        for(TableIndex = 0; TableIndex < HS_MAX_EXEC_CNT_SLOTS; TableIndex++)
        {

            ExeCount = HS_INVALID_EXECOUNT;

            if((HS_AppData.ExeCountState == HS_STATE_ENABLED) &&
               ((HS_AppData.XCTablePtr[TableIndex].ResourceType == HS_XCT_TYPE_APP_MAIN) ||
                (HS_AppData.XCTablePtr[TableIndex].ResourceType == HS_XCT_TYPE_APP_CHILD)))
            {

                Status = OS_TaskGetIdByName(&TaskId, HS_AppData.XCTablePtr[TableIndex].ResourceName);

                if (Status == OS_SUCCESS)
                {
                    Status = CFE_ES_GetTaskInfo(&TaskInfo, TaskId);
                    if (Status == CFE_SUCCESS)
                    {
                        ExeCount = TaskInfo.ExecutionCounter;
                    }

                }

            }
            else if((HS_AppData.ExeCountState == HS_STATE_ENABLED) &&
               ((HS_AppData.XCTablePtr[TableIndex].ResourceType == HS_XCT_TYPE_DEVICE) ||
                (HS_AppData.XCTablePtr[TableIndex].ResourceType == HS_XCT_TYPE_ISR)))
            {

                Status = CFE_ES_GetGenCounterIDByName(&TaskId, HS_AppData.XCTablePtr[TableIndex].ResourceName);

                if (Status == CFE_SUCCESS)
                {
                    CFE_ES_GetGenCount(TaskId, &ExeCount);
                }

            }
            HS_AppData.HkPacket.ExeCounts[TableIndex] = ExeCount;

        }        

#endif

        /*
        ** Timestamp and send housekeeping packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &HS_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &HS_AppData.HkPacket);

    } /* end HS_VerifyMsgLength if */

    return;

} /* end HS_HousekeepingCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16 ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;

        CFE_EVS_SendEvent(HS_NOOP_INF_EID, CFE_EVS_INFORMATION,
                        "No-op command: Version %d.%d.%d.%d",
                         HS_MAJOR_VERSION,
                         HS_MINOR_VERSION,
                         HS_REVISION,
                         HS_MISSION_REV);
    }

    return;

} /* end HS_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_ResetCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16 ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_ResetCounters();

        CFE_EVS_SendEvent(HS_RESET_DBG_EID, CFE_EVS_DEBUG,
                          "Reset counters command");
    }

    return;

} /* end HS_ResetCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset housekeeping counters                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_ResetCounters(void)
{
    HS_AppData.CmdCount     = 0;
    HS_AppData.CmdErrCount  = 0;
    HS_AppData.EventsMonitoredCount   = 0;
    HS_AppData.MsgActExec = 0;

    return;

} /* end HS_ResetCounters */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enable applications monitor command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_EnableAppMonCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppMonStatusRefresh();
        HS_AppData.CurrentAppMonState = HS_STATE_ENABLED;
        CFE_EVS_SendEvent (HS_ENABLE_APPMON_DBG_EID,
                           CFE_EVS_DEBUG,
                           "Application Monitoring Enabled");
    }

    return;

} /* end HS_EnableAppMonCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disable applications monitor command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_DisableAppMonCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppData.CurrentAppMonState = HS_STATE_DISABLED;
        CFE_EVS_SendEvent (HS_DISABLE_APPMON_DBG_EID,
                           CFE_EVS_DEBUG,
                           "Application Monitoring Disabled");
    }

    return;

} /* end HS_DisableAppMonCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enable events monitor command                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_EnableEventMonCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);
    int32             Status = CFE_SUCCESS;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
       /*
       ** Subscribe to Event Messages if currently disabled
       */
       if (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED)
       {

          Status = CFE_SB_SubscribeEx(CFE_EVS_EVENT_MSG_MID,
                                      HS_AppData.EventPipe,
                                      CFE_SB_Default_Qos,
                                      HS_EVENT_PIPE_DEPTH);

          if (Status != CFE_SUCCESS)
          {
             CFE_EVS_SendEvent(HS_EVENTMON_SUB_EID, CFE_EVS_ERROR,
                 "Event Monitor Enable: Error Subscribing to Events,RC=0x%08X",(unsigned int)Status);
             HS_AppData.CmdErrCount++;
          }
       }

       if(Status == CFE_SUCCESS)
       {
            HS_AppData.CmdCount++;
            HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;
            CFE_EVS_SendEvent (HS_ENABLE_EVENTMON_DBG_EID,
                               CFE_EVS_DEBUG,
                               "Event Monitoring Enabled");
       }
    }

    return;

} /* end HS_EnableEventMonCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disable event monitor command                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_DisableEventMonCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);
    int32             Status = CFE_SUCCESS;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {

       /*
       ** Unsubscribe from Event Messages if currently enabled
       */
       if (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED)
       {

          Status =  CFE_SB_Unsubscribe ( CFE_EVS_EVENT_MSG_MID,
                                         HS_AppData.EventPipe );

          if (Status != CFE_SUCCESS)
          {
             CFE_EVS_SendEvent(HS_EVENTMON_UNSUB_EID, CFE_EVS_ERROR,
                 "Event Monitor Disable: Error Unsubscribing from Events,RC=0x%08X",(unsigned int)Status);
             HS_AppData.CmdErrCount++;
          }
       }

       if(Status == CFE_SUCCESS)
       {
           HS_AppData.CmdCount++;
           HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;
           CFE_EVS_SendEvent (HS_DISABLE_EVENTMON_DBG_EID,
                              CFE_EVS_DEBUG,
                              "Event Monitoring Disabled");
       }
    }

    return;

} /* end HS_DisableEventMonCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enable aliveness indicator command                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_EnableAlivenessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppData.CurrentAlivenessState = HS_STATE_ENABLED;
        CFE_EVS_SendEvent (HS_ENABLE_ALIVENESS_DBG_EID,
                           CFE_EVS_DEBUG,
                           "Aliveness Indicator Enabled");
    }

    return;

} /* end HS_EnableAlivenessCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disable aliveness indicator command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_DisableAlivenessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppData.CurrentAlivenessState = HS_STATE_DISABLED;
        CFE_EVS_SendEvent (HS_DISABLE_ALIVENESS_DBG_EID,
                           CFE_EVS_DEBUG,
                           "Aliveness Indicator Disabled");
    }

    return;

} /* end HS_DisableAlivenessCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Enable cpu hogging indicator command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_EnableCPUHogCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppData.CurrentCPUHogState = HS_STATE_ENABLED;
        CFE_EVS_SendEvent (HS_ENABLE_CPUHOG_DBG_EID,
                           CFE_EVS_DEBUG,
                           "CPU Hogging Indicator Enabled");
    }

    return;

} /* end HS_EnableCPUHogCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Disable cpu hogging indicator command                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_DisableCPUHogCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_AppData.CurrentCPUHogState = HS_STATE_DISABLED;
        CFE_EVS_SendEvent (HS_DISABLE_CPUHOG_DBG_EID,
                           CFE_EVS_DEBUG,
                           "CPU Hogging Indicator Disabled");
    }

    return;

} /* end HS_DisableCPUHogCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset processor resets performed count command                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_ResetResetsPerformedCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_NoArgsCmd_t);

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        HS_SetCDSData(0, HS_AppData.CDSData.MaxResets);
        CFE_EVS_SendEvent (HS_RESET_RESETS_DBG_EID, CFE_EVS_DEBUG,
                           "Processor Resets Performed by HS Counter has been Reset");
    }

    return;

} /* end HS_ResetResetsPerformedCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set max processor resets command                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_SetMaxResetsCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    uint16            ExpectedLength = sizeof(HS_SetMaxResetsCmd_t);
    HS_SetMaxResetsCmd_t  *CmdPtr;

    /*
    ** Verify message packet length
    */
    if(HS_VerifyMsgLength(MessagePtr, ExpectedLength))
    {
        HS_AppData.CmdCount++;
        CmdPtr = ((HS_SetMaxResetsCmd_t *)MessagePtr);

        HS_SetCDSData(HS_AppData.CDSData.ResetsPerformed, CmdPtr->MaxResets);

        CFE_EVS_SendEvent (HS_SET_MAX_RESETS_DBG_EID, CFE_EVS_DEBUG,
                           "Max Resets Performable by HS has been set to %d", 
                           HS_AppData.CDSData.MaxResets);
    }

    return;

} /* end HS_SetMaxResetsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify message packet length                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean HS_VerifyMsgLength(CFE_SB_MsgPtr_t msg,
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

       if (MessageID == HS_SEND_HK_MID)
       {
           /*
           ** For a bad HK request, just send the event. We only increment
           ** the error counter for ground commands and not internal messages.
           */
           CFE_EVS_SendEvent(HS_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR,
                   "Invalid HK request msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                   MessageID, CommandCode, ActualLength, ExpectedLength);
       }
       else
       {
           /*
           ** All other cases, increment error counter
           */
           CFE_EVS_SendEvent(HS_LEN_ERR_EID, CFE_EVS_ERROR,
                   "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                   MessageID, CommandCode, ActualLength, ExpectedLength);
           HS_AppData.CmdErrCount++;
       }

       result = FALSE;
    }

    return(result);

} /* End of HS_VerifyMsgLength */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Acquire table pointers                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_AcquirePointers(void)
{
    int32  Status;

    /*
    ** Release the table (AppMon)
    */
    CFE_TBL_ReleaseAddress(HS_AppData.AMTableHandle);

    /*
    ** Manage the table (AppMon)
    */
    CFE_TBL_Manage(HS_AppData.AMTableHandle);

    /*
    ** Get a pointer to the table (AppMon)
    */
    Status = CFE_TBL_GetAddress((void *)&HS_AppData.AMTablePtr, HS_AppData.AMTableHandle);

    /*
    ** If there is a new table, refresh status (AppMon)
    */
    if (Status == CFE_TBL_INFO_UPDATED)
    {
        HS_AppMonStatusRefresh();
    }

    /*
    ** If Address acquisition fails and currently enabled, report and disable (AppMon)
    */
    if(Status < CFE_SUCCESS)
    {
        /*
        ** Only report and disable if enabled or the table was previously loaded (AppMon)
        */
        if ((HS_AppData.AppMonLoaded == HS_STATE_ENABLED) ||
            (HS_AppData.CurrentAppMonState == HS_STATE_ENABLED))
        {
            CFE_EVS_SendEvent(HS_APPMON_GETADDR_ERR_EID, CFE_EVS_ERROR,
                              "Error getting AppMon Table address, RC=0x%08X, Application Monitoring Disabled",
                              (unsigned int)Status);
            HS_AppData.CurrentAppMonState = HS_STATE_DISABLED;
            HS_AppData.AppMonLoaded = HS_STATE_DISABLED;
        }
    }
    /*
    ** Otherwise, mark that the table is loaded (AppMon)
    */
    else
    {
        HS_AppData.AppMonLoaded = HS_STATE_ENABLED;
    }

    /*
    ** Release the table (EventMon)
    */
    CFE_TBL_ReleaseAddress(HS_AppData.EMTableHandle);

    /*
    ** Manage the table (EventMon)
    */
    CFE_TBL_Manage(HS_AppData.EMTableHandle);

    /*
    ** Get a pointer to the table (EventMon)
    */
    Status = CFE_TBL_GetAddress((void *)&HS_AppData.EMTablePtr, HS_AppData.EMTableHandle);

    /*
    ** If Address acquisition fails and currently enabled, report and disable (EventMon)
    */
    if(Status < CFE_SUCCESS)
    {
        /*
        ** Only report and disable if enabled or the table was previously loaded (EventMon)
        */
        if ((HS_AppData.EventMonLoaded == HS_STATE_ENABLED) ||
            (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED))
        {
            CFE_EVS_SendEvent(HS_EVENTMON_GETADDR_ERR_EID, CFE_EVS_ERROR,
                              "Error getting EventMon Table address, RC=0x%08X, Event Monitoring Disabled",
                              (unsigned int)Status);

            if (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED)
            {
                Status =  CFE_SB_Unsubscribe ( CFE_EVS_EVENT_MSG_MID,
                                               HS_AppData.EventPipe );

                if (Status != CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(HS_BADEMT_UNSUB_EID, CFE_EVS_ERROR,
                        "Error Unsubscribing from Events,RC=0x%08X",(unsigned int)Status);
                }
            }

            HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;
            HS_AppData.EventMonLoaded = HS_STATE_DISABLED;

        }
    }
    /*
    ** Otherwise, mark that the table is loaded (EventMon)
    */
    else
    {
        HS_AppData.EventMonLoaded = HS_STATE_ENABLED;
    }

    /*
    ** Release the table (MsgActs)
    */
    CFE_TBL_ReleaseAddress(HS_AppData.MATableHandle);

    /*
    ** Manage the table (MsgActs)
    */
    CFE_TBL_Manage(HS_AppData.MATableHandle);

    /*
    ** Get a pointer to the table (MsgActs)
    */
    Status = CFE_TBL_GetAddress((void *)&HS_AppData.MATablePtr, HS_AppData.MATableHandle);

    /*
    ** If there is a new table, refresh status (MsgActs)
    */
    if (Status == CFE_TBL_INFO_UPDATED)
    {
        HS_MsgActsStatusRefresh();
    }

    /*
    ** If Address acquisition fails report and disable (MsgActs)
    */
    if(Status < CFE_SUCCESS)
    {
        /*
        ** To prevent redundant reporting, only report if enabled (MsgActs)
        */
        if(HS_AppData.MsgActsState == HS_STATE_ENABLED)
        {
            CFE_EVS_SendEvent(HS_MSGACTS_GETADDR_ERR_EID, CFE_EVS_ERROR,
                              "Error getting MsgActs Table address, RC=0x%08X",
                              (unsigned int)Status);
            HS_AppData.MsgActsState = HS_STATE_DISABLED;
        }
    }
    /*
    ** Otherwise, make sure it is enabled (MsgActs)
    */
    else
    {
        HS_AppData.MsgActsState = HS_STATE_ENABLED;
    }

#if HS_MAX_EXEC_CNT_SLOTS != 0
    /*
    ** Release the table (ExeCount)
    */
    CFE_TBL_ReleaseAddress(HS_AppData.XCTableHandle);

    /*
    ** Manage the table (ExeCount)
    */
    CFE_TBL_Manage(HS_AppData.XCTableHandle);

    /*
    ** Get a pointer to the table (ExeCount)
    */
    Status = CFE_TBL_GetAddress((void *)&HS_AppData.XCTablePtr, HS_AppData.XCTableHandle);

    /*
    ** If Address acquisition fails report and disable (ExeCount)
    */
    if(Status < CFE_SUCCESS)
    {
        /*
        ** To prevent redundant reporting, only report if enabled (ExeCount)
        */
        if(HS_AppData.ExeCountState == HS_STATE_ENABLED)
        {
            CFE_EVS_SendEvent(HS_EXECOUNT_GETADDR_ERR_EID, CFE_EVS_ERROR,
                              "Error getting ExeCount Table address, RC=0x%08X",
                              (unsigned int)Status);
           HS_AppData.ExeCountState = HS_STATE_DISABLED;
        }
    }
    /*
    ** Otherwise, make sure it is enabled (ExeCount)
    */
    else
    {
        HS_AppData.ExeCountState = HS_STATE_ENABLED;
    }

#endif

    return;

} /* End of HS_AcquirePointers */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Refresh AppMon Status (on Table Update or Enable)               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_AppMonStatusRefresh(void)
{
    uint32  TableIndex;
    uint32  EnableIndex;

    /*
    ** Clear all AppMon Enable bits
    */
    for (EnableIndex = 0; EnableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); EnableIndex++ )
    {
        HS_AppData.AppMonEnables[EnableIndex] = 0;

    }

    /*
    ** Set AppMon enable bits and reset Countups and Exec Counter comparisons
    */
    for (TableIndex = 0; TableIndex < HS_MAX_MONITORED_APPS; TableIndex++ )
    {
        HS_AppData.AppMonLastExeCount[TableIndex] = 0;

        if ((HS_AppData.AMTablePtr[TableIndex].CycleCount == 0) ||
            (HS_AppData.AMTablePtr[TableIndex].ActionType == HS_AMT_ACT_NOACT))
        {
            HS_AppData.AppMonCheckInCountdown[TableIndex] = 0;
        }
        else
        {
            HS_AppData.AppMonCheckInCountdown[TableIndex] = HS_AppData.AMTablePtr[TableIndex].CycleCount;
            CFE_SET((HS_AppData.AppMonEnables[TableIndex / HS_BITS_PER_APPMON_ENABLE]),
                    (TableIndex % HS_BITS_PER_APPMON_ENABLE));
        }

    }

    return;

} /* end HS_AppMonStatusRefresh */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Refresh MsgActs Status (on Table Update or Enable)              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_MsgActsStatusRefresh(void)
{
    uint32  TableIndex;

    /*
    ** Clear all MsgActs Cooldowns
    */
    for (TableIndex = 0; TableIndex < HS_MAX_MSG_ACT_TYPES; TableIndex++)
    {
        HS_AppData.MsgActCooldown[TableIndex] = 0;
    }

    return;

} /* end HS_MsgActsStatusRefresh */

/************************/
/*  End of File Comment */
/************************/
