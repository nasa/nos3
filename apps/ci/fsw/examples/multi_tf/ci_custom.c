/******************************************************************************/
/** \file  ci_custom.c
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for Custom Layer of CI Application for RS422
*       with Telecommand Transfer Frames.
*
*   \par
*     This file defines the functions for a custom implementation of the custom
*     layer of the CI application over an RS422 serial port accepting TCTF. 
*
*   \par API Functions Defined:
*     - CI_CustomInit() - Initialize the transport protocol, create child task
*     - CI_CustomAppCmds() - Process custom App Commands
*     - CI_CustomEnableTO() - Send msg to TO to enable downlink
*     - CI_CustomCleanup() - Cleanup callback to close transport channel.
*     - CI_CustomMain() - Main entry point for the custom child task. 
*     - CI_CustomGateCmds() - Process custom Gate Commands
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - All input messages are Communication Link Data Units (CLTUs)
*     - Both CI and TO makes use of the same RS422 device
*     - All config macros defined in ci_platform_cfg.h
*     - ciMutex must be used whenever g_CI_AppData is accessed.
*
*   \par Modification History:
*     - 2015-08-01 | Guy de Carufel | Code Started
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

#include "tc_sync.h"
#include "cop1.h"
#include "trans_select.h"
#include "trans_udp.h"
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
    TCTF_ChannelService_t   chnlService;
    uint8                   pktBuff[CI_CUSTOM_BUFFER_SIZE];  
    TO_CustomSetOcfCmd_t    clcwCmd;    /**< CLCW Update Command      */
} CI_CustomVChnl_t;

typedef struct
{
    CI_CustomVChnl_t        vChnls[CI_CUSTOM_TF_CHANNELS];
    uint8                   tfBuff[CI_CUSTOM_TF_BUFF_SIZE];
} CI_CustomMChnl_t;

typedef struct
{
    CI_CustomMChnl_t     mc;        /**< Master channel             */
    boolean              cltuRand;  /**< Is the cltu code blocks 
                                          randomized                */
    uint8                cltuBuff[CI_CUSTOM_CLTU_BUFF_SIZE];
} CI_CustomPChnl_t;

typedef struct
{
    IO_TransUdp_t        udp;       /**< Socket structure           */
    CI_CustomPChnl_t     pc;        /**< Physical channel           */
} CI_CustomPChnlUdp_t;

typedef struct
{
    int32                portFd;    /**< File Descriptor of port    */
    CI_CustomPChnl_t     pc;        /**< Physical channel           */
} CI_CustomPChnlSerial_t;


typedef struct
{
    IO_TransSelect_t       select;       /**< Select struct          */
    CI_CustomPChnlUdp_t    pcSocket;     /**< Udp Master Channel     */
    CI_CustomPChnlSerial_t pcSerial;     /**< Serial Master Channel  */
    TO_EnableOutputCmd_t   toEnableCmd;  /**< TO Enable CMD msg      */
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
static int32 CI_CustomReadCltuSocket(void);
static int32 CI_CustomReadCltuSerial(void);
static void CI_CustomProcessFrame(TCTF_Hdr_t *pTf, CI_CustomMChnl_t *pMc);
static void CI_CustomProcessPacket(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId);


/*******************************************************************************
** Custom Application Functions (Executed by Main Task)
*******************************************************************************/

/******************************************************************************/
/** \brief Custom Initialization
*******************************************************************************/
int32 CI_CustomInit(void)
{
    uint8 scId = CFE_SPACECRAFT_ID;
    int32 iStatus = CI_ERROR;
    uint32 taskId = 0;
    IO_TransRS422Config_t configSerial;
    IO_TransUdpConfig_t configSocket;
    
    /* Init as errors */
    g_CI_CustomData.pcSocket.udp.sockId = -1;
    g_CI_CustomData.pcSerial.portFd = -1;

    
    /* Initialize the UDP socket */
    CFE_PSP_MemSet((void *) &configSocket, 0x0, sizeof(IO_TransUdpConfig_t));
    strncpy(configSocket.cAddr, CI_CUSTOM_UDP_ADDR, 16);
    configSocket.usPort = CI_CUSTOM_UDP_PORT;
    configSocket.timeoutRcv = CI_CUSTOM_UDP_TIMEOUT;
    
    if (IO_TransUdpInit(&configSocket, &g_CI_CustomData.pcSocket.udp) < 0)
    {
        goto end_of_function;
    }
    
    /* Initialize a RS422 Serial Port */
    strncpy((char *) &configSerial.device, CI_CUSTOM_SERIAL_PORT, 
            PORT_NAME_SIZE);
    configSerial.baudRate = CI_CUSTOM_BAUD_RATE;
    configSerial.timeout  = CI_CUSTOM_TIMEOUT;
    configSerial.minBytes = CI_CUSTOM_MINBYTES;
    
    g_CI_CustomData.pcSerial.portFd = IO_TransRS422Init(&configSerial);

    if (g_CI_CustomData.pcSerial.portFd < 0)
    {
        goto end_of_function;
    }
    
    
    /* Initialize select */ 
    if (IO_TransSelectClear(&g_CI_CustomData.select) < 0)
    {
        goto end_of_function;
    }
    
    /* Add to select set */
    if (IO_TransSelectAddFd(&g_CI_CustomData.select,
                            g_CI_CustomData.pcSocket.udp.sockId) < 0)
    {
        goto end_of_function;
    }
    
    /* Add to select set  */
    if (IO_TransSelectAddFd(&g_CI_CustomData.select,
                            g_CI_CustomData.pcSerial.portFd) < 0)
    {
        goto end_of_function;
    }
    
    /* Note: We are setting the Virtual Chnl ID to: Socket: 0, Serial: 1 */

    /* Populate the ChannelService Table */
    TCTF_ChannelService_t   channelCfgTblUdp[CI_CUSTOM_TF_CHANNELS] = {
        {TCTF_SERVICE_VCP, 0, scId, 0, 0, 0, 0}
    };

    CFE_PSP_MemCpy((void *) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].chnlService,
                   (void *) &channelCfgTblUdp[0], sizeof(TCTF_ChannelService_t));
    
    /* Populate the ChannelService Table */
    TCTF_ChannelService_t   channelCfgTblSerial[CI_CUSTOM_TF_CHANNELS] = {
        {TCTF_SERVICE_VCP, 0, scId, 1, 0, 0, 0}
    };

    CFE_PSP_MemCpy((void *) &g_CI_CustomData.pcSerial.pc.mc.vChnls[0].chnlService,
                   (void *) &channelCfgTblSerial[0], sizeof(TCTF_ChannelService_t));


    /* Setup the CI Output Message (CLCW Message) */
    CFE_SB_InitMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd,
                   TO_APP_CMD_MID, sizeof(TO_CustomSetOcfCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd,
                      TO_SET_OCF_DATA_CC);
    COP1_InitClcw(&g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd.clcw, 0);

    /* Setup the CI Output Message (CLCW Message) */
    CFE_SB_InitMsg((CFE_SB_MsgPtr_t) &g_CI_CustomData.pcSerial.pc.mc.vChnls[0].clcwCmd,
                   TO_APP_CMD_MID, sizeof(TO_CustomSetOcfCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &g_CI_CustomData.pcSerial.pc.mc.vChnls[0].clcwCmd,
                      TO_SET_OCF_DATA_CC);
    COP1_InitClcw(&g_CI_CustomData.pcSerial.pc.mc.vChnls[0].clcwCmd.clcw, 1);
    

    /* Initialize the managed parameters of physical channels */
    g_CI_CustomData.pcSocket.pc.cltuRand = CI_CUSTOM_CLTU_RANDOM_SERIAL;
    g_CI_CustomData.pcSerial.pc.cltuRand = CI_CUSTOM_CLTU_RANDOM_UDP;


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
    g_CI_CustomData.toEnableCmd.iFileDesc = g_CI_CustomData.pcSerial.portFd;
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
    IO_TransRS422Close(g_CI_CustomData.pcSerial.portFd);
    IO_TransUdpCloseSocket(&g_CI_CustomData.pcSocket.udp);
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
    
    if (g_CI_CustomData.pcSocket.udp.sockId < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Socket ID not set. Check init. "
                          "Quitting CI_CustomMain.");
        goto end_of_function;
    }
    
    if (g_CI_CustomData.pcSerial.portFd < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR, 
                          "CI: Serial Port not set. Check init. "
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
                                         g_CI_CustomData.pcSocket.udp.sockId))
            {
                size = CI_CustomReadCltuSocket();
            }
            else if (IO_TransSelectFdInActive(&g_CI_CustomData.select,
                                         g_CI_CustomData.pcSerial.portFd))
            {
                size = CI_CustomReadCltuSerial();
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
/** \brief Custom Read CLTU From UDP (Private)
*******************************************************************************/
int32 CI_CustomReadCltuSocket(void)
{
    int32 iStatus = 0;
    int32 size = 0;
    CI_CustomPChnl_t *pPc = &g_CI_CustomData.pcSocket.pc;
    CI_CustomMChnl_t *pMc = &pPc->mc;
    TCTF_Hdr_t    *pTf = (TCTF_Hdr_t *) &pMc->tfBuff;

    /* Read Full CLTU from socket */
    size = IO_TransUdpRcv(&g_CI_CustomData.pcSocket.udp, 
                          &pPc->cltuBuff[0], 
                          CI_CUSTOM_CLTU_BUFF_SIZE); 

    /* Get the de-randomized transfer frame from the CLTU */
    iStatus = TC_SYNC_GetTransferFrame(pMc->tfBuff, pPc->cltuBuff,
                                       CI_CUSTOM_TF_BUFF_SIZE,
                                       CI_CUSTOM_CLTU_BUFF_SIZE,
                                       pPc->cltuRand);

    if (iStatus < 0)
    {
        /* Here we will ignore any non-cltu message. */
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                          "CI: Reveived invalid CLTU. Message Ignored.");
        return 0;
    }
    
    CI_CustomProcessFrame(pTf, pMc);

    return size;
}


/******************************************************************************/
/** \brief Custom Read CLTU From Serial (Private)
*******************************************************************************/
int32 CI_CustomReadCltuSerial(void)
{
    int32 iStatus = 0;
    int32 size = 0;
    uint16 tfOffset = 0;
    uint16 cltuOffset = 0;
    CI_CustomPChnl_t *pPc = &g_CI_CustomData.pcSerial.pc;
    CI_CustomMChnl_t *pMc = &pPc->mc;
    TCTF_Hdr_t    *pTf = (TCTF_Hdr_t *) &pMc->tfBuff;

    /* NOTE: For serial, we need to read the CLTU piecewise, since we don't 
     * know how long the cltu is. */

    /* Read CLTU start sequence */
    size = IO_TransRS422Read(g_CI_CustomData.pcSerial.portFd, 
                             &pPc->cltuBuff[0], 
                             TC_SYNC_START_SEQ_SIZE);


    if (size != TC_SYNC_START_SEQ_SIZE ||
        TC_SYNC_CheckStartSeq(&pPc->cltuBuff[0], &cltuOffset) < 0)
    {
        /* Here we will ignore any non-cltu message. */
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                          "CI: Reveived invalid CLTU. Message Ignored.");
        return 0; 
    }

    /* Extract TF from CLTU */
    while(1)
    {
        size = IO_TransRS422Read(g_CI_CustomData.pcSerial.portFd, 
                                 &pPc->cltuBuff[cltuOffset], 
                                 TC_SYNC_CODE_BLOCK_SIZE);
        
        if (size != TC_SYNC_CODE_BLOCK_SIZE)
        {
            /* Here we will ignore any non-cltu message. */
            iStatus = -1;
            break; /* while(1) */
        }

        iStatus = TC_SYNC_GetCodeBlockData(pMc->tfBuff,
                                           &pPc->cltuBuff[cltuOffset],
                                           &tfOffset, &cltuOffset, 
                                           CI_CUSTOM_TF_BUFF_SIZE,
                                           CI_CUSTOM_CLTU_BUFF_SIZE);

        /* This will also break when iStatus == TC_SYNC_FOUND_TAIL_SEQ */
        if (iStatus != TC_SYNC_SUCCESS)
        {
            break; /* while(1) */
        }
    }

    if (iStatus < 0)
    {
        /* Here we will ignore any non-cltu message. */
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                          "CI: Reveived invalid CLTU. Message Ignored.");
        return 0;
    }
    
    CI_CustomProcessFrame(pTf, pMc);    
    
    /* Now lets de-randomize if applicable */
    if (iStatus > 0 && pPc->cltuRand)
    {
        TC_SYNC_DeRandomizeFrame(pMc->tfBuff, tfOffset);
    }

    return size;
}


/******************************************************************************/
/** \brief Custom Process TF (Private)
*******************************************************************************/
void CI_CustomProcessFrame(TCTF_Hdr_t *pTf, CI_CustomMChnl_t *pMc)
{
    /* Get Transfer Frame Size */
    int32 size = 0;
    uint16 msgSize = 0;
    uint16 tfScId = TCTF_GetScId(pTf);
    uint16 tfVcId = TCTF_GetVcId(pTf);
    CFE_SB_Msg_t  *pSbMsg = NULL;
    uint8         *pSbMsgCursor = NULL;
    CFE_SB_MsgId_t msgId = 0;
    CFE_SB_Msg_t  *pClcwCmd = (CFE_SB_Msg_t *) &pMc->vChnls[0].clcwCmd;
    COP1_Clcw_t   *pClcw =
        (COP1_Clcw_t *) CFE_SB_GetUserData((void *)&pMc->vChnls[0].clcwCmd);
    
    /* Here you would route to different virtual channels based on the 
     * tfVcId. We are not using MAP services so all channels are virtual
     * channels. We only have one virtual channel in this example. */
    uint16 chIdx = 0;
    
    uint16 tfDataSize = 
        TCTF_GetPayloadLength(pTf, &pMc->vChnls[chIdx].chnlService);

    if (tfDataSize > CI_CUSTOM_BUFFER_SIZE)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                          "CI: Transfer Frame length larger than buffer. "
                          "Transfer Frame SC ID:0x%x, VC ID:0x%x dropped.", 
                          tfScId, tfVcId);
        return;
    }
    
    /* NOTE: For MAP Service, a packet may be split over multiple TF */
    pSbMsg = (CFE_SB_Msg_t *) pMc->vChnls[chIdx].pktBuff;
    pSbMsgCursor = (uint8 *) pSbMsg;

    /* Process the TCTF with COP1 */ 
    size = COP1_ProcessFrame((uint8 *) pSbMsg, pClcw, pTf,
                             &pMc->vChnls[chIdx].chnlService);

    /* Send the CLCW message for TO */
    CFE_SB_SendMsg(pClcwCmd);
    
    while (size > 0)
    {
        msgSize = CFE_SB_GetTotalMsgLength(pSbMsg);
        msgId = CFE_SB_GetMsgId(pSbMsg);
        
        pSbMsg = (CFE_SB_Msg_t *) pSbMsgCursor;
        pSbMsgCursor += msgSize;
        size -= msgSize;

        /* Note that this can be  normal behavior for MAP service. 
         * MAP services may split packets over multiple MAP channels 
         * We aren't using MAP service in this example. */
        if (size < 0)
        {
            CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_ERROR,
                "CI: Incomplete packet in Transfer Frame dropped. "
                "Transfer Frame SC ID:0x%x, VC ID:0x%x, "
                "Packet ID:0x%x.", tfScId, tfVcId, msgId);
        }
        else
        {
            CI_CustomProcessPacket(pSbMsg, msgId);
        }
    }
}

/******************************************************************************/
/** \brief Custom Process Packet (Private)
*******************************************************************************/
void CI_CustomProcessPacket(CFE_SB_MsgPtr_t pSbMsg, CFE_SB_MsgId_t msgId)
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
