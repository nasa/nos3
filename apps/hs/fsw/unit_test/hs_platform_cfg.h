/*************************************************************************
** File:
**   $Id: hs_platform_cfg.h 1.2 2016/09/07 19:17:20EDT mdeschu Exp  $
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
**   CFS Health and Safety (HS) Application Platform Configuration Header File
**
** Notes:
**
**   $Log: hs_platform_cfg.h  $
**   Revision 1.2 2016/09/07 19:17:20EDT mdeschu 
**   Update unit test asserts to match HS updates
**   
**   HS_MAX_CRITICAL_APPS/EVENTS -> HS_MAX_MONITORED_APPS/EVENTS
**   Removal of "Critical" from certain event messages.
**   Revision 1.3 2016/08/05 09:40:27EDT mdeschu 
**   Ticket #40: HS hogging initialization
**   Revision 1.2 2015/11/12 14:25:20EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.15 2015/05/04 11:59:20EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.14 2015/05/04 11:00:06EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.13 2015/05/01 16:48:50EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.12 2015/03/03 12:16:30EST sstrege 
**   Added copyright information
**   Revision 1.11 2011/10/17 16:50:20EDT aschoeni 
**   updated calibration parameter notes
**   Revision 1.10 2011/10/13 18:49:51EDT aschoeni 
**   Updated for changes to cpu utilization calibration
**   Revision 1.9 2011/08/15 15:42:55EDT aschoeni 
**   Updated so application name is configurable
**   Revision 1.8 2010/11/19 17:58:30EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.7 2010/11/16 16:36:28EST aschoeni 
**   Move HS_MISSION_REV from local header to platform config file
**   Revision 1.6 2010/10/14 17:45:42EDT aschoeni 
**   Removed assumptions of rate of utilization measurement
**   Revision 1.5 2010/09/29 18:28:10EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.4 2010/09/13 14:41:39EDT aschoeni 
**   add apps subdir to default table path
**   Revision 1.3 2009/08/20 16:03:02EDT aschoeni 
**   Changed watchdog timeout to be in milliseconds rather than seconds (to match the new API)
**   Revision 1.2 2009/05/04 17:44:35EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:52:03EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _hs_platform_cfg_h_
#define _hs_platform_cfg_h_

/** \hscfg Application Name
**
**  \par Description:
**       This definition must match the name used at startup by the cFE
**       Executive Services when creating the HS application.  Note that
**       application names are also an argument to certain cFE commands.
**       For example, the application name is needed to access tables
**       via cFE Table Services commands.
**
**  \par Limits:
**       HS requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to application names.
*/
#define HS_APP_NAME                    "HS"

/**\hscfg Idle Task Configuration Parameters (custom)
**
**  \par Description:
**       These parameters are used by #CFE_ES_CreateChildTask
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       These limits will vary by platform and available resources.
*/
#define HS_IDLE_TASK_NAME  "HS_IDLE_TASK"
#define HS_IDLE_TASK_STACK_PTR  0
#define HS_IDLE_TASK_STACK_SIZE 4096
#define HS_IDLE_TASK_FLAGS      0

/**\hscfg Idle Task Priority (custom)
**
**  \par Description:
**       This parameter is used to set the priority of the Idle Task. It should
**       be higher than all other user created tasks, but may need to be set lower
**       than the maximum value if an OS uses its own minimum priority task.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       This parameter can't be larger than 255.
*/
#define HS_IDLE_TASK_PRIORITY   252

/** \hscfg Maximum reported execution counters
**
**  \par Description:
**       Maximum number of execution counters that can be
**       specified to be reported in telemetry.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This parameter will dictate the size of the Execution
**       Counter Table (XCT):
**
**       XCT Size = HS_MAX_EXEC_CNT_SLOTS * sizeof(#HS_XCTEntry_t)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define HS_MAX_EXEC_CNT_SLOTS             32

/** \hscfg Maximum message action types
**
**  \par Description:
**       Maximum number of Message Action action types.
**
**  \par Limits:
**       This parameter can't be larger than 4 less than an
**       unsigned 16 bit integer (65531).
**
**       This parameter must be greater than 0.
**
**       This parameter will influence the size of the Message
**       Action Table (MAT):
**
**       MAT Size = HS_MAX_MSG_ACT_TYPES * (HS_MAX_MSG_ACT_SIZE + 4)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define HS_MAX_MSG_ACT_TYPES             8

/** \hscfg Maximum message action size (in bytes)
**
**  \par Description:
**       Size in bytes of maximum length of software bus message that
**       can be sent using a Message Action action type.
**
**  \par Limits:
**       This parameter can't be larger than #CFE_SB_MAX_SB_MSG_SIZE
**
**       This parameter can't be smaller than a packet header
**
**       This parameter will influence the size of the Message
**       Action Table (MAT):
**
**       MAT Size = HS_MAX_MSG_ACT_TYPES * (HS_MAX_MSG_ACT_SIZE + 4)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define HS_MAX_MSG_ACT_SIZE             16

/** \hscfg Maximum number of monitored applications
**
**  \par Description:
**       Maximum number of applications that can be
**       monitored to assure check-ins
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This parameter must be greater than 0.
**
**       This parameter will dictate the size of the Application
**       Monitor Table (AMT):
**
**       AMT Size = HS_MAX_MONITORED_APPS * sizeof(#HS_AMTEntry_t)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define HS_MAX_MONITORED_APPS             32

/** \hscfg Maximum number of monitored events
**
**  \par Description:
**       Maximum number of events that can be
**       monitored
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This parameter must be greater than 0.
**
**       This parameter will dictate the size of the Event
**       Monitor Table (EMT):
**
**       EMT Size = HS_MAX_MONITORED_EVENTS * sizeof(#HS_EMTEntry_t)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define HS_MAX_MONITORED_EVENTS          16

/** \hscfg Watchdog Timeout Value
**
**  \par Description:
**       Number of milliseconds before a watchdog timeout occurs.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This parameter must be greater than 0.
*/
#define HS_WATCHDOG_TIMEOUT_VALUE       10000

/**
**  \hscfg Time to wait after performing processing (in milliseconds)
**
**  \par Description:
**       Dictates the length of a task delay performed prior to checking
**       the Software Bus for a Wakeup Message. This ensures that HS will
**       run no more often than a certain rate. If this parameter is set to 0,
**       no task delay will be performed. Time is in milliseconds.
**
**  \par Limits
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_POST_PROCESSING_DELAY 0

/**
**  \hscfg Wakeup Message Software Bus Timeout
**
**  \par Description:
**       This parameter is passed into #CFE_SB_RcvMsg as the timeout value.
**       It can specify #CFE_SB_POLL, #CFE_SB_PEND_FOREVER, or a timeout
**       value in milliseconds.
**
**  \par Limits
**       This Parameter must be #CFE_SB_POLL, #CFE_SB_PEND_FOREVER,
**       or greater than 0 and less than 2^31 - 1
**
**       As a timeout, this parameter should be less than
**       (#HS_WATCHDOG_TIMEOUT_VALUE * 1000) - HS runtime in ms
**       otherwise HS may not be able to service the watchdog in time.
*/
#define HS_WAKEUP_TIMEOUT 1200

/** \hscfg CPU aliveness output string
**
**  \par Description:
**       String that is output to via #OS_printf periodically if aliveness
**       is enabled.
**
**  \par Limits:
**       None.
**
*/
#define HS_CPU_ALIVE_STRING        "."

/** \hscfg CPU aliveness output period
**
**  \par Description:
**       Rate in number of HS cycles at which the HS_CPU_ALIVE_STRING
**       is output via the UART.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
*/
#define HS_CPU_ALIVE_PERIOD         5

/** \hscfg Max Number of Processor Resets that may be performed by HS
**
**  \par Description:
**       Maximum number of times that the HS App will attempt a processor
**       reset as the result of either an Application Monitor or
**       Event Monitor Failure
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 16 bit
**       integer (65535).
**
**       Although not enforced, if this parameter is greater than or
**       equal to #CFE_ES_MAX_PROCESSOR_RESETS then a POWER-ON reset
**       will occur before the max count is reached, resetting the remaining
**       actions to the value set here.
**
*/
#define HS_MAX_RESTART_ACTIONS      3

/** \hscfg Software bus command pipe depth
**
**  \par Description:
**       Depth of the software bus pipe HS uses for commands and HK requests.
**       Used during initialization in the call to #CFE_SB_CreatePipe
**
**  \par Limits:
**       This parameter can't be larger than #CFE_SB_MAX_PIPE_DEPTH
**
**       This parameter must be greater than 0.
*/
#define HS_CMD_PIPE_DEPTH       12

/** \hscfg Software bus event pipe depth
**
**  \par Description:
**       Depth of the software bus pipe HS uses for event monitoring.
**       This should be set to supply sufficient room for the expected event
**       message load per second. Used during initialization in the call to
**       #CFE_SB_CreatePipe
**
**  \par Limits:
**       This parameter can't be larger than #CFE_SB_MAX_PIPE_DEPTH
**
**       This parameter must be greater than 0.
*/
#define HS_EVENT_PIPE_DEPTH       32

/** \hscfg Software bus wakeup pipe depth
**
**  \par Description:
**       Depth of the software bus pipe HS uses for wakeup messages.
**       Used during initialization in the call to #CFE_SB_CreatePipe
**
**  \par Limits:
**       This parameter can't be larger than #CFE_SB_MAX_PIPE_DEPTH
**
**       This parameter must be greater than 0.
*/
#define HS_WAKEUP_PIPE_DEPTH       1

/**
**  \hscfg Time to wait before a processor reset (in milliseconds)
**
**  \par Description:
**       Dictates the length of the task delay (milliseconds) performed prior
**       to calling #CFE_ES_ResetCFE to allow for any event message to go out.
**
**  \par Limits
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_RESET_TASK_DELAY   50

/**
**  \hscfg Time to wait for all apps to be started (in milliseconds)
**
**  \par Description:
**       Dictates the timeout for the #CFE_ES_WaitForStartupSync call that
**       HS uses to wait for all of the Applications specified in the startup
**       script to finish initialization. HS will wait this amount of time
**       before assuming all startup script applications have been started and
**       will then begin nominal processing.
**
**  \par Limits
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This should be greater than or equal to the Startup Sync timeout for
**       any application in the Application Monitor Table.
*/
#define HS_STARTUP_SYNC_TIMEOUT   65000

/** \hscfg Default State of the Application Monitor
**
**  \par Description:
**       State the Application Monitor is set to when the HS
**       application starts.
**
**  \par Limits:
**       Must be HS_STATE_ENABLED or HS_STATE_DISABLED
*/
#define HS_APPMON_DEFAULT_STATE   HS_STATE_ENABLED

/** \hscfg Default State of the Event Monitor
**
**  \par Description:
**       State the Event Monitor is set to when the HS
**       application starts.
**
**  \par Limits:
**       Must be HS_STATE_ENABLED or HS_STATE_DISABLED
*/
#define HS_EVENTMON_DEFAULT_STATE   HS_STATE_ENABLED

/** \hscfg Default State of the Aliveness Indicator
**
**  \par Description:
**       State the Aliveness Indicator is set to when the HS application
**       starts.
**
**  \par Limits:
**       Must be HS_STATE_ENABLED or HS_STATE_DISABLED
*/
#define HS_ALIVENESS_DEFAULT_STATE   HS_STATE_ENABLED

/** \hscfg Default State of the CPU Hogging Indicator
**
**  \par Description:
**       State the CPU Hogging Event Message is set to when the HS application
**       starts.
**
**  \par Limits:
**       Must be HS_STATE_ENABLED or HS_STATE_DISABLED
*/
#define HS_CPUHOG_DEFAULT_STATE   HS_STATE_ENABLED

/** \hscfg Application Monitor Table (AMT) filename
**
**  \par Description:
**       Default file to load the Applications Monitor Table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define HS_AMT_FILENAME                "/cf/apps/hs_amt.tbl"

/** \hscfg Event Monitor Table (EMT) filename
**
**  \par Description:
**       Default file to load the Event Monitor Table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define HS_EMT_FILENAME                "/cf/apps/hs_emt.tbl"

/** \hscfg Execution Counter Table (XCT) filename
**
**  \par Description:
**       Default file to load the Execution Counters Table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define HS_XCT_FILENAME                "/cf/apps/hs_xct.tbl"

/** \hscfg Message Actions Table (MAT) filename
**
**  \par Description:
**       Default file to load the Message Actions Table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define HS_MAT_FILENAME                "/cf/apps/hs_mat.tbl"

/** \hscfg CPU Utilization Calls per Mark (custom)
**
**  \par Description:
**       Number of times the Mark function must be called before it actually
**       marks the time. This influences the interval size. The function 
**       calling the Mark function may not run at the same rate as the HS cycle
**       (or HS may not want to monitor utilization every cycle) so this the
**       interval to be at least as long as an HS cycle. 
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_UTIL_CALLS_PER_MARK                         1

/** \hscfg CPU Utilization Cycles per Interval (custom)
**
**  \par Description:
**       Number of HS Cycles it takes to complete a CPU Utilization Interval.
**       HS will monitor the utilization after this number of HS wakeup cycles.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_UTIL_CYCLES_PER_INTERVAL                    1

/** \hscfg CPU Utilization Total Utils Per Interval
**
**  \par Description:
**       Number of Utils (counts) equal to full utilization. This allows for higher
**       resolution than percentages, and non decimal based values.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_UTIL_PER_INTERVAL_TOTAL             10000

/** \hscfg CPU Utilization Hogging Utils Per Interval
**
**  \par Description:
**       Number of Utils (counts) equal to utilization which is considered hogging
**       during one interval. A greater number of counts is also considered hogging.
**
**  \par Limits:
**       This parameter can't be larger than #HS_UTIL_PER_INTERVAL_TOTAL.
*/
#define HS_UTIL_PER_INTERVAL_HOGGING            9900

/** \hscfg CPU Utilization Conversion Factor Multiplication 1 (custom)
**
**  \par Description:
**       First multiplication conversion factor. Number of idle ticks is multiplied 
**       this value first when converting to utils.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       There may be processor dependent limits on value. Note that all math
**       is done using "uint32" values; it is important that the number
**       of loop iterations in HS_IDLE, times this value, not overflow.
**
**       The result of the conversion must be a 32 bit signed integer 
**       (between -2147483648 and 2147483647).
*/
#define HS_UTIL_CONV_MULT1                      2

/** \hscfg CPU Utilization Conversion Factor Division (custom)
**
**  \par Description:
**       Division conversion factor. Number of idle ticks is divided by this value
**       after it has been multiplied by #HS_UTIL_CONV_MULT1.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       There may be processor dependent limits on value.
**
**       The result of the conversion must be a 32 bit signed integer 
**       (between -2147483648 and 2147483647).
*/
#define HS_UTIL_CONV_DIV                       5

/** \hscfg CPU Utilization Conversion Factor Multiplication 2 (custom)
**
**  \par Description:
**       Second multiplication conversion factor. Number of idle ticks is multiplied 
**       this value after being divided by #HS_UTIL_CONV_DIV after being multiplied by
**       #HS_UTIL_CONV_MULT1 when converting to utils.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       There may be processor dependent limits on value.
**
**       The result of the conversion must be a 32 bit signed integer 
**       (between -2147483648 and 2147483647).
*/
#define HS_UTIL_CONV_MULT2                       11

/** \hscfg CPU Utilization Hogging Timeout
**
**  \par Description:
**       Number of intervals for which the hogging limit must be exceeded before hogging
**       is reported.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_UTIL_HOGGING_TIMEOUT                  5

/** \hscfg CPU Peak Utilization Number of Intervals
**
**  \par Description:
**       Number of intervals over which the peak utilization is determined.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
**
**       This parameter controls the size of the array which stores
**       previously measured utilization values.
*/
#define HS_UTIL_PEAK_NUM_INTERVAL                    64

/** \hscfg CPU Average Utilization Number of Intervals
**
**  \par Description:
**       Number of intervals over which the average utilization is computed.
**
**  \par Limits:
**       This parameter can't be larger than #HS_UTIL_PEAK_NUM_INTERVAL .
*/
#define HS_UTIL_AVERAGE_NUM_INTERVAL                  4

/** \hscfg CPU Utilization Diagnostics Mask (custom)
**
**  \par Description:
**       Count mask for CPU Utilization Calibration. Time will be marked
**       when (Counts & Mask) == Mask
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 32 bit
**       integer (4294967295).
*/
#define HS_UTIL_DIAG_MASK               0xFFFFFFFF

/** \hscfg CPU Utilization Diagnostics Array Configuration (custom)
**
**  \par Description:
**       Time will be marked into an array of subseconds. The independant parameter
**       controls the exponent to which 2 is raised to determine the array size. As
**       such, large values will require significant memory usage.
**       Note that these values are only necessarily relevant in the
**       default hs_custom.c.
**
**  \par Limits:
**       This parameter must be less than 32 and may not be negative.
*/
#define HS_UTIL_TIME_DIAG_ARRAY_POWER          4
#define HS_UTIL_TIME_DIAG_ARRAY_LENGTH         (1<<(HS_UTIL_TIME_DIAG_ARRAY_POWER))
#define HS_UTIL_TIME_DIAG_ARRAY_MASK           (HS_UTIL_TIME_DIAG_ARRAY_LENGTH - 1)


/** \hscfg Mission specific version number for HS application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "hs_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define HS_MISSION_REV            0

#endif /*_hs_platform_cfg_h_*/

/************************/
/*  End of File Comment */
/************************/
