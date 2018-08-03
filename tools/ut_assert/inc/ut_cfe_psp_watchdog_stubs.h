/*
**
** File: ut_cfe_psp_watchdog_stubs.h
**
** Purpose: cFE PSP Watchdog Header file for unit test stubs
**
** $Log: ut_cfe_psp_watchdog_stubs.h  $
** Revision 1.1 2016/04/10 21:16:05EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_WATCHDOG_STUBS_H_
#define UT_CFE_PSP_WATCHDOG_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_WATCHDOG_SET_INDEX,
    UT_CFE_PSP_WATCHDOG_SERVICE_INDEX,
    UT_CFE_PSP_WATCHDOG_ENABLE_INDEX,
    UT_CFE_PSP_WATCHDOG_MAX_INDEX
} Ut_CFE_PSP_WATCHDOG_INDEX_t;

typedef struct
{
    void  (*CFE_PSP_WatchdogSet)(uint32 WatchdogValue);
    void  (*CFE_PSP_WatchdogService)(void);
    void  (*CFE_PSP_WatchdogEnable)(void);
} Ut_CFE_PSP_WATCHDOG_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_WATCHDOG_ReturnCodeTable_t;

void Ut_CFE_PSP_WATCHDOG_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_WATCHDOG_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_WATCHDOG_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
