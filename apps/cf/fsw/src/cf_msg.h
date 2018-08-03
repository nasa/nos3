/************************************************************************
** File:
**   $Id: cf_msg.h 1.25.1.2 2015/03/06 15:30:40EST sstrege Exp  $
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
**  The CF Application header file
**
** Notes:
**
** $Log: cf_msg.h  $
** Revision 1.25.1.2 2015/03/06 15:30:40EST sstrege 
** Added copyright information
** Revision 1.25.1.1 2012/02/24 16:11:57EST rmcgraw 
** DCR18668:1 Added function code and struct for new cmd
** Revision 1.25 2011/05/19 15:32:08EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.24 2011/05/17 16:14:03EDT rmcgraw 
** DCR14976:6 Rename CF_SET_POLL_DIR_PATH_CC to CF_SET_POLL_PARAM_CC
** Revision 1.23 2011/05/17 09:25:10EDT rmcgraw 
** DCR14529:1 Added processing for GiveTake Cmd
** Revision 1.22 2011/05/13 14:16:12EDT rmcgraw 
** DCR13546:1 Added one byte of padding to SendActiveTransCmd to make pkt count even.
** Revision 1.21 2011/05/10 17:04:49EDT rmcgraw 
** Removed comment
** Revision 1.20 2011/05/10 15:50:39EDT rmcgraw 
** DCR14527:1 Added semaphore value to each channels telemetry
** Revision 1.19 2010/11/04 12:57:35EDT rmcgraw 
** DCR13051:1 Added DebugCompiledIn to cfg packet
** Revision 1.18 2010/11/04 11:26:29EDT rmcgraw 
** DCR13223:1 Changed WakeupCntForFileProc to WakeupForFileProc
** Revision 1.17 2010/11/04 10:56:04EDT rmcgraw 
** DCR13223:1 Added tlm ctrs for engine cycles and wakeup
** Revision 1.16 2010/11/01 16:09:35EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.15 2010/10/25 11:21:54EDT rmcgraw 
** DCR12573:1 Changes to allow more than one incoming PDU MsgId
** Revision 1.14 2010/10/20 14:15:22EDT rmcgraw 
** DCR128222:1 Added tlm counter for total abandon transactions
** Revision 1.13 2010/10/20 13:42:08EDT rmcgraw 
** Dcr12803:1 Added telemetry point to show low memory mark
** Revision 1.12 2010/10/20 10:33:32EDT rmcgraw 
** DCR13052:1 Changed pb-dir-cmd param order and corresponding users guide
** Revision 1.11 2010/08/04 15:17:41EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.10 2010/07/20 14:37:46EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.9 2010/07/08 13:47:33EDT rmcgraw 
** DCR11510:1 Added termination checking on all cmds that take a string
** Revision 1.8 2010/07/07 17:38:14EDT rmcgraw 
** DCR11510:1 Added kickstart and Quick status command defs
** Revision 1.7 2010/04/23 15:43:04EDT rmcgraw 
** DCR11510:1 Minor Cleanup
** Revision 1.6 2010/04/23 10:18:32EDT rmcgraw 
** DCR11510:1 Removed MaxRestrictedDirs in Cfg Pkt
** Revision 1.5 2010/04/23 08:39:20EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.4 2010/03/26 15:30:26EDT rmcgraw 
** DCR11510 Various developmental changes
** Revision 1.3 2010/03/12 12:14:39EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:14:20EST rmcgraw 
** DCR10350:3 Added SetPollDirNameCmd_t and ConfigPacket_t
** Revision 1.1 2009/11/24 12:48:53EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_msg_h_
#define _cf_msg_h_


/*************************************************************************
** Includes
**************************************************************************/
#include "cfe.h"
#include "cf_defs.h"
#include "cf_platform_cfg.h"


/****************************************
** CF app command packet command codes
****************************************/

#define CF_NOOP_CC                      0
#define CF_RESET_CC                     1
#define CF_PLAYBACK_FILE_CC             2
#define CF_PLAYBACK_DIR_CC              3
#define CF_FREEZE_CC                    4
#define CF_THAW_CC                      5 
#define CF_SUSPEND_CC                   6
#define CF_RESUME_CC                    7
#define CF_CANCEL_CC                    8
#define CF_ABANDON_CC                   9
#define CF_SET_MIB_PARAM_CC             10
#define CF_GET_MIB_PARAM_CC             11
#define CF_SEND_TRANS_DIAG_DATA_CC      12
#define CF_SET_POLL_PARAM_CC            13
#define CF_SEND_CFG_PARAMS_CC           14
#define CF_WRITE_QUEUE_INFO_CC          15
#define CF_ENABLE_DEQUEUE_CC            16
#define CF_DISABLE_DEQUEUE_CC           17
#define CF_ENABLE_DIR_POLLING_CC        18
#define CF_DISABLE_DIR_POLLING_CC       19
#define CF_DELETE_QUEUE_NODE_CC         20
#define CF_PURGE_QUEUE_CC               21
#define CF_WR_ACTIVE_TRANS_CC           22
#define CF_KICKSTART_CC                 23
#define CF_QUICKSTATUS_CC               24
#define CF_GIVETAKE_CC                  25
#define CF_ENADIS_AUTO_SUSPEND_CC       26
#define CF_CYCLES_PER_WAKEUP            27


/****************************
**  CF Command Formats     **
*****************************/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CF_NoArgsCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint8   Value;/* 0=all, 1=cmd, 2=fault 3=up 4=down */
    uint8   Spare[3];

} CF_ResetCtrsCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint8   Class;
    uint8   Channel;
    uint8   Priority;
    uint8   Preserve;
    char    PeerEntityId[CF_MAX_CFG_VALUE_CHARS];/* 2 byte dotted-decimal string eg. "0.24"*/
    char    SrcFilename[OS_MAX_PATH_LEN];
    char    DstFilename[OS_MAX_PATH_LEN];

}CF_PlaybackFileCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   Class;
    uint8   Chan;
    uint8   Priority;
    uint8   Preserve;
    char    PeerEntityId[CF_MAX_CFG_VALUE_CHARS];/* 2 byte dotted-decimal string eg. "0.24"*/    
    char    SrcPath[OS_MAX_PATH_LEN];
    char    DstPath[OS_MAX_PATH_LEN];
    
}CF_PlaybackDirCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint8   Chan;
    uint8   Spare[3];

} CF_EnDisDequeueCmd_t;

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];/**< \brief cFE Software Bus Command Message Header */ 
    uint8   Chan;   /* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
    uint8   Dir;    /* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
    uint8   Spare[2];

} CF_EnDisPollCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint8   Chan;   /* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
    uint8   Dir;    /* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1) */
    uint8   Class;
    uint8   Priority;
    uint8   Preserve;
    uint8   Spare[3];
    char    PeerEntityId[CF_MAX_CFG_VALUE_CHARS];
    char    SrcPath[OS_MAX_PATH_LEN];
    char    DstPath[OS_MAX_PATH_LEN];

} CF_SetPollParamCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];    
    char    Param [CF_MAX_CFG_PARAM_CHARS];
    char    Value [CF_MAX_CFG_VALUE_CHARS]; 
}CF_SetMibParam_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];    
    char    Param [CF_MAX_CFG_PARAM_CHARS];
}CF_GetMibParam_t;


/* CARS - Cancel,Abandon,Resume,Suspend Cmds */
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Trans[OS_MAX_PATH_LEN];

}CF_CARSCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];    
    uint8   Type; /*(up=1/down=2)*/
    uint8   Chan;
    uint8   Queue;/* 0=pending,1=active,2=history */
    uint8   Spare;
    char    Filename[OS_MAX_PATH_LEN];

}CF_WriteQueueCmd_t;

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   Type; /*(all=0/up=1/down=2)*/
    uint8   Spare;
    char    Filename[OS_MAX_PATH_LEN];

}CF_WriteActiveTransCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];    
    char    Trans[OS_MAX_PATH_LEN];

}CF_SendTransCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Trans[OS_MAX_PATH_LEN];

}CF_DequeueNodeCmd_t;

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   Type;/*(up=1/down=2)*/
    uint8   Chan;
    uint8   Queue;/* 0=pending,1=active,2=history */
    uint8   Spare;
    
}CF_PurgeQueueCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   Chan;
    uint8   Spare[3];

} CF_KickstartCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Trans[OS_MAX_PATH_LEN];

}CF_QuickStatCmd_t;


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   Chan;
    uint8   GiveOrTakeSemaphore;

}CF_GiveTakeCmd_t;

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint32  EnableDisable;/* 0 to disable, 1 to enable */

}CF_AutoSuspendEnCmd_t;

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; 
    uint32  NumCyclesPerWakeup;

}CF_CyclesPerWakeupCmd_t;


/****************************
**  CF Telemetry Formats   **
*****************************/
typedef struct
{
    uint32  EnFlag;
    uint32  LowFreeMark;

}AutoSuspend_Telemetry_t;

typedef struct
{
    uint32                  MetaCount;
    uint32                  UplinkActiveQFileCnt;
    uint32                  SuccessCounter;
    uint32                  FailedCounter;
    char                    LastFileUplinked[OS_MAX_PATH_LEN];    
    
}Uplink_Telemetry_t;

typedef struct
{
    
    uint32                  PDUsSent;    
    uint32                  FilesSent;  
    uint32                  SuccessCounter;
    uint32                  FailedCounter;

    uint32                  PendingQFileCnt;
    uint32                  ActiveQFileCnt;
    uint32                  HistoryQFileCnt;
    
    uint32                  Flags;  /* 0=ChanDequeue enabled,1=Chan Blast In progress,*/
    uint32                  RedLightCntr;
    uint32                  GreenLightCntr;
    uint32                  PollDirsChecked;
    uint32                  PendingQChecked;
    uint32                  SemValue;
           
}Downlink_Telemetry_t;

typedef struct
{

    char                    FlightEngineEntityId[CF_MAX_CFG_VALUE_CHARS];
    uint32                  Flags;/* bit 0=frozen */
    uint32                  MachinesAllocated;
    uint32                  MachinesDeallocated;
    uint8                   are_any_partners_frozen; /* Can be true even if there are
					                                * no transactions in-progress.*/
    uint8                   Spare[3];					                                
    uint32                  how_many_senders;        /* ...active Senders? */
    uint32                  how_many_receivers;      /* ...active Receivers? */
    uint32                  how_many_frozen;         /* ...trans are frozen? */
    uint32                  how_many_suspended;      /* ...trans are suspended? */
    uint32                  total_files_sent;        /* ...files sent succesfully */
    uint32                  total_files_received;    /* ...files received successfully */
    uint32                  total_unsuccessful_senders;
    uint32                  total_unsuccessful_receivers;    
}Engine_Telemetry_t;

/* Condition Code Table Counters */
typedef struct
{
  
    uint8                   PosAckNum; /* Positive ACK Limit Counter */                  
    uint8                   FileStoreRejNum; /* FileStore Rejection Counter */           
    uint8                   FileChecksumNum; /* File Checksum Failure Counter */         
    uint8                   FileSizeNum; /* Filesize Error Counter */         
    uint8                   NakLimitNum; /* NAK Limit Counter */   
    uint8                   InactiveNum; /* Inactivity Counter */         
    uint8                   SuspendNum;/* Suspend Request Counter */
    uint8                   CancelNum; /* Cancel Request Counter */
       
}Fault_Telemetry_t;

typedef struct
{
    uint32                  WakeupForFileProc;
    uint32                  EngineCycleCount;
    uint32                  MemInUse;
    uint32                  PeakMemInUse;
    uint32                  LowMemoryMark;
    uint32                  MaxMemNeeded;
    uint32                  MemAllocated;
    uint32                  BufferPoolHandle;
    
    uint32                  QNodesAllocated;
    uint32                  QNodesDeallocated;
    uint32                  PDUsReceived;
    uint32                  PDUsRejected;
    
    uint32                  TotalInProgTrans;
    uint32                  TotalFailedTrans;
    uint32                  TotalAbandonTrans;
    uint32                  TotalSuccessTrans;
    uint32                  TotalCompletedTrans;
    char                    LastFailedTrans[CF_MAX_TRANSID_CHARS];

}App_Telemetry_t;


/**
**  \cftlm CF Application housekeeping Packet
*/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16  CmdCounter;         /**< \cftlmmnemonic \CF_CMDPC
                                \brief Count of valid commands received */
    uint16  ErrCounter;         /**< \cftlmmnemonic \CF_CMDEC
                                \brief Count of invalid commands received */
    App_Telemetry_t         App;
    AutoSuspend_Telemetry_t AutoSuspend;
    Fault_Telemetry_t       Cond;
    Engine_Telemetry_t      Eng;
    Uplink_Telemetry_t      Up;
    Downlink_Telemetry_t    Chan[CF_MAX_PLAYBACK_CHANNELS];         
    
} CF_HkPacket_t;


typedef struct
{
    uint8       TransLen;
    uint8       TransVal;
    uint8       Naks; /* How many Nak PDUs have been sent/recd? */
    uint8       PartLen;
    uint8       PartVal;/* Who is this transaction with? */
    uint8       Phase;/* Either 1, 2, 3, or 4 */
    uint8       Spare1;
    uint8       Spare2;
    uint32      Flags;    
    uint32      TransNum;
    uint32      Attempts;/* How many attempts to send current PDU? */
    uint32      CondCode;
    uint32      DeliCode;
    uint32      FdOffset;/* Offset of last Filedata sent/received */
    uint32      FdLength;/* Length of last Filedata sent/received */
    uint32      Checksum;
    uint32      FinalStat;
    uint32      FileSize;
    uint32      RcvdFileSize;
    uint32      Role;/* (e.g. Receiver Class 1) */
    uint32      State;
    uint32      StartTime;/* When was this transaction started? */
    char        SrcFile[OS_MAX_PATH_LEN];
    char        DstFile[OS_MAX_PATH_LEN];
    char        TmpFile[OS_MAX_PATH_LEN];

}CF_EngTransStat_t;



typedef struct
{

    uint32      Status;
    uint32      CondCode;
    uint32      Priority;/* applies only to playback files*/
    uint32      Class;
    uint32      ChanNum;/* applies only to playback files*/
    uint32      Source;/* from poll dir,playbackfile cmd or playback dir cmd */
    uint32      NodeType;
    uint32      TransNum;    
    char        SrcEntityId[CF_MAX_CFG_VALUE_CHARS];
    char        SrcFile[OS_MAX_PATH_LEN];
    char        DstFile[OS_MAX_PATH_LEN];


}CF_AppTransStat_t;




/**
**  \cftlm CF Application Single Transaction Status Packet
*/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
 
    CF_EngTransStat_t   Eng;
    CF_AppTransStat_t   App;

}CF_TransPacket_t;


/**
** CF Queue Info File Entry
**
** Structure of one element of the queue information in response to CF_WRITE_QUEUE_INFO_CC
*/
typedef struct
{
    uint32  TransStatus;
    uint32  TransNum;/* Transaction number assigned by engine */
    char    SrcEntityId[CF_MAX_CFG_VALUE_CHARS];/* Entity Id of file sender */
    char    SrcFile[OS_MAX_PATH_LEN];/* Path/Filename at the source */
    
 }CF_QueueInfoFileEntry_t;
 
 
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; 
    uint32  EngCycPerWakeup;
    uint32  AckLimit;
    uint32  AckTimeout;
    uint32  NakLimit;
    uint32  NakTimeout;
    uint32  InactTimeout;
    uint32  DefOutgoingChunkSize;
    uint32  PipeDepth;    
    uint32  MaxSimultaneousTrans;
    uint32  IncomingPduBufSize;
    uint32  OutgoingPduBufSize;
    uint32  NumInputChannels;
    uint32  MaxPlaybackChans;
    uint32  MaxPollingDirsPerChan;
    uint32  MemPoolBytes;
    uint32  DebugCompiledIn;
    char    SaveIncompleteFiles[8];
    char    PipeName[OS_MAX_API_NAME];
    char    TmpFilePrefix[OS_MAX_PATH_LEN];
    char    CfgTblName[OS_MAX_PATH_LEN];
    char    CfgTbleFilename[OS_MAX_PATH_LEN];
    char    DefQInfoFilename[OS_MAX_PATH_LEN];    

}CF_ConfigPacket_t;


      
#endif /* _cf_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
