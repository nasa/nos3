/*
**
** File: ut_cfe_time_stubs.h
**
** $Id: ut_cfe_time_stubs.h 1.2 2016/06/22 19:14:17EDT czogby Exp  $
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
** Purpose: cFE Time Services Header file for unit test stubs
**
** $Log: ut_cfe_time_stubs.h  $
** Revision 1.2 2016/06/22 19:14:17EDT czogby 
** UT-Assert: Add Missing Stub Functions
** Revision 1.1 2016/02/08 20:53:28EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.5 2015/06/16 16:02:13EDT sstrege 
** Added copyright information
** Revision 1.4 2015/05/07 14:34:22EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.3 2015/05/01 12:00:52EDT czogby 
** Added CFE_TIME_Print and CFE_TIME_Compare
** Revision 1.2 2015/03/10 15:19:33EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:59:01Z]
** Added new elements to Ut_CFE_TIME_INDEX_t and Ut_CFE_TIME_HookTable_t
** Revision 1.1 2011/02/15 11:12:35EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_TIME_STUBS_H_
#define UT_CFE_TIME_STUBS_H_

typedef enum 
{
    UT_CFE_TIME_GETUTC_INDEX,
    UT_CFE_TIME_GETTAI_INDEX,
    UT_CFE_TIME_GETTIME_INDEX,
    UT_CFE_TIME_SUB2MICROSECS_INDEX,
    UT_CFE_TIME_SUBTRACT_INDEX,
    UT_CFE_TIME_MICRO2SUBSECS_INDEX,
    UT_CFE_TIME_ADD_INDEX,
    UT_CFE_TIME_PRINT_INDEX,
    UT_CFE_TIME_COMPARE_INDEX,
    UT_CFE_TIME_FS2CFESECONDS_INDEX,
    UT_CFE_TIME_REGISTERSYNCHCALLBACK_INDEX,
    UT_CFE_TIME_UNREGISTERSYNCHCALLBACK_INDEX,
    UT_CFE_TIME_MAX_INDEX
} Ut_CFE_TIME_INDEX_t;

typedef struct
{
    CFE_TIME_SysTime_t (*CFE_TIME_GetUTC)(void);
    CFE_TIME_SysTime_t (*CFE_TIME_GetTAI)(void);
    CFE_TIME_SysTime_t (*CFE_TIME_GetTime)(void);
    uint32 (*CFE_TIME_Micro2SubSecs)(uint32 MicroSeconds);
    uint32 (*CFE_TIME_Sub2MicroSecs)(uint32 SubSeconds);
    CFE_TIME_SysTime_t (*CFE_TIME_Add)(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
    CFE_TIME_SysTime_t (*CFE_TIME_Subtract)(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);
    void (*CFE_TIME_Print)(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint);
    CFE_TIME_Compare_t (*CFE_TIME_Compare)(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB);
    uint32 (*CFE_TIME_FS2CFESeconds)(uint32 SecondsFS);
    int32  (*CFE_TIME_RegisterSynchCallback)(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);
    int32  (*CFE_TIME_UnregisterSynchCallback)(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);
} Ut_CFE_TIME_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_TIME_ReturnCodeTable_t;

void Ut_CFE_TIME_Reset(void);
void Ut_CFE_TIME_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_TIME_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_TIME_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
