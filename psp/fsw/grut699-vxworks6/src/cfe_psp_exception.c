/******************************************************************************
**
** File:  cfe_psp_exception.c
**
 **      ut699 SPARC LEON3 VxWorks 6.7 Version
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
** Purpose:
**   cFE PSP Exception related functions.  
**
** History:
**   2007/05/29  A. Cudmore      | vxWorks 6.2 MCP750 version
 **   2014/08/08  S. Duran        | Modified for ut699 SPARC LEON3 board
**
******************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include "fppLib.h"
#include "excLib.h"
#include "taskLib.h"

#include "arch/sparc/esfSparc.h"
#include "arch/sparc/fppSparcLib.h"
#include "arch/sparc/archSparc.h"

/*
** cFE includes 
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

#include "cfe_psp.h" 
#include "cfe_psp_memory.h"           


/*
** Types and prototypes for this module
*/


/*
** BSP Specific defines
*/

/*
**  External Declarations
*/

                                                                                
/*
** Global variables
*/

static CFE_PSP_ExceptionContext_t CFE_PSP_ExceptionContext;
static char CFE_PSP_ExceptionReasonString[256];

/*
 **
 ** LOCAL FUNCTION PROTOTYPES
 **
 */
static void CFE_PSP_ExceptionHook(int task_id, int vector, uint8* pEsf);

/***************************************************************************
 **                        FUNCTIONS DEFINITIONS
 ***************************************************************************/

/*
**
**   Name: CFE_PSP_AttachExceptions
**
**   Purpose: This function Initializes the task execptions and adds a hook
**              into the VxWorks exception handling.  The below hook is called
**              for every exception that VxWorks catches.
**
**   Notes: if desired - to attach a custom handler put following code in
**          this function:  excConnect ((VOIDFUNCPTR*)VECTOR, ExceptionHandler);
**
*/

void CFE_PSP_AttachExceptions(void)
{
   /*
   ** Always attach exception hook 
   */
   excHookAdd((FUNCPTR)CFE_PSP_ExceptionHook);

   OS_printf("BSP: Attached cFE Exception Handler. Context Size = %d bytes.\n",
    CFE_PSP_CPU_CONTEXT_SIZE);
}

/*
** Name: CFE_PSP_ExceptionHook
**
** Purpose: Make the proper call to CFE_ES_ProcessCoreException 
**
** Notes:   pEsf - pointer to exception stack frame.
**          fppSave - When it makes this call, it captures the last floating
**                      point context - which may not be valid.  If a floating
**                      point exception occurs you can be almost 100% sure
**                      that this will reflect the proper context.  But if another
**                      type of exception occurred then this has the possibility
**                      of not being valid.  Specifically if a task that is not
**                      enabled for floating point causes a non-floating point
**                      exception, then the meaning of the floating point context
**                      will not be valid.  If the task is enabled for floating point,
**                      then it will be valid.
**
*/
static void CFE_PSP_ExceptionHook (int task_id, int vector, uint8* pEsf )
{
    char *TaskName;
    
    /*
    ** Get the vxWorks task name
    */
    TaskName = taskName(task_id);
    
    if ( TaskName == NULL )
    {
       sprintf(CFE_PSP_ExceptionReasonString, "Exception: Vector=0x%06X, vxWorks Task Name=NULL, Task ID=0x%08X", 
               vector,task_id);
    }
    else
    {
       sprintf(CFE_PSP_ExceptionReasonString, "Exception: Vector=0x%06X, vxWorks Task Name=%s, Task ID=0x%08X", 
                vector, TaskName, task_id);
    }

    /*
    ** Save floating point registers
    */
    fppSave(&CFE_PSP_ExceptionContext.fp);

    /*
     ** Call the Generic cFE routine to finish processing the exception and
     ** restart the cFE
     */
    CFE_ES_ProcessCoreException((uint32) task_id,
            (char *) CFE_PSP_ExceptionReasonString,
            (uint32 *) &CFE_PSP_ExceptionContext,
            sizeof(CFE_PSP_ExceptionContext_t));

    /*
    ** No return to here 
    */
    
} /* end function */


/*
 **
 **   Name: CFE_PSP_SetDefaultExceptionEnvironment
 **
 **   Purpose: This function sets a default exception environment that can be used
 **
 **   Notes: The exception environment is local to each task Therefore this must be
 **          called for each task that that wants to do floating point and catch exceptions
 **          Currently, this is automatically called from OS_TaskRegister for every task
 */
void CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /*
     ** Nothing to do here - BSP sets it up, From
     ** http://www.gaisler.com/doc/vxworks-bsps-6.7.pdf, section 6.2 Exception
     ** handling, Rev 1.0.11, March 2015
     ** The trap table is defined by the BSP in sysALib.s. The default action of
     ** unknown or unhandled traps is to enter excEnt().  The traps 0x02-0x04,
     ** 0x07-0x10 and 0x20-0x7f will cause a fatal exception - includes floating
     ** point and alignment exceptions.  It is possible to modify the the fatal
     ** exception handlers in target/config/comps/src/ edrStubs.c to change
     **  the default behavior.
     */
}

