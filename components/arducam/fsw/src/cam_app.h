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

#ifndef _CAM_H_
#define _CAM_H_

/*
** Required header files.
*/
#include "cfe.h"
#include "cam_perfids.h"
#include "cam_msgids.h"
#include "cam_msg.h"
#include "cam_events.h"
#include "cam_version.h"
#include "cam_child.h"
#include "cam_platform_cfg.h"
#include "hwlib.h"

/*
** This is the specified pipe depth for the app, meaning how many messages will be
**   queued in the pipe.  32 is the cFE default 
*/
#define CAM_PIPE_DEPTH  32

/* 
** The cFE convention is to put all global app data in a single struct. 
** This struct would be defined in the cam_app.h file. You would then have one global instance 
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
    CAM_Hk_tlm_t   HkTelemetryPkt;   /* CAM Housekeeping Telemetry Packet */
    
    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_MsgPtr_t MsgPtr;     /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;    /* Pipe Id for HK command pipe */
    uint32 RunStatus;           /* App run status for controlling the application state */
	CAM_NoArgsCmd_t EoE;        /* End of Experiment Packet */
    CAM_Exp_tlm_t	Exp_Pkt;    /* Experiment Packet */
	
	/*
	** Child data 
	*/
	uint32   ChildTaskID;		/* Task ID provided by CFS on initialization */
	uint32   data_mutex;         
    uint32   sem_id;            /* Semaphore ID */
    uint32   Exp;
    uint32   State; 				
	uint32   Size;				/* Resolution of picture */	
} CAM_AppData_t;


/*
** Exported Data
*/
extern CAM_AppData_t CAM_AppData; /* CAM App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (CAM_AppMain), these
**       functions are not called from any other source module.
*/
//void CAM_AppMain(void);
int32 CAM_AppInit(void);
void  CAM_ProcessCommandPacket(void);
void  CAM_ProcessGroundCommand(void);
void  CAM_ReportHousekeeping(void);
void  CAM_ProcessPR(void);
void  CAM_ResetCounters(void);

/* 
** This function is provided as an example of verifying the size of the command
*/
boolean CAM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif 
