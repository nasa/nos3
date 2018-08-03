/*
**
** File: ut_cfe_evs_hooks.c
**
** $Id: ut_cfe_evs_hooks.c 1.2 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test hooks for cFE Event Services routines
**
** $Log: ut_cfe_evs_hooks.c  $
** Revision 1.2 2016/06/23 14:39:04EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.5 2015/06/16 16:14:00EDT sstrege 
** Added copyright information
** Revision 1.4 2015/05/01 12:43:16EDT czogby 
** Added UtAssert call that fails to case where an error message is generated
** Revision 1.3 2015/03/10 15:21:45EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:19:44Z]
** Added newline character to event message
** Revision 1.2 2011/03/04 14:56:05EST sslegel 
** Added a event text length check
** Revision 1.1 2011/02/15 11:13:01EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "utassert.h"
#include "utlist.h"
#include "uttools.h"
#include <string.h>

UtListHead_t        EventQueue;

typedef struct {
    uint16      EventID;
    uint16      EventType;
    char        EventText[CFE_EVS_MAX_MESSAGE_LENGTH];
} Ut_CFE_EVS_Event_t;

void Ut_CFE_EVS_ClearEventQueue(void)
{
    UtList_Reset(&EventQueue);
}

uint32 Ut_CFE_EVS_GetEventQueueDepth(void)
{
    return(UtList_Depth(&EventQueue));
}

uint32 Ut_CFE_EVS_GetEventCount(uint16 EventID, uint16 EventType, const char *EventText)
{
    UtListNode_t        *CurrentNode;
    Ut_CFE_EVS_Event_t  *EventMessagePtr;
    uint32               EventCount = 0;

    CurrentNode = UtList_First(&EventQueue);
    while (CurrentNode) {
        EventMessagePtr = CurrentNode->Data;
        if ((EventMessagePtr->EventID == EventID) &&
            (EventMessagePtr->EventType == EventType) &&
            (strncmp(EventText, EventMessagePtr->EventText, strlen(EventText)) == 0)) {
            EventCount++;
        }
        CurrentNode = CurrentNode->Next;
    }
    return(EventCount);
}

int32 Ut_CFE_EVS_SendEventHook(uint16 EventID, uint16 EventType, const char *EventText)
{
    Ut_CFE_EVS_Event_t  EventMessage;

    if (strlen(EventText) >= CFE_EVS_MAX_MESSAGE_LENGTH) {
        UtPrintf("WARNING - Event Message Too Long: %s\n", EventText);
    }

    EventMessage.EventID = EventID;
    EventMessage.EventType = EventType;
    strncpy(&EventMessage.EventText[0], EventText, CFE_EVS_MAX_MESSAGE_LENGTH);
    UtList_Add(&EventQueue, &EventMessage, sizeof(EventMessage), 0);

    if (EventType == CFE_EVS_DEBUG)
        UtPrintf("DEBUG EVENT ID=%d %s\n", EventID, EventText);
    else if (EventType == CFE_EVS_INFORMATION)
        UtPrintf("INFO EVENT ID=%d %s\n", EventID, EventText);
    else if (EventType == CFE_EVS_ERROR)
        UtPrintf("ERROR EVENT ID=%d %s\n", EventID, EventText);
    else if (EventType == CFE_EVS_CRITICAL)
        UtPrintf("CRITICAL EVENT ID=%d %s\n", EventID, EventText);
    else
    {
        UtPrintf("Invalid Event Type %d ID=%d %s\n", EventType, EventID, EventText);
        UtAssert_True(FALSE, "Invalid Event Type");
    }

    return CFE_SUCCESS;
}

boolean Ut_CFE_EVS_EventSent(uint16 EventID, uint16 EventType, const char *EventText)
{
    UtListNode_t        *CurrentNode;
    Ut_CFE_EVS_Event_t  *EventMessagePtr;

    CurrentNode = UtList_First(&EventQueue);
    while (CurrentNode) {
        EventMessagePtr = CurrentNode->Data;
        if ((EventMessagePtr->EventID == EventID) &&
            (EventMessagePtr->EventType == EventType) &&
            (strncmp(EventText, EventMessagePtr->EventText, strlen(EventText)) == 0)) {
            return(TRUE);
        }
        CurrentNode = CurrentNode->Next;
    }
    return(FALSE);
}

boolean Ut_CFE_EVS_EventIDSent(uint16 EventID, uint16 EventType)
{
    UtListNode_t        *CurrentNode;
    Ut_CFE_EVS_Event_t  *EventMessagePtr;

    CurrentNode = UtList_First(&EventQueue);
    while (CurrentNode) {
        EventMessagePtr = CurrentNode->Data;
        if ((EventMessagePtr->EventID == EventID) &&
            (EventMessagePtr->EventType == EventType)) {
            return(TRUE);
        }
        CurrentNode = CurrentNode->Next;
    }
    return(FALSE);
}

