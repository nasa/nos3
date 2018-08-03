/*
**
** File: ut_cfe_psp_memrange_stubs.h
**
** Purpose: cFE PSP Memory Range Header file for unit test stubs
**
** $Log: ut_cfe_psp_memrange_stubs.h  $
** Revision 1.2 2016/06/22 18:39:56EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/04/10 21:16:05EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_MEMRANGE_STUBS_H_
#define UT_CFE_PSP_MEMRANGE_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX,
    UT_CFE_PSP_MEMRANGE_MAX_INDEX
} Ut_CFE_PSP_MEMRANGE_INDEX_t;

typedef struct
{
    int32 (*CFE_PSP_MemValidateRange)(cpuaddr Address, uint32 Size, uint32 MemoryType);
} Ut_CFE_PSP_MEMRANGE_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_MEMRANGE_ReturnCodeTable_t;

void Ut_CFE_PSP_MEMRANGE_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_MEMRANGE_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_MEMRANGE_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
