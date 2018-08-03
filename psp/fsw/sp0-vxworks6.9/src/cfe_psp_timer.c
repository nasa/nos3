/*******************************************************************************
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
**   This file contains glue routines between the cFE and the OS Board
**   Support Package ( BSP ).  The functions here allow the cFE to interface
**   functions that are board and OS specific and usually dont fit well in
**   the OS abstraction layer.
**
** History:
**   2005/06/05  K.Audra    | Initial version,
**   2015/04/30  S.Duran    | Modified for Aitech SP0 PPC8548 processor board
**
*******************************************************************************/

/*
**  Include Files
*/

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

#include "time.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_psp.h"
#include "cfe_time_msg.h"
/*
** Types and prototypes for this module
*/

void CFE_PSP_Init1HzTimer(void);
void CFE_PSP_PPSHandler(int num);
void CFE_PSP_AuxClkHandler(int arg);

/* External Declarations
**
*/
extern void CFE_TIME_Local1HzISR(void);
extern void CFE_TIME_Tone1HzISR(void);

/******************* Macro Definitions ***********************/

/* This corresponds to a 41.625 MHz time base register(TB or TBR) clock
 *  - which runs at 1/8 of the CCB (e500 Core
 *  Complex Bus clock or platform clock (the system clock drives the CCB clock))
 *  local bus which runs at 333 MHz.  The TB register is a 64-bit register that
 *  is incremented every 8 bus clock cycles.  It is read as tbu and tbl, using
 *  vxTimeBaseGet(), where the upper 32-bits are placed in tbu and the lower
 *  32-bits are placed in tbl.
 *  The timer will take 14043 years to roll over.
 *  (2^64 ticks * 1/41625000 sec/ticks) / 3.15569 x 10^7 sec/year = 14043 years
 *
 *  Seconds are stored in a 32-bit variable using the following computation,
 *  where tb is the 64-bit total number of ticks and seconds is a 32-bit variable
 *      seconds = tb / CFE_PSP_TIMER_TICKS_PER_SECOND;
 *  The max number of seconds before a roll over is 2^32-1 = 4294967295
 *      4294967295 / 3.15569e7 sec/year = 136 years before the seconds
 *      value rolls over.  NOTE: There is currently no roll over detection
 *      on the second value.
 */
/* Resolution of the least significant 32 bits of the 64 bit
 * time stamp returned by CFE_PSP_Get_Timebase in timer ticks per second.
 * The timer resolution for accuracy should not be any slower than 1000000
 * ticks per second or 1 us per tick */
#define CFE_PSP_TIMER_TICKS_PER_SECOND       41625000

/* So a TB tick or period is 1 / 41625000 sec = 0.000000024 sec
 * or 24 ns per tick */
#define CFE_PSP_TIMER_PERIOD 0.000000024

/* The number that the least significant 32 bits of the 64 bit
time stamp returned by CFE_PSP_Get_Timebase rolls over.  If the lower 32
bits rolls at 1 second, then the CFE_PSP_TIMER_LOW32_ROLLOVER will be 1000000.
if the lower 32 bits rolls at its maximum value (2^32) then
CFE_PSP_TIMER_LOW32_ROLLOVER will be 0. */
#define CFE_PSP_TIMER_LOW32_ROLLOVER         0

/* The number of ticks or interrupts per second */
#define CFE_PSP_TIMER_AUX_TICK_PER_SEC       100

#define CFE_PSP_TIMER_PRINT_DBG              TRUE

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
	   ** Disable the Aux timer interrupt
	   */
	   sysAuxClkDisable();

	   /*
	   ** Set the Aux timer rate
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
	      }
	   }
}

/******************************************************************************
**  Function:  CFE_PSP_Init1HzTimer()
**
**  Purpose: Initializes the 1Hz Timer and connects it to the cFE TIME 1Hz routine
**  		 The S950 was setup to use the PPS signal as it was on Morpheus,
**  		 where the PPS signal was driven by the GPS PPS.  If this signal
**  		 is not driven, then the 1Hz ISR will not run.  So you can use the
**  		 AuxClk instead.  Comment out/uncomment either the Aux clk
**  		 connect or the PPS connect to choose the one you want to use.
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

    sysAuxClkEnable();

    /* Or the PPS interrupt available on the S950 board *
    ppsIntConnect((VOIDFUNCPTR)CFE_PSP_PPSHandler);
    ppsIntEnable();
    */

}/* end CFE_PSP_Init1HzTimer */

/******************************************************************************
**  Function:  CFE_PSP_PPSHandler()
**
**  Purpose:
**    PPS 1Hz ISR handler used on Morpheus with the 1PPS interrupt
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_PPSHandler(int num)
{
	/* Morpheus called this function, not sure why.  All other PSPs
	 * call CFE_TIME_Local1HzISR() */
	CFE_TIME_Tone1HzISR();

	/* CFE_TIME_Local1HzISR(); */
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
void CFE_PSP_AuxClkHandler(int arg)
{
   static int auxCount = 0;

   if(++auxCount >= CFE_PSP_TIMER_AUX_TICK_PER_SEC)
   {
      auxCount = 0;
      CFE_TIME_Local1HzISR();

      /* FOR DEBUG
      OS_time_t LocalTime;
      CFE_PSP_GetTime(&LocalTime);
      logMsg("aux clk handler: %d.%d\n", LocalTime.seconds,LocalTime.microsecs,0,0,0,0);
      */
   }

   return;
}
/******************************************************************************
**  Function:  CFE_PSP_GetTime()
**
**  Purpose: Gets the value of the time from the hardware using the PPC
**  		 time base register
**
**
**  Arguments: LocalTime - where the time is returned through
******************************************************************************/
void CFE_PSP_GetTime( OS_time_t *LocalTime)
{
	UINT32 tbu;
	UINT32 tbl;
	unsigned long long tb;

	vxTimeBaseGet(&tbu, &tbl);

	/* reassemble 64-bit count */
	tb = ((unsigned long long)tbu << 32) | (unsigned long long) tbl;

	/* convert to seconds and microseconds using only integer computations */
	LocalTime->seconds = tb / CFE_PSP_TIMER_TICKS_PER_SECOND;
	LocalTime->microsecs = ((tb % CFE_PSP_TIMER_TICKS_PER_SECOND) * 1000000 / CFE_PSP_TIMER_TICKS_PER_SECOND);

	/* could compute seconds.fraction_of_sec to better than 1 us
	 * resolution as follows, but CFS time is based on sec, usec.  Also, if the
	 * time functions are called by an ISR, floating point is not allowed */
	/*
	double sec = tb * CFE_PSP_TIMER_PERIOD;
	OS_printf("sec: %f\n", sec);
	*/

	/*
	OS_printf("tb: 0x%016llX  tb: %llu tbu: 0x%08X  tbl: 0x%08X\n", tb, tb, tbu, tbl);
	OS_printf("sec: %u  usec: %u\n", LocalTime->seconds, LocalTime->microsecs);
	OS_printf("-------------\n");
	*/

}/* end CFE_PSP_GetLocalTime */

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
   return (sysClkRateGet());
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
    return(CFE_PSP_TIMER_TICKS_PER_SECOND);
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
   vxTimeBaseGet((UINT32 *)Tbu, (UINT32 *)Tbl);
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
   return(vxDecGet());
}

