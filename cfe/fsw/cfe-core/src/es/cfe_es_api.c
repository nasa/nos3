/*
**  File:  
**    cfe_es_api.c
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
**  Purpose:  
**    This file implements the cFE Executive Services API functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
** $Log: cfe_es_api.c  $
** Revision 1.21 2014/09/05 11:37:51GMT-05:00 acudmore 
** Updated Syslog and ERlog text in CFE_ES_ResetCFE
** Revision 1.20 2014/08/22 15:50:05GMT-05:00 lwalling 
** Changed signed loop counters to unsigned
** Revision 1.19 2014/07/23 15:35:46EDT acudmore 
** Updated where the Processor Reset counter is incremented.
** Made the ER log entry text clearer.
** Revision 1.18 2014/05/05 14:44:53GMT-05:00 acudmore 
** fixed potential unititialized variable
** Revision 1.17 2014/05/05 14:03:39GMT-05:00 acudmore 
** Fixed syslog logic to check for invalid syslog mode and index due to corrupted global memory
** Revision 1.16 2012/04/20 15:21:32GMT-05:00 acudmore 
** Fixed reset subtype in CFE_ES_ResetCFE function.
** Revision 1.15 2012/01/13 11:49:59EST acudmore 
** Changed license text to reflect open source
** Revision 1.14 2012/01/11 18:51:33EST aschoeni 
** Removed child task exit message from system log
** Revision 1.13 2011/09/02 11:02:56EDT jmdagost 
** Added new-line charactesr to the end of sys log messages that were missing them.
** Revision 1.12 2011/07/15 14:28:52EDT lwalling 
** Removed const qualifier from CounterName argument to CFE_ES_RegisterGenCounter()
** Revision 1.11 2010/11/05 15:54:33EDT aschoeni 
** Added Generic Counter API to ES
** Revision 1.10 2010/11/05 15:05:23EDT acudmore 
** Updated CRC function to read memory through PSP interface.
** Revision 1.9 2010/10/25 15:45:28EDT jmdagost 
** Added Task Name and/or TaskID in syslog messsages in CFE_ES_CreateChildTask().
** Revision 1.8 2009/07/28 16:30:29EDT jmdagost 
** Replaced 34 references to filename with references to function name.
** Revision 1.7 2009/06/10 09:08:50EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.6 2008/08/27 15:56:19EDT apcudmore 
** removed startup sync debug OS_printfs
** Revision 1.5 2008/08/01 15:54:54EDT apcudmore 
** Updated Startup sync logic after vxWorks tests.
** Revision 1.4 2008/07/31 15:41:32EDT apcudmore 
** Added execution counter API:
**   -- Added execution counter to ES internal task data
**   -- Added code to increment counter in RunLoop
**   -- Added code to report counter in GetInfo APIs
**   -- Added API to increment counter for child tasks
**   -- Added code to cFE Core apps to increment counters.
** Revision 1.3 2008/07/30 14:52:31EDT apcudmore 
** Added startup sync support and API
** Revision 1.2 2008/06/26 15:41:53EDT apcudmore 
** Added public CFE_ES_GetAppInfo API.
** Moved the old API to CFE_ES_GetAppInfoInternal
** -- May merge these two in the near future.
** Revision 1.1 2008/04/17 08:05:02EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.72 2007/09/25 10:44:33EDT apcudmore 
** Removed unused variable.
** Revision 1.71 2007/09/25 10:24:06EDT apcudmore 
** Implement Application Restart on Exception:
** - Added logic to Exception Handler
** - Changed Exception Hook to pass host task ID
** - Added App Reset ER Log entry type
** Revision 1.70 2007/09/21 15:40:20EDT David Kobe (dlkobe) 
** Modified pointer type definitions to eliminate Linux gcc compile warnings
** Revision 1.69 2007/09/20 10:53:00EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.68 2007/05/31 10:11:34EDT apcudmore 
** Removed CFE_ES_CPU_CONTEXT_SIZE from verify.h
** Removed unused variable.
** Revision 1.67 2007/05/30 10:16:50EDT apcudmore 
** Fixed RunLoop Crash 
** Revision 1.66 2007/05/25 11:55:43EDT apcudmore 
** Error Handling updates
**    - CFE_ES_GetAppInfo returns void
**    - added app ID range check in RestartApp
**  
** Revision 1.65 2007/05/15 16:08:51EDT apcudmore 
** Updated CFE_ES_ExitApp to:
**    -- Do a Processor Reset on Core app init failure
**    -- Exit the task on a Core App Runtime failure
** Revision 1.64 2007/05/14 15:43:46EDT apcudmore 
** Cleaned up events and disabled debug code for application restart.
** Revision 1.63 2007/05/04 16:02:50EDT apcudmore 
** Removed debug printf.
** Revision 1.62 2007/05/04 15:58:35EDT apcudmore 
** Implemented ES shared data protection. Eliminates Startup syncronization problem.
** Revision 1.61 2007/05/02 11:59:11EDT apcudmore 
** Initialized "ThisAppId" variable to zero from Static Analysis advice.
** Revision 1.60 2007/04/28 15:42:40EDT dlkobe 
** Added missing semicolon
** Revision 1.59 2007/04/28 15:40:53EDT dlkobe 
** Fix corrupted file
** Revision 1.58 2007/04/28 15:35:19EDT dlkobe 
** Corrected Typos from restructuring
** Revision 1.57 2007/04/28 15:32:38EDT dlkobe 
** Restructured CFE_ES_RegisterCDS call to avoid Application Unknown Error
** Revision 1.56 2007/04/28 14:49:00EDT dlkobe 
** Baseline Implementation of Critical Tables
** Revision 1.55 2007/04/25 11:57:08EDT apcudmore 
** Implemented "Back End" of Application Restart and Delete.
** Changes include all ES Cleanup code to delete App and Task Resources.
** Revision 1.54 2007/04/13 16:24:52EDT apcudmore 
** Fixed "==" bug in If/then/else clause in CFE_ES_ExitApp
** Revision 1.53 2007/04/12 10:37:19EDT apcudmore 
** Added support for ES task to call CFE_ES_ExitApp on initialization and runtime failure.
** Revision 1.52 2007/04/09 15:12:04EDT dlkobe 
** Modified 'Name' parameter to be a const pointer
** Revision 1.51 2007/04/06 12:23:58EDT apcudmore 
** Added Paramter verification and a check to make sure that CFE_ES_CreateChildTask is not 
** called from another Child Task.
** Revision 1.49 2007/04/05 16:16:03EDT apcudmore 
** Added CFE_ES_DeleteChildTask and CFE_ES_ExitChildTask along with headers,
** and associated Error codes.
** Revision 1.48 2007/03/23 15:06:26EST apcudmore 
** Fixes for DCR 470: Improved error handling for CFE_ES_RegisterApp and CFE_ES_RegisterChildTask.
**                             Removed circular include.
** Revision 1.47 2007/03/23 10:47:23EST apcudmore 
** Added CFE_ES_GetTaskInfo call and corresponding CFE_ES_TaskInfo_t data structure.
** Revision 1.46 2007/03/22 10:26:23EST apcudmore 
** Implemented all code walkthrough items related to ES Application logic, commands, and telemetry.
** Revision 1.45 2007/03/16 10:07:13EST apcudmore 
** Fixed Compile error in CreateChildTask
** Revision 1.44 2007/03/16 09:37:48EST apcudmore 
** Removed code that does not allow a Child task's priority to be greater than it's parent.
** Revision 1.43 2007/03/13 16:10:54EST apcudmore 
** Implemented all ERLog and ES API code walkthrough items.
** Revision 1.42 2007/02/28 10:09:30EST apcudmore 
** Command/control logic for ES Application Restart
**
*/

/*
** Required header files.
*/
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_global.h"
#include "cfe_es_events.h"
#include "cfe_es_cds.h"
#include "cfe_es_cds_mempool.h"
#include "cfe_psp.h"
#include "cfe_es_log.h"


#include <string.h>
#include <stdio.h>
#include <stdarg.h>


/*
** Function: CFE_ES_GetResetType
**
** Purpose:  Return The Type of reset the cFE had.
**           The function will return the start type 
**           which is CFE_PSP_RST_TYPE_POWERON or CFE_PSP_RST_TYPE_PROCESSOR.
**           The sub-type is optional and will be returned if a non-NULL pointer
**           is passed in to the function.
*/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    if ( ResetSubtypePtr != NULL )
    {
       *ResetSubtypePtr = CFE_ES_ResetDataPtr->ResetVars.ResetSubtype;
    }

    return(CFE_ES_ResetDataPtr->ResetVars.ResetType);

} /* End of CFE_ES_GetResetType() */


/*
** Function: CFE_ES_ResetCFE
**
** Purpose:  Reset the cFE core and all apps.
**
*/
int32 CFE_ES_ResetCFE(uint32 ResetType)
{
    int32 ReturnCode;
    
    if ( ResetType == CFE_PSP_RST_TYPE_PROCESSOR )
    {
       /*
       ** Increment the processor reset count
       */
       CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount++;

       /*
       ** Before doing a Processor reset, check to see 
       ** if the maximum number has been exceeded
       */
       if ( CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount > 
            CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount )
       {
           CFE_ES_WriteToSysLog("POWER ON RESET due to max proc resets (Commanded).\n");

           /*
           ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
           ** the entry just in case something fails.
           */
           CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON,
                                         CFE_PSP_RST_SUBTYPE_RESET_COMMAND,
                                         "POWER ON RESET due to max proc resets (Commanded).", NULL,0 );
           /*
           ** Call the BSP reset routine 
           */
           CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
       }
       else
       {
           CFE_ES_WriteToSysLog("PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).\n");

           /*
           ** Update the reset variables
           */
           CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = TRUE;

           /*
           ** Log the reset in the ER Log
           */
           CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_ES_PROCESSOR_RESET,
                                       CFE_ES_RESET_COMMAND,
                                       "PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).", NULL,0 );
           /*
           ** Call the BSP reset routine
           */
           CFE_PSP_Restart(CFE_PSP_RST_TYPE_PROCESSOR);
       
       } /* end if */
       
       /* 
       ** If the BSP routine is not implemented, 
       ** it will return. 
       */
       ReturnCode = CFE_ES_NOT_IMPLEMENTED;
    }
    else if ( ResetType == CFE_PSP_RST_TYPE_POWERON )
    {
       CFE_ES_WriteToSysLog("POWERON RESET called from CFE_ES_ResetCFE (Commanded).\n");

       /*
       ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
       ** the entry just in case something fails.
       */
       CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON,
                                       CFE_PSP_RST_SUBTYPE_RESET_COMMAND,
                                       "POWERON RESET called from CFE_ES_ResetCFE (Commanded).", NULL,0 );

       /*
       ** Call the BSP reset routine
       */
       CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
       
       /* 
       ** If the BSP routine is not implemented, 
       ** it will return. 
       */
       ReturnCode = CFE_ES_NOT_IMPLEMENTED;
    }
    else
    {
       CFE_ES_WriteToSysLog("ES ResetCFE: Invalid Reset Type: %d.\n",(int)ResetType);
       ReturnCode = CFE_ES_BAD_ARGUMENT;
    }

    return(ReturnCode);
    
} /* End of CFE_ES_ResetCFE() */

/*
** Function: CFE_ES_RestartApp
**
** Purpose:  Restart a single cFE App.
**
*/
int32 CFE_ES_RestartApp(uint32 AppID)
{
    int32 ReturnCode = CFE_SUCCESS;

    if ( AppID < CFE_ES_MAX_APPLICATIONS )
    {

       CFE_ES_LockSharedData(__func__,__LINE__);
    
       /*
       ** Check to see if the App is an external cFE App.
       */
       if ( CFE_ES_Global.AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
       {
          CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Restart a CORE Application: %s.\n",
                             CFE_ES_Global.AppTable[AppID].StartParams.Name );
          ReturnCode = CFE_ES_ERR_APPID; 
       }
       else if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
       {
          CFE_ES_WriteToSysLog ("CFE_ES_RestartApp: Cannot Restart Application %s, It is not running.\n",
                              CFE_ES_Global.AppTable[AppID].StartParams.Name);
          ReturnCode = CFE_ES_ERR_APPID; 
       }
       else
       {
          CFE_ES_WriteToSysLog("CFE_ES_RestartApp: Restart Application %s Initiated\n",
                             CFE_ES_Global.AppTable[AppID].StartParams.Name);
          CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_SYS_RESTART;
          CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
          CFE_ES_Global.AppTable[AppID].StateRecord.AppTimer = CFE_ES_APP_KILL_TIMEOUT;
       }
    
       CFE_ES_UnlockSharedData(__func__,__LINE__);
    }
    else /* App ID is not valid */
    {
       ReturnCode = CFE_ES_ERR_APPID;

    } /* end if */

    return(ReturnCode);

} /* End of CFE_ES_RestartApp() */

/*
** Function: CFE_ES_ReloadApp
**
** Purpose:  Reload a single cFE App.
**
*/
int32 CFE_ES_ReloadApp(uint32 AppID, const char *AppFileName)
{
    int32 ReturnCode = CFE_SUCCESS;

    CFE_ES_LockSharedData(__func__,__LINE__);
    
    /*
    ** Check to see if the App is an external cFE App.
    */
    if ( CFE_ES_Global.AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Reload a CORE Application: %s.\n", 
       CFE_ES_Global.AppTable[AppID].StartParams.Name );
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_ReloadApp: Cannot Reload Application %s, It is not running.\n",
                              CFE_ES_Global.AppTable[AppID].StartParams.Name);
       ReturnCode = CFE_ES_ERR_APPID; 
    }    
    else
    {    
       CFE_ES_WriteToSysLog("CFE_ES_ReloadApp: Reload Application %s Initiated. New filename = %s\n", 
                             CFE_ES_Global.AppTable[AppID].StartParams.Name, AppFileName);
       strncpy((char *)CFE_ES_Global.AppTable[AppID].StartParams.FileName, AppFileName, OS_MAX_PATH_LEN);
       CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_SYS_RELOAD;
       CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
       CFE_ES_Global.AppTable[AppID].StateRecord.AppTimer = CFE_ES_APP_KILL_TIMEOUT;
    }
    
    CFE_ES_UnlockSharedData(__func__,__LINE__);
        
    return(ReturnCode);

} /* End of CFE_ES_ReloadApp() */

/*
** Function: CFE_ES_DeleteApp
**
** Purpose:  Delete a cFE App.
**
*/
int32 CFE_ES_DeleteApp(uint32 AppID)
{
    int32 ReturnCode = CFE_SUCCESS;

    CFE_ES_LockSharedData(__func__,__LINE__);
    
    /*
    ** Check to see if the App is an external cFE App.
    */
    if ( CFE_ES_Global.AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Delete a CORE Application: %s.\n", 
       CFE_ES_Global.AppTable[AppID].StartParams.Name );
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Delete Application %s, It is not running.\n",
                              CFE_ES_Global.AppTable[AppID].StartParams.Name);
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else
    {
       CFE_ES_WriteToSysLog("CFE_ES_DeleteApp: Delete Application %s Initiated\n",
                             CFE_ES_Global.AppTable[AppID].StartParams.Name);    
       CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_SYS_DELETE;
       CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
       CFE_ES_Global.AppTable[AppID].StateRecord.AppTimer = CFE_ES_APP_KILL_TIMEOUT;
    }
    
    CFE_ES_UnlockSharedData(__func__,__LINE__);
        
    return(ReturnCode);

} /* End of CFE_ES_DeleteApp() */

/*
** Function: CFE_ES_ExitApp
**
** Purpose:  Exit a cFE App.
**
*/
void CFE_ES_ExitApp(uint32 ExitStatus)
{
    int32   ReturnCode;
    uint32  AppID;

    CFE_ES_LockSharedData(__func__,__LINE__);

    /*
    ** Get App ID - Be careful not to lock the shared data before calling GetAppID
    */
    ReturnCode = CFE_ES_GetAppIDInternal(&AppID);
    if ( ReturnCode == CFE_SUCCESS )
    {
       /*
       ** Check to see if the App is an external cFE App.
       */
       if ( CFE_ES_Global.AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
       {
          /*
          ** A core app should only call this function with one of two ExitStatus codes.
          */
          if ( ExitStatus == CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR )
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp: CORE Application %s Had an Init Error.\n",
                                     CFE_ES_Global.AppTable[AppID].StartParams.Name );
                                     
              /*
              ** Unlock the ES Shared data before calling ResetCFE 
              */
              CFE_ES_UnlockSharedData(__func__,__LINE__);
              
              
              /*
              ** Do a Processor Reset the cFE
              */                                                                                                          
              ReturnCode = CFE_ES_ResetCFE(CFE_PSP_RST_TYPE_PROCESSOR);
              
              /*
              ** Code will not return
              */
              
          }
          else if ( ExitStatus == CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR )
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp: CORE Application %s Had a Runtime Error.\n",
                                     CFE_ES_Global.AppTable[AppID].StartParams.Name );  
                                                                                             
              /*
              ** Unlock the ES Shared data before killing the main task
              */
              CFE_ES_UnlockSharedData(__func__,__LINE__);
                             
              /*
              ** Exit this task
              */
              OS_TaskExit();   
              
              /*
              ** Code will not return, except under unit test
              */
              return;
          }
          else 
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp, Cannot Exit CORE Application %s\n",
                                    CFE_ES_Global.AppTable[AppID].StartParams.Name );         
          }
                    
       }
       else /* It is an external App */
       {
          
          CFE_ES_WriteToSysLog ("Application %s called CFE_ES_ExitApp\n",
                                 CFE_ES_Global.AppTable[AppID].StartParams.Name);
          CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;


          /*
          ** Unlock the ES Shared data before suspending the app 
          */
          CFE_ES_UnlockSharedData(__func__,__LINE__);

          /*
          ** Suspend the Application until ES kills it.
          ** It might be better to have a way of suspending the app in the OS
          */
          while(1)
          {
             OS_TaskDelay(500);
          }

       } /* end if */
   
   } /* end if ReturnCode == CFE_SUCCESS */

   CFE_ES_UnlockSharedData(__func__,__LINE__);
   
} /* End of CFE_ES_ExitApp() */

/*
** Function: CFE_ES_RunLoop
**
** Purpose:  Check the Run Status of an Application.
**
*/
int32 CFE_ES_RunLoop(uint32 *RunStatus)
{
    int32  ReturnCode;
    uint32 AppID;
    uint32 TaskID;
    
    CFE_ES_LockSharedData(__func__,__LINE__);

    /*
    ** Get App ID
    */
    ReturnCode = CFE_ES_GetAppIDInternal(&AppID);

    if ( ReturnCode == CFE_SUCCESS )
    {
    
       /*
       ** Get the Task ID for the main task
       */
       OS_ConvertToArrayIndex(CFE_ES_Global.AppTable[AppID].TaskInfo.MainTaskId, &TaskID);
       
       /*
       ** Increment the execution counter for the main task
       */
       CFE_ES_Global.TaskTable[TaskID].ExecutionCounter++;
    
       /*
       ** Validate RunStatus
       */
       if ( *RunStatus == CFE_ES_RUNSTATUS_APP_RUN || *RunStatus == CFE_ES_RUNSTATUS_APP_EXIT || *RunStatus == CFE_ES_RUNSTATUS_APP_ERROR )
       {  
          /*
          ** Look up the system control request
          */
          if ( *RunStatus == CFE_ES_RUNSTATUS_APP_RUN )
          {            
             /* 
             ** The first time an App calls this function, change the state from 
             ** INITIALIZING to RUNNING. This is for the startup syncronization code below
             */
             if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppState == CFE_ES_APP_STATE_INITIALIZING)
             {
                /* 
                ** Change the state to RUNNING
                */
                CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
                ++CFE_ES_Global.AppReadyCount;
                
             } /* End if AppState == CFE_ES_APP_STATE_INITIALIZING */
             
             if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest != CFE_ES_RUNSTATUS_APP_RUN )
             { 
                 /*
                 ** We have an external request to stop
                 */
                 ReturnCode = FALSE;
                 
             }
             else
             {
                /*
                ** Everything is OK
                */
                ReturnCode = TRUE;
             }
          }
          else
          {
             /*
             ** Application wants to exit, so let it
             */
             CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = *RunStatus; 
             ReturnCode = FALSE;
          }
       }
       else 
       {
          /*
          ** Not a supported RunStatus code, the app will abort
          */
          CFE_ES_WriteToSysLog("CFE_ES_RunLoop Error: Invalid RunStatus:%d!\n",(int)(*RunStatus));
          CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_APP_ERROR;
          ReturnCode = FALSE;
       
       } /* End if *RunStatus == CFE_ES_APP_RUN .. */
   
    }
    else
    {
       /*
       ** Cannot do anything without the AppID
       */
       CFE_ES_WriteToSysLog("CFE_ES_RunLoop Error: Cannot get AppID for the caller: RC = %08X\n",(unsigned int)ReturnCode);
       ReturnCode = FALSE;
         
    } /* end if ReturnCode == CFE_SUCCESS */

    CFE_ES_UnlockSharedData(__func__,__LINE__);

    return(ReturnCode);

} /* End of CFE_ES_RunLoop() */

/*
** Function: CFE_ES_WaitForStartupSync
**
** Purpose:  Called by applications that need to ensure that all other apps are running
**           before completing their initialization process.
**
**           This will also mark the calling task itself as "ready"
**
*/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
    int32 Status;
    uint32 AppID;
    uint32 MinSystemState;

    MinSystemState = CFE_ES_SYSTEM_STATE_UNDEFINED;

    /*
     * An application calling CFE_ES_WaitForStartupSync() is assumed to have
     * completed its initialization and is ready to run.  This is required,
     * or else the sync delay will end up waiting for itself, or could deadlock
     * by two apps waiting for each other, etc.
     */
    CFE_ES_LockSharedData(__func__,__LINE__);
    Status = CFE_ES_GetAppIDInternal(&AppID);
    if ( Status == CFE_SUCCESS )
    {
        if ( CFE_ES_Global.AppTable[AppID].StateRecord.AppState == CFE_ES_APP_STATE_INITIALIZING)
        {
            CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
            ++CFE_ES_Global.AppReadyCount;
        }

        /*
         * For core apps that call this function, these can start once the core app init
         * process is done.  For everything else (external apps) wait until fully operational.
         */
        if ( CFE_ES_Global.AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE)
        {
            MinSystemState = CFE_ES_SYSTEM_STATE_CORE_READY;
        }
        else
        {
            MinSystemState = CFE_ES_SYSTEM_STATE_OPERATIONAL;
        }

    }
    CFE_ES_UnlockSharedData(__func__,__LINE__);

    /* Do the actual delay loop */
    CFE_ES_ApplicationSyncDelay(MinSystemState, TimeOutMilliseconds);

} /* End of CFE_ES_WaitForStartupSync() */


/*
** Function: CFE_ES_RegisterApp
**
** Purpose:  Register a new Application with the cFE. This
**           function must be called in a cFE app before
**           any other cFE function.
**
*/
int32 CFE_ES_RegisterApp(void)
{
   int32 Result = CFE_SUCCESS;

   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Register the task 
   */
   Result = OS_TaskRegister();

   if (Result == OS_SUCCESS)
   {
       Result = CFE_SUCCESS;
   }
   else
   {
       /* 
       ** Cannot create a syslog entry here because it requires the task to 
       ** be registered
       */
       Result = CFE_ES_ERR_APP_REGISTER;
   }

   /*
   ** Set the default exception environment
   */
   CFE_PSP_SetDefaultExceptionEnvironment();
   
   CFE_ES_UnlockSharedData(__func__,__LINE__);

   return(Result);
  

} /* End of CFE_ES_RegisterApp() */

/*
** Function: CFE_ES_GetAppIDByName
**
** Purpose:  Given a valid App Name, return the 
**           Application ID.
**
*/
int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, const char *AppName)
{
   int32 Result = CFE_ES_ERR_APPNAME;
   uint32 i;

   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Search the ES Application table for an app with a matching name.
   */
   for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
   {
      if ( CFE_ES_Global.AppTable[i].RecordUsed == TRUE )
      {
         if ( strncmp(AppName, (char *)CFE_ES_Global.AppTable[i].StartParams.Name, OS_MAX_API_NAME) == 0 )
         {
            *AppIdPtr = i;
            Result = CFE_SUCCESS;
            break;
         }
      }
   } /* end for */
   
   CFE_ES_UnlockSharedData(__func__,__LINE__);
      
   return(Result);

} /* End of CFE_ES_GetAppIDByName() */


/*
** Function: CFE_ES_GetAppID
**
** Purpose:  Return the Caller's cFE Application ID.
**
*/
int32 CFE_ES_GetAppID(uint32 *AppIdPtr)
{
   int32  Result = CFE_ES_ERR_APPID;
   uint32 TaskId;

   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Step 1: Get the OS task ID
   */
   if (OS_ConvertToArrayIndex(OS_TaskGetId(), &TaskId) == OS_SUCCESS)
   {
      /*
      ** Step 2: get the Application ID for the current task
      */
      if ( CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
      {
         *AppIdPtr = CFE_ES_Global.TaskTable[TaskId].AppId;
         Result = CFE_SUCCESS;
      } /* end if */
   }

   CFE_ES_UnlockSharedData(__func__,__LINE__);
      
   return(Result);

} /* End of CFE_ES_GetAppID() */

/*
** Function: CFE_ES_GetAppName
**
** Purpose:  Given a valid AppID, return the 
**           Application name string.
**
*/
int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength)
{
   int32 Result;

   CFE_ES_LockSharedData(__func__,__LINE__);

   if ( AppId < CFE_ES_MAX_APPLICATIONS )
   {
      if ( CFE_ES_Global.AppTable[AppId].RecordUsed == TRUE )
      {
         strncpy(AppName, (char *)CFE_ES_Global.AppTable[AppId].StartParams.Name, BufferLength);
         AppName[BufferLength - 1] = '\0';
         Result = CFE_SUCCESS;
      }
      else
      {
         Result = CFE_ES_ERR_APPID;
      }
   }
   else
   {
      Result = CFE_ES_ERR_APPID;
   }
   
   CFE_ES_UnlockSharedData(__func__,__LINE__);
      
   return(Result);

} /* End of CFE_ES_GetAppName() */


/*
** Function: CFE_ES_GetAppInfo
**
** Purpose:  Get Application Information given a specified App ID
**
*/
int32 CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
   int32  ReturnCode = CFE_SUCCESS;

   if ( AppInfo != 0 )
   {
      if ( AppId < CFE_ES_MAX_APPLICATIONS )
      {
         if ( CFE_ES_Global.AppTable[AppId].RecordUsed == TRUE )
         {
            CFE_ES_GetAppInfoInternal(AppId, AppInfo);
            ReturnCode = CFE_SUCCESS;
         }
         else
         {
            CFE_ES_WriteToSysLog("CFE_ES_GetAppInfo: App ID Not Active: %d\n",(int)AppId);
            ReturnCode = CFE_ES_ERR_APPID;
         }
      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_GetAppInfo: App ID Exceeds CFE_ES_APPLICATION_MAX: %d\n",(int)AppId);
         ReturnCode = CFE_ES_ERR_APPID;
      }
   }
   else
   {
      CFE_ES_WriteToSysLog("CFE_ES_GetAppInfo: Invalid Parameter ( Null Pointer )\n");
      ReturnCode = CFE_ES_ERR_BUFFER;
   }
   return(ReturnCode);

} /* End of CFE_ES_GetAppInfo() */

/*
** Function: CFE_ES_GetTaskInfo
**
** Purpose:  Get Task Information given a specified Task ID
**
*/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 OSTaskId)
{
   int32  ReturnCode = CFE_SUCCESS;
   uint32 TaskId;

   CFE_ES_LockSharedData(__func__,__LINE__);

   if (OS_ConvertToArrayIndex(OSTaskId, &TaskId) != OS_SUCCESS || TaskId >= OS_MAX_TASKS)
   {
      CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID Not Valid: %u\n",(unsigned int)OSTaskId);
      ReturnCode = CFE_ES_ERR_TASKID;
   }
   else if (  CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
   {

      /*
      ** Get the Application ID and Task Name
      */
      TaskInfo->AppId = CFE_ES_Global.TaskTable[TaskId].AppId;
      strncpy((char *)TaskInfo->TaskName,
              (char *)CFE_ES_Global.TaskTable[TaskId].TaskName,OS_MAX_API_NAME);
      TaskInfo->TaskName[OS_MAX_API_NAME - 1] = '\0';

      /*
      ** Get the Application Name
      */
      if ( CFE_ES_Global.AppTable[TaskInfo->AppId].RecordUsed == TRUE )
      {
         strncpy((char *)TaskInfo->AppName,
                 (char *)CFE_ES_Global.AppTable[TaskInfo->AppId].StartParams.Name,
                 OS_MAX_API_NAME);
         TaskInfo->AppName[OS_MAX_API_NAME - 1] = '\0';

         /*
         ** Store away the Task ID ( for the QueryAllTasks Cmd )
         */
         TaskInfo->TaskId = OSTaskId;


         /*
         ** Get the Execution counter for the task
         */
         TaskInfo->ExecutionCounter =  CFE_ES_Global.TaskTable[TaskId].ExecutionCounter;

         ReturnCode = CFE_SUCCESS;

      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID:%u Parent App ID:%d not Active.\n",
                 (unsigned int)OSTaskId,(int)TaskInfo->AppId);
         ReturnCode = CFE_ES_ERR_TASKID;
      }
   }
   else
   {
      CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID Not Active: %u\n",(unsigned int)OSTaskId);
      ReturnCode = CFE_ES_ERR_TASKID;
   }
   
   CFE_ES_UnlockSharedData(__func__,__LINE__);

   return(ReturnCode);

} /* End of CFE_ES_GetAppName() */


/*
** Function: CFE_ES_CreateChildTask
**
** Purpose:  Create a Child task for the Application.
**
*/
int32 CFE_ES_CreateChildTask(uint32 *TaskIdPtr,
                        const char   *TaskName,
                        CFE_ES_ChildTaskMainFuncPtr_t   FunctionPtr,
                        uint32 *StackPtr,
                        uint32  StackSize,
                        uint32  Priority,
                        uint32  Flags)
{

   int32          Result;
   uint32         AppId= 0xFFFFFFFF;
   int32          ReturnCode;
   uint32         TaskId;
   uint32         ParentTaskId;

   /*
   ** Validate some of the arguments
   */
   if ( TaskIdPtr == NULL )
   {
      if (TaskName == NULL)
      {
          CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Task Id and Name Pointer Parameters are NULL.\n");
          ReturnCode = CFE_ES_BAD_ARGUMENT;
      }
      else
      {
          CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Task Id Pointer Parameter is NULL for Task '%s'.\n",TaskName);
          ReturnCode = CFE_ES_BAD_ARGUMENT;
      }
   }
   else if ( TaskName == NULL )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: TaskName Parameter is NULL for Task ID %d.\n",(int)(*TaskIdPtr));
      ReturnCode = CFE_ES_BAD_ARGUMENT;
   }
   else if ( FunctionPtr == NULL )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Function Pointer Parameter is NULL for Task '%s' (ID %d).\n",TaskName,(int)(*TaskIdPtr));
      ReturnCode = CFE_ES_BAD_ARGUMENT;
   }
   else 
   {
   
      CFE_ES_LockSharedData(__func__,__LINE__);
   
      /*
      ** Get the AppID of the calling Application
      */
      Result = CFE_ES_GetAppIDInternal(&AppId);
      if (Result != CFE_SUCCESS)
      {
          CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Error calling CFE_ES_GetAppID for Task '%s'. RC = 0x%08X\n",TaskName,(unsigned int)Result);
          ReturnCode = Result;
      }
      else  /* else AppId is valid */
      {
         /*
         ** First, Make sure the Calling Task is a cFE Main task.
         ** TaskID must be the same as the Parent Task ID.
         */
         TaskId = OS_TaskGetId();
         ParentTaskId = CFE_ES_Global.AppTable[AppId].TaskInfo.MainTaskId;
         if ( TaskId == ParentTaskId )
         {
            /*
            ** Truncate the priority if needed
            */                          
            if ( Priority >  255  )
            {
                Priority = 255;
            }

            /*
            ** Step 2: Create the new task using the OS API call
            */
            Result = OS_TaskCreate(TaskIdPtr, TaskName, FunctionPtr, StackPtr,
                                StackSize, Priority, OS_FP_ENABLED );

            /*
            ** Step 3: Record the task information in the task table
            */
            if ( Result == OS_SUCCESS )
            {
               OS_ConvertToArrayIndex(*TaskIdPtr, &TaskId);

               CFE_ES_Global.TaskTable[TaskId].RecordUsed = TRUE;
               CFE_ES_Global.TaskTable[TaskId].AppId = AppId;
               CFE_ES_Global.TaskTable[TaskId].TaskId = *TaskIdPtr;
               strncpy((char *)CFE_ES_Global.TaskTable[TaskId].TaskName,TaskName,OS_MAX_API_NAME);
               CFE_ES_Global.TaskTable[TaskId].TaskName[OS_MAX_API_NAME - 1] = '\0';
               CFE_ES_Global.RegisteredTasks++;

               /*
               ** Increment the "Registered" child task count for the App
               */
               CFE_ES_Global.AppTable[AppId].TaskInfo.NumOfChildTasks ++;

               ReturnCode = CFE_SUCCESS;
            }
            else
            {
               CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Error calling OS_TaskCreate for Task '%s' RC = 0x%08X\n",TaskName,(unsigned int)Result);
               ReturnCode = CFE_ES_ERR_CHILD_TASK_CREATE;
            }
         } 
         else
         {
            CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Error: Cannot call from a Child Task (for Task '%s').\n",TaskName);
            ReturnCode = CFE_ES_ERR_CHILD_TASK_CREATE;
         
         } /* end if Calling task is a main task */
         
      }/* end If AppID is valid */
      
      CFE_ES_UnlockSharedData(__func__,__LINE__);

   } /* end if parameter checking */
   
   return(ReturnCode);

} /* End of CFE_ES_CreateChildTask() */


/*
** Function: CFE_ES_RegisterChildTask
**
** Purpose:  Register a child task. This must be called by a Child Task before any
**           other cFE calls.
**
*/
int32 CFE_ES_RegisterChildTask(void)
{
   int32 Result;
   int32 ReturnCode;

   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Register the task with the OS 
   */
   Result = OS_TaskRegister();

   if (Result != OS_SUCCESS)
   {
       /* 
       ** Cannot create a syslog entry here because it requires the task to 
       ** be registered
       */
       ReturnCode = CFE_ES_ERR_CHILD_TASK_REGISTER;
   }
   else
   { 
       ReturnCode = CFE_SUCCESS;
   }
   
   /*
   ** Set the default exception environment
   */
   CFE_PSP_SetDefaultExceptionEnvironment();

   CFE_ES_UnlockSharedData(__func__,__LINE__);

   return(ReturnCode);

} /* End of CFE_ES_RegisterChildTask() */

/*
** Function: CFE_ES_IncrementTaskCounter
**
** Purpose:  Increment the Child Task Execution Counter.
**
*/
void CFE_ES_IncrementTaskCounter(void)
{
   uint32 TaskId;

   if (OS_ConvertToArrayIndex(OS_TaskGetId(), &TaskId) == OS_SUCCESS)
   {
      CFE_ES_Global.TaskTable[TaskId].ExecutionCounter++;
   }
   
} /* End of CFE_ES_ExitChildTask() */



/*
** Function: CFE_ES_DeleteChildTask
**
** Purpose:  Remove a Child Task from the cFE system.
**
*/
int32 CFE_ES_DeleteChildTask(uint32 OSTaskId)
{
    uint32  i;
    uint32  TaskId;
    uint32  TaskIsMain = FALSE;
    int32   ReturnCode = CFE_SUCCESS;
    int32   OSReturnCode;


    /*
    ** Make sure the task ID is within range
    */
    if (OS_ConvertToArrayIndex(OSTaskId, &TaskId) == OS_SUCCESS)
    {
       CFE_ES_LockSharedData(__func__,__LINE__);
    
       /*
       ** Make sure the task is active/valid
       */
       if (TaskId < OS_MAX_TASKS && CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
       {
          /*
          ** Search for this task ID in the ES App Table to make sure
          ** it is not a cFE App Main Task
          */
          TaskIsMain = FALSE;
          for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
          {
             if ( CFE_ES_Global.AppTable[i].RecordUsed == TRUE )
             {
                if ( CFE_ES_Global.AppTable[i].TaskInfo.MainTaskId == OSTaskId )
                {
                   /*
                   ** Error, the task Id is an App Main Task ID
                   */
                   TaskIsMain = TRUE;
                   break; 
                } /* end if */
             } /* end if */
          } /* end for */
          
          if ( TaskIsMain == FALSE )
          {
             /*
             ** Can delete the Task
             */
             OSReturnCode = OS_TaskDelete(OSTaskId);
             if ( OSReturnCode == OS_SUCCESS )
             {
                /*
                ** Invalidate the task table entry
                */
                CFE_ES_Global.TaskTable[TaskId].RecordUsed = FALSE;
                CFE_ES_Global.RegisteredTasks--;

                /*
                ** Report the task delete
                */
                CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Task %u Deleted\n",(unsigned int)OSTaskId );
                ReturnCode = CFE_SUCCESS;
             }
             else
             {
                CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Error Calling OS_TaskDelete: Task %u, RC = 0x%08X\n",
                        (unsigned int)OSTaskId, (unsigned int)OSReturnCode);
                ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE;
             }
          }
          else
          {
             /*
             ** Error: The task is a cFE Application Main task
             */
             CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Task %u is a cFE Main Task.\n",(unsigned int)OSTaskId );
             ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK;
          } /* end if TaskMain == FALSE */
       }
       else
       {
          /*
          ** Task ID is not in use, so it is invalid
          */
          CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Task ID is not active: %u\n",(unsigned int)OSTaskId );
          ReturnCode = CFE_ES_ERR_TASKID;
   
       } /* end if */
       
       CFE_ES_UnlockSharedData(__func__,__LINE__);
   
    }
    else
    {
       /*
       ** Task ID is invalid ( too large )
       */
       CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Invalid Task ID: %u\n",(unsigned int)OSTaskId );
       ReturnCode = CFE_ES_ERR_TASKID;
       
    }
    return(ReturnCode);

} /* End of CFE_ES_DeleteTask() */

/*
** Function: CFE_ES_ExitChildTask
**
** Purpose:  Stop execution of a child task.
**
*/
void CFE_ES_ExitChildTask(void)
{
   uint32 TaskId;
   uint32 ParentTaskId;
   uint32 AppId; 
   uint32 ReturnCode;

   CFE_ES_LockSharedData(__func__,__LINE__);

   /*
   ** Check to see if this is being called from a cFE Application's 
   ** main task.
   */
   TaskId = OS_TaskGetId();
   
   ReturnCode = CFE_ES_GetAppIDInternal(&AppId);
   if ( ReturnCode == CFE_SUCCESS )
   {
      ParentTaskId = CFE_ES_Global.AppTable[AppId].TaskInfo.MainTaskId;
      if ( TaskId != ParentTaskId )
      {
         if (OS_ConvertToArrayIndex(TaskId, &TaskId) == OS_SUCCESS)
         {
            /*
            ** Invalidate the task table entry
            */
            CFE_ES_Global.TaskTable[TaskId].RecordUsed = FALSE;
            CFE_ES_Global.RegisteredTasks--;


            CFE_ES_UnlockSharedData(__func__,__LINE__);


            /*
            ** Call the OS AL routine
            */
            OS_TaskExit();
            /*
            ** Does not return from OS_TaskExit, except under unit test
            */
            return;
         }
         
      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_ExitChildTask Error: Cannot Call from a cFE App Main Task. ID = %d\n",(int)TaskId );
      }
   }
   else
   {   
      CFE_ES_WriteToSysLog("CFE_ES_ExitChildTask Error Calling CFE_ES_GetAppID. Task ID = %d, RC = 0x%08X\n",
              (int)TaskId, (unsigned int)ReturnCode );
   } /* end if GetAppId */

   CFE_ES_UnlockSharedData(__func__,__LINE__);
   
} /* End of CFE_ES_ExitChildTask() */


/*
** Function: CFE_ES_WriteToSysLog
**
** Purpose:  Add a time stamped message to the cFE ES System Log.
**
*/
int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    va_list       ArgPtr;
    char          TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    char          MsgWithoutTime[CFE_EVS_MAX_MESSAGE_LENGTH];
    size_t        TmpStringLen;
    size_t        LogOverflow;
    size_t        TruncTmpStringLen;
    uint32        ReturnCode;
    int32         i;

    /* write the current time into the TmpString buffer */
    CFE_TIME_Print(TmpString, CFE_TIME_GetTime());

    va_start(ArgPtr, SpecStringPtr);
    (void) vsnprintf(MsgWithoutTime,(size_t) CFE_EVS_MAX_MESSAGE_LENGTH, SpecStringPtr, ArgPtr);
    va_end(ArgPtr);

    /* insert space between the time and the start of the message */
    strcat(TmpString," ");

    /* Add the message to the time string in the TmpString buffer */
    strncat(TmpString,MsgWithoutTime,CFE_EVS_MAX_MESSAGE_LENGTH);
   
    /* Output the entry to the console. */
    OS_printf("%s",TmpString);
    TmpStringLen = strlen(TmpString);

    /* process the log entry depending on the log type */
    if ( CFE_ES_ResetDataPtr->SystemLogMode == CFE_ES_LOG_DISCARD )
    {
        /* if the index is already out of bounds, the log is full */
        if ( CFE_ES_ResetDataPtr->SystemLogIndex >= CFE_ES_SYSTEM_LOG_SIZE )  
        {
            OS_printf("Warning: System Log full, log entry discarded.\n");
            ReturnCode = CFE_ES_ERR_SYS_LOG_FULL;
        }   
        /* if the message will not fit in the remaining space, truncate it */  
        else if ((CFE_ES_ResetDataPtr->SystemLogIndex + TmpStringLen) >= CFE_ES_SYSTEM_LOG_SIZE )
        {
            LogOverflow = CFE_ES_ResetDataPtr->SystemLogIndex + TmpStringLen - CFE_ES_SYSTEM_LOG_SIZE;

            /* Add one for adding '/0' to the end because the message will be truncated */
            LogOverflow++;

            TruncTmpStringLen = TmpStringLen - LogOverflow;

            strncpy((char *)&(CFE_ES_ResetDataPtr->SystemLog[CFE_ES_ResetDataPtr->SystemLogIndex]), TmpString, TruncTmpStringLen);
            strncpy((char *)&(CFE_ES_ResetDataPtr->SystemLog[CFE_ES_ResetDataPtr->SystemLogIndex + TruncTmpStringLen]), "\0", 1 );

            CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_ResetDataPtr->SystemLogIndex + TruncTmpStringLen + 1;
            CFE_ES_ResetDataPtr->SystemLogEntryNum = CFE_ES_ResetDataPtr->SystemLogEntryNum + 1;
            OS_printf("Warning: Last System Log Message Truncated.\n");
            ReturnCode = CFE_SUCCESS;
        } 
        else /* the message fits */
        {
            TmpStringLen = strlen(TmpString);
            strncpy((char *)&(CFE_ES_ResetDataPtr->SystemLog[CFE_ES_ResetDataPtr->SystemLogIndex]), TmpString, TmpStringLen);
            CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_ResetDataPtr->SystemLogIndex + TmpStringLen;
            CFE_ES_ResetDataPtr->SystemLogEntryNum = CFE_ES_ResetDataPtr->SystemLogEntryNum + 1;
            ReturnCode = CFE_SUCCESS;
        }
    }
    else if ( CFE_ES_ResetDataPtr->SystemLogMode == CFE_ES_LOG_OVERWRITE )
    {
        /* if the index is already out of bounds, reset it to zero */
        if ( CFE_ES_ResetDataPtr->SystemLogIndex >= CFE_ES_SYSTEM_LOG_SIZE )  
        {
           CFE_ES_ResetDataPtr->SystemLogIndex = 0;
        }
        /* the message will not fit in the remaining space */
        else if ((CFE_ES_ResetDataPtr->SystemLogIndex + TmpStringLen) >= CFE_ES_SYSTEM_LOG_SIZE)  
        {
            /* pad the space at the end of the log to remove any partial old messages */
            for (i = CFE_ES_ResetDataPtr->SystemLogIndex; i < CFE_ES_SYSTEM_LOG_SIZE; i++)
            {
                CFE_ES_ResetDataPtr->SystemLog[i] = ' ';
            }
            CFE_ES_ResetDataPtr->SystemLogIndex = 0;
        }

        TmpStringLen = strlen(TmpString);
        strncpy((char *)&(CFE_ES_ResetDataPtr->SystemLog[CFE_ES_ResetDataPtr->SystemLogIndex]), TmpString, TmpStringLen);
        CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_ResetDataPtr->SystemLogIndex + TmpStringLen;
        CFE_ES_ResetDataPtr->SystemLogEntryNum = CFE_ES_ResetDataPtr->SystemLogEntryNum + 1;
        ReturnCode = CFE_SUCCESS;
    }
    else
    {
        OS_printf("Warning: Invalid System Log mode, log entry discarded.\n");
        ReturnCode = CFE_ES_ERR_SYS_LOG_FULL;
    }    

    return(ReturnCode);

} /* End of CFE_ES_WriteToSysLog() */


/*
** Function: CFE_ES_CalculateCRC
**
** Purpose:  Perform a CRC calculation on a range of memory.
**
*/
uint32 CFE_ES_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC)
{
    uint32  i;
    int16  Index;
    int16  Crc = 0;
    const uint8 *BufPtr;
    uint8  ByteValue;

    static const uint16 CrcTable[256]=
    {

		    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040

    };

    switch(TypeCRC)
    {
      case CFE_ES_CRC_32:
           CFE_ES_WriteToSysLog("CFE ES Calculate CRC32 not Implemented\n");
           break;

      case CFE_ES_CRC_16:
           Crc    =  (int16 )( 0xFFFF & InputCRC );
           BufPtr = (const uint8 *)DataPtr;

           for ( i = 0 ; i < DataLength ; i++,  BufPtr++)
           {
              if (CFE_PSP_MemRead8((cpuaddr)BufPtr, &ByteValue) != CFE_PSP_SUCCESS)
              {
                 ByteValue = 0;
              }
              Index = ( ( Crc ^ ByteValue) & 0x00FF);
              Crc = ( (Crc >> 8 ) & 0x00FF) ^ CrcTable[Index];
           }
           break;

      case CFE_ES_CRC_8:
           CFE_ES_WriteToSysLog("CFE ES Calculate CRC8 not Implemented\n");
           break;

      default:
           break;
    }
    return(Crc);

} /* End of CFE_ES_CalculateCRC() */


/*
** Function: CFE_ES_RegisterCDS
**
** Purpose:  Allocate a data block for a Critical Data Store.
**
*/
int32 CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *CDSHandlePtr, int32 BlockSize, const char *Name)
{
    int32   Status = CFE_SUCCESS;
    size_t  NameLen = 0;
    uint32  ThisAppId = 0;
    
    char    AppName[OS_MAX_API_NAME] = {"UNKNOWN"};
    char    CDSName[CFE_ES_CDS_MAX_FULL_NAME_LEN] = {""};

    /* Check to make sure calling application is legit */
    Status = CFE_ES_CDS_ValidateAppID(&ThisAppId);
    
    if ( Status != CFE_SUCCESS )  /* Application ID was invalid */
    {
         CFE_ES_WriteToSysLog("CFE_CDS:Register-Bad AppId(%d)\n", (int)ThisAppId);
    }
    else if (CFE_ES_Global.CDSVars.MemPoolSize == 0)
    {
        CFE_ES_WriteToSysLog("CFE_CDS:Register-CDS not available\n");
        Status = CFE_ES_NOT_IMPLEMENTED;
    }
    else
    {
        /* Assume we can't make a CDS and return a bad handle for now */
        *CDSHandlePtr = CFE_ES_CDS_BAD_HANDLE;

        /* Make sure specified CDS name is not too long or too short */
        NameLen = strlen(Name);
        if ((NameLen > CFE_ES_CDS_MAX_NAME_LENGTH) || (NameLen == 0))
        {
           Status = CFE_ES_CDS_INVALID_NAME;

           /* Perform a buffer overrun safe copy of name for debug log message */

           strncpy(CDSName, Name, CFE_ES_CDS_MAX_NAME_LENGTH);
           CDSName[CFE_ES_CDS_MAX_NAME_LENGTH-1] = '\0';
           CFE_ES_WriteToSysLog("CFE_CDS:Register-CDS Name (%s) is too long\n", CDSName);
        }
        else
        {
           /* Modify specified name to be processor specific name */
           /* of the form "AppName.Name"                          */
           CFE_ES_FormCDSName(CDSName, Name, ThisAppId);

           /* Make sure the specified size is acceptable */
           if (BlockSize == 0)
           {
              Status = CFE_ES_CDS_INVALID_SIZE;
              CFE_ES_WriteToSysLog("CFE_CDS:Register-CDS %s has size of zero\n", Name);
           }
           else
           {
              /* Create CDS and designate it as NOT being a Critical Table */
              Status = CFE_ES_RegisterCDSEx(CDSHandlePtr, BlockSize, CDSName, FALSE);
           }
        }
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);

        CFE_EVS_SendEventWithAppID(CFE_ES_CDS_REGISTER_ERR_EID,
                                   CFE_EVS_ERROR,
                                   ThisAppId,
                                   "%s Failed to Register CDS '%s', Status=0x%08X",
                                   AppName, Name, (unsigned int)Status);
    }
    
    return Status;
} /* End of CFE_ES_RegisterCDS */

/*
** Function: CFE_ES_CopyToCDS
**
** Purpose:  Copies a data block to a Critical Data Store.
**
*/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy)
{
    int32 Status = CFE_SUCCESS;
    
    Status = CFE_ES_CDSBlockWrite(CFE_ES_Global.CDSVars.Registry[Handle].MemHandle, DataToCopy);
    
    return Status;
} /* End of CFE_ES_CopyToCDS() */

/*
** Function: CFE_ES_RestoreFromCDS
**
** Purpose:  Restores a data block from a Critical Data Store.
**
*/
int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    int32 Status = CFE_SUCCESS;
    
    Status = CFE_ES_CDSBlockRead(RestoreToMemory, CFE_ES_Global.CDSVars.Registry[Handle].MemHandle);
    
    return Status;
} /* End of CFE_ES_RestoreFromCDS() */

/* end of file */


int32 CFE_ES_RegisterGenCounter(uint32 *CounterIdPtr, const char *CounterName)
{
   int32 ReturnCode = CFE_ES_BAD_ARGUMENT;
   uint32 CheckPtr;
   int32 Status;
   uint32 i;

   Status = CFE_ES_GetGenCounterIDByName(&CheckPtr, CounterName);

   if ((CounterIdPtr != NULL) && (CounterName != NULL) && (Status != CFE_SUCCESS))
   {
      for ( i = 0; i < CFE_ES_MAX_GEN_COUNTERS; i++ )
      {
         if ( CFE_ES_Global.CounterTable[i].RecordUsed == FALSE )
         {
            strncpy((char *)CFE_ES_Global.CounterTable[i].CounterName,CounterName,OS_MAX_API_NAME);

            CFE_ES_Global.CounterTable[i].RecordUsed = TRUE;
            CFE_ES_Global.CounterTable[i].Counter = 0;
            *CounterIdPtr = i;
            break;
         }
      }
      if (i < CFE_ES_MAX_GEN_COUNTERS)
      {
         ReturnCode = CFE_SUCCESS;
      }
   }

   return ReturnCode;

}

/*
** Function: CFE_ES_DeleteGenCounter
**
** Purpose:  Delete a Generic Counter.
**
*/
int32 CFE_ES_DeleteGenCounter(uint32 CounterId)
{

   int32 Status = CFE_ES_BAD_ARGUMENT;

   if(CounterId < CFE_ES_MAX_GEN_COUNTERS) 
   {
      CFE_ES_Global.CounterTable[CounterId].RecordUsed = FALSE;
      CFE_ES_Global.CounterTable[CounterId].Counter = 0;
      Status = CFE_SUCCESS;
   }

   return Status;

} /* End of CFE_ES_DeleteGenCounter() */

/*
** Function: CFE_ES_IncrementGenCounter
**
** Purpose:  Increment a Generic Counter.
**
*/
int32 CFE_ES_IncrementGenCounter(uint32 CounterId)
{
   int32 Status = CFE_ES_BAD_ARGUMENT;

   if((CounterId < CFE_ES_MAX_GEN_COUNTERS) &&
      (CFE_ES_Global.CounterTable[CounterId].RecordUsed == TRUE))
   {
      CFE_ES_Global.CounterTable[CounterId].Counter++;
      Status = CFE_SUCCESS;
   }
   return Status;
   
} /* End of CFE_ES_IncrementGenCounter() */

/*
** Function: CFE_ES_SetGenCount
**
** Purpose:  Sets a Generic Counter's count.
**
*/
int32 CFE_ES_SetGenCount(uint32 CounterId, uint32 Count)
{
   int32 Status = CFE_ES_BAD_ARGUMENT;

   if((CounterId < CFE_ES_MAX_GEN_COUNTERS) &&
      (CFE_ES_Global.CounterTable[CounterId].RecordUsed == TRUE))
   {
      CFE_ES_Global.CounterTable[CounterId].Counter = Count;
      Status = CFE_SUCCESS;
   }
   return Status;
} /* End of CFE_ES_SetGenCount() */

/*
** Function: CFE_ES_GetGenCount
**
** Purpose:  Gets the value of a Generic Counter.
**
*/
int32 CFE_ES_GetGenCount(uint32 CounterId, uint32 *Count)
{
   int32 Status = CFE_ES_BAD_ARGUMENT;

   if((CounterId < CFE_ES_MAX_GEN_COUNTERS) &&
      (CFE_ES_Global.CounterTable[CounterId].RecordUsed == TRUE) &&
      (Count != NULL ))
   {
      *Count = CFE_ES_Global.CounterTable[CounterId].Counter;
      Status = CFE_SUCCESS;
   }
   return Status;
} /* End of CFE_ES_GetGenCount() */

int32 CFE_ES_GetGenCounterIDByName(uint32 *CounterIdPtr, const char *CounterName)
{

   int32 Result = CFE_ES_BAD_ARGUMENT;
   uint32   i;

   /*
   ** Search the ES Generic Counter table for a counter with a matching name.
   */
   for ( i = 0; i < CFE_ES_MAX_GEN_COUNTERS; i++ )
   {
      if ( CFE_ES_Global.CounterTable[i].RecordUsed == TRUE )
      {
         if ( strncmp(CounterName, (char *)CFE_ES_Global.CounterTable[i].CounterName, OS_MAX_API_NAME) == 0 )
         {
            if(CounterIdPtr != NULL)
            {
               *CounterIdPtr = i;
               Result = CFE_SUCCESS;
            }
            break;
         }
      }
   } /* end for */
   
   return(Result);

} /* End of CFE_ES_GetGenCounterIDByName() */


/***************************************************************************************
** Private API functions
*/

/*
** Function: CFE_ES_GetAppIDInternal
**
** Purpose:  Return the Caller's cFE Application ID. This internal version is needed
**            so there are not nested calls to the ES Shared Data mutex lock.
**
*/
int32 CFE_ES_GetAppIDInternal(uint32 *AppIdPtr)
{
   int32  Result = CFE_ES_ERR_APPID;
   uint32 TaskId;

   /*
   ** Step 1: Get the OS task ID
   */
   if (OS_ConvertToArrayIndex(OS_TaskGetId(), &TaskId) == OS_SUCCESS)
   {
      /*
      ** Step 2: get the Application ID for the current task
      */
      if ( CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
      {
         *AppIdPtr = CFE_ES_Global.TaskTable[TaskId].AppId;
         Result = CFE_SUCCESS;
      }
      else
      {
         *AppIdPtr = 0;
      } /* end if */
   }
   else
   {
      *AppIdPtr = 0;
   } /* end if */

   return(Result);

} /* End of CFE_ES_GetAppIDInternal() */


/******************************************************************************
**  Function:  CFE_ES_LockSharedData()
**
**  Purpose:
**    ES internal function to take the Shared Data Mutex and handle
**    error conditions.
**
**  Arguments:
**    FunctionName   - the name of the function containing the code that generated the error.
**    LineNumber     - the file line number of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber)
{

    int32   Status;
    uint32  AppId;

    Status = OS_MutSemTake(CFE_ES_Global.SharedDataMutex);
    if (Status != OS_SUCCESS) 
    {
        CFE_ES_GetAppIDInternal(&AppId);

        CFE_ES_WriteToSysLog("ES SharedData Mutex Take Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FunctionName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_ES_LockSharedData */

/******************************************************************************
**  Function:  CFE_ES_UnlockSharedData()
**
**  Purpose:
**    ES internal function to Release the shared data mutex and handle error
**    conditions.
**
**  Arguments:
**    FunctionName   - the name of the function containing the code that generated the error.
**    LineNumber     - the file line number of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber)
{
    int32   Status;
    uint32  AppId;

    Status = OS_MutSemGive(CFE_ES_Global.SharedDataMutex);
    if (Status != OS_SUCCESS) 
    {

        CFE_ES_GetAppIDInternal(&AppId);

        CFE_ES_WriteToSysLog("ES SharedData Mutex Give Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FunctionName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_ES_UnlockSharedData */

/******************************************************************************
**  Function:  CFE_ES_ProcessCoreException()
**
**  Purpose:
**    ES internal function to handle the logging and reset from a system or
**    cFE core exception
**
**  Arguments:
**    Reason String
**    Context
**    Context Size
** 
**  Return:
**    None
*/
void CFE_ES_ProcessCoreException(uint32  HostTaskId,     const char *ReasonString,
                                 const uint32 *ContextPointer, uint32 ContextSize)
{
    uint32                 i;
    int32                  Status;
    OS_task_prop_t         TaskProp;
    CFE_ES_TaskInfo_t      EsTaskInfo;
    uint32                 FoundExceptionTask = 0;
    uint32                 ExceptionTaskID = 0;
    
    /*
    ** If a loadable cFE Application caused the reset and it's
    ** exception action is set to Restart the App rather than cause a
    ** processor reset, then just reset the App.
    */

    /*
    ** We have the Host Task Id ( vxWorks, RTEMS, etc ). Search
    ** the OSAPI to see if a match can be found.
    */ 
    for ( i = 0; i < OS_MAX_TASKS; i++ )
    {
       if (CFE_ES_Global.TaskTable[i].RecordUsed == TRUE)
       {
          ExceptionTaskID = CFE_ES_Global.TaskTable[i].TaskId;
          Status = OS_TaskGetInfo (ExceptionTaskID, &TaskProp);

          if ( Status == OS_SUCCESS && TaskProp.OStask_id == HostTaskId )
          {
             FoundExceptionTask = 1;
             break;
          }
       }
    }

    /*
    ** If the Task is found in the OS, see if the cFE App ID associated with it can be found.
    */
    if ( FoundExceptionTask == 1 )
    {
       Status = CFE_ES_GetTaskInfo( &EsTaskInfo, ExceptionTaskID );
       /*
       ** The App ID was found, now see if the ExceptionAction is set for a reset
       */                                                                                                                         
       if ( Status == CFE_SUCCESS )
       {
          if ( CFE_ES_Global.AppTable[EsTaskInfo.AppId].StartParams.ExceptionAction == CFE_ES_APP_EXCEPTION_RESTART_APP )
          {

             /*
             ** Log the Application reset 
             */
             CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_ES_APP_RESTART,
                            CFE_PSP_RST_SUBTYPE_EXCEPTION, (char *)ReasonString,
                            ContextPointer, ContextSize );

             /*
             ** Finally restart the App! This call is just a request
             ** to ES.
             */ 
             Status = CFE_ES_RestartApp(EsTaskInfo.AppId );
             
             /*
             ** Return to avoid the Processor Restart Logic
             */
             return;

          } /* end if ExceptionAction */
                                                                                                                                     
       } /* end if */
                                                                                                                                   
    } /* End if FoundExceptionTask */
                                                                                                                                     
    /*
    ** If we made it here, which means that we need to do a processor reset
    */

    /*
    ** Before doing a Processor reset, check to see 
    ** if the maximum number has been exceeded
    */
    if ( CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount >= 
         CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount )
    {
        /*
        ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
        ** the entry just in case something fails.
        */
        CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,  CFE_PSP_RST_TYPE_POWERON, 
                            CFE_PSP_RST_SUBTYPE_EXCEPTION, (char *)ReasonString,
                            ContextPointer, ContextSize );

        /*
        ** Call the BSP reset routine to do a Poweron Reset 
        */
        CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);

    }
    else /* Do a processor reset */
    {
        /*
        ** Update the reset variables
        */
        CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount++;
        CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = TRUE;

        /*
        ** Log the reset in the ER Log
        */
        CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_PROCESSOR, 
                            CFE_PSP_RST_SUBTYPE_EXCEPTION, (char *)ReasonString,
                            ContextPointer, ContextSize );

        /*
        ** Need to do a processor reset
        */   
        CFE_PSP_Restart(CFE_PSP_RST_TYPE_PROCESSOR);

    } /* end if */
           
} /* End of CFE_ES_ProcessCoreException */
