/*************************************************************************
** File:
**   $Id: lc_tbldefs.h 1.3 2015/03/04 16:09:55EST sstrege Exp  $
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
**   Specification for the CFS Limit Checker (LC) table related 
**   constant definitions.
**
** Notes:
**   These Macro definitions have been put in this file (instead of 
**   lc_tbl.h) so this file can be included directly into ASIST build 
**   test scripts. ASIST RDL files can accept C language #defines but 
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE! 
**   ADD THEM TO lc_tbl.h IF NEEDED! 
**
**   $Log: lc_tbldefs.h  $
**   Revision 1.3 2015/03/04 16:09:55EST sstrege 
**   Added copyright information
**   Revision 1.2 2012/08/01 14:19:28EDT lwalling 
**   Change NOT_MEASURED to STALE
**   Revision 1.1 2012/07/31 13:53:39PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.5 2010/03/08 10:36:31EST lwalling 
**   Move saved, not saved state definitions to common header file
**   Revision 1.4 2009/01/15 15:36:19EST dahardis 
**   Unit test fixes
**   Revision 1.3 2008/12/10 10:59:29EST dahardis 
**   Moved data type declarations to new file lc_tbl.h
**   so this file can be included in ASIST RDL files.
**   Revision 1.2 2008/12/03 13:59:42EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:19:41EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_tbldefs_
#define _lc_tbldefs_

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name LC CDS Saved on Exit Identifiers */ 
/** \{ */
#define LC_CDS_SAVED            0xF0F0
#define LC_CDS_NOT_SAVED        0x0F0F
/** \} */

/**
** \name Watchpoint Definition Table (WDT) DataType Enumerated Types */ 
/** \{ */
#define LC_WATCH_NOT_USED       0xFF    /**< \brief Use for empty entries       */
#define LC_DATA_BYTE            1       /**< \brief 8 bit signed byte           */
#define LC_DATA_UBYTE           2       /**< \brief 8 bit unsigned byte         */

#define LC_DATA_WORD_BE         3       /**< \brief 16 bit signed word
                                                    big endian byte order       */

#define LC_DATA_WORD_LE         4       /**< \brief 16 bit signed word
                                                    little endian byte order    */

#define LC_DATA_UWORD_BE        5       /**< \brief 16 bit unsigned word
                                                    big endian byte order       */

#define LC_DATA_UWORD_LE        6       /**< \brief 16 bit unsigned word
                                                    little endian byte order    */

#define LC_DATA_DWORD_BE        7       /**< \brief 32 bit signed double word
                                                    big endian byte order       */

#define LC_DATA_DWORD_LE        8       /**< \brief 32 bit signed double word
                                                    little endian byte order    */

#define LC_DATA_UDWORD_BE       9       /**< \brief 32 bit unsigned double word
                                                    big endian byte order       */

#define LC_DATA_UDWORD_LE      10       /**< \brief 32 bit unsigned double word
                                                    little endian byte order    */

#define LC_DATA_FLOAT_BE       11       /**< \brief 32 bit single precision
                                                    IEEE-754 floating point number,
                                                    big endian byte order */

#define LC_DATA_FLOAT_LE       12       /**< \brief 32 bit single precision
                                                    IEEE-754 floating point number,
                                                    little endian byte order */
/** \} */

/**
** \name Watchpoint Definition Table (WDT) OperatorID Enumerated Types */ 
/** \{ */
#define LC_NO_OPER           0xFF    /**< \brief Use for empty entries         */
#define LC_OPER_LT           1       /**< \brief Less Than (<)                 */
#define LC_OPER_LE           2       /**< \brief Less Than or Equal To (<=)    */
#define LC_OPER_NE           3       /**< \brief Not Equal (!=)                */
#define LC_OPER_EQ           4       /**< \brief Equal (==)                    */
#define LC_OPER_GE           5       /**< \brief Greater Than or Equal To (>=) */
#define LC_OPER_GT           6       /**< \brief Greater Than (>)              */
#define LC_OPER_CUSTOM       7       /**< \brief Use custom function           */
/** \} */

/**
** \name Watchpoint Definition Table (WDT) BitMask Enumerated Types */ 
/** \{ */
#define LC_NO_BITMASK        0xFFFFFFFF   /**< \brief Use for no masking       */
/** \} */

/** 
**  \name Actionpoint Definition Table (ADT) Reverse Polish Operators */
/** \{ */
#define LC_RPN_AND                 0xFFF1
#define LC_RPN_OR                  0xFFF2
#define LC_RPN_XOR                 0xFFF3
#define LC_RPN_NOT                 0xFFF4
#define LC_RPN_EQUAL               0xFFF5
/** \} */

/**
** \name Watchpoint Results Table (WRT) WatchResult Enumerated Types */ 
/** \{ */
#define LC_WATCH_STALE             0xFF
#define LC_WATCH_FALSE             0      /* This needs to be zero for 
                                                  correct RPN evalution  */
#define LC_WATCH_TRUE              1      /* This needs to be one for 
                                                  correct RPN evaluation */
#define LC_WATCH_ERROR             2
/** \} */

/**
** \name Actionpoint Results Table (ART) ActionResult Enumerated Types */ 
/** \{ */
#define LC_ACTION_STALE            0xFF
#define LC_ACTION_PASS             0
#define LC_ACTION_FAIL             1
#define LC_ACTION_ERROR            2
/** \} */

/**
** \name Watchpoint Definition Table (WDT) Validation Error Enumerated Types */ 
/** \{ */
#define LC_WDTVAL_NO_ERR           0    /**< \brief No error                          */
#define LC_WDTVAL_ERR_DATATYPE     1    /**< \brief Invalid DataType                  */
#define LC_WDTVAL_ERR_OPER         2    /**< \brief Invalid OperatorID                */
#define LC_WDTVAL_ERR_MID          3    /**< \brief Invalid MessageID                 */
#define LC_WDTVAL_ERR_FPNAN        4    /**< \brief ComparisonValue is NAN float      */
#define LC_WDTVAL_ERR_FPINF        5    /**< \brief ComparisonValue is infinite float */
/** \} */

/**
** \name Actionpoint Definition Table (ADT) Validation Error Enumerated Types */ 
/** \{ */
#define LC_ADTVAL_NO_ERR           0    /**< \brief No error                          */
#define LC_ADTVAL_ERR_DEFSTATE     1    /**< \brief Invalid DefaultState              */
#define LC_ADTVAL_ERR_RTSID        2    /**< \brief Invalid RTSId                     */
#define LC_ADTVAL_ERR_FAILCNT      3    /**< \brief MaxFailsBeforeRTS is zero         */
#define LC_ADTVAL_ERR_EVTTYPE      4    /**< \brief Invalid EventType                 */
#define LC_ADTVAL_ERR_RPN          5    /**< \brief Invalid Reverse Polish Expression */
/** \} */

#endif /*_lc_tbldefs_*/

/************************/
/*  End of File Comment */
/************************/
