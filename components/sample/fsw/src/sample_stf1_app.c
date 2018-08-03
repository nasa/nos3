/* Copyright (C) 2009 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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
** File: sample_stf1_app.c
**
** Purpose:
**   This file contains the source code for the Sample STF1 App.
**
*******************************************************************************/

/*
**   Include Files:
*/
#include "sample_stf1_app.h"
#include "sample_stf1_app_perfids.h"
#include "sample_stf1_app_msgids.h"
#include "sample_stf1_app_msg.h"
#include "sample_stf1_app_events.h"
#include "sample_stf1_app_version.h"

/*
** global data
*/
SAMPLE_AppData_t SAMPLE_AppData;

static CFE_EVS_BinFilter_t  SAMPLE_EventFilters[] =
       {  /* Event ID    mask */
          {SAMPLE_STF1_STARTUP_INF_EID,       0x0000, 0},
          {SAMPLE_STF1_COMMAND_ERR_EID,       0x0000, 0},
          {SAMPLE_STF1_COMMANDNOP_INF_EID,    0x0000, 0},
          {SAMPLE_STF1_COMMANDRST_INF_EID,    0x0000, 0},
       };

/*
** SAMPLE_AppMain() -- Application entry point and main process loop
*/
CFS_MODULE_DECLARE_APP(sample, 255, 8192);

void sample_Main( void )
{
    int32  status    = 0;
    SAMPLE_AppData.RunStatus = CFE_ES_APP_RUN;
    CFE_ES_PerfLogEntry(SAMPLE_STF1_APP_PERF_ID);

    /* 
    ** initialize the application, register the app, etc 
    */
    SAMPLE_AppInit();

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&SAMPLE_AppData.RunStatus) == TRUE)
    {
        /*
        ** Exit performance profiling.  It will be restarted later in this while loop. 
        */
        CFE_ES_PerfLogExit(SAMPLE_STF1_APP_PERF_ID);

        /* 
        ** Pend on receipt of command packet -- set timeout to 500ms as cFE default
        ** could also set no timeout - this means that this app
        ** will block until a message is received.  Refer to the header file docs
        ** for more information on using this function
        */
        status = CFE_SB_RcvMsg(&SAMPLE_AppData.MsgPtr, SAMPLE_AppData.CmdPipe, 500);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution
        */
        CFE_ES_PerfLogEntry(SAMPLE_STF1_APP_PERF_ID);

        /*
        ** If the RcvMsg() was successful, then continue to process the CommandPacket()
        ** if not successful, then 
        */
        if (status == CFE_SUCCESS)
        {
            SAMPLE_ProcessCommandPacket();
        }
        else if (status == CFE_SB_PIPE_RD_ERR)
        {
            /* This is an example of exiting on an error.
            ** Note that a SB read error is not always going to
            ** result in an app quitting.
            */
            CFE_EVS_SendEvent(SAMPLE_STF1_PIPE_ERR_EID, CFE_EVS_ERROR, "SAMPLE STF1 APP: SB Pipe Read Error, SAMPLE STF1 APP Will Exit with error = %d", status);
            SAMPLE_AppData.RunStatus = CFE_ES_APP_ERROR;
        }

    }

    CFE_ES_ExitApp(SAMPLE_AppData.RunStatus);
} 



/* 
** SAMPLE_AppInit() --  initialization
*/
void SAMPLE_AppInit(void)
{

    int32 status;

    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(SAMPLE_EventFilters,
                     sizeof(SAMPLE_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_NO_FILTER);    /* as default, no filters are used */

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&SAMPLE_AppData.CmdPipe, SAMPLE_PIPE_DEPTH, "SAMPLE_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        OS_printf("SAMPLE STF APP: CFE_SB_CreatePipe error!\n");
    }
    
    /*
    ** Subscribe to "ground commands". Ground commands are those commands with command codes
    */
    status = CFE_SB_Subscribe(SAMPLE_STF1_APP_CMD_MID, SAMPLE_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        OS_printf("SAMPLE STF APP: CFE_SB_Subscribe error (id=%i)!\n", SAMPLE_STF1_APP_CMD_MID);
    }

    /*
    ** Subscribe to housekeeping (hk) messages.  HK messages are those messages that request
    ** an app to send its HK telemetry
    */
    status = CFE_SB_Subscribe(SAMPLE_STF1_APP_SEND_HK_MID, SAMPLE_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        OS_printf("SAMPLE STF APP: CFE_SB_Subscribe error (id=%i)!\n", SAMPLE_STF1_APP_SEND_HK_MID);
    }

    /*
    ** todo - subscribe to any other messages here - these are probably going to 
    **        be messages that are published from other apps that this app will
    **        need to perform a specific task
    */

    
    /* on app init, always reset all counters */
    SAMPLE_ResetCounters();

    /* Initialize the published HK message - this HK message will contain the telemetry
    ** that has been defined in the SAMPLE_HkTelemetryPkt for this app
    */
    CFE_SB_InitMsg(&SAMPLE_AppData.HkTelemetryPkt,
                   SAMPLE_STF1_APP_HK_TLM_MID,
                   SAMPLE_STF1_APP_HK_TLM_LNGTH, TRUE);

    /*
    ** todo - initialize any other messages that this app will publish.  The cFS "way", is to 
    **        mainly use the app's HK message to push telemetry and data onto the Software Bus (SB)
    */


    /* 
     ** Important to send an information event that the app has initialized. this is
     ** useful for debugging the loading of individual apps
     */
    CFE_EVS_SendEvent (SAMPLE_STF1_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "SAMPLE STF1 App Initialized. Version %d.%d.%d.%d",
                SAMPLE_STF1_APP_MAJOR_VERSION,
                SAMPLE_STF1_APP_MINOR_VERSION, 
                SAMPLE_STF1_APP_REVISION, 
                SAMPLE_STF1_APP_MISSION_REV);	
} 


/* 
**  Name:  SAMPLE_ProcessCommandPacket
**
**  Purpose:
**  This routine will process any packet that is received on the SAMPLE command pipe.       
*/
void SAMPLE_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId = CFE_SB_GetMsgId(SAMPLE_AppData.MsgPtr);
    switch (MsgId)
    {
        /*
        ** Ground Commands with command codes fall under the SAMPLE_STF1_APP_CMD_MID
        ** message ID
        */
        case SAMPLE_STF1_APP_CMD_MID:
            SAMPLE_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        ** The HK MID comes first, as it is currently the only other messages defined
        ** besides the SAMPLE_STF1_APP_CMD_MID message above
        */
        case SAMPLE_STF1_APP_SEND_HK_MID:
            SAMPLE_ReportHousekeeping();
            break;

         /*
         ** All other invalid messages that this app doesn't recognize, increment
         ** the command error counter and log as an error event.  
         */
        default:
            SAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(SAMPLE_STF1_COMMAND_ERR_EID,CFE_EVS_ERROR, "SAMPLE STF1 App: invalid command packet, MID = 0x%x", MsgId);
            break;
    }

    return;
} 


/*
** SAMPLE_ProcessGroundCommand() -- SAMPLE ground commands
*/
void SAMPLE_ProcessGroundCommand(void)
{
    /*
    ** MsgId is only needed if the command code is not recognized. See default case below 
    */
    CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(SAMPLE_AppData.MsgPtr);   

    /*
    ** Ground Commands, by definition, has a command code associated with them.  Pull
    ** this command code from the message and then process the action associated with
    ** the command code.
    */
    uint16 CommandCode = CFE_SB_GetCmdCode(SAMPLE_AppData.MsgPtr);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case SAMPLE_APP_NOOP_CC:
            /* 
            ** notice the usage of the VerifyCmdLength() function call to verify that
            ** the command length is as expected.  
            */
            if (SAMPLE_VerifyCmdLength(SAMPLE_AppData.MsgPtr, sizeof(SAMPLE_NoArgsCmd_t)))
            {
                SAMPLE_AppData.HkTelemetryPkt.CommandCount++;
                CFE_EVS_SendEvent(SAMPLE_STF1_COMMANDNOP_INF_EID, CFE_EVS_INFORMATION, "SAMPLE STF1 App: NOOP command");
            }
            break;

        /*
        ** Reset Counters Command
        */
        case SAMPLE_APP_RESET_COUNTERS_CC:
            SAMPLE_ResetCounters();
            break;

        /*
        ** todo - add more command codes are appropriate for the app
        **        
        */


        /*
        ** Invalid Command Codes
        */
        default:
            SAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(SAMPLE_STF1_COMMAND_ERR_EID, CFE_EVS_ERROR, 
                "SAMPLE STF1 App: invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", MsgId, CommandCode);
            break;
    }
    return;
} 

/* 
**  Name:  SAMPLE_ReportHousekeeping                                             
**                                                                            
**  Purpose:                                                                  
**         This function is triggered in response to a task telemetry request 
**         from the housekeeping task. This function will gather the Apps     
**         telemetry, packetize it and send it to the housekeeping task via   
**         the software bus                                                   
*/
void SAMPLE_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &SAMPLE_AppData.HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &SAMPLE_AppData.HkTelemetryPkt);
    return;
} 

/*
**  Name:  SAMPLE_ResetCounters                                               
**                                                                            
**  Purpose:                                                                  
**         This function resets all the global counter variables that are    
**         part of the task telemetry.                                        
*/
void SAMPLE_ResetCounters(void)
{
    /* Status of commands processed by the SAMPLE App */
    SAMPLE_AppData.HkTelemetryPkt.CommandCount       = 0;
    SAMPLE_AppData.HkTelemetryPkt.CommandErrorCount  = 0;
    CFE_EVS_SendEvent(SAMPLE_STF1_COMMANDRST_INF_EID, CFE_EVS_INFORMATION, "SAMPLE STF1 App: RESET Counters Command");
    return;
} 

/*
** SAMPLE_VerifyCmdLength() -- Verify command packet length                                                                                              
*/
boolean SAMPLE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(SAMPLE_STF1_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);

        result = FALSE;
        SAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;
    }

    return(result);
} 

