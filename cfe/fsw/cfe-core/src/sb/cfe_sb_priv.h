/******************************************************************************
** File: cfe_sb_priv.h
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
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for SB internal use.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_priv.h  $
** Revision 1.21 2014/04/24 09:57:06GMT-05:00 rmcgraw 
** DCR19487:1 - Remove size argument in CFE_SB_GetBufferFromCaller
** Revision 1.20 2012/01/13 12:15:13EST acudmore
** Changed license text to reflect open source
** Revision 1.19 2011/12/20 10:26:54EST rmcgraw
** DCR15187:1 Removed error return code CFE_SB_MSGCNT_ERR
** Revision 1.18 2011/12/07 19:19:48EST aschoeni
** Removed returns for TIME and SB for cleaning up apps
** Revision 1.17 2011/04/28 10:10:15EDT rmcgraw
** DCR14592:1 Fix for the SB Send semaphore problem
** Revision 1.16 2010/11/08 14:55:21EST aschoeni
** Moved CFE_SB_DEFAULT_MSG_LIMIT from cfe_sb_priv to cfe_platform_cfg
** Revision 1.15 2010/11/04 16:41:51EDT aschoeni
** Added optional sender information storage
** Revision 1.14 2010/11/04 14:43:59EDT jmdagost
** Added cfe.h to include list.
** Revision 1.13 2010/11/04 14:24:13EDT aschoeni
** Optimized usage of locking
** Revision 1.12 2009/07/29 19:22:41EDT aschoeni
** Updated for ZeroCopyHandle_t
** Revision 1.11 2009/07/24 18:26:22EDT aschoeni
** Added Zero Copy Mode
** Revision 1.10 2009/07/20 14:10:24EDT aschoeni
** Made GetAppTskName reentrant
** Revision 1.9 2009/07/17 19:42:30EDT aschoeni
** Added PassMsg API to sb to support sequence count preservation
** Revision 1.8 2009/07/17 18:01:30EDT aschoeni
** Updated MsgMap and associated variables to be CFE_SB_MsgId_t
** Revision 1.7 2009/06/26 17:02:05EDT aschoeni
** Updated SB to use __func__ instead of __FILE__ for lock and unlock errors
** Revision 1.6 2009/05/06 09:34:35EDT rmcgraw
** DCR5801:12 Removed unused function prototype GetRoutingPtr
** Revision 1.5 2009/02/11 14:23:04EST rmcgraw
** DCR6269:1 Removed 'Buf' in mem pool names.
** Revision 1.4 2009/02/06 11:29:06EST rmcgraw
** DCR5801:2 General Cleanup
** Revision 1.3 2009/02/03 11:07:00EST rmcgraw
** DCR5801:2 Changed destination desciptors from array based to linked list
** Revision 1.2 2009/01/30 10:37:57EST rmcgraw
** DCR5801:1 Removed function prototype CFE_SB_GetNumberOfSubscribers and added
** destinations to CFE_SB_RouteEntry_t
** Revision 1.1 2008/04/17 08:05:32EDT ruperera
** Initial revision
** Member added to cfe project on tlserver3
** Revision 1.63 2007/09/13 09:36:44EDT rjmcgraw
** DCR4861 New prototype for RequestToSendEvent, new array in sb_t
** Revision 1.62 2007/09/04 16:22:01EDT rjmcgraw
** Renamed #define to CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER
** Revision 1.61 2007/07/12 16:48:28EDT rjmcgraw
** DCR4680:1 Added AppId to cfe_sb_t struct, removed sb event log related items
** Revision 1.60 2007/07/06 15:59:24EDT rjmcgraw
** DCR469:1 Added member 'Sender' to buffer descriptor struct
** Revision 1.59 2007/05/03 11:44:07EDT rjmcgraw
** Added prototype for CFE_SB_CleanUpApp
** Revision 1.58 2007/05/01 09:43:35EDT rjmcgraw
** DCR3052:7 Removed function proto for SendNetworkInfo
** Revision 1.57 2007/04/27 14:55:19EDT rjmcgraw
** DCR2987:4 Standardize app init error code
**
******************************************************************************/

#ifndef _cfe_sb_priv_
#define _cfe_sb_priv_

/*
** Includes
*/
#include "common_types.h"
#include "private/cfe_private.h"
#include "cfe_sb.h"
#include "cfe_sb_msg.h"
#include "cfe_time.h"
#include "cfe_es.h"

/*
** Macro Definitions
*/

#define CFE_SB_AVAILABLE                0xFFFF
#define CFE_SB_NO_ROUTING_IDX           0xFFFF
#define CFE_SB_INVALID_MSG_ID           0xFFFF
#define CFE_SB_UNUSED_QUEUE             0xFFFF
#define CFE_SB_INVALID_PIPE             0xFF
#define CFE_SB_NO_DESTINATION           0xFF
#define CFE_SB_FAILED                   1
#define SB_DONT_CARE                    0

#define CFE_SB_NO_DUPLICATE             0
#define CFE_SB_DUPLICATE                1

#define CFE_SB_INACTIVE                 0
#define CFE_SB_ACTIVE                   1

#define CFE_SB_GLOBAL                   0
#define CFE_SB_LOCAL                    1

#define CFE_SB_TLM                      0
#define CFE_SB_CMD                      1

#define CFE_SB_SEND_ZEROCOPY            0
#define CFE_SB_SEND_ONECOPY             1

#define CFE_SB_NOT_IN_USE               0
#define CFE_SB_IN_USE                   1

#define CFE_SB_DISABLE                  0
#define CFE_SB_ENABLE                   1

#define CFE_SB_DENIED                   0
#define CFE_SB_GRANTED                  1

#define CFE_SB_DO_NOT_INCREMENT         0
#define CFE_SB_INCREMENT_TLM            1

#define CFE_SB_MAIN_LOOP_ERR_DLY        1000
#define CFE_SB_CMD_PIPE_DEPTH           32
#define CFE_SB_CMD_PIPE_NAME            "SB_CMD_PIPE"
#define CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER     8

#define CFE_SB_QOS_LOW_PRIORITY         0
#define CFE_SB_QOS_LOW_RELIABILITY      0

#define CFE_SB_PIPE_OVERFLOW            (-1)
#define CFE_SB_PIPE_WR_ERR              (-2)
#define CFE_SB_USECNT_ERR               (-3)
#define CFE_SB_FILE_IO_ERR              (-5)

/* bit map for stopping recursive event problem */
#define CFE_SB_SEND_NO_SUBS_EID_BIT     0
#define CFE_SB_GET_BUF_ERR_EID_BIT      1
#define CFE_SB_MSGID_LIM_ERR_EID_BIT    2
#define CFE_SB_Q_FULL_ERR_EID_BIT       3
#define CFE_SB_Q_WR_ERR_EID_BIT         4

/*
** Type Definitions
*/

/******************************************************************************
**  Typedef:  CFE_SB_BufferD_t
**
**  Purpose:
**     This structure defines a BUFFER DESCRIPTOR used to specify the MsgId
**     and address of each packet buffer.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     CFE_SB_MsgId_t    MsgId;
     uint16            UseCount;
     uint32            Size;
     void              *Buffer;
     CFE_SB_SenderId_t Sender;
} CFE_SB_BufferD_t;


/******************************************************************************
**  Typedef:  CFE_SB_DestinationD_t
**
**  Purpose:
**     This structure defines a DESTINATION DESCRIPTOR used to specify
**     each destination pipe for a message.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     CFE_SB_PipeId_t PipeId;
     uint8           Active;
     uint16          MsgId2PipeLim;
     uint16          BuffCount;
     uint16          DestCnt;
     uint8           Scope;
     uint8           Spare[3];
     void            *Prev;
     void            *Next;
} CFE_SB_DestinationD_t;


/******************************************************************************
**  Typedef:  CFE_SB_ZeroCopyD_t
**
**  Purpose:
**     This structure defines a ZERO COPY BUFFER DESCRIPTOR used to specify
**     the buffer provided to a requestor.
**
**     Note: Changing the size of this structure may require the memory pool
**     block sizes to change.
*/

typedef struct {
     uint32            AppID;
     uint32            Size;
     void              *Buffer;
     void              *Next;
     void              *Prev;
} CFE_SB_ZeroCopyD_t;


/******************************************************************************
**  Typedef:  CFE_SB_RouteEntry_t
**
**  Purpose:
**     This structure defines an entry in the routing table
*/

typedef struct {
     CFE_SB_MsgId_t        MsgId;
     uint16                Destinations;
     uint32                SeqCnt;
     CFE_SB_DestinationD_t *ListHeadPtr;
} CFE_SB_RouteEntry_t;


/******************************************************************************
**  Typedef:  CFE_SB_PipeD_t
**
**  Purpose:
**     This structure defines a pipe descriptor used to specify the
**     characteristics and status of a pipe.
*/

typedef struct {
     uint8              InUse;
     CFE_SB_PipeId_t    PipeId;
     char               PipeName[OS_MAX_API_NAME];
     char               AppName[OS_MAX_API_NAME];
     uint16             Spare;
     uint32             AppId;
     uint32             SysQueueId;
     uint32             LastSender;
     uint16             QueueDepth;
     uint16             SendErrors;
     CFE_SB_BufferD_t  *CurrentBuff;
     CFE_SB_BufferD_t  *ToTrashBuff;
} CFE_SB_PipeD_t;



/******************************************************************************
**  Typedef:  CFE_SB_BufParams_t
**
**  Purpose:
**     This structure defines the variables related to the SB routing buffers.
*/
typedef struct {

   CFE_ES_MemHandle_t PoolHdl;
   uint8              Partition[CFE_SB_BUF_MEMORY_BYTES];

} CFE_SB_MemParams_t;


/******************************************************************************
**  Typedef:  cfe_sb_t
**
**  Purpose:
**     This structure contains the SB global variables.
*/
typedef struct {
    uint32              SharedDataMutexId;
    uint32              SubscriptionReporting;
    uint32              SenderReporting;
    uint32              AppId;
    uint32              StopRecurseFlags[CFE_ES_MAX_APPLICATIONS];
    void               *ZeroCopyTail;
    CFE_SB_PipeD_t      PipeTbl[CFE_SB_MAX_PIPES];
    CFE_SB_HKMsg_t      HKTlmMsg;
    CFE_SB_StatMsg_t    StatTlmMsg;
    CFE_SB_PipeId_t     CmdPipe;
    CFE_SB_Msg_t        *CmdPipePktPtr;
    CFE_SB_MemParams_t  Mem;
    CFE_SB_MsgId_t      MsgMap[CFE_SB_HIGHEST_VALID_MSGID + 1];
    CFE_SB_RouteEntry_t RoutingTbl[CFE_SB_MAX_MSG_IDS];
    CFE_SB_PrevSubMsg_t PrevSubMsg;
    CFE_SB_SubRprtMsg_t SubRprtMsg;
    CFE_EVS_BinFilter_t EventFilters[CFE_SB_MAX_CFG_FILE_EVENTS_TO_FILTER];
}cfe_sb_t;


/******************************************************************************
**  Typedef:  CFE_SB_SendErrEventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct{
  uint32            EventId;
  int32             ErrStat;
  CFE_SB_PipeId_t   PipeId;
}CFE_SB_SendErrEventBuf_t;


/******************************************************************************
**  Typedef:  CFE_SB_EventBuf_t
**
**  Purpose:
**     This structure is used to store event information during a send.
*/
typedef struct{
  uint32    EvtsToSnd;
  CFE_SB_SendErrEventBuf_t  EvtBuf[CFE_SB_MAX_DEST_PER_PKT];
}CFE_SB_EventBuf_t;


/*
** Software Bus Function Prototypes
*/

int32  CFE_SB_AppInit(void);
int32  CFE_SB_InitBuffers(void);
void   CFE_SB_InitPipeTbl(void);
void   CFE_SB_InitMsgMap(void);
void   CFE_SB_InitRoutingTbl(void);
void   CFE_SB_ResetCounts(void);
uint16 CFE_SB_GetAvailRoutingIdx(void);
void   CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber);
void   CFE_SB_ReleaseBuffer (CFE_SB_BufferD_t *bd, CFE_SB_DestinationD_t *dest);
int32  CFE_SB_ReadQueue(CFE_SB_PipeD_t *pd,uint32 TskId,
                        CFE_SB_TimeOut_t time_out,CFE_SB_BufferD_t **message );
int32  CFE_SB_WriteQueue(CFE_SB_PipeD_t *pd,uint32 TskId,
                         const CFE_SB_BufferD_t *bd,CFE_SB_MsgId_t MsgId );
CFE_SB_MsgId_t CFE_SB_GetRoutingTblIdx(CFE_SB_MsgId_t MsgId);
uint8  CFE_SB_GetPipeIdx(CFE_SB_PipeId_t PipeId);
int32  CFE_SB_ReturnBufferToPool(CFE_SB_BufferD_t *bd);
void   CFE_SB_ProcessCmdPipePkt(void);
int32  CFE_SB_DuplicateSubscribeCheck(CFE_SB_MsgId_t MsgId,CFE_SB_PipeId_t PipeId);
int32  CFE_SB_SetRoutingTblIdx(CFE_SB_MsgId_t MsgId,CFE_SB_MsgId_t Value);
void   CFE_SB_SendHKTlmPkt(void);
void   CFE_SB_ResetCounters(void);
char   *CFE_SB_GetPipeName(CFE_SB_PipeId_t PipeId);
void   CFE_SB_SetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr,uint32 Count);
void   CFE_SB_SendStats(void);
void   CFE_SB_EnableRoute(CFE_SB_MsgPayloadPtr_t Payload);
void   CFE_SB_DisableRoute(CFE_SB_MsgPayloadPtr_t Payload);
char   *CFE_SB_GetAppTskName(uint32 TaskId, char* FullName);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromPool(CFE_SB_MsgId_t MsgId, uint16 size);
CFE_SB_BufferD_t *CFE_SB_GetBufferFromCaller(CFE_SB_MsgId_t MsgId, void *Address);
CFE_SB_PipeD_t   *CFE_SB_GetPipePtr(CFE_SB_PipeId_t PipeId);
CFE_SB_PipeId_t  CFE_SB_GetAvailPipeIdx(void);
CFE_SB_DestinationD_t *CFE_SB_GetDestPtr (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId,uint32 AppId);
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,uint32 AppId);
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t   MsgId,
                           CFE_SB_PipeId_t  PipeId,
                           CFE_SB_Qos_t     Quality,
                           uint16           MsgLim,
                           uint8            Scope);

int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                                   uint32 AppId);

int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                              uint8 Scope, uint32 AppId);
int32  CFE_SB_SendMsgFull(CFE_SB_Msg_t   *MsgPtr, uint32 TlmCntIncrements, uint32 CopyMode);
int32 CFE_SB_SendRtgInfo(const char *Filename);
int32 CFE_SB_SendPipeInfo(const char *Filename);
int32 CFE_SB_SendMapInfo(const char *Filename);
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Msg_t *Ptr2Release, CFE_SB_ZeroCopyHandle_t BufferHandle);
int32 CFE_SB_ZeroCopyReleaseAppId(uint32         AppId);
int32 CFE_SB_DecrBufUseCnt(CFE_SB_BufferD_t *bd);
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId);
int32 CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId);
int32 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId);
void CFE_SB_ProcessSendRtgInfoCmd(CFE_SB_MsgPayloadPtr_t Payload);
void CFE_SB_ProcessSendPipeInfoCmd(CFE_SB_MsgPayloadPtr_t Payload);
void CFE_SB_ProcessSendMapInfoCmd(CFE_SB_MsgPayloadPtr_t Payload);
void CFE_SB_SendPrevSubs(void);
void CFE_SB_IncrCmdCtr(int32 status);
void CFE_SB_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual);
void CFE_SB_SetSubscriptionReporting(uint32 state);
uint32 CFE_SB_FindGlobalMsgIdCnt(void);
uint32 CFE_SB_RequestToSendEvent(uint32 TaskId, uint32 Bit);
void CFE_SB_FinishSendEvent(uint32 TaskId, uint32 Bit);
CFE_SB_DestinationD_t *CFE_SB_GetDestinationBlk(void);
int32 CFE_SB_PutDestinationBlk(CFE_SB_DestinationD_t *Dest);
int32 CFE_SB_AddDest(uint16 RtgTblIdx, CFE_SB_DestinationD_t *Dest);
int32 CFE_SB_RemoveDest(uint16 RtgTblIdx, CFE_SB_DestinationD_t *Dest);

/*
 * External variables private to the software bus module
 */

extern cfe_sb_t CFE_SB;

#endif /* _cfe_sb_priv_ */
/*****************************************************************************/
