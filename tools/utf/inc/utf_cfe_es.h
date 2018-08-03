/*
**
**  Filename: utf_cfe_es.h
**  $Id: utf_cfe_es.h 1.6 2012/02/28 10:33:00GMT-05:00 wmoleski Exp  $
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
**  Provide Executive Services related utilities needed for 
**  execution of Unit Tests.
**	Includes macro definitions used to specify api in utilities
** which set return values.
**
**  References:
**     User's Guide for the core Flight Executive (cFE)
**     Unit Test Framework (UTF)-- Release 2.3.0
**
**	Notes:
**
**  $Date: 2012/02/28 10:33:00GMT-05:00 $
**  $ $
**  $Log: utf_cfe_es.h  $
**  Revision 1.6 2012/02/28 10:33:00GMT-05:00 wmoleski 
**  Added function hooks and Return Code handling and updated the examples to test these changes.
**  Revision 1.5 2010/10/25 15:09:29EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.4 2010/10/20 12:45:13EDT jmdagost 
**  Added no-semaphore pool creation definition.
**  Revision 1.3 2010/10/04 15:04:16EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2008/07/18 09:22:56EDT wfmoleski 
**  The CFE_ES_GetAppInfo function along with an internal function to set this info and the test that calls 
**  these functions were implemted in the UTF.
**  Revision 1.1 2008/04/17 08:09:29EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**  Revision 1.9 2008/04/07 13:44:42EDT wfmoleski 
**  Added return code setting for ES_CPoolCreate, ES_GetPoolBuf and ES_PutPoolBuf
**  Revision 1.8 2008/04/07 09:39:42EDT wfmoleski 
**  Added the capability for the specified hook functions. Added return code setting code for several
**  OS functions that seemed appropriate.
**  Revision 1.7 2007/06/05 08:24:49EDT wfmoleski 
**  Changes to examples and utf source that reflect the cFE 4.1.0 ES subsystem.
**  Revision 1.6 2007/05/25 11:04:18EDT wfmoleski 
**  Modifications to mimic the cFE 4.1.0 ES fsw. These changes include new APIs as well as Task info storage.
**  Revision 1.5 2006/11/14 15:46:33EST wfmoleski 
**  Modifications for Restart implementation
**  Revision 1.4 2006/11/09 07:46:17EST wfmoleski 
**  UTF implementation of the CDS
**  Revision 1.3 2006/06/22 13:53:33EDT wfmoleski 
**  Most of the changes to these files was to add the copyright comments. There were other changes needed
**  to bring the UTF up to cFE 3.3
**  Revision 1.2 2006/05/26 15:07:14EDT nsschweiss 
**  Changes for UTF 3.2.
**  Revision 1.1 2006/05/05 16:15:21EDT nsschweiss 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/inc/project.pj
**
*/
#ifndef _CFE_UTF_ES_
#define _CFE_UTF_ES_

/* Function Hook Stuff */
#define CFE_ES_POOLCREATE_HOOK          0
#define CFE_ES_POOLCREATENOSEM_HOOK     1
#define CFE_ES_GETPOOLBUF_HOOK          2
#define CFE_ES_PUTPOOLBUF_HOOK          3
#define CFE_ES_CREATECHILDTASK_HOOK     4

/*********** API Identifiers Used to set return codes************/
#define CFE_ES_RESETCFE_PROC            0
#define CFE_ES_REGISTERAPP_PROC         1
#define CFE_ES_GETAPPID_PROC            2
#define CFE_ES_SETAPPNAME_PROC          3
#define CFE_ES_GETAPPNAME_PROC          4
#define CFE_ES_CREATECHILDTASK_PROC     5
#define CFE_ES_DELETECHILDTASK_PROC     6
#define CFE_ES_EXITCHILDTASK_PROC       7
#define CFE_ES_GETTASKINFO_PROC         8
#define CFE_ES_WRITETOSYSLOG_PROC       9
#define CFE_ES_REGISTERDRIVER_PROC     10
#define CFE_ES_UNLOADDRIVER_PROC       11
#define CFE_ES_CALCULATECRC_PROC       12
#define CFE_ES_GETBLOCKINCDS_PROC      13
#define CFE_ES_RESTARTAPP_PROC         14
#define CFE_ES_REGISTERCHILDTASK_PROC  15
#define CFE_ES_REGISTERCDS_PROC        16
#define CFE_ES_COPYTOCDS_PROC          17
#define CFE_ES_RESTOREFROMCDS_PROC     18
#define CFE_ES_GETRESETTYPE_PROC       19
#define CFE_ES_RELOADAPP_PROC          20
#define CFE_ES_DELETEAPP_PROC          21
#define CFE_ES_RUNLOOP_PROC            22
#define CFE_ES_POOLCREATE_PROC         23
#define CFE_ES_POOLCREATENOSEM_PROC    24
#define CFE_ES_GETPOOLBUF_PROC         25
#define CFE_ES_PUTPOOLBUF_PROC         26
#define NUM_OF_CFE_ES_API_PROCS        27

/******************************************************************************
**  Function: UTF_ES_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for an ES function call.
**    NOTE: This is used in the esmempool.c file.
*/
void UTF_ES_set_function_hook(int Index, void *FunPtr);

/******************************************************************************
**  Function: UTF_ES_API_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for an ES function call.
**    NOTE: This is used in the es_api.c file.
*/
void UTF_ES_API_set_function_hook(int Index, void *FunPtr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_ES_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_Use_Default_Api_Return_Code(int32 ApiIndex);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_ES_Set_Api_Return_Code(int argc,char *argv[]);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_ES_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_ES_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_ES_Use_Default_Api_Return_Code(int argc,char *argv[]);
int32 UTF_ES_InitAppRecords(void);
int32 UTF_ES_AddAppRecord(char *AppName, uint32 AppId);
void  UTF_ES_DumpAppRecords(void);
int32 UTF_ES_InitTaskRecords(void);
int32 UTF_ES_AddTaskRecord(char *TaskName, uint32 AppId, uint32 TaskId);
int32 UTF_ES_GetFreeTaskRecord(void);
int32 UTF_ES_FindTaskRecord(const char *Name, uint32 AppId);
int32 UTF_ES_GetTaskIDByAppID(uint32 appID);
int32 UTF_ES_GetTaskIDByName(const char *TaskName);
void  UTF_ES_DumpTaskRecords(void);
int32 UTF_ES_SetTaskID(uint32 taskId);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetAppID() -- Set app id for local task.                     */
/* Accessible by  CFE_ES_GetAppID.                                         */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetAppID(uint32 AppId);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetAppName() -- Use to set AppName in UTF                    */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetAppName(char *Name);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetResetSubtype() -- 				                           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetResetSubtype(uint32 subType);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_ES_SetDefaultAppInfo() -- 				                           */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_ES_SetDefaultAppInfo(CFE_ES_AppInfo_t AppInfo);

#endif  /* _CFE_UTF_ES_ */
