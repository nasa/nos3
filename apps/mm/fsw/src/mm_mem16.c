/*************************************************************************
** File:
**   $Id: mm_mem16.c 1.15 2015/04/14 15:29:04EDT lwalling Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   Provides CFS Memory Manager functions that are used
**   for the conditionally compiled MM_MEM16 optional memory type.
**
**   $Log: mm_mem16.c  $
**   Revision 1.15 2015/04/14 15:29:04EDT lwalling 
**   Removed unnecessary backslash characters from string format definitions
**   Revision 1.14 2015/04/06 15:41:25EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.13 2015/04/02 09:10:56EDT lwalling 
**   Added Valid = TRUE for MM_LoadMem8/16/32FromFile()
**   Revision 1.12 2015/04/01 11:41:15EDT lwalling 
**   Add ptr type to ioBuffer name, fix code indentation, change if (=) to if (==)
**   Revision 1.11 2015/03/30 17:34:01EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.10 2015/03/20 14:16:12EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.9 2015/03/02 14:27:09EST sstrege 
**   Added copyright information
**   Revision 1.8 2011/12/05 15:17:21EST jmdagost 
**   Added check for zero bytes read from file load (with event message on error)
**   Revision 1.7 2011/06/23 12:02:39EDT jmdagost 
**   Corrected type for local BytesRead, initialized local variables.
**   Revision 1.6 2010/11/29 13:35:23EST jmdagost 
**   Replaced ifdef tests with if-true tests.
**   Revision 1.5 2009/06/12 14:37:27EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.4 2008/09/05 14:23:52EDT dahardison 
**   Updated references to local HK variables
**   Revision 1.3 2008/05/19 15:23:20EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_mem16.h"
#include "mm_app.h"
#include "mm_events.h"
#include "mm_utils.h"
#include <string.h>

/*
** The code in this file is optional.
** See mm_platform_cfg.h to set this compiler switch.
*/
#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)

/*************************************************************************
** External Data
*************************************************************************/
extern MM_AppData_t MM_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load memory from a file using only 16 bit wide writes           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_LoadMem16FromFile(uint32                   FileHandle, 
                             char                    *FileName,
                             MM_LoadDumpFileHeader_t *FileHeader, 
                             uint32                   DestAddress)
{
   uint32    i              = 0;
   int32     ReadLength     = 0;
   int32     PSP_Status     = CFE_PSP_SUCCESS;
   int32     BytesProcessed = 0;
   int32     BytesRemaining = FileHeader->NumOfBytes;
   uint16   *DataPointer16  = (uint16 *) DestAddress;
   uint16   *ioBuffer16     = (uint16 *) &MM_AppData.LoadBuffer[0];
   uint32    SegmentSize    = MM_MAX_LOAD_DATA_SEG;
   boolean   Valid          = FALSE;

   while (BytesRemaining != 0)
   {
      if (BytesRemaining < MM_MAX_LOAD_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      /* Read file data into i/o buffer */
      if ((ReadLength = OS_read(FileHandle, ioBuffer16, SegmentSize)) != SegmentSize)
      {
         MM_AppData.ErrCounter++;
         BytesRemaining = 0;
         CFE_EVS_SendEvent(MM_OS_READ_ERR_EID, CFE_EVS_ERROR,
                           "OS_read error received: RC = 0x%08X Expected = %d File = '%s'", 
                           ReadLength, SegmentSize, FileName);
      }
      else
      {
         /* Load memory from i/o buffer using 16 bit wide writes */
         for (i = 0; i < (SegmentSize / sizeof(uint16)); i++)
         {
            PSP_Status = CFE_PSP_MemWrite16((uint32) DataPointer16, ioBuffer16[i]);
            if (PSP_Status == CFE_PSP_SUCCESS)
            {
               DataPointer16++;
            }
            else
            {
               /* CFE_PSP_MemWrite16 error */
               BytesRemaining = 0;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR,
                                "PSP write memory error: RC=0x%08X, Address=0x%08X, MemType=MEM16", 
                                 PSP_Status, (uint32) DataPointer16);
               /* Stop load segment loop */
               break;
            }
         }

         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            BytesProcessed += SegmentSize;
            BytesRemaining -= SegmentSize;
               
            /* Prevent CPU hogging between load segments */
            if (BytesRemaining != 0)
            {
               MM_SegmentBreak();
            }
         }
      }      
   }
   
   /* Update last action statistics */
   if (BytesProcessed == FileHeader->NumOfBytes)
   {
      Valid = TRUE;
      MM_AppData.LastAction = MM_LOAD_FROM_FILE;
      MM_AppData.MemType    = MM_MEM16;
      MM_AppData.Address    = DestAddress;
      MM_AppData.BytesProcessed = BytesProcessed;
      strncpy(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN);
   }      

   return(Valid);   
    
} /* end MM_LoadMem16FromFile */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Dump the requested number of bytes from memory to a file using  */
/* only 16 bit wide reads                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_DumpMem16ToFile(uint32                    FileHandle, 
                           char                     *FileName, 
                           MM_LoadDumpFileHeader_t  *FileHeader)
{
   boolean   Valid          = TRUE;
   int32     OS_Status      = OS_ERROR;
   int32     PSP_Status     = CFE_PSP_SUCCESS;
   uint32    i              = 0;
   uint32    BytesProcessed = 0;
   uint32    BytesRemaining = FileHeader->NumOfBytes;
   uint16   *DataPointer16  = (uint16 *) FileHeader->SymAddress.Offset;
   uint16   *ioBuffer16     = (uint16 *) &MM_AppData.DumpBuffer[0];
   uint32    SegmentSize    = MM_MAX_DUMP_DATA_SEG;
   
   while (BytesRemaining != 0)
   {
      if (BytesRemaining < MM_MAX_DUMP_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      /* Load RAM data into i/o buffer */
      for (i = 0; i < SegmentSize; i++)
      {
         if ((PSP_Status = CFE_PSP_MemRead16((uint32) DataPointer16, &ioBuffer16[i])) == CFE_PSP_SUCCESS)
         {
            DataPointer16++;
         }
         else
         {
            /* CFE_PSP_MemRead16 error */
            Valid = FALSE;
            BytesRemaining = 0;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
               "PSP read memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Type=MEM16", 
                PSP_Status, (uint32) DataPointer16, (uint32) &ioBuffer16[i]);
            /* Stop load i/o buffer loop */
            break;
         }
      }

      /* Check for error loading i/o buffer */
      if (PSP_Status == CFE_PSP_SUCCESS)
      {
         /* Write i/o buffer contents to file */
         if ((OS_Status = OS_write(FileHandle, ioBuffer16, SegmentSize)) == SegmentSize)
         {
            /* Update process counters */
            BytesRemaining -= SegmentSize;         
            BytesProcessed += SegmentSize;         

            /* Prevent CPU hogging between dump segments */
            if (BytesRemaining != 0)
            {
               MM_SegmentBreak();
            }
         }
         else
         {
            /* OS_write error */
            Valid = FALSE;
            BytesRemaining = 0;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR,
                              "OS_write error received: RC = 0x%08X Expected = %d File = '%s'", 
                               OS_Status, SegmentSize, FileName);
         }
      }
   }

   if (Valid)
   {
       /* Update last action statistics */
       MM_AppData.LastAction = MM_DUMP_TO_FILE;
       MM_AppData.MemType    = MM_MEM16;
       MM_AppData.Address    = FileHeader->SymAddress.Offset;
       strncpy(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN);
       MM_AppData.BytesProcessed = BytesProcessed;
   }    

   return(Valid);

} /* end MM_DumpMem16ToFile */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Fill memory with the command specified fill pattern using only  */
/* 16 bit wide writes                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void MM_FillMem16(uint32           DestAddress, 
                  MM_FillMemCmd_t *CmdPtr)
{
   uint32    i              = 0;
   int32     PSP_Status     = CFE_PSP_SUCCESS;
   uint32    BytesProcessed = 0;
   uint32    BytesRemaining = CmdPtr->NumOfBytes;
   uint16    FillPattern16  = (uint16) CmdPtr->FillPattern;
   uint16   *DataPointer16  = (uint16 *) DestAddress;
   uint32    SegmentSize    = MM_MAX_FILL_DATA_SEG;

   while (BytesRemaining != 0)
   {
      /* Set size of next segment */
      if (BytesRemaining < MM_MAX_FILL_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      /* Fill next segment */
      for (i = 0; i < (SegmentSize / sizeof(uint16)); i++)
      {
         PSP_Status = CFE_PSP_MemWrite16((uint32) DataPointer16, FillPattern16);
         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            DataPointer16++;
         }
         else
         {
            /* CFE_PSP_MemWrite16 error */
            BytesRemaining = 0;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR,
                             "PSP write memory error: RC=0x%08X, Address=0x%08X, MemType=MEM16", 
                              PSP_Status, (uint32) DataPointer16);
            /* Stop fill segment loop */
            break;
         }
      }

      if (PSP_Status == CFE_PSP_SUCCESS)
      {
         /* Update process counters */
         BytesRemaining -= SegmentSize;         
         BytesProcessed += SegmentSize;         

         /* Prevent CPU hogging between fill segments */
         if (BytesRemaining != 0)
         {
            MM_SegmentBreak();
         }
      }
   }

   /* Update last action statistics */
   if (BytesProcessed == CmdPtr->NumOfBytes)
   {
      MM_AppData.LastAction = MM_FILL;
      MM_AppData.MemType    = MM_MEM16;
      MM_AppData.Address    = DestAddress;
      MM_AppData.DataValue  = (uint32) FillPattern16;
      MM_AppData.BytesProcessed = BytesProcessed;
   }

   return;
   
}/* End MM_FillMem16 */


#endif /* MM_OPT_CODE_MEM16_MEMTYPE */

/************************/
/*  End of File Comment */
/************************/
