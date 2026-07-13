/*******************************************************************************
** File: cfdp_app.h
**
** Purpose:
**   This is the main header file for the CFDP application.
**
*******************************************************************************/
#ifndef _CFDP_APP_H_
#define _CFDP_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "cfdp_events.h"
#include "cfdp_msg.h"
#include "cfdp_msgids.h"
#include "cfdp_version.h"
#include "cfdp_cfdp.h"


#define CFDP_SUCCESS            (0)  //codes for success and error
#define CFDP_ERROR              (-1)


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define CFDP_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define CFDP_DEVICE_DISABLED       0
#define CFDP_DEVICE_ENABLED        1


/*
** CFDP global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `cfdp_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    CFDP_Hk_tlm_t   hk;   /* CFDP Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    CFDP_CFDP_PDU_t fileInfo;

} CFDP_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern CFDP_AppData_t CFDP_AppData; /* CFDP App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (CFDP_AppMain), these
**       functions are not called from any other source module.
*/
void  CFDP_AppMain(void);
int32 CFDP_AppInit(void);
void  CFDP_ProcessCommandPacket(void);
void  CFDP_ProcessGroundCommand(void);
void  CFDP_ProcessTelemetryRequest(void);
void  CFDP_ReportHousekeeping(void);
int32 CFDP_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _CFDP_APP_H_ */
