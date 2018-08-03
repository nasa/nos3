 /*************************************************************************
 ** File:
 **   $Id: cs_compute_test.c 1.4 2017/03/29 16:01:34EDT mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file cs_compute.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "cs_compute_test.h"
#include "cs_compute.h"
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

int32 CS_COMPUTE_TEST_CFE_TBL_GetAddressHook( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    /* This function exists so that one return code can be set for the 1st run and a different for the 2nd run */

    return CFE_TBL_ERR_UNREGISTERED;
}

int32 CS_COMPUTE_TEST_CFE_TBL_GetInfoHook1( CFE_TBL_Info_t *TblInfoPtr, const char *TblName )
{
    TblInfoPtr->Size = 5;

    return CFE_TBL_INFO_UPDATED;
}

int32 CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2( CFE_TBL_Info_t *TblInfoPtr, const char *TblName )
{
    TblInfoPtr->Size = 5;

    return CFE_SUCCESS;
}

int32 CS_COMPUTE_TEST_CFE_TBL_ShareHook( CFE_TBL_Handle_t *TblHandlePtr, const char *TblName )
{
    *TblHandlePtr = 99;

    return CFE_SUCCESS;
}

int32 CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    AppInfo->CodeSize            = 5;
    AppInfo->CodeAddress         = 1;
    AppInfo->AddressesAreValid   = TRUE;

    return CFE_SUCCESS;
}

int32 CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook2(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    AppInfo->AddressesAreValid   = FALSE;

    return CFE_SUCCESS;
}

void CS_ComputeEepromMemory_Test_Nominal(void)
{
    int32                               Result;
    CS_Res_EepromMemory_Table_Entry_t   ResultsEntry;
    uint32                              ComputedCSValue;
    boolean                             DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 1;

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeEepromMemory_Test_Nominal */

void CS_ComputeEepromMemory_Test_Error(void)
{
    int32                               Result;
    CS_Res_EepromMemory_Table_Entry_t   ResultsEntry;
    uint32                              ComputedCSValue;
    boolean                             DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 5;

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (Result == CS_ERROR, "Result == CS_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeEepromMemory_Test_Error */

void CS_ComputeEepromMemory_Test_FirstTimeThrough(void)
{
    int32                               Result;
    CS_Res_EepromMemory_Table_Entry_t   ResultsEntry;
    uint32                              ComputedCSValue;
    boolean                             DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = FALSE;

    /* ComputedCSValue and ResultsEntry.ComparisonValue will be set to value returned by this function */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.ComputedYet == TRUE, "ResultsEntry.ComputedYet == TRUE");
    UtAssert_True (ResultsEntry.ComparisonValue == 1, "ResultsEntry.ComparisonValue == 1");
    UtAssert_True (ComputedCSValue == 1, "ComputedCSValue == 1");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeEepromMemory_Test_FirstTimeThrough */

void CS_ComputeEepromMemory_Test_NotFinished(void)
{
    int32                               Result;
    CS_Res_EepromMemory_Table_Entry_t   ResultsEntry;
    uint32                              ComputedCSValue;
    boolean                             DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    ResultsEntry.NumBytesToChecksum = 2;
    CS_AppData.MaxBytesPerCycle     = 1;

    ResultsEntry.ComputedYet = FALSE;

    /* ComputedCSValue and ResultsEntry.TempChecksumValue will be set to value returned by this function */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (ResultsEntry.ByteOffset == 1, "ResultsEntry.ByteOffset == 1");
    UtAssert_True (ResultsEntry.TempChecksumValue == 1, "ResultsEntry.TempChecksumValue == 1");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeEepromMemory_Test_NotFinished */

void CS_ComputeTables_Test_TableNeverLoaded(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = 99;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy first instance of condition "ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_ERR_NEVER_LOADED, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_ERR_EID, CFE_EVS_ERROR, "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xCC000005"),
        "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xCC000005");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeTables_Test_TableNeverLoaded */

void CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = 99;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy condition "Result == CFE_TBL_ERR_UNREGISTERED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETINFO_INDEX, CFE_TBL_ERR_UNREGISTERED, 1);

    /* Satisfies second instance of conditions "Result == CFE_TBL_ERR_UNREGISTERED" and "ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED" */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetAddressHook);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_ERR_NEVER_LOADED, 2);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.TblHandle == 99, "ResultsEntry.TblHandle == 99");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");
    UtAssert_True (ResultsEntry.ComputedYet == FALSE, "ResultsEntry.ComputedYet == FALSE");
    UtAssert_True (ResultsEntry.ComparisonValue == 0, "ResultsEntry.ComparisonValue == 0");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");
    UtAssert_True (ResultsEntry.NumBytesToChecksum == 0, "ResultsEntry.NumBytesToChecksum == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_ERR_EID, CFE_EVS_ERROR, "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xCC000005"),
        "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xCC000005");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded */

void CS_ComputeTables_Test_ResultShareNotSuccess(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = 99;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy condition "Result == CFE_TBL_ERR_UNREGISTERED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETINFO_INDEX, CFE_TBL_ERR_UNREGISTERED, 1);

    /* Set to fail subsequent condition "ResultShare == CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_SHARE_INDEX, -1, 1);

    /* Set to satisfy second instance of condition "ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_ERR_UNREGISTERED, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.TblHandle == CFE_TBL_BAD_TABLE_HANDLE, "ResultsEntry.TblHandle == CFE_TBL_BAD_TABLE_HANDLE");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");
    UtAssert_True (ResultsEntry.ComputedYet == FALSE, "ResultsEntry.ComputedYet == FALSE");
    UtAssert_True (ResultsEntry.ComparisonValue == 0, "ResultsEntry.ComparisonValue == 0");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");
    UtAssert_True (ResultsEntry.NumBytesToChecksum == 0, "ResultsEntry.NumBytesToChecksum == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_ERR_EID, CFE_EVS_ERROR, "CS Tables: Problem Getting table name info Share: 0xFFFFFFFF, GetInfo: 0xCC000009, GetAddress: 0xCC000009"),
        "CS Tables: Problem Getting table name info Share: 0xFFFFFFFF, GetInfo: 0xCC000009, GetAddress: 0xCC000009");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeTables_Test_ResultShareNotSuccess */

void CS_ComputeTables_Test_TblInfoUpdated(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = 99;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    /* Sets TblInfo.Size = 5 and returns CFE_TBL_INFO_UPDATED */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook1);

    /* Set to satisfy subsequent condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");
    UtAssert_True (ComputedCSValue == 0, "ComputedCSValue == 0");
    UtAssert_True (ResultsEntry.ComputedYet == FALSE, "ResultsEntry.ComputedYet == FALSE");
    UtAssert_True (ResultsEntry.ComparisonValue == 0, "ResultsEntry.ComparisonValue == 0");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeTables_Test_TblInfoUpdated */

void CS_ComputeTables_Test_CSError(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = 99;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 1;

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");
    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CS_ERROR, "Result == CS_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeTables_Test_CSError */

void CS_ComputeTables_Test_NominalBadTableHandle(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.TblHandle == 99, "ResultsEntry.TblHandle == 99");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");
    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeTables_Test_NominalBadTableHandle */

void CS_ComputeTables_Test_FirstTimeThrough(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = FALSE;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 3, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.TblHandle == 99, "ResultsEntry.TblHandle == 99");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");

    UtAssert_True (ResultsEntry.ComputedYet == TRUE, "ResultsEntry.ComputedYet == TRUE");
    UtAssert_True (ResultsEntry.ComparisonValue == 3, "ResultsEntry.ComparisonValue == 3");

    UtAssert_True (ComputedCSValue == 3, "ComputedCSValue == 3");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeTables_Test_FirstTimeThrough */

void CS_ComputeTables_Test_EntryNotFinished(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 3;

    ResultsEntry.ComputedYet = FALSE;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 3, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.TblHandle == 99, "ResultsEntry.TblHandle == 99");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");

    UtAssert_True (ResultsEntry.ByteOffset == 3, "ResultsEntry.ByteOffset == 3");
    UtAssert_True (ResultsEntry.TempChecksumValue == 3, "ResultsEntry.TempChecksumValue == 3");
    UtAssert_True (ComputedCSValue == 3, "ComputedCSValue == 3");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeTables_Test_EntryNotFinished */

void CS_ComputeTables_Test_ComputeTablesReleaseError(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 3;

    ResultsEntry.ComputedYet = FALSE;

    ResultsEntry.ComparisonValue = 2;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 3, 1);

    /* Set to generate error message CS_COMPUTE_TABLES_RELEASE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_RELEASEADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.TblHandle == 99, "ResultsEntry.TblHandle == 99");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 0, "ResultsEntry.StartAddress == 0");

    UtAssert_True (ResultsEntry.ByteOffset == 3, "ResultsEntry.ByteOffset == 3");
    UtAssert_True (ResultsEntry.TempChecksumValue == 3, "ResultsEntry.TempChecksumValue == 3");
    UtAssert_True (ComputedCSValue == 3, "ComputedCSValue == 3");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_RELEASE_ERR_EID, CFE_EVS_ERROR, "CS Tables: Could not release addresss for table name, returned: 0xFFFFFFFF"),
        "CS Tables: Could not release addresss for table name, returned: 0xFFFFFFFF");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeTables_Test_ComputeTablesReleaseError */

void CS_ComputeTables_Test_ComputeTablesError(void)
{
    int32                          Result;
    CS_Res_Tables_Table_Entry_t    ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook2);

    /* Set to generate error message CS_COMPUTE_TABLES_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_TABLES_ERR_EID, CFE_EVS_ERROR, "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xFFFFFFFF"),
        "CS Tables: Problem Getting table name info Share: 0x00000000, GetInfo: 0x00000000, GetAddress: 0xFFFFFFFF");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeTables_Test_ComputeTablesError */

void CS_ComputeApp_Test_Nominal(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 2;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 1, "ResultsEntry.StartAddress == 1");

    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeApp_Test_Nominal */

void CS_ComputeApp_Test_GetAppIDByNameError(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_APP_ERR_EID, CFE_EVS_ERROR, "CS Apps: Problems getting app name info, GetAppID: 0xFFFFFFFF, GetAppInfo: 0xFFFFFFFF, AddressValid: 0"),
        "CS Apps: Problems getting app name info, GetAppID: 0xFFFFFFFF, GetAppInfo: 0xFFFFFFFF, AddressValid: 0");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeApp_Test_GetAppIDByNameError */

void CS_ComputeApp_Test_GetAppInfoError(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPINFO_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_APP_ERR_EID, CFE_EVS_ERROR, "CS Apps: Problems getting app name info, GetAppID: 0x00000000, GetAppInfo: 0xFFFFFFFF, AddressValid: 0"),
        "CS Apps: Problems getting app name info, GetAppID: 0x00000000, GetAppInfo: 0xFFFFFFFF, AddressValid: 0");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_ComputeApp_Test_GetAppInfoError */

void CS_ComputeApp_Test_ComputeAppPlatformError(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets AppInfo.AddressesAreValid = FALSE and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook2);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_APP_PLATFORM_DBG_EID, CFE_EVS_DEBUG, "CS cannot get a valid address for name, due to the platform"),
        "CS cannot get a valid address for name, due to the platform");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_COMPUTE_APP_ERR_EID, CFE_EVS_ERROR, "CS Apps: Problems getting app name info, GetAppID: 0x00000000, GetAppInfo: 0x00000000, AddressValid: 0"),
        "CS Apps: Problems getting app name info, GetAppID: 0x00000000, GetAppInfo: 0x00000000, AddressValid: 0");

    UtAssert_True (Result == CS_ERR_NOT_FOUND, "Result == CS_ERR_NOT_FOUND");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end CS_ComputeApp_Test_ComputeAppPlatformError */

void CS_ComputeApp_Test_DifferFromSavedValue(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 1, "ResultsEntry.StartAddress == 1");

    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CS_ERROR, "Result == CS_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeApp_Test_DifferFromSavedValue */

void CS_ComputeApp_Test_FirstTimeThrough(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    ResultsEntry.ComputedYet = FALSE;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == TRUE, "DoneWithEntry == TRUE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 1, "ResultsEntry.StartAddress == 1");

    UtAssert_True (ResultsEntry.ComputedYet == TRUE, "ResultsEntry.ComputedYet == TRUE");
    UtAssert_True (ResultsEntry.ComparisonValue == 2, "ResultsEntry.ComparisonValue == 2");

    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");
    UtAssert_True (ResultsEntry.ByteOffset == 0, "ResultsEntry.ByteOffset == 0");
    UtAssert_True (ResultsEntry.TempChecksumValue == 0, "ResultsEntry.TempChecksumValue == 0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeApp_Test_FirstTimeThrough */

void CS_ComputeApp_Test_EntryNotFinished(void)
{
    int32                          Result;
    CS_Res_App_Table_Entry_t       ResultsEntry;
    uint32                         ComputedCSValue;
    boolean                        DoneWithEntry;

    ResultsEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 3;

    ResultsEntry.ComputedYet = TRUE;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 2, 1);

    /* Execute the function being tested */
    Result = CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry);
    
    /* Verify results */
    UtAssert_True (DoneWithEntry == FALSE, "DoneWithEntry == FALSE");

    UtAssert_True (ResultsEntry.NumBytesToChecksum == 5, "ResultsEntry.NumBytesToChecksum == 5");
    UtAssert_True (ResultsEntry.StartAddress == 1, "ResultsEntry.StartAddress == 1");

    UtAssert_True (ResultsEntry.ByteOffset == 3, "ResultsEntry.ByteOffset == 3");
    UtAssert_True (ResultsEntry.TempChecksumValue == 2, "ResultsEntry.TempChecksumValue == 2");
    UtAssert_True (ComputedCSValue == 2, "ComputedCSValue == 2");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_ComputeApp_Test_EntryNotFinished */

void CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable(void)
{
    CS_Res_EepromMemory_Table_Entry_t     RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t     DefEepromTbl[10];

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefEepromTblPtr               = DefEepromTbl;

    CS_AppData.ChildTaskTable = CS_EEPROM_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefEepromTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefEepromTblPtr[1].StartAddress;

    DefEepromTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99, "CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefEepromTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefEepromTblPtr[CS_AppData.ChildTaskEntryID].State == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Eeprom entry 1 recompute finished. New baseline is 0X00000001"),
        "Eeprom entry 1 recompute finished. New baseline is 0X00000001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable */

void CS_RecomputeEepromMemoryChildTask_Test_MemoryTable(void)
{
    CS_Res_EepromMemory_Table_Entry_t     RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t     DefMemoryTbl[10];

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_MEMORY_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99, "CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, CFE_EVS_INFORMATION, "Memory entry 1 recompute finished. New baseline is 0X00000001"),
        "Memory entry 1 recompute finished. New baseline is 0X00000001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeEepromMemoryChildTask_Test_MemoryTable */

void CS_RecomputeEepromMemoryChildTask_Test_CFECore(void)
{
    CS_Res_EepromMemory_Table_Entry_t     RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t     DefMemoryTbl[10];

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_CFECORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99, "CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, CFE_EVS_INFORMATION, "cFE Core entry 1 recompute finished. New baseline is 0X00000001"),
        "cFE Core entry 1 recompute finished. New baseline is 0X00000001");

    UtAssert_True (CS_AppData.CfeCoreBaseline == 1, "CS_AppData.CfeCoreBaseline == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeEepromMemoryChildTask_Test_CFECore */

void CS_RecomputeEepromMemoryChildTask_Test_OSCore(void)
{
    CS_Res_EepromMemory_Table_Entry_t     RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t     DefMemoryTbl[10];

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet == TRUE");
    UtAssert_True (CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99, "CS_AppData.RecomputeEepromMemoryEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, CFE_EVS_INFORMATION, "OS entry 1 recompute finished. New baseline is 0X00000001"),
        "OS entry 1 recompute finished. New baseline is 0X00000001");

    UtAssert_True (CS_AppData.OSBaseline == 1, "CS_AppData.OSBaseline == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeEepromMemoryChildTask_Test_OSCore */

void CS_RecomputeEepromMemoryChildTask_Test_RegisterChildTaskError(void)
{
    /* Set to cause message "Recompute for Eeprom or Memory Child Task Registration failed!" to be printed */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();
    
    /* Verify results */
    /* Note: Cannot verify line OS_printf("Recompute for Eeprom or Memory Child Task Registration failed!\n") */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_RecomputeEepromMemoryChildTask_Test_RegisterChildTaskError */

void CS_RecomputeAppChildTask_Test_Nominal(void)
{
    CS_Res_App_Table_Entry_t     RecomputeAppEntry;
    CS_Def_App_Table_Entry_t     DefAppTbl[10];

    CS_AppData.RecomputeAppEntryPtr   = &RecomputeAppEntry;
    CS_AppData.DefAppTblPtr           = DefAppTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefAppTbl[1].State = 1;

    CS_AppData.RecomputeAppEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeAppEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_APP_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_CALCULATECRC_INDEX);

    CS_AppData.RecomputeAppEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeAppEntryPtr->Name, "name", 10);
    strncpy(DefAppTbl[1].Name, "name", 10);

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->State == 99, "CS_AppData.RecomputeAppEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State == 1");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeAppEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeAppEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeAppEntryPtr->ComputedYet == TRUE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_APP_INF_EID, CFE_EVS_INFORMATION, "App name recompute finished. New baseline is 0x00000001"),
        "App name recompute finished. New baseline is 0x00000001");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeAppChildTask_Test_Nominal */

void CS_RecomputeAppChildTask_Test_CouldNotGetAddress(void)
{
    CS_Res_App_Table_Entry_t     RecomputeAppEntry;
    CS_Def_App_Table_Entry_t     DefAppTbl[10];

    CS_AppData.RecomputeAppEntryPtr   = &RecomputeAppEntry;
    CS_AppData.DefAppTblPtr           = DefAppTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefAppTbl[1].State = 1;

    CS_AppData.RecomputeAppEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeAppEntryPtr->Name, "name", 10);
    strncpy(DefAppTbl[1].Name, "name", 10);

    /* Set to cause CS_ComputeApp to return CS_ERR_NOT_FOUND */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETAPPIDBYNAME_INDEX, -1, 1);

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = TRUE, and returns CFE_SUCCESS */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETAPPINFO_INDEX, &CS_COMPUTE_TEST_CFE_ES_GetAppInfoHook1);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->State == 99, "CS_AppData.RecomputeAppEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State == 1");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeAppEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeAppEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeAppEntryPtr->ComputedYet == FALSE, "CS_AppData.RecomputeAppEntryPtr->ComputedYet == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_ERROR_APP_ERR_EID, CFE_EVS_ERROR, "App name recompute failed. Could not get address"),
        "App name recompute failed. Could not get address");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end CS_RecomputeAppChildTask_Test_CouldNotGetAddress */

void CS_RecomputeAppChildTask_Test_RegisterChildTaskError(void)
{
    /* Set to cause message "Recompute for App Child Task Registration failed!" to be printed */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();
    
    /* Verify results */
    /* Note: Cannot verify line OS_printf("Recompute for App Child Task Registration failed!\n") */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_RecomputeAppChildTask_Test_RegisterChildTaskError */

void CS_RecomputeTablesChildTask_Test_Nominal(void)
{
    CS_Res_Tables_Table_Entry_t     RecomputeTablesEntry;
    CS_Def_Tables_Table_Entry_t     DefTablesTbl[10];

    CS_AppData.RecomputeTablesEntryPtr   = &RecomputeTablesEntry;
    CS_AppData.DefTablesTblPtr           = DefTablesTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefTablesTbl[1].State = 1;

    CS_AppData.RecomputeTablesEntryPtr->ByteOffset         = 0;
    CS_AppData.RecomputeTablesEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_TABLES_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_CALCULATECRC_INDEX);

    CS_AppData.RecomputeTablesEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeTablesEntryPtr->Name, "name", 10);
    strncpy(DefTablesTbl[1].Name, "name", 10);

    RecomputeTablesEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    RecomputeTablesEntry.ByteOffset         = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    RecomputeTablesEntry.ComputedYet = TRUE;

    RecomputeTablesEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_SHARE_INDEX, &CS_COMPUTE_TEST_CFE_TBL_ShareHook);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETINFO_INDEX, &CS_COMPUTE_TEST_CFE_TBL_GetInfoHook1);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of Result */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->State == 99, "CS_AppData.RecomputeTablesEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State == 1");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeTablesEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->ComputedYet == TRUE, "CS_AppData.RecomputeTablesEntryPtr->ComputedYet == TRUE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_FINISH_TABLES_INF_EID, CFE_EVS_INFORMATION, "Table name recompute finished. New baseline is 0x00000001"),
        "Table name recompute finished. New baseline is 0x00000001");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.ChildTaskInUse == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_RecomputeTablesChildTask_Test_Nominal */

void CS_RecomputeTablesChildTask_Test_CouldNotGetAddress(void)
{
    CS_Res_Tables_Table_Entry_t     RecomputeTablesEntry;
    CS_Def_Tables_Table_Entry_t     DefTablesTbl[10];

    CS_AppData.RecomputeTablesEntryPtr   = &RecomputeTablesEntry;
    CS_AppData.DefTablesTblPtr           = DefTablesTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefTablesTbl[1].State = 1;

    CS_AppData.RecomputeTablesEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeTablesEntryPtr->Name, "name", 10);
    strncpy(DefTablesTbl[1].Name, "name", 10);

    /* Set to make CS_ComputeTables return CS_ERR_NOT_FOUND */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_SHARE_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->State == 99, "CS_AppData.RecomputeTablesEntryPtr->State == 99");
    UtAssert_True (CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State == 1, "CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State == 1");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue == 0, "CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue == 0");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->ByteOffset == 0, "CS_AppData.RecomputeTablesEntryPtr->ByteOffset == 0");
    UtAssert_True (CS_AppData.RecomputeTablesEntryPtr->ComputedYet == FALSE, "CS_AppData.RecomputeTablesEntryPtr->ComputedYet == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_RECOMPUTE_ERROR_TABLES_ERR_EID, CFE_EVS_ERROR, "Table name recompute failed. Could not get address"),
        "Table name recompute failed. Could not get address");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Note: generates 1 event message we don't care about in this test */

} /* end CS_RecomputeTablesChildTask_Test_CouldNotGetAddress */

void CS_RecomputeTablesChildTask_Test_RegisterChildTaskError(void)
{
    /* Set to cause message "Recompute Tables Child Task Registration failed!" to be printed */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();
    
    /* Verify results */
    /* Note: Cannot verify line OS_printf("Recompute Tables Child Task Registration failed!\n") */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_RecomputeTablesChildTask_Test_RegisterChildTaskError */

void CS_OneShotChildTask_Test_Nominal(void)
{
    /* NewChecksumValue will be set to value returned by this function */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 1, 1);

    CS_AppData.LastOneShotAddress  = 0;
    CS_AppData.LastOneShotSize     = 1;
    CS_AppData.LastOneShotChecksum = 1;
    CS_AppData.LastOneShotMaxBytesPerCycle = 1;

    /* Execute the function being tested */
    CS_OneShotChildTask();
    
    /* Verify results */
    UtAssert_True (CS_AppData.LastOneShotChecksum == 1, "CS_AppData.LastOneShotChecksum == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(CS_ONESHOT_FINISHED_INF_EID, CFE_EVS_INFORMATION, "OneShot checksum on Address: 0x00000000, size 1 completed. Checksum =  0x00000001"),
        "OneShot checksum on Address: 0x00000000, size 1 completed. Checksum =  0x00000001");

    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskID == 0, "CS_AppData.ChildTaskID == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end CS_OneShotChildTask_Test_Nominal */

void CS_OneShotChildTask_Test_RegisterChildTaskError(void)
{
    /* Set to cause message "OneShot Child Task Registration failed!" to be printed */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    CS_OneShotChildTask();
    
    /* Verify results */
    /* Note: Cannot verify line OS_printf("OneShot Child Task Registration failed!\n") */
    UtAssert_True (CS_AppData.RecomputeInProgress == FALSE, "CS_AppData.RecomputeInProgress == FALSE");
    UtAssert_True (CS_AppData.OneShotInProgress == FALSE, "CS_AppData.OneShotInProgress == FALSE");
    UtAssert_True (CS_AppData.ChildTaskID == 0, "CS_AppData.ChildTaskID == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end CS_OneShotChildTask_Test_RegisterChildTaskError */

void CS_Compute_Test_AddTestCases(void)
{
    UtTest_Add(CS_ComputeEepromMemory_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeEepromMemory_Test_Nominal");
    UtTest_Add(CS_ComputeEepromMemory_Test_Error, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeEepromMemory_Test_Error");
    UtTest_Add(CS_ComputeEepromMemory_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeEepromMemory_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeEepromMemory_Test_NotFinished, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeEepromMemory_Test_NotFinished");

    UtTest_Add(CS_ComputeTables_Test_TableNeverLoaded, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_TableNeverLoaded");
    UtTest_Add(CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded");
    UtTest_Add(CS_ComputeTables_Test_ResultShareNotSuccess, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_ResultShareNotSuccess");
    UtTest_Add(CS_ComputeTables_Test_TblInfoUpdated, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_TblInfoUpdated");
    UtTest_Add(CS_ComputeTables_Test_CSError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_CSError");
    UtTest_Add(CS_ComputeTables_Test_NominalBadTableHandle, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_NominalBadTableHandle");
    UtTest_Add(CS_ComputeTables_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeTables_Test_EntryNotFinished, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_EntryNotFinished");
    UtTest_Add(CS_ComputeTables_Test_ComputeTablesReleaseError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_ComputeTablesReleaseError");
    UtTest_Add(CS_ComputeTables_Test_ComputeTablesError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_ComputeTablesError");

    UtTest_Add(CS_ComputeApp_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_Nominal");
    UtTest_Add(CS_ComputeApp_Test_GetAppIDByNameError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_GetAppIDByNameError");
    UtTest_Add(CS_ComputeApp_Test_GetAppInfoError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_GetAppInfoError");
    UtTest_Add(CS_ComputeApp_Test_ComputeAppPlatformError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_ComputeAppPlatformError");
    UtTest_Add(CS_ComputeApp_Test_DifferFromSavedValue, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_DifferFromSavedValue");
    UtTest_Add(CS_ComputeApp_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeApp_Test_EntryNotFinished, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_EntryNotFinished");

    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_MemoryTable, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeEepromMemoryChildTask_Test_MemoryTable");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_CFECore, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeEepromMemoryChildTask_Test_CFECore");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_OSCore, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeEepromMemoryChildTask_Test_OSCore");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_RegisterChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeEepromMemoryChildTask_Test_RegisterChildTaskError");

    UtTest_Add(CS_RecomputeAppChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeAppChildTask_Test_Nominal");
    UtTest_Add(CS_RecomputeAppChildTask_Test_CouldNotGetAddress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeAppChildTask_Test_CouldNotGetAddress");
    UtTest_Add(CS_RecomputeAppChildTask_Test_RegisterChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeAppChildTask_Test_RegisterChildTaskError");

    UtTest_Add(CS_RecomputeTablesChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeTablesChildTask_Test_Nominal");
    UtTest_Add(CS_RecomputeTablesChildTask_Test_CouldNotGetAddress, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeTablesChildTask_Test_CouldNotGetAddress");
    UtTest_Add(CS_RecomputeTablesChildTask_Test_RegisterChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_RecomputeTablesChildTask_Test_RegisterChildTaskError");

    UtTest_Add(CS_OneShotChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotChildTask_Test_Nominal");
    UtTest_Add(CS_OneShotChildTask_Test_RegisterChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotChildTask_Test_RegisterChildTaskError");

} /* end CS_Compute_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
