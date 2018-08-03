/************************************************************************************************
** File:  cfe_psp_timer.c
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
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  K.Audra    | Initial version,
**   2014/08/08  S. Duran   | Modified for ut699 SPARC LEON3 board
**
*************************************************************************************************/
/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "vxLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"

void CFE_PSP_Init1HzTimer(void);

/* External Declarations
**
*/
extern void CFE_TIME_Local1HzISR(void);
extern void CFE_TIME_Tone1HzISR(void);

/******************* Macro Definitions ***********************/
/*
 ** Timer value computation:
 ** The values are based on the 75.0 Mhz clock, divided by 8
 ** by the prescaler.  The divider value is set in the kernel config and the
 ** default is 8.
 ** This gives 9,375,000 ticks per second on the timer.
 ** There is not a 64-bit TBR on the SPARC.  It has a 32-bit timer ticking
 ** at 9.375MHz.
 */

/* Resolution of the 32-bit time stamp returned by CFE_PSP_Get_Timebase in
 * timer ticks per second. The timer resolution for accuracy should not be
 * any slower than 1000000 ticks per second or 1 us per tick
 * */
#define CFE_PSP_TIMER_TICKS_PER_SECOND       (9375000)

/* The number that the least significant 32-bits of the time stamp returned
 * by CFE_PSP_Get_Timebase rolls over.  If the lower 32-bits rolls at 1 second,
 * then the CFE_PSP_TIMER_LOW32_ROLLOVER will be 1000000.  If the lower 32 bits
 * rolls at its maximum value (2^32) then CFE_PSP_TIMER_LOW32_ROLLOVER will
 * be 0. */
#define CFE_PSP_TIMER_LOW32_ROLLOVER         (9375000)

#define CFE_PSP_TIMER_PRINT_DBG              (TRUE)

#define CFE_PSP_TIMER_PRINT_DBG              TRUE
#define CFE_PSP_TIMER_AUX_TICK_PER_SEC       100         /* The number of tics or interrupts per second */

/*
 ** local global variables for PSP Timer
 */
static void CFE_PSP_AuxClkHandler(int32 arg);

static boolean g_bTimerInitialized = FALSE;
static uint32 g_nSecondsCount = 0;

/******************************************************************************
**  Function:  CFE_PSP_InitLocalTime()
**
**  Purpose: Initializes the vxWorks Aux and TimeStamp timers.
**
**  Arguments: none
******************************************************************************/

void CFE_PSP_InitLocalTime(void)
{
	/* Set the sys clock rate */
	sysClkRateSet(200);

   /*
   ** Disable the Aux timer interrupt, and disable the Timestamp timer
   */
   sysAuxClkDisable();
   
   if(sysTimestampDisable() == ERROR)
   {
      OS_printf("CFE_PSP: Unable to disable the Timestamp timer!\n");
   }
   
   /*
   ** Set the Aux timer
   */
   if(sysAuxClkRateGet() != CFE_PSP_TIMER_AUX_TICK_PER_SEC)
   {
      if(sysAuxClkRateSet(CFE_PSP_TIMER_AUX_TICK_PER_SEC) == ERROR)
      {
         OS_printf("CFE_PSP: Unable to set Aux Clock Rate!\n");
      }
         

      if(CFE_PSP_TIMER_PRINT_DBG == TRUE)
      {      
         OS_printf("CFE_PSP: Aux Clock Rate %d.\n", sysAuxClkRateGet());
         
         OS_printf("CFE_PSP: Timestamp Frequency %u.\n", sysTimestampFreq());
         
         OS_printf("CFE_PSP: Timestamp Period %u.\n", sysTimestampPeriod());
      }     
   }


}/* end CFE_PSP_InitLocalTime */

/******************************************************************************
**  Function:  CFE_PSP_Init1HzTimer()
**
**  Purpose: Initializes the 1Hz Timer and connects it to the cFE TIME 1Hz routine
**
**
**  NOTE: This function has to be called after CFE_ES_Main() in CFE_PSP_Start()
**  because the 1Hz ISR has a semaphore that is created in CFE_ES_Main().
**
**  Arguments: none
******************************************************************************/
void CFE_PSP_Init1HzTimer(void)
{
    /*
    ** Attach a handler to the timer interrupt
    */

	/* Either the Aux clock */
    if(sysAuxClkConnect((FUNCPTR)CFE_PSP_AuxClkHandler,
                         CFE_PSP_TIMER_AUX_TICK_PER_SEC) == ERROR)
    {
       printf("CFE_PSP: Unable to connect handler to Aux Clock!\n");
    }

    /*
    ** Enable the Aux timer interrupt
    ** Enable the Timestamp timer, which also sets it to zero
    */
    sysAuxClkEnable();

    if(sysTimestampEnable() == ERROR)
    {
       OS_printf("CFE_PSP: Unable to enable the Timestamp timer!\n");
    }

    g_bTimerInitialized = TRUE;

}/* end CFE_PSP_Init1HzTimer */

/******************************************************************************
**  Function:  CFE_PSP_GetTime()
**
**  Purpose: Gets the value of the time from the hardware
**
**  Note: This routine will be be called within the context of an ISR.  Avoid 
**        the use of floating point numbers here.
**
**        This implementation uses the time stamp counter for the subsecond
**        of the time's second.  The second value comes from a counter
**        incrementing in the 1Hz aux clk ISR
**
**  Arguments: LocalTime - where the time is returned through
******************************************************************************/

void CFE_PSP_GetTime(OS_time_t *LocalTime)
{
    uint32 nTimeStamp = 0;
    uint32 nFrequency = 0;
    uint64 ullTimeStamp = 0;

    if (LocalTime == NULL)
    {
        return;
    }
    else
    {
        if (g_bTimerInitialized == FALSE)
        {
            LocalTime->seconds = 0;
            LocalTime->microsecs = 0;
            return;
        }
        else
        {
            /*
             ** Get the time stamp
             */
            /* Lock interrupts - really should be using this one, over
             * sysTimestamp(), according to
             * documentation on these function calls */
            nTimeStamp = sysTimestampLock();

            /* the default frequency is 9,375,000 Hz, which equals the PSP
             * ticks per second value
             * frequency = ticks per second for this counter
             */
            nFrequency = sysTimestampFreq();

            /* could use seconds = nTimeStamp / nFrequency if the timestamp counter
             * was a 64-bit counter, but being a 32-bit counter, running at
             * 9375000 Hz, it rolls over every 458.129 seconds or 7.63 minutes,
             * so we have to increment seconds using the aux clk ISR and get
             * the subseconds from the timestamp timer */
            LocalTime->seconds = g_nSecondsCount;

            /* convert to micro sec - this conversion assumes that nTimeStamp
             * is always less than a second, which it should be, because it is
             * reset at the top of each second (when the seconds counter is
             * incremented) -- this comp has to be performed as an unsigned
             * long long, as the result of the multiply will not fit in a 32-bit
             * var.  After dividing by the frequency(ticks per second), the result
             * can be stored in a 32-bit value
             */
            ullTimeStamp = ((uint64) nTimeStamp * 1000000ULL)
                    / (uint64) nFrequency;
            LocalTime->microsecs = (uint32) ullTimeStamp;

            /* Keep this invaluable log message, it is used for troubleshooting
             * and when creating a new PSP based on this one
             * It must be commented out during normal usage, but can be
             * uncommented for troubleshooting
             *
             logMsg("time: %u.%u ts: %u  sc: %u\n",
                    LocalTime->seconds,LocalTime->microsecs,
                    nTimeStamp,g_nSecondsCount,0,0);
             */
        }
    }
}/* end CFE_PSP_GetTime */



/******************************************************************************
**  Function:  CFE_PSP_Get_Timer_Tick()
**
**  Purpose:
**    Provides a common interface to system clock tick. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**  OS system clock ticks per second
*/
uint32 CFE_PSP_Get_Timer_Tick(void)
{
   uint32 rate = sysClkRateGet();
   
   return rate;
}

/******************************************************************************
**  Function:  CFE_PSP_GetTimerTicksPerSecond()
**
**  Purpose:
**    Provides the resolution of the least significant 32 bits of the 64 bit
**    time stamp returned by CFE_PSP_Get_Timebase in timer ticks per second.
**    The timer resolution for accuracy should not be any slower than 1000000
**    ticks per second or 1 us per tick
**
**  Arguments:
**
**  Return:
**    The number of timer ticks per second of the time stamp returned
**    by CFE_PSP_Get_Timebase
*/
uint32 CFE_PSP_GetTimerTicksPerSecond(void)
{
    return (CFE_PSP_TIMER_TICKS_PER_SECOND);
}

/******************************************************************************
**  Function:  CFE_PSP_GetTimerLow32Rollover()
**
**  Purpose:
**    Provides the number that the least significant 32 bits of the 64 bit
**    time stamp returned by CFE_PSP_Get_Timebase rolls over.  If the lower 32
**    bits rolls at 1 second, then the CFE_PSP_TIMER_LOW32_ROLLOVER will be 1000000.
**    if the lower 32 bits rolls at its maximum value (2^32) then
**    CFE_PSP_TIMER_LOW32_ROLLOVER will be 0.
**
**  Arguments:
**
**  Return:
**    The number that the least significant 32 bits of the 64 bit time stamp
**    returned by CFE_PSP_Get_Timebase rolls over.
*/
uint32 CFE_PSP_GetTimerLow32Rollover(void)
{
    return(CFE_PSP_TIMER_LOW32_ROLLOVER);
}

/******************************************************************************
**  Function:  CFE_PSP_Get_Timebase()
**
**  Purpose:
**    Provides a common interface to system timebase. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**  Timebase register value
*/
void CFE_PSP_Get_Timebase(uint32 *Tbu, uint32* Tbl)
{
    uint32 upper = 0;
    uint32 lower = 0;

    /*
     ** We don't want an interrupt to occur when the time requested.
     ** According to notes from vxWorks, intLock does not stop reschededuling
     ** of the calling task, so we have lock the task as well.
     **
     ** sysTimestampLock() automatically blocks interrupts during execution.
     **
     ** taskLock() is not valid on SMP systems and taskCpuLock (VxWorks 6.6+)
     **   should be used instead.
     */

    if (g_bTimerInitialized == TRUE)
    {
#if _WRS_VXWORKS_MINOR >= 6
        if (taskCpuLock() == OK)
#else
        if (taskLock() == OK)
#endif
        {

            /*
             ** Tbu is the upper 32 bits of a 64 bit counter.
             ** Tbl is the lower 32 bits of a 64 bit counter.
             ** The resolution of our timer is given by:
             **   one timer tick = 1 / sysTimestampFreq() in seconds
             */

            upper = g_nSecondsCount;

            lower = sysTimestampLock();


#if _WRS_VXWORKS_MINOR >= 6
            taskCpuUnlock();
#else
            taskUnlock();
#endif

        }
        else
        {
            /*
             ** What if you can't lock the task
             */

            printf("CFE PSP Get Timebase: Unable to lock task!\n");
        }
    }
    else
    {
        printf("CFE PSP Get Timebase: Timer is not initialized!\n");
    }

    if (Tbu != NULL)
    {
        *Tbu = upper;
    }

    if (Tbl != NULL)
    {
        *Tbl = lower;
    }
}

/******************************************************************************
**  Function:  CFE_PSP_Get_Dec()
**
**  Purpose:
**    Provides a common interface to decrementer counter. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**  Timebase register value
*/

uint32 CFE_PSP_Get_Dec(void)
{
   /*
   ** This isn't called
   */

   return 0;
}

/******************************************************************************
 **  Function:  CFE_PSP_AuxClkHandler()
 **
 **  Purpose:
 **    A timer int handler to keep track of seconds.
 **
 **  Arguments:
 **
 **  Return:
 */
static void CFE_PSP_AuxClkHandler(int32 arg)
{
    static int32 auxCount = 0;

   if(++auxCount >= CFE_PSP_TIMER_AUX_TICK_PER_SEC)
   {
      auxCount = 0;
      
      ++g_nSecondsCount;


      /* calling enable every second, resets the counter to 0, allowing us
       * to use the higher resolution timestamp counter as a subsecond
       * time source */
      sysTimestampEnable();

      CFE_TIME_Local1HzISR();

        /* Keep this invaluable log message, it is used for troubleshooting
         * and when creating a new PSP based on this one
         * It must be commented out during normal usage, but can be
         * uncommented for troubleshooting
         *
         OS_time_t LocalTime;
         CFE_PSP_GetTime(&LocalTime);
         logMsg("aux clk handler: %u %u  %u\n", LocalTime.seconds,
                 LocalTime.microsecs,sysTimestampLock(),0,0,0);
         */
    }
}


