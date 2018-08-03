 /*************************************************************************
 ** File:
 **   $Id: hs_monitors_test.c 1.4 2016/09/07 19:17:18EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hs_monitors.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_monitors_test.c  $
 **   Revision 1.4 2016/09/07 19:17:18EDT mdeschu 
 **   Update unit test asserts to match HS updates
 **   
 **   HS_MAX_CRITICAL_APPS/EVENTS -> HS_MAX_MONITORED_APPS/EVENTS
 **   Removal of "Critical" from certain event messages.
 **   Revision 1.3 2016/08/25 20:59:18EDT czogby 
 **   Improved readability of comments
 **   Revision 1.2 2016/08/19 14:07:05EDT czogby 
 **   HS UT-Assert Unit Tests - Code Walkthrough Updates
 **   Revision 1.1 2016/06/24 14:31:53EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hs_monitors_test.h"
#include "hs_app.h"
#include "hs_monitors.h"
#include "hs_custom.h"
#include "hs_msg.h"
#include "hs_msgdefs.h"
#include "hs_msgids.h"
#include "hs_events.h"
#include "hs_version.h"
#include "hs_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_watchdog_stubs.h"
#include "ut_cfe_psp_timer_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

int32 HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    AppInfo->ExecutionCounter = 3;

    return CFE_SUCCESS;
}

void HS_MonitorApplications_Test_AppNameNotFound(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = -1;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Set CFE_ES_GetAppIDByName to fail on first call, to generate error HS_APPMON_APPNAME_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_APPNAME_ERR_EID, CFE_EVS_ERROR, "App Monitor App Name not found: APP:(AppName)"),
        "App Monitor App Name not found: APP:(AppName)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorApplications_Test_AppNameNotFound */

void HS_MonitorApplications_Test_GetExeCountFailure(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = -1;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Causes "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount = 2;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 2, "HS_AppData.AppMonCheckInCountdown[0] == 2");
    UtAssert_True (HS_AppData.AppMonLastExeCount[0] == 3, "HS_AppData.AppMonLastExeCount[0] == 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorApplications_Test_GetExeCountFailure */

void HS_MonitorApplications_Test_ProcessorResetError(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_PROC_RESET;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.CDSData.MaxResets = 10;
    HS_AppData.CDSData.ResetsPerformed = 1;
    HS_AppData.AppMonEnables[0] = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");
    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_DISABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_PROC_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName): Action: Processor Reset"),
        "App Monitor Failure: APP:(AppName): Action: Processor Reset");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: App Monitor Failure: APP:(AppName): Action: Processor Reset\n"),
        "HS App: App Monitor Failure: APP:(AppName): Action: Processor Reset");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end HS_MonitorApplications_Test_ProcessorResetError */

void HS_MonitorApplications_Test_ProcessorResetActionLimitError(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_PROC_RESET;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.CDSData.MaxResets = 10;
    HS_AppData.CDSData.ResetsPerformed = 11;
    HS_AppData.AppMonEnables[0] = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_PROC_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName): Action: Processor Reset"),
        "App Monitor Failure: APP:(AppName): Action: Processor Reset");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_RESET_LIMIT_ERR_EID, CFE_EVS_ERROR, "Processor Reset Action Limit Reached: No Reset Performed"),
        "Processor Reset Action Limit Reached: No Reset Performed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorApplications_Test_ProcessorResetActionLimitError */

void HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoSuccess(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_APP_RESTART;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Set CFE_ES_RestartApp to fail on first call, to generate error HS_APPMON_NOT_RESTARTED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RESTARTAPP_INDEX, 0xFFFFFFFF, 1);


    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.AppMonEnables[0] = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_RESTART_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName) Action: Restart Application"),
        "App Monitor Failure: APP:(AppName) Action: Restart Application");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR, "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoSuccess */

void HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoNotSuccess(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_APP_RESTART;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Set CFE_ES_GetAppInfo to fail on first call, to generate error HS_APPMON_NOT_RESTARTED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPINFO_INDEX, -1, 1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.AppMonEnables[0] = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_RESTART_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName) Action: Restart Application"),
        "App Monitor Failure: APP:(AppName) Action: Restart Application");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR, "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoNotSuccess */

void HS_MonitorApplications_Test_FailError(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_EVENT;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.AppMonEnables[0] = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_FAIL_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName): Action: Event Only"),
        "App Monitor Failure: APP:(AppName): Action: Event Only");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorApplications_Test_FailError */

void HS_MonitorApplications_Test_MsgActsNOACT(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[0].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&HS_AppData.MATablePtr[0].Message, HS_NOOP_CC);

    HS_AppData.AMTablePtr = AMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_NOACT; /* Causes most of the function to be skipped, due to first if-statement */
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;
    HS_AppData.MsgActsState = HS_STATE_ENABLED;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.AppMonEnables[0] = 1;
    HS_AppData.MsgActCooldown[0] = 0;  /* (HS_AMT_ACT_LAST_NONMSG + 1) - HS_AMT_ACT_LAST_NONMSG - 1 = 0 */
    HS_AppData.MATablePtr[0].EnableState = HS_MAT_STATE_ENABLED;
    HS_AppData.MATablePtr[0].Cooldown = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorApplications_Test_MsgActsNOACT */

void HS_MonitorApplications_Test_MsgActsErrorDefault(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[0].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&HS_AppData.MATablePtr[0].Message, HS_NOOP_CC);

    HS_AppData.AMTablePtr = AMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.AMTablePtr[0].ActionType  = HS_AMT_ACT_LAST_NONMSG + 1;
    HS_AppData.AppMonCheckInCountdown[0] = 1;
    HS_AppData.AMTablePtr[0].CycleCount  = 1;
    HS_AppData.MsgActsState = HS_STATE_ENABLED;

    strncpy (HS_AppData.AMTablePtr[0].AppName, "AppName", 10);

    /* Prevents "failure to get an execution counter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &HS_MONITORS_TEST_CFE_ES_GetAppInfoHook1);

    HS_AppData.AppMonLastExeCount[0] = 3;

    HS_AppData.AppMonEnables[0] = 1;
    HS_AppData.MsgActCooldown[0] = 0;  /* (HS_AMT_ACT_LAST_NONMSG + 1) - HS_AMT_ACT_LAST_NONMSG - 1 = 0 */
    HS_AppData.MATablePtr[0].EnableState = HS_MAT_STATE_ENABLED;
    HS_AppData.MATablePtr[0].Cooldown = 1;

    /* Execute the function being tested */
    HS_MonitorApplications();
    
    /* Verify results */
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0, "HS_AppData.AppMonEnables[0] == 0");

    UtAssert_True (HS_AppData.MsgActExec = 1, "HS_AppData.MsgActExec = 1");
    UtAssert_True (HS_AppData.MsgActCooldown[0] == 1, "HS_AppData.MsgActCooldown[0] == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_MSGACTS_ERR_EID, CFE_EVS_ERROR, "App Monitor Failure: APP:(AppName): Action: Message Action Index: 0"),
        "App Monitor Failure: APP:(AppName): Action: Message Action Index: 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorApplications_Test_MsgActsErrorDefault */

void HS_MonitorEvent_Test_ProcErrorReset(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_PROC_RESET;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;
    HS_AppData.CDSData.MaxResets = 10;
    HS_AppData.CDSData.ResetsPerformed = 1;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_PROC_ERR_EID, CFE_EVS_ERROR, "Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset"),
        "Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset\n"),
        "HS App: Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_DISABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_DISABLED");

} /* end HS_MonitorEvent_Test_ProcErrorReset */

void HS_MonitorEvent_Test_ProcErrorNoReset(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_PROC_RESET;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;
    HS_AppData.CDSData.MaxResets = 10;
    HS_AppData.CDSData.ResetsPerformed = 11;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_PROC_ERR_EID, CFE_EVS_ERROR, "Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset"),
        "Event Monitor: APP:(AppName) EID:(3): Action: Processor Reset");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_RESET_LIMIT_ERR_EID, CFE_EVS_ERROR, "Processor Reset Action Limit Reached: No Reset Performed"),
        "Processor Reset Action Limit Reached: No Reset Performed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorEvent_Test_ProcErrorNoReset */

void HS_MonitorEvent_Test_AppRestartErrors(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_APP_RESTART;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Set CFE_ES_RestartApp to return -1, in order to generate error message HS_EVENTMON_NOT_RESTARTED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RESTARTAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_RESTART_ERR_EID, CFE_EVS_ERROR, "Event Monitor: APP:(AppName) EID:(3): Action: Restart Application"),
        "Event Monitor: APP:(AppName) EID:(3): Action: Restart Application");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR, "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorEvent_Test_AppRestartErrors */

void HS_MonitorEvent_Test_OnlySecondAppRestartError(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_APP_RESTART;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Set CFE_ES_GetAppIDByName to return -1, in order to generate error message HS_EVENTMON_NOT_RESTARTED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_NOT_RESTARTED_ERR_EID, CFE_EVS_ERROR, "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Restart App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorEvent_Test_OnlySecondAppRestartError */

void HS_MonitorEvent_Test_DeleteErrors(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_APP_DELETE;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Set CFE_ES_DeleteApp to return -1, in order to generate error message HS_EVENTMON_NOT_DELETED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_DELETEAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_DELETE_ERR_EID, CFE_EVS_ERROR, "Event Monitor: APP:(AppName) EID:(3): Action: Delete Application"),
        "Event Monitor: APP:(AppName) EID:(3): Action: Delete Application");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_NOT_DELETED_ERR_EID, CFE_EVS_ERROR, "Call to Delete App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Delete App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_MonitorEvent_Test_DeleteErrors */

void HS_MonitorEvent_Test_OnlySecondDeleteError(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_EMT_ACT_APP_DELETE;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Set CFE_ES_GetAppIDByName to fail on first call, to generate error HS_EVENTMON_NOT_DELETED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_NOT_DELETED_ERR_EID, CFE_EVS_ERROR, "Call to Delete App Failed: APP:(AppName) ERR: 0xFFFFFFFF"),
        "Call to Delete App Failed: APP:(AppName) ERR: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorEvent_Test_OnlySecondDeleteError */

void HS_MonitorEvent_Test_MsgActsError(void)
{
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_APPS];
    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];
    CFE_EVS_Packet_t  Packet;

    HS_AppData.MATablePtr = &MATable[0];

    CFE_SB_InitMsg (&Packet, HS_CMD_MID, sizeof(CFE_EVS_Packet_t), TRUE);

    Packet.Payload.PacketID.EventID = 3;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.MATablePtr = MATable;

    HS_AppData.EMTablePtr[0].ActionType  = HS_AMT_ACT_LAST_NONMSG + 1;
    HS_AppData.EMTablePtr[0].EventID = Packet.Payload.PacketID.EventID;

    strncpy (HS_AppData.EMTablePtr[0].AppName, "AppName", 10);
    strncpy (Packet.Payload.PacketID.AppName, "AppName", 10);

    /* Set CFE_ES_DeleteApp to return -1, in order to generate error message HS_EVENTMON_NOT_DELETED_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_DELETEAPP_INDEX, -1, 1);

    HS_AppData.MsgActsState = HS_STATE_ENABLED;
    HS_AppData.MsgActCooldown[0] = 0;
    HS_AppData.MATablePtr[0].EnableState = HS_MAT_STATE_ENABLED;
    HS_AppData.MATablePtr[0].Cooldown = 5;

    /* Execute the function being tested */
    HS_MonitorEvent((CFE_SB_MsgPtr_t)&Packet);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_MSGACTS_ERR_EID, CFE_EVS_ERROR, "Event Monitor: APP:(AppName) EID:(3): Action: Message Action Index: 0"),
        "Event Monitor: APP:(AppName) EID:(3): Action: Message Action Index: 0");

    UtAssert_True (HS_AppData.MsgActExec == 1, "HS_AppData.MsgActExec == 1");
    UtAssert_True (HS_AppData.MsgActCooldown[0] == 5, "HS_AppData.MsgActCooldown[0] == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_MonitorEvent_Test_MsgActsError */

void HS_MonitorUtilization_Test_HighCurrentUtil(void)
{
    HS_CustomData.LastIdleTaskInterval = 1;
    HS_CustomData.UtilMult1 = -3;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUUtilIndex = HS_UTIL_PEAK_NUM_INTERVAL - 2;

    /* Execute the function being tested */
    HS_MonitorUtilization();

    /* Verify results */
    UtAssert_True
        (HS_AppData.UtilizationTracker[HS_AppData.CurrentCPUUtilIndex - 1] == HS_UTIL_PER_INTERVAL_TOTAL,
         "HS_AppData.UtilizationTracker[HS_AppData.CurrentCPUUtilIndex - 1] == HS_UTIL_PER_INTERVAL_TOTAL");
    /* For this test case, we don't care about any messages or variables changed after this is set */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorUtilization_Test_HighCurrentUtil */

void HS_MonitorUtilization_Test_CurrentUtilLessThanZero(void)
{
    HS_CustomData.LastIdleTaskInterval = 1;
    HS_CustomData.UtilMult1 = HS_UTIL_PER_INTERVAL_TOTAL + 1;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUUtilIndex = 0;

    /* Execute the function being tested */
    HS_MonitorUtilization();

    /* Verify results */
    UtAssert_True
        (HS_AppData.UtilizationTracker[HS_AppData.CurrentCPUUtilIndex - 1] == 0,
         "HS_AppData.UtilizationTracker[HS_AppData.CurrentCPUUtilIndex - 1] == 0");
    /* For this test case, we don't care about any messages or variables changed after this is set */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorUtilization_Test_CurrentUtilLessThanZero */

void HS_MonitorUtilization_Test_CPUHogging(void)
{
    HS_CustomData.LastIdleTaskInterval = 0;
    HS_CustomData.UtilMult1 = 1;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUHogState = HS_STATE_ENABLED;
    HS_AppData.MaxCPUHoggingTime  = 1;

    HS_AppData.CurrentCPUUtilIndex = HS_UTIL_PEAK_NUM_INTERVAL;

    /* Execute the function being tested */
    HS_MonitorUtilization();
    
    /* Verify results */
    UtAssert_True (HS_AppData.CurrentCPUHoggingTime == 1, "HS_AppData.CurrentCPUHoggingTime == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CPUMON_HOGGING_ERR_EID, CFE_EVS_ERROR, "CPU Hogging Detected"),
        "CPU Hogging Detected");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: CPU Hogging Detected\n"),
        "HS App: CPU Hogging Detected");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

    /* For this test case, we don't care about any variables changed after this message */

} /* end HS_MonitorUtilization_Test_CPUHogging */

void HS_MonitorUtilization_Test_CurrentCPUHogStateDisabled(void)
{
    HS_CustomData.LastIdleTaskInterval = 0;
    HS_CustomData.UtilMult1 = 1;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUHogState = HS_STATE_DISABLED;
    HS_AppData.MaxCPUHoggingTime  = 1;

    HS_AppData.CurrentCPUUtilIndex = HS_UTIL_PEAK_NUM_INTERVAL;

    /* Execute the function being tested */
    HS_MonitorUtilization();
    
    /* Verify results */
    UtAssert_True (HS_AppData.CurrentCPUHoggingTime == 0, "HS_AppData.CurrentCPUHoggingTime == 0");
    /* For this test case, we don't care about any variables changed after this variable is set */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorUtilization_Test_CurrentCPUHogStateDisabled */

void HS_MonitorUtilization_Test_HighUtilIndex(void)
{
    HS_CustomData.LastIdleTaskInterval = 0;
    HS_CustomData.UtilMult1 = 1;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUHogState = HS_STATE_DISABLED;
    HS_AppData.MaxCPUHoggingTime  = 1;

    HS_AppData.CurrentCPUUtilIndex = HS_UTIL_PEAK_NUM_INTERVAL - 1;

    /* Execute the function being tested */
    HS_MonitorUtilization();
    
    /* Verify results */
    UtAssert_True (HS_AppData.CurrentCPUHoggingTime == 0, "HS_AppData.CurrentCPUHoggingTime == 0");
    UtAssert_True (HS_AppData.UtilCpuAvg  == (HS_UTIL_PER_INTERVAL_TOTAL / HS_UTIL_AVERAGE_NUM_INTERVAL) , "HS_AppData.UtilCpuAvg  == (HS_UTIL_PER_INTERVAL_TOTAL / HS_UTIL_AVERAGE_NUM_INTERVAL)");
    UtAssert_True (HS_AppData.UtilCpuPeak == HS_UTIL_PER_INTERVAL_TOTAL, "HS_AppData.UtilCpuPeak == HS_UTIL_PER_INTERVAL_TOTAL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorUtilization_Test_HighUtilIndex */

void HS_MonitorUtilization_Test_LowUtilIndex(void)
{
    HS_CustomData.LastIdleTaskInterval = 0;
    HS_CustomData.UtilMult1 = 1;
    HS_CustomData.UtilMult2 = 1;
    HS_CustomData.UtilDiv   = 1;

    HS_AppData.CurrentCPUHogState = HS_STATE_DISABLED;
    HS_AppData.MaxCPUHoggingTime  = 1;

    HS_AppData.CurrentCPUUtilIndex = 1;

    /* Execute the function being tested */
    HS_MonitorUtilization();
    
    /* Verify results */
    UtAssert_True (HS_AppData.CurrentCPUHoggingTime == 0, "HS_AppData.CurrentCPUHoggingTime == 0");
    UtAssert_True (HS_AppData.UtilCpuAvg  == (HS_UTIL_PER_INTERVAL_TOTAL / HS_UTIL_AVERAGE_NUM_INTERVAL) , "HS_AppData.UtilCpuAvg  == (HS_UTIL_PER_INTERVAL_TOTAL / HS_UTIL_AVERAGE_NUM_INTERVAL)");
    UtAssert_True (HS_AppData.UtilCpuPeak == HS_UTIL_PER_INTERVAL_TOTAL, "HS_AppData.UtilCpuPeak == HS_UTIL_PER_INTERVAL_TOTAL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MonitorUtilization_Test_LowUtilIndex */

void HS_ValidateAMTable_Test_UnusedTableEntryCycleCountZero(void)
{
    int32   Result;
    uint32  i;

    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    for (i=0; i < HS_MAX_MONITORED_APPS; i++)
    {
        HS_AppData.AMTablePtr[i].ActionType  = 99;
        HS_AppData.AMTablePtr[i].CycleCount  = 0;
        HS_AppData.AMTablePtr[i].NullTerm    = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateAMTable(HS_AppData.AMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION, "AppMon verify results: good = 0, bad = 0, unused = 32"),
        "AppMon verify results: good = 0, bad = 0, unused = 32");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateAMTable_Test_UnusedTableEntryCycleCountZero */

void HS_ValidateAMTable_Test_UnusedTableEntryActionTypeNOACT(void)
{
    int32   Result;
    uint32  i;

    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    for (i=0; i < HS_MAX_MONITORED_APPS; i++)
    {
        HS_AppData.AMTablePtr[i].ActionType  = HS_EMT_ACT_NOACT;
        HS_AppData.AMTablePtr[i].CycleCount  = 1;
        HS_AppData.AMTablePtr[i].NullTerm    = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateAMTable(HS_AppData.AMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION, "AppMon verify results: good = 0, bad = 0, unused = 32"),
        "AppMon verify results: good = 0, bad = 0, unused = 32");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateAMTable_Test_UnusedTableEntryActionTypeNOACT */

void HS_ValidateAMTable_Test_BufferNotNull(void)
{
    int32   Result;
    uint32  i;

    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    for (i=0; i < HS_MAX_MONITORED_APPS; i++)
    {
        HS_AppData.AMTablePtr[i].ActionType  = 99;
        HS_AppData.AMTablePtr[i].CycleCount  = 1;
        HS_AppData.AMTablePtr[i].NullTerm    = 2;
    }

    strncpy(HS_AppData.AMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateAMTable(HS_AppData.AMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_ERR_EID, CFE_EVS_ERROR, "AppMon verify err: Entry = 0, Err = -2, Action = 99, App = AppName"),
        "AppMon verify err: Entry = 0, Err = -2, Action = 99, App = AppName");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION, "AppMon verify results: good = 0, bad = 32, unused = 0"),
        "AppMon verify results: good = 0, bad = 32, unused = 0");

    UtAssert_True(Result == HS_AMTVAL_ERR_NUL, "Result == HS_AMTVAL_ERR_NUL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateAMTable_Test_BufferNotNull */

void HS_ValidateAMTable_Test_ActionTypeNotValid(void)
{
    int32   Result;
    uint32  i;

    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    for (i=0; i < HS_MAX_MONITORED_APPS; i++)
    {
        HS_AppData.AMTablePtr[i].ActionType  = HS_AMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES + 1;
        HS_AppData.AMTablePtr[i].CycleCount  = 1;
        HS_AppData.AMTablePtr[i].NullTerm    = 0;
    }

    strncpy(HS_AppData.AMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateAMTable(HS_AppData.AMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION, "AppMon verify results: good = 0, bad = 32, unused = 0"),
        "AppMon verify results: good = 0, bad = 32, unused = 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_ERR_EID, CFE_EVS_ERROR, "AppMon verify err: Entry = 0, Err = -1, Action = 12, App = AppName"),
        "AppMon verify err: Entry = 0, Err = -1, Action = 12, App = AppName");

    UtAssert_True(Result == HS_AMTVAL_ERR_ACT, "Result == HS_AMTVAL_ERR_ACT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateAMTable_Test_ActionTypeNotValid */

void HS_ValidateAMTable_Test_EntryGood(void)
{
    int32   Result;
    uint32  i;

    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    for (i=0; i < HS_MAX_MONITORED_APPS; i++)
    {
        HS_AppData.AMTablePtr[i].ActionType  = HS_AMT_ACT_LAST_NONMSG;
        HS_AppData.AMTablePtr[i].CycleCount  = 1;
        HS_AppData.AMTablePtr[i].NullTerm    = 0;
    }

    strncpy(HS_AppData.AMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateAMTable(HS_AppData.AMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMTVAL_INF_EID, CFE_EVS_INFORMATION, "AppMon verify results: good = 32, bad = 0, unused = 0"),
        "AppMon verify results: good = 32, bad = 0, unused = 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateAMTable_Test_EntryGood */

void HS_ValidateEMTable_Test_UnusedTableEntryEventIDZero(void)
{
    int32   Result;
    uint32  i;

    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    for (i=0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].ActionType  = 99;
        HS_AppData.EMTablePtr[i].EventID     = 0;
        HS_AppData.EMTablePtr[i].NullTerm    = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateEMTable(HS_AppData.EMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION, "EventMon verify results: good = 0, bad = 0, unused = 16"),
        "EventMon verify results: good = 0, bad = 0, unused = 16");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateEMTable_Test_UnusedTableEntryEventIDZero */

void HS_ValidateEMTable_Test_UnusedTableEntryActionTypeNOACT(void)
{
    int32   Result;
    uint32  i;

    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    for (i=0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].ActionType  = HS_EMT_ACT_NOACT;
        HS_AppData.EMTablePtr[i].EventID     = 1;
        HS_AppData.EMTablePtr[i].NullTerm    = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateEMTable(HS_AppData.EMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION, "EventMon verify results: good = 0, bad = 0, unused = 16"),
        "EventMon verify results: good = 0, bad = 0, unused = 16");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateEMTable_Test_UnusedTableEntryActionTypeNOACT */

void HS_ValidateEMTable_Test_BufferNotNull(void)
{
    int32   Result;
    uint32  i;

    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    for (i=0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].ActionType  = 99;
        HS_AppData.EMTablePtr[i].EventID     = 1;
        HS_AppData.EMTablePtr[i].NullTerm    = 2;
    }

    strncpy(HS_AppData.EMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateEMTable(HS_AppData.EMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_ERR_EID, CFE_EVS_ERROR, "EventMon verify err: Entry = 0, Err = -2, Action = 99, ID = 1 App = AppName"),
        "EventMon verify err: Entry = 0, Err = -2, Action = 99, ID = 1 App = AppName");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION, "EventMon verify results: good = 0, bad = 16, unused = 0"),
        "EventMon verify results: good = 0, bad = 16, unused = 0");

    UtAssert_True(Result == HS_EMTVAL_ERR_NUL, "Result == HS_EMTVAL_ERR_NUL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateEMTable_Test_BufferNotNull */

void HS_ValidateEMTable_Test_ActionTypeNotValid(void)
{
    int32   Result;
    uint32  i;

    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    for (i=0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].ActionType  = HS_EMT_ACT_LAST_NONMSG + HS_MAX_MSG_ACT_TYPES + 1;
        HS_AppData.EMTablePtr[i].EventID     = 1;
        HS_AppData.EMTablePtr[i].NullTerm    = 0;
    }

    strncpy(HS_AppData.EMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateEMTable(HS_AppData.EMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION, "EventMon verify results: good = 0, bad = 16, unused = 0"),
        "EventMon verify results: good = 0, bad = 16, unused = 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_ERR_EID, CFE_EVS_ERROR, "EventMon verify err: Entry = 0, Err = -1, Action = 12, ID = 1 App = AppName"),
        "EventMon verify err: Entry = 0, Err = -1, Action = 12, ID = 1 App = AppName");

    UtAssert_True(Result == HS_AMTVAL_ERR_ACT, "Result == HS_AMTVAL_ERR_ACT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateEMTable_Test_ActionTypeNotValid */

void HS_ValidateEMTable_Test_EntryGood(void)
{
    int32   Result;
    uint32  i;

    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    for (i=0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].ActionType  = HS_EMT_ACT_LAST_NONMSG;
        HS_AppData.EMTablePtr[i].EventID     = 1;
        HS_AppData.EMTablePtr[i].NullTerm    = 0;
    }

    strncpy(HS_AppData.EMTablePtr[0].AppName, "AppName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateEMTable(HS_AppData.EMTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMTVAL_INF_EID, CFE_EVS_INFORMATION, "EventMon verify results: good = 16, bad = 0, unused = 0"),
        "EventMon verify results: good = 16, bad = 0, unused = 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateEMTable_Test_EntryGood */

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_ValidateXCTable_Test_UnusedTableEntry(void)
{
    int32   Result;
    uint32  i;

    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];

    HS_AppData.XCTablePtr = XCTable;

    for (i=0; i < HS_MAX_EXEC_CNT_SLOTS; i++)
    {
        HS_AppData.XCTablePtr[i].ResourceType  = HS_XCT_TYPE_NOTYPE;
        HS_AppData.XCTablePtr[i].NullTerm      = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateXCTable(HS_AppData.XCTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_INF_EID, CFE_EVS_INFORMATION, "ExeCount verify results: good = 0, bad = 0, unused = 32"),
        "ExeCount verify results: good = 0, bad = 0, unused = 32");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateXCTable_Test_UnusedTableEntry */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_ValidateXCTable_Test_BufferNotNull(void)
{
    int32   Result;
    uint32  i;

    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];

    HS_AppData.XCTablePtr = XCTable;

    for (i=0; i < HS_MAX_EXEC_CNT_SLOTS; i++)
    {
        HS_AppData.XCTablePtr[i].ResourceType  = 99;
        HS_AppData.XCTablePtr[i].NullTerm      = 1;
    }

    strncpy(HS_AppData.XCTablePtr[0].ResourceName, "ResourceName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateXCTable(HS_AppData.XCTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_ERR_EID, CFE_EVS_ERROR, "ExeCount verify err: Entry = 0, Err = -2, Type = 99, Name = ResourceName"),
        "ExeCount verify err: Entry = 0, Err = -2, Type = 99, Name = ResourceName");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_INF_EID, CFE_EVS_INFORMATION, "ExeCount verify results: good = 0, bad = 32, unused = 0"),
        "ExeCount verify results: good = 0, bad = 32, unused = 0");

    UtAssert_True(Result == HS_XCTVAL_ERR_NUL, "Result == HS_XCTVAL_ERR_NUL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateXCTable_Test_BufferNotNull */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_ValidateXCTable_Test_ResourceTypeNotValid(void)
{
    int32   Result;
    uint32  i;

    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];

    HS_AppData.XCTablePtr = XCTable;

    for (i=0; i < HS_MAX_EXEC_CNT_SLOTS; i++)
    {
        HS_AppData.XCTablePtr[i].ResourceType  = 99;
        HS_AppData.XCTablePtr[i].NullTerm      = 0;
    }

    strncpy(HS_AppData.XCTablePtr[0].ResourceName, "ResourceName", OS_MAX_API_NAME);

    /* Execute the function being tested */
    Result = HS_ValidateXCTable(HS_AppData.XCTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_ERR_EID, CFE_EVS_ERROR, "ExeCount verify err: Entry = 0, Err = -1, Type = 99, Name = ResourceName"),
        "ExeCount verify err: Entry = 0, Err = -1, Type = 99, Name = ResourceName");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_INF_EID, CFE_EVS_INFORMATION, "ExeCount verify results: good = 0, bad = 32, unused = 0"),
        "ExeCount verify results: good = 0, bad = 32, unused = 0");

    UtAssert_True(Result == HS_XCTVAL_ERR_TYPE, "Result == HS_XCTVAL_ERR_TYPE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateXCTable_Test_ResourceTypeNotValid */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_ValidateXCTable_Test_EntryGood(void)
{
    int32   Result;
    uint32  i;

    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];

    HS_AppData.XCTablePtr = XCTable;

    for (i=0; i < HS_MAX_EXEC_CNT_SLOTS; i++)
    {
        HS_AppData.XCTablePtr[i].ResourceType  = HS_XCT_TYPE_APP_MAIN;
        HS_AppData.XCTablePtr[i].NullTerm      = 0;
    }

    /* Execute the function being tested */
    Result = HS_ValidateXCTable(HS_AppData.XCTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCTVAL_INF_EID, CFE_EVS_INFORMATION, "ExeCount verify results: good = 32, bad = 0, unused = 0"),
        "ExeCount verify results: good = 32, bad = 0, unused = 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateXCTable_Test_EntryGood */
#endif

void HS_ValidateMATable_Test_UnusedTableEntry(void)
{
    int32   Result;
    uint32  i;

    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = MATable;

    for (i=0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        HS_AppData.MATablePtr[i].EnableState = HS_MAT_STATE_DISABLED;

        CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[i].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    }

    /* Execute the function being tested */
    Result = HS_ValidateMATable(HS_AppData.MATablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION, "MsgActs verify results: good = 0, bad = 0, unused = 8"),
        "MsgActs verify results: good = 0, bad = 0, unused = 8");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateMATable_Test_UnusedTableEntry */

void HS_ValidateMATable_Test_InvalidEnableState(void)
{
    int32   Result;
    uint32  i;

    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = MATable;

    for (i=0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        HS_AppData.MATablePtr[i].EnableState = 99;

        CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[i].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    }

    /* Execute the function being tested */
    Result = HS_ValidateMATable(HS_AppData.MATablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_ERR_EID, CFE_EVS_ERROR, "MsgActs verify err: Entry = 0, Err = -3, Length = 8, ID = 6318"),
        "MsgActs verify err: Entry = 0, Err = -3, Length = 8, ID = 6318");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION, "MsgActs verify results: good = 0, bad = 8, unused = 0"),
        "MsgActs verify results: good = 0, bad = 8, unused = 0");

    UtAssert_True(Result == HS_MATVAL_ERR_ENA, "Result == HS_MATVAL_ERR_ENA");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateMATable_Test_InvalidEnableState */

void HS_ValidateMATable_Test_MessageIDTooHigh(void)
{
    int32   Result;
    uint32  i;

    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = MATable;

    for (i=0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        HS_AppData.MATablePtr[i].EnableState = HS_MAT_STATE_ENABLED;

        CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[i].Message, CFE_SB_HIGHEST_VALID_MSGID + 1, sizeof(HS_NoArgsCmd_t), TRUE);
    }

    /* Execute the function being tested */
    Result = HS_ValidateMATable(HS_AppData.MATablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_ERR_EID, CFE_EVS_ERROR, "MsgActs verify err: Entry = 0, Err = -1, Length = 8, ID = 8192"),
        "MsgActs verify err: Entry = 0, Err = -1, Length = 8, ID = 8192");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION, "MsgActs verify results: good = 0, bad = 8, unused = 0"),
        "MsgActs verify results: good = 0, bad = 8, unused = 0");

    UtAssert_True(Result == HS_MATVAL_ERR_ID, "Result == HS_MATVAL_ERR_ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateMATable_Test_MessageIDTooHigh */

void HS_ValidateMATable_Test_LengthTooHigh(void)
{
    int32   Result;
    uint32  i;

    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = MATable;

    for (i=0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        HS_AppData.MATablePtr[i].EnableState = HS_MAT_STATE_ENABLED;

        CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[i].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    }

    /* Causes Length to be set to satisfy condition (Length > CFE_SB_MAX_SB_MSG_SIZE) */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, CFE_SB_MAX_SB_MSG_SIZE + 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = HS_ValidateMATable(HS_AppData.MATablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_ERR_EID, CFE_EVS_ERROR, "MsgActs verify err: Entry = 0, Err = -2, Length = 32769, ID = 6318"),
        "MsgActs verify err: Entry = 0, Err = -2, Length = 32769, ID = 6318");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION, "MsgActs verify results: good = 0, bad = 8, unused = 0"),
        "MsgActs verify results: good = 0, bad = 8, unused = 0");

    UtAssert_True(Result == HS_MATVAL_ERR_LEN, "Result == HS_MATVAL_ERR_LEN");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_ValidateMATable_Test_LengthTooHigh */

void HS_ValidateMATable_Test_EntryGood(void)
{
    int32   Result;
    uint32  i;

    HS_MATEntry_t     MATable[HS_MAX_MSG_ACT_TYPES];

    HS_AppData.MATablePtr = MATable;

    for (i=0; i < HS_MAX_MSG_ACT_TYPES; i++)
    {
        HS_AppData.MATablePtr[i].EnableState = HS_MAT_STATE_ENABLED;

        CFE_SB_InitMsg ((HS_NoArgsCmd_t *)&HS_AppData.MATablePtr[i].Message, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    }

    /* Execute the function being tested */
    Result = HS_ValidateMATable(HS_AppData.MATablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MATVAL_INF_EID, CFE_EVS_INFORMATION, "MsgActs verify results: good = 8, bad = 0, unused = 0"),
        "MsgActs verify results: good = 8, bad = 0, unused = 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ValidateMATable_Test_EntryGood */

void HS_SetCDSData_Test(void)
{
    uint16  ResetsPerformed = 1;
    uint16  MaxResets       = 2;

    HS_AppData.CDSState = HS_STATE_ENABLED;

    /* Execute the function being tested */
    HS_SetCDSData(ResetsPerformed, MaxResets);
    
    /* Verify results */
    UtAssert_True(HS_AppData.CDSData.ResetsPerformed == 1, "HS_AppData.CDSData.ResetsPerformed == 1");
    UtAssert_True(HS_AppData.CDSData.ResetsPerformedNot == (uint16)(~HS_AppData.CDSData.ResetsPerformed), "HS_AppData.CDSData.ResetsPerformedNot == (uint16)(~HS_AppData.CDSData.ResetsPerformed)");
    UtAssert_True(HS_AppData.CDSData.MaxResets == 2, "HS_AppData.CDSData.MaxResets == 2");
    UtAssert_True(HS_AppData.CDSData.MaxResetsNot == (uint16)(~HS_AppData.CDSData.MaxResets), "HS_AppData.CDSData.MaxResetsNot == (uint16)(~HS_AppData.CDSData.MaxResets)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_SetCDSData_Test */

void HS_Monitors_Test_AddTestCases(void)
{
    UtTest_Add(HS_MonitorApplications_Test_AppNameNotFound, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_AppNameNotFound");
    UtTest_Add(HS_MonitorApplications_Test_GetExeCountFailure, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_GetExeCountFailure");
    UtTest_Add(HS_MonitorApplications_Test_ProcessorResetError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_ProcessorResetError");
    UtTest_Add(HS_MonitorApplications_Test_ProcessorResetActionLimitError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_ProcessorResetActionLimitError");
    UtTest_Add(HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoSuccess");
    UtTest_Add(HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoNotSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_RestartAppErrorsGetAppInfoNotSuccess");
    UtTest_Add(HS_MonitorApplications_Test_FailError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_FailError");
    UtTest_Add(HS_MonitorApplications_Test_MsgActsNOACT, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_MsgActsNOACT");
    UtTest_Add(HS_MonitorApplications_Test_MsgActsErrorDefault, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorApplications_Test_MsgActsErrorDefault");

    UtTest_Add(HS_MonitorEvent_Test_ProcErrorReset, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_ProcErrorReset");
    UtTest_Add(HS_MonitorEvent_Test_ProcErrorNoReset, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_ProcErrorNoReset");
    UtTest_Add(HS_MonitorEvent_Test_AppRestartErrors, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_AppRestartErrors");
    UtTest_Add(HS_MonitorEvent_Test_OnlySecondAppRestartError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_OnlySecondAppRestartError");
    UtTest_Add(HS_MonitorEvent_Test_DeleteErrors, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_DeleteErrors");
    UtTest_Add(HS_MonitorEvent_Test_OnlySecondDeleteError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_OnlySecondDeleteError");
    UtTest_Add(HS_MonitorEvent_Test_MsgActsError, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorEvent_Test_MsgActsError");

    UtTest_Add(HS_MonitorUtilization_Test_HighCurrentUtil, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_HighCurrentUtil");
    UtTest_Add(HS_MonitorUtilization_Test_CurrentUtilLessThanZero, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_CurrentUtilLessThanZero");
    UtTest_Add(HS_MonitorUtilization_Test_CPUHogging, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_CPUHogging");
    UtTest_Add(HS_MonitorUtilization_Test_CurrentCPUHogStateDisabled, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_CurrentCPUHogStateDisabled");
    UtTest_Add(HS_MonitorUtilization_Test_HighUtilIndex, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_HighUtilIndex");
    UtTest_Add(HS_MonitorUtilization_Test_LowUtilIndex, HS_Test_Setup, HS_Test_TearDown, "HS_MonitorUtilization_Test_LowUtilIndex");

    UtTest_Add(HS_ValidateAMTable_Test_UnusedTableEntryCycleCountZero, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateAMTable_Test_UnusedTableEntryCycleCountZero");
    UtTest_Add(HS_ValidateAMTable_Test_UnusedTableEntryActionTypeNOACT, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateAMTable_Test_UnusedTableEntryActionTypeNOACT");
    UtTest_Add(HS_ValidateAMTable_Test_BufferNotNull, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateAMTable_Test_BufferNotNull");
    UtTest_Add(HS_ValidateAMTable_Test_ActionTypeNotValid, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateAMTable_Test_ActionTypeNotValid");
    UtTest_Add(HS_ValidateAMTable_Test_EntryGood, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateAMTable_Test_EntryGood");

    UtTest_Add(HS_ValidateEMTable_Test_UnusedTableEntryEventIDZero, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateEMTable_Test_UnusedTableEntryEventIDZero");
    UtTest_Add(HS_ValidateEMTable_Test_UnusedTableEntryActionTypeNOACT, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateEMTable_Test_UnusedTableEntryActionTypeNOACT");
    UtTest_Add(HS_ValidateEMTable_Test_BufferNotNull, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateEMTable_Test_BufferNotNull");
    UtTest_Add(HS_ValidateEMTable_Test_ActionTypeNotValid, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateEMTable_Test_ActionTypeNotValid");
    UtTest_Add(HS_ValidateEMTable_Test_EntryGood, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateEMTable_Test_EntryGood");

#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_ValidateXCTable_Test_UnusedTableEntry, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateXCTable_Test_UnusedTableEntry");
    UtTest_Add(HS_ValidateXCTable_Test_BufferNotNull, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateXCTable_Test_BufferNotNull");
    UtTest_Add(HS_ValidateXCTable_Test_ResourceTypeNotValid, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateXCTable_Test_ResourceTypeNotValid");
    UtTest_Add(HS_ValidateXCTable_Test_EntryGood, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateXCTable_Test_EntryGood");
#endif

    UtTest_Add(HS_ValidateMATable_Test_UnusedTableEntry, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateMATable_Test_UnusedTableEntry");
    UtTest_Add(HS_ValidateMATable_Test_InvalidEnableState, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateMATable_Test_InvalidEnableState");
    UtTest_Add(HS_ValidateMATable_Test_MessageIDTooHigh, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateMATable_Test_MessageIDTooHigh");
    UtTest_Add(HS_ValidateMATable_Test_LengthTooHigh, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateMATable_Test_LengthTooHigh");
    UtTest_Add(HS_ValidateMATable_Test_EntryGood, HS_Test_Setup, HS_Test_TearDown, "HS_ValidateMATable_Test_EntryGood");

    UtTest_Add(HS_SetCDSData_Test, HS_Test_Setup, HS_Test_TearDown, "HS_SetCDSData_Test");

} /* end HS_Monitors_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
