/*
**  Filename: utf_cfe.h
**  $Id: utf_cfe.h 1.5 2010/10/25 14:09:24GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**  Purpose:
**	Includes other utf header files.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     	Unit Test Framework (UTF)-- Release 2.3.0
**
**	Notes:
**
**  $Date: 2010/10/25 14:09:24GMT-05:00 $
**  $Revision: 1.5 $
**  $Log: utf_cfe.h  $
**  Revision 1.5 2010/10/25 14:09:24GMT-05:00 jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.4 2010/10/04 15:04:53EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.3 2009/10/13 14:55:26EDT wmoleski 
**  Changes required to be compatible with cFE 6.0.0
**  Revision 1.2 2008/08/20 15:08:03EDT wfmoleski 
**  Added the include of utf_cfe_time.h to utf_cfe.h and added the utf_cfe_time_api.o specification to each
**  example's makefile
**  Revision 1.1 2008/04/17 08:09:29EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**  Revision 1.4 2006/06/22 13:53:33EDT wfmoleski 
**  Most of the changes to these files was to add the copyright comments. There were other changes needed
**  to bring the UTF up to cFE 3.3
**  Revision 1.3 2006/06/09 16:35:17EDT nsschweiss 
**  Added UTF_CFE_Init call.
**  Revision 1.2 2006/06/04 17:48:32EDT nsschweiss 
**  Added more include statements. 
**  Revision 1.1 2006/05/05 16:11:23EDT nsschweiss 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/
#ifndef UTF_CFE_H_
#define UTF_CFE_H_
#include "cfe.h"

#include "utf_custom.h"
#include "utf_types.h"

#include "utf_cfe_psp.h"
#include "utf_osapi.h"
#include "utf_osfilesys.h"

#include "utf_cfe_time.h"
#include "utf_cfe_time_utils.h"
#include "utf_cfe_es.h"
#include "utf_cfe_evs.h"
#include "utf_cfe_sb.h"
#include "utf_cfe_tbl.h"

/* Perform all standard initializations for the UTF */
int32 UTF_CFE_Init(void);
#endif /*UTF_CFE_H_*/
