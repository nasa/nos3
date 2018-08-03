/******************************************************************************/
/** \file  ci_app.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for CI Application
*
*   \par
*     This source code file contains the application layer functions for CI.
*
*   \par
*     This file defines the functions associtated with the application layer of
*     Command Ingest (CI) application. The application layer is responsible to
*     interact with the CFE Software bus, send the HK packet, and the OutData
*     packet. The custom functions are defined in the ci_custom.c file.
*
*   \par API Functions Defined:
*     - CI_AppMain() - Main entry point. Initializes, then calls CI_RcvMsg. 
*     - CI_AppInit() - Initializes the CI Application
*     - CI_InitEvent() - Initializes the events
*     - CI_InitPipe() - Initializes the pipes (Scheduler, command)
*     - CI_InitData() - Initializes HK and OutData packets.
*     - CI_RcvMsg() - Pends on SB to perform main funtions.
*     - CI_ProcessNewCmds() - Call appropriate fnct based on CMD MID.
*     - CI_ProcessNewAppCmds() - Call appropriate fnct based on CMD Code.
*     - CI_ReportHousekeeping() - Send to SB the HK packet.
*     - CI_SendOutData() - Send the OutData packet to the SB.
*
*   \par Private Functions Defined:
*     - None
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - All Custom functions are to be defined in ci_custom.c
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>
#include "cfe.h"
#include "ci_app.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
CI_AppData_t  g_CI_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/


/*****************************************************************************/
/** \brief Main Entry Point for CI Application  
******************************************************************************/
void CI_AppMain(void)
{
    int32  iStatus=CFE_SUCCESS;
    
    /* Register the Application with Executive Services */
    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CI - Failed to register the app (0x%08X)\n", 
                             iStatus);
        goto CI_AppMain_Exit_Tag;
    }

    /* Performance Log Entry stamp - #1 */
    CFE_ES_PerfLogEntry(CI_MAIN_TASK_PERF_ID);
    
    /* Perform Application initializations */
    if (CI_AppInit() != CFE_SUCCESS)
    {
        g_CI_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    }

    /* Application Main Loop. Call CFE_ES_RunLoop() to check for changes in the
    ** Application's status. If there is a request to kill this Application, 
    ** it will be passed in through the RunLoop call.  */
    while (CFE_ES_RunLoop(&g_CI_AppData.uiRunStatus) == TRUE)
    {
        /* Performance Log Exit stamp - #1 */
        CFE_ES_PerfLogExit(CI_MAIN_TASK_PERF_ID);
        
        iStatus = CI_RcvMsg(g_CI_AppData.uiWakeupTimeout); 
    }

    /* Performance Log Exit stamp - #2 */
    CFE_ES_PerfLogExit(CI_MAIN_TASK_PERF_ID);
    
CI_AppMain_Exit_Tag:
    /* Exit the application. Will call CI_CleanupCallback */
    CFE_ES_ExitApp(g_CI_AppData.uiRunStatus);
} 
    

/*****************************************************************************/
/** \brief Initialize The Application
******************************************************************************/
int32 CI_AppInit(void)
{
    int32  iStatus=CFE_SUCCESS;

    g_CI_AppData.uiRunStatus = CFE_ES_APP_RUN;

    /* Initialize Events */
    iStatus = CI_InitEvent();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CI - Event Init failed. \n");
        goto CI_AppInit_Exit_Tag;
    }

    /* Initialize Pipes */
    iStatus = CI_InitPipe();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CI_INIT_ERR_EID, CFE_EVS_ERROR,
                         "CI - Pipe Init failed.");
        goto CI_AppInit_Exit_Tag;
    }

    /* Initialize Data (never fails) */
    iStatus = CI_InitData();

    /* Custom Init */
    iStatus = CI_CustomInit();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CI_INIT_ERR_EID, CFE_EVS_ERROR,
                         "CI - Custom Init failed.");
        goto CI_AppInit_Exit_Tag;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&CI_CleanupCallback);

CI_AppInit_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CI_INIT_INF_EID, CFE_EVS_INFORMATION,
                          "Application initialized");
    }
    else
    {
        iStatus = CI_ERROR;
        CFE_ES_WriteToSysLog("CI - Application failed to initialize\n");
    }

    return (iStatus);
}
    
/*****************************************************************************/
/** \brief Initialize the Event Filter Table.
******************************************************************************/
int32 CI_InitEvent(void)
{
    int32  iStatus=CFE_SUCCESS;
    int32  ii = 0;

    /* Create the event table */
    CFE_PSP_MemSet((void*)g_CI_AppData.EventTbl, 0x00, 
                   sizeof(g_CI_AppData.EventTbl));
    
    for (ii = 0; ii < CI_EVT_CNT; ++ii)
    {
        g_CI_AppData.EventTbl[ii].EventID = ii;
    }

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_CI_AppData.EventTbl,
                               CI_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CI - Failed to register with EVS (0x%08X)\n", 
                             iStatus);
    }

    return (iStatus);
}
    
/*****************************************************************************/
/** \brief Initialize the Pipes
******************************************************************************/
int32 CI_InitPipe(void)
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe */
    g_CI_AppData.usSchPipeDepth = CI_SCH_PIPE_DEPTH;
    CFE_PSP_MemSet((void*)g_CI_AppData.cSchPipeName, '\0', 
                   sizeof(g_CI_AppData.cSchPipeName));
    strncpy(g_CI_AppData.cSchPipeName, "CI_SCH_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    iStatus = CFE_SB_CreatePipe(&g_CI_AppData.SchPipeId,
                                 g_CI_AppData.usSchPipeDepth,
                                 g_CI_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        CFE_SB_Subscribe(CI_WAKEUP_MID, g_CI_AppData.SchPipeId);
    }
    else
    {
        CFE_EVS_SendEvent(CI_INIT_ERR_EID, CFE_EVS_ERROR,
                         "CI - Failed to create SCH pipe (0x%08X)", 
                         iStatus);
        goto CI_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_CI_AppData.usCmdPipeDepth = CI_CMD_PIPE_DEPTH ;
    CFE_PSP_MemSet((void*)g_CI_AppData.cCmdPipeName, '\0', 
                   sizeof(g_CI_AppData.cCmdPipeName));
    strncpy(g_CI_AppData.cCmdPipeName, "CI_CMD_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&g_CI_AppData.CmdPipeId,
                                 g_CI_AppData.usCmdPipeDepth,
                                 g_CI_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        CFE_SB_Subscribe(CI_APP_CMD_MID, g_CI_AppData.CmdPipeId);
        CFE_SB_Subscribe(CI_SEND_HK_MID, g_CI_AppData.CmdPipeId);
    }
    else
    {
        CFE_EVS_SendEvent(CI_INIT_ERR_EID, CFE_EVS_ERROR,
                         "CI - Failed to create CMD pipe (0x%08X)", 
                         iStatus);
        goto CI_InitPipe_Exit_Tag;
    }

CI_InitPipe_Exit_Tag:
    return (iStatus);
}
    
/*****************************************************************************/
/** \brief Initialize Data
******************************************************************************/
int32 CI_InitData(void)
{
    int32  iStatus= CI_SUCCESS;

    /* Init CI mutex */
    OS_MutSemCreate(&g_CI_AppData.ciMutex, "CI Mutex", 0);
    OS_MutSemTake(g_CI_AppData.ciMutex);

    /* Init output data */
    CFE_PSP_MemSet((void*)&g_CI_AppData.OutData, 0x00, 
                   sizeof(g_CI_AppData.OutData));
    CFE_SB_InitMsg(&g_CI_AppData.OutData,
                   CI_OUT_DATA_MID, sizeof(g_CI_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    CFE_PSP_MemSet((void*)&g_CI_AppData.HkTlm, 0x00, 
                   sizeof(g_CI_AppData.HkTlm));
    CFE_SB_InitMsg(&g_CI_AppData.HkTlm,
                   CI_HK_TLM_MID, sizeof(g_CI_AppData.HkTlm), TRUE);

    
    /* Init wakeup timeout */
    /* NOTE: Saving timeout locally allows for potential customization.
     * No default command is provided to change it at run-time as a safety
     * measure. Custom implementation may choose to add such command in 
     * CI_CustomAppCmds or CI_CustomGateCmds. */
    g_CI_AppData.uiWakeupTimeout = CI_WAKEUP_TIMEOUT;
    
    OS_MutSemGive(g_CI_AppData.ciMutex);

    return (iStatus);
}
    

/*****************************************************************************/
/** \brief Receive Messages from Software Bus
******************************************************************************/
int32 CI_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_MsgPtr_t MsgPtr = NULL;
    CFE_SB_MsgId_t  MsgId;
             
    /* Wait for WAKEUP messages from scheduler or use timeout rate */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_CI_AppData.SchPipeId, iBlocking);
        
    /* Performance Log Entry stamp */
    CFE_ES_PerfLogEntry(CI_MAIN_TASK_PERF_ID); 
        
    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
        {
            case CI_WAKEUP_MID:
                CI_ProcessNewCmds();
                CI_SendOutData();
                break;
            
            default:
                CFE_EVS_SendEvent(CI_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "CI - Recvd invalid SCH msgId (0x%04x)", 
                                  MsgId);
        }
    }
    /* Implementation may set usWakeupTimeout instead of relying on
     * Scheduler for wakeup message. */
    else if (iStatus == CFE_SB_TIME_OUT)
    {
        CI_ProcessNewCmds();
        CI_SendOutData();
    }
    else
    {
        CFE_EVS_SendEvent(CI_PIPE_ERR_EID, CFE_EVS_ERROR,
                         "CI: SB pipe read error (0x%08x), app will exit", 
                         iStatus);
        g_CI_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }
    
    return (iStatus);
}


/*****************************************************************************/
/** \brief Process New Commands
******************************************************************************/
void CI_ProcessNewCmds(void)
{
    CFE_SB_MsgPtr_t CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;
    boolean         bGotNewMsg=TRUE;

    while (bGotNewMsg)
    {
        if (CFE_SB_RcvMsg(&CmdMsgPtr, g_CI_AppData.CmdPipeId, CFE_SB_POLL) == 
            CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case CI_APP_CMD_MID:
                    CI_ProcessNewAppCmds(CmdMsgPtr);
                    break;
            
                case CI_SEND_HK_MID:
                    CI_ReportHousekeeping();
                    break;

                default:
                    CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdErrCnt);
                    CFE_EVS_SendEvent(CI_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "CI - Recvd invalid CMD msgId (0x%04x)", 
                                      CmdMsgId);
                    break;
            }
        }
        else
        {
            bGotNewMsg = FALSE;
        }
    }
}
    

/*****************************************************************************/
/** \brief Process New Application Commands
******************************************************************************/
void CI_ProcessNewAppCmds(CFE_SB_MsgPtr_t pCmdMsg)
{
    int32 iStatus = CI_SUCCESS;
    uint32  uiCmdCode=0;

    if (pCmdMsg != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(pCmdMsg);
        switch (uiCmdCode)
        {
            case CI_NOOP_CC:
                if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_NoArgCmd_t)))
                {
                    CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
                    CFE_EVS_SendEvent(CI_CMD_INF_EID,
                                      CFE_EVS_INFORMATION,
                                      "No-op command. Version %d.%d.%d.%d",
                                      CI_MAJOR_VERSION,
                                      CI_MINOR_VERSION,
                                      CI_REVISION,
                                      CI_MISSION_REV);
                }
                break;

            case CI_RESET_CC:
                if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_NoArgCmd_t)))
                {
                    OS_MutSemTake(g_CI_AppData.ciMutex);
                    g_CI_AppData.HkTlm.usCmdCnt = 0;
                    g_CI_AppData.HkTlm.usCmdErrCnt = 0;
                    OS_MutSemGive(g_CI_AppData.ciMutex);
                    CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_INFORMATION,
                                      "Recvd RESET cmd (%d)", uiCmdCode);
                }
                break;
                
            case CI_ENABLE_TO_CC:
                if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_EnableTOCmd_t)))
                {
                    CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
                    CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_INFORMATION,
                                      "Sending Enable TO Cmd (%d)",
                                      CI_ENABLE_TO_CC);

                    CI_CustomEnableTO(pCmdMsg);
                }
                break;

            /* Any other commands are assumed to be custom commands. */
            default:
                iStatus = CI_CustomAppCmds(pCmdMsg);
               
                if (iStatus != CI_SUCCESS) 
                {
                    CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdErrCnt);
                    CFE_EVS_SendEvent(CI_CMD_ERR_EID, CFE_EVS_ERROR,
                                      "Recvd invalid app cmd code (%d)", 
                                      uiCmdCode);
                }
                break;
        }
    }
}


/*****************************************************************************/
/** \brief Report Housekeeping Packet
******************************************************************************/
void CI_ReportHousekeeping(void)
{
    OS_MutSemTake(g_CI_AppData.ciMutex);
    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t)&g_CI_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&g_CI_AppData.HkTlm);
    OS_MutSemGive(g_CI_AppData.ciMutex);
}
    
/*****************************************************************************/
/** \brief Send the OutData Packet
******************************************************************************/
void CI_SendOutData(void)
{
    OS_MutSemTake(g_CI_AppData.ciMutex);
    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t)&g_CI_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&g_CI_AppData.OutData);
    OS_MutSemGive(g_CI_AppData.ciMutex);
}


/*****************************************************************************/
/** \brief Perform cleanup on shutdown
******************************************************************************/
void CI_CleanupCallback(void)
{
    CI_CustomCleanup();
}
    
/*==============================================================================
** End of file ci_app.c
**============================================================================*/
