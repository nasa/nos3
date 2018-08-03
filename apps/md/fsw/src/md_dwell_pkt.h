/*************************************************************************
** File:
**   $Id: md_dwell_pkt.h 1.3 2015/03/01 17:17:49EST sstrege Exp  $
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
**   Specification for functions used to populate and send Memory Dwell packets.
**
**
** Notes:
**
**   $Log: md_dwell_pkt.h  $
**   Revision 1.3 2015/03/01 17:17:49EST sstrege 
**   Added copyright information
**   Revision 1.2 2008/10/21 13:59:39EDT nsschweiss 
**   Added MD_StartDwellStream to initialize dwell packet processing parameters.
**   Revision 1.1 2008/07/02 13:48:29EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/src/project.pj
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_dwell_pkt_h_
#define _md_dwell_pkt_h_

#include "common_types.h"



/*****************************************************************************/
/**
** \brief Process Dwell Packets
**
** \par Description
**      Look at each table.  If the table is enabled and
**      its countdown counter times out, add dwell data to dwell packet until a 
**      packet delay is reached or the dwell packet is completed.
**      If dwell packet is completed, send the packet and reset the counter.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
void MD_DwellLoop( void );


/*****************************************************************************/
/**
** \brief Read a Single Dwell Address
**
** \par Description
**          Copy memory value from a single dwell address to a dwell packet.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblIndex identifies source dwell table (0..)
**
** \param[in] EntryIndex identifies dwell entry within specified table (0..)
**                                      
** \retval CFE_SUCCESS if copy was performed successfully; non-zero otherwise.
******************************************************************************/
int32 MD_GetDwellData( uint16 TblIndex, uint16 EntryIndex );


/*****************************************************************************/
/**
** \brief Send Memory Dwell Packet
**
** \par Description
**          Send contents of memory pointed to by a table as a
**          telemetry packet.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TableIndex Identifies source dwell table for the dwell packet.
**                                      
** \retval None
******************************************************************************/
void MD_SendDwellPkt( uint16 TableIndex );

/*****************************************************************************/
/**
** \brief Start Dwell Stream
**
** \par Description
**          Initialize parameters used to control generation of dwell packets.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TableIndex Identifies source dwell table for the dwell packet.
**                                      
** \retval None
******************************************************************************/
void MD_StartDwellStream (uint16 TableIndex );




#endif /* _md_dwell_pkt_ */
/************************/
/*  End of File Comment */
/************************/
