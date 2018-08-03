/*
**
** File: ut_cfe_psp_memutils_stubs.h
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
** Purpose: cFE PSP Memory Utilities Header file for unit test stubs
**
** $Log: ut_cfe_psp_memutils_stubs.h  $
** Revision 1.2 2016/04/10 21:19:33EDT czogby 
** Separate Stub Functions Into Multiple Files Based On Source Filename
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.4 2015/06/16 16:02:12EDT sstrege 
** Added copyright information
** Revision 1.3 2015/05/07 14:30:04EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.2 2015/04/06 12:19:00EDT czogby 
** Added UT_CFE_PSP_MEMUTILS_MAX_INDEX, ReturnCodeTable_t, SetFunctionHook, and SetReturnCode
** Revision 1.1 2015/04/03 17:41:34EDT czogby 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_PSP_MEMUTILS_STUBS_H_
#define UT_CFE_PSP_MEMUTILS_STUBS_H_

typedef enum 
{
    UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX,
    UT_CFE_PSP_MEMUTILS_MEMSET_INDEX,
    UT_CFE_PSP_MEMUTILS_MAX_INDEX
} Ut_CFE_PSP_MEMUTILS_INDEX_t;

typedef struct
{
    int32 (*CFE_PSP_MemCpy)(void *Dest, void *Src, uint32 Size);
    int32 (*CFE_PSP_MemSet)(void *Dest, uint8 Value, uint32 Size);
} Ut_CFE_PSP_MEMUTILS_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_PSP_MEMUTILS_ReturnCodeTable_t;

void Ut_CFE_PSP_MEMUTILS_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_PSP_MEMUTILS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_PSP_MEMUTILS_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif
