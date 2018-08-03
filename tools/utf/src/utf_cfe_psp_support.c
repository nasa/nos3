/*
** File: utf_psp_support.c
** $Id: utf_cfe_psp_support.c 1.4 2012/01/13 12:51:59GMT-05:00 acudmore Exp  $
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
**          platform support package.
**          This file contains glue routines between the cFE and the Platform
**          Support Package (PSP).
**          The functions here allow the cFE to interface to functions that are 
**          platform specific.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:59GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_cfe_psp_support.c  $
** Revision 1.4 2012/01/13 12:51:59GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2010/10/25 15:06:35EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 14:57:25EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2009/10/20 13:25:48EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.1 2009/10/13 15:32:26EDT wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.6 2009/06/25 12:43:38EDT rmcgraw 
** DCR8290:4 More API and macro updates
** Revision 1.5 2009/06/24 10:06:29EDT rmcgraw 
** DCR8290:4 changed os_bsp_return_value to cfe_psp_return_value
** Revision 1.4 2009/06/24 09:18:10EDT rmcgraw 
** DCR8290:4 OS_BSP to CFE_PSP_ and OS_Mem to CFE_PSP_Mem changes
** Revision 1.3 2008/07/28 11:07:22EDT wfmoleski 
** Added code to allow the return code setting capability of the UTF to be used on the 2 new BSP functions.
** Also, testing of this capability was added to test11 of the UTF examples.
** Revision 1.2 2008/07/18 09:26:36EDT wfmoleski 
** The OS_BSPGetKernelTextSegmentInfo function was implemented in the UTF.
** Revision 1.1 2008/04/17 08:09:35EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.5 2006/11/28 12:37:38EST wfmoleski 
** Added the timestamp implementation to the UTF version of CFE_ES_WriteToSysLog
** Revision 1.4 2006/11/09 07:46:16EST wfmoleski 
** UTF implementation of the CDS
** Revision 1.3 2006/06/22 14:01:12EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.2 2006/04/10 15:42:53EDT sslegel 
** Added code to set the processor id and the spacecraft id
** Revision 1.1 2006/03/24 14:30:33EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
*/

/*
**  Include Files
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

/*
** Types and prototypes for this module
*/
#include "utf_types.h"
#include "utf_cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/******************************************************************************
**  Function: UTF_set_processor_id()
**
**  Purpose:
**    Sets the processor id returned from CFE_PSP_GetProcessorId.
*/
static uint32 utf_processor_id = CFE_CPU_ID;
void UTF_set_processor_id(uint32 processor_id)
{
    utf_processor_id = processor_id;   
}

/******************************************************************************
**  Function: UTF_set_spacecraft_id()
**
**  Purpose:
**    Sets the spacecraft id returned from CFE_PSP_GetSpacecraftId.
*/
static uint32 utf_spacecraft_id = 0x42;
void UTF_set_spacecraft_id(uint32 spacecraft_id)
{
    utf_spacecraft_id = spacecraft_id;   
}

/******************************************************************************
**  Function:  CFE_PSP_Restart()
**
**  Purpose:
**    Provides a common interface to the processor reset.
**
**  Arguments:
**    reset_type  : Type of reset.
**
**  Return:
**    (none)
*/

void CFE_PSP_Restart(uint32 reset_type)
{
    (void) reset_type;          /* refer to function argument to avoid compiler warning */
    
    return;
}

/******************************************************************************
**  Function:  CFE_PSP_Panic()
**  
**  Purpose:
**    Provides a common interface to abort the cFE startup process and return
**    back to the OS. 
**
**  Arguments:
**    ErrorCode  : Reason for Exiting.
**  
**  Return:
**    (none)
*/

void CFE_PSP_Panic(int32 ErrorCode)
{
   UTF_put_text("CFE_PSP_Panic Called with error code = 0x%08X. Exiting.\n",(unsigned int)ErrorCode);
   UTF_put_text("The cFE could not start.\n");
   UTF_exit();
}

/******************************************************************************
**  Function:  CFE_PSP_FlushCaches()
**
**  Purpose:
**    Provides a common interface to flush the processor caches. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**    (none)
*/
void CFE_PSP_FlushCaches(uint32 type, uint32 address, uint32 size)
{
    (void) type;          /* refer to function argument to avoid compiler warning */
    (void) address;       /* refer to function argument to avoid compiler warning */
    (void) size;          /* refer to function argument to avoid compiler warning */

    return;
}

/*
** Name: CFE_PSP_GetProcessorId
**
** Purpose:
**         return the processor ID.
**
**
** Parameters:
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values: Processor ID
*/
uint32 CFE_PSP_GetProcessorId(void)
{
    return(utf_processor_id);
}

/*
** Name: CFE_PSP_GetSpacecraftId
**
** Purpose:
**         return the spacecraft ID.
**
** Parameters:
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values: Spacecraft ID
*/
uint32 CFE_PSP_GetSpacecraftId(void)
{
   return(utf_spacecraft_id);
}

/*
** Name: CFE_PSP_GetRestartType
**
** Purpose:
**         return the reset type and subtype
**
** Parameters:
**       reset_subtype_ptr : pointer to the reset sub type
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values: reset type
*/
uint32 CFE_PSP_GetRestartType (uint32 *reset_subtype_ptr)
{
    if ( reset_subtype_ptr != NULL )
    {
       *reset_subtype_ptr = CFE_ES_POWER_CYCLE;
    }
    return(CFE_ES_POWERON_RESET);
}

