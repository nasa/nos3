/*
** $Id: sch_def_msgtbl.c 1.4.1.1 2015/03/01 14:13:09EST sstrege Exp  $
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
** Purpose: Scheduler (SCH) default message definition table data
**
** Author: 
**
** Notes:
**
** $Log: sch_def_msgtbl.c  $
** Revision 1.4.1.1 2015/03/01 14:13:09EST sstrege 
** Added copyright information
** Revision 1.4 2012/07/20 17:09:19EDT aschoeni 
** Fixed table compiler warning
** Revision 1.3 2011/07/21 14:51:01EDT aschoeni 
** removed default entries from table and replaced with unused entries
** Revision 1.2 2011/06/30 20:30:49EDT aschoeni 
** updated table header
** Revision 1.1 2009/03/27 00:32:24EDT dkobe 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sch/fsw/tables/project.pj
*/

/*************************************************************************
**
** Include section
**
**************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "sch_platform_cfg.h"
#include "sch_tbldefs.h"

#include "cfe_msgids.h"
#include "ci_lab_msgids.h"
#include "to_lab_msgids.h"
#include "sch_msgids.h"
#include "hk_msgids.h"
#include "sc_msgids.h"
/* #include "cs_msgids.h"  */
/* #include "ds_msgids.h"  */
/* #include "fm_msgids.h"  */
/* #include "hs_msgids.h"  */
/* #include "lc_msgids.h"  */
/* #include "md_msgids.h"  */
/* #include "mm_msgids.h"  */

/* STF-1 apps */
/* 
#include "nos3_msgids.h" 
#include "csee_msgids.h"
#include "imu_msgids.h"

#include "sen_msgids.h"
#include "nav_msgids.h"
#include "eps_msgids.h"
 
*/


/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/* big endian conversions */
#ifdef BYTE_ORDER_LE
#define HTONS(x) (((x >> 8) & 0x00ff) | ((x << 8) & 0xff00))
#else
#define HTONS(x) x
#endif

/*************************************************************************
**
** Type definitions
**
**************************************************************************/

/*
** (none)
*/

/*************************************************************************
**
** Exported data
**
**************************************************************************/

/*
** Message Table entry map...
**
**  Entry 0 -- reserved (DO NOT USE)
**  
**  Several Entries in this default table provide example messages for a default
**  system. These messages can be uncommented, and the SCH_UNUSED_MID entry just
**  below them can be deleted to enable them.
*/

/*
** Default command definition table data
*/
SCH_MessageEntry_t SCH_DefaultMessageTable[SCH_MAX_MESSAGES] =
{
  /*
  **  DO NOT USE -- entry #0 reserved for "unused" command ID - DO NOT USE
  */
    /* command ID #0 */
  { { HTONS(SCH_UNUSED_MID) } },

  /*
  **  cFE housekeeping request messages
  */
    /* command ID #1 - Executive Services HK Request   */
  { { HTONS(CFE_ES_SEND_HK_MID),   HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #2 - Event Services HK Request     */
  { { HTONS(CFE_EVS_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #3 - Software Bus HK Request       */
  { { HTONS(CFE_SB_SEND_HK_MID),   HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #4 - Time Services HK Request      */
  { { HTONS(CFE_TIME_SEND_HK_MID), HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #5 - Table Services HK Request     */
  { { HTONS(CFE_TBL_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },

  /*
  **  CFS housekeeping request messages
  */
    /* command ID #6 - Checksum HK Request           */
/*{ { HTONS(CS_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #7 - Data Store HK Request         */
/*{ { HTONS(DS_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #8 - File Manager HK Request       */
/*{ { HTONS(FM_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #9 - Housekeeping HK Request       */
/*   { { HTONS(HK_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },

    /* command ID #10 - Health & Safety HK Request   */
/*{ { HTONS(HS_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #11 - Limit Checker HK Request     */
/*{ { HTONS(LC_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #12 - Memory Dwell HK Request      */
/*{ { HTONS(MD_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #13 - Memory Manager HK Request    */
/*{ { HTONS(MM_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #14 - Stored Command HK Request    */
/*{ { HTONS(SC_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #15 - Scheduler HK Request         */
  { { HTONS(SCH_SEND_HK_MID), HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },

  /*
  **  CFS routine messages
  */
    /* command ID #16 - HK Send Combined Housekeeping Msg #1 */
/*   { { HTONS(HK_SEND_COMBINED_PKT_MID), HTONS(0xC000), HTONS(0x0003), HTONS(0x0000), HTONS(HK_COMBINED_PKT1_MID) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #17 - HK Send Combined Housekeeping Msg #2 */
/*   { { HTONS(HK_SEND_COMBINED_PKT_MID), HTONS(0xC000), HTONS(0x0003), HTONS(0x0000), HTONS(HK_COMBINED_PKT2_MID) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #18 - HK Send Combined Housekeeping Msg #3 */
/*{ { HTONS(HK_SEND_COMBINED_PKT_MID), HTONS(0xC000), HTONS(0x0003), HTONS(0x0000), HTONS(HK_COMBINED_PKT3_MID) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #19 - HK Send Combined Housekeeping Msg #4 */
/*{ { HTONS(HK_SEND_COMBINED_PKT_MID), HTONS(0xC000), HTONS(0x0003), HTONS(0x0000), HTONS(HK_COMBINED_PKT4_MID) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #20 - CS Background Cycle               */
/*{ { HTONS(CS_BACKGROUND_CYCLE_MID), HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #21 - SC 1 Hz Wakeup                    */
/*{ { HTONS(SC_1HZ_WAKEUP_MID),       HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #22 - LC Sample Action Points           */
/*{ { HTONS(LC_SAMPLE_AP_MID),        HTONS(0xC000), HTONS(0x0005), HTONS(0x0000), HTONS(LC_ALL_ACTIONPOINTS), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #23 - DS 1 HZ Wakeup                    */
/*{ { HTONS(DS_1HZ_WAKEUP_MID),       HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #24 - MD Wakeup                         */
/*{ { HTONS(MD_WAKEUP_MID),           HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #25 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #26 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #27 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #28 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #29 */
  { { HTONS(SCH_UNUSED_MID) } },

  /*
  **  Mission Defined Messages
  **  Mission housekeeping request messages
  */
    /* command ID #30 - Command Ingest HK Request Example */
  { { HTONS(CI_LAB_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #31 - Telemetry Output HK Request Example */
  { { HTONS(TO_LAB_SEND_HK_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } },
    /* command ID #32 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #33 */
/*   { { HTONS(SEN_SEND_HK_MID),     HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #34 */
/*   { { HTONS(NAV_CMD_REQ_NAV_SCH_MID),  HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #35 */
/*   { { HTONS(EPS_SEND_HK_MID),     HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #36 */
/*   { { HTONS(SCH_UNUSED_MID) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #37 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #38 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #39 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #40 */
    /* TODO remove only used for testing */
/*   { { HTONS(EPS_CMD_MID),        HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #41 */
    /* TODO remove only used for testing */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #42 */
/*   { { HTONS(NAV_GROUND_CMD_MID), HTONS(0xC000), HTONS(0x0001), HTONS(0x0000) } }, */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #43 */
  { { HTONS(SCH_UNUSED_MID) } },	
    /* command ID #44 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #45 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #46 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #47 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #48 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #49 */
  { { HTONS(SCH_UNUSED_MID) } },

    /* command ID #50 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #51 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #52 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #53 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #54 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #55 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #56 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #57 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #58 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #59 */
  { { HTONS(SCH_UNUSED_MID) } },

    /* command ID #60 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #61 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #62 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #63 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #64 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #65 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #66 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #67 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #68 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #69 */
  { { HTONS(SCH_UNUSED_MID) } },

    /* command ID #70 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #71 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #72 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #73 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #74 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #75 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #76 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #77 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #78 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #79 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #80 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #81 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #82 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #83 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #84 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #85 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #86 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #87 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #88 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #89 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #90 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #91 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #92 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #93 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #94 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #95 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #96 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #97 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #98 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #99 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #100 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #101 */
  { { HTONS(SCH_UNUSED_MID),} },
    /* command ID #102 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #103 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #104 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #105 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #106 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #107 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #108 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #109 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #110 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #111 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #112 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #113 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #114 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #115 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #116 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #117 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #118 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #119 */
  { { HTONS(SCH_UNUSED_MID) } },
  
    /* command ID #120 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #121 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #122 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #123 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #124 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #125 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #126 */
  { { HTONS(SCH_UNUSED_MID) } },
    /* command ID #127 */
  { { HTONS(SCH_UNUSED_MID) } }

};

/*
** Table file header
*/
CFE_TBL_FILEDEF(SCH_DefaultMessageTable, SCH.MSG_DEFS, SCH message definitions table, sch_def_msgtbl.tbl)

/*************************************************************************
**
** File data
**
**************************************************************************/

/*
** (none)
*/

/*************************************************************************
**
** Local function prototypes
**
**************************************************************************/

/*
** (none)
*/

/************************/
/*  End of File Comment */
/************************/

