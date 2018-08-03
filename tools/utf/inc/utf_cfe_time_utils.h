/*
** File: utf_cfe_time_utils.h
**  $Id: utf_cfe_time_utils.h 1.3 2010/10/25 14:09:27GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This module defines the UTF specific function prototypes for the
*   functions contained in utf_cfe_time_utils.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:27GMT-05:00 $
** $Revision: 1.3 $
** $Log: utf_cfe_time_utils.h  $
** Revision 1.3 2010/10/25 14:09:27GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:04:25EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:09:30EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.4 2006/06/21 15:39:02EDT wfmoleski 
** Added copyright prologs to utf_cfe_time files
** Revision 1.3 2006/06/04 17:44:14EDT nsschweiss 
** Removed UTF_set_time_hook.
** Revision 1.2 2006/05/09 15:07:22EDT nsschweiss 
** Added in function prototypes for time function hook, UTF_init_sim_time, etc.
** Revision 1.1 2006/04/10 16:30:21EDT sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/

#ifndef _utf_cfe_time_utils_
#define _utf_cfe_time_utils_

/*
**   Include section
*/
#include "common_types.h"


/*===========================================================================*/
/*=====           Public Interface to TIME Simulation                   =====*/
/*===========================================================================*/
void UTF_init_sim_time(double time);

double UTF_get_sim_time(void);

void UTF_set_sim_time(double time);

/* CFE_TIME_SysTime_t UTF_get_sim_hwtime(void); */

/******************************************************************************
**  Function:  UTF_set_leap_seconds()
**
**  Purpose: Set the value of the leap seconds.
*/
void UTF_set_leap_seconds(int16 leap_seconds);

/******************************************************************************
**  Function:  UTF_set_stcf()
**
**  Purpose: Set the value of the STCF in system time format.
*/
void UTF_set_stcf(CFE_TIME_SysTime_t stcf);

#endif
