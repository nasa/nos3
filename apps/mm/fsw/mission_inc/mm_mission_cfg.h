/*************************************************************************
** File:
**   $Id: mm_mission_cfg.h 1.3 2015/03/31 10:56:45EDT sstrege Exp  $
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
**   Specification for the CFS Memory Manager constants that can
**   be configured from one mission to another
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_mission_cfg.h  $
**   Revision 1.3 2015/03/31 10:56:45EDT sstrege 
**   Added cfe_mission_cfg.h include
**   Revision 1.2 2015/03/02 14:26:44EST sstrege 
**   Added copyright information
**   Revision 1.1 2008/09/05 13:11:52EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/mission_inc/project.pj
** 
*************************************************************************/
#ifndef _mm_mission_cfg_
#define _mm_mission_cfg_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe_mission_cfg.h"

/** \mmcfg CRC type for interrupts disabled loads
**  
**  \par Description:
**       CFE CRC type to use when processing the "memory load with 
**       interrupts disabled" (#MM_LOAD_MEM_WID_CC) command.
**
**  \par Limits:
**       This must be one of the CRC types supported by the 
**       #CFE_ES_CalculateCRC function.
*/
#define MM_LOAD_WID_CRC_TYPE     CFE_ES_DEFAULT_CRC

/** \mmcfg CRC type for load files
**  
**  \par Description:
**       CFE CRC type to use when processing memory loads
**       from a file.
**
**  \par Limits:
**       This must be one of the CRC types supported by the 
**       #CFE_ES_CalculateCRC function.
*/
#define MM_LOAD_FILE_CRC_TYPE    CFE_ES_DEFAULT_CRC

/** \mmcfg CRC type for dump files
**  
**  \par Description:
**       CFE CRC type to use when processing memory dumps
**       to a file.
**
**  \par Limits:
**       This must be one of the CRC types supported by the 
**       #CFE_ES_CalculateCRC function.
*/
#define MM_DUMP_FILE_CRC_TYPE    CFE_ES_DEFAULT_CRC

#endif /*_mm_mission_cfg_*/

/************************/
/*  End of File Comment */
/************************/
