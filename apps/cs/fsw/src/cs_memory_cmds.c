/************************************************************************
 ** File:
 **   $Id: cs_memory_cmds.c 1.8 2017/03/29 17:29:03EDT mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   The CFS Checksum (CS) Application's commands for checking Memory
 **
 *************************************************************************/

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_compute.h"
#include "cs_memory_cmds.h"
#include "cs_utils.h"

#include "cfe_platform_cfg.h" /* for CFE_ES_DEFAULT_STACK_SIZE */

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of Memory command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.MemoryCSState = CS_STATE_DISABLED;
        CS_ZeroMemoryTempValues();
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_DISABLE_MEMORY_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Memory is Disabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Memory command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.MemoryCSState = CS_STATE_ENABLED;
        
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_ENABLE_MEMORY_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Memory is Enabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_EnableMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Memory table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_EntryCmd_t                         * CmdPtr         = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry   = NULL; 
    uint32                                  Baseline       = 0;
    uint16                                  EntryID        = 0;
    uint16                                  State          = CS_STATE_EMPTY;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_EntryCmd_t *) MessagePtr;
        EntryID = CmdPtr -> EntryID;
        
        if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
            (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = &CS_AppData.ResMemoryTblPtr[EntryID];
            
            if (ResultsEntry->ComputedYet == TRUE)
            {
                Baseline = ResultsEntry->ComparisonValue;
                
                CFE_EVS_SendEvent (CS_BASELINE_MEMORY_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of Memory Entry %d is 0x%08X", 
                                   EntryID,
                                   (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent (CS_NO_BASELINE_MEMORY_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of Memory Entry %d has not been computed yet", 
                                   EntryID);   
            }
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResMemoryTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID,
                               CFE_EVS_ERROR,
                               "Memory report baseline failed, Entry ID invalid: %d, State: %d Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_ReportBaselineEntryIDCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineMemoryCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    uint32                                  ChildTaskID    = 0;
    int32                                   Status         = -1;  /* Init to OS error */
    CS_EntryCmd_t                         * CmdPtr         = NULL;
    uint16                                  EntryID        = 0;
    uint16                                  State          = CS_STATE_EMPTY;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_EntryCmd_t *) MessagePtr;
        EntryID = CmdPtr -> EntryID;
        
        
        if (CS_AppData.RecomputeInProgress == FALSE && CS_AppData.OneShotInProgress == FALSE)
        {

            
            /* make sure the entry is a valid number and is defined in the table */
            if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
                (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY) )
            {

                /* There is no child task running right now, we can use it*/
                CS_AppData.RecomputeInProgress           = TRUE;
                
                /* fill in child task variables */
                CS_AppData.ChildTaskTable                = CS_MEMORY_TABLE;
                CS_AppData.ChildTaskEntryID              = EntryID;
                
                
                CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.ResMemoryTblPtr[EntryID];
                
                Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                               CS_RECOMP_MEMORY_TASK_NAME,
                                               CS_RecomputeEepromMemoryChildTask,
                                               NULL,
                                               CFE_ES_DEFAULT_STACK_SIZE,
                                               CS_CHILD_TASK_PRIORITY,
                                               0);
                if (Status ==CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,
                                       CFE_EVS_DEBUG,
                                       "Recompute baseline of Memory Entry ID %d started", 
                                       EntryID);
                    CS_AppData.CmdCounter++;
                }
                else/* child task creation failed */
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "Recompute baseline of Memory Entry ID %d failed, ES_CreateChildTask returned:  0x%08X",
                                       EntryID,
                                       (unsigned int)Status);
                    CS_AppData.CmdErrCounter++;
                    CS_AppData.RecomputeInProgress = FALSE;
                }
            }
            else
            {
                if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
                {
                    State = CS_STATE_UNDEFINED;
                }
                else
                {
                    State = CS_AppData.ResMemoryTblPtr[EntryID].State;
                }
            
                CFE_EVS_SendEvent (CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Memory recompute baseline of entry failed, Entry ID invalid: %d, State: %d, Max ID: %d",
                                   EntryID,
                                   State,
                                   (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
                
                CS_AppData.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                               "Recompute baseline of Memory Entry ID %d failed: child task in use",
                               EntryID);
            CS_AppData.CmdErrCounter++;
        }
    }    
    return;
}/* end CS_RecomputeBaselineMemoryCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Memory table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_EntryCmd_t                         * CmdPtr         = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry   = NULL; 
    uint16                                  EntryID        = 0;
    uint16                                  State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_EntryCmd_t *) MessagePtr;
        EntryID = CmdPtr -> EntryID;
                
        if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
            (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = &CS_AppData.ResMemoryTblPtr[EntryID]; 
             
            ResultsEntry -> State = CS_STATE_ENABLED;
            
            CFE_EVS_SendEvent (CS_ENABLE_MEMORY_ENTRY_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of Memory Entry ID %d is Enabled", 
                                EntryID);
            
            if (CS_AppData.DefMemoryTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefMemoryTblPtr[EntryID].State = CS_STATE_ENABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.MemResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefMemoryTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update memory definition table for entry %d, State: %d",
                                   EntryID,
                                   State);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResMemoryTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID,
                               CFE_EVS_ERROR,
                               "Enable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_EnableCSEntryIDMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Memory table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_EntryCmd_t                         * CmdPtr         = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry   = NULL;
    uint16                                  EntryID        = 0;
    uint16                                  State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_EntryCmd_t *) MessagePtr;
        EntryID = CmdPtr -> EntryID;
        
        if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
            (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = & CS_AppData.ResMemoryTblPtr[EntryID]; 
            
            ResultsEntry -> State = CS_STATE_DISABLED;
            ResultsEntry -> TempChecksumValue = 0;
            ResultsEntry -> ByteOffset = 0;
            
            CFE_EVS_SendEvent (CS_DISABLE_MEMORY_ENTRY_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of Memory Entry ID %d is Disabled", 
                               EntryID);
            
            if (CS_AppData.DefMemoryTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefMemoryTblPtr[EntryID].State = CS_STATE_DISABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.MemResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefMemoryTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update memory definition table for entry %d, State: %d",
                                   EntryID,
                                   State);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResMemoryTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID,
                               CFE_EVS_ERROR,
                               "Disable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_MEMORY_TABLE_ENTRIES -1));
            
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_DisableCSEntryIDMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Retrieve an EntryID based on Address from Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GetEntryIDMemoryCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength      = sizeof(CS_GetEntryIDCmd_t);
    CS_GetEntryIDCmd_t                    * CmdPtr              = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * StartOfResultsTable = NULL; 
    CS_Res_EepromMemory_Table_Entry_t     * ResultsEntry        = NULL;
    uint16                                  Loop                = 0;
    boolean                                 EntryFound          = FALSE;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_GetEntryIDCmd_t *) MessagePtr;
        
        StartOfResultsTable = CS_AppData.ResMemoryTblPtr;   
        
        for ( Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop ++)
        {
            ResultsEntry = & StartOfResultsTable[Loop];
            
            if ( (ResultsEntry->StartAddress <= CmdPtr -> Address) &&
                CmdPtr -> Address <= (ResultsEntry->StartAddress + ResultsEntry->NumBytesToChecksum) &&
                ResultsEntry->State != CS_STATE_EMPTY)
            {
                
                CFE_EVS_SendEvent (CS_GET_ENTRY_ID_MEMORY_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Memory Found Address 0x%08X in Entry ID %d", 
                                   (unsigned int)(CmdPtr -> Address),
                                   Loop);
                EntryFound = TRUE;
            }
        }
        
        if (EntryFound == FALSE)
        {
            CFE_EVS_SendEvent (CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Address 0x%08X was not found in Memory table",
                               (unsigned int)(CmdPtr -> Address));
        }
        CS_AppData.CmdCounter++;
    }
    return;
    
} /* End of CS_GetEntryIDMemoryCmd () */

/************************/
/*  End of File Comment */
/************************/
