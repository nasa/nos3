/*
**  File: 
**  cfe_es_msg.h
**  $Id: cfe_es_msg.h 1.13 2014/08/19 13:30:55GMT-05:00 sstrege Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**  Purpose:
**  cFE Executive Services (ES) Command and Telemetry packet definition file.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**
** $Log: cfe_es_msg.h  $
** Revision 1.13 2014/08/19 13:30:55GMT-05:00 sstrege 
** Fixed doxygen warning - updated reference to OS_MAX_PRIORITY to MAX_PRIORITY
** Revision 1.12 2012/01/18 16:28:56EST jmdagost 
** Replaced Padding HK telemetry points with OSAL revision and mission revision.
** Revision 1.11 2012/01/13 12:32:14EST acudmore 
** Changed license text to reflect open source
** Revision 1.10 2012/01/10 13:35:13EST lwalling 
** Add output filename to shell command packet structure
** Revision 1.9 2011/01/18 13:42:37EST lwalling 
** Fix incorrect structure name in Doxygen comment
** Revision 1.8 2010/11/24 09:18:25EST jmdagost 
** Removed unneeded padding byte from housekeeping packet.
** Revision 1.7 2010/11/23 15:29:23EST jmdagost 
** Added cFE Mission Revision telemetry point in Housekeeping, changed cFE SubMinor Version to cFE Revision.
** Revision 1.6 2010/11/04 17:16:01EDT jmdagost 
** Added TriggerMode parameter to Performance Analyzer Start cmd, updated documentation.
** Revision 1.5 2010/11/04 14:20:20EDT acudmore 
** Changed comments to reflect RAM disk path configuration option.
** Revision 1.4 2009/08/04 13:34:17EDT aschoeni 
** Added heap information to ES telemetry
** Revision 1.3 2008/12/08 12:06:54EST dkobe 
** Updates to correct doxygen errors
** Revision 1.2 2008/06/20 15:40:28EDT apcudmore 
** Added support for OSAL based module loader
**  - Removed calls and references to the BSP based module loader
** Revision 1.1 2008/04/17 08:05:20EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.16 2007/09/27 15:54:57EDT David Kobe (dlkobe) 
** Added doxygen comments for commands and telemetry
** Revision 1.15 2007/09/25 14:38:53EDT David Kobe (dlkobe) 
** Added Criticality note to Memory Pool Stats command description.
** Revision 1.14 2007/09/20 10:52:57EDT apcudmore 
** Added Query All Tasks command, file definition, events, error codes etc.
** Revision 1.13 2007/06/07 16:03:55EDT dlkobe 
** Corrected doxygen comment syntax errors
** Revision 1.12 2007/06/07 13:07:48EDT dlkobe 
** Added Command and Telemetry doxygen comments to TIME, EVS and SB
** Revision 1.11 2007/06/02 10:10:13EDT dlkobe 
** Added doxygen comments for User's Guides
** Revision 1.10 2007/05/09 12:41:44EDT dlkobe 
** Added command structure for Dump CDS Registry Command
** Revision 1.8 2007/05/04 20:07:52EDT dlkobe 
** Added command to telemeter memory pool statistics
** Revision 1.7 2007/04/25 11:57:04EDT apcudmore 
** Implemented "Back End" of Application Restart and Delete.
** Changes include all ES Cleanup code to delete App and Task Resources.
** Revision 1.6 2007/04/20 14:22:42EDT rjmcgraw 
** Renamed performance structs for consistency
**
*/
/*************************************************************************/
#ifndef _cfe_es_msg_
#define _cfe_es_msg_

/*
** Includes
*/
#include "cfe.h"
#include "cfe_es.h"

/*
** ES task command packet command codes
*/
/** \name Executive Services Command Codes */
/** \{ */
/** \cfeescmd Executive Services No-Op
**
**  \par Description
**       This command performs no other function than to increment the 
**       command execution counter. The command may be used to verify 
**       general aliveness of the Executive Services task.
**
**  \cfecmdmnemonic \ES_NOOP
**
**  \par Command Structure
**       #CFE_ES_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_NOOP_INF_EID informational event message will 
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_LEN_ERR_EID error event message will be generated
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_ES_NOOP_CC                 0

/** \cfeescmd Executive Services Reset Counters
**
**  \par Description
**       This command resets the following counters within the Executive 
**       Services housekeeping telemetry:
**       - Command Execution Counter
**       - Command Error Counter
**
**  \cfecmdmnemonic \ES_RESETCTRS
**
**  \par Command Structure
**       #CFE_ES_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_RESET_INF_EID informational event message will be 
**         generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_LEN_ERR_EID error event message will be generated
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is 
**       possible for ground systems and on-board safing procedures 
**       to be designed such that they react to changes in the counter 
**       values that are reset by this command.
**
**  \sa  #CFE_ES_RESET_PR_COUNT_CC
*/
#define CFE_ES_RESET_CC                1

/** \cfeescmd Executive Services Processor / Power-On Reset
**
**  \par Description
**       This command restarts the cFE in one of two modes.  The Power-On Reset
**       will cause the cFE to restart as though the power were first applied
**       to the processor.  The Processor Reset will attempt to retain the contents
**       of the volatile disk and the contents of the Critical Data Store.
**       NOTE: If a requested Processor Reset should cause the 
**       Processor Reset Counter (\b \c \ES_PROCRESETCNT)
**       to exceed OR EQUAL the limit #CFE_ES_MAX_PROCESSOR_RESETS (which is reported in
**       housekeeping telemetry as \b \c \ES_MAXPROCRESETS), 
**       the command is \b AUTOMATICALLY upgraded to a Power-On Reset.
**
**  \cfecmdmnemonic \ES_RESET
**
**  \par Command Structure
**       #CFE_ES_RestartCmd_t
**
**  \par Command Verification
**       Successful execution of this command (as a Processor Reset)  
**       may be verified with the following telemetry:
**       - \b \c \ES_PROCRESETCNT - processor reset counter will increment
**       - New entries in the Exception Reset Log and System Log can be found<BR>
**       NOTE: Verification of a Power-On Reset is shown through the loss of
**       data nominally retained through a Processor Reset<BR>
**       NOTE: Since the reset of the processor resets the command execution
**       counter (\b \c \ES_CMDPC), this counter \b CANNOT be used to verify
**       command execution.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The \link #CFE_ES_RestartCmd_t.RestartType Restart Type \endlink was
**         not a recognized value. 
**       
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_BOOT_ERR_EID error event message will be generated
**
**  \par Criticality
**       This command is, by definition, dangerous.  Significant loss of
**       data will occur.  All processes and the cFE itself will be stopped
**       and restarted.  With the Power-On reset option, all data on the
**       volatile disk and the contents of the Critical Data Store will be
**       lost.
**
**  \sa  #CFE_ES_RESET_PR_COUNT_CC, #CFE_ES_SET_MAX_PR_COUNT_CC
*/
#define CFE_ES_RESTART_CC              2

/** \cfeescmd Executive Services O/S Shell Command
**
**  \par Description
**       This command passes an ASCII string as a command line to the
**       underlying realtime operating system shell.  Any response to
**       the command is both written to the shell command output file
**       and sent as a series of shell command output telemetry packets.
**
**       If the shell command output filename argument is empty, then
**       #CFE_ES_DEFAULT_SHELL_FILENAME will be used as the filename.
**
**  \cfecmdmnemonic \ES_SHELL
**
**  \par Command Structure
**       #CFE_ES_ShellCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_SHELL_INF_EID informational event message will be 
**         generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - Failure to create the shell command output file
**       - The shell command started with <tt> ES_ </tt> but was not one of the
**         recognized cFE shell commands
**       - There was an error while performing a #OS_lseek on the shell command output file
**       - There was an error while redirecting the shell command response to the
**         shell command output file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - the #CFE_ES_SHELL_ERR_EID error event message will be generated
**       - Additional information on the error should be found in the System Log
**
**  \par Criticality
**       This command should be used with caution.  Interfering with the
**       operation of the underlying realtime operating system can cause
**       significant problems.
**
**  \sa 
*/
#define CFE_ES_SHELL_CMD_CC            3

/** \cfeescmd Load and Start an Application
**
**  \par Description
**       This command starts the specified application with the 
**       specified start address, stack size, etc options.
**
**  \cfecmdmnemonic \ES_STARTAPP
**
**  \par Command Structure
**       #CFE_ES_StartAppCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_START_INF_EID informational event message will be 
**         generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified application filename string is either a NULL string
**         or less than four characters in length
**       - The specified application entry point is a NULL string
**       - The specified application name is a NULL string
**       - The specified stack size is less than #CFE_ES_DEFAULT_STACK_SIZE
**       - The specified priority is greater than MAX_PRIORITY (as defined in osapi.c)
**       - The specified exception action is neither #CFE_ES_APP_EXCEPTION_RESTART_APP (0) or
**         #CFE_ES_APP_EXCEPTION_PROC_RESTART (1)
**       - The Operating System was unable to load the specified application file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous although system resources
**       could be taxed beyond their limits with the starting of erroneous
**       or invalid applications.
**
**  \sa #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_START_APP_CC            4

/** \cfeescmd Stop and Unload Application
**
**  \par Description
**       This command halts and removes the specified Application
**       from the system. \b NOTE: This command should never be used
**       on the Command Ingest application.  This would prevent further
**       commands from entering the system.  If Command Ingest needs to
**       be stopped and restarted, use #CFE_ES_RESTART_APP_CC or 
**       #CFE_ES_RELOAD_APP_CC.
**
**  \cfecmdmnemonic \ES_STOPAPP
**
**  \par Command Structure
**       #CFE_ES_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_STOP_DBG_EID debug event message will be 
**         generated. NOTE: This event message only identifies that the
**         stop has been started, not that is has completed.
**       - Once the stop has successfully completed, the list of Applications
**         and Tasks created in response to the \b \c \ES_WRITEAPPINFO2FILE, 
**         \b \c \ES_WRITETASKINFO2FILE should no longer contain the 
**         specified application.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the removal of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_STOP_APP_CC             5

/** \cfeescmd Stops and Restarts an Application
**
**  \par Description
**       This command halts and restarts the specified Application.
**       This command does \b NOT reload the application from the onboard
**       filesystem.
**
**  \cfecmdmnemonic \ES_RESTARTAPP
**
**  \par Command Structure
**       #CFE_ES_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_RESTART_APP_DBG_EID debug event message will be 
**         generated. NOTE: This event message only identifies that the
**         act of stopping the application has begun, not that is has completed.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the restarting of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_STOP_APP_CC, #CFE_ES_RELOAD_APP_CC
*/
#define CFE_ES_RESTART_APP_CC          6

/** \cfeescmd Stops, Unloads, Loads from a File and Restarts an Application
**
**  \par Description
**       This command halts and removes the specified Application
**       from the system.  Then it immediately loads the Application from
**       the command specified file and restarts it.  This command is 
**       especially useful for restarting a Command Ingest Application
**       since once it has been stopped, no further commands can come in
**       to restart it.  
**
**  \cfecmdmnemonic \ES_RELOADAPP
**
**  \par Command Structure
**       #CFE_ES_AppReloadCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_RELOAD_APP_DBG_EID debug event message will be 
**         generated. NOTE: This event message only identifies that the
**         act of stopping the application has begun, not that is has completed.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified application name is not recognized as an active application
**       - The specified application is one of the cFE's Core applications (ES, EVS, SB, TBL, TIME)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**       - Additional information on the reason for command failure may be found
**         in the System Log
**
**  \par Criticality
**       This command is not inherently dangerous, however the restarting of certain
**       applications (e.g. - Spacecraft Attitude and Control) may have a detrimental effect
**       on the spacecraft.
**
**  \sa #CFE_ES_START_APP_CC, #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC
*/
#define CFE_ES_RELOAD_APP_CC           7

/** \cfeescmd Request Executive Services Information on a Specified Application
**
**  \par Description
**       This command takes the information kept by Executive Services on the
**       specified application and telemeters it to the ground.
**
**  \cfecmdmnemonic \ES_QUERYAPP
**
**  \par Command Structure
**       #CFE_ES_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_ONE_APP_EID debug event message will be 
**         generated. NOTE: This event message only identifies that the
**         act of stopping the application has begun, not that is has completed.
**       - Receipt of the #CFE_ES_OneAppTlm_t telemetry packet
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified application name is not recognized as an active application
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       None
**
**  \sa #CFE_ES_QUERY_ALL_CC, #CFE_ES_QUERY_ALL_TASKS_CC
*/
#define CFE_ES_QUERY_ONE_CC            8

/** \cfeescmd Writes all Executive Services Information on All Applications to a File
**
**  \par Description
**       This command takes the information kept by Executive Services on all of the
**       registered applications and writes it to the specified file.
**
**  \cfecmdmnemonic \ES_WRITEAPPINFO2FILE
**
**  \par Command Structure
**       #CFE_ES_QueryAllCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_ALL_APPS_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_APP_LOG_FILE configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system (or overwrite an existing one) and could, 
**       if performed repeatedly without sufficient file management by the 
**       operator, fill the file system.
**
**  \sa #CFE_ES_QUERY_ONE_CC, #CFE_ES_QUERY_ALL_TASKS_CC
*/
#define CFE_ES_QUERY_ALL_CC            9

/** \cfeescmd Clear Executive Services System Log
**
**  \par Description
**       This command clears the contents of the Executive Services System Log.
**
**  \cfecmdmnemonic \ES_CLEARSYSLOG
**
**  \par Command Structure
**       #CFE_ES_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_SYSLOG1_INF_EID informational event message will be 
**         generated.
**       - \b \c \ES_SYSLOGBYTEUSED - System Log Bytes Used will go to zero
**       - \b \c \ES_SYSLOGENTRIES - Number of System Log Entries will go to zero
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not dangerous.  However, any previously logged data
**       will be lost.
**
**  \sa #CFE_ES_WRITE_SYSLOG_CC, #CFE_ES_CLEAR_ERLOG_CC, #CFE_ES_WRITE_ERLOG_CC,
**      #CFE_ES_OVERWRITE_SYSLOG_CC
*/
#define CFE_ES_CLEAR_SYSLOG_CC        10  

/** \cfeescmd Writes contents of Executive Services System Log to a File
**
**  \par Description
**       This command causes the contents of the Executive Services System Log
**       to be written to a log file.
**
**  \cfecmdmnemonic \ES_WRITESYSLOG2FILE
**
**  \par Command Structure
**       #CFE_ES_WriteSyslogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_SYSLOG2_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_SYSLOG_FILE configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system (or overwrite an existing one) and could, 
**       if performed repeatedly without sufficient file management by the 
**       operator, fill the file system.
**
**  \sa #CFE_ES_CLEAR_SYSLOG_CC, #CFE_ES_CLEAR_ERLOG_CC, #CFE_ES_WRITE_ERLOG_CC,
**      #CFE_ES_OVERWRITE_SYSLOG_CC
*/
#define CFE_ES_WRITE_SYSLOG_CC        11  

/** \cfeescmd Clears the contents of the Exeception and Reset Log
**
**  \par Description
**       This command causes the contents of the Executive Services Exception
**       and Reset Log to be cleared.
**
**  \cfecmdmnemonic \ES_CLEARERLOG
**
**  \par Command Structure
**       #CFE_ES_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_ERLOG1_INF_EID informational event message will be 
**         generated.
**       - \b \c \ES_ERLOGINDEX - Index into Exception Reset Log goes to zero
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not dangerous.  However, any previously logged data
**       will be lost.
**
**  \sa #CFE_ES_CLEAR_SYSLOG_CC, #CFE_ES_WRITE_SYSLOG_CC, #CFE_ES_WRITE_ERLOG_CC
*/
#define CFE_ES_CLEAR_ERLOG_CC         12  

/** \cfeescmd Writes Exeception and Reset Log to a File
**
**  \par Description
**       This command causes the contents of the Executive Services Exception
**       and Reset Log to be written to the specified file.
**
**  \cfecmdmnemonic \ES_WRITEERLOG2FILE
**
**  \par Command Structure
**       #CFE_ES_WriteERlogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_ERLOG2_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_ER_LOG_FILE configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system (or overwrite an existing one) and could, 
**       if performed repeatedly without sufficient file management by the 
**       operator, fill the file system.
**
**  \sa #CFE_ES_CLEAR_SYSLOG_CC, #CFE_ES_WRITE_SYSLOG_CC, #CFE_ES_CLEAR_ERLOG_CC
*/
#define CFE_ES_WRITE_ERLOG_CC         13  

/** \cfeescmd Start Performance Analyzer
**
**  \par Description
**       This command causes the Performance Analyzer to begin collecting data using the specified trigger mode.
**
**  \cfecmdmnemonic \ES_STARTLADATA
**
**  \par Command Structure
**       #CFE_ES_PerfStartCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_PERFSTATE - Current performance analyzer state will change to
**         either WAITING FOR TRIGGER or, if conditions are appropriate fast enough,
**         TRIGGERED.
**       - \b \c \ES_PERFMODE - Performance Analyzer Mode will change to the commanded trigger mode (TRIGGER START, 
**         TRIGGER CENTER, or TRIGGER END).
**       - \b \c \ES_PERFTRIGCNT - Performance Trigger Count will go to zero
**       - \b \c \ES_PERFDATASTART - Data Start Index will go to zero
**       - \b \c \ES_PERFDATAEND - Data End Index will go to zero
**       - \b \c \ES_PERFDATACNT - Performance Data Counter will go to zero
**       - The #CFE_ES_PERF_STARTCMD_EID debug event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - A previous #CFE_ES_PERF_STOPDATA_CC command has not completely finished.
**       - An invalid trigger mode is requested.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous but may cause a small increase in CPU
**       utilization as the performance analyzer data is collected.
**
**  \sa #CFE_ES_PERF_STOPDATA_CC, #CFE_ES_PERF_SETFILTERMASK_CC, #CFE_ES_PERF_SETTRIGMASK_CC
*/
#define CFE_ES_PERF_STARTDATA_CC      14  

/** \cfeescmd Stop Performance Analyzer
**
**  \par Description
**       This command stops the Performance Analyzer from collecting any more data.
**
**  \cfecmdmnemonic \ES_STOPLADATA
**
**  \par Command Structure
**       #CFE_ES_PerfStopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_PERFSTATE - Current performance analyzer state will change to
**         IDLE. 
**       - The #CFE_ES_PERF_STOPCMD_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_PERF_DUMP_FILENAME configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - A previous Stop Performance Analyzer command is still in process
**       - An error occurred while spawning the child task responsible for
**         dumping the Performance Analyzer data to a file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  An additional low priority child
**       task will be spawned, however, to dump the performance analyzer data to a file.
**
**  \sa #CFE_ES_PERF_STARTDATA_CC, #CFE_ES_PERF_SETFILTERMASK_CC, #CFE_ES_PERF_SETTRIGMASK_CC
*/
#define CFE_ES_PERF_STOPDATA_CC       15                

/** \cfeescmd Set Performance Analyzer's Filter Masks
**
**  \par Description
**       This command sets the Performance Analyzer's Filter Masks.
**
**  \cfecmdmnemonic \ES_LAFILTERMASK
**
**  \par Command Structure
**       #CFE_ES_PerfSetFilterMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \B \C \ES_PERFFLTRMASK - the current performance filter mask
**         value(s) should reflect the commanded value
**       - The #CFE_ES_PERF_FILTMSKCMD_EID debug event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The Filter Mask ID number is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       Changing the filter masks may cause a small change in the Performance
**       Analyzer's CPU utilization.
**
**  \sa #CFE_ES_PERF_STARTDATA_CC, #CFE_ES_PERF_STOPDATA_CC, #CFE_ES_PERF_SETTRIGMASK_CC
*/
#define CFE_ES_PERF_SETFILTERMASK_CC  16  

/** \cfeescmd Set Performance Analyzer's Trigger Masks
**
**  \par Description
**       This command sets the Performance Analyzer's Trigger Masks.
**
**  \cfecmdmnemonic \ES_LATRIGGERMASK
**
**  \par Command Structure
**       #CFE_ES_PerfSetTrigMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_PERFTRIGMASK - the current performance trigger mask
**         value(s) should reflect the commanded value
**       - The #CFE_ES_PERF_TRIGMSKCMD_EID debug event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The Trigger Mask ID number is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       Changing the trigger masks may cause a small change in the Performance
**       Analyzer's CPU utilization.
**
**  \sa #CFE_ES_PERF_STARTDATA_CC, #CFE_ES_PERF_STOPDATA_CC, #CFE_ES_PERF_SETFILTERMASK_CC
*/
#define CFE_ES_PERF_SETTRIGMASK_CC    17  

/** \cfeescmd Set Executive Services System Log Mode to Discard/Overwrite
**
**  \par Description
**       This command allows the user to configure the Executive Services
**       to either discard new System Log messages when it is full or to
**       overwrite the oldest messages.
**
**  \cfecmdmnemonic \ES_OVERWRITESYSLOGMODE
**
**  \par Command Structure
**       #CFE_ES_OverWriteSysLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_SYSLOGMODE - Current System Log Mode should reflect
**         the commanded value
**       - The #CFE_ES_SYSLOGMODE_EID debug event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The desired mode is neither #CFE_ES_LOG_OVERWRITE or #CFE_ES_LOG_DISCARD
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       None. (It should be noted that "Overwrite" mode would allow a message
**       identifying the cause of a problem to be lost by a subsequent flood of
**       additional messages).
**
**  \sa #CFE_ES_CLEAR_SYSLOG_CC, #CFE_ES_WRITE_SYSLOG_CC
*/
#define CFE_ES_OVERWRITE_SYSLOG_CC    18

/** \cfeescmd Resets the Processor Reset Counter to Zero
**
**  \par Description
**       This command allows the user to reset the Processor Reset Counter to zero.
**       The Processor Reset Counter counts the number of Processor Resets that 
**       have occurred so as to identify when a Processor Reset should automatically
**       be upgraded to a full Power-On Reset.
**
**  \cfecmdmnemonic \ES_RESETPRCNT
**
**  \par Command Structure
**       #CFE_ES_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_PROCRESETCNT - Current number of processor resets will go to zero
**       - The #CFE_ES_RESET_PR_COUNT_EID informational event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not critical.  The only impact would be that the system
**       would have to have more processor resets before an automatic power-on reset
**       occurred.
**
**  \sa #CFE_ES_SET_MAX_PR_COUNT_CC, #CFE_ES_RESET_CC
*/
#define CFE_ES_RESET_PR_COUNT_CC      19

/** \cfeescmd Configure the Maximum Number of Processor Resets before a Power-On Reset
**
**  \par Description
**       This command allows the user to specify the number of Processor Resets that
**       are allowed before the next Processor Reset is upgraded to a Power-On Reset.
**
**  \cfecmdmnemonic \ES_SETMAXPRCNT
**
**  \par Command Structure
**       #CFE_ES_SetMaxPRCountCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - \b \c \ES_MAXPROCRESETS - Current maximum number of processor resets 
**         before an automatic power-on reset will go to the command specified value.
**       - The #CFE_ES_SET_MAX_PR_COUNT_EID informational event message will be 
**         generated.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       If the operator were to set the Maximum Processor Reset Count to too high a value,
**       the processor would require an inordinate number of consecutive processor resets
**       before an automatic power-on reset would occur.  This could potentially leave the
**       spacecraft without any control for a significant amount of time if a processor reset
**       fails to clear a problem.
**
**  \sa #CFE_ES_RESET_PR_COUNT_CC
*/
#define CFE_ES_SET_MAX_PR_COUNT_CC    20

/** \cfeescmd Delete Critical Data Store
**
**  \par Description
**       This command allows the user to delete a Critical Data Store that was created
**       by an Application that is now no longer executing.
**
**  \cfecmdmnemonic \ES_DELETECDS
**
**  \par Command Structure
**       #CFE_ES_DeleteCDSCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_CDS_DELETED_INFO_EID informational event message will be 
**         generated.
**       - The specified CDS should no longer appear in a CDS Registry dump generated
**         upon receipt of the #CFE_ES_DUMP_CDS_REG_CC command
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified CDS is the CDS portion of a Critical Table. See #CFE_TBL_DELETE_CDS_CC.
**       - The specified CDS is not found in the CDS Registry
**       - The specified CDS is associated with an Application that is still active
**       - An error occurred while accessing the CDS memory (see the System Log for more details)
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not critical because it is not possible to delete a CDS that is
**       associated with an active application.  However, deleting a CDS does eliminate
**       any "history" that an application may be wishing to keep.
**
**  \sa #CFE_ES_DUMP_CDS_REG_CC, #CFE_TBL_DELETE_CDS_CC
*/
#define CFE_ES_DELETE_CDS_CC          21

/** \cfeescmd Telemeter Memory Pool Statistics
**
**  \par Description
**       This command allows the user to obtain a snapshot of the statistics maintained
**       for a specified memory pool.
**
**  \cfecmdmnemonic \ES_TLMPOOLSTATS
**
**  \par Command Structure
**       #CFE_ES_TlmPoolStatsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_TLM_POOL_STATS_INFO_EID debug event message will be 
**         generated.
**       - The \link #CFE_ES_PoolStatsTlm_t Memory Pool Statistics Telemetry Packet \endlink
**         is produced
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - The specified handle is not associated with a known memory pool
**       - The specified handle caused a processor exception because it improperly
**         identified a segment of memory
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       <B>An incorrect Memory Pool Handle value can cause a system crash.</B>
**       Extreme care should be taken to ensure the memory handle value
**       used in the command is correct.
**
**  \sa 
*/
#define CFE_ES_TLM_POOL_STATS_CC      22

/** \cfeescmd Dump Critical Data Store Registry to a File
**
**  \par Description
**       This command allows the user to dump the Critical Data Store
**       Registry to an onboard file.
**
**  \cfecmdmnemonic \ES_DUMPCDSREG
**
**  \par Command Structure
**       #CFE_ES_DumpCDSRegCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_CDS_REG_DUMP_INF_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_CDS_REG_DUMP_FILE configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Error occurred while trying to create the dump file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system (or overwrite an existing one) and could, 
**       if performed repeatedly without sufficient file management by the 
**       operator, fill the file system.
**
**  \sa  #CFE_ES_DELETE_CDS_CC, #CFE_TBL_DELETE_CDS_CC
*/
#define CFE_ES_DUMP_CDS_REG_CC        23

/** \cfeescmd Writes a list of All Executive Services Tasks to a File
**
**  \par Description
**       This command takes the information kept by Executive Services on all of the
**       registered tasks and writes it to the specified file.
**
**  \cfecmdmnemonic \ES_WRITETASKINFO2FILE
**
**  \par Command Structure
**       #CFE_ES_QueryAllTasksCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \ES_CMDPC - command execution counter will 
**         increment
**       - The #CFE_ES_TASKINFO_EID debug event message will be 
**         generated.
**       - The file specified in the command (or the default specified
**         by the #CFE_ES_DEFAULT_TASK_LOG_FILE configuration parameter) will be 
**         updated with the lastest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The command packet length is incorrect
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \ES_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error 
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new 
**       file in the file system (or overwrite an existing one) and could, 
**       if performed repeatedly without sufficient file management by the 
**       operator, fill the file system.
**
**  \sa #CFE_ES_QUERY_ALL_CC, CFE_ES_QUERY_ONE_CC
*/
#define CFE_ES_QUERY_ALL_TASKS_CC     24


/** \} */


/*************************************************************************/
/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The No-Op Command (For details, see #CFE_ES_NOOP_CC)
** -# The Reset Counters Command (For details, see #CFE_ES_RESET_CC)
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];     /**< \brief cFE Software Bus Command Message Header */

} CFE_ES_NoArgsCmd_t;

/**
** \brief Reset cFE Command
**
** For command details, see #CFE_ES_RESTART_CC
**
**/
typedef struct
{
  uint16                RestartType;                       /**< \brief #CFE_ES_PROCESSOR_RESET=Processor Reset
                                                                or #CFE_ES_POWERON_RESET=Power-On Reset        */
} CFE_ES_RestartCmd_Payload_t;

typedef struct
{
    uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_RestartCmd_Payload_t Payload;
} CFE_ES_RestartCmd_t;

/**
** \brief Shell Command
**
** For command details, see #CFE_ES_SHELL_CMD_CC
**
**/
typedef struct
{
  char                  CmdString[CFE_ES_MAX_SHELL_CMD];   /**< \brief ASCII text string containing shell command
                                                                to be executed */
  char                  OutputFilename[OS_MAX_PATH_LEN];   /**< \brief Filename where shell command output is to
                                                                be written */
} CFE_ES_ShellCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_ShellCmd_Payload_t   Payload;
} CFE_ES_ShellCmd_t;

/**
** \brief Write All ES Application Info for All Applications to File Command
**
** For command details, see #CFE_ES_QUERY_ALL_CC
**
**/
typedef struct
{
  char                  QueryAllFileName[OS_MAX_PATH_LEN]; /**< \brief ASCII text string containing full path and
                                                                filename of file in which Application data is to be dumped */
} CFE_ES_QueryAllCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_QueryAllCmd_Payload_t    Payload;
} CFE_ES_QueryAllCmd_t;

/**
** \brief Write All ES Application Info for All Tasks to File Command
**
** For command details, see #CFE_ES_QUERY_ALL_TASKS_CC
**
**/
typedef struct
{
  char                  QueryAllFileName[OS_MAX_PATH_LEN]; /**< \brief ASCII text string containing full path and
                                                                filename of file in which Application data is to be dumped */

} CFE_ES_QueryAllTasksCmd_Payload_t;

typedef struct
{
    uint8                               CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_QueryAllTasksCmd_Payload_t   Payload;
} CFE_ES_QueryAllTasksCmd_t;


/**
** \brief Dump System Log to File Command
**
** For command details, see #CFE_ES_WRITE_SYSLOG_CC
**
**/
typedef struct
{
  char                  SysLogFileName[OS_MAX_PATH_LEN];  /**< \brief ASCII text string containing full path and
                                                                filename of file in which System Log is to be dumped */
} CFE_ES_WriteSyslogCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_WriteSyslogCmd_Payload_t Payload;
} CFE_ES_WriteSyslogCmd_t;

/**
** \brief Dump Exception and Reset Log to File Command
**
** For command details, see #CFE_ES_WRITE_ERLOG_CC
**
**/
typedef struct
{
  char                  ERLogFileName[OS_MAX_PATH_LEN];   /**< \brief ASCII text string containing full path and
                                                                filename of file in which ER Log is to be dumped */
} CFE_ES_WriteERlogCmd_Payload_t;

typedef struct
{
    uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_WriteERlogCmd_Payload_t Payload;
} CFE_ES_WriteERlogCmd_t;

/**
** \brief Overwrite/Discard System Log Configuration Command
**
** For command details, see #CFE_ES_OVERWRITE_SYSLOG_CC
**
**/
typedef struct 
{
   uint32               Mode;                            /**< \brief #CFE_ES_LOG_DISCARD=Throw away most recent messages,
                                                                     #CFE_ES_LOG_OVERWRITE=Overwrite oldest with most recent */

} CFE_ES_OverWriteSysLogCmd_Payload_t;

typedef struct
{
    uint8                               CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_OverWriteSysLogCmd_Payload_t Payload;
} CFE_ES_OverWriteSysLogCmd_t;

/**
** \brief Start Application Command
**
** For command details, see #CFE_ES_START_APP_CC
**
**/
typedef struct
{
  char                  Application[OS_MAX_API_NAME];    /**< \brief Name of Application to be started */
  char                  AppEntryPoint[OS_MAX_API_NAME];  /**< \brief Symbolic name of Application's entry point */
  char                  AppFileName[OS_MAX_PATH_LEN];    /**< \brief Full path and filename of Application's 
                                                                     executable image */

  uint32                StackSize;                       /**< \brief Desired stack size for the new application */

  uint16                ExceptionAction;                 /**< \brief #CFE_ES_APP_EXCEPTION_RESTART_APP=On exception, 
                                                                     restart Application,
                                                                     #CFE_ES_APP_EXCEPTION_PROC_RESTART=On exception,
                                                                     perform a Processor Reset */
  uint16                Priority;                        /**< \brief The new Applications runtime priority. */

} CFE_ES_StartAppCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_StartAppCmd_Payload_t    Payload;
} CFE_ES_StartAppCmd_t;

/**
** \brief Command Structure for Commands requiring just an Application Name
**
** For command details, see #CFE_ES_STOP_APP_CC, #CFE_ES_RESTART_APP_CC, #CFE_ES_QUERY_ONE_CC
**
**/
typedef struct
{
  char                  Application[OS_MAX_API_NAME];    /**< \brief ASCII text string containing Application Name */
} CFE_ES_AppNameCmd_Payload_t;

typedef struct
{
    uint8                       CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_AppNameCmd_Payload_t Payload;
} CFE_ES_AppNameCmd_t;

/**
** \brief Reload Application Command
**
** For command details, see #CFE_ES_RELOAD_APP_CC
**
**/
typedef struct
{
  char                  Application[OS_MAX_API_NAME];   /**< \brief ASCII text string containing Application Name */
  char                  AppFileName[OS_MAX_PATH_LEN];   /**< \brief Full path and filename of Application's 
                                                                    executable image */
} CFE_ES_AppReloadCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_AppReloadCmd_Payload_t   Payload;
} CFE_ES_AppReloadCmd_t;

/**
** \brief Set Maximum Processor Reset Count Command
**
** For command details, see #CFE_ES_SET_MAX_PR_COUNT_CC
**
**/
typedef struct
{
  uint16                MaxPRCount;                     /**< \brief New maximum number of Processor Resets before
                                                                    an automatic Power-On Reset is performed */
} CFE_ES_SetMaxPRCountCmd_Payload_t;

typedef struct
{
    uint8                               CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_SetMaxPRCountCmd_Payload_t   Payload;
} CFE_ES_SetMaxPRCountCmd_t;

/**
** \brief Delete Critical Data Store Command
**
** For command details, see #CFE_ES_DELETE_CDS_CC
**
**/
typedef struct
{
  char                  CdsName[CFE_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief ASCII text string containing name of CDS to delete */

} CFE_ES_DeleteCDSCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_DeleteCDSCmd_Payload_t   Payload;
} CFE_ES_DeleteCDSCmd_t;

/**
** \brief Start Performance Analyzer Command
**
** For command details, see #CFE_ES_PERF_STARTDATA_CC
**
**/
typedef struct
{
  uint32                TriggerMode;                    /**< \brief Desired trigger position (Start, Center, End) */
} CFE_ES_PerfStartCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_PerfStartCmd_Payload_t   Payload;
} CFE_ES_PerfStartCmd_t;

/**
** \brief Stop Performance Analyzer Command
**
** For command details, see #CFE_ES_PERF_STOPDATA_CC
**
**/
typedef struct
{
  char                  DataFileName[OS_MAX_PATH_LEN];  /**< \brief ASCII text string of full path and filename 
                                                                    of file Performance Analyzer data is to be written */
} CFE_ES_PerfStopCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_PerfStopCmd_Payload_t    Payload;
} CFE_ES_PerfStopCmd_t;


/**
** \brief Set Performance Analyzer Filter Mask Command
**
** For command details, see #CFE_ES_PERF_SETFILTERMASK_CC
**
**/
typedef struct
{
  uint32                FilterMaskNum;                   /**< \brief Index into array of Filter Masks */
  uint32                FilterMask;                      /**< \brief New Mask for specified entry in array of Filter Masks */

} CFE_ES_PerfSetFilterMaskCmd_Payload_t;

typedef struct
{
    uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_PerfSetFilterMaskCmd_Payload_t Payload;
} CFE_ES_PerfSetFilterMaskCmd_t;

/**
** \brief Set Performance Analyzer Trigger Mask Command
**
** For command details, see #CFE_ES_PERF_SETTRIGMASK_CC
**
**/
typedef struct
{
  uint32                TriggerMaskNum;                 /**< \brief Index into array of Trigger Masks */
  uint32                TriggerMask;                    /**< \brief New Mask for specified entry in array of Trigger Masks */

} CFE_ES_PerfSetTrigMaskCmd_Payload_t;

typedef struct
{
    uint8                               CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_PerfSetTrigMaskCmd_Payload_t Payload;
} CFE_ES_PerfSetTrigMaskCmd_t;

/**
** \brief Telemeter Memory Pool Statistics Command
**
** For command details, see #CFE_ES_TLM_POOL_STATS_CC
**
**/
typedef struct
{
  char                  Application[OS_MAX_API_NAME];   /**< \brief - RESERVED - should be all zeroes */
  CFE_ES_MemHandle_t    PoolHandle;                     /**< \brief Handle of Pool whose statistics are to be telemetered */

} CFE_ES_TlmPoolStatsCmd_Payload_t;

typedef struct
{
    uint8                               CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_TlmPoolStatsCmd_Payload_t    Payload;
} CFE_ES_TlmPoolStatsCmd_t;

/**
** \brief Dump CDS Registry Command
**
** For command details, see #CFE_ES_DUMP_CDS_REG_CC
**
**/
typedef struct
{
  char                  DumpFilename[OS_MAX_PATH_LEN];  /**< \brief ASCII text string of full path and filename 
                                                                    of file CDS Registry is to be written */
} CFE_ES_DumpCDSRegCmd_Payload_t;

typedef struct
{
    uint8                           CmdHeader[CFE_SB_CMD_HDR_SIZE];    /**< \brief cFE Software Bus Command Message Header */
    CFE_ES_DumpCDSRegCmd_Payload_t  Payload;

} CFE_ES_DumpCDSRegCmd_t;

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/** 
**  \cfeestlm Single Application Information Packet
**/
typedef struct
{
  CFE_ES_AppInfo_t      AppInfo;                        /**< \brief For more information, see #CFE_ES_AppInfo_t */
  
} CFE_ES_OneAppTlm_Payload_t;

typedef struct
{
    uint8                       TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_ES_OneAppTlm_Payload_t  Payload;
} CFE_ES_OneAppTlm_t;

/** 
**  \cfeestlm Memory Pool Statistics Packet
**/
typedef struct
{
  CFE_ES_MemHandle_t    PoolHandle;                     /**< \cfetlmmnemonic \ES_POOLHANDLE
                                                             \brief Handle of memory pool whose stats are being telemetered */
  CFE_ES_MemPoolStats_t PoolStats;                      /**< \brief For more info, see #CFE_ES_MemPoolStats_t */
} CFE_ES_PoolStatsTlm_Payload_t;

typedef struct
{
    uint8                           TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_ES_PoolStatsTlm_Payload_t   Payload;
} CFE_ES_PoolStatsTlm_t;

/*************************************************************************/

/** 
**  \cfeestlm Executive Services Housekeeping Packet
**/
typedef struct
{
  uint8                 CmdCounter;  /**< \cfetlmmnemonic \ES_CMDPC 
                                          \brief The ES Application Command Counter */
  uint8                 ErrCounter;  /**< \cfetlmmnemonic \ES_CMDEC 
                                          \brief The ES Application Command Error Counter */
  
  uint16                CFECoreChecksum;    /**< \cfetlmmnemonic \ES_CKSUM 
                                                 \brief Checksum of cFE Core Code */
  uint8                 CFEMajorVersion;    /**< \cfetlmmnemonic \ES_CFEMAJORVER 
                                                 \brief Major Version Number of cFE */
  uint8                 CFEMinorVersion;    /**< \cfetlmmnemonic \ES_CFEMINORVER 
                                                 \brief Minor Version Number of cFE */
  uint8                 CFERevision;        /**< \cfetlmmnemonic \ES_CFEREVISION 
                                                 \brief Sub-Minor Version Number of cFE */
  uint8                 CFEMissionRevision; /**< \cfetlmmnemonic \ES_CFEMISSIONREV 
                                                 \brief Mission Version Number of cFE */
  uint8                 OSALMajorVersion;   /**< \cfetlmmnemonic \ES_OSMAJORVER 
                                                 \brief OS Abstraction Layer Major Version Number */
  uint8                 OSALMinorVersion;   /**< \cfetlmmnemonic \ES_OSMINORVER 
                                                 \brief OS Abstraction Layer Minor Version Number */
  uint8                 OSALRevision;       /**< \cfetlmmnemonic \ES_OSREVISION 
                                                 \brief OS Abstraction Layer Revision Number */
  uint8                 OSALMissionRevision;/**< \cfetlmmnemonic \ES_OSMISSIONREV 
                                                 \brief OS Abstraction Layer MissionRevision Number */

  uint32                SysLogBytesUsed; /**< \cfetlmmnemonic \ES_SYSLOGBYTEUSED 
                                              \brief Total number of bytes used in system log */
  uint32                SysLogSize;      /**< \cfetlmmnemonic \ES_SYSLOGSIZE 
                                              \brief Total size of the system log */
  uint32                SysLogEntries;   /**< \cfetlmmnemonic \ES_SYSLOGENTRIES 
                                              \brief Number of entries in the system log */
  uint32                SysLogMode;      /**< \cfetlmmnemonic \ES_SYSLOGMODE 
                                              \brief Write/Overwrite Mode */
  
  uint32                ERLogIndex;       /**< \cfetlmmnemonic \ES_ERLOGINDEX 
                                               \brief Current index of the ER Log (wraps around) */
  uint32                ERLogEntries;     /**< \cfetlmmnemonic \ES_ERLOGENTRIES 
                                               \brief Number of entries made in the ER Log since the power on */

  uint32                RegisteredCoreApps;      /**< \cfetlmmnemonic \ES_REGCOREAPPS 
                                                      \brief Number of Applications registered with ES */
  uint32                RegisteredExternalApps;  /**< \cfetlmmnemonic \ES_REGEXTAPPS 
                                                      \brief Number of Applications registered with ES */
  uint32                RegisteredTasks;         /**< \cfetlmmnemonic \ES_REGTASKS 
                                                      \brief Number of Tasks ( main AND child tasks ) registered with ES */
  uint32                RegisteredLibs;  /**< \cfetlmmnemonic \ES_REGLIBS 
                                              \brief Number of Libraries registered with ES */

  uint32                ResetType;       /**< \cfetlmmnemonic \ES_RESETTYPE 
                                              \brief Reset type ( PROCESSOR or POWERON ) */
  uint32                ResetSubtype;    /**< \cfetlmmnemonic \ES_RESETSUBTYPE 
                                              \brief Reset Sub Type */
  uint32                ProcessorResets; /**< \cfetlmmnemonic \ES_PROCRESETCNT 
                                              \brief Number of processor resets since last power on */
  uint32                MaxProcessorResets; /**< \cfetlmmnemonic \ES_MAXPROCRESETS 
                                                 \brief Max processor resets before a power on is done */
  uint32                BootSource;      /**< \cfetlmmnemonic \ES_BOOTSOURCE 
                                              \brief Boot source ( as provided from BSP ) */

  uint32                PerfState;       /**< \cfetlmmnemonic \ES_PERFSTATE 
                                              \brief Current state of Performance Analyzer */
  uint32                PerfMode;        /**< \cfetlmmnemonic \ES_PERFMODE 
                                              \brief Current mode of Performance Analyzer */
  uint32                PerfTriggerCount;                           /**< \cfetlmmnemonic \ES_PERFTRIGCNT 
                                                                         \brief Number of Times Perfomance Analyzer has Triggered */
  uint32                PerfFilterMask[CFE_ES_PERF_MAX_IDS / 32];   /**< \cfetlmmnemonic \ES_PERFFLTRMASK 
                                                                         \brief Current Setting of Performance Analyzer Filter Masks */
  uint32                PerfTriggerMask[CFE_ES_PERF_MAX_IDS / 32];  /**< \cfetlmmnemonic \ES_PERFTRIGMASK 
                                                                         \brief Current Setting of Performance Analyzer Trigger Masks */
  uint32                PerfDataStart;   /**< \cfetlmmnemonic \ES_PERFDATASTART 
                                              \brief Identifies First Stored Entry in Performance Analyzer Log */
  uint32                PerfDataEnd;     /**< \cfetlmmnemonic \ES_PERFDATAEND 
                                              \brief Identifies Last Stored Entry in Performance Analyzer Log */
  uint32                PerfDataCount;   /**< \cfetlmmnemonic \ES_PERFDATACNT 
                                              \brief Number of Entries Put Into the Performance Analyzer Log */
  uint32                PerfDataToWrite; /**< \cfetlmmnemonic \ES_PERFDATA2WRITE 
                                              \brief Number of Performance Analyzer Log Entries Left to be Written to Log Dump File */
  uint32                HeapBytesFree;     /**< \cfetlmmnemonic \ES_HEAPBYTESFREE
                                              \brief Number of free bytes remaining in the OS heap */
  uint32                HeapBlocksFree;    /**< \cfetlmmnemonic \ES_HEAPBLKSFREE
                                              \brief Number of free blocks remaining in the OS heap */
  uint32                HeapMaxBlockSize;  /**< \cfetlmmnemonic \ES_HEAPMAXBLK
                                              \brief Number of bytes in the largest free block */
} CFE_ES_HkPacket_Payload_t;

typedef struct
{
    uint8                       TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_ES_HkPacket_Payload_t   Payload;
  
} CFE_ES_HkPacket_t;

/** 
**  \cfeestlm OS Shell Output Packet
**/
typedef struct
{
  char                  ShellOutput[CFE_ES_MAX_SHELL_PKT];  /**< \brief ASCII text string containing output from OS Shell
                                                                 that was received in response to an OS Shell Command */
} CFE_ES_ShellPacket_Payload_t;

typedef struct
{
    uint8                           TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_ES_ShellPacket_Payload_t    Payload;
}CFE_ES_ShellPacket_t;

/*************************************************************************/

#endif /* _cfe_es_msg_ */

/************************/
/*  End of File Comment */
/************************/

