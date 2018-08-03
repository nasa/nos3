/*
**  File:  
**    cfe_es_shell.h
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
**    This file contains definitions needed for the internal shell functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**  $Log: cfe_es_shell.h  $
**  Revision 1.5 2012/01/13 11:50:04GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.4 2012/01/10 13:36:02EST lwalling 
**  Add output filename to shell command packet structure
**  Revision 1.3 2010/10/04 17:02:03EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.2 2010/10/04 15:37:03EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:05:08EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.11 2007/09/20 10:52:59EDT apcudmore 
**  Added Query All Tasks command, file definition, events, error codes etc.
**  Revision 1.10 2007/05/15 11:16:05EDT apcudmore 
**  Added modification log tags.
*/

#ifndef _cfe_es_shell_
#define _cfe_es_shell_

/*
** Include Files
*/
#include "cfe.h"

/*
** Macro Definitions
*/
#define CFE_ES_LIST_APPS_CMD        "ES_ListApps"
#define CFE_ES_LIST_RESOURCES_CMD   "ES_ListResources"
#define CFE_ES_LIST_TASKS_CMD       "ES_ListTasks"

/*
** Type Definitions
*/


/*
** Function prototypes
*/
/*
** Operating System Shell function
*/

int32 CFE_ES_ShellOutputCommand(const char * CmdString, const char *Filename);

int32 CFE_ES_ListApplications(int32 fd);
int32 CFE_ES_ListTasks(int32 fd);
int32 CFE_ES_ListResources(int32 fd);
#endif  /* cfe_es_shell_ */
