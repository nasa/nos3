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
 * \file cfe_es_perfdata_typedef.h
 *
 *  Created on: Jan 22, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Placeholder for file content description
 */

#ifndef CFE_ES_PERFDATA_TYPEDEF_H_
#define CFE_ES_PERFDATA_TYPEDEF_H_

#include <common_types.h>
#include "cfe_platform_cfg.h"  /* Required for CFE_ES_PERF_MAX_IDS */

#define CFE_ES_PERF_32BIT_WORDS_IN_MASK ((CFE_ES_PERF_MAX_IDS) / 32)


typedef struct {
    uint32         Data;
    uint32         TimerUpper32;   /* TBU - timebase register */
    uint32         TimerLower32;   /* TBL - timebase register */
} CFE_ES_PerfDataEntry_t;

typedef struct {
    uint8                          Version;
    uint8                          Endian;
    uint8                          Spare[2];
    uint32                         TimerTicksPerSecond;
    uint32                         TimerLow32Rollover;
    uint32                         State;
    uint32                         Mode;
    uint32                         TriggerCount;
    uint32                         DataStart;
    uint32                         DataEnd;
    uint32                         DataCount;
    uint32                         InvalidMarkerReported;
    uint32                         FilterTriggerMaskSize;
    uint32                         FilterMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
    uint32                         TriggerMask[CFE_ES_PERF_32BIT_WORDS_IN_MASK];
} CFE_ES_PerfMetaData_t;

typedef struct {
    CFE_ES_PerfMetaData_t          MetaData;
    CFE_ES_PerfDataEntry_t         DataBuffer[CFE_ES_PERF_DATA_BUFFER_SIZE];
} CFE_ES_PerfData_t;



#endif /* CFE_ES_PERFDATA_TYPEDEF_H_ */
