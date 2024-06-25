/*******************************************************************************
** File: newsim_app.h
**
** Purpose:
**   This is the main header file for the NEWSIM application.
**
*******************************************************************************/
#ifndef _NEWSIM_APP_H_
#define _NEWSIM_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "newsim_device.h"
#include "newsim_events.h"
#include "newsim_platform_cfg.h"
#include "newsim_perfids.h"
#include "newsim_msg.h"
#include "newsim_msgids.h"
#include "newsim_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define NEWSIM_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define NEWSIM_DEVICE_DISABLED       0
#define NEWSIM_DEVICE_ENABLED        1


/*
** NEWSIM global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `newsim_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    NEWSIM_Hk_tlm_t   HkTelemetryPkt;   /* NEWSIM Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
    ** TODO: Make specific to your application
	*/
    NEWSIM_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t NewsimUart;             /* Hardware protocol definition */

} NEWSIM_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern NEWSIM_AppData_t NEWSIM_AppData; /* NEWSIM App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (NEWSIM_AppMain), these
**       functions are not called from any other source module.
*/
void  NEWSIM_AppMain(void);
int32 NEWSIM_AppInit(void);
void  NEWSIM_ProcessCommandPacket(void);
void  NEWSIM_ProcessGroundCommand(void);
void  NEWSIM_ProcessTelemetryRequest(void);
void  NEWSIM_ReportHousekeeping(void);
void  NEWSIM_ReportDeviceTelemetry(void);
void  NEWSIM_ResetCounters(void);
void  NEWSIM_Enable(void);
void  NEWSIM_Disable(void);
int32 NEWSIM_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _NEWSIM_APP_H_ */
