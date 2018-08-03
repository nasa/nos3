/*
**
** File:  ut_osfileapi_stubs.h
**
** $Id: ut_osfileapi_stubs.h 1.2 2016/06/22 19:14:17EDT czogby Exp  $
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
** Purpose: OSAPI File Services Header file for unit test stubs.
**
** $Log: ut_osfileapi_stubs.h  $
** Revision 1.2 2016/06/22 19:14:17EDT czogby 
** UT-Assert: Add Missing Stub Functions
** Revision 1.1 2016/02/08 20:53:28EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.7 2015/06/16 16:02:14EDT sstrege 
** Added copyright information
** Revision 1.6 2015/05/07 14:34:58EDT czogby 
** Add ability to set a custom return value to continue after a designated call count
** Revision 1.5 2015/03/17 18:18:07EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:00:40Z]
** Added function prototypes, added new elements to Ut_OSFILEAPI_INDEX_t
** Revision 1.4 2015/03/10 15:19:34EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.3 2011/03/30 09:58:56EDT rmcgraw 
** Added Hook and Return enhancements to Directory APIs
** Revision 1.2 2011/03/24 13:14:54EDT rmcgraw 
** Added Hook and RtnCode functionality to OS_stat
** Revision 1.1 2011/02/15 11:12:36EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_OSFILEAPI_STUBS_H_
#define UT_OSFILEAPI_STUBS_H_

typedef enum 
{
    UT_OSFILEAPI_CREAT_INDEX,
    UT_OSFILEAPI_WRITE_INDEX,
    UT_OSFILEAPI_READ_INDEX,
    UT_OSFILEAPI_OPENDIR_INDEX,
    UT_OSFILEAPI_READDIR_INDEX,
    UT_OSFILEAPI_CLOSE_INDEX,
    UT_OSFILEAPI_OPEN_INDEX,
    UT_OSFILEAPI_CLOSEDIR_INDEX,
    UT_OSFILEAPI_STAT_INDEX,
    UT_OSFILEAPI_FDGETINFO_INDEX,
    UT_OSFILEAPI_CHMOD_INDEX,
    UT_OSFILEAPI_LSEEK_INDEX,
    UT_OSFILEAPI_REMOVE_INDEX,
    UT_OSFILEAPI_RENAME_INDEX,
    UT_OSFILEAPI_CP_INDEX,
    UT_OSFILEAPI_MV_INDEX,
    UT_OSFILEAPI_MKDIR_INDEX,
    UT_OSFILEAPI_RMDIR_INDEX,
    UT_OSFILEAPI_CHECKNAMELENGTH_INDEX,
    UT_OSFILEAPI_SHELLOUTPUTTOFILE_INDEX,
    UT_OSFILEAPI_REWINDDIR_INDEX,
    UT_OSFILEAPI_FSBYTESFREE_INDEX,
    UT_OSFILEAPI_MAX_INDEX
} Ut_OSFILEAPI_INDEX_t;

typedef struct
{
    int32 (*OS_creat)(const char *,int32);
    int32 (*OS_write)(int32, void *, uint32);
    int32 (*OS_read)(int32, void *, uint32);
    os_dirp_t (*OS_opendir)(const char *path);
    os_dirent_t* (*OS_readdir)(os_dirp_t directory);
    int32 (*OS_close)(int32  filedes);
    int32 (*OS_open)(const char *path,  int32 access,  uint32  mode);
    int32 (*OS_closedir)(os_dirp_t directory);
    int32 (*OS_stat)(const char *path, os_fstat_t *filestats);    
    int32 (*OS_FDGetInfo) (int32 filedes, OS_FDTableEntry *fd_prop);
    int32 (*OS_chmod)(const char *path, uint32 access);
    int32 (*OS_lseek)(int32  filedes, int32 offset, uint32 whence);
    int32 (*OS_remove)(const char *path);
    int32 (*OS_rename)(const char *old, const char *new);
    int32 (*OS_cp)(const char *src, const char *dest);
    int32 (*OS_mv)(const char *src, const char *dest);
    int32 (*OS_mkdir)(const char *path, uint32 access);
    int32 (* OS_rmdir)(const char *path);
    int32 (*OS_check_name_length)(const char *path);
    void  (*OS_rewinddir)(os_dirp_t directory);
    int32 (*OS_ShellOutputToFile)(char* Cmd, int32 OS_fd);
    int32 (*OS_fsBytesFree)(const char *name, uint64 *bytes_free);
} Ut_OSFILEAPI_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count; 
    boolean ContinueReturnCodeAfterCountZero;
} Ut_OSFILEAPI_ReturnCodeTable_t;

void Ut_OSFILEAPI_Reset(void);
void Ut_OSFILEAPI_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_OSFILEAPI_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_OSFILEAPI_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif 
