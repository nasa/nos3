/*
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
*/

/**
 * \file cfe_psp_eeprom_stub.c
 *
 *  Created on: Jul 17, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * This is a stub implementation of the PSP EEPROM API calls that return CFE_PSP_ERROR_NOT_IMPLEMENTED
 */

#include "cfe_psp.h"
#include "cfe_psp_module.h"

CFE_PSP_MODULE_DECLARE_SIMPLE(eeprom_stub);

void eeprom_stub_Init(void)
{
    /* Nothing to init */
}


int32 CFE_PSP_EepromWrite32( cpuaddr MemoryAddress, uint32 uint32Value )
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromWrite16( cpuaddr MemoryAddress, uint16 uint16Value )
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromWrite8( cpuaddr MemoryAddress, uint8 ByteValue )
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromWriteEnable(uint32 Bank)
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromWriteDisable(uint32 Bank)
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromPowerUp(uint32 Bank)
{
    return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

int32 CFE_PSP_EepromPowerDown(uint32 Bank)
{
   return(CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

