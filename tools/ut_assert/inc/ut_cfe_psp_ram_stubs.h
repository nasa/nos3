/*
**
** File: ut_cfe_psp_ram_stubs.h
**
** Purpose: cFE PSP RAM Header file for unit test stubs
**
** $Log: ut_cfe_psp_ram_stubs.h  $
** Revision 1.2 2016/06/22 18:40:24EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/04/10 21:16:05EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_RAM_STUBS_H_
#define UT_CFE_PSP_RAM_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_RAM_MEMREAD8_INDEX,
    UT_CFE_PSP_RAM_MEMREAD16_INDEX,
    UT_CFE_PSP_RAM_MEMREAD32_INDEX,
    UT_CFE_PSP_RAM_MEMWRITE8_INDEX,
    UT_CFE_PSP_RAM_MEMWRITE16_INDEX,
    UT_CFE_PSP_RAM_MEMWRITE32_INDEX,
    UT_CFE_PSP_RAM_MAX_INDEX
} Ut_CFE_PSP_RAM_INDEX_t;

typedef struct
{
    int32 (*CFE_PSP_MemRead8)(cpuaddr MemoryAddress, uint8 *ByteValue);
    int32 (*CFE_PSP_MemRead16)(cpuaddr MemoryAddress, uint16 *uint16Value);
    int32 (*CFE_PSP_MemRead32)(cpuaddr MemoryAddress, uint32 *uint32Value);
    int32 (*CFE_PSP_MemWrite8)(cpuaddr MemoryAddress, uint8 ByteValue);
    int32 (*CFE_PSP_MemWrite16)(cpuaddr MemoryAddress, uint16 uint16Value);
    int32 (*CFE_PSP_MemWrite32)(cpuaddr MemoryAddress, uint32 uint32Value);
} Ut_CFE_PSP_RAM_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_RAM_ReturnCodeTable_t;

void Ut_CFE_PSP_RAM_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_RAM_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_RAM_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
