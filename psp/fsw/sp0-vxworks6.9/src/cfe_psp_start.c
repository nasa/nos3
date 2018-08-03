/******************************************************************************
** File:  cfe_psp_start.c
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
** Purpose:
**   cFE PSP main entry point.
**
** History:
**   2004/09/23  J.P. Swinski    | Initial version,
**   2004/10/01  P.Kutt          | Replaced OS API task delay with VxWorks functions
**                                 since OS API is initialized later.
**   2015/05/XX  S.Duran	 	 | Updated for Aitech SP0
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
#include "xbdBlkDev.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe.h"
#include "cfe_platform_cfg.h"  /* for processor ID */

/*
 ** Types and prototypes for this module
 */
#include "cfe_psp.h"
#include "cfe_psp_memory.h"

#define CFE_PSP_TASK_PRIORITY    (30)
#define CFE_PSP_TASK_STACK_SIZE  (20 * 1024)


/*
**  External Declarations
*/
extern void CFE_PSP_InitLocalTime(void);
extern void CFE_PSP_Init1HzTimer(void);
extern void CFE_TIME_SetState(int16);

extern CFE_PSP_ReservedMemory_t *CFE_PSP_ReservedMemoryPtr;

void CFE_PSP_Main(uint32 ModeId, char *StartupFilePath);
uint32 CFE_PSP_GetRestartType(uint32 *restartSubType);

/*
 **  Local Declarations
 */
static uint32 ResetType;
static uint32 ResetSubtype;

static void CFE_PSP_Start(uint32 ModeId, char *StartupFilePath);
static void CFE_PSP_SysInit(uint32* psp_reset_type, uint32* psp_reset_subtype,
        uint32 last_bsp_reset_type);
static void SetSysTasksPrio(void);
static void ResetSysTasksPrio(void);
static void SetTaskPrio(char* tName, const int32 tgtPrio);

/******************************************************************************
**  Function:  CFE_PSP_Main()
**
**  Purpose:
**    vxWorks/PSP Application entry point.
**
**  Arguments:
**    ModeId          - not currently used
**    StartupFilePath - path to cFE startup file to use
**
**  Return:
**    (none)
*/
void CFE_PSP_Main(uint32 ModeId, char *StartupFilePath)
{
    int32 root_task_id;


  /* had to add VX_FP_TASK (all tasks should just be created with it...
   * to deal with an "SPE unknown exception" error as soon as this task
   * would start on the SP0/PPC8548
   * NOTE: when using e500vx_gnu tool chain, including VX_FP_TASK implicitly
   * includes VX_SPE_TASK, which is needed when starting tasks that might
   * use floating point on this processor.
  */
  root_task_id = taskSpawn("PSP_START", CFE_PSP_TASK_PRIORITY,
                            VX_FP_TASK, CFE_PSP_TASK_STACK_SIZE,
                            (FUNCPTR) (void *)CFE_PSP_Start, ModeId,
                            (int)StartupFilePath,0,0,0,0,0,0,0,0);

  if ( root_task_id == ERROR )
  {
     printf("CFE_PSP_Main: ERROR - unable to spawn PSP_START task");
  }

}

/******************************************************************************
**  Function:  CFE_PSP_SysInit()
**
**  Purpose:
**    Initializes core PSP functionality.  This is called by both the primary
**      CFE_PSP_Main startup, as well as the alternative initialization in the
**      Startup Manager (SM) component.  The SM uses the PSP and OSAL but does
**      not start cFE or initialize all components of the PSP.
**
**  Arguments:
**    psp_reset_type - output
**    psp_reset_subtype - output
**    last_bsp_reset_type  - input - TBD - do we need to keep this?
**
**  Return:
**    (none)
**
*/
static void CFE_PSP_SysInit(uint32* psp_reset_type, uint32* psp_reset_subtype,
        uint32 last_bsp_reset_type)
{
   int32 TicksPerSecond = 0;

   /*
   ** Delay for one second.
   */
   TicksPerSecond = sysClkRateGet();
   taskDelay(TicksPerSecond);


   /* Assign the reset cause */
   /* for now always start up in a power on reset */
   *psp_reset_type    = CFE_PSP_RST_TYPE_POWERON;
   *psp_reset_subtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;

   ResetType = *psp_reset_type;
   ResetSubtype = *psp_reset_subtype;
}

/******************************************************************************
**  Function:  CFE_PSP_Start()
**
**  Purpose:
**    Initialize the PSP and start cFE
**
**  Arguments:
**    ModeId          - Used to indicate which bank of non-volatile memory
**                      was used to boot.  If the bank used to boot this time
**                      is different from the previous boot, then we re-initialize
**                      reserved memory.
**    StartupFilePath - path to cFE startup file to use
**
**  Return:
**    (none)
*/
static void CFE_PSP_Start(uint32 ModeId, char *StartupFilePath)
{
    uint32 reset_type = 0;
    uint32 reset_subtype = 0;
    uint32 status = 0;

    /*
     ** Initialize the hardware timer for the local time source
     ** On VxWorks, the default config sets the sysClk to 200 Hz (default 60Hz)
     ** OS_API_Init() calls OS_TimerAPIInit() which gets the
     ** clock resolution of the realtime clock, which is based on the sysClk
     ** and determines the clock accuracy which is used by the scheduler
     ** timers later.  sysClk needs to be at least 200Hz for a 100Hz minor
     ** frame rate.
     */
    CFE_PSP_InitLocalTime();

    /*
     ** Initialize the OS API data structures
     */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        printf("CFE_PSP_Start() - OS_API_Init() fail, RC = 0x%x\n", status);
    }

    /*
     ** Setup the pointer to the reserved area in vxWorks.
     ** This must be done before any of the reset variables are used.
     */
    CFE_PSP_ReservedMemoryPtr = (CFE_PSP_ReservedMemory_t *) sysMemTop();

    printf("CFE_PSP_Main: Reserved Memory Address %08X reserved mem size: %d\n",
            (uint32) CFE_PSP_ReservedMemoryPtr, sizeof(CFE_PSP_ReservedMemory_t));

    /* PSP System Initialization */
    CFE_PSP_SysInit(&reset_type, &reset_subtype,
            CFE_PSP_ReservedMemoryPtr->bsp_reset_type);
    /*
     ** Initialize the watchdog, it's left disabled
     */
    CFE_PSP_WatchdogInit();

        /*
     ** Initialize the reserved memory
     */
    CFE_PSP_InitProcessorReservedMemory(reset_type);

     /*
     * Adjust system task priorities so that tasks such as the shell are
     * at a lower priority that the CFS apps
     */
    SetSysTasksPrio();

     /*
     ** Call cFE entry point. This will return when cFE startup
     ** is complete.
     */
    CFE_ES_Main(reset_type, reset_subtype, ModeId, (char *) StartupFilePath);

     /*
     * Initializing the 1Hz timer connects the cFE 1Hz ISR for providing the
     * CFS 1Hz time sync, sync the scheduler's 1Hz major frame start to the
     * 1Hz timer.  This call can only occur after CFE_ES_Main() because the
     * 1Hz ISR uses a semaphore that is created when timer services are
     * initialized.
     */
    CFE_PSP_Init1HzTimer();

    printf("CFE_PSP_Start done, exiting.\n");

    return;
} /* End CFE_PSP_Main */

/******************************************************************************
 **  Function:  CFE_PSP_GetRestartType()
 **
 **  Purpose:
 **    Retrieve the CFE PSP reset type and subtype.
 **
 **  Arguments:
 **    restartSubType [out] Reset Sub Type
 **
 **  Return:
 **    Reset Type
 */
uint32 CFE_PSP_GetRestartType(uint32 *restartSubType)
{
    if (restartSubType != NULL)
    {
        *restartSubType = ResetSubtype;
    }
    return (ResetType);
}

/******************************************************************************
 **  Function:  SetTaskPrio()
 **
 **  Purpose:
 **    change default task priority to a given priority
 **
 **  Arguments:
 **    tName: task name
 **    tgtPrio: new priority to set task to
 **
 **  Return: none
 */
static void SetTaskPrio(char* tName, const int32 tgtPrio)
{
    int32 tid = 0;
    int32 curPrio = 0;
    int32 newPrio = 0;

    if ((tName != NULL) && (strlen((const char*) tName) > 0))
    {
        newPrio = tgtPrio;
        if (newPrio < 0)
        {
            newPrio = 0;
        }
        else if (newPrio > 255)
        {
            newPrio = 255;
        }

        tid = taskNameToId(tName);
        if (tid != ERROR)
        {
            if (taskPriorityGet(tid, (int *) &curPrio) != ERROR)
            {
                printf("Setting %s priority from %d to %d\n", tName, curPrio,
                        newPrio);
                taskPrioritySet(tid, newPrio);
            }
        }
    }
}

/******************************************************************************
 **  Function:  ResetSysTasksPrio()
 **
 **  Purpose:
 **    reset changed task priorities back to defaults
 **
 **  Arguments: none
 **
 **  Return: none
 **
 */
static void ResetSysTasksPrio(void)
{
    printf("\nResetting system tasks' priority to default\n");
    SetTaskPrio("tLogTask", 0);
    SetTaskPrio("tShell0", 1);
    SetTaskPrio("tWdbTask", 3);
    SetTaskPrio("tVxdbgTask", 25);
    SetTaskPrio("tNet0", 50);
    SetTaskPrio("ipftps", 50);
    SetTaskPrio("ipcom_syslogd", 50);
    SetTaskPrio("ipcom_telnetd", 50);
}

/******************************************************************************
 **  Function:  SetSysTasksPrio()
 **
 **  Purpose:
 **    change system task prioritiesso that they are lower than CFS system
 **    task priorities
 **
 **    NOTE: tNet0 priority should be adjusted to be right below what ever
 **    gets defined for ci/to in your system if using the network interface
 **    CCSDS/UDP for ci/to.
 **
 **  Arguments: none
 **
 **  Return: none
 */
static void SetSysTasksPrio(void)
{
    printf("\nSetting system tasks' priorities\n");
    SetTaskPrio("tLogTask", 0);
    SetTaskPrio("tShell0", 201);
    SetTaskPrio("tWdbTask", 203);
    SetTaskPrio("tVxdbgTask", 200);
    SetTaskPrio("tNet0", 130); /****** should be set right below CI/TO */
    SetTaskPrio("ipftps", 202);
    SetTaskPrio("ipcom_syslogd", 205);
    SetTaskPrio("ipcom_telnetd", 204);
}

/* undefined symbols ----------------------------------------------------------
 * added prototypes here so that the code compiles and runs w/o error
 * if there is still code that calls these functions, we will get a message
 *
 * ld<cfe-core.o
Warning: module 0x461d010 holds reference to undefined symbol vxFpscrGet.
Warning: module 0x461d010 holds reference to undefined symbol vxFpscrSet.

These do not seem to be included in 85xx build, but are

"defined(_PPC_) && CPU != PPC440" in vxworks osapi.c, line 2707

STUB them for now to build.  These are FPU exception handler functions that
are currently not used
 * *******/

unsigned int vxFpscrGet(void)
{
    printf("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);
    return(0);
}

void vxFpscrSet(unsigned int x)
{
    printf("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);
}

