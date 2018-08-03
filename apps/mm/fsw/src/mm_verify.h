/*************************************************************************
** File:
**   $Id: mm_verify.h 1.7 2015/03/02 14:26:30EST sstrege Exp  $
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
**   Contains CFS Memory Manager macros that run preprocessor checks
**   on mission configurable parameters
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_verify.h  $
**   Revision 1.7 2015/03/02 14:26:30EST sstrege 
**   Added copyright information
**   Revision 1.6 2011/11/30 15:54:48EST jmdagost 
**   Deleted tests of the default algorithm definitions.
**   Revision 1.5 2010/11/29 13:28:52EST jmdagost 
**   Updated and corrected verification tests.
**   Revision 1.4 2008/09/05 13:14:30EDT dahardison 
**   Added inclusion of mm_mission_cfg.h
**   Revision 1.3 2008/05/19 15:23:42EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_verify_
#define _mm_verify_

/*************************************************************************
** Includes
*************************************************************************/
#include "mm_mission_cfg.h"
#include "mm_platform_cfg.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/
/*
**  Maximum number of bytes for an uninterruptable load
*/
#if MM_MAX_UNINTERRUPTABLE_DATA < 1
    #error MM_MAX_UNINTERRUPTABLE_DATA cannot be less than 1
#elif MM_MAX_UNINTERRUPTABLE_DATA > 255
    #error MM_MAX_UNINTERRUPTABLE_DATA should be less than 256 Bytes
#endif 

#if (MM_OPT_CODE_MEM32_MEMTYPE != TRUE) && (MM_OPT_CODE_MEM32_MEMTYPE != FALSE)
    #error MM_OPT_CODE_MEM32_MEMTYPE must be either TRUE or FALSE
#endif

#if (MM_OPT_CODE_MEM16_MEMTYPE != TRUE) && (MM_OPT_CODE_MEM16_MEMTYPE != FALSE)
    #error MM_OPT_CODE_MEM16_MEMTYPE must be either TRUE or FALSE
#endif

#if (MM_OPT_CODE_MEM8_MEMTYPE != TRUE) && (MM_OPT_CODE_MEM8_MEMTYPE != FALSE)
    #error MM_OPT_CODE_MEM8_MEMTYPE must be either TRUE or FALSE
#endif

/*
** Minimum size for max load file defaults
*/
#if MM_MAX_LOAD_FILE_DATA_RAM < 1
    #error MM_MAX_LOAD_FILE_DATA_RAM cannot be less than 1
#endif

#if MM_MAX_LOAD_FILE_DATA_EEPROM < 1
    #error MM_MAX_LOAD_FILE_DATA_EEPROM cannot be less than 1
#endif

/*
** Minimum size for max load data segment
*/
#if MM_MAX_LOAD_DATA_SEG < 4
    #error MM_MAX_LOAD_DATA_SEG cannot be less than 4
#endif

/*
** Minimum size for max fill data segment
*/
#if MM_MAX_FILL_DATA_SEG < 4
    #error MM_MAX_FILL_DATA_SEG cannot be less than 4
#endif

/*
**  Dump, load, and fill data segment sizes
*/
#if (MM_MAX_LOAD_DATA_SEG % 4) != 0
    #error MM_MAX_LOAD_DATA_SEG should be longword aligned
#endif 

#if (MM_MAX_DUMP_DATA_SEG % 4) != 0
    #error MM_MAX_DUMP_DATA_SEG should be longword aligned
#endif 

#if (MM_MAX_FILL_DATA_SEG % 4) != 0
    #error MM_MAX_FILL_DATA_SEG should be longword aligned
#endif 

/*
** Optional MEM32 Configurable Parameters 
*/
#if (MM_OPT_CODE_MEM32_MEMTYPE == TRUE)

#if (MM_MAX_LOAD_FILE_DATA_MEM32 % 4) != 0
    #error MM_MAX_LOAD_FILE_DATA_MEM32 should be longword aligned
#endif 

#if (MM_MAX_DUMP_FILE_DATA_MEM32 % 4) != 0
    #error MM_MAX_DUMP_FILE_DATA_MEM32 should be longword aligned
#endif 

#if (MM_MAX_FILL_DATA_MEM32 % 4) != 0
    #error MM_MAX_FILL_DATA_MEM32 should be longword aligned
#endif 

#endif  /* MM_OPT_CODE_MEM32_MEMTYPE */

/*
** Optional MEM16 Configurable Parameters 
*/
#if (MM_OPT_CODE_MEM16_MEMTYPE == TRUE)

#if (MM_MAX_LOAD_FILE_DATA_MEM16 % 2) != 0
    #error MM_MAX_LOAD_FILE_DATA_MEM16 should be word aligned
#endif 

#if (MM_MAX_DUMP_FILE_DATA_MEM16 % 2) != 0
    #error MM_MAX_DUMP_FILE_DATA_MEM16 should be word aligned
#endif 

#if (MM_MAX_FILL_DATA_MEM16 % 2) != 0
    #error MM_MAX_FILL_DATA_MEM16 should be word aligned
#endif 

#endif  /* MM_OPT_CODE_MEM16_MEMTYPE */

#endif /*_mm_verify_*/

/************************/
/*  End of File Comment */
/************************/
