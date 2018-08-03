/************************************************************************************************
** File:  cfe_psp_watchdog.c
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
**   2009/07/20  A. Cudmore    | Initial version,
 **   2014/08/08  S. Duran     | Modified for ut699 SPARC LEON3 board
**
*************************************************************************************************/

/*
**  Include Files
*/


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

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
** Types and prototypes for this module
*/
#include "cfe_psp.h"

/* Register address and control bit values from the SPARC Leon3 user manual */
#define SPARC_LEON3_TIMER_REG_ADDR (0x80000300)

#define TIMER4_ID (3)
#define TIMER4_CONTROL_IP     (0x00000010U)  /* Interrupt pending */
#define TIMER4_CONTROL_IE     (0x00000008U)  /* Interrupt Enable */
#define TIMER4_CONTROL_LD     (0x00000004U)  /* Load  */
#define TIMER4_CONTROL_EN     (0x00000001U)  /* Enable */

typedef struct timer_reg_tag
{
    volatile uint32 counter;
    volatile uint32 reload;
    volatile uint32 control;
    volatile uint32 unused;
} TIMER_REG;

typedef struct sparc_leon3_timer_reg_tag
{
    volatile uint32 scaler;
    volatile uint32 scalerReload;
    volatile uint32 config;
    volatile uint32 unused;
    TIMER_REG timer[4];
} SPARC_LEON3_TIMER_REG_T;

/*
 ** Global data
 */

static SPARC_LEON3_TIMER_REG_T *timerReg =
    (SPARC_LEON3_TIMER_REG_T *) SPARC_LEON3_TIMER_REG_ADDR;


/*  Function:  CFE_PSP_WatchdogInit()
 **
 **  Purpose:
 **    To setup the timer resolution and/or other settings custom to this
 **    platform.
 **
 **  Arguments:
 **
 **  Return:
 */
void CFE_PSP_WatchdogInit(void)
{
    timerReg->timer[TIMER4_ID].reload = CFE_PSP_WATCHDOG_MAX;

    /* clear timer control register */
    timerReg->timer[TIMER4_ID].control = 0x00000000U;
}

/******************************************************************************
 **  Function:  CFE_PSP_WatchdogEnable()
 **
 **  Purpose:
 **    Enable the watchdog timer
 **
 **  Arguments:
 **
 **  Return:
 */
void CFE_PSP_WatchdogEnable(void)
{
    uint32 reg = timerReg->timer[TIMER4_ID].control;

    reg &= ~TIMER4_CONTROL_IP; /* clear a pending interrupt  */

    reg |= TIMER4_CONTROL_IE; /* Enable interrupt */

    reg |= TIMER4_CONTROL_EN; /* Enable timer */

    timerReg->timer[TIMER4_ID].control = reg;
}

/******************************************************************************
 **  Function:  CFE_PSP_WatchdogDisable()
 **
 **  Purpose:
 **    Disable the watchdog timer
 **
 **  Arguments:
 **
 **  Return:
 */
void CFE_PSP_WatchdogDisable(void)
{
    /*
     ** Disable the timer
     */
    timerReg->timer[TIMER4_ID].control &= ~TIMER4_CONTROL_EN;
}

/******************************************************************************
 **  Function:  CFE_PSP_WatchdogService()
 **
 **  Purpose:
 **    Load the watchdog timer with a count that corresponds to the millisecond
 **    time given in the parameter.
 **
 **  Arguments:
 **    None.
 **
 **  Return:
 **    None
 **
 **  Notes:
 **
 */
void CFE_PSP_WatchdogService(void)
{
    /*
     ** Reload the timer with value in the reload reload register
     */
    timerReg->timer[TIMER4_ID].control |= TIMER4_CONTROL_LD;
}

/******************************************************************************
 **  Function:  CFE_PSP_WatchdogGet
 **
 **  Purpose:
 **    Get the current watchdog value.
 **
 **  Arguments:
 **    none
 **
 **  Return:
 **    the current watchdog value in millisec
 **
 **  Notes:
 **
 */
uint32 CFE_PSP_WatchdogGet(void)
{
    return (timerReg->timer[TIMER4_ID].counter /
            CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC);
}

/******************************************************************************
 **  Function:  CFE_PSP_WatchdogSet
 **
 **  Purpose:
 **    Set the watchdog value.
 **
 **  Arguments:
 **    WatchdogValue: desired time out value in millisec
 **
 **  Return:
 **    nothing
 **
 **  Notes:
 **
 */
void CFE_PSP_WatchdogSet(uint32 WatchdogValue)
{
    uint32 newValue = 0;

    /* convert WD time in millisec to timer ticks */
#warning "need to find the cppcheck keyword to suppress the warning here."
    /* cppcheck-suppress unsignedPositive */
    if (WatchdogValue <= (CFE_PSP_WATCHDOG_MIN / CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC))
    {
        newValue = CFE_PSP_WATCHDOG_MIN;
    }
    else if (WatchdogValue >= (CFE_PSP_WATCHDOG_MAX / CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC))
    {
        newValue = CFE_PSP_WATCHDOG_MAX;
    }
    else {
        newValue = WatchdogValue * CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC;
    }

    timerReg->timer[TIMER4_ID].reload = newValue;
    timerReg->timer[TIMER4_ID].control |= TIMER4_CONTROL_LD;
}

/******************************************************************************
 **  Function:  CFE_PSP_WDT_Reg
 **
 **  Purpose:
 **    Prints out the WDT registers for debugging
 **
 **  Arguments:
 **    info string
 **
 **  Return:
 **    nothing
 **
 **  Notes:
 **
 */
void CFE_PSP_WDT_Reg(const char *str)
{
    printf("%s\n", str);

    printf("  Scaler: %08lX\n", (unsigned long)timerReg->scaler);
    printf("  Scaler Reload: %08lX\n", (unsigned long)timerReg->scalerReload);
    printf("  Config: %08lX\n\n", (unsigned long)timerReg->config);

    printf("  Counter: %08lX\n", (unsigned long)timerReg->timer[TIMER4_ID].counter);
    printf("  Reload:  %08lX\n", (unsigned long)timerReg->timer[TIMER4_ID].reload);
    printf("  Ctrl:    %08lX\n", (unsigned long)timerReg->timer[TIMER4_ID].control);
}

