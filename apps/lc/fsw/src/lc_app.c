/************************************************************************
** File:
**   $Id: lc_app.c 1.1.1.2 2015/03/04 16:15:46EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   The CFS Limit Checker (LC) is a table driven application
**   that provides telemetry monitoring and autonomous response 
**   capabilities to Core Flight Executive (cFE) based systems. 
**
**   $Log: lc_app.c  $
**   Revision 1.1.1.2 2015/03/04 16:15:46EST sstrege 
**   Added copyright information
**   Revision 1.1.1.1 2012/10/01 18:35:45EDT lwalling 
**   Apply changes to branch
**   Revision 1.2 2012/10/01 13:23:59PDT lwalling 
**   Added local variable to avoid comparing 2 macros in function LC_CreateTaskCDS()
**   Revision 1.1 2012/07/31 13:53:36PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.16 2011/10/04 17:00:30EDT lwalling 
**   Must load AP def table before init AP results table
**   Revision 1.15 2011/06/08 16:07:40EDT lwalling 
**   Change call from LC_SubscribeWP() to LC_CreateHashTable()
**   Revision 1.14 2011/03/10 14:11:10EST lwalling 
**   Cleanup use of debug events during task startup
**   Revision 1.13 2011/03/02 10:53:23EST lwalling 
**   Explicitly state return value when known to be CFE_SUCCESS
**   Revision 1.12 2011/03/01 15:38:50EST lwalling 
**   Cleanup local function prototypes, move LC_SubscribeWP() and LC_UpdateTaskCDS() to lc_cmds.c
**   Revision 1.11 2011/03/01 09:35:30EST lwalling 
**   Modified startup logic re use of CDS and critical tables
**   Revision 1.10 2011/02/14 16:57:13EST lwalling 
**   Created LC_StartedNoCDS() to clear results tables after CDS load error
**   Revision 1.9 2011/01/19 11:32:06EST jmdagost 
**   Moved mission revision number from lc_version.h to lc_platform_cfg.h.
**   Revision 1.8 2010/03/08 10:37:09EST lwalling 
**   Move saved, not saved state definitions to common header file
**   Revision 1.7 2009/06/12 14:17:23EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.6 2009/02/23 11:15:10EST dahardis 
**   Added code to update the application data in the CDS on 
**   application startup after the "saved on exit" flag is reset
**   (see DCR 7084)
**   Revision 1.5 2009/01/15 15:36:11EST dahardis 
**   Unit test fixes
**   Revision 1.4 2008/12/10 15:34:07EST dahardis 
**   Altered CDS restoration processing according to
**   DCR 4680
**   Revision 1.3 2008/12/10 09:38:33EST dahardis 
**   Fixed calls to CFE_TBL_GetAddress (DCR #4699)
**   Revision 1.2 2008/12/03 13:59:44EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:18:51EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"
#include "lc_events.h"
#include "lc_msgids.h"
#include "lc_perfids.h"
#include "lc_version.h"
#include "lc_cmds.h"
#include "lc_action.h"
#include "lc_watch.h"
#include "cfe_platform_cfg.h" /* for CFE_SB_HIGHEST_VALID_MSGID */
#include "lc_platform_cfg.h"
#include "lc_mission_cfg.h"     /* Leave these two last to make sure all   */
#include "lc_verify.h"          /* LC configuration parameters are checked */

/************************************************************************
** LC Global Data
*************************************************************************/
LC_OperData_t    LC_OperData;
LC_AppData_t     LC_AppData;           

/************************************************************************
** Local Function Prototypes
*************************************************************************/
/************************************************************************/
/** \brief Initialize the CFS Limit Checker (LC) application
**  
**  \par Description
**       Limit Checker application initialization routine. This 
**       function performs all the required startup steps to
**       initialize (or restore from CDS) LC data structures and get 
**       the application registered with the cFE services so it can 
**       begin to receive command messages. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_EvsInit      \endcode
**  \retstmt Return codes from #LC_SbInit       \endcode
**  \retstmt Return codes from #LC_InitFromCDS  \endcode
**  \retstmt Return codes from #LC_InitNoCDS    \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_AppInit(void);

/************************************************************************/
/** \brief Initialize Event Services
**  
**  \par Description
**       This function performs the steps required to setup
**       cFE Events Services for use by the LC application
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_EvsInit(void);

/************************************************************************/
/** \brief Initialize Software Bus
**  
**  \par Description
**       This function performs the steps required to setup the
**       cFE software bus for use by the LC application
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe  \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe  \endcode
**  \endreturns
**
*************************************************************************/
int32 LC_SbInit(void);

/************************************************************************/
/** \brief Initialize Table Services (includes CDS)
**  
**  \par Description
**       This function creates the tables used by the LC application and
**       establishes the initial table values based on the configuration
**       setting that enables the use of Critical Data Store (CDS) and
**       the availability of stored data to restore.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #LC_CreateResultTables  \endcode
**  \retstmt Return codes from #LC_CreateDefinitionTables  \endcode
**  \retstmt Return codes from #LC_LoadDefaultTables  \endcode
**  \retstmt Return codes from #CFE_TBL_GetAddress  \endcode
**  \endreturns
**
**  \sa #LC_SAVE_TO_CDS
**
*************************************************************************/
int32 LC_TableInit(void);

/************************************************************************/
/** \brief Create Watchpoint and Actionpoint Result Tables
**  
**  \par Description
**       This function creates the dump only result tables used by the LC
**       application.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_TBL_Register  \endcode
**  \retstmt Return codes from #CFE_TBL_GetAddress  \endcode
**  \endreturns
**
**  \sa #LC_TableInit
**
*************************************************************************/
int32 LC_CreateResultTables(void);

/************************************************************************/
/** \brief Create Watchpoint and Actionpoint Definition Tables
**  
**  \par Description
**       This function creates the loadable definition tables used by the
**       LC application.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_TBL_Register  \endcode
**  \endreturns
**
**  \sa #LC_TableInit
**
*************************************************************************/
int32 LC_CreateDefinitionTables(void);

/************************************************************************/
/** \brief Create Result Table and Application Data CDS Areas
**  
**  \par Description
**       This function creates the loadable definition tables used by the
**       LC application.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_RegisterCDS  \endcode
**  \endreturns
**
**  \sa #LC_TableInit
**
*************************************************************************/
int32 LC_CreateTaskCDS(void);

/************************************************************************/
/** \brief Load Default Table Values
**  
**  \par Description
**       This function loads the definition tables from table files named
**       in the LC platform configuration header file.  The function also
**       initializes the contents of the dump only results tables and
**       initializes the global application data structure.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_TBL_Load  \endcode
**  \endreturns
**
**  \sa #LC_TableInit
**
*************************************************************************/
int32 LC_LoadDefaultTables(void);



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void LC_AppMain(void)
{
    int32   Status      = CFE_SUCCESS;
    uint32  RunStatus   = CFE_ES_APP_RUN;
    boolean initSuccess = TRUE;
   
    /* 
    ** Performance Log, Start
    */
    CFE_ES_PerfLogEntry(LC_APPMAIN_PERF_ID);
   
    /*
    **  Register this application with Executive Services 
    */
    Status = CFE_ES_RegisterApp();

    /*
    ** Perform application specific initialization
    */
    if (Status == CFE_SUCCESS)
    {
        Status = LC_AppInit();
    }

    /*
    ** Check for start-up error...
    */
    if (Status != CFE_SUCCESS)
    {
       /*
       ** Set run status to terminate main loop
       */
       RunStatus = CFE_ES_APP_ERROR;
       
       /*
       ** Set flag that init failed so we don't
       ** attempt application cleanup before exit
       */
       initSuccess = FALSE;
    }
   
    /*
    ** Application main loop
    */
    while(CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
       /* 
       ** Performance Log, Stop
       */
       CFE_ES_PerfLogExit(LC_APPMAIN_PERF_ID);
       
       /* 
       ** Pend on the arrival of the next Software Bus message 
       */
       Status = CFE_SB_RcvMsg(&LC_OperData.MsgPtr, LC_OperData.CmdPipe, CFE_SB_PEND_FOREVER);
       
       /* 
       ** Performance Log, Start
       */
       CFE_ES_PerfLogEntry(LC_APPMAIN_PERF_ID);
      
       /*
       ** Process the software bus message
       */ 
       if (Status == CFE_SUCCESS)
       {
           Status = LC_AppPipe(LC_OperData.MsgPtr);
       }
 
       /*
       ** Note: If there were some reason to exit the task
       **       normally (without error) then we would set
       **       RunStatus = CFE_ES_APP_EXIT
       */
       if (Status != CFE_SUCCESS)
       {
           /*
           ** Set request to terminate main loop
           */
           RunStatus = CFE_ES_APP_ERROR;
       }
      
    } /* end CFS_ES_RunLoop while */
   
    /*
    ** Check for "fatal" process error...
    */
    if (Status != CFE_SUCCESS)
    {
        /*
        ** Send an event describing the reason for the termination
        */
        CFE_EVS_SendEvent(LC_TASK_EXIT_EID, CFE_EVS_CRITICAL, 
                          "Task terminating, err = 0x%08X", Status);

        /*
        ** In case cFE Event Services is not working
        */
        CFE_ES_WriteToSysLog("LC task terminating, err = 0x%08X\n", Status);
    }
   
    /* 
    ** Performance Log, Stop
    */
    CFE_ES_PerfLogExit(LC_APPMAIN_PERF_ID);

    /*
    ** Do not update CDS if inactive or startup was incomplete
    */
    if ((LC_OperData.HaveActiveCDS) &&
        (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED))
    {
        LC_UpdateTaskCDS();
    }
    
    /* 
    ** Exit the application 
    */
    CFE_ES_ExitApp(RunStatus); 

} /* end LC_AppMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC initialization                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_AppInit(void)
{
    int32 Status = CFE_SUCCESS;

    /*
    ** Zero out the global data structures...
    */
    CFE_PSP_MemSet(&LC_OperData, 0, sizeof(LC_OperData_t));
    CFE_PSP_MemSet(&LC_AppData,  0, sizeof(LC_AppData_t));
    
    /*
    ** Initialize event services
    */
    Status = LC_EvsInit();
    if (Status != CFE_SUCCESS)
    {
       return(Status);
    }

    /*
    ** Initialize software bus
    */
    Status = LC_SbInit();
    if (Status != CFE_SUCCESS)
    {
       return(Status);
    }

    /*
    ** Initialize table services
    */
    Status = LC_TableInit();
    if (Status != CFE_SUCCESS)
    {
       return(Status);
    }

   /* 
   ** If we get here, all is good
   ** Issue the application startup event message 
   */
   CFE_EVS_SendEvent(LC_INIT_INF_EID, CFE_EVS_INFORMATION, 
                    "LC Initialized. Version %d.%d.%d.%d",
                     LC_MAJOR_VERSION,
                     LC_MINOR_VERSION,
                     LC_REVISION,
                     LC_MISSION_REV);

   return(CFE_SUCCESS);

} /* end LC_AppInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize event services interface                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_EvsInit(void)
{
    int32   Status = CFE_SUCCESS;
  
    /*
    ** If an application event filter table is added
    ** in the future, initialize it here
    */    

    /*
    **  Register for event services 
    */
    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    if (Status != CFE_SUCCESS)
    {
       CFE_ES_WriteToSysLog("LC App: Error Registering For Event Services, RC = 0x%08X\n", Status);
       return (Status);
    }
    
   return(CFE_SUCCESS);
    
} /* end LC_EvsInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize the software bus interface                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_SbInit(void)
{
    int32   Status = CFE_SUCCESS;

    /* 
    ** Initialize SB variables
    */
    LC_OperData.MsgPtr  = (CFE_SB_MsgPtr_t) NULL;
    LC_OperData.CmdPipe = 0;
    
    /*
    ** Initialize housekeeping packet...
    */
    CFE_SB_InitMsg(&LC_OperData.HkPacket, LC_HK_TLM_MID,
                   sizeof(LC_HkPacket_t), FALSE);

    /*
    ** Create Software Bus message pipe...
    */
    Status = CFE_SB_CreatePipe(&LC_OperData.CmdPipe, LC_PIPE_DEPTH, LC_PIPE_NAME);    
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_CR_PIPE_ERR_EID, CFE_EVS_ERROR,
                         "Error Creating LC Pipe, RC=0x%08X", Status);
        return(Status);
    }

    /*
    ** Subscribe to Housekeeping request messages...
    */
    Status = CFE_SB_Subscribe(LC_SEND_HK_MID, LC_OperData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_SUB_HK_REQ_ERR_EID, CFE_EVS_ERROR,
                          "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X", 
                          LC_SEND_HK_MID, Status);    
        return(Status);
    }

    /*
    ** Subscribe to LC ground command messages...
    */
    Status = CFE_SB_Subscribe(LC_CMD_MID, LC_OperData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_SUB_GND_CMD_ERR_EID, CFE_EVS_ERROR,
                          "Error Subscribing to GND CMD, MID=0x%04X, RC=0x%08X", 
                          LC_CMD_MID, Status);    
        return(Status); 
    }

    /*
    ** Subscribe to LC internal actionpoint sample messages...
    */
    Status = CFE_SB_Subscribe(LC_SAMPLE_AP_MID, LC_OperData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_SUB_SAMPLE_CMD_ERR_EID, CFE_EVS_ERROR,
                          "Error Subscribing to Sample CMD, MID=0x%04X, RC=0x%08X", 
                          LC_SAMPLE_AP_MID, Status);    
        return(Status); 
    }
    
    return(CFE_SUCCESS);
    
} /* end LC_SbInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Table initialization - includes Critical Data Store (CDS)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_TableInit(void)
{
    int32 Result;

    /*
    ** LC task use of Critical Data Store (CDS)
    **
    **    Global application data (LC_AppData)
    **    Watchpint results dump only table data
    **    Actionpoint results dump only table data
    **
    ** cFE Table Services use of CDS for LC task
    **
    **    Watchpint definition loadable table data
    **    Actionpoint definition loadable table data
    **
    ** LC table initialization logic re CDS
    **
    **    If LC cannot create all the CDS storage at startup, then LC
    **    will disable LC use of CDS and continue.
    **
    **    If LC cannot register definition tables as critical, then LC
    **    will disable LC use of CDS and re-register tables as non-critical.
    **
    **    If LC cannot register definition and results tables at startup,
    **    then LC will terminate - table use is a required function.
    **
    **    If LC can create all the CDS storage and register definition
    **    tables as critical, then LC will write to CDS regardless of
    **    whether LC was able to read from CDS at startup.
    **
    **    If LC cannot restore everything from CDS at startup, then LC
    **    will initialize everything - load default definition tables,
    **    init results table contents, init global application data.
    */

    /* lc_platform_cfg.h */
    #ifdef LC_SAVE_TO_CDS
    LC_OperData.HaveActiveCDS = TRUE;
    #endif

    /*
    ** Maintain a detailed record of table initialization results
    */
    if (LC_OperData.HaveActiveCDS)
    {
        LC_OperData.TableResults |= LC_CDS_ENABLED;
    }

    /*
    ** Create watchpoint and actionpoint result tables
    */ 
    if ((Result = LC_CreateResultTables()) != CFE_SUCCESS)
    {
        return(Result);
    }

    /*
    ** If CDS is enabled - create the 3 CDS areas managed by the LC task
    **  (continue with init, but disable CDS if unable to create all 3)
    */
    if (LC_OperData.HaveActiveCDS)
    {
        if (LC_CreateTaskCDS() != CFE_SUCCESS)
        {
            LC_OperData.HaveActiveCDS = FALSE;
        }
    }

    /*
    ** Create wp/ap definition tables - critical if CDS enabled
    */ 
    if ((Result = LC_CreateDefinitionTables()) != CFE_SUCCESS)
    {
        return(Result);
    }

    /*
    ** CDS still active only if we created 3 CDS areas and 2 critical tables
    */
    if (LC_OperData.HaveActiveCDS)
    {
        LC_OperData.TableResults |= LC_CDS_CREATED;
    }

    /*
    ** If any CDS area or critical table is not restored - initialize everything.
    **  (might be due to reset type, CDS disabled or corrupt, table restore error)
    */
    if (((LC_OperData.TableResults & LC_WRT_CDS_RESTORED) == LC_WRT_CDS_RESTORED) &&
        ((LC_OperData.TableResults & LC_ART_CDS_RESTORED) == LC_ART_CDS_RESTORED) &&
        ((LC_OperData.TableResults & LC_APP_CDS_RESTORED) == LC_APP_CDS_RESTORED) &&
        ((LC_OperData.TableResults & LC_WDT_TBL_RESTORED) == LC_WDT_TBL_RESTORED) &&
        ((LC_OperData.TableResults & LC_ADT_TBL_RESTORED) == LC_ADT_TBL_RESTORED))
    {
        LC_OperData.TableResults |= LC_CDS_RESTORED;

        /*
        ** Get a pointer to the watchpoint definition table data...
        */
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

        if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
        {
            CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                              "Error getting WDT address, RC=0x%08X", Result);
            return(Result);
        }

        /*
        ** Get a pointer to the actionpoint definition table data
        */
        Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

        if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
        {
            CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                              "Error getting ADT address, RC=0x%08X", Result);
            return(Result);
        }
    }
    else
    {
        if ((Result = LC_LoadDefaultTables()) != CFE_SUCCESS)
        {
            return(Result);
        }
    }

    /*
    ** Create watchpoint hash tables -- also subscribes to watchpoint packets
    */
    LC_CreateHashTable();

    /*
    ** Display results of CDS initialization (if enabled at startup)
    */
    if ((LC_OperData.TableResults & LC_CDS_ENABLED) == LC_CDS_ENABLED)
    {
        if ((LC_OperData.TableResults & LC_CDS_RESTORED) == LC_CDS_RESTORED)
        {
            CFE_EVS_SendEvent(LC_CDS_RESTORED_INF_EID, CFE_EVS_INFORMATION, 
                              "Previous state restored from Critical Data Store");
        }
        else if ((LC_OperData.TableResults & LC_CDS_UPDATED) == LC_CDS_UPDATED)
        {
            CFE_EVS_SendEvent(LC_CDS_UPDATED_INF_EID, CFE_EVS_INFORMATION, 
                              "Default state loaded and written to CDS, activity mask = 0x%08X",
                              LC_OperData.TableResults);
        }
        else
        {
            CFE_EVS_SendEvent(LC_CDS_DISABLED_INF_EID, CFE_EVS_INFORMATION, 
                              "LC use of Critical Data Store disabled, activity mask = 0x%08X",
                              LC_OperData.TableResults);
        }
    }

    return(CFE_SUCCESS);

} /* LC_TableInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create WP and AP results tables        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_CreateResultTables(void)
{
    int32 Result;
    uint32 DataSize;
    uint32 OptionFlags;

    /*
    ** Set "dump only" table option flags
    */
    OptionFlags = CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY;

    /*
    ** Register the Watchpoint Results Table (WRT) - "dump only" tables
    ** cannot be critical with CDS use managed by CFE Table Services.
    */
    DataSize = LC_MAX_WATCHPOINTS * sizeof(LC_WRTEntry_t);
    Result = CFE_TBL_Register(&LC_OperData.WRTHandle, LC_WRT_TABLENAME,
                               DataSize, OptionFlags, NULL);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WRT_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering WRT, RC=0x%08X", Result);
        return(Result);
    }

    Result = CFE_TBL_GetAddress((void *)&LC_OperData.WRTPtr, LC_OperData.WRTHandle);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_WRT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting WRT address, RC=0x%08X", Result);
        return(Result);
    }

    LC_OperData.TableResults |= LC_WRT_TBL_CREATED;

    /*
    ** Register the Actionpoint Results Table (ART) - "dump only" tables
    ** cannot be critical with CDS use managed by CFE Table Services.
    */
    DataSize = LC_MAX_ACTIONPOINTS * sizeof (LC_ARTEntry_t);
    Result = CFE_TBL_Register(&LC_OperData.ARTHandle, LC_ART_TABLENAME,
                               DataSize, OptionFlags, NULL);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ART_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering ART, RC=0x%08X", Result);
        return(Result);
    }

    Result = CFE_TBL_GetAddress((void *)&LC_OperData.ARTPtr, LC_OperData.ARTHandle);
    
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(LC_ART_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting ART address, RC=0x%08X", Result);
        return(Result);
    }

    LC_OperData.TableResults |= LC_ART_TBL_CREATED;

    return(CFE_SUCCESS);

} /* LC_CreateResultTables() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create WP and AP definition tables     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_CreateDefinitionTables(void)
{
    int32 Result;
    uint32 DataSize;
    uint32 OptionFlags;
    boolean HadActiveCDS;

    /* 
    ** Initial state of table restoration
    */ 
    HadActiveCDS = LC_OperData.HaveActiveCDS;

    /*
    ** If CDS is still enabled, try to register the 2 definition tables as critical
    **  (if error, continue with init - but disable CDS and re-register as non-critical)
    */
    if (LC_OperData.HaveActiveCDS)
    {
        OptionFlags = CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL;
    }
    else
    {
        OptionFlags = CFE_TBL_OPT_DEFAULT;
    }

    /* 
    ** Register the Watchpoint Definition Table (WDT)
    */ 
    DataSize = LC_MAX_WATCHPOINTS * sizeof (LC_WDTEntry_t);
    Result = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME,
                               DataSize, OptionFlags, LC_ValidateWDT);
 
    if ((LC_OperData.HaveActiveCDS) &&
       ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS)))
    { 
        LC_OperData.HaveActiveCDS = FALSE;
        OptionFlags = CFE_TBL_OPT_DEFAULT;
 
        /* 
        ** Re-register the Watchpoint Definition Table (WDT) non-critical
        */ 
        Result = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME,
                                   DataSize, OptionFlags, LC_ValidateWDT);
    } 

    if (Result == CFE_TBL_INFO_RECOVERED_TBL)
    {
        LC_OperData.TableResults |= LC_WDT_CRITICAL_TBL;
        LC_OperData.TableResults |= LC_WDT_TBL_RESTORED;
    }
    else if (Result == CFE_SUCCESS)
    {
        if (LC_OperData.HaveActiveCDS)
        {
            LC_OperData.TableResults |= LC_WDT_CRITICAL_TBL;
        }
        else
        {
            LC_OperData.TableResults |= LC_WDT_NOT_CRITICAL;
        }
    }
    else
    {
        /*
        ** Task initialization fails without this table
        */ 
        CFE_EVS_SendEvent(LC_WDT_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                         "Error registering WDT, RC=0x%08X", Result);
        return(Result);
    }

    /* 
    ** Register the Actionpoint Definition Table (ADT)
    */ 
    DataSize = LC_MAX_ACTIONPOINTS * sizeof (LC_ADTEntry_t);
    Result = CFE_TBL_Register(&LC_OperData.ADTHandle, LC_ADT_TABLENAME,
                               DataSize, OptionFlags, LC_ValidateADT);

    if ((LC_OperData.HaveActiveCDS) &&
       ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS)))
    { 
        LC_OperData.HaveActiveCDS = FALSE;
        OptionFlags = CFE_TBL_OPT_DEFAULT;
 
        /* 
        ** Re-register the Actionpoint Definition Table (ADT) non-critical
        */ 
        Result = CFE_TBL_Register(&LC_OperData.ADTHandle, LC_ADT_TABLENAME,
                                   DataSize, OptionFlags, LC_ValidateADT);
    } 

    if (Result == CFE_TBL_INFO_RECOVERED_TBL)
    {
        LC_OperData.TableResults |= LC_ADT_CRITICAL_TBL;
        LC_OperData.TableResults |= LC_ADT_TBL_RESTORED;
    }
    else if (Result == CFE_SUCCESS)
    {
        if (LC_OperData.HaveActiveCDS)
        {
            LC_OperData.TableResults |= LC_ADT_CRITICAL_TBL;
        }
        else
        {
            LC_OperData.TableResults |= LC_ADT_NOT_CRITICAL;
        }
    }
    else
    {
        /*
        ** Task initialization fails without this table
        */ 
        CFE_EVS_SendEvent(LC_ADT_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering ADT, RC=0x%08X", Result);
        return(Result);
    }

    /* 
    ** In case we created a critical WDT and then created a non-critical ADT
    */ 
    if (((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) &&
        ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL))
    {
        /* 
        ** Un-register the critical watchpoint Definition Table (WDT)
        */ 
        CFE_TBL_Unregister(LC_OperData.WDTHandle);

        /* 
        ** Re-register the Watchpoint Definition Table (WDT) non-critical
        */ 
        DataSize = LC_MAX_WATCHPOINTS * sizeof (LC_WDTEntry_t);
        OptionFlags = CFE_TBL_OPT_DEFAULT;
        Result = CFE_TBL_Register(&LC_OperData.WDTHandle, LC_WDT_TABLENAME,
                                   DataSize, OptionFlags, LC_ValidateWDT);
        if (Result == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_WDT_NOT_CRITICAL;
        }
        else
        {
            /*
            ** Task initialization fails without this table
            */ 
            CFE_EVS_SendEvent(LC_WDT_REREGISTER_ERR_EID, CFE_EVS_ERROR, 
                             "Error re-registering WDT, RC=0x%08X", Result);
            return(Result);
        }
    }

    return(CFE_SUCCESS);

} /* LC_CreateDefinitionTables() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - create Critical Data Store (CDS)       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_CreateTaskCDS(void)
{
    int32 Result;
    uint32 DataSize;
    uint8 RestoredLCState;

    /* 
    ** Create CDS and try to restore Watchpoint Results Table (WRT) data
    */ 
    DataSize = LC_MAX_WATCHPOINTS * sizeof(LC_WRTEntry_t);
    Result = CFE_ES_RegisterCDS(&LC_OperData.WRTDataCDSHandle, DataSize, LC_WRT_CDSNAME);

    if (Result == CFE_SUCCESS)
    {
        /* 
        ** Normal result after a power on reset (cold boot) - continue with next CDS area
        */ 
        LC_OperData.TableResults |= LC_WRT_CDS_CREATED;
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* 
        ** Normal result after a processor reset (warm boot) - try to restore previous data
        */ 
        LC_OperData.TableResults |= LC_WRT_CDS_CREATED;

        Result = CFE_ES_RestoreFromCDS(LC_OperData.WRTPtr, LC_OperData.WRTDataCDSHandle);

        if (Result == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_WRT_CDS_RESTORED;
        }
    }
    else
    {
        CFE_EVS_SendEvent(LC_WRT_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering WRT CDS Area, RC=0x%08X", Result);
        return(Result);
    }

    /* 
    ** Create CDS and try to restore Actionpoint Results Table (ART) data
    */ 
    DataSize = LC_MAX_ACTIONPOINTS * sizeof(LC_ARTEntry_t);
    Result = CFE_ES_RegisterCDS(&LC_OperData.ARTDataCDSHandle, DataSize, LC_ART_CDSNAME);

    if (Result == CFE_SUCCESS)
    {
        /* 
        ** Normal result after a power on reset (cold boot) - continue with next CDS area
        */ 
        LC_OperData.TableResults |= LC_ART_CDS_CREATED;
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* 
        ** Normal result after a processor reset (warm boot) - try to restore previous data
        */ 
        LC_OperData.TableResults |= LC_ART_CDS_CREATED;

        Result = CFE_ES_RestoreFromCDS(LC_OperData.ARTPtr, LC_OperData.ARTDataCDSHandle);

        if (Result == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_ART_CDS_RESTORED;
        }
    }
    else
    {
        CFE_EVS_SendEvent(LC_ART_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering ART CDS Area, RC=0x%08X", Result);
        return(Result);
    }

    /* 
    ** Create CDS and try to restore Application (APP) data
    */ 
    DataSize = sizeof(LC_AppData_t);
    Result = CFE_ES_RegisterCDS(&LC_OperData.AppDataCDSHandle, DataSize, LC_APPDATA_CDSNAME);

    if (Result == CFE_SUCCESS)
    {
        /* 
        ** Normal result after a power on reset (cold boot) - continue with next CDS area
        */ 
        LC_OperData.TableResults |= LC_APP_CDS_CREATED;
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* 
        ** Normal result after a processor reset (warm boot) - try to restore previous data
        */ 
        LC_OperData.TableResults |= LC_APP_CDS_CREATED;

        Result = CFE_ES_RestoreFromCDS(&LC_AppData, LC_OperData.AppDataCDSHandle);

        if ((Result == CFE_SUCCESS) && (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED))
        {
            /* 
            ** Success - only if previous session saved CDS data at least once
            */ 
            LC_OperData.TableResults |= LC_APP_CDS_RESTORED;

            /*
            ** May need to override the restored application state
            */
            RestoredLCState = LC_STATE_WHEN_CDS_RESTORED;
            if (RestoredLCState != LC_STATE_FROM_CDS)
            {
                LC_AppData.CurrentLCState = RestoredLCState;
            }
        }
    }
    else
    {
        CFE_EVS_SendEvent(LC_APP_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, 
                          "Error registering application data CDS Area, RC=0x%08X", Result);
        return(Result);
    }

    return(CFE_SUCCESS);

} /* LC_CreateTaskCDS() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Startup initialization - load default WP/AP definition tables   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 LC_LoadDefaultTables(void)
{
    int32 Result;

    /*
    ** Load default watchpoint definition table (WDT) 
    */
    Result = CFE_TBL_Load(LC_OperData.WDTHandle, CFE_TBL_SRC_FILE, LC_WDT_FILENAME);

    if (Result == CFE_SUCCESS)
    {
        LC_OperData.TableResults |= LC_WDT_DEFAULT_TBL;
    }
    else
    {
        /*
        ** Task initialization fails without this table
        */ 
        CFE_EVS_SendEvent(LC_WDT_LOAD_ERR_EID, CFE_EVS_ERROR, 
                          "Error (RC=0x%08X) Loading WDT with '%s'", Result, LC_WDT_FILENAME);
        return(Result);
    }

    /*
    ** Get a pointer to the watchpoint definition table data...
    */
    Result = CFE_TBL_GetAddress((void *)&LC_OperData.WDTPtr, LC_OperData.WDTHandle);

    if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
    {
        CFE_EVS_SendEvent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting WDT address, RC=0x%08X", Result);
        return(Result);
    }

    /*
    ** Load default actionpoint definition table (ADT)
    */
    Result = CFE_TBL_Load(LC_OperData.ADTHandle, CFE_TBL_SRC_FILE, LC_ADT_FILENAME);
    
    if (Result == CFE_SUCCESS)
    {
        LC_OperData.TableResults |= LC_ADT_DEFAULT_TBL;
    }
    else
    {
        /*
        ** Task initialization fails without this table
        */ 
        CFE_EVS_SendEvent(LC_ADT_LOAD_ERR_EID, CFE_EVS_ERROR, 
                          "Error (RC=0x%08X) Loading ADT with '%s'", Result, LC_ADT_FILENAME);
        return(Result);
    }

    /*
    ** Get a pointer to the actionpoint definition table data
    */
    Result = CFE_TBL_GetAddress((void *)&LC_OperData.ADTPtr, LC_OperData.ADTHandle);

    if ((Result != CFE_SUCCESS) && (Result != CFE_TBL_INFO_UPDATED))
    {
        CFE_EVS_SendEvent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, 
                          "Error getting ADT address, RC=0x%08X", Result);
        return(Result);
    }

    /*
    ** Initialize the watchpoint and actionpoint result table data
    */
    LC_ResetResultsWP(0, LC_MAX_WATCHPOINTS - 1, FALSE);
    LC_OperData.TableResults |= LC_WRT_DEFAULT_DATA;

    LC_ResetResultsAP(0, LC_MAX_ACTIONPOINTS - 1, FALSE);
    LC_OperData.TableResults |= LC_ART_DEFAULT_DATA;
 
    /*
    ** Reset application data counters reported in housekeeping
    */
    LC_ResetCounters();

    /*
    ** Set LC operational state to configured startup value
    */
    LC_AppData.CurrentLCState = LC_STATE_POWER_ON_RESET;
    LC_OperData.TableResults |= LC_APP_DEFAULT_DATA;

    /*
    ** If CDS is enabled - try to update the 3 CDS areas managed by the LC task
    **  (continue, but disable CDS if unable to update all 3)
    */
    if (LC_OperData.HaveActiveCDS)
    {
        if (LC_UpdateTaskCDS() == CFE_SUCCESS)
        {
            LC_OperData.TableResults |= LC_CDS_UPDATED;
        }
        else
        {
            LC_OperData.HaveActiveCDS = FALSE;
        }
    }

    return(CFE_SUCCESS);
    
} /* LC_LoadDefaultTables() */


/************************/
/*  End of File Comment */
/************************/
