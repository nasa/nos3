/************************************************************************
** File:
**   $Id: md_app.h 1.7 2015/03/01 17:17:28EST sstrege Exp  $
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
**   Unit specification for the Core Flight System (CFS) 
**   Memory Dwell (MD) Application.  
**
** Notes:
**
** $Log: md_app.h  $
** Revision 1.7 2015/03/01 17:17:28EST sstrege 
** Added copyright information
** Revision 1.6 2009/10/02 19:12:23EDT aschoeni 
** Updated verify.h and platform config file.
** Revision 1.5 2009/09/30 14:13:02EDT aschoeni 
** Added check to make sure signature is null terminated.
** Revision 1.4 2009/01/12 14:33:00EST nschweis 
** Removed #define directive for MD_DEBUG.  CPID 4688:1.
** Revision 1.3 2008/08/07 16:47:09EDT nsschweiss 
** Changed included included file name from cfs_lib.h to cfs_utils.h.
** Revision 1.2 2008/07/02 13:45:26EDT nsschweiss 
** CFS MD Post Code Review Version
** Date: 08/05/09
** CPID: 1653:2
**
*************************************************************************/

#ifndef _md_app_h_
#define _md_app_h_

/************************************************************************
** Includes
*************************************************************************/
#include "common_types.h"
#include "md_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "md_msgids.h"
#include "cfs_utils.h"
#include "md_msg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/

#define MD_DWELL_TABLE_BASENAME  "DWELL_TABLE"

/** 
**  \name   Function Return Codes for Table Validation function and related routines */
/** \{ */

#define MD_TBL_ENA_FLAG_ERROR   (0xc0000001L) /**< \brief Enable flag in table load is invalid (valid values are 0 and 1) */

#define MD_ZERO_RATE_TBL_ERROR  (0xc0000002L) /**< \brief Table has zero value for total delay, and at least one dwell specified */

#define MD_RESOLVE_ERROR        (0xc0000003L) /**< \brief Symbolic address couldn't be resolved */ 

#define MD_INVALID_ADDR_ERROR   (0xc0000004L) /**< \brief Invalid address found */

#define MD_INVALID_LEN_ERROR    (0xc0000005L) /**< \brief Invalid dwell length found */

#define MD_NOT_ALIGNED_ERROR    (0xc0000006L) /**< \brief Dwell address improperly aligned for specified dwell length */

#define MD_SIG_LEN_TBL_ERROR    (0xc0000007L) /**< \brief Signature not null terminated in table */

/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief  MD enum used for representing values for enable state
*/
enum MD_Dwell_States {MD_DWELL_STREAM_DISABLED, MD_DWELL_STREAM_ENABLED};

/** 
**  \brief MD structure for specifying individual memory dwell
*/
typedef struct
{
	uint16 Length;          /**< \brief  Length of dwell  field in bytes.  0 indicates null entry. */
	uint16 Delay;           /**< \brief  Delay before following dwell sample in terms of number of task wakeup calls */
    uint32 ResolvedAddress; /**< \brief Dwell address in numerical form */
} MD_DwellControlEntry_t;

/** 
**  \brief MD structure for controlling dwell operations
*/
typedef struct
{
    uint16 Enabled;         /**< \brief Is table is enabled for dwell?  Valid values are #MD_DWELL_STREAM_DISABLED, #MD_DWELL_STREAM_ENABLED */
    uint16 AddrCount;       /**< \brief Number of dwell addresses to telemeter  */
    uint32 Rate;            /**< \brief Packet issuance interval in terms of number of task wakeup calls */
    uint32 Countdown;       /**< \brief Counts down from Rate to 0, then read next address */
	uint16 PktOffset;       /**< \brief Tracks where to write next data in dwell pkt */
	uint16 CurrentEntry;    /**< \brief Current entry in dwell table */
	uint16 DataSize;        /**< \brief Total number of data bytes specified in dwell table */
    uint16 Filler;          /**< \brief Preserves alignment */
	MD_DwellControlEntry_t Entry[MD_DWELL_TABLE_SIZE];  /**< \brief Array of individual memory dwell specifications */
#if MD_SIGNATURE_OPTION == 1   
    char Signature[MD_SIGNATURE_FIELD_LENGTH];          /**< \brief Signature string used for dwell table to dwell pkt traceability */
#endif
} MD_DwellPacketControl_t;


/** 
**  \brief MD global data structure
*/
typedef struct
{
    /*
    **  Command interface counters
    */
    uint8                       CmdCounter;     /**< \brief MD Application Command Counter */
    uint8                       ErrCounter;     /**< \brief MD Application Error Counter */
    
    /* 
    **  Housekeeping telemetry packet
    */
    MD_HkTlm_t                  HkPkt;          /**< \brief Housekeeping telemetry packet */
    
    /*  
    **  Operational data (not reported in housekeeping)
    */
    CFE_SB_MsgPtr_t             MsgPtr;            /**< \brief Pointer to command message    */
    CFE_SB_PipeId_t             CmdPipe;           /**< \brief Command pipe ID               */
    MD_DwellPacketControl_t     MD_DwellTables[MD_NUM_DWELL_TABLES]; /**< \brief Array of packet control structures    */
    MD_DwellPkt_t               MD_DwellPkt[MD_NUM_DWELL_TABLES];    /**< \brief Array of dwell packet  structures    */
    
    /*
    ** RunStatus variable used in the main processing loop
    */
    uint32                      RunStatus;         /**< \brief Application run status         */

    /*
    **  Initialization data (not reported in housekeeping)
    */

    char                        MD_TableName[MD_NUM_DWELL_TABLES][CFE_TBL_MAX_NAME_LENGTH + 1]; /**< \brief Array of table names used for TBL Services */
    CFE_TBL_Handle_t            MD_TableHandle[ MD_NUM_DWELL_TABLES];  /**< \brief Array of handle ids provided by TBL Services  */

} MD_AppData_t;
/************************************************************************
** Exported Functions
*************************************************************************/
/*****************************************************************************/
/**
** \brief Entry Point and main loop for the Memory Dwell task.
**
** \par Description
**          Call MD_AppInit to initialize the task. 
**          LOOP:
**            Copy any newly loaded tables
**            Pend on the Software Bus waiting to receive next message.
**            If MD_WAKEUP_MID Message is received, call MD_DwellLoop 
**               to send whatever memory values are being 'dwelled on'. 
**            If MD_CMD_MID Message is received, call MD_ExecRequest
**               for processing.
**            If MD_SEND_HK_MID Message is received, call MD_HkStatus
**               for processing.
**
** \par Assumptions, External Events, and Notes:
**          Associated with each dwell address is a 'Delay' which is the 
**          number of wake-up calls to wait before recording the next value 
**          for this Dwell Table.  The 'Rate' value associated with
**          Dwell Table is the sum of all individual delays.  
**          For a table to be dwelled on, its rate must be >=1.
**
** \retval None
******************************************************************************/
void MD_AppMain( void );

#endif /* _md_app_h_ */
/************************/
/*  End of File Comment */
/************************/
