/************************************************************************
** File:
**   $Id: cs_app.c 1.17 2017/03/29 17:29:00EDT mdeschu Exp  $
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
**   CFS Checksum (CS) Applications provides the service of background
**   checksumming user defined objects in the CFS
** 
*************************************************************************/
#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
/*************************************************************************
**
** Macro definitions
**
**************************************************************************/
#define CS_PIPE_NAME                    "CS_CMD_PIPE"
#define CS_NUM_DATA_STORE_STATES        6 /* 4 tables + OS CS + cFE core number of checksum states for CDS */

/*************************************************************************
**
** Exported data
**
**************************************************************************/
CS_AppData_t        CS_AppData;

/************************************************************************/
/** \brief Initialize the Checksum CFS application
 **  
 **  \par Description
 **       Checksum application initialization routine. This 
 **       function performs all the required startup steps to 
 **       get the application registered with the cFE services so
 **       it can begin to receive command messages and begin 
 **       background checksumming. 
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
int32 CS_AppInit (void);

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
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 **  \sa #CFE_SB_RcvMsg
 **
 *************************************************************************/
int32 CS_AppPipe (CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process housekeeping request
 **  
 **  \par Description
 **       Processes an on-board housekeeping request message.
 **
 **  \par Assumptions, External Events, and Notes:
 **       This command does not affect the command execution counter
 **       
 **  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
 **                             references the software bus message 
 **
 *************************************************************************/
void CS_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr);

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
/************************************************************************/
/** \brief Restore tables states from CDS if enabled
 **  
 **  \par Description
 **       Restore CS state of tables from CDS
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **
 **
 *************************************************************************/
int32 CS_CreateRestoreStatesFromCDS(void);
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_AppMain (void)
{
    int32               Result = 0;
    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry (CS_APPMAIN_PERF_ID);
    
    /* Register application */
    Result = CFE_ES_RegisterApp();
    
    /* Perform application specific initialization */
    if (Result == CFE_SUCCESS)
    {
        Result = CS_AppInit();
    }
    
    /* Check for start-up error */
    if (Result != CFE_SUCCESS)
    {
        /* Set request to terminate main loop */
        CS_AppData.RunStatus = CFE_ES_APP_ERROR;
    }
    
    CFE_ES_WaitForStartupSync(CS_STARTUP_TIMEOUT);
    
    /* Main process loop */
    while (CFE_ES_RunLoop(&CS_AppData.RunStatus))
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit (CS_APPMAIN_PERF_ID);
        
        /* Wait for the next Software Bus message */
        Result = CFE_SB_RcvMsg (&CS_AppData.MsgPtr,
                                CS_AppData.CmdPipe,
                                CFE_SB_PEND_FOREVER);
        
        /* Performance Log (start time counter)  */
        CFE_ES_PerfLogEntry (CS_APPMAIN_PERF_ID);
        
        if (Result == CFE_SUCCESS)
        {
            /* Process Software Bus message */
            Result = CS_AppPipe (CS_AppData.MsgPtr);
        }
        
        /*
         ** Note: If there were some reason to exit the task
         **       normally (without error) then we would set
         **       RunStatus = CFE_ES_APP_EXIT
         */
        if (Result != CFE_SUCCESS)
        {
            /* Set request to terminate main loop */
            CS_AppData.RunStatus = CFE_ES_APP_ERROR;
        }
    }/* end run loop */
    
    /* Check for "fatal" process error */
    if (CS_AppData.RunStatus == CFE_ES_APP_ERROR || CS_AppData.RunStatus == CFE_ES_SYS_EXCEPTION )
    {
        /* Send an error event with run status and result */
        CFE_EVS_SendEvent(CS_EXIT_ERR_EID, 
                          CFE_EVS_ERROR,
                          "App terminating, RunStatus:0x%08X, RC:0x%08X", 
                          (unsigned int)CS_AppData.RunStatus,
                          (unsigned int)Result);
    }
    else
    {
        /* Send an informational event describing the reason for the termination */
        CFE_EVS_SendEvent(CS_EXIT_INF_EID, 
                          CFE_EVS_INFORMATION,
                          "App terminating, RunStatus:0x%08X", 
                          (unsigned int)CS_AppData.RunStatus);
    }
    
    /* In case cFE Event Services is not working */
    CFE_ES_WriteToSysLog("CS App terminating, RunStatus:0x%08X, RC:0x%08X\n",
                         (unsigned int)CS_AppData.RunStatus,
                         (unsigned int)Result);
        
    /* Performance Log (stop time counter) */
    CFE_ES_PerfLogExit(CS_APPMAIN_PERF_ID);
    
    
     /* Let cFE kill the task (and child task) */
    CFE_ES_ExitApp(CS_AppData.RunStatus);
    
} /* End of CS_AppMain () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Application initialization function                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_AppInit (void)
{
    int32                                       Result = CFE_SUCCESS;
    int32                                       ResultInit;
    int32                                       ResultSegment;
    uint32                                      KernelSize;
    uint32                                      KernelAddress;
    uint32                                      CFESize;
    uint32                                      CFEAddress;
    
    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, 0);

    if (Result != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("CS App: Error Registering For Event Services, RC = 0x%08X\n", (unsigned int)Result);
        return Result;
    }
        
    /* Zero out all data in CS_AppData, including the housekeeping data*/
    CFE_PSP_MemSet (& CS_AppData, 0, (unsigned) sizeof (CS_AppData) );
    
    CS_AppData.RunStatus = CFE_ES_APP_RUN;
    
    /* Initialize app configuration data */
    strncpy(CS_AppData.PipeName, CS_CMD_PIPE_NAME, CS_CMD_PIPE_NAME_LEN);
    
    CS_AppData.PipeDepth = CS_PIPE_DEPTH;
    
    /* Initialize housekeeping packet */
    CFE_SB_InitMsg (& CS_AppData.HkPacket,
                    CS_HK_TLM_MID, 
                    sizeof (CS_HkPacket_t),
                    TRUE);
    

    /* Create Software Bus message pipe */
    
    Result = CFE_SB_CreatePipe (& CS_AppData.CmdPipe,
                                CS_AppData.PipeDepth,
                                CS_AppData.PipeName);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent (CS_INIT_SB_CREATE_ERR_EID,
                           CFE_EVS_ERROR,
                           "Software Bus Create Pipe for command returned: 0x%08X",(unsigned int)Result);
        return Result;
    }
    
    /* Subscribe to Housekeeping request commands */
    
    Result = CFE_SB_Subscribe (CS_SEND_HK_MID,
                               CS_AppData.CmdPipe);
    
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_HK_ERR_EID,
                           CFE_EVS_ERROR,
                           "Software Bus subscribe to housekeeping returned: 0x%08X",(unsigned int)Result);
        return Result;
    }
    
    
    /* Subscribe to background checking schedule */
    
    Result = CFE_SB_Subscribe( CS_BACKGROUND_CYCLE_MID,
                              CS_AppData.CmdPipe);
    
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID,
                           CFE_EVS_ERROR,
                           "Software Bus subscribe to background cycle returned: 0x%08X",(unsigned int)Result);
        return Result;
    }
    
    
    /* Subscribe to CS Internal command packets */
    
    Result = CFE_SB_Subscribe (CS_CMD_MID,
                               CS_AppData.CmdPipe);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID,
                           CFE_EVS_ERROR,
                           "Software Bus subscribe to command returned: 0x%08X",(unsigned int)Result);
        return Result;
    }
    
    /* Set up default tables in memory */
    CS_InitializeDefaultTables();

    CS_AppData.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
    CS_AppData.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
    CS_AppData.AppCSState    = CS_APPS_TBL_POWERON_STATE;
    CS_AppData.TablesCSState = CS_TABLES_TBL_POWERON_STATE;
    
    CS_AppData.OSCSState      = CS_OSCS_CHECKSUM_STATE;
    CS_AppData.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;
   
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)
    Result = CS_CreateRestoreStatesFromCDS();
#endif
    
    
    ResultInit = CS_TableInit(& CS_AppData.DefEepromTableHandle,
                              & CS_AppData.ResEepromTableHandle,
                              (void*) & CS_AppData.DefEepromTblPtr,
                              (void*) &CS_AppData.ResEepromTblPtr,
                              CS_EEPROM_TABLE, 
                              CS_DEF_EEPROM_TABLE_NAME,
                              CS_RESULTS_EEPROM_TABLE_NAME,
                              CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                              CS_DEF_EEPROM_TABLE_FILENAME,
                              &CS_AppData.DefaultEepromDefTable,
                              sizeof(CS_Def_EepromMemory_Table_Entry_t),
                              sizeof(CS_Res_EepromMemory_Table_Entry_t),
                              CS_ValidateEepromChecksumDefinitionTable);
    
    if(ResultInit != CFE_SUCCESS)
    {
        CS_AppData.EepromCSState = CS_STATE_DISABLED;
        CFE_EVS_SendEvent (CS_INIT_EEPROM_ERR_EID,
                           CFE_EVS_ERROR,
                           "Table initialization failed for Eeprom: 0x%08X",
                           (unsigned int)ResultInit);
        return (ResultInit);
    }
    
    ResultInit = CS_TableInit(& CS_AppData.DefMemoryTableHandle,
                              & CS_AppData.ResMemoryTableHandle,
                              (void*) & CS_AppData.DefMemoryTblPtr,
                              (void*) & CS_AppData.ResMemoryTblPtr,
                              CS_MEMORY_TABLE, 
                              CS_DEF_MEMORY_TABLE_NAME,
                              CS_RESULTS_MEMORY_TABLE_NAME,
                              CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                              CS_DEF_MEMORY_TABLE_FILENAME,
                              &CS_AppData.DefaultMemoryDefTable,
                              sizeof(CS_Def_EepromMemory_Table_Entry_t),
                              sizeof(CS_Res_EepromMemory_Table_Entry_t),
                              CS_ValidateMemoryChecksumDefinitionTable);
    
    
    if(ResultInit != CFE_SUCCESS)
    {
        CS_AppData.MemoryCSState = CS_STATE_DISABLED;
        CFE_EVS_SendEvent (CS_INIT_MEMORY_ERR_EID,
                           CFE_EVS_ERROR,
                           "Table initialization failed for Memory: 0x%08X",
                           (unsigned int)ResultInit);
        return (ResultInit);
    }
    
    ResultInit= CS_TableInit(& CS_AppData.DefAppTableHandle,
                             & CS_AppData.ResAppTableHandle,
                             (void*) & CS_AppData.DefAppTblPtr,
                             (void*) & CS_AppData.ResAppTblPtr,
                             CS_APP_TABLE, 
                             CS_DEF_APP_TABLE_NAME,
                             CS_RESULTS_APP_TABLE_NAME,
                             CS_MAX_NUM_APP_TABLE_ENTRIES,
                             CS_DEF_APP_TABLE_FILENAME,
                             &CS_AppData.DefaultAppDefTable,
                             sizeof(CS_Def_App_Table_Entry_t),
                             sizeof(CS_Res_App_Table_Entry_t),
                             CS_ValidateAppChecksumDefinitionTable);
    
    if(ResultInit != CFE_SUCCESS)
    {
        CS_AppData.AppCSState = CS_STATE_DISABLED;
        CFE_EVS_SendEvent (CS_INIT_APP_ERR_EID,
                           CFE_EVS_ERROR,
                           "Table initialization failed for Apps: 0x%08X",
                           (unsigned int)ResultInit);
        return (ResultInit);
    }

    ResultInit = CS_TableInit(& CS_AppData.DefTablesTableHandle,
                              & CS_AppData.ResTablesTableHandle,
                              (void*) & CS_AppData.DefTablesTblPtr,
                              (void*) & CS_AppData.ResTablesTblPtr,
                              CS_TABLES_TABLE, 
                              CS_DEF_TABLES_TABLE_NAME,
                              CS_RESULTS_TABLES_TABLE_NAME,
                              CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                              CS_DEF_TABLES_TABLE_FILENAME,
                              &CS_AppData.DefaultTablesDefTable,
                              sizeof(CS_Def_Tables_Table_Entry_t),
                              sizeof(CS_Res_Tables_Table_Entry_t),
                              CS_ValidateTablesChecksumDefinitionTable);
    
    if(ResultInit != CFE_SUCCESS)
    {
        CS_AppData.TablesCSState = CS_STATE_DISABLED;
        CFE_EVS_SendEvent (CS_INIT_TABLES_ERR_EID,
                           CFE_EVS_ERROR,
                           "Table initialization failed for Tables: 0x%08X",
                           (unsigned int)ResultInit);
        return (ResultInit);
    }
    

    /* Initalize the CFE core segments */
    CFE_PSP_GetCFETextSegmentInfo((void*) &CFEAddress, &CFESize);
    
    CS_AppData.CfeCoreCodeSeg.StartAddress           = CFEAddress;
    CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum     = CFESize;
    CS_AppData.CfeCoreCodeSeg.ComputedYet            = FALSE;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue        = 0;
    CS_AppData.CfeCoreCodeSeg.ByteOffset             = 0;
    CS_AppData.CfeCoreCodeSeg.TempChecksumValue      = 0;
    CS_AppData.CfeCoreCodeSeg.State                  = CS_STATE_ENABLED;
    
    /* Initialize the OS Core code segment*/
    
    ResultSegment  = CFE_PSP_GetKernelTextSegmentInfo( &KernelAddress, &KernelSize);
    
    if (ResultSegment != OS_SUCCESS)
    {
        CS_AppData.OSCodeSeg.StartAddress           = 0;
        CS_AppData.OSCodeSeg.NumBytesToChecksum     = 0;
        CS_AppData.OSCodeSeg.ComputedYet            = FALSE;
        CS_AppData.OSCodeSeg.ComparisonValue        = 0;
        CS_AppData.OSCodeSeg.ByteOffset             = 0;
        CS_AppData.OSCodeSeg.TempChecksumValue      = 0;
        CS_AppData.OSCodeSeg.State                  = CS_STATE_DISABLED;
        
        
        CFE_EVS_SendEvent (CS_OS_TEXT_SEG_INF_EID,
                           CFE_EVS_INFORMATION,
                           "OS Text Segment disabled due to platform");
    }
    else
    {
        CS_AppData.OSCodeSeg.StartAddress           = KernelAddress;
        CS_AppData.OSCodeSeg.NumBytesToChecksum     = KernelSize;
        CS_AppData.OSCodeSeg.ComputedYet            = FALSE;
        CS_AppData.OSCodeSeg.ComparisonValue        = 0;
        CS_AppData.OSCodeSeg.ByteOffset             = 0;
        CS_AppData.OSCodeSeg.TempChecksumValue      = 0;
        CS_AppData.OSCodeSeg.State                  = CS_STATE_ENABLED;
        
    }

    
    /* initialize the place to ostart background checksumming */
    CS_AppData.CurrentCSTable      = 0;
    CS_AppData.CurrentEntryInTable = 0;
    
    
    /* Initial settings for the CS Application */
    /* the rest of the tables are initialized in CS_TableInit */
    CS_AppData.ChecksumState  = CS_STATE_ENABLED;
    
    
    CS_AppData.RecomputeInProgress    = FALSE;
    CS_AppData.OneShotInProgress  = FALSE;
    
    
    CS_AppData.MaxBytesPerCycle = CS_DEFAULT_BYTES_PER_CYCLE;
    
    /* Application startup event message */
    if (Result == CFE_SUCCESS)
    {
        Result = CFE_EVS_SendEvent (CS_INIT_INF_EID,
                                    CFE_EVS_INFORMATION,
                                    "CS Initialized. Version %d.%d.%d.%d",
                                    CS_MAJOR_VERSION,
                                    CS_MINOR_VERSION,
                                    CS_REVISION,
                                    CS_MISSION_REV);
    }
    return (Result);
} /* End of CS_AppInit () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS's command pipe processing                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_AppPipe (CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t          MessageID = 0;
    uint16                  CommandCode = 0;
    int32                   Result = CFE_SUCCESS;
        
    MessageID = CFE_SB_GetMsgId(MessagePtr);
    switch (MessageID)
    {
            /* Housekeeping telemetry request */
        case CS_SEND_HK_MID:
            CS_HousekeepingCmd(MessagePtr);
            
            /* update each table if there is no recompute happening on that table */
            
            if (!((CS_AppData.RecomputeInProgress == TRUE)  && 
                  ( CS_AppData.OneShotInProgress == FALSE) && 
                  (CS_AppData.ChildTaskTable == CS_EEPROM_TABLE)))
            {
                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefEepromTblPtr,
                                               (void*) & CS_AppData.ResEepromTblPtr,
                                               CS_AppData.DefEepromTableHandle,
                                               CS_AppData.ResEepromTableHandle,
                                               CS_EEPROM_TABLE,
                                               CS_MAX_NUM_EEPROM_TABLE_ENTRIES);
                
                if(Result != CFE_SUCCESS)
                {
                    CS_AppData.EepromCSState = CS_STATE_DISABLED;
                    Result = CFE_EVS_SendEvent (CS_UPDATE_EEPROM_ERR_EID,
                                                CFE_EVS_ERROR,
                                                "Table update failed for Eeprom: 0x%08X, checksumming Eeprom is disabled",
                                                (unsigned int)Result);
                }
            }
            
            if (!((CS_AppData.RecomputeInProgress == TRUE)  && 
                  ( CS_AppData.OneShotInProgress == FALSE) && 
                  (CS_AppData.ChildTaskTable == CS_MEMORY_TABLE)))
            {
                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefMemoryTblPtr,
                                               (void*) & CS_AppData.ResMemoryTblPtr,
                                               CS_AppData.DefMemoryTableHandle,
                                               CS_AppData.ResMemoryTableHandle,
                                               CS_MEMORY_TABLE,
                                               CS_MAX_NUM_MEMORY_TABLE_ENTRIES);
                if(Result != CFE_SUCCESS)
                {
                    CS_AppData.MemoryCSState = CS_STATE_DISABLED;
                    Result = CFE_EVS_SendEvent (CS_UPDATE_MEMORY_ERR_EID,
                                                CFE_EVS_ERROR,
                                                "Table update failed for Memory: 0x%08X, checksumming Memory is disabled",
                                                (unsigned int)Result);
                }
            }
            
            if (!((CS_AppData.RecomputeInProgress == TRUE)  && 
                  ( CS_AppData.OneShotInProgress == FALSE) && 
                  (CS_AppData.ChildTaskTable == CS_APP_TABLE)))
            {
                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefAppTblPtr,
                                               (void*) & CS_AppData.ResAppTblPtr,
                                               CS_AppData.DefAppTableHandle,
                                               CS_AppData.ResAppTableHandle,
                                               CS_APP_TABLE,
                                               CS_MAX_NUM_APP_TABLE_ENTRIES);
                if(Result != CFE_SUCCESS)
                {
                    CS_AppData.AppCSState = CS_STATE_DISABLED;
                    Result = CFE_EVS_SendEvent (CS_UPDATE_APP_ERR_EID,
                                                CFE_EVS_ERROR,
                                                "Table update failed for Apps: 0x%08X, checksumming Apps is disabled",
                                                (unsigned int)Result);
                }
                
                
            }
            
            if (!((CS_AppData.RecomputeInProgress == TRUE)  && 
                  ( CS_AppData.OneShotInProgress == FALSE) && 
                  (CS_AppData.ChildTaskTable == CS_TABLES_TABLE)))
            {
                Result = CS_HandleTableUpdate ((void*) & CS_AppData.DefTablesTblPtr,
                                               (void*) & CS_AppData.ResTablesTblPtr,
                                               CS_AppData.DefTablesTableHandle,
                                               CS_AppData.ResTablesTableHandle,
                                               CS_TABLES_TABLE,
                                               CS_MAX_NUM_TABLES_TABLE_ENTRIES);
                
                if(Result != CFE_SUCCESS)
                {
                    CS_AppData.TablesCSState = CS_STATE_DISABLED;
                    Result = CFE_EVS_SendEvent (CS_UPDATE_TABLES_ERR_EID,
                                                CFE_EVS_ERROR,
                                                "Table update failed for Tables: 0x%08X, checksumming Tables is disabled",
                                                (unsigned int)Result);
                }
                
            }

            break;
            
        case CS_BACKGROUND_CYCLE_MID:
            CS_BackgroundCheckCmd (MessagePtr);
            break;   
                        
        case CS_CMD_MID:
            
            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
        {
                /* All CS Commands */
            case CS_NOOP_CC:
                CS_NoopCmd (MessagePtr);
                break;
                
            case CS_RESET_CC:
                CS_ResetCmd (MessagePtr);
                break;
                
            case CS_ONESHOT_CC:
                CS_OneShotCmd(MessagePtr);
                break;
                
            case CS_CANCEL_ONESHOT_CC:
                CS_CancelOneShotCmd(MessagePtr);
                break;                
            
            case CS_ENABLE_ALL_CS_CC:
                CS_EnableAllCSCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_ALL_CS_CC:
                CS_DisableAllCSCmd(MessagePtr);
                break;                
                
            /* cFE core Commands */                
            case CS_ENABLE_CFECORE_CC:
                CS_EnableCfeCoreCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_CFECORE_CC:
                CS_DisableCfeCoreCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_CFECORE_CC:
                CS_ReportBaselineCfeCoreCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_CFECORE_CC:
                CS_RecomputeBaselineCfeCoreCmd(MessagePtr);
                break;                
              
                /* OS Commands */
            case CS_ENABLE_OS_CC:
                CS_EnableOSCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_OS_CC:
                CS_DisableOSCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_OS_CC:
                CS_ReportBaselineOSCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_OS_CC:
                CS_RecomputeBaselineOSCmd(MessagePtr);
                break;   
            
            /* Eeprom Commands */                                
            case CS_ENABLE_EEPROM_CC:
                CS_EnableEepromCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_EEPROM_CC:
                CS_DisableEepromCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_EEPROM_CC:
                CS_ReportBaselineEntryIDEepromCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_EEPROM_CC:
                CS_RecomputeBaselineEepromCmd(MessagePtr);
                break;                
                
            case CS_ENABLE_ENTRY_EEPROM_CC:
                CS_EnableEntryIDEepromCmd(MessagePtr);
                break;  
                
            case CS_DISABLE_ENTRY_EEPROM_CC:
                CS_DisableEntryIDEepromCmd(MessagePtr);
                break;                  
                
            case CS_GET_ENTRY_ID_EEPROM_CC:
                CS_GetEntryIDEepromCmd(MessagePtr);
                break;                  
 
                /* Memory Commands */
            case CS_ENABLE_MEMORY_CC:
                CS_EnableMemoryCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_MEMORY_CC:
                CS_DisableMemoryCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_MEMORY_CC:
                CS_ReportBaselineEntryIDMemoryCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_MEMORY_CC:
                CS_RecomputeBaselineMemoryCmd(MessagePtr);
                break;                
                
            case CS_ENABLE_ENTRY_MEMORY_CC:
                CS_EnableEntryIDMemoryCmd(MessagePtr);
                break;  
                
            case CS_DISABLE_ENTRY_MEMORY_CC:
                CS_DisableEntryIDMemoryCmd(MessagePtr);
                break;                  
                
            case CS_GET_ENTRY_ID_MEMORY_CC:
                CS_GetEntryIDMemoryCmd(MessagePtr);
                break;   
                
            /*Tables Commands */
            case CS_ENABLE_TABLES_CC:
                CS_EnableTablesCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_TABLES_CC:
                CS_DisableTablesCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_TABLE_CC:
                CS_ReportBaselineTablesCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_TABLE_CC:
                CS_RecomputeBaselineTablesCmd(MessagePtr);
                break;                
                
            case CS_ENABLE_NAME_TABLE_CC:
                CS_EnableNameTablesCmd(MessagePtr);
                break;  
                
            case CS_DISABLE_NAME_TABLE_CC:
                CS_DisableNameTablesCmd(MessagePtr);
                break;   

                /*App Commands */
            case CS_ENABLE_APPS_CC:
                CS_EnableAppCmd(MessagePtr);
                break;                
                
            case CS_DISABLE_APPS_CC:
                CS_DisableAppCmd(MessagePtr);
                break;                
                
            case CS_REPORT_BASELINE_APP_CC:
                CS_ReportBaselineAppCmd(MessagePtr);
                break;                
                
            case CS_RECOMPUTE_BASELINE_APP_CC:
                CS_RecomputeBaselineAppCmd(MessagePtr);
                break;                
                
            case CS_ENABLE_NAME_APP_CC:
                CS_EnableNameAppCmd(MessagePtr);
                break;  
                
            case CS_DISABLE_NAME_APP_CC:
                CS_DisableNameAppCmd(MessagePtr);
                break;   

            default:
                CFE_EVS_SendEvent (CS_CC1_ERR_EID,
                                   CFE_EVS_ERROR,
                                   "Invalid ground command code: ID = 0x%04X, CC = %d",
                                   MessageID, 
                                   CommandCode);
                
                CS_AppData.CmdErrCounter++;
                break;
        }/* end switch */
        break;
            
        default:
            CFE_EVS_SendEvent (CS_MID_ERR_EID, CFE_EVS_ERROR,
                               "Invalid command pipe message ID: 0x%04X",
                               MessageID);
            
            CS_AppData.CmdErrCounter++;
            break;
    }
    
    return (Result);
} /* End of CS_AppPipe () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Housekeeping command                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr)
{
    /* command verification variables */
    uint16              ExpectedLength = sizeof(CS_NoArgsCmd_t);
    CFE_SB_MsgId_t MessageID;
    uint16  CommandCode;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(MessagePtr);
    
    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CommandCode = CFE_SB_GetCmdCode(MessagePtr);
        MessageID= CFE_SB_GetMsgId(MessagePtr);
        
        CFE_EVS_SendEvent(CS_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);
    }    
    else
    {
        CS_AppData.HkPacket.CmdCounter          = CS_AppData.CmdCounter;
        CS_AppData.HkPacket.CmdErrCounter       = CS_AppData.CmdErrCounter;
        CS_AppData.HkPacket.ChecksumState       = CS_AppData.ChecksumState;
        CS_AppData.HkPacket.EepromCSState       = CS_AppData.EepromCSState;
        CS_AppData.HkPacket.MemoryCSState       = CS_AppData.MemoryCSState;
        CS_AppData.HkPacket.AppCSState          = CS_AppData.AppCSState;
        CS_AppData.HkPacket.TablesCSState       = CS_AppData.TablesCSState;
        CS_AppData.HkPacket.OSCSState           = CS_AppData.OSCSState;
        CS_AppData.HkPacket.CfeCoreCSState      = CS_AppData.CfeCoreCSState;
        CS_AppData.HkPacket.RecomputeInProgress = (uint8)CS_AppData.RecomputeInProgress;
        CS_AppData.HkPacket.OneShotInProgress   = (uint8)CS_AppData.OneShotInProgress;
        CS_AppData.HkPacket.EepromCSErrCounter  = CS_AppData.EepromCSErrCounter;
        CS_AppData.HkPacket.MemoryCSErrCounter  = CS_AppData.MemoryCSErrCounter;
        CS_AppData.HkPacket.AppCSErrCounter     = CS_AppData.AppCSErrCounter;
        CS_AppData.HkPacket.TablesCSErrCounter  = CS_AppData.TablesCSErrCounter;
        CS_AppData.HkPacket.CfeCoreCSErrCounter = CS_AppData.CfeCoreCSErrCounter;
        CS_AppData.HkPacket.OSCSErrCounter      = CS_AppData.OSCSErrCounter;
        CS_AppData.HkPacket.CurrentCSTable      = CS_AppData.CurrentCSTable;
        CS_AppData.HkPacket.CurrentEntryInTable = CS_AppData.CurrentEntryInTable;
        CS_AppData.HkPacket.EepromBaseline      = CS_AppData.EepromBaseline;
        CS_AppData.HkPacket.OSBaseline          = CS_AppData.OSBaseline;
        CS_AppData.HkPacket.CfeCoreBaseline     = CS_AppData.CfeCoreBaseline;
        CS_AppData.HkPacket.LastOneShotAddress  = CS_AppData.LastOneShotAddress;
        CS_AppData.HkPacket.LastOneShotSize     = CS_AppData.LastOneShotSize;
        CS_AppData.HkPacket.LastOneShotMaxBytesPerCycle = CS_AppData.LastOneShotMaxBytesPerCycle;
        CS_AppData.HkPacket.LastOneShotChecksum = CS_AppData.LastOneShotChecksum;
        CS_AppData.HkPacket.PassCounter         = CS_AppData.PassCounter;

        /* Send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg ( (CFE_SB_Msg_t *) & CS_AppData.HkPacket);
        CFE_SB_SendMsg      ( (CFE_SB_Msg_t *) & CS_AppData.HkPacket);
    }

    return;
} /* End of CS_HousekeepingCmd () */

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_CreateCDS() - create CS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CS_CreateRestoreStatesFromCDS(void)
{
    /* Store task ena/dis state of tables in CDS */
    uint8 DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32 Result;

    /*
    ** Request for CDS area from cFE Executive Services...
    */
    Result = CFE_ES_RegisterCDS(&CS_AppData.DataStoreHandle,
                                 sizeof(DataStoreBuffer), CS_CDS_NAME);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** New CDS area - write to Critical Data Store...
        */
        DataStoreBuffer[0] = CS_AppData.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.AppCSState;
        DataStoreBuffer[3] = CS_AppData.TablesCSState;
        
        DataStoreBuffer[4] = CS_AppData.OSCSState;
        DataStoreBuffer[5] = CS_AppData.CfeCoreCSState;

        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle,  DataStoreBuffer);
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /*
        ** Pre-existing CDS area - read from Critical Data Store...
        */
        Result = CFE_ES_RestoreFromCDS(DataStoreBuffer, CS_AppData.DataStoreHandle);

        if (Result == CFE_SUCCESS)
        {
            CS_AppData.EepromCSState = DataStoreBuffer[0];
            CS_AppData.MemoryCSState = DataStoreBuffer[1];
            CS_AppData.AppCSState    = DataStoreBuffer[2];
            CS_AppData.TablesCSState = DataStoreBuffer[3];
            
            CS_AppData.OSCSState     = DataStoreBuffer[4];
            CS_AppData.CfeCoreCSState = DataStoreBuffer[5];
        }
    }

    if (Result != CFE_SUCCESS)
    {
        /*
        ** CDS is broken - prevent further errors...
        */
        CS_AppData.DataStoreHandle = 0;

        /* Use states from platform configuration */
        CS_AppData.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
        CS_AppData.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
        CS_AppData.AppCSState    = CS_APPS_TBL_POWERON_STATE;
        CS_AppData.TablesCSState = CS_TABLES_TBL_POWERON_STATE;
        
        CS_AppData.OSCSState      = CS_OSCS_CHECKSUM_STATE;
        CS_AppData.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;
        
        CFE_EVS_SendEvent(CS_INIT_CDS_ERR_EID, CFE_EVS_ERROR,
                         "Critical Data Store access error = 0x%08X", (unsigned int)Result);
        /*
        ** CDS errors are not fatal - CS can still run...
        */
        Result = CFE_SUCCESS;
    }

    return(Result);

} /* End of CS_CreateCDS() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_UpdateCDS() - update DS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CS_UpdateCDS(void)
{
    /* Store table ena/dis state in CDS */
    uint8 DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32 Result;

    /*
    ** Handle is non-zero when CDS is active...
    */
    if (CS_AppData.DataStoreHandle != 0)
    {
        /*
        ** Copy ena/dis states of tables to the data array...
        */
        DataStoreBuffer[0] = CS_AppData.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.AppCSState;
        DataStoreBuffer[3] = CS_AppData.TablesCSState;
        
        DataStoreBuffer[4] = CS_AppData.OSCSState;
        DataStoreBuffer[5] = CS_AppData.CfeCoreCSState;

        /*
        ** Update CS portion of Critical Data Store...
        */
        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_INIT_CDS_ERR_EID, CFE_EVS_ERROR,
                             "Critical Data Store access error = 0x%08X", (unsigned int)Result);
            /*
            ** CDS is broken - prevent further errors...
            */
            CS_AppData.DataStoreHandle = 0;
        }
    }

    return;

} /* End of CS_UpdateCDS() */
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == TRUE) */

/************************/
/*  End of File Comment */
/************************/
