/*
**
**  $Id: cfe_tbl_events.h 1.9 2014/08/19 13:38:15GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**  Title:   Table Services API Event ID Header File
**
**  Purpose:
**     Identifies event codes for event messages issued by Table Services.
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  Notes:
**
**  $Log: cfe_tbl_events.h  $
**  Revision 1.9 2014/08/19 13:38:15GMT-05:00 sstrege 
**  Fixed doxygen warning
**  Revision 1.8 2011/12/28 14:00:18EST lwalling 
**  Add definition for CFE_TBL_SPACECRAFT_ID_ERR_EID and CFE_TBL_PROCESSOR_ID_ERR_EID
**  Revision 1.7 2011/11/14 17:58:05EST lwalling 
**  Event EID mentioned in previous log entry should have been CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID
**  Revision 1.6 2011/11/14 17:42:30EST lwalling 
**  Modified event text and argument list for CFE_TBL_FILE_INCOMPLETE_ERR_EID
**  Revision 1.5 2010/10/27 13:53:54EDT dkobe 
**  Added error event for Notification Message send failure
**  Revision 1.4 2010/10/25 15:01:12EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.3 2010/10/04 15:24:56EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2009/05/01 12:41:44EDT dkobe 
**  Corrected a few doxygen comments
**  Revision 1.1 2008/04/17 08:05:24EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
**  Revision 1.28 2007/07/11 11:51:44EDT David Kobe (dlkobe) 
**  Corrected erroneously assigned Event ID for CFE_TBL_ASSUMED_VALID_INF_EID
**  Revision 1.27 2007/07/07 09:22:29EDT dlkobe 
**  Added event (CFE_TBL_LOAD_PENDING_ERR_EID) for attempt to load pending load table
**  Revision 1.26 2007/07/07 07:53:35EDT dlkobe 
**  Added CFE_TBL_ASSUMED_VALID_INF_EID event message
**  Revision 1.25 2007/06/02 10:10:12EDT dlkobe 
**  Added doxygen comments for User's Guides
**  Revision 1.24 2007/05/25 16:24:50EDT dlkobe 
**  Continued updating doxygen comments
**  Revision 1.23 2007/05/23 11:21:58EDT dlkobe 
**  Added doxygen formatting
**  Revision 1.22 2007/05/15 11:18:08EDT rjmcgraw 
**  DCR78:11 Removed event CFE_TBL_EXIT_ERR_EID
**  Revision 1.21 2007/05/04 15:54:39EDT dlkobe 
**  Added event messages associated with deleting a critical table's CDS
**  Revision 1.20 2007/03/16 14:59:41EST dlkobe 
**  Using file version 1.14 and applying all changes since to avoid corrupted versions.
**
**
**/

#ifndef _cfe_tbl_events_
#define _cfe_tbl_events_

/******************* Macro Definitions ***********************/
/*
** Event message ID's
*/

/** \name Informational Event Message IDs */
/** \{ */
/** \brief <tt> 'Task Initialized' </tt>
**  \event <tt> 'Task Initialized' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Table Services
**  Task completes its Initialization.
**/
#define CFE_TBL_INIT_INF_EID                    1  

/** \name Command Response Informational Event Message IDs */
/** \{ */

/** \brief <tt> 'No-op command' </tt>
**  \event <tt> 'No-op command' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Table Services \link #CFE_TBL_NOOP_CC NO-OP command \endlink
**/
#define CFE_TBL_NOOP_INF_EID                   10
  
/** \brief <tt> 'Reset Counters command' </tt>
**  \event <tt> 'Reset Counters command' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Table Services \link #CFE_TBL_RESET_CC Reset Counters command \endlink
**/
#define CFE_TBL_RESET_INF_EID                  11
  
/** \brief <tt> 'Successful load of '\%s' into '\%s' working buffer' </tt>
**  \event <tt> 'Successful load of '\%s' into '\%s' working buffer' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always generated after a successful execution of 
**  a cFE Table Services \link #CFE_TBL_LOAD_CC Load Table command \endlink
**/
#define CFE_TBL_FILE_LOADED_INF_EID            12
  
/** \brief <tt> 'Successfully overwrote '\%s' with Table '\%s'' </tt>
**  \event <tt> 'Successfully overwrote '\%s' with Table '\%s'' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always generated after a successful execution of 
**  a cFE Table Services \link #CFE_TBL_DUMP_CC Dump Table command \endlink where
**  the command specified target filename was the same as a file already present
**  in the onboard filesystem. If the specified file did not exist, the event
**  message would have been #CFE_TBL_WRITE_DUMP_INF_EID.
**/
#define CFE_TBL_OVERWRITE_DUMP_INF_EID         13
  
/** \brief <tt> 'Successfully dumped Table '\%s' to '\%s'' </tt>
**  \event <tt> 'Successfully dumped Table '\%s' to '\%s'' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always generated after a successful execution of 
**  a cFE Table Services \link #CFE_TBL_DUMP_CC Dump Table command \endlink where
**  the command specified target filename was a currently non-existent file. If
**  the file did already exist, the event message would have been 
**  #CFE_TBL_OVERWRITE_DUMP_INF_EID.
**/
#define CFE_TBL_WRITE_DUMP_INF_EID             14
  
/** \brief <tt> 'Successfully overwrote '\%s' with Table Registry' </tt>
**  \event <tt> 'Successfully overwrote '\%s' with Table Registry' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is always generated after a successful execution of 
**  a cFE Table Services \link #CFE_TBL_DUMP_REG_CC Dump Table Registry command \endlink where
**  the command specified target filename was the same as a file already present
**  in the onboard filesystem. If the specified file did not exist, the event
**  message would have been #CFE_TBL_WRITE_REG_DUMP_INF_EID.
**/
#define CFE_TBL_OVERWRITE_REG_DUMP_INF_EID     15
  
/** \brief <tt> 'Tbl Services issued validation request for '\%s'' </tt>
**  \event <tt> 'Tbl Services issued validation request for '\%s'' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful execution of 
**  a cFE Table Services \link #CFE_TBL_VALIDATE_CC Validate Table command \endlink.
**  It should be noted, however, that this Event Message does <em> NOT </em> indicate completion of
**  the Table Validation.  It is <em> ONLY </em> indicating that the appropriate flag has been
**  set to <em> NOTIFY </em> the table's owning Application that a Validation has been requested.
**  Completion of the Validation is indicated by either the #CFE_TBL_VALIDATION_INF_EID or
**  #CFE_TBL_VALIDATION_ERR_EID event messages.
**/
#define CFE_TBL_VAL_REQ_MADE_INF_EID           16
  
/** \brief <tt> 'Tbl Services notifying App that '\%s' has a load pending' </tt>
**  \event <tt> 'Tbl Services notifying App that '\%s' has a load pending' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful execution of 
**  a cFE Table Services \link #CFE_TBL_ACTIVATE_CC Activate Table command \endlink.
**  It should be noted, however, that this Event Message does <em> NOT </em> indicate completion of
**  the Table Activation.  It is <em> ONLY </em> indicating that the appropriate flag has been
**  set to <em> NOTIFY </em> the table's owning Application that an Update has been requested.
**  Completion of the Update is indicated by either the #CFE_TBL_UPDATE_SUCCESS_INF_EID or
**  #CFE_TBL_UPDATE_ERR_EID event messages.
**/
#define CFE_TBL_LOAD_PEND_REQ_INF_EID          17
  
/** \brief <tt> 'Table Registry entry for '\%s' will be telemetered' </tt>
**  \event <tt> 'Table Registry entry for '\%s' will be telemetered' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful execution of 
**  a cFE Table Services \link #CFE_TBL_TLM_REG_CC Telemeter Table Registry Entry command \endlink.
**  Subsequent Table Services Housekeeping Telemetry should contain the desired Table Registry Entry data.
**/
#define CFE_TBL_TLM_REG_CMD_INF_EID            18
  
/** \brief <tt> 'Table Load Aborted for '\%s'' </tt>
**  \event <tt> 'Table Load Aborted for '\%s'' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated upon successful execution of 
**  a cFE Table Services \link #CFE_TBL_ABORT_LOAD_CC Abort Table Load command \endlink.
**/
#define CFE_TBL_LOAD_ABORT_INF_EID             21  
  
/** \brief <tt> 'Successfully dumped Table Registry to '\%s':Size=\%d,Entries=\%d' </tt>
**  \event <tt> 'Successfully dumped Table Registry to '\%s':Size=\%d,Entries=\%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is always generated after a successful execution of 
**  a cFE Table Services \link #CFE_TBL_DUMP_REG_CC Dump Table Registry command \endlink where
**  the command specified target filename was a currently non-existent file. If
**  the file did already exist, the event message would have been 
**  #CFE_TBL_OVERWRITE_REG_DUMP_INF_EID.
**/
#define CFE_TBL_WRITE_REG_DUMP_INF_EID         22

/** \brief <tt> 'Tbl Services assumes '\%s' is valid. No Validation Function has been registered' </tt>
**  \event <tt> 'Tbl Services assumes '\%s' is valid. No Validation Function has been registered' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when Table Services has received a Validation Command
**  for a table that never specified a Validation Function when it was registered via the
**  #CFE_TBL_Register API.
**/
#define CFE_TBL_ASSUMED_VALID_INF_EID          23  
/** \} */


/** \name Command Error Event Message IDs */
/** \{ */
/** \brief <tt> 'Invalid message ID -- ID = 0x\%X' </tt>
**  \event <tt> 'Invalid message ID -- ID = 0x\%X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message has arrived on
**  the cFE Table Services Application's Message Pipe that has a
**  Message ID that is neither #CFE_TBL_SEND_HK_MID or #CFE_TBL_CMD_MID.
**  Most likely, the cFE Software Bus routing table has become corrupt
**  and is sending messages targeted for other Applications to the cFE
**  Table Services Application.  
**
**  The \c ID field in the event message identifies
**  the message ID (in hex) that was found in the message.
**/
#define CFE_TBL_MID_ERR_EID                    50
  
/** \brief <tt> 'Invalid command code -- ID = 0x\%X, CC = \%d' </tt>
**  \event <tt> 'Invalid command code -- ID = 0x\%X, CC = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_TBL_CMD_MID
**  message ID has arrived but whose Command Code is not one of the command
**  codes specified in \link #CFE_TBL_NOOP_CC cfe_tbl_msg.h \endlink.  This
**  problem is most likely to occur when:
**    -# A Message ID meant for another Application became corrupted and was
**       set equal to #CFE_TBL_CMD_MID.
**    -# The Command Code field in the Message became corrupted.
**    -# The command database at the ground station has been corrupted.
**
**  The \c ID field in the event message specifies the Message ID (in hex) and the 
**  \c CC field specifies the Command Code (in decimal) found in the message. 
**/
#define CFE_TBL_CC1_ERR_EID                    51
  
/** \brief <tt> 'Invalid cmd pkt -- ID = 0x\%X,  CC = \%d, Len = \%d' </tt>
**  \event <tt> 'Invalid cmd pkt -- ID = 0x\%X,  CC = \%d, Len = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_TBL_CMD_MID
**  message ID has arrived but whose packet length does not match the expected
**  length for the specified command code.
**
**  The \c ID field in the event message specifies the Message ID (in hex), the \c CC field
**  specifies the Command Code (in decimal) and \c Len specifies the message Length (in decimal)
**  found in the message. 
**/
#define CFE_TBL_LEN_ERR_EID                    52
  
/** \brief <tt> 'Unable to open file '\%s' for table load, Status = 0x\%08X' </tt>
**  \event <tt> 'Unable to open file '\%s' for table load, Status = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of a 
**  \link #CFE_TBL_LOAD_CC Load Table command \endlink when
**  the specified file containing the table image to be loaded
**  cannot be opened.  Possible causes for this are:
**
**  -# The filename was misspelled
**  -# The path to the file was incorrect
**  -# The length of the filename and/or path exceeds the 
**     allowable length (see #OS_MAX_PATH_LEN and #OS_MAX_FILE_NAME, respectively)
**
**  The \c Status field in the event message indicates the error code returned by the #OS_open
**  API. 
**/
#define CFE_TBL_FILE_ACCESS_ERR_EID            53
  
/** \brief <tt> 'Unable to read std header for '\%s', Status = 0x\%08X' </tt>
**  \event <tt> 'Unable to read std header for '\%s', Status = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a read failure occurs during the reading of the
**  \link #CFE_FS_Header_t cFE Standard File Header \endlink of a table image file specified 
**  either by an Application calling the #CFE_TBL_Load API or in response to a command to 
**  Table Services requesting a table image file be loaded into an inactive buffer.
**
**  The \c Status field of the event message contains the error code returned by #CFE_FS_ReadHeader. 
**/
#define CFE_TBL_FILE_STD_HDR_ERR_EID           54
  
/** \brief <tt> 'Unable to read tbl header for '\%s', Status = 0x\%08X' </tt>
**  \event <tt> 'Unable to read tbl header for '\%s', Status = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a read failure occurs during the reading of the
**  \link #CFE_TBL_File_Hdr_t cFE Table File Secondary Header \endlink of a table image file specified 
**  either by an Application calling the #CFE_TBL_Load API or in response to a command to 
**  Table Services requesting a table image file be loaded into an inactive buffer.
**
**  The \c Status field of the event message contains the error code returned by #OS_read. 
**/
#define CFE_TBL_FILE_TBL_HDR_ERR_EID           55  
  
/** \brief <tt> 'Unable to send Hk Packet (Status=0x\%08X)' </tt>
**  \event <tt> 'Unable to send Hk Packet (Status=0x\%08X)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when failure occurs while attempting to send the
**  Housekeeping Message over the Software Bus.
**
**  The \c Status field of the event message contains the error code returned by #CFE_SB_SendMsg. 
**/
#define CFE_TBL_FAIL_HK_SEND_ERR_EID           56  

/** \brief <tt> 'Unable to locate '\%s' in Table Registry' </tt>
**  \event <tt> 'Unable to locate '\%s' in Table Registry' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a command that specifies a table name
**  has a table name that is not found in the Table Registry.  Most likely causes
**  for this are:
**    -# Table name was misspelled in the command.
**    -# The Application that Registered the Table has either failed to run or
**       has been terminated thus removing the Table from the Registry.
**    -# The Table Registry has become corrupted.
**/
#define CFE_TBL_NO_SUCH_TABLE_ERR_EID          57  

/** \brief <tt> 'File '\%s' is not a cFE file type, ContentType = 0x\%08X' </tt>
**  \event <tt> 'File '\%s' is not a cFE file type, ContentType = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either an Application calls the #CFE_TBL_Load API
**  or a Table Load command has been received and the specified file has a \link #CFE_FS_Header_t
**  cFE Standard File Header \endlink whose \link #CFE_FS_Header_t::ContentType Content Type \endlink 
**  is not equal to the expected #CFE_FS_FILE_CONTENT_ID. Most likely causes for this are:
**   -# The specified file is not a cFE compatible file.
**   -# The specified file has been created with bad "endianess" (headers should always conform to
**      a big endian format).
**   -# The specified file has become corrupted.
**
**  The \c ContentType field specified in the event message contains the content type that was found
**  in the specified file.
**/
#define CFE_TBL_FILE_TYPE_ERR_EID              58  

/** \brief <tt> 'File subtype for '\%s' is wrong. Subtype = 0x\%08X' </tt>
**  \event <tt> 'File subtype for '\%s' is wrong. Subtype = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either an Application calls the #CFE_TBL_Load API
**  or a Table Load command has been received and the specified file has a \link #CFE_FS_Header_t
**  cFE Standard File Header \endlink whose \link #CFE_FS_Header_t::SubType Sub Type \endlink 
**  is not equal to the expected #CFE_FS_TBL_IMG_SUBTYPE. Most likely causes for this are:
**   -# The specified file is not a cFE table image file.
**   -# The specified file has been created with bad "endianess" (headers should always conform to
**      a big endian format).
**   -# The specified file has become corrupted.
**
**  The \c SubType field specified in the event message contains the sub type that was found
**  in the specified file.
**/
#define CFE_TBL_FILE_SUBTYPE_ERR_EID           59  

/** \brief <tt> 'No working buffers available for table '\%s'' </tt>
**  \event <tt> 'No working buffers available for table '\%s'' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either a Table Load Command for a Single Buffered Table
**  or a Table Dump Command for a Dump Only Table has been sent AND there are no Shared Buffers
**  available to hold either the load image or the dump image.  To free a Shared Buffer, either 
**  a previously loaded table image must be activated or aborted OR the operator has to wait for
**  previously dumped Dump Only tables have had a chance to be written to a file (which occurs
**  whenever the cFE Table Services receives a Housekeeping Request).
**/
#define CFE_TBL_NO_WORK_BUFFERS_ERR_EID        60  

/** \brief <tt> 'Internal Error (Status=0x\%08X)' </tt>
**  \event <tt> 'Internal Error (Status=0x\%08X)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Load command was issued and the cFE Table Services
**  is unable to allocate a working table buffer for an unexpected reason.
**
**  The \c Status field provides the return status from the function that was to provide a working buffer.
**/
#define CFE_TBL_INTERNAL_ERROR_ERR_EID         61  

/** \brief <tt> 'Error creating dump file '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error creating dump file '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump or Table Registry Dump command was 
**  received and the cFE Table Services is unable to create the specified file.
**
**  The \c Status field provides the return status from the #OS_creat function call.
**/
#define CFE_TBL_CREATING_DUMP_FILE_ERR_EID     62  

/** \brief <tt> 'Error writing cFE File Header to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing cFE File Header to '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump or Table Registry Dump command was 
**  received and the cFE Table Services is unable to write the standard cFE File Header
**  to the specified file.
**
**  The \c Status field provides the return status from the #CFE_FS_WriteHeader function call.
**/
#define CFE_TBL_WRITE_CFE_HDR_ERR_EID          63  

/** \brief <tt> 'Error writing Tbl image File Header to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing Tbl image File Header to '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump command was received and the 
**  cFE Table Services is unable to write the standard cFE Table Image Header to the specified file.
**
**  The \c Status field provides the return status from the #OS_write function call.
**/
#define CFE_TBL_WRITE_TBL_HDR_ERR_EID          64  

/** \brief <tt> 'Error writing Tbl image to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing Tbl image to '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump command was received and the 
**  cFE Table Services is unable to write the contents of the specified Table image to 
**  the specified file.
**
**  The \c Status field provides the return status from the #OS_write function call.
**/
#define CFE_TBL_WRITE_TBL_IMG_ERR_EID          65  

/** \brief <tt> 'No Inactive Buffer for Table '\%s' present' </tt>
**  \event <tt> 'No Inactive Buffer for Table '\%s' present' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump or a Table Validate command for an
**  Inactive Table Buffer was received and there isn't an Inactive Table Buffer associated
**  with the specified Table.
**/
#define CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID     66  

/** \brief <tt> 'Too many Table Validations have been requested' </tt>
**  \event <tt> 'Too many Table Validations have been requested' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Validate command was received and there are
**  no more free Validation Result Blocks available.  The number of simultaneous validations that
**  can be pending is specified by the configuration parameter #CFE_TBL_MAX_NUM_VALIDATIONS
**  which is found in the cfe_platform_cfg.h file.
**
**  Validation Commands lock one of the Validation Result Blocks upon receipt of the validation
**  command until the result of the Validation, performed by the table's owning Application, has
**  been reported in a Table Services Housekeeping Request Message.
**/
#define CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID   67  

/** \brief <tt> 'Error writing Registry to '\%s', Status=0x\%08X' </tt>
**  \event <tt> 'Error writing Registry to '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Registry Dump command was received and the 
**  cFE Table Services is unable to write the entire contents of the Table Registry to 
**  the specified file.
**
**  The \c Status field provides the return status from the #OS_write function call.
**/
#define CFE_TBL_WRITE_TBL_REG_ERR_EID          68  

/** \brief <tt> 'Cannot abort load of '\%s'. No load started.' </tt>
**  \event <tt> 'Cannot abort load of '\%s'. No load started.' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Abort Load command is received and the command specified
**  table is not currently in the process of being loaded.
**/
#define CFE_TBL_LOAD_ABORT_ERR_EID             69  

/** \brief <tt> 'Cannot activate table '\%s'. No Inactive image available' </tt>
**  \event <tt> 'Cannot activate table '\%s'. No Inactive image available' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Activate Table command is received and the command specified
**  table does not currently have an inactive buffer associated with it.
**/
#define CFE_TBL_ACTIVATE_ERR_EID               70  

/** \brief <tt> 'Incomplete load of '\%s' into '\%s' working buffer' </tt>
**  \event <tt> 'Incomplete load of '\%s' into '\%s' working buffer' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Load Table command is received and the Table Services
**  is unable to load the number of bytes specified in the Table Image Header of the command
**  specified file from the file into the Inactive Buffer.
**/
#define CFE_TBL_FILE_INCOMPLETE_ERR_EID        71  

/** \brief <tt> 'Cannot load '\%s' (\%d) at offset \%d in '\%s' (\%d)' </tt>
**  \event <tt> 'Cannot load '\%s' (\%d) at offset \%d in '\%s' (\%d)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Load Table command is received and the Table Header in
**  the specified Table Image file identifies a number of bytes with a specified starting offset
**  that would exceed the size of the specified table.  For example, if a table had 10 bytes and
**  the Table Header indicated that the Table Image in the file contains 7 bytes that starts at
**  offset 5, then the data content would have exceeded the 10 byte limit of the table.
**
**  The numbers in parenthesis in the event message text indicate the data size (in bytes) for
**  the specified load file and the registered size for the specified table.
**/
#define CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID      72  

/** \brief <tt> 'Table Hdr in '\%s' indicates no data in file' </tt>
**  \event <tt> 'Table Hdr in '\%s' indicates no data in file' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Load Table command is received and the Table Header in
**  the specified Table Image file claims the file contains no data.
**/
#define CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID       73  

/** \brief <tt> ''\%s' has partial load for uninitialized table '\%s'' </tt>
**  \event <tt> ''\%s' has partial load for uninitialized table '\%s'' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Load Table command is received and the Table Header in
**  the specified Table Image file indicates the starting offset for the table is non-zero and the
**  table has never been previously, completely loaded.  Partial Table loads are only allowed after
**  the table has had a successful load.
**/
#define CFE_TBL_PARTIAL_LOAD_ERR_EID           74

/** \brief <tt> 'File '\%s' has more data than Tbl Hdr indicates (\%d)' </tt>
**  \event <tt> 'File '\%s' has more data than Tbl Hdr indicates (\%d)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Load Table command is received and Table Services is
**  able to locate more data in the specified Table Image file than the Table Header claims is present.
**/
#define CFE_TBL_FILE_TOO_BIG_ERR_EID           75  

/** \brief <tt> 'Too many Dump Only Table Dumps have been requested' </tt>
**  \event <tt> 'Too many Dump Only Table Dumps have been requested' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump command for a Dump-Only Table was received and there are
**  no more free Dump Only Control Blocks available.  The number of simultaneous Dump Only Tables that
**  can be pending is specified by the configuration parameter  #CFE_TBL_MAX_SIMULTANEOUS_LOADS which 
**  is found in the cfe_platform_cfg.h file.
**/
#define CFE_TBL_TOO_MANY_DUMPS_ERR_EID         76  

/** \brief <tt> 'A dump for '\%s' is already pending' </tt>
**  \event <tt> 'A dump for '\%s' is already pending' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Dump command for a Dump-Only Table was received and 
**  Table Services hasn't finished processing the previous Table Dump command for the same Table.
**/
#define CFE_TBL_DUMP_PENDING_ERR_EID           77  

/** \brief <tt> 'Illegal attempt to activate dump-only table '\%s'' </tt>
**  \event <tt> 'Illegal attempt to activate dump-only table '\%s'' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Activate command for a Dump-Only Table was received.
**  By definition, Dump-Only tables are not allowed to be loaded with any new data.
**/
#define CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID     78

/** \brief <tt> 'Attempted to load DUMP-ONLY table '\%s' from '\%s'' </tt>
**  \event <tt> 'Attempted to load DUMP-ONLY table '\%s' from '\%s'' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Load command for a Dump-Only Table was received.
**  By definition, Dump-Only tables are not allowed to be loaded with any new data.
**/
#define CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID    79

/** \brief <tt> 'Cmd for Table '\%s' had illegal buffer parameter (0x\%08X)' </tt>
**  \event <tt> 'Cmd for Table '\%s' had illegal buffer parameter (0x\%08X)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either a Table Validate command or a Table Dump Command
**  contains a buffer identifier that does not equal either of the valid values 
**  (see #CFE_TBL_DumpCmd_t::ActiveTblFlag or #CFE_TBL_ValidateCmd_t::ActiveTblFlag)
**
**  The parameter in the Event Message indicates (in hex) the value found for the ActiveTblFlag in the command.
**/
#define CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID     80

/** \brief <tt> 'Cannot activate table '\%s'. Inactive image not Validated' </tt>
**  \event <tt> 'Cannot activate table '\%s'. Inactive image not Validated' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Activate command is received specifying
**  a Table Image that has not been Validated.  If a table has a validation function associated with it
**  (as defined by the owning Application when the Table is first Registered), then the Inactive Image
**  MUST be successfully Validated prior to Activation.
**/
#define CFE_TBL_UNVALIDATED_ERR_EID            81

/** \brief <tt> ''\%s' found in Table Registry. CDS cannot be deleted until table is unregistered' </tt>
**  \event <tt> ''\%s' found in Table Registry. CDS cannot be deleted until table is unregistered' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Delete Critical Data Store command is received specifying
**  a Table Image that is still registered.  Critical Table Images cannot be removed from the CDS until the
**  table is first removed from the Registry.  Unload the owning application and try again.
**/
#define CFE_TBL_IN_REGISTRY_ERR_EID            82

/** \brief <tt> 'Table '\%s' is in Critical Table Registry but CDS is not tagged as a table' </tt>
**  \event <tt> 'Table '\%s' is in Critical Table Registry but CDS is not tagged as a table' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Delete Critical Data Store command is received specifying
**  a CDS name for a Critical Data Store that is NOT a critical table image.  To delete CDSs that are not
**  Critical Table Images, the Executive Services command #CFE_ES_DELETE_CDS_CC must be used.
**/
#define CFE_TBL_NOT_CRITICAL_TBL_ERR_EID       83

/** \brief <tt> 'Table '\%s' is not found in Critical Table Registry' </tt>
**  \event <tt> 'Table '\%s' is not found in Critical Table Registry' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Delete Critical Data Store command is received specifying
**  a table name that cannot be found in the Critical Table Registry.  If a Critical Data Store exists with
**  the specified name, then the Critical Table Registry has somehow gotten out of sync with the CDS.
**  Otherwise, the likely cause of this error is a misspelled table name in the command. 
**/
#define CFE_TBL_NOT_IN_CRIT_REG_ERR_EID        84

/** \brief <tt> 'Unable to locate '\%s' in CDS Registry' </tt>
**  \event <tt> 'Unable to locate '\%s' in CDS Registry' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a Table Delete Critical Data Store command is received specifying
**  a table name that WAS found in the Critical Table Registry but its associated entry in the Critical Data
**  Store Registry was not found.  Somehow the two entities have become out of synch. 
**/
#define CFE_TBL_CDS_NOT_FOUND_ERR_EID          85

/** \brief <tt> 'Error while deleting '\%s' from CDS, See SysLog.(Err=0x\%08X)' </tt>
**  \event <tt> 'Error while deleting '\%s' from CDS, See SysLog.(Err=0x\%08X)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an unexpected error was encountered during the deletion of the CDS.
**  The System Log should have more precise information on the nature of the error. 
**/
#define CFE_TBL_CDS_DELETE_ERR_EID             86

/** \brief <tt> 'CDS '\%s' owning app is still active' </tt>
**  \event <tt> 'CDS '\%s' owning app is still active' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an attempt is made to delete a CDS while an application with the
**  same name as the CDS Prefix is still registered in the system.  Owning applications must not be active
**  before an associated CDS can be deleted. 
**/
#define CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID       87

/** \brief <tt> 'Attempted to load table '\%s' while previous load is still pending' </tt>
**  \event <tt> 'Attempted to load table '\%s' while previous load is still pending' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an attempt is made to load a table while a previous load is still
**  pending.  The most likely cause of this is the owning application is waiting for an appropriate time to
**  load the table with the specified contents.  In order to override this load, the user would be required
**  to issue the \link #CFE_TBL_ABORT_LOAD_CC Abort Load Command \endlink. 
**/
#define CFE_TBL_LOADING_PENDING_ERR_EID        88

/** \brief <tt> 'Manage Notification Pkt Error(MsgId=0x\%08X, CC=0x\%04X, Param=0x\%08X, Status=0x\%08X)' </tt>
**  \event <tt> 'Manage Notification Pkt Error(MsgId=0x\%08X, CC=0x\%04X, Param=0x\%08X, Status=0x\%08X)' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a table management notification message
**  fails to be sent via the software bus.
**
**  The \c MsgId is the message ID of the table management notification message that was attempted to be sent,
**  the \c CC is the command code, the \c Param is the application specified command parameter and the \c Status
**  is the error code returned by the #CFE_SB_SendMsg API call. 
**/
#define CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID       89
/** \} */

/** \name API Informational Event Message IDs */
/** \{ */

/** \brief <tt> 'Successfully loaded '\%s' from '\%s'' </tt>
**  \event <tt> 'Successfully loaded '\%s' from '\%s'' </tt>
**
**  \par Type: DEBUG (the first time) and INFORMATION (normally)
**
**  \par Cause:
**
**  This event message is generated when a Table is successfully updated by its owning Application
**  with the contents of the Application specified file or memory area.  This Event Message only
**  appears when an Application successfully calls the #CFE_TBL_Load API.
**/
#define CFE_TBL_LOAD_SUCCESS_INF_EID           35  

/** \brief <tt> '\%s validation successful for Inactive '\%s'' </tt>
**  \event <tt> '\%s validation successful for Inactive '\%s'' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when a Table Image is successfully validated by its owning
**  Application via the Validation function specified by the owning Application when the table
**  was first registered.
**/
#define CFE_TBL_VALIDATION_INF_EID             36  

/** \brief <tt> '\%s Successfully Updated '\%s'' </tt>
**  \event <tt> '\%s Successfully Updated '\%s'' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when a Table's Active Buffer is successfully updated with the contents
**  of its Inactive Buffer.
**/
#define CFE_TBL_UPDATE_SUCCESS_INF_EID         37  

/** \brief <tt> 'Successfully removed '\%s' from CDS' </tt>
**  \event <tt> 'Successfully removed '\%s' from CDS' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when a Critical Table's CDS has been successfully deleted.
**/
#define CFE_TBL_CDS_DELETED_INFO_EID           38  
/** \} */



/** \name API Error Event Message IDs */
/** \{ */
/** \brief <tt> '\%s Failed to Register '\%s', Status=0x\%08X' </tt>
**  \event <tt> '\%s Failed to Register '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Register unsuccessfully.
**
**  The \c Status field of the Event Message can be used to identify the reason for the failure
**  by looking it up in the cfe_error.h file
**/
#define CFE_TBL_REGISTER_ERR_EID               90  

/** \brief <tt> '\%s Failed to Share '\%s', Status=0x\%08X' </tt>
**  \event <tt> '\%s Failed to Share '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Share unsuccessfully.
**
**  The \c Status field of the Event Message can be used to identify the reason for the failure
**  by looking it up in the cfe_error.h file
**/
#define CFE_TBL_SHARE_ERR_EID                  91  

/** \brief <tt> '\%s Failed to Unregister '\%s', Status=0x\%08X' </tt>
**  \event <tt> '\%s Failed to Unregister '\%s', Status=0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Unregister unsuccessfully.
**
**  The \c Status field of the Event Message can be used to identify the reason for the failure
**  by looking it up in the cfe_error.h file
**/
#define CFE_TBL_UNREGISTER_ERR_EID             92  

/** \brief <tt> '\%s Failed to Load '\%s' from \%s, Status=0x\%08X"  </tt>
**  \event <tt> '\%s Failed to Load '\%s' from \%s, Status=0x\%08X"  </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Load unsuccessfully.
**
**  The \c Status field of the Event Message can be used to identify the reason for the failure
**  by looking it up in the cfe_error.h file
**/
#define CFE_TBL_LOAD_ERR_EID                   93  

/** \brief <tt> '\%s Failed to Load '\%s' (Invalid Source Type)"  </tt>
**  \event <tt> '\%s Failed to Load '\%s' (Invalid Source Type)"  </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Load with a bad value for
**  the \c SrcType parameter. The \c SrcType must be one of the values specified by #CFE_TBL_SrcEnum_t.
**/
#define CFE_TBL_LOAD_TYPE_ERR_EID              94  

/** \brief <tt> '\%s Failed to Update '\%s', Status=0x\%08X"  </tt>
**  \event <tt> '\%s Failed to Update '\%s', Status=0x\%08X"  </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Update (or, via an internal call,
**  the #CFE_TBL_Manage) API and the Table fails to properly update.
**
**  The \c Status parameter in the Event Message can be used to identify the reason for the failure
**  by looking it up in the cfe_error.h file.
**/
#define CFE_TBL_UPDATE_ERR_EID                 95  

/** \brief <tt> '\%s validation failed for Inactive '\%s', Status=0x\%08X"  </tt>
**  \event <tt> '\%s validation failed for Inactive '\%s', Status=0x\%08X"  </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an Application calls #CFE_TBL_Validate (or, via an internal call,
**  the #CFE_TBL_Manage) API and the Table fails its Validation.
**
**  The \c Status parameter in the Event Message contains the status code returned by the Table's
**  Validation function as defined by the owning Application when the Table was Registered.
**/
#define CFE_TBL_VALIDATION_ERR_EID             96  

/** \brief <tt> 'Unable to verify Spacecraft ID for '\%s', ID = 0x\%08X' </tt>
**  \event <tt> 'Unable to verify Spacecraft ID for '\%s', ID = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either an Application calls the #CFE_TBL_Load API or a Table
**  Load command has been received and the specified table file has failed Spacecraft ID validation.
**  Verification of Spacecraft ID in table files is enabled/disabled via #CFE_TBL_VALID_SCID_COUNT,
**  defined in the platform configuration header file.  This event message can only be generated if
**  #CFE_TBL_VALID_SCID_COUNT has a non-zero value and the table file has a \link #CFE_FS_Header_t
**  cFE Standard File Header \endlink whose \link #CFE_FS_Header_t::SpacecraftID Spacecraft ID \endlink 
**  does not match one of the values defined for Spacecraft ID verification in the platform config file.
**  The most likely causes for this error are:
**   -# The specified table file is not intended for this spacecraft.
**   -# The specified table file has been created with bad "endianess" (headers should always conform to
**      a big endian format).
**   -# The specified table file has become corrupted.
**   -# The definition for #CFE_TBL_VALID_SCID_COUNT is not large enough to include all of the valid
**      Spacecraft ID entries in the platform config file.
**   -# There is no entry for this Spacecraft ID in the platform config file list of valid Spacecraft ID's.
**
**  The \c ID field specified in the event message contains the Spacecraft ID that was found
**  in the specified table file.
**/
#define CFE_TBL_SPACECRAFT_ID_ERR_EID          97  

/** \brief <tt> 'Unable to verify Processor ID for '\%s', ID = 0x\%08X' </tt>
**  \event <tt> 'Unable to verify Processor ID for '\%s', ID = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when either an Application calls the #CFE_TBL_Load API or a Table
**  Load command has been received and the specified table file has failed Processor ID validation.
**  Verification of Processor ID in table files is enabled/disabled via #CFE_TBL_VALID_PRID_COUNT,
**  defined in the platform configuration header file.  This event message can only be generated if
**  #CFE_TBL_VALID_PRID_COUNT has a non-zero value and the table file has a \link #CFE_FS_Header_t
**  cFE Standard File Header \endlink whose \link #CFE_FS_Header_t::ProcessorID Processor ID \endlink 
**  does not match one of the values defined for Processor ID verification in the platform config file.
**  The most likely causes for this error are:
**   -# The specified table file is not intended for this processor.
**   -# The specified table file has been created with bad "endianess" (headers should always conform to
**      a big endian format).
**   -# The specified table file has become corrupted.
**   -# The definition for #CFE_TBL_VALID_PRID_COUNT is not large enough to include all of the valid
**      Processor ID entries in the platform config file.
**   -# There is no entry for this Processor ID in the platform config file list of valid Processor ID's.
**
**  The \c ID field specified in the event message contains the Processor ID that was found
**  in the specified table file.
**/
#define CFE_TBL_PROCESSOR_ID_ERR_EID           98  

/** \} */



#endif /* _cfe_tbl_events_ */

/************************/
/*  End of File Comment */
/************************/
