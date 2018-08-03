/*
**  Filename: utf_cfe_time.h
**  $Id: utf_cfe_time.h 1.1 2008/08/20 13:57:19GMT-05:00 wfmoleski Exp  $
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
**	Includes macro definitions used to specify api for utilities
**  to set return values. Also includes function signature for UTF 
**  utilities to be used in conjunction with testing TIME functions.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     Unit Test Framework (UTF)-- Release 2.3.0
**
**	Notes:
**
**  $Date: 2008/08/20 13:57:19GMT-05:00 $
**  $Revision: 1.1 $
**  $Log: utf_cfe_time.h  $
**  Revision 1.1 2008/08/20 13:57:19GMT-05:00 wfmoleski 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/
#ifndef UTF_CFE_TIME_H_
#define UTF_CFE_TIME_H_

/* Function Hook Stuff */
#define CFE_TIME_GETMETSECONDS_HOOK                   0
#define CFE_TIME_GETMETSUBSECS_HOOK                   1
#define CFE_TIME_REGISTERSYNCHCALLBACK_HOOK           2
#define CFE_TIME_UNREGISTERSYNCHCALLBACK_HOOK         3

/******************* API Identifiers ***********************/
#define CFE_TIME_GETCLOCKINFO_PROC               0
#define CFE_TIME_REGISTERSYNCHCALLBACK_PROC      1
#define CFE_TIME_UNREGISTERSYNCHCALLBACK_PROC    2
#define NUM_OF_CFE_TIME_API_PROCS                3

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TIME_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_TIME_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TIME_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TIME_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TIME_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_TIME_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_TIME_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TIME_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_TIME_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_TIME_Use_Default_Api_Return_Code(int argc,char *argv[]);

/******************************************************************************
**  Function: UTF_TIME_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a TIME function call.
*/
void UTF_TIME_set_function_hook(int Index, void *FunPtr);
                    
#endif /*UTF_CFE_TIME_H_*/
