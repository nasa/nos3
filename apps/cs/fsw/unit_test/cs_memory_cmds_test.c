 /*************************************************************************
 ** File:
 **   $Id: cs_memory_cmds_test.c 1.4 2017/03/29 17:29:03EDT mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_memory_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_memory_cmds_test.h"
#include "cs_memory_cmds.h"
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

void CS_DisableMemoryCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.MemoryCSState = CS_STATE_DISABLED, "CS_AppData.MemoryCSState = CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory is Disabled"),
        "Checksumming of Memory is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableMemoryCmd_Test */

void CS_EnableMemoryCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.MemoryCSState = CS_STATE_ENABLED, "CS_AppData.MemoryCSState = CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory is Enabled"),
        "Checksumming of Memory is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableMemoryCmd_Test */

void CS_ReportBaselineEntryIDMemoryCmd_Test_Computed(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ComputedYet = TRUE;
    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Report baseline of Memory Entry 1 is 0x00000001"),
        "Report baseline of Memory Entry 1 is 0x00000001");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDMemoryCmd_Test_Computed */

void CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ComputedYet = FALSE;
    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Report baseline of Memory Entry 1 has not been computed yet"),
        "Report baseline of Memory Entry 1 has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed */

void CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_ERROR, "Memory report baseline failed, Entry ID invalid: 16, State: 3 Max ID: 15"),
        "Memory report baseline failed, Entry ID invalid: 16, State: 3 Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_ERROR, "Memory report baseline failed, Entry ID invalid: 1, State: 0 Max ID: 15"),
        "Memory report baseline failed, Entry ID invalid: 1, State: 0 Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty */

void CS_RecomputeBaselineMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == TRUE, "CS_AppData.RecomputeInProgress == TRUE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_MEMORY_TABLE, "CS_AppData.ChildTaskTable == CS_MEMORY_TABLE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True
        (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID],
        "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID]");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute baseline of Memory Entry ID 1 started"),
        "Recompute baseline of Memory Entry ID 1 started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineMemoryCmd_Test_Nominal */

void CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Set to generate error message CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_MEMORY_TABLE, "CS_AppData.ChildTaskTable == CS_MEMORY_TABLE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True
        (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID],
        "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID]");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of Memory Entry ID 1 failed, ES_CreateChildTask returned:  0xFFFFFFFF"),
        "Recompute baseline of Memory Entry ID 1 failed, ES_CreateChildTask returned:  0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_ERROR, "Memory recompute baseline of entry failed, Entry ID invalid: 16, State: 3, Max ID: 15"),
        "Memory recompute baseline of entry failed, Entry ID invalid: 16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_ERROR, "Memory recompute baseline of entry failed, Entry ID invalid: 1, State: 0, Max ID: 15"),
        "Memory recompute baseline of entry failed, Entry ID invalid: 1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty */

void CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of Memory Entry ID 1 failed: child task in use"),
        "Recompute baseline of Memory Entry ID 1 failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress */

void CS_EnableEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory Entry ID 1 is Enabled"),
        "Checksumming of Memory Entry ID 1 is Enabled");

    UtAssert_True (CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDMemoryCmd_Test_Nominal */

void CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory Entry ID 1 is Enabled"),
        "Checksumming of Memory Entry ID 1 is Enabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID, CFE_EVS_DEBUG, "CS unable to update memory definition table for entry 1, State: 0"),
        "CS unable to update memory definition table for entry 1, State: 0");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty */

void CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Enable Memory entry failed, invalid Entry ID:  16, State: 3, Max ID: 15"),
        "Enable Memory entry failed, invalid Entry ID:  16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Enable Memory entry failed, invalid Entry ID:  1, State: 0, Max ID: 15"),
        "Enable Memory entry failed, invalid Entry ID:  1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty */

void CS_DisableEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory Entry ID 1 is Disabled"),
        "Checksumming of Memory Entry ID 1 is Disabled");

    UtAssert_True (CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDMemoryCmd_Test_Nominal */

void CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Memory Entry ID 1 is Disabled"),
        "Checksumming of Memory Entry ID 1 is Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID, CFE_EVS_DEBUG, "CS unable to update memory definition table for entry 1, State: 0"),
        "CS unable to update memory definition table for entry 1, State: 0");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty */

void CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Disable Memory entry failed, invalid Entry ID:  16, State: 3, Max ID: 15"),
        "Disable Memory entry failed, invalid Entry ID:  16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Disable Memory entry failed, invalid Entry ID:  1, State: 0, Max ID: 15"),
        "Disable Memory entry failed, invalid Entry ID:  1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty */

void CS_GetEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_GetEntryIDCmd_t   CmdPacket;

    int16  EntryID = 1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_GetEntryIDCmd_t), TRUE);

    CS_AppData.ResMemoryTblPtr[EntryID].StartAddress = 1;
    CmdPacket.Address = 1;
    CS_AppData.ResMemoryTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResMemoryTblPtr[EntryID].State = 99;

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_GET_ENTRY_ID_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Memory Found Address 0x00000001 in Entry ID 1"),
        "Memory Found Address 0x00000001 in Entry ID 1");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_GetEntryIDMemoryCmd_Test_Nominal */

void CS_GetEntryIDMemoryCmd_Test_AddressNotFound(void)
{
    CS_GetEntryIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_GetEntryIDCmd_t), TRUE);

    CmdPacket.Address = 0xFFFFFFFF;

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID, CFE_EVS_INFORMATION, "Address 0xFFFFFFFF was not found in Memory table"),
        "Address 0xFFFFFFFF was not found in Memory table");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_GetEntryIDEepromCmd_Test_AddressNotFound */

void CS_Memory_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(CS_DisableMemoryCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableMemoryCmd_Test");

    UtTest_Add(CS_EnableMemoryCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableMemoryCmd_Test");

    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_Computed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDMemoryCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineMemoryCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress");

    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDMemoryCmd_Test_Nominal");
   UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDMemoryCmd_Test_Nominal");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDMemoryCmd_Test_Nominal");
    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_AddressNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDMemoryCmd_Test_AddressNotFound");

} /* end CS_Memory_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
