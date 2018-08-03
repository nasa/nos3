/*
** File: cfe_es_perf.c
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose: This file contains the functions that implement the software timing
**  performance markers.
**
** $Log: cfe_es_perf.c  $
** Revision 1.8 2012/01/13 11:50:03GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.7 2010/11/05 12:07:55EDT acudmore 
** When the write of a performance log to a file fails, reset the DataToWrite variable so a new dump can be done later.
** Revision 1.6 2010/11/04 17:19:17EDT jmdagost 
** Updated Performance Analyzer cmd to use Trigger Mode parameter.
** Revision 1.5 2010/11/04 14:11:56EDT acudmore 
** Swapped arguments to CFE_PSP_Get_Timebase - They were wrong.
** Revision 1.4 2010/09/21 16:09:40EDT jmdagost 
** Added cfe_es_start.h.
** Revision 1.3 2009/07/28 17:17:41EDT jmdagost 
** Changed initial trigger mask and filter mask entries to use definitions instead of hard-coded values
** Revision 1.2 2009/06/10 09:08:58EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.1 2008/04/17 08:05:07EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.24.1.4 2008/01/24 11:08:15EST njyanchik 
** I changed the creation of the file to the main task, so if it fails being created and opened, the error
** is reported from the main task and the correct counter can be incremented. The file desciptor is
** passed to the child task in the same structure as the file name.
** Revision 1.24.1.3 2007/09/21 15:40:21EDT David Kobe (dlkobe) 
** Modified pointer type definitions to eliminate Linux gcc compile warnings
** Revision 1.24.1.2 2007/09/21 13:33:17EDT rjmcgraw 
** DCR5037 Replace hardcoded numbers with new BSP function calls in PerfLogAdd
** Revision 1.24.1.1 2007/09/17 15:08:45EDT rjmcgraw
** DCR4900:3 Fixed trigger mask cmd, was incrementing cmd counter by two
** Revision 1.24 2007/08/31 10:57:21EDT rjmcgraw
** Changes to eliminate perl script
** Revision 1.23 2007/06/05 14:13:21EDT rjmcgraw
** Fixed file write parameter in the meta data write step
** Revision 1.22 2007/05/11 09:36:17EDT rjmcgraw
** Added 'in progress' checks to start and stop cmd
** Revision 1.21 2007/04/20 14:23:28EDT rjmcgraw
** Renamed performance structs for consistency
** Revision 1.20 2007/04/19 13:51:02EDT rjmcgraw
** Fixed Datafile pointer problem
** Revision 1.19 2007/04/17 10:57:59EDT rjmcgraw
** Added child task to do small block writes of performance data
** Revision 1.18 2007/04/11 16:42:33EDT rjmcgraw
** Added function CFE_ES_InvalidMarkerReported
** Revision 1.17 2007/04/10 13:56:01EDT rjmcgraw
** Removed Entry and Exit function definitions
** Revision 1.16 2007/04/06 14:50:10EDT rjmcgraw
** Event id name changes
** Revision 1.15 2007/04/04 08:14:53EDT njyanchik
** In DCR 3190, two API's were changed in the OSAL for locking out interrupts.
** This change package changes TIME's and ES's use of those APIs
** Revision 1.14 2007/04/03 11:46:38EDT apcudmore
** Added unique envent IDs and updated event calls.
**
*/

/*
** Include Section
*/

#include "osapi.h"
#include "private/cfe_private.h"
#include "cfe_es_perf.h"
#include "cfe_es_log.h"
#include "cfe_es_global.h"
#include "cfe_es_start.h"
#include "cfe_es_events.h"
#include "cfe_es_task.h"
#include "cfe_fs.h"
#include "cfe_psp.h"
#include <string.h>


/*
** Pointer to performance log in the reset area
*/
CFE_ES_PerfData_t      *Perf;
CFE_ES_PerfLogDump_t    CFE_ES_PerfLogDumpStatus;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Name: CFE_ES_SetupPerfVariables                                               */
/*                                                                               */
/* Purpose:This function initializes filter mask,trigger mask, data & state vals */
/*                                                                               */
/* Assumptions and Notes: This gets called from CFE_ES_Main() at startup         */
/* This code must be called before any other task or code that would use         */
/* CFE_ES_PerfLogEntry() / CFE_ES_PerfLogExit() functions                        */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_SetupPerfVariables(uint32 ResetType)
{
    /* Create a constant union -
     * The "Endian" field will have "0x01" on a big endian processor
     * and will have value "0x00" on a little endian processor.
     */
    const union
    {
        uint16 Word;
        uint8  Endian;
    } EndianCheck = { .Word = 0x0100 };


    uint32      i;

    /*
    ** Set the pointer to the data area
    */
    Perf = (CFE_ES_PerfData_t *)&(CFE_ES_ResetDataPtr->Perf);

    if ( ResetType == CFE_PSP_RST_TYPE_PROCESSOR )
    {
       /*
       ** On a processor reset, just IDLE the data
       ** collection so the ground can dump the data
       */
       Perf->MetaData.State = CFE_ES_PERF_IDLE;
    }
    else
    {

       Perf->MetaData.Version = 1;
       Perf->MetaData.Endian = EndianCheck.Endian;
       Perf->MetaData.TimerTicksPerSecond = CFE_PSP_GetTimerTicksPerSecond();
       Perf->MetaData.TimerLow32Rollover = CFE_PSP_GetTimerLow32Rollover();

       /* set data collection state to waiting for command state */
       Perf->MetaData.State = CFE_ES_PERF_IDLE;
       Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
       Perf->MetaData.TriggerCount = 0;
       Perf->MetaData.DataStart = 0;
       Perf->MetaData.DataEnd = 0;
       Perf->MetaData.DataCount = 0;
       Perf->MetaData.InvalidMarkerReported = FALSE;
       Perf->MetaData.FilterTriggerMaskSize = CFE_ES_PERF_32BIT_WORDS_IN_MASK;

       for (i=0; i < CFE_ES_PERF_32BIT_WORDS_IN_MASK; i++)
       {
          Perf->MetaData.FilterMask[i]  = CFE_ES_PERF_FILTMASK_INIT;
          Perf->MetaData.TriggerMask[i] = CFE_ES_PERF_TRIGMASK_INIT;
       }

    }

    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
    CFE_ES_PerfLogDumpStatus.ChildID = 0;
    CFE_ES_PerfLogDumpStatus.DataFileName[0] = '\0';
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_PerfStartDataCmd() --                                                  */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfStartDataCmd(CFE_SB_MsgPtr_t msg){

   uint16 ExpectedLength = sizeof(CFE_ES_PerfStartCmd_t);
   CFE_ES_PerfStartCmd_Payload_t *CmdPtr = (CFE_ES_PerfStartCmd_Payload_t *)&msg->Byte[CFE_SB_CMD_HDR_SIZE];

   /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(msg, ExpectedLength))
    {
      /* Ensure there is no file write in progress before proceeding */
      if(CFE_ES_PerfLogDumpStatus.DataToWrite == 0)
      {
          /* Make sure Trigger Mode is valid */
          /* cppcheck-suppress unsignedPositive */
          if ((CmdPtr->TriggerMode >= CFE_ES_PERF_TRIGGER_START) && (CmdPtr->TriggerMode < CFE_ES_PERF_MAX_MODES))
          {

             CFE_ES_TaskData.CmdCounter++;

             Perf->MetaData.Mode = CmdPtr->TriggerMode;
             Perf->MetaData.TriggerCount = 0;
             Perf->MetaData.DataStart = 0;
             Perf->MetaData.DataEnd = 0;
             Perf->MetaData.DataCount = 0;
             Perf->MetaData.InvalidMarkerReported = FALSE;
             Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER; /* this must be done last */

             CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_EID, CFE_EVS_DEBUG,
                                 "Start collecting performance data cmd received, trigger mode = %d", 
                              (int)CmdPtr->TriggerMode);
          }
          else
          {
             CFE_ES_TaskData.ErrCounter++;
             CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID, CFE_EVS_ERROR,
                  "Cannot start collecting performance data, trigger mode (%d) out of range (%d to %d)", 
               (int)CmdPtr->TriggerMode, (int)CFE_ES_PERF_TRIGGER_START, (int)CFE_ES_PERF_TRIGGER_END);
          }/* end if */
       }
       else
       {
          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_ERR_EID, CFE_EVS_ERROR,
               "Cannot start collecting performance data,perf data write in progress");
       } /* end if */
    }/* end if */

} /* End of CFE_ES_PerfStartDataCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_PerfStopDataCmd() --                                                   */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfStopDataCmd(CFE_SB_MsgPtr_t Msg){

    CFE_ES_PerfStopCmd_Payload_t  *CmdPtr = (CFE_ES_PerfStopCmd_Payload_t *)&Msg->Byte[CFE_SB_CMD_HDR_SIZE];
    uint16 ExpectedLength = sizeof(CFE_ES_PerfStopCmd_t);
    int32 Stat;
    
    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {

      /* Ensure there is no file write in progress before proceeding */
      if(CFE_ES_PerfLogDumpStatus.DataToWrite == 0)
      {
          Perf->MetaData.State = CFE_ES_PERF_IDLE;

          /* Copy out the string, using default if unspecified */
          CFE_SB_MessageStringGet(CFE_ES_PerfLogDumpStatus.DataFileName, CmdPtr->DataFileName,
                  CFE_ES_DEFAULT_PERF_DUMP_FILENAME, OS_MAX_PATH_LEN, sizeof(CmdPtr->DataFileName));

          /* Create the file to dump to */
          CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_creat(&CFE_ES_PerfLogDumpStatus.DataFileName[0], OS_WRITE_ONLY);
          
          
          if(CFE_ES_PerfLogDumpStatus.DataFileDescriptor < 0)
          {
              CFE_ES_TaskData.ErrCounter++;
              CFE_EVS_SendEvent(CFE_ES_PERF_LOG_ERR_EID,CFE_EVS_ERROR,
                                "Error creating file %s, RC = 0x%08X",
                             &CFE_ES_PerfLogDumpStatus.DataFileName[0], (unsigned int)CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
          }
          else
          {
          
              /* Spawn a task to write the performance data to a file */
              Stat = CFE_ES_CreateChildTask(&CFE_ES_PerfLogDumpStatus.ChildID,
                                            CFE_ES_PERF_CHILD_NAME,
                                            CFE_ES_PerfLogDump,
                                            CFE_ES_PERF_CHILD_STACK_PTR,
                                            CFE_ES_PERF_CHILD_STACK_SIZE,
                                            CFE_ES_PERF_CHILD_PRIORITY,
                                            CFE_ES_PERF_CHILD_FLAGS);

              if(Stat == CFE_SUCCESS)
              {
                  /* Note: the file gets closed in the child task */
                  CFE_ES_TaskData.CmdCounter++;
                  CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_EID,CFE_EVS_DEBUG,
                                    "Perf Stop Cmd Rcvd,%s will write %d entries.%dmS dly every %d entries",
                                 CFE_ES_PERF_CHILD_NAME,(int)Perf->MetaData.DataCount,
                                 (int)CFE_ES_PERF_CHILD_MS_DELAY,(int)CFE_ES_PERF_ENTRIES_BTWN_DLYS);
              }
              else
              {
                  /* close the fd */
                  OS_close( CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
                  CFE_ES_TaskData.ErrCounter++;
                  CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_ERR1_EID, CFE_EVS_ERROR,
                                 "Stop performance data cmd,Error creating child task RC=0x%08X",(unsigned int)Stat);
              }/* end if */

          }/* end if fd < 0 */
          
      }/* if data to write == 0 */
      else
      {

          CFE_ES_TaskData.ErrCounter++;
          CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_ERR2_EID, CFE_EVS_ERROR,
                   "Stop performance data cmd ignored,perf data write in progress");
      }/* end if */

    }/* end VerifyCmdLength */

} /* End of CFE_ES_PerfStopDataCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Function:  CFE_ES_PerfLogDump()                                              */
/*                                                                               */
/*  Purpose:                                                                     */
/*    Write performance data to a file                                           */
/*                                                                               */
/*  Arguments:                                                                   */
/*    None                                                                       */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfLogDump(void){

    int32               WriteStat;
    uint32              i;
    uint32              FileSize;
    CFE_FS_Header_t     FileHdr;

    CFE_ES_RegisterChildTask();


    /* Zero cFE header, then fill in fields */
    CFE_FS_InitHeader(&FileHdr, CFE_ES_PERF_LOG_DESC, CFE_FS_ES_PERFDATA_SUBTYPE);

    /* write the cFE header to the file */
    WriteStat = CFE_FS_WriteHeader( CFE_ES_PerfLogDumpStatus.DataFileDescriptor, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t))
    {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_FS_Header_t),WriteStat);
        
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
    }/* end if */
    FileSize = WriteStat;

    /* write the performance metadata to the file */
    WriteStat = OS_write(CFE_ES_PerfLogDumpStatus.DataFileDescriptor,&Perf->MetaData,sizeof(CFE_ES_PerfMetaData_t));
    if(WriteStat != sizeof(CFE_ES_PerfMetaData_t))
    {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_ES_PerfMetaData_t),WriteStat);
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
    }/* end if */
    FileSize += WriteStat;

    CFE_ES_PerfLogDumpStatus.DataToWrite = Perf->MetaData.DataCount;

    /* write the collected data to the file */
    for(i=0; i < Perf->MetaData.DataCount; i++){
      WriteStat = OS_write (CFE_ES_PerfLogDumpStatus.DataFileDescriptor, &Perf->DataBuffer[i], sizeof(CFE_ES_PerfDataEntry_t));
      if(WriteStat != sizeof(CFE_ES_PerfDataEntry_t))
      {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_ES_PerfDataEntry_t),WriteStat);
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        /* Reset the DataToWrite variable, so a new file can be written */
        CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
      }/* end if */
      FileSize += WriteStat;
      CFE_ES_PerfLogDumpStatus.DataToWrite--;
      if((i % CFE_ES_PERF_ENTRIES_BTWN_DLYS) == 0){
        OS_TaskDelay(CFE_ES_PERF_CHILD_MS_DELAY);
      }/* end if */

    }/* end for */

    OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);

    CFE_EVS_SendEvent(CFE_ES_PERF_DATAWRITTEN_EID,CFE_EVS_DEBUG,
                      "%s written:Size=%d,EntryCount=%d",
                       &CFE_ES_PerfLogDumpStatus.DataFileName[0],(int)FileSize,
                       (int)Perf->MetaData.DataCount);

    CFE_ES_ExitChildTask();

}/* end CFE_ES_PerfLogDump */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_PerfSetFilterMaskCmd() --                                              */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfSetFilterMaskCmd(CFE_SB_MsgPtr_t msg){

    uint16 ExpectedLength = sizeof(CFE_ES_PerfSetFilterMaskCmd_t);
    CFE_ES_PerfSetFilterMaskCmd_Payload_t *cmd =
            (CFE_ES_PerfSetFilterMaskCmd_Payload_t *) &msg->Byte[CFE_SB_CMD_HDR_SIZE];

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(msg, ExpectedLength))
    {

         if(cmd->FilterMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK ){

            Perf->MetaData.FilterMask[cmd->FilterMaskNum] = cmd->FilterMask;

            CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKCMD_EID, CFE_EVS_DEBUG,
                        "Set Performance Filter Mask Cmd rcvd, num %d, val 0x%x",
            (int)cmd->FilterMaskNum,(unsigned int)cmd->FilterMaskNum);

            CFE_ES_TaskData.CmdCounter++;
         }else{
            CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKERR_EID, CFE_EVS_ERROR,
                      "Performance Filter Mask Cmd Error,Index(%d)out of range(%d)",
            (int)cmd->FilterMaskNum,(int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

            CFE_ES_TaskData.ErrCounter++;

        }
    }
} /* End of CFE_ES_PerfSetFilterMaskCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_PerfSetTriggerMaskCmd() --                                             */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfSetTriggerMaskCmd(CFE_SB_MsgPtr_t msg){

   uint16 ExpectedLength = sizeof(CFE_ES_PerfSetTrigMaskCmd_t);
   CFE_ES_PerfSetTrigMaskCmd_Payload_t *cmd = (CFE_ES_PerfSetTrigMaskCmd_Payload_t *) &msg->Byte[CFE_SB_CMD_HDR_SIZE];

   /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(msg, ExpectedLength))
    {

       if(cmd->TriggerMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK ){

            Perf->MetaData.TriggerMask[cmd->TriggerMaskNum] = cmd->TriggerMask;

            CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKCMD_EID, CFE_EVS_DEBUG,
                      "Set Performance Trigger Mask Cmd rcvd,num %d, val 0x%x",
                   (int)cmd->TriggerMaskNum,(int)cmd->TriggerMaskNum);

            CFE_ES_TaskData.CmdCounter++;

       }else{
           CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKERR_EID, CFE_EVS_ERROR,
                      "Performance Trigger Mask Cmd Error,Index(%d)out of range(%d)",
                   (int)cmd->TriggerMaskNum,(int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

            CFE_ES_TaskData.ErrCounter++;
       }
    }
} /* End of CFE_ES_PerfSetTriggerMaskCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Name: CFE_ES_PerfLogAdd                                                       */
/*                                                                               */
/* Purpose: This function adds a new entry to the data buffer.                   */
/*                                                                               */
/* Assumptions and Notes:                                                        */
/*                                                                               */
/*  This function implements a circular buffer using an array.                   */
/*      DataStart points to first stored entry                                   */
/*      DataEnd points to next available entry                                   */
/*      if DataStart == DataEnd then the buffer is either empty or full          */
/*      depending on the value of the DataCount                                  */
/*                                                                               */
/*  Time is stored as 2 32 bit integers, (TimerLower32, TimerUpper32):           */
/*      TimerLower32 is the curent value of the hardware timer register.         */
/*      TimerUpper32 is the number of times the timer has rolled over.           */
/*                                                                               */
/*  Time is stored as a absolute time instead of a relative time between log     */
/*  entries. This will yield better accuracy since storing relative time between */
/*  entries will accumulate (rounding/sampling) errors over time.  It also is    */
/*  faster since the time does not need to be calculated.                        */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    int32         IntFlags;

    if (Perf->MetaData.State != CFE_ES_PERF_IDLE) {

        /* if marker is out of range... */
        if(Marker >= CFE_ES_PERF_MAX_IDS){

          /* if marker has not been reported previously ... */
          if(Perf->MetaData.InvalidMarkerReported == FALSE){
            CFE_ES_WriteToSysLog("ES PERF:Invalid performance marker %d,max is %d\n",
                    (unsigned int)Marker,(CFE_ES_PERF_MAX_IDS - 1));
            Perf->MetaData.InvalidMarkerReported = TRUE;
          }/* end if */

          return;

        }/* end if */


        /* is this id filtered */
        if (CFE_ES_TEST_LONG_MASK(Perf->MetaData.FilterMask, Marker)) {

           /* disable interrupts to guarentee exclusive access to the data structures */
            IntFlags = OS_IntLock();

            Perf->DataBuffer[Perf->MetaData.DataEnd].Data = (Marker | (EntryExit << CFE_ES_PERF_EXIT_BIT));
            CFE_PSP_Get_Timebase((uint32*)&Perf->DataBuffer[Perf->MetaData.DataEnd].TimerUpper32,(uint32*)&Perf->DataBuffer[Perf->MetaData.DataEnd].TimerLower32);

            Perf->MetaData.DataEnd++;
            if (Perf->MetaData.DataEnd >= CFE_ES_PERF_DATA_BUFFER_SIZE) {
                Perf->MetaData.DataEnd = 0;
            }

            /* we have filled up the buffer */
            Perf->MetaData.DataCount++;
            if (Perf->MetaData.DataCount > CFE_ES_PERF_DATA_BUFFER_SIZE) {

                Perf->MetaData.DataCount = CFE_ES_PERF_DATA_BUFFER_SIZE;

                /* after the buffer fills up start and end point to the same entry since we
                   are now overwriting old data */
                Perf->MetaData.DataStart = Perf->MetaData.DataEnd;
            }

            /* waiting for trigger */
            if (Perf->MetaData.State == CFE_ES_PERF_WAITING_FOR_TRIGGER) {

                if (CFE_ES_TEST_LONG_MASK(Perf->MetaData.TriggerMask, Marker)) {
                    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
                }
            }
            /* triggered */
            if (Perf->MetaData.State == CFE_ES_PERF_TRIGGERED) {

                Perf->MetaData.TriggerCount++;
                if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START) {

                    if (Perf->MetaData.TriggerCount >= CFE_ES_PERF_DATA_BUFFER_SIZE) {
                        Perf->MetaData.State = CFE_ES_PERF_IDLE;
                    }
                }
                else if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER) {

                    if (Perf->MetaData.TriggerCount >= CFE_ES_PERF_DATA_BUFFER_SIZE / 2) {
                        Perf->MetaData.State = CFE_ES_PERF_IDLE;
                    }
                }
                else if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END) {

                    Perf->MetaData.State = CFE_ES_PERF_IDLE;
                }
            }

            /* enable interrupts */
            OS_IntUnlock(IntFlags);
        }
    }
}/* end CFE_ES_PerfLogAdd */


