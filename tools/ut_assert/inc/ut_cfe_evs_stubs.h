/*
**
** File:  ut_cfe_evs_stubs.h
**
** $Id: ut_cfe_evs_stubs.h 1.2 2016/06/22 18:39:56EDT czogby Exp  $
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
** Purpose: cFE Event Services Header file for unit test stubs
**
** $Log: ut_cfe_evs_stubs.h  $
** Revision 1.2 2016/06/22 18:39:56EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.5 2015/06/16 16:02:11EDT sstrege 
** Added copyright information
** Revision 1.4 2015/05/07 14:19:03EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.3 2015/03/17 18:17:21EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:26:53Z]
** Added function prototypes, added a new element in Ut_CFE_EVS_INDEX_t
** .
** Revision 1.2 2015/03/10 15:19:31EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.1 2011/02/15 11:12:33EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_EVS_STUBS_H_
#define UT_CFE_EVS_STUBS_H_

typedef enum 
{
    UT_CFE_EVS_REGISTER_INDEX,
    UT_CFE_EVS_SENDEVENT_INDEX,
    UT_CFE_EVS_SENDTIMEDEVENT_INDEX,
    UT_CFE_EVS_SENDEVENTWITHAPPID_INDEX,
    UT_CFE_EVS_CLEANUPAPP_INDEX,
    UT_CFE_EVS_MAX_INDEX
} Ut_CFE_EVS_INDEX_t;

typedef struct
{
    int32 (*CFE_EVS_Register)(void *Filters, uint16 NumEventFilters, uint16 FilterScheme);
    int32 (*CFE_EVS_SendEvent)(uint16 EventID, uint16 EventType, const char *EventText);
    int32 (*CFE_EVS_SendTimedEvent)(CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, const char *Spec, ... );
    int32 (*CFE_EVS_SendEventWithAppID)(uint16 EventID, uint16 EventType, uint32 AppID, const char *Spec, ... );
    int32 (*CFE_EVS_CleanUpApp)(uint32 AppId);
} Ut_CFE_EVS_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_EVS_ReturnCodeTable_t;

void Ut_CFE_EVS_Reset(void);
void Ut_CFE_EVS_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_EVS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_EVS_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif 
