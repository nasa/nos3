 /*************************************************************************
 ** File:
 **   $Id: cs_table_processing_test.c 1.3 2017/02/16 15:33:11EST mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_table_processing.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_table_processing_test.h"
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

int32 CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1(const char *AppName, uint32 AppId, uint32 BufferLength)
{
    strncpy((char *)AppName, "CS", 3);

    return CFE_SUCCESS;
}

int32 CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook2(const char *AppName, uint32 AppId, uint32 BufferLength)
{
    uint16   i;

    for (i = 0; i <= OS_MAX_API_NAME; i++)
    {
        strncat((char *)AppName, "x", OS_MAX_API_NAME);
    }

    return CFE_SUCCESS;
}

int32 CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    return CFE_SUCCESS;
}

int32 CS_TABLE_PROCESSING_TEST_CFE_TBL_LoadHook( CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr )
{
    return CFE_SUCCESS;
}

void CS_ValidateEepromChecksumDefinitionTable_Test_Nominal(void)
{
    int32     Result;

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_INF_EID, CFE_EVS_INFORMATION, "CS Eeprom Table verification results: good = 1, bad = 0, unused = 15"),
        "CS Eeprom Table verification results: good = 1, bad = 0, unused = 15");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_Nominal */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled(void)
{
    int32     Result;

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_ENABLED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_RANGE_ERR_EID, CFE_EVS_ERROR, "Eeprom Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF"),
        "Eeprom Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_INF_EID, CFE_EVS_INFORMATION, "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled(void)
{
    int32     Result;

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_RANGE_ERR_EID, CFE_EVS_ERROR, "Eeprom Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF"),
        "Eeprom Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_INF_EID, CFE_EVS_INFORMATION, "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32     Result;

    CS_AppData.DefEepromTblPtr[0].State = 0xFFFF;

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_INF_EID, CFE_EVS_INFORMATION, "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Eeprom Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_EEPROM_STATE_ERR_EID, CFE_EVS_ERROR, "Eeprom Table Validate: Illegal State Field (0xFFFF) found in Entry ID 0"),
        "Eeprom Table Validate: Illegal State Field (0xFFFF) found in Entry ID 0");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal(void)
{
    int32     Result;

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_INF_EID, CFE_EVS_INFORMATION, "CS Memory Table verification results: good = 1, bad = 0, unused = 15"),
        "CS Memory Table verification results: good = 1, bad = 0, unused = 15");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled(void)
{
    int32     Result;

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_ENABLED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_RANGE_ERR_EID, CFE_EVS_ERROR, "Memory Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF"),
        "Memory Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_INF_EID, CFE_EVS_INFORMATION, "CS Memory Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Memory Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled(void)
{
    int32     Result;

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_RANGE_ERR_EID, CFE_EVS_ERROR, "Memory Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF"),
        "Memory Table Validate: Illegal checksum range found in Entry ID 0, CFE_PSP_MemValidateRange returned: 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_INF_EID, CFE_EVS_INFORMATION, "CS Memory Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Memory Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32     Result;

    CS_AppData.DefMemoryTblPtr[0].State = 0xFFFF;

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_INF_EID, CFE_EVS_INFORMATION, "CS Memory Table verification results: good = 0, bad = 1, unused = 15"),
        "CS Memory Table verification results: good = 0, bad = 1, unused = 15");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_MEMORY_STATE_ERR_EID, CFE_EVS_ERROR, "Memory Table Validate: Illegal State Field (0xFFFF) found in Entry ID 0"),
        "Memory Table Validate: Illegal State Field (0xFFFF) found in Entry ID 0");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateTablesChecksumDefinitionTable_Test_Nominal(void)
{
    int32     Result;

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 1, bad = 0, unused = 23"),
        "CS Tables Table verification results: good = 1, bad = 0, unused = 23");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_Nominal */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty(void)
{
    int32     Result;

    /* All states are empty by default */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Tables Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled(void)
{
    int32     Result;

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 1, bad = 1, unused = 22"),
        "CS Tables Table verification results: good = 1, bad = 1, unused = 22");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled(void)
{
    int32     Result;

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_DISABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_DISABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Tables Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 1, bad = 1, unused = 22"),
        "CS Tables Table verification results: good = 1, bad = 1, unused = 22");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled */

void CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32     Result;

    CS_AppData.DefTablesTblPtr[0].State = 0xFFFF;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_STATE_ERR_EID, CFE_EVS_ERROR, "CS Tables Table Validate: Illegal State Field (0xFFFF) found with name name"),
        "CS Tables Table Validate: Illegal State Field (0xFFFF) found with name name");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Tables Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName(void)
{
    int32     Result;

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID, CFE_EVS_ERROR, "CS Tables Table Validate: Illegal State (0x0001) with empty name at entry 0"),
        "CS Tables Table Validate: Illegal State (0x0001) with empty name at entry 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_TABLES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Tables Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName */

void CS_ValidateAppChecksumDefinitionTable_Test_Nominal(void)
{
    int32     Result;

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 1, bad = 0, unused = 23"),
        "CS Apps Table verification results: good = 1, bad = 0, unused = 23");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_Nominal */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty(void)
{
    int32     Result;

    /* All states are empty by default */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Apps Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled(void)
{
    int32     Result;

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_ENABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 1, bad = 1, unused = 22"),
        "CS Apps Table verification results: good = 1, bad = 1, unused = 22");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled(void)
{
    int32     Result;

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_DISABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_DISABLED;  /* All other states are empty by default, and so this test also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_DEF_TBL_DUPL_ERR_EID, CFE_EVS_ERROR, "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0"),
        "CS Apps Table Validate: Duplicate Name (name) found at entries 1 and 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 1, bad = 1, unused = 22"),
        "CS Apps Table verification results: good = 1, bad = 1, unused = 22");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled */

void CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32     Result;

    CS_AppData.DefAppTblPtr[0].State = 0xFFFF;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_STATE_ERR_EID, CFE_EVS_ERROR, "CS Apps Table Validate: Illegal State Field (0xFFFF) found with name name"),
        "CS Apps Table Validate: Illegal State Field (0xFFFF) found with name name");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Apps Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName(void)
{
    int32     Result;

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID, CFE_EVS_ERROR, "CS Apps Table Validate: Illegal State (0x0001) with empty name at entry 0"),
        "CS Apps Table Validate: Illegal State (0x0001) with empty name at entry 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_VAL_APP_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table verification results: good = 0, bad = 1, unused = 23"),
        "CS Apps Table verification results: good = 0, bad = 1, unused = 23");

    UtAssert_True (Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16    NumEntries = 1;
    uint16    Table      = CS_EEPROM_TABLE;

    CS_AppData.EepromCSState = 99;
    CS_AppData.DefEepromTblPtr[0].State = 1;
    CS_AppData.DefEepromTblPtr[0].NumBytesToChecksum = 2;
    CS_AppData.DefEepromTblPtr[0].StartAddress = 3;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefEepromTblPtr, (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResEepromTblPtr, NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.EepromCSState == 99, "CS_AppData.EepromCSState == 99");

    UtAssert_True(CS_AppData.ResEepromTblPtr[0].State == 1, "CS_AppData.ResEepromTblPtr[0].State == 1");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResEepromTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].NumBytesToChecksum == 2, "CS_AppData.ResEepromTblPtr[0].NumBytesToChecksum == 2");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ComparisonValue == 0, "CS_AppData.ResEepromTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].StartAddress == 3, "CS_AppData.ResEepromTblPtr[0].StartAddress == 3");

    UtAssert_True(CS_AppData.ResEepromTblPtr[1].State == CS_STATE_EMPTY, "CS_AppData.ResEepromTblPtr[1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ComputedYet == FALSE, "CS_AppData.ResEepromTblPtr[1].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].NumBytesToChecksum == 0, "CS_AppData.ResEepromTblPtr[1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ComparisonValue == 0, "CS_AppData.ResEepromTblPtr[1].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[1].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].StartAddress == 0, "CS_AppData.ResEepromTblPtr[1].StartAddress == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16    NumEntries = 1;
    uint16    Table      = CS_MEMORY_TABLE;

    CS_AppData.MemoryCSState = 99;
    CS_AppData.DefMemoryTblPtr[0].State = 1;
    CS_AppData.DefMemoryTblPtr[0].NumBytesToChecksum = 2;
    CS_AppData.DefMemoryTblPtr[0].StartAddress = 3;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr, (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr, NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.MemoryCSState == 99, "CS_AppData.MemoryCSState == 99");

    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].State == 1, "CS_AppData.ResMemoryTblPtr[0].State == 1");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResMemoryTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].NumBytesToChecksum == 2, "CS_AppData.ResMemoryTblPtr[0].NumBytesToChecksum == 2");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ComparisonValue == 0, "CS_AppData.ResMemoryTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].StartAddress == 3, "CS_AppData.ResMemoryTblPtr[0].StartAddress == 3");

    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].State == CS_STATE_EMPTY, "CS_AppData.ResMemoryTblPtr[1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ComputedYet == FALSE, "CS_AppData.ResMemoryTblPtr[1].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].NumBytesToChecksum == 0, "CS_AppData.ResMemoryTblPtr[1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ComparisonValue == 0, "CS_AppData.ResMemoryTblPtr[1].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[1].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].StartAddress == 0, "CS_AppData.ResMemoryTblPtr[1].StartAddress == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16    NumEntries = 1;
    uint16    Table      = CS_EEPROM_TABLE;

    CS_AppData.MemoryCSState = 99;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr, (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr, NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.MemoryCSState == 99, "CS_AppData.MemoryCSState == 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID, CFE_EVS_INFORMATION, "CS Eeprom Table: No valid entries in the table"),
        "CS Eeprom Table: No valid entries in the table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16    NumEntries = 1;
    uint16    Table      = CS_MEMORY_TABLE;

    CS_AppData.MemoryCSState = 99;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr, (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr, NumEntries, Table);


    /* Verify results */
    UtAssert_True(CS_AppData.MemoryCSState == 99, "CS_AppData.MemoryCSState == 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID, CFE_EVS_INFORMATION, "CS Memory Table: No valid entries in the table"),
        "CS Memory Table: No valid entries in the table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries */

void CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefEepromTbl", 20);

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.EepResTablesTblPtr == CS_AppData.ResTablesTblPtr, "CS_AppData.EepResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefEepromTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefEepromTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefMemoryTbl", 20);

    CS_AppData.DefMemoryTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.MemResTablesTblPtr == CS_AppData.ResTablesTblPtr, "CS_AppData.MemResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefMemoryTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefMemoryTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefTablesTbl", 20);

    CS_AppData.DefTablesTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.TblResTablesTblPtr == CS_AppData.ResTablesTblPtr, "CS_AppData.TblResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefTablesTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefTablesTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefAppTbl", 20);

    CS_AppData.DefAppTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.AppResTablesTblPtr == CS_AppData.ResTablesTblPtr, "CS_AppData.AppResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefAppTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefAppTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResEepromTbl", 20);

    CS_AppData.ResEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResEepromTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResEepromTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResMemoryTbl", 20);

    CS_AppData.ResMemoryTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResMemoryTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResMemoryTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResTablesTbl", 20);

    CS_AppData.ResTablesTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResTablesTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResTablesTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResAppTbl", 20);

    CS_AppData.ResAppTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == TRUE");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResAppTbl", 20) == 0, "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResAppTbl', 20) == 0");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries(void)
{
    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == CS_STATE_EMPTY, "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0, "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == CFE_TBL_BAD_TABLE_HANDLE, "CS_AppData.ResTablesTblPtr[0].TblHandle == CFE_TBL_BAD_TABLE_HANDLE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == FALSE, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == FALSE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].Name[0] == '\0', "CS_AppData.ResTablesTblPtr[0].Name[0] == '\0'");

    UtAssert_True(CS_AppData.TablesCSState == 99, "CS_AppData.TablesCSState == 99");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_PROCESS_TABLES_NO_ENTRIES_INF_EID, CFE_EVS_INFORMATION, "CS Tables Table: No valid entries in the table"),
        "CS Tables Table: No valid entries in the table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries */

void CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength(void)
{
    uint16   i;

    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    /* String name chosen to be of length OS_MAX_API_NAME in order to satisfy condition "AppNameIndex == OS_MAX_API_NAME" */
    /* If intended branch is reached, name length will be truncated to length OS_MAX_API_NAME - 1 */
    for (i = 0; i <= OS_MAX_API_NAME; i++)
    {
        strncat(CS_AppData.DefTablesTblPtr[0].Name, "x", OS_MAX_API_NAME);
    }

    strncat(CS_AppData.DefTablesTblPtr[0].Name, ".DefEepromTbl", OS_MAX_API_NAME);

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to string of x's of length OS_MAX_API_NAME */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    /* Note: This test is a special case where we're only interested in seeing that one branch was taken: "if (AppNameIndex == OS_MAX_API_NAME)" */
    /* If branch was taken, line "CS_AppData.EepResTablesTblPtr = ResultsEntry" will not be reached, and so the following assert will pass */
    UtAssert_True(CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr, "CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength */

void CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength(void)
{
    uint16   i;

    CS_AppData.TablesCSState = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncat(CS_AppData.DefTablesTblPtr[0].Name, "CS.", CFE_TBL_MAX_NAME_LENGTH);

    /* String name chosen to be of length CFE_TBL_MAX_NAME_LENGTH in order to satisfy condition "TableNameIndex == CFE_TBL_MAX_NAME_LENGTH" */
    /* If intended branch is reached, name length will be truncated to length CFE_TBL_MAX_NAME_LENGTH - 1 */
    for (i = 0; i <= CFE_TBL_MAX_NAME_LENGTH; i++)
    {
        strncat(CS_AppData.DefTablesTblPtr[0].Name, "x", CFE_TBL_MAX_NAME_LENGTH);
    }

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPNAME_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHook1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr, (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    /* Note: This test is a special case where we're only interested in seeing that one branch was taken: "if (TableNameIndex == CFE_TBL_MAX_NAME_LENGTH)" */
    /* If branch was taken, line "CS_AppData.EepResTablesTblPtr = ResultsEntry" will not be reached, and so the following assert will pass */
    UtAssert_True(CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr, "CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength */

void CS_ProcessNewAppDefinitionTable_Test_Nominal(void)
{
    CS_AppData.AppCSState = 99;
    CS_AppData.DefAppTblPtr[0].State = 88;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 20);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewAppDefinitionTable((CS_Def_App_Table_Entry_t *)&CS_AppData.DefAppTblPtr, (CS_Res_App_Table_Entry_t *)&CS_AppData.ResAppTblPtr);
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].State == 88, "CS_AppData.ResAppTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResAppTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComparisonValue == 0, "CS_AppData.ResAppTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].StartAddress == 0, "CS_AppData.ResAppTblPtr[0].StartAddress == 0");
    UtAssert_True(strncmp(CS_AppData.ResAppTblPtr[0].Name, "name", 20) == 0, "strncmp(CS_AppData.ResAppTblPtr[0].Name, 'name', 20) == 0");

    UtAssert_True(CS_AppData.AppCSState == 99, "CS_AppData.AppCSState == 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ProcessNewAppDefinitionTable_Test_Nominal */

void CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries(void)
{
    CS_AppData.AppCSState = 99;
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    CS_ProcessNewAppDefinitionTable((CS_Def_App_Table_Entry_t *)&CS_AppData.DefAppTblPtr, (CS_Res_App_Table_Entry_t *)&CS_AppData.ResAppTblPtr);
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].State == CS_STATE_EMPTY, "CS_AppData.ResAppTblPtr[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResAppTblPtr[0].ComputedYet == FALSE");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0, "CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComparisonValue == 0, "CS_AppData.ResAppTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].StartAddress == 0, "CS_AppData.ResAppTblPtr[0].StartAddress == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].Name[0] == '\0', "CS_AppData.ResAppTblPtr[0].Name[0] == '\0'");

    UtAssert_True(CS_AppData.AppCSState == 99, "CS_AppData.AppCSState == 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_PROCESS_APP_NO_ENTRIES_INF_EID, CFE_EVS_INFORMATION, "CS Apps Table: No valid entries in the table"),
        "CS Apps Table: No valid entries in the table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries */

void CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to cause load from the default tables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_INIT_ERR_EID, CFE_EVS_ERROR, "CS received error 0xFFFFFFFF initializing Definition table for Eeprom"),
        "CS received error 0xFFFFFFFF initializing Definition table for Eeprom");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM */

void CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_INIT_ERR_EID, CFE_EVS_ERROR, "CS received error 0xFFFFFFFF initializing Definition table for Eeprom"),
        "CS received error 0xFFFFFFFF initializing Definition table for Eeprom");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM */

void CS_TableInit_Test_DefinitionTableGetAddressErrorMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefMemoryTblPtr,
                           &CS_AppData.ResMemoryTblPtr,
                           CS_MEMORY_TABLE, 
                           CS_DEF_MEMORY_TABLE_NAME,
                           CS_RESULTS_MEMORY_TABLE_NAME,
                           CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                           CS_DEF_MEMORY_TABLE_FILENAME,
                           &CS_AppData.DefaultMemoryDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_INIT_ERR_EID, CFE_EVS_ERROR, "CS received error 0xFFFFFFFF initializing Definition table for Memory"),
        "CS received error 0xFFFFFFFF initializing Definition table for Memory");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorMemory */

void CS_TableInit_Test_DefinitionTableGetAddressErrorTables(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefTablesTblPtr,
                           &CS_AppData.ResTablesTblPtr,
                           CS_TABLES_TABLE, 
                           CS_DEF_TABLES_TABLE_NAME,
                           CS_RESULTS_TABLES_TABLE_NAME,
                           CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                           CS_DEF_TABLES_TABLE_FILENAME,
                           &CS_AppData.DefaultTablesDefTable,
                           sizeof(CS_Def_Tables_Table_Entry_t),
                           sizeof(CS_Res_Tables_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_INIT_ERR_EID, CFE_EVS_ERROR, "CS received error 0xFFFFFFFF initializing Definition table for Tables"),
        "CS received error 0xFFFFFFFF initializing Definition table for Tables");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorTables */

void CS_TableInit_Test_DefinitionTableGetAddressErrorApps(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefAppTblPtr,
                           &CS_AppData.ResAppTblPtr,
                           CS_APP_TABLE, 
                           CS_DEF_APP_TABLE_NAME,
                           CS_RESULTS_APP_TABLE_NAME,
                           CS_MAX_NUM_APP_TABLE_ENTRIES,
                           CS_DEF_APP_TABLE_FILENAME,
                           &CS_AppData.DefaultAppDefTable,
                           sizeof(CS_Def_App_Table_Entry_t),
                           sizeof(CS_Res_App_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_INIT_ERR_EID, CFE_EVS_ERROR, "CS received error 0xFFFFFFFF initializing Definition table for Apps"),
        "CS received error 0xFFFFFFFF initializing Definition table for Apps");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorApps */

void CS_TableInit_Test_EepromTableAndNotLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    
    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_EepromTableAndNotLoadedFromMemory */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    
    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_DISABLED, "CS_AppData.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* On 1st call, return -1 to set LoadedFromMemory.  On 2nd call, return CFE_SUCCESS to prevent error */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_DISABLED, "CS_AppData.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    
    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" on 2nd call to TBL_Register */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_DISABLED, "CS_AppData.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    
    /* On 1st call, return -1 to satisfy condition "ResultFromLoad != CFE_SUCCESS".  On 2nd call, return CFE_SUCCESS to prevent error */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_LoadHook);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefEepromTblPtr,
                           &CS_AppData.ResEepromTblPtr,
                           CS_EEPROM_TABLE, 
                           CS_DEF_EEPROM_TABLE_NAME,
                           CS_RESULTS_EEPROM_TABLE_NAME,
                           CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                           CS_DEF_EEPROM_TABLE_FILENAME,
                           &CS_AppData.DefaultEepromDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.EepromCSState == CS_STATE_DISABLED, "CS_AppData.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError */

void CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefMemoryTblPtr,
                           &CS_AppData.ResMemoryTblPtr,
                           CS_MEMORY_TABLE, 
                           CS_DEF_MEMORY_TABLE_NAME,
                           CS_RESULTS_MEMORY_TABLE_NAME,
                           CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                           CS_DEF_MEMORY_TABLE_FILENAME,
                           &CS_AppData.DefaultMemoryDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory */

void CS_TableInit_Test_MemoryTableAndLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefMemoryTblPtr,
                           &CS_AppData.ResMemoryTblPtr,
                           CS_MEMORY_TABLE, 
                           CS_DEF_MEMORY_TABLE_NAME,
                           CS_RESULTS_MEMORY_TABLE_NAME,
                           CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                           CS_DEF_MEMORY_TABLE_FILENAME,
                           &CS_AppData.DefaultMemoryDefTable,
                           sizeof(CS_Def_EepromMemory_Table_Entry_t),
                           sizeof(CS_Res_EepromMemory_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.MemoryCSState == CS_STATE_DISABLED, "CS_AppData.MemoryCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_MemoryTableAndLoadedFromMemory */

void CS_TableInit_Test_AppTableAndNotLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefAppTblPtr,
                           &CS_AppData.ResAppTblPtr,
                           CS_APP_TABLE, 
                           CS_DEF_APP_TABLE_NAME,
                           CS_RESULTS_APP_TABLE_NAME,
                           CS_MAX_NUM_APP_TABLE_ENTRIES,
                           CS_DEF_APP_TABLE_FILENAME,
                           &CS_AppData.DefaultAppDefTable,
                           sizeof(CS_Def_App_Table_Entry_t),
                           sizeof(CS_Res_App_Table_Entry_t),
                           NULL);

    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_TableInit_Test_AppTableAndNotLoadedFromMemory */

void CS_TableInit_Test_AppTableAndLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefAppTblPtr,
                           &CS_AppData.ResAppTblPtr,
                           CS_APP_TABLE, 
                           CS_DEF_APP_TABLE_NAME,
                           CS_RESULTS_APP_TABLE_NAME,
                           CS_MAX_NUM_APP_TABLE_ENTRIES,
                           CS_DEF_APP_TABLE_FILENAME,
                           &CS_AppData.DefaultAppDefTable,
                           sizeof(CS_Def_App_Table_Entry_t),
                           sizeof(CS_Res_App_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.AppCSState == CS_STATE_DISABLED, "CS_AppData.AppCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_AppTableAndLoadedFromMemory */

void CS_TableInit_Test_TablesTableAndNotLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefTablesTblPtr,
                           &CS_AppData.ResTablesTblPtr,
                           CS_TABLES_TABLE, 
                           CS_DEF_TABLES_TABLE_NAME,
                           CS_RESULTS_TABLES_TABLE_NAME,
                           CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                           CS_DEF_TABLES_TABLE_FILENAME,
                           &CS_AppData.DefaultTablesDefTable,
                           sizeof(CS_Def_Tables_Table_Entry_t),
                           sizeof(CS_Res_Tables_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_TablesTableAndNotLoadedFromMemory */

void CS_TableInit_Test_TablesTableAndLoadedFromMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to prevent unintended errors */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_TableInit (&DefinitionTableHandle,
                           &ResultsTableHandle,
                           &CS_AppData.DefTablesTblPtr,
                           &CS_AppData.ResTablesTblPtr,
                           CS_TABLES_TABLE, 
                           CS_DEF_TABLES_TABLE_NAME,
                           CS_RESULTS_TABLES_TABLE_NAME,
                           CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                           CS_DEF_TABLES_TABLE_FILENAME,
                           &CS_AppData.DefaultTablesDefTable,
                           sizeof(CS_Def_Tables_Table_Entry_t),
                           sizeof(CS_Res_Tables_Table_Entry_t),
                           NULL);
    
    /* Verify results */
    UtAssert_True (CS_AppData.TablesCSState == CS_STATE_DISABLED, "CS_AppData.TablesCSState == CS_STATE_DISABLED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_TableInit_Test_TablesTableAndLoadedFromMemory */

void CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_TABLES_TABLE;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy condition "Result == CFE_TBL_INFO_UPDATED". */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    CS_AppData.ResTablesTblPtr[0].TblHandle = 99;
    CS_AppData.ResTablesTblPtr[0].IsCSOwner = FALSE;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, CS_MAX_NUM_TABLES_TABLE_ENTRIES);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable */

void CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_APP_TABLE;
    uint16                      NumEntries = 0;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy condition "Result == CFE_TBL_INFO_UPDATED". */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefAppTblPtr, &CS_AppData.ResAppTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable */

void CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_EEPROM_TABLE;
    uint16                      NumEntries = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy condition "Result == CFE_TBL_INFO_UPDATED". */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 2);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable */

void CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_EEPROM_TABLE;
    uint16                      NumEntries = 0;

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_UPDATE_ERR_EID, CFE_EVS_ERROR, "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0x00000000 for table Eeprom"),
        "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0x00000000 for table Eeprom");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_EEPROM_TABLE;
    uint16                      NumEntries = 0;

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls except the 2nd. */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_UPDATE_ERR_EID, CFE_EVS_ERROR, "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Eeprom"),
        "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Eeprom");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_MEMORY_TABLE;
    uint16                      NumEntries = 0;

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls except the 2nd. */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefMemoryTblPtr, &CS_AppData.ResMemoryTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_UPDATE_ERR_EID, CFE_EVS_ERROR, "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Memory"),
        "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Memory");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_TABLES_TABLE;
    uint16                      NumEntries = 0;

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls except the 2nd. */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_UPDATE_ERR_EID, CFE_EVS_ERROR, "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Table"),
        "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table Table");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps(void)
{
    int32                       Result;
    CFE_TBL_Handle_t            DefinitionTableHandle = 0;
    CFE_TBL_Handle_t            ResultsTableHandle = 0;
    uint16                      Table = CS_APP_TABLE;
    uint16                      NumEntries = 0;

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls except the 2nd. */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable 
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just pointers */
    Result = CS_HandleTableUpdate (&CS_AppData.DefAppTblPtr, &CS_AppData.ResAppTblPtr, DefinitionTableHandle, ResultsTableHandle, Table, NumEntries);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TBL_UPDATE_ERR_EID, CFE_EVS_ERROR, "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table App"),
        "CS had problems updating table. Release:0x00000000 Manage:0x00000000 Get:0xFFFFFFFF for table App");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApp */

void CS_Table_Processing_Test_AddTestCases(void)
{
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField");

    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField");

    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName");

    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName, CS_Test_Setup, CS_Test_TearDown, "CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName");

    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries");

    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength");

    UtTest_Add(CS_ProcessNewAppDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewAppDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries");

    UtTest_Add(CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_DefinitionTableGetAddressErrorMemory");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorTables, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_DefinitionTableGetAddressErrorTables");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorApps, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_DefinitionTableGetAddressErrorApps");
    UtTest_Add(CS_TableInit_Test_EepromTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError");
    UtTest_Add(CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_MemoryTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_MemoryTableAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_AppTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_AppTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_AppTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_AppTableAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_TablesTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_TablesTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_TablesTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_TablesTableAndLoadedFromMemory");

    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps");

} /* end CS_Table_Processing_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
