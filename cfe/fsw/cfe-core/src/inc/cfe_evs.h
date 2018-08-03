/*
**
**  Filename: cfe_evs.h
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
*
**  $Id: cfe_evs.h 1.4 2010/10/26 16:01:08GMT-05:00 jmdagost Exp  $
**
**  Title:    Event Services API Application Library Header File
**
**  Purpose: 
**	           Unit specification for Event services library functions and macros.
**
**  Design Notes: 
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  $Date: 2010/10/26 16:01:08GMT-05:00 $
**  $Revision: 1.4 $
**  $Log: cfe_evs.h  $
**  Revision 1.4 2010/10/26 16:01:08GMT-05:00 jmdagost 
**  Made CFE_EVS_ResetFilter() argument a signed int.
**  Revision 1.3 2010/10/04 17:06:06EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2008/12/08 12:07:11EST dkobe 
**  Updates to correct doxygen errors
**  Revision 1.1 2008/04/17 08:05:21EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
**  Revision 1.19 2007/09/20 08:37:52EDT njyanchik 
**  I added the doxygen comments to CFE_EVS_Log_t
**  Revision 1.18 2007/07/18 11:53:06EDT njyanchik 
**  The two structures specified in DCR 4407 were moved to cfe_evs_msg.h
**  Revision 1.17 2007/07/12 16:45:13EDT rjmcgraw 
**  DCR4680:1 Added #defines for more filtering options
**  Revision 1.16 2007/06/07 16:03:55EDT dlkobe 
**  Corrected doxygen comment syntax errors
**  Revision 1.15 2007/06/07 13:07:49EDT dlkobe 
**  Added Command and Telemetry doxygen comments to TIME, EVS and SB
**  Revision 1.14 2007/05/23 11:21:58EDT dlkobe 
**  Added doxygen formatting
**  Revision 1.13 2007/05/14 10:26:04EDT apcudmore 
**  Preserve the EVS Local event log on a processor restart:
**  - moved the EVS Log to the ES reset data structure
**  - Added a couple of EVS Log related variables into the EVS_Log_t structure
**  - Added a routine to the EVS early init to get a pointer to the EVS Log 
**  - Added code to the EVS housekeeping service routine to copy Log variables into TLM
**  Revision 1.12 2007/03/01 11:54:34EST njyanchik 
**  This cp handles issues 1,6,7,8 as described in the DCR
**  Revision 1.11 2006/11/17 16:46:10EST dlkobe 
**  Updated with Doxygen formatted comments
**  Revision 1.10 2006/06/08 13:14:53GMT-05:00 njyanchik 
**  I added the appropriate legal headers to all of the evs files
**  Revision 1.9 2005/11/01 15:27:35EST njyanchik 
**  Added header for CFE_EVS_SendEventWithAppID function in /evs/cfe_evs.c
**  Revision 1.8 2005/10/21 16:02:04EDT kkaudra 
**  Exposure of the EVS event packet types
**  Revision 1.7 2005/09/08 17:47:51EDT sstrege 
**  Added Event Mask CFE_EVS_EVERY_FOURTH_ONE
**  Revision 1.6 2005/08/23 14:10:07EDT kkaudra 
**  Make Command Code definitions public
**/

#ifndef _cfe_evs_
#define _cfe_evs_

/********************************** Include Files  ************************************/
#include "common_types.h"    /* Basic data types */
#include "cfe_time.h"        /* Time library function definitions */
#include "cfe_evs_msg.h"        /* EVS command codes and data structures*/
#include "osapi.h"
#include "cfe_sb.h"






/******************* Macro Definitions ***********************/

/* 
** The OS_PRINTF macro may be defined by OSAL to enable
** printf-style argument checking.  If using a version of OSAL
** that does not define this then define it as a no-op.
*/
#ifndef OS_PRINTF
#define OS_PRINTF(m,n) 
#endif

/** \name Event Filter Schemes */
/** \{ */
#define CFE_EVS_BINARY_FILTER    0
/** \} */

/** \name Common Event Filter Mask Values  */
/** \{ */
#define  CFE_EVS_NO_FILTER        0x0000       /**< \brief Stops any filtering.  All messages are sent. */
#define  CFE_EVS_FIRST_ONE_STOP   0xFFFF       /**< \brief Sends the first event.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_TWO_STOP   0xFFFE       /**< \brief Sends the first 2 events.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_4_STOP     0xFFFC       /**< \brief Sends the first 4 events.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_8_STOP     0xFFF8       /**< \brief Sends the first 8 events.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_16_STOP    0xFFF0       /**< \brief Sends the first 16 events.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_32_STOP    0xFFE0       /**< \brief Sends the first 32 events.  All remaining messages are filtered. */
#define  CFE_EVS_FIRST_64_STOP    0xFFC0       /**< \brief Sends the first 64 events.  All remaining messages are filtered. */
#define  CFE_EVS_EVERY_OTHER_ONE  0x0001       /**< \brief Sends every other event. */
#define  CFE_EVS_EVERY_OTHER_TWO  0x0002       /**< \brief Sends two, filters one, sends two, filters one, etc */
#define  CFE_EVS_EVERY_FOURTH_ONE 0x0003       /**< \brief Sends every fourth event message.  All others are filtered. */
/** \} */

/** \name Event Types */
/** \{ */
#define CFE_EVS_DEBUG            0x01          /**< \brief Events that are intended only for debugging, not nominal operations */
#define CFE_EVS_INFORMATION      0x02          /**< \brief Events that identify a state change or action that is not an error */
#define CFE_EVS_ERROR            0x03          /**< \brief Events that identify an error but are not catastrophic (e.g. - bad command */
#define CFE_EVS_CRITICAL         0x04          /**< \brief Events that identify errors that are unrecoverable autonomously. */
/** \} */

/** \name Output Ports */
/** \{ */
#define CFE_EVS_PORT1            0x01
#define CFE_EVS_PORT2            0x02
#define CFE_EVS_PORT3            0x03
#define CFE_EVS_PORT4            0x04
/** \} */

/******************  Structure Definitions *********************/

/** Event message filter defintion structure */
typedef struct {
   uint16 EventID;	/**< \brief Numerical event identifier */
   uint16 RateLimit;    /**< \Rate limit in ms */
   uint16 BurstLimit;   /**< \Burst Limit in counts */

} CFE_EVS_BinFilter_t;

/* Command packet structures */


/****************** Function Prototypes **********************/

/** 
** \brief Register an application for receiving event services
**
** \par Description
**          This routine registers an application with event services and allocates/initializes
**          the internal data structures used to support this application's events.  An application
**          may not send events unless it has called this routine.  The routine also accepts a filter
**          array structure for applications requiring event filtering.  In the current implementation
**          of the EVS, only the binary filtering scheme is supported.  See section TBD of the cFE
**          Application Programmer's Guide for a description of the behavior of binary filters.
**          Applications may call #CFE_EVS_Register more than once, but each call will wipe out all
**          filters registered by previous calls (filter registration is NOT cumulative).
**
** \par Assumptions, External Events, and Notes:
**          Note: Event filters can be added, deleted or modified by ground commands.  All filtering 
**                schemes include a default setting that results in no filtering (such as #CFE_EVS_NO_FILTER 
**                for binary filters).
**
**          <b> Filter Scheme: </b> Binary <BR>
**          <b> Code: </b> CFE_EVS_BINARY_FILTER <BR>
**          <b> Filter Structure: </b> 
**          \code
**                     typedef struct {
**                           uint16  EventID,
**                           uint16  Mask ;
**                     } CFE_EVS_BinFilter_t;
**          \endcode
**
** \param[in] Filters            Pointer to an array of event message filters, or NULL if no filtering is desired.  
**                               The structure of an event message filter depends on the FilterScheme selected. 
**                               (see Filter Schemes mentioned above) 
**
** \param[in] NumFilteredEvents  The number of event message filters included in this call.  This must be less than
**                               or equal to the maximum number of events allowed per application (#CFE_EVS_MAX_EVENT_FILTERS).
**
** \param[in] FilterScheme       The event filtering scheme that this application will use.  For the first implementation of 
**                               the event services, only filter type #CFE_EVS_BINARY_FILTER will be supported.
**
** \returns
** \retcode #CFE_SUCCESS                    \retdesc  \copydoc CFE_SUCCESS                   \endcode
** \retcode #CFE_EVS_APP_FILTER_OVERLOAD    \retdesc  \copydoc CFE_EVS_APP_FILTER_OVERLOAD   \endcode
** \retcode #CFE_EVS_UNKNOWN_FILTER         \retdesc  \copydoc CFE_EVS_UNKNOWN_FILTER        \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID     \retdesc  \copydoc CFE_EVS_APP_ILLEGAL_APP_ID    \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID    \endstmt
** \endreturns
**
** \sa #CFE_EVS_Unregister
**
**/
int32 CFE_EVS_Register (void                 *Filters,           /* Pointer to an array of filters */
                        uint16               NumFilteredEvents,  /* How many elements in the array? */
                        uint16               FilterScheme);      /* Filtering Algorithm to be implemented */

/** 
** \brief Cleanup internal structures used by the event manager for the calling Application.
**
** \par Description
**          This routine un-registers the calling application from receiving event services 
**          and removes and deletes the calling applications filters and counters from the 
**          internal event service filter and counter tables if registered.  Applications 
**          must call this routine as part of their orderly shutdown process.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_SUCCESS                  \retdesc    \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED   \retdesc    \copydoc CFE_EVS_APP_NOT_REGISTERED  \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID   \retdesc    \copydoc CFE_EVS_APP_ILLEGAL_APP_ID  \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID          \endstmt
** \retstmt Any of the error codes from #CFE_ES_PutPoolBuf        \endstmt
** \endreturns
**
** \sa #CFE_EVS_Register
**
**/
int32 CFE_EVS_Unregister( void );

/** 
** \brief Generate a software event.
**
** \par Description
**          This routine generates a software event message.  If the EventID is not filtered, 
**          the event will be sent as a software bus message, optionally logged in the local 
**          event log, and optionally sent as an ASCII text string out the enabled output port(s). 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.  
**                               The \c EventID is defined and supplied by the application sending the event.   
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_DEBUG          
**                                   \arg #CFE_EVS_INFORMATION    
**                                   \arg #CFE_EVS_ERROR          
**                                   \arg #CFE_EVS_CRITICAL       
**  
** \param[in] Spec               A pointer to a null terminated text string describing the output format 
**                               for the event.  This is the same type of format string used for the ANSI 
**                               \c printf function.  Nominally the post-conversion string is limited to 80 
**                               characters, but this limit is configurable through the parameter 
**                               #CFE_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.  
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format 
**                               string unless your application will be running in a system that supports 
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.) 
**                               in the format string; they will mess up the formatting when the events are 
**                               displayed on the ground system.
**
** \returns
** \retcode #CFE_SUCCESS                   \retdesc   \copydoc CFE_SUCCESS                    \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED    \retdesc   \copydoc CFE_EVS_APP_NOT_REGISTERED     \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID    \retdesc   \copydoc CFE_EVS_APP_ILLEGAL_APP_ID     \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID        \endstmt
** \retstmt Any of the error codes from #CFE_SB_SendMsg         \endstmt
** \endreturns
**
** \sa #CFE_EVS_SendEventWithAppID, #CFE_EVS_SendTimedEvent
**
**/
int32 CFE_EVS_SendEvent (uint16 EventID,
                         uint16 EventType,
                         const char *Spec, ... )  OS_PRINTF(3,4);


/** 
** \brief Generate a software event given the specified Application ID.
**
** \par Description
**          This routine generates a software event message.  If the EventID is not filtered, 
**          the event will be sent as a software bus message, optionally logged in the local 
**          event log, and optionally sent as an ASCII text string out the enabled output port(s).   
**          Note that this function should really only be used from within an API in order to 
**          preserve the context of an Application's event.  In general, #CFE_EVS_SendEvent should be used. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.  
**                               The \c EventID is defined and supplied by the application sending the event.   
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_DEBUG
**                                   \arg #CFE_EVS_INFORMATION
**                                   \arg #CFE_EVS_ERROR
**                                   \arg #CFE_EVS_CRITICAL
**  
** \param[in] AppID              The Application ID from which the event message should appear.   
**
** \param[in] Spec               A pointer to a null terminated text string describing the output format 
**                               for the event.  This is the same type of format string used for the ANSI 
**                               \c printf function.  Nominally the post-conversion string is limited to 80 
**                               characters, but this limit is configurable through the parameter 
**                               #CFE_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.  
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format 
**                               string unless your application will be running in a system that supports 
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.) 
**                               in the format string; they will mess up the formatting when the events are 
**                               displayed on the ground system.
**
** \returns
** \retcode #CFE_SUCCESS                  \retdesc  \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED   \retdesc  \copydoc CFE_EVS_APP_NOT_REGISTERED  \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID   \retdesc  \copydoc CFE_EVS_APP_ILLEGAL_APP_ID  \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID     \endstmt
** \retstmt Any of the error codes from #CFE_SB_SendMsg      \endstmt
** \endreturns
**
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendTimedEvent
**
**/
int32 CFE_EVS_SendEventWithAppID (uint16 EventID,
                                  uint16 EventType,
                                  uint32 AppID, 
                                  const char *Spec, ... ) OS_PRINTF(4,5);


/** 
** \brief Generate a software event with a specific time tag.
**
** \par Description
**          This routine is the same as CFE_EVS_SendEvent except that the caller specifies the event time 
**          instead of having the EVS use the current spacecraft time.  This routine should be used in 
**          situations where an error condition is detected at one time, but the event message is reported 
**          at a later time. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Time               The time to include in the event.  This will usually be a time returned 
**                               by the function #CFE_TIME_GetTime().   
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.  
**                               The \c EventID is defined and supplied by the application sending the event.   
**
** \param[in] EventType          A numeric literal used to classify an event, one of:
**                                   \arg #CFE_EVS_DEBUG          
**                                   \arg #CFE_EVS_INFORMATION    
**                                   \arg #CFE_EVS_ERROR          
**                                   \arg #CFE_EVS_CRITICAL       
**  
** \param[in] Spec               A pointer to a null terminated text string describing the output format 
**                               for the event.  This is the same type of format string used for the ANSI 
**                               \c printf function.  Nominally the post-conversion string is limited to 80 
**                               characters, but this limit is configurable through the parameter 
**                               #CFE_EVS_MAX_MESSAGE_LENGTH.  Characters beyond the limit will be truncated.  
**                               Do not use floating point conversions (%f, %e, %E, %g, and %G) in the format 
**                               string unless your application will be running in a system that supports 
**                               floating point arithmetic.  Do not use non-printable characters (\\t, \\n, etc.) 
**                               in the format string; they will mess up the formatting when the events are 
**                               displayed on the ground system.
**
** \returns
** \retcode #CFE_SUCCESS                   \retdesc   \copydoc CFE_SUCCESS                 \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED    \retdesc   \copydoc CFE_EVS_APP_NOT_REGISTERED  \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID    \retdesc   \copydoc CFE_EVS_APP_ILLEGAL_APP_ID  \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID       \endstmt
** \retstmt Any of the error codes from #CFE_SB_SendMsg        \endstmt
** \endreturns
**
** \sa #CFE_EVS_SendEvent, #CFE_EVS_SendEventWithAppID
**
**/
int32 CFE_EVS_SendTimedEvent (CFE_TIME_SysTime_t Time,
                              uint16 EventID,
                              uint16 EventType,
                              const char *Spec, ... ) OS_PRINTF(4,5);


/** 
** \brief Resets the calling application's event filter for a single event ID.
**
** \par Description
**          The effect of resetting an event filter depends on the filter scheme.  
**          The #CFE_EVS_BINARY_FILTER scheme resets the filter counter for the specified Event ID. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] EventID            A numeric literal used to uniquely identify an application event.  
**                               The \c EventID is defined and supplied by the application sending the event.   
**
** \returns
** \retcode #CFE_SUCCESS                  \retdesc  \copydoc CFE_SUCCESS                     \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED   \retdesc  \copydoc CFE_EVS_APP_NOT_REGISTERED      \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID   \retdesc  \copydoc CFE_EVS_APP_ILLEGAL_APP_ID      \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID       \endstmt
** \endreturns
**
** \sa #CFE_EVS_ResetAllFilters
**
**/
int32 CFE_EVS_ResetFilter (int16 EventID);


/** 
** \brief Resets all of the calling application's event filters.
**
** \par Description
**          This routine resets all the calling application's event filter counters to zero, providing a 
**          quick and convenient method for resetting event filters. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_SUCCESS                  \retdesc  \copydoc CFE_SUCCESS                  \endcode
** \retcode #CFE_EVS_APP_NOT_REGISTERED   \retdesc  \copydoc CFE_EVS_APP_NOT_REGISTERED   \endcode
** \retcode #CFE_EVS_APP_ILLEGAL_APP_ID   \retdesc  \copydoc CFE_EVS_APP_ILLEGAL_APP_ID   \endcode
** \retstmt Any of the error codes from #CFE_ES_GetAppID \endstmt
** \endreturns
**
** \sa #CFE_EVS_ResetFilter
**
**/
int32 CFE_EVS_ResetAllFilters ( void );



#endif  /* _cfe_evs_ */
