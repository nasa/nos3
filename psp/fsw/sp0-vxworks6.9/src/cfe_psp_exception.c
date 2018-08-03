/******************************************************************************
**
** File:  cfe_psp_exception.c
**
**      PPC85XX vxWorks 6.9 Version
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**      In original version of this code, the following load error was noted:
**      ld<cfe-core.o
**      Warning: module 0x22e1010 holds reference to undefined symbol fppRestore.
**      Warning: module 0x22e1010 holds reference to undefined symbol fppSave.
**
**      The following was found in the VxWorks 6.9 architecture supplement,
**      pg 179, for PPC85xx:
**      Do not confuse the hardware floating-point provided by the FPU with that
**      provided by the SPE (see 6.3.10 Signal Processing Engine Support,
**      p.190). If using the e500v2diab or e500v2gnu toolchains, you must use
**      the speSave() and speRestore() routines to save and restore
**      floating-point context.
**
**      The e500 core's SPE is a hardware double precision unit capable
**      of both scalar and vector(SIMD) computation
**
**
** Purpose:
**   cFE PSP Exception related functions.
**
** History:
**   2007/05/29  A. Cudmore      | vxWorks 6.2 MCP750 version
**   2015/05/xx  S. Duran        | VxWorks 6.9 PPC85XX
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
#include "arch/ppc/vxPpcLib.h"
#include "arch/ppc/esfPpc.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe.h"
#include "cfe_psp_memory.h"
#include "cfe_platform_cfg.h"  /* for processor ID */


extern FUNCPTR  _func_excTrapRtn;   /* pointer exported by VxWorks */

void user_defined_trap_rtn( ESFPPC *esf, REG_SET regs, int a, int b)
{
	  FP_CONTEXT fpContext;
	  long           my_task_id;
	  CFE_EVS_LogFileCmd_t Cmd;

	  my_task_id  = taskIdSelf();

	  CFE_EVS_SendEvent(24, CFE_EVS_ERROR, "Exception: task %s: vector: 0x%.08X",taskName(my_task_id),(int)esf->vecOffset);

	  speSave(&fpContext);
	  fpContext.fpcsr &= 0x000000ff;
	  speRestore(&fpContext);

	  strcpy((char *) Cmd.Payload.LogFilename,"\/cf\/log\/nvlog");
	  CFE_EVS_WriteLogFileCmd(&Cmd);

	  taskSuspend(my_task_id);
}

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

CFE_PSP_ExceptionContext_t CFE_PSP_ExceptionContext;
char                  CFE_PSP_ExceptionReasonString[256];

/*
**
** IMPORTED FUNCTIONS
**
*/

void CFE_ES_ProcessCoreException(uint32  HostTaskId, const char *ReasonString,
                                 const uint32 *ContextPointer, uint32 ContextSize);

/*
**
** LOCAL FUNCTION PROTOTYPES
**
*/

void CFE_PSP_ExceptionHook ( int task_id, int vector, ESFPPC* pEsf );


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
	_func_excTrapRtn = (FUNCPTR)user_defined_trap_rtn;
    excHookAdd((FUNCPTR)CFE_PSP_ExceptionHook);
    OS_printf("CFE_PSP: Attached cFE Exception Handler. Context Size = %d bytes.\n",CFE_PSP_CPU_CONTEXT_SIZE);
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
**                      will not be valid.  If the task is enabled for floating
**                      point, then it will be valid.
**
*/
void CFE_PSP_ExceptionHook (int task_id, int vector, ESFPPC* pEsf )
{

    char *TaskName;

    /*
    ** Get the vxWorks task name
    */
    TaskName = taskName(task_id);

    if ( TaskName == NULL )
    {
       sprintf(CFE_PSP_ExceptionReasonString, "Exception: Vector=0x%06X, vxWorks Task Name=NULL, Task ID=0x%08X", vector,task_id);
    }
    else
    {
       sprintf(CFE_PSP_ExceptionReasonString, "Exception: Vector=0x%06X, vxWorks Task Name=%s, Task ID=0x%08X", vector, TaskName, task_id);
    }

    /*
    ** Save Exception Stack frame
    */
    memcpy(&(CFE_PSP_ExceptionContext.esf), pEsf, sizeof(ESFPPC));

    /*
    ** Save floating point registers
    */
    speSave(&CFE_PSP_ExceptionContext.fp);

    /*
    ** Call the Generic cFE routine to finish processing the exception and
    ** restart the cFE
    */
    CFE_ES_ProcessCoreException((uint32 )task_id,
            (uint8*)CFE_PSP_ExceptionReasonString,
            (uint32 *)&CFE_PSP_ExceptionContext,
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
**   Notes: The exception environment is local to each task Therefore this must
**          be called for each task that that wants to do floating point and
**          catch exceptions.  Currently, this is automatically called from
**          OS_TaskRegister for every task
*/
void CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /* TODO: determine the prpoper exception environment for the SP0 processor
     * */
}


