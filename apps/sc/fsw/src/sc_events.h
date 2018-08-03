 /*************************************************************************
 ** File:
 **   $Id: sc_events.h 1.8 2015/03/02 12:58:44EST sstrege Exp  $
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
 ** Purpose: 
 **   his file contains the definitions of all of the events send by
 **   the Stored Command Processor
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_events.h  $ 
 **   Revision 1.8 2015/03/02 12:58:44EST sstrege  
 **   Added copyright information 
 **   Revision 1.7 2014/06/06 11:37:54EDT sjudy  
 **   Changed event msgs to have 'RTS' or 'ATS' instead of "Real Time Sequence", etc. 
 **   Revision 1.6 2011/09/26 09:41:30GMT-08:00 lwalling  
 **   Remove CDS specific events, create common events to replace CDS/no CDS event pairs 
 **   Revision 1.5 2011/09/23 14:26:05EDT lwalling  
 **   Made group commands conditional on configuration definition 
 **   Revision 1.4 2011/03/14 10:51:00EDT lwalling  
 **   Add events for commands to start/stop/enable/disable a range of RTS. 
 **   Revision 1.3 2011/02/01 11:36:33EST lwalling  
 **   Remove ATS table verify command checksum error event - SC_VERIFY_ATS_SUM_ERR_EID 
 **   Revision 1.2 2010/12/10 15:20:00EST rperera  
 **    
 **   Revision 1.1 2010/12/10 15:18:22EST rperera  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/src/project.pj 
 *************************************************************************/
#ifndef _sc_events_
#define _sc_events_

/** \brief <tt> 'App terminating, Result = 0x\%08X' </tt>
**  \event <tt> 'App terminating, Result = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when the App exits
**  due to a fatal error condition
**
**  The \c Result field contains the return status from
**  the function that caused the app to terminate
*/
#define SC_APP_EXIT_ERR_EID                                 1


/** \brief <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a command is recieved, but it is not of the expected
**  length
**  
**  The \c ID field contains the message Id of the message
**  The \c CC field contains the command code of the message
**  The \c Len field contains the length of the message received
**  The \c Expected field contains the expected length of the command 
*/
#define SC_LEN_ERR_EID                                      2


/** \brief <tt> 'Software Bus Create Pipe returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus Create Pipe returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_CreatePipe returns an
**  error
**
**  The \c returned field contains the result of the #CFE_SB_CreatePipe
**  call
*/
#define SC_INIT_SB_CREATE_ERR_EID                           3


/** \brief <tt> 'Software Bus subscribe to housekeeping returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to housekeeping returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_Subscribe to the Housekeeping 
**  Request packet fails
**
**  The \c returned field contains the result of the #CFE_SB_Subscribe call
*/
#define SC_INIT_SB_SUBSCRIBE_HK_ERR_EID                     4


/** \brief <tt> 'Software Bus subscribe to 1 Hz cycle returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to 1 Hz cycle returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_Subscribe to the 1 Hz 
**  Request packet fails  
**
**  The \c returned field contains the result of the #CFE_SB_Subscribe call
*/
#define SC_INIT_SB_SUBSCRIBE_1HZ_ERR_EID                    5


/** \brief <tt> 'Software Bus subscribe to command returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to command returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_Subscribe to the SC Command 
**  Request packet fails  
**
**  The \c returned field contains the result of the #CFE_SB_Subscribe call
*/
#define SC_INIT_SB_SUBSCRIBE_CMD_ERR_EID                    6


/** \brief <tt> 'SC Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'SC Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when the App has
**  completed initialization.
**
**  The \c Version fields contain the #SC_MAJOR_VERSION,
**  #SC_MINOR_VERSION, #SC_REVISION, and #SC_MISSION_REV
**  version identifiers. 
*/
#define SC_INIT_INF_EID                                    9    


/** \brief <tt> 'RTS Table Registration Failed for RTS \%d, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS Table Registration Failed for RTS \%d, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a call to #CFE_TBL_Register for an RTS table failed 
**
**  The \c RTS field contains the RTS ID of the table that failed and the \c returned field
**  contains the error code returned from #CFE_TBL_Register
*/
#define SC_REGISTER_RTS_TBL_ERR_EID                         10


/** \brief <tt> 'ATS Table Registration Failed for ATS \%d, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS Table Registration Failed for ATS \%d, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a call to #CFE_TBL_Register for an ATS table failed 
**
**  The \c ATS field contains the ATS ID of the table that failed and the \c returned field
**  contains the error code returned from #CFE_TBL_Register
*/
#define SC_REGISTER_ATS_TBL_ERR_EID                         11


/** \brief <tt> 'RTS info table register failed, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS info table register failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to register the RTS Info Table
**  dump only table fails
**
**  The \c returned field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_RTS_INFO_TABLE_ERR_EID                  16


/** \brief <tt> 'RTS control block table register failed, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS control block table register failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to register the RTS control block
**  dump only table fails
**
**  The \c returned field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_RTS_CTRL_BLK_TABLE_ERR_EID              17


/** \brief <tt> 'ATS info table register failed, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS info table register failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to register the ATS Info Table
**  dump only table fails
**
**  The \c returned field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_ATS_INFO_TABLE_ERR_EID                  18


/** \brief <tt> 'ATS control block table register failed, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS control block table register failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to register the ATS control block
**  dump only table fails
**
**  The \c returned field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_ATS_CTRL_BLK_TABLE_ERR_EID              19


/** \brief <tt> 'ATS command status table register failed for ATS \%d, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS command status table register failed for ATS \%d, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when one of the ATS command status tables fails table
**  registration
**
**  The \c ATS field contains the ATS Id for the table that failed, and the \c returned
**  field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_ATS_CMD_STATUS_TABLE_ERR_EID            20


/** \brief <tt> 'RTS table file load count = \%d' </tt>
**  \event <tt> 'RTS table file load count = \%d' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message indicates the number of RTS table files successfully
**  loaded at startup.
*/
#define SC_RTS_LOAD_COUNT_INFO_EID                          21


/** \brief <tt> 'ATS \%c Execution Started' </tt>
**  \event <tt> 'ATS \%c Execution Started' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an ATS is started successfully
**  
**  
**  The \c Sequence field contains which ATS was started
*/
#define SC_STARTATS_CMD_INF_EID                             23


/** \brief <tt> 'Start ATS Rejected: ATS \%c Not Loaded' </tt>
**  \event <tt> 'Start ATS Rejected: ATS \%c Not Loaded' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_START_ATS_CC cmd failed because
**  the specified ATS was not loaded.
**  
**  The \c ATS field contains the ATS that was tried to be started
*/
#define SC_STARTATS_CMD_NOT_LDED_ERR_EID                    24


/** \brief <tt> 'Start ATS Rejected: ATP is not Idle' </tt>
**  \event <tt> 'Start ATS Rejected: ATP is not Idle' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_START_ATS_CC command was issued
**  but there is already an ATS running 
*/
#define SC_STARTATS_CMD_NOT_IDLE_ERR_EID                    25


/** \brief <tt> 'Start ATS \%d Rejected: Invalid ATS ID' </tt>
**  \event <tt> 'Start ATS \%d Rejected: Invalid ATS ID' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when the ATS Id specified in the 
**  #SC_START_ATS_CC command was invalid
**  
**  The \c ATS field contains the invalid ATS Id
*/
#define SC_STARTATS_CMD_INVLD_ID_ERR_EID                    26


/** \brief <tt> 'ATS \%c stopped' </tt>
**  \event <tt> 'ATS \%c stopped' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SC_STOP_ATS_CC command successfully
**  stopped an ATS
**  
**  The \c ATS field contains the ATS that was stopped
*/
#define SC_STOPATS_CMD_INF_EID                              27


/** \brief <tt> 'There is no ATS running to stop' </tt>
**  \event <tt> 'There is no ATS running to stop' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SC_STOP_ATS_CC command was issued
**  but there was no ATS running
*/
#define SC_STOPATS_NO_ATS_INF_EID                           28


/** \brief <tt> 'All ATS commands were skipped, ATS stopped' </tt>
**  \event <tt> 'All ATS commands were skipped, ATS stopped' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS is begun, and all times for
**  the commands in the ATS exist in the past.   
*/
#define SC_ATS_SKP_ALL_ERR_EID                              29


/** \brief <tt> 'ATS started, skipped \%d commands' </tt>
**  \event <tt> 'ATS started, skipped \%d commands' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when an ATS is started, and some of the
**  times for commands in the ATS exist in the past.
**  
**  The \c skipped field contains the number of commands that were skipped
**  in the ATS
*/
#define SC_ATS_ERR_SKP_DBG_EID                              30


/** \brief <tt> 'Switch ATS Pending' </tt>
**  \event <tt> 'Switch ATS Pending' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SC_SWITCH_ATS_CC command is issued and
**  the switch is scheduled 
*/
#define SC_SWITCH_ATS_CMD_INF_EID                           31


/** \brief <tt> 'Switch ATS Failure: Destination ATS Not Loaded' </tt>
**  \event <tt> 'Switch ATS Failure: Destination ATS Not Loaded' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_SWITCH_ATS_CC command is issued, but the
**  ATS to switch to is not loaded  
*/
#define SC_SWITCH_ATS_CMD_NOT_LDED_ERR_EID                  32


/** \brief <tt> 'Switch ATS Rejected: ATP is idle' </tt>
**  \event <tt> 'Switch ATS Rejected: ATP is idle' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_SWITCH_ATS_CC command is issued, but there
**  is not an ATS running to switch from
*/
#define SC_SWITCH_ATS_CMD_IDLE_ERR_EID                      33


/** \brief <tt> 'ATS Switched from \%c to \%c' </tt>
**  \event <tt> 'ATS Switched from \%c to \%c' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an ATS switch is scheduled and has been switched 
**  
**  The \c from field contains old ATS and the \c to field contains the new running ATS
*/
#define SC_ATS_SERVICE_SWTCH_INF_EID                        34


/** \brief <tt> 'Switch ATS Failure: Destination ATS is empty' </tt>
**  \event <tt> 'Switch ATS Failure: Destination ATS is empty' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS switch is scheduled, but there are no commands
**  (ie not loaded) in the destination ATS  
*/
#define SC_SERVICE_SWITCH_ATS_CMD_LDED_ERR_EID              35


/** \brief <tt> 'Switch ATS Rejected: ATP is idle' </tt>
**  \event <tt> 'Switch ATS Rejected: ATP is idle' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS switch is scheduled, but there is no ATS running
**  This error will only occur is something gets corrupted 
*/
#define SC_ATS_SERVICE_SWITCH_IDLE_ERR_EID                  36


/** \brief <tt> 'ATS Switched from \%c to \%c' </tt>
**  \event <tt> 'ATS Switched from \%c to \%c' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an ATS is scheduled in-line and the switch is successful
** 
**  The \c from field contains old ATS and the \c to field contains the new running ATS
*/
#define SC_ATS_INLINE_SWTCH_INF_EID                         37


/** \brief <tt> 'Switch ATS Failure: Destination ATS Not Loaded' </tt>
**  \event <tt> 'Switch ATS Failure: Destination ATS Not Loaded' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS switch is scheduled, but there are no commands
**  (ie not loaded) in the destination ATS  
*/
#define SC_ATS_INLINE_SWTCH_NOT_LDED_ERR_EID                38


/** \brief <tt> 'Jump Cmd: All ATS commands were skipped, ATS stopped' </tt>
**  \event <tt> 'Jump Cmd: All ATS commands were skipped, ATS stopped' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_JUMP_ATS_CC command was issued, and the time to 
**  jump to was passed all of the commands in the ATS
*/
#define SC_JUMPATS_CMD_STOPPED_ERR_EID                      39


/** \brief <tt> 'Next ATS command time in the ATP was set to \%s' </tt>
**  \event <tt> 'Next ATS command time in the ATP was set to \%s' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SC_JUMP_ATS_CC command was executed successfully
**  
**  
**  The \c time field contains the time of the next ATS command after the jump
*/
#define SC_JUMP_ATS_INF_EID                                 40


/** \brief <tt> 'ATS Jump Failed: No active ATS' </tt>
**  \event <tt> 'ATS Jump Failed: No active ATS' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_JUMP_ATS_CC command was received, but there
**  is no ATS currently running  
*/
#define SC_JUMPATS_CMD_NOT_ACT_ERR_EID                      41


/** \brief <tt> 'Continue ATS On Failure command  failed, invalid state: \%d"' </tt>
**  \event <tt> 'Continue ATS On Failure command  failed, invalid state: \%d"' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_CONTINUE_ATS_ON_FAILURE_CC command was received, but the state
**  in the command was invalid
**  
**  The \c State field contains the state given in the command
*/
#define SC_CONT_CMD_ERR_EID                                 42


/** \brief <tt> 'Continue-ATS-On-Failure command, State: \%d' </tt>
**  \event <tt> 'Continue-ATS-On-Failure command, State: \%d' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when the #SC_CONTINUE_ATS_ON_FAILURE_CC command was recieved and 
**  the state was changed successfully
**  
**  The \c State field contains the new state for the flag
*/
#define SC_CONT_CMD_DEB_EID                                 43


/** \brief <tt> 'ATS Command Failed Checksum: Command #\%d Skipped' </tt>
**  \event <tt> 'ATS Command Failed Checksum: Command #\%d Skipped' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a command from an ATS is about to be sent out 
**  but it fails checksum validation
**
**  The \c Command field contains the number of the command that was skipped
*/
#define SC_ATS_CHKSUM_ERR_EID                               44


/** \brief <tt> 'ATS \%c Aborted' </tt>
**  \event <tt> 'ATS \%c Aborted' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a ATS command that was about to be sent out
**  failed checksum validation, and the Continue-ATS-on_checksum-Failure flag was
**  set to 'FALSE'
**  
**  The \c Sequence field contains the ATS that was stopped
*/
#define SC_ATS_ABT_ERR_EID                                  45


/** \brief <tt> 'ATS Command Distribution Failed, Cmd Number: \%d, SB returned: 0x\%08X' </tt>
**  \event <tt> 'ATS Command Distribution Failed, Cmd Number: \%d, SB returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command is about to be sent out,
**  and the #CFE_SB_SendMsg call failed to send it
**  
**  The \c Cmd \c number field contains the command number that failed
**  The \c Returned field contains the return code from #CFE_SB_SendMsg
*/
#define SC_ATS_DIST_ERR_EID                                 46


/** \brief <tt> 'ATS Command Number Mismatch: Command Skipped, expected: \%d received: \%d' </tt>
**  \event <tt> 'ATS Command Number Mismatch: Command Skipped, expected: \%d received: \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command is about to be sent out,
**  but it's command number is not what was expected  
**  
**  The \c expected field is the command number that was in the ATS control block
**  The \c received field is the command number that was in the ATS table
*/
#define SC_ATS_MSMTCH_ERR_EID                               47


/** \brief <tt> 'Invalid ATS Command Status: Command Skipped' </tt>
**  \event <tt> 'Invalid ATS Command Status: Command Skipped' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command is about to be send out,
**  but the command isn't marked as '#SC_LOADED'
**  
**  The \c Status field contains the state of the ATS command
*/
#define SC_ATS_SKP_ERR_EID                                  48


/** \brief <tt> 'RTS \%03d Command Distribution Failed: RTS Stopped. SB returned 0x%08X' </tt>
**  \event <tt> 'RTS \%03d Command Distribution Failed: RTS Stopped. SB returned 0x%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an RTS command was about to be sent out,
**  and #CFE_SB_SendMsg couldn't send the message
**  
**  The \c RTS field contains the RTS Id that was stopped
**  The \c returned field is the return code from #CFE_SB_SendMsg
*/
#define SC_RTS_DIST_ERR_EID                                 49


/** \brief <tt> 'RTS \%03d Command Failed Checksum: RTS Stopped' </tt>
**  \event <tt> 'RTS \%03d Command Failed Checksum: RTS Stopped' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an RTS comand was about to be sent out,
**  but the command failed checksum validation
**  
**  The \c RTS field contains the RTS Id that was stopped
*/
#define SC_RTS_CHKSUM_ERR_EID                               50


/** \brief <tt> 'Reset counters command' </tt>
**  \event <tt> 'Reset counters command' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when the #SC_RESET_COUNTERS_CC command was received
*/
#define SC_RESET_DEB_EID                                    51


/** \brief <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a #SC_NOOP_CC command has been received
**
**  The \c Version fields contain the #SC_MAJOR_VERSION,
**  #SC_MINOR_VERSION, #SC_REVISION, and #SC_MISSION_REV
**  version identifiers. 
*/
#define SC_NOOP_INF_EID                                     52 


/** \brief <tt> 'RTS cmd loaded with invalid MID at \%d' </tt>
**  \event <tt> 'RTS cmd loaded with invalid MID at \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when and RTS table is loaded, but there is an invalid
**  message Id in the command
**  
**  The \c Start field contains the word at which the failure occured
*/
#define SC_RTS_INVLD_MID_ERR_EID                            59


/** \brief <tt> 'RTS cmd loaded with invalid length at \%d, len: \%d' </tt>
**  \event <tt> 'RTS cmd loaded with invalid length at \%d, len: \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when and RTS table is loaded, but there is an invalid
**  length in the command
**  
**  The \c Start field contains the word at which the failure occured
**  The \c len field contauns the length in the commaand
*/
#define SC_RTS_LEN_ERR_EID                                  60


/** \brief <tt> 'RTS cmd at \%d runs off end of buffer' </tt>
**  \event <tt> 'RTS cmd at \%d runs off end of buffer' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when and RTS table is loaded, but the command
**  runs off the end of the buffer
**  
**  The \c Start field contains the word at which the failure occured
*/
#define SC_RTS_LEN_BUFFER_ERR_EID                           61


/** \brief <tt> 'RTS cmd loaded won't fit in buffer at \%d' </tt>
**  \event <tt> 'RTS cmd loaded won't fit in buffer at \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when and RTS table is loaded, but the command
**  won't fit in the buffer
**  
**  The \c Start field contains the word at which the failure occured
*/
#define SC_RTS_LEN_TOO_LONG_ERR_EID                         62


/** \brief <tt> 'Invalid command pipe message ID: 0x\%08X' </tt>
**  \event <tt> 'Invalid command pipe message ID: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an invalid message Id is recieved in the 
**  command pipe
**  
**  The \c Messge \c Id field contains the erroneous message Id
*/
#define SC_MID_ERR_EID                                      63


/** \brief <tt> 'Invalid Command Code: MID =  0x\%04X CC =  \%d' </tt>
**  \event <tt> 'Invalid Command Code: MID =  0x\%04X CC =  \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an invalid command code was recieved in
**  the command pipe
**  
**  The MID \c field contains the message Id for the command, and the \c CC
**  field contains the erroneous command code
*/
#define SC_INVLD_CMD_ERR_EID                                64


/** \brief <tt> 'RTS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of the Rts Info table
**  failed
**  
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define SC_GET_ADDRESS_RTS_INFO_ERR_EID                     65


/** \brief <tt> 'RTS Ctrl Blck table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS Ctrl Blck table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of the Rts Control Block table
**  failed
**  
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_RTS_CTRL_BLCK_ERR_EID               66


/** \brief <tt> 'ATS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of the Ats Info table
**  failed
**  
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define SC_GET_ADDRESS_ATS_INFO_ERR_EID                     67


/** \brief <tt> 'ATS Ctrl Blck table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS Ctrl Blck table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of the Ats Control Block table
**  failed
**  
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_ATS_CTRL_BLCK_ERR_EID               68


/** \brief <tt> 'ATS Cmd Status table for ATS \%d failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS Cmd Status table for ATS \%d failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of an Ats Command Status table
**  failed
**  
**  The \c ATS field contains the ATS Id of the Cmd Status table that failed
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_ATS_CMD_STAT_ERR_EID                69


/** \brief <tt> 'RTS table \%d failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'RTS table \%d failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of an RTS table
**  failed
**  
**  The \c RTS field contains the RTS Id of the table that failed
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_RTS_ERR_EID                         70


/** \brief <tt> 'ATS table \%d failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'ATS table \%d failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of an ATS table
**  failed
**  
**  The \c RTS field contains the RTS Id of the table that failed
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_ATS_ERR_EID                         71


/** \brief <tt> 'Start RTS #\%d command' </tt>
**  \event <tt> 'Start RTS #\%d command' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when a #SC_START_RTS_CC cmd is recieved and is sucessful
**  
**  The \c RTS field specifes the RTS ID to start
*/
#define SC_STARTRTS_CMD_DBG_EID                             72


/** \brief <tt> 'RTS Number \%03d Started' </tt>
**  \event <tt> 'RTS Number \%03d Started' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an RTS is started sucessfully
**  
**  The \c Number field contains the RTS Id that was started
*/
#define SC_RTS_START_INF_EID                                73


/** \brief <tt> 'Start RTS \%03d Rejected: Invld Len Field for 1st Cmd in Sequence. Invld Cmd Length = \%d' </tt>
**  \event <tt> 'Start RTS \%03d Rejected: Invld Len Field for 1st Cmd in Sequence. Invld Cmd Length = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an RTS is started but the first command has an invalid length
**  
**  The \c Number field contains the RTS Id that was started
*/
#define SC_STARTRTS_CMD_INVLD_LEN_ERR_EID                   74


/** \brief <tt> 'Start RTS \%03d Rejected: RTS Not Loaded or In Use, Status: \%d' </tt>
**  \event <tt> 'Start RTS \%03d Rejected: RTS Not Loaded or In Use, Status: \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an RTS is tried to be started, but the RTS is not
**  marked as #SC_LOADED
**  
**  The \c RTS field contains the RTS ID that was rejected, and the \c Status field 
**  contains the status of that RTS.
*/
#define SC_STARTRTS_CMD_NOT_LDED_ERR_EID                    75


/** \brief <tt> 'Start RTS \%03d Rejected: RTS Disabled' </tt>
**  \event <tt> 'Start RTS \%03d Rejected: RTS Disabled' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_START_RTS_CC command was recieved, but the
**  RTS is disabled
**  
**  The \c RTS field contains the RTS Id of the RTS that was rejected to start
*/
#define SC_STARTRTS_CMD_DISABLED_ERR_EID                    76


/** \brief <tt> 'Start RTS \%03d Rejected: Invalid RTS ID' </tt>
**  \event <tt> 'Start RTS \%03d Rejected: Invalid RTS ID' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_START_RTS_CC command was recieved, but the
**  RTS Id was invalid
**  
**  The \c RTS field contains the RTS Id of the RTS that was rejected to start
*/
#define SC_STARTRTS_CMD_INVALID_ERR_EID                     77


/** \brief <tt> 'RTS \%03d Aborted' </tt>
**  \event <tt> 'RTS \%03d Aborted' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an #SC_STOP_RTS_CC command is received and exexuted sucessfully
**  
**  The \c RTS field contains the RTS Id of the RTS that was stopped
*/
#define SC_STOPRTS_CMD_INF_EID                              78


/** \brief <tt> 'Stop RTS \%03d rejected: Invalid RTS ID' </tt>
**  \event <tt> 'Stop RTS \%03d rejected: Invalid RTS ID' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_STOP_RTS_CC command was rejected because the 
**  RTS Id given was invalid
**  
**  The \c RTS field contains the RTS Id of the RTS that was invalid
*/
#define SC_STOPRTS_CMD_ERR_EID                              79


/** \brief <tt> 'Disabled RTS \%03d' </tt>
**  \event <tt> 'Disabled RTS \%03d' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when a #SC_DISABLE_RTS_CC command was recieved, and executed sucessfully
**  
**  The \c RTS field contains the RTS Id of the RTS that was disabled
*/
#define SC_DISABLE_RTS_DEB_EID                              80


/** \brief <tt> 'Disable RTS \%03d Rejected: Invalid RTS ID' </tt>
**  \event <tt> 'Disable RTS \%03d Rejected: Invalid RTS ID' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_DISABLE_RTS_CC command was recieved, 
**  but the RTS Id given was invalid
**  
**  The \c RTS field contains the RTS Id of the RTS that was invalid 
*/
#define SC_DISRTS_CMD_ERR_EID                               81


/** \brief <tt> 'Enabled RTS \%03d' </tt>
**  \event <tt> 'Enabled RTS \%03d' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when a #SC_ENABLE_RTS_CC command was recieved, and executed sucessfully
**  
**  The \c RTS field contains the RTS Id of the RTS that was enabled
*/
#define SC_ENABLE_RTS_DEB_EID                               82


/** \brief <tt> 'Enable RTS \%03d Rejected: Invalid RTS ID' </tt>
**  \event <tt> 'Enable RTS \%03d Rejected: Invalid RTS ID' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_DISABLE_RTS_CC command was recieved, 
**  but the RTS Id given was invalid
**  
**  The \c RTS field contains the RTS Id of the RTS that was invalid 
*/
#define SC_ENARTS_CMD_ERR_EID                               83


/** \brief <tt> 'Cmd Runs passed end of Buffer, RTS \%03d Aborted' </tt>
**  \event <tt> 'Cmd Runs passed end of Buffer, RTS \%03d Aborted' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to get the next RTS command to execute,
**  and that command runs passed the end of the RTS table buffer
**  
**  The \c RTS field contains the RTS Id that failed
*/
#define SC_RTS_LNGTH_ERR_EID                                84


/** \brief <tt> 'Invalid Length Field in RTS Command, RTS \%03d Aborted. Length: \%d, Max: \%d' </tt>
**  \event <tt> 'Invalid Length Field in RTS Command, RTS \%03d Aborted. Length: \%d, Max: \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to get the next RTS command to execute,
**  and that command has an illegal length value in the command
**  
**  The \c RTS field contains the RTS Id that failed
**  The \c Length field is the length of the command
**  The \c Max field is #SC_PACKET_MAX_SIZE
*/
#define SC_RTS_CMD_LNGTH_ERR_EID                            85


/** \brief <tt> 'RTS \%03d Execution Completed' </tt>
**  \event <tt> 'RTS \%03d Execution Completed' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an RTS completes execution
**  
**  The \c RTS field contains the RTS Id that completed
*/
#define SC_RTS_COMPL_INF_EID                                86


/** \brief <tt> 'ATS \%c Execution Completed' </tt>
**  \event <tt> 'ATS \%c Execution Completed' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an ATS completes execution
**  
**  The \c ATS field contains the ATS that completed
*/
#define SC_ATS_COMPL_INF_EID                                87


/** \brief <tt> 'Jump Cmd: Skipped \%d ATS commands' </tt>
**  \event <tt> 'Jump Cmd: Skipped \%d ATS commands' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued a Jump Command is issued and
**  Some of the ATS commands were marked as skipped
**  
**  The \c Skipped field contains the number of ATS commands
**  that were skipped when jumping forward in time   
*/
#define SC_JUMP_ATS_SKIPPED_DBG_EID                         88


/** \brief <tt> 'Append ATS info table register failed, returned: 0x\%08X' </tt>
**  \event <tt> 'Append ATS info table register failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when trying to register the Append ATS Info Table
**  dump only table fails
**
**  The \c returned field contains the result from #CFE_TBL_Register
*/
#define SC_REGISTER_APPEND_INFO_TABLE_ERR_EID               90


/** \brief <tt> 'Append ATS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'Append ATS Info table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when getting the address of the Append ATS Info table
**  failed
**  
**  The returned \c field contains the result from #CFE_TBL_GetAddress
*/
#define SC_GET_ADDRESS_APPEND_INFO_ERR_EID                  91


/** \brief <tt> 'Append ATS table failed Getting Address, returned: 0x\%08X' </tt>
**  \event <tt> 'Append ATS table failed Getting Address, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates a failure to get the table data address from
**  cFE Table Services
**  
**  The returned value is the result from the call to #CFE_TBL_GetAddress
*/
#define  SC_GET_ADDRESS_APPEND_ERR_EID                      92


/** \brief <tt> 'Append ATS Table Registration Failed, returned: 0x\%08X' </tt>
**  \event <tt> 'Append ATS Table Registration Failed, returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates a failure to register the table with cFE Table Services
**
**  The returned value is the result from the call to #CFE_TBL_Register
*/
#define SC_REGISTER_APPEND_TBL_ERR_EID                      93


/** \brief <tt> 'Update Append ATS Table: load count = \%d, command count = \%d, byte count = \%d' </tt>
**  \event <tt> 'Update Append ATS Table: load count = \%d, command count = \%d, byte count = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message signals that the Append ATS table has been updated.
**
**  The displayed values include a load counter to identify this particular update,
**  the number of valid ATS command entries in the table and the size (in bytes) of
**  the portion of the table containing valid ATS command entries.
*/
#define SC_UPDATE_APPEND_EID                                97


/** \brief <tt> 'Append ATS \%c command: \%d ATS entries appended' </tt>
**  \event <tt> 'Append ATS \%c command: \%d ATS entries appended' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event signals the successful completion of an Append ATS command.
**  
**  The event text displays the ATS selection command argument (A or B) and
**  the number of Append table entries appended to the selected ATS.
*/
#define SC_APPEND_CMD_INF_EID                               98


/** \brief <tt> 'Append ATS error: invalid ATS ID = \%d' </tt>
**  \event <tt> 'Append ATS error: invalid ATS ID = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_APPEND_ATS_CC command has failed
**  because the specified ATS command argument was not a valid ATS ID.
**  
**  The event text displays the invalid ATS ID command argument.
*/
#define SC_APPEND_CMD_ARG_ERR_EID                           99


/** \brief <tt> 'Append ATS \%c error: ATS table is empty' </tt>
**  \event <tt> 'Append ATS \%c error: ATS table is empty' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_APPEND_ATS_CC command has failed
**  because the specified target ATS table is empty.  The Append ATS command
**  requires that both the source Append table and the target ATS table have
**  valid contents.
**  
**  The event text displays the ATS ID command argument (A or B).
*/
#define SC_APPEND_CMD_TGT_ERR_EID                           100


/** \brief <tt> 'Append ATS \%c error: Append table is empty' </tt>
**  \event <tt> 'Append ATS \%c error: Append table is empty' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_APPEND_ATS_CC command has failed
**  because the source Append table is empty.  The Append ATS command requires
**  that both the source Append table and the target ATS table have valid
**  contents.
**  
**  The event text displays the ATS ID command argument (A or B).
*/
#define SC_APPEND_CMD_SRC_ERR_EID                           101


/** \brief <tt> 'Append ATS \%c error: ATS size = \%d, Append size = \%d, ATS buffer = \%d' </tt>
**  \event <tt> 'Append ATS \%c error: ATS size = \%d, Append size = \%d, ATS buffer = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_APPEND_ATS_CC command has failed
**  because there is not room in the specified target ATS table to add the
**  contents of the Append table.
**  
**  The event text displays the ATS ID command argument (A or B), the size
**  (in words) of the data in the specified ATS table, the size of the data
**  in the Append table and the total size of the ATS table.
*/
#define SC_APPEND_CMD_FIT_ERR_EID                           102


/** \brief <tt> 'Verify ATS Table: command count = \%d, byte count = \%d' </tt>
**  \event <tt> 'Verify ATS Table: command count = \%d, byte count = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message signals the successful verification of an ATS or an
**     Append ATS table.
**  The displayed values indicate the number of table entries (ATS commands)
**     and the total size of the in use table data (in bytes).
*/
#define SC_VERIFY_ATS_EID                                   103


/** \brief <tt> 'Verify ATS Table error: invalid command number: buf index = \%d, cmd num = \%d' </tt>
**  \event <tt> 'Verify ATS Table error: invalid command number: buf index = \%d, cmd num = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to an invalid table entry. The cause is
**  a table entry with an invalid command number.
**
**  The event text describes the offset into the table in words for the
**  invalid ATS table entry and the invalid table entry command number.
*/
#define SC_VERIFY_ATS_NUM_ERR_EID                           104


/** \brief <tt> 'Verify ATS Table error: buffer full: buf index = \%d, cmd num = \%d, buf words = \%d' </tt>
**  \event <tt> 'Verify ATS Table error: buffer full: buf index = \%d, cmd num = \%d, buf words = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to an invalid table entry. The cause is
**  a table entry that has a valid command number but the entry begins
**  at an offset too near the end of the table buffer to provide room
**  for even the smallest possible command packet.
**
**  This error can be corrected by setting the first data word which
**  follows the last valid table entry to zero. Note that the first
**  word in an ATS table entry is the entry command number.
**
**  The event text describes the offset into the table in words for the
**  invalid ATS table entry, the valid entry command number and the
**  total size of the table buffer in words.
*/
#define SC_VERIFY_ATS_END_ERR_EID                           105


/** \brief <tt> 'Verify ATS Table error: invalid length: buf index = \%d, cmd num = \%d, pkt len = \%d' </tt>
**  \event <tt> 'Verify ATS Table error: invalid length: buf index = \%d, cmd num = \%d, pkt len = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to an invalid table entry. The cause is
**  a table entry with an invalid command packet length.
**
**  The event text describes the offset into the table in words for the
**  invalid ATS table entry, the valid table entry command number and
**  the invalid table entry command packet length in bytes.
*/
#define SC_VERIFY_ATS_PKT_ERR_EID                           106


/** \brief <tt> 'Verify ATS Table error: buffer overflow: buf index = \%d, cmd num = \%d, pkt len = \%d' </tt>
**  \event <tt> 'Verify ATS Table error: buffer overflow: buf index = \%d, cmd num = \%d, pkt len = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to an invalid table entry. The cause is
**  a table entry with an otherwise valid command packet length that
**  would extend past the end of the table buffer.
**
**  The event text describes the offset into the table in words for the
**  invalid ATS table entry, the valid table entry command number and
**  the table entry command packet length in bytes.
*/
#define SC_VERIFY_ATS_BUF_ERR_EID                           107


/** \brief <tt> 'Verify ATS Table error: dup cmd number: buf index = \%d, cmd num = \%d, dup index = \%d' </tt>
**  \event <tt> 'Verify ATS Table error: dup cmd number: buf index = \%d, cmd num = \%d, dup index = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to an invalid table entry. The cause is
**  a table entry with an otherwise valid command number that is already
**  in use by an earlier table entry.
**
**  The event text describes the offset into the table in words for the
**  invalid ATS table entry, the table entry command number and the
**  table offset in words for the earlier entry using the same number.
*/
#define SC_VERIFY_ATS_DUP_ERR_EID                           109


/** \brief <tt> 'Verify ATS Table error: table is empty' </tt>
**  \event <tt> 'Verify ATS Table error: table is empty' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message indicates an error during verification of an ATS
**  or an Append ATS table due to the table having no entries.  This
**  error can only occur if the first entry in the table has a command
**  number equal to zero - the end of data marker.
*/
#define SC_VERIFY_ATS_MPT_ERR_EID                           110


/** \brief <tt> 'Table manage command packet error: table ID = \%d' </tt>
**  \event <tt> 'Table manage command packet error: table ID = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued upon receipt of a table manage
**  request command that has an invalid table ID argument.
**
**  The event text includes the invalid table ID.
*/
#define SC_TABLE_MANAGE_ID_ERR_EID                          111


/** \brief <tt> 'RTS table manage process error: RTS = \%d, Result = 0x\%X' </tt>
**  \event <tt> 'RTS table manage process error: RTS = \%d, Result = 0x\%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  The expectation is that this command is sent by cFE Table
**  Services only when the indicated RTS table has been updated.
**  Thus, after allowing cFE Table Services an opportunity to
**  manage the table, the call to re-acquire the table data
**  pointer is expected to return an indication that the table
**  data has been updated.  This event message is issued upon
**  receipt of any other function result.
**
**  The event text includes the RTS number and the unexpected
**  result.
*/
#define SC_TABLE_MANAGE_RTS_ERR_EID                         112


/** \brief <tt> 'ATS table manage process error: ATS = \%d, Result = 0x\%X' </tt>
**  \event <tt> 'ATS table manage process error: ATS = \%d, Result = 0x\%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  The expectation is that this command is sent by cFE Table
**  Services only when the indicated ATS table has been updated.
**  Thus, after allowing cFE Table Services an opportunity to
**  manage the table, the call to re-acquire the table data
**  pointer is expected to return an indication that the table
**  data has been updated.  This event message is issued upon
**  receipt of any other function result.
**
**  The event text includes the ATS number and the unexpected
**  result.
*/
#define SC_TABLE_MANAGE_ATS_ERR_EID                         113


/** \brief <tt> 'ATS Append table manage process error: Result = 0x\%X' </tt>
**  \event <tt> 'ATS Append table manage process error: Result = 0x\%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  The expectation is that this command is sent by cFE Table
**  Services only when the ATS Append table has been updated.
**  Thus, after allowing cFE Table Services an opportunity to
**  manage the table, the call to re-acquire the table data
**  pointer is expected to return an indication that the table
**  data has been updated.  This event message is issued upon
**  receipt of any other function result.
**
**  The event text includes the unexpected result.
*/
#define SC_TABLE_MANAGE_APPEND_ERR_EID                      114


#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/** \brief <tt> 'Start RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  \event <tt> 'Start RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message is issued following the successful execution of
**  a #SC_START_RTSGRP_CC command.
**  
**  The event text includes the group definition values and the count of RTS
**  in the group that were actually affected by the command (may be zero).
*/
#define SC_STARTRTSGRP_CMD_INF_EID                          115


/** \brief <tt> 'Start RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  \event <tt> 'Start RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_START_RTSGRP_CC command was
**  rejected because the RTS group definition was invalid:
**  - First RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be greater than or equal to First RTS ID
**  
**  The event text includes the invalid group definition values.
*/
#define SC_STARTRTSGRP_CMD_ERR_EID                          116


/** \brief <tt> 'Stop RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  \event <tt> 'Stop RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message is issued following the successful execution of
**  a #SC_STOP_RTSGRP_CC command.
**  
**  The event text includes the group definition values and the count of RTS
**  in the group that were actually affected by the command (may be zero).
*/
#define SC_STOPRTSGRP_CMD_INF_EID                           117


/** \brief <tt> 'Stop RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  \event <tt> 'Stop RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_STOP_RTSGRP_CC command was
**  rejected because the RTS group definition was invalid:
**  - First RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be greater than or equal to First RTS ID
**  
**  The event text includes the invalid group definition values.
*/
#define SC_STOPRTSGRP_CMD_ERR_EID                           118


/** \brief <tt> 'Disable RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  \event <tt> 'Disable RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message is issued following the successful execution of
**  a #SC_DISABLE_RTSGRP_CC command.
**  
**  The event text includes the group definition values and the count of RTS
**  in the group that were actually affected by the command (may be zero).
*/
#define SC_DISRTSGRP_CMD_INF_EID                            119


/** \brief <tt> 'Disable RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  \event <tt> 'Disable RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_DISABLE_RTSGRP_CC command was
**  rejected because the RTS group definition was invalid:
**  - First RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be greater than or equal to First RTS ID
**  
**  The event text includes the invalid group definition values.
*/
#define SC_DISRTSGRP_CMD_ERR_EID                            120


/** \brief <tt> 'Enable RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  \event <tt> 'Enable RTS group: FirstID=\%d, LastID=\%d, Modified=\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**  This event message is issued following the successful execution of
**  a #SC_ENABLE_RTSGRP_CC command.
**  
**  The event text includes the group definition values and the count of RTS
**  in the group that were actually affected by the command (may be zero).
*/
#define SC_ENARTSGRP_CMD_INF_EID                            121


/** \brief <tt> 'Enable RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  \event <tt> 'Enable RTS group error: FirstID=\%d, LastID=\%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SC_ENABLE_RTSGRP_CC command was
**  rejected because the RTS group definition was invalid:
**  - First RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be 1 through #SC_NUMBER_OF_RTS
**  - Last RTS ID must be greater than or equal to First RTS ID
**  
**  The event text includes the invalid group definition values.
*/
#define SC_ENARTSGRP_CMD_ERR_EID                            122
#endif

#endif /*_sc_events_*/

/************************/
/*  End of File Comment */
/************************/

