/*
**  File: 
**  cfe_es_task.h
**  $Id: cfe_es_task.h 1.3 2012/01/13 11:50:06GMT-05:00 acudmore Exp  $
**
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
**  Purpose:
**  cFE Executive Services (ES) task header file
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**
** $Log: cfe_es_task.h  $
** Revision 1.3 2012/01/13 11:50:06GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.2 2009/07/28 16:40:58EDT jmdagost 
** Deleted prototype for no-longer-used ComputeChecksum function.
** Revision 1.1 2008/04/17 08:05:10EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.55 2007/09/20 10:52:56EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.54 2007/05/14 15:49:27EDT dlkobe 
** Added missing prototype for CFE_ES_DumpCDSRegCmd
** Revision 1.53 2007/05/07 11:59:38EDT apcudmore 
** Removed SB Pipe error telemetry point
** Handle SB error with sys log message
** Revision 1.52 2007/05/04 20:22:23EDT dlkobe 
** Corrected prototype errors and added missing event message IDs
** Revision 1.51 2007/05/04 20:07:51EDT dlkobe 
** Added command to telemeter memory pool statistics
** Revision 1.50 2007/04/25 11:57:04EDT apcudmore 
** Implemented "Back End" of Application Restart and Delete.
** Changes include all ES Cleanup code to delete App and Task Resources.
** Revision 1.49 2007/04/17 11:01:33EDT rjmcgraw 
** Removed function proto InvalidMarkerReported
** Revision 1.48 2007/04/12 10:37:19EDT apcudmore 
** Added support for ES task to call CFE_ES_ExitApp on initialization and runtime failure.
** Revision 1.46 2007/04/10 13:59:29EDT rjmcgraw 
** Moved prototype of PerfLogAdd to cfe_es.h
** Revision 1.45 2007/04/05 16:12:14EDT apcudmore 
** Changed ES Task main loop to NOT delay after a software bus read error. Instead ES 
** will increment a counter in telemetry.
** Revision 1.44 2007/04/03 16:49:23EDT apcudmore 
** Moved all command code defines, command , and telemetry definitions to cfe_es_msg.h
** Revision 1.43 2007/04/03 13:14:19EDT apcudmore 
** Changed ES subtypes to use the defines in cfe_fs.h. Also Removed the defines for 
** subtypes in cfe_es_task.h
** Revision 1.42 2007/04/02 11:06:44EDT rjmcgraw 
** Added CFE_ES_PERF_LOG_DESC and added prototype for PerfLogAdd
** Revision 1.41 2007/03/29 15:51:18EST rjmcgraw 
** Changed name of performance prototypes and removed extern keyword
** Revision 1.40 2007/03/29 13:29:16EST apcudmore 
** Updated global variables and housekeeping to count:
** - Registered Core Apps
** - Registered External Apps
** - ALL Tasks 
**
*/
/*************************************************************************/
#ifndef _cfe_es_task_
#define _cfe_es_task_

/*
** Includes
*/
#include "cfe.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_events.h"
#include "cfe_es_msg.h"

/*************************************************************************/

/*
** ES File descriptions
*/
#define CFE_ES_SYS_LOG_DESC    "ES system log data file"
#define CFE_ES_TASK_LOG_DESC   "ES Task Info file"
#define CFE_ES_APP_LOG_DESC    "ES Application Info file"
#define CFE_ES_ER_LOG_DESC     "ES ERlog data file"
#define CFE_ES_PERF_LOG_DESC   "ES Performance data file"

/*************************************************************************/
/*
** Type definitions
*/

/*
** Type definition (ES task global data)
*/
typedef struct
{
  /*
  ** ES Task command interface counters
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

  /*
  ** ES Task housekeeping telemetry packet
  */
  CFE_ES_HkPacket_t     HkPacket;


  /*
  ** ES Shell output telemetry packet
  */
  CFE_ES_ShellPacket_t  ShellPacket;

  /*
  ** Single application telemetry packet
  */
  CFE_ES_OneAppTlm_t    OneAppPacket;

  /*
  ** Single application telemetry packet
  */
  CFE_ES_PoolStatsTlm_t MemStatsPacket;

  /*
  ** ES Task operational data (not reported in housekeeping)
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;
  
  /*
  ** ES Task initialization data (not reported in housekeeping)
  */
  char                  PipeName[OS_MAX_API_NAME];
  uint16                PipeDepth;

  uint8                 LimitHK;
  uint8                 LimitCmd;

} CFE_ES_TaskData_t;

/*
** Executive Services (ES) task global data.
*/
extern CFE_ES_TaskData_t CFE_ES_TaskData;



/*************************************************************************/

/*
** ES Task function prototypes
*/
void  CFE_ES_TaskMain(void);
int32 CFE_ES_TaskInit(void);
void  CFE_ES_TaskPipe(CFE_SB_MsgPtr_t Msg);

void CFE_ES_HousekeepingCmd(CFE_SB_MsgPtr_t Msg);

void CFE_ES_NoopCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ResetCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_RestartCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ShellCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_StartAppCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_StopAppCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_RestartAppCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ReloadAppCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_QueryOneCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_QueryAllCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_QueryAllTasksCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ClearSyslogCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_WriteSyslogCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_OverWriteSyslogCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ClearERlogCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_WriteERlogCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_ResetPRCountCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_SetMaxPRCountCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_DeleteCDSCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_PerfStartDataCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_PerfStopDataCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_PerfSetFilterMaskCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_PerfSetTriggerMaskCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_TlmPoolStatsCmd(CFE_SB_MsgPtr_t Msg);
void CFE_ES_DumpCDSRegCmd( const CFE_SB_Msg_t *MessagePtr );
boolean CFE_ES_ValidateHandle(CFE_ES_MemHandle_t  Handle);
boolean CFE_ES_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);
void CFE_ES_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual);

/*************************************************************************/

#endif /* _cfe_es_task_ */

/************************/
/*  End of File Comment */
/************************/

