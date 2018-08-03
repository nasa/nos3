 /*************************************************************************
 ** File:
 **   $Id: hs_custom_test.c 1.4 2016/09/07 19:17:19EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hs_custom.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_custom_test.c  $
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

#include "hs_custom_test.h"
#include "hs_app.h"
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

void HS_CUSTOM_TEST_CFE_ES_IncrementTaskCounterHook(void)
{
    HS_CustomData.IdleTaskRunStatus = -1;
}

void HS_IdleTask_Test(void)
{
    HS_CustomData.UtilMask = 1;
    HS_CustomData.ThisIdleTaskExec = 1;
    HS_CustomData.ThisIdleTaskExec = 2;
    HS_CustomData.UtilArrayIndex = 0;

    /* Set to make the while loop exit after the first run */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_INCREMENTTASKCOUNTER_INDEX, &HS_CUSTOM_TEST_CFE_ES_IncrementTaskCounterHook);

    /* Execute the function being tested */
    HS_IdleTask();
    
    /* Verify results */
    UtAssert_True (HS_CustomData.UtilArray[0] == 0, "HS_CustomData.UtilArray[0] == 0");
    UtAssert_True (HS_CustomData.UtilArrayIndex == 1, "HS_CustomData.UtilArrayIndex == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_IdleTask_Test */

void HS_CustomInit_Test_Nominal(void)
{
    int32  Result;

    /* No setup required for this test */

    /* Execute the function being tested */
    Result = HS_CustomInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (HS_CustomData.UtilMult1 == HS_UTIL_CONV_MULT1, "HS_CustomData.UtilMult1 == HS_UTIL_CONV_MULT1");
    UtAssert_True (HS_CustomData.UtilMult2 == HS_UTIL_CONV_MULT2, "HS_CustomData.UtilMult2 == HS_UTIL_CONV_MULT2");
    UtAssert_True (HS_CustomData.UtilDiv == HS_UTIL_CONV_DIV, "HS_CustomData.UtilDiv == HS_UTIL_CONV_DIV");
    UtAssert_True (HS_CustomData.UtilCycleCounter == 0, "HS_CustomData.UtilCycleCounter == 0");
    UtAssert_True (HS_CustomData.UtilMask == HS_UTIL_DIAG_MASK, "HS_CustomData.UtilMask == HS_UTIL_DIAG_MASK");
    UtAssert_True (HS_CustomData.UtilArrayIndex == 0, "HS_CustomData.UtilArrayIndex == 0");
    UtAssert_True (HS_CustomData.UtilArrayMask == HS_UTIL_TIME_DIAG_ARRAY_MASK, "HS_CustomData.UtilArrayMask == HS_UTIL_TIME_DIAG_ARRAY_MASK");
    UtAssert_True (HS_CustomData.ThisIdleTaskExec == 0, "HS_CustomData.ThisIdleTaskExec == 0");
    UtAssert_True (HS_CustomData.LastIdleTaskExec == 0, "HS_CustomData.LastIdleTaskExec == 0");
    UtAssert_True (HS_CustomData.LastIdleTaskInterval == 0, "HS_CustomData.LastIdleTaskInterval == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_CustomInit_Test_Nominal */

void HS_CustomInit_Test_CreateChildTaskError(void)
{
    int32  Result;

    /* Causes event message to be generated */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_CustomInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_CHILD_TASK_ERR_EID, CFE_EVS_ERROR, "Error Creating Child Task for CPU Utilization Monitoring,RC=0xFFFFFFFF"),
        "Error Creating Child Task for CPU Utilization Monitoring,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_CustomInit_Test_CreateChildTaskError */

void HS_CustomInit_Test_RegisterSynchCallbackError(void)
{
    int32  Result;

    /* Causes event message to be generated */
    Ut_CFE_TIME_SetReturnCode(UT_CFE_TIME_REGISTERSYNCHCALLBACK_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HS_CustomInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (HS_CustomData.UtilMult1 == HS_UTIL_CONV_MULT1, "HS_CustomData.UtilMult1 == HS_UTIL_CONV_MULT1");
    UtAssert_True (HS_CustomData.UtilMult2 == HS_UTIL_CONV_MULT2, "HS_CustomData.UtilMult2 == HS_UTIL_CONV_MULT2");
    UtAssert_True (HS_CustomData.UtilDiv == HS_UTIL_CONV_DIV, "HS_CustomData.UtilDiv == HS_UTIL_CONV_DIV");
    UtAssert_True (HS_CustomData.UtilCycleCounter == 0, "HS_CustomData.UtilCycleCounter == 0");
    UtAssert_True (HS_CustomData.UtilMask == HS_UTIL_DIAG_MASK, "HS_CustomData.UtilMask == HS_UTIL_DIAG_MASK");
    UtAssert_True (HS_CustomData.UtilArrayIndex == 0, "HS_CustomData.UtilArrayIndex == 0");
    UtAssert_True (HS_CustomData.UtilArrayMask == HS_UTIL_TIME_DIAG_ARRAY_MASK, "HS_CustomData.UtilArrayMask == HS_UTIL_TIME_DIAG_ARRAY_MASK");
    UtAssert_True (HS_CustomData.ThisIdleTaskExec == 0, "HS_CustomData.ThisIdleTaskExec == 0");
    UtAssert_True (HS_CustomData.LastIdleTaskExec == 0, "HS_CustomData.LastIdleTaskExec == 0");
    UtAssert_True (HS_CustomData.LastIdleTaskInterval == 0, "HS_CustomData.LastIdleTaskInterval == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_CR_SYNC_CALLBACK_ERR_EID, CFE_EVS_ERROR, "Error Registering Sync Callback for CPU Utilization Monitoring,RC=0xFFFFFFFF"),
        "Error Registering Sync Callback for CPU Utilization Monitoring,RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_CustomInit_Test_RegisterSynchCallbackError */

void HS_CustomCleanup_Test(void)
{
    /* No setup required for this test */

    /* Execute the function being tested */
    HS_CustomCleanup();
    
    /* Verify results */
    UtAssert_True (HS_CustomData.IdleTaskRunStatus == !CFE_SUCCESS, "HS_CustomData.IdleTaskRunStatus == !CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_CustomCleanup_Test */

void HS_UtilizationIncrement_Test(void)
{
    /* No setup required for this test */

    /* Execute the function being tested */
    HS_UtilizationIncrement();
    
    /* Verify results */
    UtAssert_True (HS_CustomData.ThisIdleTaskExec == 1, "HS_CustomData.ThisIdleTaskExec == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_UtilizationIncrement_Test */

void HS_UtilizationMark_Test(void)
{
    HS_CustomData.ThisIdleTaskExec = 3;
    HS_CustomData.LastIdleTaskExec = 1;

    /* Execute the function being tested */
    HS_UtilizationMark();
    
    /* Verify results */
    UtAssert_True (HS_CustomData.LastIdleTaskInterval == 2, "HS_CustomData.LastIdleTaskInterval == 2");
    UtAssert_True (HS_CustomData.LastIdleTaskExec == 3, "HS_CustomData.LastIdleTaskExec == 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_UtilizationMark_Test */

void HS_MarkIdleCallback_Test(void)
{
    /* No setup required for this test */

    /* Execute the function being tested */
    HS_MarkIdleCallback();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_MarkIdleCallback_Test */

void HS_CustomMonitorUtilization_Test(void)
{
    HS_CustomData.UtilCycleCounter = 0;

    /* Execute the function being tested */
    HS_CustomMonitorUtilization();
    
    /* Verify results */
    UtAssert_True (HS_CustomData.UtilCycleCounter == 0, "HS_CustomData.UtilCycleCounter == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_CustomMonitorUtilization_Test */

void HS_CustomCommands_Test_UtilDiagReport(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    int32             Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_REPORT_DIAG_CC);

    /* Execute the function being tested */
    Result = HS_CustomCommands((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_CustomCommands_Test_UtilDiagReport */

void HS_CustomCommands_Test_SetUtilParamsCmd(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    int32             Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_UTIL_PARAMS_CC);

    /* Execute the function being tested */
    Result = HS_CustomCommands((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_CustomCommands_Test_SetUtilParamsCmd */

void HS_CustomCommands_Test_SetUtilDiagCmd(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    int32             Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_UTIL_DIAG_CC);

    /* Execute the function being tested */
    Result = HS_CustomCommands((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about in this test */

} /* end HS_CustomCommands_Test_SetUtilDiagCmd */

void HS_CustomCommands_Test_InvalidCommandCode(void)
{
    HS_NoArgsCmd_t    CmdPacket;
    int32             Result;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    Result = HS_CustomCommands((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (Result == !CFE_SUCCESS, "Result == !CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_CustomCommands_Test_InvalidCommandCode */

void HS_UtilDiagReport_Test(void)
{
    uint32   i;

    HS_CustomData.UtilArray[0] = 0xFFFFFFFF;
    HS_CustomData.UtilArray[1] = 0x00000111;

    /* Sets all other elements to 0 */
    for(i = 2; i < HS_UTIL_TIME_DIAG_ARRAY_LENGTH; i++)
    {
        HS_CustomData.UtilArray[i] = 0;
    }

    HS_CustomData.UtilMask = 0xFFFFFFFE;

    /* Execute the function being tested */
    HS_UtilDiagReport();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_UTIL_DIAG_REPORT_EID, CFE_EVS_INFORMATION, "Mask 0xFFFFFFFE Base Time Ticks per Idle Ticks (frequency): 0(13), 274(2), -273(1), -1(0)"),
        "Mask 0xFFFFFFFE Base Time Ticks per Idle Ticks (frequency): 0(13), 274(2), -273(1), -1(0)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_UtilDiagReport_Test */

void HS_CustomGetUtil_Test(void)
{
    int32   Result;

    HS_CustomData.LastIdleTaskInterval = 9999;
    HS_CustomData.UtilMult1 = 1;
    HS_CustomData.UtilDiv = 1;
    HS_CustomData.UtilMult2 = 1;

    /* Execute the function being tested */
    Result = HS_CustomGetUtil();
    
    /* Verify results */
    UtAssert_True (Result == 1, "Result == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HS_CustomGetUtil_Test */

void HS_SetUtilParamsCmd_Test_Nominal(void)
{
    HS_SetUtilParamsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_SetUtilParamsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_UTIL_PARAMS_CC);

    CmdPacket.Mult1 = 1;
    CmdPacket.Mult2 = 2;
    CmdPacket.Div   = 3;

    /* Execute the function being tested */
    HS_SetUtilParamsCmd((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_CustomData.UtilMult1 == 1, "HS_CustomData.UtilMult1 == 1");
    UtAssert_True (HS_CustomData.UtilMult2 == 2, "HS_CustomData.UtilMult2 == 2");
    UtAssert_True (HS_CustomData.UtilDiv == 3, "HS_CustomData.UtilDiv == 3");
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SET_UTIL_PARAMS_DBG_EID, CFE_EVS_DEBUG, "Utilization Parms set: Mult1: 1 Div: 3 Mult2: 2"),
        "Utilization Parms set: Mult1: 1 Div: 3 Mult2: 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SetUtilParamsCmd_Test_Nominal */

void HS_SetUtilParamsCmd_Test_Error(void)
{
    HS_SetUtilParamsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_SetUtilParamsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_UTIL_PARAMS_CC);

    CmdPacket.Mult1 = 0;
    CmdPacket.Mult2 = 2;
    CmdPacket.Div   = 3;

    /* Execute the function being tested */
    HS_SetUtilParamsCmd((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.CmdErrCount == 1, "HS_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SET_UTIL_PARAMS_ERR_EID, CFE_EVS_ERROR, "Utilization Parms Error: No parameter may be 0: Mult1: 0 Div: 3 Mult2: 2"),
        "Utilization Parms Error: No parameter may be 0: Mult1: 0 Div: 3 Mult2: 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SetUtilParamsCmd_Test_Error */

void HS_SetUtilDiagCmd_Test(void)
{
    HS_SetUtilDiagCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, HS_CMD_MID, sizeof(HS_SetUtilDiagCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HS_SET_UTIL_DIAG_CC);

    CmdPacket.Mask = 2;

    /* Execute the function being tested */
    HS_SetUtilDiagCmd((CFE_SB_MsgPtr_t)&CmdPacket);
    
    /* Verify results */
    UtAssert_True (HS_AppData.CmdCount == 1, "HS_AppData.CmdCount == 1");
    UtAssert_True (HS_CustomData.UtilMask == 2, "HS_AppData.CmdCount == 2");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HS_SET_UTIL_DIAG_DBG_EID, CFE_EVS_DEBUG, "Utilization Diagnostics Mask has been set to 00000002"),
        "Utilization Diagnostics Mask has been set to 00000002");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HS_SetUtilDiagCmd_Test */

void HS_Custom_Test_AddTestCases(void)
{
    UtTest_Add(HS_IdleTask_Test, HS_Test_Setup, HS_Test_TearDown, "HS_IdleTask_Test");

    UtTest_Add(HS_CustomInit_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_CustomInit_Test_Nominal");
    UtTest_Add(HS_CustomInit_Test_CreateChildTaskError, HS_Test_Setup, HS_Test_TearDown, "HS_CustomInit_Test_CreateChildTaskError");
    UtTest_Add(HS_CustomInit_Test_RegisterSynchCallbackError, HS_Test_Setup, HS_Test_TearDown, "HS_CustomInit_Test_RegisterSynchCallbackError");

    UtTest_Add(HS_CustomCleanup_Test, HS_Test_Setup, HS_Test_TearDown, "HS_CustomCleanup_Test");

    UtTest_Add(HS_UtilizationIncrement_Test, HS_Test_Setup, HS_Test_TearDown, "HS_UtilizationIncrement_Test");

    UtTest_Add(HS_UtilizationMark_Test, HS_Test_Setup, HS_Test_TearDown, "HS_UtilizationMark_Test");

    UtTest_Add(HS_MarkIdleCallback_Test, HS_Test_Setup, HS_Test_TearDown, "HS_MarkIdleCallback_Test");

    UtTest_Add(HS_CustomMonitorUtilization_Test, HS_Test_Setup, HS_Test_TearDown, "HS_CustomMonitorUtilization_Test");

    UtTest_Add(HS_CustomCommands_Test_UtilDiagReport, HS_Test_Setup, HS_Test_TearDown, "HS_CustomCommands_Test_UtilDiagReport");
    UtTest_Add(HS_CustomCommands_Test_SetUtilParamsCmd, HS_Test_Setup, HS_Test_TearDown, "HS_CustomCommands_Test_SetUtilParamsCmd");
    UtTest_Add(HS_CustomCommands_Test_SetUtilDiagCmd, HS_Test_Setup, HS_Test_TearDown, "HS_CustomCommands_Test_SetUtilDiagCmd");
    UtTest_Add(HS_CustomCommands_Test_InvalidCommandCode, HS_Test_Setup, HS_Test_TearDown, "HS_CustomCommands_Test_InvalidCommandCode");

    UtTest_Add(HS_UtilDiagReport_Test, HS_Test_Setup, HS_Test_TearDown, "HS_UtilDiagReport_Test");

    UtTest_Add(HS_CustomGetUtil_Test, HS_Test_Setup, HS_Test_TearDown, "HS_CustomGetUtil_Test");

    UtTest_Add(HS_SetUtilParamsCmd_Test_Nominal, HS_Test_Setup, HS_Test_TearDown, "HS_SetUtilParamsCmd_Test_Nominal");
    UtTest_Add(HS_SetUtilParamsCmd_Test_Error, HS_Test_Setup, HS_Test_TearDown, "HS_SetUtilParamsCmd_Test_Error");

    UtTest_Add(HS_SetUtilDiagCmd_Test, HS_Test_Setup, HS_Test_TearDown, "HS_SetUtilDiagCmd_Test");

} /* end HS_Custom_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
