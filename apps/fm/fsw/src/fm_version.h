/*
** $Id: fm_version.h 1.1.3.2.3.10 2015/02/28 18:12:45EST sstrege Exp  $
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
** Title: File Manager (FM) Version Information Header File
**
** Purpose: Specification for the CFS FM application version label
**          definitions
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**    CFS Flight Software Development Standards Version 0.11
**
** $Log: fm_version.h  $
** Revision 1.1.3.2.3.10 2015/02/28 18:12:45EST sstrege 
** Changing version number to 2.4.2 for documentation update release
** Revision 1.1.3.2.3.9 2015/02/28 18:10:53EST sstrege 
** Added copyright information
** Revision 1.1.3.2.3.8 2015/01/24 18:23:55EST sstrege 
** Changing version number for the branch tip to 9.9.9
** Revision 1.1.3.2.3.7 2015/01/24 18:19:06EST sstrege 
** Changing version number to 2.4.2 for release
** Revision 1.1.3.2.3.6 2015/01/18 13:17:50EST sstrege 
** Changing version number for the branch tip to 9.9.9
** Revision 1.1.3.2.3.5 2015/01/18 13:11:24EST sstrege 
** Changing version number for the branch to 2.4.2
** Revision 1.1.3.2.3.4 2015/01/06 19:17:41EST sstrege 
** Changing version number for the branch tip to 9.9.9
** Revision 1.1.3.2.3.3 2015/01/06 19:06:11EST sstrege 
** Changing version number for the branch to 2.4.1
** Revision 1.1.3.2.3.2 2014/12/19 12:44:51EST sstrege 
** Changing version number for the branch tip to 9.9.9
** Revision 1.1.3.2.3.1 2014/12/19 12:19:16EST sstrege 
** Changing version number for the branch to 2.4.0
** Revision 1.1.3.2 2011/01/12 14:37:50EST lwalling 
** Move mission revision number to platform config header file
** Revision 1.1.3.1 2009/10/30 14:02:28EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.1 2008/10/03 15:35:16EDT sstrege
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/src/project.pj
*/

#ifndef _fm_version_h_
#define _fm_version_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/*
**  Application Version Information:
**
**  Major.Minor.Revision.Mission_Rev
**
**  Major: Major update.  This would include major changes or new functionality.
**         Most likely will include database schema changes and interface changes.
**         Probably not backwards compatible with older versions
**
**	Minor: Minor change, may introduce new features, but backwards compatibility is mostly
**         retained.  Likely will include schema changes.
**
**  Revision: Minor bug fixes, no significant new features implemented, though a few small
**            improvements may be included.  May include a schema change.
**
**  Mission_Rev:  Used by users of the applications (nominally missions) to denote changes made
**                by the mission.  Releases from the Flight Softare Reuse Library (FSRL) should
**                use Mission_Rev zero (0).
**
*/
#define FM_MAJOR_VERSION     2
#define FM_MINOR_VERSION     4
#define FM_REVISION          2

#endif /* _fm_version_h_ */

/************************/
/*  End of File Comment */
/************************/

