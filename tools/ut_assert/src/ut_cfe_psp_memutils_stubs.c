/*
**
** File: ut_cfe_psp_memutils_stubs.c
**
** $Id: ut_cfe_psp_memutils_stubs.c 1.3 2016/04/10 21:20:42EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE PSP Memory Utilities routines
**
** $Log: ut_cfe_psp_memutils_stubs.c  $
** Revision 1.3 2016/04/10 21:20:42EDT czogby 
** Separate Stub Functions Into Multiple Files Based On Source Filename
** Revision 1.2 2016/03/13 20:06:43EDT czogby 
** Add Missing Stub Functions to UT-Assert Library
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.6 2015/06/16 16:14:01EDT sstrege 
** Added copyright information
** Revision 1.5 2015/05/07 14:39:50EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.4 2015/05/01 13:15:19EDT czogby 
** Added UtAssert call to places where an error message is generated
** Revision 1.3 2015/03/17 18:23:04EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:27:05Z]
** Added ability to call hook functions / set return values
** Revision 1.2 2015/03/10 15:21:46EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.1 2011/02/15 11:13:02EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

/*
**  Include Files
*/
#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include <string.h>

Ut_CFE_PSP_MEMUTILS_HookTable_t          Ut_CFE_PSP_MEMUTILS_HookTable;
Ut_CFE_PSP_MEMUTILS_ReturnCodeTable_t    Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[UT_CFE_PSP_MEMUTILS_MAX_INDEX];

void Ut_CFE_PSP_MEMUTILS_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX)         { Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemCpy = FunPtr; }
    else if (Index == UT_CFE_PSP_MEMUTILS_MEMSET_INDEX)         { Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemSet = FunPtr; }
    else
    {
        printf("Unsupported PSP Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP Index In SetFunctionHook Call");
    }
}

void Ut_CFE_PSP_MEMUTILS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_PSP_MEMUTILS_MAX_INDEX)
    {
        Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported PSP_MEMUTILS Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported PSP_MEMUTILS Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_PSP_MEMUTILS_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_PSP_MEMUTILS_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_PSP_MemCpy(void *Dest, void *Src, uint32 Size)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_MEMUTILS_UseReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX))
        return Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemCpy)
        return(Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemCpy(Dest, Src, Size));

    memcpy(Dest, Src, Size);
    return(CFE_PSP_SUCCESS);
}

int32 CFE_PSP_MemSet(void *Dest, uint8 Value, uint32 Size)
{
    /* Check for specified return */
    if (Ut_CFE_PSP_MEMUTILS_UseReturnCode(UT_CFE_PSP_MEMUTILS_MEMSET_INDEX))
        return Ut_CFE_PSP_MEMUTILS_ReturnCodeTable[UT_CFE_PSP_MEMUTILS_MEMSET_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemSet)
        return(Ut_CFE_PSP_MEMUTILS_HookTable.CFE_PSP_MemSet(Dest, Value, Size));

    memset(Dest, Value, Size);
    return(CFE_PSP_SUCCESS);
}
