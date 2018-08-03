/************************************************************************
** File:
**   $Id: cf_app.h 1.16.1.1 2015/03/06 15:30:47EST sstrege Exp  $
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
**  The CFS CF Application header file
**
** Notes:
**
** $Log: cf_app.h  $
** Revision 1.16.1.1 2015/03/06 15:30:47EST sstrege 
** Added copyright information
** Revision 1.16 2011/05/19 15:32:07EDT rmcgraw 
** DCR15033:1 Add auto suspend processing
** Revision 1.15 2011/05/17 15:52:48EDT rmcgraw 
** DCR14967:5 Message ptr made consistent across all cmds.
** Revision 1.14 2011/05/10 17:04:33EDT rmcgraw 
** DCR14534:1 Changed incoming PDU processing
** Revision 1.13 2010/11/01 16:09:36EDT rmcgraw 
** DCR12802:1 Changes for decoupling peer entity id from channel
** Revision 1.12 2010/10/20 16:07:03EDT rmcgraw 
** DCR13054:1 Expanded max event filters at startup from four to eight
** Revision 1.11 2010/08/04 15:17:40EDT rmcgraw 
** DCR11510:1 Changes prior to release
** Revision 1.10 2010/07/20 14:37:43EDT rmcgraw 
** Dcr11510:1 Remove Downlink buffer references
** Revision 1.9 2010/07/07 17:34:47EDT rmcgraw 
** DCR11510:1 Removed TmpPath references and some ptototypes to cmds.h
** Revision 1.8 2010/06/17 10:04:45EDT rmcgraw 
** DCR11510:1 Change PB In Progress logic to include Data Blast Trans Num
** Revision 1.7 2010/06/11 16:13:17EDT rmcgraw 
** DCR11510:1 ZeroCopy, Un-hardcoded cmd/tlm input/output pdus
** Revision 1.6 2010/05/24 14:05:45EDT rmcgraw 
** Dcr11510:1 Added CheckForTableRequests
** Revision 1.5 2010/04/23 08:39:17EDT rmcgraw 
** Dcr11510:1 Code Review Prep
** Revision 1.4 2010/03/26 15:29:11EDT rmcgraw 
** DCR11510 Added preserve in queue entry spare slot
** Revision 1.3 2010/03/12 12:14:49EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:07:57EST rmcgraw 
** DCR10350:3 Added cfg pkt to AppData and two function prototypes
** Revision 1.1 2009/11/24 12:48:50EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_app_h_
#define _cf_app_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cf_msg.h"
#include "cf_defs.h"
#include "cf_tbldefs.h"
#include "cfdp_config.h"
#include "cf_platform_cfg.h"
#include "cfdp_data_structures.h"



/************************************************************************
** Type Definitions
*************************************************************************/
typedef struct
{
    uint8   Octet1;
    uint16  PDataLen;
    uint8   Octet4;
    uint16  SrcEntityId;
    uint32  TransSeqNum;
    uint16  DstEntityId;
    
}OS_PACK CF_PDU_Hdr_t;


typedef struct
{
    void        *Prev;
    void        *Next;
    uint8       Priority;
    uint8       Class;
    uint8       ChanNum;
    uint8       Source;/* from poll dir,playbackfile cmd or playback dir cmd */
    uint8       NodeType;/* Incoming Trans or Outgoing Trans */
    uint8       CondCode;
    uint8       Status;
    uint8       Preserve;
    uint32      TransNum;
    uint8       Warning;
    char        SrcEntityId[CF_MAX_CFG_VALUE_CHARS];
    char        PeerEntityId[CF_MAX_CFG_VALUE_CHARS];    
    char        SrcFile[OS_MAX_PATH_LEN];
    char        DstFile[OS_MAX_PATH_LEN];
    
}CF_QueueEntry_t;

typedef struct
{
    CF_QueueEntry_t     *HeadPtr;
    CF_QueueEntry_t     *TailPtr;
    uint32              EntryCnt;

}CF_Queue_t;


typedef struct
{
    CF_Queue_t          PbQ[CF_QUEUES_PER_CHAN];
    uint32              HandshakeSemId;
    uint32              PendQTimer;
    uint32              PollDirTimer;
    uint32              DataBlast;
    uint32              TransNumBlasting;
    CFE_SB_ZeroCopyHandle_t ZeroCpyHandle;
    CFE_SB_Msg_t            *ZeroCpyMsgPtr;
    
}CF_ChannelData_t;


typedef struct {

   /*CFE_ES_MemHandle_t PoolHdl;*/
   uint32           PoolHdl;
   uint8            Partition[CF_MEMORY_POOL_BYTES];

} CF_MemParams_t;

/** 
**  \brief CF global data structure
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet...
    */
    CF_HkPacket_t 	    Hk;/**< \brief CF Housekeeping Packet */
    CF_TransPacket_t    Trans;/**< \brief CF Transaction Packet */

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_MsgPtr_t         MsgPtr;/**< \brief Pointer to msg received on software bus */
    uint32                  RunStatus;

    CFE_SB_PipeId_t         CmdPipe;/**< \brief Pipe Id for CF command pipe */ 
    uint8					Spare[3];/**< \brief Spare byte for alignment */
            
    CFE_TBL_Handle_t        ConfigTableHandle;/**< \brief Config Table handle */
    cf_config_table_t       *Tbl;/**< \brief Ptr to config table */
            
    CF_Queue_t              UpQ[CF_NUM_UPLINK_QUEUES];
    CF_ChannelData_t        Chan[CF_MAX_PLAYBACK_CHANNELS];
     
    CF_MemParams_t          Mem;
    CF_ConfigPacket_t       CfgPkt;
    CFDP_DATA               RawPduInputBuf;
    CFE_EVS_BinFilter_t     EventFilters[CF_MAX_EVENT_FILTERS];

} CF_AppData_t;


typedef struct
{
    uint8   Chan;
    uint8   Class;
    uint8   Priority;
    uint8   Preserve;
    uint8   CmdOrPoll;
    char    PeerEntityId[CF_MAX_CFG_VALUE_CHARS];
    char    SrcPath[OS_MAX_PATH_LEN];
    char    DstPath[OS_MAX_PATH_LEN];

} CF_QueueDirFiles_t;


/*************************************************************************
** Exported data
**************************************************************************/
extern CF_AppData_t             CF_AppData;/**< \brief */

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS CF application entry point
**  
**  \par Description
**       CF application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
*************************************************************************/
void CF_AppMain(void);


/************************************************************************
** Prototypes for functions defined in cf_app.c
*************************************************************************/
/************************************************************************/
/** \brief Initialize the housekeeping application
**  
**  \par Description
**       Housekeeping application initialization routine. This 
**       function performs all the required startup steps to 
**       get the application registered with the cFE services so
**       it can begin to receive command messages. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register         \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_AppInit (void);


/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the command.
**       
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message 
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void CF_AppPipe (CFE_SB_MsgPtr_t MessagePtr);


int32 CF_TableInit (void);
int32 CF_ChannelInit(void);
void CF_WakeupProcessing(CFE_SB_MsgPtr_t MessagePtr);
void CF_GetHandshakeSemIds(void);
void CF_CheckForTblRequests(void);
int32 CF_ValidateCFConfigTable (void * TblPtr);
void CF_SendPDUToEngine(CFE_SB_MsgPtr_t MessagePtr);
int32 CF_MsgIdMatchesInputChannel( CFE_SB_MsgId_t  MessageID);


#endif /* _cf_app_ */

/************************/
/*  End of File Comment */
/************************/
