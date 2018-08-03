/*
 *  Copyright (c) 2004-2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 *
 *  This is governed by the NASA Open Source Agreement and may be used,
 *  distributed and modified only according to the terms of that agreement.
 */

/**
 * \file cfe_es_resetdata_typedef.h
 *
 *  Created on: Jan 22, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Definition of the CFE_ES_ResetData structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_ES_RESETDATA_TYPEDEF_H_
#define CFE_ES_RESETDATA_TYPEDEF_H_

#include <common_types.h>

#include <cfe_time.h>                  /* Required for CFE_TIME_ResetVars_t definition */
#include "cfe_es_erlog_typedef.h"      /* Required for CFE_ES_ERLog_t definition */
#include "cfe_es_perfdata_typedef.h"   /* Required for CFE_ES_PerfData_t definition */
#include "cfe_evs_log_typedef.h"       /* Required for CFE_EVS_Log_t definition */
#include "cfe_platform_cfg.h"          /* CFE_EVS_LOG_ON, CFE_ES_ER_LOG_ENTRIES, CFE_ES_SYSTEM_LOG_SIZE */

/*
** Reset Variables type
*/
typedef struct
{
   uint32     ResetType;
   uint32     ResetSubtype;
   uint32     BootSource;
   uint32     ES_CausedReset;
   uint32     ProcessorResetCount;
   uint32     MaxProcessorResetCount;
} CFE_ES_ResetVariables_t;


/*
** Executive Services Global Reset Data type
** This is the special memory area for ES that is preserved
** on a processor reset.
*/
typedef struct
{
   /*
   ** Exception and Reset log declaration
   */
   CFE_ES_ERLog_t    ERLog[CFE_ES_ER_LOG_ENTRIES];
   uint32            ERLogIndex;
   uint32            ERLogEntries;
   uint32            LastAppId;

   /*
   ** System Log declaration
   */
   char            SystemLog[CFE_ES_SYSTEM_LOG_SIZE];
   uint32          SystemLogIndex;
   uint32          SystemLogMode;
   uint32          SystemLogEntryNum;

   /*
   ** Performance Data
   */
   CFE_ES_PerfData_t       Perf;

   /*
   ** Reset Variables
   */
   CFE_ES_ResetVariables_t  ResetVars;

   /*
   ** Time variables that are
   ** preserved on a processor reset
   */
   CFE_TIME_ResetVars_t TimeResetVars;

#ifdef CFE_EVS_LOG_ON
   /*
   ** EVS Log and associated variables. This needs to be preserved on a processor reset.
   */
   CFE_EVS_Log_t              EVS_Log;
#endif

} CFE_ES_ResetData_t;



#endif /* CFE_ES_RESETDATA_TYPEDEF_H_ */
