 /*************************************************************************
 ** File:
 **   $Id: hs_app_test.c 1.7 2016/09/09 18:13:53EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hs_app.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_app_test.c  $
 **   Revision 1.7 2016/09/09 18:13:53EDT czogby 
 **   Improve Comments
 **   Revision 1.6 2016/09/07 19:17:19EDT mdeschu 
 **   Update unit test asserts to match HS updates
 **   
 **   HS_MAX_CRITICAL_APPS/EVENTS -> HS_MAX_MONITORED_APPS/EVENTS
 **   Removal of "Critical" from certain event messages.
 **   Revision 1.5 2016/09/01 18:10:12EDT czogby 
 **   Improve comments
 **   Revision 1.4 2016/08/29 16:45:49EDT czogby 
 **   Improve comments
 **   Revision 1.3 2016/08/25 20:59:18EDT czogby 
 **   Improved readability of comments
 **   Revision 1.2 2016/08/19 14:07:20EDT czogby 
 **   HS UT-Assert Unit Tests - Code Walkthrough Updates
 **   Revision 1.1 2016/06/24 14:31:51EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hs_app_test.h"
#include "hs_app.h"
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

uint16 HS_APP_TEST_CFE_SB_RcvMsgHookCount;
int32 HS_APP_TEST_CFE_SB_RcvMsgHook(CFE_SB_MsgPtr_t *BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    HS_APP_TEST_CFE_SB_RcvMsgHookCount++;

    if (HS_APP_TEST_CFE_SB_RcvMsgHookCount % 2 == 1)
        return CFE_SUCCESS;
    else
        return CFE_SB_NO_MESSAGE;
}

uint16 HS_APP_TEST_CFE_TBL_LoadHookCount;
int32 HS_APP_TEST_CFE_TBL_LoadHook1( CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr )
{
    HS_APP_TEST_CFE_TBL_LoadHookCount++;

    if (HS_APP_TEST_CFE_TBL_LoadHookCount == 2)
        return -1;
    else
        return CFE_SUCCESS;
}

int32 HS_APP_TEST_CFE_TBL_LoadHook2( CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr )
{
    HS_APP_TEST_CFE_TBL_LoadHookCount++;

    if (HS_APP_TEST_CFE_TBL_LoadHookCount == 3)
        return -1;
    else
        return CFE_SUCCESS;
}

int32 HS_APP_TEST_CFE_TBL_LoadHook3( CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr )
{
    HS_APP_TEST_CFE_TBL_LoadHookCount++;

    if (HS_APP_TEST_CFE_TBL_LoadHookCount == 4)
        return -1;
    else
        return CFE_SUCCESS;
}

void HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1(uint32 TimeOutMilliseconds)
{
    /* This functionality is not directly related to WaitForStartupSync, but WaitForStartupSync is in a place where 
       it's necessary to do this for the test case HS_AppMain_Test_Nominal */

    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;
}

void HS_APP_TEST_CFE_ES_ExitAppHook(uint32 ExitStatus)
{
    HS_AppData.EventsMonitoredCount++;
}

/*
 * Function Definitions
 */

void HS_AppMain_Test_NominalRcvMsgSuccess(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Same return value as default, but bypasses default hook function to make test easier to write */    
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);
    
    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Used to verify completion of HS_AppMain by incrementing HS_AppData.EventsMonitoredCount. */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &HS_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True (HS_AppData.EventsMonitoredCount == 1, "HS_AppData.EventsMonitoredCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 7, "Ut_CFE_EVS_GetEventQueueDepth() == 7");
    /* 7 event messages that we don't care about in this test */

} /* end HS_AppMain_Test_NominalRcvMsgSuccess */

void HS_AppMain_Test_NominalRcvMsgNoMessage(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set return code being tested, to reach "Status = HS_ProcessMain()" as one of the nominal cases */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SB_NO_MESSAGE, 1);
    
    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Used to verify completion of HS_AppMain by incrementing HS_AppData.EventsMonitoredCount. */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &HS_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True (HS_AppData.EventsMonitoredCount == 1, "HS_AppData.EventsMonitoredCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 7, "Ut_CFE_EVS_GetEventQueueDepth() == 7");
    /* 7 event messages that we don't care about in this test */

} /* end HS_AppMain_Test_NominalRcvMsgNoMessage */

void HS_AppMain_Test_NominalRcvMsgTimeOut(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set return code being tested, to reach "Status = HS_ProcessMain()" as one of the nominal cases */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SB_TIME_OUT, 1);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Used to verify completion of HS_AppMain by incrementing HS_AppData.EventsMonitoredCount. */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &HS_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True (HS_AppData.EventsMonitoredCount == 1, "HS_AppData.EventsMonitoredCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 7, "Ut_CFE_EVS_GetEventQueueDepth() == 7");
    /* 7 event messages that we don't care about in this test */

} /* end HS_AppMain_Test_NominalRcvMsgTimeOut */

void HS_AppMain_Test_RegisterAppNotSuccess(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

     /* Set CFE_ES_RegisterApp to return -1 in order to reach "RunStatus = CFE_ES_APP_ERROR" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL, "Application Terminating, err = 0xFFFFFFFF"),
        "Application Terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Application Terminating, ERR = 0xFFFFFFFF\n"),
        "HS App: Application Terminating, ERR = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end HS_AppMain_Test_RegisterAppNotSuccess */

void HS_AppMain_Test_AppInitNotSuccess(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

     /* Set CFE_EVS_Register to return -1 in order to reach call to CFE_ES_WriteToSysLog */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL, "Application Terminating, err = 0xFFFFFFFF"),
        "Application Terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Application Terminating, ERR = 0xFFFFFFFF\n"),
        "HS App: Application Terminating, ERR = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 2, "Ut_CFE_ES_GetSysLogQueueDepth() == 2");
    /* 1 system log message that we don't care about in this test */

} /* end HS_AppMain_Test_AppInitNotSuccess */

void HS_AppMain_Test_ProcessMainNotSuccess(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* In order to make ProcessMain return -1, we make its call to HS_ProcessCommands return -1 by making its call to RcvMsg return -1 */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 2);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL, "Application Terminating, err = 0xFFFFFFFF"),
        "Application Terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 8, "Ut_CFE_EVS_GetEventQueueDepth() == 8");
    /* 7 event messages that we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Application Terminating, ERR = 0xFFFFFFFF\n"),
        "HS App: Application Terminating, ERR = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end HS_AppMain_Test_ProcessMainNotSuccess */

void HS_AppMain_Test_SBSubscribeEVSError(void)
{
    /* Set so the loop will never be run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 1);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Set CFE_SB_SubscribeEx to return -1 in order to generate error message HS_SUB_EVS_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBEEX_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SUB_EVS_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to Events,RC=0xFFFFFFFF"),
        "Error Subscribing to Events,RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL, "Application Terminating, err = 0xFFFFFFFF"),
        "Application Terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 9, "Ut_CFE_EVS_GetEventQueueDepth() == 9");
    /* 6 event messages that we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Application Terminating, ERR = 0xFFFFFFFF\n"),
        "HS App: Application Terminating, ERR = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* HS_AppMain_Test_SBSubscribeEVSError */

void HS_AppMain_Test_RcvMsgError(void)
{
    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set RcvMsg to return -1 in order to reach "RunStatus = CFE_ES_APP_ERROR" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Sets HS_AppData.CurrentEventMonState = HS_STATE_ENABLED (because set to 0 inside HS_AppMain) */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_WAITFORSTARTUPSYNC_INDEX, &HS_APP_TEST_CFE_ES_WaitForStartupSyncHook1);

    /* Execute the function being tested */
    HS_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APP_EXIT_EID, CFE_EVS_CRITICAL, "Application Terminating, err = 0xFFFFFFFF"),
        "Application Terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 8, "Ut_CFE_EVS_GetEventQueueDepth() == 8");
    /* 6 event messages that we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Application Terminating, ERR = 0xFFFFFFFF\n"),
        "HS App: Application Terminating, ERR = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* HS_AppMain_Test_RcvMsgError */

void HS_AppInit_Test_Nominal(void)
{
    int32 Result;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    sprintf(Message, "HS Initialized.  Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 7, "Ut_CFE_EVS_GetEventQueueDepth() == 7");
    /* 6 event messages that we don't care about in this test */

} /* end HS_AppInit_Test_Nominal */

void HS_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* Set CFE_EVS_Register to return -1 in order to reach call to CFE_ES_WriteToSysLog */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HS App: Error Registering For Event Services, RC = 0xFFFFFFFF\n"),
        "HS App: Error Registering For Event Services, RC = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end HS_AppInit_Test_EVSRegisterError */

void HS_AppInit_Test_CorruptCDSResetsPerformed(void)
{
    int32 Result;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    HS_AppData.CDSData.MaxResets    = 0;
    HS_AppData.CDSData.MaxResetsNot = 0;

    HS_AppData.CDSData.ResetsPerformed    = 1;
    HS_AppData.CDSData.ResetsPerformedNot = 3;

    /* To enter if-block after "Create Critical Data Store" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CDS_CORRUPT_ERR_EID, CFE_EVS_ERROR, "Data in CDS was corrupt, initializing resets data"),
        "Data in CDS was corrupt, initializing resets data");

    sprintf(Message, "HS Initialized.  Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 8, "Ut_CFE_EVS_GetEventQueueDepth() == 8");
    /* 6 event messages that we don't care about in this test */

} /* end HS_AppInit_Test_CorruptCDSResetsPerformed */

void HS_AppInit_Test_CorruptCDSMaxResets(void)
{
    int32 Result;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    HS_AppData.CDSData.MaxResets    = 1;
    HS_AppData.CDSData.MaxResetsNot = 3;

    HS_AppData.CDSData.ResetsPerformed    = 0;
    HS_AppData.CDSData.ResetsPerformedNot = 0;

    /* To enter if-block after "Create Critical Data Store" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CDS_CORRUPT_ERR_EID, CFE_EVS_ERROR, "Data in CDS was corrupt, initializing resets data"),
        "Data in CDS was corrupt, initializing resets data");

    sprintf(Message, "HS Initialized.  Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 8, "Ut_CFE_EVS_GetEventQueueDepth() == 8");
    /* 6 event messages that we don't care about in this test */

} /* end HS_AppInit_Test_CorruptCDSMaxResets */

void HS_AppInit_Test_RestoreCDSError(void)
{
    int32 Result;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* To enter if-block after "Create Critical Data Store" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);

    /* Set CFE_ES_RestoreFromCDS to return -1 in order to generate error HS_CDS_RESTORE_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RESTOREFROMCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CDS_RESTORE_ERR_EID, CFE_EVS_ERROR, "Failed to restore data from CDS (Err=0xffffffff), initializing resets data"),
        "Failed to restore data from CDS (Err=0xffffffff), initializing resets data");

    sprintf(Message, "HS Initialized.  Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 8, "Ut_CFE_EVS_GetEventQueueDepth() == 8");
    /* 6 event messages that we don't care about in this test */

} /* end HS_AppInit_Test_RestoreCDSError */

void HS_AppInit_Test_DisableSavingToCDS(void)
{
    int32 Result;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* Set CFE_ES_RegisterCDS to return -1 in order to reach block of code with comment "Disable saving to CDS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (HS_AppData.CDSState == HS_STATE_DISABLED, "HS_AppData.CDSState == HS_STATE_DISABLED");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    sprintf(Message, "HS Initialized.  Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 7, "Ut_CFE_EVS_GetEventQueueDepth() == 7");
    /* 6 event messages that we don't care about in this test */

} /* end HS_AppInit_Test_DisableSavingToCDS */

void HS_AppInit_Test_SBInitError(void)
{
    int32 Result;

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* Set CFE_SB_CreatePipe to return -1 on the first call in order to cause HS_SbInit to return -1, in order to enter the if-block immediately after */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    /* This event message is not generated directly by the function under test, but it's useful to check for it to ensure that an SB init error occurred rather than a TBL init error */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_CMD_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating SB Command Pipe,RC=0xFFFFFFFF"),
        "Error Creating SB Command Pipe,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_AppInit_Test_SBInitError */

void HS_AppInit_Test_TblInitError(void)
{
    int32 Result;

    HS_AppData.ServiceWatchdogFlag = 99;
    HS_AppData.AlivenessCounter = 99;
    HS_AppData.RunStatus = 99;
    HS_AppData.EventsMonitoredCount = 99;
    HS_AppData.MsgActExec = 99;
    HS_AppData.CurrentAppMonState = 99;
    HS_AppData.CurrentEventMonState = 99;
    HS_AppData.CurrentAlivenessState = 99;
    HS_AppData.CurrentCPUHogState = 99;

    /* Set CFE_TBL_Register to return -1 in order to cause HS_TblInit to return -1, in order to enter the if-block immediately after */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED, "HS_AppData.ServiceWatchdogFlag == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");
    UtAssert_True (HS_AppData.RunStatus == CFE_ES_APP_RUN, "HS_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");
    /* Not checking that HS_AppData.CurrentAppMonState == HS_APPMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    /* Not checking that HS_AppData.CurrentEventMonState == HS_EVENTMON_DEFAULT_STATE, because it's modified in a subfunction that we don't care about here */
    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE, "HS_AppData.CurrentAlivenessState == HS_ALIVENESS_DEFAULT_STATE");
    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE, "HS_AppData.CurrentCPUHogState == HS_CPUHOG_DEFAULT_STATE");

    /* This event message is not generated directly by the function under test, but it's useful to check for it to ensure that a TBL init error occurred rather than an SB init error */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMT_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering AppMon Table,RC=0xFFFFFFFF"),
        "Error Registering AppMon Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_AppInit_Test_TblInitError */

void HS_SbInit_Test_Nominal(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe != 0, "HS_AppData.EventPipe != 0"); /* Set to a new value when initialized */
    UtAssert_True (HS_AppData.WakeupPipe != 0, "HS_AppData.WakeupPipe != 0"); /* Set to a new value when initialized */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_SbInit_Test_Nominal */

void HS_SbInit_Test_CreateSBCmdPipeError(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_CreatePipe to return -1 on first call, to generate error HS_CR_CMD_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe == 0, "HS_AppData.EventPipe == 0");
    UtAssert_True (HS_AppData.WakeupPipe == 0, "HS_AppData.WakeupPipe == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_CMD_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating SB Command Pipe,RC=0xFFFFFFFF"),
        "Error Creating SB Command Pipe,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_CreateSBCmdPipeError */

void HS_SbInit_Test_CreateSBEventPipeError(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_CreatePipe to return -1 on second call, to generate error HS_CR_EVENT_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe == 0, "HS_AppData.EventPipe == 0");
    UtAssert_True (HS_AppData.WakeupPipe == 0, "HS_AppData.WakeupPipe == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_EVENT_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating SB Event Pipe,RC=0xFFFFFFFF"),
        "Error Creating SB Event Pipe,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_CreateSBEventPipeError */

void HS_SbInit_Test_CreateSBWakeupPipe(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_CreatePipe to return -1 on third call, to generate error HS_CR_WAKEUP_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe != 0, "HS_AppData.EventPipe != 0"); /* Set to a new value when initialized */
    UtAssert_True (HS_AppData.WakeupPipe == 0, "HS_AppData.WakeupPipe == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_WAKEUP_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating SB Wakeup Pipe,RC=0xFFFFFFFF"),
        "Error Creating SB Wakeup Pipe,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_CreateSBWakeupPipe */

void HS_SbInit_Test_SubscribeHKRequestError(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_Subscribe to return -1 on first call, to generate error HS_SUB_REQ_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe != 0, "HS_AppData.EventPipe != 0"); /* Set to a new value when initialized */
    UtAssert_True (HS_AppData.WakeupPipe != 0, "HS_AppData.WakeupPipe != 0"); /* Set to a new value when initialized */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SUB_REQ_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to HK Request,RC=0xFFFFFFFF"),
        "Error Subscribing to HK Request,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_SubscribeHKRequestError */

void HS_SbInit_Test_SubscribeGndCmdsError(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_Subscribe to return -1 on second call, to generate error HS_SUB_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe != 0, "HS_AppData.EventPipe != 0"); /* Set to a new value when initialized */
    UtAssert_True (HS_AppData.WakeupPipe != 0, "HS_AppData.WakeupPipe != 0"); /* Set to a new value when initialized */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SUB_CMD_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to Gnd Cmds,RC=0xFFFFFFFF"),
        "Error Subscribing to Gnd Cmds,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_SubscribeGndCmdsError */

void HS_SbInit_Test_SubscribeWakeupError(void)
{
    int32 Result;

    HS_AppData.MsgPtr  = (CFE_SB_MsgPtr_t) 99;
    HS_AppData.CmdPipe = 99;
    HS_AppData.EventPipe = 99;
    HS_AppData.WakeupPipe = 99;

    /* Set CFE_SB_Subscribe to return -1 on third call, to generate error HS_SUB_WAKEUP_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = HS_SbInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL, "HS_AppData.MsgPtr == (CFE_SB_MsgPtr_t) NULL");
    UtAssert_True (HS_AppData.CmdPipe == 0, "HS_AppData.CmdPipe == 0");
    UtAssert_True (HS_AppData.EventPipe != 0, "HS_AppData.EventPipe != 0"); /* Set to a new value when initialized */
    UtAssert_True (HS_AppData.WakeupPipe != 0, "HS_AppData.WakeupPipe != 0"); /* Set to a new value when initialized */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SUB_WAKEUP_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to Wakeup,RC=0xFFFFFFFF"),
        "Error Subscribing to Wakeup,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SbInit_Test_SubscribeWakeupError */

void HS_TblInit_Test_Nominal(void)
{
    int32 Result;

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_TblInit_Test_Nominal */

void HS_TblInit_Test_RegisterAppMonTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Register to return -1 on first call, to generate error HS_AMT_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMT_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering AppMon Table,RC=0xFFFFFFFF"),
        "Error Error Registering AppMon Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_RegisterAppMonTableError */

void HS_TblInit_Test_RegisterEventMonTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Register to return -1 on second call, to generate error HS_EMT_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMT_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering EventMon Table,RC=0xFFFFFFFF"),
        "Error Error Registering EventMon Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_RegisterEventMonTableError */

void HS_TblInit_Test_RegisterMsgActsTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Register to return -1 on third call, to generate error HS_MAT_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 3);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MAT_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering MsgActs Table,RC=0xFFFFFFFF"),
        "Error Error Registering MsgActs Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_RegisterMsgActsTableError */

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_TblInit_Test_RegisterExeCountTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Register to return -1 on fourth call, to generate error HS_XCT_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 4);

    /* Same return value as default, but bypasses default hook function to make test easier to write */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCT_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering ExeCount Table,RC=0xFFFFFFFF"),
        "Error Error Registering ExeCount Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_RegisterExeCountTableError */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_TblInit_Test_LoadExeCountTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Load to fail on first call, to generate error HS_XCT_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 2);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    /* Note: not verifying that HS_AppData.ExeCountState == HS_STATE_DISABLED, because HS_AppData.ExeCountState is modified by HS_AcquirePointers at the end of HS_TblInit */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_XCT_LD_ERR_EID, CFE_EVS_ERROR, "Error Loading ExeCount Table,RC=0xCC000013"),
        "Error Loading ExeCount Table,RC=0xCC000013");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_LoadExeCountTableError */
#endif

void HS_TblInit_Test_LoadAppMonTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Load to fail on second call, to generate error HS_AMT_LD_ERR_EID */
    HS_APP_TEST_CFE_TBL_LoadHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, &HS_APP_TEST_CFE_TBL_LoadHook1);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (HS_AppData.CurrentAppMonState == HS_STATE_DISABLED, "HS_AppData.CurrentAppMonState == HS_STATE_DISABLED");
    /* Note: not verifying that HS_AppData.AppMonLoaded == HS_STATE_DISABLED, because HS_AppData.AppMonLoaded is modified by HS_AcquirePointers at the end of HS_TblInit */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_AMT_LD_ERR_EID, CFE_EVS_ERROR, "Error Loading AppMon Table,RC=0xFFFFFFFF"),
        "Error Loading AppMon Table,RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_APPMON_ERR_EID, CFE_EVS_ERROR, "Application Monitoring Disabled due to Table Load Failure"),
        "Application Monitoring Disabled due to Table Load Failure");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_TblInit_Test_LoadAppMonTableError */

void HS_TblInit_Test_LoadEventMonTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Load to fail on third call, to generate error HS_EMT_LD_ERR_EID */
    HS_APP_TEST_CFE_TBL_LoadHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, &HS_APP_TEST_CFE_TBL_LoadHook2);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");
    /* Note: not verifying that HS_AppData.EventMonLoaded == HS_STATE_DISABLED, because HS_AppData.EventMonLoaded is modified by HS_AcquirePointers at the end of HS_TblInit */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EMT_LD_ERR_EID, CFE_EVS_ERROR, "Error Loading EventMon Table,RC=0xFFFFFFFF"),
        "Error Loading EventMon Table,RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_EVENTMON_ERR_EID, CFE_EVS_ERROR, "Event Monitoring Disabled due to Table Load Failure"),
        "Event Monitoring Disabled due to Table Load Failure");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end HS_TblInit_Test_LoadEventMonTableError */

void HS_TblInit_Test_LoadMsgActsTableError(void)
{
    int32 Result;

    /* Set CFE_TBL_Load to fail on fourth call, to generate error HS_MAT_LD_ERR_EID */
    HS_APP_TEST_CFE_TBL_LoadHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, &HS_APP_TEST_CFE_TBL_LoadHook3);

    /* Execute the function being tested */
    Result = HS_TblInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    /* Note: not verifying that HS_AppData.MsgActsState == HS_STATE_DISABLED, because HS_AppData.MsgActsState is modified by HS_AcquirePointers at the end of HS_TblInit */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MAT_LD_ERR_EID, CFE_EVS_ERROR, "Error Loading MsgActs Table,RC=0xFFFFFFFF"),
        "Error Loading MsgActs Table,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_TblInit_Test_LoadMsgActsTableError */

void HS_ProcessMain_Test(void)
{
    int32 Result;

    HS_AMTEntry_t  AMTable;

    HS_AppData.AMTablePtr = &AMTable;

    /* Prevents error messages in call to HS_AcquirePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);

    /* Prevents error messages in call to HS_AcquirePointers */
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Causes HS_ProcessCommands to return CFE_SUCCESS, which is then returned from HS_ProcessMain */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SB_NO_MESSAGE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_RCVMSG_INDEX);

    HS_AppData.MsgActCooldown[0]                         = 2;
    HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES / 2]  = 2;
    HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES - 1]  = 2;

    HS_AppData.CurrentAppMonState = HS_STATE_ENABLED;
    HS_AppData.CurrentAlivenessState = HS_STATE_ENABLED;
    HS_AppData.AlivenessCounter = HS_CPU_ALIVE_PERIOD;
    HS_AppData.ServiceWatchdogFlag = HS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = HS_ProcessMain();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.MsgActCooldown[0] == 1, "HS_AppData.MsgActCooldown[0] == 1");
    UtAssert_True (HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES / 2] == 1, "HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES / 2] == 1");
    UtAssert_True (HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES - 1] == 1, "HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES - 1] == 1");
    UtAssert_True (HS_AppData.AlivenessCounter == 0, "HS_AppData.AlivenessCounter == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_ProcessMain_Test */

void HS_ProcessCommands_Test(void)
{
    int32           Result;
    HS_NoArgsCmd_t  CmdPacket;
    uint32          i;
    HS_EMTEntry_t   EMTable[HS_MAX_MONITORED_EVENTS];

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_NOOP_CC);

    HS_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdPacket;
    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;
    HS_AppData.EMTablePtr = EMTable;

    HS_AppData.EventsMonitoredCount = 0;

    /* This loop is to prevent errors within one of the subfunctions not being tested here */
    for (i = 0; i < HS_MAX_MONITORED_EVENTS; i++)
    {
        HS_AppData.EMTablePtr[i].EventID = -1;
    }

    /* Causes CFE_SB_RcvMsg to alternate returning CFE_SUCCESS and CFE_SB_NO_MESSAGE, to reach all code branches. */
    HS_APP_TEST_CFE_SB_RcvMsgHookCount = 0;
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_RCVMSG_INDEX, &HS_APP_TEST_CFE_SB_RcvMsgHook);

    /* Execute the function being tested */
    Result = HS_ProcessCommands();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 1, "HS_AppData.EventsMonitoredCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* 1 event message that we don't care about in this test */

} /* end HS_ProcessCommands_Test */

void HS_App_Test_AddTestCases(void)
{
#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_AppMain_Test_NominalRcvMsgSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_NominalRcvMsgSuccess");
    UtTest_Add(HS_AppMain_Test_NominalRcvMsgNoMessage, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_NominalRcvMsgNoMessage");
    UtTest_Add(HS_AppMain_Test_NominalRcvMsgTimeOut, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_NominalRcvMsgTimeOut");
    UtTest_Add(HS_AppMain_Test_RegisterAppNotSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_RegisterAppNotSuccess");
    UtTest_Add(HS_AppMain_Test_AppInitNotSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_AppInitNotSuccess");
    UtTest_Add(HS_AppMain_Test_ProcessMainNotSuccess, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_ProcessMainNotSuccess");
    UtTest_Add(HS_AppMain_Test_SBSubscribeEVSError, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_SBSubscribeEVSError");
    UtTest_Add(HS_AppMain_Test_RcvMsgError, HS_Test_Setup, HS_Test_TearDown, "HS_AppMain_Test_RcvMsgError");
#endif

    UtTest_Add(HS_AppInit_Test_EVSRegisterError, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_EVSRegisterError");
#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_AppInit_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_Nominal");
    UtTest_Add(HS_AppInit_Test_CorruptCDSResetsPerformed, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_CorruptCDSResetsPerformed");
    UtTest_Add(HS_AppInit_Test_CorruptCDSMaxResets, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_CorruptCDSMaxResets");
    UtTest_Add(HS_AppInit_Test_RestoreCDSError, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_RestoreCDSError");
    UtTest_Add(HS_AppInit_Test_DisableSavingToCDS, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_DisableSavingToCDS");
    UtTest_Add(HS_AppInit_Test_SBInitError, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_SBInitError");
    UtTest_Add(HS_AppInit_Test_TblInitError, HS_Test_Setup, HS_Test_TearDown, "HS_AppInit_Test_TblInitError");
#endif

    UtTest_Add(HS_SbInit_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_Nominal");
    UtTest_Add(HS_SbInit_Test_CreateSBCmdPipeError, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_CreateSBCmdPipeError");
    UtTest_Add(HS_SbInit_Test_CreateSBEventPipeError, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_CreateSBEventPipeError");
    UtTest_Add(HS_SbInit_Test_CreateSBWakeupPipe, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_CreateSBWakeupPipe");
    UtTest_Add(HS_SbInit_Test_SubscribeHKRequestError, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_SubscribeHKRequestError");
    UtTest_Add(HS_SbInit_Test_SubscribeGndCmdsError, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_SubscribeGndCmdsError");
    UtTest_Add(HS_SbInit_Test_SubscribeWakeupError, HS_Test_Setup, HS_Test_TearDown, "HS_SbInit_Test_SubscribeWakeupError");

#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_TblInit_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_Nominal");
    UtTest_Add(HS_TblInit_Test_RegisterAppMonTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_RegisterAppMonTableError");
    UtTest_Add(HS_TblInit_Test_RegisterEventMonTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_RegisterEventMonTableError");
    UtTest_Add(HS_TblInit_Test_RegisterMsgActsTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_RegisterMsgActsTableError");
    UtTest_Add(HS_TblInit_Test_RegisterExeCountTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_RegisterExeCountTableError");
    UtTest_Add(HS_TblInit_Test_LoadExeCountTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_LoadExeCountTableError");
    UtTest_Add(HS_TblInit_Test_LoadAppMonTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_LoadAppMonTableError");
    UtTest_Add(HS_TblInit_Test_LoadEventMonTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_LoadEventMonTableError");
    UtTest_Add(HS_TblInit_Test_LoadMsgActsTableError, HS_Test_Setup, HS_Test_TearDown, "HS_TblInit_Test_LoadMsgActsTableError");
#endif

    UtTest_Add(HS_ProcessMain_Test, HS_Test_Setup, HS_Test_TearDown, "HS_ProcessMain_Test");

    UtTest_Add(HS_ProcessCommands_Test, HS_Test_Setup, HS_Test_TearDown, "HS_ProcessCommands_Test");

} /* end HS_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
