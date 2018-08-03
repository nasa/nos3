/*************************************************************************
** File:
**   $Id: mm_dump.c 1.15 2015/04/14 15:29:03EDT lwalling Exp  $
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
**   Functions used for processing CFS Memory Manager memory dump commands
**
**   $Log: mm_dump.c  $
**   Revision 1.15 2015/04/14 15:29:03EDT lwalling 
**   Removed unnecessary backslash characters from string format definitions
**   Revision 1.14 2015/04/06 15:41:25EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.13 2015/03/30 17:34:00EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.12 2015/03/20 14:16:25EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.11 2015/03/02 14:26:57EST sstrege 
**   Added copyright information
**   Revision 1.10 2010/11/29 13:35:23EST jmdagost 
**   Replaced ifdef tests with if-true tests.
**   Revision 1.9 2010/05/26 15:22:46EDT jmdagost 
**   In function MM_FillDumpInEventBuffer, put local variable declaration in a pre-processor conditional.
**   Revision 1.8 2009/06/18 10:17:11EDT rmcgraw 
**   DCR8291:1 Changed OS_MEM_ #defines to CFE_PSP_MEM_
**   Revision 1.7 2009/06/10 14:04:22EDT rmcgraw 
**   DCR82191:1 Changed os_bsp to cfe_psp and OS_Mem to CFE_PSP_Mem
**   Revision 1.6 2008/09/05 14:24:09EDT dahardison 
**   Updated references to local HK variables
**   Revision 1.5 2008/09/05 13:14:45EDT dahardison 
**   Added inclusion of mm_mission_cfg.h
**   Revision 1.4 2008/05/22 15:09:30EDT dahardison 
**   Changed inclusion of cfs_lib.h to cfs_utils.h
**   Revision 1.3 2008/05/19 15:22:59EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"
#include "mm_dump.h"
#include "mm_events.h"
#include "mm_mem32.h"
#include "mm_mem16.h"
#include "mm_mem8.h"
#include "mm_utils.h"
#include "mm_mission_cfg.h"
#include "cfs_utils.h"
#include <string.h>

/*************************************************************************
** External Data
*************************************************************************/
extern MM_AppData_t MM_AppData; 

/*************************************************************************
** Local Function Prototypes
*************************************************************************/
/************************************************************************/
/** \brief Memory peek
**  
**  \par Description
**       Support function for #MM_PeekCmd. This routine will read 
**       8, 16, or 32 bits of data and send it in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   CmdPtr       A #MM_PeekCmd_t pointer to the peek 
**                             command message
**
**  \param [in]   SrcAddress   The source address for the peek operation 
** 
*************************************************************************/
void MM_PeekMem (MM_PeekCmd_t *CmdPtr, 
                 uint32       SrcAddress);

/************************************************************************/
/** \brief Memory dump to file
**  
**  \par Description
**       Support function for #MM_DumpMemToFileCmd. This routine will 
**       read an address range and store the data in a file.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileHandle   The open file handle of the dump file  
**
**  \param [in]   FileName     A pointer to a character string holding  
**                             the dump file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to  
**                             the dump file header structure initialized
**                             with data based upon the command message 
**                             parameters
**
**  \returns
**  \retstmt Returns TRUE if the dump completed successfully  \endcode
**  \retstmt Returns FALSE if the dump failed due to an error \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_DumpMemToFile(uint32                   FileHandle, 
                         char                     *FileName, 
                         MM_LoadDumpFileHeader_t  *FileHeader);

/************************************************************************/
/** \brief Verify memory dump to file parameters
**  
**  \par Description
**       This routine will run various checks on the source address, 
**       memory type, and data size (in bytes) for a dump memory to
**       file command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The source address for the requested 
**                              dump operation 
**
**  \param [in]   MemType       The source memory type for the requested 
**                              dump operation  
**
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              dump operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyFileDumpParams(uint32 Address, 
                                uint8  MemType, 
                                uint32 SizeInBytes);

/************************************************************************/
/** \brief Write the cFE primary and and MM secondary file headers
**  
**  \par Description
**       Support function for #MM_DumpMemToFileCmd. This routine will 
**       write the cFE primary and MM secondary headers to the
**       file specified by the FileHandle.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileName     A pointer to a character string holding  
**                             the file name (used only for error event
**                             messages).
**
**  \param [in]   FileHandle   File Descriptor obtained from a previous
**                             call to #OS_open that is associated with
**                             the file whose headers are to be written.
**
**  \param [in]   CFEHeader    A #CFE_FS_Header_t pointer to the
**                             cFE primary file header structure to be
**                             written.
**
**  \param [in]   MMHeader     A #MM_LoadDumpFileHeader_t pointer to 
**                             the MM secondary file header structure
**                             to be written.
**
**  \returns
**  \retstmt Returns TRUE if the headers were written successfully \endcode
**  \retstmt Returns FALSE if a write error occurred \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_WriteFileHeaders(char                    *FileName,
                            int32                    FileHandle,
                            CFE_FS_Header_t         *CFEHeader,
                            MM_LoadDumpFileHeader_t *MMHeader);

/************************************************************************/
/** \brief Verify memory dump in event message parameters
**  
**  \par Description
**       This routine will run various checks on the source address, 
**       memory type, and data size (in bytes) for a dump memory in
**       event message command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The source address for the requested 
**                              dump operation 
**
**  \param [in]   MemType       The source memory type for the requested 
**                              dump operation  
**
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              dump operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyDumpInEventParams(uint32 Address, 
                                   uint8  MemType, 
                                   uint32 SizeInBytes);

/************************************************************************/
/** \brief Fill dump memory in event message buffer
**  
**  \par Description
**       Support function for #MM_DumpInEventCmd. This routine will 
**       read an address range and store the data in a byte array.
**       It will properly adjust for optional memory types that may
**       require 16 or 32 bit reads.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   SrcAddress   The source address to read from 
**
**  \param [in]   CmdPtr       A #MM_DumpInEventCmd_t pointer to the  
**                             dump in event command message
**
**  \param [in]   DumpBuffer   A pointer to the byte array to store
**                             the dump data in
**
**  \param [out]  *DumpBuffer  A pointer to the byte array holding the
**                             dump data
**
**  \returns
**  \retstmt Returns TRUE if all PSP memory access functions succeed  \endcode
**  \retstmt Returns FALSE if any PSP memory access function fails    \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_FillDumpInEventBuffer(uint32              SrcAddress, 
                                 MM_DumpInEventCmd_t *CmdPtr, 
                                 uint8               *DumpBuffer);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Memory peek command                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void MM_PeekCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   boolean        Valid = TRUE;
   MM_PeekCmd_t   *CmdPtr;
   uint32         SrcAddress;
   uint16         ExpectedLength = sizeof(MM_PeekCmd_t);
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_PeekCmd_t *)MessagePtr);
 
      /* Resolve the symbolic address in command message */
      Valid = CFS_ResolveSymAddr(&(CmdPtr->SrcSymAddress), &SrcAddress);

      if(Valid == TRUE)
      {
         /* Run necessary checks on command parameters */
         Valid = MM_VerifyPeekPokeParams(SrcAddress, CmdPtr->MemType, CmdPtr->DataSize);

         /* Check the specified memory type and call the appropriate routine */
         if(Valid == TRUE)
         {
            /* 
            ** We use this single peek routine for all memory types
            ** (including the optional ones)
            */ 
            MM_PeekMem(CmdPtr, SrcAddress);
         }
         
      } /* end CFS_ResolveSymAddr if */
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                           "Symbolic address can't be resolved: Name = '%s'", 
                           CmdPtr->SrcSymAddress.SymName);   
      }

   } /* end MM_VerifyCmdLength if */
   
   return;
   
} /* end MM_PeekCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Read 8,16, or 32 bits of data from any given input address      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void MM_PeekMem (MM_PeekCmd_t *CmdPtr, 
                 uint32       SrcAddress)
{
   boolean  ValidPeek = TRUE;
   uint8    ByteValue = 0;
   uint16   WordValue = 0;
   uint32   DWordValue = 0;
   int32    PSP_Status = 0;
   uint32   BytesProcessed = 0;
   uint32   DataValue = 0;

   /* 
   ** Read the requested number of bytes and report in an event message 
   */
   switch(CmdPtr->DataSize)
   {
      case MM_BYTE_BIT_WIDTH:

         PSP_Status = CFE_PSP_MemRead8(SrcAddress, &ByteValue);
         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            DataValue = (uint32) ByteValue;
            BytesProcessed = sizeof (uint8);
            CFE_EVS_SendEvent(MM_PEEK_BYTE_INF_EID, CFE_EVS_INFORMATION,
               "Peek Command: Addr = 0x%08X Size = 8 bits Data = 0x%02X", 
                SrcAddress, ByteValue);
         }
         else
         {
            ValidPeek = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
               "PSP read memory error: RC=0x%08X, Address=0x%08X, MemType=MEM8", 
                PSP_Status, SrcAddress);
         }
         break;
         
      case MM_WORD_BIT_WIDTH:

         PSP_Status = CFE_PSP_MemRead16(SrcAddress, &WordValue);
         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            DataValue = (uint32) WordValue;
            BytesProcessed = sizeof (uint16);
            CFE_EVS_SendEvent(MM_PEEK_WORD_INF_EID, CFE_EVS_INFORMATION,
               "Peek Command: Addr = 0x%08X Size = 16 bits Data = 0x%04X",
                SrcAddress, DataValue);
         }
         else
         {
            ValidPeek = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
               "PSP read memory error: RC=0x%08X, Address=0x%08X, MemType=MEM16", 
                PSP_Status, SrcAddress);
         }
         break;
         
      case MM_DWORD_BIT_WIDTH:

         PSP_Status = CFE_PSP_MemRead32(SrcAddress, &DWordValue);
         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            DataValue = DWordValue;
            BytesProcessed = sizeof(uint32);
            CFE_EVS_SendEvent(MM_PEEK_DWORD_INF_EID, CFE_EVS_INFORMATION,
               "Peek Command: Addr = 0x%08X Size = 32 bits Data = 0x%08X", 
                SrcAddress, DataValue);
         }
         else
         {
            ValidPeek = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
               "PSP read memory error: RC=0x%08X, Address=0x%08X, MemType=MEM32", 
                PSP_Status, SrcAddress);
         }
         break;
         
      /* 
      ** We don't need a default case, a bad DataSize will get caught
      ** in the MM_VerifyPeekPokeParams function and we won't get here
      */
      default:
         break;
   }

   if (ValidPeek)
   {
      MM_AppData.CmdCounter++;
      MM_AppData.LastAction  = MM_PEEK;
      MM_AppData.MemType     = CmdPtr->MemType;
      MM_AppData.Address     = SrcAddress;
      MM_AppData.BytesProcessed = BytesProcessed;
      MM_AppData.DataValue   = DataValue;
   }

   return;
   
} /* end MM_PeekMem */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Dump memory to file comand                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_DumpMemToFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   boolean                  Valid = TRUE;
   int32                    OS_Status = OS_SUCCESS;
   int32                    FileHandle;
   uint32                   SrcAddress = 0;
   MM_DumpMemToFileCmd_t    *CmdPtr;
   CFE_FS_Header_t          CFEFileHeader;
   MM_LoadDumpFileHeader_t  MMFileHeader;
   uint16                   ExpectedLength = sizeof(MM_DumpMemToFileCmd_t);
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_DumpMemToFileCmd_t *)MessagePtr);

      /* 
      ** NUL terminate the very end of the file name string array as a
      ** safety measure
      */
      CmdPtr->FileName[OS_MAX_PATH_LEN - 1] = '\0';
      
      /* Verify filename doesn't have any illegal characters */
      Valid = CFS_IsValidFilename(CmdPtr->FileName, strlen(CmdPtr->FileName));
      if(Valid == TRUE)
      {
         /* Resolve the symbolic address in command message */
         Valid = CFS_ResolveSymAddr(&(CmdPtr->SrcSymAddress), &SrcAddress);

         if(Valid == TRUE)
         {
            /* Run necessary checks on command parameters */ 
            Valid = MM_VerifyFileDumpParams(SrcAddress, CmdPtr->MemType, CmdPtr->NumOfBytes);
            
            if(Valid == TRUE)
            {
               /*
               ** Initialize the cFE primary file header structure 
               */
               CFE_PSP_MemSet(&CFEFileHeader, 0, sizeof(CFE_FS_Header_t));
               CFEFileHeader.SubType = MM_CFE_HDR_SUBTYPE;
               strcpy(&CFEFileHeader.Description[0], MM_CFE_HDR_DESCRIPTION);
        
               /* 
               ** Initialize the MM secondary file header structure
               */
               CFE_PSP_MemSet(&MMFileHeader, 0, sizeof(MM_LoadDumpFileHeader_t));
               MMFileHeader.SymAddress.SymName[0] = MM_CLEAR_SYMNAME;       
               
               /*
               ** Copy command data to file secondary header 
               */ 
               MMFileHeader.SymAddress.Offset = SrcAddress;
               MMFileHeader.MemType           = CmdPtr->MemType;
               MMFileHeader.NumOfBytes        = CmdPtr->NumOfBytes;

               /* 
               ** Create and open dump file 
               */ 
               if((FileHandle = OS_creat(CmdPtr->FileName, OS_READ_WRITE)) >= 0)
               {
                  /* Write the file headers */
                  Valid = MM_WriteFileHeaders(CmdPtr->FileName, FileHandle, &CFEFileHeader, &MMFileHeader);
                  if(Valid == TRUE)
                  {
                     switch(MMFileHeader.MemType)
                     {
                        case MM_RAM:
                        case MM_EEPROM:
                           Valid = MM_DumpMemToFile(FileHandle, CmdPtr->FileName, &MMFileHeader);
                           break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
                        case MM_MEM32:
                           Valid = MM_DumpMem32ToFile(FileHandle, CmdPtr->FileName, &MMFileHeader);
                           break;
#endif /* MM_OPT_CODE_MEM32_MEMTYPE */
                           
#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
                        case MM_MEM16:
                           Valid = MM_DumpMem16ToFile(FileHandle, CmdPtr->FileName, &MMFileHeader);
                           break;
#endif /* MM_OPT_CODE_MEM16_MEMTYPE */
                           
#if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
                        case MM_MEM8:
                           Valid = MM_DumpMem8ToFile(FileHandle, CmdPtr->FileName, &MMFileHeader);
                           break;
#endif /* MM_OPT_CODE_MEM8_MEMTYPE */                           
                     }
                     
                     if(Valid == TRUE)
                     {
                        /* 
                        ** Compute CRC of dumped data 
                        */
                        OS_lseek(FileHandle, (sizeof(CFE_FS_Header_t) + sizeof(MM_LoadDumpFileHeader_t)), OS_SEEK_SET);

                        OS_Status = CFS_ComputeCRCFromFile(FileHandle, &MMFileHeader.Crc, MM_DUMP_FILE_CRC_TYPE);
                        if(OS_Status == OS_SUCCESS)
                        {
                           /*
                           ** Rewrite the file headers. The subfunctions will take care of moving
                           ** the file pointer to the beginning of the file so we don't need to do it
                           ** here.
                           */
                           Valid = MM_WriteFileHeaders(CmdPtr->FileName, FileHandle, &CFEFileHeader, &MMFileHeader);
                           
                        } /* end CFS_ComputeCRCFromFile if */
                        else
                        {
                           MM_AppData.ErrCounter++;
                           CFE_EVS_SendEvent(MM_CFS_COMPUTECRCFROMFILE_ERR_EID, CFE_EVS_ERROR,
                                       "CFS_ComputeCRCFromFile error received: RC = 0x%08X File = '%s'", OS_Status, 
                                                                                               CmdPtr->FileName);
                        } 
                        
                     } /* end Valid == TRUE if */

                     if(Valid == TRUE)
                     {
                        MM_AppData.CmdCounter++;
                        CFE_EVS_SendEvent(MM_DMP_MEM_FILE_INF_EID, CFE_EVS_INFORMATION,
                                "Dump Memory To File Command: Dumped %d bytes from address 0x%08X to file '%s'", 
                                MM_AppData.BytesProcessed, SrcAddress, CmdPtr->FileName);
                        /* 
                        ** Update last action statistics
                        */
                        MM_AppData.LastAction = MM_DUMP_TO_FILE;
                        strncpy(MM_AppData.FileName, CmdPtr->FileName, OS_MAX_PATH_LEN);
                        MM_AppData.MemType = CmdPtr->MemType;
                        MM_AppData.Address = SrcAddress;
                        MM_AppData.BytesProcessed = CmdPtr->NumOfBytes;
                     }
                     
                  } /* end MM_WriteFileHeaders if */   

                  /*
                  ** Don't need an 'else' here. MM_WriteFileHeaders will increment
                  ** the error counter and generate an event message if needed.
                  */
                  
                  /* Close dump file */
                  if((OS_Status = OS_close(FileHandle)) != OS_SUCCESS)
                  {
                     MM_AppData.ErrCounter++;
                     CFE_EVS_SendEvent(MM_OS_CLOSE_ERR_EID, CFE_EVS_ERROR,
                                       "OS_close error received: RC = 0x%08X File = '%s'", 
                                                           OS_Status, CmdPtr->FileName);
                  } 
                  
               } /* end OS_creat if */
               else
               {
                  MM_AppData.ErrCounter++;
                  CFE_EVS_SendEvent(MM_OS_CREAT_ERR_EID, CFE_EVS_ERROR,
                                    "OS_creat error received: RC = 0x%08X File = '%s'", 
                                                       FileHandle, CmdPtr->FileName);
               }
            
            } /* end MM_VerifyFileDumpParams if */
            
         } /* end CFS_ResolveSymAddr if */
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                              "Symbolic address can't be resolved: Name = '%s'", 
                              CmdPtr->SrcSymAddress.SymName);   
         }
      } /* end IsValidFilename if */
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_CMD_FNAME_ERR_EID, CFE_EVS_ERROR,
                           "Command specified filename invalid: Name = '%s'", CmdPtr->FileName);
      }
      
   } /* end MM_VerifyCmdLength if */
    
   return;
    
} /* end MM_DumpMemoryToFileCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Dump the requested number of bytes from memory to a file        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_DumpMemToFile(uint32                  FileHandle, 
                         char                    *FileName, 
                         MM_LoadDumpFileHeader_t *FileHeader)
{
   boolean    ValidDump = FALSE;
   int32      OS_Status;
   int32      PSP_Status;
   uint32     BytesRemaining = FileHeader->NumOfBytes;
   uint32     BytesProcessed = 0;
   uint32     SegmentSize = MM_MAX_DUMP_DATA_SEG;
   uint8     *SourcePtr = (uint8 *)FileHeader->SymAddress.Offset;
   uint8     *ioBuffer = (uint8 *) &MM_AppData.DumpBuffer[0];

   while (BytesRemaining != 0)
   {
      if (BytesRemaining < MM_MAX_DUMP_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      PSP_Status = CFE_PSP_MemCpy(ioBuffer, SourcePtr, SegmentSize);
      if (PSP_Status == CFE_PSP_SUCCESS)
      {
         OS_Status = OS_write(FileHandle, ioBuffer, SegmentSize);
         if (OS_Status == SegmentSize)
         {
            SourcePtr += SegmentSize;
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
            BytesRemaining = 0;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR,
               "OS_write error received: RC = 0x%08X, Expected = %d, File = '%s'", 
                OS_Status, BytesRemaining, FileName);
         }
      }
      else
      {
         BytesRemaining = 0;
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR,
            "PSP copy memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Size=0x%08X", 
             PSP_Status, (uint32) SourcePtr, (uint32) ioBuffer, SegmentSize);
      }
   }

   /* Update last action statistics */
   if (BytesProcessed == FileHeader->NumOfBytes)
   {
      ValidDump = TRUE;
      MM_AppData.LastAction = MM_DUMP_TO_FILE;
      MM_AppData.MemType    = FileHeader->MemType;
      MM_AppData.Address    = FileHeader->SymAddress.Offset;
      MM_AppData.BytesProcessed = BytesProcessed;
      strncpy(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN);
   }      
    
   return(ValidDump);
    
} /* end MM_DumpMemToFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify dump memory to file command parameters                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyFileDumpParams(uint32 Address, 
                                uint8  MemType, 
                                uint32 SizeInBytes)
{
   boolean  Valid = TRUE;
   int32    OS_Status;
   
   switch(MemType)
   {
      case MM_RAM:
         OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (OS_Status != OS_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                        "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_FILE_DATA_RAM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;
         
      case MM_EEPROM:
         OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_EEPROM);
         
         if (OS_Status != OS_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                        "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        OS_Status, Address, SizeInBytes, CFE_PSP_MEM_EEPROM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_FILE_DATA_EEPROM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
      case MM_MEM32:
         OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (OS_Status != OS_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                        "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_FILE_DATA_MEM32))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         else if (CFS_Verify32Aligned(Address, SizeInBytes) != TRUE)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR,
                     "Data and address not 32 bit aligned: Addr = 0x%08X Size = %d", 
                                                              Address, SizeInBytes);
         }
         break;
#endif /* MM_OPT_CODE_MEM32_MEMTYPE */

#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
      case MM_MEM16:
         OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (OS_Status != OS_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                        "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_FILE_DATA_MEM16))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         else if (CFS_Verify16Aligned(Address, SizeInBytes) != TRUE)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR,
                     "Data and address not 16 bit aligned: Addr = 0x%08X Size = %d", 
                                                              Address, SizeInBytes);
         }
         break;
#endif /* MM_OPT_CODE_MEM16_MEMTYPE */
         
#if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
      case MM_MEM8:
         OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (OS_Status != OS_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                        "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_FILE_DATA_MEM8))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;
#endif /* MM_OPT_CODE_MEM8_MEMTYPE */
         
      default:
         Valid = FALSE;
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR,
                           "Invalid memory type specified: MemType = %d", MemType);
         break;
   }

   return (Valid);

} /* end MM_VerifyFileDumpParams */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Write the cFE primary and and MM secondary file headers         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_WriteFileHeaders(char                    *FileName,
                            int32                    FileHandle,
                            CFE_FS_Header_t         *CFEHeader,
                            MM_LoadDumpFileHeader_t *MMHeader)
{
   boolean     Valid = TRUE;
   int32       OS_Status;

   /*
   ** Write out the primary cFE file header
   */
   OS_Status = CFE_FS_WriteHeader(FileHandle, CFEHeader);
   if(OS_Status != sizeof(CFE_FS_Header_t))
   {
      /* We either got an error or didn't write as much data as expected */
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_CFE_FS_WRITEHDR_ERR_EID, CFE_EVS_ERROR,
                        "CFE_FS_WriteHeader error received: RC = 0x%08X Expected = %d File = '%s'", 
                        OS_Status, sizeof(CFE_FS_Header_t), FileName); 


   } /* end CFE_FS_WriteHeader if */
   else             
   {  
      /*
      ** Write out the secondary MM file header 
      */
      OS_Status = OS_write(FileHandle, MMHeader, sizeof(MM_LoadDumpFileHeader_t));
      if(OS_Status != sizeof(MM_LoadDumpFileHeader_t))
      {
         /* We either got an error or didn't read as much data as expected */
         Valid = FALSE;
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_OS_WRITE_EXP_ERR_EID, CFE_EVS_ERROR,
                           "OS_write error received: RC = 0x%08X Expected = %d File = '%s'", 
                           OS_Status, sizeof(MM_LoadDumpFileHeader_t), FileName); 

      } /* end OS_write if */
      
   } /* end CFE_FS_WriteHeader else */

   return (Valid);
   
} /* end MM_WriteFileHeaders */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Dump memory in event message command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void MM_DumpInEventCmd(CFE_SB_MsgPtr_t MessagePtr)
{
       boolean               Valid = TRUE;
       MM_DumpInEventCmd_t   *CmdPtr;
       uint32                i;
       uint32                SrcAddress;
       uint16                ExpectedLength = sizeof(MM_DumpInEventCmd_t);
       uint8                 *BytePtr;
       char                  TempString[MM_DUMPINEVENT_TEMP_CHARS];  
static char                  EventString[CFE_EVS_MAX_MESSAGE_LENGTH];

   /*
   ** Allocate a dump buffer. It's declared this way to ensure it stays
   ** longword aligned since MM_MAX_DUMP_INEVENT_BYTES can be adjusted
   ** by changing the maximum event message string size.
   */
   uint32         DumpBuffer[(MM_MAX_DUMP_INEVENT_BYTES + 3)/4];
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_DumpInEventCmd_t *)MessagePtr);

      /* Resolve the symbolic source address in the command message */
      Valid = CFS_ResolveSymAddr(&(CmdPtr->SrcSymAddress), &SrcAddress);

      if(Valid == TRUE)
      {
         /* Run necessary checks on command parameters */ 
         Valid = MM_VerifyDumpInEventParams(SrcAddress, CmdPtr->MemType, CmdPtr->NumOfBytes);
        
         if(Valid == TRUE)
         {
            /* Fill a local data buffer with the dump words */
            Valid = MM_FillDumpInEventBuffer(SrcAddress, CmdPtr, (uint8 *)DumpBuffer);

            if(Valid == TRUE)
            {
               /* 
               ** Prepare event message string header
               ** 13 characters, not counting NUL terminator 
               */
               strcpy(EventString, "Memory Dump: ");

               /* 
               ** Build dump data string
               ** Each byte of data requires 5 characters of string space 
               */
               BytePtr = (uint8 *)DumpBuffer;
               for (i = 0; i < CmdPtr->NumOfBytes; i++)            
               {
                  sprintf(TempString, "0x%02X ", *BytePtr); 
                  strcat(EventString, TempString);
                  BytePtr++;
               }

               /* 
               ** Append tail
               ** This adds 25 characters including the NUL terminator 
               */
               sprintf(TempString, "from address: 0x%08lX", SrcAddress); 
               strcat(EventString, TempString);

               /* Send it out */
               CFE_EVS_SendEvent(MM_DUMP_INEVENT_INF_EID, CFE_EVS_INFORMATION,
                                                              EventString);
               /* Update telemetry */
               MM_AppData.LastAction = MM_DUMP_INEVENT;
               MM_AppData.MemType    = CmdPtr->MemType;
               MM_AppData.Address    = SrcAddress;
               MM_AppData.BytesProcessed = CmdPtr->NumOfBytes;
               MM_AppData.CmdCounter++;
            } /* end MM_FillDumpInEventBuffer if */
         } /* end MM_VerifyDumpInEventParams if */
      } /* end CFS_ResolveSymAddr if */
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                           "Symbolic address can't be resolved: Name = '%s'", 
                           CmdPtr->SrcSymAddress.SymName);   
      }
      
   } /* end MM_VerifyCmdLength if */
   
   return;
   
} /* end MM_DumpWordsInEventCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify dump memory in event messsage command parameters         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyDumpInEventParams(uint32 Address, 
                                   uint8  MemType, 
                                   uint32 SizeInBytes)
{
   boolean   Valid = TRUE;
   int32     OS_Status;
   
   /*
   ** Verify dump size is within limits. The limit is dictated by the
   ** maximum event message string length and applies to all memory 
   ** types
   */   
   if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_DUMP_INEVENT_BYTES))
   {
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                  "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
   }
   else
   {
      /*
      ** Run a bunch of other sanity checks
      */
      switch(MemType)
      {
         case MM_RAM:
            OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
            
            if (OS_Status != OS_SUCCESS)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                           "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                           OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
            }
            break;
            
         case MM_EEPROM:
            OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_EEPROM);
            
            if (OS_Status != OS_SUCCESS)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                           "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                           OS_Status, Address, SizeInBytes, CFE_PSP_MEM_EEPROM); 
            }
            break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
         case MM_MEM32:
            OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
            
            if (OS_Status != OS_SUCCESS)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                           "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                           OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
            }
            else if (CFS_Verify32Aligned(Address, SizeInBytes) != TRUE)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR,
                        "Data and address not 32 bit aligned: Addr = 0x%08X Size = %d",
                                                                 Address, SizeInBytes);
            }
            break;
#endif /* MM_OPT_CODE_MEM32_MEMTYPE */

#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
         case MM_MEM16:
            OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
            
            if (OS_Status != OS_SUCCESS)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                           "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                           OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
            }
            else if (CFS_Verify16Aligned(Address, SizeInBytes) != TRUE)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR,
                        "Data and address not 16 bit aligned: Addr = 0x%08X Size = %d",
                                                                 Address, SizeInBytes);
            }
            break;
#endif /* MM_OPT_CODE_MEM16_MEMTYPE */

#if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
         case MM_MEM8:
            OS_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
            
            if (OS_Status != OS_SUCCESS)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                           "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                           OS_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
            }
            break;
            
         default:
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR,
                              "Invalid memory type specified: MemType = %d", MemType);
            break;
#endif /* MM_OPT_CODE_MEM8_MEMTYPE */            
      }
  
   } /* end SizeInBytes else */
   
   return (Valid); 
   
} /* end MM_VerifyDumpInEventParams */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Fill a buffer with data to be dumped in an event message string */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_FillDumpInEventBuffer(uint32              SrcAddress, 
                                 MM_DumpInEventCmd_t *CmdPtr, 
                                 uint8               *DumpBuffer)
{
#if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE) || (MM_OPT_CODE_MEM16_MEMTYPE == TRUE) || (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)   
   uint32    i;
#endif
   int32     PSP_Status;
   boolean   Valid = TRUE;

   /* Initialize buffer */
   PSP_Status = CFE_PSP_MemSet(DumpBuffer, 0, MM_MAX_DUMP_INEVENT_BYTES);
   
   switch (CmdPtr->MemType)
   {
      case MM_RAM:
      case MM_EEPROM:
         PSP_Status = CFE_PSP_MemCpy((void *)DumpBuffer, (void *)SrcAddress, CmdPtr->NumOfBytes);
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            CFE_EVS_SendEvent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR,
               "PSP copy memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Size=0x%08X", 
                PSP_Status, SrcAddress, (uint32) DumpBuffer, CmdPtr->NumOfBytes);
            MM_AppData.ErrCounter++;
            Valid = FALSE;
         }
         break;

      #if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
      case MM_MEM32:
         for (i = 0; i < (CmdPtr->NumOfBytes / 4); i++)
         {
            PSP_Status = CFE_PSP_MemRead32(SrcAddress, (uint32 *)DumpBuffer);
            if (PSP_Status == CFE_PSP_SUCCESS)
            {
               SrcAddress += sizeof (uint32);
               DumpBuffer += sizeof (uint32);
            }
            else
            {
               /* CFE_PSP_MemRead32 error */
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
                  "PSP read memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Type=MEM32", 
                   PSP_Status, SrcAddress, (uint32) DumpBuffer);
               /* Stop load dump buffer loop */
               break;
            }
         }
         break;
      #endif /* MM_OPT_CODE_MEM32_MEMTYPE */
         
      #if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
      case MM_MEM16:
         for (i = 0; i < (CmdPtr->NumOfBytes / 2); i++)
         {
            PSP_Status = CFE_PSP_MemRead16(SrcAddress, (uint16 *)DumpBuffer);
            if (PSP_Status == CFE_PSP_SUCCESS)
            {
               SrcAddress += sizeof (uint16);
               DumpBuffer += sizeof (uint16);
            }
            else
            {
               /* CFE_PSP_MemRead16 error */
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
                  "PSP read memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Type=MEM16", 
                   PSP_Status, SrcAddress, (uint32) DumpBuffer);
               /* Stop load dump buffer loop */
               break;
            }
         }
         break;
      #endif /* MM_OPT_CODE_MEM16_MEMTYPE */
         
      #if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
      case MM_MEM8:
         for (i = 0; i < CmdPtr->NumOfBytes; i++)
         {
            PSP_Status = CFE_PSP_MemRead8(SrcAddress, DumpBuffer);
            if (PSP_Status == CFE_PSP_SUCCESS)
            {
               SrcAddress ++;
               DumpBuffer ++;
            }
            else
            {
               /* CFE_PSP_MemRead8 error */
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_PSP_READ_ERR_EID, CFE_EVS_ERROR,
                  "PSP read memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Type=MEM8", 
                   PSP_Status, SrcAddress, (uint32) DumpBuffer);
               /* Stop load dump buffer loop */
               break;
            }
         }
         break;
      #endif /* MM_OPT_CODE_MEM8_MEMTYPE */

   } /* end CmdPtr->MemType switch */
   
   return(Valid);
   
} /* end FillDumpInEventBuffer */

/************************/
/*  End of File Comment */
/************************/
