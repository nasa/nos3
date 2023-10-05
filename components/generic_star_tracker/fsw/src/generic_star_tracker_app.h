/*******************************************************************************
** File: generic_star_tracker_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_STAR_TRACKER application.
**
*******************************************************************************/
#ifndef _GENERIC_STAR_TRACKER_APP_H_
#define _GENERIC_STAR_TRACKER_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_star_tracker_device.h"
#include "generic_star_tracker_events.h"
#include "generic_star_tracker_platform_cfg.h"
#include "generic_star_tracker_perfids.h"
#include "generic_star_tracker_msg.h"
#include "generic_star_tracker_msgids.h"
#include "generic_star_tracker_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_STAR_TRACKER_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_STAR_TRACKER_DEVICE_DISABLED       0
#define GENERIC_STAR_TRACKER_DEVICE_ENABLED        1


/*
** GENERIC_STAR_TRACKER global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_star_tracker_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_STAR_TRACKER_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_STAR_TRACKER Housekeeping Telemetry Packet */
    
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
    GENERIC_STAR_TRACKER_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t Generic_star_trackerUart;             /* Hardware protocol definition */

} GENERIC_STAR_TRACKER_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_STAR_TRACKER_AppData_t GENERIC_STAR_TRACKER_AppData; /* GENERIC_STAR_TRACKER App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (GENERIC_STAR_TRACKER_AppMain), these
**       functions are not called from any other source module.
*/
void  ST_AppMain(void);
int32 GENERIC_STAR_TRACKER_AppInit(void);
void  GENERIC_STAR_TRACKER_ProcessCommandPacket(void);
void  GENERIC_STAR_TRACKER_ProcessGroundCommand(void);
void  GENERIC_STAR_TRACKER_ProcessTelemetryRequest(void);
void  GENERIC_STAR_TRACKER_ReportHousekeeping(void);
void  GENERIC_STAR_TRACKER_ReportDeviceTelemetry(void);
void  GENERIC_STAR_TRACKER_ResetCounters(void);
void  GENERIC_STAR_TRACKER_Enable(void);
void  GENERIC_STAR_TRACKER_Disable(void);
int32 GENERIC_STAR_TRACKER_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _GENERIC_STAR_TRACKER_APP_H_ */
