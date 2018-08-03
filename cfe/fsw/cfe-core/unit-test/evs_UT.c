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
**    evs_UT.c
**
** Purpose:
**    Event Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/30 13:20:34GMT-05:00 $
** $Revision: 1.8 $
**
*/

/*
** Includes
*/
#include "evs_UT.h"

/*
** External global variables
*/
extern UT_SetRtn_t MutSemCreateRtn;
extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SB_SubscribeExRtn;
extern UT_SetRtn_t GetAppIDRtn;
extern UT_SetRtn_t GetResetTypeRtn;
extern UT_SetRtn_t WriteSysLogRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t OSPrintRtn;
extern UT_SetRtn_t FSWriteHdrRtn;

/*
** Functions
*/
void OS_Application_Startup(void)
{
    /* Initialize unit test */
    UT_Init("evs");
    UT_Text("cFE EVS Unit Test Output File\n\n");

    /* Test_Init is a test but also MUST be called first and only once */
    UT_ADD_TEST(Test_Init);
    UT_ADD_TEST(Test_IllegalAppID);
    UT_ADD_TEST(Test_UnregisteredApp);
    UT_ADD_TEST(Test_FilterRegistration);
    UT_ADD_TEST(Test_FilterReset);
    UT_ADD_TEST(Test_Format);
    UT_ADD_TEST(Test_Ports);
    UT_ADD_TEST(Test_Logging);
    UT_ADD_TEST(Test_WriteApp);
    UT_ADD_TEST(Test_BadAppCmd);
    UT_ADD_TEST(Test_EventCmd);
    UT_ADD_TEST(Test_FilterCmd);
    UT_ADD_TEST(Test_InvalidCmd);
    UT_ADD_TEST(Test_Misc);
}

/*
** Initialization test, must be called first
*/
void Test_Init(void)
{
    CFE_EVS_BitMaskCmd_t        bitmaskcmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    strncpy((char *) appbitcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appbitcmd.Payload.AppName));

    /* Test successful early initialization of the cFE EVS */
    UT_InitData();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_PSP_RST_TYPE_POWERON, 1);
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 4,
              "CFE_EVS_EarlyInit",
              "Early initialization successful");

    /* Test TaskMain with a command pipe read failure due to an
     * invalid command packet
     */
    UT_InitData();
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 1);
    CFE_EVS_TaskMain();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 8 &&
              SendMsgEventIDRtn.value == CFE_EVS_ERR_MSGID_EID,
              "CFE_EVS_TaskMain",
              "Error reading command pipe");

    /* Test TaskMain with a register application failure */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1 ,1);
    CFE_EVS_TaskMain();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 7 &&
              WriteSysLogRtn.count == 2,
              "CFE_EVS_TaskMain",
              "Application initialization failure");

    /* Test early initialization with a get reset area failure */
    UT_InitData();
    UT_SetStatusBSPResetArea(-1, CFE_TIME_RESET_SIGNATURE, CFE_TIME_TONE_PRI);
    CFE_EVS_EarlyInit();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 1,
              "CFE_EVS_EarlyInit",
              "CFE_PSP_GetResetArea call failure");

    /* Test early initialization, restoring the event log */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 6,
              "CFE_EVS_EarlyInit",
              "Event log restored");


    /* Test early initialization, clearing the event log (log mode path) */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_LOG_OVERWRITE +
                                             CFE_EVS_LOG_DISCARD + 1;
    CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = FALSE;
    CFE_EVS_GlobalData.EVS_LogPtr->Next = CFE_EVS_LOG_MAX - 1;
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 5,
              "CFE_EVS_EarlyInit",
              "Event log cleared (log mode path)");

    /* Test early initialization, clearing the event log (log full path) */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_LOG_DISCARD;
    CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = 2;
    CFE_EVS_GlobalData.EVS_LogPtr->Next = CFE_EVS_LOG_MAX - 1;
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 5,
              "CFE_EVS_EarlyInit",
              "Event log cleared (log full path)");

    /* Test early initialization, clearing the event log (next log path) */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_LOG_OVERWRITE;
    CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag = TRUE;
    CFE_EVS_GlobalData.EVS_LogPtr->Next = CFE_EVS_LOG_MAX;
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 5,
              "CFE_EVS_EarlyInit",
              "Event log cleared (next log path)");

    /* Test early initialization with a mutex creation failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, -1, 1);
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 3,
              "CFE_EVS_EarlyInit",
              "Mutex create failure");

    /* Test early initialization with an unexpected size returned
     * by CFE_PSP_GetResetArea
     */
    UT_InitData();
    UT_SetSizeofESResetArea(0);
    CFE_EVS_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 2,
              "CFE_EVS_EarlyInit",
              "Unexpected size returned by CFE_PSP_GetResetArea");

    /* Test task initialization where the application registration fails */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 9,
              "CFE_EVS_TaskInit",
              "Call to CFE_ES_RegisterApp failure");

    /* Test task initialization where the pipe creation fails */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 12,
              "CFE_EVS_TaskInit",
              "Call to CFE_SB_CreatePipe failure");

    /* Test task initialization where command subscription fails */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 13,
              "CFE_EVS_TaskInit",
              "Subscribing to commands failure");

    /* Test task initialization where HK request subscription fails */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -1, 2);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 14,
              "CFE_EVS_TaskInit",
              "Subscribing to HK request failure");

    /* Test task initialization where getting the application ID fails */
    UT_InitData();
    UT_SetAppID(10000);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 10,
              "CFE_EVS_TaskInit",
              "Call to CFE_ES_GetAppID Failed");

    /* Test successful task initialization */
    UT_InitData();
    UT_SetAppID(0);
    CFE_EVS_TaskInit();
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == -1,
              "CFE_EVS_TaskInit",
              "Normal init (WARM)");

    /* Enable DEBUG message output */
    UT_InitData();
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output");

    /* Disable ports */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Disable ports");
}

/*
** Test functions using an illegal application ID
*/
void Test_IllegalAppID(void)
{
    CFE_TIME_SysTime_t time = {0, 0};
    uint32 AppID;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Illegal App ID\n");
#endif

    /* Set test up with illegal application ID */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS + 1);

    /* Test registering an event using an illegal application ID */
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, 0) == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_Register",
              "Illegal app ID");

    /* Test unregistering an event using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Unregister() == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_Unregister",
              "Illegal app ID");

    /* Test sending an event using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0, 0, "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendEvent",
              "Illegal app ID");

    /* Test sending an event using an illegal application ID */
    UT_InitData();
    AppID = CFE_EVS_GlobalData.EVS_AppID;
    CFE_EVS_GlobalData.EVS_AppID = CFE_ES_MAX_APPLICATIONS;
    UT_Report(__FILE__, __LINE__,
              EVS_SendEvent(0, 0, "NULL") == CFE_SUCCESS,
              "EVS_SendEvent",
              "Illegal app ID");
    CFE_EVS_GlobalData.EVS_AppID = AppID;

    /* Test sending a timed event using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendTimedEvent(time,
                                     0,
                                     0,
                                     "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendTimedEvent",
              "Illegal app ID");

    /* Test sending an event with app ID using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEventWithAppID(0,
                                         0,
                                         CFE_ES_MAX_APPLICATIONS + 1,
                                         "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendEventWithAppID",
              "Illegal app ID");

    /* Test resetting a filter using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetFilter(0) == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_ResetFilter",
              "Illegal app ID");

    /* Test resetting all filters using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetAllFilters() == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_ResetAllFilters",
              "Illegal app ID");

    /* Test application cleanup using an illegal application ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_CleanUpApp(CFE_ES_MAX_APPLICATIONS + 1) ==
                  CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_CleanUpApp",
              "Illegal app ID");

    /* Return application ID to valid value */
    UT_SetAppID(0);
}

/*
** Test functions using an unregistered application
*/
void Test_UnregisteredApp(void)
{
    CFE_TIME_SysTime_t time = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregistered App\n");
#endif

    UT_InitData();

    /* Unregister the application (it was registered in CFE_EVS_TaskInit) */
    CFE_EVS_Unregister();

    /* Test unregistering an already unregistered application */
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Unregister() == CFE_SUCCESS,
              "CFE_EVS_Unregister","App not registered");

    /* Test sending an event to an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0, 0, "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendEvent",
              "App not registered");

    /* Test resetting a filter using an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetFilter(0) == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_ResetFilter",
              "App not registered");

    /* Test resetting all filters using an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetAllFilters() == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_ResetAllFilters",
              "App not registered");

    /* Test sending an event with app ID to an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEventWithAppID(0,
                                         CFE_EVS_INFORMATION,
                                         0,
                                         "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendEventWithAppID",
              "App not registered");

    /* Test sending a timed event to an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendTimedEvent(time,
                                     CFE_EVS_INFORMATION,
                                     0,
                                     "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendTimedEvent",
              "App not registered");

    /* Test application cleanup using an unregistered application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_CleanUpApp(0) == CFE_SUCCESS,
              "CFE_EVS_CleanUpApp",
              "App not registered");

    /* Re-register the application for subsequent tests */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register app - successful");
}

/*
** Test filter registration related calls
*/
void Test_FilterRegistration(void)
{
    int i;

    CFE_EVS_BinFilter_t filter[CFE_EVS_MAX_EVENT_FILTERS + 1];
    EVS_BinFilter_t     *FilterPtr = NULL;
    uint32              AppID;
    CFE_TIME_SysTime_t  time = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Registration\n");
#endif

    /* Test filter registration using an invalid filter option */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL,
                               0,
                               CFE_EVS_BINARY_FILTER + 1) ==
                  CFE_EVS_UNKNOWN_FILTER,
              "CFE_EVS_Register","Illegal filter option");

    /* Test successful filter registration with no filters */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid w/ no filters");

    /* Test filter unregistration with failed ES_putPool */
    UT_InitData();
    UT_SetPutPoolFail(1);
    UT_SetRtnCode(&GetAppIDRtn, -1, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Unregister() < 0, "CFE_EVS_Unregister",
              "Unregistration with failed ES_putPool");

    /* Re-register to test valid unregistration */
    UT_InitData();
    UT_SetPutPoolFail(0);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid with no filters (re-registration)");

    /* Test successful filter unregistration */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Unregister() == CFE_SUCCESS,
              "CFE_EVS_Unregister",
              "Valid unregistration");

    /* Test successful filter registration with a valid filter */
    UT_InitData();
    filter[0].EventID = 0;
    filter[0].Mask = 0x0001;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(filter,
                               1,
                               CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid w/ filter");

    /* Test successful multiple filter registration with valid filters */
    UT_InitData();

    for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS + 1; i++)
    {
        filter[i].EventID = i;
        filter[i].Mask = 1;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(filter, CFE_EVS_MAX_EVENT_FILTERS + 1,
              CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid over max filters");

    /* Send 1st information message, should get through */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "OK") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "1st info message should go through");

    /* Send 2nd information message, should be filtered */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "2nd info message should be filtered");

    /* Send last information message, which should cause filtering to lock */
    UT_InitData();
    CFE_ES_GetAppID(&AppID);
    FilterPtr = EVS_FindEventID(0,
        (EVS_BinFilter_t *) CFE_EVS_GlobalData.AppData[AppID].BinFilters);
    FilterPtr->Count = CFE_EVS_MAX_FILTER_COUNT - 1;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "OK") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Last info message should go through");

    /* Test that filter lock is applied */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Locked info message should be filtered");

    /* Test that filter lock is (still) applied */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Locked info message should still be filtered");

    /* Return application to original state: re-register application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "FE_EVS_Register",
              "Re-register application");

    /* Test sending an event with app ID to a registered, filtered
     * application
     */
    UT_InitData();
    CFE_EVS_GlobalData.AppData[0].RegisterFlag = TRUE;
    CFE_EVS_GlobalData.AppData[0].ActiveFlag = FALSE;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEventWithAppID(0,
                                         CFE_EVS_INFORMATION,
                                         0,
                                         "NULL") == CFE_SUCCESS,
              "CFE_EVS_SendEventWithAppID",
              "Application registered and filtered");

    /* Test sending a timed event to a registered, filtered application */
    UT_InitData();
    CFE_EVS_GlobalData.AppData[0].RegisterFlag = TRUE;
    CFE_EVS_GlobalData.AppData[0].ActiveFlag = FALSE;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendTimedEvent(time,
                                     CFE_EVS_INFORMATION,
                                     0,
                                     "NULL") == CFE_SUCCESS,
              "CFE_EVS_SendTimedEvent",
              "Application registered and filtered");
}

/*
** Test reset filter calls
*/
void Test_FilterReset(void)
{
    CFE_EVS_BinFilter_t filter;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Reset\n");
#endif

    /* Test successful filter registration */
    UT_InitData();
    filter.EventID = 1;
    filter.Mask = 0x0001;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(&filter,
                               1,
                               CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register filter - successful");

    /* Test filter reset using an invalid event ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetFilter(CFE_EVS_MAX_EVENT_FILTERS + 1) ==
            CFE_EVS_EVT_NOT_REGISTERED,
              "CFE_EVS_ResetFilter",
              "Invalid event ID");

    /* Test filter reset using an unregistered event ID */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetFilter(-1) == CFE_SUCCESS,
              "CFE_EVS_ResetFilter",
              "Unregistered event ID");

    /* Test successful filter reset */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetFilter(1) == CFE_SUCCESS,
              "CFE_EVS_ResetFilter",
              "Valid reset filter");

    /* Test successful reset of all filters */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_ResetAllFilters() == CFE_SUCCESS,
              "CFE_EVS_ResetAllFilters",
              "Valid reset all filters");

    /* Return application to original state: re-register application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Re-register app");
}

/*
** Test long and short format events, and event strings
** greater than the maximum length allowed
*/
void Test_Format(void)
{
    int i;
    char long_msg[CFE_EVS_MAX_MESSAGE_LENGTH + 2];
    int16 EventID[2];

    CFE_TIME_SysTime_t          time = {0, 0};
    CFE_EVS_ModeCmd_t           modecmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Format\n");
#endif

    /* Enable DEBUG message output */
    UT_InitData();
    strncpy((char *) appbitcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appbitcmd.Payload.AppName));
    modecmd.Payload.Mode = CFE_EVS_LONG_FORMAT;
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output");

    /* Test set event format mode command using an invalid mode */
    UT_InitData();
    modecmd.Payload.Mode = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLEGALFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: invalid event format mode = 0xff");

    /* Test set event format mode command using a valid command to set short
     * format, reports implicitly via event
     */
    UT_InitData();
    modecmd.Payload.Mode = CFE_EVS_SHORT_FORMAT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_SETEVTFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: short format");

    /* Test event short format mode command was successful */
    UT_InitData();
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Short format check *FAILED*");
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == 0 && SendMsgEventIDRtn.count == 0,
              "CFE_EVS_SetEventFormatModeCmd",
              "Short event format mode verification");

    /* Test set event format mode command using a valid command to set long
     * format, reports implicitly via event
     */
    UT_InitData();
    modecmd.Payload.Mode = CFE_EVS_LONG_FORMAT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_SETEVTFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: long format");

    /* Test event long format mode command was successful (the following
     * messages are output if long format selection is successful)
     */
    UT_InitData();
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Long format check (SendEvent)");
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendTimedEvent(time, 0, CFE_EVS_INFORMATION,
                           "Long format check (SendTimedEvent)");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEventWithAppID(0, CFE_EVS_INFORMATION, 0,
                               "Long format check (SendEventWithAppID)");
    UT_Report(__FILE__, __LINE__,
              EventID[0] == 0 && EventID[1] == 0 &&
              SendMsgEventIDRtn.value == 0,
              "CFE_EVS_SetEventFormatModeCmd",
              "Long event format mode verification");

    /* Test sending an event using a string length greater than
     * the maximum allowed
     */
    UT_InitData();

    for (i = 0; i <= CFE_EVS_MAX_MESSAGE_LENGTH; i++)
    {
        long_msg[i] = (char)(i % 10 + 48);
    }

    long_msg[CFE_EVS_MAX_MESSAGE_LENGTH + 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "%s", long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Sent info message with > maximum string length");

    /* Test sending an event with application ID using a string length
     * greater than the maximum allowed
     */
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendEventWithAppID(0,
                                         CFE_EVS_INFORMATION,
                                         0,
                                         "%s", long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendEventWithAppID",
              "Sent info message with > maximum string length");

    /* Test sending a timed event using a string length greater than
     * the maximum allowed
     */
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SendTimedEvent(time,
                                     0,
                                     CFE_EVS_INFORMATION,
                                     "%s", long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendTimedEvent",
              "Sent info message with > maximum string length");
}

/*
** Test enable/disable of port outputs
*/
void Test_Ports(void)
{
    CFE_EVS_BitMaskCmd_t    bitmaskcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Ports\n");
#endif

    /* Test enabling all ports; reports implicitly via port output */
    UT_InitData();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAPORT_EID &&
              OSPrintRtn.value == 4 * OS_PRINT_INCR
              && OSPrintRtn.count == 4,
              "CFE_EVS_EnablePortsCmd",
              "Enable ports command received with port bit mask = 0x0f");

    /* Test that ports are enabled by sending a message */
    UT_InitData();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Test ports message");
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == 0 &&
              OSPrintRtn.value == 4 * OS_PRINT_INCR &&
              OSPrintRtn.count == 4,
              "CFE_EVS_EnablePortsCmd",
              "Test ports output");

    /* Disable all ports to cut down on unneeded output */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Disable ports command received with port bit mask = 0x0f");

    /* Test enabling a port using a bitmask that is out of range (high) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnablePortsCmd",
              "Bit mask out of range (high)");

    /* Test disabling a port using a bitmask that is out of range */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisablePortsCmd",
              "Bit mask = 0x000000ff out of range: CC = 12");

    /* Test enabling a port using a bitmask that is out of range (low) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnablePortsCmd",
              "Bit mask out of range (low)");

    /* Test enabling a port 1 and 2, but not 3 and 4 (branch path coverage) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAPORT_EID,
              "CFE_EVS_EnablePortsCmd",
              "Enable ports 1 and 2; disable ports 3 and 4");

    /* Test enabling a port 3 and 4, but not 1 and 2 (branch path coverage) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAPORT_EID,
              "CFE_EVS_EnablePortsCmd",
              "Enable ports 3 and 4; disable ports 1 and 2");

    /* Test disabling a port using a bitmask that is out of range (low) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
            CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisablePortsCmd",
              "Bit mask out of range (low)");

    /* Test disabling a port 1 and 2, but not 3 and 4 (branch path coverage) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
            CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Enable ports 1 and 2; disable ports 3 and 4");

    /* Test disabling a port 3 and 4, but not 1 and 2 (branch path coverage) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
            CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Enable ports 3 and 4; disable ports 1 and 2");
}

/*
** Test event logging
*/
void Test_Logging(void)
{
    int                  i;
    uint32               resetAreaSize = 0;
    char                 tmpString[100];
    CFE_EVS_ModeCmd_t    modecmd;
    CFE_EVS_LogFileCmd_t logfilecmd;
    cpuaddr              TempAddr;
    CFE_ES_ResetData_t   *CFE_EVS_ResetDataPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Logging\n");
#endif

    /* Test setting the logging mode with logging disabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = FALSE;
    modecmd.Payload.Mode = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_NO_LOGSET_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode command: event log disabled");

    /* Re-enable logging and set conditions to allow complete code coverage */
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = TRUE;
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_PSP_RST_TYPE_POWERON, 1);
    CFE_PSP_GetResetArea(&TempAddr, &resetAreaSize);
    CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)TempAddr;
    CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

    /* Test setting the logging mode using an invalid mode */
    UT_InitData();
    modecmd.Payload.Mode = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_LOGMODE_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to an invalid mode");

    /* Test setting the logging mode to discard */
    UT_InitData();
    modecmd.Payload.Mode = CFE_EVS_LOG_DISCARD;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LOGMODE_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to discard mode");

    /* Test overfilling the log in discard mode */
    UT_InitData();

    /* Ensure log is filled, then add one more, implicitly testing
     * EVS_AddLog
     */
    for (i = 0; i < CFE_EVS_LOG_MAX; i++)
    {
        snprintf(tmpString, 100, "Log fill event %d", i);
        CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "%s", tmpString);
    }

    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Log overfill event discard");
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE &&
              CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LOG_DISCARD,
              "CFE_EVS_SendEvent",
              "Log overfill event (discard mode)");

    /* Test setting the logging mode to overwrite */
    UT_InitData();
    modecmd.Payload.Mode = CFE_EVS_LOG_OVERWRITE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Log overfill event overwrite");
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE &&
              CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LOG_OVERWRITE,
              "CFE_EVS_SetLoggingModeCmd",
              "Log overfill event (overwrite mode)");

    /* Test writing to the log while it is disabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = FALSE;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_LogFileCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_NO_LOGWR_EID,
              "CFE_EVS_WriteLogFileCmd",
              "Write log command with event log disabled");

    /* Test clearing the log while it is disabled*/
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_NO_LOGCLR_EID,
              "EVS_ClearLog",
              "Clear log command with event log disabled");

    /* Test sending a no op command */
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_NO_OPERATION_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_NOOP_EID,
              "CFE_EVS_ProcessGroundCommand",
              "No-op command");

    /* Clear log for next test */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_SB_CmdHdr_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == FALSE,
              "EVS_ClearLog",
              "Clear log");

    /* Test setting the logging mode to overwrite */
    UT_InitData();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_PSP_RST_TYPE_POWERON, 1);
    CFE_PSP_GetResetArea(&TempAddr, &resetAreaSize);
    CFE_EVS_ResetDataPtr = (CFE_ES_ResetData_t *)TempAddr;
    CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;
    modecmd.Payload.Mode = CFE_EVS_LOG_OVERWRITE;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_SetLoggingModeCmd((CFE_SB_MsgPayloadPtr_t) &modecmd.Payload) == TRUE,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to overwrite mode");

    /* Test successfully writing a single event log entry using the default
     * log name
     */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    logfilecmd.Payload.LogFilename[0] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == TRUE,
              "CFE_EVS_WriteLogFileCmd",
              "Write single event log entry - successful (default log name)");

    /* Test writing a log entry with a create failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == FALSE,
              "CFE_EVS_WriteLogFileCmd",
              "OS create fail");

    /* Test successfully writing all log entries */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->LogCount = CFE_EVS_LOG_MAX;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == TRUE,
              "CFE_EVS_WriteLogFileCmd",
              "Write all event log entries");

    /* Test writing a log entry with a write failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetOSFail(OS_WRITE_FAIL);
    CFE_EVS_GlobalData.EVS_LogPtr->LogCount = CFE_EVS_LOG_MAX;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == FALSE,
              "CFE_EVS_WriteLogFileCmd",
              "OS write fail");

    /* Test successfully writing a single event log entry using a specified
     * log name
     */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    strncpy((char *) logfilecmd.Payload.LogFilename, "LogFile",
            sizeof(logfilecmd.Payload.LogFilename));
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == TRUE,
              "CFE_EVS_WriteLogFileCmd",
              "Write single event log entry - successful (log name specified)");

    /* Test successfully writing a single event log entry with a failure
     * writing the header
     */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t) + 1, 1);
    logfilecmd.Payload.LogFilename[0] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteLogFileCmd((CFE_SB_MsgPayloadPtr_t) &logfilecmd.Payload) == FALSE,
              "CFE_EVS_WriteLogFileCmd",
              "Write single event log entry - write header failed");
}

/*
** Test writing application data
*/
void Test_WriteApp(void)
{
    CFE_EVS_AppDataCmd_t        AppDataCmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Write App\n");
#endif

    /* Enable DEBUG message output */
    UT_InitData();
    strncpy((char *) appbitcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appbitcmd.Payload.AppName));
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output");

    /* Test resetting counters */
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_RSTCNT_EID,
              "CFE_EVS_ResetCountersCmd",
              "Reset counters - successful");

    /* Test writing application data with a create failure using default
     * file name
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppDataCmd_t));
    strncpy((char *) AppDataCmd.Payload.AppDataFilename, "ut_cfe_evs",
            sizeof(AppDataCmd.Payload.AppDataFilename));
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_CRDATFILE_EID,
              "CFE_EVS_WriteAppDataCmd",
              "OS create fail (default file name)");

    /* Test writing application data with a write/close failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL | OS_CLOSE_FAIL);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_WRDATFILE_EID,
              "CFE_EVS_WriteAppDataCmd",
              "OS write fail");

    /* Test successfully writing application data */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_WRDAT_EID,
              "CFE_EVS_WriteAppDataCmd",
              "Write application data - successful");

    /* Test writing application data with a create failure using specified
     * file name
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppDataCmd_t));
    strncpy((char *) AppDataCmd.Payload.AppDataFilename, "AppDataFileName",
            sizeof(AppDataCmd.Payload.AppDataFilename));
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_CRDATFILE_EID,
              "CFE_EVS_WriteAppDataCmd",
              "OS create fail (specified file name)");

    /* Test writing application data with a failure writing the header */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t) + 1, 1);
    AppDataCmd.Payload.AppDataFilename[0] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_WriteAppDataCmd((CFE_SB_MsgPayloadPtr_t) &AppDataCmd.Payload) == FALSE,
              "CFE_EVS_WriteAppDataCmd",
              "Write application data - write header failed");
}

/*
** Test commands with bad application names (unknown, illegal, unregistered)
*/
void Test_BadAppCmd(void)
{
    CFE_EVS_AppNameBitMaskCmd_t     appbitcmd;
    CFE_EVS_AppNameCmd_t            appnamecmd;
    CFE_EVS_AppNameEventIDMaskCmd_t appmaskcmd;
    CFE_EVS_AppNameEventIDCmd_t     appcmdcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Bad App Command\n");
#endif

    UT_InitData();

    /* Set up event and mask */
    appbitcmd.Payload.BitMask = 0;
    appmaskcmd.Payload.Mask = 0;
    appmaskcmd.Payload.EventID = 0;
    appcmdcmd.Payload.EventID = 0;

    strncpy((char *) appbitcmd.Payload.AppName, "unknown_name",
            sizeof(appbitcmd.Payload.AppName));
    strncpy((char *) appnamecmd.Payload.AppName, "unknown_name",
            sizeof(appnamecmd.Payload.AppName));
    strncpy((char *) appmaskcmd.Payload.AppName, "unknown_name",
            sizeof(appmaskcmd.Payload.AppName));
    strncpy((char *) appcmdcmd.Payload.AppName, "unknown_name",
            sizeof(appcmdcmd.Payload.AppName));

    /* Test disabling application event types with an unknown application ID */
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Unable to retrieve application ID while disabling event types");

    /* Test enabling application event types with an unknown application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Unable to retrieve application ID while enabling event types");

    /* Test disabling application events with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Unable to retrieve application ID while disabling events");

    /* Test enabling application events with an unknown application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Unable to retrieve application ID while enabling events");

    /* Test resetting the application event counter with an unknown
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Unable to retrieve application ID while resetting events");

    /* Test modifying event filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Unable to retrieve application ID while modifying event "
                "filters");

    /* Test deleting event filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Unable to retrieve application ID while deleting event "
                "filters");

    /* Test setting the event filter mask with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Unable to retrieve application ID while setting the event "
                "filter mask");

    /* Test resetting the filter with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetFilterCmd",
              "Unable to retrieve application ID while resetting the filter");

    /* Test resetting all filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Unable to retrieve application ID while resetting all filters");

    /* Test disabling application event types with an illegal application ID */
    UT_InitData();
    strncpy((char *) appbitcmd.Payload.AppName, "illegal_id",
            sizeof(appbitcmd.Payload.AppName));
    strncpy((char *) appnamecmd.Payload.AppName, "illegal_id",
            sizeof(appnamecmd.Payload.AppName));
    strncpy((char *) appmaskcmd.Payload.AppName, "illegal_id",
            sizeof(appmaskcmd.Payload.AppName));
    strncpy((char *) appcmdcmd.Payload.AppName, "illegal_id",
            sizeof(appcmdcmd.Payload.AppName));

    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Illegal application ID while disabling application event "
                "types");

    /* Test enabling application event types with an illegal application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Illegal application ID while enabling application event types");

    /* Test disabling application events with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Illegal application ID while disabling application events");

    /* Test enabling application events with an illegal application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Illegal application ID while enabling application events");

    /* Test resetting the application event counter with an illegal
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Illegal application ID while resetting the application event "
                "counter");

    /* Test adding the event filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Illegal application ID while adding the event filter");

    /* Test deleting the event filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Illegal application ID while deleting the event filter");

    /* Test setting the filter mask with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Illegal application ID while setting the filter mask");

    /* Test resetting the filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetFilterCmd",
              "Illegal application ID while resetting the filter");

    /* Test resetting all filters with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Illegal application ID while resetting all filters");

    /* Test disabling application event types with an unregistered
     * application ID
     */
    UT_InitData();
    strncpy((char *) appbitcmd.Payload.AppName, "unregistered_app",
            sizeof(appbitcmd.Payload.AppName));
    strncpy((char *) appnamecmd.Payload.AppName, "unregistered_app",
            sizeof(appnamecmd.Payload.AppName));
    strncpy((char *) appmaskcmd.Payload.AppName, "unregistered_app",
            sizeof(appmaskcmd.Payload.AppName));
    strncpy((char *) appcmdcmd.Payload.AppName, "unregistered_app",
            sizeof(appcmdcmd.Payload.AppName));
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Application not registered while disabling application event "
                "types");

    /* Test enabling application event types with an unregistered
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Application not registered while enabling application event "
                "types");

    /* Test disabling application events with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Application not registered while disabling application events");

    /* Test enabling application events with an unregistered application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Application not registered while enabling application events");

    /* Test resetting the application event counter with an unregistered
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Application not registered while resetting the application "
                "event counter");

    /* Test adding the event filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Application not registered while adding the event filter");

    /* Test deleting the event filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Application not registered while deleting the event filter");

    /* Test setting the filter mask with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Application not registered while setting the filter mask");

    /* Test resetting the filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetFilterCmd",
              "Application not registered while resetting the filter");

    /* Test resetting all filters with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Application not registered while resetting all filters");
}

/*
** Test event commands (and reset counter)
*/
void Test_EventCmd(void)
{
    int16                       EventID[4];
    CFE_EVS_BitMaskCmd_t        bitmaskcmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;
    CFE_EVS_AppNameCmd_t        appnamecmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Event Command\n");
#endif

    UT_InitData();

    /* Run the next series of tests with valid, registered application name */
    strncpy((char *) appbitcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appbitcmd.Payload.AppName));
    strncpy((char *) appnamecmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appnamecmd.Payload.AppName));

    /* Test disabling of all events */
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_DEBUG, "FAIL : Debug message disabled");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "FAIL : Info message disabled");
    EventID[2] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_ERROR, "FAIL : Error message disabled");
    EventID[3] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "FAIL : Critical message disabled");
    UT_Report(__FILE__, __LINE__,
              EventID[0] == -1  && EventID[1] == -1 &&
              EventID[2] == -1 && EventID[3] == -1 &&
              SendMsgEventIDRtn.value == -1,
              "CFE_EVS_SendEvent",
              "Disable all events");

    /* Test enabling of all events */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_DEBUG, "Debug message enabled");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Info message enabled");
    EventID[2] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_ERROR, "Error message enabled");
    EventID[3] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "Critical message enabled");
    UT_Report(__FILE__, __LINE__,
              EventID[0] == CFE_EVS_ENAAPPEVTTYPE_EID &&
              EventID[1] == 0 && EventID[2] == 0 && EventID[3] == 0 &&
              SendMsgEventIDRtn.value == 0,
              "CFE_EVS_SendEvent",
              "Enable all event types");

    /* Test disabling event type using an illegal type */
    UT_InitData();
    appbitcmd.Payload.BitMask = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    CFE_EVS_SendEvent(0, 0xffff, "FAIL : Illegal type disabled");
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Disable events using an illegal event type");

    /* Test enabling event type using an illegal type */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable events using an illegal event type");

    /* Test successful disabling of application events */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1,
              "CFE_EVS_DisableAppEventsCmd",
              "Disable application events - successful");

    /* Test successful enabling of application events */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVT_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Enable application events - successful");

    /* Test disabling event types (leave debug enabled) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT |
                         CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DISEVTTYPE_EID,
              "CFE_EVS_DisableEventTypesCmd",
              "Disable event types except debug");

    /* Test enabling all event types (debug already enabled) */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAEVTTYPE_EID,
              "CFE_EVS_EnableEventTypesCmd",
              "Enable all event types");

    /* Test successfully resetting the application event counter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_RSTEVTCNT_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Reset event application counter - successful");

    /* Test disabling an event type using an out of range bit mask (high) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableEventTypesCmd",
              "Disable event types - bit mask out of range (high)");

    /* Test enabling an event type using an out of range bit mask (high) */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableEventTypesCmd",
              "Enable event types - bit mask out of range (high)");

    /* Test disabling an application event type using an out of
     * range bit mask (high)
     */
    UT_InitData();
    appbitcmd.Payload.BitMask = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Disable application event types - bit mask out of range "
                "(high)");

    /* Test enabling an application event type using an out of
     * range bit mask (high)
     */
    UT_InitData();
    appbitcmd.Payload.BitMask = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable application event types - bit mask out of range (high)");

    /* Test disabling an event type using an out of range bit mask (low) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableEventTypesCmd",
              "Disable event types - bit mask out of range (low)");

    /* Test enabling an event type using an out of range bit mask (low) */
    UT_InitData();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableEventTypesCmd",
              "Enable event types - bit mask out of range (low)");

    /* Test disabling an application event type using an out of
     * range bit mask (low)
     */
    UT_InitData();
    appbitcmd.Payload.BitMask = 0x0;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Disable application event types - bit mask out of range (low)");

    /* Test enabling an application event type using an out of
     * range bit mask (low)
     */
    UT_InitData();
    appbitcmd.Payload.BitMask = 0x0;
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable application event types - bit mask out of range (low)");
}

/*
** Test filter commands
*/
void Test_FilterCmd(void)
{
    int                             i;
    CFE_EVS_AppNameCmd_t            appnamecmd;
    CFE_EVS_AppNameEventIDMaskCmd_t appmaskcmd;
    CFE_EVS_AppNameEventIDCmd_t     appcmdcmd;
    CFE_EVS_AppNameBitMaskCmd_t     appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Command\n");
#endif

    UT_InitData();

    /* Set up event and mask */
    appmaskcmd.Payload.Mask = 0;
    appmaskcmd.Payload.EventID = 0;
    appcmdcmd.Payload.EventID = 0;

    /* Run the next series of tests with valid, registered application name */
    strncpy((char *) appnamecmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appnamecmd.Payload.AppName));
    strncpy((char *) appmaskcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appmaskcmd.Payload.AppName));
    strncpy((char *) appcmdcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appcmdcmd.Payload.AppName));
    strncpy((char *) appbitcmd.Payload.AppName, "ut_cfe_evs",
            sizeof(appbitcmd.Payload.AppName));

    /* Enable all application event message output */
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable all application event types - successful");

    /* Ensure there is no filter for the next tests */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete event filter - successful");

    /* Test setting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Set filter - application is not registered for filtering");

    /* Test resetting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_ResetFilterCmd",
              "Reset filter - application is not registered for filtering");


    /* Test resetting all filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_RSTALLFILTER_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Reset all filters - successful");

    /* Test successfully adding an event filter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - successful");

    /* Test adding an event filter to an event already registered
     * for filtering
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_EVT_FILTERED_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - event already registered for filtering");

    /* Test successfully setting a filter mask */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_SETFILTERMSK_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Set filter mask - successful");

    /* Test successfully resetting a filter mask */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_RSTFILTER_EID,
              "CFE_EVS_ResetFilterCmd",
              "Reset filter mask - successful");

    /* Test successfully resetting all filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_RSTALLFILTER_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Reset all filters  - successful");

    /* Test successfully deleting an event filter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DELFILTER_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete event filter - successful");

    /* Test filling the event filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));

    for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS; i++)
    {
        UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
                   CFE_EVS_ADD_EVENT_FILTER_CC);
        appmaskcmd.Payload.EventID++;
    }

    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID &&
              SendMsgEventIDRtn.count == CFE_EVS_MAX_EVENT_FILTERS,
              "CFE_EVS_AddEventFilterCmd",
              "Maximum event filters added");

    /* Test overfilling the event filters */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
              CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_MAXREGSFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Maximum event filters exceeded");

    /* Return application to original state, re-register application */
    UT_InitData();
    appmaskcmd.Payload.EventID = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register application - successful");

    /* Enable all application event message output */
    UT_InitData();
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable all application event message output");

    /* Set-up to test filtering the same event twice */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - successful");

    /* Test filtering the same event again */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_EVT_FILTERED_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - event is already registered for filtering");

    /* Test successful event filer deletion */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_DELFILTER_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete filter - successful");

    /* Return application to original state, re-register application */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register application - successful");
}

/*
** Test commands with invalid command, command packet, and command length
*/
void Test_InvalidCmd(void)
{
    CFE_EVS_ModeCmd_t modecmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Invalid Command\n");
#endif

    modecmd.Payload.Mode = CFE_EVS_LOG_OVERWRITE;

    /* Test invalid command packet */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, 0xffff, 0);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_MSGID_EID,
              "CFE_EVS_ProcessGroundCommand",
              "Invalid command packet");

    /* Test invalid command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               0xffff);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_ERR_CC_EID,
              "CFE_EVS_ProcessGroundCommand",
              "Invalid command");

    /* Test invalid command length with no op command*/
    UT_InitData();
    UT_SetSBTotalMsgLen(6);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_NO_OPERATION_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with no op command");

    /* Test invalid command length with reset counters command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset counters command");

    /* Test invalid command length with enable event type command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable event type command");

    /* Test invalid command length with disable event type command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable event type command");

    /* Test invalid command length with set event format mode command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set event format mode command");

    /* Test invalid command length with enable application event
     * type command
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable application event type "
                "command");

    /* Test invalid command length with disable application event
     * type command
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable application event type "
                "command");

    /* Test invalid command length with enable application events command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable application events command");

    /* Test invalid command length with disable application events command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable application events command");

    /* Test invalid command length with reset application counter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset application counter command");

    /* Test invalid command length with set filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set filter command");

    /* Test invalid command length with enable ports command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable ports command");

    /* Test invalid command length with disable ports command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable ports command");

    /* Test invalid command length with reset filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset filter command");

    /* Test invalid command length with reset all filters command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset all filters command");

    /* Test invalid command length with add event filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with add event filter command");

    /* Test invalid command length with delete event filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with delete event filter command");

    /* Test invalid command length with write application data command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with write application data command");

    /* Test invalid command length with write log data command */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = TRUE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with write log data command");

    /* Test invalid command length with set log mode command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set log mode command");

    /* Test invalid command length with clear log command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with clear log command");
}

/*
** Test miscellaneous functionality
*/
void Test_Misc(void)
{
    CFE_EVS_ModeCmd_t  modecmd;
    CFE_TIME_SysTime_t time = {0, 0};
    CFE_EVS_Packet_t   EVS_Pkt;
    uint32             AppID;
    int                i;
    char               AppName;
    char               msg[CFE_EVS_MAX_MESSAGE_LENGTH + 2];

#ifdef UT_VERBOSE
    UT_Text("Begin Test Miscellaneous\n");
#endif

    modecmd.Payload.Mode = CFE_EVS_LOG_OVERWRITE;

    /* Test null application ID input to exercise EVS_GetApplicationInfo */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              EVS_GetApplicationInfo(NULL, &AppName) == CFE_ES_ERR_BUFFER,
              "EVS_GetApplicationInfo",
              "Get application info with null application ID");

    /* Test successful log data file write */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_LogFileCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1,
              "CFE_EVS_WriteLogFileCmd",
              "Write log data - successful");

    /* Test successfully setting the logging mode */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == -1,
              "CFE_EVS_SetLoggingModeCmd",
              "Set logging mode - successful");

    /* Test housekeeping report with log enabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = TRUE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_SEND_HK_MID, 0);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_HK_TLM_MID,
              "CFE_EVS_ReportHousekeepingCmd",
              "Housekeeping report - successful (log enabled)");

    /* Test successful application cleanup */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_CleanUpApp(0) == CFE_SUCCESS,
              "CFE_EVS_CleanUpApp",
              "Application cleanup - successful");

    /* Test registering an application with invalid filter argument */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_Register(NULL, 1, 0) == CFE_ES_ERR_BUFFER,
              "CFE_EVS_Register",
              "Register application with invalid arguments");

    /* Test housekeeping report with log disabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.LogEnabled = FALSE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_SEND_HK_MID, 0);
    UT_Report(__FILE__, __LINE__,
              SendMsgEventIDRtn.value == CFE_EVS_HK_TLM_MID,
              "CFE_EVS_ReportHousekeepingCmd",
              "Housekeeping report - successful (log disabled)");

    /* Test null application name to exercise EVS_GetApplicationInfo */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              EVS_GetApplicationInfo(&AppID, NULL) == CFE_ES_ERR_BUFFER,
              "EVS_GetApplicationInfo",
              "Get application info with null application name");

    /* Test sending a packet with the message counter and the event counter
     * at their maximum allowed values
     */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter =
        CFE_EVS_MAX_EVENT_SEND_COUNT;
    CFE_EVS_GlobalData.AppData[0].EventCount = CFE_EVS_MAX_EVENT_SEND_COUNT;
    EVS_SendPacket(0, time, &EVS_Pkt);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter ==
                  CFE_EVS_MAX_EVENT_SEND_COUNT &&
              CFE_EVS_GlobalData.AppData[0].EventCount ==
                  CFE_EVS_MAX_EVENT_SEND_COUNT,
              "EVS_SendPacket",
              "Maximum message count and event count");

    /* Test sending a message with the message length greater than the
     * maximum allowed value
     */
    UT_InitData();

    for (i = 0; i <= CFE_EVS_MAX_MESSAGE_LENGTH; i++)
    {
        msg[i] = 'a';
    }

    msg[CFE_EVS_MAX_MESSAGE_LENGTH] = '\0';
    CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter = 0;
    CFE_EVS_GlobalData.AppData[CFE_EVS_GlobalData.EVS_AppID].ActiveFlag = TRUE;
    CFE_EVS_GlobalData.AppData[CFE_EVS_GlobalData.EVS_AppID].EventTypesActiveFlag |=
        CFE_EVS_INFORMATION_BIT;
    EVS_SendEvent(0, CFE_EVS_INFORMATION, msg);
    UT_Report(__FILE__, __LINE__,
              CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter == 1,
              "EVS_SendEvent",
              "Maximum message length exceeded");
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
    CFE_EVS_ProcessCommandPacket(MsgPtr);
}
