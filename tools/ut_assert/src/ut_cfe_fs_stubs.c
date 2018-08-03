/*
**
** File:  ut_cfe_fs_stubs.c
**
** $Id: ut_cfe_fs_stubs.c 1.2 2016/06/23 14:39:04EDT czogby Exp  $
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
** Purpose: Unit test stubs for cFE File system routines
**
** $Log: ut_cfe_fs_stubs.c  $
** Revision 1.2 2016/06/23 14:39:04EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:30EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.7 2015/06/16 16:14:01EDT sstrege 
** Added copyright information
** Revision 1.6 2015/05/07 14:39:52EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.5 2015/05/01 13:13:52EDT czogby 
** Added UtAssert call to places where an error message is generated
** Revision 1.4 2015/03/17 18:22:29EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:26:18Z]
** Added missing checks for hook functions / return values
** Revision 1.3 2015/03/10 15:21:46EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.2 2011/03/22 14:19:02EDT rmcgraw 
** Use Rtn Code Added to CFE_FS_WriteHeader
** Revision 1.1 2011/02/15 11:13:02EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_fs_stubs.h"
#include <string.h>

Ut_CFE_FS_HookTable_t           Ut_CFE_FS_HookTable;
Ut_CFE_FS_ReturnCodeTable_t     Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_MAX_INDEX];

void Ut_CFE_FS_Reset(void)
{
    memset(&Ut_CFE_FS_HookTable, 0, sizeof(Ut_CFE_FS_HookTable));
    memset(&Ut_CFE_FS_ReturnCodeTable, 0, sizeof(Ut_CFE_FS_ReturnCodeTable));
}

void Ut_CFE_FS_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_FS_READHDR_INDEX)                  { Ut_CFE_FS_HookTable.CFE_FS_ReadHeader = FunPtr; }
    else if (Index == UT_CFE_FS_WRITEHDR_INDEX)                 { Ut_CFE_FS_HookTable.CFE_FS_WriteHeader = FunPtr; }
    else if (Index == UT_CFE_FS_SETTIMESTAMP_INDEX)             { Ut_CFE_FS_HookTable.CFE_FS_SetTimestamp = FunPtr; }
    else if (Index == UT_CFE_FS_ISGZFILE_INDEX)                 { Ut_CFE_FS_HookTable.CFE_FS_IsGzFile = FunPtr; }
    else if (Index == UT_CFE_FS_EXTRACTFILENAMEFROMPATH_INDEX)  { Ut_CFE_FS_HookTable.CFE_FS_ExtractFilenameFromPath = FunPtr; }
    else if (Index == UT_CFE_FS_DECOMPRESS_INDEX)               { Ut_CFE_FS_HookTable.CFE_FS_Decompress = FunPtr; }
    else
    {
        printf("Invalid FS Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Invalid FS Index In SetFunctionHook Call");
    }
}

void Ut_CFE_FS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_FS_MAX_INDEX)
    {
        Ut_CFE_FS_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_FS_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    { 
        printf("Unsupported FS Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported FS Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_FS_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_FS_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_FS_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_FS_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_FS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_FS_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_FS_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}

int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes)
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_READHDR_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_READHDR_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_ReadHeader)
        return(Ut_CFE_FS_HookTable.CFE_FS_ReadHeader(Hdr, FileDes));

    return CFE_SUCCESS;
}

int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr)
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_WRITEHDR_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_WRITEHDR_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_WriteHeader)
        return(Ut_CFE_FS_HookTable.CFE_FS_WriteHeader(FileDes, Hdr));

    return CFE_SUCCESS;
}

int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_SETTIMESTAMP_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_SETTIMESTAMP_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_SetTimestamp)
        return(Ut_CFE_FS_HookTable.CFE_FS_SetTimestamp(FileDes, NewTimestamp));

    return CFE_SUCCESS;
}

boolean CFE_FS_IsGzFile(const char *FileName)
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_ISGZFILE_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_ISGZFILE_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_IsGzFile)
        return(Ut_CFE_FS_HookTable.CFE_FS_IsGzFile(FileName));

    return TRUE;
}

int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_EXTRACTFILENAMEFROMPATH_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_EXTRACTFILENAMEFROMPATH_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_ExtractFilenameFromPath)
        return(Ut_CFE_FS_HookTable.CFE_FS_ExtractFilenameFromPath(OriginalPath, FileNameOnly));

    return CFE_SUCCESS;
}

int32 CFE_FS_Decompress( const char * SourceFile, const char * DestinationFile )
{
    /* Check for specified return */
    if (Ut_CFE_FS_UseReturnCode(UT_CFE_FS_DECOMPRESS_INDEX))
        return Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_DECOMPRESS_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_FS_HookTable.CFE_FS_Decompress)
        return(Ut_CFE_FS_HookTable.CFE_FS_Decompress(SourceFile, DestinationFile));

    return CFE_SUCCESS;
}
