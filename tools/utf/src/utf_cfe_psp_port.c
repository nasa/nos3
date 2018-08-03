/*
** File: utf_cfe_psp_port.c
** $Id: utf_cfe_psp_port.c 1.4 2012/01/13 12:51:58GMT-05:00 acudmore Exp  $
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
** Purpose: This module defines the UTF versions of functions for the cFE 
**          platform support package Port I/O.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:58GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_cfe_psp_port.c  $
** Revision 1.4 2012/01/13 12:51:58GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:45EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:21EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:43EDT wmoleski 
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

int32 CFE_PSP_PortRead8(uint32 Address, uint8 *Data)
{
    boolean Status;

    Status = UTF_read_sim_address(Address, 1, Data);

    if(Status == FALSE)
        return(CFE_PSP_ERROR);

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_PortWrite8(uint32 Address, uint8 Data)
{
    boolean Status;

    Status = UTF_write_sim_address(Address, 1, &Data);

    if(Status == FALSE)
        return(CFE_PSP_ERROR);

    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_PortRead16(uint32 Address, uint16 *Data)
{
    boolean Status;

    /* Check that the address is on a 16 bit boundary; the flight code
       will return the CFE_PSP_ERROR_ADDRESS_MISALIGNED return code. */
    if ((Address % 2) != 0)
    {   /* Address is not on 16 bit boundary */
        UTF_put_text("Address Not On 16 Bit Boundary: 0x%08x\n", Address);
        return (CFE_PSP_ERROR);
    }
    else
    {   /* Address is on 16 bit boundary */
        Status = UTF_read_sim_address(Address, 2, Data);

        if(Status == FALSE)
            return(CFE_PSP_ERROR);
        else
            return(CFE_PSP_SUCCESS);
    }
}

int32 CFE_PSP_PortWrite16(uint32 Address, uint16 Data)
{
    boolean Status;

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

int32 CFE_PSP_PortRead32(uint32 Address, uint32 *Data)
{
    boolean Status;

    /* Check that the address is on a 32 bit boundary; the flight code
      will return the CFE_PSP_ERROR_ADDRESS_MISALIGNED return code. */
    if ((Address % 4) != 0)
    {   /* Address is not on 32 bit boundary */
        UTF_put_text("Address Not On 32 Bit Boundary: 0x%08x\n", Address);
        return (CFE_PSP_ERROR);
    }
    else
    {   /* Address is on 32 bit boundary */
        Status = UTF_read_sim_address(Address, 4, Data);

        if(Status == FALSE)
            return(CFE_PSP_ERROR);
        else
            return(CFE_PSP_SUCCESS);
    }
}

int32 CFE_PSP_PortWrite32(uint32 Address, uint32 Data)
{
    boolean Status;

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
