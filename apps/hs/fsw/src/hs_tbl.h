/*************************************************************************
** File:
**   $Id: hs_tbl.h 1.2 2015/11/12 14:25:20EST wmoleski Exp  $
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
**   Specification for the CFS Health and Safety (HS) table structures
**
** Notes:
**   Constants and enumerated types related to these table structures
**   are defined in hs_tbldefs.h. They are kept separate to allow easy
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in hs_tbldefs.h for more
**   info).
**
**   $Log: hs_tbl.h  $
**   Revision 1.2 2015/11/12 14:25:20EST wmoleski 
**   Checking in changes found with 2010 vs 2009 MKS files for the cFS HS Application
**   Revision 1.4 2015/03/03 12:16:15EST sstrege 
**   Added copyright information
**   Revision 1.3 2009/05/21 14:45:21EDT aschoeni 
**   Added comment about NullTerm being different sizes in different tables.
**   Revision 1.2 2009/05/04 17:44:31EDT aschoeni 
**   Updated based on actions from Code Walkthrough
**   Revision 1.1 2009/05/01 13:57:45EDT aschoeni 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hs_tbl_h_
#define _hs_tbl_h_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "hs_tbldefs.h"
#include "hs_platform_cfg.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name Macros for Action Type numbers of Message Actions */
/** \{ */
#define HS_AMT_ACT_MSG(num) (HS_AMT_ACT_LAST_NONMSG + 1 + (num))
#define HS_EMT_ACT_MSG(num) (HS_EMT_ACT_LAST_NONMSG + 1 + (num))
/** \} */

/*************************************************************************
** Type Definitions
*************************************************************************/
/*
** In the following definitions, NullTerm may have a differing size for alignment purposes
** specifically it must be 32 bits in the XCT to align Resource Type, while it can be 16 bits
** in the other two.
*/
/**
**  \brief Application Monitor Table (AMT) Entry
*/
typedef struct {
    char            AppName[OS_MAX_API_NAME];  /**< \brief Name of application to be monitored */
    uint16          NullTerm;                  /**< \brief Buffer of nulls to terminate string */
    uint16          CycleCount;                /**< \brief Number of cycles before application is missing */
    uint16          ActionType;                /**< \brief Action to take if application is missing */
} HS_AMTEntry_t;

/**
**  \brief Event Monitor Table (EMT) Entry
*/
typedef struct {
    char            AppName[OS_MAX_API_NAME];     /**< \brief Name of application generating event */
    uint16          NullTerm;                     /**< \brief Buffer of nulls to terminate string */
    uint16          EventID;                      /**< \brief Event number of monitored event */
    uint16          ActionType;                   /**< \brief Action to take if event is received */
} HS_EMTEntry_t;

/** 
**  \brief Execution Counters Table (XCT) Entry
*/
typedef struct {
    char            ResourceName[OS_MAX_API_NAME];     /**< \brief Name of resource being monitored */
    uint32          NullTerm;                          /**< \brief Buffer of nulls to terminate string */
    uint32          ResourceType;                      /**< \brief Type of execution counter */
} HS_XCTEntry_t;

/** 
**  \brief Message Actions Table (MAT) Entry
*/
typedef struct {
    uint16          EnableState;                   /**< \brief If entry contains message */
    uint16          Cooldown;                      /**< \brief Minimum rate at which message can be sent */
    uint8           Message[HS_MAX_MSG_ACT_SIZE];  /**< \brief Message to be sent */
} HS_MATEntry_t;


#endif /*_hs_tbl_h_*/

/************************/
/*  End of File Comment */
/************************/
