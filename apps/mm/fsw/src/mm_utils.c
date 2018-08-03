/*************************************************************************
** File:
**   $Id: mm_utils.c 1.11 2015/03/20 14:16:35EDT lwalling Exp  $
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
**   Utility functions used for processing CFS memory manager commands
**
**   $Log: mm_utils.c  $
**   Revision 1.11 2015/03/20 14:16:35EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.10 2015/03/02 14:27:10EST sstrege 
**   Added copyright information
**   Revision 1.9 2010/11/29 13:35:17EST jmdagost 
**   Replaced ifdef tests with if-true tests.
**   Revision 1.8 2009/06/18 10:17:09EDT rmcgraw 
**   DCR8291:1 Changed OS_MEM_ #defines to CFE_PSP_MEM_
**   Revision 1.7 2009/06/12 14:37:28EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.6 2008/09/05 14:24:12EDT dahardison 
**   Updated references to local HK variables
**   Revision 1.5 2008/09/05 12:33:08EDT dahardison 
**   Modified the MM_VerifyCmdLength routine to issue a special error event message and
**   not increment the command error counter if a housekeeping request is received
**   with a bad command length
**   Revision 1.4 2008/05/22 15:13:56EDT dahardison 
**   Changed inclusion of cfs_lib.h to cfs_utils.h
**   Revision 1.3 2008/05/19 15:23:35EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"
#include "mm_utils.h"
#include "mm_perfids.h"
#include "mm_msgids.h"
#include "mm_events.h"
#include "cfs_utils.h"
#include <string.h>

/*************************************************************************
** External Data
*************************************************************************/
extern MM_AppData_t MM_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset the local housekeeping variables to default parameters    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
void MM_ResetHk(void)
{
                                                     
   MM_AppData.LastAction      = MM_NOACTION;
   MM_AppData.MemType         = MM_NOMEMTYPE;
   MM_AppData.Address         = MM_CLEAR_ADDR;
   MM_AppData.DataValue       = MM_CLEAR_PATTERN;               
   MM_AppData.BytesProcessed  = 0;
   MM_AppData.FileName[0]     = MM_CLEAR_FNAME;       

   return;
    
} /* end MM_ResetHk */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a load, dump, or fill segment break                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_SegmentBreak(void)
{
   /* 
   ** Performance Log entry stamp 
   */
   CFE_ES_PerfLogEntry(MM_SEGBREAK_PERF_ID);
   
   /*
   ** Give something else the chance to run
   */
   OS_TaskDelay(MM_PROCESSOR_CYCLE);

   /* 
   ** Performance Log exit stamp 
   */
   CFE_ES_PerfLogExit(MM_SEGBREAK_PERF_ID);
   
   return;

} /* End of MM_SegmentBreak */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify command packet length                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean MM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength)
{
   boolean result = TRUE;
   uint16  CommandCode;  
   uint16  ActualLength;
   CFE_SB_MsgId_t MessageID;
   
   /*
   ** Verify the message packet length...
   */
   ActualLength = CFE_SB_GetTotalMsgLength(msg);
   if (ExpectedLength != ActualLength)
   {
      MessageID   = CFE_SB_GetMsgId(msg);
      CommandCode = CFE_SB_GetCmdCode(msg);

      if (MessageID == MM_SEND_HK_MID)
      {
          /*
          ** For a bad HK request, just send the event. We only increment
          ** the error counter for ground commands and not internal messages.
          */
          CFE_EVS_SendEvent(MM_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR,
                  "Invalid HK request msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                  MessageID, CommandCode, ActualLength, ExpectedLength);
      }
      else
      {
          CFE_EVS_SendEvent(MM_LEN_ERR_EID, CFE_EVS_ERROR,
                  "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                  MessageID, CommandCode, ActualLength, ExpectedLength);
          MM_AppData.ErrCounter++;          
      }

      result = FALSE;
   }

   return(result);

} /* End of MM_VerifyCmdLength */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify peek and poke command parameters                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */   
boolean MM_VerifyPeekPokeParams(uint32 Address, 
                                uint8  MemType, 
                                uint8  SizeInBits)
{
   boolean  Valid = TRUE;
   uint8    SizeInBytes;
   int32    OS_Status;
   
   switch(SizeInBits)
   {
      case MM_BYTE_BIT_WIDTH:
         SizeInBytes = 1;
         break;
      
      case MM_WORD_BIT_WIDTH:
         SizeInBytes = 2;
         if (CFS_Verify16Aligned(Address, SizeInBytes) != TRUE)
            {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_ALIGN16_ERR_EID, CFE_EVS_ERROR,
                              "Data and address not 16 bit aligned: Addr = 0x%08X Size = %d",
                                                                       Address, SizeInBytes);
            
            
            }
         break;
         
      case MM_DWORD_BIT_WIDTH:
         SizeInBytes = 4;
         if (CFS_Verify32Aligned(Address, SizeInBytes) != TRUE)
            {
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_ALIGN32_ERR_EID, CFE_EVS_ERROR,
                              "Data and address not 32 bit aligned: Addr = 0x%08X Size = %d",
                                                                       Address, SizeInBytes);
            }
         break;
      
      default:
         Valid = FALSE;
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR,
                     "Data size in bits invalid: Data Size = %d", SizeInBits);
         break;
   }

   /* Do other checks if this one passed */
   if (Valid == TRUE)   
   {
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
            /* 
            ** Peeks and Pokes must be 32 bits wide for this memory type 
            */
            else if (SizeInBytes != 4)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR,
                           "Data size in bits invalid: Data Size = %d", SizeInBits);
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
            /* 
            ** Peeks and Pokes must be 16 bits wide for this memory type
            */
            else if (SizeInBytes != 2)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR,
                           "Data size in bits invalid: Data Size = %d", SizeInBits);
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
            /* 
            ** Peeks and Pokes must be 8 bits wide for this memory type
            */
            else if (SizeInBytes != 1)
            {
               Valid = FALSE;
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_DATA_SIZE_BITS_ERR_EID, CFE_EVS_ERROR,
                           "Data size in bits invalid: Data Size = %d", SizeInBits);
            }
            break;
#endif /* MM_OPT_CODE_MEM8_MEMTYPE */
            
         default:
            Valid = FALSE;
            MM_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MM_MEMTYPE_ERR_EID, CFE_EVS_ERROR,
                              "Invalid memory type specified: MemType = %d", MemType);
            break;
      
      } /* end switch */

   } /* end Valid == TRUE if */
   
   return (Valid);

} /* end MM_VerifyPeekPokeParams */

/************************/
/*  End of File Comment */
/************************/
