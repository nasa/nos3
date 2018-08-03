/*
**  File: 
**  cfe_es_apps.h
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
**  This file contains the Internal interface for the cFE Application control functions of ES.
**  These functions and data structures manage the Applications and Child tasks in the cFE.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  $Log: cfe_es_apps.h  $
**  Revision 1.6 2012/01/13 11:49:59GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.5 2008/07/31 15:41:32EDT apcudmore 
**  Added execution counter API:
**    -- Added execution counter to ES internal task data
**    -- Added code to increment counter in RunLoop
**    -- Added code to report counter in GetInfo APIs
**    -- Added API to increment counter for child tasks
**    -- Added code to cFE Core apps to increment counters.
**  Revision 1.4 2008/07/30 14:52:32EDT apcudmore 
**  Added startup sync support and API
**  Revision 1.3 2008/06/26 15:41:52EDT apcudmore 
**  Added public CFE_ES_GetAppInfo API.
**  Moved the old API to CFE_ES_GetAppInfoInternal
**  -- May merge these two in the near future.
**  Revision 1.2 2008/06/20 15:40:29EDT apcudmore 
**  Added support for OSAL based module loader
**   - Removed calls and references to the BSP based module loader
**  Revision 1.1 2008/04/17 08:05:03EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.18 2007/05/25 11:55:43EDT apcudmore 
**  Error Handling updates
**     - CFE_ES_GetAppInfo returns void
**     - added app ID range check in RestartApp
**   
**  Revision 1.17 2007/05/22 14:56:52EDT apcudmore 
**  cFE Non-Volatile Disk/Startup File changes:
**    - cFE startup now passes in startup file path/name
**    - cFE startup code does not init non-volatile disk ( BSP does )
**    - BSPs now can map cFE paths without making the OS_initfs/OS_mount calls
**    - MCP750 vxWorks6.2 BSP inits/mounts CF disk
**    - Desktop ports pass in the same startup file path/name as before
**  Revision 1.16 2007/04/25 11:57:05EDT apcudmore 
**  Implemented "Back End" of Application Restart and Delete.
**  Changes include all ES Cleanup code to delete App and Task Resources.
**  Revision 1.15 2007/04/03 16:49:24EDT apcudmore 
**  Moved all command code defines, command , and telemetry definitions to cfe_es_msg.h
**  Revision 1.14 2007/03/23 15:06:26EST apcudmore 
**  Fixes for DCR 470: Improved error handling for CFE_ES_RegisterApp and CFE_ES_RegisterChildTask.
**                              Removed circular include.
**  Revision 1.13 2007/03/23 10:47:23EST apcudmore 
**  Added CFE_ES_GetTaskInfo call and corresponding CFE_ES_TaskInfo_t data structure.
**  Revision 1.12 2007/03/22 10:26:22EST apcudmore 
**  Implemented all code walkthrough items related to ES Application logic, commands, and telemetry.
**  Revision 1.11 2007/03/12 15:38:28EST apcudmore 
**  More updates for ES Startup code walkthrough items.
*/


#ifndef _cfe_es_apps_
#define _cfe_es_apps_

/*
** Include Files
*/
#include "common_types.h"
#include "osapi.h"

/*
** Macro Definitions
*/

/*
** The overall cFE System State
*/
#define CFE_ES_SYSTEM_STATE_UNDEFINED       0   /**< reserved */
#define CFE_ES_SYSTEM_STATE_EARLY_INIT      1   /**< single threaded mode while setting up CFE itself */
#define CFE_ES_SYSTEM_STATE_CORE_STARTUP    2   /**< core apps (CFE_ES_ObjectTable) are starting (multi-threaded) */
#define CFE_ES_SYSTEM_STATE_CORE_READY      3   /**< core is ready, starting other external apps/libraries (if any) */
#define CFE_ES_SYSTEM_STATE_OPERATIONAL     4   /**< normal operation mode */
#define CFE_ES_SYSTEM_STATE_SHUTTING_DOWN   5   /**< reserved for future use */

/*
** The Type of cFE Application
*/
#define CFE_ES_APP_TYPE_CORE       1
#define CFE_ES_APP_TYPE_EXTERNAL   2

/*
** Application Run-State defines
*/
#define CFE_ES_APP_STATE_RUNNING      1
#define CFE_ES_APP_STATE_STOPPED      2
#define CFE_ES_APP_STATE_WAITING      3
#define CFE_ES_APP_STATE_INITIALIZING 4

/*
** Application exception action defines
*/
#define CFE_ES_APP_EXCEPTION_RESTART_APP  0
#define CFE_ES_APP_EXCEPTION_PROC_RESTART 1

/*
** Type Definitions
*/

/*
** CFE_ES_AppState_t is a structure of information for External cFE Apps.
**   This information is used to control/alter the state of External Apps.
**   The fields in this structure are not needed or used for the cFE Core Apps.
*/
typedef struct
{
   uint32     AppControlRequest;              /* What the App should be doing next */
   uint32     AppState;                       /* Is the app running, or stopped, or waiting? */
   int32      AppTimer;                       /* Countdown timer for killing an app */

} CFE_ES_AppState_t;


/*
** CFE_ES_AppStartParams_t is a structure of information used when an application is
**   created in the system. It is stored in the cFE ES App Table
*/
typedef struct
{
  char                  Name[OS_MAX_API_NAME];
  char                  EntryPoint[OS_MAX_API_NAME];
  char                  FileName[OS_MAX_PATH_LEN];

  uint32                StackSize;
  cpuaddr               StartAddress;
  uint32                ModuleId;

  uint16                ExceptionAction;
  uint16                Priority;
   
} CFE_ES_AppStartParams_t;

/*
** CFE_ES_MainTaskInfo_t is a structure of information about the main
** task and child tasks in a cFE application. This structure is just used in the
** cFE_ES_AppRecord_t structure. 
*/
typedef struct
{
   uint32   MainTaskId;                     /* The Application's Main Task ID */
   char     MainTaskName[OS_MAX_API_NAME];  /* The Application's Main Task ID */
   uint32   NumOfChildTasks;                /* Number of Child tasks for an App */

} CFE_ES_MainTaskInfo_t;


/*
** CFE_ES_AppRecord_t is an internal structure used to keep track of
** CFE Applications that are active in the system.
*/
typedef struct
{
   boolean                 RecordUsed;                  /* Is the record used(1) or available(0) ? */
   uint32                  Type;                        /* The type of App: CORE or EXTERNAL */
   CFE_ES_AppStartParams_t StartParams;                 /* The start parameters for an App */
   CFE_ES_AppState_t       StateRecord;                 /* The State info for External cFE Apps */
   CFE_ES_MainTaskInfo_t   TaskInfo;                    /* Information about the Tasks */
      
} CFE_ES_AppRecord_t;


/*
** CFE_ES_TaskRecord_t is an internal structure used to keep track of
** CFE Tasks that are active in the system.
*/
typedef struct
{
   boolean   RecordUsed;                      /* Is the record used(1) or available(0) */
   uint32    AppId;                           /* The parent Application's App ID */
   uint32    TaskId;                          /* Task ID */
   uint32    ExecutionCounter;                /* The execution counter for the Child task */
   char      TaskName[OS_MAX_API_NAME];       /* Task Name */
   
   
} CFE_ES_TaskRecord_t;

/*
** CFE_ES_LibRecord_t is an internal structure used to keep track of
** CFE Shared Libraries that are loaded in the system.
*/
typedef struct
{
   boolean   RecordUsed;                      /* Is the record used(1) or available(0) */
   char      LibName[OS_MAX_API_NAME];        /* Library Name */
} CFE_ES_LibRecord_t;

/*****************************************************************************/
/*
** Function prototypes
*/

/*
** Internal function start applications based on the startup script
*/
void  CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath );

/*
** Internal function to parse/execute a line of the cFE application startup 'script'
*/
int32 CFE_ES_ParseFileEntry(const char *FileEntry);

/*
 * Internal function to synchronize application startup
 */
int32 CFE_ES_ApplicationSyncDelay(uint32 MinimumSystemState, uint32 TimeOutMilliseconds);

/*
** Internal function to create/start a new cFE app
** based on the parameters passed in
*/
int32 CFE_ES_AppCreate(uint32 *ApplicationIdPtr,
                       const char   *FileName,
                       const void   *EntryPointData,
                       const char   *AppName,
                       uint32  Priority,
                       uint32  StackSize,
                       uint32  ExceptionAction);
/*
** Internal function to load a a new cFE shared Library
*/
int32 CFE_ES_LoadLibrary(uint32 *LibraryIdPtr,
                       const char   *Path,
                       const void   *EntryPointData,
                       const char   *Name);

/*
** Get Application List
*/
int32 CFE_ES_AppGetList(uint32 AppIdArray[], uint32 ArraySize);

/*
** Dump All application properties to a file
**  Note: Do we want to specify a file here?
*/
int32 CFE_ES_AppDumpAllInfo(void);

/*
** Scan the Application Table for actions to take
*/
void CFE_ES_ScanAppTable(void);

/*
** Perform the requested control action for an application
*/
void CFE_ES_ProcessControlRequest(uint32 AppID);

/*
** Clean up all app resources and delete it
*/
int32 CFE_ES_CleanUpApp(uint32 AppId);

/*
** Clean up all Task resources and detete the task
*/
int32 CFE_ES_CleanupTaskResources(uint32 TaskId);

/*
** Debug function to print out resource utilization 
*/
int32 CFE_ES_ListResourcesDebug(void);

/*
** Populate the cFE_ES_AppInfo structure with the data for an app
** This is an internal function for use in ES.
** The newer external API is : CFE_ES_GetAppInfo
*/
void CFE_ES_GetAppInfoInternal(uint32 AppId, CFE_ES_AppInfo_t *AppInfoPtr );

#endif  /* _cfe_es_apps_ */
