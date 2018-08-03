/************************************************************************
** File:
**   $Id: lc_msg.h 1.3 2015/03/04 16:09:57EST sstrege Exp  $
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
**   message data types.
**
** Notes:
**   Constants and enumerated types related to these message structures
**   are defined in lc_msgdefs.h. They are kept separate to allow easy 
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in lc_msgdefs.h for more 
**   info).
**
**   $Log: lc_msg.h  $
**   Revision 1.3 2015/03/04 16:09:57EST sstrege 
**   Added copyright information
**   Revision 1.2 2012/08/01 17:02:52EDT lwalling 
**   Add age WP results option to AP sample command
**   Revision 1.1 2012/07/31 13:53:38PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.6 2011/07/04 13:26:54EDT lwalling 
**   Change cmd counter from 16 to 8 bits, adjust alignment pad fields, sort fields by data type
**   Revision 1.5 2011/02/07 17:57:48EST lwalling 
**   Modify sample AP commands to target groups of AP's
**   Revision 1.4 2010/03/03 15:30:19EST lwalling 
**   Fixed typo in Doxygen name (changed WPSINUSD to WPSINUSE)
**   Revision 1.3 2008/12/10 10:17:15EST dahardis 
**   Fixed HK structure alignment (DCR #4701)
**   Revision 1.2 2008/12/03 13:59:42EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:34EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_msg_
#define _lc_msg_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "lc_msgdefs.h"

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief No Arguments Command
**  For command details see #LC_NOOP_CC, #LC_RESET_CC
**  Also see #LC_SEND_HK_MID
*/
typedef struct
{
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

} LC_NoArgsCmd_t;

/** 
**  \brief Set LC Application State Command
**  For command details see #LC_SET_LC_STATE_CC
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         NewLCState;              /**< \brief New LC application state    */
    uint16         Padding;                 /**< \brief Structure padding           */

} LC_SetLCState_t;

/** 
**  \brief Set AP (Actionpoint) State Command
**  For command details see #LC_SET_AP_STATE_CC
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         APNumber;                /**< \brief Which actionpoint(s) to change */
    uint16         NewAPState;              /**< \brief New actionpoint state          */

} LC_SetAPState_t;

/** 
**  \brief Set AP (Actionpoint) Permanently Off
**  For command details see #LC_SET_AP_PERMOFF_CC
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         APNumber;                /**< \brief Which actionpoint to change */
    uint16         Padding;                 /**< \brief Structure padding           */

} LC_SetAPPermOff_t;

/** 
**  \brief Reset AP (Actionpoint) Statistics
**  For command details see #LC_RESET_AP_STATS_CC
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         APNumber;                /**< \brief Which actionpoint(s) to change */
    uint16         Padding;                 /**< \brief Structure padding              */

} LC_ResetAPStats_t;

/** 
**  \brief Reset WP (Watchpoint) Statistics
**  For command details see #LC_RESET_WP_STATS_CC
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         WPNumber;                /**< \brief Which watchpoint(s) to change */
    uint16         Padding;                 /**< \brief Structure padding             */

} LC_ResetWPStats_t;

/** 
**  \brief Sample AP (Actionpoint) Request
**  See #LC_SAMPLE_AP_MID
*/
typedef struct {
    uint8          CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint16         StartIndex;              /**< \brief Which actionpoint(s) to sample */
    uint16         EndIndex;

    uint16         UpdateAge;               /**< \brief Update WP results age (T or F) */

} LC_SampleAP_t;

/** 
**  \lctlm Housekeeping Packet Structure
*/
typedef struct
{
    uint8          TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< \brief cFE SB Tlm Msg Hdr */
    
    uint8          CmdCount;                        /**< \lctlmmnemonic \LC_CMDPC
                                                         \brief LC Application Command Counter       */
    uint8          CmdErrCount;                     /**< \lctlmmnemonic \LC_CMDEC
                                                         \brief LC Application Command Error Counter */
    uint8          CurrentLCState;                  /**< \lctlmmnemonic \LC_CURLCSTATE
                                                         \brief Current LC application operating 
                                                                state                               */
    uint8          Pad8;
    
    uint8          WPResults[LC_HKWR_NUM_BYTES];    /**< \lctlmmnemonic \LC_WPRESULTS
                                                         \brief Packed watchpoint results data, 
                                                                2 bits per watchpoint               */
    
    uint8          APResults[LC_HKAR_NUM_BYTES];    /**< \lctlmmnemonic \LC_APRESULTS
                                                         \brief Packed actionpoint results data, 
                                                                4 bits per actionpoint              */

    uint16         PassiveRTSExecCount;             /**< \lctlmmnemonic \LC_PASSRTSCNT
                                                         \brief Total count of RTS sequences not
                                                                initiated because the LC state is 
                                                                set to #LC_STATE_PASSIVE             */

    uint16         WPsInUse;                        /**< \lctlmmnemonic \LC_WPSINUSE
                                                         \brief How many watchpoints are currently
                                                                in effect                           */
    
    uint16         ActiveAPs;                       /**< \lctlmmnemonic \LC_ACTIVEAPS
                                                         \brief How many actionpoints are
                                                                currently active                    */
    uint16         Pad16;      
    
    uint32         APSampleCount;                   /**< \lctlmmnemonic \LC_APSAMPLECNT
                                                         \brief Total count of Actionpoints sampled  */
    uint32         MonitoredMsgCount;               /**< \lctlmmnemonic \LC_MONMSGCNT
                                                         \brief Total count of messages monitored for
                                                                watchpoints                          */
    uint32         RTSExecCount;                    /**< \lctlmmnemonic \LC_RTSCNT
                                                         \brief Total count of RTS sequences 
                                                                initiated                            */
} LC_HkPacket_t;

#endif /* _lc_msg_ */

/************************/
/*  End of File Comment */
/************************/

