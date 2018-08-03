/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

/*******************************************************************************
** File: cam_app.c
**
** Purpose:
**   This file contains the source code for the Sample STF1 App.
**
*******************************************************************************/

#include "cam_app.h"

/*
** global app data
*/
CAM_AppData_t CAM_AppData;

static CFE_EVS_BinFilter_t  CAM_EventFilters[] =
       {  /* Event ID    mask */
          {CAM_STARTUP_INF_EID,       5000, 5},
          {CAM_COMMAND_ERR_EID,       5000, 5},
          {CAM_COMMANDNOP_INF_EID,    5000, 5},
          {CAM_COMMANDRST_INF_EID,    5000, 5},
       };

/*
** CAM_AppMain() -- Application entry point and main process loop
*/
CFS_MODULE_DECLARE_APP(arducam, 118, 8192);
void arducam_Main( void )
{
    int32 status = 0;
    CAM_AppData.RunStatus = CFE_ES_APP_RUN;
    CFE_ES_PerfLogEntry(CAM_PERF_ID);

    /* 
    ** initialize the application, register the app, etc 
    */
    status = CAM_AppInit();
    if(status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CAM_INIT_ERR_EID, CFE_EVS_ERROR, "CAM App: init error %ld", status);
        CAM_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** CAM Runloop
    */
    while (CFE_ES_RunLoop(&CAM_AppData.RunStatus) == TRUE)
    {
        /*
        ** Exit performance profiling.  It will be restarted later in this while loop. 
        */
        CFE_ES_PerfLogExit(CAM_PERF_ID);

        /* 
        ** Pend on receipt of command packet -- set timeout to 500ms as cFE default
        ** could also set no timeout - this means that this app
        ** will block until a message is received.  Refer to the header file docs
        ** for more information on using this function
        */
        status = CFE_SB_RcvMsg(&CAM_AppData.MsgPtr, CAM_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution
        */
        CFE_ES_PerfLogEntry(CAM_PERF_ID);

        /*
        ** If the RcvMsg() was successful, then continue to process the CommandPacket()
        ** if not successful, then 
        */
        if (status == CFE_SUCCESS)
        {
            CAM_ProcessCommandPacket();
        }
        else if (status == CFE_SB_PIPE_RD_ERR)
        {
            /* This is an example of exiting on an error.
            ** Note that a SB read error is not always going to
            ** result in an app quitting.
            */
            CFE_EVS_SendEvent(CAM_PIPE_ERR_EID, CFE_EVS_ERROR, "CAM APP: SB Pipe Read Error, CAM APP will continue with error = %ld", status);
            //CAM_AppData.RunStatus = CFE_ES_APP_ERROR;
        }

    }

    CFE_ES_ExitApp(CAM_AppData.RunStatus);
} 


/* 
** CAM_AppInit() --  initialization
*/
int32 CAM_AppInit(void)
{
    int32 status = OS_SUCCESS;

    while (TRUE)
    {
        /*
        ** Register the app with Executive services
        */
        status = CFE_ES_RegisterApp();
        if (status != CFE_SUCCESS)
        {
            OS_printf("CAM APP: Register error %ld", status);
            break;
        }

        /*
        ** Register the events
        */ 
        status = CFE_EVS_Register(CAM_EventFilters,
                                  sizeof(CAM_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                                  CFE_EVS_BINARY_FILTER);
        if (status != CFE_SUCCESS)
        {
            OS_printf("CAM APP: EVS register error %ld", status);
            break;
        }

        /*
        ** Create the Software Bus command pipe 
        */
        status = CFE_SB_CreatePipe(&CAM_AppData.CmdPipe, CAM_PIPE_DEPTH, "CAM_CMD_PIPE");
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_PIPE_ERR_EID, CFE_EVS_ERROR, "CAM APP: Cmd pipe error %ld", status);
            break;
        }
        
        /*
        ** Subscribe to "ground commands". Ground commands are those commands with command codes
        */
        status = CFE_SB_Subscribe(CAM_CMD_MID, CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_SUB_CMD_ERR_EID, CFE_EVS_ERROR, "CAM APP: Ground command subscription error %ld", status);
            break;
        }

        /*
        ** Subscribe to housekeeping (hk) messages.  HK messages are those messages that request
        ** an app to send its HK telemetry
        */
        status = CFE_SB_Subscribe(CAM_SEND_HK_MID, CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_SUB_HK_ERR_EID, CFE_EVS_ERROR, "CAM APP: HK command subscription error %ld", status);
            break;
        }

        /*
        ** todo - subscribe to any other messages here - these are probably going to 
        **        be messages that are published from other apps that this app will
        **        need to perform a specific task
        */
        status = CFE_SB_Subscribe(0x186B, CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            OS_printf("CAM APP: CFE_SB_Subscribe error (id=%i)!\n", CAM_SEND_HK_MID);
            break;
        }

        /*
        ** Create data mutex
        */
        status = OS_MutSemCreate(&CAM_AppData.data_mutex, CAM_MUTEX_NAME, 0);
        if (status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_MUTEX_ERR_EID, CFE_EVS_ERROR, "CAM APP: Create mutex error %ld", status);
            break;
        }

        /* 
        ** Create child task wakeup semaphore
        */
        status = OS_BinSemCreate(&CAM_AppData.sem_id, CAM_SEM_NAME, 0, 0);
        if (status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_SEMAPHORE_ERR_EID, CFE_EVS_ERROR, "CAM APP: Semaphore create error %ld", status);
            break;
        }

        /*
        ** Initialize Application Data
        */
        CAM_AppData.State = CAM_STOP;
        CAM_AppData.Exp = 0;
        CAM_AppData.Size = size_160x120;
        CAM_AppData.HkTelemetryPkt.CommandCount       = 0;
        CAM_AppData.HkTelemetryPkt.CommandErrorCount  = 0;

        /* 
        ** Create child task
        */
        status = CAM_ChildInit();
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_CHILD_ERR_EID, CFE_EVS_ERROR, "CAM App: Child task init error %ld", status);
            break;
        }

        /* Initialize the published HK message - this HK message will contain the telemetry
        ** that has been defined in the CAM_HkTelemetryPkt for this app
        */
        CFE_SB_InitMsg(&CAM_AppData.HkTelemetryPkt,
            CAM_HK_TLM_MID,
            CAM_HK_TLM_LNGTH, TRUE);

        /*
        ** todo - initialize any other messages that this app will publish.  The cFS "way", is to 
        **        mainly use the app's HK message to push telemetry and data onto the Software Bus (SB)
        */
        CFE_SB_InitMsg(&CAM_AppData.Exp_Pkt,
            CAM_EXP_TLM_MID,
            CAM_EXP_TLM_LNGTH, TRUE);
        CFE_SB_InitMsg(&CAM_AppData.EoE,
            CAM_EOE_MID,
            CAM_NOARGSCMD_LNGTH, TRUE);

        /* 
        ** Important to send an information event that the app has initialized. this is
        ** useful for debugging the loading of individual apps
        */
        CFE_EVS_SendEvent (CAM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                "CAM App Initialized. Version %d.%d.%d.%d",
                    CAM_MAJOR_VERSION,
                    CAM_MINOR_VERSION, 
                    CAM_REVISION, 
                    CAM_MISSION_REV);
        break;
    }
    
    return status;
} 


/* 
**  Name:  CAM_ProcessCommandPacket
**
**  Purpose:
**  This routine will process any packet that is received on the CAM command pipe.       
*/
void CAM_ProcessCommandPacket(void)
{
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_SB_MsgId_t  MsgId = CFE_SB_GetMsgId(CAM_AppData.MsgPtr);
    OS_MutSemGive(CAM_AppData.data_mutex);
    switch (MsgId)
    {
        /*
        ** Ground Commands with command codes fall under the CAM_APP_CMD_MID
        ** message ID
        */
        case CAM_CMD_MID:
            CAM_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        ** The HK MID comes first, as it is currently the only other messages defined
        ** besides the CAM_APP_CMD_MID message above
        */
        case CAM_SEND_HK_MID:
            CAM_ReportHousekeeping();
            break;

        case 0x186B:
            CAM_ProcessPR();
            break;

         /*
         ** All other invalid messages that this app doesn't recognize, increment
         ** the command error counter and log as an error event.  
         */
        default:
            CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(CAM_COMMAND_ERR_EID,CFE_EVS_ERROR, "CAM App: invalid command packet, MID = 0x%x", MsgId);
            break;
    }

    return;
} 


/*
** CAM_ProcessGroundCommand() -- CAM ground commands
*/
void CAM_ProcessGroundCommand(void)
{
    // Local variables
    uint8  state = 1;
    uint16 x      = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case below 
    */
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(CAM_AppData.MsgPtr);
    OS_MutSemGive(CAM_AppData.data_mutex);

    /*
    ** Ground Commands, by definition, has a command code associated with them.  Pull
    ** this command code from the message and then process the action associated with
    ** the command code.
    */
    OS_MutSemTake(CAM_AppData.data_mutex);
        uint16 CommandCode = CFE_SB_GetCmdCode(CAM_AppData.MsgPtr);
    OS_MutSemGive(CAM_AppData.data_mutex);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case CAM_NOOP_CC:
            /* 
            ** notice the usage of the VerifyCmdLength() function call to verify that
            ** the command length is as expected.  
            */
            if (CAM_VerifyCmdLength(CAM_AppData.MsgPtr, sizeof(CAM_NoArgsCmd_t)))
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    CAM_AppData.HkTelemetryPkt.CommandCount++;
                OS_MutSemGive(CAM_AppData.data_mutex);
                CFE_EVS_SendEvent(CAM_COMMANDNOP_INF_EID, CFE_EVS_INFORMATION, "CAM App: NOOP command");
            }
            break;

        /*
        ** Reset Counters Command
        */
        case CAM_RESET_COUNTERS_CC:
            CAM_ResetCounters();
            break;
        
        /*
        ** Stop Science Command
        */
        case CAM_STOP_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_STOP_INF_EID, CFE_EVS_INFORMATION,
                "CAM App: STOP command");
            break;
            
        /*
        ** Pause Science Command
        */
        case CAM_PAUSE_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_PAUSE;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_PAUSE_INF_EID, CFE_EVS_INFORMATION,
                "CAM App: PAUSE command");
            break;
            
        /*
        ** Resume Science Command
        */
        case CAM_RESUME_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_RUN;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_RUN_INF_EID, CFE_EVS_INFORMATION,
                "CAM App: RESUME command");
            break;

        /*
        ** Timeout Command
        */
        case CAM_TIMEOUT_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_TIME;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_TIMEOUT_INF_EID, CFE_EVS_INFORMATION,
                "CAM App: TIMEOUT command");
            break;

        /*
        ** Low Voltage Command
        */
        case CAM_LOW_VOLTAGE_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_LOW_VOLTAGE;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_LOW_VOLTAGE_INT_EID, CFE_EVS_INFORMATION,
                "CAM App: LOW_VOLTAGE command");
            break;
        
        /*
        ** EXP 1 - Small
        */
        case CAM_EXP1_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 1;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP1_EID, CFE_EVS_INFORMATION, "CAM App: EXP 1 Command - Small Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;

        /*
        ** EXP 2  - Medium
        */
        case CAM_EXP2_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 2;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP2_EID, CFE_EVS_INFORMATION, "CAM App: EXP 2 Command - Medium Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;
        /*
        ** EXP 3 - Large
        */
        case CAM_EXP3_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 3;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP3_EID, CFE_EVS_INFORMATION, "CAM App: EXP 3 Command - Large Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;

        /*
        **  Hardware Check
        */
        case CAM_HW_CHECK_CC:
            CAM_AppData.HkTelemetryPkt.CommandCount++;
            state = OS_SUCCESS;
            state = CAM_init_i2c();
            if (state != OS_SUCCESS)
            {   
                CFE_EVS_SendEvent(CAM_INIT_I2C_ERR_EID,CFE_EVS_ERROR, "CAM App: I2C Failure");
            }
            else
            {
                state = CAM_init_spi();
                if (state != OS_SUCCESS)
                {   
                    CFE_EVS_SendEvent(CAM_INIT_SPI_ERR_EID,CFE_EVS_ERROR, "CAM App: SPI Failure"); 
                }
                else
                {  
                    CFE_EVS_SendEvent(CAM_HW_CHECK_EID, CFE_EVS_INFORMATION, "CAM App: Hardware Checked Out");
                }
            }
            break;

        /*
        **  Debug and Testing CC
        */
        case CAM_HWLIB_INIT_I2C_CC:
            CAM_init_i2c();
            break;
        case CAM_HWLIB_INIT_SPI_CC:
            CAM_init_spi();
            break;
        case CAM_HWLIB_CONFIG_CC:
            CAM_config();
            break;
        case CAM_HWLIB_JPEG_INIT_CC:
            CAM_jpeg_init();
            break;
        case CAM_HWLIB_YUV422_CC:
            CAM_yuv422();
            break;
        case CAM_HWLIB_JPEG_CC:
            CAM_jpeg();
            break;
        case CAM_HWLIB_SETUP_CC:
            CAM_setup();
            break;
        case CAM_HWLIB_SETSIZE_CC:
            CAM_setSize(CAM_AppData.Size);
            break;
        case CAM_HWLIB_CAPTURE_PREP_CC:
            CAM_capture_prep();
            break;
        case CAM_HWLIB_CAPTURE_CC:
            CAM_capture();
            break;
        case CAM_HWLIB_READ_PREP_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.Exp_Pkt.msg_count = 0x0000;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CAM_read_prep((char*) &CAM_AppData.Exp_Pkt.data, (uint16*) &x);
            break;
        case CAM_HWLIB_READ_CC:
            x = 1;
            state = 1;
            CAM_fifo(&x, &state);
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_COMMAND_ERR_EID, CFE_EVS_ERROR, 
                "CAM App: invalid command code for packet MID = 0x%x CC = 0x%x", MsgId, CommandCode);
            break;
    }
    return;
} 

/* 
**  Name:  CAM_ReportHousekeeping                                             
**                                                                            
**  Purpose:                                                                  
**         This function is triggered in response to a task telemetry request 
**         from the housekeeping task. This function will gather the Apps     
**         telemetry, packetize it and send it to the housekeeping task via   
**         the software bus                                                   
*/
void CAM_ReportHousekeeping(void)
{
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CAM_AppData.HkTelemetryPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.HkTelemetryPkt);
    OS_MutSemGive(CAM_AppData.data_mutex);
    return;
} 

/*
** CAM_ProcessPR() -- CAM Pause / Resume Commands
*/
void CAM_ProcessPR(void)
{
    /*
    ** MsgId is only needed if the command code is not recognized. See default case below 
    */
    CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(CAM_AppData.MsgPtr);   

    /*
    ** Ground Commands, by definition, has a command code associated with them.  Pull
    ** this command code from the message and then process the action associated with
    ** the command code.
    */
    uint16 CommandCode = CFE_SB_GetCmdCode(CAM_AppData.MsgPtr);
    switch (CommandCode)
    {
        case CAM_PR_PAUSE_CC:
            /* 
            ** notice the usage of the VerifyCmdLength() function call to verify that
            ** the command length is as expected.  
            */
            if (CAM_VerifyCmdLength(CAM_AppData.MsgPtr, sizeof(CAM_NoArgsCmd_t)))
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    CAM_AppData.HkTelemetryPkt.CommandCount++;
                    CAM_AppData.State = CAM_PAUSE;
                OS_MutSemGive(CAM_AppData.data_mutex);
                CFE_EVS_SendEvent(CAM_PAUSE_INF_EID, CFE_EVS_INFORMATION,
                    "CAM App: PAUSE command");
            }
            break;

        case CAM_PR_RESUME_CC:
            /* 
            ** notice the usage of the VerifyCmdLength() function call to verify that
            ** the command length is as expected.  
            */
            if (CAM_VerifyCmdLength(CAM_AppData.MsgPtr, sizeof(CAM_NoArgsCmd_t)))
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    CAM_AppData.HkTelemetryPkt.CommandCount++;
                    CAM_AppData.State = CAM_RUN;
                OS_MutSemGive(CAM_AppData.data_mutex);
                CFE_EVS_SendEvent(CAM_RUN_INF_EID, CFE_EVS_INFORMATION,
                    "CAM App: RESUME command");
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(CAM_COMMAND_ERR_EID, CFE_EVS_ERROR, 
                "CAM App: invalid command code for packet MID = 0x%x CC = 0x%x", MsgId, CommandCode);
            break;
    }
    return;
} 

/*
**  Name:  CAM_ResetCounters                                               
**                                                                            
**  Purpose:                                                                  
**         This function resets all the global counter variables that are    
**         part of the task telemetry.                                        
*/
void CAM_ResetCounters(void)
{
    /* Status of commands processed by the CAM App */
    CAM_AppData.HkTelemetryPkt.CommandCount       = 0;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount  = 0;
    CFE_EVS_SendEvent(CAM_COMMANDRST_INF_EID, CFE_EVS_INFORMATION, "CAM App: RESET Counters Command");
    return;
} 

/*
** CAM_VerifyCmdLength() -- Verify command packet length                                                                                              
*/
boolean CAM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(CAM_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X CC = %d Len = %d Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);

        result = FALSE;
        CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
    }

    return(result);
} 

