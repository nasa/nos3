/*
** File: utf_osapi.h
**  $Id: utf_osapi.h 1.6 2012/02/28 10:32:59GMT-05:00 wmoleski Exp  $
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** Purpose: This module defines the UTF specific function prototypes for the functions contained in utf_osapi.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/02/28 10:32:59GMT-05:00 $
** $Revision: 1.6 $
** $Log: utf_osapi.h  $
** Revision 1.6 2012/02/28 10:32:59GMT-05:00 wmoleski 
** Added function hooks and Return Code handling and updated the examples to test these changes.
** Revision 1.5 2010/10/25 15:09:30EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 15:04:59EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/10/13 14:41:24EDT wmoleski 
** Added the UTF_version() function to print out the current UTF version being used.
** Revision 1.2 2008/08/21 15:32:37EDT wfmoleski 
** Added the capability of defining a function hook for the OS_BinSemGive function.
** Revision 1.1 2008/04/17 08:09:31EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.6 2008/02/26 13:30:11EST wfmoleski 
** Added the OS_TaskDelete function hook capability to utf_osapi and the example test 14. I also changed
** the OS_FPUExFPUMask function in the UTF to OS_FPUExSetMask.
** Revision 1.5 2006/06/22 14:01:13EDT wfmoleski 
** All of these changes added the copyright comments. There was nothing else that required any UTF
** changes to bring it up to cFE 3.3
** Revision 1.4 2006/06/04 17:46:33EDT nsschweiss 
** Added function signature for UTF_double_to_hwtime.
** Revision 1.3 2006/04/10 15:46:08EDT sslegel 
** Updated comments
** Revision 1.2 2006/03/24 14:25:35EST sslegel 
** Added queueget and queueput hooks
*/

#ifndef _utf_osapi_
#define _utf_osapi_

/*
**   Include section
*/
#include "osapi-os-core.h"

/* Function Hook Stuff */
#define OS_TASKDELAY_HOOK                0
#define OS_BINSEMTAKE_HOOK               1
#define OS_BINSEMTIMEDWAIT_HOOK          2
#define OS_MUTSEMTAKE_HOOK               3
#define OS_GETLOCALTIME_HOOK             4
#define OS_QUEUEGET_HOOK                 5
#define OS_QUEUEPUT_HOOK                 6
#define OS_TASKDELETE_HOOK               7
#define OS_BINSEMGIVE_HOOK               8
#define OS_BINSEMCREATE_HOOK             9

/* Return Code Stuff */
#define CFE_OSAPI_BINSEMTIMEDWAIT_PROC   0
#define CFE_OSAPI_BINSEMCREATE_PROC      1
#define NUM_OF_CFE_OSAPI_PROCS           2

#define UTF_CFE_USE_DEFAULT_RETURN_CODE -500

/******************************************************************************
**  Function: UTF_OSAPI_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a OSAPI call.
*/
void UTF_OSAPI_set_function_hook(int Index, void *FunPtr);

/******************************************************************************
**  Function: UTF_init_sim_time()
**
**  Purpose:
**    Initialize global variables for simulated TIME hardware abstraction layer.
**    This function must be called before using any TIME functions.
*/
void UTF_init_sim_time(double time);

/******************************************************************************
**  Function: UTF_get_sim_time()
**
**  Purpose:
**    Return the current simulated time for the TIME library functions.
*/
double UTF_get_sim_time(void);

/******************************************************************************
**  Function: UTF_get_sim_hwtime()
**
**  Purpose:
**    Return the current simulated hardware time for the TIME library functions.
*/
OS_time_t UTF_get_sim_hwtime(void);

/******************************************************************************
**  Function: UTF_set_sim_time()
**
**  Purpose:
**    Set the simulated time for the TIME library functions.  The simulated time is 
**    set as a floating point number based on seconds "." fraction. of a second.  So 
**    setting a time of 1.0625 = 1 second and 62500 microseconds, or 1 second and 1 
**    subsecond (16 subseconds per second so 1/16 = .0625).  
*/
void UTF_set_sim_time(double time);

/******************************************************************************
**  Function: UTF_double_to_hwtime()
**
**  Purpose:
**    Convert a floating-point time value to hardware time format.
**    For this simulation, hardware time is identical to work time.
*/
OS_time_t UTF_double_to_hwtime(double time);

/******************************************************************************
**  Function: UTF_version()
**
**  Purpose:
**    This function prints the UTF Version being used.
*/
void UTF_version(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_OSAPI_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_OSAPI_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_OSAPI_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_OSAPI_Use_Default_Api_Return_Code(int32 ApiIndex);


#endif
