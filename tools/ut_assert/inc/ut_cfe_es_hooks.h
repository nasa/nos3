/*
**
** File: ut_cfe_es_hooks.h
**
** $Id: ut_cfe_es_hooks.h 1.2 2016/06/22 18:39:53EDT czogby Exp  $
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
** Purpose: Unit test header file for cFE Executive Services hooks.
**
** $Log: ut_cfe_es_hooks.h  $
** Revision 1.2 2016/06/22 18:39:53EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.3 2015/06/16 12:48:58EDT sstrege 
** Added copyright information
** Revision 1.2 2015/03/10 15:19:30EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/27 19:45:19Z]
** Added function prototypes
** Revision 1.1 2011/03/07 17:54:46EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_ES_HOOKS_H_
#define UT_CFE_ES_HOOKS_H_

#include "cfe.h"

void        Ut_CFE_ES_ClearSysLogQueue(void);
uint32      Ut_CFE_EVS_GetEventQueueDepth(void);
int32       Ut_CFE_ES_RunLoopHook(uint32 *ExitStatus);
int32       Ut_CFE_ES_WriteToSysLog(const char *SysLogEntryText);
boolean     Ut_CFE_ES_SysLogWritten(const char *ExpectedSysLogEntryText);

#endif
