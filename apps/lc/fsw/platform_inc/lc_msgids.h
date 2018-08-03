/*************************************************************************
** File:
**   $Id: lc_msgids.h 1.2 2015/03/04 16:09:56EST sstrege Exp  $
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
**   CFS Limit Checker (LC) Application Message IDs
**
** Notes:
**
**   $Log: lc_msgids.h  $
**   Revision 1.2 2015/03/04 16:09:56EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:35EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/platform_inc/project.pj
**   Revision 1.2 2011/01/19 12:45:40EST jmdagost 
**   Moved two message parameters to the message IDs file for scheduler table access.
**   Revision 1.1 2008/10/29 14:18:12EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/platform_inc/project.pj
** 
*************************************************************************/
#ifndef _lc_msgids_
#define _lc_msgids_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name LC Command Message IDs */ 
/** \{ */
#define LC_CMD_MID           0x18A4    /**< \brief Msg ID for cmds to LC                */
#define LC_SEND_HK_MID       0x18A5    /**< \brief Msg ID to request LC housekeeping    */
#define LC_SAMPLE_AP_MID     0x18A6    /**< \brief Msg ID to request actionpoint sample */
/** \} */
/*
#define LC_SPARE1            0x18A7
#define LC_SPARE2            0x18A8
*/

/**
** \name LC Telemetry Message IDs */ 
/** \{ */
#define LC_HK_TLM_MID        0x08A7    /**< \brief LC Housekeeping Telemetry */
/** \} */
/*
#define LC_TLM_SPARE1        0x08A8
#define LC_TLM_SPARE2        0x08A9
*/

/**
** \name Special Values for Commands */ 
/** \{ */
#define LC_ALL_ACTIONPOINTS         0xFFFF
#define LC_ALL_WATCHPOINTS          0xFFFF
/** \} */

#endif /*_lc_msgids_*/

/************************/
/*  End of File Comment */
/************************/
