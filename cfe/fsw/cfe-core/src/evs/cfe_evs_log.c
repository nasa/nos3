/*
**
**  File Name: cfe_evslog.c
**  $Id: cfe_evs_log.c 1.11 2014/08/22 16:53:23GMT-05:00 lwalling Exp  $
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
**
**
**
**  Title: Event Services API - Log Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           Event Services Log control interfaces
**
** $Date: 2014/08/22 16:53:23GMT-05:00 $
** $Revision: 1.11 $
** $Log: cfe_evs_log.c  $
** Revision 1.11 2014/08/22 16:53:23GMT-05:00 lwalling 
** Change signed loop counters to unsigned
** Revision 1.10 2012/01/13 12:00:54EST acudmore 
** Changed license text to reflect open source
** Revision 1.9 2011/06/01 17:45:26EDT lwalling 
** Update Add Log Entry comments, Write Log File logic and events, Set Log Mode events
** Revision 1.8 2011/06/01 10:32:42EDT lwalling 
** Remove unused reference to internal ES header file
** Revision 1.7 2011/06/01 10:19:50EDT lwalling 
** Modify function CFE_EVS_WriteLogFileCmd() to match prototype
** Revision 1.6 2011/05/23 15:57:11EDT lwalling 
** Change startup to not fail if unable to create event log, verify reset area size and event log contents
** Revision 1.5 2011/04/05 16:33:42EDT lwalling 
** Optimize EVS use of string functions, plus other performance improvements
** Revision 1.4 2010/10/04 17:08:18EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2010/09/21 16:12:12EDT jmdagost 
** Added conditional statements around conditionally-used local variables.
** Revision 1.2 2009/06/10 09:12:42EDT acudmore 
** Converted OS_Mem* and OS_BSP* calls to CFE_PSP_*
** Revision 1.1 2008/04/17 08:05:12EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
** Revision 1.24 2007/07/18 15:47:59EDT njyanchik 
** Jonathan added text into each error event in the two functions that write files (WriteLog and WriteAppData), as well as updated the doxygen comments in the header file
** Revision 1.23 2007/05/14 10:26:03EDT apcudmore 
** Preserve the EVS Local event log on a processor restart:
** - moved the EVS Log to the ES reset data structure
** - Added a couple of EVS Log related variables into the EVS_Log_t structure
** - Added a routine to the EVS early init to get a pointer to the EVS Log 
** - Added code to the EVS housekeeping service routine to copy Log variables into TLM
** Revision 1.22 2007/04/03 09:27:29EDT njyanchik 
** I added the SubType to the cFE file header for the event log and the write app data files in EVS
** Revision 1.21 2007/03/02 10:59:57EST njyanchik 
** The names of the command functions in cfe_evs_task.c didn't start with "CFE_", so that has been 
** fixed.
** Revision 1.20 2007/03/01 11:54:33EST njyanchik 
** This cp handles issues 1,6,7,8 as described in the DCR
** Revision 1.19 2006/11/14 15:02:55EST rjmcgraw 
** Removed file open check
** Revision 1.18 2006/10/20 16:26:59EDT kkaudra 
** Added cmd parameter for filenames
** Revision 1.17 2006/09/01 11:17:13EDT kkaudra 
** IV&V:Removed osapi-os-filesys.h
** Revision 1.16 2006/08/31 17:50:56EDT kkaudra 
** Added event messages to set log mode ()
** Revision 1.15 2006/06/08 14:14:53EDT njyanchik 
** added the appropriate legal headers to all of the evs files
**
*/

/* Include Files */
#include "cfe_evs_task.h"     /* EVS internal definitions */
#include "cfe_evs_log.h"      /* EVS log file definitions */
#include "cfe_evs.h"          /* EVS API definitions */
#include "cfe_evs_utils.h"    /* EVS utility function definitions */
#include "cfe_fs.h"           /* File Service definitions */
#include "cfe_error.h"        /* cFE error code definitions */
#include "cfe_psp.h"          /* Get reset area function prototype */


#include <string.h>


/*
**             Function Prologue
**
** Function Name:      EVS_AddLog
**
** Purpose:  This routine adds an event packet to the internal event log.
**
** Assumptions and Notes:
**
*/
void EVS_AddLog (CFE_EVS_Packet_t *EVS_PktPtr)
{

   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled == TRUE)
   {   
      /* Serialize access to event log control variables */
      OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

      if ((CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE) &&
          (CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LOG_DISCARD))
      {
         /* If log is full and in discard mode, just count the event */
         CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter++;
      }
      else
      {
         if (CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE)
         {
            /* If log is full and in wrap mode, count it and store it */
            CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter++;
         }

         /* Copy the event data to the next available entry in the log */
         CFE_PSP_MemCpy(&CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[CFE_EVS_GlobalData.EVS_LogPtr->Next],
                        EVS_PktPtr, sizeof(CFE_EVS_Packet_t));

         CFE_EVS_GlobalData.EVS_LogPtr->Next++;

         if (CFE_EVS_GlobalData.EVS_LogPtr->Next >= CFE_EVS_LOG_MAX)
         {
            /* This is important, even if we are in discard mode */
            CFE_EVS_GlobalData.EVS_LogPtr->Next = 0;
         }

         /* Log count cannot exceed the number of entries in the log */
         if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount < CFE_EVS_LOG_MAX)
         {
            CFE_EVS_GlobalData.EVS_LogPtr->LogCount++;

            if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount == CFE_EVS_LOG_MAX)
            {
               /* The full flag and log count are somewhat redundant */
               CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = TRUE;
            }
         }
      }
    
      OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);
   }

   return;

} /* End EVS_AddLog */


/*
**             Function Prologue
**
** Function Name:      EVS_ClearLog
**
** Purpose:  This routine clears the contents of the internal event log.
**
** Assumptions and Notes:
**
*/
void EVS_ClearLog ( void )
{

   /* Serialize access to event log control variables */
   OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   /* Clears everything but LogMode (overwrite vs discard) */
   CFE_EVS_GlobalData.EVS_LogPtr->Next = 0;
   CFE_EVS_GlobalData.EVS_LogPtr->LogCount = 0;
   CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = FALSE;
   CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter = 0;

   CFE_PSP_MemSet(CFE_EVS_GlobalData.EVS_LogPtr->LogEntry, 0,
                  CFE_EVS_LOG_MAX * sizeof(CFE_EVS_Packet_t));

   OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   return;

} /* End EVS_ClearLog */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_WriteLogFileCmd
**
** Purpose:  This routine writes the contents of the internal event log to a file
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_WriteLogFileCmd (CFE_SB_MsgPayloadPtr_t Payload)
{
   const CFE_EVS_LogFileCmd_Payload_t *CmdPtr = (const CFE_EVS_LogFileCmd_Payload_t *)Payload;
   boolean         Result = FALSE;
   int32           LogIndex;
   int32           BytesWritten;
   int32           LogFileHandle;
   uint32          i;
   CFE_FS_Header_t LogFileHdr;
   char            LogFilename[OS_MAX_PATH_LEN];


   /* Serialize access to event log control variables */
   OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
   CFE_SB_MessageStringGet(LogFilename, (const char *)CmdPtr->LogFilename, CFE_EVS_DEFAULT_LOG_FILE,
           OS_MAX_PATH_LEN, sizeof(CmdPtr->LogFilename));

   /* Create the log file */
   LogFileHandle = OS_creat(LogFilename, OS_WRITE_ONLY);

   if (LogFileHandle < OS_FS_SUCCESS)
   {
      EVS_SendEvent(CFE_EVS_ERR_CRLOGFILE_EID, CFE_EVS_ERROR,
                   "Write Log File Command Error: OS_creat = 0x%08X, filename = %s",
                    LogFileHandle, LogFilename);
   }
   else
   {
      /* Initialize cFE file header for an event log file */
      CFE_FS_InitHeader(&LogFileHdr, "cFE EVS Log File", CFE_FS_EVS_EVENTLOG_SUBTYPE);

      /* Write the file header to the log file */
      BytesWritten = CFE_FS_WriteHeader(LogFileHandle, &LogFileHdr);

      if (BytesWritten == sizeof(CFE_FS_Header_t))
      {
         /* Is the log full? -- Doesn't matter if wrap mode is enabled */
         if (CFE_EVS_GlobalData.EVS_LogPtr->LogCount == CFE_EVS_LOG_MAX)
         {
            /* Start with log entry that will be overwritten next (oldest) */
            LogIndex = CFE_EVS_GlobalData.EVS_LogPtr->Next;
         }
         else
         {
            /* Start with the first entry in the log (oldest) */
            LogIndex = 0;
         }

         /* Write all the "in-use" event log entries to the file */
         for (i = 0; i < CFE_EVS_GlobalData.EVS_LogPtr->LogCount; i++)
         {
            BytesWritten = OS_write(LogFileHandle,
                                    &CFE_EVS_GlobalData.EVS_LogPtr->LogEntry[LogIndex],
                                    sizeof(CFE_EVS_Packet_t));

            if (BytesWritten == sizeof(CFE_EVS_Packet_t))
            {
               LogIndex++;

               if (LogIndex >= CFE_EVS_LOG_MAX)
               {
                  LogIndex = 0;
               }
            }
            else
            {
               EVS_SendEvent(CFE_EVS_ERR_WRLOGFILE_EID, CFE_EVS_ERROR,
                            "Write Log File Command Error: OS_write = 0x%08X, filename = %s",
                            (unsigned int)BytesWritten, LogFilename);
               break;
            }
         }

         /* Process command handler success result */
         if (i == CFE_EVS_GlobalData.EVS_LogPtr->LogCount)
         {
            EVS_SendEvent(CFE_EVS_WRLOG_EID, CFE_EVS_DEBUG,
                         "Write Log File Command: %d event log entries written to %s",
                          (int)CFE_EVS_GlobalData.EVS_LogPtr->LogCount, LogFilename);
            Result = TRUE;
         }
      }

      OS_close(LogFileHandle);
   }

   OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

   return(Result);

} /* End CFE_EVS_WriteLogFileCmd */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SetLoggingModeCmd
**
** Purpose:  This routine sets the internal event log mode.
**
** Assumptions and Notes:
**
*/
boolean CFE_EVS_SetLoggingModeCmd (CFE_SB_MsgPayloadPtr_t Payload)
{
   CFE_EVS_ModeCmd_Payload_t *CmdPtr = (CFE_EVS_ModeCmd_Payload_t *)Payload;
   boolean Status = TRUE;

   if ((CmdPtr->Mode == CFE_EVS_LOG_OVERWRITE) || (CmdPtr->Mode == CFE_EVS_LOG_DISCARD))
   {
      /* Serialize access to event log control variables */
      OS_MutSemTake(CFE_EVS_GlobalData.EVS_SharedDataMutexID);
      CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CmdPtr->Mode;
      OS_MutSemGive(CFE_EVS_GlobalData.EVS_SharedDataMutexID);

      EVS_SendEvent(CFE_EVS_LOGMODE_EID, CFE_EVS_DEBUG,
                   "Set Log Mode Command: Log Mode = %d", (int)CmdPtr->Mode);
   }
   else
   {
      Status = FALSE;
      EVS_SendEvent(CFE_EVS_ERR_LOGMODE_EID, CFE_EVS_ERROR,
                   "Set Log Mode Command Error: Log Mode = %d", (int)CmdPtr->Mode);
   }

   return Status;

} /* End CFE_EVS_SetLoggingModeCmd */

/* END EVSLOG.C */
