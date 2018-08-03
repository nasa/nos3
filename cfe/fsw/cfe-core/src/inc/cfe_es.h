/*
**
**  File: 
**  cfe_es.h
**  $Id: cfe_es.h 1.14 2014/08/19 13:32:01GMT-05:00 sstrege Exp  $
**
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**  Purpose:
**	Unit specification for Executive Services library functions and macros.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**	Notes:
**
**  $Date: 2014/08/19 13:32:01GMT-05:00 $
**  $Revision: 1.14 $
**  $Log: cfe_es.h  $
**  Revision 1.14 2014/08/19 13:32:01GMT-05:00 sstrege 
**  Fixed doxygen warnings
**  Revision 1.13 2011/07/15 14:28:34EDT lwalling 
**  Removed const qualifier from CounterName argument to CFE_ES_RegisterGenCounter()
**  Revision 1.12 2010/11/05 15:54:27EDT aschoeni 
**  Added Generic Counter API to ES
**  Revision 1.11 2010/11/01 16:03:10EDT jmdagost 
**  Removed cfe.h from include list and added cfe_mission_cfg.h
**  Revision 1.10 2010/10/20 12:35:43EDT jmdagost 
**  Major revision to make semaphore use in memory pool creation optional.
**  Revision 1.9 2009/07/29 15:54:46EDT aschoeni 
**  Added GetPoolBufInfo function to return size or invalidity of a buffer in a pool
**  Revision 1.8 2008/12/08 16:10:54EST dkobe 
**  Correcting errors generated during detailed design document generation
**  Revision 1.7 2008/12/08 12:07:09EST dkobe 
**  Updates to correct doxygen errors
**  Revision 1.6 2008/07/31 15:41:31EDT apcudmore 
**  Added execution counter API:
**    -- Added execution counter to ES internal task data
**    -- Added code to increment counter in RunLoop
**    -- Added code to report counter in GetInfo APIs
**    -- Added API to increment counter for child tasks
**    -- Added code to cFE Core apps to increment counters.
**  Revision 1.5 2008/07/30 14:53:57EDT apcudmore 
**  Added CFE_ES_WaitForStartupSync API
**  Revision 1.4 2008/07/08 12:35:28EDT apcudmore 
**  Added an "AddressesAreValid" flag to the ES App Info structure.
**  Revision 1.3 2008/06/26 15:41:51EDT apcudmore 
**  Added public CFE_ES_GetAppInfo API.
**  Moved the old API to CFE_ES_GetAppInfoInternal
**  -- May merge these two in the near future.
**  Revision 1.2 2008/06/20 15:40:27EDT apcudmore 
**  Added support for OSAL based module loader
**   - Removed calls and references to the BSP based module loader
**  Revision 1.1 2008/04/17 08:05:19EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
**  Revision 1.58 2007/09/27 15:54:56EDT David Kobe (dlkobe) 
**  Added doxygen comments for commands and telemetry
**  Revision 1.57 2007/09/25 10:24:05EDT apcudmore 
**  Implement Application Restart on Exception:
**  - Added logic to Exception Handler
**  - Changed Exception Hook to pass host task ID
**  - Added App Reset ER Log entry type
**  Revision 1.56 2007/09/21 15:40:15EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.55 2007/09/20 10:52:57EDT apcudmore 
**  Added Query All Tasks command, file definition, events, error codes etc.
**  Revision 1.54 2007/09/12 16:06:09EDT David Kobe (dlkobe) 
**  Moved the definitions of CFE_ES_CRC_xx to the cfe_mission_cfg.h file and deleted TBL Services
**  CRC #define statement.
**  Revision 1.53 2007/08/31 14:11:05EDT apcudmore 
**  Updated reset subtypes ( trunk )
**  Revision 1.52 2007/08/07 12:52:40EDT dlkobe 
**  Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
**  Revision 1.51 2007/05/25 16:24:48EDT dlkobe 
**  Continued updating doxygen comments
**  Revision 1.50 2007/05/23 11:21:23EDT dlkobe 
**  Added doxygen formatting
**  Revision 1.49 2007/05/22 14:56:49EDT apcudmore 
**  cFE Non-Volatile Disk/Startup File changes:
**    - cFE startup now passes in startup file path/name
**    - cFE startup code does not init non-volatile disk ( BSP does )
**    - BSPs now can map cFE paths without making the OS_initfs/OS_mount calls
**    - MCP750 vxWorks6.2 BSP inits/mounts CF disk
**    - Desktop ports pass in the same startup file path/name as before
**  Revision 1.48 2007/05/14 15:43:45EDT apcudmore 
**  Cleaned up events and disabled debug code for application restart.
**  Revision 1.47 2007/05/09 12:42:13EDT dlkobe 
**  Added structure for Dump CDS Registry Record format
**  Revision 1.46 2007/05/04 20:07:51EDT dlkobe 
**  Added command to telemeter memory pool statistics
**  Revision 1.45 2007/04/12 10:37:18EDT apcudmore 
**  Added support for ES task to call CFE_ES_ExitApp on initialization and runtime failure.
**  Revision 1.44 2007/04/10 11:39:19EDT rjmcgraw 
**  Added function prototype CFE_ES_PerfLogAdd
**  Revision 1.43 2007/04/10 11:29:10EDT rjmcgraw 
**  Changed perf entry and exit routines to macros
**  Revision 1.42 2007/04/09 15:12:03EDT dlkobe 
**  Modified 'Name' parameter to be a const pointer
**  Revision 1.41 2007/04/05 16:16:03EDT apcudmore 
**  Added CFE_ES_DeleteChildTask and CFE_ES_ExitChildTask along with headers,
**  and associated Error codes.
**  Revision 1.39 2007/04/03 16:49:24EDT apcudmore 
**  Moved all command code defines, command , and telemetry definitions to cfe_es_msg.h
**  Revision 1.38 2007/04/02 10:57:45EDT rjmcgraw 
**  Moved BIT macros from perf.h to cfe_es.h
**  Revision 1.37 2007/03/29 15:49:08EST rjmcgraw 
**  Changed name of performance cmd codes
**  Revision 1.36 2007/03/29 15:22:34EST rjmcgraw 
**  Changed #define names for Logic Analyzer Cmd Codes
**
*/

#ifndef _cfe_es_
#define _cfe_es_

/*
** Includes 
*/
#include "cfe_mission_cfg.h"
#include "cfe_es_perfids.h"

/*****************************************************************************/

/*
** Macro Definitions
*/

/* 
** The OS_PRINTF macro may be defined by OSAL to enable
** printf-style argument checking.  If using a version of OSAL
** that does not define this then define it as a no-op.
*/
#ifndef OS_PRINTF
#define OS_PRINTF(m,n)
#endif

#define CFE_ES_DBIT(x) (1L << (x))                                    /* Places a one at bit positions 0 thru 31 */
#define CFE_ES_DTEST(i,x) (((i) & CFE_ES_DBIT(x)) != 0)               /* TRUE iff bit x of i is set */
#define CFE_ES_TEST_LONG_MASK(m,s)  (CFE_ES_DTEST(m[(s)/32],(s)%32))  /* Test a bit within an array of 32-bit integers. */
#define CFE_ES_MAX_MEMPOOL_BLOCK_SIZES     17    /**< Max number of size divisions allowed in a memory pool */

/*
** Note about reset type and subtypes:
**
** These values come from the PSP so the actual definition of these enumerations
** was moved to the PSP header file <cfe_psp.h>.
**
** In the future the Electronic Data sheets (EDS) for PSP/ES
** will define the exact values to use in telemetry messages.
*/

/*
**  Reset types
*/
/** \name Reset Type extensions */
/** \{ */
#define CFE_ES_APP_RESTART       CFE_PSP_RST_TYPE_MAX       /**< Application only was reset (extend the PSP enumeration here) */
/** \} */


/** \name Critical Data Store Macros */
/** \{ */
/** Maximum length allowed for CDS name. <BR>
** NOTE: "+2" is for NULL Character and "." (i.e. - "AppName.CDSName") */
#define CFE_ES_CDS_MAX_FULL_NAME_LEN (CFE_ES_CDS_MAX_NAME_LENGTH + OS_MAX_API_NAME + 2)

#define CFE_ES_CDS_BAD_HANDLE  (CFE_ES_CDSHandle_t) 0xFFFF
/** \} */

/*
** ES Run Status and Exit Status defines
*/
/** \name Run Status and Exit Status identifiers */
/** \{ */
#define CFE_ES_RUNSTATUS_APP_RUN                  1   /**< \brief Indicates that the Application should continue to run */
#define CFE_ES_RUNSTATUS_APP_EXIT                 2  /**< \brief Indicates that the Application wants to exit normally */
#define CFE_ES_RUNSTATUS_APP_ERROR                3  /**< \brief Indicates that the Application is quitting with an error */
#define CFE_ES_RUNSTATUS_SYS_EXCEPTION            4  /**< \brief The cFE App caused an exception */
#define CFE_ES_RUNSTATUS_SYS_RESTART              5  /**< \brief The system is requesting a restart of the cFE App */
#define CFE_ES_RUNSTATUS_SYS_RELOAD               6  /**< \brief The system is requesting a reload of the cFE App */
#define CFE_ES_RUNSTATUS_SYS_DELETE               7  /**< \brief The system is requesting that the cFE App is stopped */
#define CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR      8 /**< \brief Indicates that the Core Application could not Init */
#define CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR   9 /**< \brief Indicates that the Core Application had a runtime failure */
/** \} */

#define CFE_ES_APP_KILL_TIMEOUT         5 /**< \brief Number of cycles that ES will wait before killing an app */

#define CFE_ES_NO_MUTEX                 0 /**< \brief Indicates that the memory pool selection will not use a semaphore */
#define CFE_ES_USE_MUTEX                1 /**< \brief Indicates that the memory pool selection will use a semaphore */

/*
** System Log Defines
*/
/* "+1" is for the space between the timestamp and the message in the system log */
#define CFE_ES_MAX_SYSLOG_MSG_SIZE (CFE_EVS_MAX_MESSAGE_LENGTH + CFE_TIME_PRINTED_STRING_SIZE + 1)


/*
 * To preserve source-code compatibility with existing code,
 * this allows the old enum names to still work.  This should
 * be turned off after the new names are established.
 *  (sed -i -e 's/<old-name>/<new-name>/g' should take care of it)
 *
 * Note about why this is a good idea to do --
 * In the list below there are two values with similar names:
 *   CFE_ES_EXCEPTION, CFE_ES_SYS_EXCEPTION
 *
 * But these map to different values for two different purposes,
 * one is a app status and the other is a reset subtype.  Using the
 * new names makes it much clearer as to which is which, will
 * greatly reduce the chance of getting them mixed up, and make it
 * much more obvious to a code reviewer if the ARE mixed up somewhere.
 */
#ifndef CFE_ES_ENABLE_NEW_ENUM_NAMES

#define CFE_ES_PROCESSOR_RESET            CFE_PSP_RST_TYPE_PROCESSOR
#define CFE_ES_POWERON_RESET              CFE_PSP_RST_TYPE_POWERON

#define CFE_ES_POWER_CYCLE                CFE_PSP_RST_SUBTYPE_POWER_CYCLE
#define CFE_ES_PUSH_BUTTON                CFE_PSP_RST_SUBTYPE_PUSH_BUTTON
#define CFE_ES_HW_SPECIAL_COMMAND         CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND
#define CFE_ES_HW_WATCHDOG                CFE_PSP_RST_SUBTYPE_HW_WATCHDOG
#define CFE_ES_RESET_COMMAND              CFE_PSP_RST_SUBTYPE_RESET_COMMAND
#define CFE_ES_EXCEPTION                  CFE_PSP_RST_SUBTYPE_EXCEPTION
#define CFE_ES_UNDEFINED_RESET            CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET
#define CFE_ES_HWDEBUG_RESET              CFE_PSP_RST_SUBTYPE_HWDEBUG_RESET
#define CFE_ES_BANKSWITCH_RESET           CFE_PSP_RST_SUBTYPE_BANKSWITCH_RESET

#define CFE_ES_APP_RUN                    CFE_ES_RUNSTATUS_APP_RUN
#define CFE_ES_APP_EXIT                   CFE_ES_RUNSTATUS_APP_EXIT
#define CFE_ES_APP_ERROR                  CFE_ES_RUNSTATUS_APP_ERROR
#define CFE_ES_SYS_EXCEPTION              CFE_ES_RUNSTATUS_SYS_EXCEPTION
#define CFE_ES_SYS_RESTART                CFE_ES_RUNSTATUS_SYS_RESTART
#define CFE_ES_SYS_RELOAD                 CFE_ES_RUNSTATUS_SYS_RELOAD
#define CFE_ES_SYS_DELETE                 CFE_ES_RUNSTATUS_SYS_DELETE
#define CFE_ES_CORE_APP_INIT_ERROR        CFE_ES_RUNSTATUS_CORE_APP_INIT_ERROR
#define CFE_ES_CORE_APP_RUNTIME_ERROR     CFE_ES_RUNSTATUS_CORE_APP_RUNTIME_ERROR

#endif

/*****************************************************************************/
/*
** Type Definitions
*/

/*
** Memory Handle type
*/
typedef cpuaddr CFE_ES_MemHandle_t;  /**< \brief Data type used to hold Handles of Memory Pools created via CFE_ES_PoolCreate and CFE_ES_PoolCreateNoSem */

/*
** 
** CFE_ES_AppInfo_t is a structure that is used to provide
** information about an app. It is primarily used for the QueryOne and
** QueryAll Commands.
**
*/
typedef struct
{
   uint32   AppId;                          /**< \cfetlmmnemonic \ES_APP_ID
                                                 \brief Application ID for this Application */
   uint32   Type;                           /**< \cfetlmmnemonic \ES_APPTYPE
                                                 \brief The type of App: CORE or EXTERNAL */

   char     Name[OS_MAX_API_NAME];          /**< \cfetlmmnemonic \ES_APPNAME
                                                 \brief The Registered Name of the Application */
   char     EntryPoint[OS_MAX_API_NAME];    /**< \cfetlmmnemonic \ES_APPENTRYPT
                                                 \brief The Entry Point label for the Application */
   char     FileName[OS_MAX_PATH_LEN];      /**< \cfetlmmnemonic \ES_APPFILENAME
                                                 \brief The Filename of the file containing the Application */

   uint32   StackSize;                      /**< \cfetlmmnemonic \ES_STACKSIZE
                                                 \brief The Stack Size of the Application */
   uint32   ModuleId;                       /**< \cfetlmmnemonic \ES_MODULEID
                                                 \brief The ID of the Loadable Module for the Application */
   uint32   AddressesAreValid;              /**< \cfetlmmnemonic \ES_ADDRVALID
                                                 \brief Indicates that the Code, Data, and BSS addresses/sizes are valid */
   uint32   CodeAddress;                    /**< \cfetlmmnemonic \ES_CODEADDR
                                                 \brief The Address of the Application Code Segment*/
   uint32   CodeSize;                       /**< \cfetlmmnemonic \ES_CODESIZE
                                                 \brief The Code Size of the Application */
   uint32   DataAddress;                    /**< \cfetlmmnemonic \ES_DATAADDR
                                                 \brief The Address of the Application Data Segment*/
   uint32   DataSize;                       /**< \cfetlmmnemonic \ES_DATASIZE
                                                 \brief The Data Size of the Application */
   uint32   BSSAddress;                     /**< \cfetlmmnemonic \ES_BSSADDR
                                                 \brief The Address of the Application BSS Segment*/
   uint32   BSSSize;                        /**< \cfetlmmnemonic \ES_BSSSIZE
                                                  \brief The BSS Size of the Application */
   uint32   StartAddress;                   /**< \cfetlmmnemonic \ES_STARTADDR
                                                 \brief The Start Address of the Application */
   uint16   ExceptionAction;                /**< \cfetlmmnemonic \ES_EXCEPTNACTN
                                                 \brief What should occur if Application has an exception
                                                 (Restart Application OR Restart Processor) */
   uint16   Priority;                       /**< \cfetlmmnemonic \ES_PRIORITY
                                                 \brief The Priority of the Application */
   uint32   MainTaskId;                     /**< \cfetlmmnemonic \ES_MAINTASKID
                                                 \brief The Application's Main Task ID */
   uint32   ExecutionCounter;               /**< \cfetlmmnemonic \ES_MAINTASKEXECNT
                                                 \brief The Application's Main Task Execution Counter */
   char     MainTaskName[OS_MAX_API_NAME];  /**< \cfetlmmnemonic \ES_MAINTASKNAME
                                                 \brief The Application's Main Task ID */
   uint32   NumOfChildTasks;                /**< \cfetlmmnemonic \ES_CHILDTASKS
                                                 \brief Number of Child tasks for an App */

} CFE_ES_AppInfo_t;

/*
** Task Info Type
*/
typedef struct
{
   uint32   TaskId;                    /**< \brief Task Id */
   uint32   ExecutionCounter;          /**K \brief Task Execution Counter */
   uint8    TaskName[OS_MAX_API_NAME]; /**< \brief Task Name */
   uint32   AppId;                     /**< \brief Parent Application ID */
   uint8    AppName[OS_MAX_API_NAME];  /**< \brief Parent Application Name */

} CFE_ES_TaskInfo_t;

/*
** Memory Pool Statistics data type
*/
typedef struct
{
    uint32  BlockSize;               /**< \brief Number of bytes in each of these blocks */
    uint32  NumCreated;              /**< \brief Number of Memory Blocks of this size created */
    uint32  NumFree;                 /**< \brief Number of Memory Blocks of this size that are free */
} CFE_ES_BlockStats_t;

typedef struct
{
    uint32                PoolSize;                /**< \cfetlmmnemonic \ES_POOLSIZE
                                                        \brief  Size of Memory Pool (in bytes) */
    uint32                NumBlocksRequested;      /**< \cfetlmmnemonic \ES_BLKSREQ
                                                        \brief Number of times a memory block has been allocated */
    uint32                CheckErrCtr;             /**< \cfetlmmnemonic \ES_BLKERRCTR
                                                        \brief Number of errors detected when freeing a memory block */
    uint32                NumFreeBytes;            /**< \cfetlmmnemonic \ES_FREEBYTES
                                                        \brief Number of bytes never allocated to a block */
    CFE_ES_BlockStats_t   BlockStats[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES]; /**< \cfetlmmnemonic \ES_BLKSTATS
                                                                           \brief Contains stats on each block size */
} CFE_ES_MemPoolStats_t;

/*
** CDS Handle type
*/
typedef cpuaddr CFE_ES_CDSHandle_t;    /**< \brief Data type used to hold Handles of Critical Data Stores. See CFE_ES_RegisterCDS */

typedef struct
{
    CFE_ES_CDSHandle_t    Handle;          /**< \brief Handle of CDS */
    uint32                Size;            /**< \brief Size, in bytes, of the CDS memory block */
    boolean               Table;           /**< \brief Flag that indicates whether CDS contains a Critical Table */
    char                  Name[CFE_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief Processor Unique Name of CDS */
    uint8                 ByteAlignSpare1; /**< \brief Spare byte to insure structure size is multiple of 4 bytes */
} CFE_ES_CDSRegDumpRec_t;

/*
** Child Task Main Function Prototype
*/
typedef void (*CFE_ES_ChildTaskMainFuncPtr_t)(void); /**< \brief Required Prototype of Child Task Main Functions */
typedef int32 (*CFE_ES_LibraryEntryFuncPtr_t)(void); /**< \brief Required Prototype of Library Initialization Functions */

typedef enum
{
    CFE_ES_STATICENTRYTYPE_INVALID      = 0,
    CFE_ES_STATICENTRYTYPE_FIRST_VALID  = 2000,
    CFE_ES_STATICENTRYTYPE_APPLICATION,
    CFE_ES_STATICENTRYTYPE_LIBRARY,
    CFE_ES_STATICENTRYTYPE_MAX
} CFE_ES_StaticEntryType_t;

/*
** API Structure for statically linked CFS Applications
*/
typedef const struct
{
    CFE_ES_StaticEntryType_t EntryType;
    union
    {
        CFE_ES_ChildTaskMainFuncPtr_t AppEntryFunc;     /**< \brief Entry point for Application */
        CFE_ES_LibraryEntryFuncPtr_t LibInitFunc;       /**< \brief Initialization function for Library */
        cpuaddr EntryFuncAddr;
    } Ptrs;
    uint32  Priority;
    uint32  StackSize;
} CFE_ES_AppStaticModuleApi_t;

#ifdef CFS_STATIC_MODULE

#define CFS_MODULE_DECLARE_APP(name,pri,stack)              \
    void name##_Main(void);                                 \
    CFE_ES_AppStaticModuleApi_t CFS_##name##_API =          \
    {                                                       \
        .EntryType = CFE_ES_STATICENTRYTYPE_APPLICATION,    \
        .Ptrs.AppEntryFunc = name##_Main,                   \
        .Priority = pri,                                    \
        .StackSize = stack,                                 \
    }

#define CFS_MODULE_DECLARE_LIB(name)                        \
    int32 name##_Init(void);                                \
    CFE_ES_AppStaticModuleApi_t CFS_##name##_API =          \
    {                                                       \
        .EntryType = CFE_ES_STATICENTRYTYPE_LIBRARY,        \
        .Ptrs.LibInitFunc = name##_Init,                    \
    }
#else
#define CFS_MODULE_DECLARE_APP(name,pri,stack)
#define CFS_MODULE_DECLARE_LIB(name)
#endif


/*****************************************************************************/
/*
** Exported Functions
*/

/*****************************************************************************/

#if !defined(OSAL_OPAQUE_OBJECT_IDS)
/*
** \brief Compatibility wrapper for older versions of OSAL
**
** \par Description
**   In future versions of OSAL the task/object ID values might not be zero based
**   If that is the case then the OSAL must also define a function to convert back
**   to zero-based numbers such that the value can be used as an array index.
**
**   When using an existing/older version of OSAL, this inline function is defined
**   to mimic this call for backward compatibility.  It just passes through the same
**   value without modification.
**
** \param[in]  ObjectId     The object ID from OSAL
** \param[out] ArrayIndex   A zero-based value suitable for use as an array index
** \returns    OS_SUCCESS (a real version might return an error code).
*/
static inline int32 OS_ConvertToArrayIndex(uint32 ObjectId, uint32 *ArrayIndex)
{
   *ArrayIndex = ObjectId;
   return OS_SUCCESS;
}
#endif



/*****************************************************************************/
/**
** \brief cFE Main Entry Point used by Board Support Package to start cFE
**
** \par Description
**        cFE main entry point. This is the entry point into the cFE software.
**        It is called only by the Board Support Package software.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  StartType     Identifies whether this was a #CFE_PSP_RST_TYPE_POWERON or #CFE_PSP_RST_TYPE_PROCESSOR.
**
** \param[in]  StartSubtype  Specifies, in more detail, what caused the \c StartType identified above.
**                           See #CFE_ES_POWER_CYCLE for possible examples.
**
** \param[in]  ModeId        Identifies the source of the Boot as determined by the BSP.
**
** \param[in]  StartFilePath Identifies the startup file to use to initialize the cFE apps.
**
** \sa #CFE_ES_ResetCFE
**
******************************************************************************/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId , const char *StartFilePath );

/*****************************************************************************/
/**
** \brief Return the most recent Reset Type
**
** \par Description
**        Provides the caller with codes that identifies the type of Reset
**        the processor most recently underwent.  The caller can also obtain
**        information on what caused the reset by supplying a pointer to a
**        variable that will be filled with the Reset Sub-Type.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]   ResetSubtypePtr    Pointer to \c uint32 type variable in which the Reset Sub-Type will be stored.
**                                 The caller can set this pointer to NULL if the Sub-Type is of no interest.
**
** \param[out]  *ResetSubtypePtr   If the provided pointer was not \c NULL, the Reset Sub-Type is stored at the given address.
**                                 For a list of possible Sub-Type values, see \link #CFE_ES_POWER_CYCLE "Reset Sub-Types" \endlink.
**
** \returns
** \retcode #CFE_PSP_RST_TYPE_POWERON   \retdesc \copydoc CFE_PSP_RST_TYPE_POWERON    \endcode
** \retcode #CFE_PSP_RST_TYPE_PROCESSOR \retdesc \copydoc CFE_PSP_RST_TYPE_PROCESSOR  \endcode
** \endreturns
**
** \sa #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr);

/*****************************************************************************/
/**
** \brief Reset the cFE Core and all cFE Applications
**
** \par Description
**        This API causes an immediate reset of the cFE Kernel and all cFE Applications.
**        The caller can specify whether the reset should clear all memory (#CFE_PSP_RST_TYPE_POWERON)
**        or try to retain volatile memory areas (#CFE_PSP_RST_TYPE_PROCESSOR).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ResetType    Identifies the type of reset desired.  Allowable settings are:
**                          \arg #CFE_PSP_RST_TYPE_POWERON     - Causes all memory to be cleared 
**                          \arg #CFE_PSP_RST_TYPE_PROCESSOR   - Attempts to retain volatile disk, critical data store and user reserved memory. 
**
** \returns
** \retcode #CFE_ES_BAD_ARGUMENT    \retdesc \copydoc CFE_ES_BAD_ARGUMENT     \endcode
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_Main
**
******************************************************************************/
int32  CFE_ES_ResetCFE(uint32 ResetType);

/*****************************************************************************/
/**
** \brief Restart a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_ReloadApp, #CFE_ES_DeleteApp
**
******************************************************************************/
int32 CFE_ES_RestartApp(uint32 AppID);

/*****************************************************************************/
/**
** \brief Reload a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted from
**        the specified file.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \param[in]  AppFileName Identifies the new file to start.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RestartApp, #CFE_ES_DeleteApp
**
******************************************************************************/
int32 CFE_ES_ReloadApp(uint32 AppID, const char *AppFileName);

/*****************************************************************************/
/**
** \brief Delete a cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped deleted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RestartApp, #CFE_ES_ReloadApp
**
******************************************************************************/
int32 CFE_ES_DeleteApp(uint32 AppID);

/*****************************************************************************/
/**
** \brief Exit a cFE Application
**
** \par Description
**        This API is the "Exit Point" for the cFE application
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus     .
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RunLoop, #CFE_ES_RegisterApp
**
******************************************************************************/
void CFE_ES_ExitApp(uint32 ExitStatus);


/*****************************************************************************/
/**
** \brief Check for Exit, Restart, or Reload commands
**
** \par Description
**        This is the API that allows an app to check for exit requests from
**         the system.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus   A pointer to a variable containing the Application's
**                          desired run status.  Acceptable values are:
**                          \arg #CFE_ES_APP_RUN - \copydoc CFE_ES_APP_RUN
**                          \arg #CFE_ES_APP_EXIT - \copydoc CFE_ES_APP_EXIT
**                          \arg #CFE_ES_APP_ERROR - \copydoc CFE_ES_APP_ERROR
**
** \returns
** \retcode #TRUE  \retdesc The application should continue executing \endcode
** \retcode #FALSE \retdesc The application should terminate itself \endcode
** \endreturns
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RegisterApp
**
******************************************************************************/
int32 CFE_ES_RunLoop(uint32 *ExitStatus);

/*****************************************************************************/
/**
** \brief Allow an Application to Wait for others to start up
**
** \par Description
**        This is the API that allows an app to wait for the "rest" of the apps
**        to finish initializing before continuing. It is most useful for 
**        applications such as Health and Safety or the Scheduler that need
**        to wait until applications exist and are running before sending out
**        packets to them.
**
** \par Assumptions, External Events, and Notes:
**          This API should only be called as the last item of an Apps initialization.
**          In addition, this API should only be called by an App that is started
**          from the ES Startup file. It should not be used by an App that is 
**          started after the system is running. ( Although it will cause no harm )
**
** \param[in]  TimeOutMilliseconds   The timeout value in Milliseconds.
**                                   This parameter must be at least 1000. Lower values
**                                   will be rounded up. There is not an option to 
**                                   wait indefinitely to avoid hanging a critical
**                                   application because a non-critical app did not start. 
**
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds);


/*
** Application Management functions
*/

/*****************************************************************************/
/**
** \brief Registers a cFE Application with the Executive Services
**
** \par Description
**        This API registers the calling Application with the cFE.
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This function \b MUST be called before any other cFE API functions are called.
**
** \returns
** \retstmt Return codes from #OS_TaskRegister          \endcode
** \retstmt Return codes from #OS_BinSemTake            \endcode
** \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS  \endcode
** \endreturns
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RunLoop
**
******************************************************************************/
int32 CFE_ES_RegisterApp(void);

/*****************************************************************************/
/**
** \brief Get an Application ID for the calling Application
**
** \par Description
**        This routine retrieves the cFE Application ID for the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: \b All tasks associated with the Application would return the same Application ID.
**
** \param[in]   AppIdPtr       Pointer to variable that is to receive the Application's ID.
**
** \param[out]  *AppIdPtr      Application ID of the calling Application.
**
** \returns
** \retcode #CFE_SUCCESS       \retdesc \copydoc CFE_SUCCESS        \endcode
** \retcode #CFE_ES_ERR_APPID  \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \retcode #CFE_ES_ERR_BUFFER \retdesc \copydoc CFE_ES_ERR_BUFFER  \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppID(uint32 *AppIdPtr);

/*****************************************************************************/
/**
** \brief Get an Application ID associated with a specified Application name
**
** \par Description
**        This routine retrieves the cFE Application ID associated with a
**        specified Application name.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   AppIdPtr       Pointer to variable that is to receive the Application's ID.
**
** \param[in]   AppName        Pointer to null terminated character string containing an Application name.
**
** \param[out]  *AppIdPtr      Application ID of the calling Application.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_APPNAME  \retdesc \copydoc CFE_ES_ERR_APPNAME  \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, const char *AppName);

/*****************************************************************************/
/**
** \brief Get an Application name for a specified Application ID
**
** \par Description
**        This routine retrieves the cFE Application ID associated with a
**        specified Application name.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   AppName       Pointer to a character array of at least \c BufferLength in size that will
**                            be filled with the appropriate Application name.
**
** \param[in]   AppId         Application ID of Application whose name is being requested.
**
** \param[in]   BufferLength  The maximum number of characters, including the null terminator, that can be put
**                            into the \c AppName buffer.  This routine will truncate the name to this length,
**                            if necessary.
**
** \param[out]  *AppName      Null terminated Application name of the Application associated with the 
**                            specified Application ID.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS        \endcode
** \retcode #CFE_ES_ERR_APPID    \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER  \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength);

/*****************************************************************************/
/**
** \brief Get Application Information given a specified App ID
**
** \par Description
**        This routine retrieves the information about an App associated with a
**        specified App ID. The information includes all of the information ES 
**        maintains for an application ( documented in the CFE_ES_AppInfo_t type ) 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   AppInfo      Pointer to a \c CFE_ES_AppInfo_t structure that holds the specific 
**                            Application information.
**
** \param[in]   AppId        Application ID of Application whose name is being requested.
**
** \param[out]  *AppInfo     Filled out \c CFE_ES_AppInfo_t structure containing the 
**                            App Name, and application memory addresses among other fields.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_APPID    \retdesc \copydoc CFE_ES_ERR_APPID    \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
int32 CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, uint32 AppId);

/*****************************************************************************/
/**
** \brief Get Task Information given a specified Task ID
**
** \par Description
**        This routine retrieves the information about a Task associated with a
**        specified Task ID. The information includes Task Name, and Parent/Creator 
**        Application ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskInfo      Pointer to a \c CFE_ES_TaskInfo_t structure that holds the specific 
**                            task information.
**
** \param[in]   TaskId        Application ID of Application whose name is being requested.
**
** \param[out]  *TaskInfo     Filled out \c CFE_ES_TaskInfo_t structure containing the 
**                            Task Name, Parent App Name, Parent App ID among other fields.
**
** \returns
** \retcode #CFE_SUCCESS         \retdesc \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_ES_ERR_TASKID   \retdesc \copydoc CFE_ES_ERR_TASKID   \endcode
** \retcode #CFE_ES_ERR_BUFFER   \retdesc \copydoc CFE_ES_ERR_BUFFER   \endcode
** \endreturns
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId);

/*
** Task management functions
*/
/*****************************************************************************/
/**
** \brief Registers a cFE Child task associated with a cFE Application
**
** \par Description
**        This routine registers a cFE Child task and associates it with its parent
**        cFE Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API \b MUST be called by the Child Task before any other cFE API calls are made.
**
** \returns
** \retcode #CFE_SUCCESS                       \retdesc \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_ES_ERR_CHILD_TASK_REGISTER    \retdesc \copydoc CFE_ES_ERR_CHILD_TASK_REGISTER  \endcode
** \endreturns
**
** \sa #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32  CFE_ES_RegisterChildTask(void);

/*****************************************************************************/
/**
** \brief Creates a new task under an existing Application
**
** \par Description
**        This routine creates a new task (a separate execution thread) owned by the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskIdPtr     A pointer to a variable that will be filled in with the new task's ID.
**
** \param[in]   TaskName      A pointer to a string containing the desired name of the new task.  
**                            This can be up to #OS_MAX_API_NAME characters, including the trailing null.
**
** \param[in]   FunctionPtr   A pointer to the function that will be spawned as a new task.  This function 
**                            must have the following signature: uint32 function(void).  Input parameters 
**                            for the new task are not supported.
**
** \param[in]   StackPtr      A pointer to the location where the child task's stack pointer should start.   
**                            NOTE: Not all underlying operating systems support this parameter.
**
** \param[in]   StackSize     The number of bytes to allocate for the new task's stack.
**
** \param[in]   Priority      The priority for the new task.  Lower numbers are higher priority, with 0 being 
**                            the highest priority.  Applications cannot create tasks with a higher priority 
**                            (lower number) than their own priority.
**
** \param[in]   Flags         Reserved for future expansion.
**
** \param[out]  *TaskIdPtr    The Task ID of the newly created child task.
**
** \returns
** \retcode #CFE_SUCCESS                     \retdesc \copydoc CFE_SUCCESS                   \endcode
** \retcode #CFE_ES_ERR_CHILD_TASK_CREATE    \retdesc \copydoc CFE_ES_ERR_CHILD_TASK_CREATE  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32  CFE_ES_CreateChildTask(uint32                          *TaskIdPtr,
                              const char                      *TaskName,
                              CFE_ES_ChildTaskMainFuncPtr_t    FunctionPtr,
                              uint32                          *StackPtr,
                              uint32                           StackSize,
                              uint32                           Priority,
                              uint32                           Flags);

/*****************************************************************************/
/**
** \brief Deletes a task under an existing Application
**
** \par Description
**        This routine deletes a task under an Application specified by the \c TaskId obtained
**        when the child task was created using the #CFE_ES_CreateChildTask API.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskId     The task ID previously obtained when the Child Task was created with the #CFE_ES_CreateChildTask API.
**
** \returns
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc \copydoc CFE_ES_NOT_IMPLEMENTED  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
int32 CFE_ES_DeleteChildTask(uint32 TaskId);

/*****************************************************************************/
/**
** \brief Exits a child task
**
** \par Description
**        This routine allows the current executing child task to exit and 
**        be deleted by ES.
**
** \par Assumptions, External Events, and Notes:
**        This function cannot be called from an Application's Main Task.
**
** \returns
** \retstmt
**        This function does not return a value, but if it does return
**        at all, it is assumed that the Task was either unregistered or
**        this function was called from a cFE Application's main task.   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask
**
******************************************************************************/
void CFE_ES_ExitChildTask(void);

/*****************************************************************************/
/**
** \brief Increments the execution counter for the calling task
**
** \par Description
**        This routine increments the execution counter that is stored for
**        the calling task. It can be called from cFE Application main tasks, child
**        tasks, or cFE Core application main tasks. Normally, the call is not 
**        necessary from a cFE Application, since the CFE_ES_RunLoop call increments
**        the counter for the Application. 
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API is not needed for Appplications that call the CFE_ES_RunLoop call.
**
** \returns
** \retstmt
**        This function does not return a value.   \endcode
** \endreturns
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void  CFE_ES_IncrementTaskCounter(void);

/*
** Log management functions
*/

/*****************************************************************************/
/**
** \brief Write a string to the cFE System Log
**
** \par Description
**        This routine writes a formatted string to the cFE system log.  This 
**        can be used to record very low-level errors that can't be reported 
**        using the Event Services. This function is used in place of printf 
**        for flight software. It should be used for significant startup events, 
**        critical errors, and conditionally compiled debug software. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   SpecStringPtr     The format string for the log message.  
**                                This is similar to the format string for a printf() call.
**
** \returns
** \retcode #CFE_SUCCESS             \retdesc \copydoc CFE_SUCCESS             \endcode
** \retcode #CFE_ES_ERR_SYS_LOG_FULL \retdesc \copydoc CFE_ES_ERR_SYS_LOG_FULL \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...) OS_PRINTF(1,2);

/*****************************************************************************/
/**
** \brief Calculate a CRC on a block of memory
**
** \par Description
**        This routine calculates a cyclic redundancy check (CRC) on a block of memory.  The CRC algorithm
**        used is determined by the last parameter. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   DataPtr     Pointer to the base of the memory block.
**
** \param[in]   DataLength  The number of bytes in the memory block.
**
** \param[in]   InputCRC    A starting value for use in the CRC calculation.  This parameter
**                          allows the user to calculate the CRC of non-contiguous blocks as
**                          a single value.  Nominally, the user should set this value to zero.
**
** \param[in]   TypeCRC     One of the following CRC algorithm selections:
**
**                          \arg \c CFE_ES_CRC_8 - (Not currently implemented)
**                          \arg \c CFE_ES_CRC_16 - a CRC-16 algorithm
**                          \arg \c CFE_ES_CRC_32 - (not currently implemented)
**
** \returns
** \retstmt The result of the CRC calculation on the specified memory block. \endcode
** \endreturns
**
******************************************************************************/
uint32 CFE_ES_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);

/*
** Critical Data Store API
*/

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine allocates a block of memory in the Critical Data Store and associates it with
**        the calling Application.  The memory can survive an Application restart as well as a Processor Reset. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle.
**
** \param[in]   BlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        A pointer to a character string containing an application 
**                          unique name of #CFE_ES_CDS_MAX_NAME_LENGTH characters or less.
**
** \param[out]  *HandlePtr  The handle of the CDS block that can be used in 
**                          #CFE_ES_CopyToCDS and #CFE_ES_RestoreFromCDS.
**
** \returns
** \retcode #CFE_SUCCESS  \retdesc The memory block was successfully created in the CDS.           \endcode
** \retcode #CFE_ES_NOT_IMPLEMENTED \retdesc The processor does not support a Critical Data Store. \endcode
** \retcode #CFE_ES_CDS_ALREADY_EXISTS \retdesc \copydoc CFE_ES_CDS_ALREADY_EXISTS                 \endcode
** \retcode #CFE_ES_CDS_INVALID_SIZE   \retdesc \copydoc CFE_ES_CDS_INVALID_SIZE                   \endcode
** \retcode #CFE_ES_CDS_INVALID_NAME   \retdesc \copydoc CFE_ES_CDS_INVALID_NAME                   \endcode
** \retcode #CFE_ES_CDS_REGISTRY_FULL  \retdesc \copydoc CFE_ES_CDS_REGISTRY_FULL                  \endcode
** \endreturns
**
** \sa #CFE_ES_CopyToCDS, #CFE_ES_RestoreFromCDS
**
******************************************************************************/
int32 CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name);

/*****************************************************************************/
/**
** \brief Save a block of data in the Critical Data Store (CDS)
**
** \par Description
**        This routine copies a specified block of memory into the Critical Data Store that
**        had been previously registered via #CFE_ES_RegisterCDS.  The block of memory to be
**        copied must be at least as big as the size specified when registering the CDS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle       The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in]   DataToCopy   A Pointer to the block of memory to be copied into the CDS.
**
** \returns
** \retcode #OS_SUCCESS              \retdesc \copydoc OS_SUCCESS              \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE   \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \retcode #OS_ERROR                \retdesc Problem with handle or a size mismatch   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_RestoreFromCDS
**
*/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy);

/*****************************************************************************/
/**
** \brief Recover a block of data from the Critical Data Store (CDS)
**
** \par Description
**        This routine copies data from the Critical Data Store identified with the \c Handle into
**        the area of memory pointed to by the \c RestoreToMemory pointer.  The area of memory to
**        be copied into must be at least as big as the size specified when registering the CDS.
**        The recovery will indicate an error if the data integrity check maintained by the CDS 
**        indicates the contents of the CDS have changed.  However, the contents will still be
**        copied into the specified area of memory.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle             The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in]   RestoreToMemory    A Pointer to the block of memory that is to be restored with the contents of the CDS.
**
** \param[out]  *RestoreToMemory   The contents of the specified CDS.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_ES_CDS_BLOCK_CRC_ERR   \retdesc \copydoc CFE_ES_CDS_BLOCK_CRC_ERR        \endcode
** \retcode #OS_ERROR                   \retdesc Problem with handle or a size mismatch   \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_CopyToCDS
**
*/
int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle);

/*
** Memory Allocation/Deallocation API
*/

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application without using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, no mutex handling is performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr   A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[out]  *HandlePtr  The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application while using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, mutex handling will be performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr   A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[out]  *HandlePtr  The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application with application specified block sizes.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in]   HandlePtr      A pointer to the variable the caller wishes to have the memory pool handle kept in.
**
** \param[in]   MemPtr         A Pointer to the pool of memory created by the calling application. This address must
**                             be on a 32-bit boundary.
**
** \param[in]   Size           The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[in]   NumBlockSizes  The number of different block sizes specified in the \c BlockSizes array. If set equal to
**                             zero or if greater than 17, then default block sizes are used.
**
** \param[in]   BlockSizes     Pointer to an array of sizes to be used instead of the default block sizes specified by
**                             #CFE_ES_MEM_BLOCK_SIZE_01 through #CFE_ES_MAX_BLOCK_SIZE.  If the pointer is equal to NULL,
**                             the default block sizes are used.
**
** \param[in]   UseMutex       Flag indicating whether the new memory pool will be processing with mutex handling or not.
**                             Valid parameter values are #CFE_ES_USE_MUTEX and #CFE_ES_NO_MUTEX
**
** \param[out]  *HandlePtr     The memory pool handle.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS          \endcode
** \retcode #CFE_ES_BAD_ARGUMENT   \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t *HandlePtr, uint8 *MemPtr, uint32 Size, uint32 NumBlockSizes, uint32 *BlockSizes, uint16 UseMutex);

/*****************************************************************************/
/**
** \brief Gets a buffer from the memory pool created by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem
**
** \par Description
**        This routine obtains a block of memory from the memory pool supplied by the calling application.    
**
** \par Assumptions, External Events, and Notes:
**        -# The size allocated from the memory pool is, at a minimum, 12 bytes more than requested.
**
** \param[in]   BufPtr      A pointer to the Application's pointer in which will be stored the address of the allocated memory buffer.
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   Size        The size of the buffer requested.  NOTE: The size allocated may be larger.
**
** \param[out]  *BufPtr     The address of the requested buffer.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes 
**          actually allocated for the buffer. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE       \endcode
** \retcode #CFE_ES_ERR_MEM_BLOCK_SIZE  \retdesc \copydoc CFE_ES_ERR_MEM_BLOCK_SIZE   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_GetPoolBuf(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size);

/*****************************************************************************/
/**
** \brief Gets info on a buffer previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine gets info on a buffer in the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to provide status for.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes actually allocated. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \retcode #CFE_ES_BUFFER_NOT_IN_POOL  \retdesc \copydoc CFE_ES_BUFFER_NOT_IN_POOL   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_PutPoolBuf
**
******************************************************************************/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Releases a buffer from the memory pool that was previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine releases a buffer back into the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   HandlePtr   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to be released.
**
** \returns
** \retstmt When successful, the return value is a positive number and is the number of bytes actually released. \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE   \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Extracts the statistics maintained by the memory pool software
**
** \par Description
**        This routine fills the #CFE_ES_MemPoolStats_t data structure with the statistics
**        maintained by the memory pool software.  These statistics can then be telemetered
**        by the calling Application.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   BufPtr      Pointer to #CFE_ES_MemPoolStats_t data structure to be 
**                          filled with memory statistics.
**
** \param[in]   Handle      The handle to the memory pool whose statistics are desired.
**
** \param[out] *BufPtr      Memory Pool Statistics stored in given data structure.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_ERR_MEM_HANDLE      \retdesc \copydoc CFE_ES_ERR_MEM_HANDLE  \endcode
** \endreturns
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf
**
******************************************************************************/
int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t  Handle);

/*
** Performance Log API
*/

/*****************************************************************************/
/**
** \brief Entry marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the entry or start event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogExit, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogExit, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogEntry(id) (CFE_ES_PerfLogAdd(id, 0))

/*****************************************************************************/
/**
** \brief Exit marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the exit or end event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogEntry, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogExit(id) (CFE_ES_PerfLogAdd(id, 1))


/*****************************************************************************/
/**
** \brief Function called by CFE_ES_PerfLogEntry and CFE_ES_PerfLogExit macros
**
** \par Description
**        This function logs the entry and exit marker for the specified 
**        \c id. This function is used by the Software Performance Analysis 
**        tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Marker          Identifier of the specific event or marker.
** \param[in]   EntryExit       Used to specify Entry(0) or Exit(1)
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogExit
**
******************************************************************************/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit);

/*
** Generic Counter API
*/

/*****************************************************************************/
/**
** \brief Register a generic counter
**
** \par Description
**        This routine registers a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  *CounterName   The Name of the generic counter.
**
** \param[out] *CounterIdPtr  The Counter Id of the newly created counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_RegisterGenCounter(uint32 *CounterIdPtr, const char *CounterName);

/*****************************************************************************/
/**
** \brief Delete a generic counter
**
** \par Description
**        This routine deletes a previously registered generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  CounterId     The Counter Id of the newly created counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_RegisterGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_DeleteGenCounter(uint32 CounterId);

/*****************************************************************************/
/**
** \brief Increments the specified generic counter
**
** \par Description
**        This routine increments the specified generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be incremented.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_IncrementGenCounter(uint32 CounterId);

/*****************************************************************************/
/**
** \brief Set the specified generic counter
**
** \par Description
**        This routine sets the specified generic counter to the specified value. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be set.
**
** \param[in]   Count        The new value of the Counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_SetGenCount(uint32 CounterId, uint32 Count);

/*****************************************************************************/
/**
** \brief Get the specified generic counter count
**
** \par Description
**        This routine gets the value of a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to get the value from.
**
** \param[in]   *Count     The value of the Counter.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
int32 CFE_ES_GetGenCount(uint32 CounterId, uint32 *Count);


/*****************************************************************************/
/**
** \brief Get the Id associated with a generic counter name
**
** \par Description
**        This routine gets the Counter Id for a generic counter specified by name.
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   *CounterName     The name of the Counter.
**
** \param[out]  *CounterIdPtr    The Counter Id for the given name.
**
** \returns
** \retcode #CFE_SUCCESS                \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_ES_BAD_ARGUMENT      \retdesc \copydoc CFE_ES_BAD_ARGUMENT  \endcode
** \endreturns
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount
******************************************************************************/
int32 CFE_ES_GetGenCounterIDByName(uint32 *CounterIdPtr, const char *CounterName);

/*****************************************************************************/
/**
** \brief Process an exception detected by the underlying OS/PSP
**
** \par Description
**        This hook routine is called from the PSP when an exception occurs
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   HostTaskId       The OS (not OSAL) task ID
** \param[in]   ReasonString     Identifier from PSP
** \param[in]   ContextPointer   Context data from PSP
** \param[in]   ContextSize      Size of context data from PSP
**
******************************************************************************/
void CFE_ES_ProcessCoreException(uint32  HostTaskId,     const char *ReasonString,
                                 const uint32 *ContextPointer, uint32 ContextSize);



#endif  /* _cfe_es_ */
