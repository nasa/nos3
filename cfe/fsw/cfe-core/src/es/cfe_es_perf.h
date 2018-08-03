/*
** File:  cfe_es_perf.h
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose: Performance Analyzer data structures
**
** Design Notes:
**
** References:
**
** $Log: cfe_es_perf.h  $
** Revision 1.5 2012/01/13 11:50:03GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/11/04 17:18:30EDT jmdagost 
** Forced enumerations to start at zero.
** Revision 1.3 2009/07/28 17:15:27EDT jmdagost 
** Added Filter Mask and Trigger Mask definitions.
** Revision 1.2 2009/06/10 09:08:59EDT acudmore 
** Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
** Revision 1.1 2008/04/17 08:05:07EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
** Revision 1.13 2008/01/24 11:08:15EST njyanchik 
** I changed the creation of the file to the main task, so if it fails being created and opened, the error
** is reported from the main task and the correct counter can be incremented. The file desciptor is
** passed to the child task in the same structure as the file name.
** Revision 1.12 2007/08/31 10:57:15EDT rjmcgraw 
** Changes to eliminate perl script
** Revision 1.11 2007/04/19 13:52:29EDT rjmcgraw
** Shortened child task name
** Revision 1.10 2007/04/17 10:59:12EDT rjmcgraw
** Added child task variables and #defines for performance file write operation
** Revision 1.9 2007/04/10 13:49:12EDT rjmcgraw
** Minor comment changes
** Revision 1.8 2007/04/06 14:51:19EDT rjmcgraw
** Added define CFE_ES_PERF_32BIT_WORDS_IN_MASK
** Revision 1.7 2007/04/03 17:03:47EDT apcudmore
** Moved performance analyzer command data structures into cfe_es_msg.h
** Revision 1.6 2007/04/02 11:12:57EDT rjmcgraw
** Fixed double definition of CFE_ES_PerfState_t
** Revision 1.5 2007/03/29 15:52:56EST rjmcgraw
** Removed function prototypes
** Revision 1.4 2007/03/13 16:08:57EST apcudmore
** Relocated LA log in ES reset area.
** Revision 1.3 2007/03/06 15:59:27EST apcudmore
** Moved Performance log into the ES Reset area.
**
*/

#ifndef _cfe_es_perf_
#define _cfe_es_perf_


/*
** Include Files
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_es_msg.h"
#include "cfe_es_events.h"
#include "cfe_sb.h"
#include "cfe_evs.h"
#include "cfe_es_perfids.h"
#include "cfe_psp.h"

/*
**  Defines
*/
#define CFE_ES_PERF_CHILD_NAME           "ES_PerfFileWriter"
#define CFE_ES_PERF_CHILD_STACK_PTR      0
#define CFE_ES_PERF_CHILD_FLAGS          0


enum CFE_ES_PerfState_t {
    CFE_ES_PERF_IDLE = 0,
    CFE_ES_PERF_WAITING_FOR_TRIGGER,
    CFE_ES_PERF_TRIGGERED,
    CFE_ES_PERF_MAX_STATES
};

enum CFE_ES_PerfMode_t {
    CFE_ES_PERF_TRIGGER_START = 0,
    CFE_ES_PERF_TRIGGER_CENTER,
    CFE_ES_PERF_TRIGGER_END,
    CFE_ES_PERF_MAX_MODES
};

typedef struct {
    uint32                         DataToWrite;
    uint32                         ChildID;
    char                           DataFileName[OS_MAX_PATH_LEN];
    int32                          DataFileDescriptor;
} CFE_ES_PerfLogDump_t;

extern CFE_ES_PerfLogDump_t    CFE_ES_PerfLogDumpStatus;

#endif /* _cfe_es_perf_ */

