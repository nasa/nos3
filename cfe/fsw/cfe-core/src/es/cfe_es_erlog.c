/*
**  File:  
**    cfe_es_erlog.c
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
**    This file implements the cFE Executive Services Exception and Reset Log functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
** $Log: cfe_es_erlog.c  $
** Revision 1.3 2012/01/13 11:50:02GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.2 2009/06/10 09:08:56EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.1 2008/04/17 08:05:05EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.5 2007/05/30 10:26:06EDT apcudmore 
** Added Exception handler support:
**  - BSP routine to attach processor exceptions
**  - context datatype and size define
**  - added default exception init for each app+task
** Revision 1.4 2007/03/22 10:26:22EST apcudmore 
** Implemented all code walkthrough items related to ES Application logic, commands, and telemetry.
** Revision 1.3 2007/03/13 16:10:54EST apcudmore 
** Implemented all ERLog and ES API code walkthrough items.
** Revision 1.2 2006/11/13 16:56:48EST apcudmore 
** Added call to get time in ER Log.
** Revision 1.1 2006/11/13 16:35:24EST apcudmore 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**
*/

/*
** Required header files.
*/
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_global.h"
#include "cfe_es_log.h"
#include "cfe_psp.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
 * Confirm the size of the error log context buffer is at least what the user asked for.
 *
 * This is to catch errors such as if the CFE_ES_ER_LOG_MAX_CONTEXT_SIZE was set to a value
 * that is _not_ a multiple of sizeof(uint32) -- in this case the final size of the context
 * buffer would end up being less than what the macro was set to.
 */
CompileTimeAssert(sizeof(CFE_ES_ResetDataPtr->ERLog[0].Context) == CFE_ES_ER_LOG_MAX_CONTEXT_SIZE,  CfeEsErLogContextSizeError);


/*
** Function: CFE_ES_WriteToERLog
**
** Purpose:  Create an entry in the ES Exception and Reset Log. 
**
*/
int32 CFE_ES_WriteToERLog( uint32 EntryType,   uint32  ResetType, uint32 ResetSubtype,
                           const char  *Description, const uint32 *Context,   uint32 ContextSize )
{
   uint32 LogIdx;

   /*
   ** Code
   */

   /*
   ** Try to clean up an invalid ER log index variable.
   */
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }
   LogIdx = CFE_ES_ResetDataPtr->ERLogIndex;

   /*
   ** Now that the Local Index variable is set, increment the index for the next entry.
   */
   CFE_ES_ResetDataPtr->ERLogIndex++;
   if ( CFE_ES_ResetDataPtr->ERLogIndex >= CFE_ES_ER_LOG_ENTRIES )
   {
      CFE_ES_ResetDataPtr->ERLogIndex = 0;
   }


   /*
   ** Clear out the log entry we are about to use.
   */
   CFE_PSP_MemSet( (void *)&(CFE_ES_ResetDataPtr->ERLog[LogIdx]), 0, 
            sizeof (CFE_ES_ERLog_t));

   /*
   ** Fill out the log fields
   */
   CFE_ES_ResetDataPtr->ERLog[LogIdx].LogEntryType = EntryType;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ResetType    = ResetType;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ResetSubtype = ResetSubtype;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].BootSource   = CFE_ES_ResetDataPtr->ResetVars.BootSource;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].ProcessorResetCount = 
                               CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount;
   CFE_ES_ResetDataPtr->ERLog[LogIdx].MaxProcessorResetCount = 
                               CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;

   /*
   ** Copy the ES Reset variables to the log (before they are modified by the log entry).
   */
   CFE_PSP_MemCpy((void *)&(CFE_ES_ResetDataPtr->ERLog[LogIdx].DebugVars), 
          (void *)&(CFE_ES_Global.DebugVars), 
          sizeof(CFE_ES_DebugVariables_t ));

   /*
   ** Time Stamp the log entry with the system time 
   */
   CFE_ES_ResetDataPtr->ERLog[LogIdx].TimeCode = CFE_TIME_GetTime(); 

   /*
   ** Copy the Description string to the log.
   */
   if ( Description  == NULL)
   {
      strncpy(CFE_ES_ResetDataPtr->ERLog[LogIdx].Description, "No Description String Given.", 80);
   }
   else
   {
      strncpy(CFE_ES_ResetDataPtr->ERLog[LogIdx].Description, Description, 80);
   }

   /*
   ** In the case of an exception, copy the processor context data to the log.
   */
   if (Context != NULL && ContextSize > 0)
   {
      /*
      ** Copy the processor context data (i.e. register dump).  Make sure that
      ** the passed-in context_size is not greater than the declared size in
      ** the ER Log log entry.
      */
      if ( ContextSize <= CFE_ES_ER_LOG_MAX_CONTEXT_SIZE )
      {
         CFE_PSP_MemCpy ( (void *)(CFE_ES_ResetDataPtr->ERLog[LogIdx].Context),
                  (void *)Context,
                  ContextSize);
      }
      else
      {
         CFE_PSP_MemCpy ( (void *)(CFE_ES_ResetDataPtr->ERLog[LogIdx].Context),
                  (void *)Context,
                  CFE_ES_ER_LOG_MAX_CONTEXT_SIZE);
      }   
      /*
      ** Indicate that context is valid.
      ** Using the original context size (not the truncated size) so it will be
      ** evident if the context information was truncated.
      */
      CFE_ES_ResetDataPtr->ERLog[LogIdx].ContextSize = ContextSize;
   }
   else
   {
      /*
      ** Context is not valid
      */
      CFE_ES_ResetDataPtr->ERLog[LogIdx].ContextSize = 0;
   } /* end if */

   /*
   ** Increment the number of ER log entries made
   */
   CFE_ES_ResetDataPtr->ERLogEntries++;

   return(CFE_SUCCESS);
    
} /* End of CFE_ES_WriteToERLog() */

/* end of file */
