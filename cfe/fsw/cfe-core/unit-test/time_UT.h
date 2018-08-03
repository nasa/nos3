/*
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    time_UT.h
**
** Purpose:
**    TIME unit test header
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
** $Log: time_UT.h  $
** Revision 1.1 2014/05/28 09:23:30GMT-05:00 wmoleski
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.6 2012/10/01 18:47:09EDT aschoeni
** Removed relative paths in includes, this is now done by makefile
** Revision 1.5 2012/01/13 13:59:29EST acudmore
** Added license text
** Revision 1.4 2011/12/07 19:19:32EST aschoeni
** Removed returns for TIME and SB for cleaning up apps
** Revision 1.3 2011/11/30 15:09:08EST jmdagost
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.2 2008/08/06 22:43:48EDT dkobe
** Updated Unit Tests for CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:44EDT ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.9 2007/05/15 15:16:31EDT njyanchik
** Updated unit test for new code
** Revision 1.8 2007/05/04 09:10:25EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.7 2007/05/01 13:28:14EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.6 2006/11/02 13:53:58EST njyanchik
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.5 2006/11/01 12:46:54GMT-05:00 njyanchik
** Changed the Unit test to reflect the changes from removing the CDS functionality from TIME
** Revision 1.4 2006/10/30 14:09:28GMT-05:00 njyanchik
** I changed TIME to use the new ES_CDS implementation. This involved using the
** new functions, as well as modifying the CFE_TIME_TaskData structure as well as
** the CFE_TIME_DiagPacket structure. They both made reference to the address
** of the TIME CDS (the old implementation). Now they both use the new
** CFE_ES_CDSHandle_t. Also, the Unit Test for Time was updated. No new paths
** through the code were created, but since return codes from the CDS functions
** changed, there needed to be updates to the UT.
** Revision 1.3 2006/05/31 08:33:15GMT-05:00 jjhageman
** Addition of CFE_TIME_MET2SCTime testing and added case to CFE_TIME_Sub2MicroSec for full coverage.
** Revision 1.2 2006/05/31 08:29:37EDT jjhageman
** Additions for code coverage with different defines
**
*/
#ifndef _time_UT_h_
#define _time_UT_h_

/*
** Includes
*/
#include <string.h>
#include "cfe_platform_cfg.h"
#include "cfe_sb.h"
#include "cfe_time.h"
#include "cfe_time_msg.h"
#include "cfe_time_utils.h"
#include "common_types.h"
#include "ut_stubs.h"

/*
** Function prototypes
*/

/* TIME unit test functions */
/*****************************************************************************/
/**
** \brief Test task entry point and main process loop
**
** \par Description
**        This function tests the task entry point and main process loop.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, CFE_TIME_TaskMain, #UT_Report, #UT_SetRtnCode
**
******************************************************************************/
void Test_Main(void);

/*****************************************************************************/
/**
** \brief Test API and time task initialization (must be called prior to
**        remaining tests)
**
** \par Description
**        This function tests API and time task initialization (must be called
**        prior to remaining tests).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #CFE_TIME_EarlyInit, #UT_InitData, #UT_Report,
** \sa #CFE_TIME_TaskInit, #UT_SetRtnCode
**
******************************************************************************/
void Test_Init(void);

/*****************************************************************************/
/**
** \brief Test functions to retrieve time values
**
** \par Description
**        This function tests the functions to retrieve time values.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_Print,
** \sa #CFE_TIME_GetMET, #UT_Report, #CFE_TIME_GetMETseconds,
** \sa #CFE_TIME_GetMETsubsecs, #CFE_TIME_Micro2SubSecs,
** \sa #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC,
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**
******************************************************************************/
void Test_GetTime(void);

/*****************************************************************************/
/**
** \brief Test operations on time (add, subtract, compare)
**
** \par Description
**        This function tests the operations on time (add, subtract, compare).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Add, #UT_Report, #CFE_TIME_Subtract,
** \sa #CFE_TIME_Compare
**
******************************************************************************/
void Test_TimeOp(void);

/*****************************************************************************/
/**
** \brief Test time conversion functions
**
** \par Description
**        This function tests the time conversion functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Print, #CFE_TIME_MET2SCTime,
** \sa #UT_Report, #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs
**
******************************************************************************/
void Test_ConvertTime(void);

/*****************************************************************************/
/**
** \brief Test function for converting cFE seconds to file system (FS) seconds
**        and vice versa
**
** \par Description
**        This function tests the function for converting cFE seconds to file
**        system (FS) seconds and vice versa.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TIME_CFE2FSSeconds,
** \sa #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
void Test_ConvertCFEFS(void);

/*****************************************************************************/
/**
** \brief Test function for creating a text string representing the date
**        and time
**
** \par Description
**        This function tests the function for creating a text string
**        representing the date and time.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Print, #UT_Report
**
******************************************************************************/
void Test_Print(void);

/*****************************************************************************/
/**
** \brief Test function for use with register and unregister synch callback
**        API tests
**
** \par Description
**        This function tests the function for use with register and unregister
**        synch callback API tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void ut_time_MyCallbackFunc(void);

/*****************************************************************************/
/**
** \brief Test registering a synchronization callback function
**
** \par Description
**        This function tests the registering a synchronization callback
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] reportResults  Set to TRUE to report test results for calls to
**                           CFE_TIME_RegisterSynchCallback.  Set to FALSE
**                           to perform the function without reporting (this
**                           function is used by other tests so reporting
**                           is only needed once).
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_RegisterSynchCallback, #UT_Report
**
******************************************************************************/
void Test_RegisterSyncCallback(boolean reportResults);

/*****************************************************************************/
/**
** \brief Test registering a synchronization callback function; report results
**
** \par Description
**        This function tests the registering a synchronization callback
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #Test_RegisterSyncCallback
**
******************************************************************************/
void Test_RegisterSyncCallbackTrue(void);

/*****************************************************************************/
/**
** \brief Test external tone signal detection
**
** \par Description
**        This function tests external tone signal detection.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_ExternalTone,
** \sa #UT_Report
**
******************************************************************************/
void Test_ExternalTone(void);

/*****************************************************************************/
/**
** \brief Test setting time data from an external source
**
** \par Description
**        This function tests setting time data from an external source.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_ExternalMET, #UT_Report,
** \sa #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**
******************************************************************************/
void Test_External(void);

/*****************************************************************************/
/**
** \brief Test processing command pipe messages
**
** \par Description
**        This function tests processing command pipe messages.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SendMsg, #UT_Report, #UT_SetBSP_Time
**
******************************************************************************/
void Test_PipeCmds(void);

/*****************************************************************************/
/**
** \brief Test the reset area functionality
**
** \par Description
**        This function tests the reset area functionality.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetStatusBSPResetArea,
** \sa #CFE_TIME_QueryResetVars, #CFE_TIME_UpdateResetVars, #UT_Report
**
******************************************************************************/
void Test_ResetArea(void);

/*****************************************************************************/
/**
** \brief Test time state functions
**
** \par Description
**        This function tests the time state functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_TIME_CalculateState,
** \sa #CFE_TIME_SetState, #CFE_TIME_GetStateFlags
**
******************************************************************************/
void Test_State(void);

/*****************************************************************************/
/**
** \brief Test getting reference data (time at "tone")
**
** \par Description
**        This function tests getting reference data (time at "tone").
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetBSP_Time, #CFE_TIME_GetReference,
** \sa #UT_Report
**
******************************************************************************/
void Test_GetReference(void);

/*****************************************************************************/
/**
** \brief Test send tone, and validate tone and data packet functions
**
** \par Description
**        This function tests the send tone, and validate tone and data packet
**        functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_SetState, #UT_SetBSP_Time,
** \sa #UT_Report, #CFE_TIME_ToneSend, #CFE_TIME_ToneVerify, CFE_TIME_SetState,
**
******************************************************************************/
void Test_Tone(void);

/*****************************************************************************/
/**
** \brief Test the 1Hz STCF adjustment, local interrupt service request and
**        task (non-tone), and tone task functions
**
** \par Description
**        This function tests the 1Hz STCF adjustment, local interrupt service
**        request and task (non-tone), and tone task functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_Set1HzAdj, #CFE_TIME_Local1HzISR,
** \sa #UT_Report, #UT_SetBSP_Time, #UT_SetBinSemFail, #CFE_TIME_Local1HzTask,
** \sa #CFE_TIME_Tone1HzTask
**
******************************************************************************/
void Test_1Hz(void);

/*****************************************************************************/
/**
** \brief Test unregistering synchronization callback function
**
** \par Description
**        This function tests unregistering synchronization callback function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_TIME_UnregisterSynchCallback, #UT_Report
**
******************************************************************************/
void Test_UnregisterSynchCallback(void);

/*****************************************************************************/
/**
** \brief Test function to free resources associated with an application
**
** \par Description
**        This function tests freeing resources associated with an application.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #Test_RegisterSyncCallback,
** \sa #CFE_TIME_CleanUpApp, #CFE_TIME_UnregisterSynchCallback, #UT_Report
**
******************************************************************************/
void Test_CleanUpApp(void);

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
** \sa #CFE_TIME_TaskPipe
**
******************************************************************************/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

#endif /* _time_ut_h_ */
