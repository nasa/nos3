/******************************************************************************/
/** \file  ci_custom.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for Custom Layer of CI with multi channels.
*
*   \par
*     This file defines the functions for a custom implementation of the custom
*     layer of the CI application over UDP and RS422 serial port. 
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
*     - 2015-06-01 | Guy de Carufel | Code Started
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
#include "trans_select.h"
#include "trans_rs422.h"
#include "trans_udp.h"

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
    IO_TransSelect_t        select;             /**< Select struct            */
    int32                   serialFd;           /**< File Descriptor of port  */
    IO_TransUdp_t           socket;             /**< Socket structure         */
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

/*
** Local Variables
*/
static CI_CustomData_t g_CI_CustomData;

/*
** Local Function Definitions
*/
static int32 CI_CustomReadSerial(void);
static int32 CI_CustomReadSocket(void);
static void CI_CustomProcessUpMsg(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId);


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
    IO_TransRS422Config_t configSerial;
    IO_TransUdpConfig_t configSocket;

    /* Init as errors */
    g_CI_CustomData.serialFd = -1;
    g_CI_CustomData.socket.sockId = -1;

    /* Initialize select */ 
    if (IO_TransSelectClear(&g_CI_CustomData.select) < 0)
    {
        goto end_of_function;
    }

    /*Initialize a RS422 Port  */
    strncpy((char *) &configSerial.device, CI_CONFIG_SERIAL_PORT, 
            PORT_NAME_SIZE);
    configSerial.baudRate = CI_CONFIG_BAUD_RATE;
    configSerial.timeout  = CI_CONFIG_TIMEOUT;
    configSerial.minBytes = CI_CONFIG_MINBYTES;
    configSerial.cFlags   = 0;
    
    g_CI_CustomData.serialFd = IO_TransRS422Init(&configSerial);
    if (g_CI_CustomData.serialFd < 0)
    {
        goto end_of_function;
    }

    /* Add to select set  */
    if (IO_TransSelectAddFd(&g_CI_CustomData.select,
                            g_CI_CustomData.serialFd) < 0)
    {
        goto end_of_function;
    }

    /* Initialize Socket */
    CFE_PSP_MemSet((void *) &configSocket, 0x0, sizeof(IO_TransUdpConfig_t));
    strncpy(configSocket.cAddr, CI_CUSTOM_UDP_ADDR, 16);
    configSocket.usPort = CI_CUSTOM_UDP_PORT;
    configSocket.timeoutRcv = CI_CUSTOM_UDP_TIMEOUT;
    
    if (IO_TransUdpInit(&configSocket, &g_CI_CustomData.socket) < 0)
    {
        goto end_of_function;
    }

    /* Add to select set */
    if (IO_TransSelectAddFd(&g_CI_CustomData.select,
                            g_CI_CustomData.socket.sockId) < 0)
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
    /* Copy the first part of the command (for socket setup) */
    CFE_PSP_MemCpy((void *) &g_CI_CustomData.toEnableCmd, 
                   (void *) pCmdMsg, sizeof(CI_EnableTOCmd_t));

    /* Setup the toEnableCmd */
    CFE_SB_InitMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd, 
                   TO_APP_CMD_MID, sizeof(TO_EnableOutputCmd_t), FALSE); 
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &g_CI_CustomData.toEnableCmd, 
                      TO_ENABLE_OUTPUT_CC);
    g_CI_CustomData.toEnableCmd.iFileDesc = g_CI_CustomData.serialFd;
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
    IO_TransRS422Close(g_CI_CustomData.serialFd);
    IO_TransUdpCloseSocket(&g_CI_CustomData.socket);
    IO_TransSelectClear(&g_CI_CustomData.select);
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

    if (g_CI_CustomData.serialFd < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Serial Port not set. Check init. "
                          "Quitting CI_CustomMain.");
        goto end_of_function;
    }

    if (g_CI_CustomData.socket.sockId < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Socket ID not set. Check init. "
                          "Quitting CI_CustomMain.");
        goto end_of_function;
    }

    while(size >= 0)
    {
        size = IO_TransSelectInput(&g_CI_CustomData.select, 
                                   IO_TRANS_PEND_FOREVER);

        if (size > 0)
        {
            if (IO_TransSelectFdInActive(&g_CI_CustomData.select,
                                         g_CI_CustomData.serialFd))
            {
                size = CI_CustomReadSerial();
            }
            else if (IO_TransSelectFdInActive(&g_CI_CustomData.select,
                                              g_CI_CustomData.socket.sockId))
            {
                size = CI_CustomReadSocket();
            }
            else 
            {
                CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                                  "CI: Unexpected Active Device. "
                                  "Quitting CI_CustomMain.");
                break;
            }
        }
    }

end_of_function:
    return;
}


/******************************************************************************/
/** \brief Read message on Serial Port (Private)
*******************************************************************************/
int32 CI_CustomReadSerial(void)
{
    int32 msgSize = 0;
    int32 dataSize = 0;
    CFE_SB_MsgId_t  msgId;
    CFE_SB_MsgPtr_t pSbMsg = (CFE_SB_MsgPtr_t) &g_CI_CustomData.buffer[0];
    
    /* Get header of message. */
    int32 size = IO_TransRS422Read(g_CI_CustomData.serialFd, 
                                   &g_CI_CustomData.buffer[0], 6); 

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
           size = IO_TransRS422Read(g_CI_CustomData.serialFd, 
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
    
    return size;
}

/******************************************************************************/
/** \brief Read message on Serial Port (Private)
*******************************************************************************/
int32 CI_CustomReadSocket(void)
{
    CFE_SB_MsgId_t  msgId;
    CFE_SB_MsgPtr_t pSbMsg = (CFE_SB_MsgPtr_t) &g_CI_CustomData.buffer[0];
    
    /* Get header of message. */
    int32 size = IO_TransUdpRcv(&g_CI_CustomData.socket, 
                                &g_CI_CustomData.buffer[0], 
                                CI_CUSTOM_BUFFER_SIZE); 
    if (size > 0)
    {
        msgId = CFE_SB_GetMsgId(pSbMsg);
        CI_CustomProcessUpMsg(pSbMsg, msgId);
    }

    return size;
}
   

/******************************************************************************/
/** \brief Custom Process Uplink Msg (Private)
*******************************************************************************/
void CI_CustomProcessUpMsg(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId)
{
     /* NOTE: Comment this out if you would like to test with cmdUtils tool,
        As it does not include a checksum in it's commands sent. */
     
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
