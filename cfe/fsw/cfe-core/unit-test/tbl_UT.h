/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    tbl_UT.h
**
** Purpose:
**    TBL unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:23:30GMT-05:00 $
** $Revision: 1.1 $
**
*/
#ifndef _tbl_UT_h_
#define _tbl_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "cfe_tbl.h"
#include "common_types.h"
#include "ut_stubs.h"
#include "cfe_tbl_msg.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_task.h"
#include "cfe_tbl_task_cmds.h"

typedef struct
{
    uint32 TblElement1;
    uint32 TblElement2;
} UT_Table1_t;

typedef struct
{
    uint32 TblElement1;
    uint32 TblElement2;
    uint32 TblElement3;
} UT_Table2_t;

typedef struct
{
    CFE_TBL_File_Hdr_t TblHeader;
    UT_Table1_t        TblData;
} UT_TempFile_t;


/* TBL unit test functions */

/*****************************************************************************/
/**
** \brief Unit test specific call to process SB messages
**
** \par Description
**        Unit test specific call to process SB messages.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #CFE_TBL_TaskPipe
**
******************************************************************************/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/**
** \brief Initialize the registry
**
** \par Description
**        Fill the whole table registry with known table names in order to
**        detect when a table name is not registered, and set an owning
**        application ID for each table entry
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_InitializeTableRegistryNames(void);

/*****************************************************************************/
/**
** \brief Initialize table registry values
**
** \par Description
**        This function serves as a pass-through for messages coming from the
**        CFE_SB_SendMsg() stub function.  By using a common pass-through
**        function name, the stub can be generic for all of the tests for the
**        various services (i.e., EVS, TBL, etc.).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #CFE_TBL_InitRegistryRecord
**
******************************************************************************/
void UT_ResetTableRegistry(void);

/*****************************************************************************/
/**
** \brief Tests to cover table task initialization functions
**
** \par Description
**        This function tests the table task initialization functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_TaskMain, #UT_Report, #UT_SetRtnCode,
** \sa #CFE_TBL_TaskInit, #UT_SetSBTotalMsgLen, #CFE_SB_SetMsgId,
** \sa #CFE_SB_SetCmdCode, #CFE_TBL_TaskPipe
**
******************************************************************************/
void Test_CFE_TBL_TaskInit(void);

/*****************************************************************************/
/**
** \brief Test table service application data initialization
**
** \par Description
**        This function tests table service application data initialization.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_InitData, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_InitData(void);

/*****************************************************************************/
/**
** \brief Test command handler table message ID (or command code) search
**        function
**
** \par Description
**        This function tests the command handler table message ID (or command
**        code) search function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_SearchCmdHndlrTbl,
**
******************************************************************************/
void Test_CFE_TBL_SearchCmdHndlrTbl(void);

/*****************************************************************************/
/**
** \brief Test the delete critical table's CDS command message function
**
** \par Description
**        This function tests the delete critical table's CDS command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_DeleteCDSCmd,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_DeleteCDSCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing telemetry table registry command message function
**
** \par Description
**        This function tests the processing telemetry table registry command
**        message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_TlmRegCmd
**
******************************************************************************/
void Test_CFE_TBL_TlmRegCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing abort load command message function
**
** \par Description
**        This function tests the processing abort load command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_AbortLoadCmd
**
******************************************************************************/
void Test_CFE_TBL_AbortLoadCmd(void);

/*****************************************************************************/
/**
** \brief Test the activate table command message function
**
** \par Description
**        This function tests the activate table command message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_ActivateCmd
**
******************************************************************************/
void Test_CFE_TBL_ActivateCmd(void);

/*****************************************************************************/
/**
** \brief Test the write table data to a file function
**
** \par Description
**        This function tests the write table data to a file function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_TBL_DumpToFile,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_DumpToFile(void);

/*****************************************************************************/
/**
** \brief Test the processing reset counters command message function
**
** \par Description
**        This function tests the processing reset counters command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_Report, #CFE_TBL_ResetCmd
**
******************************************************************************/
void Test_CFE_TBL_ResetCmd(void);

/*****************************************************************************/
/**
** \brief Test the validate table command message function
**
** \par Description
**        This function tests the validate table command message function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_ValidateCmd
**
******************************************************************************/
void Test_CFE_TBL_ValidateCmd(void);

/*****************************************************************************/
/**
** \brief Test the processing no-operation command message function
**
** \par Description
**        This function tests the processing no-operation command message
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_NoopCmd
**
******************************************************************************/
void Test_CFE_TBL_NoopCmd(void);

/*****************************************************************************/
/**
** \brief Test the function which converts table registry entries for tables
**        into messages
**
** \par Description
**        This function tests the function which converts table registry
**        entries for tables into messages.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_GetTblRegData, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_GetTblRegData(void);

/*****************************************************************************/
/**
** \brief Test the function that collects data and stores it in the
**        housekeeping message
**
** \par Description
**        This function tests the function that collects data and stores it in
**        the housekeeping message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_GetHkData, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_GetHkData(void);

/*****************************************************************************/
/**
** \brief Test the function that processes dump table registration to file
**        command message
**
** \par Description
**        This function tests the function that processes dump table
**        registration to file command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_TBL_DumpRegCmd,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_DumpRegCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes dump table to file command message
**
** \par Description
**        This function tests the function that processes dump table to file
**        command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TBL_DumpCmd, #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_DumpCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes load table file to buffer command
**        message
**
** \par Description
**        This function tests the function that processes load table file to
**        buffer command message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_TBL_LoadCmd,
** \sa #UT_SetReadBuffer, #UT_SetReadHeader, #CFE_TBL_ByteSwapUint32,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_LoadCmd(void);

/*****************************************************************************/
/**
** \brief Test the function that processes housekeeping request message
**
** \par Description
**        This function tests the function that processes housekeeping request
**        message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetRtnCode, #UT_Report,
** \sa #CFE_TBL_HousekeepingCmd, #UT_SetOSFail
**
******************************************************************************/
void Test_CFE_TBL_HousekeepingCmd(void);

/*****************************************************************************/
/**
** \brief Prepare for test table API functions
**
** \par Description
**        Function to prepare for test table API functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_SetAppID, #UT_ResetCDS, #CFE_TBL_EarlyInit
**
******************************************************************************/
void Test_CFE_TBL_ApiInit(void);

/*****************************************************************************/
/**
** \brief Test function that registers a table with cFE to obtain table
**        management services
**
** \par Description
**        This function tests the function that registers a table with cFE to
**        obtain table management services.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetRtnCode, #CFE_TBL_Register,
** \sa #UT_EventIsInHistory, #UT_GetNumEventsSent, #UT_Report, #UT_SetAppID,
** \sa #UT_ResetTableRegistry
**
******************************************************************************/
void Test_CFE_TBL_Register(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the handle of table registered by another
**        application
**
** \par Description
**        This function tests the function that obtains the handle of table
**        registered by another application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetRtnCode, #CFE_TBL_Share,
** \sa #UT_EventIsInHistory, #UT_GetNumEventsSent, #UT_Report, #UT_SetAppID,
** \sa #CFE_TBL_Unregister, #UT_SetOSFail, #CFE_TBL_ByteSwapUint32,
** \sa #UT_SetReadBuffer, #UT_SetReadHeader
**
******************************************************************************/
void Test_CFE_TBL_Share(void);

/*****************************************************************************/
/**
** \brief Function to test unregistering a previously registered table and
**        freeing associated resources
**
** \par Description
**        This function tests unregistering a previously registered table and
**        freeing associated resources.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_Unregister,
** \sa #UT_EventIsInHistory, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_Unregister(void);

/*****************************************************************************/
/**
** \brief Test function that instructs table services to notify calling
**        application whenever the specified table requires management
**
** \par Description
**        This function tests the function that instructs table services to
**        notify calling application whenever the specified table requires
**        management.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_ResetCDS, #CFE_TBL_EarlyInit,
** \sa #UT_ResetPoolBufferIndex, #CFE_TBL_Register, #UT_GetNumEventsSent,
** \sa #UT_Report, #CFE_TBL_NotifyByMessage
**
******************************************************************************/
void Test_CFE_TBL_NotifyByMessage(void);

/*****************************************************************************/
/**
** \brief Function to test loading a specified table with data from the
**        specified source
**
** \par Description
**        This function tests the function for loading a specified table with
**        data from the specified source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_ResetTableRegistry,
** \sa #CFE_TBL_Register, #UT_EventIsInHistory, #UT_GetNumEventsSent,
** \sa #UT_Report, #UT_SetOSFail, #CFE_TBL_ByteSwapUint32, #UT_SetReadBuffer,
** \sa #UT_SetReadHeader, #UT_SetRtnCode, #CFE_TBL_Load, #CFE_TBL_Share,
** \sa #CFE_TBL_GetAddress, #CFE_TBL_ReleaseAddress
**
******************************************************************************/
void Test_CFE_TBL_Load(void);

/*****************************************************************************/
/**
** \brief Function to test obtaining the current address of the contents
**        of the specified table
**
** \par Description
**        This function tests the function for obtaining the current address of
**        the contents of the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_GetAddress,
** \sa #UT_GetNumEventsSent, #UT_Report, #UT_SetRtnCode, #CFE_TBL_Unregister
**
******************************************************************************/
void Test_CFE_TBL_GetAddress(void);

/*****************************************************************************/
/**
** \brief Function to test release of a previously obtained pointer to the
**        contents of the specified table
**
** \par Description
**        This function tests the function for releasing a previously obtained
**        pointer to the contents of the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_ResetTableRegistry,
** \sa #CFE_TBL_Register, #UT_EventIsInHistory, #UT_GetNumEventsSent,
** \sa #UT_Report, #CFE_TBL_ReleaseAddress
**
******************************************************************************/
void Test_CFE_TBL_ReleaseAddress(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the current addresses of the contents of
**        a collection of tables
**
** \par Description
**        This function tests the function that obtains the current addresses
**        of the contents of a collection of tables.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_Register,
** \sa #UT_EventIsInHistory, #UT_GetNumEventsSent, #UT_Report,
** #CFE_TBL_GetAddresses
**
******************************************************************************/
void Test_CFE_TBL_GetAddresses(void);

/*****************************************************************************/
/**
** \brief Test function that releases previously obtained pointers to the
**        contents of the specified tables
**
** \par Description
**        This function tests the function that releases previously obtained
**        pointers to the contents of the specified tables.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_ReleaseAddresses,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_ReleaseAddresses(void);

/*****************************************************************************/
/**
** \brief Test function for validating the table image for a specified table
**
** \par Description
**        This function tests the function for validating the table image for a
**        specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_Validate,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_Validate(void);

/*****************************************************************************/
/**
** \brief Test function for performing standard table maintenance on the
**        specified table
**
** \par Description
**        This function tests the function for performing standard table
**        maintenance on the specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_Manage, #UT_GetNumEventsSent,
** \sa #UT_Report, #CFE_TBL_FindTableInRegistry, #CFE_TBL_GetWorkingBuffer,
** \sa #UT_SetAppID, #CFE_TBL_Load, #UT_EventIsInHistory, #UT_SetRtnCode,
** \sa #CFE_PSP_MemCpy
**
******************************************************************************/
void Test_CFE_TBL_Manage(void);

/*****************************************************************************/
/**
** \brief Test function that updates the contents of a table if an update
**        is pending
**
** \par Description
**        This function tests the function that updates the contents of a table
**        if an update is pending.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_Update,
** \sa #UT_EventIsInHistory, #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_Update(void);

/*****************************************************************************/
/**
** \brief Test function that obtains the pending action status for specified
**        table
**
** \par Description
**        This function tests the function that obtains the pending action
**        status for specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #CFE_TBL_GetStatus,
** \sa #UT_GetNumEventsSent, #UT_Report
**
******************************************************************************/
void Test_CFE_TBL_GetStatus(void);

/*****************************************************************************/
/**
** \brief Test function that obtains characteristics/information for a
**        specified table
**
** \par Description
**        This function tests the function that obtains characteristics/
**        information for a specified table.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #
**
******************************************************************************/
void Test_CFE_TBL_GetInfo(void);

/*****************************************************************************/
/**
** \brief Test function that loads a specified table with data from the
**        specified source
**
** \par Description
**        This function tests the function that loads a specified table with
**        data from the specified source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_ResetCDS, #CFE_TBL_EarlyInit,
** \sa #UT_ResetPoolBufferIndex, #CFE_TBL_Register, #UT_GetNumEventsSent,
** \sa #UT_Report, #CFE_TBL_ByteSwapUint32, #UT_SetReadBuffer,
** \sa #UT_SetReadHeader, #CFE_TBL_Load, #UT_EventIsInHistory,
** \sa #CFE_TBL_GetAddress, #CFE_TBL_Modified, #CFE_TBL_GetInfo, #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_TBL_TblMod(void);

/*****************************************************************************/
/**
** \brief Tests for the remaining functions in cfe_tbl_internal.c
**
** \par Description
**        This function tests the remaining functions in cfe_tbl_internal.c.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TBL_GetWorkingBuffer, #UT_GetNumEventsSent,
** \sa #UT_Report, #UT_SetRtnCode, #CFE_TBL_LoadFromFile,
** \sa #CFE_TBL_ByteSwapUint32, #UT_SetReadBuffer, #UT_SetReadHeader,
** \sa #UT_EventIsInHistory, #CFE_TBL_ReadHeaders, #CFE_TBL_Unregister,
** \sa #UT_ResetCDS, #CFE_TBL_EarlyInit, #CFE_TBL_Register, #CFE_TBL_Load,
** \sa #CFE_TBL_GetAddress, #UT_ClearEventHistory, #CFE_TBL_ReleaseAddress,
** \sa #CFE_TBL_Share, #UT_SetPutPoolFail, #CFE_TBL_CleanUpApp
**
******************************************************************************/
void Test_CFE_TBL_Internal(void);

/*****************************************************************************/
/**
** \brief Test function executed when the contents of a table need to be
**        validated
**
** \par Description
**        This function tests the function executed when the contents of a
**        table need to be validated.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**      Always returns 0
**        This function does not return a value.
**
******************************************************************************/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr);

#endif /* _tbl_ut_h_ */
