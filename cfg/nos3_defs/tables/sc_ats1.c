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
 *   CFS Stored Command (SC) sample ATS table 1
 *
 * The following source code demonstrates how to create a sample
 * Stored Command ATS table using the software defined command structures.
 * It's also possible to create this table via alternative tools
 * (ground system) and or system agnostic data definitions (XTCE/EDS/JSON).
 *
 * This source file creates a sample ATS table that contains only
 * the following commands that are scheduled as follows:
 *
 * SC NOOP command, execution time = SC_TEST_TIME + 30
 * SC Enable RTS #1 command, execution time = SC_TEST_TIME + 35
 * SC Start RTS #1 command, execution time = SC_TEST_TIME + 40
 * SC Reset Counters command, execution time = SC_TEST_TIME + 100
 *
 * Before starting the sample ATS, set time = SC_TEST_TIME.  The
 * user will then have 30 seconds to start the ATS before the
 * first command in the sample ATS is scheduled to execute.
 */

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

/* Spacecraft sample ATS time offsets */
#define SC_TEST_TIME (1000000)
#define SC_CMD1_TIME (SC_TEST_TIME + 30)
#define SC_CMD2_TIME (SC_TEST_TIME + 35)
#define SC_CMD3_TIME (SC_TEST_TIME + 40)
#define SC_CMD4_TIME (SC_TEST_TIME + 100)

/* Checksum for each sample command */
#ifndef SC_NOOP_CKSUM
#define SC_NOOP_CKSUM (0x8F)
#endif
#ifndef SC_ENABLE_RTS1_CKSUM
#define SC_ENABLE_RTS1_CKSUM (0x8D)
#endif
#ifndef SC_START_RTS1_CKSUM
#define SC_START_RTS1_CKSUM (0x8E)
#endif
#ifndef SC_RESET_COUNTERS_CKSUM
#define SC_RESET_COUNTERS_CKSUM (0x8E)
#endif

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    SC_AtsEntryHeader_t hdr1;
    SC_NoArgsCmd_t      cmd1;
    SC_AtsEntryHeader_t hdr2;
    SC_RtsCmd_t         cmd2;
    SC_AtsEntryHeader_t hdr3;
    SC_RtsCmd_t         cmd3;
    SC_AtsEntryHeader_t hdr4;
    SC_NoArgsCmd_t      cmd4;
} SC_AtsStruct1_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_AtsStruct1_t ats;
    uint16          buf[SC_ATS_BUFF_SIZE];
} SC_AtsTable1_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_AtsStruct1_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_AtsTable1_t SC_Ats1 = {
.ats = {
    /* 1 */
    .hdr1.CmdNumber  = 1,
    .hdr1.TimeTag_MS = SC_CMD1_TIME >> 16,
    .hdr1.TimeTag_LS = SC_CMD1_TIME & 0xFFFF,
    .cmd1.CmdHeader  = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd1), SC_NOOP_CC, SC_NOOP_CKSUM),

    /* 2 */
    .hdr2.CmdNumber  = 2,
    .hdr2.TimeTag_MS = SC_CMD2_TIME >> 16,
    .hdr2.TimeTag_LS = SC_CMD2_TIME & 0xFFFF,
    .cmd2.CmdHeader  = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd2), SC_ENABLE_RTS_CC, SC_ENABLE_RTS1_CKSUM),
    .cmd2.RtsId      = 1,

    /* 3 */
    .hdr3.CmdNumber  = 3,
    .hdr3.TimeTag_MS = SC_CMD3_TIME >> 16,
    .hdr3.TimeTag_LS = SC_CMD3_TIME & 0xFFFF,
    .cmd3.CmdHeader  = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd3), SC_START_RTS_CC, SC_START_RTS1_CKSUM),
    .cmd3.RtsId      = 1,

    /* 4 */
    .hdr4.CmdNumber  = 4,
    .hdr4.TimeTag_MS = SC_CMD4_TIME >> 16,
    .hdr4.TimeTag_LS = SC_CMD4_TIME & 0xFFFF,
    .cmd4.CmdHeader  = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd4), SC_RESET_COUNTERS_CC, SC_RESET_COUNTERS_CKSUM),
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Ats1, SC.ATS_TBL1, SC Example ATS_TBL1, sc_ats1.tbl)
