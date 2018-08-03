/*
**  File Name:  cfe_psp_memory.h
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Author:  A. Cudmore
**
**  Purpose:  This file contains PSP support routine internal prototypes 
**            and typedefs. The routines and typedefs are intended for 
**            the local PSP routines only. 
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
////#include "cfe_platform_cfg.h"
#include "cfe_psp.h"


/*
** Typedef for the layout of the vxWorks USER_RESERVED_MEM
*/

typedef struct
{
   uint32 bsp_reset_type;
   uint32 spare1;
   uint32 spare2;
   uint32 spare3;
} CFE_PSP_ReservedFixedInfo_t;


typedef struct
{
   uint32 TotalSize;
   CFE_PSP_ReservedFixedInfo_t *FixedInfo;
   uint32 ResetSize;
   uint8  *ResetMemory;
   uint32 VolatileDiskSize;
   uint8  *VolatileDiskMemory; 
   uint32 CDSSize;
   uint8  *CDSMemory;
   uint32 UserReservedSize;
   uint8  *UserReservedMemory;
} CFE_PSP_ReservedMemoryMap_t;

/*
** External variables
*/

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
extern CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap; 

#endif  /* _cfe_psp_memory_ */
