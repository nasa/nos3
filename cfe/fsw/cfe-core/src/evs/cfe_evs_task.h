/*
**
**  Filename: cfe_evs_task.h
**  $Id: cfe_evs_task.h 1.4 2012/01/13 12:00:55GMT-05:00 acudmore Exp  $
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
**  Title:    Event Services API - Management Control Interfaces.
**
**  Purpose:
**            Unit specification for the event services management control interfaces.
**
**  Contents:
**       I.  macro and constant type definitions
**      II.  EVS internal structures
**     III.  function prototypes
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**
**  $Date: 2012/01/13 12:00:55GMT-05:00 $
**  $Revision: 1.4 $
**  $Log: cfe_evs_task.h  $
**  Revision 1.4 2012/01/13 12:00:55GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.3 2011/04/05 16:33:45EDT lwalling 
**  Optimize EVS use of string functions, plus other performance improvements
**  Revision 1.2 2010/10/04 17:07:45EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:05:14EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
**  Revision 1.46 2007/07/18 14:09:55EDT njyanchik 
**  I moved: CFE_EVS_DEBUG_BIT, CFE_EVS_INFORMATION_BIT, CFE_EVS_ERROR_BIT, CFE_EVS_CRITICAL_BIT, CFE_EVS_PORT1_BIT, CFE_EVS_PORT2_BIT, CFE_EVS_PORT3_BIT, CFE_EVS_PORT4_BIT, CFE_EVS_SHORT_FORMAT, CFE_EVS_LONG_FORMAT as well as 
**  
**  CFE_EVS_LOG_OVERWRITE and
**  CFE_EVS_LOG_DISCARD
**  
**  to cfe_evs_msg.h  because they are needed for commands
**  Revision 1.45 2007/05/14 10:26:03EDT apcudmore 
**  Preserve the EVS Local event log on a processor restart:
**  - moved the EVS Log to the ES reset data structure
**  - Added a couple of EVS Log related variables into the EVS_Log_t structure
**  - Added a routine to the EVS early init to get a pointer to the EVS Log 
**  - Added code to the EVS housekeeping service routine to copy Log variables into TLM
**  Revision 1.44 2007/04/17 07:57:01EDT njyanchik 
**  Remove EVS main loop error delay #define
**  Revision 1.43 2007/04/13 16:23:17EDT apcudmore 
**  Changed EarlyInit function prototype to return int32 status
**  Revision 1.42 2007/04/11 10:59:55EDT njyanchik 
**  Remove the TaskDelay in the main loop for EVS. I also addeda counterin telemtry that will
**  notify the ground of an error in reading the command pipe.
**  Revision 1.41 2007/03/01 11:54:33EST njyanchik 
**  This cp handles issues 1,6,7,8 as described in the DCR
**  Revision 1.40 2007/02/08 15:23:37EST njyanchik 
**  I accidentally put a typo into the #error statement. I removed it.
**  Revision 1.39 2007/02/08 13:44:17GMT-05:00 njyanchik 
**  A compiler check was added to make sure OS_BUFFER_SIZE is not less than
**  CFE_EVS_MAX_PORT_MSG_LENGTH
**  Revision 1.38 2007/02/07 13:26:33GMT-05:00 njyanchik 
**  The CFE_EVS_Table had the correct number of binary filters in it CFE_EVS_MAX_EVENT_FILTERS (8). The AppDataFile_t was using an obsolete #define that wasset at 32. This caused the file to have 32 entries,
**  but only 8 of them were legit. The AppDataFile_t was changed to use CFE_EVS_MAX_EVENT_FILTERS numver of binary filters.
**  Revision 1.37 2007/02/07 08:50:04GMT-05:00 njyanchik 
**  I inserted padding into the CFE_EVS_AppTlmData_t
**  Revision 1.36 2006/10/26 08:27:10GMT-05:00 kkaudra 
**  Changed CFE_EVS_MAX_FILTER_COUNT from 255 to  65535
**/

#ifndef _cfe_evs_task_
#define _cfe_evs_task_

/********************************** Include Files  ************************************/
#include "private/cfe_private.h"
#include "private/cfe_evs_log_typedef.h"
#include "cfe_sb.h"            /* Software Bus library function definitions */
#include "cfe_msgids.h"        /* Software Bus Message ID definitions */
#include "cfe_es.h"            /* Memory Pool definitions */
#include "osapi.h"             /* OS definitions */
#include "cfe_evs_msg.h"        /* EVS message definitions */
#include "cfe_evs_verify.h"
#include "cfe_evs.h"
#include "cfe_evs_events.h"

/*********************  Macro and Constant Type Definitions   ***************************/

#define CFE_EVS_MSG_TRUNCATED           '$'
#define CFE_EVS_FREE_SLOT               (-1)
#define CFE_EVS_NO_MASK                 0
#define CFE_EVS_PIPE_DEPTH              32
#define CFE_EVS_MSG_LIMIT               4
#define CFE_EVS_MAX_EVENT_SEND_COUNT    65535
#define CFE_EVS_MAX_FILTER_COUNT        65535
#define CFE_EVS_PIPE_NAME               "EVS_CMD_PIPE"
#define CFE_EVS_UNDEF_APPID             0xFFFFFFFF
#define CFE_EVS_MAX_PORT_MSG_LENGTH     (CFE_EVS_MAX_MESSAGE_LENGTH+OS_MAX_API_NAME+30)

/* Since CFE_EVS_MAX_PORT_MSG_LENGTH is the size of the buffer that is sent to 
 * print out (using OS_printf), we need to check to make sure that the buffer 
 * size the OS uses is big enough. This check has to be made here because it is
 * the first spot after CFE_EVS_MAX_PORT_MSG_LENGTH is defined */
#if OS_BUFFER_SIZE < CFE_EVS_MAX_PORT_MSG_LENGTH 
    #error CFE_EVS_MAX_PORT_MSG_LENGTH cannot be greater than OS_BUFFER_SIZE!
#endif


/************************  Internal Structure Definitions  *****************************/

typedef struct
{
   int16               EventID;     /* Numerical event identifier */
   uint16              Mask;        /* Binary filter mask */
   uint16              Count;       /* Binary filter counter */
   uint16              Padding;     /* Structure padding */
   uint32              lastReceived;
   uint32              burstCount;
   uint32              throttleCount;
   uint16              RateLimit;
   uint16              BurstLimit;

} EVS_BinFilter_t;


typedef struct
{
    EVS_BinFilter_t    BinFilters[CFE_EVS_MAX_EVENT_FILTERS];  /* Array of binary filters */

    uint8              ActiveFlag;             /* Application event service active flag */
    uint8              EventTypesActiveFlag;   /* Application event types active flag */
    uint16             EventCount;             /* Application event counter */
    uint16             RegisterFlag;           /* Application has registered flag */

} EVS_AppData_t;


typedef struct {
   char                AppName[OS_MAX_API_NAME];               /* Application name */
   uint8               ActiveFlag;                             /* Application event service active flag */
   uint8               EventTypesActiveFlag;                   /* Application event types active flag */
   uint16              EventCount;                             /* Application event counter */
   EVS_BinFilter_t     Filters[CFE_EVS_MAX_EVENT_FILTERS];     /* Application event filters */

} CFE_EVS_AppDataFile_t;


/* Global data structure */
typedef struct
{
   EVS_AppData_t       AppData[CFE_ES_MAX_APPLICATIONS];    /* Application state data and event filters */

   CFE_EVS_Log_t      *EVS_LogPtr;    /* Pointer to the EVS log in the ES Reset area*/
                                         /* see cfe_es_global.h */
                                         
   /*
   ** EVS task data
   */
   CFE_EVS_TlmPkt_t    EVS_TlmPkt;
   CFE_SB_PipeId_t     EVS_CommandPipe;
   uint32              EVS_SharedDataMutexID;
   uint32              EVS_AppID;

} CFE_EVS_GlobalData_t;

/*
 *  Global variable specific to EVS module
 */
extern CFE_EVS_GlobalData_t   CFE_EVS_GlobalData;


/*****************************  Function Prototypes   **********************************/

/*
* Functions used within this module and by the unit test
*/
extern int32 CFE_EVS_TaskInit (void);
extern void  CFE_EVS_ProcessCommandPacket ( CFE_SB_MsgPtr_t EVS_MsgPtr );

boolean CFE_EVS_SetFilterMaskCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_EnablePortsCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_DisablePortsCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_EnableEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_DisableEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_SetEventFormatModeCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_EnableAppEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_DisableAppEventTypesCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_EnableAppEventsCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_DisableAppEventsCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_ResetAppEventCounterCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_ResetFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_AddEventFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_DeleteEventFilterCmd (const CFE_SB_MsgPayloadPtr_t Payload);
boolean CFE_EVS_WriteAppDataCmd ( const CFE_SB_MsgPayloadPtr_t Payload );
boolean CFE_EVS_ResetAllFiltersCmd (const CFE_SB_MsgPayloadPtr_t Payload);


#endif  /* _cfe_evs_task_ */
