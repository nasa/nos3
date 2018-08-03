/*
**
** File: ut_cfe_psp_memrange_stubs.c
**
** $Id: ut_cfe_psp_memrange_stubs.c 1.2 2016/06/23 14:39:05EDT czogby Exp  $
**
** Copyright 2016 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: Unit test stubs for cFE PSP Memory Range routines
**
** $Log: ut_cfe_psp_memrange_stubs.c  $
** Revision 1.2 2016/06/23 14:39:05EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/04/10 21:20:02EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
**
*/

/*
**  Include Files
*/
#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_psp_memrange_stubs.h"
#include <string.h>

Ut_CFE_PSP_MEMRANGE_HookTable_t          Ut_CFE_PSP_MEMRANGE_HookTable;
Ut_CFE_PSP_MEMRANGE_ReturnCodeTable_t    Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[UT_CFE_PSP_MEMRANGE_MAX_INDEX];

void Ut_CFE_PSP_MEMRANGE_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX)      { Ut_CFE_PSP_MEMRANGE_HookTable.CFE_PSP_MemValidateRange = FunPtr; }
    else
    {
        printf("Unsupported PSP Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP Index In SetFunctionHook Call");
    }
}

void Ut_CFE_PSP_MEMRANGE_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_PSP_MEMRANGE_MAX_INDEX)
    {
        Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported PSP_MEMRANGE Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP_MEMRANGE Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_PSP_MEMRANGE_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_PSP_MEMRANGE_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_PSP_MemValidateRange(cpuaddr Address, uint32 Size, uint32 MemoryType)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_MEMRANGE_UseReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX))
        return Ut_CFE_PSP_MEMRANGE_ReturnCodeTable[UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_MEMRANGE_HookTable.CFE_PSP_MemValidateRange)
        return(Ut_CFE_PSP_MEMRANGE_HookTable.CFE_PSP_MemValidateRange(Address, Size, MemoryType));

    return(CFE_PSP_SUCCESS);
}

