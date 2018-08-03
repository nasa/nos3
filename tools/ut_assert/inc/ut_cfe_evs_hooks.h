/*
**
** File: ut_cfe_evs_hooks.h
**
** $Id: ut_cfe_evs_hooks.h 1.2 2016/06/22 18:39:55EDT czogby Exp  $
**
** Copyright 2012-2013 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: Unit test header file for cFE Event Services hooks.
**
** $Log: ut_cfe_evs_hooks.h  $
** Revision 1.2 2016/06/22 18:39:55EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.5 2015/06/16 16:02:11EDT sstrege 
** Added copyright information
** Revision 1.4 2015/03/10 15:19:31EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  sstrege [2015/03/27 21:10:18Z]
** No source code updates were made in this revision
** Revision 1.3 2011/03/10 11:16:45EST sslegel 
** Added EventNotSent and PacketNotSent asserts
** Revision 1.2 2011/02/16 17:06:44EST rmcgraw 
** Added "extern UtListHead_t EventQueue;" to ut_cfe_evs_hooks.h
** Revision 1.1 2011/02/15 11:12:32EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_EVS_HOOKS_H_
#define UT_CFE_EVS_HOOKS_H_

#include "cfe.h"
#include "utassert.h"
#include "utlist.h"

extern UtListHead_t        EventQueue;

#define     UtAssert_EventSent(EventID, EventType, EventText, Description) \
                UtAssert(Ut_CFE_EVS_EventSent(EventID, EventType, EventText), Description, __FILE__, __LINE__)

#define     UtAssert_EventNotSent(EventID, EventType, EventText, Description) \
                UtAssert(Ut_CFE_EVS_EventSent(EventID, EventType, EventText) == FALSE, Description, __FILE__, __LINE__)

#define     UtAssert_NoEventSent(Description) \
                UtAssert(UtList_IsEmpty(&EventQueue), Description, __FILE__, __LINE__)

/****************/
/* ITC addition */
#define     UtAssert_EventIDSent(EventID, EventType, Description) \
                UtAssert(Ut_CFE_EVS_EventIDSent(EventID, EventType), Description, __FILE__, __LINE__)
boolean     Ut_CFE_EVS_EventIDSent(uint16 EventID, uint16 EventType);
/****************/

void        Ut_CFE_EVS_ClearEventQueue(void);
uint32      Ut_CFE_EVS_GetEventQueueDepth(void);
uint32      Ut_CFE_EVS_GetEventCount(uint16 EventID, uint16 EventType, const char *EventText);
int32       Ut_CFE_EVS_SendEventHook(uint16 EventID, uint16 EventType, const char *EventText);
boolean     Ut_CFE_EVS_EventSent(uint16 EventID, uint16 EventType, const char *EventText);

#endif
