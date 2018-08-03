 /*************************************************************************
 ** File:
 **   $Id: hs_cmds_test.c 1.4 2016/09/07 19:17:19EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hs_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_cmds_test.c  $
 **   Revision 1.4 2016/09/07 19:17:19EDT mdeschu 
 **   Update unit test asserts to match HS updates
 **   
 **   HS_MAX_CRITICAL_APPS/EVENTS -> HS_MAX_MONITORED_APPS/EVENTS
 **   Removal of "Critical" from certain event messages.
 **   Revision 1.3 2016/08/25 20:59:18EDT czogby 
 **   Improved readability of comments
 **   Revision 1.2 2016/08/19 14:07:20EDT czogby 
 **   HS UT-Assert Unit Tests - Code Walkthrough Updates
 **   Revision 1.1 2016/06/24 14:31:52EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hs_cmds_test.h"
#include "hs_app.h"
#include "hs_cmds.h"
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

int32 HS_CMDS_TEST_CFE_ES_GetTaskInfoHook(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{
    TaskInfo->ExecutionCounter = 5;

    return CFE_SUCCESS;
}

void HS_AppPipe_Test_SendHK(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_SEND_HK_MID, sizeof(HS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_AppPipe_Test_SendHK */

void HS_AppPipe_Test_Noop(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_NOOP_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_Noop */

void HS_AppPipe_Test_Reset(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_RESET_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_Reset */

void HS_AppPipe_Test_EnableAppMon(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_APPMON_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_EnableAppMon */

void HS_AppPipe_Test_DisableAppMon(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_APPMON_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_DisableAppMon */

void HS_AppPipe_Test_EnableEventMon(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_EnableEventMon */

void HS_AppPipe_Test_DisableEventMon(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];

    HS_AppData.EMTablePtr = EMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_EVENTMON_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_DisableEventMon */

void HS_AppPipe_Test_EnableAliveness(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_ALIVENESS_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_EnableAliveness */

void HS_AppPipe_Test_DisableAliveness(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_ALIVENESS_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_DisableAliveness */

void HS_AppPipe_Test_ResetResetsPerformed(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_RESET_RESETS_PERFORMED_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_ResetResetsPerformed */

void HS_AppPipe_Test_SetMaxResets(void)
{
    HS_SetMaxResetsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_SetMaxResetsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_MAX_RESETS_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_SetMaxResets */

void HS_AppPipe_Test_EnableCPUHog(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_CPUHOG_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_EnableCPUHog */

void HS_AppPipe_Test_DisableCPUHog(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_CPUHOG_CC);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_AppPipe_Test_DisableCPUHog */

void HS_AppPipe_Test_InvalidCC(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CC_ERR_EID, CFE_EVS_ERROR, "Invalid command code: ID = 0x18AE, CC = 99"),
        "Invalid command code: ID = 0x18AE, CC = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_AppPipe_Test_InvalidCC */

void HS_AppPipe_Test_InvalidMID(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 255, sizeof(HS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    HS_AppPipe((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MID_ERR_EID, CFE_EVS_ERROR, "Invalid command pipe message ID: 0x00FF"),
        "Invalid command pipe message ID: 0x00FF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_AppPipe_Test_InvalidMID */

void HS_HousekeepingReq_Test_InvalidEventMon(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    uint32            TableIndex;

    HS_AppData.EMTablePtr = EMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT + 1;

    /* Satisfies condition "if (Status == CFE_ES_ERR_APPNAME)" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, CFE_ES_ERR_APPNAME, 1);

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
    {
        HS_AppData.AppMonEnables[TableIndex] = TableIndex;
    }

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 1, "HS_AppData.HkPacket.InvalidEventMonCount == 1");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_InvalidEventMon */

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_HousekeepingReq_Test_AllFlagsEnabled(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];
    uint8             ExpectedStatusFlags = 0;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.XCTablePtr = XCTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT;

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    HS_AppData.ExeCountState = HS_STATE_ENABLED;
    HS_AppData.MsgActsState = HS_STATE_ENABLED;
    HS_AppData.AppMonLoaded = HS_STATE_ENABLED;
    HS_AppData.EventMonLoaded = HS_STATE_ENABLED;
    HS_AppData.CDSState = HS_STATE_ENABLED;

    ExpectedStatusFlags   |= HS_LOADED_XCT;
    ExpectedStatusFlags   |= HS_LOADED_MAT;
    ExpectedStatusFlags   |= HS_LOADED_AMT;
    ExpectedStatusFlags   |= HS_LOADED_EMT;
    ExpectedStatusFlags   |= HS_CDS_IN_USE;

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 0, "HS_AppData.HkPacket.InvalidEventMonCount == 0");

    UtAssert_True (HS_AppData.HkPacket.StatusFlags == ExpectedStatusFlags, "HS_AppData.HkPacket.StatusFlags == ExpectedStatusFlags");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_AllFlagsEnabled */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_HousekeepingReq_Test_ResourceTypeAppMain(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];
    uint32            TableIndex;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.XCTablePtr = XCTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT;

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
    {
        HS_AppData.AppMonEnables[TableIndex] = TableIndex;
    }

    HS_AppData.ExeCountState = HS_STATE_ENABLED;
    HS_AppData.XCTablePtr[0].ResourceType = HS_XCT_TYPE_APP_MAIN;

    /* Causes line "Status = CFE_ES_GetTaskInfo(&TaskInfo, TaskId)" to be reached */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_TASKGETIDBYNAME_INDEX, OS_SUCCESS, 1);

    /* Sets TaskInfo.ExecutionCounter to 5, returns CFE_SUCCESS, goes to line "ExeCount = TaskInfo.ExecutionCounter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETTASKINFO_INDEX, &HS_CMDS_TEST_CFE_ES_GetTaskInfoHook);

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 0, "HS_AppData.HkPacket.InvalidEventMonCount == 0");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (HS_AppData.HkPacket.ExeCounts[0] == 5, "HS_AppData.HkPacket.ExeCounts[0] == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_ResourceTypeAppMain */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_HousekeepingReq_Test_ResourceTypeAppChild(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];
    uint32            TableIndex;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.XCTablePtr = XCTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT;

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
    {
        HS_AppData.AppMonEnables[TableIndex] = TableIndex;
    }

    HS_AppData.ExeCountState = HS_STATE_ENABLED;
    HS_AppData.XCTablePtr[0].ResourceType = HS_XCT_TYPE_APP_CHILD;

    /* Causes line "Status = CFE_ES_GetTaskInfo(&TaskInfo, TaskId)" to be reached */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_TASKGETIDBYNAME_INDEX, OS_SUCCESS, 1);

    /* Sets TaskInfo.ExecutionCounter to 5, returns CFE_SUCCESS, goes to line "ExeCount = TaskInfo.ExecutionCounter" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETTASKINFO_INDEX, &HS_CMDS_TEST_CFE_ES_GetTaskInfoHook);

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 0, "HS_AppData.HkPacket.InvalidEventMonCount == 0");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (HS_AppData.HkPacket.ExeCounts[0] == 5, "HS_AppData.HkPacket.ExeCounts[0] == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_ResourceTypeAppChild */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_HousekeepingReq_Test_ResourceTypeDevice(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];
    uint32            TableIndex;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.XCTablePtr = XCTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT;

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
    {
        HS_AppData.AppMonEnables[TableIndex] = TableIndex;
    }

    HS_AppData.ExeCountState = HS_STATE_ENABLED;
    HS_AppData.XCTablePtr[0].ResourceType = HS_XCT_TYPE_DEVICE;

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 0, "HS_AppData.HkPacket.InvalidEventMonCount == 0");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (HS_AppData.HkPacket.ExeCounts[0] == HS_INVALID_EXECOUNT, "HS_AppData.HkPacket.ExeCounts[0] == HS_INVALID_EXECOUNT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_ResourceTypeDevice */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_HousekeepingReq_Test_ResourceTypeISR(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_EMTEntry_t     EMTable[HS_MAX_MONITORED_EVENTS];
    HS_XCTEntry_t     XCTable[HS_MAX_EXEC_CNT_SLOTS];
    uint32            TableIndex;

    HS_AppData.EMTablePtr = EMTable;
    HS_AppData.XCTablePtr = XCTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.EMTablePtr[0].ActionType = HS_EMT_ACT_NOACT;

    HS_AppData.CmdCount = 1;
    HS_AppData.CmdErrCount = 2;
    HS_AppData.CurrentAppMonState = 3;
    HS_AppData.CurrentEventMonState = 4;
    HS_AppData.CurrentAlivenessState = 5;
    HS_AppData.CurrentCPUHogState = 6;
    HS_AppData.CDSData.ResetsPerformed = 7;
    HS_AppData.CDSData.MaxResets = 8;
    HS_AppData.EventsMonitoredCount = 9;
    HS_AppData.MsgActExec = 10;

    for(TableIndex = 0; TableIndex <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); TableIndex++)
    {
        HS_AppData.AppMonEnables[TableIndex] = TableIndex;
    }

    HS_AppData.ExeCountState = HS_STATE_ENABLED;
    HS_AppData.XCTablePtr[0].ResourceType = HS_XCT_TYPE_ISR;

    /* Execute the function being tested */
    HS_HousekeepingReq((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.HkPacket.CmdCount == 1, "HS_AppData.HkPacket.CmdCount == 1");
    UtAssert_True (HS_AppData.HkPacket.CmdErrCount == 2, "HS_AppData.HkPacket.CmdErrCount == 2");
    UtAssert_True (HS_AppData.HkPacket.CurrentAppMonState == 3, "HS_AppData.HkPacket.CurrentAppMonState == 3");
    UtAssert_True (HS_AppData.HkPacket.CurrentEventMonState == 4, "HS_AppData.HkPacket.CurrentEventMonState == 4");
    UtAssert_True (HS_AppData.HkPacket.CurrentAlivenessState == 5, "HS_AppData.HkPacket.CurrentAlivenessState == 5");
    UtAssert_True (HS_AppData.HkPacket.CurrentCPUHogState == 6, "HS_AppData.HkPacket.CurrentCPUHogState == 6");
    UtAssert_True (HS_AppData.HkPacket.ResetsPerformed == 7, "HS_AppData.HkPacket.ResetsPerformed == 7");
    UtAssert_True (HS_AppData.HkPacket.MaxResets == 8, "HS_AppData.HkPacket.MaxResets == 8");
    UtAssert_True (HS_AppData.HkPacket.EventsMonitoredCount == 9, "HS_AppData.HkPacket.EventsMonitoredCount == 9");
    UtAssert_True (HS_AppData.HkPacket.MsgActExec == 10, "HS_AppData.HkPacket.MsgActExec == 10");
    UtAssert_True (HS_AppData.HkPacket.InvalidEventMonCount == 0, "HS_AppData.HkPacket.InvalidEventMonCount == 0");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE,
        "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == (HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE");

    UtAssert_True (HS_AppData.HkPacket.ExeCounts[0] == HS_INVALID_EXECOUNT, "HS_AppData.HkPacket.ExeCounts[0] == HS_INVALID_EXECOUNT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_HousekeepingReq_Test_ResourceTypeISR */
#endif

void HS_Noop_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    char  Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_NOOP_CC);

    /* Execute the function being tested */
    HS_NoopCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    sprintf(Message, "No-op command: Version %d.%d.%d.%d", HS_MAJOR_VERSION, HS_MINOR_VERSION, HS_REVISION, HS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HS_NOOP_INF_EID, CFE_EVS_INFORMATION, Message), Message);
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_Noop_Test */

void HS_ResetCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_RESET_CC);

    /* Execute the function being tested */
    HS_ResetCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 0, "HS_AppData.CmdCount == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_RESET_DBG_EID, CFE_EVS_DEBUG, "Reset counters command"),
        "Reset counters command");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ResetCmd_Test */

void HS_ResetCounters_Test(void)
{
    /* No setup required for this test */

    /* Execute the function being tested */
    HS_ResetCounters();

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 0, "HS_AppData.CmdCount == 0");
    UtAssert_True (HS_AppData.CmdErrCount == 0, "HS_AppData.CmdErrCount == 0");
    UtAssert_True (HS_AppData.EventsMonitoredCount == 0, "HS_AppData.EventsMonitoredCount == 0");
    UtAssert_True (HS_AppData.MsgActExec == 0, "HS_AppData.MsgActExec == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_ResetCounters_Test */

void HS_EnableAppMonCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_APPMON_CC);

    /* Execute the function being tested */
    HS_EnableAppMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentAppMonState == HS_STATE_ENABLED, "HS_AppData.CurrentAppMonState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_ENABLE_APPMON_DBG_EID, CFE_EVS_DEBUG, "Application Monitoring Enabled"),
        "Application Monitoring Enabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableAppMonCmd_Test */

void HS_DisableAppMonCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_APPMON_CC);

    /* Execute the function being tested */
    HS_DisableAppMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentAppMonState == HS_STATE_DISABLED, "HS_AppData.CurrentAppMonState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_APPMON_DBG_EID, CFE_EVS_DEBUG, "Application Monitoring Disabled"),
        "Application Monitoring Disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableAppMonCmd_Test */

void HS_EnableEventMonCmd_Test_Disabled(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;

    /* Execute the function being tested */
    HS_EnableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED, "HS_AppData.CurrentEventMonState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_ENABLE_EVENTMON_DBG_EID, CFE_EVS_DEBUG, "Event Monitoring Enabled"),
        "Event Monitoring Enabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableEventMonCmd_Test_Disabled */

void HS_EnableEventMonCmd_Test_AlreadyEnabled(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;

    /* Execute the function being tested */
    HS_EnableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED, "HS_AppData.CurrentEventMonState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_ENABLE_EVENTMON_DBG_EID, CFE_EVS_DEBUG, "Event Monitoring Enabled"),
        "Event Monitoring Enabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableEventMonCmd_Test_AlreadyEnabled */

void HS_EnableEventMonCmd_Test_SubscribeError(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBEEX_INDEX, -1, 1); /* Causes event message HS_EVENTMON_SUB_EID to be generated */

    /* Execute the function being tested */
    HS_EnableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_SUB_EID, CFE_EVS_ERROR, "Event Monitor Enable: Error Subscribing to Events,RC=0xFFFFFFFF"),
        "Event Monitor Enable: Error Subscribing to Events,RC=0xFFFFFFFF");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableEventMonCmd_Test_SubscribeError */

void HS_DisableEventMonCmd_Test_Enabled(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;

    /* Execute the function being tested */
    HS_DisableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_EVENTMON_DBG_EID, CFE_EVS_DEBUG, "Event Monitoring Disabled"),
        "Event Monitoring Disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableEventMonCmd_Test_Enabled */

void HS_DisableEventMonCmd_Test_AlreadyDisabled(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;

    /* Execute the function being tested */
    HS_DisableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_EVENTMON_DBG_EID, CFE_EVS_DEBUG, "Event Monitoring Disabled"),
        "Event Monitoring Disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableEventMonCmd_Test_AlreadyDisabled */

void HS_DisableEventMonCmd_Test_UnsubscribeError(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_EVENTMON_CC);

    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_UNSUBSCRIBE_INDEX, -1, 1); /* Causes event message HS_EVENTMON_UNSUB_EID to be generated */

    /* Execute the function being tested */
    HS_DisableEventMonCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_ENABLED, "HS_AppData.CurrentEventMonState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_UNSUB_EID, CFE_EVS_ERROR, "Event Monitor Disable: Error Unsubscribing from Events,RC=0xFFFFFFFF"),
        "Event Monitor Disable: Error Unsubscribing from Events,RC=0xFFFFFFFF");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableEventMonCmd_Test_UnsubscribeError */

void HS_EnableAlivenessCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_ALIVENESS_CC);

    HS_AppData.CurrentAlivenessState = HS_STATE_DISABLED;

    /* Execute the function being tested */
    HS_EnableAlivenessCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_STATE_ENABLED, "HS_AppData.CurrentAlivenessState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_ENABLE_ALIVENESS_DBG_EID, CFE_EVS_DEBUG, "Aliveness Indicator Enabled"),
        "Aliveness Indicator Enabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableAlivenessCmd_Test */

void HS_DisableAlivenessCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_ALIVENESS_CC);

    HS_AppData.CurrentAlivenessState = HS_STATE_ENABLED;

    /* Execute the function being tested */
    HS_DisableAlivenessCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentAlivenessState == HS_STATE_DISABLED, "HS_AppData.CurrentAlivenessState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_ALIVENESS_DBG_EID, CFE_EVS_DEBUG, "Aliveness Indicator Disabled"),
        "Aliveness Indicator Disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableAlivenessCmd_Test */

void HS_EnableCPUHogCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_CPUHOG_CC);

    HS_AppData.CurrentCPUHogState = HS_STATE_DISABLED;

    /* Execute the function being tested */
    HS_EnableCPUHogCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_STATE_ENABLED, "HS_AppData.CurrentCPUHogState == HS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_ENABLE_CPUHOG_DBG_EID, CFE_EVS_DEBUG, "CPU Hogging Indicator Enabled"),
        "CPU Hogging Indicator Enabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_EnableCPUHogCmd_Test */

void HS_DisableCPUHogCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_DISABLE_CPUHOG_CC);

    HS_AppData.CurrentCPUHogState = HS_STATE_ENABLED;

    /* Execute the function being tested */
    HS_DisableCPUHogCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True (HS_AppData.CurrentCPUHogState == HS_STATE_DISABLED, "HS_AppData.CurrentCPUHogState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_DISABLE_CPUHOG_DBG_EID, CFE_EVS_DEBUG, "CPU Hogging Indicator Disabled"),
        "CPU Hogging Indicator Disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_DisableCPUHogCmd_Test */

void HS_ResetResetsPerformedCmd_Test(void)
{
    HS_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_RESET_RESETS_PERFORMED_CC);

    /* Execute the function being tested */
    HS_ResetResetsPerformedCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_RESET_RESETS_DBG_EID, CFE_EVS_DEBUG, "Processor Resets Performed by HS Counter has been Reset"),
        "Processor Resets Performed by HS Counter has been Reset");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_ResetResetsPerformedCmd_Test */

void HS_SetMaxResetsCmd_Test(void)
{
    HS_SetMaxResetsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_SetMaxResetsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_MAX_RESETS_CC);

    CmdPacket.MaxResets = 5;

    /* Execute the function being tested */
    HS_SetMaxResetsCmd((CFE_SB_MsgPtr_t)&CmdPacket);

    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SET_MAX_RESETS_DBG_EID, CFE_EVS_DEBUG, "Max Resets Performable by HS has been set to 5"),
        "Max Resets Performable by HS has been set to 5");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SetMaxResetsCmd_Test */

void HS_VerifyMsgLength_Test_Nominal(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    boolean           Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_ENABLE_CPUHOG_CC);

    /* Execute the function being tested */
    Result = HS_VerifyMsgLength((CFE_SB_MsgPtr_t)&CmdPacket, sizeof(HS_NoArgsCmd_t));

    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_VerifyMsgLength_Test_Nominal */

void HS_VerifyMsgLength_Test_LengthErrorHK(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    boolean           Result;

    CFE_SB_InitMsg (&CmdPacket, HS_SEND_HK_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 0);

    /* Execute the function being tested */
    Result = HS_VerifyMsgLength((CFE_SB_MsgPtr_t)&CmdPacket, sizeof(HS_NoArgsCmd_t));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid HK request msg length: ID = 0x18AF, CC = 0, Len = 1, Expected = 8"),
        "Invalid HK request msg length: ID = 0x18AF, CC = 0, Len = 1, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_VerifyMsgLength_Test_LengthErrorHK */

void HS_VerifyMsgLength_Test_LengthErrorNonHK(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    boolean           Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 0);

    /* Execute the function being tested */
    Result = HS_VerifyMsgLength((CFE_SB_MsgPtr_t)&CmdPacket, sizeof(HS_NoArgsCmd_t));

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x18AE, CC = 0, Len = 1, Expected = 8"),
        "Invalid msg length: ID = 0x18AE, CC = 0, Len = 1, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_VerifyMsgLength_Test_LengthErrorNonHK */

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_AcquirePointers_Test_Nominal(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];

    HS_AppData.AMTablePtr = AMTable;

    /* Satisfies all instances of (Status == CFE_TBL_INFO_UPDATED), skips all (Status < CFE_SUCCESS) blocks */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    HS_AcquirePointers();

    /* Verify results */
    UtAssert_True (HS_AppData.AppMonLoaded == HS_STATE_ENABLED, "HS_AppData.AppMonLoaded == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.EventMonLoaded == HS_STATE_ENABLED, "HS_AppData.EventMonLoaded == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.MsgActsState == HS_STATE_ENABLED, "HS_AppData.MsgActsState == HS_STATE_ENABLED");
    UtAssert_True (HS_AppData.ExeCountState == HS_STATE_ENABLED, "HS_AppData.ExeCountState == HS_STATE_ENABLED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_AcquirePointers_Test_Nominal */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_AcquirePointers_Test_ErrorsWithAppMonLoadedAndEventMonLoadedEnabled(void)
{
    HS_AppData.AppMonLoaded = HS_STATE_ENABLED;
    HS_AppData.EventMonLoaded = HS_STATE_ENABLED;
    HS_AppData.CurrentAppMonState = HS_STATE_DISABLED;
    HS_AppData.CurrentEventMonState = HS_STATE_DISABLED;
    HS_AppData.MsgActsState = HS_STATE_ENABLED;
    HS_AppData.ExeCountState = HS_STATE_ENABLED;

    /* Causes to enter all (Status < CFE_SUCCESS) blocks */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);            
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    HS_AcquirePointers();

    /* Verify results */
    UtAssert_True (HS_AppData.CurrentAppMonState == HS_STATE_DISABLED, "HS_AppData.CurrentAppMonState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.AppMonLoaded == HS_STATE_DISABLED, "HS_AppData.AppMonLoaded == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.EventMonLoaded == HS_STATE_DISABLED , "HS_AppData.EventMonLoaded == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.MsgActsState == HS_STATE_DISABLED , "HS_AppData.MsgActsState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.ExeCountState == HS_STATE_DISABLED , "HS_AppData.ExeCountState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting AppMon Table address, RC=0xFFFFFFFF, Application Monitoring Disabled"),
        "Error getting AppMon Table address, RC=0xFFFFFFFF, Application Monitoring Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting EventMon Table address, RC=0xFFFFFFFF, Event Monitoring Disabled"),
        "Error getting EventMon Table address, RC=0xFFFFFFFF, Event Monitoring Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MSGACTS_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting MsgActs Table address, RC=0xFFFFFFFF"),
        "Error getting MsgActs Table address, RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EXECOUNT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ExeCount Table address, RC=0xFFFFFFFF"),
        "Error getting ExeCount Table address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end HS_AcquirePointers_Test_ErrorsWithAppMonLoadedAndEventMonLoadedEnabled */
#endif

#if HS_MAX_EXEC_CNT_SLOTS != 0
void HS_AcquirePointers_Test_ErrorsWithCurrentAppMonAndCurrentEventMonEnabled(void)
{
    HS_AppData.AppMonLoaded = HS_STATE_DISABLED;
    HS_AppData.EventMonLoaded = HS_STATE_DISABLED;
    HS_AppData.CurrentAppMonState = HS_STATE_ENABLED;
    HS_AppData.CurrentEventMonState = HS_STATE_ENABLED;
    HS_AppData.MsgActsState = HS_STATE_ENABLED;
    HS_AppData.ExeCountState = HS_STATE_ENABLED;

    /* Causes to enter all (Status < CFE_SUCCESS) blocks */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);            
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Causes event message HS_BADEMT_UNSUB_EID to be generated */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_UNSUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    HS_AcquirePointers();

    /* Verify results */
    UtAssert_True (HS_AppData.CurrentAppMonState == HS_STATE_DISABLED, "HS_AppData.CurrentAppMonState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.AppMonLoaded == HS_STATE_DISABLED, "HS_AppData.AppMonLoaded == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.CurrentEventMonState == HS_STATE_DISABLED, "HS_AppData.CurrentEventMonState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.EventMonLoaded == HS_STATE_DISABLED , "HS_AppData.EventMonLoaded == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.MsgActsState == HS_STATE_DISABLED , "HS_AppData.MsgActsState == HS_STATE_DISABLED");
    UtAssert_True (HS_AppData.ExeCountState == HS_STATE_DISABLED , "HS_AppData.ExeCountState == HS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_APPMON_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting AppMon Table address, RC=0xFFFFFFFF, Application Monitoring Disabled"),
        "Error getting AppMon Table address, RC=0xFFFFFFFF, Application Monitoring Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EVENTMON_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting EventMon Table address, RC=0xFFFFFFFF, Event Monitoring Disabled"),
        "Error getting EventMon Table address, RC=0xFFFFFFFF, Event Monitoring Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_BADEMT_UNSUB_EID, CFE_EVS_ERROR, "Error Unsubscribing from Events,RC=0xFFFFFFFF"),
        "Error Unsubscribing from Events,RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_MSGACTS_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting MsgActs Table address, RC=0xFFFFFFFF"),
        "Error getting MsgActs Table address, RC=0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_EXECOUNT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ExeCount Table address, RC=0xFFFFFFFF"),
        "Error getting ExeCount Table address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 5, "Ut_CFE_EVS_GetEventQueueDepth() == 5");

} /* end HS_AcquirePointers_Test_ErrorsWithCurrentAppMonAndCurrentEventMonEnabled */
#endif

void HS_AppMonStatusRefresh_Test_CycleCountZero(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];
    uint32            i;

    HS_AppData.AMTablePtr = AMTable;

    for (i = 0; i <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); i++ )
    {
        HS_AppData.AppMonEnables[i] = 1 + i;

    }

    for (i = 0; i < HS_MAX_MONITORED_APPS; i++ )
    {
        HS_AppData.AMTablePtr[i].CycleCount = 0;
    }

    /* Execute the function being tested */
    HS_AppMonStatusRefresh();

    /* Verify results */
    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == 0, "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == 0, "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == 0");

    UtAssert_True (HS_AppData.AppMonLastExeCount[0] == 0, "HS_AppData.AppMonLastExeCount[0] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0");

    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == 0, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == 0");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS] == 0, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS] == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_AppMonStatusRefresh_Test_CycleCountZero */

void HS_AppMonStatusRefresh_Test_ActionTypeNOACT(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];
    uint32            i;

    HS_AppData.AMTablePtr = AMTable;

    for (i = 0; i <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); i++ )
    {
        HS_AppData.AppMonEnables[i] = 1 + i;

    }

    for (i = 0; i < HS_MAX_MONITORED_APPS; i++ )
    {
        HS_AppData.AMTablePtr[i].ActionType = HS_AMT_ACT_NOACT;
    }

    /* Execute the function being tested */
    HS_AppMonStatusRefresh();

    /* Verify results */
    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.HkPacket.AppMonEnables[0] == 0, "HS_AppData.HkPacket.AppMonEnables[0] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == 0, "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE) / 2] == 0");

    UtAssert_True
        (HS_AppData.HkPacket.AppMonEnables[(HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == 0, "((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE] == 0");

    UtAssert_True (HS_AppData.AppMonLastExeCount[0] == 0, "HS_AppData.AppMonLastExeCount[0] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0");

    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 0, "HS_AppData.AppMonCheckInCountdown[0] == 0");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == 0, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == 0");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS] == 0, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS] == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_AppMonStatusRefresh_Test_ActionTypeNOACT */

void HS_AppMonStatusRefresh_Test_ElseCase(void)
{
    HS_AMTEntry_t     AMTable[HS_MAX_MONITORED_APPS];
    uint32            i;

    HS_AppData.AMTablePtr = AMTable;

    for (i = 0; i <= ((HS_MAX_MONITORED_APPS -1) / HS_BITS_PER_APPMON_ENABLE); i++ )
    {
        HS_AppData.AppMonEnables[i] = 1 + i;

    }

    for (i = 0; i < HS_MAX_MONITORED_APPS; i++ )
    {
        HS_AppData.AMTablePtr[i].CycleCount = 1 + i;
        HS_AppData.AMTablePtr[i].ActionType = 99;
    }

    /* Execute the function being tested */
    HS_AppMonStatusRefresh();

    /* Verify results */
    UtAssert_True (HS_AppData.AppMonLastExeCount[0] == 0, "HS_AppData.AppMonLastExeCount[0] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS / 2] == 0");
    UtAssert_True (HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0, "HS_AppData.AppMonLastExeCount[HS_MAX_MONITORED_APPS] == 0");

    UtAssert_True (HS_AppData.AppMonCheckInCountdown[0] == 1, "HS_AppData.AppMonCheckInCountdown[0] == 1");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == (HS_MAX_MONITORED_APPS / 2) + 1, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS / 2] == (HS_MAX_MONITORED_APPS / 2) + 1");
    UtAssert_True (HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS - 1] == (HS_MAX_MONITORED_APPS - 1) + 1, "HS_AppData.AppMonCheckInCountdown[HS_MAX_MONITORED_APPS] == (HS_MAX_MONITORED_APPS - 1) + 1");

    /* Check first, middle, and last element */
    UtAssert_True (HS_AppData.AppMonEnables[0] == 0xFFFFFFFF, "HS_AppData.AppMonEnables[0] == 0xFFFFFFFF");

    UtAssert_True
        (HS_AppData.AppMonEnables[(((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1) / 2] == 0xFFFFFFFF,
        "HS_AppData.AppMonEnables[(((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1) / 2] == 0xFFFFFFFF");

    UtAssert_True
        (HS_AppData.AppMonEnables[(((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1) - 1] == 0xFFFFFFFF,
        "HS_AppData.AppMonEnables[(((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1) - 1] == 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_AppMonStatusRefresh_Test_ElseCase */

void HS_MsgActsStatusRefresh_Test(void)
{
    uint32  i;

    for (i = 0; i < HS_MAX_MSG_ACT_TYPES; i++ )
    {
        HS_AppData.MsgActCooldown[i] = 1 + i;
    }

    /* Execute the function being tested */
    HS_MsgActsStatusRefresh();

    /* Verify results */
    for (i = 0; i < HS_MAX_MSG_ACT_TYPES; i++ )
    {
    /* Check first, middle, and last element */
        UtAssert_True (HS_AppData.MsgActCooldown[0] == 0, "HS_AppData.MsgActCooldown[0] == 0");
        UtAssert_True (HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES / 2] == 0, "HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES / 2] == 0");
        UtAssert_True (HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES - 1] == 0, "HS_AppData.MsgActCooldown[HS_MAX_MSG_ACT_TYPES -1] == 0");
    }

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MsgActsStatusRefresh_Test */

void HS_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(HS_AppPipe_Test_SendHK, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_SendHK");
    UtTest_Add(HS_AppPipe_Test_Noop, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_Noop");
    UtTest_Add(HS_AppPipe_Test_Reset, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_Reset");
    UtTest_Add(HS_AppPipe_Test_EnableAppMon, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_EnableAppMon");
    UtTest_Add(HS_AppPipe_Test_DisableAppMon, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_DisableAppMon");
    UtTest_Add(HS_AppPipe_Test_EnableEventMon, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_EnableEventMon");
    UtTest_Add(HS_AppPipe_Test_DisableEventMon, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_DisableEventMon");
    UtTest_Add(HS_AppPipe_Test_EnableAliveness, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_EnableAliveness");
    UtTest_Add(HS_AppPipe_Test_DisableAliveness, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_DisableAliveness");
    UtTest_Add(HS_AppPipe_Test_ResetResetsPerformed, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_ResetResetsPerformed");
    UtTest_Add(HS_AppPipe_Test_SetMaxResets, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_SetMaxResets");
    UtTest_Add(HS_AppPipe_Test_EnableCPUHog, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_EnableCPUHog");
    UtTest_Add(HS_AppPipe_Test_DisableCPUHog, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_DisableCPUHog");
    UtTest_Add(HS_AppPipe_Test_InvalidCC, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_InvalidCC");
    UtTest_Add(HS_AppPipe_Test_InvalidMID, HS_Test_Setup, HS_Test_TearDown, "HS_AppPipe_Test_InvalidMID");

    UtTest_Add(HS_HousekeepingReq_Test_InvalidEventMon, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_InvalidEventMon");
#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_HousekeepingReq_Test_AllFlagsEnabled, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_AllFlagsEnabled");
    UtTest_Add(HS_HousekeepingReq_Test_ResourceTypeAppMain, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_ResourceTypeAppMain");
    UtTest_Add(HS_HousekeepingReq_Test_ResourceTypeAppChild, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_ResourceTypeAppChild");
    UtTest_Add(HS_HousekeepingReq_Test_ResourceTypeDevice, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_ResourceTypeDevice");
    UtTest_Add(HS_HousekeepingReq_Test_ResourceTypeISR, HS_Test_Setup, HS_Test_TearDown, "HS_HousekeepingReq_Test_ResourceTypeISR");
#endif

    UtTest_Add(HS_Noop_Test, HS_Test_Setup, HS_Test_TearDown, "HS_Noop_Test");

    UtTest_Add(HS_ResetCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_ResetCmd_Test");

    UtTest_Add(HS_ResetCounters_Test, HS_Test_Setup, HS_Test_TearDown, "HS_ResetCounters_Test");

    UtTest_Add(HS_EnableAppMonCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_EnableAppMonCmd_Test");

    UtTest_Add(HS_DisableAppMonCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_DisableAppMonCmd_Test");

    UtTest_Add(HS_EnableEventMonCmd_Test_Disabled, HS_Test_Setup, HS_Test_TearDown, "HS_EnableEventMonCmd_Test_Disabled");
    UtTest_Add(HS_EnableEventMonCmd_Test_AlreadyEnabled, HS_Test_Setup, HS_Test_TearDown, "HS_EnableEventMonCmd_Test_AlreadyEnabled");
    UtTest_Add(HS_EnableEventMonCmd_Test_SubscribeError, HS_Test_Setup, HS_Test_TearDown, "HS_EnableEventMonCmd_Test_SubscribeError");

    UtTest_Add(HS_DisableEventMonCmd_Test_Enabled, HS_Test_Setup, HS_Test_TearDown, "HS_DisableEventMonCmd_Test_Enabled");
    UtTest_Add(HS_DisableEventMonCmd_Test_AlreadyDisabled, HS_Test_Setup, HS_Test_TearDown, "HS_DisableEventMonCmd_Test_AlreadyDisabled");
    UtTest_Add(HS_DisableEventMonCmd_Test_UnsubscribeError, HS_Test_Setup, HS_Test_TearDown, "HS_DisableEventMonCmd_Test_UnsubscribeError");

    UtTest_Add(HS_EnableAlivenessCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_EnableAlivenessCmd_Test");

    UtTest_Add(HS_DisableAlivenessCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_DisableAlivenessCmd_Test");

    UtTest_Add(HS_EnableCPUHogCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_EnableCPUHogCmd_Test");

    UtTest_Add(HS_DisableCPUHogCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_DisableCPUHogCmd_Test");

    UtTest_Add(HS_ResetResetsPerformedCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_ResetResetsPerformedCmd_Test");

    UtTest_Add(HS_SetMaxResetsCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_SetMaxResetsCmd_Test");

    UtTest_Add(HS_VerifyMsgLength_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_VerifyMsgLength_Test_Nominal");
    UtTest_Add(HS_VerifyMsgLength_Test_LengthErrorHK, HS_Test_Setup, HS_Test_TearDown, "HS_VerifyMsgLength_Test_LengthErrorHK");
    UtTest_Add(HS_VerifyMsgLength_Test_LengthErrorNonHK, HS_Test_Setup, HS_Test_TearDown, "HS_VerifyMsgLength_Test_LengthErrorNonHK");

#if HS_MAX_EXEC_CNT_SLOTS != 0
    UtTest_Add(HS_AcquirePointers_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_AcquirePointers_Test_Nominal");
    UtTest_Add(HS_AcquirePointers_Test_ErrorsWithAppMonLoadedAndEventMonLoadedEnabled, HS_Test_Setup, HS_Test_TearDown, "HS_AcquirePointers_Test_ErrorsWithAppMonLoadedAndEventMonLoadedEnabled");
    UtTest_Add(HS_AcquirePointers_Test_ErrorsWithCurrentAppMonAndCurrentEventMonEnabled, HS_Test_Setup, HS_Test_TearDown, "HS_AcquirePointers_Test_ErrorsWithCurrentAppMonAndCurrentEventMonEnabled");
#endif

    UtTest_Add(HS_AppMonStatusRefresh_Test_CycleCountZero, HS_Test_Setup, HS_Test_TearDown, "HS_AppMonStatusRefresh_Test_CycleCountZero");
    UtTest_Add(HS_AppMonStatusRefresh_Test_ActionTypeNOACT, HS_Test_Setup, HS_Test_TearDown, "HS_AppMonStatusRefresh_Test_ActionTypeNOACT");
    UtTest_Add(HS_AppMonStatusRefresh_Test_ElseCase, HS_Test_Setup, HS_Test_TearDown, "HS_AppMonStatusRefresh_Test_ElseCase");

    UtTest_Add(HS_MsgActsStatusRefresh_Test, HS_Test_Setup, HS_Test_TearDown, "HS_MsgActsStatusRefresh_Test");

} /* end HS_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
