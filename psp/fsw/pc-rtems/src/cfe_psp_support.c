/******************************************************************************
** File:  cfe_psp_support.c
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  Alan Cudmore    | Initial version,
**
******************************************************************************/


/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"

#ifdef _ENHANCED_BUILD_

/*
 * The preferred way to obtain the CFE tunable values at runtime is via
 * the dynamically generated configuration object.  This allows a single build
 * of the PSP to be completely CFE-independent.
 */
#include <target_config.h>

#define CFE_CPU_ID                  (GLOBAL_CONFIGDATA.Default_CpuId)
#define CFE_CPU_NAME                (GLOBAL_CONFIGDATA.Default_CpuName)
#define CFE_SPACECRAFT_ID           (GLOBAL_CONFIGDATA.Default_SpacecraftId)

#else

/*
 * cfe_platform_cfg.h needed for CFE_ES_NONVOL_STARTUP_FILE, CFE_CPU_ID/CPU_NAME/SPACECRAFT_ID
 *
 *  - this should NOT be included here -
 *
 * it is only for compatibility with the old makefiles.  Including this makes the PSP build
 * ONLY compatible with a CFE build using this exact same CFE platform config.
 */

#include "cfe_platform_cfg.h"

#endif

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

   if ( reset_type == CFE_PSP_RST_TYPE_POWERON )
   {
      CFE_PSP_ReservedMemoryMap.FixedInfo->bsp_reset_type = CFE_PSP_RST_TYPE_POWERON;
      CFE_PSP_FlushCaches(1, (cpuaddr)CFE_PSP_ReservedMemoryMap.FixedInfo, CFE_PSP_ReservedMemoryMap.TotalSize);
      /* reboot(BOOT_CLEAR); Need RTEMS equiv. */
   }
   else
   {
      CFE_PSP_ReservedMemoryMap.FixedInfo->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;
      CFE_PSP_FlushCaches(1, (cpuaddr)CFE_PSP_ReservedMemoryMap.FixedInfo, CFE_PSP_ReservedMemoryMap.TotalSize);
      /* reboot(BOOT_NORMAL); Need RTEMS Equiv */
   }

   OS_printf("CFE_PSP_Restart is not implemented on this platform ( yet ! )\n");
   exit(-1);

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
   printf("CFE_PSP_Panic Called with error code = 0x%08X. Exiting.\n",(unsigned int )ErrorCode);
   OS_ApplicationExit(ErrorCode);
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

void CFE_PSP_FlushCaches(uint32 type, cpuaddr address, uint32 size)
{
   if ( type == 1 )
   {
      /* cacheTextUpdate((void *)address, size); */
   }

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
uint32 CFE_PSP_GetProcessorId    (void)
{
    return(CFE_CPU_ID);
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
uint32 CFE_PSP_GetSpacecraftId   (void)
{
   return(CFE_SPACECRAFT_ID);
}

