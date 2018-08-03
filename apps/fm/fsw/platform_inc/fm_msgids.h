/*
** $Id: fm_msgids.h 1.6 2015/02/28 17:50:52EST sstrege Exp  $
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
** Title: File Manager (FM) Message ID Header File
**
** Purpose: Specification for the CFS FM application software bus
**          message identifiers
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: fm_msgids.h  $
** Revision 1.6 2015/02/28 17:50:52EST sstrege 
** Added copyright information
** Revision 1.5 2009/11/13 16:29:01EST lwalling 
** Modify macro names
** Revision 1.4 2009/10/30 14:02:31EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.3 2009/10/16 15:51:06EDT lwalling
** Update message ID name
** Revision 1.2 2009/10/09 17:24:01EDT lwalling
** Create command to generate file system free space packet, replace device table with free space table
** Revision 1.1 2008/10/09 18:16:04EDT sstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/platform_inc/project.pj
** Revision 1.3 2008/09/24 12:01:09EDT sstrege
** Updated Message IDs to those specified in CFS Development Standards
** Revision 1.2 2008/06/20 16:21:42EDT slstrege
** Member moved from fsw/src/fm_msgids.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj to fm_msgids.h in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj.
** Revision 1.1 2008/06/20 15:21:42ACT slstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/cfs_fm.pj
*/

#ifndef _fm_msgids_h_
#define _fm_msgids_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/* FM command packet message ID's */
#define FM_CMD_MID                   0x188C      /** < \brief FM ground commands */
#define FM_SEND_HK_MID               0x188D      /** < \brief FM send housekeeping */

/* FM telemetry packet message ID's */
#define FM_HK_TLM_MID                0x088A      /** < \brief FM housekeeping */
#define FM_FILE_INFO_TLM_MID         0x088B      /** < \brief FM get file info */
#define FM_DIR_LIST_TLM_MID          0x088C      /** < \brief FM get dir list */
#define FM_OPEN_FILES_TLM_MID        0x088D      /** < \brief FM get open files */
#define FM_FREE_SPACE_TLM_MID        0x088E      /** < \brief FM get free space */

#endif /* _fm_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
