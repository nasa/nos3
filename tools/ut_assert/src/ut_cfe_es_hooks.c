/*
**
** File: ut_cfe_es_hooks.c
**
** $Id: ut_cfe_es_hooks.c 1.2 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test hooks for cFE Executive Services routines
**
** $Log: ut_cfe_es_hooks.c  $
** Revision 1.2 2016/06/23 14:39:04EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.4 2015/06/16 16:14:00EDT sstrege 
** Added copyright information
** Revision 1.3 2015/05/01 12:17:32EDT czogby 
** Replaced use of CFE_EVS_MAX_MESSAGE_LENGTH with CFE_ES_MAX_SYSLOG_MSG_SIZE
** Revision 1.2 2015/03/10 15:21:44EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:16:41Z]
** Added SysLog functions
** Revision 1.1 2011/03/07 17:54:30EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "utlist.h"
#include "uttools.h"
#include <string.h>

UtListHead_t        SysLogQueue;

void Ut_CFE_ES_ClearSysLogQueue(void)
{
    UtList_Reset(&SysLogQueue);
}

uint32 Ut_CFE_ES_GetSysLogQueueDepth(void)
{
    return(UtList_Depth(&SysLogQueue));
}

int32 Ut_CFE_ES_RunLoopHook(uint32 *ExitStatus)
{
    if (*ExitStatus == CFE_ES_APP_RUN) {
        return(TRUE);
    }
    else { /* CFE_ES_APP_EXIT, CFE_ES_APP_ERROR */
        return(FALSE);
    }
}

int32 Ut_CFE_ES_WriteToSysLog(const char *SysLogEntryText)
{
    char SysLogEntryTextCopy[CFE_ES_MAX_SYSLOG_MSG_SIZE];

    if (strlen(SysLogEntryText) >= CFE_ES_MAX_SYSLOG_MSG_SIZE)
    {
        UtPrintf("WARNING - System Log Message Too Long: %s\n", SysLogEntryText);
    }

    strncpy(SysLogEntryTextCopy, SysLogEntryText, CFE_ES_MAX_SYSLOG_MSG_SIZE);

    UtList_Add(&SysLogQueue, &SysLogEntryTextCopy, CFE_ES_MAX_SYSLOG_MSG_SIZE, 0);

    UtPrintf("SYSTEM LOG: %s\n", SysLogEntryText);

    return CFE_SUCCESS;
}

boolean Ut_CFE_ES_SysLogWritten(const char *ExpectedSysLogEntryText)
{
    UtListNode_t   *CurrentNode;
    char           *SysLogEntryText;

    CurrentNode = UtList_First(&SysLogQueue);
    while (CurrentNode)
    {
        SysLogEntryText = (char *)(CurrentNode->Data);

        if (strncmp(ExpectedSysLogEntryText, SysLogEntryText, CFE_ES_MAX_SYSLOG_MSG_SIZE) == 0)
        {
            return(TRUE);
        }
        CurrentNode = CurrentNode->Next;
    }
    return(FALSE);
}
