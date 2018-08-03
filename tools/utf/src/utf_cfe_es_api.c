/*
** File: utf_cfe_es_api.c
** $Id: utf_cfe_es_api.c 1.9 2012/02/28 10:33:01GMT-05:00 wmoleski Exp  $
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
** Purpose:  cFE Executive Services (ES) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: utf_cfe_es_api.c  $
** Revision 1.9 2012/02/28 10:33:01GMT-05:00 wmoleski 
** Added function hooks and Return Code handling and updated the examples to test these changes.
** Revision 1.8 2012/01/13 12:51:55EST acudmore 
** Changed license text to reflect open source
** Revision 1.7 2010/10/25 15:06:43EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.6 2010/10/04 14:57:00EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.5 2009/10/13 14:46:30EDT wmoleski 
** Updated the ES_GetAppInfoInternal function to copy the execution counter.
** Revision 1.4 2008/08/19 14:14:40EDT wfmoleski 
** When an invalid AppID was used and retrieved in the RunLoop, a statement tried to set an array
** variable in the AppID's slot. Since the AppID was beyond the array bounds, the process crashed
** with a stack dump.
** Revision 1.3 2008/08/19 14:08:48EDT wfmoleski 
** Execution Counters added to the UTF and test example
** Revision 1.2 2008/07/18 09:22:55EDT wfmoleski 
** The CFE_ES_GetAppInfo function along with an internal function to set this info and the test that calls 
** these functions were implemted in the UTF.
** Revision 1.1 2008/04/17 08:09:36EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.17 2008/04/07 13:44:41EDT wfmoleski 
** Added return code setting for ES_CPoolCreate, ES_GetPoolBuf and ES_PutPoolBuf
** Revision 1.16 2008/03/19 12:00:27EDT rjmcgraw 
** DCR7016:1 Changed uint to uint32 on line 619
** Revision 1.15 2007/10/05 07:39:17EDT wfmoleski 
** The function pointer argument to CFE_ES_CreateChildTask changed from const void* to
** CFE_ES_ChildTaskMainFuncPtr_t in order to eliminate compiler warnings.
** Revision 1.14 2007/06/05 08:24:50EDT wfmoleski 
** Changes to examples and utf source that reflect the cFE 4.1.0 ES subsystem.
** Revision 1.13 2007/05/25 11:04:18EDT wfmoleski 
** Modifications to mimic the cFE 4.1.0 ES fsw. These changes include new APIs as well as Task info storage.
** Revision 1.12 2006/11/28 12:37:38EST wfmoleski 
** Added the timestamp implementation to the UTF version of CFE_ES_WriteToSysLog
** Revision 1.11 2006/11/14 15:46:34EST wfmoleski 
** Modifications for Restart implementation
** Revision 1.10 2006/11/09 07:46:17EST wfmoleski 
** UTF implementation of the CDS
** Revision 1.9 2006/10/24 07:27:00EDT wfmoleski 
** Modified the constant DEFINES to match the cfe_es.h DEFINES.
** Revision 1.8 2006/10/03 10:53:19EDT wfmoleski 
** Added this implementation to the UTF version of this function.
** Revision 1.7 2006/07/21 14:06:25EDT wfmoleski 
** Added stubs for the performance monitor Entry and Exit functions.
** Revision 1.6 2006/06/22 13:53:34EDT wfmoleski 
** Most of the changes to these files was to add the copyright comments. There were other changes needed
** to bring the UTF up to cFE 3.3
** Revision 1.5 2006/06/05 16:41:40EDT nsschweiss 
** Added comment.
** Revision 1.4 2006/05/26 15:02:39EDT nsschweiss 
** 
** Revision 1.3 2006/04/28 15:01:52EDT nsschweiss 
** Added OS_TaskRegister call to CreateChildTask function.
** Revision 1.2 2006/04/20 11:38:47EDT nsschweiss 
** Updates UTF ES API source to be consistent with cFE 3.2.
** Revision 1.1 2006/03/21 16:44:26EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.4 2005/12/20 16:43:07EST NSSCHWEISS 
** Provided additional utility to allow default api behavior to be restored with procedure call from test driver.
** Revision 1.3 2005/12/01 20:23:06EST NSSCHWEISS 
** Change to macro ids.
** Revision 1.2 2005/12/01 18:20:43EST NSSCHWEISS 
** utf 2.3.0.
** 2005/12/01 nschweiss Change needed to be consistent with identifier
** name change in cfe_es.h.
** 2005/11/23 nschweiss Add in CFE_ES_RegisterChildTask stub definition
**  to resolve link error.
** 2005/10/12 nschweiss Revised CFE_ES_RegisterApp to set state variable.
** Made CFE_ES_GetAppID functional.
** 2005/09/14 Added CFE_ES_RegisterApp().
**
** 2005/08/08 nschweiss
** Incorporated changes for conforming to cFE 2.0.
** DeleteTask became CFE_ES_DeleteChildTask.  
** SuspendTask became SuspendChildTask.
** ResumeTask became ResumeChildTask.
** Names of some return values have changed.
**
** Revision 1.1 2005/06/09 10:57:45EDT rperera 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/es/project.pj
** Revision 1.7  2005/05/05 18:08:10  swalling
** updated api functions to match latest documentation
**
** Revision 1.6  2005/05/04 22:55:03  acudmore
** Updated GetName function
**
** Revision 1.5  2005/05/04 22:49:32  acudmore
** Addded log write function
**
** Revision 1.2  2005/05/03 22:28:04  swalling
** implemented some commands and API functions
**
** Revision 1.1  2005/05/02 22:34:00  swalling
** entry point functions for ES task API
**
*/

/*
** Required header files...
*/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cfe.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"  /* for CFE_ES_AppRecord_t */
#include "cfe_es_cds.h"  /* for CFE_ES_CDSVariables_t */
#include "cfe_es_events.h"
/*#include "cfe_platform_cfg.h"  for CFE_ES_MAX_PROCESSOR_RESETS */
#include "utf_cfe_es.h"
#include "utf_osapi.h" /* for UTF_CFE_USE_DEFAULT_RETURN_CODE*/
#include "utf_types.h" /* for UTF_put_text */

/* UTF Data variables originally in cfe_es_global.h */
static int8 utf_es_app_registered = FALSE;
static uint32 utf_es_appid = 0;
static char utf_es_app_name[OS_MAX_API_NAME];
static CFE_ES_AppRecord_t ES_AppTable[CFE_ES_MAX_APPLICATIONS];
static uint32 utf_ProcessorResetCount = 0;
static uint32 utf_ResetType = 0;
static uint32 utf_ResetSubType = 0;
static uint32 utf_RegisteredTasks;
static CFE_ES_TaskRecord_t ES_TaskTable[OS_MAX_TASKS];
static uint32 utf_es_taskid = 0;
static CFE_ES_AppInfo_t ES_DefaultAppInfo;

CFE_ES_CDSVariables_t CDSVars;

/* Function Hook Stuff */
typedef struct
{
    int32 (*CFE_ES_CreateChildTask) (uint32, const char *, CFE_ES_ChildTaskMainFuncPtr_t, const uint32 *, uint32, uint32, uint32);

} UTF_ES_API_HookTable_t;

UTF_ES_API_HookTable_t UTF_ES_API_HookTable = {
    NULL
};

/******************************************************************************
**  Function: UTF_ES_API_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for an ES function call.
*/
void UTF_ES_API_set_function_hook(int Index, void *FunPtr)
{
    if (Index == CFE_ES_CREATECHILDTASK_HOOK)
    {
      UTF_ES_API_HookTable.CFE_ES_CreateChildTask = FunPtr;
    }
    else
    {
      UTF_error("Invalid ES API Hook Index In Set Hook Call %d", Index);
    }
}


int32 cfe_es_api_return_value[NUM_OF_CFE_ES_API_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/****************************************************************************
** Function: CFE_ES_GetResetType
**
** Purpose:  Return The Type of reset the cFE had.
**           The function will return the start type 
**           which is CFE_ES_POWERON_RESET or CFE_ES_PROCESSOR_RESET.
**           The sub-type is optional and will be returned if a non-NULL pointer
**           is passed in to the function.
****************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
   	/* Handle Preset Return Code */
   	if (cfe_es_api_return_value[CFE_ES_GETRESETTYPE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return cfe_es_api_return_value[CFE_ES_GETRESETTYPE_PROC];
   	}

    if ( ResetSubtypePtr != NULL )
    {
       *ResetSubtypePtr = utf_ResetSubType;
    }

    return(utf_ResetType);
} /* End of CFE_ES_GetResetType() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_ResetCFE() -- xxx       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_ResetCFE(uint32 ResetType)
{
   	/* Handle Preset Return Code */
   	if (cfe_es_api_return_value[CFE_ES_RESETCFE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   	{
		return cfe_es_api_return_value[CFE_ES_RESETCFE_PROC];
   	}

   	if (ResetType == CFE_ES_PROCESSOR_RESET)
   	{
    	if (utf_ProcessorResetCount == CFE_ES_MAX_PROCESSOR_RESETS)
    	{
        	CFE_ES_WriteToSysLog("ES ResetCFE: CFE ES Power On Reset Due to Max Processor Resets.\n");
			utf_ProcessorResetCount = 0;
			utf_ResetType = CFE_ES_POWERON_RESET;
    	}
		else
		{
			CFE_ES_WriteToSysLog("ES ResetCFE: CFE ES Processor Reset called.\n");
			utf_ProcessorResetCount++;
			utf_ResetType = CFE_ES_PROCESSOR_RESET;
		}
   	}
    else if (ResetType == CFE_ES_POWERON_RESET)
    {
    	CFE_ES_WriteToSysLog("ES ResetCFE: CFE ES Power On Reset called.\n");
		utf_ResetType = CFE_ES_POWERON_RESET;
    }
   	else
   	{
	   CFE_ES_WriteToSysLog("CFE_ES_ResetCFE called with bad ResetType argument.\n");   
       return (CFE_ES_BAD_ARGUMENT);
   	}

    return(CFE_SUCCESS);

} /* End of CFE_ES_ResetCFE() */


/*
** Function: CFE_ES_RestartApp
**
** Purpose:  Reset a single cFE App. Can do a WARM or COLD reset.
**
*/
int32 CFE_ES_RestartApp(uint32 AppID)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_RESTARTAPP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_RESTARTAPP_PROC];
   }
    
    int32 ReturnCode = CFE_SUCCESS;
    /*
    ** Check to see if the App is an external cFE App.
    */
    if (ES_AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Restart a CORE Application: %s.\n",
                             ES_AppTable[AppID].StartParams.Name );
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else if (ES_AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_RestartApp: Cannot Restart Application %s, It is not running.\n",
                              ES_AppTable[AppID].StartParams.Name);
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else
    {
       CFE_ES_WriteToSysLog("CFE_ES_RestartApp: Restart Application %s Initiated\n",
                             ES_AppTable[AppID].StartParams.Name);
    }
        
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
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_RELOADAPP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_RELOADAPP_PROC];
   }

    int32 ReturnCode = CFE_SUCCESS;
    
    /*
    ** Check to see if the App is an external cFE App.
    */
    if (ES_AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Reload a CORE Application: %s.\n", 
       		ES_AppTable[AppID].StartParams.Name );
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else if (ES_AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_ReloadApp: Cannot Reload Application %s, It is not running.\n",
                              ES_AppTable[AppID].StartParams.Name);
       ReturnCode = CFE_ES_ERR_APPID; 
    }    
    else
    {    
       CFE_ES_WriteToSysLog("CFE_ES_ReloadApp: Reload Application %s Initiated. New filename = %s\n", 
                             ES_AppTable[AppID].StartParams.Name, AppFileName);
       strncpy((char *)ES_AppTable[AppID].StartParams.FileName, AppFileName, OS_MAX_PATH_LEN);
    }
    
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
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_DELETEAPP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_DELETEAPP_PROC];
   }

    int32 ReturnCode = CFE_SUCCESS;
    
    /*
    ** Check to see if the App is an external cFE App.
    */
    if (ES_AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Delete a CORE Application: %s.\n",
       		ES_AppTable[AppID].StartParams.Name );
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else if (ES_AppTable[AppID].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING )
    {
       CFE_ES_WriteToSysLog ("CFE_ES_DeleteApp: Cannot Delete Application %s, It is not running.\n",
                              ES_AppTable[AppID].StartParams.Name);
       ReturnCode = CFE_ES_ERR_APPID; 
    }
    else
    {
    	CFE_ES_WriteToSysLog("CFE_ES_DeleteApp: Delete Application %s Initiated\n",
                             ES_AppTable[AppID].StartParams.Name);
       	ES_AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_SYS_DELETE;
       	ES_AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
       	ES_AppTable[AppID].StateRecord.AppTimer = CFE_ES_APP_KILL_TIMEOUT;

		/* Set the Application Table entry to unused */
	    ES_AppTable[AppID].RecordUsed = FALSE;
	    
		/* Set the Task Table entry to unused for Main Task */
    	ES_TaskTable[ES_AppTable[AppID].TaskInfo.MainTaskId].RecordUsed = FALSE;

		/* Set the Task Table entries to unused for all Child Tasks of this app */
		if (ES_AppTable[AppID].TaskInfo.NumOfChildTasks > 0)
		{
			int i;
			for (i = 0; i < OS_MAX_TASKS; i++)
      		{
         		if ((ES_TaskTable[i].RecordUsed == TRUE) && 
         	 		(ES_TaskTable[i].AppId == AppID))
                {
         			ES_TaskTable[i].RecordUsed = FALSE;
         	 		ES_TaskTable[i].AppId = 0;
		        }
      		}
        }
    }
        
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

    /*
    ** Get App ID - Be careful not to lock the shared data before calling GetAppID
    */
    ReturnCode = CFE_ES_GetAppID(&AppID);
    if ( ReturnCode == CFE_SUCCESS )
    {
       /*
       ** Check to see if the App is an external cFE App.
       */
       if (ES_AppTable[AppID].Type == CFE_ES_APP_TYPE_CORE )
       {
          /*
          ** A core app should only call this function with one of two ExitStatus codes.
          */
          if ( ExitStatus == CFE_ES_CORE_APP_INIT_ERROR )
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp: CORE Application %s Had an Init Error.\n",
                                     ES_AppTable[AppID].StartParams.Name );
              
          }
          else if ( ExitStatus == CFE_ES_CORE_APP_RUNTIME_ERROR )
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp: CORE Application %s Had a Runtime Error.\n",
                                     ES_AppTable[AppID].StartParams.Name );  
          }
          else 
          {
              CFE_ES_WriteToSysLog ("CFE_ES_ExitApp, Cannot Exit CORE Application %s\n",
                                    ES_AppTable[AppID].StartParams.Name );         
          }
       }
       else /* It is an external App */
       {
          CFE_ES_WriteToSysLog ("Application %s called CFE_ES_ExitApp\n",
                                 ES_AppTable[AppID].StartParams.Name);
          ES_AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
       } /* end if */
   } /* end if ReturnCode == CFE_SUCCESS */

} /* End of CFE_ES_ExitApp() */

/*
** Function: CFE_ES_RunLoop
**
** Purpose:  Check the Run Status of an Application.
**
*/
int32 CFE_ES_RunLoop(uint32 *RunStatus)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_RUNLOOP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_RUNLOOP_PROC];
   }

    int32  ReturnCode = TRUE;
    uint32 AppID;
    int32  TaskID;
    /*
    ** Validate RunStatus
    */
    if ( *RunStatus == CFE_ES_APP_RUN || *RunStatus == CFE_ES_APP_EXIT || *RunStatus == CFE_ES_APP_ERROR )
    {
       /*
       ** Get App ID
       */
       ReturnCode = CFE_ES_GetAppID(&AppID);
    
       /*
       ** Look up the system control request
       */
       if ( ReturnCode == CFE_SUCCESS )
       {
       	  TaskID = UTF_ES_GetTaskIDByAppID(AppID);
	      /*
           ** Increment the execution counter for the main task
          */
          ES_TaskTable[TaskID].ExecutionCounter++;
       	         
          if ( *RunStatus == CFE_ES_APP_RUN )
          {
             if (ES_AppTable[AppID].StateRecord.AppControlRequest != CFE_ES_APP_RUN )
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
             ES_AppTable[AppID].StateRecord.AppControlRequest = *RunStatus; 
             ReturnCode = FALSE;
          }
       }
       else 
       {
          CFE_ES_WriteToSysLog("CFE_ES_RunLoop Error: Cannot get AppID for the caller: RC = %08X\n",ReturnCode);
          ReturnCode = FALSE;
         
       } /* end if ReturnCode == CFE_SUCCESS */

    }
    else
    {
       CFE_ES_WriteToSysLog("CFE_ES_RunLoop Error: Invalid RunStatus:%d!\n",*RunStatus);
       ReturnCode = FALSE;
       
    } /* End if *RunStatus == CFE_ES_APP_RUN .. */
    
    return(ReturnCode);

} /* End of CFE_ES_RunLoop() */

/*
** Function: CFE_ES_WaitForStartupSync
**
** Purpose:  Waits for all of the applications in the CFE startup table to initialize before
**           continuing.
**           Since the UTF is a single-thread application test framework, this 
**           function does not do anything since there are no other apps to wait for.
**
*/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_RegisterApp()--this will be the first thing every sub-system calls*/
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_RegisterApp(void)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_REGISTERAPP_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_REGISTERAPP_PROC];
   }
   utf_es_app_registered = TRUE;	
   return(CFE_SUCCESS);
}

/*
** Function: CFE_ES_GetAppIDByName
**
** Purpose:  Given a valid App Name, return the 
**           Application ID.
**
*/
int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, char *AppName)
{
   int32 Result = CFE_ES_ERR_APPNAME;
   int   i;

   if ( AppName != NULL && AppIdPtr != NULL )
   {
      /*
      ** Search the ES Application table for an app with a matching name.
      */
      for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
      {
         if ( ES_AppTable[i].RecordUsed == TRUE )
         {
            if ( strcmp(AppName, ES_AppTable[i].StartParams.Name) == 0 )
            {
               *AppIdPtr = i;
               Result = CFE_SUCCESS;
               break;
            }
         }
      } /* end for */
   }
   else
   {
      Result = CFE_ES_ERR_BUFFER;
   }
   return(Result);

} /* End of CFE_ES_GetAppIDByName() */

int32 UTF_ES_InitAppRecords(void)
{
	int i;
	  
	for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
    {
    	ES_AppTable[i].RecordUsed = FALSE;
    }

	ES_DefaultAppInfo.Type = CFE_ES_APP_TYPE_EXTERNAL;
    ES_DefaultAppInfo.ExceptionAction = 0;
    ES_DefaultAppInfo.Priority = 25;
    ES_DefaultAppInfo.StackSize = 8192;
    ES_DefaultAppInfo.AddressesAreValid = FALSE;
    ES_DefaultAppInfo.CodeAddress = 0;
    ES_DefaultAppInfo.CodeSize = 0;
    ES_DefaultAppInfo.DataAddress = 0;
    ES_DefaultAppInfo.DataSize = 0;
    ES_DefaultAppInfo.BSSAddress = 0;
    ES_DefaultAppInfo.BSSSize = 0;

	return CFE_SUCCESS;	
}

/*
** Function: UTF_ES_AddAppRecord
**
** Purpose:  Add record to application table ES_AppTable which is accessed by
** CFE_ES_GetAppIDByName and CFE_ES_GetAppName.
**
*/
int32 UTF_ES_AddAppRecord(char *AppName, uint32 AppId)
{
	int32 returnStatus = CFE_SUCCESS;
	
    /* Check argument validity */
	if ( strlen (AppName) == 0  )
   	{
      UTF_put_text("UTF Info: Null string passed to UTF_ES_AddAppRecord\n");
 	  return CFE_ES_ERR_APPNAME;
   	}
   
	if (strlen (AppName) >= OS_MAX_API_NAME)
	{
		UTF_put_text("UTF Info: App name <%s> is too long for App Tbl. (Max %d chars incl. null terminator.)\n",
		AppName, OS_MAX_API_NAME);
		return CFE_ES_ERR_APPNAME;
	}
	
	if (AppId >= CFE_ES_MAX_APPLICATIONS)
	{
		UTF_put_text("UTF Info: UTF_ES_AddAppRecord App Id arg <%d> is invalid.\n",AppId);
		return -1;
	}
	
    /*
    ** Initialize the ES_AppTable record.
    */
    if (ES_AppTable[AppId].RecordUsed == TRUE)
    {
      	UTF_put_text("UTF Info: Overwriting ES App Table Record\n");
    }
     
    strcpy(ES_AppTable[AppId].StartParams.Name, AppName);
    strcpy(ES_AppTable[AppId].TaskInfo.MainTaskName, AppName);
    ES_AppTable[AppId].RecordUsed = TRUE;
    ES_AppTable[AppId].Type = ES_DefaultAppInfo.Type;
    ES_AppTable[AppId].StartParams.ExceptionAction = ES_DefaultAppInfo.ExceptionAction;
    ES_AppTable[AppId].StartParams.Priority = ES_DefaultAppInfo.Priority;
    ES_AppTable[AppId].StartParams.StackSize = ES_DefaultAppInfo.StackSize;
    ES_AppTable[AppId].TaskInfo.NumOfChildTasks = 0;
    ES_AppTable[AppId].StateRecord.AppControlRequest = CFE_ES_APP_RUN;
    ES_AppTable[AppId].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    ES_AppTable[AppId].StateRecord.AppTimer = 0;

    /*
    ** Search the ES_TaskTable for this AppId and TaskName
    */
    int32 taskIndex = UTF_ES_FindTaskRecord(AppName,AppId);
	if (taskIndex == -1)
    {
	    /*
    	** Get an available ES_TaskTable entry
    	*/
    	taskIndex = UTF_ES_GetFreeTaskRecord();
    	if (taskIndex == -1)
    	{
 	     	UTF_put_text("UTF Error: The Task Table is full.\n");
	    }
    }
    uint32 TaskId;
    if (taskIndex != -1)
    {
    	TaskId = (uint32)taskIndex;
	    returnStatus = UTF_ES_AddTaskRecord(AppName,AppId,TaskId);
	    if (returnStatus == CFE_SUCCESS)
	    {
			ES_AppTable[AppId].TaskInfo.MainTaskId = TaskId;
	    }
    }    
    if (returnStatus == CFE_SUCCESS)
	    CFE_ES_WriteToSysLog("ES Startup: %s loaded and created\n", AppName);

    UTF_put_text("UTF Info: App <%s> added to App Tbl.\n",AppName);

    return(returnStatus);
	
} /* end UTF_ES_AddAppRecord */

/*
** Function: UTF_ES_DumpAppRecords
**
** Purpose:  Dumps the ES_AppTable.
**
*/
void UTF_ES_DumpAppRecords(void)
{
	int i;	  
	  
	for (i = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
    {
		if (ES_AppTable[i].RecordUsed == TRUE)
        {
	   	 	UTF_put_text("AppTable[%d]:\n",i);
	   	 	UTF_put_text("       Name    = %s\n",ES_AppTable[i].StartParams.Name);
	   	 	UTF_put_text("    ChildTasks = %d\n",ES_AppTable[i].TaskInfo.NumOfChildTasks);
    		if (ES_AppTable[i].StateRecord.AppState == CFE_ES_APP_STATE_RUNNING)
    		{
	   	 		UTF_put_text("    State = Running\n");
    		}
			else if (ES_AppTable[i].StateRecord.AppState == CFE_ES_APP_STATE_WAITING)
    		{
	   	 		UTF_put_text("    State = Waiting\n");
    		}
			else if (ES_AppTable[i].StateRecord.AppState == CFE_ES_APP_STATE_STOPPED)
    		{
	   	 		UTF_put_text("    State = Stopped\n");
    		}
        }
    }
}

/*
** Function: UTF_ES_InitTaskRecords
**
** Purpose:  Initializes the ES_TaskTable which is accessed by CFE_ES_GetTaskInfo.
**
*/
int32 UTF_ES_InitTaskRecords(void)
{
	  int i;
	  
	  for ( i = 0; i < OS_MAX_TASKS; i++ )
      {
         ES_TaskTable[i].RecordUsed = FALSE;
      }
      
      utf_RegisteredTasks = 0;
	
	return CFE_SUCCESS;	
}

/*
** Function: UTF_ES_AddTaskRecord
**
** Purpose:  Add record to task table ES_TaskTable which is accessed 
**			 by CFE_ES_GetTaskInfo.
**
*/
int32 UTF_ES_AddTaskRecord(char *TaskName, uint32 AppId, uint32 TaskId)
{
   /* Check argument validity */
	if ( strlen (TaskName) == 0  )
   	{
      UTF_put_text("UTF Info: Null string passed to UTF_ES_AddTaskRecord\n");
 	  return CFE_ES_ERR_APPNAME;
   	}
   
	if (strlen (TaskName) >= OS_MAX_API_NAME)
	{
		UTF_put_text("UTF Info: Task name <%s> is too long for App Tbl. (Max %d chars incl. null terminator.)\n",
			TaskName, OS_MAX_API_NAME);
		return CFE_ES_ERR_APPNAME;
	}
	
	if (AppId >= CFE_ES_MAX_APPLICATIONS)
	{
		UTF_put_text("UTF Info: UTF_ES_AddTaskRecord App Id arg <%d> is invalid.\n",AppId);
		return -1;
	}	

	if (TaskId >= OS_MAX_TASKS)
	{
		UTF_put_text("UTF Info: UTF_ES_AddTaskRecord Task Id arg <%d> is invalid.\n",AppId);
		return -1;
	}	

     /*
     ** Update or Add ES_TaskTable record.
     */
    if (ES_TaskTable[TaskId].RecordUsed == TRUE)
    {
      	UTF_put_text("UTF Info: Overwriting ES Task Table Record[%d]\n", TaskId);
	    ES_TaskTable[TaskId].AppId = AppId;
    	ES_TaskTable[TaskId].TaskId = TaskId;
    	ES_TaskTable[TaskId].ExecutionCounter = 0;
    }
	else
	{
		strcpy(ES_TaskTable[TaskId].TaskName, TaskName);
    	ES_TaskTable[TaskId].RecordUsed = TRUE;
    	ES_TaskTable[TaskId].AppId = AppId;
   		ES_TaskTable[TaskId].TaskId = TaskId;
    	ES_TaskTable[TaskId].ExecutionCounter = 0;
    	utf_RegisteredTasks++;
    	UTF_put_text("UTF Info: Task <%s> added to Task Tbl.\n",TaskName);
	}
	
    return(CFE_SUCCESS);
	
} /* end UTF_ES_AddTaskRecord */

/*
** Function: UTF_ES_GetFreeTaskRecord
**
** Purpose:  Initializes the ES_TaskTable which is accessed by CFE_ES_GetTaskInfo.
**
*/
int32 UTF_ES_GetFreeTaskRecord(void)
{
	  int i;
	  int32 slotFree=-1;
	  
	  for ( i = 0; i < OS_MAX_TASKS; i++ )
      {
         if (ES_TaskTable[i].RecordUsed == FALSE)
         {
         	slotFree = i;
         	break;
         }
      }
      
	return slotFree;	
}

/*
** Function: UTF_ES_FindTaskRecord
**
** Purpose:  Searches the ES_TaskTable for the specified Name & AppId.
**
*/
int32 UTF_ES_FindTaskRecord(const char *Name, uint32 AppId)
{
	  int i;
	  int32 entryIndex=-1;
	  
	  for ( i = 0; i < OS_MAX_TASKS; i++ )
      {
         if ((ES_TaskTable[i].RecordUsed == TRUE) && 
         	 (ES_TaskTable[i].AppId == AppId) &&
             (strcmp(Name, ES_TaskTable[i].TaskName) == 0))
         {
         	entryIndex = i;
         	break;
         }
      }
      
	return entryIndex;	
}

/*
** Function: UTF_ES_GetTaskIDByAppID
**
** Purpose:  Returns the ES_TaskTable entry index for the specified AppID.
**
*/
int32 UTF_ES_GetTaskIDByAppID(uint32 appID)
{
	  int i;
	  int32 idFound=-1;
	  
	  for (i = 0; i < OS_MAX_TASKS; i++ )
      {
         if (ES_TaskTable[i].AppId == appID)
         {
         	idFound = ES_TaskTable[i].TaskId;
         	break;
         }
      }
      
	return idFound;	
}

/*
*
** Function: UTF_ES_GetTaskIDByAppID
**
** Purpose:  Returns the ES_TaskTable entry index for the specified AppID.
**
*/
int32 UTF_ES_GetTaskIDByName(const char *TaskName)
{
	  int i;
	  int32 idFound=-1;
	  
	  for (i = 0; i < OS_MAX_TASKS; i++ )
      {
         if ( ES_TaskTable[i].RecordUsed == TRUE )
         {
            if ( strcmp(TaskName, ES_TaskTable[i].TaskName) == 0 )
            {
         		idFound = ES_TaskTable[i].TaskId;
         		break;
            }
         }
      }
      
	return idFound;	
}

/*
** Function: UTF_ES_DumpTaskRecords
**
** Purpose:  Dumps the ES_TaskTable by calling CFE_ES_GetTaskInfo.
**
*/
void UTF_ES_DumpTaskRecords(void)
{
	  int i;	  
	  
	  for (i = 0; i < OS_MAX_TASKS; i++)
      {
         if (ES_TaskTable[i].RecordUsed == TRUE)
         {
	   	 	UTF_put_text("TaskTable[%d]:\n",i);
	   	 	UTF_put_text("    AppID  = %d\n",ES_TaskTable[i].AppId);
	   	 	UTF_put_text("    TaskID = %d\n",ES_TaskTable[i].TaskId);
	   	 	UTF_put_text("    Name   = %s\n",ES_TaskTable[i].TaskName);
	   	 	UTF_put_text("  Exe Ctr  = %d\n",ES_TaskTable[i].ExecutionCounter);
         }
      }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetAppID() -- Set the local task id    .                     */
/* Needed for CFE_ES_ExitChildTask.                                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_ES_SetTaskID(uint32 taskId)
{
	utf_es_taskid = taskId;
	return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_GetAppID() -- query application ID from application name   */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_GetAppID(uint32 *AppIdPtr)
{
   int32 Result;
 
   /* Check that task has been registered with Executive Services */
   if (!utf_es_app_registered )
   {
   	  /* If Application hasn't been registered, notify user   */
   	   UTF_put_text("UTF WARNING: Task must call CFE_ES_RegisterApp before calling CFE_ES_GetAppID.\n");
   	   Result = CFE_ES_ERR_APPID;  	
   }
   
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_GETAPPID_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_GETAPPID_PROC];
   }

   /* As in operational code, check for proper input */
    if (AppIdPtr == NULL)
   {
   	  /* If input is invalid, notify user            */
   	   UTF_put_text("UTF WARNING: CFE_ES_GetAppID argument = NULL\n");
   	   Result = CFE_ES_ERR_BUFFER;
   }
   else
   {
   		/* if pointer argument is valid, use it to assign an id. */
   		/* 0 will be the number used when ES_AppCreate is called */
   		/* in test driver.      */
   		*AppIdPtr = utf_es_appid;
   		Result = CFE_SUCCESS;  		
   }   
 
   return(Result);

} /* End of CFE_ES_GetAppID() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetAppID() -- Set app id for local task.                     */
/* Accessible by  CFE_ES_GetAppID.                                         */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetAppID(uint32 AppId)
{
	utf_es_appid = AppId;
	return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetAppName() -- Use to set AppName in UTF                    */
/* CURRENTLY, ONCE SET, utf_es_app_name IS NOT RETRIEVABLE OR USED BY      */
/* ANY FUNCTION.  THAT MAKES THIS FUNCTION NO LONGER USEFUL AND IT SHOULD  */
/* PROBABLY BE DELETED.                                                    */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_ES_SetAppName(char *Name)
{
	int32 Result;

   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_SETAPPNAME_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_SETAPPNAME_PROC];
   }
	strncpy(utf_es_app_name, Name, OS_MAX_API_NAME);
	Result = CFE_SUCCESS;
	return Result;
}


/*
** Function: CFE_ES_GetAppName
**
** Purpose:  Given a valid AppID, return the 
**           Application name string.
**
*/
int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength)
{
   uint32 Result;

   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_GETAPPNAME_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_GETAPPNAME_PROC];
   }


   if ( AppName != NULL )
   {
      if ( AppId < CFE_ES_MAX_APPLICATIONS )
      {
         if ( ES_AppTable[AppId].RecordUsed == TRUE )
         {
            strncpy(AppName, ES_AppTable[AppId].StartParams.Name, BufferLength);
            Result = CFE_SUCCESS;
         }
         else
         {
         	UTF_put_text("AppId %d is not in App Tbl.\n", AppId);
            Result = CFE_ES_ERR_APPID;
         }
      }
      else
      {
      	UTF_put_text("AppId %d exceeds max value %d.\n", 
      					AppId, CFE_ES_MAX_APPLICATIONS);
         Result = CFE_ES_ERR_APPID;
      }
   }
   else
   {
   		UTF_put_text("Invalid Null char pointer passed to CFE_ES_GetAppName\n");
      	Result = CFE_ES_ERR_BUFFER;
   }

   return(Result);

} /* End of CFE_ES_GetAppName() */

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_GetAppInfoInternal
**
**   Purpose: Populate the cFE_ES_AppInfo structure with the data for an app.
**            The ModuleInfo is taken from the ES_DefaultAppInfo structure. Thus,
**            the tester should populate this structure with relevant data prior
**            to calling CFE_ES_GetAppInfo function.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_GetAppInfoInternal(uint32 AppId, CFE_ES_AppInfo_t *AppInfoPtr )
{
/*   int32              ReturnCode;
   OS_module_record_t ModuleInfo; */
   
   AppInfoPtr->AppId = AppId;
   AppInfoPtr->Type = ES_AppTable[AppId].Type;
   strncpy((char *)AppInfoPtr->Name, (char *)ES_AppTable[AppId].StartParams.Name, OS_MAX_API_NAME);
   AppInfoPtr->Name[OS_MAX_API_NAME - 1] = '\0';

   strncpy((char *)AppInfoPtr->EntryPoint, (char *)ES_AppTable[AppId].StartParams.EntryPoint, OS_MAX_API_NAME);
   AppInfoPtr->EntryPoint[OS_MAX_API_NAME - 1] = '\0';
   
   strncpy((char *)AppInfoPtr->FileName, (char *)ES_AppTable[AppId].StartParams.FileName, OS_MAX_PATH_LEN);
   AppInfoPtr->FileName[OS_MAX_PATH_LEN - 1] = '\0';
   
   AppInfoPtr->ModuleId = ES_AppTable[AppId].StartParams.ModuleId;
   AppInfoPtr->StackSize = ES_AppTable[AppId].StartParams.StackSize;
   AppInfoPtr->StartAddress = ES_AppTable[AppId].StartParams.StartAddress;
   AppInfoPtr->ExceptionAction = ES_AppTable[AppId].StartParams.ExceptionAction;
   AppInfoPtr->Priority = ES_AppTable[AppId].StartParams.Priority;
 
   AppInfoPtr->MainTaskId = ES_AppTable[AppId].TaskInfo.MainTaskId;   
   strncpy((char *)AppInfoPtr->MainTaskName, (char *)ES_AppTable[AppId].TaskInfo.MainTaskName, OS_MAX_API_NAME);
   AppInfoPtr->MainTaskName[OS_MAX_API_NAME - 1] = '\0';
   AppInfoPtr->NumOfChildTasks = ES_AppTable[AppId].TaskInfo.NumOfChildTasks;

   /*
   ** Get the execution counter for the main task
   */
   AppInfoPtr->ExecutionCounter = ES_TaskTable[AppInfoPtr->MainTaskId].ExecutionCounter;


   /* Use the ES_DefaultAppInfo variable to populate the following values */
   AppInfoPtr->AddressesAreValid = ES_DefaultAppInfo.AddressesAreValid;
   AppInfoPtr->CodeAddress = ES_DefaultAppInfo.CodeAddress;
   AppInfoPtr->CodeSize = ES_DefaultAppInfo.CodeSize;
   AppInfoPtr->DataAddress = ES_DefaultAppInfo.DataAddress;
   AppInfoPtr->DataSize = ES_DefaultAppInfo.DataSize;
   AppInfoPtr->BSSAddress = ES_DefaultAppInfo.BSSAddress;
   AppInfoPtr->BSSSize = ES_DefaultAppInfo.BSSSize;

} /* End of CFE_ES_GetAppInfoInternal */

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
         if ( ES_AppTable[AppId].RecordUsed == TRUE )
         {
            CFE_ES_GetAppInfoInternal(AppId, AppInfo);
            ReturnCode = CFE_SUCCESS;
         }
         else
         {
            CFE_ES_WriteToSysLog("CFE_ES_GetAppInfo: App ID Not Active: %d\n",AppId);
            ReturnCode = CFE_ES_ERR_APPID;
         }
      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_GetAppInfo: App ID Exceeds CFE_ES_APPLICATION_MAX: %d\n",AppId);
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetDefaultAppInfo() -- Set the default app info.             */
/* Accessible by  CFE_ES_GetAppInfo & UTF_ES_AddAppRecord.                 */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetDefaultAppInfo(CFE_ES_AppInfo_t AppInfo)
{
	ES_DefaultAppInfo.Type = AppInfo.Type;
    ES_DefaultAppInfo.ExceptionAction = AppInfo.ExceptionAction;
    ES_DefaultAppInfo.Priority = AppInfo.Priority;
    ES_DefaultAppInfo.StackSize = AppInfo.StackSize;
	
    ES_DefaultAppInfo.AddressesAreValid = AppInfo.AddressesAreValid;
    ES_DefaultAppInfo.CodeAddress = AppInfo.CodeAddress;
    ES_DefaultAppInfo.CodeSize = AppInfo.CodeSize;
    ES_DefaultAppInfo.DataAddress = AppInfo.DataAddress;
    ES_DefaultAppInfo.DataSize = AppInfo.DataSize;
    ES_DefaultAppInfo.BSSAddress = AppInfo.BSSAddress;
    ES_DefaultAppInfo.BSSSize = AppInfo.BSSSize;
    
    return CFE_SUCCESS;
}

/*
** Function: CFE_ES_GetTaskInfo
**
** Purpose:  Get Task Information given a specified Task ID
**
*/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{
   int32  ReturnCode = CFE_SUCCESS;

   if ( TaskId < OS_MAX_TASKS )
   {
      if (ES_TaskTable[TaskId].RecordUsed == TRUE )
      {
         /*
         ** Get the Application ID and Task Name
         */  
         TaskInfo->AppId = ES_TaskTable[TaskId].AppId;
         strncpy((char *)TaskInfo->TaskName, 
                 (char *)ES_TaskTable[TaskId].TaskName,OS_MAX_API_NAME);
         TaskInfo->TaskName[OS_MAX_API_NAME - 1] = '\0';  
         TaskInfo->ExecutionCounter =  ES_TaskTable[TaskId].ExecutionCounter;

         /*
         ** Get the Application Name
         */
         if (ES_AppTable[TaskInfo->AppId].RecordUsed == TRUE )
         {
            strncpy((char *)TaskInfo->AppName, 
                    (char *)ES_AppTable[TaskInfo->AppId].StartParams.Name, 
                    OS_MAX_API_NAME);
            TaskInfo->AppName[OS_MAX_API_NAME - 1] = '\0';
            ReturnCode = CFE_SUCCESS;
         }
         else
         {
            CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID:%d Parent App ID:%d not Active.\n",
               TaskId,TaskInfo->AppId);
            ReturnCode = CFE_ES_ERR_TASKID;
         }
      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID Not Active: %d\n",TaskId);
         ReturnCode = CFE_ES_ERR_TASKID;
      }
   }
   else
   {
         CFE_ES_WriteToSysLog("CFE_ES_GetTaskInfo: Task ID Exceeds OS MAX: %d\n",TaskId);
         ReturnCode = CFE_ES_ERR_TASKID;
   }
      
   return(ReturnCode);
   
} /* End of CFE_ES_GetTaskInfo() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_CreateChildTask() -- xxx       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_CreateChildTask(uint32 *TaskIdPtr, const char *TaskName,
                             CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr,
                             const uint32 *StackPtr,
                             uint32 StackSize,
                             uint32 Priority,
                             uint32 Flags)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_CREATECHILDTASK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_CREATECHILDTASK_PROC];
   }

   /* Handle Function Hook */
   if (UTF_ES_API_HookTable.CFE_ES_CreateChildTask)
      return(UTF_ES_API_HookTable.CFE_ES_CreateChildTask(TaskIdPtr,TaskName,FunctionPtr,StackPtr,StackSize,Priority,Flags));

   int32          Result;
   uint32         AppId= 0xFFFFFFFF;
   int32  ReturnCode = CFE_SUCCESS;

   /*
   ** Validate some of the arguments
   */
   if ( TaskIdPtr == NULL )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Task Id Pointer Parameter is NULL.\n");
      ReturnCode = CFE_ES_BAD_ARGUMENT;
   }
   else if ( TaskName == NULL )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: TaskName Parameter is NULL.\n");
      ReturnCode = CFE_ES_BAD_ARGUMENT;
   }
/*   else if ( FunctionPtr == NULL )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Function Pointer Parameter is NULL.\n");
      ReturnCode = CFE_ES_BAD_ARGUMENT;
   } */
   else 
   {
      /*
      ** Get the AppID of the calling Application
      */
      Result = CFE_ES_GetAppID(&AppId);
      if (Result != CFE_SUCCESS)
      {
          CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Error calling CFE_ES_GetAppID. RC = 0x%08X\n",Result);
          ReturnCode = Result;
      }
      else  /* else AppId is valid */
      {
         /*
         ** First, Make sure the Calling Task is a cFE Main task.
         ** TaskID must be the same as the Parent Task ID.
		 ** For the UTF, we are going to assume that the calling task is
		 ** the Main Application task
		 */
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
		/* Need to find the next available slot in the Task Table */
		Result = UTF_ES_GetFreeTaskRecord();
		if (Result != -1)
		{
			*TaskIdPtr = Result;
			/* Attempt to add the Task information in the ES_TaskTable */
			Result = UTF_ES_AddTaskRecord((char *)TaskName,AppId,*TaskIdPtr);

        	/*
        	** Step 3: Record the task information in the task table
        	*/
        	if ( Result == OS_SUCCESS )
        	{
           		ES_TaskTable[*TaskIdPtr].RecordUsed = TRUE;
           		ES_TaskTable[*TaskIdPtr].AppId = AppId;
           		ES_TaskTable[*TaskIdPtr].TaskId = *TaskIdPtr;
           		strncpy((char *)ES_TaskTable[*TaskIdPtr].TaskName,TaskName,OS_MAX_API_NAME);
           		ES_TaskTable[*TaskIdPtr].TaskName[OS_MAX_API_NAME - 1] = '\0';
           		utf_RegisteredTasks++;

           		/*
           		** Increment the "Registered" child task count for the App
           		*/
           		ES_AppTable[AppId].TaskInfo.NumOfChildTasks++;

           		ReturnCode = CFE_SUCCESS;
         	}
         	else
         	{
           		CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Error calling OS_TaskCreate RC = 0x%08X\n",Result);
           		ReturnCode = CFE_ES_ERR_CHILD_TASK_CREATE;
         	}
		}
        else
        {
           CFE_ES_WriteToSysLog("CFE_ES_CreateChildTask: Could not find a free task entry\n");
           ReturnCode = CFE_ES_ERR_CHILD_TASK_CREATE;
        }
         
      }/* end If AppID is valid */

   } /* end if parameter checking */
	
	return ReturnCode;

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
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_REGISTERCHILDTASK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_REGISTERCHILDTASK_PROC];
   }

   int32 Result;
   int32 ReturnCode;

   /*
   ** Right now, only OS task registration is required
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
   int32 TaskId;

   TaskId = OS_TaskGetId();
   
   ES_TaskTable[TaskId].ExecutionCounter++;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_DeleteTask() -- xxx       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_DeleteChildTask(uint32 TaskId)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_DELETECHILDTASK_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_DELETECHILDTASK_PROC];
   }

    uint32  i;
    uint32  TaskIsMain = FALSE;
    int32   ReturnCode = CFE_SUCCESS;
    int32   OSReturnCode;

    /*
    ** Make sure the task ID is within range
    */
    if (TaskId < OS_MAX_TASKS )
    {
       /*
       ** Make sure the task is active/valid
       */
       if (ES_TaskTable[TaskId].RecordUsed == TRUE )
       {
          /*
          ** Search for this task ID in the ES App Table to make sure
          ** it is not a cFE App Main Task
          */
          TaskIsMain = FALSE;
          for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
          {
             if (ES_AppTable[i].RecordUsed == TRUE )
             {
                if (ES_AppTable[i].TaskInfo.MainTaskId == TaskId )
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
             OSReturnCode = OS_TaskDelete(TaskId);
             if ( OSReturnCode == OS_SUCCESS )
             {
                /*
                ** Invalidate the task table entry
                */
                ES_TaskTable[TaskId].RecordUsed = FALSE;
                utf_RegisteredTasks--;

                /*
                ** Report the task delete
                */
                CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Task %d Deleted\n",TaskId );
                ReturnCode = CFE_SUCCESS;
             }
             else
             {
                CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Error Calling OS_TaskDelete: Task %d, RC = 0x%08X\n",
                                     TaskId, OSReturnCode);
                ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE;
             }
          }
          else
          {
             /*
             ** Error: The task is a cFE Application Main task
             */
             CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Task %d is a cFE Main Task.\n",TaskId );
             ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK;
          } /* end if TaskMain == FALSE */
       }
       else
       {
          /*
          ** Task ID is not in use, so it is invalid
          */
          CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Task ID is not active: %d\n",TaskId );
          ReturnCode = CFE_ES_ERR_TASKID;
   
       } /* end if */
       
    }
    else
    {
       /*
       ** Task ID is invalid ( too large )
       */
       CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Invalid Task ID: %d\n",TaskId );
       ReturnCode = CFE_ES_ERR_TASKID;
       
    }
    return(ReturnCode);

} /* End of CFE_ES_DeleteChildTask() */

/*
** Function: CFE_ES_ExitChildTask
**
** Purpose:  Stop execution of a child task.
**
*/
void CFE_ES_ExitChildTask(void)
{
   int32 TaskId;
   uint32 ParentTaskId;
   uint32 AppId; 
   uint32 ReturnCode;

   /*
   ** Check to see if this is being called from a cFE Application's 
   ** main task.
   */
   TaskId = utf_es_taskid;
   
   ReturnCode = CFE_ES_GetAppID(&AppId);
   if (ReturnCode == CFE_SUCCESS )
   {
      ParentTaskId = ES_AppTable[AppId].TaskInfo.MainTaskId;
      if ( TaskId != ParentTaskId )
      {
         CFE_ES_WriteToSysLog("Child Task %d exited.\n",TaskId );  
         
         /*
         ** Invalidate the task table entry
         */
         ES_TaskTable[TaskId].RecordUsed = FALSE;
         utf_RegisteredTasks--;
      }
      else
      {
         CFE_ES_WriteToSysLog("CFE_ES_ExitChildTask Error: Cannot Call from a cFE App Main Task. ID = %d\n",TaskId );
      }
   }
   else
   {   
      CFE_ES_WriteToSysLog("CFE_ES_ExitChildTask Error Calling CFE_ES_GetAppID. Task ID = %d, RC = 0x%08X\n",
                           TaskId, ReturnCode );
   } /* end if GetAppId */
   
} /* End of CFE_ES_ExitChildTask() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* CFE_ES_WriteToSysLog() -- write entry to system text message log        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    va_list ArgPtr;
    char    TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    char    MsgWithoutTime[CFE_EVS_MAX_MESSAGE_LENGTH];
/*    char    TmpString[128]; */
    int     TmpStringLen;
/*    char    LogString[128]; */
    char    LogString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    int     LogStringLen;

   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_WRITETOSYSLOG_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_WRITETOSYSLOG_PROC];
   }

    /* write the current time into the TmpString buffer */
    CFE_TIME_Print(TmpString, CFE_TIME_GetTime());

    /* Form a single string from arguments */
    va_start(ArgPtr, SpecStringPtr);
/*    vsnprintf(TmpString, SpecStringPtr, ArgPtr); */
/*    vsnprintf(TmpString, 128, SpecStringPtr, ArgPtr);*/
    (void) vsnprintf(MsgWithoutTime,(size_t) CFE_EVS_MAX_MESSAGE_LENGTH, SpecStringPtr, ArgPtr);
    va_end(ArgPtr);

    /* insert space between the time and the start of the message */
    strcat(TmpString," ");

    /* Add the message to the time string in the TmpString buffer */
    strncat(TmpString,MsgWithoutTime,CFE_EVS_MAX_MESSAGE_LENGTH);

    TmpStringLen = strlen(TmpString);

	/* Place SysLog indicator at beginning of output string to  */
	/* differentiate output from other UTF messages.            */
	strcpy(LogString, "SysLog:");
	LogStringLen = strlen(LogString);

	/* Will entire TmpString fit into output string? */
/*	if (TmpStringLen <= 128 - LogStringLen -1) */
	if (TmpStringLen <= CFE_ES_MAX_SYSLOG_MSG_SIZE - LogStringLen -1)
	{	/* If yes, concatenate entire string */
		strcat(LogString,TmpString);
		/*LogString[LogStringLen + TmpStringLen]='\n';*/
	}
	else
	{	/* If no, concatenate truncated string */
/*		strncat(LogString, TmpString, 128 - LogStringLen -1);  */
		strncat(LogString, TmpString, CFE_ES_MAX_SYSLOG_MSG_SIZE - LogStringLen -1);  
		LogString[128 - 2]='\n';		
	}	
	
	/* Output completed format string */
	UTF_put_text(LogString);

    return CFE_SUCCESS;

} /* End of CFE_ES_WriteToSysLog() */


/*
** Function: CFE_ES_RegisterDriver
**
** Purpose:  Initialize a cFE device driver.
**
*/
int32 CFE_ES_RegisterDriver(uint32 *DriverIdPtr, CFE_ES_DeviceDriver_t *DriverPtr)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_REGISTERDRIVER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_REGISTERDRIVER_PROC];
   }

   CFE_ES_WriteToSysLog("CFE ES Register Driver not Implemented\n");

    return(CFE_ES_NOT_IMPLEMENTED);

} /* End of CFE_ES_RegisterDriver() */


/*
** Function: CFE_ES_UnloadDriver
**
** Purpose:  Unload a cFE device driver.
**
*/
int32 CFE_ES_UnloadDriver(uint32 DriverId)
{
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_UNLOADDRIVER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_UNLOADDRIVER_PROC];
   }
    
	CFE_ES_WriteToSysLog("CFE ES Unload Driver not Implemented\n");

    return(CFE_ES_NOT_IMPLEMENTED);

} /* End of CFE_ES_UnloadDriver() */
   

/*
** Function: CFE_ES_CalculateCRC
**
** Purpose:  Perform a CRC calculation on a range of memory.
**
*/
uint32 CFE_ES_CalculateCRC(void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC)
{
	int32  i;
    int16  Index;
    int16  Crc = 0;
    uint8 *BufPtr;

    const uint16 CrcTable[256]=
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
	
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_CALCULATECRC_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_CALCULATECRC_PROC];
   }

    switch(TypeCRC)
    {
    case CFE_ES_CRC_32:
         CFE_ES_WriteToSysLog("CFE ES Calculate CRC32 not Implemented\n");
         break;

    case CFE_ES_CRC_16:

         Crc    =  InputCRC;
         BufPtr = (uint8 *)DataPtr;

         for ( i = 0 ; i < DataLength ; i++,  BufPtr++)
	     {
	         Index = ( ( Crc ^ *BufPtr) & 0x00FF);
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
    int32   NameLen = 0;
/*    uint32  RegIndx = CFE_ES_CDS_NOT_FOUND; */
    uint32  ThisAppId;
/*    CFE_ES_CDS_RegRec_t *RegRecPtr = NULL; */
    
   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_REGISTERCDS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_REGISTERCDS_PROC];
   }
    
    char   AppName[OS_MAX_API_NAME] = {"UNKNOWN"};
    char   LogMsg[128];
    char   CDSName[CFE_ES_CDS_MAX_FULL_NAME_LEN] = {""};
    
    if (CDSVars.MemPoolSize == 0)
    {
        CFE_ES_WriteToSysLog("CFE_CDS:Register-CDS not available\n");
        Status = CFE_ES_NOT_IMPLEMENTED;
    }
    else
    {
        /* Check to make sure calling application is legit */
        Status = CFE_ES_CDS_ValidateAppID(&ThisAppId);

        if (Status == CFE_SUCCESS)
        {
            /* Assume we can't make a CDS and return a bad handle for now */
            *CDSHandlePtr = CFE_ES_CDS_BAD_HANDLE;

            /* Make sure specified CDS name is not too long or too short */
            NameLen = strlen(Name);
            if ((NameLen > CFE_ES_CDS_MAX_NAME_LENGTH) || (NameLen == 0))
            {
                Status = CFE_ES_CDS_INVALID_NAME;

                /* Perform a buffer overrun safe copy of name for debug log message */
                strncpy(LogMsg, "CFE_CDS:Register-CDS Name (", 27);
                strncpy(&LogMsg[27], Name, 72);
                strcat(LogMsg, ") is too long\n");
                CFE_ES_WriteToSysLog(LogMsg);

                strncpy(CDSName, Name, CFE_ES_CDS_MAX_NAME_LENGTH);
                CDSName[CFE_ES_CDS_MAX_NAME_LENGTH-1] = '\0';
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
        else  /* Application ID was invalid */
        {
            CFE_ES_WriteToSysLog("CFE_CDS:Register-Bad AppId(%d)\n", ThisAppId);
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
                                   AppName, Name, Status);
    }
    
    return (Status);
} /* End of CFE_ES_CDSRegister() */

/*
** Function: CFE_ES_CopyToCDS
**
** Purpose:  Copies a data block to a Critical Data Store.
**
*/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy)
{
    int32 Status = CFE_SUCCESS;

   /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_COPYTOCDS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_COPYTOCDS_PROC];
   }
    
    Status = CFE_ES_CDSBlockWrite(CDSVars.Registry[Handle].MemHandle, DataToCopy);
    
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
 
    /* Handle Preset Return Code */
   if (cfe_es_api_return_value[CFE_ES_RESTOREFROMCDS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_es_api_return_value[CFE_ES_RESTOREFROMCDS_PROC];
   }
    
    Status = CFE_ES_CDSBlockRead(RestoreToMemory, CDSVars.Registry[Handle].MemHandle);
    
    return Status;
} /* End of CFE_ES_RestoreFromCDS() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Name: CFE_ES_PerfLogAdd                                                     */
/*                                                                               */
/* Purpose: This function adds a new entry to the data buffer.  */
/*                                                                               */
/* Assumptions and Notes:                                                        */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetResetSubtype() -- Set the Reset Subtype.                  */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetResetSubtype(uint32 subType)
{
	switch (subType)
	{
		case CFE_ES_POWER_CYCLE:
		case CFE_ES_PUSH_BUTTON:
		case CFE_ES_HW_SPECIAL_COMMAND:
		case CFE_ES_HW_WATCHDOG:
		case CFE_ES_RESET_COMMAND:
		case CFE_ES_EXCEPTION:
		case CFE_ES_UNDEFINED_RESET:	
		case CFE_ES_HWDEBUG_RESET:	
		case CFE_ES_BANKSWITCH_RESET:	
			utf_ResetSubType = subType;
			break;
		
		default:
			utf_ResetSubType = 0;
			return CFE_ES_BAD_ARGUMENT;
			break;
	}
	return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_ES_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_ES_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_ES_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   cfe_es_api_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for CFE ES Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for CFE ES Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_ES_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_ES_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_CFE_ES_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_ES_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_ES_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_CFE_ES_Set_Api_Return_Code(Index, Code);
	return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_ES_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_ES_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_CFE_ES_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}




/* End UTF_CFE_ES.C */
