/*
**  File: utf_cfe_esmempool.c
** $Id: utf_cfe_esmempool.c 1.6 2012/01/13 12:51:56GMT-05:00 acudmore Exp  $ 
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
**  Purpose:
**  Set of services for management of discrete sized memory pools.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  $Log: utf_cfe_esmempool.c  $
**  Revision 1.6 2012/01/13 12:51:56GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.5 2010/10/25 15:06:39EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.4 2010/10/20 12:45:58EDT jmdagost 
**  Updates to include no-semaphore memory pool creation.
**  Revision 1.3 2010/10/04 14:57:08EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2009/10/13 14:55:29EDT wmoleski 
**  Changes required to be compatible with cFE 6.0.0
**  Revision 1.1 2008/04/17 08:09:36EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
**  Revision 1.10 2008/04/07 13:44:41EDT wfmoleski 
**  Added return code setting for ES_CPoolCreate, ES_GetPoolBuf and ES_PutPoolBuf
**  Revision 1.9 2008/04/07 09:39:37EDT wfmoleski 
**  Added the capability for the specified hook functions. Added return code setting code for several
**  OS functions that seemed appropriate.
**  Revision 1.8 2007/09/24 07:51:51EDT wfmoleski 
**  Made changes to UTF source to reflect the API change to GetPoolBuf in the cFE fsw.
**  Revision 1.7 2007/06/05 08:24:51EDT wfmoleski 
**  Changes to examples and utf source that reflect the cFE 4.1.0 ES subsystem.
**  Revision 1.6 2007/05/25 11:04:17EDT wfmoleski 
**  Modifications to mimic the cFE 4.1.0 ES fsw. These changes include new APIs as well as Task info storage.
**  Revision 1.5 2006/11/09 08:03:45EST wfmoleski 
**  UTF changes for cFE 4.0
**  Revision 1.4 2006/07/21 14:06:00EDT wfmoleski 
**  Modified an if statement to conform to the fsw version.
**  Revision 1.3 2006/06/22 13:53:34EDT wfmoleski 
**  Most of the changes to these files was to add the copyright comments. There were other changes needed
**  to bring the UTF up to cFE 3.3
**  Revision 1.2 2006/05/26 15:03:39EDT nsschweiss 
**  Changes for UTF 3.2.
**  Revision 1.1 2006/04/26 17:25:15EDT nsschweiss 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
**  Revision 1.6 2006/02/16 11:48:35EST apcudmore 
**  Updated sprintf format to eliminate warnings.
**  Revision 1.5 2006/01/26 20:16:44GMT rjmcgraw 
**  Added semaphore for each pool
**
*/

/*
** Includes
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_es.h"
#include "utf_types.h" /* for UTF_put_text() */
#include "utf_osapi.h" /* for UTF_CFE_USE_DEFAULT_RETURN_CODE*/
#include "utf_cfe_es.h"

#include <stdio.h>

/*****************************************************************************/
/*
** Local Macro Definitions
*/
#define CFE_ES_CHECK_PATTERN  0x5a
#define CFE_ES_MEMORY_ALLOCATED 0xaaaa
#define CFE_ES_MEMORY_DEALLOCATED 0xdddd
#define CFE_ES_MAX_MEMPOOL_BLOCK_SIZES     17
/*****************************************************************************/
/*
** Type Definitions
*/

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
   uint32          *Start;
   uint32           Size;
   uint32           End;
   uint32          *Current;
   BlockSizeDesc_t *SizeDescPtr;
   uint16           CheckErrCntr;
   uint16           RequestCntr;
   uint32           MutexId;
   uint32           UseMutex;
   BlockSizeDesc_t  SizeDesc[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES];
} OS_PACK Pool_t;

uint32 CFE_ES_MemPoolDefSize[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES] = 
{
    CFE_ES_MAX_BLOCK_SIZE,
    CFE_ES_MEM_BLOCK_SIZE_16,
    CFE_ES_MEM_BLOCK_SIZE_15,
    CFE_ES_MEM_BLOCK_SIZE_14,
    CFE_ES_MEM_BLOCK_SIZE_13,
    CFE_ES_MEM_BLOCK_SIZE_12,
    CFE_ES_MEM_BLOCK_SIZE_11,
    CFE_ES_MEM_BLOCK_SIZE_10,
    CFE_ES_MEM_BLOCK_SIZE_09,
    CFE_ES_MEM_BLOCK_SIZE_08,
    CFE_ES_MEM_BLOCK_SIZE_07,
    CFE_ES_MEM_BLOCK_SIZE_06,
    CFE_ES_MEM_BLOCK_SIZE_05,
    CFE_ES_MEM_BLOCK_SIZE_04,
    CFE_ES_MEM_BLOCK_SIZE_03,
    CFE_ES_MEM_BLOCK_SIZE_02,
    CFE_ES_MEM_BLOCK_SIZE_01
};

/* Function Hook Stuff */
typedef struct
{
    int32 (*CFE_ES_PoolCreate)(CFE_ES_MemHandle_t*, uint8*, uint32);
    int32 (*CFE_ES_PoolCreateNoSem)(CFE_ES_MemHandle_t*, uint8*, uint32);
    int32 (*CFE_ES_GetPoolBuf)(uint32**, CFE_ES_MemHandle_t, uint32);
    int32 (*CFE_ES_PutPoolBuf)(CFE_ES_MemHandle_t, uint32);
} UTF_ES_HookTable_t;

UTF_ES_HookTable_t UTF_ES_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL
};

extern int32 cfe_es_api_return_value[];

/*****************************************************************************/
/* 
** Local Function Prototypes 
*/
uint32 CFE_ES_GetBlockSize(Pool_t  *PoolPtr, uint32 Size);

/******************************************************************************
**  Function: UTF_ES_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for an ES function call.
*/
void UTF_ES_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == CFE_ES_POOLCREATE_HOOK)       { UTF_ES_HookTable.CFE_ES_PoolCreate = FunPtr; }
    else if (Index == CFE_ES_POOLCREATENOSEM_HOOK)  { UTF_ES_HookTable.CFE_ES_PoolCreateNoSem = FunPtr; }
    else if (Index == CFE_ES_GETPOOLBUF_HOOK)       { UTF_ES_HookTable.CFE_ES_GetPoolBuf = FunPtr; }
    else if (Index == CFE_ES_PUTPOOLBUF_HOOK)       { UTF_ES_HookTable.CFE_ES_PutPoolBuf = FunPtr; }
    else                                            { UTF_error("Invalid ES Hook Index In Set Hook Call %d", Index); }
}

/*****************************************************************************/
/*
** Functions
*/

/*
** CFE_ES_PoolCreate will initialize a pre-allocated memory pool.  
*/
int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t  *HandlePtr,
                        uint8               *MemPtr,
                        uint32               Size )
{
   	/* Handle Preset Return Code */
   	if (cfe_es_api_return_value[CFE_ES_POOLCREATE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return cfe_es_api_return_value[CFE_ES_POOLCREATE_PROC];
   	}

   /* Handle Function Hook */
   if (UTF_ES_HookTable.CFE_ES_PoolCreate)
      return(UTF_ES_HookTable.CFE_ES_PoolCreate(HandlePtr,*MemPtr, Size));

    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, &CFE_ES_MemPoolDefSize[0], CFE_ES_USE_MUTEX);
}

/*
** CFE_ES_PoolCreateNoSem will initialize a pre-allocated memory pool without using a mutex.  
*/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t  *HandlePtr,
                             uint8               *MemPtr,
                             uint32               Size )
{
   	/* Handle Preset Return Code */
   	if (cfe_es_api_return_value[CFE_ES_POOLCREATENOSEM_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return cfe_es_api_return_value[CFE_ES_POOLCREATENOSEM_PROC];
   	}

   /* Handle Function Hook */
   if (UTF_ES_HookTable.CFE_ES_PoolCreateNoSem)
      return(UTF_ES_HookTable.CFE_ES_PoolCreateNoSem(HandlePtr,*MemPtr, Size));

    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, &CFE_ES_MemPoolDefSize[0], CFE_ES_NO_MUTEX);
}

int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t  *HandlePtr,
                          uint8               *MemPtr,
                          uint32               Size,
                          uint32               NumBlockSizes,
                          uint32              *BlockSizes,
                          uint16               UseMutex )
{
    /* char MutexName[10]; */
    uint32   i;
    uint32   j;
    uint32   k;
    uint32  *BlockSizeArrayPtr;
    uint32   BlockSizeArraySize;
    uint32   MinBlockSize;

   /*
   ** Local Variables
   */
   Pool_t *PoolPtr = (Pool_t *)MemPtr;
   *HandlePtr      = (uint32)MemPtr;

   /* Force the size given to be 32 bit aligned */
   Size &= 0xFFFFFFFC;

   /* Determine if the memory pool address is 32-bit aligned */
   if ((((uint32)MemPtr) & 0x00000003) != 0)
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Pool Address(0x%08X) is not 32-bit aligned.\n", (uint32)MemPtr);
      return(CFE_ES_BAD_ARGUMENT);
   }
   
   /* If too many sizes are specified, return an error */
   if (NumBlockSizes > CFE_ES_MAX_MEMPOOL_BLOCK_SIZES)
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Num Block Sizes (%d) greater than max (%d)\n",
                           NumBlockSizes, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES);
      return(CFE_ES_BAD_ARGUMENT);
   }

   PoolPtr->UseMutex = UseMutex;
   
   /*
   ** Construct a name for the Mutex from the address
   ** This is needed only because OS_MutSemCreate requires
   ** a unique name for each semaphore created.
   
   sprintf(MutexName, "%08X", (unsigned int)MemPtr);
   */
   
   /* create a semphore to protect this memory pool 
   OS_MutSemCreate(&(PoolPtr->MutexId), MutexName, 0); */

   /* Take the semaphore to ensure the mem pool is not being used during it's creation 
   OS_MutSemTake(PoolPtr->MutexId); */

   PoolPtr->Start        = (uint32 *)*HandlePtr;
   PoolPtr->End          = (uint32)((uint8 *)PoolPtr->Start + Size);
   PoolPtr->Size         = Size;
   PoolPtr->Current      = (uint32 *)(MemPtr + sizeof(Pool_t));
   PoolPtr->SizeDescPtr  = NULL;

   PoolPtr->CheckErrCntr = 0;
   PoolPtr->RequestCntr  = 0;

   for (i=0; i<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES; i++)
   {
      PoolPtr->SizeDesc[i].NumCreated = 0;
      PoolPtr->SizeDesc[i].NumFree = 0;
      PoolPtr->SizeDesc[i].MaxSize = 0;
      PoolPtr->SizeDesc[i].Top = NULL;
   }

   /* Use default block sizes if none or too many sizes are specified */
   if ((NumBlockSizes == 0) || (BlockSizes == NULL))
   {
       BlockSizeArrayPtr = &CFE_ES_MemPoolDefSize[0];
       BlockSizeArraySize = CFE_ES_MAX_MEMPOOL_BLOCK_SIZES;
   }
   else
   {
       BlockSizeArrayPtr = BlockSizes;
       BlockSizeArraySize = NumBlockSizes;
   }
   
   /* Use specified block sizes but make sure they are ordered largest to smallest */
   MinBlockSize = 0xffffffff;
   for (i=0; i<BlockSizeArraySize; i++)
   {
        if ((BlockSizeArrayPtr[i] < MinBlockSize) && (BlockSizeArrayPtr[i] != 0))
        {
            MinBlockSize = BlockSizeArrayPtr[i];
        }
        
        j = 0;
        while (j<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES)
        {
            if (BlockSizeArrayPtr[i] > PoolPtr->SizeDesc[j].MaxSize)
            {
                /* Make space for new size */
                for (k=i; k>j; k--)
                {
                    PoolPtr->SizeDesc[k].MaxSize = PoolPtr->SizeDesc[k-1].MaxSize;
                }
                
                /* Insert the size in the correct location */
                PoolPtr->SizeDesc[j].MaxSize = BlockSizeArrayPtr[i];
                j = CFE_ES_MAX_MEMPOOL_BLOCK_SIZES;
            }
            else
            {
                j++;
            }
        }
   }

   if (Size <= (sizeof(Pool_t) + MinBlockSize + sizeof(BD_t) ) )
   {  /* must be able make Pool management structure and at least one of the smallest blocks  */
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Pool size(%d) too small for management structure, need >=(%d)\n",
                            (int)Size,
                            (int)(sizeof(Pool_t) + MinBlockSize + sizeof(BD_t)) );
                            
      /* Mutex is no longer needed with error in initialization.  Give it and delete it         
      OS_MutSemGive(PoolPtr->MutexId);
      OS_MutSemDelete(PoolPtr->MutexId); */
      return(CFE_ES_BAD_ARGUMENT);
   }

   /* OS_MutSemGive(PoolPtr->MutexId); */

   return(CFE_SUCCESS);
}


/*
** Function:
**   CFE_ES_GetPoolBuf
** 
** Purpose:
**   CFE_ES_GetPoolBuf allocates a block from the memory pool.  
*/
int32 CFE_ES_GetPoolBuf(uint32             **BufPtr,
                        CFE_ES_MemHandle_t  HandlePtr,
                        uint32              Size )
{
  Pool_t  * PoolPtr = (Pool_t *)HandlePtr;
  uint32    Block;
  BD_t    * BdPtr;
  uint32    AppId = 0xFFFFFFFF;

  /* OS_MutSemTake(PoolPtr->MutexId);  */
  /* Handle Preset Return Code */
  if (cfe_es_api_return_value[CFE_ES_GETPOOLBUF_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
  {
	return cfe_es_api_return_value[CFE_ES_GETPOOLBUF_PROC];
  }

  /* Handle Function Hook */
  if (UTF_ES_HookTable.CFE_ES_GetPoolBuf)
     return(UTF_ES_HookTable.CFE_ES_GetPoolBuf(BufPtr, HandlePtr, Size));

  *BufPtr = NULL;

  CFE_ES_GetAppID(&AppId);

  if (HandlePtr != (CFE_ES_MemHandle_t)PoolPtr->Start)
  {
    CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Bad handle.AppId=%d\n",AppId);
    /* OS_MutSemGive(PoolPtr->MutexId); */
    return(CFE_ES_ERR_MEM_HANDLE);   
  }

  Block = CFE_ES_GetBlockSize(PoolPtr, Size);
  if (Block == 0xFFFFFFFF)
  {
    CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:size(%d) > max(%d).\n",Size,CFE_ES_MAX_BLOCK_SIZE);
    /* OS_MutSemGive(PoolPtr->MutexId); */
    return(CFE_ES_ERR_MEM_BLOCK_SIZE);
  }

  /* 
  ** Check if any of the requested size are available 
  */
  if (PoolPtr->SizeDescPtr->Top != NULL) /* Set by CFE_ES_GetBlockSize call */
  { 
    /* 
     ** Get it off the top on the list 
    */
    BdPtr = PoolPtr->SizeDescPtr->Top;

    BdPtr->CheckBits     = CFE_ES_CHECK_PATTERN;
    BdPtr->Allocated     = CFE_ES_MEMORY_ALLOCATED; /* Flag memory block as allocated */
    BdPtr->Size          = Block;
    PoolPtr->SizeDescPtr->Top = (BD_t *)BdPtr->Next;
    PoolPtr->SizeDescPtr->NumFree--;
    BdPtr->Next          = NULL;
    *BufPtr              = (uint32 *)(BdPtr + 1);
  }
  else /* go make one */
  {
    BdPtr = (BD_t *)PoolPtr->Current; /* point to new memory block   */
    if (((uint32)BdPtr + sizeof(BD_t) + Block) >= PoolPtr->End )
    { /* can't fit in remaing mem */
      CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Request won't fit in remaining memory\n");
/*      OS_MutSemGive(PoolPtr->MutexId); */
      return(CFE_ES_ERR_MEM_BLOCK_SIZE);
    }

    PoolPtr->SizeDescPtr->NumCreated++;
    PoolPtr->RequestCntr++;

    /* 
    ** create the buffer descriptor at the front of it 
    */
    BdPtr->CheckBits = CFE_ES_CHECK_PATTERN;
    BdPtr->Allocated = CFE_ES_MEMORY_ALLOCATED; /* Flag memory block as allocated */
    BdPtr->Size      = Block;
    BdPtr->Next      = NULL;
    *BufPtr          = (uint32 *)(BdPtr + 1);

    /* 
    ** adjust pool current pointer 
    */
    PoolPtr->Current = (uint32 *)( (uint8 *)PoolPtr->Current
                                             + Block
                                             + sizeof(BD_t) );
  }

  /* OS_MutSemGive(PoolPtr->MutexId); */
  return (int32)Block;
}

/*
** CFE_ES_GetPoolBufInfo gets the size of the specified block (if it exists).
*/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t   Handle,
                            uint32             * BufPtr)
{
  Pool_t   *PoolPtr =  (Pool_t *)Handle;
  BD_t     *BdPtr    = (BD_t *) ((uint8 *)BufPtr - sizeof(BD_t));

  if ( ((uint32)BdPtr < Handle) || ((uint32)BdPtr >= (PoolPtr->End - sizeof(BD_t))) )
  {
      /* sanity check */
      return(CFE_ES_BUFFER_NOT_IN_POOL);
  }

  /*
  ** Simple sanity checks for descriptor
  */
  /* If a block is no longer allocated, report an error */
  if (BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED)
  {
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
  {
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  return ((int32) (BdPtr->Size));
}


/*
** CFE_ES_putPoolBuf returns a block back to the memory pool.  
*/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t   Handle,
                        uint32             * BufPtr)
{
  Pool_t   *PoolPtr =  (Pool_t *)Handle;
  BD_t     *BdPtr    = (BD_t *) ((uint8 *)BufPtr - sizeof(BD_t));
  uint32    Block;

  /* OS_MutSemTake(PoolPtr->MutexId); */
  /* Handle Preset Return Code */
  if (cfe_es_api_return_value[CFE_ES_PUTPOOLBUF_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
  {
	return cfe_es_api_return_value[CFE_ES_PUTPOOLBUF_PROC];
  }
 
   /* Handle Function Hook */
  if (UTF_ES_HookTable.CFE_ES_PutPoolBuf)
     return(UTF_ES_HookTable.CFE_ES_PutPoolBuf(Handle, *BufPtr));
  
  if ( ((uint32)BdPtr < Handle) || ((uint32)BdPtr >= (PoolPtr->End - sizeof(BD_t))) ) 
  { 
    /* sanity check */
    CFE_ES_WriteToSysLog("CFE_ES:putPool-Invalid Memory Handle (0x%08X) or memory block (0x%08X).\n", 
                         (uint32) Handle, (uint32)BdPtr);
    /* OS_MutSemGive(PoolPtr->MutexId); */
    return(CFE_ES_ERR_MEM_HANDLE);
  }

  /* 
  ** Simple sanity checks for descriptor 
  */
 
 /* Don't allow a block that has already been deallocated to be deallocated again */
  if (BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED)
  {
    PoolPtr->CheckErrCntr++;
    CFE_ES_WriteToSysLog("CFE_ES:putPool-Deallocating unallocated memory block @ 0x%08X\n", (uint32)BdPtr);
    /* OS_MutSemGive(PoolPtr->MutexId); */
    return(CFE_ES_ERR_MEM_HANDLE);
  }
  
  if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
  {
    PoolPtr->CheckErrCntr++;
    CFE_ES_WriteToSysLog("CFE_ES:putPool-Invalid Memory descriptor @ 0x%08X\n", (uint32)BdPtr);
    /* OS_MutSemGive(PoolPtr->MutexId); */
    return(CFE_ES_ERR_MEM_HANDLE);
  }

  Block = CFE_ES_GetBlockSize(PoolPtr, BdPtr->Size);
  
  if (Block == 0xFFFFFFFF)
  {
      PoolPtr->CheckErrCntr++;
      CFE_ES_WriteToSysLog("CFE_ES:putPool-Invalid/Corrupted Memory descriptor @ 0x%08X\n", (uint32)BdPtr);
      /* OS_MutSemGive(PoolPtr->MutexId); */
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  BdPtr->Allocated = CFE_ES_MEMORY_DEALLOCATED;
  BdPtr->Next = (uint32 *)PoolPtr->SizeDescPtr->Top; /* Set by GetBlockSize call */
  PoolPtr->SizeDescPtr->Top = BdPtr;
  PoolPtr->SizeDescPtr->NumFree++;

  /* OS_MutSemGive(PoolPtr->MutexId); */
  return (int32)Block;
}

/*
** Function:
**   ES_GetBlockSize
** 
** Purpose:
**   
*/
uint32 CFE_ES_GetBlockSize(Pool_t  *PoolPtr, uint32 Size)
{
  uint32 i=0;

  PoolPtr->SizeDescPtr = NULL;

  /* If caller is requesting a size larger than the largest allowed */
  /* then return an error code */  
  if (Size > PoolPtr->SizeDesc[0].MaxSize)
  {
        return(0xFFFFFFFF);
  }
  
  /* Locate the smallest size that holds the desired size */
  while ((i < (CFE_ES_MAX_MEMPOOL_BLOCK_SIZES-1)) &&
         (Size <= PoolPtr->SizeDesc[i+1].MaxSize))
  {
        i++;
  }
  
  PoolPtr->SizeDescPtr = &PoolPtr->SizeDesc[i];
  return(PoolPtr->SizeDesc[i].MaxSize);
}

/*
** Function:
**   CFE_ES_GetMemPoolStats
**
** Purpose:
**
*/
int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr,
                             CFE_ES_MemHandle_t  Handle)
{
    uint32    AppId = 0xFFFFFFFF;
    Pool_t   *PoolPtr;
    uint32    i;
    
    PoolPtr = (Pool_t *)Handle;

    CFE_ES_GetAppID(&AppId);

    if (Handle != (CFE_ES_MemHandle_t)PoolPtr->Start)
    {
        CFE_ES_WriteToSysLog("CFE_ES:getMemPoolStats err:Bad handle(0x%08X) AppId=%d\n", Handle, AppId);
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    BufPtr->PoolSize = PoolPtr->Size;
    BufPtr->NumBlocksRequested = PoolPtr->RequestCntr;
    BufPtr->CheckErrCtr = PoolPtr->CheckErrCntr;
    BufPtr->NumFreeBytes = PoolPtr->End - ((uint32)PoolPtr->Current);
    
    for (i=0; i<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES; i++)
    {
        BufPtr->BlockStats[i].BlockSize = PoolPtr->SizeDesc[i].MaxSize;
        BufPtr->BlockStats[i].NumCreated = PoolPtr->SizeDesc[i].NumCreated;
        BufPtr->BlockStats[i].NumFree = PoolPtr->SizeDesc[i].NumFree;
    }
    
    return(CFE_SUCCESS);
}

/*
** Function:
**   CFE_ES_ValidateHandle
**
** Purpose:
**   Insures that the handle passed in meets all of the requirements of a valid handle.
*/
boolean CFE_ES_ValidateHandle(CFE_ES_MemHandle_t  Handle)
{
    boolean   HandleValid = TRUE;
    Pool_t   *PoolPtr;
    
    PoolPtr = (Pool_t *)Handle;

    /* There are various ways to make sure the memory pool handle is valid */
   
    /* Step #1) Make sure the handle holds an address that is on a 32-bit boundary */
    if ((Handle & 0x03) != 0)
    {
        HandleValid = FALSE;
    }
    else 
    {
        /* Step #2) TODO!!!  INSERT CHECK TO MAKE SURE HANDLE CONTAINS A VALID MEMORY ADDRESS */
        
        /* Step #3) The first field of the pool structure should contain the start address of the pool */
        if (Handle != (CFE_ES_MemHandle_t)PoolPtr->Start)
        {
            HandleValid = FALSE;
        }
        /* Step #4) The size of the memory pool must be a multiple of 4 bytes */
        else if ((PoolPtr->Size & 0x03) != 0)
        {
            HandleValid = FALSE;
        }
        /* Step #5) The pool structure should have an End pointer that is equal to the start plus the size */
        else if ((*PoolPtr->Start + PoolPtr->Size) != PoolPtr->End)
        {
            HandleValid = FALSE;
        }
    }
    
    return(HandleValid);
}
