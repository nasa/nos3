/*
** $Id: cfe_tbl_task_cmds.c 1.15 2014/08/22 16:30:24GMT-05:00 lwalling Exp  $
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
** Subsystem: cFE TBL Task Command Processing Functions
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
** $Log: cfe_tbl_task_cmds.c  $
** Revision 1.15 2014/08/22 16:30:24GMT-05:00 lwalling 
** Change signed loop counters to unsigned
** Revision 1.14 2014/06/09 10:28:32EDT lwalling 
** Store name of last table loaded in housekeeping, modify comments when storing last table updated info
** Revision 1.13 2012/02/22 15:13:33EST lwalling 
** Remove obsolete TODO comments
** Revision 1.12 2012/01/18 16:32:20EST jmdagost 
** Updated no-op event msg to include cFE version numbers.
** Revision 1.11 2012/01/13 12:17:40EST acudmore 
** Changed license text to reflect open source
** Revision 1.10 2011/11/14 17:59:52EST lwalling 
** Event EID mentioned in previous log entry should have been CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID
** Revision 1.9 2011/11/14 17:43:02EST lwalling 
** Modified event text and argument list for CFE_TBL_FILE_INCOMPLETE_ERR_EID
** Revision 1.8 2010/10/27 16:36:49EDT dkobe 
** Added computation and reporting of Table CRCs to table load and registry reporting commands
** Revision 1.7 2010/10/27 13:57:56EDT dkobe 
** Added calls to send notification messages when table commands are processed.
** Revision 1.6 2010/10/25 15:00:30EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.5 2010/10/04 15:18:54EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.4 2009/06/10 09:20:11EDT acudmore 
** Changed OS_Mem* and OS_BSP* calls to CFE_PSP_* calls
** Revision 1.3 2008/12/08 16:10:56EST dkobe 
** Correcting errors generated during detailed design document generation
** Revision 1.2 2008/07/29 14:05:34EDT dkobe 
** Removed redundant FileCreateTimeSecs and FileCreateTimeSubSecs fields
** Revision 1.1 2008/04/17 08:05:36EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/tbl/project.pj
** Revision 1.43 2007/09/12 16:06:11EDT David Kobe (dlkobe) 
** Moved the definitions of CFE_ES_CRC_xx to the cfe_mission_cfg.h file and deleted TBL Services
** CRC #define statement.
** Revision 1.42 2007/07/07 09:20:35EDT dlkobe 
** Added check for pending load in Load Cmd Processing
** Revision 1.41 2007/07/07 07:51:10EDT dlkobe 
** Added CFE_TBL_ASSUMED_VALID_INFO_EID event message
** Revision 1.40 2007/07/05 15:38:36EDT dlkobe 
** Added Critical Table Flag to Registry Dump/Tlm Cmds
**
*/


/*
** Required header files
*/
#include "cfe_version.h"
#include "cfe_evs.h"
#include "cfe_es.h"
#include "cfe_sb.h"
#include "cfe_fs.h"
#include "cfe_psp.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_events.h"
#include "cfe_tbl_msg.h"
#include "cfe_tbl_task_cmds.h"
#include <string.h>


/*******************************************************************
**
** CFE_TBL_HousekeepingCmd() -- Process Housekeeping Request Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_HousekeepingCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    int32                     Status;
    uint32                    i;
    CFE_TBL_DumpControl_t    *DumpCtrlPtr;
    CFE_TIME_SysTime_t        DumpTime;
    int32                     FileDescriptor;
    
    /*
    ** Collect housekeeping data from Table Services
    */
    CFE_TBL_GetHkData();

    /*
    ** Send housekeeping telemetry packet
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.HkPacket);
    Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.HkPacket);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_TBL_FAIL_HK_SEND_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to send Hk Packet (Status=0x%08X)",
                          (unsigned int)Status);
    }

    /* If a table's registry entry has been requested for telemetry, then pack it and send it */
    if (CFE_TBL_TaskData.HkTlmTblRegIndex != CFE_TBL_NOT_FOUND)
    {
        CFE_TBL_GetTblRegData();

        /*
        ** Send Table Registry Info Packet
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.TblRegPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CFE_TBL_TaskData.TblRegPacket);

        /* Once the data has been sent, clear the index so that we don't send it again and again */
        CFE_TBL_TaskData.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    }
    
    /* Check to see if there are any dump-only table dumps pending */
    for (i=0; i < CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        if (CFE_TBL_TaskData.DumpControlBlocks[i].State == CFE_TBL_DUMP_PERFORMED)
        {
        	DumpCtrlPtr = &CFE_TBL_TaskData.DumpControlBlocks[i];
            Status = CFE_TBL_DumpToFile(DumpCtrlPtr->DumpBufferPtr->DataSource,
                                        DumpCtrlPtr->TableName,
                                        DumpCtrlPtr->DumpBufferPtr->BufferPtr,
                                        DumpCtrlPtr->Size);
                                
            /* If dump file was successfully written, update the file header so that the timestamp */
            /* is the time of the actual capturing of the data, NOT the time when it was written to the file */        
            if (Status == CFE_TBL_INC_CMD_CTR)
            {
                DumpTime.Seconds = DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSecs;
                DumpTime.Subseconds = DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSubSecs;
            
                FileDescriptor = OS_open(DumpCtrlPtr->DumpBufferPtr->DataSource, OS_READ_WRITE, 0);

                if (FileDescriptor >= 0)
                {
                    Status = CFE_FS_SetTimestamp(FileDescriptor, DumpTime);
                    
                    if (Status != OS_FS_SUCCESS)
                    {
                        CFE_ES_WriteToSysLog("CFE_TBL:HkCmd-Unable to update timestamp in dump file '%s'\n", 
                                             DumpCtrlPtr->DumpBufferPtr->DataSource);
                    }
                    
                    OS_close(FileDescriptor);
                }
                else
                {
                    CFE_ES_WriteToSysLog("CFE_TBL:HkCmd-Unable to open dump file '%s' to update timestamp\n", 
                                         DumpCtrlPtr->DumpBufferPtr->DataSource);
                }
            }       
                        
            /* Free the shared working buffer */
            CFE_TBL_TaskData.LoadBuffs[DumpCtrlPtr->RegRecPtr->LoadInProgress].Taken = FALSE;
            DumpCtrlPtr->RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
            
            /* Free the Dump Control Block for later use */
            DumpCtrlPtr->State = CFE_TBL_DUMP_FREE;       
        }
    }

    return CFE_TBL_DONT_INC_CTR;

} /* End of CFE_TBL_HousekeepingCmd() */


/*******************************************************************
**
** CFE_TBL_GetHkData() -- Collect data and store it into the Housekeeping Message
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

void CFE_TBL_GetHkData(void)
{
    uint32 i;
    uint16 Count;
    CFE_TBL_ValidationResult_t *ValPtr = NULL;

    /* Copy command counter data */
    CFE_TBL_TaskData.HkPacket.Payload.CmdCounter = CFE_TBL_TaskData.CmdCounter;
    CFE_TBL_TaskData.HkPacket.Payload.ErrCounter = CFE_TBL_TaskData.ErrCounter;
    CFE_TBL_TaskData.HkPacket.Payload.FailedValCtr = CFE_TBL_TaskData.FailedValCtr;
    CFE_TBL_TaskData.HkPacket.Payload.NumLoadPending = 0;
    CFE_SB_SET_MEMADDR(CFE_TBL_TaskData.HkPacket.Payload.MemPoolHandle, CFE_TBL_TaskData.Buf.PoolHdl);

    /* Determine the number of tables currently registered and Number of Load Pending Tables */
    Count = 0;
    for (i=0; i<CFE_TBL_MAX_NUM_TABLES; i++)
    {
        if (CFE_TBL_TaskData.Registry[i].OwnerAppId != CFE_TBL_NOT_OWNED)
        {
            Count++;

            if (CFE_TBL_TaskData.Registry[i].LoadPending)
            {
                CFE_TBL_TaskData.HkPacket.Payload.NumLoadPending++;
            }
        }
    }
    CFE_TBL_TaskData.HkPacket.Payload.NumTables = Count;

    /* Determine the number of free shared buffers */
    CFE_TBL_TaskData.HkPacket.Payload.NumFreeSharedBufs = CFE_TBL_MAX_SIMULTANEOUS_LOADS;
    for (i=0; i<CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        if (CFE_TBL_TaskData.LoadBuffs[i].Taken)
        {
            CFE_TBL_TaskData.HkPacket.Payload.NumFreeSharedBufs--;
        }
    }

    /* Locate a completed, but unreported, validation request */
    i=0;
    while ((i < CFE_TBL_MAX_NUM_VALIDATIONS) && (ValPtr == NULL))
    {
        if (CFE_TBL_TaskData.ValidationResults[i].State == CFE_TBL_VALIDATION_PERFORMED)
        {
            ValPtr = &CFE_TBL_TaskData.ValidationResults[i];
        }
        else
        {
            i++;
        }
    }

    if (ValPtr != NULL)
    {
        CFE_TBL_TaskData.HkPacket.Payload.LastValCrc = ValPtr->CrcOfTable;
        CFE_TBL_TaskData.HkPacket.Payload.LastValStatus = ValPtr->Result;
        CFE_TBL_TaskData.HkPacket.Payload.ActiveBuffer = ValPtr->ActiveBuffer;

        /* Keep track of the number of failed and successful validations */
        if (ValPtr->Result == CFE_SUCCESS)
        {
            CFE_TBL_TaskData.SuccessValCtr++;
        }
        else
        {
            CFE_TBL_TaskData.FailedValCtr++;
        }

        CFE_SB_MessageStringSet(CFE_TBL_TaskData.HkPacket.Payload.LastValTableName, ValPtr->TableName,
                  sizeof(CFE_TBL_TaskData.HkPacket.Payload.LastValTableName), sizeof(ValPtr->TableName));
        CFE_TBL_TaskData.ValidationCtr++;

        /* Free the Validation Response Block for next time */
        ValPtr->Result = 0;
        ValPtr->CrcOfTable = 0;
        ValPtr->TableName[0] = '\0';
        ValPtr->ActiveBuffer = FALSE;
        ValPtr->State = CFE_TBL_VALIDATION_FREE;
    }

    CFE_TBL_TaskData.HkPacket.Payload.ValidationCtr  = CFE_TBL_TaskData.ValidationCtr;
    CFE_TBL_TaskData.HkPacket.Payload.SuccessValCtr  = CFE_TBL_TaskData.SuccessValCtr;
    CFE_TBL_TaskData.HkPacket.Payload.FailedValCtr   = CFE_TBL_TaskData.FailedValCtr;
    CFE_TBL_TaskData.HkPacket.Payload.NumValRequests = CFE_TBL_TaskData.NumValRequests;
    
    /* Validate the index of the last table updated before using it */
    if ((CFE_TBL_TaskData.LastTblUpdated >= 0) && 
        (CFE_TBL_TaskData.LastTblUpdated < CFE_TBL_MAX_NUM_TABLES))
    {
        /* Check to make sure the Registry Entry is still valid */
        if (CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.LastTblUpdated].OwnerAppId != CFE_TBL_NOT_OWNED)
        {
            /* Get the time at the last table update */
            CFE_TBL_TaskData.HkPacket.Payload.LastUpdateTime =
              CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.LastTblUpdated].TimeOfLastUpdate;

            /* Get the table name used for the last table update */
            CFE_SB_MessageStringSet(CFE_TBL_TaskData.HkPacket.Payload.LastUpdatedTbl,
                    CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.LastTblUpdated].Name,
                    sizeof(CFE_TBL_TaskData.HkPacket.Payload.LastUpdatedTbl),
                    sizeof(CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.LastTblUpdated].Name));
        }      
    }
}

/*******************************************************************
**
** CFE_TBL_GetTblRegData() -- Convert Table Registry Entry for a Table into a Message
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

void CFE_TBL_GetTblRegData(void)
{
    CFE_TBL_RegistryRec_t      *RegRecPtr;

    RegRecPtr = &CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.HkTlmTblRegIndex];

    CFE_TBL_TaskData.TblRegPacket.Payload.Size = RegRecPtr->Size;
    CFE_SB_SET_MEMADDR(CFE_TBL_TaskData.TblRegPacket.Payload.ActiveBufferAddr,
          RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);

    if (RegRecPtr->DblBuffered)
    {
        /* For a double buffered table, the inactive is the other allocated buffer */
       CFE_SB_SET_MEMADDR(CFE_TBL_TaskData.TblRegPacket.Payload.InactiveBufferAddr,
            RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)].BufferPtr);
    }
    else
    {
        /* Check to see if an inactive buffer has currently been allocated to the single buffered table */
        if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
        {
           CFE_SB_SET_MEMADDR(CFE_TBL_TaskData.TblRegPacket.Payload.InactiveBufferAddr,
                CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr);
        }
        else
        {
           CFE_TBL_TaskData.TblRegPacket.Payload.InactiveBufferAddr = 0;
        }
    }

    CFE_SB_SET_MEMADDR(CFE_TBL_TaskData.TblRegPacket.Payload.ValidationFuncPtr, RegRecPtr->ValidationFuncPtr);
    CFE_TBL_TaskData.TblRegPacket.Payload.TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
    CFE_TBL_TaskData.TblRegPacket.Payload.TableLoadedOnce = RegRecPtr->TableLoadedOnce;
    CFE_TBL_TaskData.TblRegPacket.Payload.LoadPending = RegRecPtr->LoadPending;
    CFE_TBL_TaskData.TblRegPacket.Payload.DumpOnly = RegRecPtr->DumpOnly;
    CFE_TBL_TaskData.TblRegPacket.Payload.DblBuffered = RegRecPtr->DblBuffered;
    CFE_TBL_TaskData.TblRegPacket.Payload.FileCreateTimeSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSecs;
    CFE_TBL_TaskData.TblRegPacket.Payload.FileCreateTimeSubSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSubSecs;
    CFE_TBL_TaskData.TblRegPacket.Payload.Crc = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
    CFE_TBL_TaskData.TblRegPacket.Payload.Critical = RegRecPtr->CriticalTable;

    CFE_SB_MessageStringSet(CFE_TBL_TaskData.TblRegPacket.Payload.Name, RegRecPtr->Name,
            sizeof(CFE_TBL_TaskData.TblRegPacket.Payload.Name), sizeof(RegRecPtr->Name));
    CFE_SB_MessageStringSet(CFE_TBL_TaskData.TblRegPacket.Payload.LastFileLoaded, RegRecPtr->LastFileLoaded,
            sizeof(CFE_TBL_TaskData.TblRegPacket.Payload.LastFileLoaded), sizeof(RegRecPtr->LastFileLoaded));
    CFE_ES_GetAppName(CFE_TBL_TaskData.TblRegPacket.Payload.OwnerAppName, RegRecPtr->OwnerAppId,
            sizeof(CFE_TBL_TaskData.TblRegPacket.Payload.OwnerAppName));
}


/*******************************************************************
**
** CFE_TBL_NoopCmd() -- Process NO-Op Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_NoopCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    /* Acknowledge receipt of NOOP with Event Message */
    CFE_EVS_SendEvent(CFE_TBL_NOOP_INF_EID, CFE_EVS_INFORMATION, "No-op command. cFE Version %d.%d.%d.%d",
                      CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

    return CFE_TBL_INC_CMD_CTR;

} /* End of CFE_TBL_NoopCmd() */


/*******************************************************************
**
** CFE_TBL_ResetCmd() -- Process Reset Counters Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_ResetCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_TaskData.CmdCounter = 0;
    CFE_TBL_TaskData.ErrCounter = 0;
    CFE_TBL_TaskData.SuccessValCtr = 0;
    CFE_TBL_TaskData.FailedValCtr = 0;
    CFE_TBL_TaskData.NumValRequests = 0;
    CFE_TBL_TaskData.ValidationCtr = 0;

    CFE_EVS_SendEvent(CFE_TBL_RESET_INF_EID,
                      CFE_EVS_DEBUG,
                      "Reset Counters command");

    return CFE_TBL_DONT_INC_CTR;

} /* End of CFE_TBL_ResetCmd() */


/*******************************************************************
**
** CFE_TBL_LoadCmd() -- Process Load Table File to Buffer Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_LoadCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t        ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    const CFE_TBL_LoadCmd_Payload_t    *CmdPtr = (const CFE_TBL_LoadCmd_Payload_t *) Payload;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    int32                       FileDescriptor;
    int32                       Status;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_LoadBuff_t         *WorkingBufferPtr;
    char                        LoadFilename[OS_MAX_PATH_LEN];
    uint8                       ExtraByte;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(LoadFilename, (char *)CmdPtr->LoadFilename, NULL,
            OS_MAX_PATH_LEN, sizeof(CmdPtr->LoadFilename));

    /* Try to open the specified table file */
    FileDescriptor = OS_open(LoadFilename, OS_READ_ONLY, 0);

    if (FileDescriptor >= 0)
    {
        Status = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, &LoadFilename[0]);

        if (Status == CFE_SUCCESS)
        {
            /* Locate specified table in registry */
            Status = CFE_TBL_FindTableInRegistry(TblFileHeader.TableName);

            if (Status == CFE_TBL_NOT_FOUND)
            {
                CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Unable to locate '%s' in Table Registry",
                                  TblFileHeader.TableName);
            }
            else
            {
                /* Translate the registry index into a record pointer */
                RegRecPtr = &CFE_TBL_TaskData.Registry[Status];

                if (RegRecPtr->DumpOnly)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "Attempted to load DUMP-ONLY table '%s' from '%s'",
                                      TblFileHeader.TableName, LoadFilename);
                }
                else if (RegRecPtr->LoadPending)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOADING_PENDING_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "Attempted to load table '%s' while previous load is still pending",
                                      TblFileHeader.TableName);
                }
                else
                {
                    /* Make sure of the following:                                               */
                    /*    1) If table has not been loaded previously, then make sure the current */
                    /*       load starts with the first byte                                     */
                    /*    2) The number of bytes to load is greater than zero                    */
                    /*    3) The offset plus the number of bytes does not exceed the table size  */
                    if (((RegRecPtr->TableLoadedOnce) || (TblFileHeader.Offset == 0)) &&
                        (TblFileHeader.NumBytes > 0) &&
                        ((TblFileHeader.NumBytes + TblFileHeader.Offset) <= RegRecPtr->Size))
                    {
                        /* Get a working buffer, either a free one or one allocated with previous load command */
                        Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);

                        if (Status == CFE_SUCCESS)
                        {
                            /* Copy data from file into working buffer */
                            Status = OS_read(FileDescriptor,
                                             &WorkingBufferPtr->BufferPtr[TblFileHeader.Offset],
                                             TblFileHeader.NumBytes);
                                    
                            /* Make sure the appropriate number of bytes were read */
                            if (Status == (int32)TblFileHeader.NumBytes)
                            {
                                /* Check to ensure the file does not have any extra data at the end */
                                Status = OS_read(FileDescriptor, &ExtraByte, 1);

                                /* If another byte was successfully read, then file contains more data than header claims */
                                if (Status == 1)
                                {
                                    CFE_EVS_SendEvent(CFE_TBL_FILE_TOO_BIG_ERR_EID,
                                                      CFE_EVS_ERROR,
                                                      "File '%s' has more data than Tbl Hdr indicates (%d)",
                                                      LoadFilename,
                                                      (int)TblFileHeader.NumBytes);
                                }
                                else /* If error reading file or zero bytes read, assume it was the perfect size */
                                {
                                    CFE_EVS_SendEvent(CFE_TBL_FILE_LOADED_INF_EID,
                                                      CFE_EVS_INFORMATION,
                                                      "Successful load of '%s' into '%s' working buffer",
                                                      LoadFilename,
                                                      TblFileHeader.TableName);

                                    /* Save file information statistics for later use in registry */
                                    CFE_PSP_MemCpy(WorkingBufferPtr->DataSource, LoadFilename, OS_MAX_PATH_LEN);

                                    /* Save file creation time for later storage into Registry */
                                    WorkingBufferPtr->FileCreateTimeSecs = StdFileHeader.TimeSeconds;
                                    WorkingBufferPtr->FileCreateTimeSubSecs = StdFileHeader.TimeSubSeconds;
                                    
                                    /* Compute the CRC on the specified table buffer */
                                    WorkingBufferPtr->Crc = CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr,
                                                                                RegRecPtr->Size,
                                                                                0,
                                                                                CFE_ES_DEFAULT_CRC);
                                    
                                    /* Initialize validation flag with TRUE if no Validation Function is required to be called */
                                    WorkingBufferPtr->Validated = (RegRecPtr->ValidationFuncPtr == NULL);
                            
                                    /* Save file information statistics for housekeeping telemetry */
                                    strncpy(CFE_TBL_TaskData.HkPacket.Payload.LastFileLoaded, LoadFilename,
                                            sizeof(CFE_TBL_TaskData.HkPacket.Payload.LastFileLoaded));
                                    strncpy(CFE_TBL_TaskData.HkPacket.Payload.LastTableLoaded, TblFileHeader.TableName,
                                            sizeof(CFE_TBL_TaskData.HkPacket.Payload.LastTableLoaded));

                                    /* Increment successful command completion counter */
                                    ReturnCode = CFE_TBL_INC_CMD_CTR;
                                }
                            }
                            else
                            {
                                /* A file whose header claims has 'x' amount of data but it only has 'y' */
                                /* is considered a fatal error during a load process                     */
                                CFE_EVS_SendEvent(CFE_TBL_FILE_INCOMPLETE_ERR_EID,
                                                  CFE_EVS_ERROR,
                                                  "Incomplete load of '%s' into '%s' working buffer",
                                                  LoadFilename,
                                                  TblFileHeader.TableName);
                            }
                        }
                        else if (Status == CFE_TBL_ERR_NO_BUFFER_AVAIL)
                        {
                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "No working buffers available for table '%s'",
                                              TblFileHeader.TableName);
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_INTERNAL_ERROR_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "Internal Error (Status=0x%08X)",
                                              (unsigned int)Status);
                        }
                    }
                    else
                    {
                        if ((TblFileHeader.NumBytes + TblFileHeader.Offset) > RegRecPtr->Size)
                        {
                            CFE_EVS_SendEvent(CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "Cannot load '%s' (%d) at offset %d in '%s' (%d)",
                                              LoadFilename, (int)TblFileHeader.NumBytes, (int)TblFileHeader.Offset,
                                              TblFileHeader.TableName, (int)RegRecPtr->Size);
                        }
                        else if (TblFileHeader.NumBytes == 0)
                        {
                            CFE_EVS_SendEvent(CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "Table Hdr in '%s' indicates no data in file",
                                              LoadFilename);
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_PARTIAL_LOAD_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "'%s' has partial load for uninitialized table '%s'",
                                              LoadFilename, TblFileHeader.TableName);
                        }
                    }
                }
            }
        }  /* No need to issue event messages in response to errors reading headers */
           /* because the function that read the headers will generate messages     */

        /* Close the file now that the contents have been read */
        OS_close(FileDescriptor);
    }
    else
    {
        /* Error opening specified file */
        CFE_EVS_SendEvent(CFE_TBL_FILE_ACCESS_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to open file '%s' for table load, Status = 0x%08X",
                          LoadFilename, (unsigned int)FileDescriptor);
    }

    return ReturnCode;

} /* End of CFE_TBL_LoadCmd() */


/*******************************************************************
**
** CFE_TBL_DumpCmd() -- Process Dump Table to File Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_DumpCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t        ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    int16                       RegIndex;
    const CFE_TBL_DumpCmd_Payload_t    *CmdPtr = (const CFE_TBL_DumpCmd_Payload_t *) Payload;
    char                        DumpFilename[OS_MAX_PATH_LEN];
    char                        TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    void                       *DumpDataAddr = NULL;
    CFE_TBL_LoadBuff_t         *WorkingBufferPtr;
    int32                       DumpIndex;
    int32                       Status;
    CFE_TBL_DumpControl_t      *DumpCtrlPtr;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(DumpFilename, (char *)CmdPtr->DumpFilename, NULL,
            OS_MAX_PATH_LEN, sizeof(CmdPtr->DumpFilename));

    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];

        /* Determine what data is to be dumped */
        if (CmdPtr->ActiveTblFlag == CFE_TBL_ACTIVE_BUFFER)
        {
            DumpDataAddr = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr;
        }
        else if (CmdPtr->ActiveTblFlag == CFE_TBL_INACTIVE_BUFFER)/* Dumping Inactive Buffer */
        {
            /* If this is a double buffered table, locating the inactive buffer is trivial */
            if (RegRecPtr->DblBuffered)
            {
                DumpDataAddr = RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)].BufferPtr;
            }
            else
            {
                /* For single buffered tables, the index to the inactive buffer is kept in 'LoadInProgress' */
                /* Unless this is a table whose address was defined by the owning Application.              */
                if ((RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS) && (!RegRecPtr->UserDefAddr))
                {
                    DumpDataAddr = CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr;
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "No Inactive Buffer for Table '%s' present",
                                      TableName);
                }
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID,
                              CFE_EVS_ERROR,
                              "Cmd for Table '%s' had illegal buffer parameter (0x%08X)",
                              TableName, CmdPtr->ActiveTblFlag);
        }

        /* If we have located the data to be dumped, then proceed with creating the file and dumping the data */
        if (DumpDataAddr != NULL)
        {
            /* If this is not a dump only table, then we can perform the dump immediately */
            if (!RegRecPtr->DumpOnly)
            {
                ReturnCode = CFE_TBL_DumpToFile(DumpFilename, TableName, DumpDataAddr, RegRecPtr->Size);
            }
            else /* Dump Only tables need to synchronize their dumps with the owner's execution */
            {
                /* Make sure a dump is not already in progress */
                if (RegRecPtr->DumpControlIndex == CFE_TBL_NO_DUMP_PENDING)
                {
                    /* Find a free Dump Control Block */
                    DumpIndex = 0;
                    while ((DumpIndex < CFE_TBL_MAX_SIMULTANEOUS_LOADS) &&
                           (CFE_TBL_TaskData.DumpControlBlocks[DumpIndex].State != CFE_TBL_DUMP_FREE))
                    {
                        DumpIndex++;
                    }

                    if (DumpIndex < CFE_TBL_MAX_SIMULTANEOUS_LOADS)
                    {
                        /* Allocate a shared memory buffer for storing the data to be dumped */
                        Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);
                        
                        if (Status == CFE_SUCCESS)
                        {
                            DumpCtrlPtr = &CFE_TBL_TaskData.DumpControlBlocks[DumpIndex];
                            DumpCtrlPtr->State = CFE_TBL_DUMP_PENDING;
                            DumpCtrlPtr->RegRecPtr = RegRecPtr;
                        
                            /* Save the name of the desired dump filename, table name and size for later */
                            DumpCtrlPtr->DumpBufferPtr = WorkingBufferPtr;
                            CFE_PSP_MemCpy(DumpCtrlPtr->DumpBufferPtr->DataSource, DumpFilename, OS_MAX_PATH_LEN);
                            CFE_PSP_MemCpy(DumpCtrlPtr->TableName, TableName, CFE_TBL_MAX_FULL_NAME_LEN);
                            DumpCtrlPtr->Size = RegRecPtr->Size;
                        
                            /* Notify the owning application that a dump is pending */
                            RegRecPtr->DumpControlIndex = DumpIndex;
                            
                            /* If application requested notification by message, then do so */
                            CFE_TBL_SendNotificationMsg(RegRecPtr);
                            
                            /* Consider the command completed successfully */
                            ReturnCode = CFE_TBL_INC_CMD_CTR;
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID,
                                              CFE_EVS_ERROR,
                                              "No working buffers available for table '%s'",
                                              TableName);
                        }
                    }
                    else
                    {
                        CFE_EVS_SendEvent(CFE_TBL_TOO_MANY_DUMPS_ERR_EID,
                                          CFE_EVS_ERROR,
                                          "Too many Dump Only Table Dumps have been requested");
                    }
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_DUMP_PENDING_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "A dump for '%s' is already pending",
                                      TableName);
                }
            }
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to locate '%s' in Table Registry",
                          TableName);
    }

    return ReturnCode;

} /* End of CFE_TBL_DumpCmd() */


/*******************************************************************
**
** CFE_TBL_DumpToFile() -- Write table data to a file
**
** NOTE: For complete prolog information, see prototype above
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_DumpToFile( const char *DumpFilename, const char *TableName, void *DumpDataAddr, uint32 TblSizeInBytes)
{
    CFE_TBL_CmdProcRet_t        ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    boolean                     FileExistedPrev = FALSE;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    int32                       FileDescriptor;
    int32                       Status;
    int32                       EndianCheck = 0x01020304;
    
    /* Clear Header of any garbage before copying content */
    CFE_PSP_MemSet(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

    /* Check to see if the dump file already exists */
    FileDescriptor = OS_open(DumpFilename, OS_READ_ONLY, 0);

    if (FileDescriptor >= 0)
    {
        FileExistedPrev = TRUE;

        OS_close(FileDescriptor);
    }

    /* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat(DumpFilename, OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "Table Dump Image", CFE_FS_TBL_IMG_SUBTYPE);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
        {
            /* Initialize the Table Image Header for the Dump File */
            strncpy(TblFileHeader.TableName, TableName, sizeof(TblFileHeader.TableName));
            TblFileHeader.Offset = 0;
            TblFileHeader.NumBytes = TblSizeInBytes;
            TblFileHeader.Reserved = 0;
            
            /* Determine if this is a little endian processor */
            if ((*(char *)&EndianCheck) == 0x04)
            {
                /* If this is a little endian processor, then byte swap the header to a big endian format */
                /* to maintain the cFE Header standards */
                /* NOTE: FOR THE REMAINDER OF THIS FUNCTION, THE CONTENTS OF THE HEADER IS UNREADABLE BY */
                /*       THIS PROCESSOR!  THE DATA WOULD NEED TO BE SWAPPED BACK BEFORE READING.         */
                CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
            }

            /* Output the Table Image Header to the Dump File */
            Status = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

            /* Make sure the header was output completely */
            if (Status == sizeof(CFE_TBL_File_Hdr_t))
            {
                /* Output the requested data to the dump file */
                /* Output the active table image data to the dump file */
                Status = OS_write(FileDescriptor,
                                  DumpDataAddr,
                                  TblSizeInBytes);

                if (Status == (int32)TblSizeInBytes)
                {
                    if (FileExistedPrev)
                    {
                        CFE_EVS_SendEvent(CFE_TBL_OVERWRITE_DUMP_INF_EID,
                                          CFE_EVS_INFORMATION,
                                          "Successfully overwrote '%s' with Table '%s'",
                                          DumpFilename, TableName);
                    }
                    else
                    {
                        CFE_EVS_SendEvent(CFE_TBL_WRITE_DUMP_INF_EID,
                                          CFE_EVS_INFORMATION,
                                          "Successfully dumped Table '%s' to '%s'",
                                          TableName, DumpFilename);
                    }

                    /* Save file information statistics for housekeeping telemetry */
                    strncpy(CFE_TBL_TaskData.HkPacket.Payload.LastFileDumped, DumpFilename,
                            sizeof(CFE_TBL_TaskData.HkPacket.Payload.LastFileDumped));

                    /* Increment Successful Command Counter */
                    ReturnCode = CFE_TBL_INC_CMD_CTR;
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_WRITE_TBL_IMG_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "Error writing Tbl image to '%s', Status=0x%08X",
                                      DumpFilename, (unsigned int)Status);
                }
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_WRITE_TBL_HDR_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Error writing Tbl image File Header to '%s', Status=0x%08X",
                                  DumpFilename, (unsigned int)Status);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_WRITE_CFE_HDR_ERR_EID,
                              CFE_EVS_ERROR,
                              "Error writing cFE File Header to '%s', Status=0x%08X",
                              DumpFilename, (unsigned int)Status);
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        CFE_EVS_SendEvent(CFE_TBL_CREATING_DUMP_FILE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating dump file '%s', Status=0x%08X",
                          DumpFilename, (unsigned int)FileDescriptor);
    }
    
    return ReturnCode;
}

/*******************************************************************
**
** CFE_TBL_ValidateCmd() -- Process Validate Table Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_ValidateCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t         ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    int16                        RegIndex;
    const CFE_TBL_ValidateCmd_Payload_t *CmdPtr = (const CFE_TBL_ValidateCmd_Payload_t *) Payload;
    CFE_TBL_RegistryRec_t       *RegRecPtr;
    void                        *ValidationDataPtr = NULL;
    char                         TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    uint32                       CrcOfTable;
    int32                        ValIndex;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];

        /* Determine what data is to be validated */
        if (CmdPtr->ActiveTblFlag == CFE_TBL_ACTIVE_BUFFER)
        {
            ValidationDataPtr = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr;
        }
        else if (CmdPtr->ActiveTblFlag == CFE_TBL_INACTIVE_BUFFER) /* Validating Inactive Buffer */
        {
            /* If this is a double buffered table, locating the inactive buffer is trivial */
            if (RegRecPtr->DblBuffered)
            {
                ValidationDataPtr = RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)].BufferPtr;
            }
            else
            {
                /* For single buffered tables, the index to the inactive buffer is kept in 'LoadInProgress' */
                if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
                {
                    ValidationDataPtr = CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr;
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID,
                                      CFE_EVS_ERROR,
                                      "No Inactive Buffer for Table '%s' present",
                                      TableName);
                }
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID,
                              CFE_EVS_ERROR,
                              "Cmd for Table '%s' had illegal buffer parameter (0x%08X)",
                              TableName, CmdPtr->ActiveTblFlag);
        }

        /* If we have located the data to be validated, then proceed with notifying the application, if */
        /* necessary, and computing the CRC value for the block of memory                               */
        if (ValidationDataPtr != NULL)
        {
            /* Find a free Validation Response Block */
            ValIndex = 0;
            while ((ValIndex < CFE_TBL_MAX_NUM_VALIDATIONS) &&
                   (CFE_TBL_TaskData.ValidationResults[ValIndex].State != CFE_TBL_VALIDATION_FREE))
            {
                ValIndex++;
            }

            if (ValIndex < CFE_TBL_MAX_NUM_VALIDATIONS)
            {
                /* Allocate this Validation Response Block */
                CFE_TBL_TaskData.ValidationResults[ValIndex].State = CFE_TBL_VALIDATION_PENDING;
                CFE_TBL_TaskData.ValidationResults[ValIndex].Result = 0;
                CFE_PSP_MemCpy(CFE_TBL_TaskData.ValidationResults[ValIndex].TableName,
                          TableName, CFE_TBL_MAX_FULL_NAME_LEN);

                /* Compute the CRC on the specified table buffer */
                CrcOfTable = CFE_ES_CalculateCRC(ValidationDataPtr,
                                                 RegRecPtr->Size,
                                                 0,
                                                 CFE_ES_DEFAULT_CRC);

                CFE_TBL_TaskData.ValidationResults[ValIndex].CrcOfTable = CrcOfTable;
                CFE_TBL_TaskData.ValidationResults[ValIndex].ActiveBuffer = (CmdPtr->ActiveTblFlag != 0);

                /* If owner has a validation function, then notify the  */
                /* table owner that there is data to be validated       */
                if (RegRecPtr->ValidationFuncPtr != NULL)
                {
                    if (CmdPtr->ActiveTblFlag)
                    {
                        RegRecPtr->ValidateActiveIndex = ValIndex;
                    }
                    else
                    {
                        RegRecPtr->ValidateInactiveIndex = ValIndex;
                    }
                    
                    /* If application requested notification by message, then do so */
                    if (CFE_TBL_SendNotificationMsg(RegRecPtr) == CFE_SUCCESS)
                    {
                        /* Notify ground that validation request has been made */
                        CFE_EVS_SendEvent(CFE_TBL_VAL_REQ_MADE_INF_EID,
                                          CFE_EVS_DEBUG,
                                          "Tbl Services issued validation request for '%s'",
                                          TableName);
                    }

                    /* Maintain statistic on number of validation requests given to applications */
                    CFE_TBL_TaskData.NumValRequests++;
                }
                else
                {
                    /* If there isn't a validation function pointer, then the process is complete  */
                    /* By setting this value, we are letting the Housekeeping process recognize it */
                    /* as data to be sent to the ground in telemetry.                              */
                    CFE_TBL_TaskData.ValidationResults[ValIndex].State = CFE_TBL_VALIDATION_PERFORMED;
                    
                    CFE_EVS_SendEvent(CFE_TBL_ASSUMED_VALID_INF_EID,
                                      CFE_EVS_INFORMATION,
                                      "Tbl Services assumes '%s' is valid. No Validation Function has been registered",
                                      TableName);
                }

                /* Increment Successful Command Counter */
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Too many Table Validations have been requested");
            }
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to locate '%s' in Table Registry",
                          TableName);
    }

    return ReturnCode;

} /* End of CFE_TBL_ValidateCmd() */


/*******************************************************************
**
** CFE_TBL_ActivateCmd() -- Process Activate Table Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_ActivateCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t         ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    int16                        RegIndex;
    const CFE_TBL_ActivateCmd_Payload_t *CmdPtr = (const CFE_TBL_ActivateCmd_Payload_t *) Payload;
    char                         TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_RegistryRec_t       *RegRecPtr;
    boolean                      ValidationStatus = FALSE;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];
        
        if (RegRecPtr->DumpOnly)
        {
            CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID,
                              CFE_EVS_ERROR,
                              "Illegal attempt to activate dump-only table '%s'",
                              TableName);
        }
        else if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
        {
            /* Determine if the inactive buffer has been successfully validated or not */
            if (RegRecPtr->DblBuffered)
            {
                ValidationStatus = RegRecPtr->Buffers[(1U-RegRecPtr->ActiveBufferIndex)].Validated;
            }
            else
            {
                ValidationStatus = CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Validated;
            }
            
            if (ValidationStatus == TRUE)
            {
                CFE_TBL_TaskData.Registry[RegIndex].LoadPending = TRUE;
                
                /* If application requested notification by message, then do so */
                if (CFE_TBL_SendNotificationMsg(RegRecPtr) == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOAD_PEND_REQ_INF_EID,
                                      CFE_EVS_DEBUG,
                                      "Tbl Services notifying App that '%s' has a load pending",
                                      TableName);
                }

                /* Increment Successful Command Counter */
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_UNVALIDATED_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Cannot activate table '%s'. Inactive image not Validated",
                                  TableName);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_ERR_EID,
                              CFE_EVS_ERROR,
                              "Cannot activate table '%s'. No Inactive image available",
                              TableName);
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to locate '%s' in Table Registry",
                          TableName);
    }

    return ReturnCode;

} /* End of CFE_TBL_ActivateCmd() */


/*******************************************************************
**
** CFE_TBL_DumpRegCmd() -- Process Dump Table Registry to file Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_DumpRegCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t        ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    boolean                     FileExistedPrev = FALSE;
    CFE_FS_Header_t             StdFileHeader;
    int32                       FileDescriptor;
    int32                       Status;
    int16                       RegIndex=0;
    const CFE_TBL_DumpRegCmd_Payload_t *CmdPtr = (const CFE_TBL_DumpRegCmd_Payload_t *) Payload;
    char                        DumpFilename[OS_MAX_PATH_LEN];
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_Handle_t            HandleIterator;
    CFE_TBL_RegDumpRec_t        DumpRecord;
    int32                       FileSize=0;
    int32                       NumEntries=0;

    /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
    CFE_SB_MessageStringGet(DumpFilename, (char *)CmdPtr->DumpFilename, CFE_TBL_DEFAULT_REG_DUMP_FILE,
            OS_MAX_PATH_LEN, sizeof(CmdPtr->DumpFilename));

    /* Check to see if the dump file already exists */
    FileDescriptor = OS_open(DumpFilename, OS_READ_ONLY, 0);

    if (FileDescriptor >= 0)
    {
        FileExistedPrev = TRUE;

        OS_close(FileDescriptor);
    }

    /* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat(DumpFilename, OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "Table Registry", CFE_FS_TBL_REG_SUBTYPE);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);
        
        /* Maintain statistics of amount of data written to file */
        FileSize += Status;

        if (Status == sizeof(CFE_FS_Header_t))
        {
            Status = sizeof(CFE_TBL_RegDumpRec_t);
            while ((RegIndex < CFE_TBL_MAX_NUM_TABLES) && (Status == sizeof(CFE_TBL_RegDumpRec_t)))
            {
                /* Make a pointer to simplify code look and to remove redundant indexing into registry */
                RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];

                /* Check to see if the Registry entry is empty */
                if ((RegRecPtr->OwnerAppId != CFE_TBL_NOT_OWNED) ||
                    (RegRecPtr->HeadOfAccessList != CFE_TBL_END_OF_LIST))
                {
                    /* Fill Registry Dump Record with relevant information */
                    DumpRecord.Size             = RegRecPtr->Size;
                    DumpRecord.TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
                    DumpRecord.LoadInProgress   = RegRecPtr->LoadInProgress;
                    DumpRecord.ValidationFunc   = (RegRecPtr->ValidationFuncPtr != NULL);
                    DumpRecord.TableLoadedOnce  = RegRecPtr->TableLoadedOnce;
                    DumpRecord.LoadPending      = RegRecPtr->LoadPending;
                    DumpRecord.DumpOnly         = RegRecPtr->DumpOnly;
                    DumpRecord.DblBuffered      = RegRecPtr->DblBuffered;
                    DumpRecord.FileCreateTimeSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSecs;
                    DumpRecord.FileCreateTimeSubSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSubSecs;
                    DumpRecord.Crc              = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
                    DumpRecord.CriticalTable    = RegRecPtr->CriticalTable;

                    /* Convert LoadInProgress flag into more meaningful information */
                    /* When a load is in progress, identify which buffer is being used as the inactive buffer */
                    if (DumpRecord.LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
                    {
                        if (DumpRecord.DblBuffered)
                        {
                            /* For double buffered tables, the value of LoadInProgress, when a load is actually in progress, */
                            /* should identify either buffer #0 or buffer #1.  Convert these to enumerated value for ground  */
                            /* display.  LoadInProgress = -2 means Buffer #1, LoadInProgress = -3 means Buffer #0.           */
                            DumpRecord.LoadInProgress = DumpRecord.LoadInProgress - 3;
                        }
                        /* For single buffered tables, the value of LoadInProgress, when a load is actually in progress,     */
                        /* indicates which shared buffer is allocated for the inactive buffer.  Since the number of inactive */
                        /* buffers is a platform configuration parameter, then 0 on up merely identifies the buffer number.  */
                        /* No translation is necessary for single buffered tables.                                           */
                    }
                    
                    /* Zero character arrays to remove garbage text */
                    CFE_PSP_MemSet(DumpRecord.Name, 0, CFE_TBL_MAX_FULL_NAME_LEN);
                    CFE_PSP_MemSet(DumpRecord.LastFileLoaded, 0, OS_MAX_PATH_LEN);
                    CFE_PSP_MemSet(DumpRecord.OwnerAppName, 0, OS_MAX_API_NAME);

                    strncpy(DumpRecord.Name, RegRecPtr->Name, CFE_TBL_MAX_FULL_NAME_LEN);
                    strncpy(DumpRecord.LastFileLoaded, RegRecPtr->LastFileLoaded, OS_MAX_PATH_LEN);

                    /* Walk the access descriptor list to determine the number of users */
                    DumpRecord.NumUsers = 0;
                    HandleIterator = RegRecPtr->HeadOfAccessList;
                    while (HandleIterator != CFE_TBL_END_OF_LIST)
                    {
                        DumpRecord.NumUsers++;
                        HandleIterator = CFE_TBL_TaskData.Handles[HandleIterator].NextLink;
                    }

                    /* Determine the name of the owning application */
                    if (RegRecPtr->OwnerAppId != CFE_TBL_NOT_OWNED)
                    {
                        CFE_ES_GetAppName(DumpRecord.OwnerAppName, RegRecPtr->OwnerAppId, OS_MAX_API_NAME);
                    }
                    else
                    {
                        strncpy(DumpRecord.OwnerAppName, "--UNOWNED--", OS_MAX_API_NAME);
                    }

                    /* Output Registry Dump Record to Registry Dump File */
                    Status = OS_write(FileDescriptor,
                                      &DumpRecord,
                                      sizeof(CFE_TBL_RegDumpRec_t));
                    
                    FileSize += Status;
                    NumEntries++;      
                }

                /* Look at the next entry in the Registry */
                RegIndex++;
            }

            if (Status == sizeof(CFE_TBL_RegDumpRec_t))
            {
                if (FileExistedPrev)
                {
                    CFE_EVS_SendEvent(CFE_TBL_OVERWRITE_REG_DUMP_INF_EID,
                                      CFE_EVS_DEBUG,
                                      "Successfully overwrote '%s' with Table Registry:Size=%d,Entries=%d",
                                      DumpFilename, (int)FileSize, (int)NumEntries);
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_WRITE_REG_DUMP_INF_EID,
                                      CFE_EVS_DEBUG,
                                      "Successfully dumped Table Registry to '%s':Size=%d,Entries=%d",
                                      DumpFilename, (int)FileSize, (int)NumEntries);
                }

                /* Increment Successful Command Counter */
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_WRITE_TBL_REG_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Error writing Registry to '%s', Status=0x%08X",
                                  DumpFilename, (unsigned int)Status);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_WRITE_CFE_HDR_ERR_EID,
                              CFE_EVS_ERROR,
                              "Error writing cFE File Header to '%s', Status=0x%08X",
                              DumpFilename, (unsigned int)Status);
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        CFE_EVS_SendEvent(CFE_TBL_CREATING_DUMP_FILE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating dump file '%s', Status=0x%08X",
                          DumpFilename, (unsigned int)FileDescriptor);
    }

    return ReturnCode;

} /* End of CFE_TBL_DumpRegCmd() */


/*******************************************************************
**
** CFE_TBL_TlmRegCmd() -- Process Telemeter Table Registry Entry Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_TlmRegCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t         ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    int16                        RegIndex;
    const CFE_TBL_TlmRegCmd_Payload_t   *CmdPtr = (const CFE_TBL_TlmRegCmd_Payload_t *) Payload;
    char                         TableName[CFE_TBL_MAX_FULL_NAME_LEN];

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Change the index used to identify what data is to be telemetered */
        CFE_TBL_TaskData.HkTlmTblRegIndex = RegIndex;

        CFE_EVS_SendEvent(CFE_TBL_TLM_REG_CMD_INF_EID,
                          CFE_EVS_DEBUG,
                          "Table Registry entry for '%s' will be telemetered",
                          TableName);

        /* Increment Successful Command Counter */
        ReturnCode = CFE_TBL_INC_CMD_CTR;
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to locate '%s' in Table Registry",
                          TableName);
    }

    return ReturnCode;

} /* End of CFE_TBL_TlmRegCmd() */


/*******************************************************************
**
** CFE_TBL_DeleteCDSCmd() -- Process Delete Critical Table's CDS Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_DeleteCDSCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t         ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    const CFE_TBL_DelCDSCmd_Payload_t   *CmdPtr = (const CFE_TBL_DelCDSCmd_Payload_t *) Payload;
    char                         TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_CritRegRec_t        *CritRegRecPtr = NULL;
    uint32                       i;
    uint32                       RegIndex;
    int32                        Status;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table is no longer in the registry */
    /* This would imply that the owning application has been terminated and that it */
    /* is safe to delete the associated critical table image in the CDS. */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex == CFE_TBL_NOT_FOUND)
    {
        /* Find table in the Critical Table Registry */
        for (i=0; i<CFE_TBL_MAX_CRITICAL_TABLES; i++)
        {
            if (strncmp(CFE_TBL_TaskData.CritReg[i].Name, TableName, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
            {
                CritRegRecPtr = &CFE_TBL_TaskData.CritReg[i];
                i=CFE_TBL_MAX_CRITICAL_TABLES;
            }
        }
        
        if (CritRegRecPtr != NULL)
        {
            Status = CFE_ES_DeleteCDS(TableName, TRUE);
        
            if (Status == CFE_ES_CDS_WRONG_TYPE_ERR)
            {
                CFE_EVS_SendEvent(CFE_TBL_NOT_CRITICAL_TBL_ERR_EID,
                                  CFE_EVS_ERROR,
                                  "Table '%s' is in Critical Table Registry but CDS is not tagged as a table",
                                  TableName);
            }
            else if (Status == CFE_ES_CDS_OWNER_ACTIVE_ERR)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID, CFE_EVS_ERROR,
                                  "CDS '%s' owning app is still active", 
                                  TableName);
            }
            else if (Status == CFE_ES_CDS_NOT_FOUND_ERR)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_NOT_FOUND_ERR_EID, CFE_EVS_ERROR,        
                                 "Unable to locate '%s' in CDS Registry", TableName);
            }
            else if (Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_DELETE_ERR_EID, CFE_EVS_ERROR,
                                  "Error while deleting '%s' from CDS, See SysLog.(Err=0x%08X)", 
                                  TableName, (unsigned int)Status);
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_DELETED_INFO_EID, CFE_EVS_INFORMATION,
                                  "Successfully removed '%s' from CDS", TableName);
                            
                /* Free the entry in the Critical Table Registry */      
                CritRegRecPtr->CDSHandle = CFE_ES_CDS_BAD_HANDLE;
                     
                /* Increment Successful Command Counter */
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_NOT_IN_CRIT_REG_ERR_EID,
                              CFE_EVS_ERROR,
                              "Table '%s' is not found in Critical Table Registry",
                              TableName);
        }      
    }
    else /* Table was found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_IN_REGISTRY_ERR_EID,
                          CFE_EVS_ERROR,
                          "'%s' found in Table Registry. CDS cannot be deleted until table is unregistered",
                          TableName);
    }
    return ReturnCode;

} /* End of CFE_TBL_DeleteCDSCmd() */


/*******************************************************************
**
** CFE_TBL_AbortLoadCmd() -- Process Abort Load Command Message
**
** NOTE: For complete prolog information, see 'cfe_tbl_task_cmds.h'
********************************************************************/

CFE_TBL_CmdProcRet_t CFE_TBL_AbortLoadCmd( const CFE_SB_MsgPayloadPtr_t  Payload )
{
    CFE_TBL_CmdProcRet_t         ReturnCode = CFE_TBL_INC_ERR_CTR;        /* Assume failure */
    int16                        RegIndex;
    const CFE_TBL_AbortLdCmd_Payload_t  *CmdPtr = (const CFE_TBL_AbortLdCmd_Payload_t *) Payload;
    CFE_TBL_RegistryRec_t       *RegRecPtr;
    char                         TableName[CFE_TBL_MAX_FULL_NAME_LEN];

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL,
            CFE_TBL_MAX_FULL_NAME_LEN, sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Make a pointer to simplify code look and to remove redundant indexing into registry */
        RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];

        /* Check to make sure a load was in progress before trying to abort it */
        /* NOTE: LoadInProgress contains index of buffer when dumping a dump-only table */
        /* so we must ensure the table is not a dump-only table, otherwise, we would be aborting a dump */
        if ((RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS) && (!RegRecPtr->DumpOnly))
        {
            CFE_TBL_AbortLoad(RegRecPtr);

            /* Increment Successful Command Counter */
            ReturnCode = CFE_TBL_INC_CMD_CTR;
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_LOAD_ABORT_ERR_EID,
                              CFE_EVS_ERROR,
                              "Cannot abort load of '%s'. No load started.",
                              TableName);
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID,
                          CFE_EVS_ERROR,
                          "Unable to locate '%s' in Table Registry",
                          TableName);
    }

    return ReturnCode;

} /* End of CFE_TBL_AbortLoadCmd() */


/*******************************************************************
**
** CFE_TBL_AbortLoad() -- Abort load, free buffers, issue event message
**
** NOTE: For complete prolog information, see above
********************************************************************/

void CFE_TBL_AbortLoad(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* The ground has aborted the load, free the working buffer for another attempt */
    if (!RegRecPtr->DblBuffered)
    {
        /* For single buffered tables, freeing shared buffer entails resetting flag */
        CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Taken = FALSE;
    }

    /* For double buffered tables, freeing buffer is simple */
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    
    /* Make sure the load was not already pending */
    RegRecPtr->LoadPending = FALSE;

    CFE_EVS_SendEvent(CFE_TBL_LOAD_ABORT_INF_EID,
                      CFE_EVS_INFORMATION,
                      "Table Load Aborted for '%s'",
                      RegRecPtr->Name);
}

/************************/
/*  End of File Comment */
/************************/

