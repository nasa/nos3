/************************************************************************
 * NASA Docket No. GSC-18,924-1, and identified as “Core Flight
 * System (cFS) Stored Command Application version 3.1.1”
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
 *   CFS Stored Command (SC) sample RTS table 6
 *
 * The following source code demonstrates how to create a sample
 * Stored Command RTS table using the software defined command structures.
 * It's also possible to create this table via alternative tools
 * (ground system) and or system agnostic data definitions (XTCE/EDS/JSON).
 *
 * This source file creates a sample RTS table that contains only
 * the following commands that are scheduled as follows:
 *
 * SC NOOP command, execution time relative to start of RTS = 0
 * SC NOOP command, execution time relative to prev cmd = 5
 * SC NOOP command, execution time relative to prev cmd = 5
 */

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

// Command Includes
#include "sample_app.h"

/* Checksum for each sample command */
#ifndef SC_NOOP_CKSUM
#define SC_NOOP_CKSUM (0x8F)
#endif

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    SC_RtsEntryHeader_t hdr1;
    SC_NoArgsCmd_t      cmd1;
    SC_RtsEntryHeader_t hdr2;
    SC_NoArgsCmd_t      cmd2;
    SC_RtsEntryHeader_t hdr3;
    SC_NoArgsCmd_t      cmd3;
} SC_RtsStruct006_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct006_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable006_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct006_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable006_t SC_Rts006 = {   
.rts = {
    /* 1 */
    .hdr1.TimeTag   = 0,
    .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd1), SAMPLE_NOOP_CC, 0x00),

    /* 2 */
    .hdr2.TimeTag   = 5,
    .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd2), SAMPLE_NOOP_CC, 0x00),

    /* 3 */
    .hdr3.TimeTag   = 5,
    .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd3), SAMPLE_NOOP_CC, 0x00),
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts006, SC.RTS_TBL006, SC Example RTS_TBL006, sc_rts006.tbl)
