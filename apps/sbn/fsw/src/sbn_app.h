/******************************************************************************
** File: sbn_app.h
**
**      Copyright (c) 2004-2006, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This software may be used only pursuant to a United States government
**      sponsored project and the United States government may not be charged
**      for use thereof.
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for the Software Bus Network Application.
**
** Authors:   J. Wilmot/GSFC Code582
**            R. McGraw/SSI
**
** $Log: sbn_app.h  $
** Revision 1.3 2010/10/05 15:24:14EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2008/04/08 08:07:10EDT ruperera 
** Member moved from sbn_app.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sbn/cfs_sbn.pj to sbn_app.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sbn/fsw/src/project.pj.
** Revision 1.1 2008/04/08 07:07:10ACT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sbn/cfs_sbn.pj
** Revision 1.1 2007/11/30 17:18:03EST rjmcgraw 
** Initial revision
** Member added to project d:/mksdata/MKS-CFS-REPOSITORY/sbn/cfs_sbn.pj
** Revision 1.14 2007/04/19 15:52:53EDT rjmcgraw 
** DCR3052:7 Moved and renamed CFE_SB_MAX_NETWORK_PEERS to this file
** Revision 1.13 2007/03/13 13:18:50EST rjmcgraw 
** Added #define SBN_ITEMS_PER_FILE_LINE
** Revision 1.12 2007/03/07 11:07:23EST rjmcgraw 
** Removed references to channel
** Revision 1.11 2007/02/28 15:05:03EST rjmcgraw 
** Removed unused #defines
** Revision 1.10 2007/02/27 09:37:42EST rjmcgraw 
** Minor cleanup
** Revision 1.9 2007/02/23 14:53:41EST rjmcgraw 
** Moved subscriptions from protocol port to data port
** Revision 1.8 2007/02/22 15:15:50EST rjmcgraw 
** Changed internal structs to exclude mac address
** Revision 1.7 2007/02/21 14:49:39EST rjmcgraw 
** Debug events changed to OS_printfs
**
******************************************************************************/


#ifndef _sbn_app_
#define _sbn_app_

#include "osconfig.h"
#include "cfe.h"
#include "sbn_interfaces.h"
#include "sbn_msg.h"
#include "sbn_platform_cfg.h"
#include "cfe_sb_msg.h"
#include "cfe_sb.h"

typedef struct {
  SBN_InterfaceData IfData[SBN_MAX_NETWORK_PEERS*2];  /* Data on all devices in the peer file (allow a host for every peer) */
  SBN_InterfaceData *Host[SBN_MAX_NETWORK_PEERS];   /* Data only on devices that are the host */
  SBN_PeerData_t    Peer[SBN_MAX_NETWORK_PEERS];    /* Data only no devices that are not the host */
  uint32            AppId;
  char              App_FullName[(OS_MAX_API_NAME * 2)];
  int               NumPeers;
  int               NumHosts;
  int               NumEntries;
  CFE_SB_PipeId_t   SubPipe;
  CFE_SB_PipeId_t   CmdPipe;
  CFE_SB_MsgPtr_t   CmdMsgPtr;

  int               LocalSubCnt;
  SBN_Subs_t        LocalSubs[SBN_MAX_SUBS_PER_PEER + 1];

  /* CFE scheduling pipe */
  CFE_SB_PipeId_t  SchPipeId;
  uint16           usSchPipeDepth;
  char             cSchPipeName[OS_MAX_API_NAME];

  SBN_InterfaceOperations *IfOps[SBN_MAX_INTERFACE_TYPES + 1];

  SBN_HkPacket_t   HkPkt;

}sbn_t;

sbn_t SBN;

/*
** Prototypes
*/
void SBN_AppMain(void);
int SBN_CreatePipe4Peer(int PeerIdx);
void SBN_ProcessNetMsg(SBN_MsgType_t MsgType, SBN_CpuId_t CpuId,
    SBN_MsgSize_t MsgSize, void *Msg);
int SBN_GetPeerIndex(uint32 ProcessorId);

/* define to turn on debug event messages, warning, chatty! */
#undef SBN_DEBUG_MSGS

#ifdef SBN_DEBUG_MSGS
#define DEBUG_MSG(...) CFE_EVS_SendEvent(SBN_DEBUG_EID, CFE_EVS_DEBUG, __VA_ARGS__)
#define DEBUG_START() CFE_EVS_SendEvent(SBN_DEBUG_EID, CFE_EVS_DEBUG, "%s starting", __FUNCTION__)
#else /* !SBN_DEBUG_MSGS */
#define DEBUG_START() ;
#define DEBUG_MSG(...) ;
#endif /* SBN_DEBUG_MSGS */


#endif /* _sbn_app_ */
/*****************************************************************************/
