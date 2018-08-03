/*
** cfe_psp_config.h
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This software may be used only pursuant to a United States government 
**      sponsored project and the United States government may not be charged
**      for use thereof. 
**
**
*/

#ifndef _cfe_psp_config_
#define _cfe_psp_config_



#include "common_types.h"


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
   uint32   regs[32];
  
} CFE_PSP_ExceptionContext_t;

#define CFE_PSP_CPU_CONTEXT_SIZE (sizeof(CFE_PSP_ExceptionContext_t))


/*
** Watchdog minimum and maximum values ( in milliseconds )
*/
#define CFE_PSP_WATCHDOG_MIN (0)
#define CFE_PSP_WATCHDOG_MAX (0xFFFFFFFF) 

/*
** Number of EEPROM banks on this platform
*/
#define CFE_PSP_NUM_EEPROM_BANKS 1

#endif

