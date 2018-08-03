/*
** File: utf_cfe_psp_eeprom.c
** $Id: utf_cfe_psp_eeprom.c 1.5 2012/01/13 12:51:57GMT-05:00 acudmore Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose: This module defines the UTF versions of the cFE platform support package
**          EEPROM functions.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:57GMT-05:00 $
** $Revision: 1.5 $
** $Log: utf_cfe_psp_eeprom.c  $
** Revision 1.5 2012/01/13 12:51:57GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/11/29 08:45:13EST jmdagost 
** Enhanced support for CFE_PSP_EepromWriteEnable and CFE_PSP_EepromWriteDisable
** Revision 1.3 2010/10/25 15:06:35EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:11EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:31EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
*/

/*
**  Include Files
*/
#include "common_types.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "utf_types.h"
#include "utf_cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Function Hook Stuff */
typedef struct
{
    int32 (*CFE_PSP_EepromWrite8)(uint32, uint8);
    int32 (*CFE_PSP_EepromWrite16)(int32, uint16);
    int32 (*CFE_PSP_EepromWrite32)(int32, uint32);
    int32 (*CFE_PSP_EepromWriteEnable)(uint32);
    int32 (*CFE_PSP_EepromWriteDisable)(uint32);
} UTF_PSP_HookTable_t;

UTF_PSP_HookTable_t UTF_PSP_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

extern int32 cfe_psp_return_value[NUM_OF_CFE_PSP_PROCS];

/******************************************************************************
**  Function: UTF_PSP_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a CFE_PSP function call.
*/
void UTF_PSP_set_function_hook(int Index, void *FunPtr)
{
    if (Index == CFE_PSP_EEPROMWRITE8_HOOK) { UTF_PSP_HookTable.CFE_PSP_EepromWrite8 = FunPtr; }
    else if (Index == CFE_PSP_EEPROMWRITE16_HOOK) { UTF_PSP_HookTable.CFE_PSP_EepromWrite16 = FunPtr; }
    else if (Index == CFE_PSP_EEPROMWRITE32_HOOK) { UTF_PSP_HookTable.CFE_PSP_EepromWrite32 = FunPtr; }
    else if (Index == CFE_PSP_EEPROMWRITEENA_HOOK) { UTF_PSP_HookTable.CFE_PSP_EepromWriteEnable = FunPtr; }
    else if (Index == CFE_PSP_EEPROMWRITEDIS_HOOK) { UTF_PSP_HookTable.CFE_PSP_EepromWriteDisable = FunPtr; }
    else { UTF_error("Invalid PSP Hook Index In Set Hook Call %d", Index); }
}

/*******************************************************************************
                                  Memory Access API
*******************************************************************************/
int32 CFE_PSP_EepromWrite8(uint32 Address, uint8 Data)
{
    boolean Status;

    /* Handle the Function Hook */
    if (UTF_PSP_HookTable.CFE_PSP_EepromWrite8)
        return(UTF_PSP_HookTable.CFE_PSP_EepromWrite8(Address,Data));

    Status = UTF_write_sim_address(Address, 1, &Data);

    if(Status == FALSE)
        return(CFE_PSP_ERROR);

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWrite16(uint32 Address, uint16 Data)
{
    int32 Status;

        /* Handle the Function Hook */
    if (UTF_PSP_HookTable.CFE_PSP_EepromWrite16)
        return(UTF_PSP_HookTable.CFE_PSP_EepromWrite16(Address,Data));

    /* Check that the address is on a 16 bit boundary; the flight code
       will return the CFE_PSP_ERROR_ADDRESS_MISALIGNED return code. */
    if ((Address % 2) != 0)
    {   /* Address is not on 16 bit boundary */
         UTF_put_text("Address Not On 16 Bit Boundary: 0x%08x\n", Address);
        return (CFE_PSP_ERROR);
    }
    else
    {   /* Address is on 16 bit boundary */
        Status = UTF_write_sim_address(Address, 2, &Data);
        if(Status == FALSE)
                return(CFE_PSP_ERROR);
        else
            return(CFE_PSP_SUCCESS);
    }
}

int32 CFE_PSP_EepromWrite32(uint32 Address, uint32 Data)
{
    int32 Status;

        /* Handle the Function Hook */
    if (UTF_PSP_HookTable.CFE_PSP_EepromWrite32)
        return(UTF_PSP_HookTable.CFE_PSP_EepromWrite32(Address,Data));

    /* Check that the address is on a 32 bit boundary; the flight code
      will return the CFE_PSP_ERROR_ADDRESS_MISALIGNED return code. */
    if ((Address % 4) != 0)
    {   /* Address is not on 32 bit boundary */
        UTF_put_text("Address Not On 32 Bit Boundary: 0x%08x\n", Address);
        return (CFE_PSP_ERROR);
    }
    else
    {   /* Address is on 32 bit boundary */
        Status = UTF_write_sim_address(Address, 4, &Data);
        if(Status == FALSE)
                return(CFE_PSP_ERROR);
        else
            return(CFE_PSP_SUCCESS);
    }

}

int32 CFE_PSP_EepromWriteEnable(uint32 Bank)
{
    /* Handle the Function Hook */
    if (UTF_PSP_HookTable.CFE_PSP_EepromWriteEnable)
        return(UTF_PSP_HookTable.CFE_PSP_EepromWriteEnable(Bank));

    /* Handle Preset Return Code */
    if (cfe_psp_return_value[CFE_PSP_EEPROMWRITEENA_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
       return cfe_psp_return_value[CFE_PSP_EEPROMWRITEENA_PROC];
    }

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromWriteDisable(uint32 Bank)
{
    /* Handle the Function Hook */
    if (UTF_PSP_HookTable.CFE_PSP_EepromWriteDisable)
        return(UTF_PSP_HookTable.CFE_PSP_EepromWriteDisable(Bank));

    /* Handle Preset Return Code */
    if (cfe_psp_return_value[CFE_PSP_EEPROMWRITEDIS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
       return cfe_psp_return_value[CFE_PSP_EEPROMWRITEDIS_PROC];
    }

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromPowerUp(uint32 Bank)
{
    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_EepromPowerDown(uint32 Bank)
{
    return(CFE_PSP_SUCCESS);
}
