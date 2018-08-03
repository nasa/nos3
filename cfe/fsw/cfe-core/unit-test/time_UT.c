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
**    time_UT.c
**
** Purpose:
**    Time Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:21:46GMT-05:00 $
** $Revision: 1.7 $
** $Log: time_UT.c  $
** Revision 1.7 2014/05/28 09:21:46GMT-05:00 wmoleski
** Overwriting cFE Unit Test files with the updated JSC files.
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

/*
** Includes
*/
#include "time_UT.h"

/*
** External global variables
*/
extern uint32              UT_AppID;
extern CFE_ES_ResetData_t  *UT_CFE_ES_ResetDataPtr;
extern OS_time_t           BSP_Time;

extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t GetAppIDRtn;
extern UT_SetRtn_t ES_CreateChildRtn;
extern UT_SetRtn_t SB_SubscribeRtn;
extern UT_SetRtn_t SB_SubscribeLocalRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t OS_BinSemCreateRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t WriteSysLogRtn;
extern UT_SetRtn_t SetMsgIdRtn;

/*
** Global variables
*/
int32 ut_time_CallbackCalled;

/*
** Functions
*/
#if (CFE_TIME_CFG_SIGNAL == TRUE)
/*****************************************************************************/
/**
** \brief OS_SelectTone stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_SelectTone.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_SelectTone(int16 Signal)
{
}
#endif

void OS_Application_Startup(void)
{
    /* Initialize unit test */
    UT_Init("time");
    UT_Text("cFE TIME Unit Test Output File\n\n");

    UT_ADD_TEST(Test_Main);
    UT_ADD_TEST(Test_Init);
    UT_ADD_TEST(Test_GetTime);
    UT_ADD_TEST(Test_TimeOp);
    UT_ADD_TEST(Test_ConvertTime);
    UT_ADD_TEST(Test_ConvertCFEFS);
    UT_ADD_TEST(Test_Print);
    UT_ADD_TEST(Test_RegisterSyncCallbackTrue);
    UT_ADD_TEST(Test_ExternalTone);
    UT_ADD_TEST(Test_External);
    UT_ADD_TEST(Test_PipeCmds);
    UT_ADD_TEST(Test_ResetArea);
    UT_ADD_TEST(Test_State);
    UT_ADD_TEST(Test_GetReference);
    UT_ADD_TEST(Test_Tone);
    UT_ADD_TEST(Test_1Hz);
    UT_ADD_TEST(Test_UnregisterSynchCallback);
    UT_ADD_TEST(Test_CleanUpApp);
}

/*
** Test task entry point and main process loop
*/
void Test_Main(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Main\n");
#endif

    /* Test successful run through (a pipe read error is expected) */
    UT_InitData();
    CFE_TIME_TaskMain();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == TIME_SYSLOG_OFFSET + 1,
              "CFE_TIME_TaskMain",
              "Command pipe read error");

    /* Test with an application initialization failure */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1 ,1);
    CFE_TIME_TaskMain();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == TIME_SYSLOG_OFFSET + 2,
              "CFE_TIME_TaskMain",
              "Application initialization fail");
}

/*
** Test API and time task initialization (must be called prior to
** remaining tests)
*/
void Test_Init(void)
{
    int16 ExpRtn;
    int16 SubErrCnt = 0;

#if (CFE_TIME_CFG_SERVER == TRUE)
    int16 SubLocalErrCnt = 0;
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    /* Test successful API initialization */
    UT_InitData();
    ExpRtn = 3;
#if (CFE_TIME_CFG_SERVER == TRUE)
    ExpRtn++;
#endif
#if (CFE_TIME_CFG_SERVER == TRUE)
 #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    ExpRtn++;
 #endif
#endif
#if (CFE_TIME_ENA_1HZ_CMD_PKT == TRUE)
    ExpRtn++;
#endif
    CFE_TIME_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              SetMsgIdRtn.count == ExpRtn,
              "CFE_TIME_EarlyInit",
              "Successful");

    /* Test successful time task initialization */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == CFE_SUCCESS,
              "CFE_TIME_Task_Init",
              "Successful");

    /* Test response to a failure to register an application */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Register application failure");

    /* Test response to a failure creating the first child task */
    UT_InitData();
    UT_SetRtnCode(&ES_CreateChildRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Child task 1 create failure");

    /*  Test response to a failure creating the second child task */
    UT_InitData();
    UT_SetRtnCode(&ES_CreateChildRtn, -3, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -3,
              "CFE_TIME_Task_Init",
              "Child task 2 create failure");

    /* Test response to an error creating a command pipe */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Create pipe failure");

    /* Test response to failure of the HK request subscription */
    UT_InitData();
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -SubErrCnt,
              "CFE_TIME_Task_Init",
              "HK request subscription failure");

    /* Test response to failure of the tone commands subscription */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_TIME_CFG_CLIENT == TRUE)
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    ExpRtn = -SubErrCnt;
#endif

#if (CFE_TIME_CFG_SERVER == TRUE)
    SubLocalErrCnt++;
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -SubLocalErrCnt, SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
#endif

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Tone commands subscription failure (client)");

    /* Test response to failure of the time at the tone "data" commands
     * subscription
     */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_TIME_CFG_CLIENT == TRUE)
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    ExpRtn = -SubErrCnt;
#endif

#if (CFE_TIME_CFG_SERVER == TRUE)
    SubLocalErrCnt++;
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -SubLocalErrCnt, SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
#endif

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time data command subscription failure");

    /* Test response to failure of the fake tone signal commands
     * subscription
     */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_TIME_CFG_FAKE_TONE == TRUE)
 #if (CFE_TIME_CFG_CLIENT == TRUE)
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    ExpRtn = -SubErrCnt;
 #endif

 #if (CFE_TIME_CFG_SERVER == TRUE)
    SubLocalErrCnt++;
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -SubLocalErrCnt, SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
 #endif

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Fake tone signal commands subscription failure");
#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_Task_Init",
              "*Not tested* Fake tone signal commands subscription failure");
#endif

    /* Test response to failure of the time at tone signal commands
     * subscription
     */
    UT_InitData();
#if (CFE_TIME_CFG_SERVER == TRUE && CFE_TIME_CFG_SOURCE != TRUE && \
     CFE_TIME_CFG_FAKE_TONE != TRUE)
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    ExpRtn = -SubErrCnt;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time at tone signal commands subscription failure");
#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_Task_Init",
              "*Not tested* Time at tone signal commands subscription failure");
#endif

    /* Test response to failure of the time task ground commands
     * subscription
     */
    UT_InitData();
    SubErrCnt++;
    UT_SetRtnCode(&SB_SubscribeRtn, -SubErrCnt, SubErrCnt);
    ExpRtn = -SubErrCnt;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time task ground commands subscription failure");

    /* Test response to failure creating a tone semaphore */
    UT_InitData();
    UT_SetRtnCode(&OS_BinSemCreateRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Tone semaphore create failure");

    /* Test response to failure creating a local semaphore */
    UT_InitData();
    UT_SetRtnCode(&OS_BinSemCreateRtn, -2, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -2,
              "CFE_TIME_Task_Init",
              "Local semaphore create failure");

    /* Test response to an EVS register failure */
    UT_InitData();
    UT_SetRtnCode(&EVS_RegisterRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "EVS register failure");

    /* Test response to an error sending an initialization event */
    UT_InitData();
    UT_SetRtnCode(&EVS_SendEventRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Send initialization event error");

    /* Test response to a failure to get the ID by name */
    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimeBaseGetIdByName), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.count == 0 && WriteSysLogRtn.value == -1,
              "CFE_TIME_Task_Init",
              "Get ID by name failure");

    /* Test response to an error setting up the 1Hz callback.
     * Note that this is only a SysLog message, it does not return the
     * error.  This allows the overall system to continue without the 1Hz
     */
    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimerAdd), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.count == 1 && WriteSysLogRtn.value ==
                (TIME_SYSLOG_OFFSET + 3),
              "CFE_TIME_Task_Init",
              "1Hz OS_TimerAdd failure");

    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimerSet), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.count == 1 && WriteSysLogRtn.value == (TIME_SYSLOG_OFFSET + 4),
              "CFE_TIME_Task_Init",
              "1Hz OS_TimerSet failure");
}

/*
** Test functions to retrieve time values
*/
void Test_GetTime(void)
{
    int result;
    uint16 StateFlags, ActFlags;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    /* Note: Time is in seconds + microseconds since 1980-001-00:00:00:00000 */
    /* The time below equals 2013-001-02:03:04.56789 */
    int seconds = 1041472984;
    int microsecs = 567890;
    char *expectedMET = "2013-001-02:03:14.56789";
    char *expectedTAI = "2013-001-03:03:14.56789";
    char *expectedUTC = "2013-001-03:02:42.56789";
    char *expectedSTCF = "1980-001-01:00:00.00000";

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Time\n");
#endif

    /* Test successfully retrieving the mission elapsed time */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_TaskData.AtToneMET.Seconds = 20; /* 20.00000 */
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 3600; /* 01:00:00.00000 */
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLeaps = 32;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0; /* 0.00000 */
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10; /* 10.00000 */
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET; /* Force invalid time */
    CFE_TIME_Print(timeBuf, CFE_TIME_GetMET());
    result = !strcmp(timeBuf, expectedMET);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedMET, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetMET",
              testDesc);

    /* Test successfully retrieving the mission elapsed time (seconds
     * portion)
     */
    UT_InitData();
    seconds = CFE_TIME_GetMETseconds();
    result = BSP_Time.seconds + CFE_TIME_TaskData.AtToneMET.Seconds -
             CFE_TIME_TaskData.AtToneLatch.Seconds;
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, seconds);
    UT_Report(__FILE__, __LINE__,
              result == seconds,
              "CFE_TIME_GetMETseconds",
              testDesc);

    /* Test successfully retrieving the mission elapsed time (sub-seconds
     * portion)
     */
    UT_InitData();
    seconds = CFE_TIME_GetMETsubsecs();
    result = CFE_TIME_Micro2SubSecs(BSP_Time.microsecs) +
             CFE_TIME_TaskData.AtToneMET.Subseconds -
             CFE_TIME_TaskData.AtToneLatch.Subseconds;
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, seconds);
    UT_Report(__FILE__, __LINE__,
              result == seconds,
              "CFE_TIME_GetMETsubsecs",
              testDesc);

    /* Test successfully retrieving the leap seconds */
    UT_InitData();
    seconds = CFE_TIME_GetLeapSeconds();
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d",
             CFE_TIME_TaskData.AtToneLeaps, seconds);
    UT_Report(__FILE__, __LINE__,
              seconds == CFE_TIME_TaskData.AtToneLeaps,
              "CFE_TIME_GetLeapSeconds",
              testDesc);

    /* Test successfully retrieving the international atomic time (TAI) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTAI());
    result = !strcmp(timeBuf, expectedTAI);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedTAI, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetTAI",
              testDesc);

    /* Test successfully retrieving the coordinated universal time (UTC) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetUTC());
    result = !strcmp(timeBuf, expectedUTC);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedUTC, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetUTC",
              testDesc);

    /* Test successfully retrieving the default time (UTC or TAI) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTime());

#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)
    result = !strcmp(timeBuf, expectedTAI);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = TAI) Expected = %s, actual = %s",
             expectedTAI, timeBuf);
#else
    result = !strcmp(timeBuf, expectedUTC);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = UTC) Expected = %s, actual = %s",
             expectedUTC, timeBuf);
#endif
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetTime",
              testDesc);

    /* Test successfully retrieving the spacecraft time correlation
     * factor (SCTF)
     */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetSTCF());
    result = !strcmp(timeBuf, expectedSTCF);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSTCF, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetSTCF",
              testDesc);

    /* Test retrieving the time status (invalid time is expected) */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetClockState() == CFE_TIME_INVALID,
              "CFE_TIME_GetClockState",
              "Invalid time");


    /* Test alternate flag values */
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.Forced2Fly = FALSE;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_SUB_ADJUST;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_SUB_ADJUST;
    CFE_TIME_TaskData.DelayDirection = CFE_TIME_SUB_ADJUST;
    CFE_TIME_TaskData.IsToneGood = FALSE;
    ActFlags = CFE_TIME_GetClockInfo();
    StateFlags = 0;

#if (CFE_TIME_CFG_SERVER == TRUE)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = 0x%04X, actual = 0x%04X", StateFlags, ActFlags);
    UT_Report(__FILE__, __LINE__,
              ActFlags == StateFlags,
              "CFE_TIME_GetClockInfo",
              testDesc);

    /* Test successfully converting the clock state data to flag values */
    UT_InitData();
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_TaskData.Forced2Fly = TRUE;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.DelayDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.IsToneGood = TRUE;
    StateFlags = CFE_TIME_FLAG_CLKSET |
                 CFE_TIME_FLAG_FLYING |
                 CFE_TIME_FLAG_SRCINT |
                 CFE_TIME_FLAG_SIGPRI |
                 CFE_TIME_FLAG_SRVFLY |
                 CFE_TIME_FLAG_CMDFLY |
                 CFE_TIME_FLAG_ADD1HZ |
                 CFE_TIME_FLAG_ADDADJ |
                 CFE_TIME_FLAG_ADDTCL |
                 CFE_TIME_FLAG_GDTONE;

#if (CFE_TIME_CFG_SERVER == TRUE)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    ActFlags = CFE_TIME_GetClockInfo();
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = 0x%04X, actual = 0x%04X", StateFlags, ActFlags);
    UT_Report(__FILE__, __LINE__,
              ActFlags == StateFlags,
              "CFE_TIME_GetClockInfo",
              testDesc);
}

/*
** Test operations on time (add, subtract, compare)
*/
void Test_TimeOp(void)
{
    CFE_TIME_SysTime_t time1, time2, result, exp_result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time Operations\n");
#endif

    /* Initialize to zero time values */
    time1.Subseconds = 0;
    time1.Seconds = 0;
    time2.Subseconds = 0;
    time2.Seconds = 0;
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;

    /* Test adding with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = 0 seconds/subseconds");

    /* Test subtracting with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = 0 seconds/subseconds");

    /* Test comparing with both times equal zero */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = 0 seconds/subseconds");

    /* Initialize to maximum time values */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffffffff;
    time2.Subseconds = 0xffffffff;
    time2.Seconds = 0xffffffff;

    /* Test adding two maximum time values (extreme time rollover case) */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = maximum seconds/subseconds (rollover)");

    /* Test subtracting two maximum time values (zero result) */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = maximum seconds/subseconds (zero result)");

    /* Test comparing two maximum time values */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = maximum seconds/subseconds");

    /* Initialize to single time value at maximum subseconds */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffff0000;
    time2.Subseconds = 0x00000001;
    time2.Seconds = 0x0000ffff;

    /* Test adding two time values; time A > time B (minimal time
     * rollover case)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A > time B (rollover)");

    /* Test subtracting two time values; time A > time B */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xfffe0001;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A > time B");

    /* Test comparing two time values; time A > time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A > time B");

    /* Test adding two time values; time A < time B */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A < time B");

    /* Test subtracting two time values; time A < time B (rollover) */
    UT_InitData();
    exp_result.Subseconds = 0x00000002;
    exp_result.Seconds = 0x0001fffe;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A < time B (rollover)");

    /* Test comparing two time values; time A < time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A < time B");

    /* Initialize so that only subseconds are different; seconds are
     * the same
     */
    time1.Subseconds = 30;
    time1.Seconds = 3;
    time2.Subseconds = 29;
    time2.Seconds = 3;

    /* Test adding two time values; time A subseconds > time B subseconds
     * (seconds same) */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds > time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 1;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds > time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test adding two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds < time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0xffffffff;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Initialize so that only seconds are different; subseconds are
     * the same
     */
    time1.Subseconds = 18;
    time1.Seconds = 8;
    time2.Subseconds = 18;
    time2.Seconds = 7;

    /* Test adding two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 1;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test adding two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds < time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds < time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A seconds < time B seconds (subseconds same)");
}

/*
** Test time conversion functions
*/
void Test_ConvertTime(void)
{
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    CFE_TIME_SysTime_t METTime;

#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)
    /* TAI time derived = MET + STCF */
    char *expectedSCTime = "1980-001-02:00:40.00000";
#else
    /* UTC time derived = MET + STCF - Leaps */
    char *expectedSCTime = "1980-001-02:00:08.00000";
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert Time\n");
#endif

    /* Test MET to SCTF conversion */
    UT_InitData();
    METTime.Seconds = 0;
    METTime.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 7240; /* 01:00:00.00000 */
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLeaps = 32;
    CFE_TIME_Print(timeBuf, CFE_TIME_MET2SCTime(METTime));

    /* SC = MET + SCTF [- Leaps for UTC] */
    result = !strcmp(timeBuf, expectedSCTime);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSCTime, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_MET2SCTime",
              testDesc);

    /* Test subseconds to microseconds conversion; zero subsecond value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0) == 0,
              "CFE_TIME_Sub2MicroSecs",
              "Convert 0 subsecond value");

    /* Test subseconds to microseconds conversion; positive microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0xffff) == 16,
              "CFE_TIME_Sub2MicroSecs",
              "+1 microsecond adjustment");

    /* Test subseconds to microseconds conversion; no microsecond adjust */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0x80000000) == 500000,
              "CFE_TIME_Sub2MicroSecs",
              "No microsecond adjustment");

    /* Test subseconds to microseconds conversion; negative microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0x80002000) == 500001,
              "CFE_TIME_Sub2MicroSecs",
              "-1 microsecond adjustment");

    /* Test subseconds to microseconds conversion; subseconds exceeds
     * microseconds limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0xffffffff) == 999999,
              "CFE_TIME_Sub2MicroSecs",
              "Subseconds exceeds maximum microseconds value (limit ms)");

    /* Test microseconds to subseconds conversion; zero microsecond value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0) == 0,
              "CFE_TIME_Micro2SubSecs",
              "Convert 0 microseconds to 0 subseconds");

    /* Test microseconds to subseconds conversion; no subsecond adjust */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0xffff) == 281468928,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (no subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds below
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(999998) == 0xffffe000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds equals
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(999999) == 0xfffff000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds equals maximum value (subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds exceeds
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0xffffffff) == 0xffffffff,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds exceeds maximum; set maximum subseconds value");
}

/*
** Test function for converting cFE seconds to file system (FS) seconds and
** vice versa
*/
void Test_ConvertCFEFS(void)
{
    uint32 result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert cFE and FS Seconds\n");
#endif

    /* Test cFE to FS conversion using 0 for the cFE seconds value */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_TIME_FS_FACTOR < 0 && -CFE_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = CFE_TIME_FS_FACTOR;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert 0 cFE seconds to FS seconds");

    /* Test cFE to FS conversion using mid-range value for cFE seconds */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_TIME_FS_FACTOR < 0 && -CFE_TIME_FS_FACTOR > 0xffff)
    {
        result = 0;
    }
    else
    {
        result = CFE_TIME_FS_FACTOR + 0xffff;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0xffff) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert mid-range cFE seconds to FS seconds");

    /* Test cFE to FS conversion using the maximum cFE seconds value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0xffffffff) ==
              (uint32) (CFE_TIME_FS_FACTOR - 1),
              "CFE_TIME_CFE2FSSeconds",
              "Maximum cFE seconds value");

    /* Test FS to cFE conversion using 0 for the FS seconds value */
    UT_InitData();

    if (CFE_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = -(uint32) CFE_TIME_FS_FACTOR;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(0) == result,
              "CFE_TIME_FS2CFESeconds",
              "Convert 0 FS seconds to cFE seconds");

    /* Test FS to cFE conversion response to a FS seconds value that results
     * in a negative cFE time (forces cFE seconds to zero)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(CFE_TIME_FS_FACTOR - 1) == 0,
              "CFE_TIME_FS2CFESeconds",
              "Negative cFE seconds conversion (force to zero)");

    /* Test FS to cFE conversion using the minimum convertible FS
     * seconds value
     */
    UT_InitData();

    if (CFE_TIME_FS_FACTOR > (uint32) (CFE_TIME_FS_FACTOR + 1))
    {
        result = 0;
    }
    else
    {
        result = 1;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(CFE_TIME_FS_FACTOR + 1) == result,
              "CFE_TIME_FS2CFESeconds",
              "Minimum convertible FS seconds value");

    /* Test FS to cFE conversion using the maximum FS seconds value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(0xffffffff) == 0xffffffff -
              CFE_TIME_FS_FACTOR,
              "CFE_TIME_FS2CFESeconds",
              "Maximum FS seconds value");
}

/*
** Test function for creating a text string representing the date and time
**
** NOTE: Test results depend on the epoch values in cfe_mission_cfg.h (the
**       tests below assume an epoch of 1980-001-00:00:00.00000).  Full
**       coverage is possible only when CFE_TIME_EPOCH_SECOND > 0
*/
void Test_Print(void)
{
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    CFE_TIME_SysTime_t time;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Print\n");
#endif

    /* Test with zero time value */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 0;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:00.00000");
    strncat(testDesc," Zero time value", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with a time value that causes seconds >= 60 when
     * CFE_TIME_EPOCH_SECOND > 0
     */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 59;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:59.00000");
    strncat(testDesc,
            " Seconds overflow if CFE_TIME_EPOCH_SECOND > 0",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with mission representative time values */
    UT_InitData();
    time.Subseconds = 215000;
    time.Seconds = 1041472984;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2013-001-02:03:04.00005");
    strncat(testDesc," Mission representative time", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with maximum seconds and subseconds values */
    UT_InitData();
    time.Subseconds = 0xffffffff;
    time.Seconds = 0xffffffff;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2116-038-06:28:15.99999");
    strncat(testDesc," Maximum seconds/subseconds values",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);
}

/*
** Test function for use with register and unregister synch callback API tests
*/
void ut_time_MyCallbackFunc(void)
{
    ut_time_CallbackCalled++;
}

/*
** Test registering a synchronization callback function; report results
*/
void Test_RegisterSyncCallbackTrue(void)
{
    Test_RegisterSyncCallback(TRUE);
}

/*
** Test registering a synchronization callback function
*/
void Test_RegisterSyncCallback(boolean reportResults)
{
    uint32  i = 0;
    int32   Result = CFE_SUCCESS;
    boolean SuccessfulTestResult = TRUE;
    boolean UnsuccessfulTestResult = TRUE;
    boolean BadIDResult = TRUE;

#ifdef UT_VERBOSE
    if (reportResults)
    {
        UT_Text("Begin Test Register Synch Callback\n");
    }
#endif


    if (reportResults)
    {
        /* Test registering the callback function with a bad application ID */
        UT_InitData();
        UT_SetRtnCode(&GetAppIDRtn, -1, 1);
        CFE_TIME_TaskData.SynchCallback[0].Ptr = NULL;

        if (CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                                     &ut_time_MyCallbackFunc) != -1)
        {
            BadIDResult = FALSE;
        }
    }

    /* Test registering the callback function the maximum number of times,
     * then attempt registering one more time
     */
    UT_InitData();

    /* Register callback function one too many times to test all cases */
    for (i = 0; i <= CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Result = CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                                &ut_time_MyCallbackFunc);

        if (i < CFE_TIME_MAX_NUM_SYNCH_FUNCS && Result != CFE_SUCCESS)
        {
            SuccessfulTestResult = FALSE;
        }
        else if (i == CFE_TIME_MAX_NUM_SYNCH_FUNCS &&
                 Result != CFE_TIME_TOO_MANY_SYNCH_CALLBACKS)
        {
            UnsuccessfulTestResult = FALSE;
        }
    }

    if (reportResults)
    {
        UT_Report(__FILE__, __LINE__,
              SuccessfulTestResult,
                  "CFE_TIME_RegisterSynchCallback",
                  "Successfully registered callbacks");

        UT_Report(__FILE__, __LINE__,
              UnsuccessfulTestResult,
                  "CFE_TIME_RegisterSynchCallback",
                  "Unsuccessfully registered callback(s)");

        UT_Report(__FILE__, __LINE__,
              BadIDResult,
                  "CFE_TIME_RegisterSynchCallback",
                  "Bad application ID");
    }
}

/*
** Test external tone signal detection
*/
void Test_ExternalTone(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test External Tone\n");
#endif

    UT_InitData();
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_ExternalTone();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_ExternalTone",
              "External tone");
}

/*
** Test setting time data from an external source
*/
void Test_External(void)
{
#if (CFE_TIME_CFG_SRC_MET == TRUE || \
     CFE_TIME_CFG_SRC_GPS == TRUE || \
     CFE_TIME_CFG_SRC_TIME == TRUE)

    CFE_TIME_SysTime_t settime = {5, 4};
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test External Time Set\n");
#endif

#if (CFE_TIME_CFG_SRC_MET == TRUE)
    /* Test setting time data from MET using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and clock state not set");

    /* Test setting time data from MET using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time out of range (low)");

    /* Test setting time data from MET using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time out of range (high)");

    /* Test setting time data from MET (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time in range");

    /* Test setting time data from MET (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET (internal source)");
#endif

#if (CFE_TIME_CFG_SRC_GPS == TRUE)
    /* Test setting time data from GPS using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and clock state not set");

    /* Test setting time data from GPS using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time out of range (low)");

    /* Test setting time data from GPS using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time out of range (high)");

    /* Test setting time data from GPS (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time in range");

    /* Test setting time data from GPS (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalGPS",
              "Not tested* External GPS - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS (internal source)");
#endif

#if (CFE_TIME_CFG_SRC_TIME == TRUE)
    /* Test setting time data from Time using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and clock state not set");

    /* Test setting time data from Time using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time out of range (low)");

    /* Test setting time data from Time using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time out of range (high)");

    /* Test setting time data from Time (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
   CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time in range");

    /* Test setting time data from Time (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalTime",
              "Not tested* External Time - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time (internal source)");
#endif

    /* Reset to normal value for subsequent tests */
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
}

/*
** Test processing command pipe messages
*/
void Test_PipeCmds(void)
{
    CFE_SB_MsgPtr_t msgptr;
    CFE_SB_Msg_t message;
    CFE_TIME_ToneDataCmd_t tonedatacmd;
    CFE_TIME_StateCmd_t statecmd;
    CFE_TIME_SourceCmd_t sourcecmd;
    CFE_TIME_SignalCmd_t signalcmd;
    CFE_TIME_TimeCmd_t timecmd;
    CFE_TIME_LeapsCmd_t leapscmd;
#if (CFE_TIME_CFG_SERVER == TRUE && CFE_TIME_CFG_SOURCE != TRUE && \
     CFE_TIME_CFG_FAKE_TONE != TRUE)
    uint32 count;
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Pipe Commands\n");
#endif

    /* Test sending the housekeeping telemetry request command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_SEND_HK_MID, 0);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_HK_TLM_MID,
              "CFE_TIME_HousekeepingCmd",
              "Housekeeping telemetry request");

    /* Test sending the time at the tone "signal" command */
    UT_InitData();
    CFE_TIME_TaskData.ToneSignalCount = 0;
    UT_SendMsg(&message, CFE_TIME_TONE_CMD_MID, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalCount == 1,
              "CFE_TIME_ToneSignalCmd",
              "Time at tone signal");

    /* Test sending the time at the tone "data" command */
    UT_InitData();
    CFE_TIME_TaskData.ToneDataCount = 0;
    msgptr = (CFE_SB_MsgPtr_t) &tonedatacmd;
    UT_SendMsg(msgptr, CFE_TIME_DATA_CMD_MID, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCount == 1,
              "CFE_TIME_ToneDataCmd",
              "Time at tone data");

    /* Test sending the simulate time at the tone "signal" command */
    UT_InitData();
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    UT_SendMsg(&message, CFE_TIME_FAKE_CMD_MID, 0);

#if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_FakeToneCmd",
              "Simulated time at tone signal");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
              "CFE_TIME_FakeToneCmd",
              "*Command not available* Simulated time at tone signal");
#endif

    /* Test sending the request time at the tone "data" command */
    UT_InitData();

#if (CFE_TIME_CFG_SERVER == TRUE && CFE_TIME_CFG_SOURCE != TRUE && \
     CFE_TIME_CFG_FAKE_TONE != TRUE)
    count = CFE_TIME_TaskData.InternalCount;
#endif

    UT_SendMsg(&message, CFE_TIME_SEND_CMD_MID, 0);

#if (CFE_TIME_CFG_SERVER == TRUE && CFE_TIME_CFG_SOURCE != TRUE && \
     CFE_TIME_CFG_FAKE_TONE != TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value != CFE_TIME_ID_ERR_EID &&
              CFE_TIME_TaskData.InternalCount == count + 1,
              "CFE_TIME_ToneSendCmd",
              "Request time at tone data");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
              "CFE_TIME_ToneSendCmd",
              "*Command not available* Request time at tone data");
#endif

    /* Test sending the no-op command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_NOOP_EID,
              "CFE_TIME_NoopCmd",
              "No-op");

    /* Test sending the reset counters command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_RESET_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_RESET_EID,
              "CFE_TIME_ResetCmd",
              "Reset counters");

    /* Test sending the request diagnostics command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_DIAG_TLM_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DIAG_EID,
              "CFE_TIME_DiagCmd",
              "Request diagnostics");

    /* Test sending a clock state = invalid command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t)&statecmd;
    statecmd.Payload.ClockState = CFE_TIME_INVALID;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = invalid");

    /* Test sending a clock state = valid command */
    UT_InitData();
    statecmd.Payload.ClockState = CFE_TIME_VALID;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = valid");

    /* Test sending a clock state = flywheel command */
    UT_InitData();
    statecmd.Payload.ClockState = CFE_TIME_FLYWHEEL;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = flywheel");

    /* Test response to sending a clock state command using an
     * invalid state
     */
    UT_InitData();
    statecmd.Payload.ClockState = 99;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STATE_ERR_EID,
              "CFE_TIME_SetStateCmd",
              "Invalid clock state");

    /* Test sending the set time source = internal command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &sourcecmd;
    sourcecmd.Payload.TimeSource = CFE_TIME_USE_INTERN;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SOURCE_EID,
              "CFE_TIME_SetSourceCmd",
              "Set internal source");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SOURCE_CFG_EID,
              "CFE_TIME_SetSourceCmd",
              "Set internal source invalid");
#endif

    /* Test sending the set time source = external command */
    UT_InitData();
    sourcecmd.Payload.TimeSource = CFE_TIME_USE_EXTERN;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SOURCE_EID,
              "CFE_TIME_SetSourceCmd",
              "Set external source");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SOURCE_CFG_EID,
              "CFE_TIME_SetSourceCmd",
              "Set external source invalid");
#endif

    /* Test response to sending a set time source command using an
     * invalid source
     */
    UT_InitData();
    sourcecmd.Payload.TimeSource = -1;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SOURCE_ERR_EID,
              "CFE_TIME_SetSourceCmd",
              "Invalid time source");

    /* Test sending a set tone signal source = primary command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &signalcmd;
    signalcmd.Payload.ToneSource = CFE_TIME_TONE_PRI;
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);

#if (CFE_TIME_CFG_SIGNAL == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = primary");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_CFG_EID &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Set tone source = primary invalid");
#endif

    /* Test sending a set tone signal source = redundant command */
    UT_InitData();
    signalcmd.Payload.ToneSource = CFE_TIME_TONE_RED;
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);

#if (CFE_TIME_CFG_SIGNAL == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_CFG_EID &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant invalid");
#endif

    /* Test response to sending a set tone signal source command using an
     * invalid source
     */
    UT_InitData();
    signalcmd.Payload.ToneSource = -1;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_ERR_EID &&
            CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Invalid tone source");

    /* Test sending a time tone add delay command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &timecmd;
    timecmd.Payload.MicroSeconds = 0;
    timecmd.Payload.Seconds = 0;
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_DELAY_CC);

#if (CFE_TIME_CFG_CLIENT == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELAY_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELAY_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay invalid");
#endif

    /* Test sending a time tone subtract delay command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_DELAY_CC);

#if (CFE_TIME_CFG_CLIENT == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELAY_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetDelayCmd",
              "Set tone subtract delay");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELAY_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetDelayCmd",
              "Set subtract delay invalid");
#endif

    /* Test sending a set time command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_TIME_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_TIME_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetTimeCmd",
              "Set time");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_TIME_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetTimeCmd",
              "Set time invalid");
#endif

    /* Test sending a set MET command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_MET_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_MET_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetMETCmd",
              "Set MET");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_MET_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetMETCmd",
              "Set MET invalid");
#endif

    /* Test sending a set STCF command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STCF_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STCF_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetSTCFCmd",
              "Set STCF");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STCF_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetSTCFCmd",
              "Set STCF invalid");
#endif

    /* Test sending an adjust STCF positive command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_ADJUST_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELTA_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust positive");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELTA_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust positive invalid");
#endif

    /* Test sending an adjust STCF negative command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_ADJUST_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELTA_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust negative");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELTA_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust negative invalid");
#endif

    /* Test sending an adjust STCF 1 Hz positive command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_1HZADJ_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_1HZ_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust positive");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_1HZ_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust positive invalid");
#endif

    /* Test sending an adjust STCF 1 Hz negative command */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_1HZADJ_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_1HZ_EID &&
              CFE_TIME_TaskData.CmdCounter == 1 &&
              CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust negative");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_1HZ_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust negative invalid");
#endif

    /* Test response to sending a tone delay command using an invalid time */
    UT_InitData();
    timecmd.Payload.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_DELAY_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELAY_ERR_EID,
              "CFE_TIME_SetDelayCmd",
              "Invalid tone delay");

    /* Test response to sending a set time command using an invalid time */
    UT_InitData();
    timecmd.Payload.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_TIME_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_TIME_ERR_EID,
              "CFE_TIME_SetTimeCmd",
              "Invalid time");

    /* Test response to sending a set MET command using an invalid time */
    UT_InitData();
    timecmd.Payload.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_MET_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_MET_ERR_EID,
              "CFE_TIME_SetMETCmd",
              "Invalid MET");

    /* Test response to sending a set STCF command using an invalid time */
    UT_InitData();
    timecmd.Payload.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STCF_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_STCF_ERR_EID,
              "CFE_TIME_SetSTCFCmd",
              "Invalid STCF");

    /* Test response to sending an adjust STCF command using an invalid time */
    UT_InitData();
    timecmd.Payload.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_ADJUST_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_DELTA_ERR_EID,
              "CFE_TIME_AdjustCmd",
              "Invalid STCF adjust");

    /* Test sending a set leap seconds commands */
    UT_InitData();
    CFE_TIME_TaskData.CmdCounter = 0;
    CFE_TIME_TaskData.ErrCounter = 0;
    msgptr = (CFE_SB_MsgPtr_t) &leapscmd;
    leapscmd.Payload.LeapSeconds = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_LEAPS_CC);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_LEAPS_EID &&
            CFE_TIME_TaskData.CmdCounter == 1 &&
            CFE_TIME_TaskData.ErrCounter == 0,
              "CFE_TIME_SetLeapsCmd",
              "Set leap seconds");
#else
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_LEAPS_CFG_EID &&
              CFE_TIME_TaskData.CmdCounter == 0 &&
              CFE_TIME_TaskData.ErrCounter == 1,
              "CFE_TIME_SetLeapsCmd",
              "Set leap seconds invalid");
#endif

    /* Test response to sending an invalid command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, 0xffff);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_CC_ERR_EID,
              "CFE_TIME_TaskPipe",
              "Invalid command code");

    /* Test response to sending a command using an invalid message ID */
    UT_InitData();
    UT_SendMsg(&message, 0xffff, 0);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
              "CFE_TIME_TaskPipe",
              "Invalid message ID");
}

/*
** Test the reset area functionality
*/
void Test_ResetArea(void)
{
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Reset Area\n");
#endif

    /* Test successfully updating the reset area */
    UT_InitData();
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal ==
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Successful update");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone PRI");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_RED);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone RED");

    /* Test response to a bad reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_BAD,
              "CFE_TIME_QueryResetVars",
              "Reset area error");

    /* Test initializing to default time values */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE + 1,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_NEW,
              "CFE_TIME_QueryResetVars",
              "Initialize to default values");

    /* Test response to a bad clock signal selection parameter */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_RED+1);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_NEW,
              "CFE_TIME_QueryResetVars",
              "Bad clock signal selection");

    /* Test response to a reset area error */
    UT_InitData();
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_ERROR;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal !=
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Reset area error");

    /* Test failure to get reset area updating the reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(CFE_PSP_ERROR, 0, CFE_TIME_TONE_PRI);
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal !=
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Get reset area fail");
}

/*
** Test time state functions
*/
void Test_State(void)
{
    uint16 flag;
    int16  ExpState;
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time State\n");
#endif

    /* Test determining if the clock state is valid with the server state
     * in "no flywheel"
     */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_WAS_SET;
    Reference.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
    ExpState = CFE_TIME_VALID;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == ExpState,
              "CFE_TIME_CalculateState",
              "Valid time state; server state - no flywheel");

    /* Test determining if the clock state is valid with the server state
     * in "flywheel"
     */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_WAS_SET;
    Reference.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
#if (CFE_TIME_CFG_CLIENT == TRUE)
    ExpState = CFE_TIME_FLYWHEEL;
#else
    ExpState = CFE_TIME_VALID;
#endif
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == ExpState,
              "CFE_TIME_CalculateState",
              "Valid time state; server state - flywheel");

    /* Test determining if the clock state = flywheel */
    UT_InitData();
    Reference.ClockFlyState = CFE_TIME_IS_FLY;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == CFE_TIME_FLYWHEEL,
              "CFE_TIME_CalculateState",
              "Flywheel time state");

    /* Test determining if the clock state = invalid */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_INVALID;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == CFE_TIME_INVALID,
              "CFE_TIME_CalculateState",
              "Invalid time state");

    /* Test converting state data to flag values */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_VALID);
    CFE_TIME_SetState(CFE_TIME_FLYWHEEL);
    flag = CFE_TIME_GetClockInfo();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetStateFlags() == flag,
              "CFE_TIME_GetStateFlags",
              "State data to flag conversion");

    /* Test alternate flag values */
    UT_InitData();

    /* Add server flag depending on configuration */
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.Forced2Fly = FALSE;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_SUB_ADJUST;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_SUB_ADJUST;
    CFE_TIME_TaskData.DelayDirection = CFE_TIME_SUB_ADJUST;
    flag = CFE_TIME_GetClockInfo();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetStateFlags() == flag,
              "CFE_TIME_GetStateFlags",
              "State data with alternate flags");

    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
}

/*
** Test getting reference data (time at "tone")
*/
void Test_GetReference(void)
{
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Reference\n");
#endif

    /* Test with local clock rollover */
    UT_InitData();
    CFE_TIME_TaskData.AtToneMET.Seconds = 20;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 1000;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + MaxLocalClock - AtToneLatch +
     *              BSP_Time [+ or - AtToneDelay]
     */
    UT_Report(__FILE__, __LINE__,
              Reference.CurrentMET.Seconds == 1010 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock < latch at tone time");

    /* Test without local clock rollover */
    UT_InitData();
    CFE_TIME_TaskData.AtToneMET.Seconds = 20;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(15, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + BSP_Time - AtToneLatch [+ or - AtToneDelay]
     */
    UT_Report(__FILE__, __LINE__,
              Reference.CurrentMET.Seconds == 25 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock > latch at tone time");
}

/*
** Test send tone, and validate tone and data packet functions
*/
void Test_Tone(void)
{
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;

#if (CFE_TIME_CFG_SERVER == TRUE)
    int seconds = 7654321;
    int virtualSeconds = 1234567;
#endif

    uint32 MinElapsed = CFE_TIME_TaskData.MinElapsed;
    uint32 MaxElapsed = CFE_TIME_TaskData.MaxElapsed;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Tone\n");
#endif

    /* Test time at the tone in flywheel mode */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_FLYWHEEL);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_SetBSP_Time(seconds, 0);
    CFE_TIME_TaskData.AtToneMET.Seconds = 0; /* 20.00000 */
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0; /* 0.00000 */
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0; /* 10.00000 */
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.VirtualMET = virtualSeconds;
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds = seconds;
    CFE_TIME_ToneSend();

 #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    seconds++;
 #endif

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds == seconds &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel ON");

    /* Test time at the tone when not in flywheel mode */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_ToneSend();

 #if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    virtualSeconds++;
 #endif

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds ==
                  virtualSeconds &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel OFF");

#else
    /* Added to prevent a missing test */
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel ON");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel OFF"); /* Added to prevent a missing test # */
#endif

    time1.Seconds = 10;
    time1.Subseconds = 0;
    time2.Seconds = 10;
    time2.Subseconds = 100;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * FALSE.  This test also serves to set the internal PrevTime variables
     * to known values
     */
    UT_InitData();
    CFE_TIME_TaskData.VersionCount = 0; /* Verifies 'ForcedToFly' path */
    CFE_TIME_TaskData.ToneMatchCount = 0;
    CFE_TIME_TaskData.Forced2Fly = FALSE; /* Exercises '!ForcedToFly' path */
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.VersionCount == 1 &&
              CFE_TIME_TaskData.ToneMatchCount == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 < time 2, Forced2Fly FALSE");

    /* Test tone validation when time 1 equals the previous time 1 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time1, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 same as previous time 1");

    /* Test tone validation when time 2 equals the previous time 2 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Time 2 same as previous time 2");

    /* Test tone validation with time 1 > time 2 value (clock rollover) */
    UT_InitData();
    time1.Seconds = 20;
    time2.Seconds = 0;
    time1.Subseconds = 0;
    time2.Subseconds = 100;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 20; /* 1000.00000 */
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchCount = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchCount == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 > time 2 (clock rollover)");

    /* Test tone validation when time between packet and tone is out of
     * limits (seconds)
     */
    UT_InitData();
    time1.Seconds = 10;
    time2.Seconds = 11;
    time1.Subseconds = 0;
    time2.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (seconds)");

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds low)
     */
    UT_InitData();
    time1.Seconds = 12;
    time2.Seconds = 12;
    time1.Subseconds = 0;
    time2.Subseconds = 10;
    CFE_TIME_TaskData.MinElapsed = 20;
    CFE_TIME_TaskData.MaxElapsed = 30;
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (subseconds low)");

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds high)
     */
    UT_InitData();
    time1.Seconds = 13;
    time2.Seconds = 13;
    time1.Subseconds = 0;
    time2.Subseconds = 40;
    CFE_TIME_TaskData.MinElapsed = 20;
    CFE_TIME_TaskData.MaxElapsed = 30;
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (subseconds high)");

    CFE_TIME_TaskData.MinElapsed = MinElapsed;
    CFE_TIME_TaskData.MaxElapsed = MaxElapsed;

    time1.Seconds = 10;
    time1.Subseconds = 0;
    time2.Seconds = 10;
    time2.Subseconds = 100;

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * FALSE and the clock source is external
     */
    UT_InitData();
    CFE_TIME_TaskData.VersionCount = 0; /* Verifies 'ForcedToFly' path */
    CFE_TIME_TaskData.ToneMatchCount = 0;
    CFE_TIME_TaskData.Forced2Fly = FALSE; /* Exercises '!ForcedToFly' path */
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.VirtualMET = 5;
    CFE_TIME_TaskData.PendingMET.Seconds = CFE_TIME_TaskData.VirtualMET;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.VersionCount == 1 &&
              CFE_TIME_TaskData.ToneMatchCount == 1 &&
              CFE_TIME_TaskData.VirtualMET == 5,
              "CFE_TIME_ToneVerify",
              "Time 1 < time 2, Forced2Fly FALSE, Clock EXTERN");

    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;

#if (CFE_TIME_CFG_CLIENT == TRUE)
    /* Test tone update using an invalid pending state */
    UT_InitData();
    CFE_TIME_TaskData.PendingState = CFE_TIME_INVALID;
    CFE_TIME_TaskData.ClockSetState  = CFE_TIME_WAS_SET;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_ToneUpdate();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ClockSetState == CFE_TIME_NOT_SET &&
              CFE_TIME_TaskData.ServerFlyState == CFE_TIME_NO_FLY,
              "CFE_TIME_ToneUpdate",
              "Invalid pending state");

    /* Test tone update using FLYWHEEL as the pending state */
    UT_InitData();
    CFE_TIME_TaskData.PendingState = CFE_TIME_FLYWHEEL;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_ToneUpdate();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ServerFlyState == CFE_TIME_IS_FLY,
              "CFE_TIME_ToneUpdate",
              "Pending state is FLYWHEEL");

#else
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ToneUpdate",
              "*Not tested* Invalid pending state");

    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_ToneUpdate",
              "*Not tested* Pending state is FLYWHEEL");
#endif
}

/*
** Test the 1Hz STCF adjustment, local interrupt service request and task
** (non-tone), and tone task functions
*/
void Test_1Hz(void)
{
    uint16             Arg;
    uint32             delSec = 3;
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;

#if (CFE_TIME_CFG_SERVER == TRUE)
    /* Test 1Hz STCF adjustment in positive direction */
    UT_InitData();
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 20;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 60;
    time1.Seconds = 10;
    time1.Subseconds = 30;
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_ADD_ADJUST);
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AtToneSTCF.Seconds == 30 &&
              CFE_TIME_TaskData.AtToneSTCF.Subseconds == 90,
              "CFE_TIME_Set1HzAdj",
              "Positive adjustment");

    /* Test 1Hz STCF adjustment in negative direction */
    UT_InitData();
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_SUB_ADJUST);
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AtToneSTCF.Seconds == 20 &&
              CFE_TIME_TaskData.AtToneSTCF.Subseconds == 60,
              "CFE_TIME_Set1HzAdj",
              "Negative adjustment");
#else
    /* These prevent missing tests when CFE_TIME_CFG_SERVER is FALSE */
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Positive adjustment");
    UT_Report(__FILE__, __LINE__,
              TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Negative adjustment");
#endif

    /* Test local 1Hz interrupt when enough time has elapsed (seconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.AutoStartFly = FALSE;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 1;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 1;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.MaxLocalClock.Seconds = CFE_TIME_CFG_LATCH_FLY - 1;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AutoStartFly == TRUE,
              "CFE_TIME_Local1HzISR",
              "Auto start flywheel (seconds)");

    /* Test local 1Hz interrupt when enough time has elapsed since receiving a
     * time update to automatically update the MET
     */
    UT_InitData();
    time1.Seconds = 11;
    time2.Seconds = 1;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = time1.Seconds;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = time2.Seconds;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.MaxLocalClock.Seconds = CFE_TIME_CFG_LATCH_FLY + delSec;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AtToneMET.Seconds == time1.Seconds +
                                                     CFE_TIME_CFG_LATCH_FLY +
                                                     delSec - time2.Seconds &&
              CFE_TIME_TaskData.AtToneLatch.Seconds == 0,
              "CFE_TIME_Local1HzISR",
              "Auto update MET");

    /* Test the tone signal ISR when the tone doesn't occur ~1 second after
     * the previous one
     */
    UT_InitData();
    CFE_TIME_TaskData.IsToneGood = TRUE;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 1;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == FALSE,
              "CFE_TIME_Tone1HzISR",
              "Invalid tone signal interrupt");

    /* Test the tone signal ISR call to the application synch callback
     * function by verifying the number of callbacks made matches the number
     * of registered callbacks
     */
    UT_InitData();
    UT_SetBSP_Time(1, 0);
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              ut_time_CallbackCalled == CFE_TIME_MAX_NUM_SYNCH_FUNCS,
              "CFE_TIME_Tone1HzISR",
              "Proper number of callbacks made");

    /* Test the local 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.AutoStartFly = TRUE;
    CFE_TIME_TaskData.LocalTaskCount = 0;
    UT_SetBinSemFail(2);
    CFE_TIME_Local1HzTask();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.LocalTaskCount == 1,
              "CFE_TIME_Local1HzTask",
              "Semaphore creation pass, then fail");

    /* Test the tone 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.ToneTaskCount = 0;
    UT_SetBinSemFail(2);
    CFE_TIME_Tone1HzTask();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneTaskCount == 1,
              "CFE_TIME_Tone1HzTask",
              "Semaphore creation pass, then fail");

    /* Test the tone 1Hz task with the tone signal over the time limit */
    UT_InitData();
    UT_SetBSP_Time(1, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneOverLimit));
    CFE_TIME_TaskData.IsToneGood = TRUE;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == FALSE,
              "CFE_TIME_Tone1HzISR",
              "Invalid tone signal interrupt; tolerance over limit");

    /* Test the tone 1Hz task with the tone signal within the time limits */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneUnderLimit) + 1);
    CFE_TIME_TaskData.IsToneGood = FALSE;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == TRUE,
              "CFE_TIME_Tone1HzISR",
              "Valid tone signal interrupt, tolerance in limits");

    /* Test the tone 1Hz task with the tone signal under the time limit */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneUnderLimit) - 1);
    CFE_TIME_TaskData.IsToneGood = TRUE;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == FALSE,
              "CFE_TIME_Tone1HzISR",
              "Valid tone signal interrupt, tolerance under limits");

    /* Test local 1Hz interrupt when enough time has elapsed (subseconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.AutoStartFly = FALSE;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 1;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 1;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AutoStartFly == TRUE,
              "CFE_TIME_Local1HzISR",
              "Auto start flywheel (subseconds)");

    /* Test local 1Hz interrupt when enough time has not elapsed since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.VersionCount = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.VersionCount == 0,
              "CFE_TIME_Local1HzISR",
              "Do not auto start flywheel");

    /* Test the local 1Hz task where auto start flywheel is disabled */
    UT_InitData();
    CFE_TIME_TaskData.LocalTaskCount = 0;
    CFE_TIME_TaskData.AutoStartFly = FALSE;
    UT_SetBinSemFail(2);
    CFE_TIME_Local1HzTask();
    UT_Report(__FILE__, __LINE__,
             CFE_TIME_TaskData.LocalTaskCount == 1 &&
              !UT_EventIsInHistory(CFE_TIME_FLY_ON_EID),
              "CFE_TIME_Local1HzTask",
              "Do not auto start flywheel");

    /* Test the CFE_TIME_Local1HzTimerCallback function */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.VersionCount = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzTimerCallback(123, &Arg);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.VersionCount == 0,
              "CFE_TIME_Local1HzTimerCallback",
              "Pass through to CFE_TIME_Local1HzISR");
}

/*
** Test unregistering synchronization callback function
*/
void Test_UnregisterSynchCallback(void)
{
    uint32  i = 0;
    int32   Result = CFE_SUCCESS;
    boolean SuccessfulTestResult = TRUE;
    boolean UnsuccessfulTestResult = TRUE;
    boolean BadIDResult = TRUE;

    ut_time_CallbackCalled = 0;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregister Callback\n");
#endif

    /* Unregister callback function one too many times to test both valid and
     * invalid cases
     */
    UT_InitData();

    for (i = 0; i <= CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Result =
            CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                             &ut_time_MyCallbackFunc);

        if (i < CFE_TIME_MAX_NUM_SYNCH_FUNCS && Result != CFE_SUCCESS)
        {
            SuccessfulTestResult = FALSE;
        }
        else if (i == CFE_TIME_MAX_NUM_SYNCH_FUNCS &&
                 Result != CFE_TIME_CALLBACK_NOT_REGISTERED)
        {
            UnsuccessfulTestResult = FALSE;
        }
    }

    UT_Report(__FILE__, __LINE__,
              SuccessfulTestResult,
              "CFE_TIME_UnregisterSynchCallback",
              "Successfully unregister callback");

    UT_Report(__FILE__, __LINE__,
              UnsuccessfulTestResult,
              "CFE_TIME_UnregisterSynchCallback",
              "Unsuccessful unregister callback");

    /* Test unregistering the callback function with a bad application ID */
    UT_InitData();
    UT_SetRtnCode(&GetAppIDRtn, -1, 1);
    CFE_TIME_TaskData.SynchCallback[0].Ptr = NULL;

    if (CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                         &ut_time_MyCallbackFunc) != -1)
    {
        BadIDResult = FALSE;
    }

    UT_Report(__FILE__, __LINE__,
              BadIDResult,
              "CFE_TIME_UnregisterSynchCallback",
              "Bad application ID");

    CFE_TIME_TaskData.SynchCallback[0].App = -1;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
              &ut_time_MyCallbackFunc) == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_UnregisterSynchCallback",
              "Unknown synch callback");

    /* Test tone notification with an invalid time synch application */
    UT_InitData();
    CFE_TIME_TaskData.IsToneGood = TRUE;
    CFE_TIME_TaskData.SynchCallback[0].Ptr = NULL;
    CFE_TIME_NotifyTimeSynchApps();
    UT_Report(__FILE__, __LINE__,
              ut_time_CallbackCalled == 0,
              "CFE_TIME_NotifyTimeSynchApps",
              "Invalid time synch application");
}

/*
** Test function to free resources associated with an application
*/
void Test_CleanUpApp(void)
{
    uint16   i;
    uint16 Status1 = 0;
    int32  Status2 = CFE_SUCCESS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Cleanup App\n");
#endif

    UT_InitData();

    /* Add multiple entries into callback registry table */
    Test_RegisterSyncCallback(FALSE);

    /* Clean up callbacks */
    CFE_TIME_CleanUpApp(UT_AppID);

    for (i = 0; i < CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Status1 += CFE_TIME_TaskData.SynchCallback[i].App;
    }

    /* Try to unregister an entry again to make sure all are gone */
    Status2 = CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                               &ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
              Status1 == 0 && Status2 == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_CleanUpApp",
              "Removed all callback entries for app");


    /* Test response to a bad application ID */
    CFE_TIME_TaskData.SynchCallback[0].App = -1;
    CFE_TIME_CleanUpApp(UT_AppID);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.SynchCallback[0].App != 0,
              "CFE_TIME_CleanUpApp",
              "Bad application ID");
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
  CFE_TIME_TaskPipe(MsgPtr);
}
