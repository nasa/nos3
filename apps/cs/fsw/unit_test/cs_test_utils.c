 /*************************************************************************
 ** File:
 **   $Id: cs_test_utils.c 1.2 2017/02/16 15:33:20EST mdeschu Exp  $
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
 **   This file contains unit test utilities for the CS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

#include "cs_test_utils.h"
#include "cs_tbldefs.h"

/*
 * Function Definitions
 */

CS_Res_EepromMemory_Table_Entry_t   CS_DefaultEepromResTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES];
CS_Res_EepromMemory_Table_Entry_t   CS_DefaultMemoryResTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES];
CS_Res_Tables_Table_Entry_t         CS_DefaultTablesResTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES];
CS_Res_App_Table_Entry_t            CS_DefaultAppResTable[CS_MAX_NUM_APP_TABLE_ENTRIES];

void CS_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&CS_AppData, 0, sizeof(CS_AppData_t));

    CS_AppData.DefEepromTblPtr    = &CS_AppData.DefaultEepromDefTable[0];
    CS_AppData.ResEepromTblPtr    = &CS_DefaultEepromResTable[0];
    CS_AppData.DefMemoryTblPtr    = &CS_AppData.DefaultMemoryDefTable[0];
    CS_AppData.ResMemoryTblPtr    = &CS_DefaultMemoryResTable[0];
    CS_AppData.DefTablesTblPtr    = &CS_AppData.DefaultTablesDefTable[0];
    CS_AppData.ResTablesTblPtr    = &CS_DefaultTablesResTable[0];
    CS_AppData.DefAppTblPtr       = &CS_AppData.DefaultAppDefTable[0];
    CS_AppData.ResAppTblPtr       = &CS_DefaultAppResTable[0];

    memset(CS_DefaultEepromResTable, 0, sizeof(CS_Res_EepromMemory_Table_Entry_t)*CS_MAX_NUM_EEPROM_TABLE_ENTRIES);
    memset(CS_DefaultMemoryResTable, 0, sizeof(CS_Res_EepromMemory_Table_Entry_t)*CS_MAX_NUM_MEMORY_TABLE_ENTRIES);
    memset(CS_DefaultTablesResTable, 0, sizeof(CS_Res_Tables_Table_Entry_t)*CS_MAX_NUM_TABLES_TABLE_ENTRIES);
    memset(CS_DefaultAppResTable, 0, sizeof(CS_Res_App_Table_Entry_t)*CS_MAX_NUM_APP_TABLE_ENTRIES);
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end CS_Test_Setup */

void CS_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end CS_Test_TearDown */

/*
 * Additional UT-Assert Stub Functions and Required Data Structures
 *
 * Note: This code needs to be moved into the UT-Assert library.  We are including it here for now because the 
 * next release of the UT-Assert library is not expected to happen in the near future.
 */

Ut_CFE_PSP_MEMORY_HookTable_t          Ut_CFE_PSP_MEMORY_HookTable;
Ut_CFE_PSP_MEMORY_ReturnCodeTable_t    Ut_CFE_PSP_MEMORY_ReturnCodeTable[UT_CFE_PSP_MEMORY_MAX_INDEX];

void Ut_CFE_PSP_MEMORY_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if        (Index == UT_CFE_PSP_MEMORY_GETKERNELTEXTSEGMENTINFO_INDEX)      { Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetKernelTextSegmentInfo = FunPtr; }
    else if   (Index == UT_CFE_PSP_MEMORY_GETCFETEXTSEGMENTINFO_INDEX)         { Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetCFETextSegmentInfo = FunPtr; }
    else
    {
        printf("Unsupported PSP Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP Index In SetFunctionHook Call");
    }
}

void Ut_CFE_PSP_MEMORY_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_PSP_MEMORY_MAX_INDEX)
    {
        Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported PSP_MEMORY Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP_MEMORY Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_PSP_MEMORY_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_PSP_MEMORY_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_PSP_MEMORY_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_MEMORY_UseReturnCode(UT_CFE_PSP_MEMORY_GETKERNELTEXTSEGMENTINFO_INDEX))
        return Ut_CFE_PSP_MEMORY_ReturnCodeTable[UT_CFE_PSP_MEMORY_GETKERNELTEXTSEGMENTINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetKernelTextSegmentInfo)
        return(Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetKernelTextSegmentInfo(PtrToKernelSegment, SizeOfKernelSegment));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_MEMORY_UseReturnCode(UT_CFE_PSP_MEMORY_GETCFETEXTSEGMENTINFO_INDEX))
        return Ut_CFE_PSP_MEMORY_ReturnCodeTable[UT_CFE_PSP_MEMORY_GETCFETEXTSEGMENTINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetCFETextSegmentInfo)
        return(Ut_CFE_PSP_MEMORY_HookTable.CFE_PSP_GetCFETextSegmentInfo(PtrToCFESegment, SizeOfCFESegment));

    return(CFE_PSP_SUCCESS);
}

/************************/
/*  End of File Comment */
/************************/
