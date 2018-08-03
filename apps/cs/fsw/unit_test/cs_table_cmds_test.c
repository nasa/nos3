 /*************************************************************************
 ** File:
 **   $Id: cs_table_cmds_test.c 1.4 2017/03/29 17:29:02EDT mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_table_cmds.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_table_cmds_test.h"
#include "cs_table_cmds.h"
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

void CS_DisableTablesCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_DisableTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.TablesCSState = CS_STATE_DISABLED, "CS_AppData.TablesCSState = CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_TABLES_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Tables is Disabled"),
        "Checksumming of Tables is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableTablesCmd_Test */

void CS_EnableTablesCmd_Test(void)
{
    CS_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    CS_EnableTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.TablesCSState = CS_STATE_ENABLED, "CS_AppData.TablesCSState = CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_TABLES_INF_EID, CFE_EVS_INFORMATION, "Checksumming of Tables is Enabled"),
        "Checksumming of Tables is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableTablesCmd_Test */

void CS_ReportBaselineTablesCmd_Test_Computed(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    CS_AppData.ResTablesTblPtr[0].ComputedYet = TRUE;
    CS_AppData.ResTablesTblPtr[0].ComparisonValue = 1;

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_ReportBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_TABLES_INF_EID, CFE_EVS_INFORMATION, "Report baseline of table name is 0x00000001"),
        "Report baseline of table name is 0x00000001");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineTablesCmd_Test_Computed */

void CS_ReportBaselineTablesCmd_Test_NotYetComputed(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    CS_AppData.ResTablesTblPtr[0].ComputedYet = FALSE;

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_ReportBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_NO_BASELINE_TABLES_INF_EID, CFE_EVS_INFORMATION, "Report baseline of table name has not been computed yet"),
        "Report baseline of table name has not been computed yet");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineTablesCmd_Test_NotYetComputed */

void CS_ReportBaselineTablesCmd_Test_TableNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name1", 10);
    strncpy(CmdPacket.Name, "name2", 10);

    /* Execute the function being tested */
    CS_ReportBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_BASELINE_INVALID_NAME_TABLES_ERR_EID, CFE_EVS_ERROR, "Tables report baseline failed, table name2 not found"),
        "Tables report baseline failed, table name2 not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ReportBaselineTablesCmd_Test_TableNotFound */

void CS_RecomputeBaselineTablesCmd_Test_Nominal(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_RecomputeBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == TRUE, "CS_AppData.RecomputeInProgress == TRUE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_TABLES_TABLE, "CS_AppData.ChildTaskTable == CS_TABLES_TABLE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_TABLES_STARTED_DBG_EID, CFE_EVS_DEBUG, "Recompute baseline of table name started"),
        "Recompute baseline of table name started");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineTablesCmd_Test_Nominal */

void CS_RecomputeBaselineTablesCmd_Test_CreateChildTaskError(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Set to generate error message CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");

    UtAssert_True (CS_AppData.ChildTaskTable == CS_TABLES_TABLE, "CS_AppData.ChildTaskTable == CS_TABLES_TABLE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Recompute baseline of table name failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF"),
        "Recompute baseline of table name failed, CFE_ES_CreateChildTask returned: 0xFFFFFFFF");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineTablesCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineTablesCmd_Test_TableNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "name", 10);

    /* Execute the function being tested */
    CS_RecomputeBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID, CFE_EVS_ERROR, "Tables recompute baseline failed, table name not found"),
        "Tables recompute baseline failed, table name not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineTablesCmd_Test_TableNotFound */

void CS_RecomputeBaselineTablesCmd_Test_RecomputeInProgress(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.RecomputeInProgress = TRUE;

    /* Execute the function being tested */
    CS_RecomputeBaselineTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID, CFE_EVS_ERROR, "Tables recompute baseline for table name failed: child task in use"),
        "Tables recompute baseline for table name failed: child task in use");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeBaselineTablesCmd_Test_RecomputeInProgress */

void CS_DisableNameTablesCmd_Test_Nominal(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    CS_AppData.DefTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableDefTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_DisableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].State == CS_STATE_DISABLED, "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True (CS_AppData.DefTablesTblPtr[0].State == CS_STATE_DISABLED, "CS_AppData.DefTablesTblPtr[0].State == CS_STATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_TABLES_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of table name is Disabled"),
        "Checksumming of table name is Disabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableNameTablesCmd_Test_Nominal */

void CS_DisableNameTablesCmd_Test_TableDefNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_DisableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].State == CS_STATE_DISABLED, "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_DISABLED");
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_TABLES_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of table name is Disabled"),
        "Checksumming of table name is Disabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_TABLE_DEF_NOT_FOUND_DBG_EID, CFE_EVS_DEBUG, "CS unable to update tables definition table for entry name"),
        "CS unable to update tables definition table for entry name");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_DisableNameTablesCmd_Test_TableDefNotFound */

void CS_DisableNameTablesCmd_Test_TableNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "name", 10);

    /* Execute the function being tested */
    CS_DisableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID, CFE_EVS_ERROR, "Tables disable table command failed, table name not found"),
        "Tables disable table command failed, table name not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_DisableNameTablesCmd_Test_TableNotFound */

void CS_EnableNameTablesCmd_Test_Nominal(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    CS_AppData.DefTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableDefTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_EnableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].State == CS_STATE_ENABLED, "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_ENABLED");
    UtAssert_True (CS_AppData.DefTablesTblPtr[0].State == CS_STATE_ENABLED, "CS_AppData.DefTablesTblPtr[0].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_TABLES_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of table name is Enabled"),
        "Checksumming of table name is Enabled");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableNameTablesCmd_Test_Nominal */

void CS_EnableNameTablesCmd_Test_TableDefNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;  /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Execute the function being tested */
    CS_EnableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (CS_AppData.ResTablesTblPtr[0].State == CS_STATE_ENABLED, "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_ENABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_TABLES_NAME_INF_EID, CFE_EVS_INFORMATION, "Checksumming of table name is Enabled"),
        "Checksumming of table name is Enabled");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_TABLE_DEF_NOT_FOUND_DBG_EID, CFE_EVS_DEBUG, "CS unable to update tables definition table for entry name"),
        "CS unable to update tables definition table for entry name");

    UtAssert_True (CS_AppData.CmdCounter == 1, "CS_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_EnableNameTablesCmd_Test_TableDefNotFound */

void CS_EnableNameTablesCmd_Test_TableNotFound(void)
{
    CS_TableNameCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_TableNameCmd_t), TRUE);

    strncpy(CmdPacket.Name, "name", 10);

    /* Execute the function being tested */
    CS_EnableNameTablesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID, CFE_EVS_ERROR, "Tables enable table command failed, table name not found"),
        "Tables enable table command failed, table name not found");

    UtAssert_True (CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_EnableNameTablesCmd_Test_TableNotFound */

void CS_Table_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(CS_DisableTablesCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableTablesCmd_Test");

    UtTest_Add(CS_EnableTablesCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableTablesCmd_Test");

    UtTest_Add(CS_ReportBaselineTablesCmd_Test_Computed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineTablesCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineTablesCmd_Test_NotYetComputed, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineTablesCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineTablesCmd_Test_TableNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_ReportBaselineTablesCmd_Test_TableNotFound");

    UtTest_Add(CS_RecomputeBaselineTablesCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineTablesCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineTablesCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineTablesCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineTablesCmd_Test_TableNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineTablesCmd_Test_TableNotFound");
    UtTest_Add(CS_RecomputeBaselineTablesCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeBaselineTablesCmd_Test_RecomputeInProgress");

    UtTest_Add(CS_DisableNameTablesCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameTablesCmd_Test_Nominal");
    UtTest_Add(CS_DisableNameTablesCmd_Test_TableDefNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameTablesCmd_Test_TableDefNotFound");
    UtTest_Add(CS_DisableNameTablesCmd_Test_TableNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameTablesCmd_Test_TableNotFound");

    UtTest_Add(CS_EnableNameTablesCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameTablesCmd_Test_Nominal");
    UtTest_Add(CS_EnableNameTablesCmd_Test_TableDefNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameTablesCmd_Test_TableDefNotFound");
    UtTest_Add(CS_EnableNameTablesCmd_Test_TableNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameTablesCmd_Test_TableNotFound");

} /* end CS_Table_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
