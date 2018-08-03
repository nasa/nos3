/*************************************************************************
** File:
**   $Id: hs_monitors.c 1.4 2016/09/07 18:49:18EDT mdeschu Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   Functions used for CFS Health and Safety Monitors for Applications
**   and Events
**
**   $Log: hs_monitors.c  $
**   Revision 1.4 2016/09/07 18:49:18EDT mdeschu 
**   All CFE_EVS_SendEvents with format warning arguments were explicitly cast
**   Revision 1.3 2016/08/05 09:27:05EDT mdeschu 
**   Ticket #17 HS - Fix payload structure access
**   Revision 1.2 2015/11/12 14:25:14EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.19 2015/05/04 11:59:20EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.18 2015/05/04 11:00:09EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.17 2015/05/01 16:48:56EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.16 2015/03/03 12:16:18EST sstrege 
**   Added copyright information
**   Revision 1.15 2011/10/13 18:48:06EDT aschoeni 
**   updated for hs utilization calibration changes
**   Revision 1.14 2011/03/23 12:16:12EDT aschoeni 
**   Fixed event number in hogging event
**   Revision 1.13 2010/11/19 17:58:31EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.12 2010/11/17 17:05:09EST aschoeni 
**   minor fixes for CPU utilization
**   Revision 1.11 2010/11/16 18:19:29EST aschoeni 
**   Added support for Device Driver and ISR Execution Counters
**   Revision 1.10 2010/10/14 17:45:13EDT aschoeni 
**   Removed assumptions of rate of utilization measurement
**   Revision 1.9 2010/10/01 15:18:37EDT aschoeni 
**   Added Telemetry point to track message actions
**   Revision 1.8 2010/09/29 18:28:39EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.7 2010/09/13 14:41:10EDT aschoeni 
**   Made Table validation events Info instead of Debug
**   Revision 1.6 2009/08/20 16:03:59EDT aschoeni 
**   Updated validation error output to output the proper app/resource name and limit it to 20 characters.
**   Revision 1.5 2009/06/02 16:34:11EDT aschoeni 
**   Removed 'ID' field from XCT val error event
**   Revision 1.4 2009/05/21 16:21:36EDT aschoeni 
**   added newline characters to syslog messages
**   Revision 1.3 2009/05/21 16:10:55EDT aschoeni 
**   Updated based on errors found during unit testing
**   Revision 1.2 2009/05/04 17:44:32EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:43EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "hs_app.h"
#include "hs_monitors.h"
#include "hs_custom.h"
#include "hs_tbldefs.h"
#include "hs_events.h"

#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Monitor Applications                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_MonitorApplications(void)
{
    CFE_ES_AppInfo_t   AppInfo;
    uint32             AppId;
    int32              Status;
    uint32             TableIndex;
    uint16             ActionType;
    uint32             MsgActsIndex;

    for(TableIndex = 0; TableIndex < HS_MAX_MONITORED_APPS; TableIndex++)
    {

        ActionType = HS_AppData.AMTablePtr[TableIndex].ActionType;

        /*
        ** Check this App if it has an action, and hasn't already expired
        */
        if((ActionType != HS_AMT_ACT_NOACT) &&
           (HS_AppData.AppMonCheckInCountdown[TableIndex] != 0))
        {
            Status = CFE_ES_GetAppIDByName(&AppId, HS_AppData.AMTablePtr[TableIndex].AppName);

            if (Status == CFE_SUCCESS)
            {
                Status = CFE_ES_GetAppInfo(&AppInfo, AppId);
            }
            else if(HS_AppData.AppMonCheckInCountdown[TableIndex] == HS_AppData.AMTablePtr[TableIndex].CycleCount)
            {
                /*
                ** Only send an event the first time the App fails to resolve
                */
                CFE_EVS_SendEvent(HS_APPMON_APPNAME_ERR_EID, CFE_EVS_ERROR,
                                 "App Monitor App Name not found: APP:(%s)",
                                  HS_AppData.AMTablePtr[TableIndex].AppName);
            }

            /*
            ** Failure to get an execution counter is not considered an automatic failure (or eventworthy)
            */
            if((Status == CFE_SUCCESS) &&
               (HS_AppData.AppMonLastExeCount[TableIndex] != AppInfo.ExecutionCounter))
            {
                /*
                ** Set the current count, and reset the timeout
                */
                HS_AppData.AppMonCheckInCountdown[TableIndex] = HS_AppData.AMTablePtr[TableIndex].CycleCount;
                HS_AppData.AppMonLastExeCount[TableIndex] = AppInfo.ExecutionCounter;
            }
            else
            {
                HS_AppData.AppMonCheckInCountdown[TableIndex]--;

                /*
                ** Take Action once the counter reaches zero
                */
                if(HS_AppData.AppMonCheckInCountdown[TableIndex] == 0)
                {

                    /*
                    ** Unset the enabled bit flag
                    */
                    CFE_CLR(HS_AppData.AppMonEnables[TableIndex / HS_BITS_PER_APPMON_ENABLE],
                            (TableIndex % HS_BITS_PER_APPMON_ENABLE));
                    switch (ActionType)
                    {

                        case HS_AMT_ACT_PROC_RESET:
                            CFE_EVS_SendEvent(HS_APPMON_PROC_ERR_EID, CFE_EVS_ERROR,
                               "App Monitor Failure: APP:(%s): Action: Processor Reset",
                               HS_AppData.AMTablePtr[TableIndex].AppName);

                            /*
                            ** Perform a reset if we can
                            */
                            if(HS_AppData.CDSData.ResetsPerformed < HS_AppData.CDSData.MaxResets)
                            {
                                HS_SetCDSData((HS_AppData.CDSData.ResetsPerformed + 1), HS_AppData.CDSData.MaxResets);

                                OS_TaskDelay(HS_RESET_TASK_DELAY);
                                CFE_ES_WriteToSysLog("HS App: App Monitor Failure: APP:(%s): Action: Processor Reset\n",
                                                      HS_AppData.AMTablePtr[TableIndex].AppName);
                                HS_AppData.ServiceWatchdogFlag = HS_STATE_DISABLED;
                                CFE_ES_ResetCFE(CFE_ES_PROCESSOR_RESET);
                            }
                            else
                            {
                                CFE_EVS_SendEvent(HS_RESET_LIMIT_ERR_EID, CFE_EVS_ERROR,
                                   "Processor Reset Action Limit Reached: No Reset Performed");
                            }

                            break;
    
                        case HS_AMT_ACT_APP_RESTART:
                            CFE_EVS_SendEvent(HS_APPMON_RESTART_ERR_EID, CFE_EVS_ERROR,
                                "App Monitor Failure: APP:(%s) Action: Restart Application",
                                HS_AppData.AMTablePtr[TableIndex].AppName);
                            /*
                            ** Attempt to restart the App if we resolved the AppId
                            */
                            if (Status == CFE_SUCCESS)
                            {
                                Status = CFE_ES_RestartApp(AppId);
                            }
 
                            /*
                            ** Report an error; either no valid AppId, or RestartApp failed
                            */
                            if (Status != CFE_SUCCESS)
                            {
                                CFE_EVS_SendEvent(HS_APPMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR,
                                    "Call to Restart App Failed: APP:(%s) ERR: 0x%08X",
                                    HS_AppData.AMTablePtr[TableIndex].AppName, (unsigned int)Status);
                            }

                            break;
    
                        case HS_AMT_ACT_EVENT:
                            CFE_EVS_SendEvent(HS_APPMON_FAIL_ERR_EID, CFE_EVS_ERROR,
                                "App Monitor Failure: APP:(%s): Action: Event Only",
                                HS_AppData.AMTablePtr[TableIndex].AppName);
                            break;
    
                        /*
                        ** Also the case for Message Action types
                        */
                        case HS_AMT_ACT_NOACT:
                        default:
                            /*
                            ** Check to see if this is a Message Action Type
                            */
                            if((HS_AppData.MsgActsState == HS_STATE_ENABLED) &&
                               (ActionType > HS_AMT_ACT_LAST_NONMSG) &&
                               (ActionType <= (HS_AMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES)))
                            {
                                MsgActsIndex = ActionType - HS_AMT_ACT_LAST_NONMSG - 1;

                                /*
                                ** Send the message if off cooldown and not disabled
                                */
                                if((HS_AppData.MsgActCooldown[MsgActsIndex] == 0) &&
                                    (HS_AppData.MATablePtr[MsgActsIndex].EnableState != HS_MAT_STATE_DISABLED))
                                {
                                    CFE_SB_SendMsg((CFE_SB_Msg_t *) HS_AppData.MATablePtr[MsgActsIndex].Message);
                                    HS_AppData.MsgActExec++;
                                    HS_AppData.MsgActCooldown[MsgActsIndex] = HS_AppData.MATablePtr[MsgActsIndex].Cooldown;
                                    if(HS_AppData.MATablePtr[MsgActsIndex].EnableState != HS_MAT_STATE_NOEVENT)
                                    {
                                        CFE_EVS_SendEvent(HS_APPMON_MSGACTS_ERR_EID, CFE_EVS_ERROR,
                                            "App Monitor Failure: APP:(%s): Action: Message Action Index: %d",
                                            HS_AppData.AMTablePtr[TableIndex].AppName, (int)MsgActsIndex);
                                    
                                    }

                                }

                            }

                            /* Otherwise, Take No Action */
                            break;
                    } /* end switch */

                } /* end (HS_AppData.AppMonCheckInCountdown[TableIndex] == 0) if */

            } /* end "failed to update counter" else */

        } /* end (HS_AppData.AppMonCheckInCountdown[TableIndex] != 0) if */

    } /* end for loop */

    return;

} /* end HS_MonitorApplications */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Monitor Events                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_MonitorEvent(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_EVS_Packet_t  *EventPtr;
    uint32 TableIndex;
    int32  Status;
    uint32 AppId = 0;
    uint16 ActionType;
    uint32 MsgActsIndex;

    EventPtr = ((CFE_EVS_Packet_t *)MessagePtr);

    for(TableIndex = 0; TableIndex < HS_MAX_MONITORED_EVENTS; TableIndex++)
    {
        ActionType = HS_AppData.EMTablePtr[TableIndex].ActionType;

        /*
        ** Check this Event Monitor if it has an action, and the event IDs match
        */
        if ((ActionType != HS_EMT_ACT_NOACT) &&
            (HS_AppData.EMTablePtr[TableIndex].EventID == EventPtr->Payload.PacketID.EventID))
        {
            if ( strncmp(HS_AppData.EMTablePtr[TableIndex].AppName, EventPtr->Payload.PacketID.AppName, OS_MAX_API_NAME) == 0 )
            {

                /*
                ** Perform the action if the strings also match
                */
                switch (ActionType)
                {

                    case HS_EMT_ACT_PROC_RESET:
                       CFE_EVS_SendEvent(HS_EVENTMON_PROC_ERR_EID, CFE_EVS_ERROR,
                                         "Event Monitor: APP:(%s) EID:(%d): Action: Processor Reset",
                                         HS_AppData.EMTablePtr[TableIndex].AppName,
                                         HS_AppData.EMTablePtr[TableIndex].EventID);

                        /*
                        ** Perform a reset if we can
                        */
                        if(HS_AppData.CDSData.ResetsPerformed < HS_AppData.CDSData.MaxResets)
                        {
                            HS_SetCDSData((HS_AppData.CDSData.ResetsPerformed + 1), HS_AppData.CDSData.MaxResets);

                            OS_TaskDelay(HS_RESET_TASK_DELAY);
                            CFE_ES_WriteToSysLog("HS App: Event Monitor: APP:(%s) EID:(%d): Action: Processor Reset\n",
                                                  HS_AppData.EMTablePtr[TableIndex].AppName,
                                                  (int)HS_AppData.EMTablePtr[TableIndex].EventID);
                            HS_AppData.ServiceWatchdogFlag = HS_STATE_DISABLED;
                            CFE_ES_ResetCFE(CFE_ES_PROCESSOR_RESET);
                        }
                        else
                        {
                           CFE_EVS_SendEvent(HS_RESET_LIMIT_ERR_EID, CFE_EVS_ERROR,
                              "Processor Reset Action Limit Reached: No Reset Performed");
                        }


                        break;

                    case HS_EMT_ACT_APP_RESTART:
                        /*
                        ** Check to see if the App is still there, and try to restart if it is
                        */
                        Status = CFE_ES_GetAppIDByName(&AppId, HS_AppData.EMTablePtr[TableIndex].AppName);
                        if (Status == CFE_SUCCESS)
                        {
                            CFE_EVS_SendEvent(HS_EVENTMON_RESTART_ERR_EID, CFE_EVS_ERROR,
                                "Event Monitor: APP:(%s) EID:(%d): Action: Restart Application",
                                HS_AppData.EMTablePtr[TableIndex].AppName,
                                HS_AppData.EMTablePtr[TableIndex].EventID);
                            Status = CFE_ES_RestartApp(AppId);
                        }

                        if (Status != CFE_SUCCESS)
                        {
                            CFE_EVS_SendEvent(HS_EVENTMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR,
                                "Call to Restart App Failed: APP:(%s) ERR: 0x%08X",
                                HS_AppData.EMTablePtr[TableIndex].AppName, (unsigned int)Status);
                        }

                        break;


                    case HS_EMT_ACT_APP_DELETE:
                        /*
                        ** Check to see if the App is still there, and try to delete if it is
                        */
                        Status = CFE_ES_GetAppIDByName(&AppId, HS_AppData.EMTablePtr[TableIndex].AppName);
                        if (Status == CFE_SUCCESS)
                        {
                            CFE_EVS_SendEvent(HS_EVENTMON_DELETE_ERR_EID, CFE_EVS_ERROR,
                                "Event Monitor: APP:(%s) EID:(%d): Action: Delete Application",
                                HS_AppData.EMTablePtr[TableIndex].AppName,
                                HS_AppData.EMTablePtr[TableIndex].EventID);
                            Status = CFE_ES_DeleteApp(AppId);
                        }

                        if (Status != CFE_SUCCESS)
                        {
                            CFE_EVS_SendEvent(HS_EVENTMON_NOT_DELETED_ERR_EID, CFE_EVS_ERROR,
                                "Call to Delete App Failed: APP:(%s) ERR: 0x%08X",
                                HS_AppData.EMTablePtr[TableIndex].AppName, (unsigned int)Status);
                        }

                        break;

                    /*
                    ** Also the case for Message Action types
                    */
                    case HS_EMT_ACT_NOACT:
                    default:
                        /* 
                        ** Check to see if this is a Message Action Type
                        */
                        if((HS_AppData.MsgActsState == HS_STATE_ENABLED) &&
                           (ActionType > HS_EMT_ACT_LAST_NONMSG) &&
                           (ActionType <= (HS_EMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES)))
                        {
                            MsgActsIndex = ActionType - HS_EMT_ACT_LAST_NONMSG - 1;

                            /*
                            ** Send the message if off cooldown and not disabled
                            */
                            if((HS_AppData.MsgActCooldown[MsgActsIndex] == 0) &&
                               (HS_AppData.MATablePtr[MsgActsIndex].EnableState != HS_MAT_STATE_DISABLED))
                            {
                                CFE_SB_SendMsg((CFE_SB_Msg_t *) HS_AppData.MATablePtr[MsgActsIndex].Message);
                                HS_AppData.MsgActExec++;
                                HS_AppData.MsgActCooldown[MsgActsIndex] = HS_AppData.MATablePtr[MsgActsIndex].Cooldown;
                                if(HS_AppData.MATablePtr[MsgActsIndex].EnableState != HS_MAT_STATE_NOEVENT)
                                {
                                    CFE_EVS_SendEvent(HS_EVENTMON_MSGACTS_ERR_EID, CFE_EVS_ERROR,
                                       "Event Monitor: APP:(%s) EID:(%d): Action: Message Action Index: %d",
                                       HS_AppData.EMTablePtr[TableIndex].AppName,
                                       HS_AppData.EMTablePtr[TableIndex].EventID, (int)MsgActsIndex);
                                }

                            }
                        }

                        /* Otherwise, Take No Action */
                        break;
                } /* end switch */

            } /* end AppName comparison */

        } /* end EventID comparison */

    } /* end for loop */

    return;

} /* end HS_MonitorEvent */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Monitor CPU Utilization and Hogging                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_MonitorUtilization(void)
{
    int32 CurrentUtil;
    uint32 UtilIndex;
    uint32 CombinedUtil = 0;
    uint32 PeakUtil = 0;
    uint32 ThisUtilIndex = HS_AppData.CurrentCPUUtilIndex;

    HS_AppData.CurrentCPUUtilIndex++;

    if(HS_AppData.CurrentCPUUtilIndex >= HS_UTIL_PEAK_NUM_INTERVAL)
    {
        HS_AppData.CurrentCPUUtilIndex = 0;
    }

    CurrentUtil = HS_CustomGetUtil();

    if (CurrentUtil > HS_UTIL_PER_INTERVAL_TOTAL)
    {
        CurrentUtil = HS_UTIL_PER_INTERVAL_TOTAL;
    }
    else if (CurrentUtil < 0)
    {
        CurrentUtil = 0;
    }


    if ((CurrentUtil >= HS_UTIL_PER_INTERVAL_HOGGING) &&
        (HS_AppData.CurrentCPUHogState == HS_STATE_ENABLED))
    {
        HS_AppData.CurrentCPUHoggingTime++;

        if (HS_AppData.CurrentCPUHoggingTime == HS_AppData.MaxCPUHoggingTime)
        {
            CFE_EVS_SendEvent(HS_CPUMON_HOGGING_ERR_EID, CFE_EVS_ERROR, "CPU Hogging Detected");
            CFE_ES_WriteToSysLog("HS App: CPU Hogging Detected\n");
        }
    }
    else
    {
        HS_AppData.CurrentCPUHoggingTime = 0;
    }

    HS_AppData.UtilizationTracker[ThisUtilIndex] = CurrentUtil;

    for(UtilIndex = 0; UtilIndex < HS_UTIL_PEAK_NUM_INTERVAL; UtilIndex++)
    {
        if (HS_AppData.UtilizationTracker[UtilIndex] > PeakUtil)
        {
            PeakUtil = HS_AppData.UtilizationTracker[UtilIndex];
        }

        if (ThisUtilIndex >= HS_UTIL_AVERAGE_NUM_INTERVAL)
        {
            if ((UtilIndex >  (ThisUtilIndex - HS_UTIL_AVERAGE_NUM_INTERVAL)) &&
                (UtilIndex <=  ThisUtilIndex))
            {
                CombinedUtil += HS_AppData.UtilizationTracker[UtilIndex];
            }
        }
        else
        {
            if (UtilIndex <= ThisUtilIndex)
            {
                CombinedUtil += HS_AppData.UtilizationTracker[UtilIndex];
            }
            else if (UtilIndex > (HS_UTIL_PEAK_NUM_INTERVAL - (HS_UTIL_AVERAGE_NUM_INTERVAL - ThisUtilIndex)))
            {
                CombinedUtil += HS_AppData.UtilizationTracker[UtilIndex];
            }

        }

    }

    HS_AppData.UtilCpuAvg  = (CombinedUtil / HS_UTIL_AVERAGE_NUM_INTERVAL);
    HS_AppData.UtilCpuPeak = PeakUtil;

    return;

} /* end HS_MonitorUtilization */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the Application Monitor Table                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ValidateAMTable(void *TableData)
{
    HS_AMTEntry_t *TableArray = (HS_AMTEntry_t *) TableData;

    int32 TableResult = CFE_SUCCESS;
    uint32 TableIndex;
    int32 EntryResult;

    uint16 ActionType;
    uint16 CycleCount;
    uint16 NullTerm;

    uint32 GoodCount   = 0;
    uint32 BadCount    = 0;
    uint32 UnusedCount = 0;
    char BadName[OS_MAX_API_NAME] = "";

    for (TableIndex = 0; TableIndex < HS_MAX_MONITORED_APPS; TableIndex++ )
    {

        ActionType = TableArray[TableIndex].ActionType;
        CycleCount = TableArray[TableIndex].CycleCount;
        NullTerm   = TableArray[TableIndex].NullTerm;
        EntryResult = HS_AMTVAL_NO_ERR;

        if ((CycleCount == 0) ||
            (ActionType == HS_AMT_ACT_NOACT))
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if (NullTerm != 0)   
        {
            /*
            ** Null Terminator Safety Buffer is not Null
            */
            EntryResult = HS_AMTVAL_ERR_NUL;
            BadCount++;
        }
        else if(ActionType > (HS_AMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES))
        {
            /*
            ** Action Type is not valid
            */
            EntryResult = HS_AMTVAL_ERR_ACT;
            BadCount++;
        }
        else
        {
            /*
            ** Otherwise, this entry is good
            */
            GoodCount++;
        }
        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != HS_AMTVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            strncpy(BadName,TableArray[TableIndex].AppName,OS_MAX_API_NAME);
            BadName[OS_MAX_API_NAME-1] = '\0'; 
            CFE_EVS_SendEvent(HS_AMTVAL_ERR_EID, CFE_EVS_ERROR,
                    "AppMon verify err: Entry = %d, Err = %d, Action = %d, App = %s",
                    (int)TableIndex, (int)EntryResult, ActionType, BadName );
            TableResult = EntryResult;
        }

    }

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION,
                     "AppMon verify results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount, (int)BadCount, (int)UnusedCount);

    return(TableResult);

} /* end HS_ValidateAMTable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the Event Monitor Table                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ValidateEMTable(void *TableData)
{
    HS_EMTEntry_t *TableArray = (HS_EMTEntry_t *) TableData;

    int32  TableResult = CFE_SUCCESS;
    uint32 TableIndex;
    int32  EntryResult;

    uint16 ActionType;
    uint16 EventID;
    uint16 NullTerm;

    uint32 GoodCount   = 0;
    uint32 BadCount    = 0;
    uint32 UnusedCount = 0;
    char BadName[OS_MAX_API_NAME] = "";

    for (TableIndex = 0; TableIndex < HS_MAX_MONITORED_EVENTS; TableIndex++ )
    {

        ActionType = TableArray[TableIndex].ActionType;
        EventID    = TableArray[TableIndex].EventID;
        NullTerm   = TableArray[TableIndex].NullTerm;
        EntryResult = HS_EMTVAL_NO_ERR;

        if ((EventID == 0) ||
            (ActionType == HS_EMT_ACT_NOACT))
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if (NullTerm != 0)   
        {
            /*
            ** Null Terminator Safety Buffer is not Null
            */
            EntryResult = HS_EMTVAL_ERR_NUL;
            BadCount++;
        }
        else if(ActionType > (HS_EMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES))
        {
            /*
            ** Action Type is not valid
            */
            EntryResult = HS_EMTVAL_ERR_ACT;
            BadCount++;
        }
        else
        {
            /*
            ** Otherwise, this entry is good
            */
            GoodCount++;
        }
        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != HS_EMTVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            strncpy(BadName,TableArray[TableIndex].AppName,OS_MAX_API_NAME);
            BadName[OS_MAX_API_NAME-1] = '\0'; 
            CFE_EVS_SendEvent(HS_EMTVAL_ERR_EID, CFE_EVS_ERROR,
                    "EventMon verify err: Entry = %d, Err = %d, Action = %d, ID = %d App = %s",
                    (int)TableIndex, (int)EntryResult, ActionType, EventID, BadName );
            TableResult = EntryResult;
        }

    }

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION,
                     "EventMon verify results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount, (int)BadCount, (int)UnusedCount);

    return(TableResult);

} /* end HS_ValidateEMTable */

#if HS_MAX_EXEC_CNT_SLOTS != 0
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the Execution Counters Table                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ValidateXCTable(void *TableData)
{
    HS_XCTEntry_t *TableArray = (HS_XCTEntry_t *) TableData;

    int32  TableResult = CFE_SUCCESS;
    uint32 TableIndex;
    int32  EntryResult;

    uint16 ResourceType;
    uint32 NullTerm;

    uint32 GoodCount   = 0;
    uint32 BadCount    = 0;
    uint32 UnusedCount = 0;
    char BadName[OS_MAX_API_NAME] = "";

    for (TableIndex = 0; TableIndex < HS_MAX_EXEC_CNT_SLOTS; TableIndex++ )
    {

        ResourceType = TableArray[TableIndex].ResourceType;
        NullTerm = TableArray[TableIndex].NullTerm;
        EntryResult = HS_XCTVAL_NO_ERR;


        if (ResourceType == HS_XCT_TYPE_NOTYPE)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if (NullTerm != 0)   
        {
            /*
            ** Null Terminator Safety Buffer is not Null
            */
            EntryResult = HS_XCTVAL_ERR_NUL;
            BadCount++;
        }
        else if((ResourceType != HS_XCT_TYPE_APP_MAIN)  &&
                (ResourceType != HS_XCT_TYPE_APP_CHILD)  &&
                (ResourceType != HS_XCT_TYPE_DEVICE)  &&
                (ResourceType != HS_XCT_TYPE_ISR))
        {
            /*
            ** Resource Type is not valid
            */
            EntryResult = HS_XCTVAL_ERR_TYPE;
            BadCount++;
        }
        else
        {
            /*
            ** Otherwise, this entry is good
            */
            GoodCount++;
        }

        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != HS_XCTVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            strncpy(BadName,TableArray[TableIndex].ResourceName,OS_MAX_API_NAME);
            BadName[OS_MAX_API_NAME-1] = '\0'; 
            CFE_EVS_SendEvent(HS_XCTVAL_ERR_EID, CFE_EVS_ERROR,
                    "ExeCount verify err: Entry = %d, Err = %d, Type = %d, Name = %s",
                    (int)TableIndex, (int)EntryResult, ResourceType, BadName );
            TableResult = EntryResult;
        }

    }

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(HS_XCTVAL_INF_EID, CFE_EVS_INFORMATION,
                     "ExeCount verify results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount, (int)BadCount, (int)UnusedCount);

    return(TableResult);

} /* end HS_ValidateXCTable */
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate the Message Actions Table                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 HS_ValidateMATable(void *TableData)
{
    HS_MATEntry_t *TableArray = (HS_MATEntry_t *) TableData;

    int32  TableResult = CFE_SUCCESS;
    uint32 TableIndex;
    uint16 Length;
    uint16 MessageID;
    uint16 EnableState;
    int32  EntryResult;

    CFE_SB_MsgPtr_t Msg;

    uint32 GoodCount   = 0;
    uint32 BadCount    = 0;
    uint32 UnusedCount = 0;

    for (TableIndex = 0; TableIndex < HS_MAX_MSG_ACT_TYPES; TableIndex++ )
    {

        EntryResult = HS_MATVAL_NO_ERR;
        Msg = (CFE_SB_MsgPtr_t) TableArray[TableIndex].Message;
        Length = CFE_SB_GetTotalMsgLength(Msg);
        MessageID = CFE_SB_GetMsgId(Msg);
        EnableState = TableArray[TableIndex].EnableState;

        if(EnableState == HS_MAT_STATE_DISABLED)
        {
            /*
            ** Unused table entry
            */
            UnusedCount++;
        }
        else if((EnableState != HS_MAT_STATE_ENABLED)  &&
                (EnableState != HS_MAT_STATE_NOEVENT))
        {
            /*
            ** Enable State is Invalid
            */
            EntryResult = HS_MATVAL_ERR_ENA;
            BadCount++;
        }
        else if (MessageID > CFE_SB_HIGHEST_VALID_MSGID)
        {
            /*
            ** Message ID is too high
            */
            EntryResult = HS_MATVAL_ERR_ID;
            BadCount++;
        }
        else if (Length > CFE_SB_MAX_SB_MSG_SIZE)
        {
            /*
            ** Length is too high
            */
            EntryResult = HS_MATVAL_ERR_LEN;
            BadCount++;
        }
        else
        {
            /*
            ** Otherwise, this entry is good
            */
            GoodCount++;
        }
        /*
        ** Generate detailed event for "first" error
        */
        if ((EntryResult != HS_MATVAL_NO_ERR) && (TableResult == CFE_SUCCESS))
        {
            CFE_EVS_SendEvent(HS_MATVAL_ERR_EID, CFE_EVS_ERROR,
                    "MsgActs verify err: Entry = %d, Err = %d, Length = %d, ID = %d",
                    (int)TableIndex, (int)EntryResult, Length, MessageID );
            TableResult = EntryResult;
        }
    }

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION,
                     "MsgActs verify results: good = %d, bad = %d, unused = %d",
                      (int)GoodCount, (int)BadCount, (int)UnusedCount);

    return(TableResult);

} /* end HS_ValidateMATable */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Set the values being stored in the CDS                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void HS_SetCDSData(uint16 ResetsPerformed, uint16 MaxResets)
{
    /*
    ** Set CDS data and verification inverses
    */
    HS_AppData.CDSData.ResetsPerformed = ResetsPerformed;
    HS_AppData.CDSData.ResetsPerformedNot = ~HS_AppData.CDSData.ResetsPerformed;
    HS_AppData.CDSData.MaxResets = MaxResets;
    HS_AppData.CDSData.MaxResetsNot = ~HS_AppData.CDSData.MaxResets;
    /*
    ** Copy the data to the CDS if CDS Creation was successful
    */
    if(HS_AppData.CDSState == HS_STATE_ENABLED)
    {
        CFE_ES_CopyToCDS(HS_AppData.MyCDSHandle,&HS_AppData.CDSData);
    }
    return;

} /* end HS_SetCDSData */

/************************/
/*  End of File Comment */
/************************/
