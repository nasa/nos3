/*************************************************************************
** File:
**   $Id: mm_perfids.h 1.3 2015/03/02 14:26:52EST sstrege Exp  $
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
**   CFS Memory Manager (MM) Application Performance IDs
**
** Notes:
**
**   $Log: mm_perfids.h  $
**   Revision 1.3 2015/03/02 14:26:52EST sstrege 
**   Added copyright information
**   Revision 1.2 2010/05/21 15:35:03EDT jmdagost 
**   CHanged performance IDs to decimal values specified in the CFS Development Standards.
**   Revision 1.1 2008/05/22 15:01:11EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/mission_inc/project.pj
** 
*************************************************************************/
#ifndef _mm_perfids_
#define _mm_perfids_

/*************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MM CFS Application Performance IDs */ 
/** \{ */
#define MM_APPMAIN_PERF_ID              30
#define MM_SEGBREAK_PERF_ID             31
#define MM_EEPROM_POKE_PERF_ID          32
#define MM_EEPROM_FILELOAD_PERF_ID      33
#define MM_EEPROM_FILL_PERF_ID          34
/** \} */

#endif /*_mm_perfids_*/

/************************/
/*  End of File Comment */
/************************/
