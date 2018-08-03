/************************************************************************
** File:
**   $Id: cf_app.c 1.41.1.1 2015/03/06 15:30:49EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**  The CFS CF Application file containing the application
**  initialization routines, the main routine and the command interface.
**
** Notes:
**
** $Log: cf_app.c  $
** Revision 1.41.1.1 2015/03/06 15:30:49EST sstrege 
** Added copyright information
** Revision 1.41 2011/05/20 14:45:24EDT rmcgraw 
** DCR14529:2 Initialize HandshakeSemIds to CF_INVALID
** Revision 1.40 2011/05/20 13:38:02EDT rmcgraw 
** DCR15033:1 Add auto suspend to AppPipe
** Revision 1.39 2011/05/19 15:32:09EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.38 2011/05/17 16:14:04EDT rmcgraw 
** DCR14976:6 Rename CF_SET_POLL_DIR_PATH_CC to CF_SET_POLL_PARAM_CC
** Revision 1.37 2011/05/17 15:52:49EDT rmcgraw 
** DCR14967:5 Message ptr made consistent across all cmds.
** Revision 1.36 2011/05/17 10:21:48EDT rmcgraw 
** DCR14967:4 Function Register_callbacks cannot return err, check for err removed
** Revision 1.35 2011/05/17 09:22:38EDT rmcgraw 
** DCR14529:1 Added case statement for GiveTakeCmd
** Revision 1.34 2011/05/13 14:59:28EDT rmcgraw 
** DCR13439:1 Added platform config param CF_STARTUP_SYNC_TIMEOUT
** Revision 1.33 2011/05/10 17:04:32EDT rmcgraw 
** DCR14534:1 Changed incoming PDU processing
** Revision 1.32 2011/05/09 11:52:16EDT rmcgraw 
** DCR13317:1 Allow Destintaion path to be blank
** Revision 1.31 2011/05/04 16:55:32EDT rmcgraw 
** Check pkt length on wakeup cmd
** Revision 1.30 2011/05/03 16:47:19EDT rmcgraw 
** Cleaned up events, removed \n from some events, removed event id ganging
** Revision 1.29 2011/04/29 14:45:03EDT rmcgraw 
** Fixed pointer problem when RcvMsg returned error
** Revision 1.28 2011/04/28 10:50:21EDT rmcgraw 
** DCR14848:2 Include cfe.h for CFE_ES_USE_MUTEX
** Revision 1.27 2011/04/19 11:10:08EDT rmcgraw 
** DCR14848:1 Added CFE_ES_USE_MUTEX parameter to PoolCreateEx call.
** Revision 1.26 2010/11/04 11:37:45EDT rmcgraw 
** Dcr13051:1 Wrap OS_printfs in platform cfg CF_DEBUG
** Revision 1.25 2010/11/04 10:52:39EDT rmcgraw 
** DCR13223:1 Added tlm counters for engine cycles and wakeup
** Revision 1.24 2010/11/02 09:37:04EDT rmcgraw 
** DCR12802:1 Moved peer entity ID validation from chan to poll dir
** Revision 1.23 2010/10/25 11:21:54EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.22 2010/10/20 16:07:02EDT rmcgraw 
** DCR13054:1 Expanded max event filters at startup from four to eight
** Revision 1.21 2010/10/20 14:15:21EDT rmcgraw 
** DCR128222:1 Added tlm counter for total abandon transactions
** Revision 1.20 2010/08/06 18:45:57EDT rmcgraw 
** Dcr11510:1 Fixed cfg params with buffer sizes
** Revision 1.19 2010/08/04 15:17:42EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.18 2010/07/20 14:37:47EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.17 2010/07/08 13:47:34EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.16 2010/07/07 17:27:09EDT rmcgraw 
** DCR11510:1  Removed AddSlash and manual terminations, added better table validation
** Revision 1.15 2010/06/17 10:03:47EDT rmcgraw 
** DCR11510:1 Changed PB In Progress logic to include Data Blast Trans Num
** Revision 1.14 2010/06/11 16:13:14EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.13 2010/06/02 21:33:14EDT rmcgraw 
** DCR11510:1 Added queue info cmd length check
** Revision 1.12 2010/06/01 10:52:59EDT rmcgraw 
** DCR111510:1 Queue info cmd starts with first file to be deqeued, was reversed
** Revision 1.11 2010/05/24 14:05:44EDT rmcgraw 
** Dcr11510:1 Added CheckForTableRequests
** Revision 1.10 2010/04/27 09:06:18EDT rmcgraw 
** DCR11510:1 Removed CF_SendDiagDataCmd
** Revision 1.9 2010/04/26 10:10:00EDT rmcgraw 
** DCR11510:1 Fixed ptr used before assignment error
** Revision 1.8 2010/04/23 15:49:19EDT rmcgraw 
** DCR11510:1 Comment changes
** Revision 1.7 2010/04/23 13:27:13EDT rmcgraw 
** DCR11510:1 Fixed linker error regarding CF_PlaybackDirCmd
** Revision 1.6 2010/04/23 10:17:48EDT rmcgraw 
** DCR11510:1 Removed reference to MaxRestrictedDirs
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cf_app.h"
#include "cf_cmds.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_msgids.h"
#include "cf_verify.h"
#include "cf_perfids.h"
#include "cf_version.h"
#include "cf_platform_cfg.h"
#include "cf_callbacks.h"
#include "cfdp_syntax.h"
#include "cfdp_provides.h"
#include "cf_defs.h"
#include "cf_playback.h"
#include "cfdp_requires.h"
#include <string.h>
#include <ctype.h> /* toupper */


/************************************************************************
** CF global data
*************************************************************************/
#ifdef CF_DEBUG
uint32              cfdbg = 0;
#endif

CF_AppData_t        CF_AppData;

/* Some AutoSuspend variables are defined in HK pkt */
uint32              CF_AutoSuspendCnt;
uint32              CF_AutoSuspendArray[CF_AUTOSUSPEND_MAX_TRANS];
uint32              CF_MemPoolDefSize[CF_MAX_MEMPOOL_BLK_SIZES] = 
{
    CF_MAX_BLOCK_SIZE,
    CF_MEM_BLOCK_SIZE_07,
    CF_MEM_BLOCK_SIZE_06,
    CF_MEM_BLOCK_SIZE_05,
    CF_MEM_BLOCK_SIZE_04,
    CF_MEM_BLOCK_SIZE_03,
    CF_MEM_BLOCK_SIZE_02,
    CF_MEM_BLOCK_SIZE_01
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CF application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CF_AppMain(void)
{
    int32 Status;
    
    /*
    ** Register the Application with Executive Services
    */
    CFE_ES_RegisterApp();
    
    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(CF_APPMAIN_PERF_ID);
    
    
    /* Perform Application Initialization */
    Status = CF_AppInit();
    if (Status != CFE_SUCCESS)
    {
        CF_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /* Wait to be sure TO has created the semaphore */
    CFE_ES_WaitForStartupSync(CF_STARTUP_SYNC_TIMEOUT);
    
    CF_GetHandshakeSemIds();

    /*
    ** Application Main Loop.
    */
    while(CFE_ES_RunLoop(&CF_AppData.RunStatus) == TRUE)
    {
        /*
        ** Performance Log Exit Stamp.
        */
        CFE_ES_PerfLogExit(CF_APPMAIN_PERF_ID);
        
        /*
        ** Pend on the arrival of the next Software Bus message.
        */
        Status = CFE_SB_RcvMsg(&CF_AppData.MsgPtr,CF_AppData.CmdPipe,CFE_SB_PEND_FOREVER);
        
        /*
        ** Performance Log Entry Stamp.
        */
        CFE_ES_PerfLogEntry(CF_APPMAIN_PERF_ID);

        if(Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_RCV_MSG_ERR_EID, CFE_EVS_ERROR,
               "CF_APP Exiting due to CFE_SB_RcvMsg error 0x%08X", Status);
            
            CF_AppData.RunStatus = CFE_ES_APP_ERROR;
        
        }else{
        
            /* Perform Message Processing */
            CF_AppPipe(CF_AppData.MsgPtr);
            
        }/* end if */

    } /* end while */

   /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(CF_APPMAIN_PERF_ID);

    /*
    ** Exit the Application.
    */
    CFE_ES_ExitApp(CF_AppData.RunStatus);

} /* end of CF_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CF application initialization routine                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CF_AppInit(void)
{
    int32       Status = CFE_SUCCESS;
    uint32      i;
    uint32      CfgFileEventsToFilter = 0;  

    CF_AppData.RunStatus = CFE_ES_APP_RUN;

    /* Initialize Housekeeping packet  */
    CFE_SB_InitMsg(&CF_AppData.Hk,CF_HK_TLM_MID,sizeof(CF_HkPacket_t),TRUE);
    
    /* Initialize Transaction packet  */
    CFE_SB_InitMsg(&CF_AppData.Trans,CF_TRANS_TLM_MID,sizeof(CF_TransPacket_t),TRUE);
    
    /* Initialize Configuration Parameters packet  */
    CFE_SB_InitMsg(&CF_AppData.CfgPkt,CF_CONFIG_TLM_MID,sizeof(CF_ConfigPacket_t),TRUE); 

    /* Process the platform cfg file events to be filtered */
    if(CF_FILTERED_EVENT1 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT1;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK1;      
      CfgFileEventsToFilter++;
    }/* end if */           

    if(CF_FILTERED_EVENT2 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT2;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK2;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CF_FILTERED_EVENT3 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT3;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK3;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CF_FILTERED_EVENT4 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT4;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK4;      
      CfgFileEventsToFilter++;
    }/* end if */ 
    
    if(CF_FILTERED_EVENT5 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT5;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK5;      
      CfgFileEventsToFilter++;
    }/* end if */
    
    if(CF_FILTERED_EVENT6 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT6;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK6;      
      CfgFileEventsToFilter++;
    }/* end if */
    
    if(CF_FILTERED_EVENT7 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT7;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK7;      
      CfgFileEventsToFilter++;
    }/* end if */
    
    if(CF_FILTERED_EVENT8 != 0){
      CF_AppData.EventFilters[CfgFileEventsToFilter].EventID = CF_FILTERED_EVENT8;
      CF_AppData.EventFilters[CfgFileEventsToFilter].Mask    = CF_FILTER_MASK8;      
      CfgFileEventsToFilter++;
    }/* end if */    
    
    /* Register with event services...        */
    Status = CFE_EVS_Register (CF_AppData.EventFilters, CfgFileEventsToFilter, CFE_EVS_BINARY_FILTER);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog(
         "CF App: Error Registering Events,RC=0x%08X",Status);      
        return (Status);
    }

    /* Create CF Command Pipe */
    Status = CFE_SB_CreatePipe (&CF_AppData.CmdPipe,CF_PIPE_DEPTH,CF_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CR_PIPE_ERR_EID, CFE_EVS_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",Status);
        return (Status);
    }

    /* Subscribe to Housekeeping Request */
    Status = CFE_SB_Subscribe(CF_SEND_HK_MID,CF_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_SUB_REQ_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to HK Request,RC=0x%08X",Status);
        return (Status);
     }

    /* Subscribe to CF ground commands */
    Status = CFE_SB_Subscribe(CF_CMD_MID,CF_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_SUB_CMD_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to CF Gnd Cmds,RC=0x%08X",Status);
        return (Status);
     }

    /* Subscribe to CF Wakeup command */
    Status = CFE_SB_Subscribe(CF_WAKE_UP_REQ_CMD_MID,CF_AppData.CmdPipe);
    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_SUB_WAKE_ERR_EID, CFE_EVS_ERROR,
            "Error Subscribing to Wakeup Cmd,RC=0x%08X",Status);
        return (Status);
    }

    /* initialize uplink queues */
    for(i=0;i<CF_NUM_UPLINK_QUEUES;i++)
    {
        CF_AppData.UpQ[i].HeadPtr   = NULL;
        CF_AppData.UpQ[i].TailPtr   = NULL;
        CF_AppData.UpQ[i].EntryCnt  = 0;
    }


    /* Register the CF Configuration Table */
    Status = CF_TableInit();
    if(Status != CFE_SUCCESS)
    {
        /* Specific failure is detailed in function CF_TableInit */
      return (Status);
    }

    /* 
    ** Register CFDP engine callback routines 
    */
    CF_RegisterCallbacks();

    /* 
    ** Initialize playback output channels 
    */
    Status = CF_ChannelInit();
    if(Status != CFE_SUCCESS)
    {
      /* Specific failure is detailed in function CF_ChannelInit */  
      return (Status);
    }

    /* Subscribe to incoming pdus */
    for(i=0;i<CF_NUM_INPUT_CHANNELS;i++)
    {
        Status = CFE_SB_Subscribe(CF_AppData.Tbl->InCh[i].IncomingPDUMsgId,CF_AppData.CmdPipe);    
        if (Status != CFE_SUCCESS)
        {            
            CFE_EVS_SendEvent(CF_SUB_PDUS_ERR_EID, CFE_EVS_ERROR,
                "Error Subscribing to Incoming PDUs,RC=0x%08X",Status);
            return (Status);
        }
    }


    /* Initialize Housekeeping Counters */    
    CF_AppData.Hk.App.WakeupForFileProc   = 0;
    CF_AppData.Hk.App.EngineCycleCount    = 0;
    CF_AppData.Hk.App.QNodesAllocated     = 0;
    CF_AppData.Hk.App.QNodesDeallocated   = 0;
    CF_AppData.Hk.App.MemInUse            = 0;
    CF_AppData.Hk.App.PeakMemInUse        = 0;
    CF_AppData.Hk.App.MemAllocated        = CF_MEMORY_POOL_BYTES;
    CF_AppData.Hk.App.PDUsReceived        = 0;
    CF_AppData.Hk.App.PDUsRejected        = 0;
    CF_AppData.Hk.App.TotalAbandonTrans   = 0;
    CF_AppData.Hk.App.LastFailedTrans[0]  = '\0';

    CF_AppData.Hk.Up.MetaCount            = 0;    
    CF_AppData.Hk.Up.SuccessCounter       = 0;
    CF_AppData.Hk.Up.FailedCounter        = 0;
    CF_AppData.Hk.Up.LastFileUplinked[0]  = '\0'; 
    
    CF_AppData.Hk.AutoSuspend.EnFlag      = CF_DISABLED;


    /* 
    ** Initialize MIB or the CFDP State Machine
    */
    cfdp_set_mib_parameter (MIB_ACK_TIMEOUT, CF_AppData.Tbl->AckTimeout);
    cfdp_set_mib_parameter (MIB_ACK_LIMIT, CF_AppData.Tbl->AckLimit);
    cfdp_set_mib_parameter (MIB_NAK_TIMEOUT, CF_AppData.Tbl->NakTimeout);
    cfdp_set_mib_parameter (MIB_NAK_LIMIT, CF_AppData.Tbl->NakLimit);
    cfdp_set_mib_parameter (MIB_INACTIVITY_TIMEOUT, CF_AppData.Tbl->InactivityTimeout);
    cfdp_set_mib_parameter (MIB_OUTGOING_FILE_CHUNK_SIZE, CF_AppData.Tbl->OutgoingFileChunkSize);
    cfdp_set_mib_parameter (MIB_SAVE_INCOMPLETE_FILES, CF_AppData.Tbl->SaveIncompleteFiles);    
    cfdp_set_mib_parameter (MIB_MY_ID,CF_AppData.Tbl->FlightEntityId);
    
    strncpy(&CF_AppData.Hk.Eng.FlightEngineEntityId[0],
            CF_AppData.Tbl->FlightEntityId,
            CF_MAX_CFG_VALUE_CHARS);

    /* Application initialization event */
    Status = CFE_EVS_SendEvent (CF_INIT_EID, CFE_EVS_INFORMATION,
               "CF Initialized.  Version %d.%d.%d.%d",
                CF_MAJOR_VERSION,
                CF_MINOR_VERSION, 
                CF_REVISION, 
                CF_MISSION_REV);

    if (Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog(
         "CF App:Error Sending Initialization Event,RC=0x%08X\n", Status);
    }
    

#ifdef CF_DEBUG 
    if(sizeof(CF_QueueEntry_t) != CF_MEM_BLOCK_SIZE_05)
              CFE_ES_WriteToSysLog("CF block size not set properly QE %d, B5 %d\n",
                  sizeof(CF_QueueEntry_t), CF_MEM_BLOCK_SIZE_05); 
#endif                    

    return (Status);

} /* end of CF_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CF application table initialization routine                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CF_TableInit (void)
{
    int32       Status = CFE_SUCCESS;

    /* Register The CF Config Table */
    Status = CFE_TBL_Register (&CF_AppData.ConfigTableHandle,
                                CF_CONFIG_TABLE_NAME,
                                sizeof (cf_config_table_t) ,
                                CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                                CF_ValidateCFConfigTable);

    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(CF_CFGTBL_REG_ERR_EID, CFE_EVS_ERROR,
            "Error Registering Config Table,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_Load (CF_AppData.ConfigTableHandle,
                           CFE_TBL_SRC_FILE,
                           CF_CONFIG_TABLE_FILENAME);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(CF_CFGTBL_LD_ERR_EID, CFE_EVS_ERROR,
            "Error Loading Config Table,RC=0x%08X",Status);
        return (Status);
     }


    Status = CFE_TBL_Manage (CF_AppData.ConfigTableHandle);
    if (Status != CFE_SUCCESS)
    {
      CFE_EVS_SendEvent(CF_CFGTBL_MNG_ERR_EID, CFE_EVS_ERROR,
            "Error from TBL Manage call for Config Table,RC=0x%08X",Status);
        return (Status);
     }

    Status = CFE_TBL_GetAddress ( (void *) (&CF_AppData.Tbl),
                                      CF_AppData.ConfigTableHandle);
    /* Status should be CFE_TBL_INFO_UPDATED because we loaded it above */
    if (Status != CFE_TBL_INFO_UPDATED)
    {
      CFE_EVS_SendEvent(CF_CFGTBL_GADR_ERR_EID, CFE_EVS_ERROR,
            "Error Getting Adr for Config Tbl,RC=0x%08X",Status);
        return (Status);
    }

    return CFE_SUCCESS;

}   /* CF_TableInit */



int32 CF_ChannelInit (void)
{
    int32  Stat = 0;
    uint32  i,qs;
    uint32  ChanMemNeeded = 0;


    /* create mem pool */
    Stat = CFE_ES_PoolCreateEx(&CF_AppData.Mem.PoolHdl, 
                                CF_AppData.Mem.Partition, 
                                CF_MEMORY_POOL_BYTES, 
                                CF_MAX_MEMPOOL_BLK_SIZES, 
                                &CF_MemPoolDefSize[0],
                                CFE_ES_USE_MUTEX);
    
    if(Stat != CFE_SUCCESS){
        CFE_ES_WriteToSysLog("PoolCreate failed for CF, gave adr 0x%x,size %d,stat=0x%x\n",
                             CF_AppData.Mem.Partition,CF_MEMORY_POOL_BYTES,Stat);
        return Stat;
    }
    
    CF_AppData.Hk.App.BufferPoolHandle = CF_AppData.Mem.PoolHdl;

    /* Calculate max memory needed by assuming all queues are full */
    /* Note - Memory Pool Overhead not included */
    CF_AppData.Hk.App.MaxMemNeeded  = (CF_AppData.Tbl->UplinkHistoryQDepth * sizeof(CF_QueueEntry_t)) +
                    (CF_MAX_SIMULTANEOUS_TRANSACTIONS * sizeof(CF_QueueEntry_t));
    
    for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
    {           

        CF_AppData.Chan[i].DataBlast        = CF_NOT_IN_PROGRESS;
        CF_AppData.Chan[i].PendQTimer       = 0;
        CF_AppData.Chan[i].PollDirTimer     = 0;
        CF_AppData.Chan[i].TransNumBlasting = 0;
        CF_AppData.Hk.Chan[i].PollDirsChecked = 0;
        CF_AppData.Hk.Chan[i].PendingQChecked = 0;
        CF_AppData.Hk.Chan[i].RedLightCntr  = 0;
        CF_AppData.Hk.Chan[i].RedLightCntr  = 0;
        CF_AppData.Hk.Chan[i].PDUsSent      = 0;
        CF_AppData.Hk.Chan[i].FilesSent     = 0;
        CF_AppData.Hk.Chan[i].SuccessCounter = 0;
        CF_AppData.Hk.Chan[i].FailedCounter = 0;
    
        CF_AppData.Chan[i].HandshakeSemId = CF_INVALID;

        /* initialize pending queue, active queue and history queue variables */
        for(qs=0;qs<CF_QUEUES_PER_CHAN;qs++)
        {
            CF_AppData.Chan[i].PbQ[qs].HeadPtr   = NULL;
            CF_AppData.Chan[i].PbQ[qs].TailPtr   = NULL;
            CF_AppData.Chan[i].PbQ[qs].EntryCnt  = 0;
        }
                   

        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE) 
        {

            ChanMemNeeded = (CF_AppData.Tbl->OuCh[i].PendingQDepth * sizeof(CF_QueueEntry_t)) +
                            (CF_AppData.Tbl->OuCh[i].HistoryQDepth * sizeof(CF_QueueEntry_t));
            
            CF_AppData.Hk.App.MaxMemNeeded += ChanMemNeeded;
          
        }/* end if */
        
    }/* end for */
   
    return Stat;

}/* end CF_ChannelInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CF validate the config table contents                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CF_ValidateCFConfigTable (void * TblPtr)
{

    int32   ValueAsInt,RtnStat;
    uint32  i,j,TblValidationErrs = 0;

    cf_config_table_t *Tbl = (cf_config_table_t *)TblPtr;
    
    /* Validate Flight Entity Id */
    if(CF_ValidateEntityId(&Tbl->FlightEntityId[0]) == CF_ERROR)
    {
        TblValidationErrs++;

        if(TblValidationErrs == 1)
            CFE_EVS_SendEvent(CF_TBL_VAL_ERR1_EID, CFE_EVS_ERROR,
                "Cannot set FlightEntityId to %s, must be 2 byte, dotted decimal fmt like 0.24",
                &Tbl->FlightEntityId[0]);          
    }     


    for(i=0;i<CF_NUM_INPUT_CHANNELS;i++)
    {        
        if(Tbl->InCh[0].IncomingPDUMsgId > CFE_SB_HIGHEST_VALID_MSGID)    
        {        
            TblValidationErrs++;

            if(TblValidationErrs == 1)
                CFE_EVS_SendEvent(CF_TBL_VAL_ERR2_EID, CFE_EVS_ERROR,
                    "Cannot set IncomingPDUMsgId 0x%X > CFE_SB_HIGHEST_VALID_MSGID 0x%X",
                    Tbl->InCh[0].IncomingPDUMsgId,CFE_SB_HIGHEST_VALID_MSGID);          
        }

    }

    /* Validate Outgoing File Chunk Size */
    /* convert the tbl value to integer, then compare */
    ValueAsInt = atoi(&Tbl->OutgoingFileChunkSize[0]);
    if(ValueAsInt > CF_MAX_OUTGOING_CHUNK_SIZE)
    {                
        TblValidationErrs++;

        if(TblValidationErrs == 1)
            CFE_EVS_SendEvent(CF_TBL_VAL_ERR3_EID, CFE_EVS_ERROR,
                "Cannot set OUTGOING_FILE_CHUNK_SIZE(%d) > CF_MAX_OUTGOING_CHUNK_SIZE (%d)",
                ValueAsInt,CF_MAX_OUTGOING_CHUNK_SIZE);  
    } 

    for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        /* verify entry-in-use is 0 (unused) or 1 (in use) */
        if(Tbl->OuCh[i].EntryInUse > 1)
        {
            TblValidationErrs++;

            if(TblValidationErrs == 1)
                CFE_EVS_SendEvent(CF_TBL_VAL_ERR4_EID, CFE_EVS_ERROR,
                    "Ch%d EntryInUse %d must be 0-unused or 1-in use",
                    i,Tbl->OuCh[i].EntryInUse);
        }

        /* need only test in-use entries, EntryInUse cannot be changed by cmd */
        if(Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE) 
        {
            /* verify dequeue enable is 0 (disabled) or 1 (enabled) */
            if(Tbl->OuCh[i].DequeueEnable > 1)
            {
                TblValidationErrs++;
    
                if(TblValidationErrs == 1)
                    CFE_EVS_SendEvent(CF_TBL_VAL_ERR5_EID, CFE_EVS_ERROR,
                        "Ch%d DequeueEnable %d must be 0-disabled or 1-enabled",
                        i,Tbl->OuCh[i].DequeueEnable);
            }


            /* Validate Downlink MsgId for this channel */
            if(Tbl->OuCh[i].OutgoingPduMsgId > CFE_SB_HIGHEST_VALID_MSGID)
            {
                TblValidationErrs++;

                if(TblValidationErrs == 1)
                    CFE_EVS_SendEvent(CF_TBL_VAL_ERR7_EID, CFE_EVS_ERROR,
                        "Cannot set Ch%d OutgoingPduMsgId 0x%X > CFE_SB_HIGHEST_VALID_MSGID 0x%X",
                        i,Tbl->OuCh[i].OutgoingPduMsgId,CFE_SB_HIGHEST_VALID_MSGID);
            }

                
            for(j=0;j<CF_MAX_POLLING_DIRS_PER_CHAN;j++)
            {
                /* verify entry-in-use is 0 (unused) or 1 (in use) */
                if(Tbl->OuCh[i].PollDir[j].EntryInUse > 1)
                {
                    TblValidationErrs++;
    
                    if(TblValidationErrs == 1)
                        CFE_EVS_SendEvent(CF_TBL_VAL_ERR8_EID, CFE_EVS_ERROR,
                            "Ch%d,PollDir%d EntryInUse %d must be 0-unused or 1-in use",
                            i,j,Tbl->OuCh[i].PollDir[j].EntryInUse);
                }                
                
                /* need only test in-use entries, EntryInUse cannot be changed by cmd */
                if(Tbl->OuCh[i].PollDir[j].EntryInUse == CF_ENTRY_IN_USE)
                {
                    /* verify enable state is 0 (disabled) or 1 (enabled) */
                    if(Tbl->OuCh[i].PollDir[j].EnableState > 1)
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR9_EID, CFE_EVS_ERROR,
                                "Ch%d,PollDir%d EnableState %d must be 0-disabled or 1-enabled",
                                i,j,Tbl->OuCh[i].PollDir[j].EnableState);
                    }
                    

                    /* Validate Poll directory class values for this channel */            
                    if((Tbl->OuCh[i].PollDir[j].Class < 1)||(Tbl->OuCh[i].PollDir[j].Class > 2))
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR10_EID, CFE_EVS_ERROR,
                                "Chan %d,PollDir%d Class %d must be 1-unreliable or 2-reliable",
                                i,j,Tbl->OuCh[i].PollDir[j].Class);          
                    }

                    /* verify preserve state is 0 (delete) or 1 (preserve) */
                    if(Tbl->OuCh[i].PollDir[j].Preserve > 1)
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR11_EID, CFE_EVS_ERROR,
                                "Ch%d,PollDir%d Preserve %d must be 0-delete or 1-preserve",
                                i,j,Tbl->OuCh[i].PollDir[j].Preserve);
                    }



                    /* check that the polling paths are terminated, have */
                    /* forward slash at last character and no spaces */
                    /* if error do not print path in event, it may be unterminated */
                    if(CF_ValidateSrcPath(Tbl->OuCh[i].PollDir[j].SrcPath)==CF_ERROR)
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR12_EID, CFE_EVS_ERROR,
                                "Ch%d,PollSrcPath must be terminated,have no spaces,slash at end",i);                        
                    }

                    if(CF_ValidateDstPath(Tbl->OuCh[i].PollDir[j].DstPath)==CF_ERROR)
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR13_EID, CFE_EVS_ERROR,
                                "Ch%d,PollDstPath must be terminated and have no spaces",i);
                    }
                   
            
                    /* Validate Peer Entity Id for this channel */
                    if(CF_ValidateEntityId(&Tbl->OuCh[i].PollDir[j].PeerEntityId[0]) == CF_ERROR)
                    {
                        TblValidationErrs++;
        
                        if(TblValidationErrs == 1)
                            CFE_EVS_SendEvent(CF_TBL_VAL_ERR6_EID, CFE_EVS_ERROR,
                                "Cannot set Ch%d Poll %d PeerEntityId to %s,must be 2 byte, dotted decimal fmt like 0.24",
                                i,j,&Tbl->OuCh[i].PollDir[j].PeerEntityId[0]);          
                    }




                }/* end if poll dir in use */
            
            }/* end looping through poll dirs */        

        }/* end if channel in use */

    }/* end looping through channels */

    if(TblValidationErrs == 0)
    {    
        RtnStat = CF_SUCCESS;
    
    }else{
    
        CFE_EVS_SendEvent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR,
             "Total Validation Errors - %d for CF Configuration Table",
                                TblValidationErrs);

        RtnStat = CF_ERROR;        
    }
    
    return RtnStat;

}   /* end CF_ValidateCFConfigTable */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process a command pipe message                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CF_AppPipe (CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t  MessageID;
    uint16          CommandCode;
    
    MessageID = CFE_SB_GetMsgId (MessagePtr);
    switch (MessageID)
    {
        case CF_WAKE_UP_REQ_CMD_MID:
            CF_WakeupProcessing(MessagePtr);                    
            break;
            
        case CF_SEND_HK_MID:
            CF_HousekeepingCmd(MessagePtr);
            CF_CheckForTblRequests();
            break;

        /* CF commands   */
        case CF_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(MessagePtr);
            switch (CommandCode)
            {
                case CF_NOOP_CC:
                    CF_NoopCmd(MessagePtr);
                    break;

                case CF_RESET_CC:
                    CF_ResetCtrsCmd(MessagePtr);
                    break;
                    
                case CF_PLAYBACK_FILE_CC: 
                    CF_PlaybackFileCmd(MessagePtr);
                    break;

                case CF_PLAYBACK_DIR_CC:
                    CF_PlaybackDirectoryCmd(MessagePtr);
                    break;                                                                                                                                    
                                
                case CF_FREEZE_CC:                    
                    CF_FreezeCmd(MessagePtr);
                    break;                        
                
                case CF_THAW_CC:                    
                    CF_ThawCmd(MessagePtr);
                    break;                                                             
                    
                case CF_SUSPEND_CC:
                    CF_CARSCmd(MessagePtr,"Suspend");
                    break;
                 
                case CF_RESUME_CC:
                    CF_CARSCmd(MessagePtr,"Resume");
                    break;                                   

                case CF_CANCEL_CC:                    
                    CF_CARSCmd(MessagePtr,"Cancel");
                    break;

                case CF_ABANDON_CC:
                    CF_CARSCmd(MessagePtr,"Abandon");
                    break;

                case CF_SET_MIB_PARAM_CC:
                    CF_SetMibCmd(MessagePtr);
                    break; 

                case CF_GET_MIB_PARAM_CC:
                    CF_GetMibCmd(MessagePtr);
                    break;

                case CF_SEND_CFG_PARAMS_CC:
                    CF_SendCfgParams(MessagePtr);
                    break;
                
                case CF_SET_POLL_PARAM_CC:
                    CF_SetPollParam(MessagePtr);
                    break;
                                
                case CF_SEND_TRANS_DIAG_DATA_CC:                   
                    CF_SendTransDataCmd(MessagePtr);
                    break;                
                    
                case CF_WRITE_QUEUE_INFO_CC:
                    CF_WriteQueueCmd(MessagePtr);                      
                    break;

                case CF_ENABLE_DEQUEUE_CC:
                    CF_EnableDequeueCmd(MessagePtr);
                    break;                    
                                      
                case CF_DISABLE_DEQUEUE_CC:
                    CF_DisableDequeueCmd(MessagePtr);
                    break;
                    
                case CF_ENABLE_DIR_POLLING_CC:
                    CF_EnablePollCmd(MessagePtr);
                    break;

                case CF_DISABLE_DIR_POLLING_CC:
                    CF_DisablePollCmd(MessagePtr);
                    break;

                case CF_DELETE_QUEUE_NODE_CC:
                    CF_DequeueNodeCmd(MessagePtr);
                    break;

                case CF_PURGE_QUEUE_CC:
                    CF_PurgeQueueCmd(MessagePtr);    
                    break;
                    
                case CF_WR_ACTIVE_TRANS_CC:
                    CF_WriteActiveTransCmd(MessagePtr);
                    break; 
                    
                case CF_KICKSTART_CC:
                    CF_KickstartCmd(MessagePtr);
                    break;
                    
                case CF_QUICKSTATUS_CC:
                    CF_QuickStatusCmd(MessagePtr);
                    break;                    

                case CF_GIVETAKE_CC:
                    CF_GiveTakeSemaphoreCmd(MessagePtr);
                    break;

                case CF_ENADIS_AUTO_SUSPEND_CC:
                    CF_AutoSuspendEnCmd(MessagePtr);
                    break;       
                
                default:
                    CFE_EVS_SendEvent(CF_CC_ERR_EID, CFE_EVS_ERROR,
                    "Cmd Msg with Invalid command code Rcvd -- ID = 0x%04X, CC = %d",
                        MessageID, CommandCode);

                    CF_AppData.Hk.ErrCounter++;
                    break;
            }
            break;
        
        default:
            /* First check to see if MessageID is an Incoming PDU */
            if (CF_MsgIdMatchesInputChannel(MessageID))
            {
                CF_SendPDUToEngine(MessagePtr);
            
            }else{

                CFE_EVS_SendEvent(CF_MID_ERR_EID, CFE_EVS_ERROR,
                      "Unexpected Msg Received MsgId -- ID = 0x%04X",
                      MessageID);
            }
            break;
    }

    return;

} /* end of CF_AppPipe() */



void CF_SendPDUToEngine(CFE_SB_MsgPtr_t MessagePtr)
{
    
    CF_PDU_Hdr_t            *PduHdrPtr;
    uint8                   *IncomingPduPtr;
    uint8                   EntityIdBytes,TransSeqBytes,PduHdrBytes;
#ifdef CF_DEBUG
    uint8                   PduData0,PduData1;
    uint8                   *PduDataPtr;
#endif
    /* This will count up for class 2 playback responses too */
    CF_AppData.Hk.App.PDUsReceived++;    

    /* IncomingPduPtr points to first byte of incoming pdu hdr */
    IncomingPduPtr = ((uint8 *)MessagePtr);
    
    if(CF_GetPktType(CFE_SB_GetMsgId(MessagePtr))==CF_CMD)
    {
        IncomingPduPtr += CFE_SB_CMD_HDR_SIZE;
    }else{
        IncomingPduPtr += CFE_SB_TLM_HDR_SIZE;

    }/* end if */
    
    PduHdrPtr = (CF_PDU_Hdr_t *)IncomingPduPtr;

    /* calculate size of incoming pdu to ensure we don't overflow the buf */
    EntityIdBytes = ((PduHdrPtr->Octet4 >> 4) & 0x07) + 1;
    TransSeqBytes = (PduHdrPtr->Octet4 & 0x07) + 1;
    PduHdrBytes = CF_PDUHDR_FIXED_FIELD_BYTES + (EntityIdBytes * 2) + TransSeqBytes;

    /*  
    **  This version of CF does not support all the various header sizes that  
    **  are allowed by the cfdp standard.
    **  Entity ids in the hdr must be 2 bytes each and the transaction id must
    **  be 4 bytes. The full header size must be 12 bytes.
    */
    if(PduHdrBytes != CF_PDU_HDR_BYTES){
        CFE_EVS_SendEvent(CF_PDU_RCV_ERR1_EID, CFE_EVS_ERROR,
            "PDU Rcv Error:PDU Hdr illegal size - %d, must be %d bytes",
            PduHdrBytes,CF_PDU_HDR_BYTES);
        CF_AppData.Hk.App.PDUsRejected++; 
        return;
    }/* end if */

#ifdef CF_DEBUG
    if(cfdbg > 0){
        if(CFE_TST(PduHdrPtr->Octet1,4)){
            OS_printf("CF:Received File Data PDU,len=%d\n",
                    PduHdrPtr->PDataLen + PduHdrBytes);
        }else{
            /* Get the values of the first two pdu data field bytes. Needed */
            /* to find out what type of file directive pdu has been received */
            PduDataPtr = (uint8 *)PduHdrPtr + PduHdrBytes;
            PduData0 = *PduDataPtr;
            PduDataPtr++;
            PduData1 = *PduDataPtr;
        
            OS_printf("CF:Received ");
            CF_PrintPDUType(PduData0,PduData1);
            OS_printf(" PDU,len=%d\n", PduHdrPtr->PDataLen + PduHdrBytes);
        }
    }/* end if */           
#endif

    /* claculate the pdu 'length' field needed by the engine */
    CF_AppData.RawPduInputBuf.length = PduHdrPtr->PDataLen + PduHdrBytes;

    if(CF_AppData.RawPduInputBuf.length > CF_INCOMING_PDU_BUF_SIZE){
        CFE_EVS_SendEvent(CF_PDU_RCV_ERR2_EID, CFE_EVS_ERROR,
            "PDU Rcv Error:length %d exceeds CF_INCOMING_PDU_BUF_SIZE %d",
            CF_AppData.RawPduInputBuf.length,CF_INCOMING_PDU_BUF_SIZE);
        CF_AppData.Hk.App.PDUsRejected++; 
        return;
    }/* end if */

    CFE_PSP_MemCpy(&CF_AppData.RawPduInputBuf.content[0],
                    IncomingPduPtr,
                    CF_AppData.RawPduInputBuf.length);
      
    if(!cfdp_give_pdu(CF_AppData.RawPduInputBuf))
    {
        CFE_EVS_SendEvent(CF_PDU_RCV_ERR3_EID, CFE_EVS_ERROR,
            "cfdp_give_pdu returned error in CF_SendPDUToEngine");
        CF_AppData.Hk.App.PDUsRejected++;
    }
    
}/* end CF_SendPDUToEngine */



void CF_GetHandshakeSemIds(void)
{
    int32 Status,i;

    for(i=0;i<CF_MAX_PLAYBACK_CHANNELS;i++)
    {
        
        if(CF_AppData.Tbl->OuCh[i].EntryInUse == CF_ENTRY_IN_USE) 
        {
            Status = OS_CountSemGetIdByName(&CF_AppData.Chan[i].HandshakeSemId, 
                 (const char *)&CF_AppData.Tbl->OuCh[i].SemName);
            if (Status != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_HANDSHAKE_ERR1_EID, CFE_EVS_ERROR,
                    "SemGetId Err:Chan %d downlink PDUs cannot be throttled.0x%08X",i,Status);
                CF_AppData.Chan[i].HandshakeSemId = CF_INVALID;
        
            }
#ifdef CF_DEBUG            
            else{
                
                if(cfdbg > 0)
                  OS_printf("CF:Chan %d SemId = 0x%08X\n",i,CF_AppData.Chan[i].HandshakeSemId);
            
            }/* end if */
#endif        
        }/* end if */
        
    }/* end for */

}/* end CF_GetHandshakeSemIds */


void CF_WakeupProcessing(CFE_SB_MsgPtr_t MessagePtr)
{

    uint32  chan;
    uint32  i;
    char    TransIdBuf[CF_MAX_TRANSID_CHARS];

    if(CF_VerifyCmdLength(MessagePtr,sizeof(CF_NoArgsCmd_t))==CF_BAD_MSG_LENGTH_RC)
    {
        CF_AppData.Hk.ErrCounter++;

    }else{

        CF_AppData.Hk.App.WakeupForFileProc++;
        
        /* Do Auto Suspends of Outgoing Transactions if enabled */
        if(CF_AppData.Hk.AutoSuspend.EnFlag == CF_ENABLED)
        {        
            if((CF_AutoSuspendCnt > 0) && (CF_AutoSuspendCnt <= CF_AUTOSUSPEND_MAX_TRANS))
            {
                for(i=0;i < CF_AutoSuspendCnt;i++)
                {
                    sprintf(TransIdBuf,"%s_%lu",CF_AppData.Tbl->FlightEntityId,
                                                CF_AutoSuspendArray[i]);                            
                                    
                    CF_BuildCmdedRequest("Suspend",&TransIdBuf[0]);
                
                }/* end for */
                        
            }/* end if CF_AutoSuspendCnt > 0 */
                    
            CF_AutoSuspendCnt = 0;
    
        }/* end if Auto suspend is enabled */

        /* determine if it's time to poll directories or dequeue pending queue */
        for(chan=0;chan<CF_MAX_PLAYBACK_CHANNELS;chan++){
            if(CF_AppData.Tbl->OuCh[chan].EntryInUse == CF_ENTRY_IN_USE)            
            {                        
                /* check polling directories */
                CF_AppData.Chan[chan].PollDirTimer++;
                if(CF_AppData.Chan[chan].PollDirTimer == CF_AppData.Tbl->NumWakeupsPerPollDirChk)
                {
                    CF_CheckPollDirs(chan);
                    CF_AppData.Chan[chan].PollDirTimer = 0;
                    CF_AppData.Hk.Chan[chan].PollDirsChecked++;
                }/* end */
    
    
                /* Dequeue pending queue if playback not in progress and channel enabled */
                if((CF_AppData.Chan[chan].DataBlast == CF_NOT_IN_PROGRESS) &&
                    (CF_AppData.Tbl->OuCh[chan].DequeueEnable == CF_ENABLED))
                {
                    CF_AppData.Chan[chan].PendQTimer++;
                    if(CF_AppData.Chan[chan].PendQTimer == CF_AppData.Tbl->NumWakeupsPerQueueChk)
                    {
                        CF_StartNextFile(chan);
                        CF_AppData.Chan[chan].PendQTimer = 0;
                        CF_AppData.Hk.Chan[chan].PendingQChecked++;
                    }
                }/* end if playback not in progress */        
            
            }/* end if channel in use and enabled */    
        
        }/* end loop through max channels */        
        
        /* cycle the engine */
        for(i=0;i<CF_AppData.Tbl->NumEngCyclesPerWakeup;i++)
        {    
            CF_AppData.Hk.App.EngineCycleCount++;        
    
            CFE_ES_PerfLogEntry(CF_CYCLE_ENG_PERF_ID);
            cfdp_cycle_each_transaction();
            CFE_ES_PerfLogExit(CF_CYCLE_ENG_PERF_ID);
        
        }/* end if */
        
    }/* end if pkt length is ok */

    return;        
    
}/* end CF_WakeupProcessing */



/* CF does not allow table loads. If the gnd tries to do one, they must abort
** the attempt (via table cmd) or table services will be waiting forever to get
** the 'Update' call from CF. If the abort is not executed, the gnd may have  
** trouble trying to dump the table or get a checksum of the table (via tbl 
** validate cmd).
*/
void CF_CheckForTblRequests(void)
{

    int32   Status = CFE_SUCCESS;

    
    Status = CFE_TBL_GetStatus(CF_AppData.ConfigTableHandle);

    /* gnd may be trying to get the checksum of the table (via tbl validate cmd) */
    if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
    {
        /* Validate the Table */
        CFE_TBL_Validate(CF_AppData.ConfigTableHandle);

    
    }
    /* if gnd is trying to do a tbl load  */
    else if (Status == CFE_TBL_INFO_UPDATE_PENDING)
    {
        CFE_EVS_SendEvent(CF_TBL_LD_ATTEMPT_EID, CFE_EVS_ERROR,
           "CF Config Tbl cannot be updated! Load attempt must be aborted!");        
    }


}/* end CF_CheckForTblRequests */



/* Determine if MsgId received is an incoming PDU MsgId */
int32 CF_MsgIdMatchesInputChannel( CFE_SB_MsgId_t  MessageID)
{
    int32 i;
    int32 bMatchFound = 0;
    for(i=0; !bMatchFound && i<CF_NUM_INPUT_CHANNELS; i++)
    {
        if (CF_AppData.Tbl->InCh[i].IncomingPDUMsgId == MessageID)
        {
            bMatchFound = 1;
        }
    }
    return bMatchFound;
}




/************************/
/*  End of File Comment */
/************************/
