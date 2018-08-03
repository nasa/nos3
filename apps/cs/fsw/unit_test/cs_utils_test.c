 /*************************************************************************
 ** File:
 **   $Id: cs_utils_test.c 1.3 2017/02/16 15:33:20EST mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_utils.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_utils_test.h"
#include "cs_utils.h"
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

int32 CS_UTILS_TEST_CFE_ES_GetAppInfoHook1(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    AppInfo->CodeSize            = 5;
    AppInfo->CodeAddress         = 1;
    AppInfo->AddressesAreValid   = TRUE;

    return CFE_SUCCESS;
}

void CS_ZeroEepromTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroEepromTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].TempChecksumValue == 0, "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroEepromTempValues_Test */

void CS_ZeroMemoryTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroMemoryTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].TempChecksumValue == 0, "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroMemoryTempValues_Test */

void CS_ZeroTablesTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroTablesTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroTablesTempValues_Test */

void CS_ZeroAppTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroAppTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].TempChecksumValue == 0, "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].ByteOffset == 0, "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].TempChecksumValue == 0, "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].ByteOffset == 0, "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroAppTempValues_Test */

void CS_ZeroCfeCoreTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroCfeCoreTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CfeCoreCodeSeg.TempChecksumValue == 0, "CS_AppData.CfeCoreCodeSeg.TempChecksumValue == 0");
    UtAssert_True(CS_AppData.CfeCoreCodeSeg.ByteOffset == 0, "CS_AppData.CfeCoreCodeSeg.ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroCfeCoreTempValues_Test */

void CS_ZeroOSTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroOSTempValues();
    
    /* Verify results */
    UtAssert_True(CS_AppData.OSCodeSeg.TempChecksumValue == 0, "CS_AppData.OSCodeSeg.TempChecksumValue == 0");
    UtAssert_True(CS_AppData.OSCodeSeg.ByteOffset == 0, "CS_AppData.OSCodeSeg.ByteOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ZeroOSTempValues_Test */

void CS_InitializeDefaultTables_Test(void)
{

    /* Execute the function being tested */
    CS_InitializeDefaultTables();
    
    /* Verify results */
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].State == CS_STATE_EMPTY, "CS_AppData.DefaultEepromDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].NumBytesToChecksum == 0, "CS_AppData.DefaultEepromDefTable[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].StartAddress == 0, "CS_AppData.DefaultEepromDefTable[0].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0, "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].State == CS_STATE_EMPTY, "CS_AppData.DefaultMemoryDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].NumBytesToChecksum == 0, "CS_AppData.DefaultMemoryDefTable[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].StartAddress == 0, "CS_AppData.DefaultMemoryDefTable[0].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0, "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultAppDefTable[0].State == CS_STATE_EMPTY, "CS_AppData.DefaultAppDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[0].Name[0] == '\0', "CS_AppData.DefaultAppDefTable[0].Name[0] == '\0'");

    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY, "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == '\0', "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == '\0'");

    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY, "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == '\0', "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == '\0'");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[0].State == CS_STATE_EMPTY, "CS_AppData.DefaultTablesDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[0].Name[0] == '\0', "CS_AppData.DefaultTablesDefTable[0].Name[0] == '\0'");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY, "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == '\0', "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == '\0'");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY, "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == '\0', "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == '\0'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_InitializeDefaultTables_Test */

void CS_GoToNextTable_Test_Nominal(void)
{
    CS_AppData.CurrentCSTable = CS_NUM_TABLES - 2;

    /* Execute the function being tested */
    CS_GoToNextTable();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentCSTable == CS_NUM_TABLES - 1, "CS_AppData.CurrentCSTable == CS_NUM_TABLES - 1");
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GoToNextTable_Test_Nominal */

void CS_GoToNextTable_Test_UpdatePassCounter(void)
{
    CS_AppData.CurrentCSTable = CS_NUM_TABLES - 1;

    /* Execute the function being tested */
    CS_GoToNextTable();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentCSTable == 0, "CS_AppData.CurrentCSTable == 0");
    UtAssert_True(CS_AppData.PassCounter == 1, "CS_AppData.PassCounter == 1");
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GoToNextTable_Test_UpdatePassCounter */

void CS_GetTableResTblEntryByName_Test(void)
{
    boolean     Result;

    CS_Res_Tables_Table_Entry_t     *EntryPtr;

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetTableResTblEntryByName(&EntryPtr, "name");
    
    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.ResTablesTblPtr, "EntryPtr == CS_AppData.ResTablesTblPtr");
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GetTableResTblEntryByName_Test */

void CS_GetTableDefTblEntryByName_Test(void)
{
    boolean     Result;

    CS_Def_Tables_Table_Entry_t     *EntryPtr;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    CS_AppData.DefTablesTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetTableDefTblEntryByName(&EntryPtr, "name");
    
    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.DefTablesTblPtr, "EntryPtr == CS_AppData.DefTablesTblPtr");
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GetTableDefTblEntryByName_Test */

void CS_GetAppResTblEntryByName_Test(void)
{
    boolean     Result;

    CS_Res_App_Table_Entry_t     *EntryPtr;

    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);

    CS_AppData.ResAppTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetAppResTblEntryByName(&EntryPtr, "name");
    
    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.ResAppTblPtr, "EntryPtr == CS_AppData.ResAppTblPtr");
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GetAppResTblEntryByName_Test */

void CS_GetAppDefTblEntryByName_Test(void)
{
    boolean     Result;

    CS_Def_App_Table_Entry_t     *EntryPtr;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    CS_AppData.DefAppTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetAppDefTblEntryByName(&EntryPtr, "name");
    
    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.DefAppTblPtr, "EntryPtr == CS_AppData.DefAppTblPtr");
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_GetAppDefTblEntryByName_Test */

void CS_FindEnabledEepromEntry_Test(void)
{
    boolean     Result;
    uint16      EnabledEntry;

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledEepromEntry(&EnabledEntry);
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 16, "CS_AppData.CurrentEntryInTable == 16");

    UtAssert_True
        (memcmp(&CS_AppData.ResEepromTblPtr[0], &CS_AppData.ResEepromTblPtr[15], CS_MAX_NUM_EEPROM_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t)),
        "memcmp(&CS_AppData.ResEepromTblPtr[0], &CS_AppData.ResEepromTblPtr[15], CS_MAX_NUM_EEPROM_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 16, "EnabledEntry == 16");

    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_FindEnabledEepromEntry_Test */

void CS_FindEnabledMemoryEntry_Test(void)
{
    boolean     Result;
    uint16      EnabledEntry;

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledMemoryEntry(&EnabledEntry);
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 16, "CS_AppData.CurrentEntryInTable == 16");

    UtAssert_True
        (memcmp(&CS_AppData.ResMemoryTblPtr[0], &CS_AppData.ResMemoryTblPtr[15], CS_MAX_NUM_MEMORY_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t)),
        "memcmp(&CS_AppData.ResMemoryTblPtr[0], &CS_AppData.ResMemoryTblPtr[15], CS_MAX_NUM_MEMORY_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 16, "EnabledEntry == 16");

    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_FindEnabledMemoryEntry_Test */

void CS_FindEnabledTablesEntry_Test(void)
{
    boolean     Result;
    uint16      EnabledEntry;

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledTablesEntry(&EnabledEntry);
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 24, "CS_AppData.CurrentEntryInTable == 24");

    UtAssert_True
        (memcmp(&CS_AppData.ResTablesTblPtr[0], &CS_AppData.ResTablesTblPtr[23], CS_MAX_NUM_TABLES_TABLE_ENTRIES*sizeof(CS_Res_Tables_Table_Entry_t)),
        "memcmp(&CS_AppData.ResTablesTblPtr[0], &CS_AppData.ResTablesTblPtr[23], CS_MAX_NUM_TABLES_TABLE_ENTRIES*sizeof(CS_Res_Tables_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 24, "EnabledEntry == 24");

    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_FindEnabledTablesEntry_Test */

void CS_FindEnabledAppEntry_Test(void)
{
    boolean     Result;
    uint16      EnabledEntry;

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledAppEntry(&EnabledEntry);
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 24, "CS_AppData.CurrentEntryInTable == 24");

    UtAssert_True
        (memcmp(&CS_AppData.ResAppTblPtr[0], &CS_AppData.ResAppTblPtr[23], CS_MAX_NUM_APP_TABLE_ENTRIES*sizeof(CS_Res_App_Table_Entry_t)),
        "memcmp(&CS_AppData.ResAppTblPtr[0], &CS_AppData.ResAppTblPtr[23], CS_MAX_NUM_APP_TABLE_ENTRIES*sizeof(CS_Res_App_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 24, "EnabledEntry == 24");

    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_FindEnabledAppEntry_Test */

void CS_VerifyCmdLength_Test_Nominal(void)
{
    boolean           Result;
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), sizeof(CS_OneShotCmd_t));
    
    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_VerifyCmdLength_Test_Nominal */

void CS_VerifyCmdLength_Test_InvalidMsgLength(void)
{
    boolean           Result;
    CS_OneShotCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 88);

    CS_AppData.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), 99);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x189F, CC = 88, Len = 20, Expected = 99"),
        "Invalid msg length: ID = 0x189F, CC = 88, Len = 20, Expected = 99");

    UtAssert_True(CS_AppData.CmdErrCounter == 1, "CS_AppData.CmdErrCounter == 1");

    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_VerifyCmdLength_Test_InvalidMsgLength */

void CS_BackgroundCfeCore_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.CfeCoreCSState = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = 99;
    CS_AppData.CfeCoreCodeSeg.State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.CfeCoreBaseline == 0, "CS_AppData.CfeCoreBaseline == 0"); /* Reset to 0 by CS_ComputeEepromMemory */

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCfeCore_Test_Nominal */

void CS_BackgroundCfeCore_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.CfeCoreCSState = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = 99;
    CS_AppData.CfeCoreCodeSeg.State = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.CfeCoreCodeSeg.ByteOffset         = 0;
    CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                  = 2;
    CS_AppData.CfeCoreCodeSeg.ComputedYet        = TRUE;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue    = 5;

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* End of section to make CS_ComputeEepromMemory return error */

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CfeCoreCSErrCounter == 1, "CS_AppData.CfeCoreCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_CFECORE_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: cFE Core, Expected: 0x00000005, Calculated: 0x00000001"),
        "Checksum Failure: cFE Core, Expected: 0x00000005, Calculated: 0x00000001");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.CfeCoreBaseline == 5, "CS_AppData.CfeCoreBaseline == 5");

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundCfeCore_Test_Miscompare */

void CS_BackgroundCfeCore_Test_ResultsEntryDisabled(void)
{
    boolean     Result;

    CS_AppData.CfeCoreCSState       = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCfeCore_Test_ResultsEntryDisabled */

void CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.CfeCoreCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled */

void CS_BackgroundOS_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.OSCSState = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.ComparisonValue = 99;
    CS_AppData.OSCodeSeg.State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.OSBaseline == 0, "CS_AppData.OSBaseline == 0"); /* Reset to 0 by CS_ComputeEepromMemory */

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundOS_Test_Nominal */

void CS_BackgroundOS_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.OSCSState = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.ComparisonValue = 99;
    CS_AppData.OSCodeSeg.State = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.OSCodeSeg.ByteOffset         = 0;
    CS_AppData.OSCodeSeg.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                  = 2;
    CS_AppData.OSCodeSeg.ComputedYet        = TRUE;
    CS_AppData.OSCodeSeg.ComparisonValue    = 5;

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* End of section to make CS_ComputeEepromMemory return error */

    /* Execute the function being tested */
    Result = CS_BackgroundOS();
    
    /* Verify results */
    UtAssert_True(CS_AppData.OSCSErrCounter == 1, "CS_AppData.OSCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_OS_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: OS code segment, Expected: 0x00000005, Calculated: 0x00000001"),
        "Checksum Failure: cFE Core, Expected: 0x00000005, Calculated: 0x00000001");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.OSBaseline == 5, "CS_AppData.OSBaseline == 5");

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundOS_Test_Miscompare */

void CS_BackgroundOS_Test_ResultsEntryDisabled(void)
{
    boolean     Result;

    CS_AppData.OSCSState       = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.State = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundOS_Test_ResultsEntryDisabled */

void CS_BackgroundOS_Test_OSCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.OSCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundOS_Test_OSCSStateDisabled */

void CS_BackgroundEeprom_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.EepromCSState = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 99;
    CS_AppData.ResEepromTblPtr[0].State = CS_STATE_ENABLED;

    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 1;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.EepromBaseline == 1, "CS_AppData.EepromBaseline == 1");
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundEeprom_Test_Nominal */

void CS_BackgroundEeprom_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.EepromCSState = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ByteOffset         = 0;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                       = 2;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComputedYet        = TRUE;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComparisonValue    = 5;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue  = 5; /* ResEepromTblPtr[0] is added to EntireEepromCS, ResEepromTblPtr[16] is not, because it's outside the valid range */

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* End of section to make CS_ComputeEepromMemory return error */

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();
    
    /* Verify results */
    UtAssert_True(CS_AppData.EepromCSErrCounter == 1, "CS_AppData.EepromCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_EEPROM_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: Entry 16 in Eeprom Table, Expected: 0x00000005, Calculated: 0x00000001"),
        "Checksum Failure: Entry 16 in Eeprom Table, Expected: 0x00000005, Calculated: 0x00000001");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.EepromBaseline == 5, "CS_AppData.EepromBaseline == 5");

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundEeprom_Test_Miscompare */

void CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse(void)
{
    boolean     Result;

    CS_AppData.EepromCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse */

void CS_BackgroundEeprom_Test_EepromCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.EepromCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundEeprom_Test_EepromCSStateDisabled */

void CS_BackgroundMemory_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.MemoryCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 1;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundMemory_Test_Nominal */

void CS_BackgroundMemory_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.MemoryCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ByteOffset         = 0;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                       = 2;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComputedYet        = TRUE;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue    = 5;
    CS_AppData.ResMemoryTblPtr[0].ComparisonValue  = 5; /* ResMemoryTblPtr[0] is added to EntireMemoryCS, ResMemoryTblPtr[16] is not, because it's outside the valid range */

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* End of section to make CS_ComputeEepromMemory return error */

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();
    
    /* Verify results */
    UtAssert_True(CS_AppData.MemoryCSErrCounter == 1, "CS_AppData.MemoryCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_MEMORY_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: Entry 16 in Memory Table, Expected: 0x00000005, Calculated: 0x00000001"),
        "Checksum Failure: Entry 16 in Memory Table, Expected: 0x00000005, Calculated: 0x00000001");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundMemory_Test_Miscompare */

void CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse(void)
{
    boolean     Result;

    CS_AppData.MemoryCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse */

void CS_BackgroundMemory_Test_MemoryCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.MemoryCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundMemory_Test_MemoryCSStateDisabled */

void CS_BackgroundTables_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    CS_AppData.TablesCSState = CS_STATE_ENABLED;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to prevent CS_ComputeTables from returning an error */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = CS_BackgroundTables();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundTables_Test_Nominal */

void CS_BackgroundTables_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    CS_AppData.TablesCSState = CS_STATE_ENABLED;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    strncpy(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].Name, "name", 10);


    /* The following section is just to make CS_ComputeTables return error */

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].TblHandle = 99;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComputedYet = TRUE;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* End of section to make CS_ComputeTables return error */


    /* Execute the function being tested */
    Result = CS_BackgroundTables();
    
    /* Verify results */
    UtAssert_True(CS_AppData.TablesCSErrCounter == 1, "CS_AppData.TablesCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_TABLES_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: Table name, Expected: 0x00000001, Calculated: 0x00000002"),
        "Checksum Failure: Table name, Expected: 0x00000001, Calculated: 0x00000002");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundTables_Test_Miscompare */

void CS_BackgroundTables_Test_TablesNotFound(void)
{
    boolean     Result;

    CS_AppData.TablesCSState = CS_STATE_ENABLED;
    CS_AppData.CurrentEntryInTable = 0;
    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_ENABLED;
    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);

    /* Set to make ComputeTables return CS_ERR_NOT_FOUND */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_BackgroundTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID, CFE_EVS_ERROR, "Tables table computing: Table name could not be found, skipping"),
        "Tables table computing: Table name could not be found, skipping");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 1, "CS_AppData.CurrentEntryInTable == 1");

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_BackgroundTables_Test_TablesNotFound */

void CS_BackgroundTables_Test_FindEnabledTablesEntryFalse(void)
{
    boolean     Result;

    CS_AppData.TablesCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundTables();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundTables_Test_FindEnabledTablesEntryFalse */

void CS_BackgroundTables_Test_TablesCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.TablesCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundTables();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundTables_Test_TablesCSStateDisabled */

void CS_BackgroundApp_Test_Nominal(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_APP_TABLE_ENTRIES;

    CS_AppData.AppCSState = CS_STATE_ENABLED;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to prevent CS_ComputeApp from returning an error */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = CS_BackgroundApp();
    
    /* Verify results */
    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundApp_Test_Nominal */

void CS_BackgroundApp_Test_Miscompare(void)
{
    boolean     Result;

    CS_AppData.CurrentEntryInTable = CS_MAX_NUM_APP_TABLE_ENTRIES;

    CS_AppData.AppCSState = CS_STATE_ENABLED;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    strncpy(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].Name, "name", 10);


    /* The following section is just to make CS_ComputeApp return error */

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ByteOffset = 0;
    CS_AppData.MaxBytesPerCycle = 5;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComputedYet = TRUE;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_UTILS_TEST_CFE_ES_GetAppInfoHook1);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* End of section to make CS_ComputeApp return error */


    /* Execute the function being tested */
    Result = CS_BackgroundApp();
    
    /* Verify results */
    UtAssert_True(CS_AppData.AppCSErrCounter == 1, "CS_AppData.AppCSErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_APP_MISCOMPARE_ERR_EID, CFE_EVS_ERROR, "Checksum Failure: Application name, Expected: 0x00000003, Calculated: 0x00000002"),
        "Checksum Failure: Application name, Expected: 0x00000003, Calculated: 0x00000002");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 0, "CS_AppData.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_BackgroundApp_Test_Miscompare */

void CS_BackgroundApp_Test_AppNotFound(void)
{
    boolean     Result;

    CS_AppData.AppCSState = CS_STATE_ENABLED;
    CS_AppData.CurrentEntryInTable = 0;
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_ENABLED;
    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);

    /* Set to make ComputeApp return CS_ERR_NOT_FOUND */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_BackgroundApp();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_APP_NOT_FOUND_ERR_EID, CFE_EVS_ERROR, "App table computing: App name could not be found, skipping"),
        "App table computing: App name could not be found, skipping");

    UtAssert_True(CS_AppData.CurrentEntryInTable == 1, "CS_AppData.CurrentEntryInTable == 1");

    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_BackgroundApp_Test_AppNotFound */

void CS_BackgroundApp_Test_FindEnabledAppEntryFalse(void)
{
    boolean     Result;

    CS_AppData.AppCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundApp();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundApp_Test_FindEnabledAppEntryFalse */

void CS_BackgroundApp_Test_AppCSStateDisabled(void)
{
    boolean     Result;

    CS_AppData.AppCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundApp();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_BackgroundApp_Test_AppCSStateDisabled */

void CS_ResetTablesTblResultEntry_Test(void)
{
    /* Execute the function being tested */
    CS_ResetTablesTblResultEntry(CS_AppData.ResTablesTblPtr);
    
    /* Verify results */
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0, "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE, "CS_AppData.ResTablesTblPtr[0].ComputedYet == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ResetTablesTblResultEntry_Test */

void CS_Utils_Test_AddTestCases(void)
{
    UtTest_Add(CS_ZeroEepromTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroEepromTempValues_Test");

    UtTest_Add(CS_ZeroMemoryTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroMemoryTempValues_Test");

    UtTest_Add(CS_ZeroTablesTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroTablesTempValues_Test");

    UtTest_Add(CS_ZeroAppTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroAppTempValues_Test");

    UtTest_Add(CS_ZeroCfeCoreTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroCfeCoreTempValues_Test");

    UtTest_Add(CS_ZeroOSTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroOSTempValues_Test");

    UtTest_Add(CS_InitializeDefaultTables_Test, CS_Test_Setup, CS_Test_TearDown, "CS_InitializeDefaultTables_Test");

    UtTest_Add(CS_GoToNextTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_GoToNextTable_Test_Nominal");
    UtTest_Add(CS_GoToNextTable_Test_UpdatePassCounter, CS_Test_Setup, CS_Test_TearDown, "CS_GoToNextTable_Test_UpdatePassCounter");

    UtTest_Add(CS_GetTableResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableResTblEntryByName_Test");

    UtTest_Add(CS_GetTableDefTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableDefTblEntryByName_Test");

    UtTest_Add(CS_GetAppResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppResTblEntryByName_Test");

    UtTest_Add(CS_GetAppResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppDefTblEntryByName_Test");

    UtTest_Add(CS_FindEnabledEepromEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledEepromEntry_Test");

    UtTest_Add(CS_FindEnabledMemoryEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledMemoryEntry_Test");

    UtTest_Add(CS_FindEnabledTablesEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledTablesEntry_Test");

    UtTest_Add(CS_FindEnabledAppEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledAppEntry_Test");

    UtTest_Add(CS_VerifyCmdLength_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_VerifyCmdLength_Test_Nominal");
    UtTest_Add(CS_VerifyCmdLength_Test_InvalidMsgLength, CS_Test_Setup, CS_Test_TearDown, "CS_VerifyCmdLength_Test_InvalidMsgLength");

    UtTest_Add(CS_BackgroundCfeCore_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test_Nominal");
    UtTest_Add(CS_BackgroundCfeCore_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test_Miscompare");
    UtTest_Add(CS_BackgroundCfeCore_Test_ResultsEntryDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test_ResultsEntryDisabled");
    UtTest_Add(CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled");

    UtTest_Add(CS_BackgroundOS_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_Nominal");
    UtTest_Add(CS_BackgroundOS_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_Miscompare");
    UtTest_Add(CS_BackgroundOS_Test_ResultsEntryDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_ResultsEntryDisabled");
    UtTest_Add(CS_BackgroundOS_Test_OSCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_OSCSStateDisabled");

    UtTest_Add(CS_BackgroundEeprom_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test_Nominal");
    UtTest_Add(CS_BackgroundEeprom_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test_Miscompare");
    UtTest_Add(CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse");
    UtTest_Add(CS_BackgroundEeprom_Test_EepromCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test_EepromCSStateDisabled");

    UtTest_Add(CS_BackgroundMemory_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test_Nominal");
    UtTest_Add(CS_BackgroundMemory_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test_Miscompare");
    UtTest_Add(CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse");
    UtTest_Add(CS_BackgroundMemory_Test_MemoryCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test_MemoryCSStateDisabled");

    UtTest_Add(CS_BackgroundTables_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_Nominal");
    UtTest_Add(CS_BackgroundTables_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_Miscompare");
    UtTest_Add(CS_BackgroundTables_Test_TablesNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_TablesNotFound");
    UtTest_Add(CS_BackgroundTables_Test_FindEnabledTablesEntryFalse, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_FindEnabledTablesEntryFalse");
    UtTest_Add(CS_BackgroundTables_Test_TablesCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_TablesCSStateDisabled");

    UtTest_Add(CS_BackgroundApp_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_Nominal");
    UtTest_Add(CS_BackgroundApp_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_Miscompare");
    UtTest_Add(CS_BackgroundApp_Test_AppNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_AppNotFound");
    UtTest_Add(CS_BackgroundApp_Test_FindEnabledAppEntryFalse, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_FindEnabledAppEntryFalse");
    UtTest_Add(CS_BackgroundApp_Test_AppCSStateDisabled, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_AppCSStateDisabled");

    UtTest_Add(CS_ResetTablesTblResultEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetTablesTblResultEntry_Test");

} /* end CS_Utils_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
