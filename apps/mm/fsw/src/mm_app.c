/************************************************************************
** File:
**   $Id: mm_app.c 1.17 2015/04/14 15:29:04EDT lwalling Exp  $
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
**   The CFS Memory Manager (MM) Application provides onboard hardware
**   and software maintenance services by processing commands for memory 
**   operations and read and write accesses to memory mapped hardware.
**
**   $Log: mm_app.c  $
**   Revision 1.17 2015/04/14 15:29:04EDT lwalling 
**   Removed unnecessary backslash characters from string format definitions
**   Revision 1.16 2015/03/30 17:34:00EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.15 2015/03/20 14:16:55EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.14 2015/03/02 14:26:42EST sstrege 
**   Added copyright information
**   Revision 1.13 2011/11/30 15:58:34EST jmdagost 
**   Removed unused local variable and function call, initialized local variables.
**   Revision 1.12 2010/12/08 14:38:59EST jmdagost 
**   Added filename validation for symbol table dump command.
**   Revision 1.11 2010/11/29 08:47:30EST jmdagost 
**   Added support for EEPROM write-enable/disable commands
**   Revision 1.10 2010/11/26 13:04:49EST jmdagost 
**   Included mm_platform_cfg.h to access mission revision number.
**   Revision 1.9 2010/11/24 17:09:20EST jmdagost 
**   Implemented the MM Write Symbol Table to File command
**   Revision 1.8 2009/04/18 15:29:37EDT dkobe 
**   Corrected doxygen comments
**   Revision 1.7 2008/09/06 15:33:57EDT dahardison 
**   Added support for new init and noop event strings with version information
**   Revision 1.6 2008/09/06 15:01:15EDT dahardison 
**   Updated to support the symbol lookup ground command
**   Revision 1.5 2008/09/05 14:27:44EDT dahardison 
**   Updated references of local HK variables and the MM_HousekeepingCmd
**   function accordingly for changes related to DCR 3611
**   Revision 1.4 2008/05/22 15:15:05EDT dahardison 
**   Added header includes for mm_msgids.h and mm_perfids.h
**   Revision 1.3 2008/05/19 15:22:53EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"
#include "mm_perfids.h"
#include "mm_msgids.h"
#include "mm_load.h"
#include "mm_dump.h"
#include "mm_utils.h"
#include "mm_events.h"
#include "mm_verify.h"
#include "mm_version.h"
#include "mm_platform_cfg.h"
#include <string.h>

/************************************************************************
** MM global data
*************************************************************************/
MM_AppData_t MM_AppData;

/************************************************************************
** Local function prototypes
*************************************************************************/
/************************************************************************/
/** \brief Initialize the memory manager CFS application
**  
**  \par Description
**       Memory manager application initialization routine. This 
**       function performs all the required startup steps to 
**       get the application registered with the cFE services so
**       it can begin to receive command messages. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register         \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
**  \endreturns
**
*************************************************************************/
int32 MM_AppInit(void); 

/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void MM_AppPipe(CFE_SB_MsgPtr_t msg);
 
/************************************************************************/
/** \brief Process housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This command does not affect the command execution counter
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
*************************************************************************/
void MM_HousekeepingCmd(CFE_SB_MsgPtr_t msg);
 
/************************************************************************/
/** \brief Process noop command
**  
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
**  \sa #MM_NOOP_CC
**
*************************************************************************/
void MM_NoopCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Process reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the memory manager commmand error and command execution counters
**       to zero.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_RESET_CC
**
*************************************************************************/
void MM_ResetCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process lookup symbol command
**  
**  \par Description
**       Processes a lookup symbol ground command which takes a 
**       symbol name and tries to resolve it to an address using the
**       #OS_SymbolLookup OSAL function.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_LOOKUP_SYM_CC
**
*************************************************************************/
void MM_LookupSymbolCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Dump symbol table to file command
**  
**  \par Description
**       Processes a dump symbol table to file ground command which calls  
**       the #OS_SymbolTableDump OSAL function using the specified dump
**       file name.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_SYMTBL_TO_FILE_CC
**
*************************************************************************/
void MM_SymTblToFileCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Write-enable EEPROM command
**  
**  \par Description
**       Processes a EEPROM write enable ground command which calls  
**       the #CFE_PSP_EepromWriteEnable cFE function using the specified
**       bank number.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_ENABLE_EEPROM_WRITE_CC
**
*************************************************************************/
void MM_EepromWriteEnaCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Write-disable EEPROM command
**  
**  \par Description
**       Processes a EEPROM write disable ground command which calls  
**       the #CFE_PSP_EepromWriteDisable cFE function using the specified
**       bank number.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DISABLE_EEPROM_WRITE_CC
**
*************************************************************************/
void MM_EepromWriteDisCmd(CFE_SB_MsgPtr_t msg);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MM application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_AppMain(void)
{
   int32 Status = CFE_SUCCESS;
    
   /*
   **  Register the application with Executive Services 
   */
   CFE_ES_RegisterApp();

   /* 
   ** Create the first Performance Log entry
   */
   CFE_ES_PerfLogEntry(MM_APPMAIN_PERF_ID);

   /*
   ** Perform application specific initialization
   */
   Status = MM_AppInit();
   if (Status != CFE_SUCCESS)
   {
      MM_AppData.RunStatus = CFE_ES_APP_ERROR;
   }

   /*
   ** Application main loop
   */
   while(CFE_ES_RunLoop(&MM_AppData.RunStatus) == TRUE)
   {
      /* 
      ** Performance Log exit stamp 
      */
      CFE_ES_PerfLogExit(MM_APPMAIN_PERF_ID);
       
      /* 
      ** Pend on the arrival of the next Software Bus message 
      */
      Status = CFE_SB_RcvMsg(&MM_AppData.MsgPtr, MM_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
       
      /* 
      ** Performance Log entry stamp 
      */
      CFE_ES_PerfLogEntry(MM_APPMAIN_PERF_ID);       

      /*
      ** Check the return status from the software bus
      */ 
      if (Status == CFE_SUCCESS)
      {
         /* Process Software Bus message */
         MM_AppPipe(MM_AppData.MsgPtr);
      }
      else
      {
         /* 
         ** Exit on pipe read error
         */
         CFE_EVS_SendEvent(MM_PIPE_ERR_EID, CFE_EVS_ERROR,
                           "SB Pipe Read Error, App will exit. RC = 0x%08X", Status);         
          
         MM_AppData.RunStatus = CFE_ES_APP_ERROR;
          
      }
   } /* end CFS_ES_RunLoop while */

   /* 
   ** Performance Log exit stamp 
   */
   CFE_ES_PerfLogExit(MM_APPMAIN_PERF_ID);
   
   /* 
   ** Exit the application 
   */
   CFE_ES_ExitApp(MM_AppData.RunStatus); 

} /* end MM_AppMain */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* MM initialization                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 MM_AppInit(void)
{
   int32  Status = CFE_SUCCESS;

   /*
   ** MM doesn't use the critical data store and
   ** doesn't need to identify power on vs. processor resets.
   ** If this changes add it here as shown in the qq_app.c template
   */
    
   /*
   ** Setup the RunStatus variable
   */
   MM_AppData.RunStatus = CFE_ES_APP_RUN;
    
   /*
   ** Initialize application command execution counters
   */
   MM_AppData.CmdCounter = 0;
   MM_AppData.ErrCounter = 0;
    
   /*
   ** Initialize application configuration data
   */
   strcpy(MM_AppData.PipeName, "MM_CMD_PIPE");
   MM_AppData.PipeDepth = MM_CMD_PIPE_DEPTH;
    
   MM_AppData.LimitHK  = MM_HK_LIMIT;
   MM_AppData.LimitCmd = MM_CMD_LIMIT;
    
   /*
   ** Register for event services
   */
   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
   
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("MM App: Error Registering For Event Services, RC = 0x%08X\n", Status);
      return (Status);
   }
    
   /*
   ** Initialize the local housekeeping telemetry packet (clear user data area) 
   */
   CFE_SB_InitMsg(&MM_AppData.HkPacket, MM_HK_TLM_MID, sizeof(MM_HkPacket_t), TRUE);

   /* 
   ** Create Software Bus message pipe 
   */
   Status = CFE_SB_CreatePipe(&MM_AppData.CmdPipe, MM_AppData.PipeDepth, MM_AppData.PipeName);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("MM App: Error Creating SB Pipe, RC = 0x%08X\n", Status);
      return (Status);
   }    

   /* 
   ** Subscribe to Housekeeping request commands 
   */
   Status = CFE_SB_Subscribe(MM_SEND_HK_MID, MM_AppData.CmdPipe);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("MM App: Error Subscribing to HK Request, RC = 0x%08X\n", Status);
      return (Status);
   }    

   /* 
   ** Subscribe to MM ground command packets 
   */
   Status = CFE_SB_Subscribe(MM_CMD_MID, MM_AppData.CmdPipe);
   if (Status != CFE_SUCCESS)
   {
      CFE_ES_WriteToSysLog("MM App: Error Subscribing to MM Command, RC = 0x%08X\n", Status);
      return (Status);
   }    

   /*
   ** MM doesn't use tables. If this changes add table registration
   ** and initialization here as shown in the qq_app.c template
   */
   
   /*
   ** MM doesn't use the critical data store. If this changes add CDS 
   ** creation here as shown in the qq_app.c template
   */
    
   /* 
   ** Initialize MM housekeeping information 
   */
   MM_ResetHk();

   /* 
   ** Application startup event message 
   */
   CFE_EVS_SendEvent(MM_INIT_INF_EID, CFE_EVS_INFORMATION, 
                    "MM Initialized. Version %d.%d.%d.%d",
                     MM_MAJOR_VERSION,
                     MM_MINOR_VERSION,
                     MM_REVISION,
                     MM_MISSION_REV);

   return(CFE_SUCCESS);

} /* end MM_AppInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_AppPipe(CFE_SB_MsgPtr_t msg)
{
   CFE_SB_MsgId_t MessageID = 0;
   uint16 CommandCode = 0;
    
   MessageID = CFE_SB_GetMsgId(msg);
   switch (MessageID)
   {
      /* 
      ** Housekeeping telemetry request 
      */
      case MM_SEND_HK_MID:
         MM_HousekeepingCmd(msg);
         break;

      /* 
      ** MM ground commands
      */
      case MM_CMD_MID:
         MM_ResetHk(); /* Clear all "Last Action" data */
         CommandCode = CFE_SB_GetCmdCode(msg);
         switch (CommandCode)
         {
            case MM_NOOP_CC:
               MM_NoopCmd(msg);
               break;

            case MM_RESET_CC:
               MM_ResetCmd(msg);
               break;
            
            case MM_PEEK_CC:
               MM_PeekCmd(msg);
               break;
             
            case MM_POKE_CC:
               MM_PokeCmd(msg);
               break;
             
            case MM_LOAD_MEM_WID_CC:
               MM_LoadMemWIDCmd(msg);
               break;
         
            case MM_LOAD_MEM_FROM_FILE_CC:
               MM_LoadMemFromFileCmd(msg);
               break;
         
            case MM_DUMP_MEM_TO_FILE_CC:
               MM_DumpMemToFileCmd(msg);
               break;
                 
            case MM_DUMP_IN_EVENT_CC:
               MM_DumpInEventCmd(msg);
               break;
             
            case MM_FILL_MEM_CC:
               MM_FillMemCmd(msg);
               break; 

            case MM_LOOKUP_SYM_CC:
               MM_LookupSymbolCmd(msg);
               break; 

            case MM_SYMTBL_TO_FILE_CC:
               MM_SymTblToFileCmd(msg);
               break; 
               
            case MM_ENABLE_EEPROM_WRITE_CC:
               MM_EepromWriteEnaCmd(msg);
               break; 
               
            case MM_DISABLE_EEPROM_WRITE_CC:
               MM_EepromWriteDisCmd(msg);
               break; 
               
            default:
               MM_AppData.ErrCounter++;
               CFE_EVS_SendEvent(MM_CC1_ERR_EID, CFE_EVS_ERROR,
                                 "Invalid ground command code: ID = 0x%X, CC = %d",
                                 MessageID, CommandCode);
             break;
         } 
         break;

      /*
      ** Unrecognized Message ID
      */    
      default:
         MM_AppData.ErrCounter++;
         CFE_EVS_SendEvent(MM_MID_ERR_EID, CFE_EVS_ERROR,
                           "Invalid command pipe message ID: 0x%X", MessageID);
         break;
   
   } /* end switch */

   return;

} /* End MM_AppPipe */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Housekeeping request                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
   uint16 ExpectedLength = sizeof (MM_NoArgsCmd_t);
   
   /*
   ** Verify command packet length 
   */
   if (MM_VerifyCmdLength(msg, ExpectedLength))
   {
      /*
      ** Copy local housekeeping variables to packet structure
      */   
      MM_AppData.HkPacket.CmdCounter     = MM_AppData.CmdCounter;               
      MM_AppData.HkPacket.ErrCounter     = MM_AppData.ErrCounter;
      MM_AppData.HkPacket.LastAction     = MM_AppData.LastAction;
      MM_AppData.HkPacket.MemType        = MM_AppData.MemType;
      MM_AppData.HkPacket.Address        = MM_AppData.Address;
      MM_AppData.HkPacket.DataValue      = MM_AppData.DataValue;
      MM_AppData.HkPacket.BytesProcessed = MM_AppData.BytesProcessed;

      strncpy(MM_AppData.HkPacket.FileName, MM_AppData.FileName, OS_MAX_PATH_LEN);      
      
      /* 
      ** Send housekeeping telemetry packet 
      */
      CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &MM_AppData.HkPacket);
      CFE_SB_SendMsg((CFE_SB_Msg_t *) &MM_AppData.HkPacket);
      
      /*
      ** This command does not affect the command execution counter
      */

   } /* end if */
   
   return;

} /* end MM_HousekeepingCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_NoopCmd(CFE_SB_MsgPtr_t msg)
{
   uint16 ExpectedLength = sizeof(MM_NoArgsCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      MM_AppData.LastAction = MM_NOOP;
      MM_AppData.CmdCounter++;
      
      CFE_EVS_SendEvent(MM_NOOP_INF_EID, CFE_EVS_INFORMATION,
                       "No-op command. Version %d.%d.%d.%d",
                        MM_MAJOR_VERSION,
                        MM_MINOR_VERSION,
                        MM_REVISION,
                        MM_MISSION_REV);
   }

   return;

} /* end MM_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_ResetCmd(CFE_SB_MsgPtr_t msg)
{
   uint16 ExpectedLength = sizeof(MM_NoArgsCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      MM_AppData.LastAction = MM_RESET;
      MM_AppData.CmdCounter = 0;
      MM_AppData.ErrCounter = 0;
      
      CFE_EVS_SendEvent(MM_RESET_DBG_EID, CFE_EVS_DEBUG,
                        "Reset counters command received");
   }
   
   return;

} /* end MM_ResetCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Lookup symbol name command                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_LookupSymbolCmd(CFE_SB_MsgPtr_t msg)
{
   int32   OS_Status = OS_ERROR;  /* Set to error instead of success since we explicitly test for success */
   uint32  ResolvedAddr = 0;
   MM_LookupSymCmd_t  *CmdPtr = NULL;
   uint16 ExpectedLength = sizeof(MM_LookupSymCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      CmdPtr = ((MM_LookupSymCmd_t *) msg);
       
      /* 
      ** NUL terminate the very end of the symbol name string as a
      ** safety measure
      */
      CmdPtr->SymName[OS_MAX_SYM_LEN - 1] = '\0';
       
      /* 
      ** Check if the symbol name string is a nul string
      */ 
      if(strlen(CmdPtr->SymName) == 0)
       {
          MM_AppData.ErrCounter++;
          CFE_EVS_SendEvent(MM_SYMNAME_NUL_ERR_EID, CFE_EVS_ERROR,
                            "NUL (empty) string specified as symbol name");   
       }
      else
      {
         /* 
         ** If symbol name is not an empty string look it up using the OSAL API 
         */
         OS_Status = OS_SymbolLookup(&ResolvedAddr, CmdPtr->SymName);
         if (OS_Status == OS_SUCCESS)
          {
             /* Update telemetry */
             MM_AppData.LastAction = MM_SYM_LOOKUP;
             MM_AppData.Address    = ResolvedAddr;
             MM_AppData.CmdCounter++;
             
             CFE_EVS_SendEvent(MM_SYM_LOOKUP_INF_EID, CFE_EVS_INFORMATION,
                               "Symbol Lookup Command: Name = '%s' Addr = 0x%08X",
                               CmdPtr->SymName, ResolvedAddr);
          }
         else
         {
             MM_AppData.ErrCounter++;
             CFE_EVS_SendEvent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR,
                               "Symbolic address can't be resolved: Name = '%s'", 
                               CmdPtr->SymName);   
         }

      } /* end strlen(CmdPtr->SymName) == 0 else */
      
   } /* end MM_VerifyCmdLength if */
   
   return;

} /* end MM_LookupSymbolCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Dump symbol table to file command                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_SymTblToFileCmd(CFE_SB_MsgPtr_t msg)
{
   boolean                Valid = TRUE;
   int32                  OS_Status = OS_ERROR;  /* Set to error instead of success since we explicitly test for success */
   MM_SymTblToFileCmd_t  *CmdPtr = NULL;
   uint16                 ExpectedLength = sizeof(MM_SymTblToFileCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      CmdPtr = ((MM_SymTblToFileCmd_t *) msg);
       
      /* 
      ** NUL terminate the very end of the filename string as a
      ** safety measure
      */
      CmdPtr->FileName[OS_MAX_PATH_LEN - 1] = '\0';
       
      /* 
      ** Check if the filename string is a nul string
      */ 
      if(strlen(CmdPtr->FileName) == 0)
       {
          MM_AppData.ErrCounter++;
          CFE_EVS_SendEvent(MM_SYMFILENAME_NUL_ERR_EID, CFE_EVS_ERROR,
                            "NUL (empty) string specified as symbol dump file name");   
       }
      else
      {
         Valid = CFS_IsValidFilename(CmdPtr->FileName, strlen(CmdPtr->FileName));
         
         if (Valid == TRUE)
         {
             /* 
             ** If filename is good pass it to the OSAL API 
             */
             OS_Status = OS_SymbolTableDump(CmdPtr->FileName, MM_MAX_DUMP_FILE_DATA_SYMTBL);
             if (OS_Status == OS_SUCCESS)
             {
                 /* Update telemetry */
                 MM_AppData.LastAction = MM_SYMTBL_SAVE;
                 strncpy(MM_AppData.FileName, CmdPtr->FileName, OS_MAX_PATH_LEN);
                 MM_AppData.CmdCounter++;
             
                 CFE_EVS_SendEvent(MM_SYMTBL_TO_FILE_INF_EID, CFE_EVS_INFORMATION,
                                   "Symbol Table Dump to File Started: Name = '%s'",
                                   CmdPtr->FileName);
              }
              else
             {
                 MM_AppData.ErrCounter++;
                 CFE_EVS_SendEvent(MM_SYMTBL_TO_FILE_FAIL_ERR_EID, CFE_EVS_ERROR,
                                   "Error dumping symbol table, OS_Status= 0x%X, File='%s'", 
                                   OS_Status, CmdPtr->FileName);   
             }
         }
         else
         {
             MM_AppData.ErrCounter++;
             CFE_EVS_SendEvent(MM_SYMTBL_TO_FILE_INVALID_ERR_EID, CFE_EVS_ERROR,
                               "Illegal characters in target filename, File='%s'", 
                               CmdPtr->FileName);   
         }

      } /* end strlen(CmdPtr->FileName) == 0 else */
      
   } /* end MM_VerifyCmdLength if */
   
   return;

} /* end MM_SymTblToFileCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EEPROM write-enable command                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_EepromWriteEnaCmd(CFE_SB_MsgPtr_t msg)
{
   int32                    cFE_Status = CFE_PSP_ERROR;  /* Set to error since we explicitly test for success */
   MM_EepromWriteEnaCmd_t  *CmdPtr = NULL;
   uint16                   ExpectedLength = sizeof(MM_EepromWriteEnaCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      CmdPtr = ((MM_EepromWriteEnaCmd_t *) msg);
       
         /* 
         ** Call the cFE to write-enable the requested bank 
         */
         cFE_Status = CFE_PSP_EepromWriteEnable(CmdPtr->Bank);
         if (cFE_Status == CFE_PSP_SUCCESS)
          {
             /* Update telemetry */
             MM_AppData.LastAction = MM_EEPROMWRITE_ENA;
             MM_AppData.MemType    = MM_EEPROM;
             MM_AppData.CmdCounter++;
             
             CFE_EVS_SendEvent(MM_EEPROM_WRITE_ENA_INF_EID, CFE_EVS_INFORMATION,
                               "EEPROM bank %d write enabled, cFE_Status= 0x%X",
                               CmdPtr->Bank, cFE_Status);
          }
         else
         {
             MM_AppData.ErrCounter++;
             CFE_EVS_SendEvent(MM_EEPROM_WRITE_ENA_ERR_EID, CFE_EVS_ERROR,
                               "Error requesting EEPROM bank %d write enable, cFE_Status= 0x%X", 
                               CmdPtr->Bank, cFE_Status);   
         }
      
   } /* end MM_VerifyCmdLength if */
   
   return;

} /* end MM_EepromWriteEnaCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* EEPROM write-disable command                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void MM_EepromWriteDisCmd(CFE_SB_MsgPtr_t msg)
{
   int32                    cFE_Status = CFE_PSP_ERROR;  /* Set to error since we explicitly test for success */
   MM_EepromWriteDisCmd_t  *CmdPtr = NULL;
   uint16                   ExpectedLength = sizeof(MM_EepromWriteDisCmd_t);
   
   /* 
   ** Verify command packet length 
   */
   if(MM_VerifyCmdLength(msg, ExpectedLength))
   {
      CmdPtr = ((MM_EepromWriteDisCmd_t *) msg);
       
         /* 
         ** Call the cFE to write-enable the requested bank 
         */
         cFE_Status = CFE_PSP_EepromWriteDisable(CmdPtr->Bank);
         if (cFE_Status == CFE_PSP_SUCCESS)
          {
             /* Update telemetry */
             MM_AppData.LastAction = MM_EEPROMWRITE_DIS;
             MM_AppData.MemType    = MM_EEPROM;
             MM_AppData.CmdCounter++;
             
             CFE_EVS_SendEvent(MM_EEPROM_WRITE_DIS_INF_EID, CFE_EVS_INFORMATION,
                               "EEPROM bank %d write disabled, cFE_Status= 0x%X",
                               CmdPtr->Bank, cFE_Status);
          }
         else
         {
             MM_AppData.ErrCounter++;
             CFE_EVS_SendEvent(MM_EEPROM_WRITE_DIS_ERR_EID, CFE_EVS_ERROR,
                               "Error requesting EEPROM bank %d write disable, cFE_Status= 0x%X", 
                               CmdPtr->Bank, cFE_Status);   
         }
      
   } /* end MM_VerifyCmdLength if */
   
   return;

} /* end MM_EepromWriteDisCmd */

/************************/
/*  End of File Comment */
/************************/
