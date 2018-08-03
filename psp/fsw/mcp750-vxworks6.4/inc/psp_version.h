/*
**  $Id: psp_version.h 1.2.2.3 2014/10/01 15:41:26GMT-05:00 sstrege Exp  $
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**
**  Purpose:
**  Provide version identifiers for the cFE Platform Support Packages (PSP).
**
*/

#ifndef _psp_version_
#define _psp_version_


/*
** Macro Definitions
*/
#define CFE_PSP_IMPL_MAJOR_VERSION          1
#define CFE_PSP_IMPL_MINOR_VERSION          3
#define CFE_PSP_IMPL_REVISION               0
#define CFE_PSP_IMPL_MISSION_REV            0

#ifndef _ENHANCED_BUILD_

#define CFE_PSP_MAJOR_VERSION          CFE_PSP_IMPL_MAJOR_VERSION
#define CFE_PSP_MINOR_VERSION          CFE_PSP_IMPL_MINOR_VERSION
#define CFE_PSP_REVISION               CFE_PSP_IMPL_REVISION
#define CFE_PSP_MISSION_REV            CFE_PSP_IMPL_MISSION_REV

/* For backwards compatibility */
#define CFE_PSP_SUBMINOR_VERSION       CFE_PSP_IMPL_REVISION 

#endif

#endif  /* _psp_version_ */

