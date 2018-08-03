/******************************************************************************
** File: cfe_evs_verify.h
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
**
** Purpose:
**   This header file performs compile time checking for EVS configuration
**   parameters.
**
** Author:   K.Audra(Microtel)
**
** Notes:
** $Revision: 1.5 $
** $Log: cfe_evs_verify.h  $
** Revision 1.5 2012/01/13 12:00:56GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/10/27 16:35:23EDT jmdagost 
** Updated error message for default log mode test
** Revision 1.3 2010/10/25 15:01:43EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.2 2010/10/04 15:27:14EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:05:15EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
** Revision 1.5 2007/05/03 09:15:54EDT njyanchik 
** I added checks against the EVS parameters that made sense. This included ones that had 
** counter-limited max's as well as ones that only had a specific number of possible values
** Revision 1.4 2007/04/23 10:28:11EDT njyanchik 
** I removed the #defines in evs_verify.gh that weren't hard max values
** Revision 1.3 2006/06/08 14:14:54EDT njyanchik 
** I added the appropriate legal headers to all of the evs files
**
**
******************************************************************************/

#ifndef _cfe_evs_verify_
#define _cfe_evs_verify_

/* NOTE: Besides the checks in this file, there is one more in cfe_evs_task.h.
 * The check is not here because it is checking a local #define based on a 
 * configuration parameter
 */
#if CFE_EVS_MAX_EVENT_FILTERS > 32767
    #error CFE_EVS_MAX_EVENT_FILTERS cannot be more than 32767!
#endif

#if CFE_EVS_DEFAULT_TYPE_FLAG > 0x0F 
    #error CFE_EVS_DEFAULT_TYPE_FLAG cannot be more than 0x0F!
#endif

#if (CFE_EVS_DEFAULT_LOG_MODE != 0) && (CFE_EVS_DEFAULT_LOG_MODE != 1)
    #error CFE_EVS_DEFAULT_LOG_MODE can only be 0 (Overwrite) or 1 (Discard)!
#endif

#if CFE_EVS_LOG_MAX > 65535
    #error CFE_EVS_LOG_MAX cannot be greater than 65535
#endif

#if( CFE_EVS_DEFAULT_MSG_FORMAT_MODE != CFE_EVS_LONG_FORMAT) && (CFE_EVS_DEFAULT_MSG_FORMAT_MODE != CFE_EVS_SHORT_FORMAT)
    #error CFE_EVS_DEFAULT_MSG_FORMAT can only be CFE_EVS_LONG_FORMAT or CFE_EVS_SHORT_FORMAT !
#endif

#if CFE_EVS_PORT_DEFAULT > 0x0F
    #error CFE_EVS_PORT_DEFAULT cannot be greater than 0x0F!
#endif

#endif /* _cfe_evs_verify_ */
/*****************************************************************************/
