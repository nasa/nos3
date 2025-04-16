/************************************************************************
 * NASA Docket No. GSC-18,921-1, and identified as “CFS Limit Checker
 * Application version 2.2.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   Limit Checker (LC) default watchpoint definition table (WDT)
 *
 * @note
 *   This file provides a default WDT table that simply sets all
 *   watchpoint entries to "not used". It has been formatted to make
 *   it easy for mission developers to edit as needed (see the
 *   examples section below).
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe_tbl_filedef.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_extern_typedefs.h"
#include "lc_tbldefs.h"

/*************************************************************************
** Examples
** (note that comment delimiters have been changed to '**')
**
** Incremental tests on the same data point:
** (see lc_def_adt.c for companion actionpoint definitions)
**
**    ** #112 (Diviner - low s/c bus voltage, level 1) **
**    {
**        .DataType                   = LC_DATA_UWORD_BE,
**        .OperatorID                 = LC_OPER_LT,
**        .MessageID                  = PSE_FAST_HK_TLM_MID,
**        .WatchpointOffset           = 184,
**        .BitMask                    = LC_NO_BITMASK,
**        .CustomFuncArgument         = 0,
**        .ResultAgeWhenStale         = 0,
**        .ComparisonValue.Unsigned16in32.Unsigned16 = 3417,
**    },
**
**    ** #113 (Diviner - low s/c bus voltage, level 2) **
**    {
**        .DataType                   = LC_DATA_UWORD_BE,
**        .OperatorID                 = LC_OPER_LT,
**        .MessageID                  = PSE_FAST_HK_TLM_MID,
**        .WatchpointOffset           = 184,
**        .BitMask                    = LC_NO_BITMASK,
**        .CustomFuncArgument         = 0,
**        .ResultAgeWhenStale         = 0,
**        .ComparisonValue.Unsigned16in32.Unsigned16 = 3319,
**    },
**
** Use of bitmasking and a custom function:
**
**    ** #154 (IRU - 24 bit value with custom transform) **
**    {
**        .DataType                   = LC_DATA_UDWORD_BE,
**        .OperatorID                 = LC_OPER_CUSTOM,
**        .MessageID                  = IRU_FAST_HK_TLM_MID,
**        .WatchpointOffset           = 76,
**        .BitMask                    = 0x00FFFFFF,
**        .CustomFuncArgument         = LC_CUSTOM_XYZ_TRANSFORM,
**        .ResultAgeWhenStale         = 0,
**        .ComparisonValue.Unsigned32 = 1050000,
**    },
**
*************************************************************************/

/*************************************************************************
** Exported Data
*************************************************************************/
/*
** Table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef
    __attribute__((__used__)) = {"LC_DefaultWDT", LC_APP_NAME "." LC_WDT_TABLENAME, "LC watchpoint definition table",
                                 "lc_def_wdt.tbl", (sizeof(LC_WDTEntry_t) * LC_MAX_WATCHPOINTS)};

/*
** Default watchpoint definition table (WDT) data
*/
LC_WDTEntry_t LC_DefaultWDT[LC_MAX_WATCHPOINTS] = {
    /* #0 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #1 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #2 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #3 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #4 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #5 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #6 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #7 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #8 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #9 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #10 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #11 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #12 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #13 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #14 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #15 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #16 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #17 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #18 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #19 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #20 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #21 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #22 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #23 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #24 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #25 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #26 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #27 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #28 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #29 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #30 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #31 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #32 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #33 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #34 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #35 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #36 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #37 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #38 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #39 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #40 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #41 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #42 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #43 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #44 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #45 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #46 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #47 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #48 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #49 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #50 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #51 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #52 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #53 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #54 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #55 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #56 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #57 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #58 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #59 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #60 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #61 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #62 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #63 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #64 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #65 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #66 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #67 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #68 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #69 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #70 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #71 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #72 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #73 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #74 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #75 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #76 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #77 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #78 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #79 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #80 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #81 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #82 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #83 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #84 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #85 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #86 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #87 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #88 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #89 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #90 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #91 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #92 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #93 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #94 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #95 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #96 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #97 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #98 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #99 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #100 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #101 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #102 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #103 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #104 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #105 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #106 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #107 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #108 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #109 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #110 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #111 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #112 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #113 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #114 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #115 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #116 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #117 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #118 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #119 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #120 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #121 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #122 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #123 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #124 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #125 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #126 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #127 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #128 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #129 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #130 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #131 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #132 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #133 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #134 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #135 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #136 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #137 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #138 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #139 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #140 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #141 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #142 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #143 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #144 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #145 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #146 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #147 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #148 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #149 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #150 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #151 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #152 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #153 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #154 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #155 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #156 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #157 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #158 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #159 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #160 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #161 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #162 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #163 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #164 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #165 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #166 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #167 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #168 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #169 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #170 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #171 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #172 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #173 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #174 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    },

    /* #175 (unused) */
    {
        .DataType                   = LC_WATCH_NOT_USED,
        .OperatorID                 = LC_NO_OPER,
        .MessageID                  = CFE_SB_MSGID_RESERVED,
        .WatchpointOffset           = 0,
        .BitMask                    = LC_NO_BITMASK,
        .CustomFuncArgument         = 0,
        .ResultAgeWhenStale         = 0,
        .ComparisonValue.Unsigned32 = 0,
    }}; /* end LC_DefaultWDT */
