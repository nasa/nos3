/*************************************************************************
** File:
**   $Id: hs_tbldefs.h 1.2 2015/11/12 14:25:25EST wmoleski Exp  $
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
**   Specification for the CFS Health and Safety (HS) table related
**   constant definitions.
**
** Notes:
**   These Macro definitions have been put in this file (instead of
**   hs_tbl.h) so this file can be included directly into ASIST build
**   test scripts. ASIST RDL files can accept C language #defines but
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE!
**   ADD THEM TO hs_tbl.h IF NEEDED!
**
**   $Log: hs_tbldefs.h  $
**   Revision 1.2 2015/11/12 14:25:25EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.3 2015/03/03 12:16:09EST sstrege 
**   Added copyright information
**   Revision 1.2 2009/05/04 17:44:29EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:46EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_tbldefs_h_
#define _hs_tbldefs_h_

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name HS Table Name Strings */
/** \{ */
#define HS_AMT_TABLENAME    "AppMon_Tbl"
#define HS_EMT_TABLENAME    "EventMon_Tbl"
#define HS_XCT_TABLENAME    "ExeCount_Tbl"
#define HS_MAT_TABLENAME    "MsgActs_Tbl"
/** \} */

/**
** \name Application Monitor Table (AMT) Action Types */
/** \{ */
#define HS_AMT_ACT_NOACT          0    /**< \brief No action is taken */
#define HS_AMT_ACT_PROC_RESET     1    /**< \brief Generates Processor Reset on failure */
#define HS_AMT_ACT_APP_RESTART    2    /**< \brief Attempts to restart application on failure */
#define HS_AMT_ACT_EVENT          3    /**< \brief Generates event message on failure */
#define HS_AMT_ACT_LAST_NONMSG    3    /**< \brief Index for finding end of non-message actions */
/** \} */

/**
** \name Event Monitor Table (EMT) Action Types */
/** \{ */
#define HS_EMT_ACT_NOACT          0    /**< \brief No action is taken */
#define HS_EMT_ACT_PROC_RESET     1    /**< \brief Generates Processor Reset on detection */
#define HS_EMT_ACT_APP_RESTART    2    /**< \brief Attempts to restart application on detection */
#define HS_EMT_ACT_APP_DELETE     3    /**< \brief Deletes application on detection */
#define HS_EMT_ACT_LAST_NONMSG    3    /**< \brief Index for finding end of non-message actions */
/** \} */

/**
** \name Execution Counters Table (XCT) Resource Types */
/** \{ */
#define HS_XCT_TYPE_NOTYPE        0    /**< \brief No type */
#define HS_XCT_TYPE_APP_MAIN      1    /**< \brief Counter for Application Main task */
#define HS_XCT_TYPE_APP_CHILD     2    /**< \brief Counter for Application Child task */
#define HS_XCT_TYPE_DEVICE        3    /**< \brief Counter for Device Driver */
#define HS_XCT_TYPE_ISR           4    /**< \brief Counter for Interrupt Service Routine */
/** \} */

/**
** \name Message Actions Table (MAT) Enable State */
/** \{ */
#define HS_MAT_STATE_DISABLED     0    /**< \brief Message Actions are Disabled */
#define HS_MAT_STATE_ENABLED      1    /**< \brief Message Actions are Enabled  */
#define HS_MAT_STATE_NOEVENT      2    /**< \brief Message Actions are Enabled but produce no events */
/** \} */

/**
** \name Application Monitor Table (AMT) Validation Error Enumerated Types */
/** \{ */
#define HS_AMTVAL_NO_ERR          0    /**< \brief No error                          */
#define HS_AMTVAL_ERR_ACT         -1   /**< \brief Invalid ActionType specified      */
#define HS_AMTVAL_ERR_NUL         -2   /**< \brief Null Safety Buffer not Null       */
/** \} */

/**
** \name Event Monitor Table (EMT) Validation Error Enumerated Types */
/** \{ */
#define HS_EMTVAL_NO_ERR          0    /**< \brief No error                          */
#define HS_EMTVAL_ERR_ACT         -1   /**< \brief Invalid ActionType specified      */
#define HS_EMTVAL_ERR_NUL         -2   /**< \brief Null Safety Buffer not Null       */
/** \} */

/**
** \name Event Counter Table (XCT) Validation Error Enumerated Types */
/** \{ */
#define HS_XCTVAL_NO_ERR          0    /**< \brief No error                          */
#define HS_XCTVAL_ERR_TYPE        -1   /**< \brief Invalid Counter Type specified    */
#define HS_XCTVAL_ERR_NUL         -2   /**< \brief Null Safety Buffer not Null       */
/** \} */

/**
** \name Message Actions Table (MAT) Validation Error Enumerated Types */
/** \{ */
#define HS_MATVAL_NO_ERR          0    /**< \brief No error                          */
#define HS_MATVAL_ERR_ID          -1   /**< \brief Invalid Message ID specified      */
#define HS_MATVAL_ERR_LEN         -2   /**< \brief Invalid Length specified          */
#define HS_MATVAL_ERR_ENA         -3   /**< \brief Invalid Enable State specified    */
/** \} */

#endif /*_hs_tbldefs_h_*/

/************************/
/*  End of File Comment */
/************************/
