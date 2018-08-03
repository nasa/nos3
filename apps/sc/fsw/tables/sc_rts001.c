/************************************************************************
**
** $Id: sc_rts001.c 1.9 2015/03/02 13:01:55EST sstrege Exp  $
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
** CFS Stored Command (SC) sample RTS table #1
**
** Note 1: The following source code demonstrates how to create a sample
**         Stored Command RTS table.  The preferred method for creating
**         flight versions of RTS tables is to use custom ground system
**         tools that output the binary table files, skipping this step
**         altogether.
**         
** Note 2: This source file creates a sample RTS table that contains only
**         the following commands that are scheduled as follows:
**
**         SC NOOP command, execution time relative to start of RTS = 0
**         SC Enable RTS #2 command, execution time relative to prev cmd = 5
**         SC Start RTS #2 command, execution time relative to prev cmd = 5
**
** Note 3: The byte following the command code in each command packet
**         secondary header must contain an 8 bit checksum.  Refer to
**         the SC Users Guide for information on how to calculate this
**         checksum.
**
** Note 4: If the command length (in bytes) is odd, a pad byte must be added 
**         to the RTS command structure (opt data portion) to ensure the next 
**         command starts on a word (uint16) boundary.
**
** $Log: sc_rts001.c  $
** Revision 1.9 2015/03/02 13:01:55EST sstrege 
** Added copyright information
** Revision 1.8 2014/12/18 17:13:39EST sstrege 
** Added note to alert users of required pad byte for odd length commands
** Revision 1.7 2014/12/15 10:32:38EST lwalling 
** Force Big Endian ccsds packet primary headers
** Revision 1.6 2014/12/02 19:00:14EST lwalling 
** Remove table compile warning from default tables
** Revision 1.5 2010/04/22 13:30:42EDT lwalling 
** Member renamed from sc_rts1.c to sc_rts001.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/tables/project.pj.
** Revision 1.4 2010/04/22 12:30:42ACT lwalling 
** Change default RTS table names from sc_rts1 to sc_rts001
** Revision 1.3 2010/03/30 11:52:15EDT lwalling 
** Calculate correct command checksum values
** Revision 1.2 2010/03/26 18:04:19EDT lwalling 
** Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
** Revision 1.1 2010/03/16 15:43:08EDT lwalling 
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
** Execution time for each sample command
*/
#define CMD1_TIME     0
#define CMD2_TIME     5
#define CMD3_TIME     5


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x008F
#define CMD2_XSUM     0x0088
#define CMD3_XSUM     0x008B


/*
** Optional command data values
*/
#define CMD2_ARG      2
#define CMD3_ARG      2


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


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table001", "SC.RTS_TBL001", "SC Sample RTS_TBL001",
    "sc_rts001.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table001[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD1_TIME,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((SC_NOOP_CC << 8) | CMD1_XSUM),
     CMD2_TIME,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((SC_ENABLE_RTS_CC << 8) | CMD2_XSUM), CMD2_ARG,
     CMD3_TIME,  CFE_MAKE_BIG16(SC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((SC_START_RTS_CC << 8) | CMD3_XSUM), CMD3_ARG
};

/************************/
/*  End of File Comment */
/************************/
