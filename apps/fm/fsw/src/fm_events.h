/*
** $Id: fm_events.h 1.24.1.2 2015/02/28 18:10:52EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: CFS File Manager (FM) Application Event ID Header File
**
** Purpose: Specification for the CFS File Manager Event Identifers.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_events.h  $
** Revision 1.24.1.2 2015/02/28 18:10:52EST sstrege 
** Added copyright information
** Revision 1.24.1.1 2015/01/16 16:06:03EST lwalling 
** Change event 42 type from debug to information
** Revision 1.24 2014/06/30 17:03:39EDT sjudy 
** Changed some command event types from INFO to DEBUG.
** Revision 1.23 2011/07/04 15:44:06EDT lwalling 
** Add child task events for delete, move, rename, create dir and delete dir commands
** Revision 1.22 2011/04/19 15:54:58EDT lwalling 
** Added overwrite argument error events for copy and move commands, renumbered events
** Revision 1.21 2011/04/19 11:06:49EDT lwalling 
** Change empty queue and invalid queue index from cmd execution errors to task termination errors
** Revision 1.20 2011/04/19 10:05:31EDT lwalling 
** Add error event text for attempt to invoke child task command handler when disabled
** Revision 1.19 2010/04/12 11:29:11EDT lwalling 
** Added definition for table verify summary event
** Revision 1.18 2010/03/03 18:22:13EST lwalling 
** Changed some Doxygen symbols, changed cmd names to match ASIST database
** Revision 1.17 2009/11/20 15:30:58EST lwalling 
** Remove events from FM_AppendPathSep, add events for invalid and missing directory names
** Revision 1.16 2009/11/13 16:31:29EST lwalling 
** Add more CRC events, modify comment text, add SetTableEntryState events
** Revision 1.15 2009/11/09 16:59:41EST lwalling 
** Update doxygen comments and event text for event descriptions
** Revision 1.14 2009/10/30 14:02:33EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.13 2009/10/28 16:40:57EDT lwalling
** Complete effort to replace the use of phrase device table with file system free space table
** Revision 1.12 2009/10/28 16:30:10EDT lwalling
** Modify events generated during table verification
** Revision 1.11 2009/10/27 17:27:38EDT lwalling
** Add a Get File Info warning event for failed CRC calculations
** Revision 1.10 2009/10/26 16:41:25EDT lwalling
** Add child queue error event to GetFileInfo command handler
** Revision 1.9 2009/10/26 11:30:59EDT lwalling
** Remove Close File command from FM application
** Revision 1.8 2009/10/23 14:49:07EDT lwalling
** Update event text and descriptions of event text
** Revision 1.7 2009/10/16 15:48:41EDT lwalling
** Update event text, event descriptions, event ID names
** Revision 1.6 2009/10/09 17:23:52EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.5 2009/09/28 14:15:28EDT lwalling
** Create common filename verification functions
** Revision 1.4 2008/12/30 15:03:49EST sstrege
** Updated "command received" events to "command completed" events
** Updated event types for events 51, 52, 53, 68, 71, 77, 88, 94, 97, 104
** Removed FM_DIR_LIST_FILE_CMD_EID
** Revision 1.3 2008/12/24 16:25:50EST sstrege
** Added new DeleteFile and DeleteAllFiles Event IDs
** Updated Event IDs
** Revision 1.2 2008/06/20 16:21:36EDT slstrege
** Member moved from fsw/src/fm_events.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_events.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:36ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#ifndef _fm_events_h_
#define _fm_events_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM event message ID's                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \brief <tt> 'Initialization Complete' </tt>
**  \event <tt> 'Initialization complete: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is issued after the File Manager application has
**  successfully completed startup initialization.
**
**  The version numbers indicate the application major version number,
**  minor version number, revision number and mission revision number.
*/
#define FM_STARTUP_EID                      1


/** \brief <tt> 'Initialization Error: Register for Event Services' </tt>
**  \event <tt> 'Initialization error: register for event services: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to register for event services during startup
**  initialization.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_EVS_Register.
*/
#define FM_STARTUP_EVENTS_ERR_EID           2


/** \brief <tt> 'Initialization Error: Create SB Input Pipe' </tt>
**  \event <tt> 'Initialization error: create SB input pipe: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to create a Software Bus input pipe during startup
**  initialization.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_CreatePipe.
*/
#define FM_STARTUP_CREAT_PIPE_ERR_EID       3


/** \brief <tt> 'Initialization Error: Subscribe to HK Request' </tt>
**  \event <tt> 'Initialization error: subscribe to HK request: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to subscribe to the HK telemetry request command
**  during startup initialization.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define FM_STARTUP_SUBSCRIB_HK_ERR_EID      4


/** \brief <tt> 'Initialization Error: Subscribe to FM Commands' </tt>
**  \event <tt> 'Initialization error: subscribe to FM commands: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to subscribe to the FM ground command packet
**  during startup initialization.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define FM_STARTUP_SUBSCRIB_GCMD_ERR_EID    5


/** \brief <tt> 'Initialization Error: Register Free Space Table' </tt>
**  \event <tt> 'Initialization error: register free space table: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to register its file system free space table
**  during startup initialization.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_TBL_Register.
*/
 #define FM_STARTUP_TABLE_INIT_ERR_EID      6


/** \brief <tt> 'Main Loop Error: Software Bus Receive' </tt>
**  \event <tt> 'Main Loop Error: SB receive: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  failed in its attempt to read from its Software Bus input pipe while
**  processing the software main loop sequence.
**
**  This is a fatal error that will cause the File Manager application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_RcvMsg.
*/
#define FM_SB_RECEIVE_ERR_EID               7


/** \brief <tt> 'Application Terminating' </tt>
**  \event <tt> 'Application terminating: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the File Manager application is about
**  to terminate.
**
**  A non-zero result value in the event text is the error code from a
**  fatal error that has occurred.  Fatal errors all have descriptive
**  events.
**
**  If the result value in the event text is zero, then it is likely that
**  the CFE has terminated the FM application, presumably by command.
*/
#define FM_EXIT_ERR_EID                     8


/** \brief <tt> 'Main Loop Error: Invalid Message ID' </tt>
**  \event <tt> 'Main loop error: invalid message ID: mid = 0x\%04X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  received an unexpected Software Bus packet.  There is no obvious
**  explanation of why or how FM could receive such a packet.
**
**  The number in the message text is the unexpected MessageID.
*/
#define FM_MID_ERR_EID                      9


/** \brief <tt> 'Main Loop Error: Invalid Command Code' </tt>
**  \event <tt> 'Main loop error: invalid command code: cc = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the File Manager application has
**  received a command packet with an unexpected command code value.
**
**  Mal-formed command packets are generally prevented by the ground
**  system.  Therefore, the source for the problem command is likely
**  to be one of the on-board tables that contain commands.
**
**  The number in the message text is the unexpected command code.
*/
#define FM_CC_ERR_EID                       10


/** \brief <tt> 'HK Request Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'HK Request error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a housekeeping
**  request command packet with an invalid length.
*/
#define FM_HK_REQ_ERR_EID                   11


/** \brief <tt> 'No-op Command Success' </tt>
**  \event <tt> 'No-op command: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Noop command.
**
**  The version data includes the application major version, minor version,
**  revision and mission revision numbers.
*/
#define FM_NOOP_CMD_EID                     12


/** \brief <tt> 'No-op Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'No-op error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Noop
**  command packet with an invalid length.
*/
#define FM_NOOP_PKT_ERR_EID                 13


/** \brief <tt> 'Reset Counters Command Success' </tt>
**  \event <tt> 'Reset Counters command' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command resets housekeeping
**  telemetry counters that also signal the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_ResetCtrs command.
*/
#define FM_RESET_CMD_EID                    14


/** \brief <tt> 'Reset Counters Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Reset Counters error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_ResetCtrs
**  command packet with an invalid length.
*/
#define FM_RESET_PKT_ERR_EID                15


/** \brief <tt> 'Copy File Command Success' </tt>
**  \event <tt> 'Copy File command: src = \%s, tgt = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Copy command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
 #define FM_COPY_CMD_EID                    16


/** \brief <tt> 'Copy File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Copy File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Copy
**  command packet with an invalid length.
*/
#define FM_COPY_PKT_ERR_EID                 17


/** \brief <tt> 'Copy File Error: Invalid Overwrite' </tt>
**  \event <tt> 'Copy File error: invalid overwrite = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Copy
**  command packet with an invalid overwrite argument.  Overwrite
**  must be set to TRUE (one) or FALSE (zero).
*/
#define FM_COPY_OVR_ERR_EID                 18


/** \brief <tt> 'Copy File Error: Source Filename' </tt>
**  \event <tt> 'Copy File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Copy File error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Copy File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Copy
**  command packet with an invalid source filename.
*/
#define FM_COPY_SRC_ERR_EID                 19


/** \brief <tt> 'Copy File Error: Target Filename' </tt>
**  \event <tt> 'Copy File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Copy File error: file already exists: name = \%s' </tt>
**  \event <tt> 'Copy File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Copy
**  command packet with an invalid target filename.
*/
#define FM_COPY_TGT_ERR_EID                 20


/** \brief <tt> 'Copy File Error: Child Task' </tt>
**  \event <tt> 'Copy File error: child task is disabled' </tt>
**  \event <tt> 'Copy File error: child task queue is full' </tt>
**  \event <tt> 'Copy File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_COPY_CHILD_ERR_EID               21


/** \brief <tt> 'Copy File Error: OS Error' </tt>
**  \event <tt> 'Copy File error: OS_cp failed: result = \%d, src = \%s, tgt = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source file exists and the target name
**  is unused and appears to be valid. Verify that the target
**  filename is reasonable.  Also, verify that the file system has
**  sufficient free space for this operation. Then refer to the OS
**  specific return value.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the source and
**  target names for the file being copied.
*/
#define FM_COPY_OS_ERR_EID                  22


/** \brief <tt> 'Move File Command Success' </tt>
**  \event <tt> 'Move File command: src = \%s, tgt = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Move command.
*/
#define FM_MOVE_CMD_EID                     23


/** \brief <tt> 'Move File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Move File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Move
**  command packet with an invalid length.
*/
#define FM_MOVE_PKT_ERR_EID                 24


/** \brief <tt> 'Move File Error: Invalid Overwrite' </tt>
**  \event <tt> 'Move File error: invalid overwrite = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Move
**  command packet with an invalid overwrite argument.  Overwrite
**  must be set to TRUE (one) or FALSE (zero).
*/
#define FM_MOVE_OVR_ERR_EID                 25


/** \brief <tt> 'Move File Error: Source Filename' </tt>
**  \event <tt> 'Move File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Move File error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Move File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Move
**  command packet with an invalid source filename.
*/
#define FM_MOVE_SRC_ERR_EID                 26


/** \brief <tt> 'Move File Error: Target Filename' </tt>
**  \event <tt> 'Move File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Move File error: file already exists: name = \%s' </tt>
**  \event <tt> 'Move File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Move
**  command packet with an invalid target filename.
*/
#define FM_MOVE_TGT_ERR_EID                 27


/** \brief <tt> 'Move File Error: Child Task' </tt>
**  \event <tt> 'Move File error: child task is disabled' </tt>
**  \event <tt> 'Move File error: child task queue is full' </tt>
**  \event <tt> 'Move File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_MOVE_CHILD_ERR_EID               28


/** \brief <tt> 'Move File Error: OS Error' </tt>
**  \event <tt> 'Move File error: OS_mv error = 0x\%08X, src = \%s, tgt = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source file exists and the target name
**  is unused and appears to be valid. Verify that the target
**  filename is reasonable.  Also, verify that the file system has
**  sufficient free space for this operation. Then refer to the OS
**  specific return value.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the source and
**  target names for the file being moved.
*/
#define FM_MOVE_OS_ERR_EID                  29


/** \brief <tt> 'Rename File Command Success' </tt>
**  \event <tt> 'Rename File command: src = \%s, tgt = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Rename command.
*/
#define FM_RENAME_CMD_EID                   30


/** \brief <tt> 'Rename File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Rename File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Rename
**  command packet with an invalid length.
*/
#define FM_RENAME_PKT_ERR_EID               31


/** \brief <tt> 'Rename File Error: Source Filename' </tt>
**  \event <tt> 'Rename File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Rename File error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Rename File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Rename
**  command packet with an invalid source filename.
*/
#define FM_RENAME_SRC_ERR_EID               32


/** \brief <tt> 'Rename File Error: Target Filename' </tt>
**  \event <tt> 'Rename File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Rename File error: file already exists: name = \%s' </tt>
**  \event <tt> 'Rename File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Rename
**  command packet with an invalid target filename.
*/
#define FM_RENAME_TGT_ERR_EID               33


/** \brief <tt> 'Rename File Error: Child Task' </tt>
**  \event <tt> 'Rename File error: child task is disabled' </tt>
**  \event <tt> 'Rename File error: child task queue is full' </tt>
**  \event <tt> 'Rename File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_RENAME_CHILD_ERR_EID             34


/** \brief <tt> 'Rename File Error: OS Error' </tt>
**  \event <tt> 'Rename File error: OS_rename error = 0x\%08X, src = \%s, tgt = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source file exists and the target name
**  is unused and appears to be valid. Verify that the target
**  filename is reasonable.  Also, verify that the file system has
**  sufficient free space for this operation. Then refer to the OS
**  specific return value.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the source and
**  target names for the file being renamed.
*/
#define FM_RENAME_OS_ERR_EID                35


/** \brief <tt> 'Delete File Command Success' </tt>
**  \event <tt> 'Delete File command: file = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Delete command.
*/
#define FM_DELETE_CMD_EID                   36


/** \brief <tt> 'Delete File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Delete File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Delete
**  command packet with an invalid length.
*/
#define FM_DELETE_PKT_ERR_EID               37


/** \brief <tt> 'Delete File Error: Filename' </tt>
**  \event <tt> 'Delete File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Delete File error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Delete File error: file is already open: name = \%s' </tt>
**  \event <tt> 'Delete File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Delete
**  command packet with an invalid filename.
*/
#define FM_DELETE_SRC_ERR_EID               38


/** \brief <tt> 'Delete File Error: Child Task' </tt>
**  \event <tt> 'Delete File error: child task is disabled' </tt>
**  \event <tt> 'Delete File error: child task queue is full' </tt>
**  \event <tt> 'Delete File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_DELETE_CHILD_ERR_EID             39


/** \brief <tt> 'Delete File Error: OS Error' </tt>
**  \event <tt> 'Delete File error: OS_remove error = 0x\%08X, file = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the filename exists and is not open. Refer to the
**  OS-specific return value for an indication of what might have
**  caused this error.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the name of the file
**  being deleted.
*/
#define FM_DELETE_OS_ERR_EID                40


/** \brief <tt> 'Delete All Files Command Success' </tt>
**  \event <tt> 'Delete All Files command: deleted \%d of \%d dir entries: dir = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_DeleteAll command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_DELETE_ALL_CMD_EID               41


/** \brief <tt> 'Delete All Files Warning' </tt>
**  \event <tt> 'Delete All Files warning: combined directory and entry name too long: dir = \%s, entry = \%s' </tt>
**  \event <tt> 'Delete All Files warning: entry is invalid: entry = \%s' </tt>
**  \event <tt> 'Delete All Files warning: entry no longer exists: entry = \%s' </tt>
**  \event <tt> 'Delete All Files warning: cannot delete sub-directory: sub = \%s' </tt>
**  \event <tt> 'Delete All Files warning: cannot delete open file: file = \%s' </tt>
**  \event <tt> 'Delete All Files warning: OS_remove failed: result = \%d, file = \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  The /FM_DeleteAll command will succeed if the handler is able
**  to successfully read the directory and attempt to delete the entries
**  in the directory. Command warnings are issued when directory entries,
**  for whatever reason, cannot be deleted.
*/
#define FM_DELETE_ALL_WARNING_EID           42


/** \brief <tt> 'Delete All Files Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Delete All Files error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_DeleteAll
**  command packet with an invalid length.
*/
#define FM_DELETE_ALL_PKT_ERR_EID           43


/** \brief <tt> 'Delete All Files Error: Directory Name' </tt>
**  \event <tt> 'Delete All Files error: directory name is invalid: name = \%s' </tt>
**  \event <tt> 'Delete All Files error: directory does not exist: name = \%s' </tt>
**  \event <tt> 'Delete All Files error: directory name exists as a file: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_DeleteAll
**  command packet with an invalid directory name.
*/
#define FM_DELETE_ALL_SRC_ERR_EID           44


/** \brief <tt> 'Delete All Files Error: Child Task' </tt>
**  \event <tt> 'Delete All Files error: child task is disabled' </tt>
**  \event <tt> 'Delete All Files error: child task queue is full' </tt>
**  \event <tt> 'Delete All Files error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_DELETE_ALL_CHILD_ERR_EID         45


/** \brief <tt> 'Delete All Files Error: OS Error' </tt>
**  \event <tt> 'Delete All Files error: OS_opendir failed: dir = \%s' </tt>
**  \event <tt> 'Delete All Files error: OS_remove failed: result = \%d, file = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the directory exists. Refer to the OS-specific
**  return value for an indication of what might have caused this
**  error.
**
**  Note: the call to OS_opendir returns a pointer, or NULL.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the name of the
**  directory or the directory entry.
*/
#define FM_DELETE_ALL_OS_ERR_EID            46


/** \brief <tt> 'Decompress File Command Success' </tt>
**  \event <tt> 'Decompress File command: src = \%s, tgt = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Decompress command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_DECOM_CMD_EID                    47


/** \brief <tt> 'Decompress File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Decompress File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Decompress
**  command packet with an invalid length.
*/
#define FM_DECOM_PKT_ERR_EID                48


/** \brief <tt> 'Decompress File Error: Source Filename' </tt>
**  \event <tt> 'Decompress File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Decompress File error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Decompress File error: file is already open: name = \%s' </tt>
**  \event <tt> 'Decompress File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Decompress
**  command packet with an invalid source filename.
*/
#define FM_DECOM_SRC_ERR_EID                49


/** \brief <tt> 'Decompress File Error: Target Filename' </tt>
**  \event <tt> 'Decompress File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Decompress File error: file already exists: name = \%s' </tt>
**  \event <tt> 'Decompress File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Decompress
**  command packet with an invalid source filename.
*/
#define FM_DECOM_TGT_ERR_EID                50


/** \brief <tt> 'Decompress File Error: Child Task' </tt>
**  \event <tt> 'Decompress File error: child task is disabled' </tt>
**  \event <tt> 'Decompress File error: child task queue is full' </tt>
**  \event <tt> 'Decompress File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_DECOM_CHILD_ERR_EID              51


/** \brief <tt> 'Decompress File Error: CFE Error' </tt>
**  \event <tt> 'Decompress File error: CFE_FS_Decompress failed: result = \%d, src = \%s, tgt = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an API function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source file exists. Refer to the function
**  specific return value for an indication of what might have caused
**  this particular error.
**
**  The numeric data in the event is the return value from the API
**  function call.  The string data identifies the names of the
**  source and target files.
*/
#define FM_DECOM_CFE_ERR_EID                52


/** \brief <tt> 'Concat Files Command Success' </tt>
**  \event <tt> 'Concat Files command: src1 = \%s, src2 = \%s, tgt = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_Concat command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_CONCAT_CMD_EID                   53


/** \brief <tt> 'Concat Files Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Concat Files error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Concat
**  command packet with an invalid length.
*/
#define FM_CONCAT_PKT_ERR_EID               54


/** \brief <tt> 'Concat Files Error: Source 1 Filename' </tt>
**  \event <tt> 'Concat Files error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Concat Files error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Concat Files error: file is already open: name = \%s' </tt>
**  \event <tt> 'Concat Files error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Concat
**  command packet with an invalid source 1 filename.
*/
#define FM_CONCAT_SRC1_ERR_EID              55


/** \brief <tt> 'Concat Files Error: Source 2 Filename' </tt>
**  \event <tt> 'Concat Files error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Concat Files error: file does not exist: name = \%s' </tt>
**  \event <tt> 'Concat Files error: file is already open: name = \%s' </tt>
**  \event <tt> 'Concat Files error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Concat
**  command packet with an invalid source 2 filename.
*/
#define FM_CONCAT_SRC2_ERR_EID              56


/** \brief <tt> 'Concat Files Error: Target Filename' </tt>
**  \event <tt> 'Concat Files error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Concat Files error: file already exists: name = \%s' </tt>
**  \event <tt> 'Concat Files error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_Concat
**  command packet with an invalid target filename.
*/
#define FM_CONCAT_TGT_ERR_EID               57


/** \brief <tt> 'Concat File Error: Child Task' </tt>
**  \event <tt> 'Concat File error: child task is disabled' </tt>
**  \event <tt> 'Concat File error: child task queue is full' </tt>
**  \event <tt> 'Concat File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_CONCAT_CHILD_ERR_EID             58


/** \brief <tt> 'Concat File Error: OS Error' </tt>
**  \event <tt> 'Concat File error: OS_cp failed: result = \%d, src = \%s, tgt = \%s' </tt>
**  \event <tt> 'Concat File error: OS_open failed: result = \%d, src2 = \%s' </tt>
**  \event <tt> 'Concat File error: OS_open failed: result = \%d, tgt = \%s' </tt>
**  \event <tt> 'Concat File error: OS_read failed: result = \%d, file = \%s' </tt>
**  \event <tt> 'Concat File error: OS_write failed: result = \%d, expected = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an API function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source files exist. Refer to the function
**  specific return value for an indication of what might have caused
**  this particular error.
**
**  The numeric data in the event is the return value from the API
**  function call.  The string data identifies the name of the file(s)
**  being operated on by the API function.
*/
#define FM_CONCAT_OS_ERR_EID                59


/** \brief <tt> 'Get File Info Command Success' </tt>
**  \event <tt> 'Get File Info command: name = \%s' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command generates a telemetry 
**  packet that also signals the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_GetFileInfo command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_GET_FILE_INFO_CMD_EID            60


/** \brief <tt> 'Get File Info Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Get File Info error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetFileInfo
**  command packet with an invalid length.
*/
#define FM_GET_FILE_INFO_PKT_ERR_EID        61


/** \brief <tt> 'Get File Info Error: Source Filename' </tt>
**  \event <tt> 'Get File Info error: invalid name: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetFileInfo
**  command packet with an invalid filename.
*/
#define FM_GET_FILE_INFO_SRC_ERR_EID        62


/** \brief <tt> 'Get File Info Error: Child Task' </tt>
**  \event <tt> 'Get File Info error: child task is disabled' </tt>
**  \event <tt> 'Get File Info error: child task queue is full' </tt>
**  \event <tt> 'Get File Info error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_GET_FILE_INFO_CHILD_ERR_EID      63


/** \brief <tt> 'Get File Info Warning: unable to compute CRC' </tt>
**  \event <tt> 'Get File Info warning: unable to compute CRC: invalid file state = \%d, file = \%s' </tt>
**  \event <tt> 'Get File Info warning: unable to compute CRC: invalid CRC type = \%d, file = \%s' </tt>
**  \event <tt> 'Get File Info warning: unable to compute CRC: OS_open result = \%d, file = \%s' </tt>
**  \event <tt> 'Get File Info warning: unable to compute CRC: OS_read result = \%d, file = \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is generated due to an API function error that
**  occurred after preliminary command argument verification tests
**  indicated that the source files exist. Refer to the function
**  specific return value for an indication of what might have caused
**  this particular error.
**
**  The numeric data in the event is the return value from the API
**  function call.  The string data identifies the name of the file
**  being operated on by the API function.
*/
#define FM_GET_FILE_INFO_WARNING_EID        64


/** \brief <tt> 'Get Open Files Command Success' </tt>
**  \event <tt> 'Get Open Files command' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command generates a telemetry 
**  packet that also signals the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_GetOpenFiles command.
*/
#define FM_GET_OPEN_FILES_CMD_EID           65


/** \brief <tt> 'Get Open Files Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Get Open Files error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetOpenFiles
**  command packet with an invalid length.
*/
#define FM_GET_OPEN_FILES_PKT_ERR_EID       66


/** \brief <tt> 'Create Directory Command Success' </tt>
**  \event <tt> 'Create Directory command: dir = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_CreateDir command.
*/
#define FM_CREATE_DIR_CMD_EID               67


/** \brief <tt> 'Create Directory Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Create Directory error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_CreateDir
**  command packet with an invalid length.
*/
#define FM_CREATE_DIR_PKT_ERR_EID           68


/** \brief <tt> 'Create Directory Error: Directory Name' </tt>
**  \event <tt> 'Create Directory error: directory name is invalid: name = \%s' </tt>
**  \event <tt> 'Create Directory error: directory name exists as a file: name = \%s' </tt>
**  \event <tt> 'Create Directory error: directory already exists: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_CreateDir
**  command packet with an invalid directory name.
*/
#define FM_CREATE_DIR_SRC_ERR_EID           69


/** \brief <tt> 'Create Directory File Error: Child Task' </tt>
**  \event <tt> 'Create Directory File error: child task is disabled' </tt>
**  \event <tt> 'Create Directory File error: child task queue is full' </tt>
**  \event <tt> 'Create Directory File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_CREATE_DIR_CHILD_ERR_EID         70


/** \brief <tt> 'Create Directory Error: OS Error' </tt>
**  \event <tt> 'Create Directory error: OS_mkdir failed: result = \%d, dir = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the directory name is unused and appears to be
**  valid. Refer to the OS specific return value.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the directory name.
*/
#define FM_CREATE_DIR_OS_ERR_EID            71


/** \brief <tt> 'Delete Directory Command Success' </tt>
**  \event <tt> 'Delete Directory command: dir = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_DeleteDir command.
*/
#define FM_DELETE_DIR_CMD_EID               72


/** \brief <tt> 'Delete Directory Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Delete Directory error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_DeleteDir
**  command packet with an invalid length.
*/
#define FM_DELETE_DIR_PKT_ERR_EID           73


/** \brief <tt> 'Delete Directory Error: Directory Name' </tt>
**  \event <tt> 'Delete Directory error: directory name is invalid: name = \%s' </tt>
**  \event <tt> 'Delete Directory error: directory does not exist: name = \%s' </tt>
**  \event <tt> 'Delete Directory error: directory name exists as a file: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_DeleteDir
**  command packet with an invalid directory name.
*/
#define FM_DELETE_DIR_SRC_ERR_EID           74


/** \brief <tt> 'Delete Directory File Error: Child Task' </tt>
**  \event <tt> 'Delete Directory File error: child task is disabled' </tt>
**  \event <tt> 'Delete Directory File error: child task queue is full' </tt>
**  \event <tt> 'Delete Directory File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_DELETE_DIR_CHILD_ERR_EID         75


/** \brief <tt> 'Delete Directory Error: Source Filename' </tt>
**  \event <tt> 'Delete Directory error: directory is not empty: dir = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_DeleteDir
**  command packet that references a directory that is not empty.
*/
#define FM_DELETE_DIR_EMPTY_ERR_EID         76


/** \brief <tt> 'Delete Directory Error: OS Error' </tt>
**  \event <tt> 'Delete Directory error: OS_opendir failed: dir = \%s' </tt>
**  \event <tt> 'Delete Directory error: OS_rmdir failed: result = \%d, dir = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the directory exists and appears to be valid.
**  Refer to the OS specific return values.
**
**  Note: the call to OS_opendir returns a pointer, or NULL.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the name of the
**  directory or the directory entry.
*/
#define FM_DELETE_DIR_OS_ERR_EID            77


/** \brief <tt> 'Directory List to File command' </tt>
**  \event <tt> 'Directory List to File command: wrote \%d of \%d names: dir = \%s, filename = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_GetDirFile command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_GET_DIR_FILE_CMD_EID             78


/** \brief <tt> 'Directory List to File Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Directory List to File error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetDirFile
**  command packet with an invalid length.
*/
#define FM_GET_DIR_FILE_PKT_ERR_EID         79


/** \brief <tt> 'Directory List to File Error: Directory Name' </tt>
**  \event <tt> 'Directory List to File error: directory name is invalid: name = \%s' </tt>
**  \event <tt> 'Directory List to File error: directory does not exist: name = \%s' </tt>
**  \event <tt> 'Directory List to File error: directory name exists as a file: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetDirFile
**  command packet with an invalid directory name.
*/
#define FM_GET_DIR_FILE_SRC_ERR_EID         80


/** \brief <tt> 'Directory List to File Error: Output Filename' </tt>
**  \event <tt> 'Directory List to File error: filename is invalid: name = \%s' </tt>
**  \event <tt> 'Directory List to File error: file already exists: name = \%s' </tt>
**  \event <tt> 'Directory List to File error: filename is a directory: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetDirFile
**  command packet with an invalid output filename.
*/
#define FM_GET_DIR_FILE_TGT_ERR_EID         81


/** \brief <tt> 'Directory List to File Warning: Pathname' </tt>
**  \event <tt> 'Directory List to File warning: combined directory and entry name too long: dir = \%s, entry = \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is generated when the combined length of the
**  directory name plus the directory entry name exceeds the maximum
**  qualified filename length.  It is unclear how this condition
**  might arise, but since we are copying both strings into a fixed
**  length buffer, we must first verify the length.
**
**  The /FM_GetDirFile command handler will not write information
**  regarding this directory entry to the output file.
*/
#define FM_GET_DIR_FILE_WARNING_EID         82


/** \brief <tt> 'Directory List to File Error: Child Task' </tt>
**  \event <tt> 'Directory List to File error: child task is disabled' </tt>
**  \event <tt> 'Directory List to File error: child task queue is full' </tt>
**  \event <tt> 'Directory List to File error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_GET_DIR_FILE_CHILD_ERR_EID       83


/** \brief <tt> 'Directory List to File Error: OS Error' </tt>
**  \event <tt> 'Directory List to File error: OS_opendir failed: dir = \%s' </tt>
**  \event <tt> 'Directory List to File error: OS_write blank stats failed: result = \%d, expected = \%d' </tt>
**  \event <tt> 'Directory List to File error: CFE_FS_WriteHeader failed: result = \%d, expected = \%d' </tt>
**  \event <tt> 'Directory List to File error: OS_creat failed: result = \%d, file = \%s' </tt>
**  \event <tt> 'Directory List to File error: OS_write entry failed: result = \%d, expected = \%d' </tt>
**  \event <tt> 'Directory List to File error: OS_write update stats failed: result = \%d, expected = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated due to an OS function error that
**  occurred after preliminary command argument verification tests
**  indicated that the directory exists and the output filename
**  is unused and appears to be valid. Verify that the output
**  filename is reasonable.  Also, verify that the file system has
**  sufficient free space for this operation. Then refer to the OS
**  specific return values.
**
**  Note: the call to OS_opendir returns a pointer, or NULL.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the name of the
**  directory or the directory entry.
*/
#define FM_GET_DIR_FILE_OS_ERR_EID          84


/** \brief <tt> 'Directory List to Packet command' </tt>
**  \event <tt> 'Directory List to Packet command: offset = \%d, dir = \%s' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command generates a telemetry 
**  packet that also signals the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_GetDirPkt command.
**
**  Note that the execution of this command generally occurs within the
**  context of the FM low priority child task.  Thus this event may not
**  occur until some time after the command was invoked.  However, this
**  event message does signal the actual completion of the command.
*/
#define FM_GET_DIR_PKT_CMD_EID              85


/** \brief <tt> 'Directory List to Packet Warning' </tt>
**  \event <tt> 'Directory List to Packet warning: dir + entry is too long: dir = \%s, entry = \%s' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is generated when the combined length of the
**  directory name plus the directory entry name exceeds the maximum
**  qualified filename length.  It is unclear how this condition
**  might arise, but since we are copying both strings into a fixed
**  length buffer, we must first verify the length.
*/
#define FM_GET_DIR_PKT_WARNING_EID          86


/** \brief <tt> 'Directory List to Packet Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Directory List to Packet error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetDirPkt
**  command packet with an invalid length.
*/
#define FM_GET_DIR_PKT_PKT_ERR_EID          87


/** \brief <tt> 'Directory List to Packet Error: Directory Name' </tt>
**  \event <tt> 'Directory List to Packet error: directory name is invalid: name = \%s' </tt>
**  \event <tt> 'Directory List to Packet error: directory does not exist: name = \%s' </tt>
**  \event <tt> 'Directory List to Packet error: directory name exists as a file: name = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetDirPkt
**  command packet with an invalid directory name.
*/
#define FM_GET_DIR_PKT_SRC_ERR_EID          88


/** \brief <tt> 'Directory List to Packet Error: Child Task' </tt>
**  \event <tt> 'Directory List to Packet error: child task is disabled' </tt>
**  \event <tt> 'Directory List to Packet error: child task queue is full' </tt>
**  \event <tt> 'Directory List to Packet error: child task interface is broken: count = \%d, index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when the FM child task command queue
**  interface cannot be used.
**
**  If the child task command queue is full, the problem may be temporary,
**  caused by sending too many FM commands too quickly.  If the command
**  queue does not empty itself within a reasonable amount of time then
**  the child task may be hung. It may be possible to use CFE commands to
**  terminate the child task, which should then cause FM to process all
**  commands in the main task.
**
**  If the child task queue is broken then either the handshake interface
**  logic is flawed, or there has been some sort of data corruption that
**  affected the interface control variables.  In either case, it may be
**  necessary to restart the FM application to resync the interface.
*/
#define FM_GET_DIR_PKT_CHILD_ERR_EID        89


/** \brief <tt> 'Directory List to Packet Error: OS Error' </tt>
**  \event <tt> 'Directory List to Packet error: OS_opendir failed: dir = \%s' </tt>
**  \event <tt> 'Directory List to Packet error: ' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  Note: the call to OS_opendir returns a pointer, or NULL.
**
**  The numeric data in the event is the return value from the OS
**  function call.  The string data identifies the name of the
**  directory or the directory entry.
*/
#define FM_GET_DIR_PKT_OS_ERR_EID           90


/** \brief <tt> 'Get Free Space Command' </tt>
**  \event <tt> 'Get Free Space command' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command generates a telemetry 
**  packet that also signals the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_GetFreeSpace command.
*/
#define FM_GET_FREE_SPACE_CMD_EID           91


/** \brief <tt> 'Get Free Space Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Get Free Space error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetFreeSpace
**  command packet with an invalid length.
*/
#define FM_GET_FREE_SPACE_PKT_ERR_EID       92


/** \brief <tt> 'Get Free Space Error: Table Not Loaded' </tt>
**  \event <tt> 'Get Free Space error: file system free space table is not loaded' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_GetFreeSpace
**  command packet when the FM file system free space table has not yet
**  been loaded.
*/
#define FM_GET_FREE_SPACE_TBL_ERR_EID       93


/** \brief <tt> 'Set Table State Command' </tt>
**  \event <tt> 'Set Table State command: index = \%d, state = \%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /FM_SetTableState command.
*/
#define FM_SET_TABLE_STATE_CMD_EID          94


/** \brief <tt> 'Set Table State Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Set Table State error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_SetTableState
**  command packet with an invalid length.
*/
#define FM_SET_TABLE_STATE_PKT_ERR_EID      95


/** \brief <tt> 'Set Table State Error: Table Not Loaded' </tt>
**  \event <tt> 'Set Table State error: file system free space table is not loaded' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_SetTableState
**  command packet when the FM file system free space table has not yet
**  been loaded.
*/
#define FM_SET_TABLE_STATE_TBL_ERR_EID      96


/** \brief <tt> 'Set Table State Error: Invalid Command Argument' </tt>
**  \event <tt> 'Set Table State error: invalid command argument: index = \%d' </tt>
**  \event <tt> 'Set Table State error: invalid command argument: state = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a FM_SetTableState
**  command packet with an invalid table index or entry state argument.
*/
#define FM_SET_TABLE_STATE_ARG_ERR_EID      97


/** \brief <tt> 'Set Table State Error: Unused Table Entry' </tt>
**  \event <tt> 'Set Table State error: cannot modify unused table entry: index = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /FM_SetTableState
**  command packet that references an unused free space table entry.
*/
#define FM_SET_TABLE_STATE_UNUSED_ERR_EID   98


/** \brief <tt> 'Free Space Table Verify Error' </tt>
**  \event <tt> 'Free Space Table verify error: index = \%d, empty name string' </tt>
**  \event <tt> 'Free Space Table verify error: index = \%d, name too long' </tt>
**  \event <tt> 'Free Space Table verify error: index = \%d, invalid name = \%s' </tt>
**  \event <tt> 'Free Space Table verify error: index = \%d, invalid state = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when a file system free space table fails the table
**  verification process.  Each file system table entry has only 2 fields: table entry
**  state and file system name.  The table entry state field must be either enabled
**  or disabled.  The file system name string must have a non-zero length, include a
**  string terminator and not contain characters considered invalid for filenames.
**
**  If the file system free space table loaded at startup fails verification, the FM
**  application will not terminate.  However, the FM application will not process
**  commands that request the file system free space telemetry packet if a file
**  system free space table has not been successfully loaded.  Thereafter, if an
**  attempt to load a new table fails verification, the FM application will continue
**  to use the previous table.
*/
#define FM_TABLE_VERIFY_ERR_EID             99


/** \brief <tt> 'Child Task Initialization Complete' </tt>
**  \event <tt> 'Child Task initialization complete' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of the initialization
**  process for the FM child task.
*/
#define FM_CHILD_INIT_EID                   100


/** \brief <tt> 'Child Task Initialization Error' </tt>
**  \event <tt> 'Child Task initialization error: create task failed: result = \%d' </tt>
**  \event <tt> 'Child Task initialization error: register child failed: result = \%d' </tt>
**  \event <tt> 'Child Task initialization error: create semaphore failed: result = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message indicates an unsuccessful attempt to create and initialize the
**  low priority FM child task.  Commands which would have otherwise been handed off
**  to the child task for execution, will now be processed by the main FM application.
**  Refer to the return code in the event text for the exact cause of the error.
*/
#define FM_CHILD_INIT_ERR_EID               101


/** \brief <tt> 'Child Task Termination Error' </tt>
**  \event <tt> 'Child Task termination error: empty queue' </tt>
**  \event <tt> 'Child Task termination error: invalid queue index: index = \%d' </tt>
**  \event <tt> 'Child Task termination error: semaphore take failed: result = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message indicates that the FM child task has suffered a fatal error
**  and has terminated.  The error occurred when referencing the interface handshake
**  variables or while pending on the handshake semaphore.  Refer to the event text
**  for the exact cause of the error.
*/
#define FM_CHILD_TERM_ERR_EID               102


/** \brief <tt> 'Child Task Execution Error' </tt>
**  \event <tt> 'Child Task execution error: invalid command code: cc = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message indicates that the FM child task is unable to process the current
**  handshake request.  Either the handshake queue index or the handshake command code
**  is invalid.  This error suggests that either the handshake interface logic is flawed,
**  or there has been some sort of data corruption that affected the interface data.
**  It may be necessary to restart the FM application to resync the handshake interface.
*/
#define FM_CHILD_EXE_ERR_EID                103


/**
**  \brief <tt> 'Free Space Table Validation Results' </tt>
**
**  \event <tt> 'Free Space Table verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event describes the results of the Free Space Table validation
**  function.  The cFE Table Services Manager will call this function autonomously
**  when the default table is loaded at startup and also whenever a table validate
**  command (that targets this table) is processed.
**
**  The event text will indicate
**  the number of table entries that were verified without error (good), the number
**  of table entries that had one or more errors (bad) and the number of unused
**  table entries (unused).  Thus, the sum of good + bad
**  + unused results will equal the total number of table entries.
*/
#define FM_TABLE_VERIFY_EID                 104


#endif /* _fm_events_h_ */

/************************/
/*  End of File Comment */
/************************/
