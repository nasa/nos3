 /************************************************************************
 ** File:
 **   $Id: cs_events.h 1.7 2017/03/29 17:29:03EDT mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   Specification for the CFS Checksum event identifers.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 ** 
 *************************************************************************/

#ifndef _cs_events_
#define _cs_events_


 /*************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

 /** \brief <tt> 'CS Initialized. Version \%d.\%d.\%d.\%d' </tt>
 **  \event <tt> 'CS Initialized. Version \%d.\%d.\%d.\%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when the CFS Checksum has
 **  completed initialization. The fields are the #CS_MAJOR_VERSION,
 **  #CS_MINOR_VERSION, #CS_REVISION, and #CS_MISSION_REV numbers.
 */
#define CS_INIT_INF_EID                                     1    

 /** \brief <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
 **  \event <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a NOOP command has been received.
 **  The fields are the #CS_MAJOR_VERSION, #CS_MINOR_VERSION, #CS_REVISION, 
 **  and #CS_MISSION_REV numbers.
 */
 #define CS_NOOP_INF_EID                                    2

 /** \brief <tt> 'Reset counters command received' </tt>
 **  \event <tt> 'Reset counters command received' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a reset counters command has
 **  been received.    
 */
 #define CS_RESET_DBG_EID                                   3

 /** \brief <tt> 'Background Checksumming Disabled' </tt>
 **  \event <tt> 'Background Checksumming Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  command has been received. 
 */
#define CS_DISABLE_ALL_INF_EID                              4

 /** \brief <tt> 'Background Checksumming Enabled' </tt>
 **  \event <tt> 'Background Checksumming Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  command has been received. 
 */
#define CS_ENABLE_ALL_INF_EID                               5

 /** \brief <tt> 'Checksumming of cFE Core is Disabled' </tt>
 **  \event <tt> 'Checksumming of cFE Core is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the cFE core command has been received. 
 */
#define CS_DISABLE_CFECORE_INF_EID                          6

 /** \brief <tt> 'Checksumming of cFE Core is Enabled' </tt>
 **  \event <tt> 'Checksumming of cFE Core is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the cFE core command has been received. 
 */
#define CS_ENABLE_CFECORE_INF_EID                           7  

 /** \brief <tt> 'Checksumming of OS code segment is Disabled' </tt>
 **  \event <tt> 'Checksumming of OS code segment is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the OS code segment command has been received. 
 */
#define CS_DISABLE_OS_INF_EID                               8              

 /** \brief <tt> 'Checksumming of OS code segment is Enabled' </tt>
 **  \event <tt> 'Checksumming of OS code segment is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the OS code segment command has been received. 
 */
#define CS_ENABLE_OS_INF_EID                                9  

 /** \brief <tt> 'Baseline of cFE Core is 0x\%08X' </tt>
 **  \event <tt> 'Baseline of cFE Core is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the cFE core command has been received and there is
 **  a baseline computed to report.
 **  
 **  The \c baseline field identifies the baseline checksum
 **  of the cFE core.
 */
#define CS_BASELINE_CFECORE_INF_EID                         10  

 /** \brief <tt> 'Baseline of cFE Core has not been computed yet' </tt>
 **  \event <tt> 'Baseline of cFE Core has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the cFE core command has been received but the 
 **  baseline has not yet been computed.
 */
#define CS_NO_BASELINE_CFECORE_INF_EID                      11  

 /** \brief <tt> 'Baseline of OS code segment is 0x\%08X' </tt>
 **  \event <tt> 'Baseline of OS code segment is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the OS code segment command has been received 
 **  and there is a baseline computed to report.
 **  
 **  The \c baseline field identifies the baseline checksum
 **  of the OS code segment.
 */
#define CS_BASELINE_OS_INF_EID                              12

 /** \brief <tt> 'Baseline of OS code segment has not been computed yet' </tt>
 **  \event <tt> 'Baseline of OS code segment has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the cFE core command has been received but the 
 **  baseline has not yet been computed.
 */
#define CS_NO_BASELINE_OS_INF_EID                           13

 /** \brief <tt> 'Recompute of cFE core started' </tt>
 **  \event <tt> 'Recompute of cFE core started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the cFE core command has been received and the
 **  recompute task has been started.
 */
#define CS_RECOMPUTE_CFECORE_STARTED_DBG_EID                14

 /** \brief <tt> 'Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  \event <tt> 'Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the cFE core command has been received and the
 **  recompute failed because CFE_ES_CreateChildTask returned
 **  an error.
 **
 **  The \c returned field specifies the error returned by
 **  CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID         15  

 /** \brief <tt> 'Recompute cFE core failed: child task in use' </tt>
 **  \event <tt> 'Recompute cFE core failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the cFE core command has been received and the
 **  recompute failed because there is already a CS child
 **  task running.
 */
#define CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID                16  

 /** \brief <tt> 'Recompute of OS code segment started' </tt>
 **  \event <tt> 'Recompute of OS code segment started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the OS code segment command has been received and the
 **  recompute task has been started.
 */
#define CS_RECOMPUTE_OS_STARTED_DBG_EID                     17

 /** \brief <tt> 'Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  \event <tt> 'Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the OS code segment command has been received and the
 **  recompute failed because CFE_ES_CreateChildTask returned
 **  an error.
 **
 **  The \c returned field specifies the error returned by
 **  CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID              18

 /** \brief <tt> 'Recompute OS code segment failed: child task in use' </tt>
 **  \event <tt> 'Recompute OS code segment failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the OS code segment command has been received and the
 **  recompute failed because there is already a CS child
 **  task running.
 */
#define CS_RECOMPUTE_OS_CHDTASK_ERR_EID                     19

 /** \brief <tt> 'OneShot checksum started on address: 0x\%08X, size: \%d' </tt>
 **  \event <tt> 'OneShot checksum started on address: 0x\%08X, size: \%d' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a OneShot calculation
 **  command has been received and the OneShot task has been started.
 **
 ** The \c address field denote the start of the OneShot calculation. <br>
 ** The \c size denotes the number of bytes over which to calculate.
 */
#define CS_ONESHOT_STARTED_DBG_EID                          20

 /** \brief <tt> 'OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  \event <tt> 'OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a OneShot calculation
 **  command has been received and the OneShot failed because
 **  CFE_ES_CreateChildTask returned an error.
 **
 **  The \c returned field specifies the error returned by
 **  CFE_ES_CreateChildTask.
 */
#define CS_ONESHOT_CREATE_CHDTASK_ERR_EID                   21

 /** \brief <tt> 'OneShot checksum failed: child task in use' </tt>
 **  \event <tt> 'OneShot checksum failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a OneShot command
 **  has been received and the OneShot failed because
 **  there is already a CS child task running.
 */
#define CS_ONESHOT_CHDTASK_ERR_EID                          22

 /** \brief <tt> 'OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  \event <tt> 'OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a OneShot command
 **  has been received and the OneShot failed because
 **  CFE_PSP_MemValidateRange returned an error.
 */
#define CS_ONESHOT_MEMVALIDATE_ERR_EID                      23

 /** \brief <tt> 'OneShot checksum calculation has been cancelled' </tt>
 **  \event <tt> 'OneShot checksum calculation has been cancelled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a  cancel OneShot calculation
 **  command has been received and the OneShot task has been cancelled.
 */
#define CS_ONESHOT_CANCELLED_INF_EID                        24

 /** \brief <tt> 'Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x\%08X' </tt>
 **  \event <tt> 'Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a cancel OneShot calculation
 **  command has been received and the cancel OneShot failed because
 **  CFE_ES_DeleteChildTask returned an error.
 **
 **  The \c returned field specifies the error returned by
 **  CFE_ES_DeelteChildTask
 */
#define CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID            25

 /** \brief <tt> 'Cancel OneShot checksum failed. No OneShot active' </tt>
 **  \event <tt> 'Cancel OneShot checksum failed. No OneShot active' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a cancel OneShot command
 **  has been received and the cancel OneShot failed because
 **  there was no OneShot child task running.
 */
#define CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID                26

 /** \brief <tt> 'Checksum Failure: Entry %d in Eeprom Table, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: Entry %d in Eeprom Table, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming entries in the EEPROM table.
 **
 **  The \c Entry field is the entry ID in the table. <br>
 **  The \c Expected field is the checksum value that was expected to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_EEPROM_MISCOMPARE_ERR_EID                        27

 /** \brief <tt> 'Checksum Failure: Entry \%d in Memory Table, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: Entry \%d in Memory Table, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming entries in the user-define memory table
 **
 **  The \c Entry field is the entry ID in the table. <br>
 **  The \c Expected field is the checksum value that was expected to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_MEMORY_MISCOMPARE_ERR_EID                        28

 /** \brief <tt> 'Checksum Failure: Table \%s, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: Table \%s, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming entries in the table of tables to checksum.
 **
 **  The \c Table field is the name of the table. <br>
 **  The \c Expected field is the checksum value that was expected to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_TABLES_MISCOMPARE_ERR_EID                        29

 /** \brief <tt> 'Checksum Failure: Application \%s, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: Application \%s, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming entries in the table of applications to checksum.
 **
 **  The \c Application field is the name of the app. <br>
 **  The \c Expected field is the checksum value that was expected to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_APP_MISCOMPARE_ERR_EID                           30

 /** \brief <tt> 'Checksum Failure: cFE Core, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: cFE Core, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming the cFE Core.
 **
 **  The \c Expected field is the checksum value that was expected
 **  to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_CFECORE_MISCOMPARE_ERR_EID                       31

 /** \brief <tt> 'Checksum Failure: OS code segment, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  \event <tt> 'Checksum Failure: OS code segment, Expected: 0x\%08X, Calculated: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a checksum miscompare occurs
 **  when checksumming the OS code segment.
 **
 **  The \c Expected field is the checksum value that was expected
 **  to be calculated. <br>
 **  The \c Calculated field is the new value that was calculated.
 */
#define CS_OS_MISCOMPARE_ERR_EID                            32

  /** \brief <tt> 'Invalid command pipe message ID: 0x\%X' </tt>
 **  \event <tt> 'Invalid command pipe message ID: 0x\%X' </tt>
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
#define CS_MID_ERR_EID                                      33

  /** \brief <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt>
 **  \event <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a software bus message is received
 **  with an invalid command code.    
 **
 **  The \c ID field contains the message ID. <br>
 **  The \c CC field contains the command code that generated the error.
 */
#define CS_CC1_ERR_EID                                      34

 /** \brief <tt> 'App terminating, RunStatus:0x\%08X, RC:0x\%08X' </tt>
 **  \event <tt> 'App terminating, RunStatus:0x\%08X, RC:0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CS has a critical error in its
 **  main loop
 **
 **  The \c RunStatus field specifies the reason for CS to 
 **  stop execution. The \c RC field is the return code from
 **  Application initialization, read Software Message Bus error
 **  or an unrecoverable error from the command processing
 */
#define CS_EXIT_ERR_EID                                     35


 /** \brief <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
 **  \event <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when command message is received with a message
 **  length that doesn't match the expected value. 
 **
 **  The \c ID field contains the message ID. <br>
 **  The \c CC field contains the command code. <br>
 **  The \c Len field is the actual length returned by the CFE_SB_GetTotalMsgLength call. <br>
 **  The \c Expected field is the expected length for messages with that command code.
 */
#define CS_LEN_ERR_EID                                      36


 /**********************************************************************/
 /*EEPROM Commands                                                     */
 /**********************************************************************/
 /** \brief <tt> 'Checksumming of Eeprom is Disabled' </tt>
 **  \event <tt> 'Checksumming of Eeprom is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the Eeprom table command has been received. 
 */
#define CS_DISABLE_EEPROM_INF_EID                           37

 /** \brief <tt> 'Checksumming of Eeprom is Enabled' </tt>
 **  \event <tt> 'Checksumming of Eeprom is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the Eeprom table command has been received. 
 */
#define CS_ENABLE_EEPROM_INF_EID                            38

 /** \brief <tt> 'Report baseline of Eeprom Entry \%d is 0x\%08X' </tt>
 **  \event <tt> 'Report baseline of Eeprom Entry \%d is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the Eeprom entry specifiedcommand has been received
 **  and there is a baseline computed to report.
 **  
 **  The \c Entry field is the command specified entry. <br> 
 **  The \c baseline field identifies the baseline checksum
 **  of the Eeprom entry.
 */
#define CS_BASELINE_EEPROM_INF_EID                          39

 /** \brief <tt> 'Report baseline of Eeprom Entry \%d has not been computed yet' </tt>
 **  \event <tt> 'Report baseline of Eeprom Entry \%d has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified entry command has been 
 **  received but the baseline has not yet been computed.
 **
 **  The \c Entry field identifies the command specified Eeprom Entry ID.
 */
#define CS_NO_BASELINE_EEPROM_INF_EID                       40

 /** \brief <tt> 'Eeprom report baseline failed, Entry ID invalid: \%d, State: \%d Max: \%d' </tt>
 **  \event <tt> 'Eeprom report baseline failed, Entry ID invalid: \%d, State: \%d Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified entry command has been 
 **  received but specified Entry ID is invalid.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID            41

 /** \brief <tt> 'Recompute baseline of Eeprom Entry ID \%d started' </tt>
 **  \event <tt> 'Recompute baseline of Eeprom Entry ID \%d started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified Eeprom Entry ID command has been received and the
 **  recompute task has been started.
 **
 **  The \c Entry \c ID field is the command specified Eeprom entry to recompute.
 */
#define CS_RECOMPUTE_EEPROM_STARTED_DBG_EID                 42

 /** \brief <tt> 'Recompute baseline of Eeprom Entry ID \%d failed, CFE_ES_CreateChildTask returned:  0x\%08X' </tt>
 **  \event <tt> 'Recompute baseline of Eeprom Entry ID \%d failed, CFE_ES_CreateChildTask returned:  0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified Eeprom Entry ID command has been 
 **  received and the recompute failed because 
 **  CFE_ES_CreateChildTask returned an error.
 **
 **  The \c Entry \c ID field is the entry that was specified 
 **  in the command. <br>
 **  The \c returned field specifies the error 
 **  returned by CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID          43

 /** \brief <tt> 'Eeprom recompute baseline of entry failed, Entry ID invalid: \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Eeprom recompute baseline of entry failed, Entry ID invalid: \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified entry command has been 
 **  received but specified Entry ID is invalid.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID           44

 /** \brief <tt> 'Recompute baseline of Eeprom Entry ID \%d failed: child task in use' </tt>
 **  \event <tt> 'Recompute baseline of Eeprom Entry ID \%d failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified Entry ID command has been 
 **  received and the recompute failed because there is 
 **  already a CS child task running.
 **
  **  The \c Entry \c ID field is the command specified Eeprom entry to recompute.
 */
#define CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID                 45

 /** \brief <tt> 'Checksumming of Eeprom Entry ID \%d is Enabled' </tt>
 **  \event <tt> 'Checksumming of Eeprom Entry ID \%d is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:invalid
 **
 **  This event message is issued when an enable Eeprom Entry ID
 **  command is accepted.
 **
 **  The \c Entry \c ID field is the command specifed Entry ID
 **  to enable.
 */
#define CS_ENABLE_EEPROM_ENTRY_INF_EID                      46

 /** \brief <tt> 'Enable Eeprom entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Enable Eeprom entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable Eeprom Entry ID
 **  command is received, but has an invalid Entry ID.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID              47

 /** \brief <tt> 'Checksumming of Eeprom Entry ID \%d is Disabled' </tt>
 **  \event <tt> 'Checksumming of Eeprom Entry ID \%d is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable Eeprom Entry ID
 **  command is accepted.
 **
 **  The \c Entry \c ID field is the command specifed Entry ID
 **  to disable.
 */
#define CS_DISABLE_EEPROM_ENTRY_INF_EID                     48

 /** \brief <tt> 'Disable Eeprom entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Disable Eeprom entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable Eeprom Entry ID
 **  command is received, but has an invalid Entry ID.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */

#define CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID             49


 /** \brief <tt> 'Eeprom Found Address 0x\%08X in Entry ID \%d' </tt>
 **  \event <tt> 'Eeprom Found Address 0x\%08X in Entry ID \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a Get Entry ID Eeprom
 **  command is received and the command specified address
 **  is found in the Eeprom table.
 **
 **  The \c Address field is the command specified address. <br>
 **  The \c Entry \c ID field is the Entry ID where the 
 **  the address was found
 **  
 **  Note that more than one of these event messages can
 **  be sent per command (if the address is in several
 **  entries in the table).
 */
#define CS_GET_ENTRY_ID_EEPROM_INF_EID                      50

 /** \brief <tt> 'Address 0x\%08X was not found in Eeprom table' </tt>
 **  \event <tt> 'Address 0x\%08X was not found in Eeprom table' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when the command specified
 **  address in a Get Entry ID Eeprom command cannot be found
 **  in the Eeprom table.
 **
 **  The \c Address field is the command specified address.
 */
#define CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID            51

 /**********************************************************************/
 /*MEMORY Commands                                                     */
 /**********************************************************************/
 /** \brief <tt> 'Checksumming of Memory is Disabled' </tt>
 **  \event <tt> 'Checksumming of Memory is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the Memory table command has been received. 
 */
#define CS_DISABLE_MEMORY_INF_EID                           52

 /** \brief <tt> 'Checksumming of Memory is Enabled' </tt>
 **  \event <tt> 'Checksumming of Memory is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the Memory table command has been received. 
 */
#define CS_ENABLE_MEMORY_INF_EID                            53

 /** \brief <tt> 'Report baseline of Memory Entry \%d is 0x\%08X' </tt>
 **  \event <tt> 'Report baseline of Memory Entry \%d is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the Memory entry specifiedcommand has been received
 **  and there is a baseline computed to report.
 **  
 **  The \c Entry field is the command specified entry. <br> 
 **  The \c baseline field identifies the baseline checksum
 **  of the Memory entry.
 */
#define CS_BASELINE_MEMORY_INF_EID                          54

 /** \brief <tt> 'Report baseline of Memory Entry \%d has not been computed yet' </tt>
 **  \event <tt> 'Report baseline of Memory Entry \%d has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified entry command has been 
 **  received but the baseline has not yet been computed.
 **
 **  The \c Entry field identifies the command specified Memory Entry ID.
 */
#define CS_NO_BASELINE_MEMORY_INF_EID                       55

 /** \brief <tt> 'Memory report baseline failed, Entry ID invalid: \%d, State: \%d Max: \%d' </tt>
 **  \event <tt> 'Memory report baseline failed, Entry ID invalid: \%d, State: \%d Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified entry command has been 
 **  received but specified Entry ID is invalid.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID            56

 /** \brief <tt> 'Recompute baseline of Memory Entry ID \%d started' </tt>
 **  \event <tt> 'Recompute baseline of Memory Entry ID \%d started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified Memory Entry ID command has been received and the
 **  recompute task has been started.
 **
 **  The \c Entry \c ID field is the command specified Memory entry to recompute.
 */
#define CS_RECOMPUTE_MEMORY_STARTED_DBG_EID                 57

 /** \brief <tt> 'Recompute baseline of Memory Entry ID \%d failed, ES_CreateChildTask returned:  0x\%08X' </tt>
 **  \event <tt> 'Recompute baseline of Memory Entry ID \%d failed, ES_CreateChildTask returned:  0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified Memory Entry ID command has been 
 **  received and the recompute failed because 
 **  CFE_ES_CreateChildTask returned an error.
 **
 **  The \c Entry \c ID field is the entry that was specified 
 **  in the command. <br>
 **  The \c returned field specifies the error 
 **  returned by CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID          58

 /** \brief <tt> 'Memory recompute baseline of entry failed, Entry ID invalid: \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Memory recompute baseline of entry failed, Entry ID invalid: \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified entry command has been 
 **  received but specified Entry ID is invalid.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID           59

 /** \brief <tt> 'Recompute baseline of Memory Entry ID \%d failed: child task in use' </tt>
 **  \event <tt> 'Recompute baseline of Memory Entry ID \%d failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified Entry ID command has been 
 **  received and the recompute failed because there is 
 **  already a CS child task running.
 **
 **  The \c Entry \c ID field is the command specified Memory entry to recompute.
 */
#define CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID                 60

 /** \brief <tt> 'Checksumming of Memory Entry ID \%d is Enabled' </tt>
 **  \event <tt> 'Checksumming of Memory Entry ID \%d is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable Memory Entry ID
 **  command is accepted.
 **
 **  The \c Entry \c ID field is the command specifed Entry ID
 **  to enable.
 */
#define CS_ENABLE_MEMORY_ENTRY_INF_EID                      61

 /** \brief <tt> 'Enable Memory entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Enable Memory entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable Memory Entry ID
 **  command is received, but has an invalid Entry ID.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
 
#define CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID              62

 /** \brief <tt> 'Checksumming of Memory Entry ID \%d is Disabled' </tt>
 **  \event <tt> 'Checksumming of Memory Entry ID \%d is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable Memory Entry ID
 **  command is accepted.
 **
 **  The \c Entry \c ID field is the command specifed Entry ID
 **  to disable.
 */
#define CS_DISABLE_MEMORY_ENTRY_INF_EID                     63

 /** \brief <tt> 'Disable Memory entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  \event <tt> 'Disable Memory entry failed, invalid Entry ID:  \%d, State: \%d, Max: \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable Memory Entry ID
 **  command is received, but has an invalid Entry ID.
 **
 **  The \c Entry \c ID field is the command specified Entry ID
 **  that was invalid. <br>
 **  The \c State field is the state of the invalid entry. <br>
 **  The \c Max field is the highest entry ID allowed. 
 */
#define CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID             64


 /** \brief <tt> 'Memory Found Address 0x\%08X in Entry ID \%d' </tt>
 **  \event <tt> 'Memory Found Address 0x\%08X in Entry ID \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a Get Entry ID Memory
 **  command is received and the command specified address
 **  is found in the Memory table.
 **
 **  The \c Address field is the command specified address. <br>
 **  The \c Entry \c ID field is the Entry ID wherre the 
 **  the address was found
 **  
 **  Note that more than one of these event messages can
 **  be sent per command (if the address is in several
 **  entries in the table).
 */
#define CS_GET_ENTRY_ID_MEMORY_INF_EID                      65

 /** \brief <tt> 'Address 0x\%08X was not found in Memory table' </tt>
 **  \event <tt> 'Address 0x\%08X was not found in Memory table' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when the command specified
 **  address in a Get Entry ID Memory command cannot be found
 **  in the Memory table.
 **
 **  The \c Address field is the command specified address.
 */
#define CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID            66

 /**********************************************************************/
 /*TABLES Commands                                                     */
 /**********************************************************************/

 /** \brief <tt> 'Checksumming of Tables is Disabled' </tt>
 **  \event <tt> 'Checksumming of Tables is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the Tables table command has been received. 
 */
#define CS_DISABLE_TABLES_INF_EID                           67

 /** \brief <tt> 'Checksumming of Tables is Enabled' </tt>
 **  \event <tt> 'Checksumming of Tables is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the Tables table command has been received. 
 */
#define CS_ENABLE_TABLES_INF_EID                            68

 /** \brief <tt> 'Report baseline of table \%s is 0x\%08X' </tt>
 **  \event <tt> 'Report baseline of table \%s is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the Tables entry specifiedcommand has been received
 **  and there is a baseline computed to report.
 **  
 **  The \c table field is the command specified table name. <br> 
 **  The \c baseline field identifies the baseline checksum
 **  of the table.
 */
#define CS_BASELINE_TABLES_INF_EID                          69

 /** \brief <tt> 'Report baseline of table \%s has not been computed yet' </tt>
 **  \event <tt> 'Report baseline of table \%s has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified table command has been 
 **  received but the baseline has not yet been computed.
 **  
 **  The \c table field identifies the command specified table.
 */
#define CS_NO_BASELINE_TABLES_INF_EID                       70

 /** \brief <tt> 'Tables report baseline failed, table \%s not found' </tt>
 **  \event <tt> 'Tables report baseline failed, table \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified table command has been 
 **  received but specified table name cannot be found
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c table field is the command specified table
 **  that wasn't found in CS.
 */
#define CS_BASELINE_INVALID_NAME_TABLES_ERR_EID             71

 /** \brief <tt> 'Recompute baseline of table \%s started' </tt>
 **  \event <tt> 'Recompute baseline of table \%s started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified table command has been received and the
 **  recompute task has been started.
 **
 **  The \c table field is the command specified table to recompute.
 */
#define CS_RECOMPUTE_TABLES_STARTED_DBG_EID                 72

 /** \brief <tt> 'Recompute baseline of table \%s failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  \event <tt> 'Recompute baseline of table \%s failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified Tables Entry ID command has been 
 **  received and the recompute failed because 
 **  CFE_ES_CreateChildTask returned an error.
 **
 **  The \c table field is the table that was specified 
 **  in the command. <br>
 **  The \c returned field specifies the error 
 **  returned by CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID          73

 /** \brief <tt> 'Tables recompute baseline failed, table \%s not found' </tt>
 **  \event <tt> 'Tables recompute baseline failed, table \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified table command has been 
 **  received but specified table cannot be found in CS
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c table field is the command specified table
 **  that could not be found in the CS table.
 */
#define CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID            74

 /** \brief <tt> 'Tables recompute baseline for table \%s failed: child task in use' </tt>
 **  \event <tt> 'Tables recompute baseline for table \%s failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified table command has been 
 **  received and the recompute failed because there is 
 **  already a CS child task running.
 */
#define CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID                 75

 /** \brief <tt> 'Checksumming of table \%s is Enabled' </tt>
 **  \event <tt> 'Checksumming of table \%s is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable Table name 
 **  command is accepted.
 **
 **  The \c table field is the command specifed table
 **  to enable.
 */
#define CS_ENABLE_TABLES_NAME_INF_EID                       76

 /** \brief <tt> 'Tables enable table command failed, table \%s not found' </tt>
 **  \event <tt> 'Tables enable table command failed, table \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable Table name
 **  command is received, but has an unknown name
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c table field is the command specifed table name
 **  to enable.
 */
#define CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID               77

 /** \brief <tt> 'Checksumming of table \%s is Disabled' </tt>
 **  \event <tt> 'Checksumming of table \%s is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable table name
 **  command is accepted.
 **
 **  The \c table field is the command specifed table name
 **  to disable.
 */
#define CS_DISABLE_TABLES_NAME_INF_EID                      78

 /** \brief <tt> 'Tables disable table command failed, table \%s not found' </tt>
 **  \event <tt> 'Tables disable table command failed, table \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable able name
 **  command is received, but has an unknown name
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c table field is the command specifed table name
 **  to disable.
 */
#define CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID              79

 /**********************************************************************/
 /*APP Commands                                                        */
 /**********************************************************************/

 /** \brief <tt> 'Checksumming of Apps is Disabled' </tt>
 **  \event <tt> 'Checksumming of Apps is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when disable checksumming 
 **  for the App table command has been received. 
 */
#define CS_DISABLE_APP_INF_EID                              80

 /** \brief <tt> 'Checksumming of Apps is Enabled' </tt>
 **  \event <tt> 'Checksumming of Apps is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when enable checksumming 
 **  for the App table command has been received. 
 */
#define CS_ENABLE_APP_INF_EID                               81

 /** \brief <tt> 'Report baseline of app \%s is 0x\%08X' </tt>
 **  \event <tt> 'Report baseline of app \%s is 0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the App entry specified command has been received
 **  and there is a baseline computed to report.
 **  
 **  The \c app field is the command specified app name. <br>
 **  The \c baseline field identifies the baseline checksum
 **  of the app.
 */
#define CS_BASELINE_APP_INF_EID                             82

 /** \brief <tt> 'Report baseline of app \%s has not been computed yet' </tt>
 **  \event <tt> 'Report baseline of app \%s has not been computed yet' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified app command has been 
 **  received but the baseline has not yet been computed.
 **  
 **  The \c app field identifies the command specified app.
 */
#define CS_NO_BASELINE_APP_INF_EID                          83

 /** \brief <tt> 'App report baseline failed, app \%s not found' </tt>
 **  \event <tt> 'App report baseline failed, app \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a report baseline
 **  for the command specified app command has been 
 **  received but specified app name cannot be found
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c app field is the command specified app that 
 **  wasn't found in CS.
 */
#define CS_BASELINE_INVALID_NAME_APP_ERR_EID                84

 /** \brief <tt> 'Recompute baseline of app \%s started' </tt>
 **  \event <tt> 'Recompute baseline of app \%s started' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified app command has been received and the
 **  recompute task has been started.
 **
 **  The \c app field is the command specified app to recompute.
 */
#define CS_RECOMPUTE_APP_STARTED_DBG_EID                   85

 /** \brief <tt> 'Recompute baseline of app \%s failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  \event <tt> 'Recompute baseline of app \%s failed, CFE_ES_CreateChildTask returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the specified app command has been 
 **  received and the recompute failed because 
 **  CFE_ES_CreateChildTask returned an error.
 **
 **  The \c app field is the app that was specified 
 **  in the command. <br>
 **  The \c returned field specifies the error 
 **  returned by CFE_ES_CreateChildTask.
 */
#define CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID             86

 /** \brief <tt> 'App recompute baseline failed, app \%s not found' </tt>
 **  \event <tt> 'App recompute baseline failed, app \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified app command has been 
 **  received but specified app cannot be found in CS
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c app field is the command specified app
 **  that could not be found in the CS app table.
 */
#define CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID               87

 /** \brief <tt> 'App recompute baseline for app \%s failed: child task in use' </tt>
 **  \event <tt> 'App recompute baseline for app \%s failed: child task in use' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute baseline
 **  for the command specified app command has been 
 **  received and the recompute failed because there is 
 **  already a CS child task running.
 */
#define CS_RECOMPUTE_APP_CHDTASK_ERR_EID                    88

 /** \brief <tt> 'Checksumming of app \%s is Enabled' </tt>
 **  \event <tt> 'Checksumming of app \%s is Enabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable app name 
 **  command is accepted.
 **
 **  The \c app field is the command specifed app
 **  to enable.
 */
#define CS_ENABLE_APP_NAME_INF_EID                          89

 /** \brief <tt> 'App enable app command failed, app \%s not found' </tt>
 **  \event <tt> 'App enable app command failed, app \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an enable app name
 **  command is received, but has an unknown name
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c app field is the command specifed app name
 **  to enable.
 */
#define CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID                  90

 /** \brief <tt> 'Checksumming of app \%s is Disabled' </tt>
 **  \event <tt> 'Checksumming of app \%s is Disabled' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable app name
 **  command is accepted.
 **
 **  The \c app field is the command specifed app name
 **  to disable.
 */
#define CS_DISABLE_APP_NAME_INF_EID                         91

 /** \brief <tt> 'App disable app command failed, app \%s not found' </tt>
 **  \event <tt> 'App disable app command failed, app \%s not found' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a disable app name
 **  command is received, but has an unknown name 
 **  or is marked as #CS_STATE_EMPTY.
 **
 **  The \c app field is the command specifed app name
 **  to disable.
 */
#define CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID                 92

 /**********************************************************************/
 /* Compute Events                                                     */
 /**********************************************************************/


 /** \brief <tt> 'App table computing: App \%s could not be found, skipping' </tt>
 **  \event <tt> 'App table computing: App \%s could not be found, skipping' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an app cannot be found when checksumming.
 **
 **  The \c App field specifies the name of the app that could not be found in
 **  the system 
 */

#define CS_COMPUTE_APP_NOT_FOUND_ERR_EID                        93

 /** \brief <tt> 'Tables table computing: Table \%s could not be found, skipping' </tt>
 **  \event <tt> 'Tables table computing: Table \%s could not be found, skipping' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when an table cannot be found when checksumming.
 **
 **  The \c Table field specifies the name of the table that could not be found in
 **  the system 
 */

#define CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID                     94

 /** \brief <tt> '\%s entry \%d recompute finished. New baseline is 0X\%08X' </tt>
 **  \event <tt> '\%s entry \%d recompute finished. New baseline is 0X\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute entry for Eeprom or Memory
 **  has finished sucesessfully.
 **
 **  The \c name field specifies whether this message is for a Eeprom recompute
 **  or a Memory recompute. <br>
 **  The \c entry field specifies the Entry ID in the
 **  table that was recomputed. <br>
 **  The \c baseline field is the new baseline
 **  checksum for the specified entry.
 */
#define CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID           95

 /** \brief <tt> 'Table \%s recompute failed. Could not get address' </tt>
 **  \event <tt> 'Table \%s recompute failed. Could not get address' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute entry for Tables
 **  fails because CS cannot get the address of the table.
 **
 **  The \c Table field specifies the name of the
 **  table that was asked to recompute.
 */
#define CS_RECOMPUTE_ERROR_TABLES_ERR_EID                   96

 /** \brief <tt> 'App \%s recompute failed. Could not get address' </tt>
 **  \event <tt> 'App \%s recompute failed. Could not get address' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute entry for Apps
 **  fails because CS cannot get the address of the application.
 **
 **  The \c App field specifies the name of the
 **  application that was asked to recompute.
 */
#define CS_RECOMPUTE_ERROR_APP_ERR_EID                      97


 /** \brief <tt> 'Table \%s recompute finished. New baseline is 0X\%08X' </tt>
 **  \event <tt> 'Table \%s recompute finished. New baseline is 0X\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute entry for a Table
 **  has finished sucesessfully.
 **
 **  The \c Table field specifies the name of the
 **  table that was recomputed. <br> 
 **  The \c baseline field is the new baseline
 **  checksum for the specified entry.
 **  However, for the CS Table Definitions Table only, the checksum
 **  value will be incorrect. This is because all entries in this
 **  table are disabled while being recomputed and disabling the
 **  entry for itself modifies the contents of the table being
 **  recomputed. Thus, recomputing the CS Tables Definition Table
 **  checksum is not recommended.
 */
#define CS_RECOMPUTE_FINISH_TABLES_INF_EID                  98

 /** \brief <tt> 'App \%s recompute finished. New baseline is 0X\%08X' </tt>
 **  \event <tt> 'App \%s recompute finished. New baseline is 0X\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a recompute entry for an app
 **  has finished sucesessfully.
 **
 **  The \c App field specifies the name of the
 **  app that was recomputed. <br>
 **  The \c baseline field is the new baseline
 **  checksum for the specified entry.
 */
#define CS_RECOMPUTE_FINISH_APP_INF_EID                     99

 /** \brief <tt> 'OneShot checksum on Address: 0x\%08X, size \%d completed. Checksum =  0x\%08X' </tt>
 **  \event <tt> 'OneShot checksum on Address: 0x\%08X, size \%d completed. Checksum =  0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a OneShot command finishes
 **
 **  The \c Address field specifies the address to start the checksum. <br>
 **  The \c size field specifies the number of bytes to checksum. <br>
 **  The \c Checksum field is the checksum for the given address and size.
 */
#define CS_ONESHOT_FINISHED_INF_EID                         100

 /**********************************************************************/
 /* Table processing events                                            */
 /**********************************************************************/

 /** \brief <tt> 'Eeprom Table Validate: Illegal State Field (0x\%04X) found in Entry ID \%d' </tt>
 **  \event <tt> 'Eeprom Table Validate: Illegal State Field (0x\%04X) found in Entry ID \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the Eeprom table validation function detects an illegal state
 **
 **  The \c State field specifies the state of the entry that failed. <br>
 **  The \c Entry \c ID field specifies the entry that failed.
 */
#define CS_VAL_EEPROM_STATE_ERR_EID                         101

 /** \brief <tt> 'Eeprom Table Validate: Illegal checksum range found in Entry ID \%d, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  \event <tt> 'Eeprom Table Validate: Illegal checksum range found in Entry ID \%d, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the Eeprom table validation function detects an illegal checksum range 
 **
 **  The \c Entry \c ID field specifies the state of the entry that failed. <br>
 **  The \c returned field specifies the return code from CFE_PSP_MemValidateRange.
 */
#define CS_VAL_EEPROM_RANGE_ERR_EID                         102

 /** \brief <tt> 'Memory Table Validate: Illegal State Field (0x\%04X) found in Entry ID \%d' </tt>
 **  \event <tt> 'Memory Table Validate: Illegal State Field (0x\%04X) found in Entry ID \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the Memory table validation function detects an illegal state
 **
 **  The \c State field specifies the state of the entry that failed. <br>
 **  The \c Entry \c ID field specifies the entry that failed.
 */
#define CS_VAL_MEMORY_STATE_ERR_EID                         103

 /** \brief <tt> 'Memory Table Validate: Illegal checksum range found in Entry ID \%d, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  \event <tt> 'Memory Table Validate: Illegal checksum range found in Entry ID \%d, CFE_PSP_MemValidateRange returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the Memory table validation function detects an illegal checksum range 
 **
 **  The \c Entry \c ID field specifies the state of the entry that failed. <br>
 **  The \c returned field specifies the return code from CFE_PSP_MemValidateRange.
 */
#define CS_VAL_MEMORY_RANGE_ERR_EID                         104


 /** \brief <tt> 'Tables Table Validate: Illegal State Field (0x\%04X) found with name \%s' </tt>
 **  \event <tt> 'Tables Table Validate: Illegal State Field (0x\%04X) found with name \%s' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the Tables table validation function detects an illegal state
 **
 **  The \c State field specifies the state of the table that failed. <br>
 **  The \c name field specifies the name of the table that failed.
 */
#define CS_VAL_TABLES_STATE_ERR_EID                         105


 /** \brief <tt> 'App Table Validate: Illegal State Field (0x\%04X) found with name \%s' </tt>
 **  \event <tt> 'App Table Validate: Illegal State Field (0x\%04X) found with name \%s' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the App table validation function detects an illegal state
 **
 **  The \c state field specifies the state of the app that failed. <br>
 **  The \c name field specifies the name of the app that failed.
 */
#define CS_VAL_APP_STATE_ERR_EID                            106

 /** \brief <tt> 'CS \%s Table: No valid entries in the table' </tt>
 **  \event <tt> 'CS \%s Table: No valid entries in the table' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a new definition table has finished being processed
 **  and there are no valid entries in that table
 **
 **  The \c table field specifies whether the table was the Eeprom or Memory table
 */
#define CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID         107

 /** \brief <tt> 'CS App Table: No valid entries in the table' </tt>
 **  \event <tt> 'CS App Table: No valid entries in the table' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a new definition table has finished being processed
 **  and there are no valid entries in that table
 **
 */
#define CS_PROCESS_APP_NO_ENTRIES_INF_EID                   108

 /** \brief <tt> 'CS Tables Table: No valid entries in the table' </tt>
 **  \event <tt> 'CS Tables Table: No valid entries in the table' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when a new definition table has finished being processed
 **  and there are no valid entries in that table
 **
 */
#define CS_PROCESS_TABLES_NO_ENTRIES_INF_EID                109

 /** \brief <tt> 'CS received error 0x\%08X initializing Definition table for \%s' </tt>
 **  \event <tt> 'CS received error 0x\%08X initializing Definition table for \%s' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a table initialization failed.
 **
 **  The \c error field specifies the error that occurred. <br>
 **  The \c table filed specifies which table initialization failed.
 */
#define CS_TBL_INIT_ERR_EID                                 110

 /** \brief <tt> 'CS had problems updating table. Release:0x\%08X Manage:0x\%08X Get:0x\%08X for table \%s' </tt>
 **  \event <tt> 'CS had problems updating table. Release:0x\%08X Manage:0x\%08X Get:0x\%08X for table \%s' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when a problem occurs during a table update.
 **
 **  The \c Release field specifies the return code of the CFE_TBL_ReleaseAddress function. <br>
 **  The \c Manage field specifies the return code of the CFE_TBL_Manage function. <br>
 **  The \c Get field specifies the return code of the CFE_TBL_GetAddress function. <br>
 **  The \c table field specifies which table failed the update.
 */
#define CS_TBL_UPDATE_ERR_EID                               111



/***********************************************************************************************/

/** \brief <tt> 'Software Bus Create Pipe for command returned: 0x\%08X' </tt>
 ** \event <tt> 'Software Bus Create Pipe for command returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CFE_SB_CreatePipe fails for the command pipe
 **
 **  The \c returned field specifies the return code of CFE_SB_CreatePipe.
 */
#define CS_INIT_SB_CREATE_ERR_EID                           112

/** \brief <tt> 'Software Bus subscribe to housekeeping returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to housekeeping returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFE_SB_Subscribe failed to subscribe to the
**  housekeeping MID.
**
**  The \c returned field specifies the return code of CFE_SB_Subscribe.
*/
#define CS_INIT_SB_SUBSCRIBE_HK_ERR_EID                     113

/** \brief <tt> 'Software Bus subscribe to background cycle returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to background cycle returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFE_SB_Subscribe fails to subscribe to the
**  background cycle MID.
**
**  The \c returned field specifies the return code of CFE_SB_Subscribe.
*/
#define CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID                   114

/** \brief <tt> 'Software Bus subscribe to command returned: 0x\%08X' </tt>
**  \event <tt> 'Software Bus subscribe to command returned: 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when CFE_SB_Subscribe fails to subscribe to the 
**  command MID.
**
**  The \c returned field specifies the return code of CFE_SB_Subscribe.
*/
#define CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID                    115

/** \brief <tt> 'Table initialization failed for Eeprom: 0x\%08X, checksumming Eeprom is disabled' </tt>
**  \event <tt> 'Table initialization failed for Eeprom: 0x\%08X, checksumming Eeprom is disabled' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the table could not be initialized at startup.
**
**  The \c Result field specifies the return code of CS_TableInit.
*/
#define CS_INIT_EEPROM_ERR_EID                              116


/** \brief <tt> 'Table initialization failed for Memory: 0x\%08X, checksumming Memory is disabled' </tt>
 ** \event <tt> 'Table initialization failed for Memory: 0x\%08X, checksumming Memory is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_INIT_MEMORY_ERR_EID                              117


/** \brief <tt> 'Table initialization failed for Tables: 0x\%08X, checksumming Tables is disabled' </tt>
 ** \event <tt> 'Table initialization failed for Tables: 0x\%08X, checksumming Tables is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_INIT_TABLES_ERR_EID                              118

/** \brief <tt> 'Table initialization failed for Apps: 0x\%08X, checksumming Apps is disabled' </tt>
 ** \event <tt> 'Table initialization failed for Apps: 0x\%08X, checksumming Apps is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_INIT_APP_ERR_EID                                 119

/*************************************************************************************************/

/** \brief <tt> 'CS Tables: Could not release addresss for table \%s, returned: 0x\%08X' </tt>
 ** \event <tt> 'CS Tables: Could not release addresss for table \%s, returned: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when Table Services can't release the address for the table
 **  being checksummed.
 **
 **  The \c table field specifies the name of the table whose address can't be released. <br>
 **  The \c returned field specifies the return code of CFE_TBL_ReleaseAddress.
 */
#define CS_COMPUTE_TABLES_RELEASE_ERR_EID                   120

/** \brief <tt> 'CS Tables: Problem Getting table \%s info Share: 0x\%08X, GetInfo: 0x\%08X, GetAddress: 0x\%08X' </tt>
 ** \event <tt> 'CS Tables: Problem Getting table \%s info Share: 0x\%08X, GetInfo: 0x\%08X, GetAddress: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when there was a problem getting the table information to checksum.
 **
 **  The \c table field specifies the name of the table to be checksummed. <br>
 **  The \c Share field specifies the return code of CFE_TBL_Share. <br>
 **  The \c GetInfo field specifies the return code of CFE_TBL_GetInfo. <br>
 **  The \c GetAddress field specifies the return code of CFE_TBL_GetAddress.
 */
#define CS_COMPUTE_TABLES_ERR_EID                           121

/** \brief <tt> 'CS Apps: Problems getting app \%s info, GetAppID: 0x\%08X, GetAppInfo: 0x\%08X, AddressValid: 0x\%08X' </tt>
 ** \event <tt> 'CS Apps: Problems getting app \%s info, GetAppID: 0x\%08X, GetAppInfo: 0x\%08X, AddressValid: 0x\%08X' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when there was a problem getting the app information to checksum.
 **
 **  The \c app field specifies the name of the app to be checksummed. <br>
 **  The \c GetAppID field specifies the return code of CFE_ES_GetAppIDByName. <br>
 **  The \c GetAppInfo field specifies the return code of CFE_ES_GetAppInfo. <br>
 **  The \c AddressValid field specifies whether or not the addresses obtained from CFE_ES_GetAppInfo are valid.
 */

#define CS_COMPUTE_APP_ERR_EID                              122
/*************************************************************************************************/



/** \brief <tt> 'Table update failed for Eeprom: 0x\%08X, checksumming Eeprom is disabled' </tt>
 ** \event <tt> 'Table update failed for Eeprom: 0x\%08X, checksumming Eeprom is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_UPDATE_EEPROM_ERR_EID                              123


/** \brief <tt> 'Table update failed for Memory: 0x\%08X, checksumming Memory is disabled' </tt>
 ** \event <tt> 'Table update failed for Memory: 0x\%08X, checksumming Memory is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_UPDATE_MEMORY_ERR_EID                              124



/** \brief <tt> 'Table update failed for Tables: 0x\%08X, checksumming Tables is disabled' </tt>
 ** \event <tt> 'Table update failed for Tables: 0x\%08X, checksumming Tables is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_UPDATE_TABLES_ERR_EID                              125


/** \brief <tt> 'Table update failed for Apps: 0x\%08X, checksumming Apps is disabled' </tt>
 ** \event <tt> 'Table update failed for Apps: 0x\%08X, checksumming Apps is disabled' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when the table could not be initialized at startup.
 **
 **  The \c Result field specifies the return code of CS_TableInit.
 */
#define CS_UPDATE_APP_ERR_EID                                 126



/** \brief <tt> 'OS Text Segment disabled due to platform' </tt>
 ** \event <tt> 'OS Text Segment disabled due to platform' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when CS is running on a platform that does not support
 **  getting the information of the OS text segment, such as OS X and Linux.
 **
 */
#define CS_OS_TEXT_SEG_INF_EID                                127

/** \brief <tt> 'CS cannot get a valid address for \%s, due to the platform' </tt>
 ** \event <tt> 'CS cannot get a valid address for \%s, due to the platform' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS is running on a platform that does not support
 **  getting the address information of the applications, such as OS X and Linux.
 **
 */
#define CS_COMPUTE_APP_PLATFORM_DBG_EID                                128

/** \brief <tt> 'CS unable to update tables definition table for entry \%s' </tt>
 ** \event <tt> 'CS unable to update tables definition table for entry \%s' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully enables an entry (specified
 **  by name) in the Tables results table but is unable to find the same entry
 **  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 **
 */
#define CS_ENABLE_TABLE_DEF_NOT_FOUND_DBG_EID                          129

/** \brief <tt> 'CS unable to update tables definition table for entry \%s' </tt>
 ** \event <tt> 'CS unable to update tables definition table for entry \%s' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully disables an entry (specified
 **  by name) in the Tables results table but is unable to find the same entry
 **  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 **
 */
#define CS_DISABLE_TABLE_DEF_NOT_FOUND_DBG_EID                          130

/** \brief <tt> 'CS unable to update apps definition table for entry \%s' </tt>
 ** \event <tt> 'CS unable to update apps definition table for entry \%s' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully enables an entry (specified
 **  by name) in the Apps results table but is unable to find the same entry
 **  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 **
 */
#define CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID                          131

/** \brief <tt> 'CS unable to update apps definition table for entry \%s' </tt>
 ** \event <tt> 'CS unable to update apps definition table for entry \%s' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully disables an entry (specified
 **  by name) in the Apps results table but is unable to find the same entry
 **  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 **
 */
#define CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID                          132

/** \brief <tt> 'CS unable to update memory definition table for entry \%d, State: \%d' </tt>
 ** \event <tt> 'CS unable to update memory definition table for entry \%d, State: \%d' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully disables an entry (specified
 **  by name) in the Memory results table but identifies the corresponding entry in
 **  the definitions table to be set to #CS_STATE_EMPTY.
 **
 */
#define CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID                          133

/** \brief <tt> 'CS unable to update memory definition table for entry \%d, State: \%d' </tt>
 ** \event <tt> 'CS unable to update memory definition table for entry \%d, State: \%d' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully enables an entry (specified
 **  by name) in the Memory results table but identifies the corresponding entry in
 **  the definitions table to be set to #CS_STATE_EMPTY.
 **
 */
#define CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID                          134

/** \brief <tt> 'CS unable to update Eeprom definition table for entry \%d, State: \%d' </tt>
 ** \event <tt> 'CS unable to update Eeprom definition table for entry \%d, State: \%d' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully disables an entry (specified
 **  by name) in the Eeprom results table but identifies the corresponding entry in
 **  the definitions table to be set to #CS_STATE_EMPTY.
 **
 */
#define CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID                          135

/** \brief <tt> 'CS unable to update Eeprom definition table for entry \%d, State: \%d' </tt>
 ** \event <tt> 'CS unable to update Eeprom definition table for entry \%d, State: \%d' </tt>
 **  
 **  \par Type: DEBUG
 **
 **  \par Cause:
 **
 **  This event message is issued when CS successfully enables an entry (specified
 **  by name) in the Eeprom results table but identifies the corresponding entry in
 **  the definitions table to be set to #CS_STATE_EMPTY.
 **
 */
#define CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID                          136

/** \brief <tt> 'CS Tables Table Validate: Duplicate Name (\%s) found at entries \%d and \%d' </tt>
 ** \event <tt> 'CS Tables Table Validate: Duplicate Name (\%s) found at entries \%d and \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CS validation for the Tables definition table finds more
 **  than one entry with the same table name.  Only one entry per table is allowed.  This event
 **  is only issued if it is the first error found during validation.
 **
 */
#define CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID                          137

/** \brief <tt> 'CS Tables Table Validate: Illegal State (0x\%04X) with empty name at entry \%d' </tt>
 ** \event <tt> 'CS Tables Table Validate: Illegal State (0x\%04X) with empty name at entry \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CS validation for the Tables definition table finds an entry
 **  that contains a zero-length name field with a state field that is not marked as #CS_STATE_EMPTY.  This
 **  event is only issued if it is the first error found during validation.
 **
 */
#define CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID                      138

/** \brief <tt> 'CS Tables Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 ** \event <tt> 'CS Tables Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message when CS completes validation of the tables definition table.  This message
 **  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of 
 **  bad entries (due to invalid state definitions or duplicate names), and the number of entries 
 **  marked as #CS_STATE_EMPTY.
 **
 */
#define CS_VAL_TABLES_INF_EID                                        139

/** \brief <tt> 'CS Apps Table Validate: Duplicate Name (\%s) found at entries \%d and \%d' </tt>
 ** \event <tt> 'CS Apps Table Validate: Duplicate Name (\%s) found at entries \%d and \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CS validation for the Apps definition table finds more
 **  than one entry with the same table name.  Only one entry per table is allowed.  This event
 **  is only issued if it is the first error found during validation.
 **
 */
#define CS_VAL_APP_DEF_TBL_DUPL_ERR_EID                              140

/** \brief <tt> 'CS Apps Table Validate: Illegal State (0x\%04X) with empty name at entry \%d' </tt>
 ** \event <tt> 'CS Apps Table Validate: Illegal State (0x\%04X) with empty name at entry \%d' </tt>
 **  
 **  \par Type: ERROR
 **
 **  \par Cause:
 **
 **  This event message is issued when CS validation for the Apps definition table finds an entry
 **  that contains a zero-length name field with a state field that is not marked as #CS_STATE_EMPTY.  This
 **  event is only issued if it is the first error found during validation.
 **
 */
#define CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID                         141

/** \brief <tt> 'CS Apps Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 ** \event <tt> 'CS Apps Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message when CS completes validation of the Apps definition table.  This message
 **  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of 
 **  bad entries (due to invalid state definitions or duplicate names), and the number of entries 
 **  marked as #CS_STATE_EMPTY.
 **
 */
#define CS_VAL_APP_INF_EID                                            142

/** \brief <tt> 'CS Memory Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 ** \event <tt> 'CS Memory Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message when CS completes validation of the Memory definition table.  This message
 **  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of 
 **  bad entries (due to invalid state definitions or bad range), and the number of entries 
 **  marked as #CS_STATE_EMPTY.
 **
 */
#define CS_VAL_MEMORY_INF_EID                                         143

/** \brief <tt> 'CS Eeprom Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 ** \event <tt> 'CS Eeprom Table verification results: good = \%d, bad = \%d, unused = \%d' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message when CS completes validation of the Eeprom definition table.  This message
 **  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of 
 **  bad entries (due to invalid state definitions or bad range), and the number of entries 
 **  marked as #CS_STATE_EMPTY.
 **
 */
#define CS_VAL_EEPROM_INF_EID                                         144

/**
**  \brief <tt> 'Critical Data Store Access Error' </tt>
**
**  \event <tt> 'Critical Data Store access error = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The CS application optionally stores the table states
**  in the Critical Data Store (CDS).  This ensures that CS
**  will not overwrite old data storage files following a processor reset.
**  This event indicates an error at startup as CS is initializing access
**  to the Critical Data Store.  Subsequent CDS errors are ignored by CS.
*/
#define CS_INIT_CDS_ERR_EID 145


 /** \brief <tt> 'App terminating, RunStatus:0x\%08X' </tt>
 **  \event <tt> 'App terminating, RunStatus:0x\%08X' </tt>
 **  
 **  \par Type: INFORMATION
 **
 **  \par Cause:
 **
 **  This event message is issued when CS has exited without error 
 **  or exception from its main loop
 **
 **  The \c RunStatus field specifies the reason for CS to 
 **  stop execution.
 */
#define CS_EXIT_INF_EID                                     146

#endif /* _cs_events_ */

 /************************/
/*  End of File Comment */
 /************************/
