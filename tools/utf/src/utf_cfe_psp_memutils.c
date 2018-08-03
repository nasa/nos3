/*
** File: utf_cfe_psp_memutils.c
** $Id: utf_cfe_psp_memutils.c 1.4 2012/01/13 12:51:58GMT-05:00 acudmore Exp  $
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
**          memory utility functions.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:58GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_cfe_psp_memutils.c  $
** Revision 1.4 2012/01/13 12:51:58GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:34EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:19EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:40EDT wmoleski 
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


/* This also works when the src, dest, or both addresses reside within the simulated
   memory address space */
int32 CFE_PSP_MemCpy(void *Dest,  void *Src, uint32 Size)
{
    /* If Dest is in simulated memory address space */
    if (UTF_sim_address_lookup((uint32)Dest, (uint16)Size) != NULL)
    {
        /* and Src is in simulated memory address space */
        if (UTF_sim_address_lookup((uint32)Src, (uint16)Size) != NULL)
        {
            /* Then perform copy operation */
            UTF_put_text("IO MemCpy, Dest: 0x%08x [%s], Src: 0x%08x [%s], Size: %d\n",
                        Dest, UTF_sim_name_lookup((uint32)Dest), Src, UTF_sim_name_lookup((uint32)Src), Size);
            memcpy(UTF_sim_address_lookup((uint32)Dest, (uint16)Size),
                   UTF_sim_address_lookup((uint32)Src, (uint16)Size), Size);
        }
        else
        {   /* Case: Dest in simulated memory; Src is not. */
            UTF_put_text("IO MemCpy, Dest: 0x%08x [%s], Src: 0x%08x [%s], Size: %d\n",
                          Dest, UTF_sim_name_lookup((uint32)Dest), Src, UTF_sim_name_lookup((uint32)Src), Size);
            memcpy(UTF_sim_address_lookup((uint32)Dest, (uint16)Size), (char *)Src, Size);
        }
    }
    else
    {
        if (UTF_sim_address_lookup((uint32)Src, (uint16)Size) != NULL)
        {
           UTF_put_text("IO MemCpy, Dest: 0x%08x [%s], Src: 0x%08x [%s], Size: %d\n",
			Dest, UTF_sim_name_lookup((uint32)Dest), Src, UTF_sim_name_lookup((uint32)Src), Size);
            memcpy((char *)Dest, UTF_sim_address_lookup((uint32)Src, (uint16)Size), Size);
        }
        else
        {
            memcpy(Dest, Src, Size);
        }
    }
    return(CFE_PSP_SUCCESS);
}

/* This also works when the dest address resides within the simulated memory address space */
int32 CFE_PSP_MemSet(void *Dest, uint8 Value, uint32 Size)
{
    if (UTF_sim_address_lookup((uint32)Dest, (uint16)Size) != NULL)
    {
        UTF_put_text("IO MemSet, Dest: 0x%08x [%s], Data: 0x%02x, Size: %d\n",                            Dest, UTF_sim_name_lookup((uint32)Dest), Value, Size);
        memset(UTF_sim_address_lookup((uint32)Dest, (uint16)Size), Value, Size);
    }    else
    {
        memset(Dest, Value, Size);    }
    return(CFE_PSP_SUCCESS);
}
