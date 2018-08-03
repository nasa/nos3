/*
**
** File: ut_cfe_psp_timer_stubs.h
**
** Purpose: cFE PSP Timer Header file for unit test stubs
**
** $Log: ut_cfe_psp_timer_stubs.h  $
** Revision 1.1 2016/04/10 21:16:05EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_TIMER_STUBS_H_
#define UT_CFE_PSP_TIMER_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_TIMER_GETTIME_INDEX,
    UT_CFE_PSP_TIMER_MAX_INDEX
} Ut_CFE_PSP_TIMER_INDEX_t;

typedef struct
{
    void  (*CFE_PSP_GetTime)(OS_time_t *LocalTime);
} Ut_CFE_PSP_TIMER_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_TIMER_ReturnCodeTable_t;

void Ut_CFE_PSP_TIMER_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_TIMER_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_TIMER_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
