/*************************************************************************
** File:
**   $Id: mm_load.c 1.21 2015/04/14 15:29:00EDT lwalling Exp  $
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
**   Provides functions for the execution of the CFS Memory Manager 
**   load and fill ground commands
**
**   $Log: mm_load.c  $
**   Revision 1.21 2015/04/14 15:29:00EDT lwalling 
**   Removed unnecessary backslash characters from string format definitions
**   Revision 1.20 2015/04/07 10:16:04EDT lwalling 
**   Report bytes processed for RAM and EEPROM fill commands
**   Revision 1.19 2015/04/06 15:41:15EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.18 2015/03/30 17:33:57EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.17 2015/03/20 14:30:18EDT lwalling 
**   Remove unnecessary include of osapi-os-filesys.h
**   Revision 1.16 2015/03/20 14:16:53EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.15 2015/03/02 14:27:05EST sstrege 
**   Added copyright information
**   Revision 1.14 2011/12/05 15:17:19EST jmdagost 
**   Added check for zero bytes read from file load (with event message on error)
**   Revision 1.13 2010/11/29 13:35:20EST jmdagost 
**   Replaced ifdef tests with if-true tests.
**   Revision 1.12 2010/11/29 08:48:40EST jmdagost 
**   Removed in-line calls to enable/disable EEPROM bank writes (now done via command)
**   Revision 1.11 2009/07/31 12:28:08EDT jmdagost 
**   Modified calls to EepromWrite enable and disable to include temporary argument of bank 0.  This allows us to
**   use the updated PSP functions.
**   Revision 1.10 2009/07/23 07:41:20EDT wmoleski 
**   Updating the OS_EepromWrite32 call to CFE_PSP_EepromWrite32
**   Revision 1.9 2009/07/22 10:05:40EDT wmoleski 
**   OS_EepromWritexxx calls changed to CFE_PSP_EepromWritexxx calls for cFE 6.0.0
**   Revision 1.8 2009/06/18 10:17:11EDT rmcgraw 
**   DCR8291:1 Changed OS_MEM_ #defines to CFE_PSP_MEM_
**   Revision 1.7 2009/06/12 14:37:32EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.6 2008/09/05 14:24:24EDT dahardison 
**   Updated references to local HK variables
**   Revision 1.5 2008/09/05 13:14:40EDT dahardison 
**   Added inclusion of mm_mission_cfg.h
**   Revision 1.4 2008/05/22 15:13:21EDT dahardison 
**   Changed inclusion of cfs_lib.h to cfs_utils.h
**   Revision 1.3 2008/05/19 15:23:15EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"
#include "mm_load.h"
#include "mm_perfids.h"
#include "mm_events.h"
#include "mm_utils.h"
#include "mm_mem32.h"
#include "mm_mem16.h"
#include "mm_mem8.h"
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
/** \brief Memory poke
**  
**  \par Description
**       Support function for #MM_PokeCmd. This routine will write 
**       8, 16, or 32 bits of data to a single ram address.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   CmdPtr        A #MM_PokeCmd_t pointer to the poke 
**                              command message
**
**  \param [in]   DestAddress   The destination address for the poke 
**                              operation
**
*************************************************************************/
void MM_PokeMem(MM_PokeCmd_t *CmdPtr, 
                uint32       DestAddress);

/************************************************************************/
/** \brief Eeprom poke
**  
**  \par Description
**       Support function for #MM_PokeCmd. This routine will write 
**       8, 16, or 32 bits of data to a single EEPROM address.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   CmdPtr        A #MM_PokeCmd_t pointer to the poke 
**                              command message
**
**  \param [in]   DestAddress   The destination address for the poke 
**                              operation
**
*************************************************************************/
void MM_PokeEeprom (MM_PokeCmd_t *CmdPtr, 
                    uint32       DestAddress);

/************************************************************************/
/** \brief Load memory with interrupts disabled
**  
**  \par Description
**       Support function for #MM_LoadMemWIDCmd. This routine will 
**       load up to #MM_MAX_UNINTERRUPTABLE_DATA bytes into
**       ram with interrupts disabled during the actual load
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   CmdPtr        A #MM_LoadMemWIDCmd_t pointer to the 
**                              command message
**
**  \param [in]   DestAddress   The destination address for the load 
**                              operation
**
*************************************************************************/
boolean MM_LoadMemWID(MM_LoadMemWIDCmd_t *CmdPtr, 
                      uint32             DestAddress);

/************************************************************************/
/** \brief Verify load memory with interrupts disabled parameters
**  
**  \par Description
**       This routine will run various checks on the destination address 
**       and data size (in bytes) for a load memory with interrupts
**       disabled command.
**
**  \par Assumptions, External Events, and Notes:
**       This command is only valid for ram addresses so no 
**       memory type checks are performed
**       
**  \param [in]   Address       The destination address for the requested 
**                              load operation 
**
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              load operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyLoadWIDParams(uint32 Address, 
                               uint32 SizeInBytes);

/************************************************************************/
/** \brief Memory load from file
**  
**  \par Description
**       Support function for #MM_LoadMemFromFileCmd. This routine will 
**       read a file and write the data to memory
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileHandle   The open file handle of the load file  
**
**  \param [in]   FileName     A pointer to a character string holding  
**                             the load file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to 
**                             the load file header structure
**
**  \param [in]   DestAddress  The destination address for the requested 
**                             load operation 
** 
**  \returns
**  \retstmt Returns TRUE if the load completed successfully  \endcode
**  \retstmt Returns FALSE if the load failed due to an error \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_LoadMemFromFile(uint32                  FileHandle, 
                           char                    *FileName, 
                           MM_LoadDumpFileHeader_t *FileHeader, 
                           uint32                  DestAddress);

/************************************************************************/
/** \brief Verify load memory from file parameters
**  
**  \par Description
**       This routine will run various checks on the destination address, 
**       memory type, and data size (in bytes) for a load memory from
**       file command
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The destination address for the requested 
**                              load operation 
**
**  \param [in]   MemType       The destination memory type for the requested 
**                              load operation
**  
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              load operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyFileLoadParams(uint32 Address, 
                                uint8  MemType, 
                                uint32 SizeInBytes);

/************************************************************************/
/** \brief Verify load file size
**  
**  \par Description
**       Support function for #MM_LoadMemFromFileCmd. This routine will 
**       check if the size of a load file as reported by the filesystem
**       is what it should be based upon the number of load bytes 
**       specified in the MM file header.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileName     A pointer to a character string holding  
**                             the load file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to 
**                             the load file header structure
**
**  \returns
**  \retstmt Returns TRUE if the load file size is as expected \endcode
**  \retstmt Returns FALSE if the load file size is not as expected \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_VerifyLoadFileSize(char                    *FileName, 
                              MM_LoadDumpFileHeader_t *FileHeader);

/************************************************************************/
/** \brief Read the cFE primary and and MM secondary file headers
**  
**  \par Description
**       Support function for #MM_LoadMemFromFileCmd. This routine will 
**       read the cFE primary and MM secondary headers from the
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
**                             the file whose headers are to be read.
**
**  \param [in]   CFEHeader    A #CFE_FS_Header_t pointer to 
**                             the cFE primary file header structure.
**
**  \param [in]   MMHeader     A #MM_LoadDumpFileHeader_t pointer to 
**                             the MM secondary file header structure
**
**  \param [out]  *CFEHeader   Contents of the cFE primary file header 
**                             structure for the specified file.
**
**  \param [out]  *MMHeader    Contents of the MM secondary file header 
**                             structure for the specified file.
**
**  \returns
**  \retstmt Returns TRUE if the headers were read successfully \endcode
**  \retstmt Returns FALSE if a read error occurred \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_ReadFileHeaders(char                    *FileName,
                           int32                    FileHandle,
                           CFE_FS_Header_t         *CFEHeader,
                           MM_LoadDumpFileHeader_t *MMHeader);

/************************************************************************/
/** \brief Fill memory
**  
**  \par Description
**       Support function for #MM_FillMemCmd. This routine will  
**       load memory with a command specified fill pattern
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   DestAddr   The destination address for the fill 
**                           operation
**
**  \param [in]   CmdPtr     A #MM_FillMemCmd_t pointer to the fill
**                           command message
**
*************************************************************************/
void MM_FillMem (uint32          DestAddr, 
                 MM_FillMemCmd_t *CmdPtr);

/************************************************************************/
/** \brief Verify memory fill parameters
**  
**  \par Description
**       This routine will run various checks on the destination address, 
**       memory type, and data size (in bytes) for a memory fill command
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The destination address for the requested 
**                              fill operation 
**
**  \param [in]   MemType       The destination memory type for the 
**                              requested fill operation
**  
**  \param [in]   SizeInBytes   The number of bytes to fill 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyFillParams(uint32 Address, 
                            uint8  MemType, 
                            uint32 SizeInBytes);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Memory poke ground command                                      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_PokeCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   boolean         Valid = TRUE;
   uint32          DestAddress;
   MM_PokeCmd_t    *CmdPtr;
   uint16          ExpectedLength = sizeof(MM_PokeCmd_t);
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_PokeCmd_t *)MessagePtr);

      /* Resolve the symbolic address in command message */
      Valid = CFS_ResolveSymAddr(&(CmdPtr->DestSymAddress), &DestAddress);

      if(Valid == TRUE)
      {  
         /* Run necessary checks on command parameters */
         Valid = MM_VerifyPeekPokeParams(DestAddress, CmdPtr->MemType, CmdPtr->DataSize);
         
         /* Check the specified memory type and call the appropriate routine */
         if(Valid == TRUE)
         {
            /* Check if we need special EEPROM processing */
            if (CmdPtr->MemType == MM_EEPROM)
            {
               MM_PokeEeprom(CmdPtr, DestAddress);               
            }
            else
            {
               /*
               ** We can use this routine for all other memory types
               *  (including the optional ones)
               */
               MM_PokeMem(CmdPtr, DestAddress);
            }
            
         } /* end MM_VerifyPeekPokeParams if */
         
      } /* end CFS_ResolveSymAddr */
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                           "Symbolic address can't be resolved: Name = '%s'", 
                           CmdPtr->DestSymAddress.SymName);   
      }
      
   } /* end MM_VerifyCmdLength if */
   
   return;
   
} /* end MM_PokeCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Write 8, 16, or 32 bits of data to any RAM memory address       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_PokeMem (MM_PokeCmd_t *CmdPtr, 
                 uint32       DestAddress)
{
   uint8      ByteValue;
   uint16     WordValue;
   int32      PSP_Status;
   uint32     DataValue = 0;
   uint32     BytesProcessed = 0;
   boolean    ValidPoke = FALSE;

   /* Write input number of bits to destination address */
   switch(CmdPtr->DataSize)
   {
      case MM_BYTE_BIT_WIDTH:
         ByteValue = (uint8) CmdPtr->Data;
         DataValue = (uint32) ByteValue;
         BytesProcessed = sizeof (uint8);
         if ((PSP_Status = CFE_PSP_MemWrite8(DestAddress, ByteValue)) == CFE_PSP_SUCCESS)
         {
            CFE_EVS_SendEvent(MM_POKE_BYTE_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 8 bits, Data = 0x%02X",
                              DestAddress, ByteValue);
            ValidPoke = TRUE;
         }
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR,
                             "PSP write memory error: RC=0x%08X, Address=0x%08X, MemType=MEM8", 
                              PSP_Status, DestAddress);
         }
         break;
         
      case MM_WORD_BIT_WIDTH:
         WordValue = (uint16)CmdPtr->Data;
         DataValue = (uint32) WordValue;
         BytesProcessed = sizeof (uint16);
         if ((PSP_Status = CFE_PSP_MemWrite16(DestAddress, WordValue)) == CFE_PSP_SUCCESS)
         {
            CFE_EVS_SendEvent(MM_POKE_WORD_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 16 bits, Data = 0x%04X",
                              DestAddress, WordValue);
            ValidPoke = TRUE;
         }
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR,
                             "PSP write memory error: RC=0x%08X, Address=0x%08X, MemType=MEM16", 
                              PSP_Status, DestAddress);
         }
         break;
         
      case MM_DWORD_BIT_WIDTH:
         DataValue = CmdPtr->Data;
         BytesProcessed = sizeof (uint32);
         if ((PSP_Status = CFE_PSP_MemWrite32(DestAddress, DataValue)) == CFE_PSP_SUCCESS)
         {
            CFE_EVS_SendEvent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 32 bits, Data = 0x%08X",
                              DestAddress, DataValue);
            ValidPoke = TRUE;
         }
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_WRITE_ERR_EID, CFE_EVS_ERROR,
                             "PSP write memory error: RC=0x%08X, Address=0x%08X, MemType=MEM32", 
                              PSP_Status, DestAddress);
         }
         break;
         
      /* 
      ** We don't need a default case, a bad DataSize will get caught
      ** in the MM_VerifyPeekPokeParams function and we won't get here
      */
      default:
         break;
   }

   if (ValidPoke)
   {
      /* Update cmd counter and last action stats */
      MM_AppData.CmdCounter++;
      MM_AppData.LastAction = MM_POKE;
      MM_AppData.MemType    = CmdPtr->MemType;
      MM_AppData.Address    = DestAddress;
      MM_AppData.DataValue  = DataValue;
      MM_AppData.BytesProcessed = BytesProcessed;
   }

   return;
   
} /* end MM_PokeMem */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Write 8, 16, or 32 bits of data to any EEPROM memory address    */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_PokeEeprom (MM_PokeCmd_t *CmdPtr, 
                    uint32       DestAddress)
{
   uint8      ByteValue;
   uint16     WordValue;
   int32      PSP_Status;
   uint32     DataValue = 0;
   uint32     BytesProcessed = 0;
   boolean    ValidPoke = FALSE;
   
   CFE_ES_PerfLogEntry(MM_EEPROM_POKE_PERF_ID);
   
   /* Write input number of bits to destination address */
   switch(CmdPtr->DataSize)
   {
      case MM_BYTE_BIT_WIDTH:
         ByteValue = (uint8) CmdPtr->Data;
         DataValue = (uint32) ByteValue;
         BytesProcessed = sizeof (uint8);
         PSP_Status = CFE_PSP_EepromWrite8(DestAddress, ByteValue);
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_EEPROMWRITE8_ERR_EID, CFE_EVS_ERROR,
                             "CFE_PSP_EepromWrite8 error received: RC = 0x%08X, Addr = 0x%08X", 
                              PSP_Status, DestAddress); 
         }
         else
         {
            CFE_EVS_SendEvent(MM_POKE_BYTE_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 8 bits, Data = 0x%02X",
                              DestAddress, ByteValue);
            ValidPoke = TRUE;
         }
         break;
         
      case MM_WORD_BIT_WIDTH:
         WordValue = (uint16)CmdPtr->Data;
         DataValue = (uint32) WordValue;
         BytesProcessed = sizeof (uint16);
         PSP_Status = CFE_PSP_EepromWrite16(DestAddress, WordValue);
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_EEPROMWRITE16_ERR_EID, CFE_EVS_ERROR,
                             "CFE_PSP_EepromWrite16 error received: RC = 0x%08X, Addr = 0x%08X", 
                              PSP_Status, DestAddress); 
         }
         else
         {
            CFE_EVS_SendEvent(MM_POKE_WORD_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 16 bits, Data = 0x%04X",
                              DestAddress, WordValue);
            ValidPoke = TRUE;
         }
         break;
         
      case MM_DWORD_BIT_WIDTH:
         DataValue = CmdPtr->Data;
         BytesProcessed = sizeof (uint32);
         PSP_Status = CFE_PSP_EepromWrite32(DestAddress, CmdPtr->Data);
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_EEPROMWRITE32_ERR_EID, CFE_EVS_ERROR,
                             "CFE_PSP_EepromWrite32 error received: RC = 0x%08X, Addr = 0x%08X",
                              PSP_Status, DestAddress); 
         }
         else
         {
            CFE_EVS_SendEvent(MM_POKE_DWORD_INF_EID, CFE_EVS_INFORMATION,
                             "Poke Command: Addr = 0x%08X, Size = 32 bits, Data = 0x%08X", 
                              DestAddress, CmdPtr->Data);
            ValidPoke = TRUE;
         }
         break;
         
      /* 
      ** We don't need a default case, a bad DataSize will get caught
      ** in the MM_VerifyPeekPokeParams function and we won't get here
      */
      default:
         break;
   }

   if (ValidPoke)
   {
      /* Update cmd counter and last action stats */
      MM_AppData.CmdCounter++;
      MM_AppData.LastAction = MM_POKE;
      MM_AppData.MemType    = CmdPtr->MemType;
      MM_AppData.Address    = DestAddress;
      MM_AppData.DataValue  = DataValue;
      MM_AppData.BytesProcessed = BytesProcessed;
   }

   CFE_ES_PerfLogExit(MM_EEPROM_POKE_PERF_ID);
   
   return;
   
} /* end MM_PokeEeprom */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load memory with interrupts disabled                            */ 
/* Only valid for RAM addresses                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_LoadMemWIDCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   MM_LoadMemWIDCmd_t     *CmdPtr;
   int32                  LockKey;
   int32                  PSP_Status;
   uint32                 ComputedCRC = 0;
   uint32                 DestAddress;
   uint16                 ExpectedLength = sizeof(MM_LoadMemWIDCmd_t);
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_LoadMemWIDCmd_t *)MessagePtr);
        
      /* Resolve the symbolic address in command message */
      if (CFS_ResolveSymAddr(&(CmdPtr->DestSymAddress), &DestAddress) == TRUE)
      {
         /*
         ** Run some necessary checks on command parameters 
         ** NOTE: A load with interrupts disabled command is only valid for RAM addresses 
         */
         if (MM_VerifyLoadWIDParams(DestAddress, CmdPtr->NumOfBytes) == TRUE)
         {
            /* Verify data integrity check value */
            ComputedCRC = CFE_ES_CalculateCRC(CmdPtr->DataArray,
                                              CmdPtr->NumOfBytes,
                                              0, MM_LOAD_WID_CRC_TYPE);
            /* 
            ** If the CRC matches do the load 
            */ 
            if(ComputedCRC == CmdPtr->Crc)
            {
               /* Lock current interrupts */
               LockKey = OS_IntLock();

               /* Load input data to input memory address */
               PSP_Status = CFE_PSP_MemCpy((void *)DestAddress,
                                           CmdPtr->DataArray,
                                           CmdPtr->NumOfBytes);

               /* Restore interrupt state */
               OS_IntUnlock(LockKey);

               if (PSP_Status == CFE_PSP_SUCCESS)
               {
                  MM_AppData.CmdCounter++;
                  CFE_EVS_SendEvent(MM_LOAD_WID_INF_EID, CFE_EVS_INFORMATION,
                     "Load Memory WID Command: Wrote %d bytes to address: 0x%08X", 
                                    CmdPtr->NumOfBytes, DestAddress);

                  /* Update last action statistics */
                  MM_AppData.LastAction  = MM_LOAD_WID;
                  MM_AppData.Address     = DestAddress;
                  MM_AppData.MemType     = MM_RAM;
                  MM_AppData.BytesProcessed = CmdPtr->NumOfBytes; 
               }
               else
               {
                  MM_AppData.ErrCounter++;
                  CFE_EVS_SendEvent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR,
                     "PSP copy memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Size=0x%08X", 
                                    PSP_Status, (uint32) CmdPtr->DataArray,
                                    DestAddress, CmdPtr->NumOfBytes);
               }
            }
            else
            {
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_LOAD_WID_CRC_ERR_EID, CFE_EVS_ERROR,
                  "Interrupts Disabled Load CRC failure: Expected = 0x%X Calculated = 0x%X", 
                                 CmdPtr->Crc, ComputedCRC);
            }    
               
         } /* end MM_VerifyLoadWIDParams */
            
      } /* end CFS_ResolveSymAddr if */
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                           "Symbolic address can't be resolved: Name = '%s'", 
                           CmdPtr->DestSymAddress.SymName);   
      }

   } /* end MM_VerifyCmdLength if */

   return;
    
} /* end MM_LoadMemWIDCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify load memory with interrupts disabled parameters          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyLoadWIDParams(uint32 Address, 
                               uint32 SizeInBytes)
{
   boolean  Valid = TRUE;
   int32    PSP_Status;
   
   PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
   if (PSP_Status != CFE_PSP_SUCCESS)
   {
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
         "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                        PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
   }
   else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_UNINTERRUPTABLE_DATA))
   {
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                  "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
   }
         
   return (Valid);

} /* end MM_VerifyLoadWIDParams */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load memory from a file command                                 */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_LoadMemFromFileCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   boolean                  Valid = TRUE;
   int32                    FileHandle;
   int32                    OS_Status = OS_SUCCESS;
   uint32                   DestAddress;
   MM_LoadMemFromFileCmd_t  *CmdPtr;
   CFE_FS_Header_t          CFEFileHeader;
   MM_LoadDumpFileHeader_t  MMFileHeader;
   uint32                   ComputedCRC;
   uint16                   ExpectedLength = sizeof(MM_LoadMemFromFileCmd_t);
   
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      CmdPtr = ((MM_LoadMemFromFileCmd_t *)MessagePtr);
      
      /* 
      ** NUL terminate the very end of the file name string array as a
      ** safety measure
      */
      CmdPtr->FileName[OS_MAX_PATH_LEN - 1] = '\0';
      
      /* Verify filename doesn't have any illegal characters */
      Valid = CFS_IsValidFilename(CmdPtr->FileName, strlen(CmdPtr->FileName));
      if(Valid == TRUE)
      {
         /* Open load file for reading */
         if((FileHandle = OS_open(CmdPtr->FileName, OS_READ_ONLY, 0)) >= OS_SUCCESS)
         {
            /* Read in the file headers */
            Valid = MM_ReadFileHeaders(CmdPtr->FileName, FileHandle, &CFEFileHeader, &MMFileHeader);
            if(Valid == TRUE)
            {
               /* Verify the file size is correct */
               Valid = MM_VerifyLoadFileSize(CmdPtr->FileName, &MMFileHeader);
               if(Valid == TRUE)
               {
                  /* Verify data integrity check value */
                  OS_Status = CFS_ComputeCRCFromFile(FileHandle, &ComputedCRC, MM_LOAD_FILE_CRC_TYPE);
                  if(OS_Status == OS_SUCCESS)
                  {
                     /*
                     ** Reset the file pointer to the start of the load data, need to do this
                     ** because CFS_ComputeCRCFromFile reads to the end of file 
                     */
                     OS_lseek(FileHandle, (sizeof(CFE_FS_Header_t) 
                                           + sizeof(MM_LoadDumpFileHeader_t)), OS_SEEK_SET);
                     
                     /* Check the computed CRC against the file header CRC */
                     if(ComputedCRC == MMFileHeader.Crc)
                     {
                        /* Resolve symbolic address in file header */
                        Valid = CFS_ResolveSymAddr(&(MMFileHeader.SymAddress), &DestAddress);

                        if(Valid == TRUE)
                        {
                           /* Run necessary checks on command parameters */ 
                           Valid = MM_VerifyFileLoadParams(DestAddress, MMFileHeader.MemType, 
                                                                        MMFileHeader.NumOfBytes);
                           if(Valid == TRUE)
                           {
                              /* Call the load routine for the specified memory type */
                              switch(MMFileHeader.MemType)
                                 {
                                    case MM_RAM:
                                    case MM_EEPROM:
                                       Valid = MM_LoadMemFromFile(FileHandle, CmdPtr->FileName, 
                                                                  &MMFileHeader, DestAddress);    
                                       break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
                                    case MM_MEM32:
                                       Valid = MM_LoadMem32FromFile(FileHandle, CmdPtr->FileName, 
                                                                    &MMFileHeader, DestAddress);    
                                       break;
#endif /* MM_OPT_CODE_MEM32_MEMTYPE */
                                       
#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
                                    case MM_MEM16:
                                       Valid = MM_LoadMem16FromFile(FileHandle, CmdPtr->FileName, 
                                                                    &MMFileHeader, DestAddress);    
                                       break;
#endif /* MM_OPT_CODE_MEM16_MEMTYPE */
                                       
#if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
                                    case MM_MEM8:
                                       Valid = MM_LoadMem8FromFile(FileHandle, CmdPtr->FileName, 
                                                                   &MMFileHeader, DestAddress);    
                                       break;
#endif /* MM_OPT_CODE_MEM8_MEMTYPE */
                                       
                                    /* 
                                    ** We don't need a default case, a bad MemType will get caught
                                    ** in the MM_VerifyFileLoadParams function and we won't get here
                                    */
                                    default:
                                       break;
                                 }
                            
                                 if(Valid == TRUE)
                                 {
                                    MM_AppData.CmdCounter++;
                                    CFE_EVS_SendEvent(MM_LD_MEM_FILE_INF_EID, CFE_EVS_INFORMATION,
                                            "Load Memory From File Command: Loaded %d bytes to address 0x%08X from file '%s'", 
                                            MM_AppData.BytesProcessed, DestAddress, CmdPtr->FileName);
                                 }

                           } /* end MM_VerifyFileLoadParams if */
                           else
                           {
                              /*
                              ** We don't need to increment the error counter here, it was done by the
                              ** MM_VerifyFileLoadParams routine when the error was first discovered.
                              ** We send this event as a supplemental message with the filename attached.
                              */
                              CFE_EVS_SendEvent(MM_FILE_LOAD_PARAMS_ERR_EID, CFE_EVS_ERROR,
                                          "Load file failed parameters check: File = '%s'", CmdPtr->FileName); 
                           }

                        }   /* end CFS_ResolveSymAddr if */ 
                        else
                        {
                           MM_AppData.ErrCounter++;
                           CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                                             "Symbolic address can't be resolved: Name = '%s'", 
                                             MMFileHeader.SymAddress.SymName);   
                        }  

                     } /* end ComputedCRC == MMFileHeader.Crc if */
                     else
                     {
                        MM_AppData.ErrCounter++;
                        CFE_EVS_SendEvent(MM_LOAD_FILE_CRC_ERR_EID, CFE_EVS_ERROR,
                                          "Load file CRC failure: Expected = 0x%X Calculated = 0x%X File = '%s'", 
                                          MMFileHeader.Crc, ComputedCRC, CmdPtr->FileName);
                     }
                     
                  } /* end CFS_ComputeCRCFromFile if */
                  else
                  {
                     MM_AppData.ErrCounter++;
                     CFE_EVS_SendEvent(MM_CFS_COMPUTECRCFROMFILE_ERR_EID, CFE_EVS_ERROR,
                                 "CFS_ComputeCRCFromFile error received: RC = 0x%08X File = '%s'", OS_Status, 
                                                                                         CmdPtr->FileName);
                  } 
                  
               } /* end MM_VerifyLoadFileSize */
               
               /*
               ** Don't need an 'else' here. MM_VerifyLoadFileSize will increment
               ** the error counter and generate an event message if needed.
               */
               
            } /* end MM_ReadFileHeaders if */

            /*
            ** Don't need an 'else' here. MM_ReadFileHeaders will increment
            ** the error counter and generate an event message if needed.
            */
            
            /* Close the load file for all cases after the open call succeeds */
            OS_Status = OS_close(FileHandle);
            if(OS_Status != OS_SUCCESS)
            {
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_OS_CLOSE_ERR_EID, CFE_EVS_ERROR,
                                 "OS_close error received: RC = 0x%08X File = '%s'", OS_Status, 
                                                                           CmdPtr->FileName);
            }     

         } /* end OS_open if */
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_OPEN_ERR_EID, CFE_EVS_ERROR,
                              "OS_open error received: RC = 0x%08X File = '%s'", FileHandle, 
                                                                        CmdPtr->FileName);
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
    
} /* end LoadMemFromFileCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Loads memory from a file                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean MM_LoadMemFromFile(uint32                   FileHandle, 
                           char                     *FileName, 
                           MM_LoadDumpFileHeader_t  *FileHeader, 
                           uint32                   DestAddress)
{
   boolean  Valid          = FALSE;
   int32    BytesRemaining = FileHeader->NumOfBytes;
   int32    BytesProcessed = 0;
   int32    ReadLength;
   int32    PSP_Status; 
   uint32   SegmentSize    = MM_MAX_LOAD_DATA_SEG;
   uint8   *ioBuffer       = (uint8 *) &MM_AppData.LoadBuffer[0];
   uint8   *TargetPointer  = (uint8 *) DestAddress;

   if(FileHeader->MemType == MM_EEPROM)
   {
      CFE_ES_PerfLogEntry(MM_EEPROM_FILELOAD_PERF_ID);
   }

   while (BytesRemaining != 0)
   {
      if (BytesRemaining < MM_MAX_LOAD_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      if ((ReadLength = OS_read(FileHandle, ioBuffer, SegmentSize)) == SegmentSize)
      {
         PSP_Status = CFE_PSP_MemCpy(TargetPointer, ioBuffer, SegmentSize);

         if (PSP_Status == CFE_PSP_SUCCESS)
         {
            BytesRemaining -= SegmentSize;
            BytesProcessed += SegmentSize;
            TargetPointer  += SegmentSize;

            /* Prevent CPU hogging between load segments */
            if (BytesRemaining != 0)      
            {
               MM_SegmentBreak();
            }
         }
         else
         {
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR,
               "PSP copy memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Size=0x%08X", 
                PSP_Status, (uint32) ioBuffer, (uint32) TargetPointer, SegmentSize);
            BytesRemaining = 0;
         }
      }
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_OS_READ_ERR_EID, CFE_EVS_ERROR,
                "OS_read error received: RC = 0x%08X Expected = %d File = '%s'", 
                 ReadLength, SegmentSize, FileName);
         BytesRemaining = 0;
      }
   }

   /* Update last action statistics */
   if (BytesProcessed == FileHeader->NumOfBytes)
   {
      Valid = TRUE;
      MM_AppData.LastAction = MM_LOAD_FROM_FILE;
      MM_AppData.MemType    = FileHeader->MemType;
      MM_AppData.Address    = DestAddress;
      MM_AppData.BytesProcessed = BytesProcessed;
      strncpy(MM_AppData.FileName, FileName, OS_MAX_PATH_LEN);
   }      

   return(Valid);   
   
} /* end MM_LoadMemFromFile */   

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify load memory from a file parameters                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyFileLoadParams(uint32 Address, 
                                uint8  MemType, 
                                uint32 SizeInBytes)
{
   boolean  Valid = TRUE;
   int32    PSP_Status;
   
   switch(MemType)
   {
      case MM_RAM:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_LOAD_FILE_DATA_RAM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;
         
      case MM_EEPROM:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_EEPROM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_EEPROM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_LOAD_FILE_DATA_EEPROM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
      case MM_MEM32:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_LOAD_FILE_DATA_MEM32))
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
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_LOAD_FILE_DATA_MEM16))
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
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_LOAD_FILE_DATA_MEM8))
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

   } /* end MemType switch */

   return (Valid);
   
} /* end MM_VerifyFileLoadParams */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify load file size                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyLoadFileSize(char                    *FileName, 
                              MM_LoadDumpFileHeader_t *FileHeader)
{

   boolean     Valid = TRUE;
   int32       OS_Status;
   uint32      ExpectedSize;
   int32       ActualSize;   /* The size returned by OS_stat is signed */
   os_fstat_t  FileStats;

   /*
   ** Get the filesystem statistics on our load file
   */
   OS_Status = OS_stat(FileName, &FileStats);
   if(OS_Status != OS_FS_SUCCESS)
   {
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_OS_STAT_ERR_EID, CFE_EVS_ERROR,
                        "OS_stat error received: RC = 0x%08X File = '%s'", 
                                                   OS_Status, FileName);
   }
   else
   {
      /*
      ** Check the reported size of the file against what it should be based
      ** upon the number of load bytes specified in the file header
      */
      ActualSize   = FileStats.st_size;
      ExpectedSize = FileHeader->NumOfBytes + sizeof(CFE_FS_Header_t) 
                                            + sizeof(MM_LoadDumpFileHeader_t);
      if(ActualSize != ExpectedSize)
      {
         Valid = FALSE;
         MM_AppData.ErrCounter++;

         /*
         ** Note: passing FileStats.st_size in this event message will cause
         ** a segmentation fault under cygwin during unit testing, so we added 
         ** the variable ActualSize to this function.
         */
         CFE_EVS_SendEvent(MM_LD_FILE_SIZE_ERR_EID, CFE_EVS_ERROR,
                           "Load file size error: Reported by OS = %d Expected = %d File = '%s'",
                           ActualSize, ExpectedSize, FileName);
      }
      
   }
   
   return (Valid);
   
} /* end MM_VerifyLoadFileSize */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Read the cFE primary and and MM secondary file headers          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_ReadFileHeaders(char                    *FileName,
                           int32                    FileHandle,
                           CFE_FS_Header_t         *CFEHeader,
                           MM_LoadDumpFileHeader_t *MMHeader)
{
   boolean     Valid = TRUE;
   int32       OS_Status;

   /*
   ** Read in the primary cFE file header
   */
   OS_Status = CFE_FS_ReadHeader(CFEHeader, FileHandle);
   if(OS_Status != sizeof(CFE_FS_Header_t))
   {
      /* We either got an error or didn't read as much data as expected */
      Valid = FALSE;
      MM_AppData.ErrCounter++;
      CFE_EVS_SendEvent(MM_CFE_FS_READHDR_ERR_EID, CFE_EVS_ERROR,
                        "CFE_FS_ReadHeader error received: RC = 0x%08X Expected = %d File = '%s'", 
                        OS_Status, sizeof(CFE_FS_Header_t), FileName); 


   } /* end CFE_FS_ReadHeader if */
   else             
   {  
      /*
      ** Read in the secondary MM file header 
      */
      OS_Status = OS_read(FileHandle, MMHeader, sizeof(MM_LoadDumpFileHeader_t));
      if(OS_Status != sizeof(MM_LoadDumpFileHeader_t))
      {
         /* We either got an error or didn't read as much data as expected */
         Valid = FALSE;
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_OS_READ_EXP_ERR_EID, CFE_EVS_ERROR,
                           "OS_read error received: RC = 0x%08X Expected = %d File = '%s'", 
                           OS_Status, sizeof(MM_LoadDumpFileHeader_t), FileName); 

      } /* end OS_read if */
      
   } /* end CFE_FS_ReadHeader else */

   return (Valid);
   
} /* end MM_ReadFileHeaders */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Fill memory command                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_FillMemCmd(CFE_SB_MsgPtr_t MessagePtr)
{
   uint32            DestAddress;
   MM_FillMemCmd_t  *CmdPtr = (MM_FillMemCmd_t *) MessagePtr;
   uint16            ExpectedLength = sizeof(MM_FillMemCmd_t);
    
   /* Verify command packet length */
   if(MM_VerifyCmdLength(MessagePtr, ExpectedLength))
   {
      /* Resolve symbolic address */
      if (CFS_ResolveSymAddr(&(CmdPtr->DestSymAddress), &DestAddress) == TRUE)
      {
         /* Run necessary checks on command parameters */
         if (MM_VerifyFillParams(DestAddress, CmdPtr->MemType, CmdPtr->NumOfBytes) == TRUE)
         {
            switch(CmdPtr->MemType)
            {
               case MM_RAM:
               case MM_EEPROM:
                    MM_FillMem(DestAddress, CmdPtr);
                    break;

               #if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
               case MM_MEM32:
                    MM_FillMem32(DestAddress, CmdPtr);
                    break;
               #endif
                     
               #if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)
               case MM_MEM16:
                    MM_FillMem16(DestAddress, CmdPtr);
                    break;
               #endif
                     
               #if (MM_OPT_CODE_MEM8_MEMTYPE == TRUE)
               case MM_MEM8:
                    MM_FillMem8(DestAddress, CmdPtr);
                    break;
               #endif
                             
               /* 
               ** We don't need a default case, a bad MemType will get caught
               ** in the MM_VerifyFillParams function and we won't get here
               */
               default:
                    break;
            }
            
            if (MM_AppData.LastAction == MM_FILL)
            {
               MM_AppData.CmdCounter++;
               CFE_EVS_SendEvent(MM_FILL_INF_EID, CFE_EVS_INFORMATION,
                  "Fill Memory Command: Filled %d bytes at address: 0x%08X with pattern: 0x%08X", 
                   MM_AppData.BytesProcessed, DestAddress, MM_AppData.DataValue);
            }
         }
      }
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                          "Symbolic address can't be resolved: Name = '%s'", 
                           CmdPtr->DestSymAddress.SymName);   
      }
   }

   return;  
        
} /* end MM_FillMemCmd */       


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Fill memory with the command specified fill pattern             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_FillMem(uint32          DestAddress, 
                MM_FillMemCmd_t *CmdPtr)
{
   uint16   i;
   int32    PSP_Status;
   boolean  Valid = TRUE;
   uint32   BytesProcessed = 0;
   uint32   BytesRemaining = CmdPtr->NumOfBytes;
   uint32   SegmentSize    = MM_MAX_FILL_DATA_SEG;
   uint8   *TargetPointer  = (uint8 *) DestAddress;
   uint8   *FillBuffer     = (uint8 *) &MM_AppData.FillBuffer[0];

   /* Create a scratch buffer with one fill segment */
   for (i = 0; i < (MM_MAX_FILL_DATA_SEG/sizeof(uint32)); i++)
   {
       FillBuffer[i] = CmdPtr->FillPattern;
   }

   /* Start EEPROM performance monitor */
   if(CmdPtr->MemType == MM_EEPROM)
   {
      CFE_ES_PerfLogEntry(MM_EEPROM_FILL_PERF_ID);
   }

   /* Fill memory one segment at a time */
   while (BytesRemaining != 0)
   {
      /* Last fill segment may be partial size */
      if (BytesRemaining < MM_MAX_FILL_DATA_SEG)
      {
         SegmentSize = BytesRemaining;
      }

      PSP_Status = CFE_PSP_MemCpy(TargetPointer, FillBuffer, SegmentSize);

      if (PSP_Status == CFE_PSP_SUCCESS)
      {
         TargetPointer  += SegmentSize;
         BytesProcessed += SegmentSize;
         BytesRemaining -= SegmentSize;
      }
      else
      {
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_PSP_COPY_ERR_EID, CFE_EVS_ERROR,
            "PSP copy memory error: RC=0x%08X, Src=0x%08X, Tgt=0x%08X, Size=0x%08X", 
             PSP_Status, (uint32) FillBuffer, (uint32) TargetPointer, SegmentSize);
         BytesRemaining = 0;
         Valid = FALSE;
      }
   }
       
   /* Stop EEPROM performance monitor */
   if(CmdPtr->MemType == MM_EEPROM)
   {
      CFE_ES_PerfLogExit(MM_EEPROM_FILL_PERF_ID);
   }

   /* Update last action statistics */ 
   if (Valid)
   {
      MM_AppData.LastAction = MM_FILL;
      MM_AppData.MemType = CmdPtr->MemType;
      MM_AppData.Address = DestAddress;
      MM_AppData.DataValue = CmdPtr->FillPattern;
      MM_AppData.BytesProcessed = BytesProcessed;
   }         
           
   return;

}/* End MM_FillMem */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify fill memory parameters                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyFillParams(uint32 Address, 
                            uint8  MemType, 
                            uint32 SizeInBytes)
{
   boolean  Valid = TRUE;
   int32    PSP_Status;
   
   switch(MemType)
   {
      case MM_RAM:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_FILL_DATA_RAM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;
         
      case MM_EEPROM:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_EEPROM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_EEPROM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_FILL_DATA_EEPROM))
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_DATA_SIZE_BYTES_ERR_EID, CFE_EVS_ERROR,
                        "Data size in bytes invalid or exceeds limits: Data Size = %d", SizeInBytes);
         }
         break;

#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)
      case MM_MEM32:
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_FILL_DATA_MEM32))
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
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_FILL_DATA_MEM16))
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
         PSP_Status = CFE_PSP_MemValidateRange(Address, SizeInBytes, CFE_PSP_MEM_RAM);
         
         if (PSP_Status != CFE_PSP_SUCCESS)
         {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_OS_MEMVALIDATE_ERR_EID, CFE_EVS_ERROR,
                              "CFE_PSP_MemValidateRange error received: RC = 0x%08X Addr = 0x%08X Size = %d MemType = %d",
                              PSP_Status, Address, SizeInBytes, CFE_PSP_MEM_RAM); 
         }
         else if((SizeInBytes == 0) || (SizeInBytes > MM_MAX_FILL_DATA_MEM8))
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

   } /* end MemType switch */
   
   return (Valid);
   
} /* end MM_VerifyFillParams */

/************************/
/*  End of File Comment */
/************************/
