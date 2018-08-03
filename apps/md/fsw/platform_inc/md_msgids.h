/*************************************************************************
** File:
**   $Id: md_msgids.h 1.4 2015/03/01 17:17:50EST sstrege Exp  $
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
**   Specification for the CFS Memory Dwell macro constants that can
**   be configured from one mission to another
**
**
** Notes:
**
**   $Log: md_msgids.h  $
**   Revision 1.4 2015/03/01 17:17:50EST sstrege 
**   Added copyright information
**   Revision 1.3 2009/09/16 15:30:01EDT aschoeni 
**   Updated MIDs to development standards document value for cpu1
**   Revision 1.2 2008/09/12 12:16:11EDT nsschweiss 
**   Changed Message IDs to those specified in CFS Flight Software Development Standards Document, 
**   Appendix A.
**   CPID 4289:1.
**   Revision 1.1 2008/07/02 13:25:33EDT nsschweiss 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _md_msgids_h_
#define _md_msgids_h_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MD Telemetry Message Ids  */ 
/** \{ */
#define MD_HK_TLM_MID         0x0890 /**< \brief Message Id for Memory Dwell's housekeeping pkt */
#define MD_DWELL_PKT_MID_BASE 0x0891 /**< \brief Base Message Id for Memory Dwell's dwell packets.  MIDs will be base, base + 1, etc.  */

/** \} */

/**
** \name MD Command Message Ids */ 
/** \{ */
#define MD_CMD_MID            0x1890 /**< \brief Message Id for Memory Dwell's ground command */
#define MD_SEND_HK_MID        0x1891 /**< \brief Message Id for Memory Dwell's 'Send Housekeeping' message */
#define MD_WAKEUP_MID         0x1892 /**< \brief Message Id for Memory Dwell's wakeup message */
/** \} */


#endif /*_md_msgids_h_*/

/************************/
/*  End of File Comment */
/************************/
