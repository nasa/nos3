/*
**
** File: ut_cfe_psp_eeprom_stubs.h
**
** Purpose: cFE PSP EEPROM Header file for unit test stubs
**
** $Log: ut_cfe_psp_eeprom_stubs.h  $
** Revision 1.2 2016/06/22 18:39:56EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/04/10 21:16:05EDT czogby 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_EEPROM_STUBS_H_
#define UT_CFE_PSP_EEPROM_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_EEPROM_WRITEENABLE_INDEX,
    UT_CFE_PSP_EEPROM_WRITEDISABLE_INDEX,
    UT_CFE_PSP_EEPROM_WRITE8_INDEX,
    UT_CFE_PSP_EEPROM_WRITE16_INDEX,
    UT_CFE_PSP_EEPROM_WRITE32_INDEX,
    UT_CFE_PSP_EEPROM_MAX_INDEX
} Ut_CFE_PSP_EEPROM_INDEX_t;

typedef struct
{
    int32 (*CFE_PSP_EepromWriteEnable)(uint32 Bank);
    int32 (*CFE_PSP_EepromWriteDisable)(uint32 Bank);
    int32 (*CFE_PSP_EepromWrite8)(cpuaddr MemoryAddress, uint8 ByteValue);
    int32 (*CFE_PSP_EepromWrite16)(cpuaddr MemoryAddress, uint16 uint16Value);
    int32 (*CFE_PSP_EepromWrite32)(cpuaddr MemoryAddress, uint32 uint32Value);
} Ut_CFE_PSP_EEPROM_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_EEPROM_ReturnCodeTable_t;

void Ut_CFE_PSP_EEPROM_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_EEPROM_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_EEPROM_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
