/*
** File: utf_osfileapi.h
**  $Id: utf_osfileapi.h 1.6 2010/10/25 14:09:26GMT-05:00 jmdagost Exp  $
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
**          contained in utf_osfileapi.c
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2010/10/25 14:09:26GMT-05:00 $
** $Revision: 1.6 $
** $Log: utf_osfileapi.h  $
** Revision 1.6 2010/10/25 14:09:26GMT-05:00 jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.5 2010/10/04 15:05:00EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.4 2008/09/04 07:19:36EDT wfmoleski 
** Function hook support was added for the OS_close function. Test code was also added.
** Revision 1.3 2008/06/03 13:52:11EDT wfmoleski 
** Added the OS_opendir and OS_readdir function hooks that were needed by the CFS File Manager
** to complete unit testing.
** Revision 1.2 2008/06/03 13:43:13EDT wfmoleski 
** The return code argument was modified from a uint32 to an int32 to accomodate the OS return
** codes that have negative values.
** Revision 1.1 2008/04/17 08:09:32EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
** Revision 1.1 2008/04/07 09:40:20EDT wfmoleski 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
*/

#ifndef _utf_osfileapi_
#define _utf_osfileapi_

/*
**   Include section
*/

/* Function Hook Stuff */
#define OS_CREAT_HOOK   0
#define OS_WRITE_HOOK   1
#define OS_READ_HOOK    2
#define OS_OPENDIR_HOOK 3
#define OS_READDIR_HOOK 4
#define OS_CLOSE_HOOK   5

/*********** API Identifiers Used to set return codes************/
#define OS_CREAT_PROC           0
#define OS_OPEN_PROC            1
#define OS_CLOSE_PROC           2
#define OS_READ_PROC            3
#define OS_WRITE_PROC           4
#define OS_CHMOD_PROC           5
#define OS_STAT_PROC            6
#define OS_LSEEK_PROC           7
#define OS_REMOVE_PROC          8
#define OS_RENAME_PROC          9
#define OS_CP_PROC              10
#define OS_MV_PROC              11
#define OS_MKDIR_PROC           12
#define OS_CLOSEDIR_PROC        13
#define OS_RMDIR_PROC           14
#define NUM_OF_OSFILEAPI_PROCS  15

#define UTF_CFE_USE_DEFAULT_RETURN_CODE -500

/******************************************************************************
**  Function: UTF_OSAPI_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a OSAPI call.
*/
void UTF_OSFILEAPI_set_function_hook(int Index, void *FunPtr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSFILEAPI_Set_Api_Return_Code() --                                  */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_OSFILEAPI_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_OSFILEAPI_Use_Default_Api_Return_Code() --                          */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_OSFILEAPI_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_OSFILEAPI_Set_Api_Return_Code.       */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_OSFILEAPI_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_OSFILEAPI_Use_Default_Api_Return_Code. */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_OSFILEAPI_Use_Default_Api_Return_Code(int argc,char *argv[]);

#endif
