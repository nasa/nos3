/******************************************************************************
** File: sbn_netif.h
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
** $Log: sbn_netif.h  $
**
******************************************************************************/


#ifndef _sbn_netif_
#define _sbn_netif_

#include "cfe.h"

void  SBN_ShowPeerData(void);
int32 SBN_GetPeerFileData(void);

int SBN_InitPeerInterface(void);
int SBN_CheckForNetProtoMsg(int PeerIdx);
void inline SBN_ProcessNetAppMsgsFromHost(int HostIdx);
void  SBN_CheckForNetAppMsgs(void);
void  SBN_VerifyPeerInterfaces(void);
void  SBN_VerifyHostInterfaces(void);

int SBN_SendNetMsg(SBN_MsgType_t MsgType, SBN_MsgSize_t MsgSize, void *Msg,
    int PeerIdx);

uint8 SBN_GetReliabilityFromQoS(uint8 QoS);
uint8 SBN_GetPriorityFromQoS(uint8 QoS);
uint8 SBN_GetPeerQoSReliability(const SBN_PeerData_t * peer);
uint8 SBN_GetPeerQoSPriority(const SBN_PeerData_t * peer);

extern sbn_t  SBN;

#endif /* _sbn_netif_ */
/*****************************************************************************/
