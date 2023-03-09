/*******************************************************************************
** File: 
**  generic_mag_app.c
**
** Purpose:
**   This file contains the source code for the Generic_mag App.
**
*******************************************************************************/

/*
** Include Files:
*/

//#include "string.h"
//#include "generic_mag_app_events.h"

#include "generic_mag_app.h"
#include "generic_mag_app_version.h"
#include "generic_mag_app_msgids.h"
#include "generic_mag_app_perfids.h"
#include "generic_mag_device.h"
#include "cfe_error.h"

/*
** global data
*/
GENERIC_MAG_AppData_t GENERIC_MAG_AppData;

// Forward declarations
static int32 GENERIC_MAG_AppInit(void);
static void  GENERIC_MAG_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg);
static void  GENERIC_MAG_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg);
static int32 GENERIC_MAG_ReportHousekeeping(const CFE_SB_CmdHdr_t *Msg);
static int32 GENERIC_MAG_ResetCounters(const GENERIC_MAG_ResetCounters_t *Msg);
static int32 GENERIC_MAG_Noop(const GENERIC_MAG_Noop_t *Msg);
static bool GENERIC_MAG_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Name:  GENERIC_MAG_AppMain()                                                    */
/* Purpose:                                                                   */
/*        Application entry point and main process loop                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void GENERIC_MAG_AppMain(void)
{
    int32 status;

    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_MAG_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = GENERIC_MAG_AppInit();
    if (status != CFE_SUCCESS)
    {
        RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** GENERIC_MAG Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(GENERIC_MAG_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_RcvMsg(&GENERIC_MAG_AppData.MsgPtr, GENERIC_MAG_AppData.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(GENERIC_MAG_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            GENERIC_MAG_ProcessCommandPacket(GENERIC_MAG_AppData.MsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_MAG_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "GENERIC_MAG APP: SB Pipe Read Error, App Will Exit");

            RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    RunStatus = CFE_ES_RunStatus_APP_EXIT; // we are wanting to exit... make sure everyone knows it

    status = GENERIC_MAG_DeviceShutdown();
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error Shutting Down Device, RC = 0x%08lX\n", (unsigned long)status);
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(GENERIC_MAG_APP_PERF_ID);

    CFE_ES_ExitApp(RunStatus);

} /* End of GENERIC_MAG_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Name:  GENERIC_MAG_AppInit()                                                    */
/*                                                                            */
/* Purpose:                                                                   */
/*        Initialization                                                      */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
static int32 GENERIC_MAG_AppInit(void)
{
    int32 status;

    RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandCounter = 0;
    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter = 0;

    /*
    ** Initialize app configuration data
    */
    GENERIC_MAG_AppData.PipeDepth = GENERIC_MAG_PIPE_DEPTH;

    /*
    ** Initialize event filter table...
    */
    GENERIC_MAG_AppData.EventFilters[0].EventID = GENERIC_MAG_STARTUP_INF_EID;
    GENERIC_MAG_AppData.EventFilters[0].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[1].EventID = GENERIC_MAG_COMMAND_ERR_EID;
    GENERIC_MAG_AppData.EventFilters[1].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[2].EventID = GENERIC_MAG_COMMANDNOP_INF_EID;
    GENERIC_MAG_AppData.EventFilters[2].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[3].EventID = GENERIC_MAG_COMMANDRST_INF_EID;
    GENERIC_MAG_AppData.EventFilters[3].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[4].EventID = GENERIC_MAG_INVALID_MSGID_ERR_EID;
    GENERIC_MAG_AppData.EventFilters[4].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[5].EventID = GENERIC_MAG_LEN_ERR_EID;
    GENERIC_MAG_AppData.EventFilters[5].Mask    = 0x0000;
    GENERIC_MAG_AppData.EventFilters[6].EventID = GENERIC_MAG_PIPE_ERR_EID;
    GENERIC_MAG_AppData.EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(GENERIC_MAG_AppData.EventFilters, GENERIC_MAG_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&GENERIC_MAG_AppData.HkBuf.MsgHdr, GENERIC_MAG_APP_HK_TLM_MID, sizeof(GENERIC_MAG_AppData.HkBuf), true);

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&GENERIC_MAG_AppData.CommandPipe, GENERIC_MAG_AppData.PipeDepth, "GENERIC_MAG_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(GENERIC_MAG_APP_SEND_HK_MID, GENERIC_MAG_AppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(GENERIC_MAG_APP_CMD_MID, GENERIC_MAG_AppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }

    status = GENERIC_MAG_DeviceInit();
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_mag App: Error Initializing Device, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }

    CFE_EVS_SendEvent(GENERIC_MAG_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "GENERIC_MAG App Initialized. Version %d.%d.%d.%d",
                      GENERIC_MAG_APP_MAJOR_VERSION,
                      GENERIC_MAG_APP_MINOR_VERSION,
                      GENERIC_MAG_APP_REVISION,
                      GENERIC_MAG_APP_MISSION_REV);

    return (CFE_SUCCESS);

} /* End of GENERIC_MAG_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_MAG_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the GENERIC_MAG    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
static void GENERIC_MAG_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg)
{
    CFE_SB_MsgId_t MsgId;

    MsgId = CFE_SB_GetMsgId(Msg);

    switch (MsgId)
    {
        case GENERIC_MAG_APP_CMD_MID:
            GENERIC_MAG_ProcessGroundCommand(Msg);
            break;

        case GENERIC_MAG_APP_SEND_HK_MID:
            GENERIC_MAG_ReportHousekeeping((CFE_SB_CmdHdr_t *)Msg);
            break;

        default:
            CFE_EVS_SendEvent(GENERIC_MAG_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "GENERIC_MAG: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End GENERIC_MAG_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Name:  GENERIC_MAG_ProcessGroundCommand()                                       */
/*                                                                            */
/* Purpose:                                                                   */
/*        GENERIC_MAG ground commands                                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
static void GENERIC_MAG_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg)
{
    uint16 CommandCode = CFE_SB_GetCmdCode(Msg);
    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandCounter++;

    /*
    ** Process "known" GENERIC_MAG app ground commands
    */
    switch (CommandCode)
    {
        case GENERIC_MAG_APP_NOOP_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_Noop_t)))
            {
                GENERIC_MAG_Noop((GENERIC_MAG_Noop_t *)Msg);
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }

            break;

        case GENERIC_MAG_APP_RESET_COUNTERS_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_ResetCounters_t)))
            {
                GENERIC_MAG_ResetCounters((GENERIC_MAG_ResetCounters_t *)Msg);
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }

            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        */
        case GENERIC_MAG_APP_RESET_DEV_CNTRS_CC:
            GENERIC_MAG_DeviceResetCounters();
            break;

        case GENERIC_MAG_GET_DEV_DATA_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_GetDevData_cmd_t))) {
                GENERIC_MAG_DeviceGetGeneric_magDataCommand();
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        case GENERIC_MAG_CONFIG_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_Config_cmd_t)))
            {
                GENERIC_MAG_DeviceConfigurationCommand(((GENERIC_MAG_Config_cmd_t *)Msg)->MillisecondStreamDelay);
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        case GENERIC_MAG_OTHER_CMD_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_Other_cmd_t)))
            {
                GENERIC_MAG_DeviceOtherCommand();
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        case GENERIC_MAG_RAW_CMD_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_Raw_cmd_t)))
            {
                GENERIC_MAG_DeviceRawCommand(((GENERIC_MAG_Raw_cmd_t *)Msg)->RawCmd, sizeof(((GENERIC_MAG_Raw_cmd_t *)Msg)->RawCmd));
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        case GENERIC_MAG_SEND_DEV_HK_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_SendDevHk_cmd_t))) {
                GENERIC_MAG_ReportDeviceHousekeeping();
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        case GENERIC_MAG_SEND_DEV_DATA_CC:
            if (GENERIC_MAG_VerifyCmdLength(Msg, sizeof(GENERIC_MAG_SendDevData_cmd_t))) {
                GENERIC_MAG_ReportDeviceGeneric_magData();
            } else {
                GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(GENERIC_MAG_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
            break;
    }

    return;

} /* End of GENERIC_MAG_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_MAG_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
static int32 GENERIC_MAG_ReportHousekeeping(const CFE_SB_CmdHdr_t *Msg)
{
    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandCounter++;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&GENERIC_MAG_AppData.HkBuf.MsgHdr);
    CFE_SB_SendMsg(&GENERIC_MAG_AppData.HkBuf.MsgHdr);

    return CFE_SUCCESS;

} /* End of GENERIC_MAG_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Name:  GENERIC_MAG_Noop                                                         */
/*                                                                            */
/* Purpose:                                                                   */
/*        GENERIC_MAG NOOP command                                                 */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
static int32 GENERIC_MAG_Noop(const GENERIC_MAG_Noop_t *Msg)
{

    CFE_EVS_SendEvent(GENERIC_MAG_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_MAG: NOOP command");

    return CFE_SUCCESS;

} /* End of GENERIC_MAG_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_MAG_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
static int32 GENERIC_MAG_ResetCounters(const GENERIC_MAG_ResetCounters_t *Msg)
{

    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandCounter = 0;
    GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter = 0;

    CFE_EVS_SendEvent(GENERIC_MAG_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_MAG: RESET command");

    return CFE_SUCCESS;

} /* End of GENERIC_MAG_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Name:  GENERIC_MAG_VerifyCmdLength()                                            */
/*                                                                            */
/* Purpose:                                                                   */
/*        Verify command packet length                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
static bool GENERIC_MAG_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
{
    bool result = true;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(Msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(GENERIC_MAG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          (unsigned int)CFE_SB_MsgIdToValue(MessageID), CommandCode, ActualLength, ExpectedLength);

        result = false;

        GENERIC_MAG_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter++;
    }

    return (result);

} /* End of GENERIC_MAG_VerifyCmdLength() */
