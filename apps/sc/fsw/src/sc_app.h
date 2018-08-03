/*************************************************************************
 ** File:
 **   $Id: sc_app.h 1.14 2015/03/02 12:58:41EST sstrege Exp  $
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
 **     This file contains the Stored Command main event loop header
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_app.h  $
 **   Revision 1.14 2015/03/02 12:58:41EST sstrege 
 **   Added copyright information
 **   Revision 1.13 2011/09/26 13:49:40EDT lwalling 
 **   Change function and structure names from HkStatus to HkPacket, remove references to CDS
 **   Revision 1.12 2011/03/15 17:28:13EDT lwalling 
 **   Change boolean AutoStartFlag to uint16 AutoStartRTS
 **   Revision 1.11 2010/09/28 10:34:34EDT lwalling 
 **   Update list of included header files
 **   Revision 1.10 2010/05/18 14:13:58EDT lwalling 
 **   Change AtsCmdIndexBuffer contents from entry pointer to entry index
 **   Revision 1.9 2010/05/05 11:15:03EDT lwalling 
 **   Cleanup function return code definitions, move dup cmd test array to global structure
 **   Revision 1.8 2010/04/21 15:35:51EDT lwalling 
 **   Changed local storage of Append ATS table use from bytes to words
 **   Revision 1.7 2010/04/16 15:21:36EDT lwalling 
 **   Add status variables for Append ATS table
 **   Revision 1.6 2010/04/15 15:17:43EDT lwalling 
 **   Add macro definitions for Append ATS verify function
 **   Revision 1.5 2010/04/05 11:47:35EDT lwalling 
 **   Add Append ATS table data structures
 **   Revision 1.4 2010/03/26 18:02:02EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.3 2009/01/26 14:44:42EST nyanchik 
 **   Check in of Unit test
 **   Revision 1.2 2009/01/05 08:26:49EST nyanchik 
 **   Check in after code review changes
 
 *************************************************************************/

#ifndef _sc_app_
#define _sc_app_

/*************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "sc_platform_cfg.h"
#include "sc_tbldefs.h"
#include "sc_msgdefs.h"
#include "sc_msg.h"


/************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name SC number of bytes in a word */ 
/** \{ */
#define SC_BYTES_IN_WORD            2        /**< \brief Words are used to define table lengths   */
/** \} */

/**
** \name Sizes of the heats for the RTC's and ATC's */ 
/** \{ */
#define SC_ATS_HEADER_SIZE (sizeof(SC_AtsEntryHeader_t) - CFE_SB_CMD_HDR_SIZE)
#define SC_RTS_HEADER_SIZE (sizeof(SC_RtsEntryHeader_t) - CFE_SB_CMD_HDR_SIZE)
/** \} */

/**
** \name SC error return value */ 
/** \{ */
#define SC_ERROR                -1
/** \} */

/**
** \name  Cmd pipe information*/ 
/** \{ */
#define SC_CMD_PIPE_NAME        "SC_CMD_PIPE"
/** \} */

#define SC_ATS_HDR_WORDS          (sizeof(SC_AtsEntryHeader_t) / 2)
#define SC_ATS_HDR_NOPKT_WORDS    (SC_ATS_HDR_WORDS - (CFE_SB_CMD_HDR_SIZE / 2))

#define SC_DUP_TEST_UNUSED  -1

/*********************************************************************************************/

/** 
**  \brief ATS Table Entry Header Type
*/
typedef struct
{
    /* Command identifier, range = 1 to SC_MAX_ATS_CMDS */
    uint16  CmdNumber;

    /* 32 bit absolute time, stored as two 16 bit values */
    uint16  TimeTag1;
    uint16  TimeTag2;

    /* Command packet header */
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

    /*
    ** Note: the command packet data is variable length,
    **       only the command packet header is shown here.
    */

}SC_AtsEntryHeader_t;


/** 
**  \brief  RTS Command Header Type
*/
typedef struct {
    /* The Cmd Header comes after the Time tag so the
       Cmd Header butts up against the rest of the cFE command */
    SC_RelTimeTag_t     TimeTag;
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    /* Note: This structure has natural alignment and needs no padding */
}SC_RtsEntryHeader_t;

/** 
**  \brief SC Operational Data Structure 
*/
typedef struct
{
    CFE_SB_MsgPtr_t         MsgPtr;                             /**< \brief Pointer to command message          */
    CFE_SB_PipeId_t         CmdPipe;                            /**< \brief Command pipe ID                     */
 
    CFE_TBL_Handle_t        AtsTblHandle[SC_NUMBER_OF_ATS];     /**< \brief Table handles for all ATS tables    */
    uint16*                 AtsTblAddr[SC_NUMBER_OF_ATS];       /**< \brief Table Addresses for all ATS tables  */
    
    CFE_TBL_Handle_t        AppendTblHandle;                    /**< \brief Table handle for Append ATS table   */
    uint16*                 AppendTblAddr;                      /**< \brief Table Address for Append ATS table  */
    
    CFE_TBL_Handle_t        RtsTblHandle[SC_NUMBER_OF_RTS];     /**< \brief Table handles for all RTS tables    */
    uint16*                 RtsTblAddr[SC_NUMBER_OF_RTS];       /**< \brief Table addresses for all RTS tables  */
    
    CFE_TBL_Handle_t        AtsInfoHandle;                      /**< \brief Table handle the for ATS Info Table */
    SC_AtsInfoTable_t*      AtsInfoTblAddr;                     /**< \brief Table address for the ATS Info Table*/
    
    CFE_TBL_Handle_t        AppendInfoHandle;                   /**< \brief Table handle for Append Info Table  */
    SC_AtsInfoTable_t*      AppendInfoTblAddr;                  /**< \brief Table address for Append Info Table */
    
    CFE_TBL_Handle_t        RtsInfoHandle;                      /**< \brief Table handle for RTS Info Table     */
    SC_RtsInfoEntry_t*      RtsInfoTblAddr;                     /**< \brief Table address for RTS INfo Table    */

    CFE_TBL_Handle_t        RtsCtrlBlckHandle;                  /**< \brief Table handle for the RTP ctrl block */
    SC_RtpControlBlock_t*   RtsCtrlBlckAddr;                    /**< \brief Table address for the RTP ctrl block*/
    
    CFE_TBL_Handle_t        AtsCtrlBlckHandle;                  /**< \brief Table handle for the ATP ctrl block */
    SC_AtpControlBlock_t*   AtsCtrlBlckAddr;                    /**< \brief Table address for the ATP ctrl block*/
    
    CFE_TBL_Handle_t        AtsCmdStatusHandle[SC_NUMBER_OF_ATS];   /**< \brief ATS Cmd Status table handle     */
    uint8*                  AtsCmdStatusTblAddr[SC_NUMBER_OF_ATS];  /**< \brief ATS Cmd Status table address    */
    
    int32                   AtsDupTestArray[SC_MAX_ATS_CMDS];   /**< \brief ATS test for duplicate cmd numbers  */

    uint16                  NumCmdsSec;                         /**< \brief the num of cmds that have gone out
                                                                      in a one second period                    */
    SC_HkTlm_t              HkPacket;                           /**< \brief SC Housekeeping structure           */

    
} SC_OperData_t;

/** 
**  \brief SC Application Data Structure
*/
typedef struct
{
    uint8                   ContinueAtsOnFailureFlag ; 
    /**< \brief If an Ats command fails checksum the ATS execution will continue if 
          This flag is set to TRUE and will stop if this flag is set to FALSE*/

    uint16                  AtsTimeIndexBuffer[SC_NUMBER_OF_ATS][SC_MAX_ATS_CMDS]; 
    /**< \brief  This table is used to keep a time ordered listing
         of ATS commands. The index used is the ATS command number. The first
         in this table holds the command number of the command that will execute
         first, the second entry has the number of the 2nd cmd, etc.. */
         
    int32                   AtsCmdIndexBuffer[SC_NUMBER_OF_ATS][SC_MAX_ATS_CMDS]; 
  
    uint8                   NextProcNumber;   /**< \brief the next command processor number */
    SC_AbsTimeTag_t         NextCmdTime[2];   /**< \brief The overall next command time  0 - ATP, 1- RTP*/
    SC_AbsTimeTag_t         CurrentTime;      /**< \brief this is the current time for SC */
    uint16                  CmdErrCtr;        /**< \brief Counts Request Errors  */
    uint16                  CmdCtr;           /**< \brief  Counts Ground Requests */
    uint16                  RtsActiveErrCtr;  /**< \brief Increments when an attempt to start an RTS fails */
    uint16                  RtsActiveCtr;     /**< \brief Increments when an RTS is started without error */
    uint16                  AtsCmdCtr;        /**< \brief Total ATS cmd counter counts commands sent by the ATS */
    uint16                  AtsCmdErrCtr;     /**< \brief Total ATS cmd Error ctr command errors in the ATS */
    uint16                  RtsCmdCtr;        /**< \brief Counts TOTAL rts commands that were sent out from ALL active RTSs */
    uint16                  RtsCmdErrCtr;     /**< \brief Counts TOTAL number of errs from ALL RTSs that are active */
    uint16                  LastAtsErrSeq;    /**< \brief Last ATS Errant Sequence Num Values: 1 or 2 */
    uint16                  LastAtsErrCmd;    /**< \brief Last ATS Errant Command Num */
    uint16                  LastRtsErrSeq;    /**< \brief Last RTS Errant Sequence Num */
    uint16                  LastRtsErrCmd;   
    /**< \brief The OFFSET in the RTS buffer of the command. It will be a WORD value 
          i.e.  1st command had an error, this value would be 0, if the 2nd command 
          started at int8 10 in the buffer, this value would be 5. */
                                   
    uint16                  AppendCmdArg;     /**< \brief ATS selection argument from most recent Append ATS command */
    uint16                  AppendEntryCount; /**< \brief Number of cmd entries in current Append ATS table */
    uint16                  AppendWordCount;  /**< \brief Size of cmd entries in current Append ATS table */
    uint16                  AppendLoadCount;  /**< \brief Total number of Append ATS table loads */

    uint16                  Unused;           /**< \brief Unused */
    uint16                  AutoStartRTS;     /**< \brief Start selected auto-exec RTS after init */                              
    
} SC_AppData_t;

/************************************************************************
** Exported Data
*************************************************************************/
extern SC_AppData_t     SC_AppData;
extern SC_OperData_t    SC_OperData;

#endif /* _sc_app_ */

/************************/
/*  End of File Comment */
/************************/
