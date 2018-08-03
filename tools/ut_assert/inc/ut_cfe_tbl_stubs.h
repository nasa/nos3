/*
**
** File:  ut_cfe_tbl_stubs.h
**
** $Id: ut_cfe_tbl_stubs.h 1.1 2016/02/08 20:53:28EST rperera Exp  $
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
** Purpose: cFE Table Services Header file for unit test stubs
**
** $Log: ut_cfe_tbl_stubs.h  $
** Revision 1.1 2016/02/08 20:53:28EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/inc/project.pj
** Revision 1.6 2015/06/16 16:02:13EDT sstrege 
** Added copyright information
** Revision 1.5 2015/05/01 11:44:00EDT czogby 
** Added CFE_TBL_NotifyByMessage and Ut_CFE_TBL_ContinueReturnCodeAfterCountZero
** Revision 1.4 2015/03/17 18:17:43EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 17:56:12Z]
** Added function prototypes, added constants, added new elements to Ut_CFE_TBL_INDEX_t
** Revision 1.3 2015/03/10 15:19:33EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.2 2011/02/18 15:57:43EST sslegel 
** Added new hooks and return codes
** Changed Ut_CFE_TBL_LoadHook to automatically call the table validate function
** Revision 1.1 2011/02/15 11:12:35EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_TBL_STUBS_H_
#define UT_CFE_TBL_STUBS_H_

#define CFE_TBL_NOT_OWNED               0xFFFFFFFF
#define CFE_TBL_NOT_FOUND               (-1)
#define CFE_TBL_END_OF_LIST             (CFE_TBL_Handle_t)0xFFFF

typedef enum 
{
    UT_CFE_TBL_REGISTER_INDEX,
    UT_CFE_TBL_LOAD_INDEX,
    UT_CFE_TBL_MANAGE_INDEX,
    UT_CFE_TBL_GETADDRESS_INDEX,
    UT_CFE_TBL_GETADDRESSES_INDEX,
    UT_CFE_TBL_GETSTATUS_INDEX,
    UT_CFE_TBL_GETINFO_INDEX,
    UT_CFE_TBL_SHARE_INDEX,
    UT_CFE_TBL_UNREGISTER_INDEX,
    UT_CFE_TBL_UPDATE_INDEX,
    UT_CFE_TBL_RELEASEADDRESS_INDEX,
    UT_CFE_TBL_RELEASEADDRESSES_INDEX,
    UT_CFE_TBL_VALIDATE_INDEX,
    UT_CFE_TBL_DUMPTOBUFFER_INDEX,
    UT_CFE_TBL_MODIFIED_INDEX,
    UT_CFE_TBL_CLEANUPAPP_INDEX,
    UT_CFE_TBL_NOTIFYBYMESSAGE_INDEX,
    UT_CFE_TBL_MAX_INDEX
} Ut_CFE_TBL_INDEX_t;

typedef struct
{
    int32 (*CFE_TBL_Register)(CFE_TBL_Handle_t*, const char *,uint32, uint16, CFE_TBL_CallbackFuncPtr_t);
    int32 (*CFE_TBL_Load)(CFE_TBL_Handle_t, CFE_TBL_SrcEnum_t, const void *);
    int32 (*CFE_TBL_Manage)(CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddress)(void **, CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddresses)(void **[], uint16, const CFE_TBL_Handle_t []);
    int32 (*CFE_TBL_GetInfo)(CFE_TBL_Info_t *TblInfoPtr, const char *TblName);
    int32 (*CFE_TBL_Share)(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName);
    int32 (*CFE_TBL_Unregister)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_Update)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_ReleaseAddress)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_ReleaseAddresses)(uint16 NumTables, const CFE_TBL_Handle_t TblHandles[]);
    int32 (*CFE_TBL_Validate)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_GetStatus)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_DumpToBuffer)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_Modified)(CFE_TBL_Handle_t TblHandle);
    int32 (*CFE_TBL_CleanUpApp)(uint32 AppId);
    int32 (*CFE_TBL_NotifyByMessage)(CFE_TBL_Handle_t TblHandle, uint32 MsgId, uint16 CommandCode, uint32 Parameter);
} Ut_CFE_TBL_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
    boolean ContinueReturnCodeAfterCountZero;
} Ut_CFE_TBL_ReturnCodeTable_t;

void Ut_CFE_TBL_Reset(void);
void Ut_CFE_TBL_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_TBL_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);
void Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(uint32 Index);

#endif 
