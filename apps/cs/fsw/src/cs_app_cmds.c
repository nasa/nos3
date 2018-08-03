/************************************************************************
 ** File:
 **   $Id: cs_app_cmds.c 1.8 2017/03/29 17:28:59EDT mdeschu Exp  $
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
 **   The CFS Checksum (CS) Application's commands for checking App
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
#include "cs_app_cmds.h"

#include "cfe_platform_cfg.h" /* for CFE_ES_DEFAULT_STACK_SIZE */

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of App command                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableAppCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.AppCSState = CS_STATE_DISABLED;
        CS_ZeroAppTempValues();
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_DISABLE_APP_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of App is Disabled");
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_DisableAppCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of App command                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableAppCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CS_AppData.AppCSState = CS_STATE_ENABLED;
        
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
        CS_UpdateCDS();
#endif
        
        CFE_EVS_SendEvent (CS_ENABLE_APP_INF_EID,
                           CFE_EVS_INFORMATION,
                           "Checksumming of App is Enabled");
        CS_AppData.CmdCounter++;
    }
    return;
} /* End of CS_EnableAppCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the App table    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineAppCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_AppNameCmd_t);
    
    CS_AppNameCmd_t                       * CmdPtr;
    CS_Res_App_Table_Entry_t              * ResultsEntry; 
    uint32                                  Baseline;
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_AppNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[OS_MAX_API_NAME - 1] = '\0';
        if ( CS_GetAppResTblEntryByName(&ResultsEntry, CmdPtr -> Name))
        {
            if (ResultsEntry -> ComputedYet == TRUE)
            {
                Baseline = ResultsEntry -> ComparisonValue;
                CFE_EVS_SendEvent (CS_BASELINE_APP_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of app %s is 0x%08X", 
                                   CmdPtr -> Name,
                                   (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent (CS_NO_BASELINE_APP_INF_EID,
                                   CFE_EVS_INFORMATION,
                                   "Report baseline of app %s has not been computed yet", 
                                   CmdPtr -> Name);   
            }
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_BASELINE_INVALID_NAME_APP_ERR_EID,
                               CFE_EVS_ERROR,
                               "App report baseline failed, app %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_ReportBaselineAppCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the App table cmd      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineAppCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                  ExpectedLength = sizeof(CS_AppNameCmd_t);
    
    uint32                                  ChildTaskID;
    int32                                   Status;
    CS_AppNameCmd_t                       * CmdPtr;
    CS_Res_App_Table_Entry_t              * ResultsEntry;
    
    /* Verify command packet length */
    
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_AppNameCmd_t *) MessagePtr;
        
        if (CS_AppData.RecomputeInProgress == FALSE && CS_AppData.OneShotInProgress == FALSE)
        {
            
            /* make sure the entry is a valid number and is defined in the table */
            
            CmdPtr -> Name[OS_MAX_API_NAME - 1] = '\0';
            
            if (CS_GetAppResTblEntryByName(&ResultsEntry, CmdPtr -> Name))
            {
                /* There is no child task running right now, we can use it*/
                CS_AppData.RecomputeInProgress           = TRUE;
                
                /* fill in child task variables */
                CS_AppData.ChildTaskTable                = CS_APP_TABLE;
                                
                
                CS_AppData.RecomputeAppEntryPtr = ResultsEntry;
                
                Status= CFE_ES_CreateChildTask(&ChildTaskID,
                                               CS_RECOMP_APP_TASK_NAME,
                                               CS_RecomputeAppChildTask,
                                               NULL,
                                               CFE_ES_DEFAULT_STACK_SIZE,
                                               CS_CHILD_TASK_PRIORITY,
                                               0);
                if (Status ==CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_APP_STARTED_DBG_EID,
                                       CFE_EVS_DEBUG,
                                       "Recompute baseline of app %s started", 
                                       CmdPtr -> Name);
                    CS_AppData.CmdCounter++;
                }
                else/* child task creation failed */
                {
                    CFE_EVS_SendEvent (CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID,
                                       CFE_EVS_ERROR,
                                       "Recompute baseline of app %s failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                       CmdPtr -> Name,
                                       (unsigned int)Status);
                    CS_AppData.CmdErrCounter++;
                    CS_AppData.RecomputeInProgress = FALSE;
                }
            }
            else
            {
                CFE_EVS_SendEvent (CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "App recompute baseline failed, app %s not found",
                                   CmdPtr -> Name);
                CS_AppData.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent (CS_RECOMPUTE_APP_CHDTASK_ERR_EID,
                               CFE_EVS_ERROR,
                                "App recompute baseline for app %s failed: child task in use",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
}/* end CS_RecomputeBaselineAppCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the App table command            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableNameAppCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                 ExpectedLength = sizeof(CS_AppNameCmd_t);
    CS_AppNameCmd_t                      * CmdPtr;
     
    CS_Res_App_Table_Entry_t             * ResultsEntry;
    CS_Def_App_Table_Entry_t             * DefinitionEntry; 
    
    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength)  )
    {
        CmdPtr = (CS_AppNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[OS_MAX_API_NAME - 1] = '\0';
        
        if ( CS_GetAppResTblEntryByName( &ResultsEntry, CmdPtr -> Name))
        {
            
            ResultsEntry -> State = CS_STATE_DISABLED;
            ResultsEntry -> TempChecksumValue = 0;
            ResultsEntry -> ByteOffset = 0;
            
            CFE_EVS_SendEvent (CS_DISABLE_APP_NAME_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of app %s is Disabled", 
                               CmdPtr -> Name);
            
            if ( CS_GetAppDefTblEntryByName(&DefinitionEntry, CmdPtr -> Name))
            {
                DefinitionEntry -> State = CS_STATE_DISABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.AppResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefAppTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update apps definition table for entry %s", 
                                   CmdPtr -> Name);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID,
                               CFE_EVS_ERROR,
                               "App disable app command failed, app %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_DisableNameAppCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the App table command             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableNameAppCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16                                 ExpectedLength = sizeof(CS_AppNameCmd_t);
    CS_AppNameCmd_t                      * CmdPtr;
    
    CS_Res_App_Table_Entry_t             * ResultsEntry; 
    CS_Def_App_Table_Entry_t             * DefinitionEntry; 

    /* Verify command packet length */
    if ( CS_VerifyCmdLength (MessagePtr,ExpectedLength) )
    {
        CmdPtr = (CS_AppNameCmd_t *) MessagePtr;
        
        CmdPtr -> Name[OS_MAX_API_NAME -1 ] = '\0';
     
        if ( CS_GetAppResTblEntryByName(&ResultsEntry,CmdPtr -> Name))
        {
            ResultsEntry -> State = CS_STATE_ENABLED;
            
            CFE_EVS_SendEvent (CS_ENABLE_APP_NAME_INF_EID,
                               CFE_EVS_INFORMATION,
                               "Checksumming of app %s is Enabled", 
                                CmdPtr -> Name);
            
            if ( CS_GetAppDefTblEntryByName(&DefinitionEntry, CmdPtr -> Name))
            {
                DefinitionEntry -> State = CS_STATE_ENABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.AppResTablesTblPtr);                
                CFE_TBL_Modified(CS_AppData.DefAppTableHandle);
            }
            else 
            {
                CFE_EVS_SendEvent (CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID,
                                   CFE_EVS_DEBUG,
                                   "CS unable to update apps definition table for entry %s", 
                                   CmdPtr -> Name);
            }
            
            CS_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent (CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID,
                               CFE_EVS_ERROR,
                               "App enable app command failed, app %s not found",
                               CmdPtr -> Name);
            CS_AppData.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_EnableNameAppCmd () */

/************************/
/*  End of File Comment */
/************************/
