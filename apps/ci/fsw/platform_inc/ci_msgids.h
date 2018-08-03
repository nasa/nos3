 /*************************************************************************
 ** File:
 **  ci_msgids.h
 **
 **  Copyright © 2016 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **  This software was created at NASA's Johnson Space Center.
 **  This software is governed by the NASA Open Source Agreement and may be 
 **  used, distributed and modified only pursuant to the terms of that 
 **  agreement.
 **
 ** Purpose: 
 **   This file contains the message ID's used by Command Ingest 
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** Notes:
 **
 **  \par Modification History:
 **     - 2016-05-11 | Allen Brown | Initial Version
 **
 *************************************************************************/
#ifndef _ci_msgids_
#define _ci_msgids_

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/**
 ** \name CI Command Message Numbers */ 
/** \{ */
#define CI_APP_CMD_MID      (0x1884)
#define CI_SEND_HK_MID      (0x1885)
#define CI_WAKEUP_MID       (0x1886)
#define CI_GATE_CMD_MID     (0x1887)
/** \} */

/**
 ** \name CI Telemetery Message Number */ 
/** \{ */
#define CI_HK_TLM_MID       (0x0884)
#define CI_OUT_DATA_MID     (0x0885)
/** \} */

#endif /*_ci_msgids_*/

/************************/
/*  End of File Comment */
/************************/
