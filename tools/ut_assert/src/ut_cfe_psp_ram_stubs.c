/*
**
** File: ut_cfe_psp_ram_stubs.c
**
** $Id: ut_cfe_psp_ram_stubs.c 1.2 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE PSP RAM routines
**
** $Log: ut_cfe_psp_ram_stubs.c  $
** Revision 1.2 2016/06/23 14:39:04EDT czogby 
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
#include "ut_cfe_psp_ram_stubs.h"
#include <string.h>

Ut_CFE_PSP_RAM_HookTable_t          Ut_CFE_PSP_RAM_HookTable;
Ut_CFE_PSP_RAM_ReturnCodeTable_t    Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MAX_INDEX];

void Ut_CFE_PSP_RAM_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_PSP_RAM_MEMREAD8_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead8 = FunPtr; }
    else if (Index == UT_CFE_PSP_RAM_MEMREAD16_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead16 = FunPtr; }
    else if (Index == UT_CFE_PSP_RAM_MEMREAD32_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead32 = FunPtr; }
    else if (Index == UT_CFE_PSP_RAM_MEMWRITE8_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite8 = FunPtr; }
    else if (Index == UT_CFE_PSP_RAM_MEMWRITE16_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite16 = FunPtr; }
    else if (Index == UT_CFE_PSP_RAM_MEMWRITE32_INDEX)         { Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite32 = FunPtr; }
    else
    {
        printf("Unsupported PSP Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP Index In SetFunctionHook Call");
    }
}

void Ut_CFE_PSP_RAM_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_PSP_RAM_MAX_INDEX)
    {
        Ut_CFE_PSP_RAM_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_PSP_RAM_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported PSP_RAM Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP_RAM Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_PSP_RAM_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_PSP_RAM_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_PSP_RAM_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_PSP_RAM_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_PSP_RAM_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_PSP_RAM_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_PSP_RAM_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_PSP_MemRead8( cpuaddr MemoryAddress, uint8 *ByteValue )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMREAD8_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMREAD8_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead8)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead8(MemoryAddress, ByteValue));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemRead16( cpuaddr MemoryAddress, uint16 *uint16Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMREAD16_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMREAD16_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead16)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead16(MemoryAddress, uint16Value));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemRead32( cpuaddr MemoryAddress, uint32 *uint32Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMREAD32_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMREAD32_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead32)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemRead32(MemoryAddress, uint32Value));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemWrite8( cpuaddr MemoryAddress, uint8 ByteValue )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMWRITE8_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMWRITE8_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite8)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite8(MemoryAddress, ByteValue));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemWrite16( cpuaddr MemoryAddress, uint16 uint16Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMWRITE16_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMWRITE16_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite16)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite16(MemoryAddress, uint16Value));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemWrite32( cpuaddr MemoryAddress, uint32 uint32Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_RAM_UseReturnCode(UT_CFE_PSP_RAM_MEMWRITE32_INDEX))
        return Ut_CFE_PSP_RAM_ReturnCodeTable[UT_CFE_PSP_RAM_MEMWRITE32_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite32)
        return(Ut_CFE_PSP_RAM_HookTable.CFE_PSP_MemWrite32(MemoryAddress, uint32Value));

    return(CFE_PSP_SUCCESS);
}
