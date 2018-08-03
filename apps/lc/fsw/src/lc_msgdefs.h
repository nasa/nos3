/************************************************************************
** File:
**   $Id: lc_msgdefs.h 1.3 2015/03/04 16:09:57EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) command and telemetry 
**   message constant definitions.
**
** Notes:
**   These Macro definitions have been put in this file (instead of 
**   lc_msg.h) so this file can be included directly into ASIST build 
**   test scripts. ASIST RDL files can accept C language #defines but 
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE! 
**   ADD THEM TO lc_msg.h IF NEEDED! 
**
**   $Log: lc_msgdefs.h  $
**   Revision 1.3 2015/03/04 16:09:57EST sstrege 
**   Added copyright information
**   Revision 1.2 2012/08/01 14:19:40EDT lwalling 
**   Change NOT_MEASURED to STALE
**   Revision 1.1 2012/07/31 13:53:39PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.3 2011/01/19 12:45:40EST jmdagost 
**   Moved two message parameters to the message IDs file for scheduler table access.
**   Revision 1.2 2008/12/10 10:17:17EST dahardis 
**   Fixed HK structure alignment (DCR #4701)
**   Revision 1.1 2008/10/29 14:19:37EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_msgdefs_
#define _lc_msgdefs_

/************************************************************************
** Includes
*************************************************************************/
#include "lc_platform_cfg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name LC Application States */ 
/** \{ */
#define LC_STATE_ACTIVE             1
#define LC_STATE_PASSIVE            2
#define LC_STATE_DISABLED           3
#define LC_STATE_FROM_CDS           4      /**< \brief Only used for reset processing 
                                                       Not valid as a command argument     */
/** \} */

/**
** \name Actionpoint States */ 
/** \{ */
#define LC_ACTION_NOT_USED          0xFF   /**< \brief Used to indicate unused actionpoint 
                                                       table entries. Not valid as a 
                                                       command argument                    */
#define LC_APSTATE_ACTIVE           1
#define LC_APSTATE_PASSIVE          2
#define LC_APSTATE_DISABLED         3
#define LC_APSTATE_PERMOFF          4      /**< \brief To set this state requires using 
                                                       the #LC_SET_AP_PERMOFF_CC command   */
/** \} */

/**
** \name Housekeeping Packed Watch Results */ 
/** \{ */
#define LC_HKWR_FALSE               0x00   /**< \brief Two bit value used for FALSE        */ 
#define LC_HKWR_TRUE                0x01   /**< \brief Two bit value used for TRUE         */
#define LC_HKWR_ERROR               0x02   /**< \brief Two bit value used for ERROR        */
#define LC_HKWR_STALE               0x03   /**< \brief Two bit value used for STALE        */
/** \} */

/**
** \name Housekeeping Packed Action Results */ 
/** \{ */
#define LC_HKAR_PASS                0x00  /**< \brief Two bit value used for PASS         */ 
#define LC_HKAR_FAIL                0x01  /**< \brief Two bit value used for FAIL         */ 
#define LC_HKAR_ERROR               0x02  /**< \brief Two bit value used for ERROR        */ 
#define LC_HKAR_STALE               0x03  /**< \brief Two bit value used for STALE        */ 
/** \} */

/**
** \name Housekeeping Packed Action Results, State Identifiers */ 
/** \{ */
#define LC_HKAR_STATE_NOT_USED      0x00  /**< \brief Two bit value used for NOT USED
                                                      as well as PERMOFF                  */
#define LC_HKAR_STATE_ACTIVE        0x01  /**< \brief Two bit value used for ACTIVE       */
#define LC_HKAR_STATE_PASSIVE       0x02  /**< \brief Two bit value used for PASSIVE      */
#define LC_HKAR_STATE_DISABLED      0x03  /**< \brief Two bit value used for DISABLED     */
/** \} */

/**
** \name Housekeeping Packed Results, Array Sizes */ 
/** \{ */
#define LC_HKWR_NUM_BYTES  (((LC_MAX_WATCHPOINTS  + 15) / 16) * 4)   /**< \brief 2 bits per WP and keeping
                                                                                 array on longword boundary   */
#define LC_HKAR_NUM_BYTES  (((LC_MAX_ACTIONPOINTS +  7) /  8) * 4)   /**< \brief 4 bits per AP and keeping  
                                                                                 array on longword boundary   */
/** \} */

/** \lccmd Noop 
**  
**  \par Description
**       Implements the Noop command that insures the LC task is alive
**
**  \lccmdmnemonic \LC_NOOP
**
**  \par Command Structure
**       #LC_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC - command counter will increment
**       - The #LC_NOOP_INF_EID informational event message will be 
**         generated when the command is received
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #LC_RESET_CC
*/
#define LC_NOOP_CC                  0

/** \lccmd Reset Counters
**  
**  \par Description
**       Resets the LC housekeeping counters
**
**  \lccmdmnemonic \LC_RESETCTRS
**
**  \par Command Structure
**       #LC_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC       - command counter will be cleared
**       - \b \c \LC_CMDEC       - command error counter will be cleared
**       - \b \c \LC_APSAMPLECNT - actionpoint sample counter will be cleared
**       - \b \c \LC_MONMSGCNT   - monitored message counter will be cleared
**       - \b \c \LC_RTSCNT      - RTS execution counter will be cleared
**       - \b \c \LC_PASSRTSCNT  - passive RTS execution counter will be cleared
**       - The #LC_RESET_DBG_EID debug event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #LC_NOOP_CC
*/
#define LC_RESET_CC                 1

/** \lccmd Set LC Application State
**  
**  \par Description
**       Sets the operational state of the LC application
**
**  \lccmdmnemonic \LC_SETLCSTATE
**
**  \par Command Structure
**       #LC_SetLCState_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC       - command counter will increment
**       - \b \c \LC_CURLCSTATE  - will be set to the new state
**       - The #LC_LCSTATE_INF_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Invalid new state specified in command message
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**       - Error specific event message #LC_LCSTATE_ERR_EID
**
**  \par Criticality
**       None
**
*/
#define LC_SET_LC_STATE_CC          2

/** \lccmd Set AP State
**  
**  \par Description
**       Set actionpoint state
**
**  \lccmdmnemonic \LC_SETAPSTATE
**
**  \par Command Structure
**       #LC_SetAPState_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC - command counter will increment
**       - The #LC_APSTATE_INF_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Invalid actionpoint state specified in command message
**       - Actionpoint number specified in command message is 
**         out of range
**       - Actionpoint current state is either #LC_ACTION_NOT_USED 
**         or #LC_APSTATE_PERMOFF
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**       - Error specific event message #LC_APSTATE_NEW_ERR_EID
**       - Error specific event message #LC_APSTATE_APNUM_ERR_EID
**       - Error specific event message #LC_APSTATE_CURR_ERR_EID
**
**  \par Criticality
**       None
**
*/
#define LC_SET_AP_STATE_CC          3

/** \lccmd Set AP Permanently Off
**  
**  \par Description
**       Set the specified actionpoint's state to #LC_APSTATE_PERMOFF
**
**  \lccmdmnemonic \LC_SETAPPERMOFF
**
**  \par Command Structure
**       #LC_SetAPPermOff_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC - command counter will increment
**       - The #LC_APOFF_INF_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Actionpoint number specified in command message is 
**         out of range
**       - Actionpoint current state is not #LC_APSTATE_DISABLED
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**       - Error specific event message #LC_APOFF_APNUM_ERR_EID
**       - Error specific event message #LC_APOFF_CURR_ERR_EID
**
**  \par Criticality
**       None
**
*/
#define LC_SET_AP_PERMOFF_CC        4

/** \lccmd Reset AP Statistics
**  
**  \par Description
**       Resets actionpoint statistics
**
**  \lccmdmnemonic \LC_RESETAPSTATS
**
**  \par Command Structure
**       #LC_ResetAPStats_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC - command counter will increment
**       - The #LC_APSTATS_INF_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Actionpoint number specified in command message is 
**         out of range
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**       - Error specific event message #LC_APSTATS_APNUM_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #LC_RESET_WP_STATS_CC
*/
#define LC_RESET_AP_STATS_CC        5

/** \lccmd Reset WP Statistics
**  
**  \par Description
**       Resets watchpoint statistics
**
**  \lccmdmnemonic \LC_RESETWPSTATS
**
**  \par Command Structure
**       #LC_ResetWPStats_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \LC_CMDPC - command counter will increment
**       - The #LC_WPSTATS_INF_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Watchpoint number specified in command message is 
**         out of range
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \LC_CMDEC - command error counter will increment
**       - Error specific event message #LC_LEN_ERR_EID
**       - Error specific event message #LC_WPSTATS_WPNUM_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #LC_RESET_AP_STATS_CC
*/
#define LC_RESET_WP_STATS_CC        6

#endif /* _lc_msgdefs_ */

/************************/
/*  End of File Comment */
/************************/
