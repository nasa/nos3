/*************************************************************************
** File:
**   $Id: md_verify.h 1.5 2015/03/01 17:17:27EST sstrege Exp  $
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
**   Contains CFS Memory Dwell macros that run preprocessor checks
**   on mission configurable parameters
**
** Notes:
**   This file contains function prototypes and other misc data 
**   declarations for things that should be rolled into the cFE or operating
**   system abstraction layer (OSAL) and should not be part of the 
**   final MD baseline release.
**
*************************************************************************/
#ifndef _md_verify_h_
#define _md_verify_h_

#include "cfe_platform_cfg.h"
#include "md_platform_cfg.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/

/*
** Number of memory dwell tables.
** Max limitation is restricted by size of telemetry variable (16 bits) used to
** report dwell stream enable statuses, the size of the mask used for
** start and stop commands, and the size of variables used in code.
*/
#if MD_NUM_DWELL_TABLES < 1
   #error MD_NUM_DWELL_TABLES must be at least one.
#elif MD_NUM_DWELL_TABLES > 16
   #error MD_NUM_DWELL_TABLES cannot be greater than 16.
#endif

/* If MD_DWELL_TABLE_SIZE is too large, the table load structure */
/*  will be too large for Table Services */
#if MD_DWELL_TABLE_SIZE  < 1
   #error MD_DWELL_TABLE_SIZE must be at least one.
#elif MD_NUM_DWELL_TABLES > 65535
   #error MD_DWELL_TABLE_SIZE cannot be greater than 65535.
#endif 

#if (MD_ENFORCE_DWORD_ALIGN != 0) && (MD_ENFORCE_DWORD_ALIGN != 1)
   #error MD_ENFORCE_DWORD_ALIGN must be 0 or 1.
#endif

#if (MD_SIGNATURE_OPTION != 0) && (MD_SIGNATURE_OPTION != 1)
   #error MD_SIGNATURE_OPTION must be 0 or 1.
#endif

#if (MD_SIGNATURE_FIELD_LENGTH % 4) != 0
    #error MD_SIGNATURE_FIELD_LENGTH should be longword aligned
#elif MD_SIGNATURE_FIELD_LENGTH < 4
   #error MD_SIGNATURE_FIELD_LENGTH cannot be less than 4.
#endif 


/*************************************************************************
** Exported Functions
*************************************************************************/

#endif /* _md_verify_ */

/************************/
/*  End of File Comment */
/************************/
