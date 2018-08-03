/************************************************************************
**
** $Id: sc_ats1.c 1.8 2015/03/02 13:01:50EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** CFS Stored Command (SC) sample ATS table #1
**
** Note 1: The following source code demonstrates how to create a sample
**         Stored Command ATS table.  The preferred method for creating
**         flight versions of ATS tables is to use custom ground system
**         tools that output the binary table files, skipping this step
**         altogether.
**         
** Note 2: This source file creates a sample ATS table that contains the
**         following commands that are scheduled as follows:
**
**         SC NOOP command, execution time = SC_TEST_TIME + 30
**         SC Enable RTS #1 command, execution time = SC_TEST_TIME + 35
**         SC Start RTS #1 command, execution time = SC_TEST_TIME + 40
**         SC Reset Counters command, execution time = SC_TEST_TIME + 100
**
** Note 3: Before starting the sample ATS, set time = SC_TEST_TIME.  The
**         user will then have 30 seconds to start the ATS before the
**         first command in the sample ATS is scheduled to execute.
**
** Note 4: The byte following the command code in each command packet
**         secondary header must contain an 8 bit checksum.  Refer to
**         the SC Users Guide for information on how to calculate this
**         checksum.
**
** Note 5: If the command length (in bytes) is odd, a pad byte must be added 
**         to the ATS command structure (opt data portion) to ensure the next 
**         command starts on a word (uint16) boundary.
**
** Note 6: There is a crucial safety measure that is required of all ATS tables.  
**         The ATP relies on a sentinel word of zeroes at the end of an ATS table 
**         to signal the end of the ATS table (end of data marker).
**
** $Log: sc_ats1.c  $
** Revision 1.8 2015/03/02 13:01:50EST sstrege 
** Added copyright information
** Revision 1.7 2015/01/11 18:30:04EST sstrege 
** Added note describing need for sentinal word of zeros at the end of an ATS table.  Updated table to include sentinal word of zeros.
** Revision 1.6 2014/12/18 17:13:38EST sstrege 
** Added note to alert users of required pad byte for odd length commands
** Revision 1.5 2014/12/15 10:32:37EST lwalling 
** Force Big Endian ccsds packet primary headers
** Revision 1.4 2014/12/02 19:00:14EST lwalling 
** Remove table compile warning from default tables
** Revision 1.3 2010/03/30 11:52:14EDT lwalling 
** Calculate correct command checksum values
** Revision 1.2 2010/03/26 18:04:19EDT lwalling 
** Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
** Revision 1.1 2010/03/16 15:43:07EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/tables/project.pj
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */


/*
** Arbitrary spacecraft time for start of sample ATS
*/
#define TEST_TIME     1000000


/*
** Execution time for each sample command
*/
#define CMD1_TIME     (TEST_TIME + 30)
#define CMD2_TIME     (TEST_TIME + 35)
#define CMD3_TIME     (TEST_TIME + 40)
#define CMD4_TIME     (TEST_TIME + 100)


/*
** Create execution time as two 16 bit values
*/
#define CMD1_TIME_A   ((uint16) ((uint32) CMD1_TIME >> 16))
#define CMD2_TIME_A   ((uint16) ((uint32) CMD2_TIME >> 16))
#define CMD3_TIME_A   ((uint16) ((uint32) CMD3_TIME >> 16))
#define CMD4_TIME_A   ((uint16) ((uint32) CMD4_TIME >> 16))

#define CMD1_TIME_B   ((uint16) ((uint32) CMD1_TIME))
#define CMD2_TIME_B   ((uint16) ((uint32) CMD2_TIME))
#define CMD3_TIME_B   ((uint16) ((uint32) CMD3_TIME))
#define CMD4_TIME_B   ((uint16) ((uint32) CMD4_TIME))


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x008F
#define CMD2_XSUM     0x008B
#define CMD3_XSUM     0x0088
#define CMD4_XSUM     0x008E


/*
** Optional command data values
*/
#define CMD2_ARG      1
#define CMD3_ARG      1


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000


/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   1
#define CMD2_LENGTH   3
#define CMD3_LENGTH   3
#define CMD4_LENGTH   1


/*
** Sample ATS_TBL1 Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "ATS_Table1", "SC.ATS_TBL1", "SC Sample ATS_TBL1",
    "sc_ats1.tbl", (SC_ATS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample ATS_TBL1 Table Data
*/
uint16 ATS_Table1[SC_ATS_BUFF_SIZE] =
{
  /* cmd num, <---- cmd exe time ---->   <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
           1, CMD1_TIME_A, CMD1_TIME_B,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((SC_NOOP_CC << 8) | CMD1_XSUM),
           2, CMD2_TIME_A, CMD2_TIME_B,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((SC_ENABLE_RTS_CC << 8) | CMD2_XSUM), CMD2_ARG,
           3, CMD3_TIME_A, CMD3_TIME_B,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((SC_START_RTS_CC << 8) | CMD3_XSUM), CMD3_ARG,
           4, CMD4_TIME_A, CMD4_TIME_B,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD4_LENGTH), ((SC_RESET_COUNTERS_CC << 8) | CMD4_XSUM),
           0, 0, 0, 0, 0, 0, 0
};

/************************/
/*  End of File Comment */
/************************/
