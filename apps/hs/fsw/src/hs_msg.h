/************************************************************************
** File:
**   $Id: hs_msg.h 1.2 2015/11/12 14:25:26EST wmoleski Exp  $
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
**   message data types.
**
** Notes:
**   Constants and enumerated types related to these message structures
**   are defined in hs_msgdefs.h. They are kept separate to allow easy
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in hs_msgdefs.h for more
**   info).
**
**   $Log: hs_msg.h  $
**   Revision 1.2 2015/11/12 14:25:26EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.12 2015/05/04 11:59:08EDT lwalling 
**   Change critical event to monitored event
**   Revision 1.11 2015/05/04 10:59:54EDT lwalling 
**   Change definitions for MAX_CRITICAL to MAX_MONITORED
**   Revision 1.10 2015/05/01 16:48:37EDT lwalling 
**   Remove critical from application monitor descriptions
**   Revision 1.9 2015/03/03 12:16:27EST sstrege 
**   Added copyright information
**   Revision 1.8 2011/08/16 14:59:24EDT aschoeni 
**   telemetry cmd counters are not 8 bit instead of 16
**   Revision 1.7 2010/11/19 17:58:25EST aschoeni 
**   Added command to enable and disable CPU Hogging Monitoring
**   Revision 1.6 2010/10/01 15:18:11EDT aschoeni 
**   Added Telemetry point to track message actions
**   Revision 1.5 2010/09/29 18:26:22EDT aschoeni 
**   Added Utilization Monitoring Telemetry
**   Revision 1.4 2009/06/02 16:38:44EDT aschoeni 
**   Updated telemetry and internal status to support HS Internal Status bit flags
**   Revision 1.3 2009/05/21 16:10:54EDT aschoeni 
**   Updated based on errors found during unit testing
**   Revision 1.2 2009/05/04 17:44:34EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:44EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_msg_h_
#define _hs_msg_h_

/************************************************************************
** Includes
*************************************************************************/
#include "hs_msgdefs.h"
#include "hs_platform_cfg.h"
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \brief HS Bits per AppMon Enable entry */
/** \{ */
#define HS_BITS_PER_APPMON_ENABLE 32
/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/**
**  \brief No Arguments Command
**  For command details see #HS_NOOP_CC, #HS_RESET_CC, #HS_ENABLE_APPMON_CC, #HS_DISABLE_APPMON_CC,
**  #HS_ENABLE_EVENTMON_CC, #HS_DISABLE_EVENTMON_CC, #HS_ENABLE_ALIVENESS_CC, #HS_DISABLE_ALIVENESS_CC,
**  #HS_RESET_RESETS_PERFORMED_CC
**  Also see #HS_SEND_HK_MID
*/
typedef struct
{
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

} HS_NoArgsCmd_t;

/**
**  \brief Set Max Resets Command
**  For command details see #HS_SET_MAX_RESETS_CC
*/
typedef struct
{
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16         MaxResets;

} HS_SetMaxResetsCmd_t;

/**
**  \hstlm Housekeeping Packet Structure
*/
typedef struct
{
    uint8     TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB Tlm Msg Hdr */

    uint8     CmdCount;                       /**< \hstlmmnemonic \HS_CMDPC
                                                        \brief HS Application Command Counter       */
    uint8     CmdErrCount;                    /**< \hstlmmnemonic \HS_CMDEC
                                                        \brief HS Application Command Error Counter */
    uint8     CurrentAppMonState;             /**< \hstlmmnemonic \HS_APPMONSTATE
                                                        \brief Status of HS Application Monitor     */
    uint8     CurrentEventMonState;           /**< \hstlmmnemonic \HS_EVTMONSTATE
                                                        \brief Status of HS Event Monitor */
    uint8     CurrentAlivenessState;          /**< \hstlmmnemonic \HS_CPUALIVESTATE
                                                        \brief Status of HS Aliveness Indicator     */
    uint8     CurrentCPUHogState;             /**< \hstlmmnemonic \HS_CPUHOGSTATE
                                                        \brief Status of HS Hogging Indicator     */
    uint8     StatusFlags;                    /**< \hstlmmnemonic \HS_STATUSFLAGS
                                                        \brief Internal HS Error States*/
    uint8     SpareBytes;                     /**< \hstlmmnemonic \HS_SPAREBYTES
                                                        \brief Alignment Spares*/
    uint16    ResetsPerformed;                /**< \hstlmmnemonic \HS_PRRESETCNT
                                                        \brief HS Performed Processor Reset Count   */
    uint16    MaxResets;                      /**< \hstlmmnemonic \HS_MAXRESETCNT
                                                        \brief HS Maximum Processor Reset Count   */
    uint32    EventsMonitoredCount;           /**< \hstlmmnemonic \HS_EVTMONCNT
                                                        \brief Total count of Event Messages
                                                         Monitored by the Events Monitor   */
    uint32    InvalidEventMonCount;           /**< \hstlmmnemonic \HS_INVALIDEVTAPPCNT
                                                        \brief Total count of Invalid Event Monitors
                                                         Monitored by the Events Monitor   */
    uint32    AppMonEnables[((HS_MAX_MONITORED_APPS - 1) / HS_BITS_PER_APPMON_ENABLE)+1];/**< \hstlmmnemonic \HS_APPSTATUS
                                                        \brief Enable states of App Monitor Entries */
    uint32    MsgActExec;                     /**< \hstlmmnemonic \HS_MSGACTEXEC
                                                        \brief Number of Software Bus Message Actions Executed */
    uint32    UtilCpuAvg;                     /**< \hstlmmnemonic \HS_UTILAVG
                                                        \brief Current CPU Utilization Average */
    uint32    UtilCpuPeak;                    /**< \hstlmmnemonic \HS_UTILPEAK
                                                        \brief Current CPU Utilization Peak */
#if HS_MAX_EXEC_CNT_SLOTS != 0
    uint32    ExeCounts[HS_MAX_EXEC_CNT_SLOTS]; /**< \hstlmmnemonic \HS_EXECUTIONCTR
                                                             \brief Execution Counters              */
#endif

} HS_HkPacket_t;

#endif /* _hs_msg_h_ */

/************************/
/*  End of File Comment */
/************************/

