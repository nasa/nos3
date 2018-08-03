/************************************************************************
 ** File:
 **   $Id: cs_table_cmds.c 1.8 2017/03/29 17:29:01EDT mdeschu Exp  $
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
 **   The CFS Checksum (CS) Application's commands for checking Tables
 **
 *************************************************************************/

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_table_cmds.h"

#include "cfe_platform_cfg.h" /* for CFE_ES_DEFAULT_STACK_SIZE */

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of Tables command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableTablesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.TablesCSState = CS_STATE_DISABLED;
        CS_ZeroTablesTempValues();
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_DISABLE_TABLES_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Tables is Disabled");
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableTablesCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Tables command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableTablesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.TablesCSState = CS_STATE_ENABLED;
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_ENABLE_TABLES_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of Tables is Enabled");
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_EnableTablesCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Tables table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineTablesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_TableNameCmd_t);
    
    CS_TableNameCmd_t                     * CmdPtr;
    CS_Res_Tables_Table_Entry_t           * ResultsEntry; 
    uint32                                  Baseline;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_TableNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
        if ( CS_GetTableResTblEntryByName(&ResultsEntry, CmdPtr -> Name))
        {
            if (ResultsEntry -> ComputedYet == TRUE)
            {
                Baseline = ResultsEntry -> ComparisonValue;
                CFE_EVS_SendEvent (CS_BASELINE_TABLES_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of table %s is 0x%08X", 
                                   CmdPtr -> Name,
                                   (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent (CS_NO_BASELINE_TABLES_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of table %s has not been computed yet", 
                                   CmdPtr -> Name);   
            }
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_BASELINE_INVALID_NAME_TABLES_ERR_EID,
                               CFE_EVS_ERROR,
                               "Tables report baseline failed, table %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_ReportBaselineTablesCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Tables table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineTablesCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_TableNameCmd_t);
    
    uint32                                  ChildTaskID;
    int32                                   Status;
    CS_TableNameCmd_t                     * CmdPtr;
    CS_Res_Tables_Table_Entry_t           * ResultsEntry;
    
    /* Verify command packet length */
    
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_TableNameCmd_t *) MessagePtr;
        
        if (CS_AppData.RecomputeInProgress == FALSE && CS_AppData.OneShotInProgress == FALSE)
        {            
            /* make sure the entry is a valid number and is defined in the table */
            
            CmdPtr -> Name[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
            
            if (CS_GetTableResTblEntryByName(&ResultsEntry, CmdPtr -> Name))
            {
                /* There is no child task running right now, we can use it*/
                CS_AppData.RecomputeInProgress           = TRUE;
                
                /* fill in child task variables */
                CS_AppData.ChildTaskTable                = CS_TABLES_TABLE;
                
                
                
                CS_AppData.RecomputeTablesEntryPtr = ResultsEntry;
                
                Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                               CS_RECOMP_TABLES_TASK_NAME,
                                               CS_RecomputeTablesChildTask,
                                               NULL,
                                               CFE_ES_DEFAULT_STACK_SIZE,
                                               CS_CHILD_TASK_PRIORITY,
                                               0);
                if (Status ==CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_TABLES_STARTED_DBG_EID,
                                       CFE_EVS_DEBUG,
                                       "Recompute baseline of table %s started", 
                                       CmdPtr -> Name);
                    CS_AppData.CmdCounter++;
                }
                else/* child task creation failed */
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "Recompute baseline of table %s failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                       CmdPtr -> Name,
                                       (unsigned int)Status);
                    CS_AppData.CmdErrCounter++;
                    CS_AppData.RecomputeInProgress = FALSE;
                }
            }
            else
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Tables recompute baseline failed, table %s not found",
                                   CmdPtr -> Name);
                CS_AppData.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                                "Tables recompute baseline for table %s failed: child task in use",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_RecomputeBaselineTablesCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Tables table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableNameTablesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_TableNameCmd_t);
    CS_TableNameCmd_t                     * CmdPtr;
    
    CS_Res_Tables_Table_Entry_t           * ResultsEntry;
    CS_Def_Tables_Table_Entry_t           * DefinitionEntry;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength)  )
    {
        CmdPtr = (CS_TableNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
        
        if ( CS_GetTableResTblEntryByName( &ResultsEntry, CmdPtr -> Name))
        {
            
            ResultsEntry -> State = CS_STATE_DISABLED;
            ResultsEntry -> TempChecksumValue = 0;
            ResultsEntry -> ByteOffset = 0;
            
            CFE_EVS_SendEvent (CS_DISABLE_TABLES_NAME_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of table %s is Disabled", 
                               CmdPtr -> Name);
             
            if ( CS_GetTableDefTblEntryByName( &DefinitionEntry, CmdPtr -> Name))
            {
                DefinitionEntry -> State = CS_STATE_DISABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefTablesTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_DISABLE_TABLE_DEF_NOT_FOUND_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update tables definition table for entry %s", 
                                   CmdPtr -> Name);
            }

            
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID,
                               CFE_EVS_ERROR,
                               "Tables disable table command failed, table %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_DisableNameTablesCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Tables table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableNameTablesCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                 ExpectedLength = sizeof(CS_TableNameCmd_t);
    CS_TableNameCmd_t                    * CmdPtr;
    
    CS_Res_Tables_Table_Entry_t          * ResultsEntry; 
    CS_Def_Tables_Table_Entry_t           * DefinitionEntry;

    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_TableNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[CFE_TBL_MAX_FULL_NAME_LEN -1 ] = '\0';
     
        if ( CS_GetTableResTblEntryByName(&ResultsEntry,CmdPtr -> Name))
        {
            ResultsEntry -> State = CS_STATE_ENABLED;
            
            CFE_EVS_SendEvent (CS_ENABLE_TABLES_NAME_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of table %s is Enabled", 
                                CmdPtr -> Name);
            
            if ( CS_GetTableDefTblEntryByName( &DefinitionEntry, CmdPtr -> Name))
            {
                DefinitionEntry -> State = CS_STATE_ENABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefTablesTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_ENABLE_TABLE_DEF_NOT_FOUND_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update tables definition table for entry %s", 
                                   CmdPtr -> Name);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID,
                               CFE_EVS_ERROR,
                               "Tables enable table command failed, table %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_EnableNameTablesCmd () */

/************************/
/*  End of File Comment */
/************************/

