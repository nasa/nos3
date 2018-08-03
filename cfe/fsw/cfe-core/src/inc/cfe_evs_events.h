/*
**
**  Filename: cfe_evs_events.h
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASAs Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**  $Id: cfe_evs_events.h 1.7 2011/06/02 17:06:44GMT-05:00 lwalling Exp  $
**
**  Purpose:
**	           cFE Event Services (EVS) Event IDs
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**  $Date: 2011/06/02 17:06:44GMT-05:00 $
**  $Revision: 1.7 $
**  $Log: cfe_evs_events.h  $
**  Revision 1.7 2011/06/02 17:06:44GMT-05:00 lwalling 
**  Updated text and descriptions for Write Application Data to File command events
**  Revision 1.6 2011/06/01 17:38:43EDT lwalling 
**  Update Write Log File and Set Log Mode command events
**  Revision 1.5 2011/05/23 15:40:20EDT lwalling 
**  Update descriptions for event log command events when event log is disabled
**  Revision 1.4 2010/10/04 15:25:17EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.3 2009/07/31 19:54:05EDT aschoeni 
**  added length check error message and changed reported bitmask lengths
**  Revision 1.2 2009/07/28 17:16:33EDT aschoeni 
**  Added event message when filtering limit is reached.
**  Revision 1.1 2008/04/17 08:05:21EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
**  Revision 1.13 2007/07/18 15:48:00EDT njyanchik 
**  Jonathan added text into each error event in the two functions that write files (WriteLog and WriteAppData), as well as updated the doxygen comments in the header file
**  Revision 1.11 2007/06/02 10:10:12EDT dlkobe 
**  Added doxygen comments for User's Guides
**  Revision 1.10 2007/05/23 11:21:59EDT dlkobe 
**  Added doxygen formatting
**  Revision 1.9 2007/05/07 10:42:03EDT njyanchik 
**  I added a sending of an Event message from EVS on the first time an EVS-unregistered
**  application tries to send an event message. A system log message is also written
**  Revision 1.8 2007/03/05 14:08:43EST njyanchik 
**  This CP fixes the fact that there were multiple event messages using the same event ID, but 
**  using different (but similar) event text. The issue was fixed by making all the text generic, and 
**  providing a command code parameter that specified what command the message was comming
**  from.
*/

#ifndef _cfe_evs_events_
#define _cfe_evs_events_

/* Event Service event ID's */

/** \brief  <tt> 'No-op command' </tt>
**  \event  <tt> 'No-op command' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Event Services \link #CFE_EVS_NO_OPERATION_CC NO-OP command \endlink
**/
#define CFE_EVS_NOOP_EID                   0    /* Noop event identifier */

/** \brief <tt> 'cFE EVS Initialized' </tt>
**  \event <tt> 'cFE EVS Initialized' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is always automatically issued when the Event Services
**  Task completes its Initialization.
**/
#define CFE_EVS_STARTUP_EID                1

/** \brief <tt> 'Write Log File Command Error: OS_write = 0x\%08X, filename = \%s' </tt>
**  \event <tt> 'Write Log File Command Error: OS_write = 0x\%08X, filename = \%s' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a filesystem error occurred while writing the contents of the
**  event message log to a file.
**
**  The message text identifies the event log filename and specifies the return value, in hex,
**  from the system function call.  The expected return value is the number of bytes written,
**  which in this case should be equal to the size of a #CFE_EVS_Packet_t structure.  Error
**  codes are negative.
**/
#define CFE_EVS_ERR_WRLOGFILE_EID          2

/** \brief <tt> 'Write Log File Command Error: OS_creat = 0x\%08X, filename = \%s' </tt>
**  \event <tt> 'Write Log File Command Error: OS_creat = 0x\%08X, filename = \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a filesystem error occurred when attempting to create the file
**  that is to hold the event message log.
**
**  The message text identifies the event log filename and specifies the return value, in hex,
**  from the system function call.  The expected return value is a file handle, which in this case
**  should be a relatively small positive number.  Error codes are negative.
**/
#define CFE_EVS_ERR_CRLOGFILE_EID          3

/** \brief <tt> 'Invalid command packet, Message ID = 0x\%08X' </tt>
**  \event <tt> 'Invalid command packet, Message ID = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message has arrived on
**  the cFE Event Services Application's Message Pipe that has a
**  Message ID that is neither #CFE_EVS_CMD_MID or #CFE_EVS_SEND_HK_MID.
**  Most likely, the cFE Software Bus routing table has become corrupt
**  and is sending messages targeted for other Applications to the cFE
**  Event Services Application.  
**
**  The \c ID field in the event message identifies
**  the message ID (in hex) that was found in the message.
**/
#define CFE_EVS_ERR_MSGID_EID              5

/** \brief <tt> '\%s Event ID \%d not registered for filtering: CC = \%lu' </tt>
**  \event <tt> '\%s Event ID \%d not registered for filtering: CC = \%lu' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the specified command identifies an Application and Event ID combination
**  that is not found in the Events Registry.
**
**  The \c \%s string contains the command specified Application Name the \c Event \c ID field
**  identifies the command specified EventID (in decimal) that was not found in the Events Registry.
**  The \c CC field specifies the Command Code whose processing generated the event message.  It can
**  be equal to either #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, or #CFE_EVS_DELETE_EVENT_FILTER_CC.
**/
#define CFE_EVS_ERR_EVTIDNOREGS_EID        6

/** \brief <tt> '\%s not registered with EVS: CC = \%lu' </tt>
**  \event <tt> '\%s not registered with EVS: CC = \%lu' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the specified command identifies an Application that has not been
**  registered with the cFE Event Services.
**
**  The \c CC field contains the Command Code whose processing resulted in the generation of the event message.
**  Possible values are #CFE_EVS_SET_FILTER_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC,
**  #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC, #CFE_EVS_RESET_APP_COUNTER_CC, #CFE_EVS_RESET_FILTER_CC,
**  #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, or #CFE_EVS_DELETE_EVENT_FILTER_CC.
**/
#define CFE_EVS_ERR_APPNOREGS_EID          7

/** \brief <tt> 'Illegal application ID \%d retrieved for \%s: CC = \%lu' </tt>
**  \event <tt> 'Illegal application ID \%d retrieved for \%s: CC = \%lu' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the specified command identifies an Application whose name is
**  found in the Events Registry but does not appear to be properly registered with the cFE Executive Services.
**
**  The \c CC field contains the Command Code whose processing resulted in the generation of the event message.
**  Possible values are #CFE_EVS_SET_FILTER_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC,
**  #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC, #CFE_EVS_RESET_APP_COUNTER_CC, #CFE_EVS_RESET_FILTER_CC,
**  #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, or #CFE_EVS_DELETE_EVENT_FILTER_CC.
**/
#define CFE_EVS_ERR_ILLAPPIDRANGE_EID      8

/** \brief <tt> 'Unable to retrieve application ID for \%s: CC = \%lu' </tt>
**  \event <tt> 'Unable to retrieve application ID for \%s: CC = \%lu' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the specified command contains an Application name that is
**  apparently found in the Events Registry but does not appear to be registered with the cFE Executive Services.
**
**  The \c CC field contains the Command Code whose processing resulted in the generation of the event message.
**  Possible values are #CFE_EVS_SET_FILTER_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC,
**  #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC, #CFE_EVS_RESET_APP_COUNTER_CC, #CFE_EVS_RESET_FILTER_CC,
**  #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, or #CFE_EVS_DELETE_EVENT_FILTER_CC.
**/
#define CFE_EVS_ERR_NOAPPIDFOUND_EID       9

/** \brief <tt> 'Set Event Format Mode Command: Invalid Event Format Mode = 0x\%02x' </tt>
**  \event <tt> 'Set Event Format Mode Command: Invalid Event Format Mode = 0x\%02x' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a "Set Event Format Mode" command
**  message has arrived and the #CFE_EVS_ModeCmd_t::Mode field is equal to
**  neither #CFE_EVS_SHORT_FORMAT or #CFE_EVS_LONG_FORMAT.  These are the 
**  only allowed values for the mode field.
**  
**  The \c Mode field in the event message identifies
**  the Mode value (in hex) that was found in the message.
**/
#define CFE_EVS_ERR_ILLEGALFMTMOD_EID     10

/** \brief <tt> 'Add Filter Command: number of registered filters has reached max = \%d' </tt>
**  \event <tt> 'Add Filter Command: number of registered filters has reached max = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of an "Add Filter" command
**  and the specified Application has already reached the maximum number of
**  filters allowed (#CFE_EVS_MAX_EVENT_FILTERS).
**
**  The \c max field in the event message identifies the maximum number of
**  event filters allowed per Application.  This value should be equal to
**  the configuration parameter #CFE_EVS_MAX_EVENT_FILTERS.
**/
#define CFE_EVS_ERR_MAXREGSFILTER_EID     11

/** \brief <tt> 'Write App Data Command Error: OS_write = 0x\%08X, filename = \%s' </tt>
**  \event <tt> 'Write App Data Command Error: OS_write = 0x\%08X, filename = \%s' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a filesystem error occurred while writing the contents of the
**  event registry to a file.
**
**  The message text identifies the registry filename and specifies the return value, in hex,
**  from the system function call.  The expected return value is the number of bytes written,
**  which in this case should be equal to the size of a #CFE_EVS_AppDataFile_t structure.  Error
**  codes are negative.
**/
#define CFE_EVS_ERR_WRDATFILE_EID         12

/** \brief <tt> 'Write App Data Command Error: OS_creat = 0x\%08X, filename = \%s' </tt>
**  \event <tt> 'Write App Data Command Error: OS_creat = 0x\%08X, filename = \%s' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a filesystem error occurred when attempting to create the file
**  that is to hold the event registry data.
**
**  The message text identifies the registry filename and specifies the return value, in hex,
**  from the system function call.  The expected return value is a file handle, which in this case
**  should be a relatively small positive number.  Error codes are negative.
**/
#define CFE_EVS_ERR_CRDATFILE_EID         13

/** \brief <tt> 'Invalid command code -- ID = 0x\%08x, CC = \%d' </tt>
**  \event <tt> 'Invalid command code -- ID = 0x\%08x, CC = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_EVS_CMD_MID
**  message ID has arrived but whose Command Code is not one of the specified
**  accepted command codes specified.  This problem is most likely to occur when:
**    -# A Message ID meant for another Application became corrupted and was
**       set equal to #CFE_EVS_CMD_MID.
**    -# The Command Code field in the Message became corrupted.
**    -# The command database at the ground station has been corrupted.
**
**  The \c ID field in the event message specifies the Message ID (in hex) and the 
**  \c CC field specifies the Command Code (in decimal) found in the message. 
**/
#define CFE_EVS_ERR_CC_EID                15

/** \brief <tt> 'Reset Counters Command Received' </tt>
**  \event <tt> 'Reset Counters Command Received' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is always automatically issued in response 
**  to a cFE Event Services Reset Counters command
**/
#define CFE_EVS_RSTCNT_EID                16

/** \brief <tt> 'Set Filter Mask Command Received with AppName=\%s, EventID=0x\%08x, Mask=0x\%04x' </tt>
**  \event <tt> 'Set Filter Mask Command Received with AppName=\%s, EventID=0x\%08x, Mask=0x\%04x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of a Set Filter Mask command.
**
**  The \c AppName field identifies the Application whose Filter Mask has been changed.
**  The \c EventID field identifies the Event whose Filter Mask has been changed.
**  The \c Mask field identifies the new Mask value associated with the specified event.
**/
#define CFE_EVS_SETFILTERMSK_EID          17

/** \brief <tt> 'Enable Ports Command Received with Port Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Enable Ports Command Received with Port Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the "Enable Ports" command.
**
**  The \c Mask field identifies the ports that are enabled.
**  Mask bits are defined by #CFE_EVS_PORT1_BIT, #CFE_EVS_PORT2_BIT, #CFE_EVS_PORT3_BIT and #CFE_EVS_PORT4_BIT.
**/
#define CFE_EVS_ENAPORT_EID               18

/** \brief <tt> 'Disable Ports Command Received with Port Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Disable Ports Command Received with Port Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the "Disable Ports" command.
**
**  The \c Mask field identifies (in hex) the ports are to be disabled.
**  Mask bits are defined by #CFE_EVS_PORT1_BIT, #CFE_EVS_PORT2_BIT, #CFE_EVS_PORT3_BIT and #CFE_EVS_PORT4_BIT.
**/
#define CFE_EVS_DISPORT_EID               19

/** \brief <tt> 'Enable Event Type Command Received with Event Type Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Enable Event Type Command Received with Event Type Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the "Enable Event Type" command.
**
**  The \c Mask field identifies the Event Types that are enabled.
**  Mask bits are defined by #CFE_EVS_DEBUG_BIT, #CFE_EVS_INFORMATION_BIT, 
**  #CFE_EVS_ERROR_BIT and #CFE_EVS_CRITICAL_BIT.
**/
#define CFE_EVS_ENAEVTTYPE_EID            20

/** \brief <tt> 'Disable Event Type Command Received with Event Type Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Disable Event Type Command Received with Event Type Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued upon successful processing of the "Disable Event Type" command.
**
**  The \c Mask field identifies the Event Types that are disabled.
**  Mask bits are defined by #CFE_EVS_DEBUG_BIT, #CFE_EVS_INFORMATION_BIT, 
**  #CFE_EVS_ERROR_BIT and #CFE_EVS_CRITICAL_BIT.
**/
#define CFE_EVS_DISEVTTYPE_EID            21

/** \brief <tt> 'Set Event Format Mode Command Received with Mode = 0x\%02x' </tt>
**  \event <tt> 'Set Event Format Mode Command Received with Mode = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Set Event Format Mode" command.
**
**  The \c Mode field contains the newly chosen Event Format Mode (specified in hex).  Acceptable values
**  for this parameter are: #CFE_EVS_SHORT_FORMAT or #CFE_EVS_LONG_FORMAT
**/
#define CFE_EVS_SETEVTFMTMOD_EID          22

/** \brief <tt> 'Enable App Event Type Command Received with AppName = \%s, EventType Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Enable App Event Type Command Received with AppName = \%s, EventType Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Enable Application Event Types" command.
**
**  The \c AppName field identifies the Application whose Event Type Enable status has changed and the
**  \c Mask field specifies (in hex) the Event Types that have been enabled.
**  Mask bits are defined by #CFE_EVS_DEBUG_BIT, #CFE_EVS_INFORMATION_BIT, 
**  #CFE_EVS_ERROR_BIT and #CFE_EVS_CRITICAL_BIT.
**/
#define CFE_EVS_ENAAPPEVTTYPE_EID         23

/** \brief <tt> 'Disable App Event Type Command Received with AppName = \%s, EventType Bit Mask = 0x\%02x' </tt>
**  \event <tt> 'Disable App Event Type Command Received with AppName = \%s, EventType Bit Mask = 0x\%02x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Disable Application Event Types" command.
**
**  The \c AppName field identifies the Application whose Event Type Disable status has changed and the
**  \c Mask field specifies (in hex) the Event Types that have been disabled.
**  Mask bits are defined by #CFE_EVS_DEBUG_BIT, #CFE_EVS_INFORMATION_BIT, 
**  #CFE_EVS_ERROR_BIT and #CFE_EVS_CRITICAL_BIT.
**/
#define CFE_EVS_DISAPPENTTYPE_EID         24

/** \brief <tt> 'Enable App Events Command Received with AppName = \%s' </tt> 
**  \event <tt> 'Enable App Events Command Received with AppName = \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Enable Application Events" command.
**
**  The \c AppName field identifies the Application whose Events have been Enabled.
**/
#define CFE_EVS_ENAAPPEVT_EID             25

/** \brief <tt> 'Disable App Events Command Received with AppName = \%s' </tt>
**  \event <tt> 'Disable App Events Command Received with AppName = \%s' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Disable Application Events" command.
**
**  The \c AppName field identifies the Application whose Events have been Disabled.
**/
#define CFE_EVS_DISAPPEVT_EID             26

/** \brief <tt> 'Reset Event Counter Command Received with AppName = \%s' </tt>
**  \event <tt> 'Reset Event Counter Command Received with AppName = \%s' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Reset Application Event Counter" command.
**
**  The \c AppName field identifies the Application whose Event Counter has been reset.
**/
#define CFE_EVS_RSTEVTCNT_EID             27

/** \brief <tt> 'Reset Filter Command Received with AppName = \%s, EventID = 0x\%08x' </tt>
**  \event <tt> 'Reset Filter Command Received with AppName = \%s, EventID = 0x\%08x' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Reset Application Event Message Filter" command.
**
**  The \c AppName field identifies the Application whose Event Message Filter has been reset and
**  the \c EventID field identifies the specific event message whose filter has been reset.
**/
#define CFE_EVS_RSTFILTER_EID             28

/** \brief <tt> 'Reset All Filters Command Received with AppName = \%s' </tt>
**  \event <tt> 'Reset All Filters Command Received with AppName = \%s' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Reset Application Event Message Filters" command.
**
**  The \c AppName field identifies the Application whose entire set of Event Filters has been reset.
**/
#define CFE_EVS_RSTALLFILTER_EID          29

/** \brief <tt> 'Add Filter Command Received with AppName = \%s, EventID = 0x\%08x, Mask = 0x\%04x' </tt> 
**  \event <tt> 'Add Filter Command Received with AppName = \%s, EventID = 0x\%08x, Mask = 0x\%04x' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Add Filter" command.
**
**  The \c AppName field identifies the Application who is getting the new filter, the \c EventID field
**  identifies the Event Identifier, in hex, that is getting the filter, and the \c Mask field specifies, in hex,
**  what the binary filter mask has been set to.
**/
#define CFE_EVS_ADDFILTER_EID             30

/** \brief <tt> 'Delete Filter Command Received with AppName = \%s, EventID = 0x\%08x' </tt> 
**  \event <tt> 'Delete Filter Command Received with AppName = \%s, EventID = 0x\%08x' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the "Delete Filter" command.
**
**  The \c AppName field identifies the Application who is getting the filter removed, the \c EventID field
**  identifies the Event Identifier, in hex, whose filter is being deleted.
**/
#define CFE_EVS_DELFILTER_EID             31

/** \brief <tt> 'Write App Data Command: \%d application data entries written to \%s' </tt> 
**  \event <tt> 'Write App Data Command: \%d application data entries written to \%s' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the 
**  \link #CFE_EVS_FILE_WRITE_APP_DATA_CC "Write Event Services Application Information to File" \endlink command.
**
**  The message text identifies the event log filename and specifies the number, in decimal,
**  of events written to the file.
**/
#define CFE_EVS_WRDAT_EID                 32

/** \brief <tt> 'Write Log File Command: \%d event log entries written to \%s' </tt>
**  \event <tt> 'Write Log File Command: \%d event log entries written to \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated upon successful completion of the 
**  \link #CFE_EVS_FILE_WRITE_LOG_DATA_CC "Write Event Log to File" \endlink command.
**
**  The message text identifies the event log filename and specifies the number, in decimal,
**  of events written to the file.
**/
#define CFE_EVS_WRLOG_EID                 33

/** \brief <tt> 'Set Log Mode Command: Event Log is Disabled' </tt>
**  \event <tt> 'Set Log Mode Command: Event Log is Disabled' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of a "Set Log Mode"
**  command when the use of the Event Log has been disabled.  To enable
**  the Event Log, the cFE code must be compiled for the target with
**  the \b CFE_EVS_LOG_ON macro defined.  The EVS task must also succeed
**  during task initialization in acquiring a pointer to the cFE reset
**  area and in the creation of a serializing semaphore to control
**  access to the Event Log.
**/
#define CFE_EVS_NO_LOGSET_EID             34

/** \brief <tt> 'Clear Log Command: Event Log is Disabled' </tt>
**  \event <tt> 'Clear Log Command: Event Log is Disabled' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of a "Clear Log"
**  command when the use of the Event Log has been disabled.  To enable
**  the Event Log, the cFE code must be compiled for the target with
**  the \b CFE_EVS_LOG_ON macro defined.  The EVS task must also succeed
**  during task initialization in acquiring a pointer to the cFE reset
**  area and in the creation of a serializing semaphore to control
**  access to the Event Log.
**/
#define CFE_EVS_NO_LOGCLR_EID             35

/** \brief <tt> 'Write Log Command: Event Log is Disabled' </tt>
**  \event <tt> 'Write Log Command: Event Log is Disabled' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated upon receipt of a "Write Log"
**  command when the use of the Event Log has been disabled.  To enable
**  the Event Log, the cFE code must be compiled for the target with
**  the \b CFE_EVS_LOG_ON macro defined.  The EVS task must also succeed
**  during task initialization in acquiring a pointer to the cFE reset
**  area and in the creation of a serializing semaphore to control
**  access to the Event Log.
**/
#define CFE_EVS_NO_LOGWR_EID              36

/** \brief <tt> 'Add Filter Command:AppName = \%s, EventID = 0x\%08x is already registered for filtering' </tt>
**  \event <tt> 'Add Filter Command:AppName = \%s, EventID = 0x\%08x is already registered for filtering' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an "Add Filter" command was received specifying an Event ID that has already
**  had a filter added.
**
**  The \c AppName field identifies the Application whose filter was to be added and the \c EventID field identifies,
**  in hex, the Event ID that the command was trying to add a filter for.
**/
#define CFE_EVS_EVT_FILTERED_EID          37

/** \brief <tt> 'Set Log Mode Command Error: Log Mode = \%d' </tt> 
**  \event <tt> 'Set Log Mode Command Error: Log Mode = \%d' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is generated when a "Set Log Mode" command is completed successfully.
**
**  The event text identifies the Log Mode command argument.  Valid Log Mode command
**  arguments are: #CFE_EVS_LOG_OVERWRITE or #CFE_EVS_LOG_DISCARD.
**/
#define CFE_EVS_LOGMODE_EID               38

/** \brief <tt> 'Set Log Mode Command Error: Log Mode = \%d' </tt>
**  \event <tt> 'Set Log Mode Command Error: Log Mode = \%d </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a "Set Log Mode" command is received that specifies
**  an invalid Log Mode command argument.
**  
**  The event text identifies the invalid Log Mode command argument.  Valid Log Mode command
**  arguments are: #CFE_EVS_LOG_OVERWRITE or #CFE_EVS_LOG_DISCARD.
**/
#define CFE_EVS_ERR_LOGMODE_EID           39

/** \brief <tt> 'Bit Mask = 0x\%X out of range: CC = \%lu' </tt>
**  \event <tt> 'Bit Mask = 0x\%X out of range: CC = \%lu' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the bit mask passed in is  equal to zero or greater than 0x0F, because a 
**  bit mask of zero does nothing, and a bitmask of greater than 0x0F  is invalid.
**/
#define CFE_EVS_ERR_INVALID_BITMASK_EID   40

/** \brief <tt> 'App \%s not registered with Event Services. Unable to send event' </tt>
**  \event <tt> 'App \%s not registered with Event Services. Unable to send event' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when an event message has been requested to be sent by an Application that
**  has not registered itself with cFE Event Services. 
**/
#define CFE_EVS_ERR_UNREGISTERED_EVS_APP  41

/** \brief <tt> 'Max filter count reached, AppName = \%s, EventID = 0x\%08x: Filter locked until reset' </tt>
**  \event <tt> 'Max filter count reached, AppName = \%s, EventID = 0x\%08x: Filter locked until reset' </tt> 
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is generated when the filtering count for a specific App and Event ID reaches #CFE_EVS_MAX_FILTER_COUNT
**  The filtered event will no longer be received until the reset counter is reset via a
**  \link #CFE_EVS_RESET_FILTER_CC "Reset an Event Filter for an Application" \endlink or a
**  \link #CFE_EVS_RESET_ALL_FILTERS_CC "Reset All Event Filters for an Application" \endlink
**  
**  The \c AppName field identifies the Application and the \c EventID field identifies, in hex, the Event ID
**  for the filter whose maxium was reached.
**/
#define CFE_EVS_FILTER_MAX_EID                 42

/** \brief <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt>
**  \event <tt> 'Invalid cmd length: ID = 0x\%X, CC = \%d, Exp Len = \%d, Len = \%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when a message with the #CFE_EVS_CMD_MID
**  message ID has arrived but whose packet length does not match the expected
**  length for the specified command code.
**
**  The \c ID field in the event message specifies the Message ID (in hex), the \c CC field
**  specifies the Command Code (in decimal), the \c Exp Len field specified the Expected
**  Length (in decimal ), and \c Len specifies the message Length (in decimal)
**  found in the message. 
**/
#define CFE_EVS_LEN_ERR_EID       43

#endif  /* _cfe_evs_events_ */

