/******************************************************************************/
/** \file  ci_custom.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for Custom Layer of CI Application for UDP
*
*   \par
*     This file defines the functions for a custom implementation of the custom
*     layer of the CI application over a UDP socket. 
*
*   \par API Functions Defined:
*     - CI_CustomInit() - Initialize the transport protocol, create child task
*     - CI_CustomAppCmds() - Process custom App Commands
*     - CI_CustomEnableTO() - Send msg to TO to enable downlink
*     - CI_CustomCleanup() - Cleanup callback to close transport channel.
*     - CI_CustomMain() - Main entry point for the custom child task. 
*     - CI_CustomGateCmds() - Process custom Gate Commands
*
*   \par Private Functions Defined:
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - All input messages are CCSDS messages
*     - CI and TO makes use of seperate UDP sockets
*     - All config macros defined in ci_platform_cfg.h
*     - ciMutex must be used whenever g_CI_AppData is accessed.
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*     - 2015-06-02 | Guy de Carufel | Revised for new UDP API
*******************************************************************************/

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "network_includes.h"
#include "trans_udp.h"

#include "ci_app.h"
#include "ci_platform_cfg.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/
typedef struct
{
    IO_TransUdp_t           udp;                /**< UDP working              */
    TO_EnableOutputCmd_t    toEnableCmd;        /**< TO Enable CMD msg        */
    uint8                   buffer[CI_CUSTOM_BUFFER_SIZE];  /**< buffer       */
} CI_CustomData_t;


/*
** External Global Variables
*/
/* NOTE: Make use of ciMutex when accessing data shared by the main task. */
extern CI_AppData_t g_CI_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/
static CI_CustomData_t g_CI_CustomData;

/*
** Local Function Definitions
*/


/*******************************************************************************
** Custom Application Functions (Executed by Main Task)
*******************************************************************************/

/******************************************************************************/
/** \brief Custom Initialization
*******************************************************************************/
int32 CI_CustomInit(void)
{
    int32 iStatus = CI_ERROR;
    uint32 taskId = 0;
    IO_TransUdpConfig_t config;

    /* Set Config parameters */
    CFE_PSP_MemSet((void *) &config, 0x0, sizeof(IO_TransUdpConfig_t));
    strncpy(config.cAddr, CI_CUSTOM_UDP_ADDR, 16);
    config.usPort = CI_CUSTOM_UDP_PORT;
    config.timeoutRcv = CI_CUSTOM_UDP_TIMEOUT;
    
    if (IO_TransUdpInit(&config, &g_CI_CustomData.udp) < 0)
    {
        goto end_of_function;
    }

    iStatus = CFE_ES_CreateChildTask(&taskId,
                                     "CI Custom Main Task",
                                     CI_CustomMain,
                                     CI_CUSTOM_TASK_STACK_PTR, 
                                     CI_CUSTOM_TASK_STACK_SIZE, 
                                     CI_CUSTOM_TASK_PRIO,
                                     0);
end_of_function:    
    return (iStatus);
}
    

/******************************************************************************/
/** \brief Custom app command response
*******************************************************************************/
int32 CI_CustomAppCmds(CFE_SB_MsgPtr_t cmdMsgPtr)
{
    int32 iStatus = CI_SUCCESS;
    uint32 uiCmdCode = CFE_SB_GetCmdCode(cmdMsgPtr);
    switch (uiCmdCode)
    {
        /*  Example of a valid custom command. Declare at top of file. 
        case CI_CUSTOM_EXAMPLE_CC:
            if (CI_VerifyCmdLength(cmdMsgPtr, sizeof(CI_CustomExampleCmd_t)))
            {
                CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
                CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "CI: Recvd example custom app cmd (%d)", uiCmdCode);
            }
            break;
        */

        default:
            iStatus = CI_ERROR;
            break;
    }
    
    return iStatus;
}


/******************************************************************************/
/** \brief Custom response to CI_ENABLE_TO_CC cmd code
*******************************************************************************/
void CI_CustomEnableTO(CFE_SB_MsgPtr_t cmdMsgPtr)
{
    /* NOTE: In this case, we are simply piping the cmd to TO. */
    CFE_PSP_MemCpy((void *) &g_CI_CustomData.toEnableCmd, 
                   (void *) cmdMsgPtr, sizeof(TO_EnableOutputCmd_t));

    /* Setup the toEnableCmd */
    CFE_SB_InitMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd, 
                   TO_APP_CMD_MID, sizeof(TO_EnableOutputCmd_t), FALSE); 
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd, 
                      TO_ENABLE_OUTPUT_CC);
    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd);

    /* Send the TO Enable Telemetry Output Message */    
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd);
    return;
}


/******************************************************************************/
/** \brief Custom Cleanup
*******************************************************************************/
void CI_CustomCleanup(void)
{
    IO_TransUdpCloseSocket(&g_CI_CustomData.udp);
}


/*******************************************************************************
** Custom Functions (Executed by Custom child task)
*******************************************************************************/

/******************************************************************************/
/** \brief Entry Point of custom child task
*******************************************************************************/
void CI_CustomMain(void)
{
    int32 size = 0;
    CFE_SB_MsgPtr_t sbMsg;
    CFE_SB_MsgId_t  msgId;

    if (g_CI_CustomData.udp.sockId < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Socket ID not set. Check init. "
                          "Quitting CI_CustomMain.");
        return;
    }

    sbMsg = (CFE_SB_MsgPtr_t) &g_CI_CustomData.buffer[0];

    while(size >= 0)
    {
        size = IO_TransUdpRcvTimeout(&g_CI_CustomData.udp,
                                     &g_CI_CustomData.buffer[0], 
                                     CI_CUSTOM_BUFFER_SIZE,
                                     IO_TRANS_PEND_FOREVER);

        if (size > 0)
        {
            /* Get Msg ID */
            msgId = CFE_SB_GetMsgId(sbMsg);

            /* NOTE: For this simple UDP example, the Checksum validation is 
               not included as to be able to test with cmdUtils tool. */

            /* CCSDS command checksum check. */
            /*
            if (CFE_SB_ValidateChecksum(sbMsg) == FALSE)
            {
                uint16 cmdCode = CFE_SB_GetCmdCode(sbMsg);
                CFE_EVS_SendEvent(CI_CMD_ERR_EID, CFE_EVS_ERROR,
                                  "CI: MID:0x%04x - Cmd Checksum failed. CmdCode:%u",
                                  msgId, cmdCode);
                continue;
            }
            */

            /* If command is GATE command, execute immediately. */
            if (msgId == CI_GATE_CMD_MID)
            {
                CI_CustomGateCmds(sbMsg);
            }
            /* Any other message is passed through to the SB. */
            else if (size > 0)
            {
                CFE_SB_SendMsg(sbMsg);
            }
        }
    }

    if (size < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                          "CI: Error occured on socket read. "
                          "Quitting CI_CustomMain.");
    }

    return;
}
   

/******************************************************************************/
/** \brief Custom Gate command response
*******************************************************************************/
void CI_CustomGateCmds(CFE_SB_MsgPtr_t cmdMsgPtr)
{
    uint32 uiCmdCode = 0;

    uiCmdCode = CFE_SB_GetCmdCode(cmdMsgPtr);
    switch (uiCmdCode)
    {
        /*  Example of a valid custom command.
        case CI_EXAMPLE_GATE_CC:
            if (CI_VerifyCmdLength(cmdMsgPtr, sizeof(CI_CustomExampleCmd_t)))
            {
                CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
                CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "CI: Recvd example custom gate cmd (%d)", uiCmdCode);
            }
            break;
        */

        default:
            CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdErrCnt);
            CFE_EVS_SendEvent(CI_CMD_ERR_EID, CFE_EVS_ERROR,
                              "CI: Recvd invalid Gate cmd (%d)", uiCmdCode);
            break;
    }
    
    return;
}

/*==============================================================================
** End of file ci_custom.c
**============================================================================*/
