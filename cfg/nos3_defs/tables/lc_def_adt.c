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
 *   Limit Checker (LC) default actionpoint definition table (ADT)
 *
 * @note
 *   This file provides a default ADT table that simply sets all
 *   actionpoint entries to "not used". It has been formatted to make
 *   it easy for mission developers to edit as needed (see the
 *   examples section below).
 *
 *   LC will append a trailer string to the end of the text
 *   specified in the "EventText" field with additional information.
 *   See lc_action.h for the format.
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe_tbl_filedef.h"
#include "lc_platform_cfg.h"
#include "lc_msgdefs.h"
#include "lc_extern_typedefs.h"
#include "lc_tbldefs.h"
#include "lc_events.h"

/*************************************************************************
** Examples
** (note that comment delimiters have been changed to '**')
**
** Actions that trigger off a single watchpoint:
** (see lc_def_wdt.c for companion watchpoint definitions)
**
**    ** #100 **
**    {
**        .DefaultState        = LC_APSTATE_DISABLED,
**        .MaxPassiveEvents    = 2,
**        .MaxPassFailEvents   = 2,
**        .MaxFailPassEvents   = 2,
**        .RTSId               = RTS_ID_DIVINER_SAFE_MODE,
**        .MaxFailsBeforeRTS   = 60,
**        .EventType           = CFE_EVS_EventType_INFORMATION,
**        .EventID             = LC_BASE_AP_EID + 100,
**        .EventText           = { "Diviner: low input volt (1)" },
**        .RPNEquation         = { ** (WP_112) **
**                                 112,
**                                 LC_RPN_EQUAL
**                               }
**    },
**
**    ** #101 **
**    {
**        .DefaultState        = LC_APSTATE_DISABLED,
**        .MaxPassiveEvents    = 2,
**        .MaxPassFailEvents   = 2,
**        .MaxFailPassEvents   = 2,
**        .RTSId               = RTS_ID_DIVINER_SAFE_MODE,
**        .MaxFailsBeforeRTS   = 3,
**        .EventType           = CFE_EVS_EventType_INFORMATION,
**        .EventID             = LC_BASE_AP_EID + 101,
**        .EventText           = { "Diviner: low input volt (2)" },
**        .RPNEquation         = { ** (WP_113) **
**                                 113,
**                                 LC_RPN_EQUAL
**                               }
**    },
**
** Examples of more complex Reverse Polish Notation expressions:
**
**    ** #43 **
**    {
**        .DefaultState        = LC_APSTATE_ENABLED,
**        .MaxPassiveEvents    = 2,
**        .MaxPassFailEvents   = 2,
**        .MaxFailPassEvents   = 2,
**        .RTSId               = RTS_ID_ACS_EXIT_THRUSTER_MODE,
**        .MaxFailsBeforeRTS   = 10,
**        .EventType           = CFE_EVS_EventType_INFORMATION,
**        .EventID             = LC_BASE_AP_EID + 43,
**        .EventText           = { "GNC: delta-V sys attitude" },
**        .RPNEquation         = { ** (WP_26 && !WP_61 && !WP_64 && !WP_45 && WP_46 && WP_47) **
**                                 26, 61,
**                                 LC_RPN_NOT,
**                                 LC_RPN_AND,
**                                 64,
**                                 LC_RPN_NOT,
**                                 LC_RPN_AND,
**                                 45,
**                                 LC_RPN_NOT,
**                                 LC_RPN_AND,
**                                 46,
**                                 LC_RPN_AND,
**                                 47,
**                                 LC_RPN_AND,
**                                 LC_RPN_EQUAL
**                               }
**    },
**
**    ** #47 **
**    {
**        .DefaultState        = LC_APSTATE_ENABLED,
**        .MaxPassiveEvents    = 2,
**        .MaxPassFailEvents   = 2,
**        .MaxFailPassEvents   = 2,
**        .RTSId               = RTS_ID_ACS_POWER_OFF_ALL_RW,
**        .MaxFailsBeforeRTS   = 2,
**        .EventType           = CFE_EVS_EventType_INFORMATION,
**        .EventID             = LC_BASE_AP_EID + 47,
**        .EventText           = { "GNC: wheel on, attached" },
**        .RPNEquation         = { ** (!WP_80 && (WP_48 || WP_49 || WP_50 || WP_51))) **
**                                 80,
**                                 LC_RPN_NOT,
**                                 48, 49, 50, 51,
**                                 LC_RPN_OR,
**                                 LC_RPN_OR,
**                                 LC_RPN_OR,
**                                 LC_RPN_AND,
**                                 LC_RPN_EQUAL
**                               }
**    },
**
**    ** #142 **
**    {
**        .DefaultState        = LC_APSTATE_DISABLED,
**        .MaxPassiveEvents    = 2,
**        .MaxPassFailEvents   = 2,
**        .MaxFailPassEvents   = 2,
**        .RTSId               = RTS_ID_LEND_POWER_OFF,
**        .MaxFailsBeforeRTS   = 60,
**        .EventType           = CFE_EVS_EventType_INFORMATION,
**        .EventID             = LC_BASE_AP_EID + 142,
**        .EventText           = { "LEND: comp over temp #1" },
**        .RPNEquation         = { ** (WP_142 && WP_143) || (WP_144 && WP_145) || (WP_146 && WP_147) **
**                                 142, 143,
**                                 LC_RPN_AND,
**                                 144, 145,
**                                 LC_RPN_AND,
**                                 146, 147,
**                                 LC_RPN_AND,
**                                 LC_RPN_OR,
**                                 LC_RPN_OR,
**                                 LC_RPN_EQUAL
**                               }
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
    __attribute__((__used__)) = {"LC_DefaultADT", LC_APP_NAME "." LC_ADT_TABLENAME, "LC actionpoint definition table",
                                 "lc_def_adt.tbl", (sizeof(LC_ADTEntry_t) * LC_MAX_ACTIONPOINTS)};

/*
** Default actionpoint definition table (ADT) data
*/
LC_ADTEntry_t LC_DefaultADT[LC_MAX_ACTIONPOINTS] = {
    /* #0 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 0,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #1 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 1,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #2 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 2,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #3 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 3,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #4 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 4,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #5 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 5,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #6 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 6,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #7 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 7,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #8 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 8,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #9 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 9,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #10 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 10,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #11 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 11,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #12 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 12,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #13 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 13,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #14 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 14,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #15 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 15,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #16 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 16,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #17 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 17,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #18 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 18,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #19 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 19,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #20 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 20,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #21 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 21,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #22 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 22,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #23 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 23,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #24 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 24,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #25 Science_Reboot to Science */
    {.DefaultState      = LC_APSTATE_PASSIVE,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 25,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 25,
     .EventText         = {"Sci_Reboot transition to Sci Now"},
     .RPNEquation =
         {/* (WP_25) */
          25, LC_RPN_EQUAL}},

    /* #26 Enable Science Mode */
    {.DefaultState      = LC_APSTATE_PASSIVE,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 26,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 26,
     .EventText         = {"Enabling Science Mode"},
     .RPNEquation =
         {/* (WP_26) */
          26, LC_RPN_EQUAL}},

    /* #27 Science Mode: Low Power */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 27,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 27,
     .EventText         = {"SciMode: Low Power"},
     .RPNEquation =
         {/* (WP_27) */
          27, LC_RPN_EQUAL}},

    /* #28 Science Mode: Recharged */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 28,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 28,
     .EventText         = {"SciMode: Recharged"},
     .RPNEquation =
         {/* (WP_28) */
          28, LC_RPN_EQUAL}},

    /* #29 Science Mode: EXIT Science Mode */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 29,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 29,
     .EventText         = {"SciMode: Exit Sci Mode"},
     .RPNEquation =
         {/* (WP_29) */
          29, LC_RPN_EQUAL}},

    /* #30 Science Mode: Entering AK Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 30,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 30,
     .EventText         = {"SciMode: Entering AK Region"},
     .RPNEquation =
         {/* (WP_30) && (WP_31) && (WP_32) && (WP_33) && (WP_34) */
          30, 31, 
          LC_RPN_AND,
          32,
          LC_RPN_AND,
          33,  
          LC_RPN_AND,
          34, 
          LC_RPN_AND, 
          LC_RPN_EQUAL}},

    /* #31 Science Mode: Entering CONUS Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 31,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 31,
     .EventText         = {"SciMode: Entering CONUS Region"},
     .RPNEquation =
         {/* (WP_35) && (WP_36) && (WP_37) && (WP_38) && (WP_39) */
          35, 36, 
          LC_RPN_AND,
          37,
          LC_RPN_AND,
          38,  
          LC_RPN_AND,
          39, 
          LC_RPN_AND, 
          LC_RPN_EQUAL}},

    /* #32 Science Mode: Entering HI Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 32,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 32,
     .EventText         = {"SciMode: Entering HI Region"},
     .RPNEquation =
         {/* (WP_40) && (WP_41) && (WP_42) && (WP_43) && (WP_44) */
          40, 41, 
          LC_RPN_AND,
          42,
          LC_RPN_AND,
          43,  
          LC_RPN_AND,
          44, 
          LC_RPN_AND, 
          LC_RPN_EQUAL}},

    /* #33 Science Mode: Left AK Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 33,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 33,
     .EventText         = {"SciMode: Left AK Region"},
     .RPNEquation =
         {/* !((WP_30) && (WP_31) && (WP_32) && (WP_33) && (WP_34)) */
          30, 31, 
          LC_RPN_AND,
          32,
          LC_RPN_AND,
          33,  
          LC_RPN_AND,
          34, 
          LC_RPN_AND,
          LC_RPN_NOT, 
          LC_RPN_EQUAL}},

    /* #34 Science Mode: Left CONUS Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 34,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 34,
     .EventText         = {"SciMode: Left CONUS Region"},
     .RPNEquation =
         {//* !((WP_35) && (WP_36) && (WP_37) && (WP_38) && (WP_39)) */
          35, 36, 
          LC_RPN_AND,
          37,
          LC_RPN_AND,
          38,  
          LC_RPN_AND,
          39, 
          LC_RPN_AND,
          LC_RPN_NOT, 
          LC_RPN_EQUAL}},

    /* #35 Science Mode: Left HI Region */
    {.DefaultState      = LC_APSTATE_DISABLED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 35,
     .MaxFailsBeforeRTS = 1,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 35,
     .EventText         = {"SciMode: Left HI Region"},
     .RPNEquation =
         {/* !((WP_40) && (WP_41) && (WP_42) && (WP_43) && (WP_44)) */
          40, 41, 
          LC_RPN_AND,
          42,
          LC_RPN_AND,
          43,  
          LC_RPN_AND,
          44, 
          LC_RPN_AND,
          LC_RPN_NOT, 
          LC_RPN_EQUAL}},

    /* #36 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 36,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #37 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 37,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #38 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 38,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #39 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 39,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #40 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 40,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #41 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 41,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #42 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 42,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #43 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 43,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #44 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 44,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #45 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 45,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #46 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 46,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #47 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 47,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #48 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 48,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #49 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 49,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #50 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 50,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #51 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 51,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #52 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 52,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #53 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 53,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #54 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 54,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #55 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 55,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #56 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 56,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #57 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 57,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #58 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 58,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #59 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 59,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #60 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 60,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #61 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 61,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #62 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 62,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #63 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 63,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #64 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 64,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #65 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 65,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #66 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 66,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #67 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 67,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #68 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 68,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #69 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 69,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #70 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 70,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #71 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 71,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #72 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 72,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #73 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 73,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #74 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 74,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #75 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 75,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #76 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 76,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #77 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 77,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #78 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 78,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #79 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 79,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #80 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 80,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #81 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 81,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #82 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 82,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #83 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 83,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #84 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 84,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #85 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 85,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #86 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 86,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #87 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 87,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #88 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 88,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #89 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 89,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #90 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 90,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #91 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 91,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #92 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 92,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #93 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 93,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #94 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 94,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #95 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 95,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #96 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 96,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #97 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 97,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #98 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 98,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #99 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 99,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #100 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 100,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #101 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 101,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #102 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 102,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #103 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 103,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #104 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 104,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #105 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 105,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #106 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 106,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #107 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 107,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #108 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 108,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #109 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 109,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #110 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 110,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #111 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 111,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #112 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 112,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #113 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 113,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #114 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 114,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #115 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 115,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #116 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 116,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #117 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 117,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #118 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 118,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #119 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 119,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #120 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 120,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #121 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 121,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #122 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 122,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #123 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 123,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #124 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 124,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #125 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 125,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #126 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 126,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #127 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 127,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #128 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 128,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #129 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 129,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #130 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 130,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #131 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 131,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #132 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 132,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #133 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 133,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #134 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 134,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #135 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 135,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #136 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 136,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #137 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 137,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #138 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 138,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #139 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 139,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #140 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 140,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #141 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 141,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #142 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 142,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #143 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 143,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #144 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 144,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #145 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 145,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #146 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 146,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #147 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 147,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #148 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 148,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #149 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 149,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #150 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 150,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #151 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 151,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #152 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 152,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #153 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 153,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #154 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 154,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #155 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 155,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #156 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 156,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #157 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 157,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #158 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 158,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #159 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 159,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #160 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 160,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #161 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 161,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #162 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 162,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #163 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 163,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #164 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 164,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #165 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 165,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #166 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 166,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #167 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 167,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #168 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 168,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #169 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 169,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #170 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 170,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #171 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 171,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #172 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 172,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #173 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 173,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #174 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 174,
     .EventText         = {" "},
     .RPNEquation =
         {/* (WP_0) */
          0, LC_RPN_EQUAL}},

    /* #175 (unused) */
    {.DefaultState      = LC_ACTION_NOT_USED,
     .MaxPassiveEvents  = 0,
     .MaxPassFailEvents = 0,
     .MaxFailPassEvents = 0,
     .RTSId             = 0,
     .MaxFailsBeforeRTS = 0,
     .EventType         = CFE_EVS_EventType_INFORMATION,
     .EventID           = LC_BASE_AP_EID + 175,
     .EventText         = {" "},
     .RPNEquation       = 
         {/* (WP_0) */
          0, LC_RPN_EQUAL}}
    }; /* end LC_DefaultADT */
