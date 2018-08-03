/*
**  File: 
**  cfe_es_start.c
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
**  This file contains the Main entrypoint and startup code for the cFE core.
**  The entry point is called by the board support package for the OS. When the
**  entry point is finished, the cFE should be fully initialized and running.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  $Log: cfe_es_start.c  $
**  Revision 1.16 2014/09/05 11:39:05GMT-05:00 acudmore 
**  Updated CFE_ES_SetupResetVariables to correctly log the Boot Source ( bank ) and clarify ERLog and Syslog text for restarts.
**  Revision 1.15 2014/08/22 15:50:11GMT-05:00 lwalling 
**  Changed signed loop counters to unsigned
**  Revision 1.14 2014/07/23 15:39:42EDT acudmore 
**  Changed where processor reset count is incremented to make it more consistent.
**  Removed 2nd ERLog entry when a processor reset reverts to a power on reset.
**  Clarified ERlog entry text.
**  Revision 1.13 2012/10/01 16:29:46GMT-05:00 aschoeni 
**  Fixed missing parenthesis issue
**  Revision 1.12 2012/07/16 16:18:21EDT lwalling 
**  Added code to release stuck startup sync semaphore
**  Revision 1.11 2012/01/13 08:50:04PST acudmore 
**  Changed license text to reflect open source
**  Revision 1.10 2010/11/08 12:33:04EST acudmore 
**  Updated logic that checks for max processor reset count during a non-cfe caused reset ( watchdog ).
**  Revision 1.9 2010/11/05 15:54:34EDT aschoeni 
**  Added Generic Counter API to ES
**  Revision 1.8 2010/11/04 14:05:56EDT acudmore 
**  Added ram disk mount path configuration option.
**  Revision 1.7 2010/09/21 15:52:17EDT jmdagost 
**  Replaced syslog argument RetStatus with BlocksFree.
**  Revision 1.6 2009/07/30 16:06:55EDT acudmore 
**  Fixed check for blocks free in RAM disk ( > vs >= )
**  Revision 1.5 2009/07/28 16:33:19EDT jmdagost 
**  Replaced 3 references to filename with references to function name.
**  Revision 1.4 2009/06/10 09:09:01EDT acudmore 
**  Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
**  Revision 1.3 2008/07/30 14:53:30EDT apcudmore 
**  Updated init code for startup sync support.
**  Revision 1.2 2008/06/20 15:40:32EDT apcudmore 
**  Added support for OSAL based module loader
**   - Removed calls and references to the BSP based module loader
**  Revision 1.1 2008/04/17 08:05:08EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.46 2007/09/21 15:40:20EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.45 2007/05/31 10:13:56EDT apcudmore 
**  Fixed Syslog message text for Volatile Disk initialization
**  Revision 1.44 2007/05/30 10:26:06EDT apcudmore 
**  Added Exception handler support:
**   - BSP routine to attach processor exceptions
**   - context datatype and size define
**   - added default exception init for each app+task
**  Revision 1.43 2007/05/25 14:20:10EDT apcudmore 
**  Fixed compiler warnings
**  Revision 1.42 2007/05/25 10:38:12EDT apcudmore 
**  Added OS_rmfs call to remove file system before reformatting it.
**  Revision 1.41 2007/05/25 10:02:42EDT apcudmore 
**  Fixed Typo and Moved WriteToSysLog call
**  Revision 1.40 2007/05/24 15:09:23EDT apcudmore 
**  App-Decompression updates:
**    - Added verify macros for PERCENT_FREE config parameter
**    - Updated comments in config files
**    - Updated logic to turn off check if RAM_DISK_PERCENT_FREE is 0
**  Revision 1.39 2007/05/24 10:57:48EDT apcudmore 
**  Application Decompression Support:
**   - Added configuration parameter for Percent of space free in Volatile disk on a P.R.
**   - Added logic to check for amount of free space and reformat RAM disk if needed on a P.R.
**   - Fixed warnings in decompression code.
**  Revision 1.38 2007/05/22 14:56:55EDT apcudmore 
**  cFE Non-Volatile Disk/Startup File changes:
**    - cFE startup now passes in startup file path/name
**    - cFE startup code does not init non-volatile disk ( BSP does )
**    - BSPs now can map cFE paths without making the OS_initfs/OS_mount calls
**    - MCP750 vxWorks6.2 BSP inits/mounts CF disk
**    - Desktop ports pass in the same startup file path/name as before
**  Revision 1.37 2007/05/07 15:56:32EDT apcudmore 
**  Removed all ES startup semaphore code
**  Removed all ES startup delays.
**  Startup sync problems ( seem to be ) eliminated by protecting the ES global data tables with a mutex.
**  Revision 1.36 2007/05/04 15:58:36EDT apcudmore 
**  Implemented ES shared data protection. Eliminates Startup syncronization problem.
**  Revision 1.35 2007/04/13 16:28:52EDT apcudmore 
**  Changed function prototype to return int32 status on the ES Object table functions. 
**  ES will now call OS_BSPPanic when the function returns something other than CFE_SUCCESS
**  Revision 1.34 2007/04/10 13:50:46EDT rjmcgraw 
**  Renamed function setupAnalyzerVariables to SetupPerfVariables
**  Revision 1.33 2007/03/29 13:29:16EST apcudmore 
**  Updated global variables and housekeeping to count:
**  - Registered Core Apps
**  - Registered External Apps
**  - ALL Tasks 
**  Revision 1.32 2007/03/22 10:26:23EST apcudmore 
**  Implemented all code walkthrough items related to ES Application logic, commands, 
**  and telemetry.
**  Revision 1.31 2007/03/13 14:32:37EST apcudmore 
**  Fixed ES Startup code walkthrough items.
*/

/*
** Includes
*/

#include "private/cfe_private.h"
#include "cfe_es.h"
#include "cfe_es_global.h"       
#include "cfe_es_start.h" 
#include "cfe_es_apps.h"
#include "cfe_es_log.h"
#include "cfe_psp.h"

#include <stdio.h>
#include <string.h>

/***************************************************************************/

/*
** Defines for this module
*/

/* 
** Number of msecs to delay before exiting cFE. Allows LogMsg to get through 
*/
#define CFE_ES_PANIC_DELAY  500


/*
** Global data for the ES startup code and Runtime library
*/
CFE_ES_Global_t     CFE_ES_Global;

/*
** Pointer to the Reset data that is preserved on a processor reset
*/
CFE_ES_ResetData_t  *CFE_ES_ResetDataPtr;


/***************************************************************************/
/*
** Code
*/

/*
** Name: CFE_ES_Main
** Purpose: This is the entry point to the cFE application code.
**
*/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath )
{
   uint32 i;
   int32 ReturnCode;

   /*
   ** Indicate that the CFE is the earliest initialization state
   */
   CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_EARLY_INIT;

   /*
   ** Initialize the Reset variables. This call is required
   ** Before most of the ES functions can be used including the 
   ** ES System log.
   */
   CFE_ES_SetupResetVariables(StartType, StartSubtype, ModeId);

   /*
   ** Initialize the Logic Perf variables
   ** Because this is in the ES Reset area, it must be called after
   ** CFE_ES_SetupResetVariables.
   */
   CFE_ES_SetupPerfVariables(StartType);

   /*
   ** Announce the startup
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main in EARLY_INIT state\n");

   /*
   ** Create and Mount the filesystems needed
   */
   CFE_ES_InitializeFileSystems(StartType);
   
   /*
   ** Install exception Handlers ( Placeholder )
   */
   CFE_PSP_AttachExceptions();

   /*
   ** Initialize the ES Application Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < CFE_ES_MAX_APPLICATIONS; i++ )
   {
      CFE_ES_Global.AppTable[i].RecordUsed = FALSE;
   }
   
   /*
   ** Initialize the ES Task Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < OS_MAX_TASKS; i++ )
   {
      CFE_ES_Global.TaskTable[i].RecordUsed = FALSE;
   }

   /*
   ** Initialize the ES Generic Counter Table
   ** to mark all entries as unused.
   */
   for ( i = 0; i < CFE_ES_MAX_GEN_COUNTERS; i++ )
   {
      CFE_ES_Global.CounterTable[i].RecordUsed = FALSE;
   }

   /*
   ** Create the ES Shared Data Mutex
   */
   ReturnCode = OS_MutSemCreate(&(CFE_ES_Global.SharedDataMutex), "ES_DATA_MUTEX", 0 );
   if(ReturnCode != OS_SUCCESS)
   {
      CFE_ES_WriteToSysLog("ES Startup: Error: ES Shared Data Mutex could not be created. RC=0x%08X\n",
              (unsigned int)ReturnCode);
         
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up. 
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_STARTUP_SEM);
         
   } /* end if */

   /*
   ** Indicate that the CFE core is now starting up / going multi-threaded
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering CORE_STARTUP state\n");
   CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_CORE_STARTUP;

   /*
   ** Create the tasks, OS objects, and initialize hardware
   */
   CFE_ES_CreateObjects();

   /*
   ** Indicate that the CFE core is ready
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering CORE_READY state\n");
   CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_CORE_READY;

   /*
   ** Start the cFE Applications from the disk using the file
   ** specified in the CFE_ES_NONVOL_STARTUP_FILE or CFE_ES_VOLATILE_STARTUP_FILE 
   ** ( defined in the cfe_platform_cfg.h file )
   */   
   CFE_ES_StartApplications(StartType, StartFilePath );

   /*
    * Wait for applications to be "running" before declaring as operational.
    * However, if not everything starts up, that is not a fatal error, we will
    * continue anyway since the core apps are OK and control/telemetry should function.
    * The problem app could be deleted/restarted/etc by the ground station.
    */
   if (CFE_ES_ApplicationSyncDelay(CFE_ES_SYSTEM_STATE_UNDEFINED,
           CFE_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) != CFE_SUCCESS)
   {
       CFE_ES_WriteToSysLog("ES Startup: Startup Sync failed - Applications may not have all started\n");
   }

   /*
   ** Startup is fully complete
   */
   CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Main entering OPERATIONAL state\n");
   CFE_ES_Global.SystemState = CFE_ES_SYSTEM_STATE_OPERATIONAL;
}

/*
** Name: CFE_ES_SetupResetVariables
**
** Purpose: This function initializes the ES reset variables depending on the reset type.
**          It will also initiate a power on reset when too many processor resets
**           have happened.
**
*/
void CFE_ES_SetupResetVariables(uint32 StartType, uint32 StartSubtype, uint32 BootSource )
{

   int32  status;
   uint32 resetAreaSize;
   cpuaddr ResetDataAddr;
   
   /*
   ** Get the pointer to the Reset area from the BSP
   */
   status = CFE_PSP_GetResetArea (&ResetDataAddr, &resetAreaSize);
      
   /*
   ** Make sure the status is OK or size is big enough
   */
   if ( status != CFE_PSP_SUCCESS )
   {
      /*
      ** Cannot use the ES System log without the Reset Area
      */
      OS_printf("ES Startup: CFE_PSP_GetResetArea call Failed (0x%08x)!\n", (unsigned int)status);
      
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up. 
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
      
   }
   else if ( resetAreaSize < sizeof(CFE_ES_ResetData_t))
   {
      /*
      ** Cannot use the ES system log without the Reset Area
      */
      OS_printf("ES Startup: Error: ES Reset area not big enough. Needed: %d, Given: %d.\n",
            (int)sizeof(CFE_ES_ResetData_t),
            (int)resetAreaSize);
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up. 
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);
      
   }

   CFE_ES_ResetDataPtr = (CFE_ES_ResetData_t *)ResetDataAddr;

   /*
   ** Record the BootSource (bank) so it will be valid in the ER log entries.
   */
   CFE_ES_ResetDataPtr->ResetVars.BootSource   = BootSource;

   /*
   ** Determine how the system was started. The choices are:
   **   CFE_ES_POWER_ON_RESET, or CFE_PSP_RST_TYPE_PROCESSOR
   ** The subtypes include:
   **   CFE_ES_POWER_CYCLE, CFE_ES_PUSH_BUTTON, CFE_ES_HW_SPECIAL_COMMAND,
   **   CFE_ES_HW_WATCHDOG, CFE_PSP_RST_TYPE_COMMAND, or CFE_ES_EXCEPTION.
   ** Some of these reset types are logged before the system is restarted.
   **  ( CFE_PSP_RST_TYPE_COMMAND, CFE_ES_EXCEPTION ) while others occur
   **  without the knowledge of the software and must be logged here.
   */
   if ( StartType == CFE_PSP_RST_TYPE_POWERON )
   {
      /*
      ** Record the reset type and subtype
      */
      CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
      CFE_ES_ResetDataPtr->ResetVars.ResetType = CFE_PSP_RST_TYPE_POWERON;

      /*
      ** Log the power-on reset. 
      */
      if ( StartSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE )
      {
         CFE_ES_WriteToSysLog("POWER ON RESET due to Power Cycle (Power Cycle).\n");
         status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                    "POWER ON RESET due to Power Cycle (Power Cycle)", NULL,0 );
      }
      else if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
      {
         CFE_ES_WriteToSysLog("POWER ON RESET due to HW Special Cmd (Hw Spec Cmd).\n");
         status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                    "POWER ON RESET due to HW Special Cmd (Hw Spec Cmd)", NULL,0 );
      }
      else
      {
         CFE_ES_WriteToSysLog("POWER ON RESET due to other cause (See Subtype).\n");
         status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                    "POWER ON RESET due to other cause (See Subtype)", NULL,0 );
      }

      /*
      ** Initialize all reset counters.
      */
      CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
      CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = CFE_ES_MAX_PROCESSOR_RESETS;
      CFE_ES_Global.DebugVars.DebugFlag = 0;
      
   }
   else if ( StartType == CFE_PSP_RST_TYPE_PROCESSOR )
   {
      /*
      ** If a Processor reset was not commanded, it must be a watchdog or other non-commanded reset
      ** Log the reset before updating any reset variables.
      */
      if ( CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset != TRUE )
      {
         CFE_ES_ResetDataPtr->ResetVars.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
         CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype; 
         CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount++;
         
         /*
         ** When coming up from a Processor reset that was not caused by ES, check to see 
         ** if the maximum number has been exceeded
         */
         if ( CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount > 
              CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount )
         {
             if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
             {
                 CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                 CFE_ES_WriteToSysLog("POWER ON RESET due to max proc resets (HW Spec Cmd).\n");

                 /*
                 ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                 ** the entry just in case something fails.
                 */
                 status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                       "POWER ON RESET due to max proc resets (HW Spec Cmd).", NULL,0 );
             }
             else
             {
                 CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                 CFE_ES_WriteToSysLog("POWER ON RESET due to max proc resets (Watchdog).\n");

                 /*
                 ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
                 ** the entry just in case something fails.
                 */
                 status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_POWERON, StartSubtype,
                                       "POWER ON RESET due to max proc resets (Watchdog).", NULL,0 );
             } 
             /*
             ** Call the BSP reset routine 
             */
             CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
             
             /*
             ** Should not return here.
             */
             CFE_ES_WriteToSysLog("ES Startup: Error: CFE_PSP_Restart returned.\n");
           
         }
         else /* Maximum processor reset not exceeded */
         {
             if ( StartSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND )
             {
                CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND;
                CFE_ES_WriteToSysLog("PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd).\n");

                /*
                ** Log the watchdog reset 
                */
                status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                       "PROCESSOR RESET due to Hardware Special Command (Hw Spec Cmd).", NULL,0 );
 
             }
             else
             {
                CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                CFE_ES_WriteToSysLog("PROCESSOR RESET due to Watchdog (Watchdog).\n");

                /*
                ** Log the watchdog reset 
                */
                status =  CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY, CFE_PSP_RST_TYPE_PROCESSOR, StartSubtype,
                                       "PROCESSOR RESET due to Watchdog (Watchdog).", NULL,0 );

             }
 
         } /* end if */
         
      }
      /*
      ** If a processor reset is due to a command or exception, the reset has already been logged.
      ** Update the reset variables only.
      ** The logic for detecting maximum resets is done on the command/exception side
      ** on the "way down" when the command or exception handler is executed.
      */
      else
      {
         CFE_ES_ResetDataPtr->ResetVars.ResetType    = CFE_PSP_RST_TYPE_PROCESSOR;
         CFE_ES_ResetDataPtr->ResetVars.ResetSubtype = StartSubtype;
      }

      /*
      ** Initialize processor reset counters.
      */
      CFE_ES_Global.DebugVars.DebugFlag = 0;
   }
   
   /*
   ** Clear the commanded reset flag, in case a watchdog happens.
   */
   CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = FALSE;
      
}

/*
** Name: CFE_ES_InitializeFileSystems
**
** Purpose: This function initializes the file systems used in the cFE core.
**
*/
void CFE_ES_InitializeFileSystems(uint32 start_type)
{
   int32   RetStatus;
   cpuaddr RamDiskMemoryAddress;
   uint32  RamDiskMemorySize;
   int32   BlocksFree;
   int32   PercentFree;
 
   /* 
   ** Get the memory area for the RAM disk 
   */
   RetStatus = CFE_PSP_GetVolatileDiskMem(&(RamDiskMemoryAddress), &(RamDiskMemorySize));

   if ( RetStatus != CFE_PSP_SUCCESS )
   {
      CFE_ES_WriteToSysLog("ES Startup: Cannot Get Memory for Volatile Disk. EC = 0x%08X\n",(unsigned int)RetStatus);

      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up.  
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);

   }      
   
   /*
   ** Next, either format, or just initialize the RAM disk depending on
   ** the reset type
   */
   if ( start_type == CFE_PSP_RST_TYPE_POWERON )
   {
      RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_ES_RAM_DISK_SECTOR_SIZE, CFE_ES_RAM_DISK_NUM_SECTORS );
      if ( RetStatus != OS_FS_SUCCESS )
      {
         CFE_ES_WriteToSysLog("ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);

         /*
         ** Delay to allow the message to be read
         */
         OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
         /* 
         ** cFE Cannot continue to start up.  
         */
         CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
      }
   }
   else
   {
      RetStatus = OS_initfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_ES_RAM_DISK_SECTOR_SIZE, CFE_ES_RAM_DISK_NUM_SECTORS );
      if ( RetStatus != OS_FS_SUCCESS )
      {
         CFE_ES_WriteToSysLog("ES Startup: Error Initializing Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
         CFE_ES_WriteToSysLog("ES Startup: Formatting Volatile(RAM) Volume.\n");
         
         RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", "RAM", CFE_ES_RAM_DISK_SECTOR_SIZE, CFE_ES_RAM_DISK_NUM_SECTORS );
         if ( RetStatus != OS_SUCCESS )
         {
            CFE_ES_WriteToSysLog("ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);

            /*
            ** Delay to allow the message to be read
            */
            OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
            /* 
            ** cFE Cannot continue to start up.  
            */
            CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
         }
         
      }
   }

   /*
   ** Now, mount the RAM disk
   */
   RetStatus = OS_mount("/ramdev0", CFE_ES_RAM_DISK_MOUNT_STRING);
   if ( RetStatus != OS_FS_SUCCESS )
   {
      CFE_ES_WriteToSysLog("ES Startup: Error Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
      /*
      ** Delay to allow the message to be read
      */
      OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
      /* 
      ** cFE Cannot continue to start up.  
      */
      CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
   }


   /*
   ** During a Processor reset, if the RAM disk has less than a defined 
   ** amount of free space, reformat and re-mount it.
   ** The parameter being checked is CFE_ES_RAM_DISK_PERCENT_RESERVED
   ** Note: When CFE_ES_RAM_DISK_PERCENT_RESERVED is set to 0, this feature is 
   **       disabled.
   */
   if ((start_type == CFE_PSP_RST_TYPE_PROCESSOR) && (CFE_ES_RAM_DISK_PERCENT_RESERVED > 0))
   {
      /*
      ** See how many blocks are free in the RAM disk
      */
      BlocksFree = OS_fsBlocksFree(CFE_ES_RAM_DISK_MOUNT_STRING);   
      if ( BlocksFree >= 0 )
      {
         /*
         ** Need a sanity check for the desktop systems.
         ** Because the desktop ports map the volatile disk to the host 
         ** hard disk, it will report more free blocks than the defined number
         ** of sectors ( blocks ). Therefore it must be truncated.
         */
         if ( BlocksFree > CFE_ES_RAM_DISK_NUM_SECTORS )
         {
             BlocksFree = CFE_ES_RAM_DISK_NUM_SECTORS - 1;
         }
         
         /*
         ** Determine if the disk is too full 
         */
         BlocksFree = BlocksFree * 100;
         PercentFree = BlocksFree / CFE_ES_RAM_DISK_NUM_SECTORS;
         CFE_ES_WriteToSysLog("Volatile Disk has %d Percent free space.\n",(int)PercentFree);

         if ( PercentFree < CFE_ES_RAM_DISK_PERCENT_RESERVED )
         {
            CFE_ES_WriteToSysLog("ES Startup: Insufficent Free Space on Volatile Disk, Reformatting.\n");
          
            /*
            ** First, unmount the disk
            */
            RetStatus = OS_unmount(CFE_ES_RAM_DISK_MOUNT_STRING);
            if ( RetStatus == OS_FS_SUCCESS )
            {

               /*
               ** Remove the file system from the OSAL
               */
               RetStatus = OS_rmfs("/ramdev0");
               if ( RetStatus == OS_FS_SUCCESS )
               {
               
                  /*
                  ** Next, make a new file system on the disk
                  */
                  RetStatus = OS_mkfs((void *)RamDiskMemoryAddress, "/ramdev0", 
                                      "RAM", CFE_ES_RAM_DISK_SECTOR_SIZE, 
                                       CFE_ES_RAM_DISK_NUM_SECTORS );
                  if ( RetStatus == OS_FS_SUCCESS )
                  {
                     /*
                     ** Last, remount the disk
                     */
                     RetStatus = OS_mount("/ramdev0", CFE_ES_RAM_DISK_MOUNT_STRING);
                     if ( RetStatus != OS_FS_SUCCESS )
                     {
                        CFE_ES_WriteToSysLog("ES Startup: Error Re-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                        /*
                        ** Delay to allow the message to be read
                        */
                        OS_TaskDelay(CFE_ES_PANIC_DELAY);
                     
                        /* 
                        ** cFE Cannot continue to start up.  
                        */
                        CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
                     
                     } /* end if mount */
                  }
                  else
                  {

                     CFE_ES_WriteToSysLog("ES Startup: Error Re-Formating Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                     /*
                     ** Delay to allow the message to be read
                     */
                     OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                     /* 
                     ** cFE Cannot continue to start up.  
                     */
                     CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
                                 
                  } /* end if mkfs */

              }
              else /* could not Remove File system */
              {

                  CFE_ES_WriteToSysLog("ES Startup: Error Removing Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
                  /*
                  ** Delay to allow the message to be read
                  */
                  OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                  /* 
                  ** cFE Cannot continue to start up.  
                  */
                  CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);

              } /* end if OS_rmfs */

            }
            else /* could not un-mount disk */
            {
               CFE_ES_WriteToSysLog("ES Startup: Error Un-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",(unsigned int)RetStatus);
               /*
               ** Delay to allow the message to be read
               */
               OS_TaskDelay(CFE_ES_PANIC_DELAY);
               
               /* 
               ** cFE Cannot continue to start up.  
               */
               CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);            
            }
            
         } /* end if enough free space */
         
      }
      else  /* could not determine free blocks */
      {         
         /* Log error message -- note that BlocksFree returns the error code in this case */
         CFE_ES_WriteToSysLog("ES Startup: Error Determining Blocks Free on Volume. EC = 0x%08X\n",(unsigned int)BlocksFree);

         /*
         ** Delay to allow the message to be read
         */
         OS_TaskDelay(CFE_ES_PANIC_DELAY);

         /* 
         ** cFE Cannot continue to start up.  
         */
         CFE_PSP_Panic(CFE_PSP_PANIC_VOLATILE_DISK);
         
      } /* end if BlocksFree */
   
   } /* end if processor reset */
     
} /* end function */

/*-------------------------------------------------------------------------
**
**                  Functional Prolog
**
**   Name: CFE_ES_CreateObjects
**
**   Purpose: This function reads the es_object_table and performs all of the
**            application layer initialization.
**----------------------------------------------------------------------------
*/
void  CFE_ES_CreateObjects(void)
{
    int32     ReturnCode;
    uint32    TaskIndex;
    boolean   AppSlotFound;
    uint16    i;
    uint16    j;

    CFE_ES_WriteToSysLog("ES Startup: Starting Object Creation calls.\n");

    for ( i = 0; i < CFE_ES_OBJECT_TABLE_SIZE; i++ )
    {
        switch ( CFE_ES_ObjectTable[i].ObjectType )
        {
            case CFE_ES_DRIVER_TASK:
            case CFE_ES_CORE_TASK:

            /*
            ** Allocate an ES AppTable entry
            */
            AppSlotFound = FALSE;
            for ( j = 0; j < CFE_ES_MAX_APPLICATIONS; j++ )
            {
               if ( CFE_ES_Global.AppTable[j].RecordUsed == FALSE )
               {
                  AppSlotFound = TRUE;
                  break;
               }
            }

            /*
            ** If a slot was found, create the application
            */
            if ( AppSlotFound == TRUE )
            {
            
               CFE_ES_LockSharedData(__func__,__LINE__);

               /*
               ** Allocate and populate the ES_AppTable entry
               */
               CFE_PSP_MemSet ( (void *)&(CFE_ES_Global.AppTable[j]), 0, sizeof(CFE_ES_AppRecord_t));
               CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
               CFE_ES_Global.AppTable[j].Type = CFE_ES_APP_TYPE_CORE;
               
               /*
               ** Fill out the parameters in the AppStartParams sub-structure
               */         
               strncpy((char *)CFE_ES_Global.AppTable[j].StartParams.Name, (char *)CFE_ES_ObjectTable[i].ObjectName, OS_MAX_API_NAME);
               CFE_ES_Global.AppTable[j].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
               /* EntryPoint field is not valid here for base apps */
               /* FileName is not valid for base apps, either */
               CFE_ES_Global.AppTable[j].StartParams.StackSize = CFE_ES_ObjectTable[i].ObjectSize;
               CFE_ES_Global.AppTable[j].StartParams.StartAddress = (cpuaddr)CFE_ES_ObjectTable[i].FuncPtrUnion.VoidPtr;
               CFE_ES_Global.AppTable[j].StartParams.ExceptionAction = CFE_ES_APP_EXCEPTION_PROC_RESTART;
               CFE_ES_Global.AppTable[j].StartParams.Priority = CFE_ES_ObjectTable[i].ObjectPriority;
               
               
               /*
               ** Fill out the Task Info
               */
               strncpy((char *)CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName, (char *)CFE_ES_ObjectTable[i].ObjectName, OS_MAX_API_NAME);
               CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName[OS_MAX_API_NAME - 1] = '\0';
               CFE_ES_Global.AppTable[j].TaskInfo.NumOfChildTasks = 0;
               
               /*
               ** Core apps still have the notion of an init/running state
               */
               CFE_ES_Global.AppTable[j].StateRecord.AppState = CFE_ES_APP_STATE_INITIALIZING;
               ++CFE_ES_Global.AppStartedCount;
               
               
               /*
               ** Create the task
               */
               ReturnCode = OS_TaskCreate(&CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId, /* task id */
                                  CFE_ES_ObjectTable[i].ObjectName,              /* task name */
                                  CFE_ES_ObjectTable[i].FuncPtrUnion.MainAppPtr, /* task function pointer */
                                  NULL,                                          /* stack pointer */
                                  CFE_ES_ObjectTable[i].ObjectSize,              /* stack size */
                                  CFE_ES_ObjectTable[i].ObjectPriority,          /* task priority */
                                  OS_FP_ENABLED);                                /* task options */

               if(ReturnCode != OS_SUCCESS)
               {
                  CFE_ES_Global.AppTable[j].RecordUsed = FALSE;
                  CFE_ES_WriteToSysLog("ES Startup: OS_TaskCreate error creating core App: %s: EC = 0x%08X\n",
                                        CFE_ES_ObjectTable[i].ObjectName, (unsigned int)ReturnCode);
      
                                        
                  CFE_ES_UnlockSharedData(__func__,__LINE__);

                  /*
                  ** Delay to allow the message to be read
                  */
                  OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                  /* 
                  ** cFE Cannot continue to start up.  
                  */
                  CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
                                              
               }
               else
               {
                  OS_ConvertToArrayIndex(CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId, &TaskIndex);

                  /*
                  ** Allocate and populate the CFE_ES_Global.TaskTable entry
                  */
                  if ( CFE_ES_Global.TaskTable[TaskIndex].RecordUsed == TRUE )
                  {
                     CFE_ES_WriteToSysLog("ES Startup: CFE_ES_Global.TaskTable record used error for App: %s, continuing.\n",
                                           CFE_ES_ObjectTable[i].ObjectName);
                  }
                  else
                  {
                     CFE_ES_Global.TaskTable[TaskIndex].RecordUsed = TRUE;
                  }
                  CFE_ES_Global.TaskTable[TaskIndex].AppId = j;
                  CFE_ES_Global.TaskTable[TaskIndex].TaskId = CFE_ES_Global.AppTable[j].TaskInfo.MainTaskId;
                  strncpy((char *)CFE_ES_Global.TaskTable[TaskIndex].TaskName, (char *)CFE_ES_Global.AppTable[j].TaskInfo.MainTaskName, OS_MAX_API_NAME);
                  CFE_ES_Global.TaskTable[TaskIndex].TaskName[OS_MAX_API_NAME - 1] = '\0';

                  CFE_ES_WriteToSysLog("ES Startup: Core App: %s created. App ID: %d\n",
                                       CFE_ES_ObjectTable[i].ObjectName,j);
                                       
                  /*
                  ** Increment the registered App and Registered External Task variables.
                  */
                  CFE_ES_Global.RegisteredTasks++;
                  CFE_ES_Global.RegisteredCoreApps++;
                  
                  CFE_ES_UnlockSharedData(__func__,__LINE__);
                                                                                                      
               }
            }
            else /* appSlot not found -- This should never happen!*/
            {
               CFE_ES_WriteToSysLog("ES Startup: Error, No free application slots available for CORE App!\n");
               /*
               ** Delay to allow the message to be read
               */
               OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
               /* 
               ** cFE Cannot continue to start up.  
               */
               CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
            
            }

            /*
             * CFE_ES_ApplicationSyncDelay() will delay this thread until the
             * newly-started thread calls CFE_ES_WaitForStartupSync()
             */
            if (CFE_ES_ApplicationSyncDelay(CFE_ES_SYSTEM_STATE_UNDEFINED, CFE_CORE_MAX_STARTUP_MSEC) != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("ES Startup: Core App %s did not complete initialization\n",
                                      CFE_ES_ObjectTable[i].ObjectName);

                /*
                ** Delay to allow the message to be read
                */
                OS_TaskDelay(CFE_ES_PANIC_DELAY);

                /*
                ** cFE Cannot continue to start up.
                */
                CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
            }
            break;

            case CFE_ES_FUNCTION_CALL: /*----------------------------------------------------------*/

               if ( CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr != NULL )
               {
                  CFE_ES_WriteToSysLog("ES Startup: Calling %s\n",CFE_ES_ObjectTable[i].ObjectName);
                  /*
                  ** Call the function
                  */
                  ReturnCode = (*CFE_ES_ObjectTable[i].FuncPtrUnion.FunctionPtr)();
                  if(ReturnCode != CFE_SUCCESS)
                  {
                     CFE_ES_WriteToSysLog("ES Startup: Error returned when calling function: %s: EC = 0x%08X\n",
                                           CFE_ES_ObjectTable[i].ObjectName, (unsigned int)ReturnCode);
                                        
                     /*
                     ** Delay to allow the message to be read
                     */
                     OS_TaskDelay(CFE_ES_PANIC_DELAY);
      
                     /* 
                     ** cFE Cannot continue to start up.  
                     */
                     CFE_PSP_Panic(CFE_PSP_PANIC_CORE_APP);
                                              
                  } /* end if */
                  
               }
               else
               {
                  CFE_ES_WriteToSysLog("ES Startup: bad function pointer ( table entry = %d).\n",i);
               }
               break;

            case CFE_ES_NULL_ENTRY: /*-------------------------------------------------------*/
               break;
            default:
               break;
       } /* end switch */

    } /* end for */

    CFE_ES_WriteToSysLog("ES Startup: Finished ES CreateObject table entries.\n");
}

/*
** Function: CFE_ES_ApplicationSyncDelay
**
** Purpose:  Waits for all of the applications that CFE has started thus far to
**           reach the "running" state, i.e. call CFE_ES_RunLoop or CFE_ES_WaitForStartupSync
**
**           It will also wait for the overall system state to be (at least) reach
**           the desired state.  This may be "CFE_ES_SYSTEM_STATE_UNDEFINED" to match any.
**
*/
int32 CFE_ES_ApplicationSyncDelay(uint32 MinimumSystemState, uint32 TimeOutMilliseconds)
{
    int32 Status;
    uint32 WaitTime;
    uint32 WaitRemaining;

    Status = CFE_SUCCESS;
    WaitRemaining = TimeOutMilliseconds;
    while (CFE_ES_Global.SystemState < MinimumSystemState ||
            (CFE_ES_Global.AppReadyCount != CFE_ES_Global.AppStartedCount &&
                    CFE_ES_SYSTEM_STATE_UNDEFINED == MinimumSystemState))
    {
        /* TBD: Very Crude timing here, but not sure if it matters,
         * as this is only done during startup, not real work */
        if (WaitRemaining > CFE_ES_STARTUP_SYNC_POLL_MSEC)
        {
            WaitTime = CFE_ES_STARTUP_SYNC_POLL_MSEC;
        }
        else if (WaitRemaining > 0)
        {
            WaitTime = WaitRemaining;
        }
        else
        {
            Status = CFE_ES_OPERATION_TIMED_OUT;
            break;
        }

        OS_TaskDelay(WaitTime);
        WaitRemaining -= WaitTime;
    }

    return Status;
}

