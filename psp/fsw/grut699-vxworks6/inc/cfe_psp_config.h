/*
** cfe_psp_config.h
**
**      Copyright (c) 2004-2011, United States Government as represented by Administrator 
**      for The National Aeronautics and Space Administration. All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
*/

#ifndef _cfe_psp_config_
#define _cfe_psp_config_


#include "common_types.h"

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include "fppLib.h"
#include "excLib.h"
#include "taskLib.h"
#include "arch/sparc/esfSparc.h"
#include "arch/sparc/fppSparcLib.h"

/*
** This define sets the number of memory ranges that are defined in the memory range defintion
** table.
*/
#define CFE_PSP_MEM_TABLE_SIZE 10

/*
** Processor Context type. 
** This is needed to determine the size of the context entry in the ER log.
** Although this file is in a CPU directory, it really is OS dependant, so supporting
** multiple OSs on the same CPU architecture ( i.e. x86/linux, x86/windows, x86/osx ) 
** will require IFDEFS. 
*/
typedef struct 
{
    FP_CONTEXT  fp;     /* floating point registers */
   
} CFE_PSP_ExceptionContext_t;

#define CFE_PSP_CPU_CONTEXT_SIZE (sizeof(CFE_PSP_ExceptionContext_t))

/*
** Watchdog minimum and maximum values ( in milliseconds )
*/
#define CFE_PSP_WATCHDOG_MIN                     0x00000004U  /* */
#define CFE_PSP_WATCHDOG_MAX                     0xFFFFFFFEU  /* ~458 sec (7:38) */

/* 75MHz clk / prescaler = 8 (default in VxWorks SPARC Leon3 BSP)
 *  = 9,375,000 ticks per second on, 9375 ticks/ms */
#define CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC  9375U

/*
** Number of EEPROM banks on this platform
*/
#define CFE_PSP_NUM_EEPROM_BANKS 0

#endif

