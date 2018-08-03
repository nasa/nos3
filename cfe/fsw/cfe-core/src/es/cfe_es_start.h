/*
**
**  File: 
**  cfe_es_start.h
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:
**  cFE core startup module defines, data types and prototypes.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**  $Log: cfe_es_start.h  $
**  Revision 1.2 2012/01/13 11:50:05GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.1 2008/04/17 08:05:09EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.12 2007/09/21 15:40:18EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.11 2007/05/15 11:16:05EDT apcudmore 
**  Added modification log tags.
*/

#ifndef _cfe_es_start_
#define _cfe_es_start_

/*
** Include Files
*/

#include "cfe.h"

/*
** Macro Definitions
*/

/* 
** values of object_type in OS object table 
*/
#define CFE_ES_NULL_ENTRY    0x00
#define CFE_ES_CORE_TASK     0x01
#define CFE_ES_DRIVER_TASK   0x02
#define CFE_ES_BIN_SEM       0x03
#define CFE_ES_FUNCTION_CALL 0x04
#define CFE_ES_MUTEX_SEM     0x05

/*
** Type Definitions
*/

typedef int32 (*CFE_ES_EarlyInitFuncPtr_t)(void); /**< \brief Req'd prototype of Early Init Functions */
typedef void  (*CFE_ES_MainAppFuncPtr_t)(void);   /**< \brief Req'd prototype of Application Main Functions */

typedef union
{
    CFE_ES_EarlyInitFuncPtr_t     FunctionPtr;
    CFE_ES_MainAppFuncPtr_t       MainAppPtr;
    void                         *VoidPtr;
} CFE_ES_FuncPtrUnion_t;

typedef struct 
{
    uint32                   ObjectType;                   /* The type of object being created */
    char                     ObjectName[OS_MAX_API_NAME];  /* task or OS object name */
    CFE_ES_FuncPtrUnion_t    FuncPtrUnion;                 /* task or function reference */
    uint32                   ObjectPriority;               /* object priority */
    uint32                   ObjectSize;                   /* size used for stack, queue size, etc. */
    uint32                   ObjectFlags;                  /* extra flags to pass */

} CFE_ES_ObjectTable_t;

/*
** Exported data
*/
extern CFE_ES_ObjectTable_t CFE_ES_ObjectTable[CFE_ES_OBJECT_TABLE_SIZE];  /* es object table */

/*
** Function prototypes
*/
extern void  CFE_ES_CreateObjects(void);
extern void  CFE_ES_SetupResetVariables(uint32 StartType, uint32 StartSubtype, uint32 BootSource);
extern void  CFE_ES_InitializeFileSystems(uint32 StartType);
extern void CFE_ES_SetupPerfVariables(uint32 StartType);


#endif  /* _cfe_es_start_ */
