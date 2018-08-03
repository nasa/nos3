/*
**  File:  
**    cfe_es_log.h
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
**  Purpose:  
**    This file contains definitions needed for the cFE ES Logs. The 
**    logs include the Mode Transition log, the System Log, and the
**    Performance log.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
** $Log: cfe_es_log.h  $
** Revision 1.2 2012/01/13 11:50:02GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.1 2008/04/17 08:05:06EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.13 2007/04/17 11:03:01EDT rjmcgraw 
** Changed PerfLogDump function proto so that it can be a child task
** Revision 1.12 2007/04/02 11:03:28EDT rjmcgraw 
** Changed PerfLogDump to take filename as parameter
** Revision 1.11 2007/03/16 09:23:00EST apcudmore 
** Cosmetic changes for ES Code walkthrough.
**
*/

#ifndef _cfe_es_log_
#define _cfe_es_log_

/*
** Include Files
*/
#include "cfe.h"
#include "cfe_es.h"
#include "cfe_es_global.h" 

/*
** Macro Definitions
*/

/*
** ER log types
*/
#define CFE_ES_CORE_LOG_ENTRY        1
#define CFE_ES_APPLICATION_LOG_ENTRY 2

/*
** ES Log Modes
*/
#define CFE_ES_LOG_OVERWRITE       0
#define CFE_ES_LOG_DISCARD         1  


/*
** Function prototypes
*/

/*
** System log management
**   CFE_ES_WriteToSysLog is a public routine in cfe_es.h 
*/
int32 CFE_ES_SysLogOverwrite(uint32 Mode);
void  CFE_ES_SysLogClear(void);
int32 CFE_ES_SysLogDump(const char *Filename);

/*
** Processor Performance log management
*/
int32 CFE_ES_PerfLogClear(void);
void CFE_ES_PerfLogDump(void); 

/*
** Exception and Reset Log API
*/
int32 CFE_ES_WriteToERLog( uint32 EntryType,   uint32  ResetType, uint32 ResetSubtype, 
                              const char  *Description, const uint32 *Context,   uint32 ContextSize );
int32 CFE_ES_ERLogDump(const char *Filename);

#endif  /* _cfe_es_log_ */
