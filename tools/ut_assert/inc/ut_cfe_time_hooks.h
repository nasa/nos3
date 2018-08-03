/*
**
** File: ut_cfe_time_hooks.h
**
** $Id: ut_cfe_time_hooks.h 1.1 2016/02/08 20:53:28EST rperera Exp  $
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
** Purpose: Unit test hooks for cFE Time routines
**
** $Log: ut_cfe_time_hooks.h  $
** Revision 1.1 2016/02/08 20:53:28EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.3 2015/06/16 16:02:13EDT sstrege 
** Added copyright information
** Revision 1.2 2015/05/01 16:40:55EDT czogby 
** Added CFE_TIME_Print and CFE_TIME_Compare
** Revision 1.1 2015/04/03 17:41:37EDT czogby 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_TIME_HOOKS_H_
#define UT_CFE_TIME_HOOKS_H_

#include "cfe.h"

CFE_TIME_SysTime_t  Ut_CFE_TIME_AddHook (CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
CFE_TIME_SysTime_t  Ut_CFE_TIME_SubtractHook (CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
uint32              Ut_CFE_TIME_Sub2MicroSecsHook (uint32 SubSeconds);
uint32              Ut_CFE_TIME_Micro2SubSecsHook (uint32 MicroSeconds);
void                Ut_CFE_TIME_PrintHook(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint);
CFE_TIME_Compare_t  Ut_CFE_TIME_CompareHook(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB);


#endif
