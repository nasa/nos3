/************************************************************************
** File:
**   $Id: md_msg.h 1.6 2015/03/01 17:17:26EST sstrege Exp  $
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
**   Specification for the CFS Memory Dwell command and telemetry 
**   messages.
**
** Notes:
**
**   $Log: md_msg.h  $
**   Revision 1.6 2015/03/01 17:17:26EST sstrege 
**   Added copyright information
**   Revision 1.5 2009/10/02 19:23:21EDT aschoeni 
**   split macros out to msgdefs.h
**   Revision 1.4 2008/10/06 10:29:49EDT dkobe 
**   Updated and Corrected Doxygen Comments
**   Revision 1.3 2008/08/07 16:24:43EDT nsschweiss 
**   Changed included filename from cfs_lib.h to cfs_utils.h.
**   Revision 1.2 2008/07/02 13:29:38EDT nsschweiss 
**   CFS MD Post Code Review Version
**   Date: 08/05/09
**   CPID: 1653:2
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_msg_h_
#define _md_msg_h_

/*
** Required header files...
*/
#include "md_platform_cfg.h"
#include "cfe.h"
/* cfs_utils.h needed for CFS_SymAddr_t */
#include "cfs_utils.h"
#include "md_msgdefs.h"

/*************************************************************************/
/************************************************************************
** Type Definitions
*************************************************************************/

/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The Wakeup Message
** -# The No-Op Command (For details, see #MD_NOOP_CC)
** -# The Reset Counters Command (For details, see #MD_RESET_CNTRS_CC)
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];   /**< \brief cFE Software Bus Command Message Header */

} MD_NoArgsCmd_t;

/**
** \brief Start and Stop Dwell Commands
**
** For command details, see #MD_START_DWELL_CC and #MD_STOP_DWELL_CC
**
**/
typedef struct                             
{
    uint8           Header[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
    uint16          TableMask;        /**< \brief 0x0001=TBL1  bit,
            0x0002=TBL2 bit,0x0004=TBL3 bit,0x0008=TBL4 enable bit, etc. */
} MD_CmdStartStop_t;



/**
** \brief Jam Dwell Command
**
** For command details, see #MD_JAM_DWELL_CC
**
**/
typedef struct                              /* for MD_JAM_DWELL */
{
    uint8    Header[CFE_SB_CMD_HDR_SIZE];  /**< \brief cFE Software Bus Command Message Header */
    uint16   TableId;           /**< \brief Table Id: 1..#MD_NUM_DWELL_TABLES */
    uint16   EntryId;           /**< \brief Address index: 1..#MD_DWELL_TABLE_SIZE  */
    uint16	 FieldLength;       /**< \brief Length of Dwell Field : 0, 1, 2, or 4  */
    uint16	 DwellDelay;        /**< \brief Dwell Delay (number of task wakeup calls before following dwell) */
    CFS_SymAddr_t DwellAddress; /**< \brief Dwell Address in #CFS_SymAddr_t format */
} MD_CmdJam_t;

#if MD_SIGNATURE_OPTION == 1  
/**
** \brief Set Signature Command
**
** For command details, see #MD_SET_SIGNATURE_CC
**
**/
typedef struct                              
{
    uint8    Header[CFE_SB_CMD_HDR_SIZE];  
    /**< \brief cFE Software Bus Command Message Header */
    uint16   TableId;      /**< \brief Table Id: 1..MD_NUM_DWELL_TABLES */
    uint16   Padding; /**< \brief Padding  */
    char     Signature[MD_SIGNATURE_FIELD_LENGTH];
} MD_CmdSetSignature_t;

#endif

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/** 
**  \mdtlm Memory Dwell HK Telemetry format
**/

typedef struct
    {
    uint8             TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< \brief cFE SB Tlm Msg Hdr */
    /*
    ** Task command interface counters...
    */
	uint8                  InvalidCmdCntr;     /**< \mdtlmmnemonic \MD_CMDEC 
                                                    \brief Count of invalid commands received */
    uint8                  ValidCmdCntr;       /**< \mdtlmmnemonic \MD_CMDPC 
                                                    \brief Count of valid commands received */
    uint16                 DwellEnabledMask;   /**< \mdtlmmnemonic \MD_ENABLEMASK 
                                                    \brief Each bit in bit mask enables a table
                                                    0x0001=TBL1 enable bit,0x0002=TBL2 enable bit,
                                                    0x0004=TBL3 enable bit,0x0008=TBL4 enable bit, etc. */
    uint16  DwellTblAddrCount[MD_NUM_DWELL_TABLES]; /**< \mdtlmmnemonic \MD_ADDRCNT
                                                         \brief Number of dwell addresses in table */
    uint16  NumWaitsPerPkt[MD_NUM_DWELL_TABLES];    /**< \mdtlmmnemonic \MD_RATES
                                                         \brief Number of delay counts in table */
	uint16  ByteCount[MD_NUM_DWELL_TABLES];         /**< \mdtlmmnemonic \MD_DATASIZE
                                                         \brief Number of bytes of data specified by table */
	uint16  DwellPktOffset[MD_NUM_DWELL_TABLES];    /**< \mdtlmmnemonic \MD_DWPKTOFFSET 
                                                         \brief Current write offset within dwell pkt data region  */
	uint16  DwellTblEntry[MD_NUM_DWELL_TABLES];     /**< \mdtlmmnemonic \MD_DWTBLENTRY 
                                                         \brief Next dwell table entry to be processed  */

	uint16  Countdown[MD_NUM_DWELL_TABLES];         /**< \mdtlmmnemonic \MD_COUNTDOWN 
                                                         \brief Current value of countdown timer  */

    } MD_HkTlm_t;

#define MD_HK_TLM_LNGTH        sizeof(MD_HkTlm_t)

/**********************************/
/** 
**  \mdtlm Memory Dwell Telemetry Packet format 
**/
typedef struct                          /* Actual Dwell information */
    {
    uint8             TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< \brief cFE SB Tlm Msg Hdr */
    
    uint8             TableId;         /**< \mdtlmmnemonic \MD_TABLEID
                                            \brief TableId from 1 to #MD_NUM_DWELL_TABLES */
    
    uint8             AddrCount;       /**< \mdtlmmnemonic \MD_NUMADDRESSES 
                                            \brief Number of addresses being sent - 1..#MD_DWELL_TABLE_SIZE valid */
    
    uint16            ByteCount;       /**< \mdtlmmnemonic \MD_PKTDATASIZE
                                            \brief Number of bytes of dwell data contained in packet */

    
    uint32            Rate;            /**< \mdtlmmnemonic \MD_RATE \brief Number of counts between packet sends*/

#if MD_SIGNATURE_OPTION == 1      
    char                Signature[MD_SIGNATURE_FIELD_LENGTH];    
                                       /**< \mdtlmmnemonic \MD_SIGNATURE \brief Signature */
    
#endif

    uint8             Data[MD_DWELL_TABLE_SIZE*4];   
                                      /**< \mdtlmmnemonic \MD_DWELLDATA 
                                           \brief Dwell data ( number of bytes varies up to MD_DWELL_TABLE_SIZE *4) */
    
    } MD_DwellPkt_t;

#define MD_DWELL_PKT_LNGTH         (sizeof(MD_DwellPkt_t))


/*************************************************************************/

#endif /* _md_msg_ */

/************************/
/*  End of File Comment */
/************************/
