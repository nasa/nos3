/*************************************************************************
** File:
**   $Id: sc_tbldefs.h 1.6 2015/03/02 12:58:47EST sstrege Exp  $
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
**   Specification for the CFS Stored Command (SC) table structures 
**
** Notes:
**
**   $Log: sc_tbldefs.h  $
**   Revision 1.6 2015/03/02 12:58:47EST sstrege 
**   Added copyright information
**   Revision 1.5 2010/09/28 10:45:52EDT lwalling 
**   Update list of included header files, add definitions for table notification ID values
**   Revision 1.4 2009/02/19 10:07:13EST nyanchik 
**   Update SC To work with cFE 5.2 Config parameters
**   Revision 1.3 2009/01/26 14:47:16EST nyanchik 
**   Check in of Unit test
**   Revision 1.2 2009/01/05 08:26:58EST nyanchik 
**   Check in after code review changes
** 
*************************************************************************/
#ifndef _sc_tbldefs_
#define _sc_tbldefs_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "sc_platform_cfg.h"

/** 
**  \brief ID definitions for cFE Table Services manage table request command
*/
#define SC_TBL_ID_ATS_0       (1)
#define SC_TBL_ID_APPEND      (SC_TBL_ID_ATS_0 + SC_NUMBER_OF_ATS)
#define SC_TBL_ID_RTS_0       (SC_TBL_ID_APPEND + 1)
#define SC_TBL_ID_RTS_INFO    (SC_TBL_ID_RTS_0 + SC_NUMBER_OF_RTS)
#define SC_TBL_ID_RTP_CTRL    (SC_TBL_ID_RTS_INFO + 1)
#define SC_TBL_ID_ATS_INFO    (SC_TBL_ID_RTP_CTRL + 1)
#define SC_TBL_ID_APP_INFO    (SC_TBL_ID_ATS_INFO + 1)
#define SC_TBL_ID_ATP_CTRL    (SC_TBL_ID_APP_INFO + 1)
#define SC_TBL_ID_ATS_CMD_0   (SC_TBL_ID_ATP_CTRL + 1)


/************************************************************************
** Type Definitions
*************************************************************************/

/** 
**  \brief Absolute Value time tag for ATC's
*/
typedef     uint32              SC_AbsTimeTag_t;

/** 
**  \brief Relative time tag for RTC's
*/
typedef     uint16              SC_RelTimeTag_t;

/** 
**  \brief ATS Info Table Type - One of these records are kept for each ATS
*/
typedef struct {

    uint16     AtsUseCtr;           /* How many times it has been used */
    uint16     NumberOfCommands;    /* number of commands in the ATS */
    uint32     AtsSize;             /* size of the ATS */

}  SC_AtsInfoTable_t;



/** 
**  \brief ATP Control Block Type
*/
typedef struct {
    
    uint8      AtpState;           /* execution state of the ATP */
    uint8      AtsNumber;          /* current ATS running if any */
    uint32     CmdNumber;          /* current cmd number to run if any */
    uint16     TimeIndexPtr;       /* time index pointer for current cmd */
    uint16     SwitchPendFlag;     /* indicates that a buffer switch is waiting */
    
} SC_AtpControlBlock_t;


/** 
**  \brief RTP Control Block Type
   Note: now there is only really one RTP
   This structure contains overall info for the next relative time
   processor.
*/
typedef struct {
    
    uint16              NumRtsActive;    /* number of RTSs currently active */
    uint16              RtsNumber;       /* next RTS number */
    
} SC_RtpControlBlock_t;

/** 
**  \brief RTS info table entry type -One of these records is kept for each RTS
*/
typedef struct {
    
    uint8              RtsStatus;       /* status of the RTS */
    boolean            DisabledFlag;    /* disabled/enabled flag */
    uint8              CmdCtr;          /* Cmds executed in current rts */
    uint8              CmdErrCtr;       /* errs in current RTS */
    SC_AbsTimeTag_t    NextCommandTime; /* next command time for RTS */
    uint16             NextCommandPtr;  /* where next rts cmd is */
    uint16             UseCtr;          /* how many times RTS is run */
    
} SC_RtsInfoEntry_t;


#endif /*_sc_tbldefs_*/

/************************/
/*  End of File Comment */
/************************/

