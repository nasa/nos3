/*************************************************************************
** File:
**   $Id: lc_platform_cfg.h 1.2 2015/03/04 16:09:54EST sstrege Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   CFS Limit Checker (LC) Application Platform Configuration Header File
**
** Notes:
**
**   $Log: lc_platform_cfg.h  $
**   Revision 1.2 2015/03/04 16:09:54EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:35EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/platform_inc/project.pj
**   Revision 1.8 2011/03/04 09:47:04EST lwalling 
**   Change default table directory from ram to cf/apps
**   Revision 1.7 2011/01/19 11:32:05EST jmdagost 
**   Moved mission revision number from lc_version.h to lc_platform_cfg.h.
**   Revision 1.6 2010/02/23 11:24:14EST lwalling 
**   Change application name from LC_APP to LC per CFS naming convention
**   Revision 1.5 2009/12/22 13:17:45EST lwalling 
**   Create common definition for LC application name
**   Revision 1.4 2008/12/10 15:59:26EST dahardis 
**   Reduced the maximum number of watchpoints and actionpoints config params
**   from 256 to 176
**   Revision 1.3 2008/12/10 15:33:21EST dahardis 
**   Changed default configuration parameter setting to not attempt a restore 
**   from CDS on application restart
**   Revision 1.2 2008/12/03 13:59:40EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:18:17EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _lc_platform_cfg_
#define _lc_platform_cfg_

/** \lccfg Application Name
**
**  \par Description:
**       This definition must match the name used at startup by the cFE
**       Executive Services when creating the LC application.  Note that
**       application names are also an argument to certain cFE commands.
**       For example, the application name is needed to access tables
**       via cFE Table Services commands.
**
**  \par Limits:
**       LC requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to application names.
*/
#define LC_APP_NAME                    "LC"


/** \lccfg Command Pipe Depth
**  
**  \par Description:
**       Maximum number of messages that will be allowed in the
**       LC command pipe at one time. Used during initialization
**       in the call to #CFE_SB_CreatePipe
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 16 bit
**       integer (65535).
*/
#define LC_PIPE_DEPTH                  12

/** \lccfg Maximum number of watchpoints
**  
**  \par Description:
**       Maximum number of watchpoints that can be defined in the
**       Watchpoint Definition Table (WDT)
**
**  \par Limits:
**       This parameter can't be larger than 65520 (0xFFF0) because
**       higher values are reserved for use as Reversh Polish 
**       operators.
**
**       This parameter will dictate the size of the Watchpoint
**       Definition Table:
**
**       WDT Size = LC_MAX_WATCHPOINTS * sizeof(#LC_WDTEntry_t)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define LC_MAX_WATCHPOINTS             176

/** \lccfg Maximum number of actionpoints
**  
**  \par Description:
**       Maximum number of actionpoints that can be defined in the
**       Actionpoint Definition Table (ADT)
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 16 bit
**       integer (65535).
**
**       This parameter will dictate the size of the Actionpoint
**       Definition Table:
**
**       ADT Size = LC_MAX_ACTIONPOINTS * sizeof(#LC_ADTEntry_t)
**
**       The total size of this table should not exceed the
**       cFE size limit for a single buffered table set by the
**       #CFE_TBL_MAX_SNGL_TABLE_SIZE parameter
*/
#define LC_MAX_ACTIONPOINTS            176

/** \lccfg LC state after power-on reset
**  
**  \par Description:
**       What operating state LC should initialize to after a power-on
**       reset.
**
**  \par Limits:
**       This parameter must be one of the following:
**       #LC_STATE_ACTIVE
**       #LC_STATE_PASSIVE
**       #LC_STATE_DISABLED
*/
#define LC_STATE_POWER_ON_RESET        LC_STATE_DISABLED

/** \lccfg Save data to CDS compiler switch
**  
**  \par Description:
**       Compile switch that tells LC that we should save data 
**       over a processor or application reset by using the 
**       Critical Data Store (CDS).
**       Comment out or \#undef to force LC to do a default (power-on) 
**       initialization sequence on all restarts (this is the 
**       default case).
**
**  \par Limits:
**       n/a
*/
/* #define LC_SAVE_TO_CDS */

/** \lccfg LC state when CDS is restored
**  
**  \par Description:
**       What operating state LC should initialize to after successfully
**       restoring information from the CDS after a processor or 
**       application reset. This is only used when #LC_SAVE_TO_CDS
**       is set to TRUE, and provides a way to override any state LC
**       may have been operating in prior to the reset occurring.
**
**  \par Limits:
**       This parameter must be one of the following:
**       #LC_STATE_ACTIVE
**       #LC_STATE_PASSIVE
**       #LC_STATE_DISABLED
**       #LC_STATE_FROM_CDS
*/
#define LC_STATE_WHEN_CDS_RESTORED     LC_STATE_FROM_CDS

/** \lccfg Watchpoint Definition Table (WDT) filename
**  
**  \par Description:
**       Default file to load the watchpoint definition table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define LC_WDT_FILENAME                "/cf/lc_def_wdt.tbl"

/** \lccfg Actionpoint Definition Table (ADT) filename
**  
**  \par Description:
**       Default file to load the actionpoint definition table from
**       during a power-on reset sequence
**
**  \par Limits:
**       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
**       target platform in question
*/
#define LC_ADT_FILENAME                "/cf/lc_def_adt.tbl"

/** \lccfg Maximum reverse polish (RPN) equation size
**  
**  \par Description:
**       Maximum combined number of operators and operands that may 
**       exist in an actionpoint definition's reverse polish equation 
**
**  \par Limits:
**       The LC app does not place a limit on this parameter.
**       However, raising this value will increase the size of the 
**       Actionpoint Definition Table (ADT)
*/
#define LC_MAX_RPN_EQU_SIZE            20

/** \lccfg Maximum actionpoint event text string size
**  
**  \par Description:
**       Maximum length of the event message string that can specified 
**       in an actionpoint definition (including NUL terminator) 
**
**  \par Limits:
**       LC appends the trailer text #LC_AP_EVENT_TAIL_STR to this 
**       string when reporting actionpoint failures. The size of this 
**       string is #LC_AP_EVENT_TAIL_LEN
** 
**       The total value of LC_MAX_ACTION_TEXT + #LC_AP_EVENT_TAIL_LEN
**       should be less than #CFE_EVS_MAX_MESSAGE_LENGTH to avoid
**       event message truncation
**
**       Raising this value will also increase the size of the 
**       Actionpoint Definition Table (ADT)
*/
#define LC_MAX_ACTION_TEXT             32

/** \lccfg Maximum valid ADT RTS ID
**  
**  \par Description:
**       The maximum RTS ID that LC will allow during table 
**       validation in a Actionpoint Definition Table (ADT) entry
**
**  \par Limits:
**       This parameter can't be larger than an unsigned 16 bit
**       integer (65535).
*/
#define LC_MAX_VALID_ADT_RTSID         0xFFF0

/** \lccfg Floating Point Compare Tolerance
**  
**  \par Description:
**       Difference between 2 floats that will still compare as
         equal. The default value of (1.0e-25) was taken from
         the GNC file mathconstants.h
**
**  \par Limits:
**       The LC app does not place a limit on this parameter.
*/
#define LC_FLOAT_TOLERANCE             (1.0e-25)

/** \mmcfg Mission specific version number for LC application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "lc_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define LC_MISSION_REV            0

#endif /*_lc_platform_cfg_*/

/************************/
/*  End of File Comment */
/************************/
