/*

** $Id: cfe_time_utils.h 1.8 2014/07/07 10:22:52GMT-05:00 acudmore Exp  $
**
**
**      Copyright � 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA�s Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose:  cFE Time Services (TIME) library utilities header file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: cfe_time_utils.h  $
** Revision 1.8 2014/07/07 10:22:52GMT-05:00 acudmore 
** Added spare byte to TIME task global data structure for alignment.
** Revision 1.7 2014/04/14 10:51:41GMT-05:00 lwalling 
** Created platform config definitions for Time 1HZ and Time TONE task priorities and stack sizes
** Revision 1.6 2012/01/13 12:21:36EST acudmore 
** Changed license text to reflect open source
** Revision 1.5 2011/11/30 15:10:06EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.4 2011/01/18 16:05:53EST lwalling 
** Make sending 1hz command packet a configuration option
** Revision 1.3 2009/06/10 09:23:15EDT acudmore 
** Changed OS_Mem* and OS_BSP* calls to CFE_PSP_* calls
** Revision 1.2 2008/08/06 22:42:37EDT dkobe 
** Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:40EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/time/project.pj
** Revision 1.19 2007/04/27 12:31:05EDT njyanchik 
** I changed EVS and TIME to not have a telemetry counter for SB pipe reads in the main loop. 
** Instead, I have added a syslog message to state the problem
** Revision 1.18 2007/04/11 11:02:09EDT njyanchik 
** Remove the TaskDelay in the main loop for TIME. I also addeda counterin telemtry that will
** notify the ground of an error in reading the command pipe.
** Revision 1.17 2007/03/15 13:05:23EST njyanchik 
** I believe I removed a prototype accidentally for CFE_TIME_GetHKData when I modifed this file
** Revision 1.16 2007/03/07 12:03:37EST njyanchik 
** There were errors in the previous change package, so I fixed them
** Revision 1.14 2007/03/06 16:01:35EST apcudmore 
** ES Walkthrough changes related to cfe_es_global.h
** Revision 1.13 2006/12/20 10:36:30EST njyanchik 
** I changed the prototypes to int32, to match the functions
** Revision 1.12 2006/11/01 11:33:18GMT-05:00 njyanchik 
** This change removes any references to the CDS in TIME. TIME now uses  the ES
** Reset Area to store its critial variables. this was due to an issue of trying to call
** CDS functions before apps were registered (in the EarlyInit call).
** Revision 1.11 2006/10/30 14:47:47GMT-05:00 njyanchik 
** To get to the new OS_BSPGetTime function, os_bsp.h needs to be included
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_utils_
#define _cfe_time_utils_

/*
** Required header files...
*/
#include "cfe.h"
#include "cfe_time_verify.h"
#include "cfe_time_msg.h"
#include "cfe_time_events.h"
#include "cfe_psp.h"

/*************************************************************************/

#define CFE_TIME_NEGATIVE        0x80000000  /* ~ 68 * 31,536,000 seconds */

/*************************************************************************/

/*
** Main task definitions...
*/
#define CFE_TIME_TASK_NAME       "CFE_TIME"
#define CFE_TIME_RESET_SIGNATURE   0xA5A55A5A

/*
** Interrupt task definitions...
*/
#define CFE_TIME_TASK_TONE_NAME  "TIME_TONE_TASK"
#define CFE_TIME_TASK_1HZ_NAME   "TIME_1HZ_TASK"
#define CFE_TIME_TASK_STACK_PTR  0
#define CFE_TIME_TASK_FLAGS      0

/*
** Interrupt semaphore definitions...
*/
#define CFE_TIME_SEM_TONE_NAME   "TIME_TONE_SEM"
#define CFE_TIME_SEM_1HZ_NAME    "TIME_1HZ_SEM"
#define CFE_TIME_SEM_VALUE       0
#define CFE_TIME_SEM_OPTIONS     0

/*
** Main Task Pipe definitions...
*/

#define CFE_TIME_TASK_PIPE_NAME  "TIME_CMD_PIPE"
#define CFE_TIME_TASK_PIPE_DEPTH 12

/*
** Reset Area state state at startup...
*/

#define CFE_TIME_RESET_AREA_ERROR       1   /* no mem available */
#define CFE_TIME_RESET_AREA_BAD         2   /* had invalid data */
#define CFE_TIME_RESET_AREA_NEW         3   /* new memory block */
#define CFE_TIME_RESET_AREA_EXISTING    4   /* had valid data   */

/*************************************************************************/

/*
** Type definition (time reference data)...
*/
typedef struct
{

  CFE_TIME_SysTime_t    AtToneMET;      /* MET at time of tone */
  CFE_TIME_SysTime_t    AtToneSTCF;     /* STCF at time of tone */
  int16                 AtToneLeaps;    /* Leap Seconds at time of tone */
  int16                 ClockSetState;  /* Time has been "set" */
  int16                 ClockFlyState;  /* Current fly-wheel state */
  CFE_TIME_SysTime_t    AtToneDelay;    /* Adjustment for slow tone detection */
  CFE_TIME_SysTime_t    AtToneLatch;    /* Local clock latched at time of tone */
  CFE_TIME_SysTime_t    CurrentLatch;   /* Local clock latched just "now" */
  CFE_TIME_SysTime_t    TimeSinceTone;  /* Time elapsed since the tone */
  CFE_TIME_SysTime_t    CurrentMET;     /* MET at this instant */

} CFE_TIME_Reference_t;

/*
** Time Synchronization Callback Registry Information
*/
typedef struct
{
  uint32                         App;  /**< \brief Application ID associated with Callback Func */
  CFE_TIME_SynchCallbackPtr_t    Ptr;  /**< \brief Pointer to Callback function */
} CFE_TIME_SynchCallbackRegEntry_t;

/*************************************************************************/

/*
** Type definition (time task global data)...
*/
typedef struct
{
  /*
  ** Task command interface counters...
  */
  uint8                 CmdCounter;
  uint8                 ErrCounter;

  /*
  ** Task housekeeping and diagnostics telemetry packets...
  */
  CFE_TIME_HkPacket_t   HkPacket;
  CFE_TIME_DiagPacket_t DiagPacket;

  /*
  ** Task operational data (not reported in housekeeping)...
  */
  CFE_SB_MsgPtr_t       MsgPtr;
  CFE_SB_PipeId_t       CmdPipe;

  /*
  ** Task initialization data (not reported in housekeeping)...
  */
  char                  PipeName[16];
  uint16                PipeDepth;

  /*
  ** Data values used to compute time (in reference to "tone")...
  */
  CFE_TIME_SysTime_t    AtToneMET;
  CFE_TIME_SysTime_t    AtToneSTCF;
  int16                 AtToneLeaps;
  CFE_TIME_SysTime_t    AtToneDelay;
  CFE_TIME_SysTime_t    AtToneLatch;
  /*
  ** Data values used to define the current clock state...
  */
  int16                 ClockSetState;
  int16                 ClockFlyState;
  int16                 ClockSource;
  int16                 ClockSignal;
  int16                 ServerFlyState;

  /*
  ** Pending data values (from "time at tone" command data)...
  */
  CFE_TIME_SysTime_t    PendingMET;
  CFE_TIME_SysTime_t    PendingSTCF;
  int16                 PendingLeaps;
  int16                 PendingState;

  /*
  ** STCF adjustment values...
  */
  CFE_TIME_SysTime_t    OneTimeAdjust;
  CFE_TIME_SysTime_t    OneHzAdjust;

  int16                 OneTimeDirection; /* Add = TRUE */
  int16                 OneHzDirection;
  int16                 DelayDirection;

  /*
  ** Most recent local clock latch values...
  */
  CFE_TIME_SysTime_t    ToneSignalLatch;  /* Latched at tone */
  CFE_TIME_SysTime_t    ToneDataLatch;    /* Latched at packet */

  /*
  ** Miscellaneous counters...
  */
  uint32                ToneMatchCount;   /* Tone and data match */
  uint32                ToneMatchErrors;  /* Tone and data mismatch */
  uint32                ToneSignalCount;  /* Tone signal commands */
  uint32                ToneDataCount;    /* Tone data commands */
  uint32                ToneIntCount;     /* Tone interrupts (valid) */
  uint32                ToneIntErrors;    /* Tone interrupts (invalid) */
  uint32                ToneTaskCount;    /* Tone task wake-ups */
  uint32                VirtualMET;       /* Software MET */
  uint32                VersionCount;     /* Updates to "AtTone" values */
  uint32                LocalIntCount;    /* Local 1Hz interrupts */
  uint32                LocalTaskCount;   /* Local 1Hz task wake-ups */
  uint32                InternalCount;    /* Time from internal data */
  uint32                ExternalCount;    /* Time from external data */

  /*
  ** Time window verification values (converted from micro-secs)...
  **
  ** Regardless of whether the tone follows the time packet, or vice
  **    versa, these values define the acceptable window of time for
  **    the second event to follow the first.  The minimum value may
  **    be as little as zero, and the maximum must be something less
  **    than a second.
  */
  uint32                MinElapsed;
  uint32                MaxElapsed;

  /*
  ** Maximum local clock value (before roll-over)...
  */
  CFE_TIME_SysTime_t    MaxLocalClock;

  /*
  ** Clock state has been commanded into (CFE_TIME_FLYWHEEL)...
  */
  boolean               Forced2Fly;

  /*
  ** Clock state has just transitioned into (CFE_TIME_FLYWHEEL)...
  **   (not in HK since it won't be true long enough to detect)
  */

  boolean               AutoStartFly;
  boolean               IsToneGood;

  /*
  ** Spare byte for alignment 
  */
  boolean               Spare;

  /*
  ** Local 1Hz wake-up command packet (not related to time at tone)...
  */
  #if (CFE_TIME_ENA_1HZ_CMD_PKT == TRUE)
  CFE_TIME_1HzCmd_t     Local1HzCmd;
  #endif

  /*
  ** Time at the tone command packets (sent by time servers)...
  */
  CFE_TIME_FakeToneCmd_t FakeToneCmd;
  CFE_TIME_ToneDataCmd_t ToneDataCmd;
  CFE_TIME_ToneSignalCmd_t ToneSignalCmd;

  /*
  ** Interrupt task semaphores...
  */
  uint32                LocalSemaphore;
  uint32                ToneSemaphore;
  /*
  ** Interrupt task ID's...
  */
  uint32                LocalTaskID;
  uint32                ToneTaskID;

  /*
  ** Maximum difference from expected for external time sources...
  */

  CFE_TIME_SysTime_t    MaxDelta;

  /*
  ** Tone signal tolerance limits...
  */
  uint32                ToneOverLimit;
  uint32                ToneUnderLimit;

  /*
  ** Reset Area ...
  */
  uint32                DataStoreStatus;
  
  /*
  ** Synchronization Callback Registry
  */
  CFE_TIME_SynchCallbackRegEntry_t SynchCallback[CFE_TIME_MAX_NUM_SYNCH_FUNCS];

} CFE_TIME_TaskData_t;

/*
** Time task global data (from "cfe_time_task.c")...
*/
extern CFE_TIME_TaskData_t CFE_TIME_TaskData;


/*************************************************************************/

/*
** Macro to copy systime into another systime.
** Preferred to use this macro as it does not require the two arguments to be exactly the same type,
** it will work with any two structures that define "Seconds" and "Subseconds" members.
*/
#define CFE_TIME_Copy(m,t)   { (m)->Seconds = (t)->Seconds; (m)->Subseconds = (t)->Subseconds; }

/*
** Function prototypes (get local clock)...
*/
CFE_TIME_SysTime_t CFE_TIME_LatchClock(void);

/*
** Function prototypes (Time Services utilities data)...
*/
int32 CFE_TIME_TaskInit (void);
void  CFE_TIME_TaskPipe(CFE_SB_MsgPtr_t MessagePtr);
void CFE_TIME_InitData(void);
void CFE_TIME_QueryResetVars(void);
void CFE_TIME_UpdateResetVars(const CFE_TIME_Reference_t *Reference);
uint16 CFE_TIME_GetStateFlags(void);
void CFE_TIME_GetDiagData(void);
void CFE_TIME_GetHkData(const CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (reference)...
*/
void CFE_TIME_GetReference(CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (calculate TAI/UTC)...
*/
CFE_TIME_SysTime_t CFE_TIME_CalculateTAI(const CFE_TIME_Reference_t *Reference);
CFE_TIME_SysTime_t CFE_TIME_CalculateUTC(const CFE_TIME_Reference_t *Reference);

int16 CFE_TIME_CalculateState(const CFE_TIME_Reference_t *Reference);

/*
** Function prototypes (set time globals)...
*/
void CFE_TIME_SetState(int16 NewState);
#if (CFE_TIME_CFG_SOURCE == TRUE)
void CFE_TIME_SetSource(int16 NewSource);
#endif

#if (CFE_TIME_CFG_SIGNAL == TRUE)
void CFE_TIME_SetSignal(int16 NewSignal);
#endif

#if (CFE_TIME_CFG_CLIENT == TRUE)
void CFE_TIME_SetDelay(CFE_TIME_SysTime_t NewDelay, int16 Direction);
#endif
#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_SetTime(CFE_TIME_SysTime_t NewTime);
void CFE_TIME_SetMET(CFE_TIME_SysTime_t NewMET);
void CFE_TIME_SetSTCF(CFE_TIME_SysTime_t NewSTCF);
void CFE_TIME_SetLeaps(int16 NewLeaps);
void CFE_TIME_SetAdjust(CFE_TIME_SysTime_t NewAdjust, int16 Direction);
void CFE_TIME_Set1HzAdj(CFE_TIME_SysTime_t NewAdjust, int16 Direction);
#endif

/*
** Function prototypes (send time at tone data packet -- local MET)...
*/
#if (CFE_TIME_CFG_SERVER == TRUE)
void CFE_TIME_ToneSend(void); /* signal to send time at tone packet */
#endif

/*
** Function prototypes (send time at tone data packet -- external time)...
*/
#if (CFE_TIME_CFG_SRC_MET == TRUE)
int32 CFE_TIME_ToneSendMET(CFE_TIME_SysTime_t NewMET);
#endif

#if (CFE_TIME_CFG_SRC_GPS == TRUE)
int32 CFE_TIME_ToneSendGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);
#endif

#if (CFE_TIME_CFG_SRC_TIME == TRUE)
int32 CFE_TIME_ToneSendTime(CFE_TIME_SysTime_t NewTime);
#endif

/*
** Function prototypes (process time at the tone signal and data packet)...
*/
void CFE_TIME_ToneSignal(void);
void CFE_TIME_ToneData(CFE_TIME_ToneDataCmd_Payload_t *Packet);
void CFE_TIME_ToneVerify(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
void CFE_TIME_ToneUpdate(void);

/*
** Function prototypes (tone 1Hz interrupt)...
*/
void CFE_TIME_Tone1HzISR(void);
void CFE_TIME_Tone1HzTask(void);
void CFE_TIME_NotifyTimeSynchApps(void);

/*
** Function prototypes (local 1Hz interrupt)...
*/
void CFE_TIME_Local1HzISR(void);
void CFE_TIME_Local1HzTask(void);
void CFE_TIME_Local1HzTimerCallback(uint32 TimerId, void *Arg);


#endif /* _cfe_time_utils_ */

/************************/
/*  End of File Comment */
/************************/
