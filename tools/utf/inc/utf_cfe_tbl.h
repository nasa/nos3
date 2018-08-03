/*
**  Filename: utf_cfe_tbl.h
**  $Id: utf_cfe_tbl.h 1.3 2010/10/25 14:09:22GMT-05:00 jmdagost Exp  $
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
**  utilities to be used in conjunction with testing TBL functions.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     Unit Test Framework (UTF)-- Release 2.3.0
**
**	Notes:
**
**  $Date: 2010/10/25 14:09:22GMT-05:00 $
**  $Revision: 1.3 $
**  $Log: utf_cfe_tbl.h  $
**  Revision 1.3 2010/10/25 14:09:22GMT-05:00 jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.2 2010/10/04 15:04:24EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:09:30EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**  Revision 1.7 2008/04/07 09:39:32EDT wfmoleski 
**  Added the capability for the specified hook functions. Added return code setting code for several
**  OS functions that seemed appropriate.
**  Revision 1.6 2007/05/25 11:07:49EDT wfmoleski 
**  Modification to add Critical Table support as well as other fsw changes for cFE 4.1.0.
**  Revision 1.5 2006/06/22 11:04:17EDT wfmoleski 
**  Added the CFE_TBL_GetInfo implementation.
**  Revision 1.4 2006/06/01 16:15:50EDT nsschweiss 
**  • Added function prototypes to UTF_TBL header file for utilities which set API return codes.
**  • Added function prototype to UTF_TBL header file for CFE_TBL_Early_Init().
**  Revision 1.3 2006/05/26 15:09:34EDT nsschweiss 
**  Changes for UTF 3.2.
**  Revision 1.2 2006/05/05 16:10:01EDT nsschweiss 
**  
**  Revision 1.1 2006/05/05 16:01:43EDT nsschweiss 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/
#ifndef UTF_CFE_TBL_H_
#define UTF_CFE_TBL_H_

/* Function Hook Stuff */
#define CFE_TBL_REGISTER_HOOK           0
#define CFE_TBL_LOAD_HOOK               1
#define CFE_TBL_MANAGE_HOOK             2
#define CFE_TBL_GETADDRESS_HOOK         3
#define CFE_TBL_GETADDRESSES_HOOK       4

/******************* API Identifiers ***********************/
#define CFE_TBL_REGISTER_PROC           0
#define CFE_TBL_SHARE_PROC              1
#define CFE_TBL_UNREGISTER_PROC         2
#define CFE_TBL_LOAD_PROC               3
#define CFE_TBL_UPDATE_PROC             4
#define CFE_TBL_GETADDRESS_PROC         5
#define CFE_TBL_RELEASEADDRESS_PROC     6
#define CFE_TBL_GETADDRESSES_PROC       7
#define CFE_TBL_RELEASEADDRESSES_PROC   8
#define CFE_TBL_VALIDATE_PROC           9
#define CFE_TBL_MANAGE_PROC            10
#define CFE_TBL_GETSTATUS_PROC         11
#define CFE_TBL_GETINFO_PROC           12
#define NUM_OF_CFE_TBL_API_PROCS       13

/******************************************************************************
**  Function:  CFE_TBL_EarlyInit()
**
**  Purpose:
**    Initialize the Table Services
**
**  Arguments:
**
**  Notes:
**    This function MUST be called before any TBL API's are called.
**
**  Return:
**    none
*/
int32 CFE_TBL_EarlyInit (void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TBL_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_TBL_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_TBL_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_TBL_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TBL_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_TBL_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_TBL_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_TBL_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_TBL_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_TBL_Use_Default_Api_Return_Code(int argc,char *argv[]);

/******************************************************************************
**  Function: UTF_TBL_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a TBL function call.
*/
void UTF_TBL_set_function_hook(int Index, void *FunPtr);
                    
/******************************************************************************
** Name:    UTF_TBL_LoadTableFromGround
**  
** Purpose: Simulate the loading of the working buffer of a specified table.
**          The data source is a file.
** Assumptions, External Events, and Notes:
**  
**          Note: 
**  
** Date Written: 
**          10/2005
**
** Inputs:
**
**          Table_Name - The full name of the table, as maintained
**                       in the Table Services registry.
**  
**          File_Name - Full path name of the file containing data to be
**                       loaded to working buffer. 
**  
** Outputs:
**          None
** 
** Return Values:
**         CFE_SUCCESS
**          CFE_TBL_ERR_BAD_APP_ID
**          CFE_TBL_ERR_NO_ACCESS
**          CFE_TBL_ERR_INVALID_HANDLE
**          CFE_ES_??? (return codes for CFE_ES_GetAppID)
**          CFE_TBL_ERR_DUMP_ONLY
**          CFE_TBL_ERR_LOAD_IN_PROGRESS
**          CFE_TBL_ERR_NO_BUFFER_AVAIL
**          CFE_TBL_ERR_FILE_NOT_FOUND
**          CFE_TBL_ERR_FILE_TOO_LARGE
**          CFE_TBL_WARN_FILE_TOO_SMALL
******************************************************************************/
int32   UTF_TBL_LoadTableFromGround(char *Full_Table_Name, char *File_Name);

int32 CFE_TBL_LoadWorkingBuffer( CFE_TBL_Handle_t TblHandle,
                    CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr );

int32 UTF_CFE_TBL_DisplayTableRegistryContents(void);
#endif /*UTF_CFE_TBL_H_*/
