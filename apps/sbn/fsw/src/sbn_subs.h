/******************************************************************************
** File: sbn_subs.h
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
**      This header file contains prototypes for private functions related to 
**      handling message subscriptions
**
** Authors:   J. Wilmot/GSFC Code582
**            R. McGraw/SSI
**            E. Timmons/GSFC Code587
**
******************************************************************************/

#ifndef _sbn_subs_h_
#define _sbn_subs_h_

#include "sbn_app.h"
#include "sbn_netif.h"

void  SBN_SendLocalSubsToPeer(int PeerIdx); 
int32 SBN_CheckSubscriptionPipe(void); 
void  SBN_ProcessSubFromPeer(int PeerIdx, void *submsg);
void  SBN_ProcessUnsubFromPeer(int PeerIdx, void *submsg);
void  SBN_ProcessAllSubscriptions(CFE_SB_PrevSubMsg_t *Ptr); 
void  SBN_RemoveAllSubsFromPeer(int PeerIdx); 
void  SBN_SendSubsRequests(void); 

#endif /* _sbn_subs_h_ */
