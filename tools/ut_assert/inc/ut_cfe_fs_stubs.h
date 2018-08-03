/*
**
** File:  ut_cfe_fs_stubs.h
**
** $Id: ut_cfe_fs_stubs.h 1.2 2016/06/22 18:39:56EDT czogby Exp  $
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
** Purpose: cFE File System Header file for unit test stubs
**
** $Log: ut_cfe_fs_stubs.h  $
** Revision 1.2 2016/06/22 18:39:56EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:27EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.4 2015/06/16 16:02:11EDT sstrege 
** Added copyright information
** Revision 1.3 2015/05/07 14:29:47EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.2 2015/03/10 15:19:31EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:29:38Z]
** No source code updates were made in this revision.
** Revision 1.1 2011/02/15 11:12:33EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_FS_STUBS_H_
#define UT_CFE_FS_STUBS_H_

typedef enum 
{
    UT_CFE_FS_READHDR_INDEX,
    UT_CFE_FS_WRITEHDR_INDEX,
    UT_CFE_FS_SETTIMESTAMP_INDEX,
    UT_CFE_FS_ISGZFILE_INDEX,
    UT_CFE_FS_EXTRACTFILENAMEFROMPATH_INDEX,
    UT_CFE_FS_DECOMPRESS_INDEX,
    UT_CFE_FS_MAX_INDEX
} Ut_CFE_FS_INDEX_t;

typedef struct
{
    int32 (*CFE_FS_ReadHeader)(CFE_FS_Header_t *Hdr, int32 FileDes);
    int32 (*CFE_FS_WriteHeader)(int32 FileDes, CFE_FS_Header_t *Hdr);
    int32 (*CFE_FS_SetTimestamp)(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp);
    int32 (*CFE_FS_IsGzFile)(const char *FileName);
    int32 (*CFE_FS_ExtractFilenameFromPath)(const char *OriginalPath, char *FileNameOnly);
    int32 (*CFE_FS_Decompress)( const char * SourceFile, const char * DestinationFile );
} Ut_CFE_FS_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_FS_ReturnCodeTable_t;

void Ut_CFE_FS_Reset(void);
void Ut_CFE_FS_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_FS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_FS_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif 
