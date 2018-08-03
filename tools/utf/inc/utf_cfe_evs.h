/*
**
**  Filename: utf_cfe_evs.h
**  $Id: utf_cfe_evs.h 1.3 2010/10/25 14:09:29GMT-05:00 jmdagost Exp  $
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
**	Includes macro definitions used to specify api in utilities
** which set return values.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     	Unit Test Framework (UTF)-- Release 2.3.0
**
**	Notes:
**
**  $Date: 2010/10/25 14:09:29GMT-05:00 $
**  $ $
**  $Log: utf_cfe_evs.h  $
**  Revision 1.3 2010/10/25 14:09:29GMT-05:00 jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.2 2010/10/04 15:04:19EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:09:29EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**  Revision 1.4 2007/05/25 11:06:34EDT wfmoleski 
**  Modified to mimic cFE 4.1.0 fsw changes. These were minor but necessary changes.
**  Revision 1.3 2006/06/22 13:53:32EDT wfmoleski 
**  Most of the changes to these files was to add the copyright comments. There were other changes needed
**  to bring the UTF up to cFE 3.3
**  Revision 1.2 2006/06/01 16:14:12EDT nsschweiss 
**  Added function prototypes to UTF_EVS header file for utilities which set API return codes.
**  Revision 1.1 2006/05/05 16:15:21EDT nsschweiss 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/
#ifndef UTF_CFE_EVS_H_
#define UTF_CFE_EVS_H_
/******************* API Identifiers ***********************/
#define CFE_EVS_REGISTER_PROC           0  
#define CFE_EVS_UNREGISTER_PROC         1  
#define CFE_EVS_SENDEVENT_PROC          2  
#define CFE_EVS_SENDEVENTWITHAPPID_PROC 3  
#define CFE_EVS_SENDTIMEDEVENT_PROC     4  
#define CFE_EVS_RESETFILTER_PROC        5  
#define CFE_EVS_RESETALLFILTERS_PROC    6  
#define NUM_OF_CFE_EVS_API_PROCS        7  

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_EVS_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_EVS_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_EVS_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_EVS_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_EVS_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_EVS_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_EVS_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_EVS_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_EVS_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_EVS_Use_Default_Api_Return_Code(int argc,char *argv[]);





#endif /*UTF_CFE_EVS_H_*/
