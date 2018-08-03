/*
**  File Name:  cfe_psp_memory.h
**
**  Copyright (c) 2004-2011, United States Government as represented by 
**  Administrator for The National Aeronautics and Space Administration. 
**  All Rights Reserved.
**
**  This is governed by the NASA Open Source Agreement and may be used,
**  distributed and modified only pursuant to the terms of that agreement.
**
**  Author:  A. Cudmore
**
**  Purpose:  This file contains PSP support routine internal prototypes 
**            and typedefs. The routines and typedefs are intended for 
**            the local BSP routines only. 
**
**  Modification History:
**
*/

#ifndef _cfe_psp_memory_
#define _cfe_psp_memory_

/*
** Include Files
*/

#include "common_types.h"
#include "cfe_psp.h"

/*
** Macro Definitions
*/
#define CFE_PSP_CDS_SIZE            CFE_ES_CDS_SIZE
#define CFE_PSP_RESET_AREA_SIZE     CFE_ES_RESET_AREA_SIZE
#define CFE_PSP_VOLATILE_DISK_SIZE  (CFE_ES_RAM_DISK_SECTOR_SIZE * CFE_ES_RAM_DISK_NUM_SECTORS)
#define CFE_PSP_USER_RESERVED_SIZE  CFE_ES_USER_RESERVED_SIZE


/*
** Typedef for the layout of the vxWorks USER_RESERVED_MEM
*/
typedef struct
{
   uint32 bsp_last_reset_type;     /* Once the reset type has been determined by the PSP
                                   ** at startup, it is stored here. */
   uint32 bsp_reset_type;          /* If the PSP is used to restart the board, this variable 
                                   ** is set to a value that describes how the board was reset
                                   ** Upon system startup, the PSP will look at this location,
                                   ** latch it's value in the "bsp_last_reset_type" variable,
                                   ** perform any action associated with the reset type 
                                   ** (like clear reserved memory if appropriate),
                                   ** then re-initialize this variable so the next reset
                                   ** will not be confused with the last. */
   uint32 PreviousModeId;
   uint32 spare2;
   uint32 spare3;
   uint8  ResetMemory[CFE_PSP_RESET_AREA_SIZE];
   uint8  VolatileDiskMemory[CFE_PSP_VOLATILE_DISK_SIZE];
   uint8  CDSMemory[CFE_PSP_CDS_SIZE];
   uint8  UserReservedMemory[CFE_PSP_USER_RESERVED_SIZE];

} CFE_PSP_ReservedMemory_t;

#define CFE_PSP_RESERVED_MEM_ADDR   ((CFE_PSP_ReservedMemory_t * ) sysMemTop())

/*
** External variables
*/

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
extern CFE_PSP_ReservedMemory_t *CFE_PSP_ReservedMemoryPtr; 

#endif  /* _cfe_psp_memory_ */
