/************************************************************************
** File:
**   $Id: hk_msg.h 1.8 2015/03/04 14:58:31EST sstrege Exp  $
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
**  The CFS Housekeeping (HK) Application header file
**
** Notes:
**
** $Log: hk_msg.h  $
** Revision 1.8 2015/03/04 14:58:31EST sstrege 
** Added copyright information
** Revision 1.7 2009/12/03 17:00:54EST jmdagost 
** Member moved from hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/public_inc/project.pj to hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj.
** Revision 1.6 2009/12/03 17:00:54ACT jmdagost 
** Member moved from hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj to hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/public_inc/project.pj.
** Revision 1.5 2009/12/03 17:00:54ACT jmdagost 
** Deleted message definitions that are now in hk_msgdefs.h.
** Revision 1.4 2009/04/18 12:55:17EDT dkobe 
** Updates to correct doxygen comments
** Revision 1.3 2008/05/15 09:32:24EDT rjmcgraw 
** DCR1647:1 Added padding to hk tlm packet
** Revision 1.2 2008/04/09 16:40:35EDT rjmcgraw 
** Member moved from hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/public_inc/project.pj to hk_msg.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj.
** Revision 1.1 2008/04/09 15:40:35ACT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/public_inc/project.pj
**
*************************************************************************/
#ifndef _hk_msg_h_
#define _hk_msg_h_


/*************************************************************************
** Includes
**************************************************************************/
#include "cfe.h"


/****************************
**  HK Command Formats     **
*****************************/

/**
**  \brief Send Combined Output Message Command
**
**  This structure contains the format of the command used to inform HK to send
**  the specified combined output message 
*/
typedef struct
{
	CFE_SB_CmdHdr_t   Hdr;/**< \brief cFE Software Bus Command Message Header #CFE_SB_CmdHdr_t */
	CFE_SB_MsgId_t    OutMsgToSend;/**< \brief MsgId #CFE_SB_MsgId_t of combined tlm pkt to send  */
	
} HK_Send_Out_Msg_t;



/****************************
**  HK Telemetry Formats   **
*****************************/

/**
**  \hktlm HK Application housekeeping Packet
*/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];/**< \brief cFE Software Bus Telemetry Message Header */
 
    uint8   CmdCounter;         /**< \hktlmmnemonic \HK_CMDPC
                                \brief Count of valid commands received */
    uint8   ErrCounter;         /**< \hktlmmnemonic \HK_CMDEC
                                \brief Count of invalid commands received */
    uint16  Padding;			/**< \hktlmmnemonic \HK_PADDING
                                \brief Padding to force 32 bit alignment */
    uint16  CombinedPacketsSent;/**< \hktlmmnemonic \HK_CMBPKTSSENT
                                \brief Count of combined tlm pkts sent */
    uint16  MissingDataCtr;     /**< \hktlmmnemonic \HK_MISSDATACTR
                                \brief Number of times missing data was detected */ 
    uint32  MemPoolHandle;      /**< \hktlmmnemonic \HK_MEMPOOLHNDL
                                \brief Memory pool handle used to get mempool diags */

} HK_HkPacket_t;

      
#endif /* _hk_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
