/************************************************************************
 ** File:
 **   $Id: cs_eeprom_cmds.c 1.8 2017/03/29 17:29:02EDT mdeschu Exp  $
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
 **   The CFS Checksum (CS) Application's commands for checking Eeprom
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
#include "cs_eeprom_cmds.h"
#include "cs_utils.h"

#include "cfe_platform_cfg.h" /* for CFE_ES_DEFAULT_STACK_SIZE */

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of Eeprom command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.EepromCSState = CS_STATE_DISABLED;
        CS_ZeroEepromTempValues();
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_DISABLE_EEPROM_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Eeprom is Disabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableEepromCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Eeprom command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.EepromCSState = CS_STATE_ENABLED;
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_ENABLE_EEPROM_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Eeprom is Enabled");
        
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_EnableEepromCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Eeprom table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineEntryIDEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    
    CS_EntryCmd_t                         * CmdPtr         = 0;
    uint32                                  Baseline       = 0;
    uint16                                  EntryID        = 0;
    uint16                                  State          = CS_STATE_EMPTY;
    CS_Res_EepromMemory_Table_Entry_t       ResultsEntry; 
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_EntryCmd_t *) MessagePtr;
        EntryID = CmdPtr -> EntryID;
        
        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = CS_AppData.ResEepromTblPtr[EntryID];
            
            if (ResultsEntry.ComputedYet == TRUE)
            {
                Baseline = ResultsEntry.ComparisonValue;
                
                CFE_EVS_SendEvent (CS_BASELINE_EEPROM_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of Eeprom Entry %d is 0x%08X", 
                                   EntryID,
                                   (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent (CS_NO_BASELINE_EEPROM_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of Eeprom Entry %d has not been computed yet", 
                                   EntryID);   
            }
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID,
                               CFE_EVS_ERROR,
                               "Eeprom report baseline failed, Entry ID invalid: %d, State: %d Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_ReportBaselineEntryIDCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Eeprom table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineEepromCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_EntryCmd_t);
    
    uint32                                  ChildTaskID    = 0;
    int32                                   Status         = -1;
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
            if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
                (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY) )
            {
                
                /* There is no child task running right now, we can use it*/
                CS_AppData.RecomputeInProgress           = TRUE;
                
                /* fill in child task variables */
                CS_AppData.ChildTaskTable                = CS_EEPROM_TABLE;
                CS_AppData.ChildTaskEntryID              = EntryID;
                
                
                CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.ResEepromTblPtr[EntryID];
                
                Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                               CS_RECOMP_EEPROM_TASK_NAME,
                                               CS_RecomputeEepromMemoryChildTask,
                                               NULL,
                                               CFE_ES_DEFAULT_STACK_SIZE,
                                               CS_CHILD_TASK_PRIORITY,
                                               0);
                if (Status ==CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_EEPROM_STARTED_DBG_EID,
                                       CFE_EVS_DEBUG,
                                       "Recompute baseline of Eeprom Entry ID %d started", 
                                       EntryID);
                    CS_AppData.CmdCounter++;
                }
                else/* child task creation failed */
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "Recompute baseline of Eeprom Entry ID %d failed, CFE_ES_CreateChildTask returned:  0x%08X",
                                       EntryID,
                                       (unsigned int)Status);
                    CS_AppData.CmdErrCounter++;
                    CS_AppData.RecomputeInProgress = FALSE;
                }
            }
            else
            {
                if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
                {
                    State = CS_STATE_UNDEFINED;
                }
                else
                {
                    State = CS_AppData.ResEepromTblPtr[EntryID].State;
                }
            
                CFE_EVS_SendEvent (CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Eeprom recompute baseline of entry failed, Entry ID invalid: %d, State: %d, Max ID: %d",
                                   EntryID,
                                   State,
                                   (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
                
                CS_AppData.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                               "Recompute baseline of Eeprom Entry ID %d failed: child task in use",
                               EntryID);
            CS_AppData.CmdErrCounter++;
        }
    }    
    return;
}/* end CS_RecomputeBaselineEepromCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Eeprom table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEntryIDEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
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
                
        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = &CS_AppData.ResEepromTblPtr[EntryID]; 
             
            ResultsEntry -> State = CS_STATE_ENABLED;
            
            CFE_EVS_SendEvent (CS_ENABLE_EEPROM_ENTRY_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of Eeprom Entry ID %d is Enabled", 
                                EntryID);
            
            if (CS_AppData.DefEepromTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefEepromTblPtr[EntryID].State = CS_STATE_ENABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.EepResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefEepromTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update Eeprom definition table for entry %d, State: %d",
                                   EntryID,
                                   State);
            }
        
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID,
                               CFE_EVS_ERROR,
                               "Enable Eeprom entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_EnableCSEntryIDEepromCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Eeprom table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEntryIDEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
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
        
        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY) )
        {
            ResultsEntry = & CS_AppData.ResEepromTblPtr[EntryID]; 
            
            ResultsEntry -> State = CS_STATE_DISABLED;
            ResultsEntry -> TempChecksumValue = 0;
            ResultsEntry -> ByteOffset = 0;
            
            CFE_EVS_SendEvent (CS_DISABLE_EEPROM_ENTRY_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of Eeprom Entry ID %d is Disabled", 
                               EntryID);
            
            if (CS_AppData.DefEepromTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefEepromTblPtr[EntryID].State = CS_STATE_DISABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.EepResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefEepromTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update Eeprom definition table for entry %d, State: %d",
                                   EntryID,
                                   State);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }
            
            CFE_EVS_SendEvent (CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID,
                               CFE_EVS_ERROR,
                               "Disable Eeprom entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                               EntryID,
                               State,
                               (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
            
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_DisableCSEntryIDEepromCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Retrieve an EntryID based on Address from Eeprom table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GetEntryIDEepromCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength      = sizeof(CS_GetEntryIDCmd_t);
    
    CS_GetEntryIDCmd_t                    * CmdPtr              = NULL;
    CS_Res_EepromMemory_Table_Entry_t     * StartOfResultsTable = NULL; 
    uint16                                  Loop                = 0;
    boolean                                 EntryFound          = FALSE;
    CS_Res_EepromMemory_Table_Entry_t       ResultsEntry;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_GetEntryIDCmd_t *) MessagePtr;
        
        StartOfResultsTable = CS_AppData.ResEepromTblPtr;   
        
        for ( Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop ++)
        {
            ResultsEntry = StartOfResultsTable[Loop];
            
            if ( (ResultsEntry.StartAddress <= CmdPtr -> Address) &&
                CmdPtr -> Address <= (ResultsEntry.StartAddress + ResultsEntry.NumBytesToChecksum) &&
                ResultsEntry.State != CS_STATE_EMPTY)
            {
                
                CFE_EVS_SendEvent (CS_GET_ENTRY_ID_EEPROM_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Eeprom Found Address 0x%08X in Entry ID %d", 
                                   (unsigned int)(CmdPtr -> Address),
                                   Loop);
                EntryFound = TRUE;
            }
        }
        
        if (EntryFound == FALSE)
        {
            CFE_EVS_SendEvent (CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Address 0x%08X was not found in Eeprom table",
                               (unsigned int)(CmdPtr -> Address));
        }
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_GetEntryIDEepromCmd () */

/************************/
/*  End of File Comment */
/************************/
