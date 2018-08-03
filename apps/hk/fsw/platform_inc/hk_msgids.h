/************************************************************************
** File:
**   $Id: hk_msgids.h 1.5 2015/03/04 14:58:29EST sstrege Exp  $
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
**  The CFS Housekeeping (HK) Application Message IDs header file
**
** Notes:
**
** $Log: hk_msgids.h  $
** Revision 1.5 2015/03/04 14:58:29EST sstrege 
** Added copyright information
** Revision 1.4 2008/09/17 15:50:29EDT rjmcgraw 
** DCR4040:1 Carriage return/line feed missing on some lines
** Revision 1.3 2008/05/07 10:00:28EDT rjmcgraw 
** DCR1647:4 Removed the _CMD from HK_SEND_HK_CMD_MID
** Revision 1.2 2008/04/28 11:02:26EDT rjmcgraw 
** DCR1647:1 Changed msg id values to CFS cpu1 assigned IDs
** Revision 1.1 2008/04/23 11:39:38EDT rjmcgraw 
** Initial revision
** Member added to CFS project 
**
*************************************************************************/
#ifndef _hk_msgids_h_
#define _hk_msgids_h_

/**************************
** HK Command Message IDs
***************************/

#define HK_CMD_MID                     0x189A /**< \brief HK Ground Commands Message ID */
#define HK_SEND_HK_MID                 0x189B /**< \brief HK Send Housekeeping Data Cmd Message ID */

#define HK_SEND_COMBINED_PKT_MID       0x189C /**< \brief HK Send Combined Pkt Cmd Message ID */


/***************************
** HK Telemetry Message IDs
****************************/

#define HK_HK_TLM_MID                  0x089B /**< \brief HK Housekeeping Telemetry Message ID */

#define HK_COMBINED_PKT1_MID           0x089C /**< \brief HK Combined Packet 1 Message ID */
#define HK_COMBINED_PKT2_MID           0x089D /**< \brief HK Combined Packet 2 Message ID */
#define HK_COMBINED_PKT3_MID           0x089E /**< \brief HK Combined Packet 3 Message ID */

#define HK_COMBINED_PKT4_MID           0x089F /**< \brief HK Combined Packet 4 Message ID */

#endif /* _hk_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
