/*
** $Id: ut_version.h 1.1.1.1 2016/07/13 12:00:30EDT sstrege Exp  $
**
**  Copyright © 2012-2013 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be
**  used, distributed and modified only pursuant to the terms of that
**  agreement.
**
** Title: Unit Test (UT) Assert Library Version Information Header File
**
** Purpose: Specification for the UT Assert Library version label
**          definitions
**
** Notes: This build (UT-Assert Version 1.1) was updated to comply with the
**        minor API changes that were made in OSAL version 4.2.0, PSP 1.3.0,
**        and cFE 6.5.0.  Backward compatibility is not supported with older
**        versions of the OSAL and PSP libraries and cFE.
**
** References:
**    Flight Software Branch C Coding Standard Version 1.1
*/

#ifndef _ut_version_h_
#define _ut_version_h_

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/*
**  Library Version Information:
**
**  Major.Minor.Revision
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
**
*/
#define UT_MAJOR_VERSION     1
#define UT_MINOR_VERSION     1
#define UT_REVISION          0

#endif /* _ut_version_h_ */

/************************/
/*  End of File Comment */
/************************/

