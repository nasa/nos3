/*
**
** File: ut_cfe_tbl_stubs.c
**
** $Id: ut_cfe_tbl_stubs.c 1.2 2016/03/08 18:03:27EST czogby Exp  $
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
** Purpose: Unit test stubs for cFE Table Services routines
**
** $Log: ut_cfe_tbl_stubs.c  $
** Revision 1.2 2016/03/08 18:03:27EST czogby 
** Add entry for CFE_TBL_GetStatus in Ut_CFE_TBL_SetFunctionHook
** Revision 1.1 2016/02/08 20:53:31EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.8 2015/06/16 16:14:02EDT sstrege 
** Added copyright information
** Revision 1.7 2015/05/01 14:04:29EDT czogby 
** Add UtAssert fail where error messages, add return, add NotifyByMessage, add ContinueReturnCodeAfterCountZero
** Revision 1.6 2015/03/17 18:23:50EDT czogby 
** Add missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:31:05Z]
** Added missing ability to call hook functions / set return values
** Revision 1.5 2015/03/10 15:21:48EDT czogby 
** Add Missing Functionality to UT Assert Library
** Revision 1.4 2011/03/10 11:19:20EST sslegel 
** Added return code support to CFE_TBL_GetInfo
** Revision 1.3 2011/02/18 15:57:42EST sslegel 
** Added new hooks and return codes
** Changed Ut_CFE_TBL_LoadHook to automatically call the table validate function
** Revision 1.2 2011/02/17 16:34:27EST rmcgraw 
** Tbl GetAdr Hook change to return TBL_UPDATED after a TBL load
** Revision 1.1 2011/02/15 11:13:04EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "utassert.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include <string.h>

Ut_CFE_TBL_HookTable_t          Ut_CFE_TBL_HookTable;
Ut_CFE_TBL_ReturnCodeTable_t    Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_MAX_INDEX];

void Ut_CFE_TBL_Reset(void)
{
    memset(&Ut_CFE_TBL_HookTable, 0, sizeof(Ut_CFE_TBL_HookTable));
    memset(&Ut_CFE_TBL_ReturnCodeTable, 0, sizeof(Ut_CFE_TBL_ReturnCodeTable));
    
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_REGISTER_INDEX, (void *)&Ut_CFE_TBL_RegisterHook);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_LOAD_INDEX, (void *)&Ut_CFE_TBL_LoadHook);
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, (void *)&Ut_CFE_TBL_GetAddressHook);

    Ut_CFE_TBL_ClearTables();
}

void Ut_CFE_TBL_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_TBL_REGISTER_INDEX)        { Ut_CFE_TBL_HookTable.CFE_TBL_Register = FunPtr; }
    else if (Index == UT_CFE_TBL_LOAD_INDEX)            { Ut_CFE_TBL_HookTable.CFE_TBL_Load = FunPtr; }
    else if (Index == UT_CFE_TBL_MANAGE_INDEX)          { Ut_CFE_TBL_HookTable.CFE_TBL_Manage = FunPtr; }
    else if (Index == UT_CFE_TBL_GETADDRESS_INDEX)      { Ut_CFE_TBL_HookTable.CFE_TBL_GetAddress = FunPtr; }
    else if (Index == UT_CFE_TBL_GETADDRESSES_INDEX)    { Ut_CFE_TBL_HookTable.CFE_TBL_GetAddresses = FunPtr; }
    else if (Index == UT_CFE_TBL_GETINFO_INDEX)         { Ut_CFE_TBL_HookTable.CFE_TBL_GetInfo = FunPtr; }
    else if (Index == UT_CFE_TBL_NOTIFYBYMESSAGE_INDEX) { Ut_CFE_TBL_HookTable.CFE_TBL_NotifyByMessage = FunPtr; }
    else if (Index == UT_CFE_TBL_GETSTATUS_INDEX)       { Ut_CFE_TBL_HookTable.CFE_TBL_GetStatus = FunPtr; }
    else
    {
        printf("Unsupported TBL Index In SetFunctionHook Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported TBL Index In SetFunctionHook Call");
    }
}

void Ut_CFE_TBL_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_TBL_MAX_INDEX)
    {
        Ut_CFE_TBL_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_TBL_ReturnCodeTable[Index].Count = CallCnt;
    }
    else
    {
        printf("Unsupported TBL Index In SetReturnCode Call %lu\n", Index);
        UtAssert_True(FALSE, "Unsupported TBL Index In SetReturnCode Call");
    }
}

boolean Ut_CFE_TBL_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_TBL_ReturnCodeTable[Index].Count > 0)
    {
        Ut_CFE_TBL_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_TBL_ReturnCodeTable[Index].Count == 0)
            return(TRUE);
    }
    else if (Ut_CFE_TBL_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero == TRUE)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

void Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(uint32 Index)
{
    Ut_CFE_TBL_ReturnCodeTable[Index].ContinueReturnCodeAfterCountZero = TRUE;
}
        
int32 CFE_TBL_Register( CFE_TBL_Handle_t *TblHandlePtr,                 
                        const char   *Name,                             
                        uint32  Size,                                   
                        uint16  TblOptionFlags,                         
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_REGISTER_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_REGISTER_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Register)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Register(TblHandlePtr, Name, Size, TblOptionFlags,TblValidationFuncPtr));
    
    return CFE_SUCCESS;
}

int32 CFE_TBL_Share( CFE_TBL_Handle_t *TblHandlePtr,      /* Returned Handle */
                     const char *TblName )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_SHARE_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_SHARE_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Share)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Share(TblHandlePtr, TblName));

    return CFE_SUCCESS;
}

int32 CFE_TBL_Unregister ( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_UNREGISTER_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_UNREGISTER_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Unregister)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Unregister(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_Load( CFE_TBL_Handle_t TblHandle,
                    CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_LOAD_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_LOAD_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Load)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Load(TblHandle, SrcType, SrcDataPtr));

    return CFE_SUCCESS;
}

int32 CFE_TBL_Update( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_UPDATE_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_UPDATE_INDEX].Value;
    
    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Update)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Update(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_GetAddress( void **TblPtr,
                          CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_GETADDRESS_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_GETADDRESS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_GetAddress)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_GetAddress(TblPtr, TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_ReleaseAddress( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_RELEASEADDRESS_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_RELEASEADDRESS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_ReleaseAddress)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_ReleaseAddress(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_GetAddresses( void **TblPtrs[],
                            uint16 NumTables,
                            const CFE_TBL_Handle_t TblHandles[] )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_GETADDRESSES_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_GETADDRESSES_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_GetAddresses)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_GetAddresses(TblPtrs, NumTables, TblHandles));

    return CFE_SUCCESS;
}

int32 CFE_TBL_ReleaseAddresses( uint16 NumTables,
                                const CFE_TBL_Handle_t TblHandles[] )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_RELEASEADDRESSES_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_RELEASEADDRESSES_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_ReleaseAddresses)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_ReleaseAddresses(NumTables, TblHandles));

    return CFE_SUCCESS;
}
                                
int32 CFE_TBL_Validate( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_VALIDATE_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_VALIDATE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Validate)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Validate(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_Manage( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_MANAGE_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_MANAGE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Manage)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Manage(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_GetStatus( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_GETSTATUS_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_GETSTATUS_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_GetStatus)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_GetStatus(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_GetInfo( CFE_TBL_Info_t *TblInfoPtr, const char *TblName )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_GETINFO_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_GETINFO_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_GetInfo)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_GetInfo(TblInfoPtr, TblName));

    return CFE_SUCCESS;
}

int32   CFE_TBL_DumpToBuffer( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_DUMPTOBUFFER_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_DUMPTOBUFFER_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_DumpToBuffer)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_DumpToBuffer(TblHandle));

    return CFE_SUCCESS;
}

int32   CFE_TBL_Modified( CFE_TBL_Handle_t TblHandle )
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_MODIFIED_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_MODIFIED_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_Modified)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_Modified(TblHandle));

    return CFE_SUCCESS;
}

int32 CFE_TBL_CleanUpApp (uint32 AppId)
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_CLEANUPAPP_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_CLEANUPAPP_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_CleanUpApp)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_CleanUpApp(AppId));

    return CFE_SUCCESS;
}

int32 CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, uint32 MsgId, uint16 CommandCode, uint32 Parameter)
{
    /* Check for specified return */
    if (Ut_CFE_TBL_UseReturnCode(UT_CFE_TBL_NOTIFYBYMESSAGE_INDEX))
        return Ut_CFE_TBL_ReturnCodeTable[UT_CFE_TBL_NOTIFYBYMESSAGE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_TBL_HookTable.CFE_TBL_NotifyByMessage)
        return(Ut_CFE_TBL_HookTable.CFE_TBL_NotifyByMessage(TblHandle, MsgId, CommandCode, Parameter));

    return CFE_SUCCESS;
}
