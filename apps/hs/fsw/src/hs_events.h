/************************************************************************
** File:
**   $Id: hs_events.h 1.2 2015/11/12 14:25:16EST wmoleski Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**   Specification for the CFS Health and Safety (HS) event identifers.
**
** Notes:
**
**   $Log: hs_events.h  $
**   Revision 1.2 2015/11/12 14:25:16EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.13 2015/05/04 11:59:12EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.12 2015/05/01 16:55:33EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.11 2015/05/01 13:52:07EDT lwalling 
**   Change event HS_MAT_LD_ERR_EID description typo from AppMon to MsgActs
**   Revision 1.10 2015/03/03 12:16:21EST sstrege 
**   Added copyright information
**   Revision 1.9 2011/10/13 18:47:32EDT aschoeni 
**   updated for hs utilization calibration changes
**   Revision 1.8 2011/08/15 18:44:31EDT aschoeni 
**   HS Unsubscibes when eventmon is disabled
**   Revision 1.7 2010/11/19 17:58:28EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.6 2010/09/29 18:27:28EDT aschoeni 
**   Added Utilization Monitoring Events
**   Revision 1.5 2010/09/13 14:40:46EDT aschoeni 
**   Made Table validation events Info instead of Debug
**   Revision 1.4 2009/06/02 16:34:10EDT aschoeni 
**   Removed 'ID' field from XCT val error event
**   Revision 1.3 2009/05/22 17:40:12EDT aschoeni 
**   Updated CDS related events
**   Revision 1.2 2009/05/04 17:44:33EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:43EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_events_h_
#define _hs_events_h_

/** \brief <tt> 'HS Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'HS Initialized. Version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when the CFS Health and Safety has
**  completed initialization.
**
**  The \c Version fields contain the #HS_MAJOR_VERSION,
**  #HS_MINOR_VERSION, #HS_REVISION, and #HS_MISSION_REV
**  version identifiers.
*/
#define HS_INIT_EID 1

/** \brief <tt> 'Application Terminating, err = 0x\%08X' </tt>
**  \event <tt> 'Application Terminating, err = 0x\%08X' </tt>
**
**  \par Type: CRITICAL
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  exits due to a fatal error condition.
**
**  The \c err field contains the return status from the
**  cFE call that caused the app to terminate
*/
#define HS_APP_EXIT_EID 2

/** \brief <tt> 'Failed to restore data from CDS (Err=0x\%08x), initializing resets data' </tt>
**  \event <tt> 'Failed to restore data from CDS (Err=0x\%08x), initializing resets data' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to restore data from its critical data store
**
**  The \c Err field contains the return status from the
**  #CFE_ES_RestoreFromCDS call that generated the error
*/
#define HS_CDS_RESTORE_ERR_EID 3

/** \brief <tt> 'Error Creating SB Command Pipe,RC=0x\%08X' </tt>
**  \event <tt> 'Error Creating SB Command Pipe,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to create its command pipe via the #CFE_SB_CreatePipe
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_SB_CreatePipe call that generated the error
*/
#define HS_CR_CMD_PIPE_ERR_EID 4

/** \brief <tt> 'Error Creating SB Event Pipe,RC=0x\%08X' </tt>
**  \event <tt> 'Error Creating SB Event Pipe,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to create its event pipe via the #CFE_SB_CreatePipe
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_SB_CreatePipe call that generated the error
*/
#define HS_CR_EVENT_PIPE_ERR_EID 5

/** \brief <tt> 'Error Creating SB Wakeup Pipe,RC=0x\%08X' </tt>
**  \event <tt> 'Error Creating SB Wakeup Pipe,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  is unable to create its wakeup pipe via the #CFE_SB_CreatePipe
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_SB_CreatePipe call that generated the error
*/
#define HS_CR_WAKEUP_PIPE_ERR_EID 6

/** \brief <tt> 'Error Subscribing to Events,RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to Events,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to #CFE_SB_Subscribe
**  for the #CFE_EVS_EVENT_MSG_MID, during initialization returns
**  a value other than CFE_SUCCESS
*/
#define HS_SUB_EVS_ERR_EID 7

/** \brief <tt> 'Error Subscribing to HK Request,RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to HK Request,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to #CFE_SB_Subscribe
**  for the #HS_SEND_HK_MID, during initialization returns
**  a value other than CFE_SUCCESS
*/
#define HS_SUB_REQ_ERR_EID 8

/** \brief <tt> 'Error Subscribing to Gnd Cmds,RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to Gnd Cmds,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to #CFE_SB_Subscribe
**  for the #HS_CMD_MID, during initialization returns a value
**  other than CFE_SUCCESS
*/
#define HS_SUB_CMD_ERR_EID 9

/** \brief <tt> 'Error Registering AppMon Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Registering AppMon Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when Health and Safety is unable to register its
**  Application Monitor Table with cFE Table Services via the #CFE_TBL_Register API.
**
**  The \c RC value is the return code from the #CFE_TBL_Register API call.
*/
#define HS_AMT_REG_ERR_EID 10

/** \brief <tt> 'Error Registering EventMon Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Registering EventMon Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when Health and Safety is unable to register its
**  Event Monitor Table with cFE Table Services via the #CFE_TBL_Register API.
**
**  The \c RC value is the return code from the #CFE_TBL_Register API call.
*/
#define HS_EMT_REG_ERR_EID 11

/** \brief <tt> 'Error Registering ExeCount Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Registering ExeCount Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when Health and Safety is unable to register its
**  Execution Counters Table with cFE Table Services via the #CFE_TBL_Register API.
**
**  The \c RC value is the return code from the #CFE_TBL_Register API call.
*/
#define HS_XCT_REG_ERR_EID 12

/** \brief <tt> 'Error Registering MsgActs Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Registering MsgActs Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when Health and Safety is unable to register its
**  Message Actions Table with cFE Table Services via the #CFE_TBL_Register API.
**
**  The \c RC value is the return code from the #CFE_TBL_Register API call.
*/
#define HS_MAT_REG_ERR_EID 13

/** \brief <tt> 'Error Loading AppMon Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Loading AppMon Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to CFE_TBL_Load
**  for the application monitor table returns a value other than CFE_SUCCESS
*/
#define HS_AMT_LD_ERR_EID 14

/** \brief <tt> 'Error Loading EventMon Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Loading EventMon Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to CFE_TBL_Load
**  for the event monitor table returns a value other than CFE_SUCCESS
*/
#define HS_EMT_LD_ERR_EID 15

/** \brief <tt> 'Error Loading ExeCount Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Loading ExeCount Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to CFE_TBL_Load
**  for the execution counters table returns a value other than CFE_SUCCESS
*/
#define HS_XCT_LD_ERR_EID 16

/** \brief <tt> 'Error Loading MsgActs Table,RC=0x\%08X' </tt>
**  \event <tt> 'Error Loading MsgActs Table,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to CFE_TBL_Load
**  for the message actions table returns a value other than CFE_SUCCESS
*/
#define HS_MAT_LD_ERR_EID 17

/** \brief <tt> 'Data in CDS was corrupt, initializing resets data' </tt>
**  \event <tt> 'Data in CDS was corrupt, initializing resets data' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFS Health and Safety
**  restores data from the CDS that does not pass validation
*/
#define HS_CDS_CORRUPT_ERR_EID 18

/** \brief <tt> 'Invalid command code: ID = 0x\%04X, CC = \%d' </tt>
**  \event <tt> 'Invalid command code: ID = 0x\%04X, CC = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a software bus message is received
**  with an invalid command code.
**
**  The \c ID field contains the message ID, the \c CC field contains
**  the command code that generated the error.
*/
#define HS_CC_ERR_EID 19

/** \brief <tt> 'Invalid command pipe message ID: 0x\%04X' </tt>
**  \event <tt> 'Invalid command pipe message ID: 0x\%04X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a software bus message is received
**  with an invalid message ID.
**
**  The \c message \c ID field contains the message ID that generated
**  the error.
*/
#define HS_MID_ERR_EID 20

/** \brief <tt> 'Invalid HK request msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid HK request msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a housekeeping request is received
**  with a message length that doesn't match the expected value.
**
**  The \c ID field contains the message ID, the \c CC field contains the
**  command code, the \c Len field is the actual length returned by the
**  #CFE_SB_GetTotalMsgLength call, and the \c Expected field is the expected
**  length for the message.
*/
#define HS_HKREQ_LEN_ERR_EID 21

/** \brief <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a ground command message is received
**  with a message length that doesn't match the expected value.
**
**  The \c ID field contains the message ID, the \c CC field contains the
**  command code, the \c Len field is the actual length returned by the
**  #CFE_SB_GetTotalMsgLength call, and the \c Expected field is the expected
**  length for a message with that command code.
*/
#define HS_LEN_ERR_EID 22

/** \brief <tt> 'No-op command: Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'No-op command: Version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a NOOP command has been received.
**
**  The \c Version fields contain the #HS_MAJOR_VERSION,
**  #HS_MINOR_VERSION, #HS_REVISION, and #HS_MISSION_REV
**  version identifiers.
*/
#define HS_NOOP_INF_EID 23

/** \brief <tt> 'Reset counters command' </tt>
**  \event <tt> 'Reset counters command' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a reset counters command has
**  been received.
*/
#define HS_RESET_DBG_EID 24

/** \brief <tt> 'Application Monitoring Enabled' </tt>
**  \event <tt> 'Application Monitoring Enabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an enable application monitoring
**  command has been received.
*/
#define HS_ENABLE_APPMON_DBG_EID 25

/** \brief <tt> 'Application Monitoring Disabled' </tt>
**  \event <tt> 'Application Monitoring Disabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a disable application monitoring
**  command has been received.
*/
#define HS_DISABLE_APPMON_DBG_EID 26

/** \brief <tt> 'Event Monitoring Enabled' </tt>
**  \event <tt> 'Event Monitoring Enabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an enable event monitoring
**  command has been received.
*/
#define HS_ENABLE_EVENTMON_DBG_EID 27

/** \brief <tt> 'Event Monitoring Disabled' </tt>
**  \event <tt> 'Event Monitoring Disabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a disable event monitoring
**  command has been received.
*/
#define HS_DISABLE_EVENTMON_DBG_EID 28

/** \brief <tt> 'Aliveness Indicator Enabled' </tt>
**  \event <tt> 'Aliveness Indicator Enabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an enable aliveness indicator
**  command has been received.
*/
#define HS_ENABLE_ALIVENESS_DBG_EID 29

/** \brief <tt> 'Aliveness Indicator Disabled' </tt>
**  \event <tt> 'Aliveness Indicator Disabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a disable aliveness indicator
**  command has been received.
*/
#define HS_DISABLE_ALIVENESS_DBG_EID 30

/** \brief <tt> 'Processor Resets Performed by HS Counter has been Reset' </tt>
**  \event <tt> 'Processor Resets Performed by HS Counter has been Reset' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a reset processor resets count
**  command has been received.
*/
#define HS_RESET_RESETS_DBG_EID 31

/** \brief <tt> 'Max Resets Performable by HS has been set to \%d' </tt>
**  \event <tt> 'Max Resets Performable by HS has been set to \%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a set max processor resets
**  command has been received.
**
**  The value the max resets count has been set to is listed in the event.
*/
#define HS_SET_MAX_RESETS_DBG_EID 32

/** \brief <tt> 'Error getting AppMon Table address, RC=0x\%08X, Application Monitoring Disabled' </tt>
**  \event <tt> 'Error getting AppMon Table address, RC=0x\%08X, Application Monitoring Disabled' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the applications monitor table.
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define HS_APPMON_GETADDR_ERR_EID 33

/** \brief <tt> 'Error getting EventMon Table address, RC=0x\%08X, Event Monitoring Disabled' </tt>
**  \event <tt> 'Error getting EventMon Table address, RC=0x\%08X, Event Monitoring Disabled' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the event monitor table.
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define HS_EVENTMON_GETADDR_ERR_EID 34

/** \brief <tt> 'Error getting ExeCount Table address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting ExeCount Table address, RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the execution counters table.
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define HS_EXECOUNT_GETADDR_ERR_EID 35

/** \brief <tt> 'Error getting MsgActs Table address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting MsgActs Table address, RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the message actions table.
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define HS_MSGACTS_GETADDR_ERR_EID 36

/** \brief <tt> 'Processor Reset Action Limit Reached: No Reset Performed' </tt>
**  \event <tt> 'Processor Reset Action Limit Reached: No Reset Performed' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the action specified by an Application or Event
**  monitor that fails is Processor Reset, and no more Processor Resets are allowed.
**
*/
#define HS_RESET_LIMIT_ERR_EID 37


/** \brief <tt> 'App Monitor App Name not found: APP:(\%s)' </tt>
**  \event <tt> 'App Monitor App Name not found: APP:(\%s)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an application name cannot be resolved
**  into an application ID by the OS.
**
**  The \c APP field specifies the name in the table that was not found in the system.
*/
#define HS_APPMON_APPNAME_ERR_EID 38

/** \brief <tt> 'App Monitor Failure: APP:(\%s): Action: Restart Application' </tt>
**  \event <tt> 'App Monitor Failure: APP:(\%s): Action: Restart Application' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a monitored application fails to increment its
**  execution counter in the table specified number of cycles, and the specified
**  action type is Application Restart.
**
**  The \c APP field specifies the name of the application that failed to check in.
*/
#define HS_APPMON_RESTART_ERR_EID 39

/** \brief <tt> 'Call to Restart App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**  \event <tt> 'Call to Restart App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the app monitor attempts to restart a task
**  and is unable to.
**
**  The \c APP field is the name of the application that was being restarted
**  The \c ERR field is the return code from the #CFE_ES_RestartApp or #CFE_ES_GetAppInfo or
**  #CFE_ES_GetAppIDByName function call that generated the error.
*/
#define HS_APPMON_NOT_RESTARTED_ERR_EID 40

/** \brief <tt> 'App Monitor Failure: APP:(\%s): Action: Event Only' </tt>
**  \event <tt> 'App Monitor Failure: APP:(\%s): Action: Event Only' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a monitored application fails to increment its
**  execution counter in the table specified number of cycles, and the specified
**  action type is Event Only.
**
**  The \c APP field specifies the name of the application that failed to check in.
*/
#define HS_APPMON_FAIL_ERR_EID 41

/** \brief <tt> 'App Monitor Failure: APP:(\%s): Action: Processor Reset' </tt>
**  \event <tt> 'App Monitor Failure: APP:(\%s): Action: Processor Reset' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a monitored application fails to increment its
**  execution counter in the table specified number of cycles, and the specified
**  action type is Processor Reset.
**
**  The \c APP field specifies the name of the application that failed to check in.
*/
#define HS_APPMON_PROC_ERR_EID 42

/** \brief <tt> 'App Monitor Failure: APP:(\%s): Action: Message Action Index: \%d' </tt>
**  \event <tt> 'App Monitor Failure: APP:(\%s): Action: Message Action Index: \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a monitored application fails to increment its
**  execution counter in the table specified number of cycles, and the specified
**  action type is a Message Action.
**
**  The \c APP field specifies the name of the application that failed to check in.
**  The \c Message \c Action \c Index specifies the message action number.
*/
#define HS_APPMON_MSGACTS_ERR_EID 43

/** \brief <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Message Action Index: \%d' </tt>
**  \event <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Message Action Index: \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a monitored event is detected, and the specified
**  action type is a Message Action.
**
**  The \c APP field specifies the name of the application that sent the message.
**  The \c EID field specifies the event ID in the message.
**  The \c Message \c Action \c Index specifies the message action number.
*/
#define HS_EVENTMON_MSGACTS_ERR_EID 44

/** \brief <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Processor Reset' </tt>
**  \event <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Processor Reset' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an event is received that matches an event in the
**  event monitor table that specifies Processor Reset as the action type
**
**  The \c APP field specifies the name of the application that issued the event and is
**  being restarted.
**  The\c EID specifies the event ID of that message.
*/
#define HS_EVENTMON_PROC_ERR_EID 45

/** \brief <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Restart Application' </tt>
**  \event <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Restart Application' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an event is received that matches an event in the
**  event monitor table that specifies Restart Application as the action type
**
**  The \c APP field specifies the name of the application that issued the event and is
**  being restarted.
**  The\c EID specifies the event ID of that message.
*/
#define HS_EVENTMON_RESTART_ERR_EID 46

/** \brief <tt> 'Call to Restart App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**  \event <tt> 'Call to Restart App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the event monitor attempts to restart a task
**  and is unable to.
**
**  The \c APP field is the name of the application that was being restarted
**  The \c ERR field is the return code from the #CFE_ES_RestartApp
**  function call that generated the error.
*/
#define HS_EVENTMON_NOT_RESTARTED_ERR_EID 47

/** \brief <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Delete Application' </tt>
**  \event <tt> 'Event Monitor: APP:(\%s) EID:(\%d): Action: Delete Application' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an event is received that matches an event in the
**  event monitor table that specifies Delete Application as the action type
**
**  The \c APP field specifies the name of the application that issued the event, and is
**  being deleted.
**  The \c EID field specifies the event ID of that message.
*/
#define HS_EVENTMON_DELETE_ERR_EID 48

/** \brief <tt> 'Call to Delete App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**  \event <tt> 'Call to Delete App Failed: APP:(\%s) ERR: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the event monitor attempts to delete a task
**  and is unable to.
**
**  The \c APP field is the name of the application that was being deleted
**  The \c ERR field is the return code from the #CFE_ES_DeleteApp
**  function call that generated the error.
*/
#define HS_EVENTMON_NOT_DELETED_ERR_EID 49

/** \brief <tt> 'AppMon verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'AppMon verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a table validation has been
**  completed for an application monitor table load
**
**  The \c good field is number of entries that passed.
**  The \c bad field is number of entries that failed.
**  The \c unused field is the number of entries that weren't checked because they
**  were marked unused.
*/
#define HS_AMTVAL_INF_EID 50

/** \brief <tt> 'AppMon verify err: Entry = \%d, Err = \%d, Action = \%d, App = \%s' </tt>
**  \event <tt> 'AppMon verify err: Entry = \%d, Err = \%d, Action = \%d, App = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation
**  fails for a application monitor table load.
**
**  The \c Entry field is the number of the application monitor table entry.
**  The \c Err field is the error id of the error that occurred.
**  The \c Action field is the action listed for the entry.
**  The \c App is field the application name specified in the table.
*/
#define HS_AMTVAL_ERR_EID 51

/** \brief <tt> 'EventMon verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'EventMon verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a table validation has been
**  completed for an event monitor table load
**
**  The \c good field is number of entries that passed.
**  The \c bad field is number of entries that failed.
**  The \c unused field is the number of entries that weren't checked because they
**  were marked unused.
*/
#define HS_EMTVAL_INF_EID 52

/** \brief <tt> 'EventMon verify err: Entry = \%d, Err = \%d, Action = \%d, ID = \%d App = \%s' </tt>
**  \event <tt> 'EventMon verify err: Entry = \%d, Err = \%d, Action = \%d, ID = \%d App = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation
**  fails for an event monitor table load.
**
**  The \c Entry field is the number of the event monitor table entry.
**  The \c Err field is the error id of the error that occurred.
**  The \c Action field is the action listed for the entry.
**  The \c ID field is the Event ID listed in the table.
**  The \c App is field the application name specified in the table.
*/
#define HS_EMTVAL_ERR_EID 53

/** \brief <tt> 'ExeCount verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'ExeCount verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a table validation has been
**  completed for an execution counters table load
**
**  The \c good field is number of entries that passed.
**  The \c bad field is number of entries that failed.
**  The \c unused field is the number of entries that weren't checked because they
**  were marked unused.
*/
#define HS_XCTVAL_INF_EID 54

/** \brief <tt> 'ExeCount verify err: Entry = \%d, Err = \%d, Type = \%d, Name = \%s' </tt>
**  \event <tt> 'ExeCount verify err: Entry = \%d, Err = \%d, Type = \%d, Name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation
**  fails for an execution counter table load.
**
**  The \c Entry field is the number of the execution counter table entry.
**  The \c Err field is the error id of the error that occurred.
**  The \c Type field is the resource type for the entry.
**  The \c Name is field the resource name specified in the table.
*/
#define HS_XCTVAL_ERR_EID 55

/** \brief <tt> 'MsgActs verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'MsgActs verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a table validation has been
**  completed for an message actions table load
**
**  The \c good field is number of entries that passed.
**  The \c bad field is number of entries that failed.
**  The \c unused field is the number of entries that weren't checked because they
**  were marked unused.
*/
#define HS_MATVAL_INF_EID 56

/** \brief <tt> 'MsgActs verify err: Entry = \%d, Err = \%d, Length = \%d, ID = \%d' </tt>
**  \event <tt> 'MsgActs verify err: Entry = \%d, Err = \%d, Length = \%d, ID = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation
**  fails for an message actions table load.
**
**  The \c Entry field is the number of the message action table entry.
**  The \c Err field is the error id of the error that occurred.
**  The \c Length field is the length of the message.
**  The \c ID field is the Message ID of the message.
*/
#define HS_MATVAL_ERR_EID 57

/** \brief <tt> 'Application Monitoring Disabled due to Table Load Failure' </tt>
**  \event <tt> 'Application Monitoring Disabled due to Table Load Failure' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when application monitoring has been disabled
**  due to a table load failure.
*/
#define HS_DISABLE_APPMON_ERR_EID 58

/** \brief <tt> 'Event Monitoring Disabled due to Table Load Failure' </tt>
**  \event <tt> 'Event Monitoring Disabled due to Table Load Failure' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when event monitoring has been disabled
**  due to a table load failure.
*/
#define HS_DISABLE_EVENTMON_ERR_EID 59

/** \brief <tt> 'Error Subscribing to Wakeup,RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to Wakeup,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the call to #CFE_SB_Subscribe
**  for the #HS_WAKEUP_MID, during initialization returns a value
**  other than CFE_SUCCESS
*/
#define HS_SUB_WAKEUP_ERR_EID 60

/** \brief <tt> 'CPU Hogging Detected' </tt>
**  \event <tt> 'CPU Hogging Detected' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CPU monitoring detects
**  that the utilization has exceeded the CPU Hogging threshhold
**  for longer than the CPU Hogging duration
*/
#define HS_CPUMON_HOGGING_ERR_EID 61

/** \brief <tt> 'CPU Hogging Indicator Enabled' </tt>
**  \event <tt> 'CPU Hogging Indicator Enabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an enable cpu hogging indicator
**  command has been received.
*/
#define HS_ENABLE_CPUHOG_DBG_EID 64

/** \brief <tt> 'CPU Hogging Indicator Disabled' </tt>
**  \event <tt> 'CPU Hogging Indicator Disabled' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a disable cpu hogging indicator
**  command has been received.
*/
#define HS_DISABLE_CPUHOG_DBG_EID 65

/** \brief <tt> 'Event Monitor Enable: Error Subscribing to Events,RC=0x\%08X' </tt>
**  \event <tt> 'Event Monitor Enable: Error Subscribing to Events,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a ground command message is received
**  to enable event monitoring while it is disabled, and there is an error
**  subscribing to the event mid.
**
**  The \c Status field indicates the error status passed but the subscribe call.
*/
#define HS_EVENTMON_SUB_EID 66

/** \brief <tt> 'Event Monitor Disable: Error Unsubscribing from Events,RC=0x\%08X' </tt>
**  \event <tt> 'Event Monitor Disable: Error Unsubscribing from Events,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a ground command message is received
**  to disable event monitoring while it is enabled, and there is an error
**  unsubscribing from the event mid.
**
**  The \c Status field indicates the error status passed but the subscribe call.
*/
#define HS_EVENTMON_UNSUB_EID 67

/** \brief <tt> 'Error Unsubscribing from Events,RC=0x\%08X' </tt>
**  \event <tt> 'Error Unsubscribing from Events,RC=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued if when acquiring the event monitor table
**  from table services, it is bad and event monitoring is disabled, but
**  there is a failure unsubscribing from the event mid.
**
**  The \c Status field indicates the error status passed but the subscribe call.
*/
#define HS_BADEMT_UNSUB_EID 68

#endif /* _hs_events_h_ */

/************************/
/*  End of File Comment */
/************************/
