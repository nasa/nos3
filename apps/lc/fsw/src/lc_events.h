/************************************************************************
** File:
**   $Id: lc_events.h 1.2 2015/03/04 16:09:56EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) event identifers.
**
** Notes:
**
**   $Log: lc_events.h  $
**   Revision 1.2 2015/03/04 16:09:56EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:38EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.11 2011/03/10 14:13:18EST lwalling 
**   Cleanup use of debug events during task startup
**   Revision 1.10 2011/03/01 09:36:38EST lwalling 
**   Modified startup logic re use of CDS and critical tables, remove unused event IDs
**   Revision 1.9 2011/02/07 17:57:58EST lwalling 
**   Modify sample AP commands to target groups of AP's
**   Revision 1.8 2010/03/03 15:17:17EST lwalling 
**   Removed pound symbols from some Doxygen names
**   Revision 1.7 2010/02/25 11:45:22EST lwalling 
**   Defined LC_BASE_AP_EID for use in actionpoint definition table
**   Revision 1.6 2009/12/28 14:51:15EST lwalling 
**   Change limited events from debug to info
**   Revision 1.5 2009/02/23 11:17:10EST dahardis 
**   Added two event messages and modified two others for
**   consistency when addressing DCR 7084
**   Revision 1.4 2009/01/29 15:39:21EST dahardis 
**   Changed an event message from INFO to DEBUG as documented
**   in DCR #6811
**   Revision 1.3 2008/12/10 15:34:47EST dahardis 
**   Added an event message needed for 
**   DCR 4680
**   Revision 1.2 2008/12/03 13:59:47EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:27EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_events_
#define _lc_events_

/** \brief <tt> 'Task terminating, err = 0x\%08X' </tt>
**  \event <tt> 'Task terminating, err = 0x\%08X' </tt>
**  
**  \par Type: CRITICAL
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker
**  exits due to a fatal error condition.
**
**  The \c err field contains the return status from the
**  cFE call that caused the task to terminate
*/
#define LC_TASK_EXIT_EID                         1    

/** \brief <tt> 'LC Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'LC Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker has
**  completed initialization.
**
**  The first \c %d field contains the Application's Major Version Number
**  The second \c %d field contains the Application's Minor Version Number
**  The third \c %d field contains the Application's Revision Number
**  The fourth \c %d field contains the Application's Mission Revision Number
*/
#define LC_INIT_INF_EID                          2    

/** \brief <tt> 'Error Creating LC Pipe, RC=0x\%08X' </tt>
**  \event <tt> 'Error Creating LC Pipe, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker
**  is unable to create its command pipe via the #CFE_SB_CreatePipe
**  API
**
**  The \c RC field contains the return status from the
**  #CFE_SB_CreatePipe call that generated the error
*/
#define LC_CR_PIPE_ERR_EID                       3

/** \brief <tt> 'Error Subscribing to HK Request, MID=0x\%04X, RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to HK Request, MID=0x\%04X, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker
**  is unable to subscribe to its Housekeeping Request message 
**  via the #CFE_SB_Subscribe API
**
**  The \c MID field contains the Message ID that LC was 
**  attempting to subscribe to. The \c RC field contains the 
**  return status from the #CFE_SB_Subscribe call that generated 
**  the error
*/
#define LC_SUB_HK_REQ_ERR_EID                    4

/** \brief <tt> 'Error Subscribing to GND CMD, MID=0x\%04X, RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to GND CMD, MID=0x\%04X, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker
**  is unable to subscribe to its ground commands via the
**  #CFE_SB_Subscribe API
**
**  The \c MID field contains the Message ID that LC was 
**  attempting to subscribe to. The \c RC field contains the 
**  return status from the #CFE_SB_Subscribe call that generated 
**  the error
*/
#define LC_SUB_GND_CMD_ERR_EID                   5

/** \brief <tt> 'Error Subscribing to Sample CMD, MID=0x\%04X, RC=0x\%08X' </tt>
**  \event <tt> 'Error Subscribing to Sample CMD, MID=0x\%04X, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS Limit Checker
**  is unable to subscribe to its actionpoint sample command via the
**  #CFE_SB_Subscribe API
**
**  The \c MID field contains the Message ID that LC was 
**  attempting to subscribe to. The \c RC field contains the 
**  return status from the #CFE_SB_Subscribe call that generated 
**  the error
*/
#define LC_SUB_SAMPLE_CMD_ERR_EID                6

/** \brief <tt> 'Error registering WDT, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering WDT, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the watchpoint definition
**  table (WDT) could not be registered.
**
**  The \c RC field is the return code from the #CFE_TBL_Register
**  function call that generated the error.
*/
#define LC_WDT_REGISTER_ERR_EID                  7

/** \brief <tt> 'Error re-registering WDT, RC=0x\%08X' </tt>
**  \event <tt> 'Error re-registering WDT, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the watchpoint definition
**  table (WDT) could not be registered non-critical after first
**  registering and then unregistering the table as critical.
**  This sequence can only occur when the WDT succeeds and the
**  ADT fails the attempt to register as a critical table. This
**  error is extremely unlikely to occur.
**
**  The \c RC field is the return code from the #CFE_TBL_Register
**  function call that generated the error.
*/
#define LC_WDT_REREGISTER_ERR_EID                8

/** \brief <tt> 'Error registering ADT, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering ADT, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the actionpoint definition
**  table (ADT) could not be registered.
**
**  The \c RC field is the return code from the #CFE_TBL_Register
**  function call that generated the error.
*/
#define LC_ADT_REGISTER_ERR_EID                  9

/** \brief <tt> 'Error registering WRT, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering WRT, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the watchpoint results 
**  table (WRT) could not be registered.
**
**  The \c RC field is the return code from the #CFE_TBL_Register
**  function call that generated the error.
*/
#define LC_WRT_REGISTER_ERR_EID                  10

/** \brief <tt> 'Error registering ART, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering ART, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the actionpoint results 
**  table (ART) could not be registered.
**
**  The \c RC field is the return code from the #CFE_TBL_Register
**  function call that generated the error.
*/
#define LC_ART_REGISTER_ERR_EID                  11

/** \brief <tt> 'Error registering WRT CDS Area, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering WRT CDS Area, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CDS area for the
**  watchpoint restuls table (WRT) data could not be registered.
**
**  The \c RC field is the return code from the #CFE_ES_RegisterCDS
**  function call that generated the error.
*/
#define LC_WRT_CDS_REGISTER_ERR_EID              12

/** \brief <tt> 'Error registering ART CDS Area, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering ART CDS Area, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CDS area for the
**  actionpont restuls table (ART) data could not be registered.
**
**  The \c RC field is the return code from the #CFE_ES_RegisterCDS
**  function call that generated the error.
*/
#define LC_ART_CDS_REGISTER_ERR_EID              13

/** \brief <tt> 'Error registering application data CDS Area, RC=0x\%08X' </tt>
**  \event <tt> 'Error registering application data CDS Area, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CDS area for the
**  LC application data could not be registered.
**
**  The \c RC field is the return code from the #CFE_ES_RegisterCDS
**  function call that generated the error.
*/
#define LC_APP_CDS_REGISTER_ERR_EID              14

/** \brief <tt> 'Error (RC=0x\%08X) Loading WDT with '\%s'' </tt>
**  \event <tt> 'Error (RC=0x\%08X) Loading WDT with '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an error is encountered
**  loading the watchpoint definition table (WDT) from the default
**  file image
**
**  The \c RC field is the return code from the #CFE_TBL_Load 
**  call that generated the error, the \c with field is the name
**  of the load file
*/
#define LC_WDT_LOAD_ERR_EID                      15

/** \brief <tt> 'Error (RC=0x\%08X) Loading ADT with '\%s'' </tt>
**  \event <tt> 'Error (RC=0x\%08X) Loading ADT with '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an error is encountered
**  loading the actionpoint definition table (ADT) from the default
**  file image
**
**  The \c RC field is the return code from the #CFE_TBL_Load 
**  call that generated the error, the \c with field is the name
**  of the load file
*/
#define LC_ADT_LOAD_ERR_EID                      16

/** \brief <tt> 'Error getting WRT address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting WRT address, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the watchpoint results table (WRT).
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define LC_WRT_GETADDR_ERR_EID                   17

/** \brief <tt> 'Error getting ART address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting ART address, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the actionpoint results table (ART).
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define LC_ART_GETADDR_ERR_EID                   18

/** \brief <tt> 'Error getting WDT address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting WDT address, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the watchpoint definition table (WDT).
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define LC_WDT_GETADDR_ERR_EID                   19

/** \brief <tt> 'Error getting ADT address, RC=0x\%08X' </tt>
**  \event <tt> 'Error getting ADT address, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the address can't be obtained
**  from table services for the actionpoint definition table (ADT).
**
**  The \c RC field is the return code from the #CFE_TBL_GetAddress
**  function call that generated the error.
*/
#define LC_ADT_GETADDR_ERR_EID                   20

/** \brief <tt> 'Previous state restored from Critical Data Store' </tt>
**  \event <tt> 'Previous state restored from Critical Data Store' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued following a processor reset when the
**  entire LC execution state has been successfully restored from the
**  Critical Data Store (CDS). Application global data, actionpoint
**  results data and watchpoint results data were restored from CDS
**  areas managed directly by LC. Actionpoint definition table data
**  and watchpoint definition table data were restored from the CDS
**  area managed by cFE Table Services.
** 
**  Note that an attempt to use CDS is only made when the appropriate
**  parameter is enabled in the LC platform configuration header file.
**
**  \sa #LC_SAVE_TO_CDS, #LC_CDS_UPDATED_INF_EID, #LC_CDS_DISABLED_INF_EID
*/
#define LC_CDS_RESTORED_INF_EID                  21

/** \brief <tt> 'Default state loaded and written to CDS, activity mask = 0x\%08X' </tt>
**  \event <tt> 'Default state loaded and written to CDS, activity mask = 0x\%08X' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued following LC startup initialization
**  when the entire LC execution state has been set to default values
**  successfully stored to the Critical Data Store (CDS). Application
**  global data, actionpoint results data and watchpoint results data
**  were stored to CDS areas managed directly by LC. Actionpoint and
**  watchpoint definition table data are stored to the CDS area
**  managed by cFE Table Services.
**
**  This event implies that the CDS areas were created successfully.
**  The reason that previous data was not restored may be due to the
**  reset type (warm vs cold) or to a failure to restore the entire
**  set of data described above. If data from any CDS area cannot be
**  restored, then all data is set to defaults and written to CDS.
**  
**
**  Refer to the \c ActivityMask bit field for specific information
**  about which CDS areas were created and successfully restored if
**  there are any questions about why the previous values were not
**  restored.
** 
**  Note that an attempt to use CDS is only made when the appropriate
**  parameter is enabled in the LC platform configuration header file.
**
**  \sa #LC_SAVE_TO_CDS, #LC_CDS_RESTORED_INF_EID, #LC_CDS_DISABLED_INF_EID
*/
#define LC_CDS_UPDATED_INF_EID                   22

/** \brief <tt> 'LC use of Critical Data Store disabled, activity mask = 0x\%08X' </tt>
**  \event <tt> 'LC use of Critical Data Store disabled, activity mask = 0x\%08X' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued following LC startup initialization
**  when the entire LC execution state has been set to default values
**  successfully stored to the Critical Data Store (CDS). Application
**  global data, actionpoint results data and watchpoint results data
**  were stored to CDS areas managed directly by LC. Actionpoint and
**  watchpoint definition table data are stored to the CDS area
**  managed by cFE Table Services.
**
**  This event implies that the CDS areas were not created successfully
**  or that LC was unable to write the default values to CDS.
**
**  Refer to the \c ActivityMask bit field for specific information
**  about which CDS areas were created successfully.
** 
**  Note that an attempt to use CDS is only made when the appropriate
**  parameter is enabled in the LC platform configuration header file.
**
**  \sa #LC_SAVE_TO_CDS, #LC_CDS_RESTORED_INF_EID, #LC_CDS_UPDATED_INF_EID
*/
#define LC_CDS_DISABLED_INF_EID                  23

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
#define LC_CC_ERR_EID                            24

/** \brief <tt> 'Sample AP error: invalid AP number, start = \%d, end = \%d' </tt>
**  \event <tt> 'Sample AP error: invalid AP number, start = \%d, end = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SAMPLE_AP_MID message
**  has been received with an invalid actionpoint start or end number specified
**
**  The \c invalid \c AP \c number fields are the numbers specified in 
**  the command message that triggered the error
*/
#define LC_APSAMPLE_APNUM_ERR_EID                25

/** \brief <tt> 'No-op command: Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'No-op command: Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when a NOOP command has been received.
**
**  The first \c %d field contains the Application's Major Version Number
**  The second \c %d field contains the Application's Minor Version Number
**  The third \c %d field contains the Application's Revision Number
**  The fourth \c %d field contains the Application's Mission Revision Number
*/
#define LC_NOOP_INF_EID                          26

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
#define LC_RESET_DBG_EID                         27

/** \brief <tt> 'Set LC state command: new state = \%d' </tt>
**  \event <tt> 'Set LC state command: new state = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_LC_STATE_CC command
**  has been successfully executed
**
**  The \c new \c state field is the state specified in the command
**  message that the LC operating state has been set to. 
*/
#define LC_LCSTATE_INF_EID                       28

/** \brief <tt> 'Set LC state error: invalid state = \%d' </tt>
**  \event <tt> 'Set LC state error: invalid state = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_LC_STATE_CC command
**  has been received with an invalid state argument specified
**
**  The \c invalid \c state field is the state specified in the command
**  message that triggered the error
*/
#define LC_LCSTATE_ERR_EID                       29

/** \brief <tt> 'Set AP state error: AP = \%d, Invalid new state = \%d' </tt>
**  \event <tt> 'Set AP state error: AP = \%d, Invalid new state = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_STATE_CC command
**  has been received with an invalid state argument specified
**
**  The \c AP field is the specified actionpoint number and the 
**  \c Invalid \c new \c state field is the state specified in 
**  the command message that triggered the error
*/
#define LC_APSTATE_NEW_ERR_EID                   30

/** \brief <tt> 'Set AP state error: AP = \%d, Invalid current AP state = \%d' </tt>
**  \event <tt> 'Set AP state error: AP = \%d, Invalid current AP state = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_STATE_CC command
**  has been received and the current actionpoint state is either
**  #LC_ACTION_NOT_USED or #LC_APSTATE_PERMOFF which can only be changed
**  with a table load.
**
**  The \c AP field is the specified actionpoint number and the 
**  \c Invalid \c current \c AP \c state field is the current state 
**  that was determined invalid.
*/
#define LC_APSTATE_CURR_ERR_EID                  31

/** \brief <tt> 'Set AP state error: Invalid AP number = \%d' </tt>
**  \event <tt> 'Set AP state error: Invalid AP number = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_STATE_CC command
**  has been received with an invalid actionpoint number specified
**
**  The \c Invalid \c AP \c number field is the number specified in 
**  the command message that triggered the error
*/
#define LC_APSTATE_APNUM_ERR_EID                 32

/** \brief <tt> 'Set AP state command: AP = \%d, New state = \%d' </tt>
**  \event <tt> 'Set AP state command: AP = \%d, New state = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_STATE_CC command
**  has been successfully executed
**
**  The \c AP field is the actionpoint number, the \c New \c state field 
**  is the state specified in the command message that the actionpoint 
**  state has been set to. 
*/
#define LC_APSTATE_INF_EID                       33

/** \brief <tt> 'Set AP perm off error: Invalid AP number = \%d' </tt>
**  \event <tt> 'Set AP perm off error: Invalid AP number = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_PERMOFF_CC command
**  has been received with an invalid actionpoint number specified
**
**  The \c Invalid \c AP \c number field is the number specified in 
**  the command message that triggered the error
*/
#define LC_APOFF_APNUM_ERR_EID                   34

/** \brief <tt> 'Set AP perm off error, AP NOT Disabled: AP = \%d, Current state = \%d' </tt>
**  \event <tt> 'Set AP perm off error, AP NOT Disabled: AP = \%d, Current state = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_PERMOFF_CC command
**  has been received and the current actionpoint state is not
**  #LC_APSTATE_DISABLED
**
**  The \c AP field is the specified actionpoint number and the 
**  The \c Current \c state field is the current state of the 
**  actionpoint
*/
#define LC_APOFF_CURR_ERR_EID                    35

/** \brief <tt> 'Set AP permanently off command: AP = \%d' </tt>
**  \event <tt> 'Set AP permanently off command: AP = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the #LC_SET_AP_PERMOFF_CC command
**  has been successfully executed
**
**  The \c AP field is the actionpoint number that has been set
**  to permanently off
*/
#define LC_APOFF_INF_EID                         36

/** \brief <tt> 'Reset AP stats error: invalid AP number = \%d' </tt>
**  \event <tt> 'Reset AP stats error: invalid AP number = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_RESET_AP_STATS_CC command
**  has been received with an invalid actionpoint number specified
**
**  The \c invalid \c AP \c number field is the number specified in 
**  the command message that triggered the error
*/
#define LC_APSTATS_APNUM_ERR_EID                 37

/** \brief <tt> 'Reset AP stats command: AP = \%d' </tt>
**  \event <tt> 'Reset AP stats command: AP = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the #LC_RESET_AP_STATS_CC command
**  has been successfully executed
**
**  The \c AP field is the actionpoint number whose stats have been
**  cleared
*/
#define LC_APSTATS_INF_EID                       38

/** \brief <tt> 'Reset WP stats error: invalid WP number = \%d' </tt>
**  \event <tt> 'Reset WP stats error: invalid WP number = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_RESET_WP_STATS_CC command
**  has been received with an invalid watchpoint number specified
**
**  The \c invalid \c WP \c number field is the number specified in 
**  the command message that triggered the error
*/
#define LC_WPSTATS_WPNUM_ERR_EID                 39

/** \brief <tt> 'Reset WP stats command: WP = \%d' </tt>
**  \event <tt> 'Reset WP stats command: WP = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the #LC_RESET_WP_STATS_CC command
**  has been successfully executed
**
**  The \c WP field is the watchpoint number whose stats have been
**  cleared
*/
#define LC_WPSTATS_INF_EID                       40

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
#define LC_HKREQ_LEN_ERR_EID                     41

/** \brief <tt> 'Invalid AP sample msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid AP sample msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a actionpoint sample request is received
**  with a message length that doesn't match the expected value.   
**
**  The \c ID field contains the message ID, the \c CC field contains the 
**  command code, the \c Len field is the actual length returned by the
**  #CFE_SB_GetTotalMsgLength call, and the \c Expected field is the expected
**  length for the message.
*/
#define LC_APSAMPLE_LEN_ERR_EID                  42

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
#define LC_LEN_ERR_EID                           43

/** \brief <tt> 'Error unsubscribing watchpoint: MID=0x\%04X, RC=0x\%08X' </tt>
**  \event <tt> 'Error unsubscribing watchpoint: MID=0x\%04X, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an error is encountered
**  unsubscribing to a watchpoint message ID
**
**  The \c MID field is the message ID, the \c RC field is the return
**  code from the #CFE_SB_Unsubscribe call that generated the error.
*/
#define LC_UNSUB_WP_ERR_EID                      44

/** \brief <tt> 'Error subscribing watchpoint: MID=0x\%04X, RC=0x\%08X' </tt>
**  \event <tt> 'Error subscribing watchpoint: MID=0x\%04X, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an error is encountered
**  when subscribing to a watchpoint message iD
**
**  The \c MID field is the message ID, the \c RC field is the return
**  code from the #CFE_SB_Subscribe call that generated the error.
*/
#define LC_SUB_WP_ERR_EID                        45

/** \brief <tt> 'WRT data NOT saved to CDS on exit, RC=0x\%08X' </tt>
**  \event <tt> 'WRT data NOT saved to CDS on exit, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the watchpoint results
**  table (WRT) data could not be saved to the CDS on application
**  exit.    
**
**  The \c RC field is the return code from the #CFE_ES_CopyToCDS
**  call that generated the error.
*/
#define LC_WRT_NO_SAVE_ERR_EID                   46

/** \brief <tt> 'ART data NOT saved to CDS on exit, RC=0x\%08X' </tt>
**  \event <tt> 'ART data NOT saved to CDS on exit, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the actionpoint results
**  table (ART) data could not be saved to the CDS on application
**  exit.    
**
**  The \c RC field is the return code from the #CFE_ES_CopyToCDS
**  call that generated the error.
*/
#define LC_ART_NO_SAVE_ERR_EID                   47

/** \brief <tt> 'Application data NOT saved to CDS on startup, RC=0x\%08X' </tt>
**  \event <tt> 'Application data NOT saved to CDS on startup, RC=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the LC application data
**  could not be saved to the CDS on application startup.
**
**  The \c RC field is the return code from the #CFE_ES_CopyToCDS
**  call that generated the error.
*/
#define LC_APP_NO_SAVE_START_ERR_EID             48

/** \brief <tt> 'Msg with unreferenced message ID rcvd: ID = 0x\%04X' </tt>
**  \event <tt> 'Msg with unreferenced message ID rcvd: ID = 0x\%04X' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when a software bus message has
**  been received that isn't a recognized LC message and has no
**  defined watchpoints referencing it's message ID
**
**  The \c ID field is the message ID
*/
#define LC_MID_INF_EID                           49

/** \brief <tt> 'WP has undefined data type: WP = \%d, DataType = \%d' </tt>
**  \event <tt> 'WP has undefined data type: WP = \%d, DataType = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued whenever an undefined watchpoint data type
**  identifier is detected
**
**  The \c WP field is the watchpoint number, the \c DataType field is
**  the data type value that triggered the error
*/
#define LC_WP_DATATYPE_ERR_EID                   50

/** \brief <tt> 'WP has invalid operator ID: WP = \%d, OperID = \%d' </tt>
**  \event <tt> 'WP has invalid operator ID: WP = \%d, OperID = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued whenever an undefined watchpoint
**  operator identifier is detected
**
**  The \c WP field is the watchpoint number, the \c OperID field is
**  the operator ID value that triggered the error
*/
#define LC_WP_OPERID_ERR_EID                     51

/** \brief <tt> 'WP data value is a float NAN: WP = \%d, Value = 0x\%08X' </tt>
**  \event <tt> 'WP data value is a float NAN: WP = \%d, Value = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a watchpoint is defined as a float
**  type, but the extracted value would equate to a floating point
**  NAN (not-a-number) value
**
**  The \c WP field is the watchpoint number, the \c Value field is
**  the watchpoint value that triggered the error displayed as a 32 
**  bit hexidecimal number.
*/
#define LC_WP_NAN_ERR_EID                        52

/** \brief <tt> 'WP offset error: MID = \%d, WP = \%d, Offset = \%d, DataSize = \%d, MsgLen = \%d' </tt>
**  \event <tt> 'WP offset error: MID = \%d, WP = \%d, Offset = \%d, DataSize = \%d, MsgLen = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a watchpoint offset value extends past
**  the end of the message as reported by the #CFE_SB_GetTotalMsgLength
**  function
**
**  The \c MID field is the message ID, the \c WP field is the watchpoint 
**  number, the \c Offset field is the watchpoint offset, the \c DataSize 
**  field is the size of the watchpoint data in bytes, the \c MsgLen field 
**  is the reported message length from #CFE_SB_GetTotalMsgLength.
*/
#define LC_WP_OFFSET_ERR_EID                     53

/** \brief <tt> 'WDT verify float err: WP = \%d, Err = \%d, ComparisonValue = 0x\%08X' </tt>
**  \event <tt> 'WDT verify float err: WP = \%d, Err = \%d, ComparisonValue = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation 
**  fails for a watchpoint definition table (WDT) load and the error is
**  a failed floating point check. This error is caused when the data type
**  for a wachpoint definition is floating point and the comparison value 
**  equates to a floating point NAN (not-a-number) or infinite value.
**
**  The \c WP field is the watchpoint number, the \c Err field
**  is an error identifier, the \c ComparisonValue field contains
**  the data that triggered the error displayed as a 32 bit hexadecimal
**  number
*/
#define LC_WDTVAL_FPERR_EID                      54

/** \brief <tt> 'WDT verify err: WP = \%d, Err = \%d, DType = \%d, Oper = \%d, MID = \%d' </tt>
**  \event <tt> 'WDT verify err: WP = \%d, Err = \%d, DType = \%d, Oper = \%d, MID = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation 
**  fails for a watchpoint definition table (WDT) load and the error is
**  NOT a failed floating point check. 
**
**  The \c WP field is the watchpoint number and the other fields are
**  from that watchpoint's definition table entry that failed validation
*/
#define LC_WDTVAL_ERR_EID                        55

/** \brief <tt> 'WDT verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'WDT verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when a table validation has been 
**  completed for a watchpoint definition table (WDT) load 
**
**  The \c good field is number of entries that passed, the \c bad field
**  is number of entries that failed, the \c unused field is the 
**  number of entries that weren't checked because they were 
**  marked unused.
*/
#define LC_WDTVAL_INF_EID                        56

/** \brief <tt> 'Sample AP error, invalid current AP state: AP = \%d, State = \%d' </tt>
**  \event <tt> 'Sample AP error, invalid current AP state: AP = \%d, State = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the #LC_SAMPLE_AP_MID message
**  has been received and the current state for the specified 
**  actionpoint state is either #LC_ACTION_NOT_USED or #LC_APSTATE_PERMOFF.
**
**  The \c AP field is the actionpoint number, the \c state field is the
**  current state of the actionpoint
*/
#define LC_APSAMPLE_CURR_ERR_EID                 57

/** \brief <tt> 'AP state change from PASS to FAIL: AP = \%d' </tt>
**  \event <tt> 'AP state change from PASS to FAIL: AP = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when an actionpoint evaluation transitions
**  from #LC_ACTION_PASS to #LC_ACTION_FAIL
**
**  The \c AP field is the actionpoint number that transitioned.
*/
#define LC_AP_PASSTOFAIL_INF_EID                 58

/** \brief <tt> 'AP failed while LC App passive: AP = \%d, FailCount = \%d, RTS = \%d' </tt>
**  \event <tt> 'AP failed while LC App passive: AP = \%d, FailCount = \%d, RTS = \%d' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an actionpoint fails evaluation while
**  the LC task operating state is #LC_STATE_PASSIVE
**
**  The \c AP field is the actionpoint number, the \c FailCount field is how
**  many times this actionpoint has failed, the \c RTS field is the RTS 
**  that wasn't initiated because LC was passive.
*/
#define LC_PASSIVE_FAIL_DBG_EID                  59

/** \brief <tt> 'AP failed while passive: AP = \%d, FailCount = \%d, RTS = \%d' </tt>
**  \event <tt> 'AP failed while passive: AP = \%d, FailCount = \%d, RTS = \%d' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when an actionpoint fails evaluation while
**  the actionpoint state is #LC_APSTATE_PASSIVE
**
**  The \c AP field is the actionpoint number, the \c FailCount field is how
**  many times this actionpoint has failed, the \c RTS field is the RTS 
**  that wasn't initiated because the actionpoint was passive.
*/
#define LC_AP_PASSIVE_FAIL_INF_EID               60

/** \brief <tt> 'AP state change from FAIL to PASS: AP = \%d' </tt>
**  \event <tt> 'AP state change from FAIL to PASS: AP = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when an actionpoint evaluation transitions
**  from #LC_ACTION_FAIL to #LC_ACTION_PASS
**
**  The \c AP field is the actionpoint number that transitioned.
*/
#define LC_AP_FAILTOPASS_INF_EID                 61

/** \brief <tt> 'AP evaluated to error: AP = \%d, Result = \%d' </tt>
**  \event <tt> 'AP evaluated to error: AP = \%d, Result = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when an actionpoint evaluation
**  results in #LC_ACTION_ERROR
**
**  The \c AP field is the actionpoint number, the \c Result field
**  is the evaluation value that triggered the error
*/
#define LC_ACTION_ERROR_ERR_EID                  62

/** \brief <tt> 'AP has illegal RPN expression: AP = \%d, LastOperand = \%d, StackPtr = \%d' </tt>
**  \event <tt> 'AP has illegal RPN expression: AP = \%d, LastOperand = \%d, StackPtr = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an illegal Reverse Polish Notation
**  (RPN) expression is detected during an actionpoint evaluation
**
**  The \c AP field is the actionpoint number, the \c LastOperand field
**  is the operand when the error occured, the \c StackPtr field
**  is the value of the equation stack pointer when the error occured.
*/
#define LC_INVALID_RPN_ERR_EID                   63

/** \brief <tt> 'ADT verify RPN err: AP = \%d, Index = \%d, StackDepth = \%d' </tt>
**  \event <tt> 'ADT verify RPN err: AP = \%d, Index = \%d, StackDepth = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation 
**  fails for a actionpoint definition table (ADT) load and the error is
**  a failed RPN equation check. 
**
**  The \c AP field is the watchpoint number, the \c Index field is the 
**  index into the equation where the error occurred, and the \c StackDepth 
**  field contains the RPN stack index when the error occurred.
*/
#define LC_ADTVAL_RPNERR_EID                     64

/** \brief <tt> 'ADT verify err: AP = \%d, Err = \%d, State = \%d, RTS = \%d, FailCnt = \%d, EvtType = \%d' </tt>
**  \event <tt> 'ADT verify err: AP = \%d, Err = \%d, State = \%d, RTS = \%d, FailCnt = \%d, EvtType = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued on the first error when a table validation 
**  fails for an actioinpoint definition table (ADT) load and the error is
**  NOT a failed RPN equation check. 
**
**  The \c AP field is the actionpoint number and the other fields are
**  from that actionpoint's definition table entry that failed validation
*/
#define LC_ADTVAL_ERR_EID                        65

/** \brief <tt> 'ADT verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'ADT verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when a table validation has been 
**  completed for an actionpoint definition table (ADT) load 
**
**  The \c good field is number of entries that passed, the \c bad field
**  is number of entries that failed, the \c unused field is the 
**  number of entries that weren't checked because they were 
**  marked unused.
*/
#define LC_ADTVAL_INF_EID                        66

/** \brief <tt> 'Unexpected LC_CustomFunction call: WP = \%d' </tt>
**  \event <tt> 'Unexpected LC_CustomFunction call: WP = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the mission specific custom
**  function /LC_CustomFunction is called with an unexpected
**  watchpoint ID
**
**  The \c WP field is the watchpoint number that generated the
**  call
*/
#define LC_CFCALL_ERR_EID                        67

/** \brief <tt> 'Base event ID for events defined in the Actionpoint Definition Table' </tt>
**  
**  \par Type: User defined in Actionpoint Definition Table
**
**  \par Cause:
**
**  The actionpoint base event ID is designed to avoid conflicts
**  between the event ID's defined above for use by the LC application
**  and the user defined event ID's in the actionpoint table.
**
**  These events are generated when the evaluation of an actionpoint
**  results in sending a command to the stored command (SC) processor
**  to start a real time command sequence (RTS).  The event text is
**  user defined and specific to the particular actionpoint.
**
**  Note that user defined event ID's can be easily recognized if the
**  base number is easily recognizable.  For example, using the value
**  1000 for the base event ID and using the actionpoint table index as
**  the offset portion creates an obvious correlation.  Thus, if an LC
**  event ID is 1025 then it is immediately apparent that the event is
**  the user defined event for actionpoint table index 25.
*/
#define LC_BASE_AP_EID                           1000

#endif /* _lc_events_ */

/************************/
/*  End of File Comment */
/************************/
