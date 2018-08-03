/******************************************************************************/
/** \file  ci_custom.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for Custom Layer of CI Application for RS422
*
*   \par
*     This file defines the functions for a custom implementation of the custom
*     layer of the CI application over an RS422 serial port. 
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
*     - CI_CustomProcessUpMsg() - Process new uplink message
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - All input messages are CCSDS messages
*     - Both CI and TO makes use of the same RS422 device
*     - All config macros defined in ci_platform_cfg.h
*     - ciMutex must be used whenever g_CI_AppData is accessed.
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
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cfe.h"
#include "network_includes.h"
#include "trans_rs422.h"

#include "ci_app.h"
#include "ci_platform_cfg.h"
#include "to_mission_cfg.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/
typedef struct
{
    int32                   iFileDesc;          /**< File Descriptor of port  */
    TO_EnableOutputCmd_t    toEnableCmd;        /**< TO Enable CMD msg        */
    uint8                   buffer[CI_CUSTOM_BUFFER_SIZE];  /**< buffer       */
} CI_CustomData_t;

/*
** External Global Variables
*/
extern CI_AppData_t g_CI_AppData;

/*
** Global Variables
*/
CI_CustomData_t g_CI_CustomData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/
void CI_CustomProcessUpMsg(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId);


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
    IO_TransRS422Config_t config;

    /*Initialize a RS422 Port */
    strncpy((char *) &config.device, CI_CONFIG_SERIAL_PORT, 
            PORT_NAME_SIZE);
    config.baudRate = CI_CONFIG_BAUD_RATE;
    config.timeout  = CI_CONFIG_TIMEOUT;
    config.minBytes = CI_CONFIG_MINBYTES;
    
    g_CI_CustomData.iFileDesc = IO_TransRS422Init(&config);

    if (g_CI_CustomData.iFileDesc < 0)
    {
        goto end_of_function;
    }
  
    /* Setup the toEnableCmd */
    CFE_SB_InitMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd,
                   TO_APP_CMD_MID, sizeof(TO_EnableOutputCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd, 
                      TO_ENABLE_OUTPUT_CC);
    g_CI_CustomData.toEnableCmd.iFileDesc = g_CI_CustomData.iFileDesc;
    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd);

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
int32 CI_CustomAppCmds(CFE_SB_MsgPtr_t pCmdMsg)
{
    int32 iStatus = CI_SUCCESS;
    uint32 uiCmdCode = CFE_SB_GetCmdCode(pCmdMsg);
    switch (uiCmdCode)
    {
        /*  Example of a valid custom command. Declare at top of file. 
        case CI_CUSTOM_EXAMPLE_CC:
            if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_CustomExampleCmd_t)))
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
void CI_CustomEnableTO(CFE_SB_MsgPtr_t pCmdMsg)
{
    /* Send the TO Enable Telemetry Output Message */    
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd);
    
    return;
}


/******************************************************************************/
/** \brief Custom Cleanup
*******************************************************************************/
void CI_CustomCleanup(void)
{
    IO_TransRS422Close(g_CI_CustomData.iFileDesc);
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
    int32 msgSize = 0;
    int32 dataSize = 0;
    CFE_SB_MsgId_t  msgId;
    CFE_SB_MsgPtr_t pSbMsg = (CFE_SB_MsgPtr_t) &g_CI_CustomData.buffer[0];

    if (g_CI_CustomData.iFileDesc == -1)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Serial Port not set. Check init. "
                          "Quitting CI_CustomMain.");
        return;
    }

    while(size >= 0)
    {
         /* Pend forever, waiting for header of message. */
         size = IO_TransRS422ReadTimeout(g_CI_CustomData.iFileDesc, 
                                         &g_CI_CustomData.buffer[0], 6, 
                                         IO_TRANS_PEND_FOREVER);

         /* Received CCSDS message. */
         if (size == 6)
         {
            /* Get Msg ID */
            msgId = CFE_SB_GetMsgId(pSbMsg);
            
            /* Get message size */
            msgSize = CFE_SB_GetTotalMsgLength(pSbMsg);
            dataSize = msgSize - 6;

            if (msgSize > CI_CUSTOM_BUFFER_SIZE)
            {
                CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                                  "CI: Message received larger than buffer. "
                                  "Message ID:0x%x dropped.", msgId);
            }
            else if (dataSize >= 0)
            {
                /* Read full message. May timeout based on init config. */
                size = IO_TransRS422Read(g_CI_CustomData.iFileDesc, 
                                         &g_CI_CustomData.buffer[6], dataSize);

                if (size != dataSize)
                {
                    CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                                      "CI: Incomplete message received. "
                                      "Message ID:0x%x dropped.", msgId);
                }
                else
                {
                    CI_CustomProcessUpMsg(pSbMsg, msgId);
                }
            }
            else
            {
                CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                                  "CI: Error on serial port read. errno:%d",
                                  errno);
            }

         }
         else
         {
             /* Deal with messages smaller than CCSDS packets here. */
         }
    }
}

/******************************************************************************/
/** \brief Custom Process Uplink Msg (Private)
*******************************************************************************/
void CI_CustomProcessUpMsg(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId)
{
     /* CCSDS command checksum check. */
     if (CFE_SB_ValidateChecksum(pSbMsg) == FALSE)
     {
         uint16 cmdCode = CFE_SB_GetCmdCode(pSbMsg);
         CFE_EVS_SendEvent(CI_CMD_ERR_EID, CFE_EVS_ERROR,
                           "CI: MID:0x%04x - Cmd Checksum failed. CmdCode:%u",
                           msgId, cmdCode);
         return;
     }

     /* If command is GATE command, execute immediately. */
     if (msgId == CI_GATE_CMD_MID)
     {
         CI_CustomGateCmds(pSbMsg);
     }
     /* Any other message is passed through to the SB. */
     else 
     {
         CFE_SB_SendMsg(pSbMsg);
     }

    return;
}
   
/******************************************************************************/
/** \brief Custom Gate command response
*******************************************************************************/
void CI_CustomGateCmds(CFE_SB_MsgPtr_t pCmdMsg)
{
    uint32 uiCmdCode = 0;

    uiCmdCode = CFE_SB_GetCmdCode(pCmdMsg);
    switch (uiCmdCode)
    {
        /*  Example of a valid custom command.
        case CI_EXAMPLE_GATE_CC:
            if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_CustomExampleCmd_t)))
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
