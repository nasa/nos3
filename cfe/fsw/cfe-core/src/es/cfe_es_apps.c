/*
**  File:  
**    cfe_es_apps.c
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
**    This file contains functions for starting cFE applications from a filesystem.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  $Log: cfe_es_apps.c  $
**  Revision 1.26 2014/09/05 11:35:15GMT-05:00 acudmore 
**  Changed CFE_ES_AppCreate and CFE_ES_LoadLibrary to remove big shared data lock. Locking shared data at several points now to give other apps a chance to run.
**  Revision 1.25 2014/08/22 15:50:04GMT-05:00 lwalling 
**  Changed signed loop counters to unsigned
**  Revision 1.24 2014/07/25 11:56:54EDT lwalling 
**  Changed INCLUDE_CFE_TBL to EXCLUDE_CFE_TBL
**  Revision 1.23 2014/07/23 11:33:22EDT lwalling 
**  Made Table Services conditionsal based on new environment variable INCLUDE_CFE_TBL
**  Revision 1.22 2014/05/05 15:00:08EDT acudmore 
**  Clarified misleading event message text.
**  Revision 1.21 2014/05/05 13:28:00GMT-05:00 acudmore 
**  Fixed event message string, replaced variable name with %s.
**  Revision 1.20 2012/09/28 16:13:24GMT-05:00 aschoeni 
**  Replaced OS_ERROR with NOT OS_SUCCESS in calls for semphore deletion
**  Revision 1.19 2012/01/13 11:49:59EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.18 2011/12/07 19:20:26EST aschoeni 
**  Removed returns for TIME and SB for cleaning up apps
**  Revision 1.17 2011/11/30 15:43:11EST jmdagost 
**  Added test for task delete error when cleaning up resources.
**  Revision 1.16 2011/09/02 11:04:11EDT jmdagost 
**  Corrected filename string copy length from max api length to max path length.
**  Revision 1.15 2010/11/04 14:06:43EDT acudmore 
**  Added ram disk mount path configuration option.
**  Revision 1.14 2010/10/25 17:43:44EDT jmdagost 
**  Add length tests for extracted filename and path
**  Revision 1.13 2009/07/30 19:17:49EDT jmdagost 
**  Deleted static keyword in CFE_ES_StartApplications
**  Revision 1.12 2009/07/28 16:31:15EDT jmdagost 
**  Replaced 20 references to filename with references to function name.
**  Revision 1.11 2009/06/10 09:08:52EDT acudmore 
**  Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
**  Revision 1.10 2008/08/28 08:46:53EDT apcudmore 
**  changed CFS call back to CFE
**  Revision 1.9 2008/08/27 16:59:01EDT apcudmore 
**  Added timer delete code in ES task cleanup.
**  Revision 1.8 2008/08/06 22:42:33EDT dkobe 
**  Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
**  Revision 1.7 2008/08/04 15:06:48EDT apcudmore 
**  Updated handling code when file decompress fails.
**  Revision 1.6 2008/07/31 15:41:35EDT apcudmore 
**  Added execution counter API:
**    -- Added execution counter to ES internal task data
**    -- Added code to increment counter in RunLoop
**    -- Added code to report counter in GetInfo APIs
**    -- Added API to increment counter for child tasks
**    -- Added code to cFE Core apps to increment counters.
**  Revision 1.5 2008/07/30 14:52:32EDT apcudmore 
**  Added startup sync support and API
**  Revision 1.4 2008/07/08 12:35:29EDT apcudmore 
**  Added an "AddressesAreValid" flag to the ES App Info structure.
**  Revision 1.3 2008/06/26 15:41:54EDT apcudmore 
**  Added public CFE_ES_GetAppInfo API.
**  Moved the old API to CFE_ES_GetAppInfoInternal
**  -- May merge these two in the near future.
**  Revision 1.2 2008/06/20 15:40:31EDT apcudmore 
**  Added support for OSAL based module loader
**   - Removed calls and references to the BSP based module loader
**  Revision 1.1 2008/04/17 08:05:02EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.32 2007/09/25 11:46:09EDT apcudmore 
**  Updated cfe_es_startup.scr file format
**  Revision 1.31 2007/09/21 15:40:19EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.30 2007/07/11 13:55:38EDT dlkobe 
**  Corrected order of WriteToSysLog parameters to match format string
**  Revision 1.29 2007/07/05 13:37:14EDT apcudmore 
**  Fixed error with Volatile and Nonvolatile ES startup file names.
**  Revision 1.28 2007/05/25 14:04:12EDT apcudmore 
**  Return an error code on CFE_ES_CleanUpApp function.
**  Revision 1.27 2007/05/25 11:55:43EDT apcudmore 
**  Error Handling updates
**     - CFE_ES_GetAppInfo returns void
**     - added app ID range check in RestartApp
**   
**  Revision 1.26 2007/05/22 14:56:55EDT apcudmore 
**  cFE Non-Volatile Disk/Startup File changes:
**    - cFE startup now passes in startup file path/name
**    - cFE startup code does not init non-volatile disk ( BSP does )
**    - BSPs now can map cFE paths without making the OS_initfs/OS_mount calls
**    - MCP750 vxWorks6.2 BSP inits/mounts CF disk
**    - Desktop ports pass in the same startup file path/name as before
**  Revision 1.25 2007/05/15 11:16:07EDT apcudmore 
**  Added modification log tags.
**
*/

/* 
** Includes
*/
#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_psp.h"     
#include "cfe_es_global.h"
#include "cfe_es_apps.h"

#include <stdio.h>
#include <string.h> /* memset() */
#include <fcntl.h>

#ifdef _ENHANCED_BUILD_
#include "target_config.h"
#endif

/*
** Defines
*/
#define ES_START_BUFF_SIZE 128

/* For extra debug code */
#undef ES_APP_DEBUG

/*
**
**  Global Variables
**
*/

/*
****************************************************************************
** Functions
***************************************************************************
*/

/*
** Name: 
**   CFE_ES_StartApplications
**
** Purpose: 
**   This routine loads/starts cFE applications.
**
*/
void CFE_ES_StartApplications(uint32 ResetType, const char *StartFilePath )
{
   char ES_AppLoadBuffer[ES_START_BUFF_SIZE];  /* A buffer of for a line in a file */   
   uint32      BuffLen = 0;                            /* Length of the current buffer */
   int32       AppFile = 0;
   char        c;
   int32       ReadStatus;
   boolean     LineTooLong = FALSE;
   boolean     FileOpened = FALSE;

#ifdef _ENHANCED_BUILD_
   CFE_StaticModuleLoadEntry_t *Entry;
   CFE_ES_AppStaticModuleApi_t *ApiPtr;
   uint32       StaticId;

   /* First run through the static lib list */
   Entry = GLOBAL_CONFIGDATA.CfsModuleList;
   if (Entry != NULL)
   {
       while (Entry->Name != NULL)
       {
           ApiPtr = (CFE_ES_AppStaticModuleApi_t *)Entry->Api;
           if (ApiPtr->EntryType == CFE_ES_STATICENTRYTYPE_LIBRARY)
           {
               ReadStatus = CFE_ES_LoadLibrary(&StaticId, NULL,
                       Entry->Api, Entry->Name);
           }
           ++Entry;
       }
   }

   /* Second run through the static application list */
   Entry = GLOBAL_CONFIGDATA.CfsModuleList;
   if (Entry != NULL)
   {
       while (Entry->Name != NULL)
       {
           ApiPtr = (CFE_ES_AppStaticModuleApi_t *)Entry->Api;
           if (ApiPtr->EntryType == CFE_ES_STATICENTRYTYPE_APPLICATION)
           {
               ReadStatus = CFE_ES_AppCreate(&StaticId, NULL,
                       Entry->Api, Entry->Name, ApiPtr->Priority,
                       ApiPtr->StackSize, CFE_ES_APP_EXCEPTION_PROC_RESTART);
           }
           ++Entry;
       }
   }
#endif

   /*
   ** Get the ES startup script filename.
   ** If this is a Processor Reset, try to open the file in the volatile disk first.
   */
   if ( ResetType == CFE_PSP_RST_TYPE_PROCESSOR )
   {
      /*
      ** Open the file in the volatile disk.
      */
      AppFile = OS_open( CFE_ES_VOLATILE_STARTUP_FILE, O_RDONLY, 0);

      if ( AppFile >= 0 )
      {
         CFE_ES_WriteToSysLog ("ES Startup: Opened ES App Startup file: %s\n", 
                                CFE_ES_VOLATILE_STARTUP_FILE);
         FileOpened = TRUE;
      }
      else 
      {
         CFE_ES_WriteToSysLog ("ES Startup: Cannot Open Volatile Startup file, Trying Nonvolatile.\n");
         FileOpened = FALSE;
      }

   } /* end if */
   
   /*
   ** This if block covers two cases: A Power on reset, and a Processor reset when
   ** the startup file on the volatile file system could not be opened.
   */
   if ( FileOpened == FALSE && StartFilePath != NULL)
   {
      /*
      ** Try to Open the file passed in to the cFE start.
      */
      AppFile = OS_open( (const char *)StartFilePath, O_RDONLY, 0);

      if ( AppFile >= 0 )
      {
         CFE_ES_WriteToSysLog ("ES Startup: Opened ES App Startup file: %s\n",StartFilePath);
         FileOpened = TRUE;
      }
      else 
      {
         CFE_ES_WriteToSysLog ("ES Startup: Error, Can't Open ES App Startup file: %s EC = 0x%08X\n",
                              StartFilePath, (unsigned int)AppFile );
         FileOpened = FALSE;
      }
   
   }
   
   /*
   ** If the file is opened in either the Nonvolatile or the Volatile disk, process it.
   */
   if ( FileOpened == TRUE)
   {
      memset(ES_AppLoadBuffer,0x0,ES_START_BUFF_SIZE);
      BuffLen = 0;      
      
      /*
      ** Parse the lines from the file. If it has an error
      ** or reaches EOF, then abort the loop.
      */
      while(1)
      {
         ReadStatus = OS_read(AppFile, &c, 1);
         if ( ReadStatus == OS_FS_ERROR )
         {
            CFE_ES_WriteToSysLog ("ES Startup: Error Reading Startup file. EC = 0x%08X\n",(unsigned int)ReadStatus);
            break;
         }
         else if ( ReadStatus == 0 )
         {
            /*
            ** EOF Reached
            */
            break;
         }
         else if(c != '!')
         {
             if ( c <= ' ')
             {
                /*
                ** Skip all white space in the file
                */
                ;
             }
             else if ( c == ',' )
             {
                /*
                ** replace the field delimiter with a space
                ** This is used for the sscanf string parsing
                */
                if ( BuffLen < ES_START_BUFF_SIZE )
                {
                   ES_AppLoadBuffer[BuffLen] = ' ';
                }
                else
                {
                   LineTooLong = TRUE;
                } 
                BuffLen++;
             }
             else if ( c != ';' )
             {
                /*
                ** Regular data gets copied in
                */
                if ( BuffLen < ES_START_BUFF_SIZE )
                {
                   ES_AppLoadBuffer[BuffLen] = c;
                }
                else
                {
                   LineTooLong = TRUE;
                }
                BuffLen++;
             }
             else
             {
                if ( LineTooLong == TRUE )
                {   
                   /*
                   ** The was too big for the buffer
                   */
                   CFE_ES_WriteToSysLog ("ES Startup: ES Startup File Line is too long: %u bytes.\n",(unsigned int)BuffLen);
                   LineTooLong = FALSE;
                }
                else
                {
                   /*
                   ** Send the line to the file parser
                   */
                   CFE_ES_ParseFileEntry(ES_AppLoadBuffer);
                }
                memset(ES_AppLoadBuffer,0x0,ES_START_BUFF_SIZE);
                BuffLen = 0;
             }
         }
         else
         {
           /*
           ** break when EOF character '!' is reached
           */
           break;
         }
      }
      /*
      ** close the file
      */
      OS_close(AppFile);
       
   }
}

/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_ParseFileEntry
**
**   Purpose: This function parses the startup file line for an individual
**            cFE application.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_ParseFileEntry(const char *FileEntry)
{
   char   FileName[OS_MAX_PATH_LEN];
   char   AppName[OS_MAX_API_NAME];
   char   EntryPoint[OS_MAX_API_NAME];
   char   EntryType[OS_MAX_API_NAME];
   int    ScanfStatus;
   unsigned int Priority;
   unsigned int StackSize;
   unsigned int Unused;
   unsigned int ExceptionAction;
   uint32 ApplicationId;
   int32  CreateStatus = CFE_ES_ERR_APP_CREATE;

   /*
   ** Use sscanf to parse the string.
   ** When this routine is called, the buffer is already verified to be less than the 
   ** maximum size, and the parameters have been separated by a single space each.
   ** If the sscanf fails, then it is a malformed file entry.  
   **
   ** CFS CCB is tracking this issue separately, and we no longer need
   ** warnings from cppcheck about this call.
   */
   /* cppcheck-suppress invalidscanf */
   ScanfStatus = sscanf(FileEntry,"%s %s %s %s %u %u %x %u",
                        EntryType, FileName, EntryPoint, AppName, 
                        &Priority, &StackSize, &Unused, &ExceptionAction);

   /*
   ** Check to see if the correct number of items were parsed
   */
   if ( ScanfStatus != 8 )
   {
      CFE_ES_WriteToSysLog("ES Startup: Invalid ES Startup file entry: %d\n",ScanfStatus);
      return (CreateStatus);
   }
     
   if(strncmp(EntryType,"CFE_APP",OS_MAX_API_NAME)==0)
   {
      CFE_ES_WriteToSysLog("ES Startup: Loading file: %s, APP: %s\n",
                            FileName, AppName);
      
      /*
      ** Validate Some parameters
      ** Exception action should be 0 ( Restart App ) or
      ** 1 ( Processor reset ). If it's non-zero, assume it means
      ** reset CPU.
      */
      if ( ExceptionAction > CFE_ES_APP_EXCEPTION_RESTART_APP ) 
          ExceptionAction = CFE_ES_APP_EXCEPTION_PROC_RESTART;
      /*
      ** Now create the application 
      */
      CreateStatus = CFE_ES_AppCreate(&ApplicationId, FileName, 
                               EntryPoint, AppName, (uint32) Priority, 
                               (uint32) StackSize, (uint32) ExceptionAction );
   }
   else if(strncmp(EntryType,"CFE_LIB",OS_MAX_API_NAME)==0)
   {            
      CFE_ES_WriteToSysLog("ES Startup: Loading shared library: %s\n",FileName);
                                                             
      /*
      ** Now load the library
      */
      CreateStatus = CFE_ES_LoadLibrary(&ApplicationId, FileName, 
                               EntryPoint, AppName);

   }
   else if(strncmp(EntryType,"CFE_DRV",OS_MAX_API_NAME)==0)
   { 
      CFE_ES_WriteToSysLog("ES Startup: Device Drivers not yet implemented. %s\n",FileName);
   }
   else
   {
      CFE_ES_WriteToSysLog("ES Startup: Unexpected EntryType %s in startup file.\n",EntryType);
   }

   return (CreateStatus);

}

/*
**---------------------------------------------------------------------------------------
** Name: ES_AppCreate
**
**   Purpose: This function loads and creates a cFE Application.
**            This function can be called from the ES startup code when it
**            loads the cFE Applications from the disk using the startup script, or it
**            can be called when the ES Start Application command is executed.
**
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_AppCreate(uint32 *ApplicationIdPtr,
                       const char   *FileName,
                       const void   *EntryPointData,
                       const char   *AppName,
                       uint32  Priority,
                       uint32  StackSize,
                       uint32  ExceptionAction)
{
   cpuaddr StartAddr;
   int32   ReturnCode;
   uint32  i;
   boolean AppSlotFound;
   uint32  TaskId;
   uint32  ModuleId;
   char    FileNameOnly[OS_MAX_PATH_LEN];
   char    RamDiskPath[OS_MAX_PATH_LEN];
   size_t  StringLength;
   boolean IsRamDiskFile = FALSE;
   CFE_ES_AppStaticModuleApi_t *StaticEntryPtr = NULL;


   /*
   ** Allocate an ES_AppTable entry
   */
   CFE_ES_LockSharedData(__func__,__LINE__);
   AppSlotFound = FALSE;
   for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
   {
      if ( CFE_ES_Global.AppTable[i].RecordUsed == FALSE )
      {
         AppSlotFound = TRUE;
         CFE_ES_Global.AppTable[i].RecordUsed = TRUE; /* Reserve Slot */
         break;
      }
   }
   CFE_ES_UnlockSharedData(__func__,__LINE__);

   /*
   ** If a slot was found, create the application
   */
   if ( AppSlotFound == TRUE)
   {
      /*
       * Check if dynamically loading -
       * if statically linked then FileName is NULL and EntryPointData points to the API structure
       */
      if (FileName != NULL)
      {
          /*
          ** Check to see if the code is a Gzip file
          */
          if ( CFE_FS_IsGzFile(FileName) == TRUE )
          {
             /*
             ** Build up the destination path in the RAM disk
             */
             (void) CFE_SB_MessageStringGet(RamDiskPath, 
                                            CFE_ES_RAM_DISK_MOUNT_STRING"/", 
                                            NULL, 
                                            sizeof(RamDiskPath), 
                                            sizeof(CFE_ES_RAM_DISK_MOUNT_STRING"/"));
            
             /*
             ** Extract the filename from the path
             */
             ReturnCode = CFE_FS_ExtractFilenameFromPath(FileName, FileNameOnly);

             if ( ReturnCode == CFE_SUCCESS )
             {
                if ((strlen(RamDiskPath) + strlen(FileNameOnly)) < OS_MAX_PATH_LEN)
                {

                    /*
                    ** Cat the Filename to the RamDiskPath
                    */
                    strcat(RamDiskPath, FileNameOnly);
            
                    /*
                    ** Remove the ".gz" prefix from the filename
                    ** Already Determined that the filename ends in ".gz"
                    */
                    StringLength = strlen(RamDiskPath);
                    RamDiskPath[StringLength - 3] = '\0';

                    /*
                    ** Decompress the file:
                    */
                    ReturnCode =  CFE_FS_Decompress( FileName, RamDiskPath);

                    if ( ReturnCode != OS_SUCCESS )
                    {
                       CFE_ES_WriteToSysLog("ES Startup: Unable to decompress Application File: %s\n",FileName);

                       CFE_ES_LockSharedData(__func__,__LINE__);
                       CFE_ES_Global.AppTable[i].RecordUsed = FALSE; /* Release slot */
                       CFE_ES_UnlockSharedData(__func__,__LINE__);

                       return(CFE_ES_ERR_APP_CREATE);
                    }
                    else
                    {
                       /*
                       ** All ready to use unzipped RAM disk file
                       */
                       IsRamDiskFile = TRUE;
                       ReturnCode = OS_ModuleLoad( &ModuleId, AppName, RamDiskPath);
                    }
                }
                else
                {
                    /* Can't include the name string since it could be too long for the message */
                    CFE_ES_WriteToSysLog("ES Startup: Application path plus file name length (%d) exceeds max allowed (%d)\n",
                                         (int)(strlen(RamDiskPath) + strlen(FileNameOnly)), OS_MAX_PATH_LEN);

                    CFE_ES_LockSharedData(__func__,__LINE__);
                    CFE_ES_Global.AppTable[i].RecordUsed = FALSE; /* Release slot */
                    CFE_ES_UnlockSharedData(__func__,__LINE__);

                    return(CFE_ES_ERR_APP_CREATE);
                }

             }
             else
             {
                CFE_ES_WriteToSysLog("ES Startup: Unable to extract filename from path: %s.\n",FileName);
                CFE_ES_LockSharedData(__func__,__LINE__);
                CFE_ES_Global.AppTable[i].RecordUsed = FALSE; /* Release slot */
                CFE_ES_UnlockSharedData(__func__,__LINE__);
                return(CFE_ES_ERR_APP_CREATE);
             }

          }
          else
          {
              /*
              ** Load the module directly
              */
              ReturnCode = OS_ModuleLoad ( &ModuleId, AppName, FileName );
          }
          /*
          ** If the Load was OK, then lookup the address of the entry point
          */
          if ( ReturnCode == OS_SUCCESS )
          {
             ReturnCode = OS_SymbolLookup( &StartAddr, (const char*)EntryPointData );
             if ( ReturnCode != OS_SUCCESS )
             {
                 CFE_ES_WriteToSysLog("ES Startup: Could not find symbol:%s. EC = 0x%08X\n",
                         (const char*)EntryPointData, (unsigned int)ReturnCode);

                 CFE_ES_LockSharedData(__func__,__LINE__);
                 CFE_ES_Global.AppTable[i].RecordUsed = FALSE; /* Release slot */
                 CFE_ES_UnlockSharedData(__func__,__LINE__);

                 return(CFE_ES_ERR_APP_CREATE);
             }
          }
          else /* load not successful */
          {
              CFE_ES_WriteToSysLog("ES Startup: Could not load cFE application file:%s. EC = 0x%08X\n",
                                FileName, (unsigned int)ReturnCode);

              CFE_ES_LockSharedData(__func__,__LINE__);
              CFE_ES_Global.AppTable[i].RecordUsed = FALSE; /* Release slot */
              CFE_ES_UnlockSharedData(__func__,__LINE__);

              return(CFE_ES_ERR_APP_CREATE);
          }
      }
      else /* Not a dynamic load - app must be statically linked into current exe */
      {
          StaticEntryPtr = (CFE_ES_AppStaticModuleApi_t *)EntryPointData;
          StartAddr = StaticEntryPtr->Ptrs.EntryFuncAddr;
          ModuleId = 0;
      }

      /*
      ** If the EntryPoint symbol was found, then start creating the App
      */
      CFE_ES_LockSharedData(__func__,__LINE__);
      /*
      ** Allocate and populate the ES_AppTable entry
      */
      CFE_PSP_MemSet ( (void *)&(CFE_ES_Global.AppTable[i]), 0, sizeof(CFE_ES_AppRecord_t));
      CFE_ES_Global.AppTable[i].RecordUsed = TRUE;
      CFE_ES_Global.AppTable[i].Type = CFE_ES_APP_TYPE_EXTERNAL;

      /*
      ** Fill out the parameters in the AppStartParams sub-structure
      */
      strncpy((char *)CFE_ES_Global.AppTable[i].StartParams.Name, AppName, OS_MAX_API_NAME);
      CFE_ES_Global.AppTable[i].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';

      if (StaticEntryPtr != NULL)
      {
         /* Leave entry point / filename blank if static load entry */
         CFE_ES_Global.AppTable[i].StartParams.EntryPoint[0] = 0;
         CFE_ES_Global.AppTable[i].StartParams.FileName[0] = 0;
      }
      else
      {
         strncpy((char *)CFE_ES_Global.AppTable[i].StartParams.EntryPoint, (const char *)EntryPointData, OS_MAX_API_NAME);
         CFE_ES_Global.AppTable[i].StartParams.EntryPoint[OS_MAX_API_NAME - 1] = '\0';
         strncpy((char *)CFE_ES_Global.AppTable[i].StartParams.FileName, FileName, OS_MAX_PATH_LEN);
         CFE_ES_Global.AppTable[i].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
      }

      CFE_ES_Global.AppTable[i].StartParams.StackSize = StackSize;

      CFE_ES_Global.AppTable[i].StartParams.StartAddress = StartAddr;
      CFE_ES_Global.AppTable[i].StartParams.ModuleId = ModuleId;

      CFE_ES_Global.AppTable[i].StartParams.ExceptionAction = ExceptionAction;
      CFE_ES_Global.AppTable[i].StartParams.Priority = Priority;

      /*
      ** Fill out the Task Info
      */
      strncpy((char *)CFE_ES_Global.AppTable[i].TaskInfo.MainTaskName, AppName, OS_MAX_API_NAME);
      CFE_ES_Global.AppTable[i].TaskInfo.MainTaskName[OS_MAX_API_NAME - 1] = '\0';
      CFE_ES_Global.AppTable[i].TaskInfo.NumOfChildTasks = 0;

      /*
      ** Increment the app started counter. This is used in the startup sync
      ** code.
      */
      ++CFE_ES_Global.AppStartedCount;

      /*
      ** Fill out the Task State info
      */
      CFE_ES_Global.AppTable[i].StateRecord.AppControlRequest = CFE_ES_RUNSTATUS_APP_RUN;
      CFE_ES_Global.AppTable[i].StateRecord.AppState = CFE_ES_APP_STATE_INITIALIZING;
      CFE_ES_Global.AppTable[i].StateRecord.AppTimer = 0;

      /*
      ** Create the primary task for the newly loaded task
      */
      ReturnCode = OS_TaskCreate(&CFE_ES_Global.AppTable[i].TaskInfo.MainTaskId,   /* task id */
                       AppName,             /* task name */
                       (osal_task_entry)StartAddr,   /* task function pointer */
                       NULL,                /* stack pointer */
                       StackSize,           /* stack size */
                       Priority,            /* task priority */
                       OS_FP_ENABLED);     /* task options */


      if(ReturnCode != OS_SUCCESS)
      {
         CFE_ES_WriteToSysLog("ES Startup: AppCreate Error: TaskCreate %s Failed. EC = 0x%08X!\n",
                       AppName,(unsigned int)ReturnCode);

         CFE_ES_Global.AppTable[i].RecordUsed = FALSE;
         CFE_ES_UnlockSharedData(__func__,__LINE__);

         return(CFE_ES_ERR_APP_CREATE);
      }
      else
      {

         /*
         ** Record the ES_TaskTable entry
         */
         OS_ConvertToArrayIndex(CFE_ES_Global.AppTable[i].TaskInfo.MainTaskId, &TaskId);

         if ( CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
         {
            CFE_ES_WriteToSysLog("ES Startup: Error: ES_TaskTable slot in use at task creation!\n");
         }
         else
         {
            CFE_ES_Global.TaskTable[TaskId].RecordUsed = TRUE;
         }
         CFE_ES_Global.TaskTable[TaskId].AppId = i;
         CFE_ES_Global.TaskTable[TaskId].TaskId = CFE_ES_Global.AppTable[i].TaskInfo.MainTaskId;
         strncpy((char *)CFE_ES_Global.TaskTable[TaskId].TaskName,
             (char *)CFE_ES_Global.AppTable[i].TaskInfo.MainTaskName,OS_MAX_API_NAME );
         CFE_ES_Global.TaskTable[TaskId].TaskName[OS_MAX_API_NAME - 1]='\0';
         CFE_ES_WriteToSysLog("ES Startup: %s loaded and created\n", AppName);
         *ApplicationIdPtr = i;

         /*
         ** Increment the registered App and Registered External Task variables.
         */
         CFE_ES_Global.RegisteredTasks++;
         CFE_ES_Global.RegisteredExternalApps++;

         CFE_ES_UnlockSharedData(__func__,__LINE__);

         /*
         ** Remove the temporary RAM disk file
         */
         if ( IsRamDiskFile == TRUE )
         {
            ReturnCode = OS_remove(RamDiskPath);
         }
          
         return(CFE_SUCCESS);

      } /* End If OS_TaskCreate */
   }
   else /* appSlot not found */
   {
      CFE_ES_WriteToSysLog("ES Startup: No free application slots available\n");
      return(CFE_ES_ERR_APP_CREATE);
   }

} /* End Function */
/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_LoadLibrary
**
**   Purpose: This function loads and initializes a cFE Shared Library.
**
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_LoadLibrary(uint32 *LibraryIdPtr,
                         const char   *FileName,
                         const void   *EntryPointData,
                         const char   *LibName)
{
   cpuaddr                      StartAddr;
   int32                        ReturnCode;
   boolean                      LibSlotFound;
   CFE_ES_LibraryEntryFuncPtr_t FunctionPointer;
   uint32                       ModuleId;
   boolean                      IsRamDiskFile = FALSE;
   char                         FileNameOnly[OS_MAX_PATH_LEN];
   char                         RamDiskPath[OS_MAX_PATH_LEN];
   size_t                       StringLength;
   uint32                       i;
   CFE_ES_AppStaticModuleApi_t  *StaticEntryPtr = NULL;


   /*
   ** Allocate an ES_LibTable entry
   */
   CFE_ES_LockSharedData(__func__,__LINE__);
   LibSlotFound = FALSE;
   for ( i = 0; i < CFE_ES_MAX_LIBRARIES; i++ )
   {
      if ( CFE_ES_Global.LibTable[i].RecordUsed == FALSE )
      {
         LibSlotFound = TRUE;
         CFE_ES_Global.LibTable[i].RecordUsed = TRUE; /* Reserve Slot */
         break;
      }
   }
   CFE_ES_UnlockSharedData(__func__,__LINE__);

   /*
   ** If a slot was found, Load and Register the library 
   */
   if ( LibSlotFound == TRUE)
   {
      if (FileName != NULL)
      {
          /*
          ** Check to see if the library is a Gzip file
          */
          if ( CFE_FS_IsGzFile(FileName) == TRUE )
          {
             /*
             ** Build up the destination path in the RAM disk
             */
             (void) CFE_SB_MessageStringGet(RamDiskPath,
                                            CFE_ES_RAM_DISK_MOUNT_STRING"/",
                                            NULL,
                                            sizeof(RamDiskPath),
                                            sizeof(CFE_ES_RAM_DISK_MOUNT_STRING"/"));
            
             /*
             ** Extract the filename from the path
             */
             ReturnCode = CFE_FS_ExtractFilenameFromPath(FileName, FileNameOnly);

             if ( ReturnCode == CFE_SUCCESS )
             {
                if ((strlen(RamDiskPath) + strlen(FileNameOnly)) < OS_MAX_PATH_LEN)
                {
                    /*
                    ** Cat the Filename to the RamDiskPath
                    */
                    strcat(RamDiskPath, FileNameOnly);
            
                    /*
                    ** Remove the ".gz" prefix from the filename
                    ** Already Determined that the filename ends in ".gz"
                    */
                    StringLength = strlen(RamDiskPath);
                    RamDiskPath[StringLength - 3] = '\0';

                    /*
                    ** Decompress the file:
                    */
                    ReturnCode =  CFE_FS_Decompress( FileName, RamDiskPath);

                    if ( ReturnCode != OS_SUCCESS )
                    {
                       CFE_ES_WriteToSysLog("ES Startup: Unable to decompress library file: %s\n",FileName);

                       CFE_ES_LockSharedData(__func__,__LINE__);
                       CFE_ES_Global.LibTable[i].RecordUsed = FALSE; /* Release Slot */
                       CFE_ES_UnlockSharedData(__func__,__LINE__);

                       return(CFE_ES_ERR_LOAD_LIB);
                    }
                    else
                    {
                       /*
                       ** All ready to use unzipped RAM disk file
                       */
                       IsRamDiskFile = TRUE;
                       ReturnCode = OS_ModuleLoad( &ModuleId, LibName, RamDiskPath);
                    }
                }
                else
                {
                    /* Can't include the name string since it could be too long for the message */
                    CFE_ES_WriteToSysLog("ES Startup: Library path plus file name length (%d) exceeds max allowed (%d)\n",
                                         (int)(strlen(RamDiskPath) + strlen(FileNameOnly)), OS_MAX_PATH_LEN);

                    CFE_ES_LockSharedData(__func__,__LINE__);
                    CFE_ES_Global.LibTable[i].RecordUsed = FALSE; /* Release Slot */
                    CFE_ES_UnlockSharedData(__func__,__LINE__);

                    return(CFE_ES_ERR_LOAD_LIB);
                }
             }
             else
             {
                CFE_ES_WriteToSysLog("ES Startup: Unable to extract filename from path: %s.\n",FileName);

                CFE_ES_LockSharedData(__func__,__LINE__);
                CFE_ES_Global.LibTable[i].RecordUsed = FALSE; /* Release Slot */
                CFE_ES_UnlockSharedData(__func__,__LINE__);

                return(CFE_ES_ERR_LOAD_LIB);
             }

          }
          else
          {
              /*
              ** Load the library module directly
              */
             ReturnCode = OS_ModuleLoad ( &ModuleId, LibName, FileName );
          }

          /*
          ** If the Load was OK, then lookup the address of the entry point
          */
          if ( ReturnCode == OS_SUCCESS )
          {

             ReturnCode = OS_SymbolLookup( &StartAddr, (const char *)EntryPointData );
             if (ReturnCode != OS_SUCCESS) /* could not find symbol */
             {
                 CFE_ES_WriteToSysLog("ES Startup: Could not find Library Init symbol:%s. EC = 0x%08X\n",
                                        (const char *)EntryPointData, (unsigned int)ReturnCode);

                 CFE_ES_LockSharedData(__func__,__LINE__);
                 CFE_ES_Global.LibTable[i].RecordUsed = FALSE; /* Release Slot */
                 CFE_ES_UnlockSharedData(__func__,__LINE__);

                 return(CFE_ES_ERR_LOAD_LIB);

             } /* end if -- look up symbol */

             FunctionPointer = (CFE_ES_LibraryEntryFuncPtr_t)StartAddr;
          }
          else /* load not successful */
          {
             CFE_ES_WriteToSysLog("ES Startup: Could not load cFE Shared Library\n");

             CFE_ES_LockSharedData(__func__,__LINE__);
             CFE_ES_Global.LibTable[i].RecordUsed = FALSE; /* Release Slot */
             CFE_ES_UnlockSharedData(__func__,__LINE__);

             return(CFE_ES_ERR_LOAD_LIB);
          }
      }
      else /* Not a dynamic load - library must be statically linked into current exe */
      {
          StaticEntryPtr = (CFE_ES_AppStaticModuleApi_t *)EntryPointData;
          FunctionPointer = StaticEntryPtr->Ptrs.LibInitFunc;
          ModuleId = 0;
      }
     
      /*
      ** If the EntryPoint symbol was found, then finish creating the Library
      */
      CFE_ES_LockSharedData(__func__,__LINE__);
      /*
      ** Allocate and populate the ES_LibTable entry
      */
      CFE_ES_Global.LibTable[i].RecordUsed = TRUE;
      strncpy(CFE_ES_Global.LibTable[i].LibName, LibName, OS_MAX_API_NAME);
      CFE_ES_Global.LibTable[i].LibName[OS_MAX_API_NAME - 1] = '\0';

      /*
      ** Call the library initialization routine
      */
      ReturnCode = (*FunctionPointer)();

      if(ReturnCode != CFE_SUCCESS)
      {
          CFE_ES_Global.LibTable[i].RecordUsed = FALSE;
          CFE_ES_UnlockSharedData(__func__,__LINE__);

          CFE_ES_WriteToSysLog("ES Startup: Load Shared Library Init Error.\n");
          return(CFE_ES_ERR_LOAD_LIB);
      }
      else
      {
          CFE_ES_Global.RegisteredLibs++;
          *LibraryIdPtr = i;

          CFE_ES_UnlockSharedData(__func__,__LINE__);

          /*
          ** Remove the temporary RAM disk file
          */
          if ( IsRamDiskFile == TRUE )
          {
             ReturnCode = OS_remove(RamDiskPath);
          }
          return(CFE_SUCCESS);
      }
   }
   else /* libSlot not found */
   {
      CFE_ES_WriteToSysLog("ES Startup: No free library slots available\n");
      return(CFE_ES_ERR_LOAD_LIB); 
   }


} /* End Function */

/*
**---------------------------------------------------------------------------------------
** Name: CFE_ES_ScanAppTable
**
**   Purpose: This function scans the ES Application table and acts on the changes
**             in application states. This is where the external cFE Applications are
**             restarted, reloaded, or deleted.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_ScanAppTable(void)
{
   uint32 i;
         
   /*
   ** Scan the ES Application table. Skip entries that are:
   **  - Not in use, or 
   **  - cFE Core apps, or
   **  - Currently running
   */
   for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
   {
   
      if ((CFE_ES_Global.AppTable[i].RecordUsed == TRUE) && 
          (CFE_ES_Global.AppTable[i].Type == CFE_ES_APP_TYPE_EXTERNAL) && 
          (CFE_ES_Global.AppTable[i].StateRecord.AppState != CFE_ES_APP_STATE_RUNNING))
      {
      
          /*
          ** Process the External cFE App according to it's state.
          */
          if ( CFE_ES_Global.AppTable[i].StateRecord.AppState == CFE_ES_APP_STATE_WAITING )
          {
              /*
              ** If the timeout value is zero, take the action to delete/restart/reload the app
              */
              if ( CFE_ES_Global.AppTable[i].StateRecord.AppTimer <= 0 )
              {              
                 CFE_ES_ProcessControlRequest(i);
              }
              else
              {
                 #ifdef ES_APP_DEBUG
                    OS_printf("%d..\n",(int)CFE_ES_Global.AppTable[i].StateRecord.AppTimer);
                 #endif
                 CFE_ES_Global.AppTable[i].StateRecord.AppTimer --;   
                                
              }
              
          }
          else if ( CFE_ES_Global.AppTable[i].StateRecord.AppState == CFE_ES_APP_STATE_STOPPED )
          {
              /*
              ** The App is stopped and ready to get deleted/restarted/reloaded
              */
              CFE_ES_ProcessControlRequest(i);
            
          } /* end if */
                       
      } /* end if */

   } /* end for loop */

} /* End Function */


/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_ProcessControlRequest
**
**   Purpose: This function will perform the requested control action for an application.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_ProcessControlRequest(uint32 AppID)
{
   
   int32                   Status;
   CFE_ES_AppStartParams_t AppStartParams;
   uint32                  NewAppId;
   
   /*
   ** First get a copy of the Apps Start Parameters
   */
   CFE_PSP_MemCpy(&AppStartParams, &(CFE_ES_Global.AppTable[AppID].StartParams), sizeof(CFE_ES_AppStartParams_t));
   
   /*
   ** Now, find out what kind of Application control is being requested
   */
   switch ( CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest )
   {
         
      case CFE_ES_RUNSTATUS_APP_EXIT:
         /*
         ** Kill the app, and dont restart it
         */
         Status = CFE_ES_CleanUpApp(AppID);

         if ( Status == CFE_SUCCESS )
         {
            CFE_EVS_SendEvent(CFE_ES_EXIT_APP_INF_EID, CFE_EVS_INFORMATION, 
                              "Exit Application %s Completed.",AppStartParams.Name);
         }
         else
         {
            CFE_EVS_SendEvent(CFE_ES_EXIT_APP_ERR_EID, CFE_EVS_ERROR, 
                               "Exit Application %s Failed: CleanUpApp Error 0x%08X.",AppStartParams.Name, (unsigned int)Status);
         }
         break;
                
      case CFE_ES_RUNSTATUS_APP_ERROR:
         /*
         ** Kill the app, and dont restart it
         */
         Status = CFE_ES_CleanUpApp(AppID);

         if ( Status == CFE_SUCCESS )
         {         
            CFE_EVS_SendEvent(CFE_ES_ERREXIT_APP_INF_EID, CFE_EVS_INFORMATION, 
                               "Exit Application %s on Error Completed.",AppStartParams.Name);
         }
         else
         {
            CFE_EVS_SendEvent(CFE_ES_ERREXIT_APP_ERR_EID, CFE_EVS_ERROR, 
                              "Exit Application %s on Error Failed: CleanUpApp Error 0x%08X.",AppStartParams.Name, (unsigned int)Status);
         }
         break;
         
      case CFE_ES_RUNSTATUS_SYS_DELETE:
         /*
         ** Kill the app, and dont restart it
         */
         Status = CFE_ES_CleanUpApp(AppID);

         if ( Status == CFE_SUCCESS )
         {         
            CFE_EVS_SendEvent(CFE_ES_STOP_INF_EID, CFE_EVS_INFORMATION, 
                              "Stop Application %s Completed.",AppStartParams.Name);
         }
         else
         {
            CFE_EVS_SendEvent(CFE_ES_STOP_ERR3_EID, CFE_EVS_ERROR, 
                              "Stop Application %s Failed: CleanUpApp Error 0x%08X.",AppStartParams.Name, (unsigned int)Status);
         }
         break;
                                         
      case CFE_ES_RUNSTATUS_SYS_RESTART:
         /*
         ** Kill the app
         */
         Status = CFE_ES_CleanUpApp(AppID);

         if ( Status == CFE_SUCCESS )
         {
            /*
            ** And start it back up again
            */
            Status = CFE_ES_AppCreate(&NewAppId, (char *)AppStartParams.FileName, 
                                           (char *)AppStartParams.EntryPoint, 
                                           (char *)AppStartParams.Name, 
                                           AppStartParams.Priority, 
                                           AppStartParams.StackSize, 
                                           AppStartParams.ExceptionAction);
                                           
            if ( Status == CFE_SUCCESS )
            {
               CFE_EVS_SendEvent(CFE_ES_RESTART_APP_INF_EID, CFE_EVS_INFORMATION, 
                                  "Restart Application %s Completed.", AppStartParams.Name);
            }
            else
            {
               CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR3_EID, CFE_EVS_ERROR, 
                                  "Restart Application %s Failed: AppCreate Error 0x%08X.", AppStartParams.Name, (unsigned int)Status);
            }
         }
         else
         {
               CFE_EVS_SendEvent(CFE_ES_RESTART_APP_ERR4_EID, CFE_EVS_ERROR, 
                                  "Restart Application %s Failed: CleanUpApp Error 0x%08X.", AppStartParams.Name, (unsigned int)Status);
         }
         break;
            
      case CFE_ES_RUNSTATUS_SYS_RELOAD:
         /*
         ** Kill the app
         */
         Status = CFE_ES_CleanUpApp(AppID);

         if ( Status == CFE_SUCCESS )
         {
            /*
            ** And start it back up again
            */
            Status = CFE_ES_AppCreate(&NewAppId, (char *)AppStartParams.FileName, 
                                           (char *)AppStartParams.EntryPoint, 
                                           (char *)AppStartParams.Name, 
                                           AppStartParams.Priority, 
                                           AppStartParams.StackSize, 
                                           AppStartParams.ExceptionAction);
            if ( Status == CFE_SUCCESS )
            {
               CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_INF_EID, CFE_EVS_INFORMATION, 
                                  "Reload Application %s Completed.", AppStartParams.Name);
            }
            else
            {
               CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR3_EID, CFE_EVS_ERROR, 
                                  "Reload Application %s Failed: AppCreate Error 0x%08X.", AppStartParams.Name, (unsigned int)Status);
            }
         }
         else
         {
            CFE_EVS_SendEvent(CFE_ES_RELOAD_APP_ERR4_EID, CFE_EVS_ERROR, 
                              "Reload Application %s Failed: CleanUpApp Error 0x%08X.", AppStartParams.Name, (unsigned int)Status);
         }
                                           
         break;
                
      case CFE_ES_RUNSTATUS_SYS_EXCEPTION:
      
         CFE_EVS_SendEvent(CFE_ES_PCR_ERR1_EID, CFE_EVS_ERROR, 
                            "ES_ProcControlReq: Invalid State (EXCEPTION) Application %s.",
                             AppStartParams.Name);            
         break;
         
      default:
 
         CFE_EVS_SendEvent(CFE_ES_PCR_ERR2_EID, CFE_EVS_ERROR, 
                            "ES_ProcControlReq: Unknown State ( %d ) Application %s.",
                            (int)CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest, AppStartParams.Name);
         break;
      
   } 
   
} /* End Function */

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanUpApp
**
**   Purpose: Delete an application by cleaning up all of it's resources.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_CleanUpApp(uint32 AppId)
{
   uint32    i;
   int32  Status;
   uint32 MainTaskId;
   int32  ReturnCode = CFE_SUCCESS;

   #ifdef ES_APP_DEBUG
      OS_printf("------------- Starting App Cleanup: AppID = %d -----------\n",AppId);
      CFE_ES_ListResourcesDebug();
   #endif
   
   /*
   ** Call the Table Clean up function
   */
#ifndef EXCLUDE_CFE_TBL
   CFE_TBL_CleanUpApp(AppId);
#endif   
   /*
   ** Call the Software Bus clean up function
   */
   CFE_SB_CleanUpApp(AppId);
   
   /*
   ** Call the TIME Clean up function
   */
   CFE_TIME_CleanUpApp(AppId);
     
   /*
   ** Call the EVS Clean up function
   */
   Status = CFE_EVS_CleanUpApp(AppId);
   if ( Status != CFE_SUCCESS )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: Call to CFE_EVS_CleanUpApp returned Error: 0x%08X\n",(unsigned int)Status);
      ReturnCode = CFE_ES_APP_CLEANUP_ERR;
   }
     
   
   /*
   ** Delete the ES Resources
   */
   CFE_ES_LockSharedData(__func__,__LINE__);
        
   /*
   ** Get Main Task ID
   */
   MainTaskId = CFE_ES_Global.AppTable[AppId].TaskInfo.MainTaskId;   

   /*
   ** Delete all of the OS resources, close files, and delete the main task
   */
   Status = CFE_ES_CleanupTaskResources(MainTaskId);
   if ( Status != CFE_SUCCESS )
   {
      CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: CleanUpTaskResources for Task ID:%d returned Error: 0x%08X\n",
                               (int)MainTaskId, (unsigned int)Status);
      ReturnCode = CFE_ES_APP_CLEANUP_ERR;
 
   }

   /*
   ** Delete any child tasks associated with this app
   */
   for ( i = 0; i < OS_MAX_TASKS; i++ )
   {
      /* delete only CHILD tasks - not the MainTaskId, which is already deleted (above) */
      if ((CFE_ES_Global.TaskTable[i].RecordUsed == TRUE) &&
          (CFE_ES_Global.TaskTable[i].AppId == AppId) &&
          (CFE_ES_Global.TaskTable[i].TaskId != MainTaskId))
      {         
         Status = CFE_ES_CleanupTaskResources(CFE_ES_Global.TaskTable[i].TaskId);
         if ( Status != CFE_SUCCESS )
         {
            CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: CleanUpTaskResources for Task ID:%d returned Error: 0x%08X\n",
                                  (int)i, (unsigned int)Status);
            ReturnCode = CFE_ES_APP_CLEANUP_ERR;
         }
      } /* end if */
   } /* end for */

   /*
   ** Remove the app from the AppTable
   */
   if ( CFE_ES_Global.AppTable[AppId].Type == CFE_ES_APP_TYPE_EXTERNAL )
   {
      /*
      ** Unload the module only if it is an external app
      */
      Status = OS_ModuleUnload(CFE_ES_Global.AppTable[AppId].StartParams.ModuleId);
      if ( Status == OS_ERROR )
      {
           CFE_ES_WriteToSysLog("CFE_ES_CleanUpApp: Module (ID:0x%08X) Unload failed. RC=0x%08X\n",
                                 (unsigned int)CFE_ES_Global.AppTable[AppId].StartParams.ModuleId, (unsigned int)Status);
           ReturnCode = CFE_ES_APP_CLEANUP_ERR;
      }
      CFE_ES_Global.RegisteredExternalApps--;
   }
      
   CFE_ES_Global.AppTable[AppId].RecordUsed = FALSE;

    #ifdef ES_APP_DEBUG   
       OS_TaskDelay(1000);   
       CFE_ES_ListResourcesDebug();
       printf("--------- Finished CFE_ES_CleanUpApp-------------\n");
    #endif 

   CFE_ES_UnlockSharedData(__func__,__LINE__);
    
   return(ReturnCode);
   
} /* end function */


/*
 * Better methods to find OSAL resources associated with a task
 * have been added in the updated version of OSAL.  Instead of just
 * generating an ID in the range of 0..OS_MAX_<OBJTYPE> and calling
 * OSAL to see if it is valid or not, the OSAL now has a dedicated
 * API to loop through the IDs that _actually are_ valid and call
 * a user-defined function for each one.
 *
 * This is leveraged if a new OSAL is being used, otherwise fall back
 * to assuming that the OSAL ID will be in the range of O..OS_MAX_<OBJTYPE>
 */
#if defined(OSAL_OPAQUE_OBJECT_IDS)

/*
 * Simple state structure used when cleaning up objects associated with tasks
 *
 * This is used locally between CFE_ES_CleanupTaskResources and the
 * CFE_ES_CountObjectCallback helper function.
 */
typedef struct
{
    uint32 ErrorFlag;
    uint32 FoundObjects;
    uint32 PrevFoundObjects;
    uint32 DeletedObjects;
    int32  OverallStatus;
} CFE_ES_CleanupState_t;

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CountObjectCallback
**
**   Purpose: Helper function for CFE_ES_ListResourcesDebug to count all objects.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CleanupObjectCallback(uint32 ObjectId, void *arg)
{
    CFE_ES_CleanupState_t   *CleanState;
    int32                   Status;
    uint32                  ObjType;
    osalbool                ObjIsValid;

    CleanState = (CFE_ES_CleanupState_t *)arg;
    ObjIsValid = TRUE;

    ObjType = OS_IdentifyObject(ObjectId);
    switch(ObjType)
    {
    case OS_OBJECT_TYPE_OS_TASK:
        Status = OS_TaskDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_QUEUE:
        Status = OS_QueueDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_BINSEM:
        Status = OS_BinSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_COUNTSEM:
        Status = OS_CountSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_MUTEX:
        Status = OS_MutSemDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_TIMECB:
        Status = OS_TimerDelete(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_STREAM:
        Status = OS_close(ObjectId);
        break;
    case OS_OBJECT_TYPE_OS_MODULE:
        Status = OS_ModuleUnload(ObjectId);
        break;
    default:
        ObjIsValid = FALSE;
        Status = OS_ERROR;
        break;
    }

    if (ObjIsValid)
    {
        ++CleanState->FoundObjects;
        if (Status == OS_SUCCESS)
        {
            ++CleanState->DeletedObjects;
        }
        else
        {
            CFE_ES_WriteToSysLog("Call to OSAL Delete Object (ID:%d) failed. RC=0x%08X\n",
                         (int)ObjectId, (unsigned int)Status);
            if (CleanState->OverallStatus == CFE_SUCCESS)
            {
                /*
                 * Translate any OS failures into the appropriate CFE_ES return codes
                 * (Some object types have special return codes, depending on what type
                 * of object failed to delete)
                 */
                switch(ObjType)
                {
                case OS_OBJECT_TYPE_OS_TASK:
                    CleanState->OverallStatus = CFE_ES_ERR_CHILD_TASK_DELETE;
                    break;
                case OS_OBJECT_TYPE_OS_QUEUE:
                    CleanState->OverallStatus = CFE_ES_QUEUE_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_BINSEM:
                    CleanState->OverallStatus = CFE_ES_BIN_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_COUNTSEM:
                    CleanState->OverallStatus = CFE_ES_COUNT_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_MUTEX:
                    CleanState->OverallStatus = CFE_ES_MUT_SEM_DELETE_ERR;
                    break;
                case OS_OBJECT_TYPE_OS_TIMECB:
                    CleanState->OverallStatus = CFE_ES_TIMER_DELETE_ERR;
                    break;
                default:
                    /* generic failure */
                    CleanState->OverallStatus = CFE_ES_APP_CLEANUP_ERR;
                    break;
                }
            }
        }
    }
}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanupTaskResources
**
**   Purpose: Clean up the OS resources associated with an individual Task
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_CleanupTaskResources(uint32 TaskId)
{
    CFE_ES_CleanupState_t   CleanState;
    int32                   Result;

    /*
    ** Delete all OSAL resources that belong to this task
    */
    memset(&CleanState, 0, sizeof(CleanState));
    --CleanState.PrevFoundObjects;
    while (1)
    {
        OS_ForEachObject (TaskId, CFE_ES_CleanupObjectCallback, &CleanState);
        if (CleanState.FoundObjects == 0 || CleanState.ErrorFlag != 0)
        {
            break;
        }
        /*
         * The number of found objects should show a downward trend,
         * if not, then stop and do not loop here forever.  (This can
         * happen when using the UT stub functions, or if an object
         * cannot be fully deleted successfully).
         */
        CleanState.ErrorFlag = (CleanState.DeletedObjects == 0 ||
                CleanState.FoundObjects >= CleanState.PrevFoundObjects);
        CleanState.PrevFoundObjects = CleanState.FoundObjects;
        CleanState.FoundObjects = 0;
        CleanState.DeletedObjects = 0;
    }

    /*
    ** Delete the task itself
    */
    Result = OS_TaskDelete(TaskId);
    if (Result == OS_SUCCESS)
    {
        Result = CleanState.OverallStatus;
        if (Result == CFE_SUCCESS && CleanState.FoundObjects > 0)
        {
            /* Objects leftover after cleanup -- resource leak */
            Result = CFE_ES_APP_CLEANUP_ERR;
        }
    }
    else
    {
        Result = CFE_ES_TASK_DELETE_ERR;
    }

    /*
    ** Invalidate ES Task Table entry
    */
    if (OS_ConvertToArrayIndex(TaskId, &TaskId) == OS_SUCCESS)
    {
       CFE_ES_Global.TaskTable[TaskId].RecordUsed = FALSE;
    }

    CFE_ES_Global.RegisteredTasks--;
    return(Result);

}

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CountObjectCallback
**
**   Purpose: Helper function for CFE_ES_ListResourcesDebug to count all objects.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_CountObjectCallback(uint32 ObjectId, void *arg)
{
    uint32                 *CountState;
    uint32                 idtype;

    CountState = (uint32 *)arg;
    idtype = OS_IdentifyObject(ObjectId);
    if (idtype < OS_OBJECT_TYPE_USER)
    {
        ++CountState[idtype];
    }
}


/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_ListResourcesDebug
**
**   Purpose: List the resources.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_ListResourcesDebug(void)
{
    uint32   CountState[OS_OBJECT_TYPE_USER];

    memset(CountState,0,sizeof(CountState));
    OS_ForEachObject (0, CFE_ES_CountObjectCallback, CountState);

    OS_printf("OS Resources in Use:\n");
    OS_printf("Number of Tasks: %d\n", (int) CountState[OS_OBJECT_TYPE_OS_TASK]);
    OS_printf("Number of Queues: %d\n", (int) CountState[OS_OBJECT_TYPE_OS_QUEUE]);
    OS_printf("Number of Counting Semaphores: %d\n",(int) CountState[OS_OBJECT_TYPE_OS_COUNTSEM]);
    OS_printf("Number of Binary Semaphores: %d\n",(int) CountState[OS_OBJECT_TYPE_OS_BINSEM]);
    OS_printf("Number of Mutexes: %d\n", (int) CountState[OS_OBJECT_TYPE_OS_MUTEX]);
    OS_printf("Number of Open Files: %d\n",(int) CountState[OS_OBJECT_TYPE_OS_STREAM]);

    return CFE_SUCCESS;

}


#else  /* Implementation for old versions of OSAL where object IDs are always zero based */


/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_CleanupTaskResources
**
**   Purpose: Clean up the OS resources associated with an individual Task
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_CleanupTaskResources(uint32 TaskId)
{
    OS_queue_prop_t         QueueProp;
    OS_timer_prop_t         TimerProp;
    OS_bin_sem_prop_t       SemProp;
    OS_count_sem_prop_t     CountSemProp;
    OS_mut_sem_prop_t       MutProp;
    OS_FDTableEntry         FileProp;
    
    int32                   Status;
    
    int32 Result = CFE_SUCCESS;
    uint32 i;

    /*
    ** Delete Mutexes that belong to this task
    */
    for ( i= 0; i < OS_MAX_MUTEXES; i++)
    {
        if (OS_MutSemGetInfo(i, &MutProp) == OS_SUCCESS)
        {
            if ( MutProp.creator == TaskId )
            {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found a Mutex that belongs to Task %d\n", TaskId);
               #endif
               
               /*
               ** Delete it
               */
               Status = OS_MutSemDelete(i);
               if ( Status != OS_SUCCESS )
               {
                  CFE_ES_WriteToSysLog("Call to OS_MutSemDelete (ID:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_MUT_SEM_DELETE_ERR;
               }
               
            }            
        }
    }

    /*
    ** Delete Binary Semaphores that belong to this task
    */
    for ( i= 0; i < OS_MAX_BIN_SEMAPHORES; i++)
    {
        if (OS_BinSemGetInfo(i, &SemProp) == OS_SUCCESS)
        {
           if(SemProp.creator == TaskId )
           {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found a Bin Semaphore that belongs to Task %d\n", TaskId);
               #endif
              /*
              ** Delete it
              */
              Status = OS_BinSemDelete(i);
               if ( Status != OS_SUCCESS )
               {
                  CFE_ES_WriteToSysLog("Call to OS_BinSemDelete (ID:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_BIN_SEM_DELETE_ERR;

               }

           }
         
        }
    }

    /*
    ** Delete Counting Semaphores that belong to this task
    */
    for ( i= 0; i < OS_MAX_COUNT_SEMAPHORES; i++)
    {
       if (OS_CountSemGetInfo(i, &CountSemProp) == OS_SUCCESS)
        {
           if(CountSemProp.creator == TaskId )
           {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found a Counting Semaphore that belongs to Task %d\n", TaskId);
               #endif
              /*
              ** Delete it
              */
              Status = OS_CountSemDelete(i);
               if ( Status != OS_SUCCESS )
               {
                  CFE_ES_WriteToSysLog("Call to OS_CountSemDelete (ID:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_COUNT_SEM_DELETE_ERR;

               }

           }
        }
    }


    /*
    ** Delete Queues that belong to this task
    */
    for ( i= 0; i < OS_MAX_QUEUES; i++)
    {
        if (OS_QueueGetInfo(i, &QueueProp) == OS_SUCCESS)
        {
           if(QueueProp.creator == TaskId )
           {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found a Queue that belongs to Task %d\n", TaskId);
               #endif
              /*
              ** Delete it
              */
              Status = OS_QueueDelete(i);
               if ( Status == OS_ERROR )
               {
                  CFE_ES_WriteToSysLog("Call to OS_QueueDelete (ID:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_QUEUE_DELETE_ERR;

               }

           }
        }
    }

    /*
    ** Delete Timers that belong to this task
    */
    for ( i= 0; i < OS_MAX_TIMERS; i++)
    {
        if (OS_TimerGetInfo(i, &TimerProp) == OS_SUCCESS)
        {
           if(TimerProp.creator == TaskId )
           {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found a Timer that belongs to Task %d\n", TaskId);
               #endif
              /*
              ** Delete it
              */
              Status = OS_TimerDelete(i);
               if ( Status != OS_SUCCESS )
               {
                  CFE_ES_WriteToSysLog("Call to OS_TimerDelete (ID:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_TIMER_DELETE_ERR;

               }

           }
        }
    }

    /*
    ** Close files that belong to this task
    */
    for ( i= 0; i < OS_MAX_NUM_OPEN_FILES; i++)
    {
        if (OS_FDGetInfo(i, &FileProp) == OS_FS_SUCCESS)
        {
           if(FileProp.User == TaskId )
           {
               #ifdef ES_APP_DEBUG   
                 OS_printf("Found an open File that belongs to Task %d\n", TaskId);
               #endif
              /*
              ** Close it
              */
              Status = OS_close(i);
               if ( Status < OS_FS_SUCCESS )
               {
                  CFE_ES_WriteToSysLog("Call to OS_close (FD:%d) failed. RC=0x%08X\n",
                        (int)i, (unsigned int)Status);
                  Result = CFE_ES_FILE_CLOSE_ERR;
               }

           }
        }
    }

    /*
    ** Delete the task
    */
    Status = OS_TaskDelete(TaskId);
    if ( Status < OS_FS_SUCCESS )
    {
       CFE_ES_WriteToSysLog("Call to OS_TaskDelete (TaskID:%d) failed. RC=0x%08X\n",
             (int)TaskId, (unsigned int)Status);
       Result = CFE_ES_TASK_DELETE_ERR;
    }
    
    /*
    ** Invalidate ES Task Table entry
    */
    CFE_ES_Global.TaskTable[TaskId].RecordUsed = FALSE;
    CFE_ES_Global.RegisteredTasks--;
    
    return(Result);

}


/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_ListResourcesDebug
**
**   Purpose: List the resources.
**---------------------------------------------------------------------------------------
*/
int32 CFE_ES_ListResourcesDebug(void)
{
    OS_task_prop_t        TaskProp;
    OS_queue_prop_t       QueueProp;
    OS_bin_sem_prop_t     SemProp;
    OS_count_sem_prop_t   CountSemProp;
    OS_mut_sem_prop_t     MutProp;
    OS_FDTableEntry       FileProp;
    int32                 NumSemaphores = 0;
    int32                 NumCountSems =0;
    int32                 NumMutexes = 0;
    int32                 NumQueues = 0;
    int32                 NumTasks = 0;
    int32                 NumFiles = 0;
    uint32                i;

    OS_printf("OS Resources in Use:\n");

    for ( i= 0; i < OS_MAX_TASKS; i++)
    {
        if (OS_TaskGetInfo(i, &TaskProp) == OS_SUCCESS)
        {
            NumTasks++;
        }
    }
    OS_printf("Number of Tasks: %d\n", (int) NumTasks);

    for ( i= 0; i < OS_MAX_QUEUES; i++)
    {
        if (OS_QueueGetInfo(i, &QueueProp) == OS_SUCCESS)
        {
            NumQueues++;
        }
    }
    OS_printf("Number of Queues: %d\n", (int) NumQueues);


    for ( i= 0; i < OS_MAX_COUNT_SEMAPHORES; i++)
    {
       if (OS_CountSemGetInfo(i, &CountSemProp) == OS_SUCCESS)
        {
            NumCountSems++;
        }
    }
    OS_printf("Number of Counting Semaphores: %d\n",(int) NumCountSems);
    
    
    for ( i= 0; i < OS_MAX_BIN_SEMAPHORES; i++)
    {
        if (OS_BinSemGetInfo(i, &SemProp) == OS_SUCCESS)
        {
            NumSemaphores++;
        }
    }
    OS_printf("Number of Binary Semaphores: %d\n",(int) NumSemaphores);


    for ( i= 0; i < OS_MAX_MUTEXES; i++)
    {
        if (OS_MutSemGetInfo(i, &MutProp) == OS_SUCCESS)
        {
            NumMutexes++;
        }
    }
    OS_printf("Number of Mutexes: %d\n", (int) NumMutexes);

    for ( i= 0; i < OS_MAX_NUM_OPEN_FILES; i++)
    {
        if (OS_FDGetInfo(i, &FileProp) == OS_FS_SUCCESS)
        {
            NumFiles++;
        }
    }
    OS_printf("Number of Open Files: %d\n",(int) NumFiles);

    return CFE_SUCCESS;

}

#endif

/*
**---------------------------------------------------------------------------------------
**   Name: CFE_ES_GetAppInfoInternal
**
**   Purpose: Populate the cFE_ES_AppInfo structure with the data for an app.
**---------------------------------------------------------------------------------------
*/
void CFE_ES_GetAppInfoInternal(uint32 AppId, CFE_ES_AppInfo_t *AppInfoPtr )
{

   int32              ReturnCode;
   OS_module_record_t ModuleInfo;
   uint32             TaskIndex;
   
   
   CFE_ES_LockSharedData(__func__,__LINE__);

   AppInfoPtr->AppId = AppId;
   AppInfoPtr->Type = CFE_ES_Global.AppTable[AppId].Type;
   strncpy((char *)AppInfoPtr->Name, (char *)CFE_ES_Global.AppTable[AppId].StartParams.Name, OS_MAX_API_NAME);
   AppInfoPtr->Name[OS_MAX_API_NAME - 1] = '\0';

   strncpy((char *)AppInfoPtr->EntryPoint, (char *)CFE_ES_Global.AppTable[AppId].StartParams.EntryPoint, OS_MAX_API_NAME);
   AppInfoPtr->EntryPoint[OS_MAX_API_NAME - 1] = '\0';
   
   strncpy((char *)AppInfoPtr->FileName, (char *)CFE_ES_Global.AppTable[AppId].StartParams.FileName, OS_MAX_PATH_LEN);
   AppInfoPtr->FileName[OS_MAX_PATH_LEN - 1] = '\0';
   
   AppInfoPtr->ModuleId = CFE_ES_Global.AppTable[AppId].StartParams.ModuleId;
   AppInfoPtr->StackSize = CFE_ES_Global.AppTable[AppId].StartParams.StackSize;
   CFE_SB_SET_MEMADDR(AppInfoPtr->StartAddress, CFE_ES_Global.AppTable[AppId].StartParams.StartAddress);
   AppInfoPtr->ExceptionAction = CFE_ES_Global.AppTable[AppId].StartParams.ExceptionAction;
   AppInfoPtr->Priority = CFE_ES_Global.AppTable[AppId].StartParams.Priority;
 
   AppInfoPtr->MainTaskId = CFE_ES_Global.AppTable[AppId].TaskInfo.MainTaskId;   
   strncpy((char *)AppInfoPtr->MainTaskName, (char *)CFE_ES_Global.AppTable[AppId].TaskInfo.MainTaskName, OS_MAX_API_NAME);
   AppInfoPtr->MainTaskName[OS_MAX_API_NAME - 1] = '\0';
   AppInfoPtr->NumOfChildTasks = CFE_ES_Global.AppTable[AppId].TaskInfo.NumOfChildTasks;

   /*
   ** Get the execution counter for the main task
   */
   if (OS_ConvertToArrayIndex(AppInfoPtr->MainTaskId, &TaskIndex) == OS_SUCCESS)
   {
      AppInfoPtr->ExecutionCounter = CFE_ES_Global.TaskTable[TaskIndex].ExecutionCounter;
   }

   /* 
   ** Get the address information from the OSAL
   */
   ReturnCode = OS_ModuleInfo ( AppInfoPtr->ModuleId, &ModuleInfo );
   if ( ReturnCode == OS_SUCCESS )
   {
      AppInfoPtr->AddressesAreValid = ModuleInfo.addr.valid;
      CFE_SB_SET_MEMADDR(AppInfoPtr->CodeAddress, ModuleInfo.addr.code_address);
      CFE_SB_SET_MEMADDR(AppInfoPtr->CodeSize, ModuleInfo.addr.code_size);
      CFE_SB_SET_MEMADDR(AppInfoPtr->DataAddress, ModuleInfo.addr.data_address);
      CFE_SB_SET_MEMADDR(AppInfoPtr->DataSize, ModuleInfo.addr.data_size);
      CFE_SB_SET_MEMADDR(AppInfoPtr->BSSAddress, ModuleInfo.addr.bss_address);
      CFE_SB_SET_MEMADDR(AppInfoPtr->BSSSize, ModuleInfo.addr.bss_size);
   } 
   else
   {
      AppInfoPtr->AddressesAreValid = FALSE;
      AppInfoPtr->CodeAddress = 0;
      AppInfoPtr->CodeSize = 0;
      AppInfoPtr->DataAddress = 0;
      AppInfoPtr->DataSize = 0;
      AppInfoPtr->BSSAddress = 0;
      AppInfoPtr->BSSSize = 0;
   }
   


   CFE_ES_UnlockSharedData(__func__,__LINE__);

} /* end function */

