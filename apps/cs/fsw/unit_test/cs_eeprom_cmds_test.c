 /*************************************************************************
 ** File:
 **   $Id: cs_eeprom_cmds_test.c 1.4 2017/03/29 17:29:01EDT mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_eeprom_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_eeprom_cmds_test.h"
#include "cs_eeprom_cmds.h"
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

void CS_DisableEepromCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState = CS_STATE_DISABLED, "CS_AppData.EepromCSState = CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom is Disabled"),
        "Checksumming of Eeprom is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEepromCmd_Test */

void CS_EnableEepromCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState = CS_STATE_ENABLED, "CS_AppData.EepromCSState = CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom is Enabled"),
        "Checksumming of Eeprom is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEepromCmd_Test */

void CS_ReportBaselineEntryIDEepromCmd_Test_Computed(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComputedYet = TRUE;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_EEPROM_INF_EID, CFE_EVS_INFORMATION, "Report baseline of Eeprom Entry 1 is 0x00000001"),
        "Report baseline of Eeprom Entry 1 is 0x00000001");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_Computed */

void CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComputedYet = FALSE;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_EEPROM_INF_EID, CFE_EVS_INFORMATION, "Report baseline of Eeprom Entry 1 has not been computed yet"),
        "Report baseline of Eeprom Entry 1 has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed */

void CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_ERROR, "Eeprom report baseline failed, Entry ID invalid: 16, State: 3 Max ID: 15"),
        "Eeprom report baseline failed, Entry ID invalid: 16, State: 3 Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_ERROR, "Eeprom report baseline failed, Entry ID invalid: 1, State: 0 Max ID: 15"),
        "Eeprom report baseline failed, Entry ID invalid: 1, State: 0 Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_RecomputeBaselineEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == TRUE, "CS_AppData.RecomputeInProgress == TRUE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_EEPROM_TABLE, "CS_AppData.ChildTaskTable == CS_EEPROM_TABLE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True
        (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID],
        "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID]");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute baseline of Eeprom Entry ID 1 started"),
        "Recompute baseline of Eeprom Entry ID 1 started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineEepromCmd_Test_Nominal */

void CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Set to generate error message CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_EEPROM_TABLE, "CS_AppData.ChildTaskTable == CS_EEPROM_TABLE");
    UtAssert_True (CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True
        (CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID],
        "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID]");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of Eeprom Entry ID 1 failed, CFE_ES_CreateChildTask returned:  0xFFFFFFFF"),
        "Recompute baseline of Eeprom Entry ID 1 failed, CFE_ES_CreateChildTask returned:  0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_ERROR, "Eeprom recompute baseline of entry failed, Entry ID invalid: 16, State: 3, Max ID: 15"),
        "Eeprom recompute baseline of entry failed, Entry ID invalid: 16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_ERROR, "Eeprom recompute baseline of entry failed, Entry ID invalid: 1, State: 0, Max ID: 15"),
        "Eeprom recompute baseline of entry failed, Entry ID invalid: 1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of Eeprom Entry ID 1 failed: child task in use"),
        "Recompute baseline of Eeprom Entry ID 1 failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress */

void CS_EnableEntryIDEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom Entry ID 1 is Enabled"),
        "Checksumming of Eeprom Entry ID 1 is Enabled");

    UtAssert_True (CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDEepromCmd_Test_Nominal */

void CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom Entry ID 1 is Enabled"),
        "Checksumming of Eeprom Entry ID 1 is Enabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID, CFE_EVS_DEBUG, "CS unable to update Eeprom definition table for entry 1, State: 0"),
        "CS unable to update Eeprom definition table for entry 1, State: 0");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty */

void CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Enable Eeprom entry failed, invalid Entry ID:  16, State: 3, Max ID: 15"),
        "Enable Eeprom entry failed, invalid Entry ID:  16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Enable Eeprom entry failed, invalid Entry ID:  1, State: 0, Max ID: 15"),
        "Enable Eeprom entry failed, invalid Entry ID:  1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_DisableEntryIDEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom Entry ID 1 is Disabled"),
        "Checksumming of Eeprom Entry ID 1 is Disabled");

    UtAssert_True (CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDEepromCmd_Test_Nominal */

void CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Eeprom Entry ID 1 is Disabled"),
        "Checksumming of Eeprom Entry ID 1 is Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID, CFE_EVS_DEBUG, "CS unable to update Eeprom definition table for entry 1, State: 0"),
        "CS unable to update Eeprom definition table for entry 1, State: 0");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty */

void CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Disable Eeprom entry failed, invalid Entry ID:  16, State: 3, Max ID: 15"),
        "Disable Eeprom entry failed, invalid Entry ID:  16, State: 3, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_EntryCmd_t), TRUE);

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_ERROR, "Disable Eeprom entry failed, invalid Entry ID:  1, State: 0, Max ID: 15"),
        "Disable Eeprom entry failed, invalid Entry ID:  1, State: 0, Max ID: 15");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_GetEntryIDEepromCmd_Test_Nominal(void)
{
    CS_GetEntryIDCmd_t   CmdPacket;

    int16  EntryID = 1;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_GetEntryIDCmd_t), TRUE);

    CS_AppData.ResEepromTblPtr[EntryID].StartAddress = 1;
    CmdPacket.Address = 1;
    CS_AppData.ResEepromTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResEepromTblPtr[EntryID].State = 99;

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_GET_ENTRY_ID_EEPROM_INF_EID, CFE_EVS_INFORMATION, "Eeprom Found Address 0x00000001 in Entry ID 1"),
        "Eeprom Found Address 0x00000001 in Entry ID 1");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_GetEntryIDEepromCmd_Test_Nominal */

void CS_GetEntryIDEepromCmd_Test_AddressNotFound(void)
{
    CS_GetEntryIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_GetEntryIDCmd_t), TRUE);

    CmdPacket.Address = 0xFFFFFFFF;

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID, CFE_EVS_INFORMATION, "Address 0xFFFFFFFF was not found in Eeprom table"),
        "Address 0xFFFFFFFF was not found in Eeprom table");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_GetEntryIDEepromCmd_Test_AddressNotFound */

void CS_Eeprom_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(CS_DisableEepromCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEepromCmd_Test");

    UtTest_Add(CS_EnableEepromCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEepromCmd_Test");

    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_Computed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDEepromCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineEepromCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress");

    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_GetEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_GetEntryIDEepromCmd_Test_AddressNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDEepromCmd_Test_AddressNotFound");

} /* end CS_Eeprom_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
