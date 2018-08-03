/*
** $Id: fm_perfids.h 1.6 2015/02/28 17:50:49EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Application Performance IDs
**
** Purpose:
**   Specification for the CFS File Manager (FM) Application Performance IDs
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**    CFS Development Standards Version 0.11
**
** $Log: fm_perfids.h  $
** Revision 1.6 2015/02/28 17:50:49EST sstrege 
** Added copyright information
** Revision 1.5 2010/03/23 11:28:07EDT lwalling 
** Change FM perf IDs from 0x27 hex to 39 decimal, change FM child from 0x33 hex to 44 decimal
** Revision 1.4 2009/10/30 14:02:30EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.3 2009/10/23 14:35:38EDT lwalling
** Create FM child task to process slow commands
** Revision 1.2 2008/10/01 13:17:54EDT sstrege
** Newline character added to eof
** Revision 1.1 2008/09/30 15:57:15EDT sstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/mission_inc/project.pj
**
*/

#ifndef _fm_perfids_h_
#define _fm_perfids_h_

/*************************************************************************
**
** Macro Definitions
**
*************************************************************************/
/**
** \name FM CFS Application Performance IDs */
/** \{ */
#define FM_APPMAIN_PERF_ID          39
#define FM_CHILD_TASK_PERF_ID       44
/** \} */

#endif /*_fm_perfids_h_*/

/************************/
/*  End of File Comment */
/************************/

