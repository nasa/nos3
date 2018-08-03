 /*************************************************************************
 ** File:
 **   $Id: cs_app_cmds_test.c 1.4 2017/03/29 17:29:01EDT mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file cs_app_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_app_cmds_test.h"
#include "cs_app_cmds.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_test_utils.h"
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

void CS_DisableAppCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.AppCSState == CS_STATE_DISABLED, "CS_AppData.AppCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_APP_INF_EID, CFE_EVS_INFORMATION, "Checksumming of App is Disabled"),
        "Checksumming of App is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableAppCmd_Test */

void CS_EnableAppCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.AppCSState == CS_STATE_ENABLED, "CS_AppData.AppCSState == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_APP_INF_EID, CFE_EVS_INFORMATION, "Checksumming of App is Enabled"),
        "Checksumming of App is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableAppCmd_Test */

void CS_ReportBaselineAppCmd_Test_Baseline(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State             = 1;
    CS_AppData.ResAppTblPtr->ComputedYet       = TRUE;
    CS_AppData.ResAppTblPtr->ComparisonValue   = 1;

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_APP_INF_EID, CFE_EVS_INFORMATION, "Report baseline of app App1 is 0x00000001"),
        "Report baseline of app App1 is 0x00000001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineAppCmd_Test_Baseline */

void CS_ReportBaselineAppCmd_Test_NoBaseline(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State             = 1;
    CS_AppData.ResAppTblPtr->ComputedYet       = FALSE;

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_APP_INF_EID, CFE_EVS_INFORMATION, "Report baseline of app App1 has not been computed yet"),
        "Report baseline of app App1 has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineAppCmd_Test_NoBaseline */

void CS_ReportBaselineAppCmd_Test_BaselineInvalidName(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_NAME_APP_ERR_EID, CFE_EVS_ERROR, "App report baseline failed, app App1 not found"),
        "App report baseline failed, app App1 not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineAppCmd_Test_BaselineInvalidName */

void CS_RecomputeBaselineAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to generate event message CS_RECOMPUTE_APP_STARTED_DBG_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr, "CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_APP_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute baseline of app App1 started"),
        "Recompute baseline of app App1 started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineAppCmd_Test_Nominal */

void CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to generate event message CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr, "CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of app App1 failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF"),
        "Recompute baseline of app App1 failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID, CFE_EVS_ERROR, "App recompute baseline failed, app App1 not found"),
        "App recompute baseline failed, app App1 not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineAppCmd_Test_UnknownNameError */

void CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_APP_CHDTASK_ERR_EID, CFE_EVS_ERROR, "App recompute baseline for app App1 failed: child task in use"),
        "App recompute baseline for app App1 failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress */

void CS_DisableNameAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.DefAppTblPtr->State = 1;

    /* Execute the function being tested */
    CS_DisableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_APP_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of app App1 is Disabled"),
        "Checksumming of app App1 is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableNameAppCmd_Test_Nominal */

void CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to make subfunction CS_GetAppDefTblEntryByName return FALSE */
    CS_AppData.DefAppTblPtr->State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_APP_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of app App1 is Disabled"),
        "Checksumming of app App1 is Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID, CFE_EVS_DEBUG, "CS unable to update apps definition table for entry App1"),
        "CS unable to update apps definition table for entry App1");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError */

void CS_DisableNameAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Set to make subfunction CS_GetAppResTblEntryByName return FALSE */
    CS_AppData.ResAppTblPtr->State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID, CFE_EVS_ERROR, "App disable app command failed, app App1 not found"),
        "App disable app command failed, app App1 not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableNameAppCmd_Test_UnknownNameError */

void CS_EnableNameAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.DefAppTblPtr->State = 1;

    /* Execute the function being tested */
    CS_EnableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_APP_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of app App1 is Enabled"),
        "Checksumming of app App1 is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableNameAppCmd_Test_Nominal */

void CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to make subfunction CS_GetAppDefTblEntryByName return FALSE */
    CS_AppData.DefAppTblPtr->State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_APP_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of app App1 is Enabled"),
        "Checksumming of app App1 is Enabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID, CFE_EVS_DEBUG, "CS unable to update apps definition table for entry App1"),
        "CS unable to update apps definition table for entry App1");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError */

void CS_EnableNameAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t            CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_AppNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Set to make subfunction CS_GetAppResTblEntryByName return FALSE */
    CS_AppData.ResAppTblPtr->State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableNameAppCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID, CFE_EVS_ERROR, "App enable app command failed, app App1 not found"),
        "App enable app command failed, app App1 not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableNameAppCmd_Test_UnknownNameError */

void CS_App_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(CS_DisableAppCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAppCmd_Test");

    UtTest_Add(CS_EnableAppCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAppCmd_Test");

    UtTest_Add(CS_ReportBaselineAppCmd_Test_Baseline, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineAppCmd_Test_Baseline");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_NoBaseline, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineAppCmd_Test_NoBaseline");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_BaselineInvalidName, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineAppCmd_Test_BaselineInvalidName");

    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineAppCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineAppCmd_Test_UnknownNameError");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress");

    UtTest_Add(CS_DisableNameAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameAppCmd_Test_Nominal");
    UtTest_Add(CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError");
    UtTest_Add(CS_DisableNameAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameAppCmd_Test_UnknownNameError");

    UtTest_Add(CS_EnableNameAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameAppCmd_Test_Nominal");
    UtTest_Add(CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError");
    UtTest_Add(CS_EnableNameAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameAppCmd_Test_UnknownNameError");

} /* end CS_App_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
