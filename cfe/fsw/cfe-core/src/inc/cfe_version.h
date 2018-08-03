/*
**  File: 
**  cfe_es_version.h
**  $Id: cfe_version.h 1.8.2.5 2014/12/02 19:26:21GMT-05:00 sstrege Exp  $
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:
**  Provide version identifiers for the cFE core.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**
**  Modification History:
**  $Log: cfe_version.h  $
**  Revision 1.8.2.5 2014/12/02 19:26:21GMT-05:00 sstrege 
**  Changing version number for the branch to 6.4.1
**  Revision 1.8.2.4 2014/09/30 17:39:28EDT sstrege 
**  Changing the version number for the branch tip to 9.9.9
**  Revision 1.8.2.3 2014/09/30 15:56:42EDT sstrege 
**  Changing version number for 6.4.0 release
**  Revision 1.8.2.2 2014/09/10 22:36:45EDT sstrege 
**  Changing the version number for the branch tip to 9.9.9
**  Revision 1.8.2.1 2014/09/10 22:27:48EDT sstrege 
**  Changing the version number for the branch to 6.4.0
**  Revision 1.8 2012/01/13 11:50:06EST acudmore 
**  Member renamed from cfe_es_version.h to cfe_version.h in project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj.
**  Revision 1.7 2012/01/13 11:50:06ACT acudmore 
**  Member moved from cfe_es_version.h in project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj to cfe_es_version.h in project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj.
**  Revision 1.6 2012/01/13 11:50:06ACT acudmore 
**  Changed license text to reflect open source
**  Revision 1.5 2010/11/23 15:27:32EST jmdagost 
**  Changed SubMinor Version definition to Revision to match other cFE/CFS components.
**  Revision 1.4 2010/10/25 15:02:13EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.3 2010/10/04 15:37:32EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2008/07/21 16:00:07EDT rjmcgraw 
**  DCR4195:1 Changed version number from 9.9.9 to 0.0.0
**  Revision 1.1 2008/04/17 08:05:10EDT ruperera 
**  Initial revision
**  Member added to cFE project 
*/

#ifndef _cfe_es_version_
#define _cfe_es_version_

/*
 * The "enhanced build" (cmake-driven) instantiates a configuration
 * object that contains extended version information within it.
 *
 * This information is generated automatically by the build using
 * git to determine the most recent tag and commitid.
 *
 * This is only available when compiling with the cmake scripts which
 * is why it is conditionally included.
 */
#ifdef _ENHANCED_BUILD_
#include <target_config.h>
#endif


/*
** Macro Definitions
*/
#define CFE_MAJOR_VERSION         6
#define CFE_MINOR_VERSION         5
#define CFE_REVISION              0


#endif  /* _cfe_es_version_ */

