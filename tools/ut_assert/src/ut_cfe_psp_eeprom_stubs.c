/*
**
** File: ut_cfe_psp_eeprom_stubs.c
**
** $Id: ut_cfe_psp_eeprom_stubs.c 1.2 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE PSP EEPROM routines
**
** $Log: ut_cfe_psp_eeprom_stubs.c  $
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
#include "ut_cfe_psp_eeprom_stubs.h"
#include <string.h>

Ut_CFE_PSP_EEPROM_HookTable_t          Ut_CFE_PSP_EEPROM_HookTable;
Ut_CFE_PSP_EEPROM_ReturnCodeTable_t    Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_MAX_INDEX];

void Ut_CFE_PSP_EEPROM_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_PSP_EEPROM_WRITEENABLE_INDEX)         { Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteEnable = FunPtr; }
    else if (Index == UT_CFE_PSP_EEPROM_WRITEDISABLE_INDEX)         { Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteDisable = FunPtr; }
    else if (Index == UT_CFE_PSP_EEPROM_WRITE8_INDEX)         { Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite8 = FunPtr; }
    else if (Index == UT_CFE_PSP_EEPROM_WRITE16_INDEX)         { Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite16 = FunPtr; }
    else if (Index == UT_CFE_PSP_EEPROM_WRITE32_INDEX)         { Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite32 = FunPtr; }
    else
    {
        printf("Unsupported PSP Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP Index In SetFunctionHook Call");
    }
}

void Ut_CFE_PSP_EEPROM_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_PSP_EEPROM_MAX_INDEX)
    {
        Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported PSP_EEPROM Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP_EEPROM Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_PSP_EEPROM_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_PSP_EEPROM_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_PSP_EEPROM_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_PSP_EepromWriteEnable(uint32 Bank)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_EEPROM_UseReturnCode(UT_CFE_PSP_EEPROM_WRITEENABLE_INDEX))
        return Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_WRITEENABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteEnable)
        return(Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteEnable(Bank));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWriteDisable(uint32 Bank)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_EEPROM_UseReturnCode(UT_CFE_PSP_EEPROM_WRITEDISABLE_INDEX))
        return Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_WRITEDISABLE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteDisable)
        return(Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWriteDisable(Bank));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWrite8( cpuaddr MemoryAddress, uint8 ByteValue )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_EEPROM_UseReturnCode(UT_CFE_PSP_EEPROM_WRITE8_INDEX))
        return Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_WRITE8_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite8)
        return(Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite8(MemoryAddress, ByteValue));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWrite16( cpuaddr MemoryAddress, uint16 uint16Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_EEPROM_UseReturnCode(UT_CFE_PSP_EEPROM_WRITE16_INDEX))
        return Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_WRITE16_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite16)
        return(Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite16(MemoryAddress, uint16Value));

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWrite32( cpuaddr MemoryAddress, uint32 uint32Value )
{
    /* Check for specified return */
    if (Ut_CFE_PSP_EEPROM_UseReturnCode(UT_CFE_PSP_EEPROM_WRITE32_INDEX))
        return Ut_CFE_PSP_EEPROM_ReturnCodeTable[UT_CFE_PSP_EEPROM_WRITE32_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite32)
        return(Ut_CFE_PSP_EEPROM_HookTable.CFE_PSP_EepromWrite32(MemoryAddress, uint32Value));

    return(CFE_PSP_SUCCESS);
}
