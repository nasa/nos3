/*
** $Id: cfe.h 1.4 2010/10/25 14:01:09GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose:  cFE header file
**
** Author:   David Kobe, the Hammers Company, Inc.
**
** Notes:    This header file centralizes the includes for all cFE
**           Applications.  It includes all header files necessary
**           to completely define the cFE interface.
**
** $Log: cfe.h  $
** Revision 1.4 2010/10/25 14:01:09GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.3 2010/10/04 15:24:39EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2009/06/10 13:28:30EDT acudmore 
** added cfe_psp.h include
** Revision 1.1 2008/04/17 08:05:18EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.4 2006/06/08 14:28:31EDT David Kobe (dlkobe) 
** Added NASA Open Source Legal Statement
** Revision 1.3 2005/11/04 15:27:51GMT-05:00 rjmcgraw 
** Added cfe_tbl.h
** Revision 1.2 2005/07/05 10:57:21EDT lswalling 
** add cfe_error.h to included header files
** Revision 1.1 2005/06/09 10:57:50EDT rperera 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/inc/project.pj
** Revision 1.1  2005/04/28 19:30:28  swalling
** initial version
**
*/

/*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_
#define _cfe_

#include "common_types.h"     /* Define basic data types */

#include "osapi.h"            /* Define OS API function prototypes */

#include "cfe_mission_cfg.h"  /* Define mission configuration parameters */

/*
 * Note that the platform configuration is _not_ included with cfe.h anymore.
 *
 * Most applications should not depend on any of the #defines in that file.
 * If an application truly does need a value from the platform config, then
 * it can retrieve it from the mission data dictionary using a public API.
 */
#if !defined(_ENHANCED_BUILD_)
#include "cfe_platform_cfg.h" /* Define platform configuration parameters */
#endif

#include "cfe_msgids.h"       /* Define common cFE Message IDs */
#include "cfe_error.h"        /* Define common cFE error codes */

#include "cfe_es.h"           /* Define Executive Service API */
#include "cfe_evs.h"          /* Define Event Service API */
#include "cfe_fs.h"           /* Define File Service API */
#include "cfe_sb.h"           /* Define Software Bus API */
#include "cfe_time.h"         /* Define Time Service API */
#include "cfe_tbl.h"          /* Define Table Service API */

#include "cfe_psp.h"          /* Define Platform Support Package API */

#endif  /* _cfe_ */
