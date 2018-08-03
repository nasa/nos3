/*
**  File: 
**  cfe_es_events.h
**  $Id: cfe_es_events.h 1.6 2014/08/21 15:34:53GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**
**  Purpose:
**  cFE Executive Services (ES) Event IDs
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**
** $Log: cfe_es_events.h  $
** Revision 1.6 2014/08/21 15:34:53GMT-05:00 sstrege 
** Updated CFE_ES_ERREXIT_APP_ERR_EID from 31 to 33
** Revision 1.5 2014/08/19 14:40:27EDT sstrege 
** Fixed doxygen warnings
** Revision 1.4 2014/05/05 15:56:42EDT acudmore 
** Fixed misleading event message text.
** Revision 1.3 2010/11/04 16:17:43GMT-05:00 jmdagost 
** Updated Perf Analyzer start message, inserted perf analyzer error msg, updated subsequent event IDs.
** Revision 1.2 2008/12/08 12:07:19EST dkobe 
** Updates to correct doxygen errors
** Revision 1.1 2008/04/17 08:05:19EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.32 2007/09/20 10:52:56EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.31 2007/09/18 16:39:10EDT apcudmore 
** Updated ES invalid command length event message to make it clear.
** Revision 1.30 2007/06/02 10:10:10EDT dlkobe 
** Added doxygen comments for User's Guides
** Revision 1.29 2007/05/25 11:51:39EDT dlkobe 
** Modified doxygen to improve supporting docs
** Revision 1.28 2007/05/14 15:43:45EDT apcudmore 
** Cleaned up events and disabled debug code for application restart.
** Revision 1.27 2007/05/11 11:59:05EDT apcudmore 
** Removed Event IDs 33 and 34.
** They were no longer needed with the revised startup sync code.
** Revision 1.26 2007/05/11 10:15:07EDT rjmcgraw 
** Added information to comments for CFE_ES_PERF_STOPCMD_EID
** Revision 1.25 2007/05/11 09:32:54EDT rjmcgraw 
** DCR3211:2 Added 3 new perf events for 'in progress' checks
** Revision 1.24 2007/05/09 12:41:24EDT dlkobe 
** Added event messages associated with Dump CDS Registry command
*/
/*************************************************************************/
#ifndef _cfe_es_events_
#define _cfe_es_events_

/*
** ES task event message ID's.
*/

/** \brief <tt> 'cFE ES Initialized' </tt>
**  \event <tt> 'cFE ES Initialized' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Executive Services
**  Task completes its Initialization.
**/
#define CFE_ES_INIT_INF_EID       1    /* start up message "informational" */

/** \brief <tt> 'cFE Version \%d.\%d.\%d chksm \%d, OSAL Version \%d.\%d' </tt>
**  \event <tt> 'cFE Version \%d.\%d.\%d chksm \%d, OSAL Version \%d.\%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Executive Services
**  Task completes its Initialization.
**
**  The \c Version field identifies the tagged version for the cFE Build, the \c chksm field
**  provides the 16-bit checksum of the cFE Build and the \c OSAL \c Version field identifies 
**  the version of the OS Abstraction Layer on which this particular version of the cFE was built.
**/
#define CFE_ES_INITSTATS_INF_EID  2

/** \brief <tt> 'No-op command' </tt>
**  \event <tt> 'No-op command' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Executive Services \link #CFE_ES_NOOP_CC NO-OP command \endlink
**/
#define CFE_ES_NOOP_INF_EID       3    /* processed command "informational" */

/** \brief <tt> 'Reset Counters command' </tt>
**  \event <tt> 'Reset Counters command' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Executive Services \link #CFE_ES_RESET_CC Reset Counters command \endlink
**/
#define CFE_ES_RESET_INF_EID      4

/** \brief <tt> 'Invoked shell command \%s' </tt>
**  \event <tt> 'Invoked shell command \%s' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response
**  to a cFE Executive Services \link #CFE_ES_SHELL_CMD_CC Shell Command \endlink
**
**  The \c 's' string contains the actual shell command string issued.
**/
#define CFE_ES_SHELL_INF_EID      5

/** \brief <tt> 'Started \%s from \%s, AppID = \%d' </tt>
**  \event <tt> 'Started \%s from \%s, AppID = \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is automatically issued upon successful completion of
**  a cFE Executive Services \link #CFE_ES_START_APP_CC Start Application command \endlink
**
**  The first \c 's' string identifies the name of the started Application, the
**  second \c 's' string identifies the filename from which the Application was
**  loaded and the \c AppId field specifies the Application ID assigned to the
**  newly started Application by the cFE Executive Services.
**/
#define CFE_ES_START_INF_EID      6

/** \brief <tt> 'Stop Application \%s Initiated.' </tt>
**  \event <tt> 'Stop Application \%s Initiated.' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the 
**  cFE Executive Services \link #CFE_ES_STOP_APP_CC Stop Application command \endlink
**  Note that when this event is displayed, the Application is not deleted. ES has 
**  accepted the request to delete the application, and it will be deleted after the app exits
**  it's main loop, or times out. 
**
**  The \c 's' field identifies the name of the Application that will be stopped.
**/
#define CFE_ES_STOP_DBG_EID       7

/** \brief <tt> 'Stop Application \%s Completed.' </tt>
**  \event <tt> 'Stop Application \%s Completed.' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the cFE finishes deleting the cFE Application
**  That was started when the \link #CFE_ES_STOP_APP_CC Stop Application command \endlink
**  was issued.
**
**  The \c 's' field identifies the name of the Application that was stopped.
*/
#define CFE_ES_STOP_INF_EID       8

/** \brief <tt> 'Restart Application \%s Initiated.' </tt>
**  \event <tt> 'Restart Application \%s Initiated.' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the 
**  cFE Executive Services \link #CFE_ES_RESTART_APP_CC Restart Application command \endlink
**  Note that when this event is displayed, the Application is not restarted. ES has 
**  accepted the request to restart the application, and it will be restarted after the app exits
**  it's main loop, or times out. 
**
**  The \c 's' field identifies the name of the Application that will be restarted.
**/
#define CFE_ES_RESTART_APP_DBG_EID  9

/** \brief <tt> 'Restart Application \%s Completed.' </tt>
**  \event <tt> 'Restart Application \%s Completed.' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the cFE finishes Restarting the cFE Application
**  That was started when the \link #CFE_ES_RESTART_APP_CC Restart Application command \endlink
**  was issued.
**
**  The \c 's' field identifies the name of the Application that was reloaded.
*/
#define CFE_ES_RESTART_APP_INF_EID  10


/** \brief <tt> 'Reload Application \%s Initiated.' </tt>
**  \event <tt> 'Reload Application \%s Initiated.' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the
**  cFE Executive Services \link #CFE_ES_RELOAD_APP_CC Reload Application command \endlink
**  Note that when this event is displayed, the Application is not reloaded. ES has 
**  accepted the request to reload the application, and it will be reloaded after the app exits
**  it's main loop, or times out. 
**
**
**  The \c 's' field identifies the name of the Application that will be reloaded.
**/
#define CFE_ES_RELOAD_APP_DBG_EID 11


/** \brief <tt> 'Reload Application \%s Completed.' </tt>
**  \event <tt> 'Reload Application \%s Completed.' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the cFE finishes Reloading the cFE Application
**  That was started when the \link #CFE_ES_RELOAD_APP_CC Restart Application command \endlink
**  was issued.
**
**  The \c 's' field identifies the name of the Application that was reloaded.
*/
#define CFE_ES_RELOAD_APP_INF_EID 12 

/** \brief <tt> 'Exit Application \%s Completed.' </tt>
**  \event <tt> 'Exit Application \%s Completed.' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the cFE finishes exiting/cleaning up an 
**  application that called the CFE_ES_ExitApp API with the CFE_ES_APP_EXIT parameter.
**  When an App calls this API, the request is recorded and the Executive Services App will
**  actually delete cFE Application before issuing this event message.
**
**  The \c 's' field identifies the name of the Application that was exited.
*/
#define CFE_ES_EXIT_APP_INF_EID 13

/** \brief <tt> 'Exit Application \%s Completed.' </tt>
**  \event <tt> 'Exit Application \%s Completed.' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the cFE finishes exiting/cleaning up an 
**  application that called the CFE_ES_ExitApp API with an ERROR condition. 
**  When an App calls this API, with the CFE_ES_APP_ERROR parameter, it indicates
**  that the Application exited due to an error condition. The details of the 
**  error that occurred should be given by the Application through an event message,
**  System Log entry, or both.
**  The request is recorded and the Executive Services App will actually delete
**  cFE Application before issuing this event message.
**
**  The \c 's' field identifies the name of the Application that was exited.
*/
#define CFE_ES_ERREXIT_APP_INF_EID 14


/** \brief <tt> 'Sent \%s application data' </tt>
**  \event <tt> 'Sent \%s application data' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful completion of the 
**  cFE Executive Services \link #CFE_ES_QUERY_ONE_CC Query One Application command \endlink
**
**  The \c 's' field identifies the name of the Application whose Executive Services
**  Application information has been telemetered.
**/
#define CFE_ES_ONE_APP_EID       15

/** \brief <tt> 'App Info file written to \%s, Entries=\%d, FileSize=\%d' </tt>
**  \event <tt> 'App Info file written to \%s, Entries=\%d, FileSize=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful completion of the cFE Executive
**  Services \link #CFE_ES_QUERY_ALL_CC Query All Applications command \endlink
**
**  The \c 's' field identifies the name of the file to which all Executive Services Application
**  data has been written.  The \c Entries field identifies, in decimal, the number of Applications
**  whose data was written and the \c FileSize field gives the total number of bytes written to the
**  file.
**/
#define CFE_ES_ALL_APPS_EID      16

/** \brief <tt> 'Cleared Executive Services log data' </tt>
**  \event <tt> 'Cleared Executive Services log data' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the cFE Executive
**  Services \link #CFE_ES_CLEAR_SYSLOG_CC Clear System Log command \endlink
**/
#define CFE_ES_SYSLOG1_INF_EID     17

/** \brief <tt> '\%s written:Size=\%d,Entries=\%d' </tt>
**  \event <tt> '\%s written:Size=\%d,Entries=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when the System Log has been successfully written
**  to a file after receiving the cFE Executive Services \link #CFE_ES_CLEAR_SYSLOG_CC Write Executive
**  Services System Log command \endlink
**
**  The \c 's' field identifies the name of the file written to, the \c Size field specifies, in decimal,
**  the number of bytes written to the file and the \c Entries field identifies the number of System Log
**  messages that were written.
**/
#define CFE_ES_SYSLOG2_EID         18

/** \brief <tt> 'Cleared mode log data' </tt>
**  \event <tt> 'Cleared mode log data' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the cFE Executive
**  Services \link #CFE_ES_CLEAR_ERLOG_CC Clear Exception Reset Log command \endlink
**/
#define CFE_ES_ERLOG1_INF_EID    19

/** \brief <tt> '\%s written:Size=\%d' </tt>
**  \event <tt> '\%s written:Size=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when the Exception Reset Log has been successfully written
**  to a file after receiving the cFE Executive Services \link #CFE_ES_WRITE_ERLOG_CC Write Executive
**  Services Exception Reset Log command \endlink
**
**  The \c 's' field identifies the name of the file written to and the \c Size field specifies, in decimal,
**  the number of bytes written to the file.
**/
#define CFE_ES_ERLOG2_EID        20

/** \brief <tt> 'Invalid command pipe message ID: 0x\%X' </tt>
**  \event <tt> 'Invalid command pipe message ID: 0x\%X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message has arrived on
**  the cFE Executive Services Application's Message Pipe that has a
**  Message ID that is neither #CFE_ES_SEND_HK_MID or #CFE_ES_CMD_MID.
**  Most likely, the cFE Software Bus routing table has become corrupt
**  and is sending messages targeted for other Applications to the cFE
**  Executive Services Application.  
**
**  The \c ID field in the event message identifies
**  the message ID (in hex) that was found in the message.
**/
#define CFE_ES_MID_ERR_EID       21    /* invalid command packet "error" */

/** \brief <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt>
**  \event <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_ES_CMD_MID
**  message ID has arrived but whose Command Code is not one of the command
**  codes specified in \link #CFE_ES_NOOP_CC cfe_es.h \endlink.  This
**  problem is most likely to occur when:
**    -# A Message ID meant for another Application became corrupted and was
**       set equal to #CFE_ES_CMD_MID.
**    -# The Command Code field in the Message became corrupted.
**    -# The command database at the ground station has been corrupted.
**
**  The \c ID field in the event message specifies the Message ID (in hex) and the 
**  \c CC field specifies the Command Code (in decimal) found in the message. 
**/
#define CFE_ES_CC1_ERR_EID       22

/** \brief <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**  \event <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_ES_CMD_MID
**  message ID has arrived but whose packet length does not match the expected
**  length for the specified command code.
**
**  The \c ID field in the event message specifies the Message ID (in hex), the \c CC field
**  specifies the Command Code (in decimal), the \c Exp Len field specified the Expected
**  Length (in decimal ), and \c Len specifies the message Length (in decimal)
**  found in the message. 
**/
#define CFE_ES_LEN_ERR_EID       23

/** \brief <tt> 'Invalid cFE restart type \%d' </tt>
**  \event <tt> 'Invalid cFE restart type \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the cFE Executive Services receives a
**  \link #CFE_ES_RESTART_CC cFE Restart Command \endlink whose parameter
**  identifying the restart type is not equal to either #CFE_ES_PROCESSOR_RESET
**  or #CFE_ES_POWERON_RESET.
**
**  The 'd' field identifies the numeric, in decimal, of the restart type found
**  in the received cFE Restart Command Packet.
**/
#define CFE_ES_BOOT_ERR_EID      24    /* command specific "error" */

/** \brief <tt> 'Failed to invoke shell command \%s, rc = \%08X' </tt>
**  \event <tt> 'Failed to invoke shell command \%s, rc = \%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever the cFE Executive Services receives an OS Shell
**  command, via the \link #CFE_ES_SHELL_CMD_CC Executive Services Shell Command \endlink, and
**  the underlying OS returns an error code.
**
**  The 's' field in the message identifies the shell command string that was issued
**  and the \c rc field displays the shell's return code, in hex.
**/
#define CFE_ES_SHELL_ERR_EID     25

/** \brief <tt> 'Failed to start \%s from \%s, RC = \%08X' </tt>
**  \event <tt> 'Failed to start \%s from \%s, RC = \%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message is a general failure when the command passes the parameter validation, but
**  fails when a call to CFE_ES_AppCreate is called.
**
**  The \c 's' term identifies the name of the Application that was attempted to start.  
**  The second \c 's' field specifies the file from which the Application was loaded.
**  The \c 'X' field is the return code returned by the CFE_ES_AppCreate.
**/
#define CFE_ES_START_ERR_EID     26

/** \brief <tt> 'CFE_ES_StartAppCmd: invalid filename: \%s' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: invalid filename: \%s' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Start Appplication Command is given
**  an invalid filename. ( Either NULL or too short to be a valid cFE file name ).
**
**  The \c 's' term identifies the invalid filename that was sent with the command.
**/
#define CFE_ES_START_INVALID_FILENAME_ERR_EID     27

/** \brief <tt> 'CFE_ES_StartAppCmd: App Entry Point is NULL.' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: App Entry Point is NULL.' </tt>  
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Start Appplication Command is given
**  a NULL Application Entry Point parameter. The command must contain an application entry 
**  point string. ( Example: "SC_AppMain" ).
**
**/
#define CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID     28

/** \brief <tt> 'CFE_ES_StartAppCmd: App Name is NULL.' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: App Name is NULL.' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Start Appplication Command is given
**  a NULL Application Name parameter. The command must contain an application name string.
**/
#define CFE_ES_START_NULL_APP_NAME_ERR_EID     29

/** \brief <tt> 'CFE_ES_StartAppCmd: Stack size is less than system Minimum: \%d.' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: Stack size is less than system Minimum: \%d.' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Application Stack Size parameter is 
**  less than the default stack size defined in the cfe_platform_cfg.h file: CFE_ES_DEFAULT_STACK_SIZE.
**
**  The \c 'd' term identifies the size of the stack that was given in the command.  
**/
#define CFE_ES_START_STACK_ERR_EID     30

/** \brief <tt> 'CFE_ES_StartAppCmd: Priority is too large: \%d.' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: Priority is too large: \%d.' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Application priority greater than the 
**  maximum priority for a Task defined by the OS Abstraction Layer ( 256 ). 
**
**  The \c 'd' term identifies the priority that was given in the command.  
**/
#define CFE_ES_START_PRIORITY_ERR_EID     31

/** \brief <tt> 'CFE_ES_StartAppCmd: Invalid Exception Action: \%d.' </tt>
**  \event <tt> 'CFE_ES_StartAppCmd: Invalid Exception Action: \%d.' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated for an error encountered in response
**  to an Executive Services \link #CFE_ES_START_APP_CC Start Application Command \endlink.
**
**  This message reports a command failure when the Application Exception Action parameter is
**  invalid. The valid options for this parameter are: 0 = Application will restart on an exception
**                                                     1 = Application cause a processor restart on
**                                                         exception.  
**
**  The \c 'd' term identifies the Exception Action parameter that was given in the command.  
**/
#define CFE_ES_START_EXC_ACTION_ERR_EID     32

/** \brief <tt> 'Exit Application \%s on Error Failed: CleanUpApp Error 0x\%08X.' </tt>
**  \event <tt> 'Exit Application \%s on Error Failed: CleanUpApp Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when ES is completing the processing of the 
**  CFE_ES_ExitApp API call with the CFE_ES_APP_ERROR parameter and the call to CFE_ES_CleanUpApp fails. 
**  At this point the Application will likely be stopped or deleted, but it may be in an unknown state.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_ERREXIT_APP_ERR_EID 33

/** \brief <tt> 'Stop Application \%s Failed, RC = 0x\%08X' </tt>
**  \event <tt> 'Stop Application \%s Failed, RC = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_STOP_APP_CC Stop Application Command \endlink which fails.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  stopped and the \c rc field identifies the error code, in hex, that may identify
**  the precise reason for the failure.
**/
#define CFE_ES_STOP_ERR1_EID      35

/** \brief <tt> 'Stop Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt>
**  \event <tt> 'Stop Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_STOP_APP_CC Stop Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_GetAppIDByName fails. The application
**  will not be deleted at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  stopped and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_STOP_ERR2_EID      36

/*
**  "Stop Application \%s Failed: CleanUpApp Error 0x\%08X."
*/

/** \brief <tt> 'Stop Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt>
**  \event <tt> 'Stop Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_STOP_APP_CC Stop Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_GetAppIDByName fails. The application
**  will not be deleted at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  stopped and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_STOP_ERR3_EID      37


/** \brief <tt> 'Restart Application \%s Failed, RC = 0x\%08X' </tt>
**  \event <tt> 'Restart Application \%s Failed, RC = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_RESTART_APP_CC Restart Application
**  Command \endlink fails.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reset and the \c rc field identifies the error code, in hex, that may identify
**  the precise reason for the failure.
**/
#define CFE_ES_RESTART_APP_ERR1_EID 38

/** \brief <tt> 'Restart Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt>
**  \event <tt> 'Restart Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RESTART_APP_CC Restart Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_GetAppIDByName fails. The application
**  will not be restarted at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  restarted and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RESTART_APP_ERR2_EID      39

/*
** "Restart Application \%s Failed: AppCreate Error 0x\%08X."
*/

/** \brief <tt> 'Restart Application \%s Failed: AppCreate Error 0x\%08X.' </tt>
**  \event <tt> 'Restart Application \%s Failed: AppCreate Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RESTART_APP_CC Restart Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_AppCreate fails. The application
**  will not be restarted at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  restarted and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RESTART_APP_ERR3_EID      40


/** \brief <tt> 'Restart Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt>
**  \event <tt> 'Restart Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RESTART_APP_CC Restart Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_CleanUpApp fails. The application
**  will not be restarted at this point, but will likely be deleted or in an unknown state.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  restarted and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RESTART_APP_ERR4_EID      41


/** \brief <tt> 'Failed to reload Application \%s, rc = \%08X' </tt>
**  \event <tt> 'Failed to reload Application \%s, rc = \%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_RELOAD_APP_CC Reload Application
**  Command \endlink fails.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c rc field identifies the error code, in hex, that may identify
**  the precise reason for the failure.
**/
#define CFE_ES_RELOAD_APP_ERR1_EID 42

/** \brief <tt> 'Reload Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt>
**  \event <tt> 'Reload Application \%s, GetAppIDByName failed. RC = 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RELOAD_APP_CC Reload Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_GetAppIDByName fails. The application
**  will not be reloaded at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RELOAD_APP_ERR2_EID      43


/** \brief <tt> 'Reload Application \%s Failed: AppCreate Error 0x\%08X.' </tt>
**  \event <tt> 'Reload Application \%s Failed: AppCreate Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RELOAD_APP_CC Reload Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_AppCreate fails. The application
**  will not be reloaded at this point. 
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RELOAD_APP_ERR3_EID      44


/** \brief <tt> 'Reload Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt>
**  \event <tt> 'Reload Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an Executive Services 
**  \link #CFE_ES_RELOAD_APP_CC Reload Application Command \endlink which fails. This message
**  is for a specific failure when the call to CFE_ES_CleanUpApp fails. The application
**  will not be reloaded at this point, and will likely be deleted or in an unknown state.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_RELOAD_APP_ERR4_EID 45


/** \brief <tt> 'Exit Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt>
**  \event <tt> 'Exit Application \%s Failed: CleanUpApp Error 0x\%08X.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when ES is completing the processing of the 
**  CFE_ES_ExitApp API call and the call to CFE_ES_CleanUpApp fails. At this point the Application will
**  likely be stopped or deleted, but it may be in an unknown state.
**
**  The \c 's' field identifies the name of the Application which was attempted to be
**  reloaded and the \c RC field identifies the error code, in hex, that will identify
**  the precise reason for the failure.
**/
#define CFE_ES_EXIT_APP_ERR_EID 46


/** \brief <tt> 'ES_ProcControlReq: Invalid State (EXCEPTION) Application \%s.' </tt>
**  \event <tt> 'ES_ProcControlReq: Invalid State (EXCEPTION) Application \%s.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when ES is processing it's internal Application table and encounters
**  an App with the EXCEPTION state. Because exceptions are supposed to be processed immediately, this is 
**  an invalid state and should not happen. It may indicate some sort of memory corruption or other problem.
**/
#define CFE_ES_PCR_ERR1_EID 47


/*
** "CFE_ES_CleanUpApp: Unknown State ( \%d ) Application \%s."
*/
/** \brief <tt> 'ES_ProcControlReq: Unknown State ( \%d ) Application \%s.' </tt>
**  \event <tt> 'ES_ProcControlReq: Unknown State ( \%d ) Application \%s.' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when ES is processing it's internal Application table and encounters
**  an App with an unknown state. If this message occurs, it might be an indication of a memory corruption
**  or other problem.
**/
#define CFE_ES_PCR_ERR2_EID 48

/** \brief <tt> 'Failed to send \%s application data, RC = \%08X' </tt>
**  \event <tt> 'Failed to send \%s application data, RC = \%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ONE_CC Request Application
**  Data Command \endlink failed.
**
**  The \c 's' field identifies the name of the Application whose data was attempted 
**  to be telemetered and the \c rc field identifies the error code, in hex, that may identify
**  the precise reason for the failure.
**/
#define CFE_ES_ONE_ERR_EID       49

/** \brief <tt> 'Failed to send \%s application data: GetAppIDByName Failed, RC = 0x\%08X' </tt>
**  \event <tt> 'Failed to send \%s application data: GetAppIDByName Failed, RC = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ONE_CC Request Application
**  Data Command \endlink failed.
**
**  The \c 's' field identifies the name of the Application whose data was attempted 
**  to be telemetered and the \c rc field identifies the error code, in hex, that may identify
**  the precise reason for the failure.
**/
#define CFE_ES_ONE_APPID_ERR_EID       50


/** \brief <tt> 'Failed to write App Info file, OS_creat returned \%d' </tt>
**  \event <tt> 'Failed to write App Info file, OS_creat returned \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ALL_CC Dump Application
**  Data Command \endlink fails to create the dump file.
**
**  The \c 'd' parameter identifies, in decimal, the error code returned by #OS_creat when the attempt was made
**  to create the file.
**/
#define CFE_ES_OSCREATE_ERR_EID  51

/** \brief <tt> 'Failed to write App Info file, WriteHdr rtnd \%08X, exp \%d' </tt>
**  \event <tt> 'Failed to write App Info file, WriteHdr rtnd \%08X, exp \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ALL_CC Dump Application
**  Data Command \endlink fails while writing the cFE Standard File Header.
**
**  The \c rtnd field contains the error code returned by the #CFE_FS_WriteHeader API.  Nominally, the
**  returned result should have been equal to the \c exp field (i.e. - sizeof(#CFE_FS_Header_t)).
**/
#define CFE_ES_WRHDR_ERR_EID     52


/** \brief <tt> 'Failed to write App Info file, Task write RC = 0x\%08X, exp \%d' </tt>
**  \event <tt> 'Failed to write App Info file, Task write RC = 0x\%08X, exp \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever an Executive Services \link #CFE_ES_QUERY_ALL_CC Dump Application
**  Data Command \endlink fails while writing Application data to the specified file.
**
**  The \c rtnd field contains, in hex, the error code returned from the #OS_write API.  The expected return
**  value is identified, in decimal, in the \c exp field.
**/
#define CFE_ES_TASKWR_ERR_EID    53

/** \brief <tt> 'Error creating file \%s, stat=0x\%x' </tt>
**  \event <tt> 'Error creating file \%s, stat=0x\%x' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_WRITE_SYSLOG_CC Dump System Log
**  Command \endlink fails while attempting to create the specified file.
**
**  The \c 's' field identifies the name of the file that was attempted to be created and the \c stat field
**  specifies, in hex, the error code returned by the #OS_creat API.
**/
#define CFE_ES_SYSLOG2_ERR_EID     55

/** \brief <tt> 'Error creating file \%s, stat=0x\%x' </tt>
**  \event <tt> 'Error creating file \%s, stat=0x\%x' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_WRITE_ERLOG_CC Dump Exception Reset Log
**  Command \endlink fails while attempting to create the specified file.
**
**  The \c 's' field identifies the name of the file that was attempted to be created and the \c stat field
**  specifies, in hex, the error code returned by the #OS_creat API.
**/
#define CFE_ES_ERLOG2_ERR_EID    56

/** \brief <tt> 'Start collecting performance data command, trigger mode = %d' </tt>
**  \event <tt> 'Start collecting performance data command, trigger mode = %d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_STARTDATA_CC Start Performance Analyzer Data Collection Command \endlink
**
**  The \c 'd' field identifies the requested trigger mode.  Valid values are #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_START (0),
**  #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_CENTER (1), and #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_END (2)
**/
#define CFE_ES_PERF_STARTCMD_EID      57

/** \brief <tt> 'Cannot start collecting performance data,perf data write in progress' </tt>
**  \event <tt> 'Cannot start collecting performance data,perf data write in progress' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_STARTDATA_CC Start Performance Analyzer Data Collection Command \endlink
**/
#define CFE_ES_PERF_STARTCMD_ERR_EID      58


/** \brief <tt> 'Cannot start collecting performance data, trigger mode (%d) out of range (%d to %d)' </tt>
**  \event <tt> 'Cannot start collecting performance data, trigger mode (%d) out of range (%d to %d)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_PERF_STARTDATA_CC 
**  Start Performance Analyzer Data Collection Command \endlink command is received with a bad
**  value for the requested trigger mode.
**
**  The first \c 'd' field identifies the received trigger mode value.
**  The second and third \c 'd' fields specify the valid range of values for the trigger mode.  Valid values 
**  are #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_START (0), #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_CENTER (1), and 
**  #CFE_ES_PerfMode_t CFE_ES_PERF_TRIGGER_END (2).
**/
#define CFE_ES_PERF_STARTCMD_TRIG_ERR_EID      59


/** \brief <tt> 'Perf Stop Cmd Rcvd,\%s will write \%d entries.\%dmS dly every \%d entries' </tt>
**  \event <tt> 'Perf Stop Cmd Rcvd,\%s will write \%d entries.\%dmS dly every \%d entries' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon receipt of a successful Performance Data Stop
**  Command after receiving the cFE Executive Services \link #CFE_ES_PERF_STOPDATA_CC Stop 
**  Performance Analyzer Data Collection Command \endlink
**
**  The \c 's' field identifies the name of the file write task that has begun execution.
**  The first \c 'd' identifies the total number of performance entries(in decimal) that will be written to the file.
**  A performance data entry is defined by an unsigned 32 bit data point and an unsigned 64 bit time stamp.
**  The second \c 'd' identifies the millisecond delay between writes and the 
**  third \c 'd' identifies the number of entries written (in decimal) between delays. 
**/
#define CFE_ES_PERF_STOPCMD_EID       60


/** \brief <tt> 'Stop performance data cmd,Error creating child task RC=0x\%08X' </tt>
**  \event <tt> 'Stop performance data cmd,Error creating child task RC=0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an unsuccessful Performance Data Stop
**  Command after receiving the cFE Executive Services \link #CFE_ES_PERF_STOPDATA_CC Stop 
**  Performance Analyzer Data Collection Command \endlink
**
**  The \c 'RC' field specifies, in hex, the error code returned by the #CFE_ES_CreateChildTask API
**  
**/
#define CFE_ES_PERF_STOPCMD_ERR1_EID       61


/** \brief <tt> 'Stop performance data cmd ignored,perf data write in progress' </tt>
**  \event <tt> 'Stop performance data cmd ignored,perf data write in progress' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an unsuccessful Performance Data Stop
**  Command after receiving the cFE Executive Services \link #CFE_ES_PERF_STOPDATA_CC Stop 
**  Performance Analyzer Data Collection Command \endlink
**
**/
#define CFE_ES_PERF_STOPCMD_ERR2_EID       62


/** \brief <tt> 'Set Performance Filter Mask command' </tt>
**  \event <tt> 'Set Performance Filter Mask command' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_SETFILTERMASK_CC Set Performance Analyzer Filter Mask Command \endlink.
**
**/
#define CFE_ES_PERF_FILTMSKCMD_EID  63

/** \brief  <tt> 'Error:Performance Filter Mask Index value  
** greater than CFE_ES_PERF_32BIT_WORDS_IN_MASK (which is a whole number derived from 
** CFE_ES_PERF_MAX_IDS / 32)' </tt>
**  \event <tt> 'Error:Performance Filter Mask Index value  
** greater than CFE_ES_PERF_32BIT_WORDS_IN_MASK (which is a whole number derived from 
** CFE_ES_PERF_MAX_IDS / 32)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_SETFILTERMASK_CC Set Performance Analyzer Filter Mask Command \endlink.
**
**/
#define CFE_ES_PERF_FILTMSKERR_EID  64


/** \brief <tt> 'Set Performance Trigger Mask  command' </tt>
**  \event <tt> 'Set Performance Trigger Mask  command' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_SETTRIGMASK_CC Set Performance Analyzer Trigger Mask Command \endlink.
**
**/
#define CFE_ES_PERF_TRIGMSKCMD_EID 65

/** \brief  <tt> 'Error: Performance Trigger Mask Index value  
** greater than CFE_ES_PERF_32BIT_WORDS_IN_MASK (which is a whole number derived from 
** CFE_ES_PERF_MAX_IDS / 32)' </tt>
**  \event <tt> 'Error: Performance Trigger Mask Index value  
** greater than CFE_ES_PERF_32BIT_WORDS_IN_MASK (which is a whole number derived from 
** CFE_ES_PERF_MAX_IDS / 32)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated in response to receiving an Executive Services
**  \link #CFE_ES_PERF_SETTRIGMASK_CC Set Performance Analyzer Trigger Mask Command \endlink.
**
**/
#define CFE_ES_PERF_TRIGMSKERR_EID 66


/** \brief <tt> 'Error creating file \%s, stat=\%d' </tt>
**  \event <tt> 'Error creating file \%s, stat=\%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_PERF_STOPDATA_CC Stop Performance Analyzer Data Collection Command \endlink
**  fails to create the associated logic analyzer dump file.
**
**  The \c 's' field identifies the name of the file that was attempted to be created and the \c stat field
**  specifies, in decimal, the error code returned by the #OS_creat API.
**/
#define CFE_ES_PERF_LOG_ERR_EID     67


/** \brief <tt> '\%s written:Size=\%d,EntryCount=\%d' </tt>
**  \event <tt> '\%s written:Size=\%d,EntryCount=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when the Performance Log has been successfully written
**  to a file after receiving the cFE Executive Services \link #CFE_ES_PERF_STOPDATA_CC Stop 
**  Performance Analyzer Data Collection Command \endlink
**
**  The \c 's' field identifies the name of the file written to, the \c Size field specifies, in decimal,
**  the number of bytes written to the file and the \c EntryCount field identifies the number of data
**  entries that were written.
**/
#define CFE_ES_PERF_DATAWRITTEN_EID     68

/** \brief <tt> '\%s Failed to Register CDS '\%s', Status=0x\%08X' </tt>
**  \event <tt> '\%s Failed to Register CDS '\%s', Status=0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever an Application calls the #CFE_ES_RegisterCDS API and
**  fails to successfully create the desired CDS.
**
**  The first \c 's' field identifies the name of the Application which made the API call, the second
**  \c 's' field specifies the name of the CDS as requested by the Application and the \c Status field
**  provides the error code which identifies in more detail the nature of the failure (See return codes
**  for the #CFE_ES_RegisterCDS API).
**/
#define CFE_ES_CDS_REGISTER_ERR_EID     69

/** \brief <tt> 'Set OverWriteSysLog Command Received with Mode setting = \%d' </tt>
**  \event <tt> 'Set OverWriteSysLog Command Received with Mode setting = \%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of an Executive Services \link #CFE_ES_OVERWRITE_SYSLOG_CC
**  Set System Log Overwrite Mode Command \endlink.
**
**  The \c setting field identifies the newly chosen Overwrite Mode and should be equal to either
**  #CFE_ES_LOG_OVERWRITE or #CFE_ES_LOG_DISCARD.
**/
#define CFE_ES_SYSLOGMODE_EID           70

/** \brief <tt> 'Set OverWriteSysLog Command: Invalid Mode setting = \%d' </tt>
**  \event <tt> 'Set OverWriteSysLog Command: Invalid Mode setting = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon unsuccessful completion of an Executive Services \link #CFE_ES_OVERWRITE_SYSLOG_CC
**  Set System Log Overwrite Mode Command \endlink.
**
**  The \c setting field identifies the illegal Overwrite Mode found in the command message.  The mode
**  must be either #CFE_ES_LOG_OVERWRITE (0) or #CFE_ES_LOG_DISCARD (1).
**/
#define CFE_ES_ERR_SYSLOGMODE_EID       71

/** \brief <tt> 'Reset Processor Reset Count to Zero' </tt>
**  \event <tt> 'Reset Processor Reset Count to Zero' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always generated in response to the Executive Services
**  \link #CFE_ES_RESET_PR_COUNT_CC Set Processor Reset Counter to Zero Command \endlink.
**/
#define CFE_ES_RESET_PR_COUNT_EID       72

/** \brief <tt> 'Maximum Processor Reset Count set to: \%d' </tt>
**  \event <tt> 'Maximum Processor Reset Count set to: \%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always generated in response to the Executive Services
**  \link #CFE_ES_RESET_PR_COUNT_CC Set Maximum Processor Reset Limit Command \endlink.
**
**  The \c 'd' field identifies, in decimal, the number of Processor Resets that will need
**  to occur before a Power-On Reset is automatically performed.
**/
#define CFE_ES_SET_MAX_PR_COUNT_EID     73

/** \brief <tt> 'File write,byte cnt err,file \%s,request=\%d,actual=\%d' </tt>
**  \event <tt> 'File write,byte cnt err,file \%s,request=\%d,actual=\%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated in response to any command requesting information to be written
**  to a file and whose data is not completely written to the specified file.
**
**  The \c file field identifies the filename of the file to which the data failed to write completely,
**  the \c request field specifies, in decimal, the number of bytes that were attempted to be written and
**  the \c actual field indicates, in decimal, the actual number of bytes written to the file.
**/
#define CFE_ES_FILEWRITE_ERR_EID        74

/** \brief <tt> 'Error accessing ER Log,\%s not written.Stat=0x\%08x' </tt>
**  \event <tt> 'Error accessing ER Log,\%s not written.Stat=0x\%08x' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated in response to an Exception Reset Log Dump command and there is
**  an error obtaining the contents of the ER Log.
**
**  The \c 's' field identifies the filename of the file to which the data failed to write,
**  the \c Stat field specifies, in hex, the error status returned from #CFE_PSP_GetResetArea.
**/
#define CFE_ES_RST_ACCESS_EID           75

/** \brief <tt> 'Error while deleting '\%s' from CDS, See SysLog.(Err=0x\%08X)' </tt>
**  \event <tt> 'Error while deleting '\%s' from CDS, See SysLog.(Err=0x\%08X)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink
**  fails to cleanly remove the specified CDS.
**
**  The \c 's' field identifies the name of the CDS that was attempted to be deleted the \c Err field
**  specifies, in hex, the error code.
**/
#define CFE_ES_CDS_DELETE_ERR_EID       76

/** \brief <tt> 'Unable to locate '\%s' in CDS Registry' </tt>
**  \event <tt> 'Unable to locate '\%s' in CDS Registry' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink
**  specifies a name for a CDS that cannot be found in the CDS Registry.
**
**  The \c 's' field identifies the name of the CDS that was attempted to be deleted.
**/
#define CFE_ES_CDS_NAME_ERR_EID         77

/** \brief <tt> 'Successfully removed '\%s' from CDS' </tt>
**  \event <tt> 'Successfully removed '\%s' from CDS' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink
**  is successfully completed.
**
**  The \c 's' field identifies the name of the CDS that was deleted.
**/
#define CFE_ES_CDS_DELETED_INFO_EID     78

/** \brief <tt> 'CDS '\%s' is a Critical Table CDS. Must be deleted via TBL Command' </tt>
**  \event <tt> 'CDS '\%s' is a Critical Table CDS. Must be deleted via TBL Command' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink
**  specifies a name for a CDS that is a Critical Table image.
**  Critical Table images can only be deleted via a Table Services
**  command (#CFE_TBL_DELETE_CDS_CC).
**
**  The \c 's' field identifies the name of the CDS that was attempted to be deleted.
**/
#define CFE_ES_CDS_DELETE_TBL_ERR_EID   79

/** \brief <tt> 'CDS '\%s' not deleted because owning app is active' </tt>
**  \event <tt> 'CDS '\%s' not deleted because owning app is active' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DELETE_CDS_CC Delete CDS Command \endlink
**  specifies a name for a CDS whose prefix name identifies
**  an application that is still registered in the system.
**  CDSs can only be deleted when their owning applications
**  have been removed from the system.
**
**  The \c 's' field identifies the name of the CDS that was attempted to be deleted.
**/
#define CFE_ES_CDS_OWNER_ACTIVE_EID     80

/** \brief <tt> 'Successfully telemetered memory pool stats for 0x\%08X' </tt>
**  \event <tt> 'Successfully telemetered memory pool stats for 0x\%08X' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated following successful execution of the 
**  \link #CFE_ES_TLM_POOL_STATS_CC Telemeter Memory Statistics Command \endlink.
**/
#define CFE_ES_TLM_POOL_STATS_INFO_EID  81

/** \brief <tt> 'Cannot telemeter memory pool stats. Illegal Handle (0x\%08X)' </tt>
**  \event <tt> 'Cannot telemeter memory pool stats. Illegal Handle (0x\%08X)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_TLM_POOL_STATS_CC Telemeter Memory Statistics Command \endlink
**  specifies a memory pool handle that is invalid.  A handle is determined to
**  be invalid when any of the following are true:
**  -# The handle does not contain a value that is an integral multiple of 4
**  -# The handle does not specify a valid area of memory
**  -# The handle does not point to an area of memory that contains the handle itself
**  -# The handle does not point to an area of memory whose Size field is an integral multiple of 4
**  -# The handle does not point to an area of memory whose End field is equal to the Start plus the Size
**
**  The \c '08X' field identifies the handle that was found in the command.
**/
#define CFE_ES_INVALID_POOL_HANDLE_ERR_EID   82

/** \brief <tt> 'Successfully dumped CDS Registry to '\%s':Size=\%d,Entries=\%d' </tt>
**  \event <tt> 'Successfully dumped CDS Registry to '\%s':Size=\%d,Entries=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DUMP_CDS_REG_CC Dump Critical Data Store Registry Command \endlink
**  is successfully executed.  The specified file should have been created
**  and contains the CDS Registry Entries. 
**
**  The \c 's' field identifies the CDS Registry Dump Filename.
**  The first \c 'd' field specifies the size of the file (in bytes)
**  The second \c 'd' field specifies the number of CDS Registry Records that were written
**/
#define CFE_ES_CDS_REG_DUMP_INF_EID          83

/** \brief <tt> 'Error writing CDS Registry to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing CDS Registry to '\%s', Status=0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DUMP_CDS_REG_CC Dump Critical Data Store Registry Command \endlink
**  was being performed and it encountered a filesystem write error while writing
**  a CDS Registry record. 
**
**  The \c 's' field identifies the CDS Registry Dump Filename.
**  The \c '08X' field identifies the error code returned from #OS_write that caused the command to abort.
**/
#define CFE_ES_CDS_DUMP_ERR_EID              84

/** \brief <tt> 'Error writing cFE File Header to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing cFE File Header to '\%s', Status=0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DUMP_CDS_REG_CC Dump Critical Data Store Registry Command \endlink
**  command successfully created the CDS Dump File onboard but encountered an error
**  while writing the standard cFE File Header to the file. 
**
**  The \c 's' field identifies the CDS Registry Dump Filename.
**  The \c '08X' field identifies error code returned by the API #CFE_FS_WriteHeader.
**/
#define CFE_ES_WRITE_CFE_HDR_ERR_EID          85

/** \brief <tt> 'Error creating CDS dump file '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error creating CDS dump file '\%s', Status=0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services 
**  \link #CFE_ES_DUMP_CDS_REG_CC Dump Critical Data Store Registry Command \endlink
**  is unable to create the specified file on the onboard filesystem. 
**
**  The \c 's' field identifies the CDS Registry Dump Filename.
**  The \c '08X' field identifies error code returned by the API #OS_creat.
**/
#define CFE_ES_CREATING_CDS_DUMP_ERR_EID      86


/** \brief <tt> 'Task Info file written to \%s, Entries=\%d, FileSize=\%d' </tt>
**  \event <tt> 'Task Info file written to \%s, Entries=\%d, FileSize=\%d' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful completion of the cFE Executive
**  Services \link #CFE_ES_QUERY_ALL_TASKS_CC Query All Tasks command \endlink
**
**  The \c 's' field identifies the name of the file to which all Executive Services Task
**  data has been written.  The \c Entries field identifies, in decimal, the number of Tasks
**  whose data was written and the \c FileSize field gives the total number of bytes written to the
**  file.
**/
#define CFE_ES_TASKINFO_EID      87


/** \brief <tt> 'Failed to write Task Info file, OS_creat returned \%d' </tt>
**  \event <tt> 'Failed to write Task Info file, OS_creat returned \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ALL_TASKS_CC Dump Task
**  Data Command \endlink fails to create the dump file.
**
**  The \c 'd' parameter identifies, in decimal, the error code returned by #OS_creat when the attempt was made
**  to create the file.
**/
#define CFE_ES_TASKINFO_OSCREATE_ERR_EID  88

/** \brief <tt> 'Failed to write Task Info file, WriteHdr rtnd \%08X, exp \%d' </tt>
**  \event <tt> 'Failed to write Task Info file, WriteHdr rtnd \%08X, exp \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Executive Services \link #CFE_ES_QUERY_ALL_TASKS_CC Dump Task
**  Data Command \endlink fails while writing the cFE Standard File Header.
**
**  The \c rtnd field contains the error code returned by the #CFE_FS_WriteHeader API.  Nominally, the
**  returned result should have been equal to the \c exp field (i.e. - sizeof(#CFE_FS_Header_t)).
**/
#define CFE_ES_TASKINFO_WRHDR_ERR_EID     89


/** \brief <tt> 'Failed to write Task Info file, Task write RC = 0x\%08X, exp \%d' </tt>
**  \event <tt> 'Failed to write Task Info file, Task write RC = 0x\%08X, exp \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated whenever an Executive Services \link #CFE_ES_QUERY_ALL_TASKS_CC Dump Tasks
**  Data Command \endlink fails while writing Tasks data to the specified file.
**
**  The \c rtnd field contains, in hex, the error code returned from the #OS_write API.  The expected return
**  value is identified, in decimal, in the \c exp field.
**/
#define CFE_ES_TASKINFO_WR_ERR_EID    90


/** \brief <tt> 'Mission %s.%s, %s, %s' </tt>
**  \event <tt> 'Mission %s.%s, %s, %s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Executive Services
**  Task completes its Initialization
**
**  The \c Mission field identifies the tagged build identifiers and configuration name.
**  If available, this will also indicate the revision control identifiers for CFE and OSAL
**  that this binary was built with.
**/
#define CFE_ES_VERSION_INF_EID    91

/** \brief <tt> 'Build %s %s' </tt>
**  \event <tt> 'Build %s %s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Executive Services
**  Task completes its Initialization, and as part of the Noop command.
**
**  The \c Build field identifies the build date, time, hostname and user identifier of
**  the build host machine for the current running binary.  The first string is the
**  build date/time, and the second string is formatted as "user@hostname"
**
**  By default, if not specified/overridden, the default values of these variables will be:
**    BUILDDATE ==> the output of "date +%Y%m%d%H%M"
**    HOSTNAME ==> the output of "hostname"
**    USER ==> the output of "whoami"
**
**  The values can be overridden by setting an environment variable with the names above
**  to the value desired for the field when running "make".
**/
#define CFE_ES_BUILD_INF_EID    92



#endif /* _cfe_es_events_ */

/************************/
/*  End of File Comment */
/************************/
