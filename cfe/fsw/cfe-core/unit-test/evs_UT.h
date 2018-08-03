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
**    evs_UT.h
**
** Purpose:
**    EVS unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:23:28GMT-05:00 $
** $Revision: 1.1 $
**
*/
#ifndef _evs_UT_h_
#define _evs_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "osapi.h"
#include "cfe_evs.h"
#include "cfe_evs_log.h"
#include "cfe_evs_task.h"
#include "cfe_evs_utils.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_time.h"
#include "ut_stubs.h"

/* EVS unit test functions */
/*****************************************************************************/
/**
** \brief Initializes the EVS for subsequent tests and performs tests on the
**        initialization and main functions
**
** \par Description
**        This function configures the EVS for subsequent tests and must be
**        called, once, prior to any subsequent test routines.  Additionally,
**        this routine performs tests on the EVS early initialization, task
**        initialization, and main functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, UT_InitData, #UT_SetSizeofESResetArea, #UT_SetRtnCode
** \sa #CFE_EVS_EarlyInit, #UT_Report, #CFE_EVS_TaskMain
** \sa #UT_SetStatusBSPResetArea, #UT_SetAppID, #UT_SendMsg
** \sa #UT_SetSBTotalMsgLen
**
******************************************************************************/
void Test_Init(void);

/*****************************************************************************/
/**
** \brief Test API function response to an illegal application ID
**
** \par Description
**        This function tests the response to an illegal application ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetAppID, #UT_Report, #CFE_EVS_Register
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendTimedEvent, #CFE_EVS_SendEventWithAppID
** \sa #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters, #CFE_EVS_CleanUpApp
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_DisablePortsCmd
**
******************************************************************************/
void Test_IllegalAppID(void);

/*****************************************************************************/
/**
** \brief Test API function response to an unregistered application
**
** \par Description
**        This function tests the response to an unregistered application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #CFE_EVS_Unregister, #UT_Report
** \sa #CFE_EVS_SendEvent, #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_SendTimedEvent
** \sa #CFE_EVS_CleanUpApp, #CFE_EVS_Register
**
******************************************************************************/
void Test_UnregisteredApp(void);

/*****************************************************************************/
/**
** \brief Test filter registration related calls
**
** \par Description
**        This function tests the filter registration related calls.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_EVS_Register
** \sa #UT_SetPutPoolFail, #UT_SetRtnCode, #CFE_EVS_Unregister
** \sa #CFE_EVS_SendEvent, #CFE_ES_GetAppID
**
******************************************************************************/
void Test_FilterRegistration(void);

/*****************************************************************************/
/**
** \brief Test reset filter API calls
**
** \par Description
**        This function tests the reset filter API calls.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_EVS_Register
** \sa #CFE_EVS_ResetFilter, #CFE_EVS_ResetAllFilters
**
******************************************************************************/
void Test_FilterReset(void);

/*****************************************************************************/
/**
** \brief Test long and short format events, and event strings
**        greater than the maximum length allowed
**
** \par Description
**        This function tests long and short format events, and event strings
**        greater than the maximum length allowed.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg, #UT_Report
** \sa #CFE_EVS_SendEvent, #UT_SetRtnCode, #CFE_EVS_SendTimedEvent
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_SetEventFormatModeCmd
**
******************************************************************************/
void Test_Format(void);

/*****************************************************************************/
/**
** \brief Test enable/disable of port outputs
**
** \par Description
**        This function tests enable/disable of port outputs.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetRtnCode, #UT_SetSBTotalMsgLen
** \sa #UT_SendMsg, #UT_Report, #CFE_EVS_SendEvent, #CFE_EVS_EnablePortsCmd
** \sa #CFE_EVS_DisablePortsCmd
**
******************************************************************************/
void Test_Ports(void);

/*****************************************************************************/
/**
** \brief Test event logging
**
** \par Description
**        This function tests event logging.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #UT_SetSizeofESResetArea, #UT_SetRtnCode
** \sa #CFE_PSP_GetResetArea, #CFE_EVS_SendEvent, #UT_SetOSFail
** \sa #CFE_EVS_SetLoggingModeCmd, #CFE_EVS_WriteLogFileCmd, #EVS_ClearLog
** \sa #CFE_EVS_ProcessGroundCommand
**
******************************************************************************/
void Test_Logging(void);

/*****************************************************************************/
/**
** \brief Test writing application data
**
** \par Description
**        This function tests writing application data.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #UT_SetOSFail, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_ResetCountersCmd, #CFE_EVS_WriteAppDataCmd
**
******************************************************************************/
void Test_WriteApp(void);

/*****************************************************************************/
/**
** \brief Test commands with bad application names (unknown, illegal,
**        unregistered)
**
** \par Description
**        This function tests the commands with bad application names
**        (unknown, illegal, unregistered).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_DisableAppEventTypesCmd
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_EnableAppEventsCmd
** \sa #CFE_EVS_DisableAppEventsCmd, #CFE_EVS_ResetAppEventCounterCmd
** \sa #CFE_EVS_AddEventFilterCmd, #CFE_EVS_DeleteEventFilterCmd
** \sa #CFE_EVS_SetFilterMaskCmd, #CFE_EVS_ResetFilterCmd
** \sa #CFE_EVS_ResetAllFiltersCmd, #CFE_EVS_DisableAppEventTypesCmd
**
******************************************************************************/
void Test_BadAppCmd(void);

/*****************************************************************************/
/**
** \brief Test event commands (and reset counter)
**
** \par Description
**        This function tests the event commands (and reset counter).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #CFE_EVS_SendEvent, #UT_Report, #CFE_EVS_DisableAppEventTypesCmd
** \sa #CFE_EVS_EnableAppEventTypesCmd, #CFE_EVS_DisableAppEventsCmd
** \sa #CFE_EVS_EnableAppEventsCmd, #CFE_EVS_DisableEventTypesCmd
** \sa #CFE_EVS_EnableEventTypesCmd, #CFE_EVS_ResetAppEventCounterCmd
**
******************************************************************************/
void Test_EventCmd(void);

/*****************************************************************************/
/**
** \brief Test filter commands
**
** \par Description
**        This function tests the .
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_Register, #CFE_EVS_EnableAppEventTypesCmd
** \sa #CFE_EVS_DeleteEventFilterCmd, #CFE_EVS_SetFilterMaskCmd
** \sa #CFE_EVS_ResetFilterCmd, #CFE_EVS_ResetAllFiltersCmd
** \sa #CFE_EVS_AddEventFilterCmd
**
******************************************************************************/
void Test_FilterCmd(void);

/*****************************************************************************/
/**
** \brief Test commands with invalid command, command packet, and
**        command length
**
** \par Description
**        This function tests the commands with invalid command, command
**        packet, and command length.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_SetSBTotalMsgLen, #UT_SendMsg
** \sa #UT_Report, #CFE_EVS_ProcessGroundCommand, #CFE_EVS_VerifyCmdLength
**
******************************************************************************/
void Test_InvalidCmd(void);

/*****************************************************************************/
/**
** \brief Test miscellaneous functionality
**
** \par Description
**        This function tests miscellaneous functionality.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #EVS_GetApplicationInfo
** \sa #UT_SetSBTotalMsgLen, #UT_SendMsg, #CFE_EVS_CleanUpApp
** \sa #CFE_EVS_Register, #CFE_EVS_WriteLogFileCmd, #CFE_EVS_SetLoggingModeCmd
** \sa #CFE_EVS_ReportHousekeepingCmd
**
******************************************************************************/
void Test_Misc(void);

/*****************************************************************************/
/**
** \brief Unit test specific call to process SB messages
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
** \sa #CFE_EVS_ProcessCommandPacket
**
******************************************************************************/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

#endif /* _evs_UT_h_ */
