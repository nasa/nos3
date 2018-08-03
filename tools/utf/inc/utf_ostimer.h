/*
** File: utf_ostimer.h
**  $Id: utf_ostimer.h 1.3 2010/10/25 14:09:22GMT-05:00 jmdagost Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This module defines the UTF specific function prototypes for the functions
**          contained in utf_ostimer.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:22GMT-05:00 $
** $Revision: 1.3 $
** $Log: utf_ostimer.h  $
** Revision 1.3 2010/10/25 14:09:22GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:05:03EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/09/04 15:17:36EDT wfmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
*/

#ifndef _utf_ostimer_
#define _utf_ostimer_

/*
**   Include section
*/

/*********** API Identifiers Used to set return codes************/
#define OS_TIMER_CREATE_PROC        0
#define OS_TIMER_SET_PROC           1
#define OS_TIMER_DELETE_PROC        2
#define OS_TIMER_GETIDBYNAME_PROC   3
#define OS_TIMER_GETINFO_PROC       4
#define NUM_OF_OSTIMER_PROCS		5

#define UTF_CFE_USE_DEFAULT_RETURN_CODE -500

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Set_Api_Return_Code() --                                    */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSTIMER_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Use_Default_Api_Return_Code() --                            */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSTIMER_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSTIMER_Set_Api_Return_Code() --                             */
/*  Script interface function for UTF_OSTIMER_Set_Api_Return_Code.         */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_OSTIMER_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_OSTIMER_Use_Default_Api_Return_Code() --                     */
/*  Script interface function for UTF_OSTIMER_Use_Default_Api_Return_Code. */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_OSTIMER_Use_Default_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSTIMER_Set_ClockAccuracy() --                                      */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_OSTIMER_Set_ClockAccuracy(uint32 clockAccuracy);

#endif
