/* TODO: this is from the s950 PSP, need to update for SP0
 *
 * The WD functions must not be the same as the s950
 * ld<cfe-core.o
Warning: module 0x461d010 holds reference to undefined symbol watchDogDisable.
Warning: module 0x461d010 holds reference to undefined symbol watchDogRate.
Warning: module 0x461d010 holds reference to undefined symbol watchDogEnable.
Warning: module 0x461d010 holds reference to undefined symbol watchDogToggle.
 * */


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
**   2010/06/25  S. Duran/JSC  | Modified for Aitech S950 board, reference section 12 Internal 
**   							 Watchdog Driver in S950 VxWorks BSP Programmer's Guide 
**   							 PRVS950_x23.pdf, page 48
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
**  System Include Files
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

/* TODO: for 950 BSP functions, needs to be in -I path #include "sys950Lib.h" */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"



/*
** Global data
*/

/*
** The watchdog time in milliseconds
*/
/* uint32 CFE_PSP_WatchdogValue = CFE_PSP_WATCHDOG_MAX; MAX is really big */
uint32 CFE_PSP_WatchdogValue = 20000; /* 5 seconds */
	
/*  Function:  CFE_PSP_WatchdogInit()
**
**  Purpose:
**    To setup the timer resolution and/or other settings custom to this platform.
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogInit(void)
{
	/* in ms in this code, but S950 WG can only be set in terms of seconds */  
	/* watchDogRate(CFE_PSP_WatchdogValue / 1000);*/
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogEnable()
**
**  Purpose:
**    Enable the watchdog timer
**    If the S950's BSP is set to define "INCLUDE_WATCHDOGS" in config.h or
**    using the kernel config tool, this function will be called in
**    sysHwInit2() in sysLib.c.  The watchdog timer is disabled by default in 
**    startup firmware.
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogEnable(void)
{
	/* First argument: BOOL enable_reset - if true enables board 
	 * reset on watchdog failure 
	 * Second argument: BOOL enable_nmi - if true enable non-maskable 
	 * interrupt on watchdog failure */
	/*watchDogEnable(TRUE, FALSE);*/
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
	/*watchDogDisable();*/
}

/******************************************************************************
**  Function:  CFE_PSP_WatchdogService()
**
**  Purpose:
**    Feed the dog
**
**  Arguments:
**    None.
**
**  Return:
**    None
**
**  Note:
*/
void CFE_PSP_WatchdogService(void)
{
	/* This function toggles the watchdog timer and it must be 
	 * called at least once every watchdog timer period if the watchdog timer 
	 * is enabled.  Otherwise, the board will perform a reset once the timer
	 * expires */
	/*watchDogToggle();*/
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
**    the current watchdog value 
**
**  Notes:
**
*/
uint32 CFE_PSP_WatchdogGet(void)
{
   return(CFE_PSP_WatchdogValue);
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogSet
**
**  Purpose:
**    Get the current watchdog value. 
**
**  Arguments:
**    The new watchdog value 
**
**  Return:
**    nothing 
**
**  Notes:
**
*/
void CFE_PSP_WatchdogSet(uint32 WatchdogValue)
{
    CFE_PSP_WatchdogValue = WatchdogValue;
    
	/* in ms in this code, but S950 WG can only be set in terms of seconds */  
	/*watchDogRate(CFE_PSP_WatchdogValue / 1000);*/
}

