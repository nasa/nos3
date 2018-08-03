/*
**
** File: ut_cfe_tbl_hooks.h
**
** $Id: ut_cfe_tbl_hooks.h 1.2 2016/06/22 18:41:04EDT czogby Exp  $
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
** Purpose: Unit test header file for cFE Table Services hooks.
**
** $Log: ut_cfe_tbl_hooks.h  $
** Revision 1.2 2016/06/22 18:41:04EDT czogby 
** Update Stub Functions to Match Changes in Corresponding Function Prototypes in New Releases of CFE, OSAL, and PSP
** Revision 1.1 2016/02/08 20:53:28EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.5 2015/06/16 16:02:12EDT sstrege 
** Added copyright information
** Revision 1.4 2015/05/01 11:08:06EDT czogby 
** Moved Ut_CFE_TBL_GetAddressHook to improve readability
** Revision 1.3 2015/03/10 15:19:32EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:50:37Z]
** No source code changes were added in this revision.
** Revision 1.2 2011/02/18 15:57:43EST sslegel 
** Added new hooks and return codes
** Changed Ut_CFE_TBL_LoadHook to automatically call the table validate function
** Revision 1.1 2011/02/15 11:12:34EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_TBL_HOOKS_H_
#define UT_CFE_TBL_HOOKS_H_

#include "cfe.h"

void        Ut_CFE_TBL_ClearTables(void);
int32       Ut_CFE_TBL_RegisterTable(const char *Name, uint32 Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr);
int32       Ut_CFE_TBL_AddTable(const char *Filename, void *TablePtr);
int32       Ut_CFE_TBL_LoadTable(CFE_TBL_Handle_t TblHandle, void *SrcDataPtr);
int32       Ut_CFE_TBL_FindTable(const char *Filename);
void       *Ut_CFE_TBL_GetAddress(CFE_TBL_Handle_t TblHandle);
int32       Ut_CFE_TBL_GetAddressHook(void **TblPtr, CFE_TBL_Handle_t TblHandle);
int32       Ut_CFE_TBL_RegisterHook(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, uint32 Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr);
int32       Ut_CFE_TBL_LoadHook(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr);

#endif
