/*
**   File:
**    cfe_es_objtab.c
**
**
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**   Purpose:
**     This file contains the OS_object_table for MAP Build1.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**  $Log: cfe_es_objtab.c  $
**  Revision 1.5 2014/07/25 10:56:45GMT-05:00 lwalling 
**  Changed INCLUDE_CFE_TBL to EXCLUDE_CFE_TBL
**  Revision 1.4 2014/07/23 11:33:21EDT lwalling 
**  Made Table Services conditionsal based on new environment variable INCLUDE_CFE_TBL
**  Revision 1.3 2012/01/13 11:50:03EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.2 2008/07/08 15:40:39EDT apcudmore 
**  Added CFE_FS global data, lock/unlock functions, log messages for decompress API.
**  Revision 1.1 2008/04/17 08:05:06EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.15 2007/09/21 15:40:22EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.14 2007/05/15 11:16:05EDT apcudmore 
**  Added modification log tags.
**
*/

/*
** Include files
*/
#include "private/cfe_private.h"
#include "cfe_es_global.h"
#include "cfe_es_start.h"

/*
**
** ES_object_table
** Note: The name field in this table should be no more than OS_MAX_API_NAME -1 characters.
**
*/
CFE_ES_ObjectTable_t  CFE_ES_ObjectTable[CFE_ES_OBJECT_TABLE_SIZE] =
{
   /*
   ** Spare entries -- The spares should be distributed evenly through this table
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   
   /*
   ** cFE core early initialization calls. These must be done before the tasks start
   */
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_ES_CDSEarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_ES_CDS_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_EVS_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_EVS_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_SB_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_SB_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_TIME_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_TIME_EarlyInit
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY },
#ifndef EXCLUDE_CFE_TBL
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_TBL_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_TBL_EarlyInit
   },

#else
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#endif
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_FUNCTION_CALL,
           .ObjectName = "CFE_FS_EarlyInit",
           .FuncPtrUnion.FunctionPtr = CFE_FS_EarlyInit
   },


   /*
   ** Spare entries
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },

   /*
   ** cFE core tasks
   */
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_EVS",
           .FuncPtrUnion.MainAppPtr = CFE_EVS_TaskMain,
           .ObjectPriority = CFE_EVS_START_TASK_PRIORITY,
           .ObjectSize = CFE_EVS_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_SB",
           .FuncPtrUnion.MainAppPtr = CFE_SB_TaskMain,
           .ObjectPriority = CFE_SB_START_TASK_PRIORITY,
           .ObjectSize = CFE_SB_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_ES",
           .FuncPtrUnion.MainAppPtr = CFE_ES_TaskMain,
           .ObjectPriority = CFE_ES_START_TASK_PRIORITY,
           .ObjectSize = CFE_ES_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_TIME",
           .FuncPtrUnion.MainAppPtr = CFE_TIME_TaskMain,
           .ObjectPriority = CFE_TIME_START_TASK_PRIORITY,
           .ObjectSize = CFE_TIME_START_TASK_STACK_SIZE
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#ifndef EXCLUDE_CFE_TBL
   {
           .ObjectType = CFE_ES_CORE_TASK,
           .ObjectName = "CFE_TBL",
           .FuncPtrUnion.MainAppPtr = CFE_TBL_TaskMain,
           .ObjectPriority = CFE_TBL_START_TASK_PRIORITY,
           .ObjectSize = CFE_TBL_START_TASK_STACK_SIZE
   },
#else
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
#endif

   /*
   ** Spare entries
   */
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   },
   {
           .ObjectType = CFE_ES_NULL_ENTRY
   }

};
