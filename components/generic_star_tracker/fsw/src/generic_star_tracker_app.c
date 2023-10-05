/*******************************************************************************
** File: generic_star_tracker_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_STAR_TRACKER application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "generic_star_tracker_app.h"


/*
** Global Data
*/
GENERIC_STAR_TRACKER_AppData_t GENERIC_STAR_TRACKER_AppData;

/*
** Application entry point and main process loop
*/
void ST_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_STAR_TRACKER_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_STAR_TRACKER_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_STAR_TRACKER_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_STAR_TRACKER_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_STAR_TRACKER_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_STAR_TRACKER_AppData.MsgPtr,  GENERIC_STAR_TRACKER_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_STAR_TRACKER_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_STAR_TRACKER_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: SB Pipe Read Error = %d", (int) status);
            GENERIC_STAR_TRACKER_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    GENERIC_STAR_TRACKER_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_STAR_TRACKER_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_STAR_TRACKER_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_STAR_TRACKER_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_STAR_TRACKER_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_STAR_TRACKER: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_STAR_TRACKER_AppData.CmdPipe, GENERIC_STAR_TRACKER_PIPE_DEPTH, "GENERIC_ST_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_STAR_TRACKER_CMD_MID), GENERIC_STAR_TRACKER_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_STAR_TRACKER_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_STAR_TRACKER_REQ_HK_MID), GENERIC_STAR_TRACKER_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_STAR_TRACKER_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /*
    ** TODO: Subscribe to any other messages here
    */


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_STAR_TRACKER_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_STAR_TRACKER_HK_TLM_MID),
                   GENERIC_STAR_TRACKER_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_STAR_TRACKER_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_STAR_TRACKER_DEVICE_TLM_MID),
                   GENERIC_STAR_TRACKER_DEVICE_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */


    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_STAR_TRACKER_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_STAR_TRACKER_DEVICE_DISABLED;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus = 0;

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_STAR_TRACKER App Initialized. Version %d.%d.%d.%d",
                GENERIC_STAR_TRACKER_MAJOR_VERSION,
                GENERIC_STAR_TRACKER_MINOR_VERSION, 
                GENERIC_STAR_TRACKER_REVISION, 
                GENERIC_STAR_TRACKER_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_STAR_TRACKER: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_STAR_TRACKER command pipe
*/
void GENERIC_STAR_TRACKER_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_STAR_TRACKER_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_STAR_TRACKER_CMD_MID (Message ID)
        */
        case GENERIC_STAR_TRACKER_CMD_MID:
            GENERIC_STAR_TRACKER_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_STAR_TRACKER_REQ_HK_MID:
            GENERIC_STAR_TRACKER_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void GENERIC_STAR_TRACKER_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_STAR_TRACKER_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_STAR_TRACKER_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_STAR_TRACKER_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_STAR_TRACKER_VerifyCmdLength(GENERIC_STAR_TRACKER_AppData.MsgPtr, sizeof(GENERIC_STAR_TRACKER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_STAR_TRACKER_RESET_COUNTERS_CC:
            if (GENERIC_STAR_TRACKER_VerifyCmdLength(GENERIC_STAR_TRACKER_AppData.MsgPtr, sizeof(GENERIC_STAR_TRACKER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: RESET counters command received");
                GENERIC_STAR_TRACKER_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case GENERIC_STAR_TRACKER_ENABLE_CC:
            if (GENERIC_STAR_TRACKER_VerifyCmdLength(GENERIC_STAR_TRACKER_AppData.MsgPtr, sizeof(GENERIC_STAR_TRACKER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: Enable command received");
                GENERIC_STAR_TRACKER_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case GENERIC_STAR_TRACKER_DISABLE_CC:
            if (GENERIC_STAR_TRACKER_VerifyCmdLength(GENERIC_STAR_TRACKER_AppData.MsgPtr, sizeof(GENERIC_STAR_TRACKER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: Disable command received");
                GENERIC_STAR_TRACKER_Disable();
            }
            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case GENERIC_STAR_TRACKER_CONFIG_CC:
            if (GENERIC_STAR_TRACKER_VerifyCmdLength(GENERIC_STAR_TRACKER_AppData.MsgPtr, sizeof(GENERIC_STAR_TRACKER_Config_cmd_t)) == OS_SUCCESS)
            {
                uint32_t config = ntohl(((GENERIC_STAR_TRACKER_Config_cmd_t*) GENERIC_STAR_TRACKER_AppData.MsgPtr)->DeviceCfg); // command is defined as big-endian... need to convert to host representation
                CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: Configuration command received: %u", config);
                /* Command device to send HK */
                status = GENERIC_STAR_TRACKER_CommandDevice(&GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart, GENERIC_STAR_TRACKER_DEVICE_CFG_CMD, config);
                if (status == OS_SUCCESS)
                {
                    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_STAR_TRACKER: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void GENERIC_STAR_TRACKER_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_STAR_TRACKER_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_STAR_TRACKER_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_STAR_TRACKER_REQ_HK_TLM:
            GENERIC_STAR_TRACKER_ReportHousekeeping();
            break;

        case GENERIC_STAR_TRACKER_REQ_DATA_TLM:
            GENERIC_STAR_TRACKER_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_STAR_TRACKER: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_STAR_TRACKER_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_STAR_TRACKER_DEVICE_ENABLED)
    {
        status = GENERIC_STAR_TRACKER_RequestHK(&GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart, (GENERIC_STAR_TRACKER_Device_HK_tlm_t*) &GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount++;
        }
        else
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_STAR_TRACKER: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void GENERIC_STAR_TRACKER_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_STAR_TRACKER_DEVICE_ENABLED)
    {
        status = GENERIC_STAR_TRACKER_RequestData(&GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart, (GENERIC_STAR_TRACKER_Device_Data_tlm_t*) &GENERIC_STAR_TRACKER_AppData.DevicePkt.Generic_star_tracker);
        if (status == OS_SUCCESS)
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_STAR_TRACKER_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_STAR_TRACKER_AppData.DevicePkt, true);
        }
        else
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_STAR_TRACKER: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_STAR_TRACKER_ResetCounters(void)
{
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void GENERIC_STAR_TRACKER_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_STAR_TRACKER_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols available
        */ 
        GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart.deviceString = GENERIC_STAR_TRACKER_CFG_STRING;
        GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart.handle = GENERIC_STAR_TRACKER_CFG_HANDLE;
        GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart.isOpen = PORT_CLOSED;
        GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart.baud = GENERIC_STAR_TRACKER_CFG_BAUDRATE_HZ;
        GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart);
        if (status == OS_SUCCESS)
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_STAR_TRACKER_DEVICE_ENABLED;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: Device enabled");
        }
        else
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: UART port initialization error %d", status);
        }
    }
    else
    {
        GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void GENERIC_STAR_TRACKER_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_STAR_TRACKER_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(&GENERIC_STAR_TRACKER_AppData.Generic_star_trackerUart);
        if (status == OS_SUCCESS)
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_STAR_TRACKER_DEVICE_DISABLED;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_STAR_TRACKER: Device disabled");
        }
        else
        {
            GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: UART port close error %d", status);
        }
    }
    else
    {
        GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_STAR_TRACKER: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 GENERIC_STAR_TRACKER_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_STAR_TRACKER_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_STAR_TRACKER_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 
