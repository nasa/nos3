/*************************************************************************
** File:
**   $Id: mm_msgids.h 1.2 2015/03/02 14:26:59EST sstrege Exp  $
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
**   CFS Memory Manager (MM) Application Message IDs
**
** Notes:
**
**   $Log: mm_msgids.h  $
**   Revision 1.2 2015/03/02 14:26:59EST sstrege 
**   Added copyright information
**   Revision 1.1 2008/05/22 14:59:11EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _mm_msgids_
#define _mm_msgids_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MM Command Message IDs */ 
/** \{ */
#define MM_CMD_MID        0x1888    /**< \brief Msg ID for cmds to mm     */
#define MM_SEND_HK_MID    0x1889    /**< \brief Msg ID to request mm HK   */
/** \} */

/**
** \name MM Telemetry Message ID */ 
/** \{ */
#define MM_HK_TLM_MID     0x0887    /**< \brief MM Housekeeping Telemetry */
/** \} */

#endif /*_mm_msgids_*/

/************************/
/*  End of File Comment */
/************************/
