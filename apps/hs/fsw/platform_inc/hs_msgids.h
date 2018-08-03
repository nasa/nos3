/*************************************************************************
** File:
**   $Id: hs_msgids.h 1.2 2015/11/12 14:25:23EST wmoleski Exp  $
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
**   CFS Health and Safety (HS) Application Message IDs
**
** Notes:
**
**   $Log: hs_msgids.h  $
**   Revision 1.2 2015/11/12 14:25:23EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.3 2015/03/03 12:16:04EST sstrege 
**   Added copyright information
**   Revision 1.2 2009/05/04 17:44:28EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:52:00EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/platform_inc/project.pj
**   Revision 1.1 2008/10/29 14:18:12EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _hs_msgids_h_
#define _hs_msgids_h_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name HS Command Message IDs */
/** \{ */
#define HS_CMD_MID           0x18AE    /**< \brief Msg ID for cmds to HS                */
#define HS_SEND_HK_MID       0x18AF    /**< \brief Msg ID to request HS housekeeping    */
#define HS_WAKEUP_MID        0x18B0    /**< \brief Msg ID to wake up HS                 */
/** \} */

/**
** \name HS Telemetry Message IDs */
/** \{ */
#define HS_HK_TLM_MID        0x08AD    /**< \brief HS Housekeeping Telemetry            */
/** \} */

#endif /*_hs_msgids_h_*/

/************************/
/*  End of File Comment */
/************************/
