/*************************************************************************
** File:
**   $Id: cs_verify.h 1.4 2017/02/16 15:33:22EST mdeschu Exp  $
**
**   Copyright (c) 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   Contains CFS Checksum macros that run preprocessor checks
**   on mission configurable parameters
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS CS Heritage Analysis Document
**   CFS CS CDR Package
**
*************************************************************************/
#ifndef _cs_verify_
#define _cs_verify_

/*************************************************************************
** Includes
*************************************************************************/
#include "cs_platform_cfg.h"
#include "cs_mission_cfg.h"
/*************************************************************************
** Macro Definitions
*************************************************************************/

#if CS_MAX_NUM_EEPROM_TABLE_ENTRIES  > 65535
    #error CS_MAX_NUM_EEPROM_TABLE_ENTRIES cannot be greater than 65535!
#endif 

#if CS_MAX_NUM_MEMORY_TABLE_ENTRIES  > 65535
    #error CS_MAX_NUM_MEMORY_TABLE_ENTRIES cannot be greater than 65535!
#endif

/*
 * JPH 2015-06-29 - Removed checks of:
 *  CS_MAX_NUM_APP_TABLE_ENTRIES > CFE_ES_MAX_APPLICATIONS
 *  CS_MAX_NUM_TABLES_TABLE_ENTRIES > CFE_TBL_MAX_NUM_TABLES
 *
 * These are not valid checks anymore, as the CS app does not have knowledge
 * of either CFE_ES_MAX_APPLICATIONS nor CFE_TBL_MAX_NUM_TABLES.  Also, if
 * these actually violate the rule, this will either show up as an obvious
 * run-time error OR it will still work perfectly fine.
 */

#if (CS_MAX_NUM_EEPROM_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_EEPROM_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_MAX_NUM_MEMORY_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_MEMORY_TABLE_ENTRIES must be at least 1!
#endif 

#if (CS_MAX_NUM_TABLES_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_TABLES_TABLE_ENTRIES must be at least 1!
#endif 

#if (CS_MAX_NUM_APP_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_APP_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE > 0xFFFFFFFF)
    #error CS_DEFAULT_BYTES_PER_CYCLE cannot be greater than 0xFFFFFFFF!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE < 0)
    #error CS_DEFAULT_BYTES_PER_CYCLE cannot be less than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY < 1)
    #error CS_CHILD_TASK_PRIORITY must be greater than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY > 255)
    #error CS_CHILD_TASK_PRIORITY cannot be greater than 255!
#endif


#endif
/*_cs_verify_*/


/************************/
/*  End of File Comment */
/************************/
