 /*************************************************************************
 ** File:
 **   $Id: cs_cmds_test.c 1.6 2017/03/29 17:28:58EDT mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_cmds_test.h"
#include "cs_cmds.h"
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
#include "ut_cfe_psp_memrange_stubs.h"
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

int32 CS_CMDS_TEST_CFE_ES_CreateChildTaskHook(uint32                          *TaskIdPtr,
                                              const char                      *TaskName,
                                              CFE_ES_ChildTaskMainFuncPtr_t    FunctionPtr,
                                              uint32                          *StackPtr,
                                              uint32                           StackSize,
                                              uint32                           Priority,
                                              uint32                           Flags)
{
    *TaskIdPtr = 5;

    return CFE_SUCCESS;
}

void CS_NoopCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;
    char             Message[125];

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_NoopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command. Version %d.%d.%d.%d", CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(CS_NOOP_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_NoopCmd_Test */

void CS_ResetCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

        CS_AppData.CmdCounter          = 1;
        CS_AppData.CmdErrCounter       = 2;
        CS_AppData.EepromCSErrCounter  = 3;
        CS_AppData.MemoryCSErrCounter  = 4;
        CS_AppData.TablesCSErrCounter  = 5;
        CS_AppData.AppCSErrCounter     = 6;
        CS_AppData.CfeCoreCSErrCounter = 7;
        CS_AppData.OSCSErrCounter      = 8;
        CS_AppData.PassCounter         = 9;     

    /* Execute the function being tested */
    CS_ResetCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.CmdCounter == 0, "CS_AppData.CmdCounter == 0");
    UtAssert_True (CS_AppData.CmdErrCounter == 0, "CS_AppData.CmdErrCounter == 0");
    UtAssert_True (CS_AppData.EepromCSErrCounter == 0, "CS_AppData.EepromCSErrCounter == 0");
    UtAssert_True (CS_AppData.MemoryCSErrCounter == 0, "CS_AppData.MemoryCSErrCounter == 0");
    UtAssert_True (CS_AppData.TablesCSErrCounter == 0, "CS_AppData.TablesCSErrCounter == 0");
    UtAssert_True (CS_AppData.AppCSErrCounter == 0, "CS_AppData.AppCSErrCounter == 0");
    UtAssert_True (CS_AppData.CfeCoreCSErrCounter == 0, "CS_AppData.CfeCoreCSErrCounter == 0");
    UtAssert_True (CS_AppData.OSCSErrCounter == 0, "CS_AppData.OSCSErrCounter == 0");
    UtAssert_True (CS_AppData.PassCounter == 0, "CS_AppData.PassCounter == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RESET_DBG_EID, CFE_EVS_DEBUG, "Reset Counters command recieved"),
        "Reset Counters command recieved");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ResetCmd_Test */

void CS_BackgroundCheckCmd_Test_InvalidMsgLength(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, 10, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 1);

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x189F, CC = 1, Len = 10, Expected = 8"),
        "Invalid msg length: ID = 0x189F, CC = 1, Len = 10, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundCheckCmd_Test_InvalidMsgLength */

void CS_BackgroundCheckCmd_Test_BackgroundCfeCore(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_CFECORE;
    CS_AppData.CfeCoreCSState       = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundCfeCore */

void CS_BackgroundCheckCmd_Test_BackgroundOS(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_OSCORE;
    CS_AppData.OSCSState            = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.State      = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundOS */

void CS_BackgroundCheckCmd_Test_BackgroundEeprom(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_EEPROM_TABLE;
    CS_AppData.EepromCSState        = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundEeprom */

void CS_BackgroundCheckCmd_Test_BackgroundMemory(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_MEMORY_TABLE;
    CS_AppData.MemoryCSState        = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundMemory */

void CS_BackgroundCheckCmd_Test_BackgroundTables(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_TABLES_TABLE;
    CS_AppData.TablesCSState        = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundTables */

void CS_BackgroundCheckCmd_Test_BackgroundApp(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = CS_APP_TABLE;
    CS_AppData.AppCSState           = CS_STATE_ENABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_BackgroundApp */

void CS_BackgroundCheckCmd_Test_Default(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState        = CS_STATE_ENABLED;
    CS_AppData.CurrentCSTable       = 99;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.CurrentCSTable == 0, "CS_AppData.CurrentCSTable == 0");
    UtAssert_True (CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0");
    UtAssert_True (CS_AppData.PassCounter == 1, "CS_AppData.PassCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_Default */

void CS_BackgroundCheckCmd_Test_Disabled(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.ChecksumState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.PassCounter == 0, "CS_AppData.PassCounter == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCheckCmd_Test_Disabled */


void CS_DisableAllCSCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableAllCSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ChecksumState == CS_STATE_DISABLED, "CS_AppData.ChecksumState == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_ALL_INF_EID, CFE_EVS_INFORMATION, "Background Checksumming Disabled"),
        "Background Checksumming Disabled");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableAllCSCmd_Test */

void CS_EnableAllCSCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableAllCSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ChecksumState == CS_STATE_ENABLED, "CS_AppData.ChecksumState == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_ALL_INF_EID, CFE_EVS_INFORMATION, "Background Checksumming Enabled"),
        "Background Checksumming Enabled");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableAllCSCmd_Test */

void CS_DisableCfeCoreCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_STATE_DISABLED, "CS_AppData.CfeCoreCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_CFECORE_INF_EID, CFE_EVS_INFORMATION, "Checksumming of cFE Core is Disabled"),
        "Checksumming of cFE Core is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableCfeCoreCmd_Test */

void CS_EnableCfeCoreCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.CfeCoreCSState == CS_STATE_ENABLED, "CS_AppData.CfeCoreCSState == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_CFECORE_INF_EID, CFE_EVS_INFORMATION, "Checksumming of cFE Core is Enabled"),
        "Checksumming of cFE Core is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableCfeCoreCmd_Test */

void CS_DisableOSCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OSCSState == CS_STATE_DISABLED, "CS_AppData.OSCSState == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_OS_INF_EID, CFE_EVS_INFORMATION, "Checksumming of OS code segment is Disabled"),
        "Checksumming of OS code segment is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableOSCmd_Test */

void CS_EnableOSCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OSCSState == CS_STATE_ENABLED, "CS_AppData.OSCSState == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_OS_INF_EID, CFE_EVS_INFORMATION, "Checksumming of OS code segment is Enabled"),
        "Checksumming of OS code segment is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableOSCmd_Test */

void CS_ReportBaselineCfeCoreCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.CfeCoreCodeSeg.ComputedYet     = TRUE;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = -1;

    /* Execute the function being tested */
    CS_ReportBaselineCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_CFECORE_INF_EID, CFE_EVS_INFORMATION, "Baseline of cFE Core is 0xFFFFFFFF"),
        "Baseline of cFE Core is 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineCfeCoreCmd_Test_Nominal */

void CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.CfeCoreCodeSeg.ComputedYet = FALSE;

    /* Execute the function being tested */
    CS_ReportBaselineCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_CFECORE_INF_EID, CFE_EVS_INFORMATION, "Baseline of cFE Core has not been computed yet"),
        "Baseline of cFE Core has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet */

void CS_ReportBaselineOSCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.OSCodeSeg.ComputedYet     = TRUE;
    CS_AppData.OSCodeSeg.ComparisonValue = -1;

    /* Execute the function being tested */
    CS_ReportBaselineOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_OS_INF_EID, CFE_EVS_INFORMATION, "Baseline of OS code segment is 0xFFFFFFFF"),
        "Baseline of OS code segment is 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineOSCmd_Test_Nominal */

void CS_ReportBaselineOSCmd_Test_NotComputedYet(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.OSCodeSeg.ComputedYet = FALSE;

    /* Execute the function being tested */
    CS_ReportBaselineOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_OS_INF_EID, CFE_EVS_INFORMATION, "Baseline of OS code segment has not been computed yet"),
        "Baseline of OS code segment has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineOSCmd_Test_NotComputedYet */

void CS_RecomputeBaselineCfeCoreCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == TRUE, "CS_AppData.RecomputeInProgress == TRUE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskTable == CS_CFECORE, "CS_AppData.ChildTaskTable == CS_CFECORE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == 0, "CS_AppData.ChildTaskEntryID == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg, "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute of cFE core started"),
        "Recompute of cFE core started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineCfeCoreCmd_Test_Nominal */

void CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Set to generate error message CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskTable == CS_CFECORE, "CS_AppData.ChildTaskTable == CS_CFECORE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == 0, "CS_AppData.ChildTaskEntryID == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg, "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF"),
        "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute cFE core failed: child task in use"),
        "Recompute cFE core failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError */

void CS_RecomputeBaselineOSCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == TRUE, "CS_AppData.RecomputeInProgress == TRUE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskTable == CS_OSCORE, "CS_AppData.ChildTaskTable == CS_OSCORE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == 0, "CS_AppData.OSCodeSeg == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg, "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_OS_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute of OS code segment started"),
        "Recompute of OS code segment started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineOSCmd_Test_Nominal */

void CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Set to generate error message CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskTable == CS_OSCORE, "CS_AppData.ChildTaskTable == CS_OSCORE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == 0, "CS_AppData.OSCodeSeg == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg, "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF"),
        "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineOSCmd_Test_ChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_OS_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute OS code segment failed: child task in use"),
        "Recompute OS code segment failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineOSCmd_Test_ChildTaskError */

void CS_OneShotCmd_Test_Nominal(void)
{
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CmdPacket.Address = 0x00000001;
    CmdPacket.Size    = 2;
    CmdPacket.MaxBytesPerCycle = 0;

    CS_AppData.RecomputeInProgress = FALSE;
    CS_AppData.MaxBytesPerCycle = 8;

    /* Sets ChildTaskID to 5 and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_CREATECHILDTASK_INDEX, &CS_CMDS_TEST_CFE_ES_CreateChildTaskHook);

    /* Execute the function being tested */
    CS_OneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == TRUE, "CS_AppData.OneShotInProgress == TRUE");

    UtAssert_True (CS_AppData.LastOneShotAddress == CmdPacket.Address, "CS_AppData.LastOneShotAddress == CmdPacket.Address");
    UtAssert_True (CS_AppData.LastOneShotSize == CmdPacket.Size, "CS_AppData.LastOneShotSize == CmdPacket.Size");
    UtAssert_True (CS_AppData.LastOneShotChecksum == 0, "CS_AppData.LastOneShotChecksum == 0");
    UtAssert_True (CS_AppData.LastOneShotMaxBytesPerCycle == CS_AppData.MaxBytesPerCycle, "CS_AppData.LastOneShotMaxBytesPerCycle == CS_AppData.MaxBytesPerCycle");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_STARTED_DBG_EID, CFE_EVS_DEBUG, "OneShot checksum started on address: 0x00000001, size: 2"),
        "OneShot checksum started on address: 0x00000001, size: 2");

    UtAssert_True (CS_AppData.ChildTaskID == 5, "CS_AppData.ChildTaskID == 5");
    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotCmd_Test_Nominal */


void CS_OneShotCmd_Test_MaxBytesPerCycleNonZero(void)
{
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CmdPacket.Address = 0x00000001;
    CmdPacket.Size    = 2;
    CmdPacket.MaxBytesPerCycle = 1;

    CS_AppData.RecomputeInProgress = FALSE;
    CS_AppData.MaxBytesPerCycle = 8;

    /* Sets ChildTaskID to 5 and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_CREATECHILDTASK_INDEX, &CS_CMDS_TEST_CFE_ES_CreateChildTaskHook);

    /* Execute the function being tested */
    CS_OneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == TRUE, "CS_AppData.OneShotInProgress == TRUE");

    UtAssert_True (CS_AppData.LastOneShotAddress == CmdPacket.Address, "CS_AppData.LastOneShotAddress == CmdPacket.Address");
    UtAssert_True (CS_AppData.LastOneShotSize == CmdPacket.Size, "CS_AppData.LastOneShotSize == CmdPacket.Size");
    UtAssert_True (CS_AppData.LastOneShotChecksum == 0, "CS_AppData.LastOneShotChecksum == 0");
    UtAssert_True (CS_AppData.LastOneShotMaxBytesPerCycle == CmdPacket.MaxBytesPerCycle, "CS_AppData.LastOneShotMaxBytesPerCycle == CmdPacket.MaxBytesPerCycle");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_STARTED_DBG_EID, CFE_EVS_DEBUG, "OneShot checksum started on address: 0x00000001, size: 2"),
        "OneShot checksum started on address: 0x00000001, size: 2");

    UtAssert_True (CS_AppData.ChildTaskID == 5, "CS_AppData.ChildTaskID == 5");
    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotCmd_Test_MaxBytesPerCycleNonZero */

void CS_OneShotCmd_Test_CreateChildTaskError(void)
{
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = FALSE;

    /* Set to generate error message CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_OneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.LastOneShotAddress == CmdPacket.Address, "CS_AppData.LastOneShotAddress == CmdPacket.Address");
    UtAssert_True (CS_AppData.LastOneShotSize == CmdPacket.Size, "CS_AppData.LastOneShotSize == CmdPacket.Size");
    UtAssert_True (CS_AppData.LastOneShotChecksum == 0, "CS_AppData.LastOneShotChecksum == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF"),
        "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotCmd_Test_CreateChildTaskError */

void CS_OneShotCmd_Test_ChildTaskError(void)
{
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_OneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_CHDTASK_ERR_EID, CFE_EVS_ERROR, "OneShot checksum failed: child task in use"),
        "OneShot checksum failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotCmd_Test_ChildTaskError */

void CS_OneShotCmd_Test_MemValidateRangeError(void)
{
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Set to generate error message CS_ONESHOT_MEMVALIDATE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_OneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR, "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF"),
        "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotCmd_Test_MemValidateRangeError */

void CS_CancelOneShotCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress   = FALSE;
    CS_AppData.OneShotInProgress = TRUE;

    /* Execute the function being tested */
    CS_CancelOneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ChildTaskID == 0, "CS_AppData.ChildTaskID == 0");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_CANCELLED_INF_EID, CFE_EVS_INFORMATION, "OneShot checksum calculation has been cancelled"),
        "OneShot checksum calculation has been cancelled");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_CancelOneShotCmd_Test_Nominal */

void CS_CancelOneShotCmd_Test_DeleteChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress   = FALSE;
    CS_AppData.OneShotInProgress = TRUE;

    /* Set to generate error message CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_DELETECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_CancelOneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0xFFFFFFFF"),
        "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_CancelOneShotCmd_Test_DeleteChildTaskError */

void CS_CancelOneShotCmd_Test_NoChildTaskError(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    CS_AppData.RecomputeInProgress   = TRUE;
    CS_AppData.OneShotInProgress = FALSE;

    /* Set to generate error message CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_DELETECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_CancelOneShotCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Cancel OneShot checksum failed. No OneShot active"),
        "Cancel OneShot checksum failed. No OneShot active");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_CancelOneShotCmd_Test_NoChildTaskError */

void CS_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(CS_NoopCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_NoopCmd_Test");

    UtTest_Add(CS_ResetCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetCmd_Test");
    
    UtTest_Add(CS_BackgroundCheckCmd_Test_InvalidMsgLength, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_InvalidMsgLength");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundCfeCore, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundCfeCore");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundOS, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundOS");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundEeprom, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundEeprom");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundMemory, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundMemory");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundTables, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundTables");
    UtTest_Add(CS_BackgroundCheckCmd_Test_BackgroundApp, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_BackgroundApp");
    UtTest_Add(CS_BackgroundCheckCmd_Test_Default, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_Default");
    UtTest_Add(CS_BackgroundCheckCmd_Test_Disabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCheckCmd_Test_Disabled");
    
    UtTest_Add(CS_DisableAllCSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAllCSCmd_Test");

    UtTest_Add(CS_EnableAllCSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAllCSCmd_Test");

    UtTest_Add(CS_DisableCfeCoreCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableCfeCoreCmd_Test");

    UtTest_Add(CS_EnableCfeCoreCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableCfeCoreCmd_Test");

    UtTest_Add(CS_DisableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableOSCmd_Test");

    UtTest_Add(CS_EnableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableOSCmd_Test");

    UtTest_Add(CS_ReportBaselineCfeCoreCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineCfeCoreCmd_Test_Nominal");
    UtTest_Add(CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet");

    UtTest_Add(CS_ReportBaselineOSCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineOSCmd_Test_Nominal");
    UtTest_Add(CS_ReportBaselineOSCmd_Test_NotComputedYet, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineOSCmd_Test_NotComputedYet");

    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineCfeCoreCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError");

    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineOSCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_ChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineOSCmd_Test_ChildTaskError");

    UtTest_Add(CS_OneShotCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_Nominal");
    UtTest_Add(CS_OneShotCmd_Test_MaxBytesPerCycleNonZero, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_MaxBytesPerCycleNonZero");
    UtTest_Add(CS_OneShotCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_OneShotCmd_Test_ChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_ChildTaskError");
    UtTest_Add(CS_OneShotCmd_Test_MemValidateRangeError, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_MemValidateRangeError");

    UtTest_Add(CS_CancelOneShotCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_CancelOneShotCmd_Test_Nominal");
    UtTest_Add(CS_CancelOneShotCmd_Test_DeleteChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_CancelOneShotCmd_Test_DeleteChildTaskError");
    UtTest_Add(CS_CancelOneShotCmd_Test_NoChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_CancelOneShotCmd_Test_NoChildTaskError");

} /* end CS_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
