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
#include "vxWorks.h"
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "rebootLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_mission_cfg.h"   /* for spacecraft ID */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"

#ifdef _WRS_VX_SMP
#include "vxCpuLib.h"
#endif


/*
** defines for this module
*/

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

   OS_printf("\nWarning: CFE PSP Panic with error code %d!  A restart will occur!\n\n", ErrorCode);
   
   /*
   ** Delay for second or two, allow the print statement to send
   */
   
   taskDelay(100);
    
   /*
   ** Debug Switch is not set, do a processor Reset
   */
   CFE_PSP_Restart(CFE_PSP_RST_TYPE_PROCESSOR);
}

/******************************************************************************
**  Function:  CFE_PSP_Restart()
**
**  Purpose:
**    Provides a common interface to reset the processor.  This implementation
**      may need to be customized for missions with custom reset requirements.
**
**  Arguments:
**    reset_type  : Type of reset.
**
**  Return:
**    (none)
*/
void CFE_PSP_Restart(uint32 reset_type)
{
   OS_printf("\nWarning: CFE PSP Restart with reset type %u!\n\n", reset_type);

   /*
   ** Delay for second or two, allow the print statement to send
   */
   
   taskDelay(100);

#ifndef _WRS_VX_SMP /* Changed for SMP API compatability. */
   taskLock();
   intLock();
#else
   /* Note: This only locks the current CPU core.  Other cores
    *  are still active and may continue to access system resources
    *  or service the watchdog on an SMP system.
    */
   taskCpuLock();
   intCpuLock();
#endif
   
   if ( reset_type == CFE_PSP_RST_TYPE_POWERON )
   {
      CFE_PSP_ReservedMemoryPtr->bsp_reset_type = CFE_PSP_RST_TYPE_POWERON;
      /*
      ** The sysToMonitor function flushes caches for us
      *
      * reboot(BOOT_CLEAR);
      */
      
      /*
      ** Use the watchdog timer to assert a reset
      */
      CFE_PSP_WatchdogEnable();

      for(;;)
      {
         /*
         ** Set the current count value to something small
         */
         CFE_PSP_WatchdogSet(CFE_PSP_WATCHDOG_MIN);
         
         /*
         ** Wait for the watchdog to expire...
         */
         taskDelay(100);
      }
      
   }
   else
   {
      CFE_PSP_ReservedMemoryPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;
      /*
      ** The sysToMonitor function flushes caches for us
      *
      * reboot(0);
      */

      /*
      ** Use the watchdog timer to assert a reset
      */
      CFE_PSP_WatchdogEnable();
      
      for(;;)
      {
         /*
         ** Set the current count value to something small
         */
         CFE_PSP_WatchdogSet(CFE_PSP_WATCHDOG_MIN);
         
         /*
         ** Wait for the watchdog to expire...
         */
         taskDelay(100);
      }
   }
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
/* implement cache flush function if needed.  It is commented out, but
 * kept as an example in case another PSP is made from this one
 * It is not currently used or implemented in this PSP or CFS build
 * on this target
    if (type == 1)
    {
        cacheTextUpdate((void *)address, size);
    }
*/
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
    return(CFE_CPU_ID);
}



/*
** Name: CFE_PSP_GetProcessId
**
** Purpose:
**         return the process ID. Only really useful for usermode
** Otherwise always returns 0
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
** Return Values: Process ID
*/
uint32 CFE_PSP_GetProcessId(void)
{
    return(0);
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
   return(CFE_SPACECRAFT_ID);
}


