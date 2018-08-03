/************************************************************************
** File:
**   $Id: sc_msg.h 1.13 2015/03/02 12:58:40EST sstrege Exp  $
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
**   Specification for the CFS Stored Command (SC) command and telemetry 
**   message data types.
**
** Notes:
**   Constants and enumerated types related to these message structures
**   are defined in sc_msgdefs.h. They are kept separate to allow easy 
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in sc_msgdefs.h for more 
**   info).
**
**   $Log: sc_msg.h  $
**   Revision 1.13 2015/03/02 12:58:40EST sstrege 
**   Added copyright information
**   Revision 1.12 2011/09/23 14:26:16EDT lwalling 
**   Made group commands conditional on configuration definition
**   Revision 1.11 2011/07/04 13:06:33EDT lwalling 
**   Sort housekeeping fields by data type
**   Revision 1.10 2011/07/04 12:57:06EDT lwalling 
**   Change command counter from 16 bits to 8 bits, remove obsolete alignment pad word
**   Revision 1.9 2011/03/14 10:51:46EDT lwalling 
**   Add definition for RTS group command packet structure -- SC_RtsGrpCmd_t.
**   Revision 1.8 2010/09/28 10:33:29EDT lwalling 
**   Update list of included header files
**   Revision 1.7 2010/05/18 15:31:39EDT lwalling 
**   Change AtsId/RtsId to AtsIndex/RtsIndex or AtsNumber/RtsNumber
**   Revision 1.6 2010/04/21 15:45:00EDT lwalling 
**   Added Append ATS command packet structure definition
**   Revision 1.5 2010/04/16 15:27:52EDT lwalling 
**   Added new Append ATS variables to HK packet
**   Revision 1.4 2010/04/05 11:55:08EDT lwalling 
**   Add Append ATS data to local and hk data structures
**   Revision 1.3 2009/01/26 14:44:57EST nyanchik 
**   Check in of Unit test
**   Revision 1.2 2009/01/05 08:26:54EST nyanchik 
**   Check in after code review changes
**   
** 
*************************************************************************/



#ifndef _sc_msg_
#define _sc_msg_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "sc_platform_cfg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/




/************************************************************************
** Type Definitions
*************************************************************************/

/** 
**  \sctlm Housekeeping Packet Structure
*/
typedef struct {
    uint8       TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8       AtsNumber;                      /**< \sctlmmnemonic \SC_ATSNUM */
                                                /**< \brief current ATS number  1 = ATS A, 2 = ATS B     */
    uint8       AtpState;                       /**< \sctlmmnemonic \SC_ATPSTATE */         
                                                /**< \brief  current ATP state valid values are: 2 = IDLE, 5 = EXECUTING */                                                                         
    uint8       ContinueAtsOnFailureFlag ;      /**< \sctlmmnemonic \SC_CONTONFAIL */
    /**< \brief In the event of ATS execution failure (ats command fails checksum) ,
     the ATS execution will continue if this flag is set to TRUE and will stop
     if this flag is set to FALSE
     */   
    uint8       CmdErrCtr;                       /**< \sctlmmnemonic \SC_CMDEC */   
                                                 /**< \brief  Counts Request Errors  */
    uint8       CmdCtr;                          /**< \sctlmmnemonic \SC_CMDPC */    
                                                 /**< \brief  Counts Ground Requests */
    uint8       Padding8;                        /**< \sctlmmnemonic \SC_PAD8 */                               

    uint16      SwitchPendFlag;                  /**< \sctlmmnemonic \SC_SWITCHPEND */  
                                                 /**< \brief  is an ats switch pending? 0 = NO, 1 = YES  
                                                      This means that the ATS switch is waiting until a safe time */
    uint16      NumRtsActive;                    /**< \sctlmmnemonic \SC_NUMRTSACTIVE */ 
                                                 /**< \brief number of RTSs currently active */
    uint16      RtsNumber;                       /**< \sctlmmnemonic \SC_NEXTRTSNUM */  
                                                 /**< \brief  next RTS number */           
    uint16      RtsActiveCtr;                    /**< \sctlmmnemonic \SC_RTSACTPC */
                                                 /**< \brief Increments when an RTS is started without error */
    uint16      RtsActiveErrCtr;                 /**< \sctlmmnemonic \SC_RTSACTEC */ 
                                                 /**< \brief Increments when an attempt to start an RTS fails */
    uint16      AtsCmdCtr;                       /**< \sctlmmnemonic \SC_ATSCMDPC */
                                                 /**< \brief Total ATS cmd cnter counts commands sent by the ATS */
    uint16      AtsCmdErrCtr;                    /**< \sctlmmnemonic \SC_ATSCMDEC */  
                                                 /**< \brief Total ATS cmd Error ctr command errors in the ATS */
    uint16      RtsCmdCtr;                       /**< \sctlmmnemonic \SC_RTSCMDPC */    
                                                 /**< \brief Counts TOTAL rts cmds that were sent out from ALL active RTSs */
    uint16      RtsCmdErrCtr;                    /**< \sctlmmnemonic \SC_RTSCMDEC */  
                                                 /**< \brief Counts TOTAL number of errs from ALL RTSs that are active */
    uint16      LastAtsErrSeq;                   /**< \sctlmmnemonic \SC_ATSLASTERRID */ 
                                                 /**< \brief Last ATS Errant Sequence Num Values: 1 or 2 */
    uint16      LastAtsErrCmd;                   /**< \sctlmmnemonic \SC_ATSLASTERRCMD */  
                                                 /**< \brief  Last ATS Errant Command Num */
    uint16      LastRtsErrSeq;                   /**< \sctlmmnemonic \SC_RTSLASTERRID */ 
                                                 /**< \brief  Last RTS Errant Sequence Num */                                                    
    uint16      LastRtsErrCmd;                   /**< \sctlmmnemonic \SC_RTSLASTERRCMD */
    /**< \brief  The OFFSET in the RTS buffer of the command that had an error  It will be a WORD value i.e.
    1st command had an error, this value would be 0, if the 2nd command started at int8 10 in the buffer,
    this value would be 5. */    
    
    uint16      AppendCmdArg;                    /**< \sctlmmnemonic \SC_APPENDATSID */
                                                 /**< \brief ATS selection argument from most recent Append ATS command */
    uint16      AppendEntryCount;                /**< \sctlmmnemonic \SC_APPENDCOUNT */
                                                 /**< \brief Number of cmd entries in current Append ATS table */
    uint16      AppendByteCount;                 /**< \sctlmmnemonic \SC_APPENDSIZE */
                                                 /**< \brief Size of cmd entries in current Append ATS table */
    uint16      AppendLoadCount;                 /**< \sctlmmnemonic \SC_APPENDLOADS */
                                                 /**< \brief Total number of Append ATS table loads */
    uint32      AtpCmdNumber;                    /**< \sctlmmnemonic \SC_ATPCMDNUM */  
                                                 /**< \brief  current command number */    
    uint32      AtpFreeBytes[SC_NUMBER_OF_ATS];   /**< \sctlmmnemonic \SC_ATPFREEBYTES */ 
                                                  /**< \brief  Free Bytes in each ATS  */
    uint32      NextRtsTime;                      /**< \sctlmmnemonic \SC_NXTRTSTIME */  
                                                  /**< \brief next RTS cmd Absolute Time */
    uint32      NextAtsTime;                      /**< \sctlmmnemonic \SC_NXTATSTIME */    
                                                  /**< \brief Next ATS Command Time (seconds) */
                                                            
    uint16      RtsExecutingStatus[(SC_NUMBER_OF_RTS + 15) / 16];    /**< \sctlmmnemonic \SC_RTSEXEC */
    /**< \brief RTS executing status bit map where each uint16 represents 16 RTS numbers.  Note: array
     index numbers and bit numbers use base zero indexing, but RTS numbers use base one indexing.  Thus,
     the LSB (bit zero) of uint16 array index zero represents RTS number 1, and bit one of uint16 array
     index zero represents RTS number 2, etc.  If an RTS is IDLE, then the corresponding bit is zero.
     If an RTS is EXECUTING, then the corresponding bit is one. */    

    uint16      RtsDisabledStatus[(SC_NUMBER_OF_RTS + 15) / 16];     /**< \sctlmmnemonic \SC_RTSDISABLED */
    /**< \brief RTS disabled status bit map where each uint16 represents 16 RTS numbers.  Note: array
     index numbers and bit numbers use base zero indexing, but RTS numbers use base one indexing.  Thus,
     the LSB (bit zero) of uint16 array index zero represents RTS number 1, and bit one of uint16 array
     index zero represents RTS number 2, etc.  If an RTS is ENABLED, then the corresponding bit is zero.
     If an RTS is DISABLED, then the corresponding bit is one. */    

}   SC_HkTlm_t;


/** 
**  \brief No Arguments Command
**  For command details see #SC_NOOP_CC, #SC_RESET_COUNTERS_CC, #SC_STOP_ATS_CC, #SC_SWITCH_ATS_CC
**  Also see #SC_SEND_HK_MID
*/
typedef struct
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    
} SC_NoArgsCmd_t;

/** 
**  \brief ATS Id Command
**  For command details see #SC_START_ATS_CC
*/
typedef struct 
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16              AtsId;              /**< \brief The ID of the ATS to start, 1 = ATS_A, 2 = ATS_B */    
} SC_StartAtsCmd_t;

/** 
**  \brief RTS Id Command
**  For command details see #SC_START_RTS_CC, #SC_STOP_RTS_CC, #SC_DISABLE_RTS_CC, #SC_ENABLE_RTS_CC
*/
typedef struct {
    
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16              RtsId;              /**< \brief The ID of the RTS to start, 1 through #SC_NUMBER_OF_RTS */
}  SC_RtsCmd_t;

/** 
**  \brief Jump running ATS to a new time Command 
**  For command details see #SC_JUMP_ATS_CC
*/
typedef struct {
    
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32              NewTime;            /**< \brief the time to 'jump' to                           */
    
}  SC_JumpAtsCmd_t;

/** 
**  \brief Continue ATS on failure command
**  For command details see #SC_CONTINUE_ATS_ON_FAILURE_CC
*/
typedef struct {
    
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16              ContinueState;      /**< \brief TRUE or FALSE, to continue ATS after a failure  */
    
} SC_SetContinueAtsOnFailureCmd_t ;

/** 
**  \brief Append to ATS Command
**  For command details see #SC_APPEND_ATS_CC
*/
typedef struct 
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16              AtsId;              /**< \brief The ID of the ATS to append to, 1 = ATS_A, 2 = ATS_B */    
} SC_AppendAtsCmd_t;

#if (SC_ENABLE_GROUP_COMMANDS == TRUE)
/** 
**  \brief RTS Group Command
**  For command details see #SC_START_RTSGRP_CC, #SC_STOP_RTSGRP_CC, #SC_DISABLE_RTSGRP_CC, #SC_ENABLE_RTSGRP_CC
*/
typedef struct {
    
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16              FirstRtsId;         /**< \brief ID of the first RTS to act on, 1 through #SC_NUMBER_OF_RTS */
    uint16              LastRtsId;          /**< \brief ID of the last RTS to act on, 1 through #SC_NUMBER_OF_RTS */
}  SC_RtsGrpCmd_t;
#endif

#endif /* _sc_msg_ */

/************************/
/*  End of File Comment */
/************************/
