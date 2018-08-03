/************************************************************************
** File:
**   $Id: hs_msgdefs.h 1.2 2015/11/12 14:25:15EST wmoleski Exp  $
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
**   Specification for the CFS Health and Safety (HS) command and telemetry
**   message constant definitions.
**
** Notes:
**   These Macro definitions have been put in this file (instead of
**   hs_msg.h) so this file can be included directly into ASIST build
**   test scripts. ASIST RDL files can accept C language #defines but
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE!
**   ADD THEM TO hs_msg.h IF NEEDED!
**
**   $Log: hs_msgdefs.h  $
**   Revision 1.2 2015/11/12 14:25:15EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.7 2015/05/04 11:59:07EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.6 2015/05/01 16:48:30EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.5 2015/03/03 12:16:09EST sstrege 
**   Added copyright information
**   Revision 1.4 2010/11/19 17:58:24EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.3 2009/06/02 16:38:43EDT aschoeni 
**   Updated telemetry and internal status to support HS Internal Status bit flags
**   Revision 1.2 2009/05/04 17:44:29EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:45EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_msgdefs_h_
#define _hs_msgdefs_h_

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name HS Switch States (AppMon, EventMon, Aliveness) */
/** \{ */
#define HS_STATE_DISABLED           0
#define HS_STATE_ENABLED            1
/** \} */

/**
** \name HS Internal Status Flags */
/** \{ */
#define HS_LOADED_XCT            0x01
#define HS_LOADED_MAT            0x02
#define HS_LOADED_AMT            0x04
#define HS_LOADED_EMT            0x08
#define HS_CDS_IN_USE            0x10
/** \} */

/**
** \name HS Invalid Execution Counter */
/** \{ */
#define HS_INVALID_EXECOUNT 0xFFFFFFFF
/** \} */

/** \hscmd Noop
**
**  \par Description
**       Implements the Noop command that insures the HS task is alive
**
**  \hscmdmnemonic \HS_NOOP
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC - command counter will increment
**       - The #HS_NOOP_INF_EID informational event message will be
**         generated when the command is received
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_RESET_CC
*/
#define HS_NOOP_CC                  0

/** \hscmd Reset Counters
**
**  \par Description
**       Resets the HS housekeeping counters
**
**  \hscmdmnemonic \HS_RESETCTRS
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC        - command counter will be cleared
**       - \b \c \HS_CMDEC        - command error counter will be cleared
**       - \b \c \HS_EVENTCOUNT   - events monitored counter will be cleared
**       - The #HS_RESET_DBG_EID debug event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_NOOP_CC
*/
#define HS_RESET_CC                 1

/** \hscmd Enable Applications Monitor
**
**  \par Description
**       Enables the Applications Monitor
**
**  \hscmdmnemonic \HS_ENABLEAPPMON
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC       - command counter will increment
**       - \b \c \HS_APPMONSTAT  - will be set to Enabled
**       - The #HS_ENABLE_APPMON_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_DISABLE_APPMON_CC
*/
#define HS_ENABLE_APPMON_CC          2

/** \hscmd Disable Applications Monitor
**
**  \par Description
**       Disables the Applications Monitor
**
**  \hscmdmnemonic \HS_DISABLEAPPMON
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC       - command counter will increment
**       - \b \c \HS_APPMONSTAT  - will be set to Disabled
**       - The #HS_DISABLE_APPMON_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_ENABLE_APPMON_CC
*/
#define HS_DISABLE_APPMON_CC          3

/** \hscmd Enable Events Monitor
**
**  \par Description
**       Enables the Events Monitor
**
**  \hscmdmnemonic \HS_ENABLEEVENTMON
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC         - command counter will increment
**       - \b \c \HS_EVENTMONSTAT  - will be set to Enabled
**       - The #HS_ENABLE_EVENTMON_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_DISABLE_EVENTMON_CC
*/
#define HS_ENABLE_EVENTMON_CC          4

/** \hscmd Disable Events Monitor
**
**  \par Description
**       Disables the Events Monitor
**
**  \hscmdmnemonic \HS_DISABLEEVENTMON
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC         - command counter will increment
**       - \b \c \HS_EVENTMONSTAT  - will be set to Disabled
**       - The #HS_DISABLE_EVENTMON_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_ENABLE_EVENTMON_CC
*/
#define HS_DISABLE_EVENTMON_CC          5

/** \hscmd Enable Aliveness Indicator
**
**  \par Description
**       Enables the Aliveness Indicator UART output
**
**  \hscmdmnemonic \HS_ENABLEALIVENESS
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_ALIVENESSSTAT  - will be set to Enabled
**       - The #HS_ENABLE_ALIVENESS_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_DISABLE_ALIVENESS_CC
*/
#define HS_ENABLE_ALIVENESS_CC          6

/** \hscmd Disable Aliveness Indicator
**
**  \par Description
**       Disables the Aliveness Indicator UART output
**
**  \hscmdmnemonic \HS_DISABLEALIVENESS
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_ALIVENESSSTAT  - will be set to Disabled
**       - The #HS_DISABLE_ALIVENESS_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_ENABLE_ALIVENESS_CC
*/
#define HS_DISABLE_ALIVENESS_CC          7

/** \hscmd Reset Processor Resets Performed Count
**
**  \par Description
**       Resets the count of HS performed resets maintained by HS
**
**  \hscmdmnemonic \HS_RESETRESETS
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_PROCRESETCNT   - will be set to 0
**       - The #HS_RESET_RESETS_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_SET_MAX_RESETS_CC
*/
#define HS_RESET_RESETS_PERFORMED_CC     8

/** \hscmd Set Max Processor Resets Performed Count
**
**  \par Description
**       Sets the max allowable count of processor resets to the provided value
**
**  \hscmdmnemonic \HS_SETMAXRESETS
**
**  \par Command Structure
**       #HS_SetMaxResetsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_MAXRESETS      - will be set to the provided value
**       - The #HS_SET_MAX_RESETS_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_RESET_RESETS_PERFORMED_CC
*/
#define HS_SET_MAX_RESETS_CC     9

/** \hscmd Enable CPU Hogging Indicator
**
**  \par Description
**       Enables the CPU Hogging Indicator Event Message
**
**  \hscmdmnemonic \HS_ENABLECPUHOG
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_CPUHOGSTAT  - will be set to Enabled
**       - The #HS_ENABLE_CPUHOG_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_DISABLE_CPUHOG_CC
*/
#define HS_ENABLE_CPUHOG_CC          10

/** \hscmd Disable CPU Hogging Indicator
**
**  \par Description
**       Disables the CPU Hogging Indicator Event Message
**
**  \hscmdmnemonic \HS_DISABLECPUHOG
**
**  \par Command Structure
**       #HS_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \HS_CMDPC          - command counter will increment
**       - \b \c \HS_CPUHOGSTAT  - will be set to Disabled
**       - The #HS_DISABLE_CPUHOG_DBG_EID informational event message will be
**         generated when the command is executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \HS_CMDEC - command error counter will increment
**       - Error specific event message #HS_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #HS_ENABLE_CPUHOG_CC
*/
#define HS_DISABLE_CPUHOG_CC          11

#endif /* _hs_msgdefs_h_ */

/************************/
/*  End of File Comment */
/************************/
