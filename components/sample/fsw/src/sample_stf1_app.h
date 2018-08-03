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
** File: sample_stf1_app.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE STF1 application.
**
** Notes:
**
*******************************************************************************/
#ifndef _SAMPLE_STF1_APP_H_
#define _SAMPLE_STF1_APP_H_

/*
** Required header files.
*/
#include "cfe.h"
#include "sample_stf1_app_msg.h"

/*
** Optional Header Files - May be target dependent
*/
#ifdef __linux__
/* intended for 32bit/64bit intel x86 linux */
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
#endif

/*
** Macros - Put any custom app-specific macros here
*/

/*
** This is the specified pipe depth for the app, meaning how many messages will be
**   queued in the pipe.  32 is the cFE default 
*/
#define SAMPLE_PIPE_DEPTH   32

/* Follow cFE convention and define a success macro that is for the usage of the VerifyCmdLength() function */
#define SAMPLE_SUCCESS 0

/*
** Type Definitions - Put any type definitions here
*/


/**
**  SAMPLE global data structure
*/

/* 
** The cFE convention is to put all global app data in a single struct. 
** This struct would be defined in the sample_stf1_app.h file. You would then have one global instance 
** in the.c file. You would also extern the global struct in the header file. This is important for 
** the Unit Test Framework(UTF). When using the UTF, you need access to the app global data so 
** it's a good idea to play along. 
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet...each app defines its own packet which contains
    ** its OWN telemetry that you want to be sent
    */
    SAMPLE_Hk_tlm_t   HkTelemetryPkt;   /* SAMPLE Housekeeping Telemetry Packet */
    

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_MsgPtr_t MsgPtr;     /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;    /* Pipe Id for HK command pipe */
    uint32 RunStatus;           /* App run status for controlling the application state */
} SAMPLE_AppData_t;


/*
** Exported Data
*/
extern SAMPLE_AppData_t SAMPLE_AppData; /* SAMPLE App Data */



/*
**
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void SAMPLE_AppMain(void);
void SAMPLE_AppInit(void);
void SAMPLE_ProcessCommandPacket(void);
void SAMPLE_ProcessGroundCommand(void);
void SAMPLE_ReportHousekeeping(void);
void SAMPLE_ResetCounters(void);

/* 
** This function is provided as an example of verifying the size of the command
*/
boolean SAMPLE_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif 
