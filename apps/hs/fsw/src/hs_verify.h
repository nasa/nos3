/*************************************************************************
** File:
**   $Id: hs_verify.h 1.3 2016/08/05 17:40:38EDT mdeschu Exp  $
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
**   Contains CFS Health and Safety (HS) macros that run preprocessor checks
**   on mission and platform configurable parameters
**
** Notes:
**
**   $Log: hs_verify.h  $
**   Revision 1.3 2016/08/05 17:40:38EDT mdeschu 
**   Ticket #26 - Verification of watchdog values not possible at compile time
**   Revision 1.2 2015/11/12 14:25:15EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.11 2015/05/04 11:00:02EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.10 2015/05/01 16:48:50EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.9 2015/03/03 12:15:59EST sstrege 
**   Added copyright information
**   Revision 1.8 2010/11/19 17:58:29EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.7 2010/11/16 16:36:21EST aschoeni 
**   Move HS_MISSION_REV from local header to platform config file
**   Revision 1.6 2010/10/14 17:43:40EDT aschoeni 
**   Change assumptions concerning utilization time period
**   Revision 1.5 2010/09/29 18:30:33EDT aschoeni 
**   Added Utilization Monitoring
**   Revision 1.4 2009/08/20 16:01:02EDT aschoeni 
**   Updated Watchdog to use CFE_PSP defines as limits
**   Revision 1.3 2009/05/21 15:44:15EDT aschoeni 
**   Updated min length check for msgacts based on the usage of CCSDS
**   Revision 1.2 2009/05/04 17:44:14EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:46EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _hs_verify_h_
#define _hs_verify_h_

/*************************************************************************
** Macro Definitions
*************************************************************************/

/*
** Idle Task Priority
*/
#if HS_IDLE_TASK_PRIORITY < 0
    #error HS_IDLE_TASK_PRIORITY cannot be less than 0
#elif HS_IDLE_TASK_PRIORITY > 255
    #error HS_IDLE_TASK_PRIORITY can not exceed 255
#endif

/*
** Maximum number execution counters
*/
#if HS_MAX_EXEC_CNT_SLOTS < 0
    #error HS_MAX_MSG_ACT_TYPES cannot be less than 0
#elif HS_MAX_EXEC_CNT_SLOTS > 4294967295
    #error HS_MAX_MSG_ACT_TYPES can not exceed 4294967295
#endif

/*
** Maximum number of message actions
*/
#if HS_MAX_MSG_ACT_TYPES < 1
    #error HS_MAX_MSG_ACT_TYPES cannot be less than 1
#elif HS_MAX_MSG_ACT_TYPES > 65531
    #error HS_MAX_MSG_ACT_TYPES can not exceed 65531
#endif

/*
** Maximum length of message actions
*/
#ifdef MESSAGE_FORMAT_IS_CCSDS
#if HS_MAX_MSG_ACT_SIZE < 7
    #error HS_MAX_MSG_ACT_SIZE cannot be less than 7
#endif
#else
#if HS_MAX_MSG_ACT_SIZE < 1
    #error HS_MAX_MSG_ACT_SIZE cannot be less than 1
#endif
#endif

#if HS_MAX_MSG_ACT_SIZE > CFE_SB_MAX_SB_MSG_SIZE
    #error HS_MAX_MSG_ACT_SIZE can not exceed CFE_SB_MAX_SB_MSG_SIZE
#endif

/*
** Maximum number of monitored applications
*/
#if HS_MAX_MONITORED_APPS < 1
    #error HS_MAX_MONITORED_APPS cannot be less than 1
#elif HS_MAX_MONITORED_APPS > 4294967295
    #error HS_MAX_MONITORED_APPS can not exceed 4294967295
#endif

/*
** Maximum number of critical events
*/
#if HS_MAX_MONITORED_EVENTS < 1
    #error HS_MAX_MONITORED_EVENTS cannot be less than 1
#elif HS_MAX_MONITORED_EVENTS > 4294967295
    #error HS_MAX_MONITORED_EVENTS can not exceed 4294967295
#endif

/*
 * JPH 2015-06-29 - Removed check of Watchdog timer values
 *
 * This is not a valid check anymore, as the HS app does not have knowledge
 * of PSP Watchdog definitions
 */

/*
** Post Processing Delay
*/
#if HS_POST_PROCESSING_DELAY < 0
    #error HS_POST_PROCESSING_DELAY can not be less than 0
#elif HS_POST_PROCESSING_DELAY > 4294967295
    #error HS_POST_PROCESSING_DELAY can not exceed 4294967295
#endif

/*
** Wakeup Timeout
*/
#if (HS_WAKEUP_TIMEOUT < -1)  || \
    (HS_WAKEUP_TIMEOUT > 2147483647)
#error HS_WAKEUP_TIMEOUT not defined as a proper SB Timeout value
#endif

/*
** CPU Aliveness Period
*/
#if HS_CPU_ALIVE_PERIOD < 1
    #error HS_CPU_ALIVE_PERIOD cannot be less than 1
#elif HS_CPU_ALIVE_PERIOD > 4294967295
    #error HS_CPU_ALIVE_PERIOD can not exceed 4294967295
#endif

/*
** Maximum processor restart actions
*/
#if HS_MAX_RESTART_ACTIONS < 0
    #error HS_MAX_RESTART_ACTIONS can not be less than 0
#elif HS_MAX_RESTART_ACTIONS > 65535
    #error HS_MAX_RESTART_ACTIONS can not exceed 65535
#endif

/*
** Pipe depths
*/
#if HS_CMD_PIPE_DEPTH < 1
    #error HS_CMD_PIPE_DEPTH cannot be less than 1
#endif
/*
 * JPH 2015-06-29 - Removed check of:
 *  HS_CMD_PIPE_DEPTH > CFE_SB_MAX_PIPE_DEPTH
 *
 * This is not a valid check anymore, as the HS app does not have knowledge
 * of CFE_SB_MAX_PIPE_DEPTH.  But if the configuration violates this rule it will
 * show up as an obvious run-time error so the compile-time check is redundant.
 */

#if HS_EVENT_PIPE_DEPTH < 1
    #error HS_EVENT_PIPE_DEPTH cannot be less than 1
#endif
/*
 * JPH 2015-06-29 - Removed check of:
 *  HS_EVENT_PIPE_DEPTH > CFE_SB_MAX_PIPE_DEPTH
 *
 * This is not a valid check anymore, as the HS app does not have knowledge
 * of CFE_SB_MAX_PIPE_DEPTH.  But if the configuration violates this rule it will
 * show up as an obvious run-time error so the compile-time check is redundant.
 */

#if HS_WAKEUP_PIPE_DEPTH < 1
    #error HS_WAKEUP_PIPE_DEPTH cannot be less than 1
#endif
/*
 * JPH 2015-06-29 - Removed check of:
 *  HS_WAKEUP_PIPE_DEPTH > CFE_SB_MAX_PIPE_DEPTH
 *
 * This is not a valid check anymore, as the HS app does not have knowledge
 * of CFE_SB_MAX_PIPE_DEPTH.  But if the configuration violates this rule it will
 * show up as an obvious run-time error so the compile-time check is redundant.
 */

/*
** Reset Task Delay
*/
#if HS_RESET_TASK_DELAY < 0
    #error HS_RESET_TASK_DELAY can not be less than 0
#elif HS_RESET_TASK_DELAY > 4294967295
    #error HS_RESET_TASK_DELAY can not exceed 4294967295
#endif

/*
** Startup Sync Timeout
*/
#if HS_STARTUP_SYNC_TIMEOUT < 0
    #error HS_STARTUP_SYNC_TIMEOUT can not be less than 0
#elif HS_STARTUP_SYNC_TIMEOUT > 4294967295
    #error HS_STARTUP_SYNC_TIMEOUT can not exceed 4294967295
#endif

/*
** Default Application Monitor State
*/
#if (HS_APPMON_DEFAULT_STATE != HS_STATE_DISABLED)  && \
    (HS_APPMON_DEFAULT_STATE != HS_STATE_ENABLED)
#error HS_APPMON_DEFAULT_STATE not defined as a supported enumerated type
#endif

/*
** Default Event Monitor State
*/
#if (HS_EVENTMON_DEFAULT_STATE != HS_STATE_DISABLED)  && \
    (HS_EVENTMON_DEFAULT_STATE != HS_STATE_ENABLED)
#error HS_EVENTMON_DEFAULT_STATE not defined as a supported enumerated type
#endif

/*
** Default Aliveness Indicator State
*/
#if (HS_ALIVENESS_DEFAULT_STATE != HS_STATE_DISABLED)  && \
    (HS_ALIVENESS_DEFAULT_STATE != HS_STATE_ENABLED)
#error HS_ALIVENESS_DEFAULT_STATE not defined as a supported enumerated type
#endif

/*
** Default CPU Hogging Indicator State
*/
#if (HS_CPUHOG_DEFAULT_STATE != HS_STATE_DISABLED)  && \
    (HS_CPUHOG_DEFAULT_STATE != HS_STATE_ENABLED)
#error HS_CPUHOG_DEFAULT_STATE not defined as a supported enumerated type
#endif

/*
** Utilization Calls Per Mark
*/
#if HS_UTIL_CALLS_PER_MARK > 4294967295
    #error HS_UTIL_CALLS_PER_MARK can not exceed 4294967295
#endif

/*
** Utilization Cycles per Interval
*/
#if HS_UTIL_CALLS_PER_MARK > 4294967295
    #error HS_UTIL_CYCLES_PER_INTERVAL can not exceed 4294967295
#endif

/*
** Total number of Utils per Interval
*/
#if HS_UTIL_PER_INTERVAL_TOTAL < 1
    #error HS_UTIL_PER_INTERVAL_TOTAL cannot be less than 1
#elif HS_UTIL_PER_INTERVAL_TOTAL > 4294967295
    #error HS_UTIL_PER_INTERVAL_TOTAL can not exceed 4294967295
#endif

/*
** Hogging number of Utils per Interval
*/
#if HS_UTIL_PER_INTERVAL_HOGGING < 1
    #error HS_UTIL_PER_INTERVAL_HOGGING cannot be less than 1
#elif HS_UTIL_PER_INTERVAL_HOGGING > HS_UTIL_PER_INTERVAL_TOTAL
    #error HS_UTIL_PER_INTERVAL_HOGGING can not exceed HS_UTIL_PER_INTERVAL_TOTAL
#endif

/*
** Hogging Timeout in Intervals
*/
#if HS_UTIL_HOGGING_TIMEOUT < 1
    #error HS_UTIL_HOGGING_TIMEOUT cannot be less than 1
#elif HS_UTIL_HOGGING_TIMEOUT > 4294967295
    #error HS_UTIL_HOGGING_TIMEOUT can not exceed 4294967295
#endif

/*
** Utilization Peak Number of Intervals
*/
#if HS_UTIL_PEAK_NUM_INTERVAL < 1
    #error HS_UTIL_PEAK_NUM_INTERVAL cannot be less than 1
#elif HS_UTIL_PEAK_NUM_INTERVAL > 4294967295
    #error HS_UTIL_PEAK_NUM_INTERVAL can not exceed 4294967295
#endif

/*
** Utilization Average Number of Intervals
*/
#if HS_UTIL_AVERAGE_NUM_INTERVAL < 1
    #error HS_UTIL_AVERAGE_NUM_INTERVAL cannot be less than 1
#elif HS_UTIL_AVERAGE_NUM_INTERVAL > HS_UTIL_PEAK_NUM_INTERVAL
    #error HS_UTIL_AVERAGE_NUM_INTERVAL can not exceed HS_UTIL_PEAK_NUM_INTERVAL
#endif

/*
** Utilization Average Number of Intervals
*/
#if HS_UTIL_TIME_DIAG_ARRAY_POWER < 0
    #error HS_UTIL_TIME_DIAG_ARRAY_POWER cannot be less than 0
#elif HS_UTIL_TIME_DIAG_ARRAY_POWER > 31
    #error HS_UTIL_TIME_DIAG_ARRAY_POWER can not exceed 31
#endif

#ifndef HS_MISSION_REV
    #error HS_MISSION_REV must be defined!
#elif (HS_MISSION_REV < 0)
    #error HS_MISSION_REV must be greater than or equal to zero!
#endif 

#endif /*_hs_verify_h_*/

/************************/
/*  End of File Comment */
/************************/
