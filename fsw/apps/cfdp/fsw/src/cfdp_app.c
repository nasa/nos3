/*******************************************************************************
** File: cfdp_app.c
**
** Purpose:
**   This file contains the source code for the CFDP application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "cfdp_app.h"
#include "cfdp_cfdp.h"
#include "cfdp_cmd.h"
#include "cfdp_fcncodes.h"
// #include "cfs_utils.h"
// #include "cfe.h"
// #include "cfdp_events.h"
#include "cfdp_platform_cfg.h"
#include "cfdp_perfids.h"
#include "cfdp_msg.h"
#include "cfdp_msgids.h"
#include "cfdp_version.h"



/*
** Global Data
*/
CFDP_AppData_t CFDP_AppData;

/*
** Application entry point and main process loop
*/
void CFDP_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(CFDP_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = CFDP_AppInit();
    if (status != CFE_SUCCESS)
    {
        CFDP_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&CFDP_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(CFDP_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&CFDP_AppData.MsgPtr,  CFDP_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(CFDP_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            CFDP_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(CFDP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "CFDP: SB Pipe Read Error = %d", (int) status);
            CFDP_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(CFDP_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(CFDP_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 CFDP_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    CFDP_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFDP: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&CFDP_AppData.CmdPipe, CFDP_PIPE_DEPTH, "CFDP_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFDP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFDP_CMD_MID), CFDP_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFDP_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            CFDP_CMD_MID, (unsigned int) status);
        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFDP_WAKE_UP_MID), CFDP_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFDP_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            CFDP_WAKE_UP_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFDP_REQ_HK_MID), CFDP_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFDP_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            CFDP_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /*
    if (status == CFE_SUCCESS)
    {
        status = CFDP_TableInit(); 
    }

    if (status == CFE_SUCCESS)
    {
        status = CFDP_CFDP_InitEngine(); 
    }
    */

    /*
    ** TODO: Subscribe to any other messages here
    */


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the CFDP_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(CFDP_AppData.hk.TlmHeader),
                   CFE_SB_ValueToMsgId(CFDP_HK_TLM_MID),
                   CFDP_HK_TLM_LNGTH);

    CFE_MSG_Init(CFE_MSG_PTR(CFDP_AppData.fileInfo.TlmHeader),
                   CFE_SB_ValueToMsgId(CFDP_FILEDOWNLOAD_TLM_MID),
                   CFDP_CFDP_PDU_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */


    /* 
    ** Always reset all counters during application initialization 
    */
    CFDP_ResetCounters();

    memset(CFDP_AppData.fileInfo.buffer, '\0', sizeof(CFDP_AppData.fileInfo.buffer));

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(CFDP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "CFDP App Initialized. Version %d.%d.%d.%d",
                CFDP_MAJOR_VERSION,
                CFDP_MINOR_VERSION, 
                CFDP_REVISION, 
                CFDP_MISSION_REV); 
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFDP: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the CFDP command pipe
*/
void CFDP_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t msg = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(CFDP_AppData.MsgPtr, &msg);
    switch (CFE_SB_MsgIdToValue(msg))
    {
        /*
        ** Ground Commands with command codes fall under the CFDP_CMD_MID (Message ID)
        */
        case CFDP_CMD_MID:
            CFDP_ProcessGroundCommand();
            break;

         case CFDP_WAKE_UP_MID:
            //CFDP_WakeupCmd((const CFDP_WakeupCmd_t *)CFDP_AppData.MsgPtr);
            break;
        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case CFDP_REQ_HK_MID:
            CFDP_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            CFDP_AppData.hk.cmdErrorCount++;
            CFE_EVS_SendEvent(CFDP_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "CFDP: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(msg));
            break;
    }
    return;
} 

/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void CFDP_ProcessGroundCommand()
{
    CFE_SB_MsgId_t msg = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(CFDP_AppData.MsgPtr, &msg);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(CFDP_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case CFDP_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (CFDP_VerifyCmdLength(CFDP_AppData.MsgPtr, sizeof(CFDP_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFDP_NoopCmd();
            }
            break;

        /*
        ** Reset Counters Command
        */
        case CFDP_RST_CTR_CC:
            if (CFDP_VerifyCmdLength(CFDP_AppData.MsgPtr, sizeof(CFDP_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFDP_ResetCounters();
            }
            break;
        /*
        ** Download file from Satellite
        */
        case CFDP_DOWNLOADFILE_CC:
            if (CFDP_VerifyCmdLength(CFDP_AppData.MsgPtr, sizeof(CFDP_FileTxSatellite_t)) == OS_SUCCESS)
            {
                CFDP_DownloadFromSatellite(CFDP_AppData.MsgPtr);
            }
            break;
        /*
        ** Upload file to Satellite
        */
        case CFDP_UPLOADFILE_CC:
            if (CFDP_VerifyCmdLength(CFDP_AppData.MsgPtr, sizeof(CFDP_FileTxSatellite_t)) == OS_SUCCESS)
            {
                CFDP_UploadToSatelliteConfirm(CFDP_AppData.MsgPtr);
            }
            break;
        case CFDP_UPLOADFILE_DATA_CC:
            if (CFDP_VerifyCmdLength(CFDP_AppData.MsgPtr, sizeof(CFDP_FileSatellite_t)) == OS_SUCCESS)
            {
                CFDP_UploadToSatellite(CFDP_AppData.MsgPtr);
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            CFDP_AppData.hk.cmdErrorCount++;
            CFE_EVS_SendEvent(CFDP_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "CFDP: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(msg), CommandCode);
            break;
    }
    return;
} 

/*
** Verify command packet length matches expected
*/
int32 CFDP_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        CFDP_AppData.hk.cmdCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(CFDP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %ld, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        CFDP_AppData.hk.cmdErrorCount++;
    }
    return status;
} 

/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void CFDP_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(CFDP_AppData.MsgPtr, &MsgId);   

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(CFDP_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case CFDP_REQ_HK_TLM:
            CFDP_ReportHousekeeping();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            CFDP_AppData.hk.cmdErrorCount++;
            CFE_EVS_SendEvent(CFDP_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "CFDP: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void CFDP_ReportHousekeeping(void)
{

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFDP_AppData.hk);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CFDP_AppData.hk, true);
    return;
}