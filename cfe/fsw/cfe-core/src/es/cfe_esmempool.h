/*
 *  Copyright (c) 2004-2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 *
 *  This is governed by the NASA Open Source Agreement and may be used,
 *  distributed and modified only according to the terms of that agreement.
 */

/**
 * \file cfe_esmempool.h
 *
 *  Created on: Jan 21, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Contains data structure definitions used by the ES mempool implementation.
 * These had previously been defined in cfe_esmempool.c.  The definitions are
 * moved into this header file so they can be shared with the unit test.
 */

#ifndef _CFE_ES_MEMPOOL_H_
#define _CFE_ES_MEMPOOL_H_

/*
** Include Files
*/
#include "common_types.h"

typedef struct
{
  uint16    CheckBits;
  uint16    Allocated;
  uint32    Size;
  uint32   *Next;
} OS_PACK BD_t;

typedef struct
{
  BD_t    *Top;
  uint32   NumCreated;
  uint32   NumFree;
  uint32   MaxSize;
} BlockSizeDesc_t;
/*
** Memory Pool Type
*/
typedef struct {
   cpuaddr         *Start;
   uint32           Size;
   cpuaddr          End;
   uint32          *Current;
   BlockSizeDesc_t *SizeDescPtr;
   uint16           CheckErrCntr;
   uint16           RequestCntr;
   uint32           MutexId;
   uint32           UseMutex;
   BlockSizeDesc_t  SizeDesc[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES];
} OS_PACK Pool_t;



#endif  /* _cfe_esmempool_ */
