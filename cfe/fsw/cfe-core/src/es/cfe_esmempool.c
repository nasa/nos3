/*
**  File:
**  cfe_esmempool.c
**
**
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:
**  Set of services for management of discrete sized memory pools.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  $Log: cfe_esmempool.c  $
**  Revision 1.12 2014/07/07 10:57:32GMT-05:00 acudmore 
**  Modified CFE_ES_ValidateHandle to conditionally compile alignment check.
**  Revision 1.11 2012/01/13 11:50:17GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.10 2011/03/18 11:41:54EDT lwalling 
**  Add memory range test to pool handle verification function
**  Revision 1.9 2010/11/23 13:22:43EST jmdagost 
**  Corrected getPoolBuf and putPoolBuf error messages.
**  Revision 1.8 2010/11/04 14:17:32EDT acudmore 
**  Added option to disable memory pool alignment check.
**  Revision 1.7 2010/10/25 15:02:12EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.6 2010/10/20 12:34:54EDT jmdagost 
**  Major revision to make semaphore use for memory pool creation optional.  Also added NULL pointer tests.
**  Revision 1.5 2010/10/04 15:37:57EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.4 2010/09/21 16:26:21EDT jmdagost 
**  Moved calls to CFE_ES_GetAppID() to inside error condition blocks.
**  Revision 1.3 2010/09/21 16:09:20EDT jmdagost 
**  Added cfe_es_task.h.
**  Revision 1.2 2009/07/29 15:55:42EDT aschoeni 
**  Added GetPoolBufInfo function to return size or invalidity of a buffer in a pool
**  Revision 1.1 2008/04/17 08:05:11EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.30 2007/08/07 12:52:44EDT David Kobe (dlkobe) 
**  Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
**  Revision 1.29 2007/05/04 20:07:53EDT dlkobe 
**  Added command to telemeter memory pool statistics
**  Revision 1.28 2007/04/04 16:27:58EDT dlkobe 
**  Syntax errors
**  Revision 1.26 2007/04/04 16:06:37EDT dlkobe 
**  Made numerous changes to correct issues identified during ES code walkthrough
**  Revision 1.25 2006/11/13 10:55:33GMT-05:00 njyanchik 
**  This CP fixes the errors that were found in the LDRA testing. The list of specific errors fixed is
**  in the solution to this DCR
**  Revision 1.24 2006/11/07 11:29:00GMT-05:00 dlkobe 
**  Changed CFE_ES_MIN_BLOCK_SIZE to CFE_ES_MEM_BLOCK_SIZE_01
**  Revision 1.21 2006/10/31 13:44:10GMT-05:00 dlkobe 
**  Replaced memory pool block sizes with symbols that are defined in the cfe_platform_cfg.h file
**  Revision 1.20 2006/10/31 10:35:31GMT-05:00 dlkobe 
**  Modified parameters to CFE_ES_GetBlockSize to be of correct uint32 type
**  Revision 1.19 2006/10/31 10:28:54GMT-05:00 dlkobe 
**  Added "Allocated" flag to memory block descriptor to prevent multiple deallocations
**
*/

/*
** Includes
*/
#include "private/cfe_private.h"
#include "cfe_esmempool.h"
#include "cfe_es.h"
#include "cfe_es_task.h"
#include <stdio.h>

/*****************************************************************************/
/*
** Local Macro Definitions
*/
#define CFE_ES_CHECK_PATTERN           0x5a5a
#define CFE_ES_MEMORY_ALLOCATED        0xaaaa
#define CFE_ES_MEMORY_DEALLOCATED      0xdddd
/*****************************************************************************/
/*
** Type Definitions
*/


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

/*****************************************************************************/
/*
** Local Function Prototypes
*/
uint32 CFE_ES_GetBlockSize(Pool_t  *PoolPtr, uint32 Size);

/*****************************************************************************/
/*
** Functions
*/

/*
** CFE_ES_PoolCreateNoSem will initialize a pre-allocated memory pool without using a mutex.
*/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t  *HandlePtr,
                             uint8               *MemPtr,
                             uint32               Size )
{
    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
                               &CFE_ES_MemPoolDefSize[0],CFE_ES_NO_MUTEX);
}

/*
** CFE_ES_PoolCreate will initialize a pre-allocated memory pool while using a mutex.
*/
int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t  *HandlePtr,
                        uint8               *MemPtr,
                        uint32               Size )
{
    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
                               &CFE_ES_MemPoolDefSize[0],CFE_ES_USE_MUTEX);
}


int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t  *HandlePtr,
                          uint8               *MemPtr,
                          uint32               Size,
                          uint32               NumBlockSizes,
                          uint32              *BlockSizes,
                          uint16               UseMutex )
{
    char     MutexName[OS_MAX_API_NAME];
    cpuaddr  MemAddr;
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
   MemAddr         = (cpuaddr)MemPtr;
   *HandlePtr      = MemAddr;

   /* Force the size given to be 32 bit aligned */
   Size &= 0xFFFFFFFC;

#ifdef CFE_ES_MEMPOOL_ALIGNED
   /* Determine if the memory pool address is 32-bit aligned */
   if ((MemAddr & 0x00000003) != 0)
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Pool Address(0x%08lX) is not 32-bit aligned.\n", (unsigned long)MemAddr);
      return(CFE_ES_BAD_ARGUMENT);
   }
#endif
   
   /* If too many sizes are specified, return an error */
   if (NumBlockSizes > CFE_ES_MAX_MEMPOOL_BLOCK_SIZES)
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Num Block Sizes (%d) greater than max (%d)\n",
                           (int)NumBlockSizes, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES);
      return(CFE_ES_BAD_ARGUMENT);
   }

   if ((UseMutex != CFE_ES_USE_MUTEX) && (UseMutex != CFE_ES_NO_MUTEX))
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Invalid Mutex Usage Option (%d), must be %d or %d\n",
                           UseMutex, CFE_ES_NO_MUTEX, CFE_ES_USE_MUTEX);
      return(CFE_ES_BAD_ARGUMENT);
   }
   
   if (UseMutex == CFE_ES_USE_MUTEX)
   {
      /*
      ** Construct a name for the Mutex from the address
      ** This is needed only because OS_MutSemCreate requires
      ** a unique name for each semaphore created.
      */
      snprintf(MutexName, OS_MAX_API_NAME, "%08lX", (unsigned long)MemPtr);

      /* create a semphore to protect this memory pool */
      OS_MutSemCreate(&(PoolPtr->MutexId), MutexName, 0);

      /* Take the semaphore to ensure the mem pool is not being used during its creation */
      OS_MutSemTake(PoolPtr->MutexId);
   }
   
   PoolPtr->Start        = (cpuaddr *)*HandlePtr;
   PoolPtr->End          = MemAddr + Size;
   PoolPtr->Size         = Size;
   PoolPtr->Current      = (uint32 *)(MemAddr + sizeof(Pool_t));
   PoolPtr->SizeDescPtr  = NULL;

   PoolPtr->CheckErrCntr = 0;
   PoolPtr->RequestCntr  = 0;
   
   PoolPtr->UseMutex     = UseMutex;

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
                            
      if (UseMutex == CFE_ES_USE_MUTEX)
      {
         /* Mutex is no longer needed with error in initialization.  Give it and delete it */        
         OS_MutSemGive(PoolPtr->MutexId);
         OS_MutSemDelete(PoolPtr->MutexId);
      }        
      return(CFE_ES_BAD_ARGUMENT);
   }

   if (UseMutex == CFE_ES_USE_MUTEX)
   {
       OS_MutSemGive(PoolPtr->MutexId);
   }

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
                        CFE_ES_MemHandle_t   Handle,
                        uint32               Size )
{
   Pool_t  * PoolPtr = (Pool_t *)Handle;
   uint32    Block;
   BD_t    * BdPtr;
   uint32    AppId= 0xFFFFFFFF;

   if (PoolPtr != NULL)
   {
      if (Handle != (CFE_ES_MemHandle_t)PoolPtr->Start)
      {
         CFE_ES_GetAppID(&AppId);
         CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Bad handle(0x%08lX) AppId=%d\n",(unsigned long)Handle,(int)AppId);
         return(CFE_ES_ERR_MEM_HANDLE);
      }
   }
   else
   {
      CFE_ES_GetAppID(&AppId);
      CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Bad handle(0x%08lX) AppId=%d\n",(unsigned long)Handle,(int)AppId);
      return(CFE_ES_ERR_MEM_HANDLE);
   }


   if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
   {
      OS_MutSemTake(PoolPtr->MutexId);
   }

   *BufPtr = NULL;

   Block = CFE_ES_GetBlockSize(PoolPtr, Size);
   if (Block == 0xFFFFFFFF)
   {
      CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:size(%d) > max(%d).\n",(int)Size,(int)PoolPtr->SizeDesc[0].MaxSize);
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_BLOCK_SIZE);
   }

   /*
   ** Check if any of the requested size are available
   */
   if (PoolPtr->SizeDescPtr->Top != NULL) /* Set by GetBlockSize call */
   {
         /*
         ** Get it off the top on the list
         */
         BdPtr = PoolPtr->SizeDescPtr->Top;

         BdPtr->CheckBits          = CFE_ES_CHECK_PATTERN;
         BdPtr->Allocated          = CFE_ES_MEMORY_ALLOCATED; /* Flag memory block as allocated */
         BdPtr->Size               = Block;
         PoolPtr->SizeDescPtr->Top = (BD_t *)BdPtr->Next;
         PoolPtr->SizeDescPtr->NumFree--;
         BdPtr->Next               = NULL;
         *BufPtr                   = (uint32 *)(BdPtr + 1);
    }
    else /* go make one */
    {
         BdPtr = (BD_t *)PoolPtr->Current; /* point to new memory block   */
         if ( ((cpuaddr)BdPtr + sizeof(BD_t) + Block ) >= PoolPtr->End ){ /* can't fit in remaing mem */
            CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Request won't fit in remaining memory\n");
            if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
            {
               OS_MutSemGive(PoolPtr->MutexId);
            }
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

     if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
     {
        OS_MutSemGive(PoolPtr->MutexId);
     }
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

  if (PoolPtr != NULL)
  {
     if ( ((cpuaddr)BdPtr < Handle) || ((cpuaddr)BdPtr >= (PoolPtr->End - sizeof(BD_t))) )
     {
         /* sanity check */
         return(CFE_ES_BUFFER_NOT_IN_POOL);
     }
  }
  else
  {
      /* bad handle */
      return(CFE_ES_ERR_MEM_HANDLE);
  }
  
  if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
  {
     OS_MutSemTake(PoolPtr->MutexId);
  }

  /*
  ** Simple sanity checks for descriptor
  */
  
  /* If a block is no longer allocated, report an error */
  if (BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED)
  {
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_HANDLE);
  }
  
  if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
  {
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  
  if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
  {
     OS_MutSemGive(PoolPtr->MutexId);
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
  cpuaddr   BdAddr;

  BdAddr = (cpuaddr)BdPtr;

  if (PoolPtr != NULL)
  {
     if ( (BdAddr < Handle) || (BdAddr >= (PoolPtr->End - sizeof(BD_t))) )
     {
         /* sanity check */
         CFE_ES_WriteToSysLog("CFE_ES:putPoolBuf err:Invalid Memory Handle (0x%08lX) or memory block (0x%08lX).\n",
                              (unsigned long) Handle, (unsigned long)BdAddr);
         return(CFE_ES_ERR_MEM_HANDLE);
     }
  }
  else
  {
      /* sanity check */
      CFE_ES_WriteToSysLog("CFE_ES:putPoolBuf err:Invalid Memory Handle (0x%08lX).\n", (unsigned long) Handle);
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  
  if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
  {
     OS_MutSemTake(PoolPtr->MutexId);
  }

  /*
  ** Simple sanity checks for descriptor
  */
  
  /* Don't allow a block that has already been deallocated to be deallocated again */
  if (BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED)
  {
      PoolPtr->CheckErrCntr++;
      CFE_ES_WriteToSysLog("CFE_ES:putPoolBuf err:Deallocating unallocated memory block @ 0x%08lX\n", (unsigned long)BdAddr);
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_HANDLE);
  }
  
  if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
  {
      PoolPtr->CheckErrCntr++;
      CFE_ES_WriteToSysLog("CFE_ES:putPoolBuf err:Invalid/Corrupted Memory descriptor @ 0x%08lX\n", (unsigned long)BdAddr);
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  Block = CFE_ES_GetBlockSize(PoolPtr, BdPtr->Size);
  
  if (Block == 0xFFFFFFFF)
  {
      PoolPtr->CheckErrCntr++;
      CFE_ES_WriteToSysLog("CFE_ES:putPoolBuf err:size(%d) > max(%d).\n",(int)BdPtr->Size,(int)PoolPtr->SizeDesc[0].MaxSize);
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      return(CFE_ES_ERR_MEM_HANDLE);
  }

  BdPtr->Allocated = CFE_ES_MEMORY_DEALLOCATED;
  BdPtr->Next = (uint32 *)PoolPtr->SizeDescPtr->Top; /* Set by GetBlockSize call */
  PoolPtr->SizeDescPtr->Top = BdPtr;
  PoolPtr->SizeDescPtr->NumFree++;

  if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
  {
     OS_MutSemGive(PoolPtr->MutexId);
  }
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

    if (Handle != (CFE_ES_MemHandle_t)PoolPtr->Start)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("CFE_ES:getMemPoolStats err:Bad handle(0x%08lX) AppId=%d\n", (unsigned long)Handle, (int)AppId);
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    BufPtr->PoolSize = PoolPtr->Size;
    BufPtr->NumBlocksRequested = PoolPtr->RequestCntr;
    BufPtr->CheckErrCtr = PoolPtr->CheckErrCntr;
    BufPtr->NumFreeBytes = PoolPtr->End - ((cpuaddr)PoolPtr->Current);
    
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

    /* There are several tests to make sure the memory pool handle is valid */
   
    if ( PoolPtr == NULL )
    /* Test #1) Handle must not be a NULL pointer */
    {
        HandleValid = FALSE;
    }
#ifdef CFE_ES_MEMPOOL_ALIGNED
    /* Test #2) Handle must be an address on a 32-bit boundary */
    else if ((Handle & 0x03) != 0)
    {
        HandleValid = FALSE;
    }
#endif
    /* Test #3) Handle must be a valid memory address (allows both RAM and EEPROM) */
    else if (CFE_PSP_MemValidateRange(Handle, sizeof(Pool_t), CFE_PSP_MEM_ANY) != CFE_PSP_SUCCESS)
    {
        HandleValid = FALSE;
    }
    /* Test #4) First field of pool structure must be start address of Pool */
    else if (Handle != (CFE_ES_MemHandle_t)PoolPtr->Start)
    {
        HandleValid = FALSE;
    }
    /* Test #5) Size of memory pool must be a multiple of 4 bytes */
    else if ((PoolPtr->Size & 0x03) != 0)
    {
        HandleValid = FALSE;
    }
    /* Test #6) Pool structure must have End ptr equal to Start plus Size */
    else if ((*PoolPtr->Start + PoolPtr->Size) != PoolPtr->End)
    {
        /*
        ** The test above deserves some explanation (it fooled me - LSW)
        **
        ** The first field in a Pool_t structure is "uint32 *Start;"
        ** The Start field is set to point to the beginning of the Pool structure.
        ** Therefore, the Start field actually points to itself.
        ** So, (PoolPtr->Start) is the Start address in the form of a pointer to a uint32.
        ** And, (*PoolPtr->Start) is the Start address in the form of a uint32.
        ** Thus, (*PoolPtr->Start) has same result as ((uint32) PoolPtr->Start).
        */
        HandleValid = FALSE;
    }
    
    return(HandleValid);
}
