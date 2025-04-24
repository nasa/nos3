#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

/* Command Includes */
#include "cam_app.h"
#include "generic_radio_app.h"
#include "generic_eps_app.h"
#include "generic_eps_msgids.h"
#include "generic_eps_msg.h"
#include "sample_app.h"
#include "lc_app.h"
#include "lc_msgids.h"
#include "mgr_msgids.h"
#include "mgr_app.h"

/* 
** ********************************************************
** RTS 032 - Science Mode: Doing Science, Over HI Region
** ********************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Manager Note: Doing Science, Over CONUS Region */
    SC_RtsEntryHeader_t hdr1;
    MGR_U8_cmd_t cmd1;
    /* 2 - Increment Science Pass Counter */
    SC_RtsEntryHeader_t hdr2;
    MGR_NoArgs_cmd_t cmd2;
    /* 3 - Enable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr3;
    GENERIC_EPS_Switch_cmd_t cmd3;
    /* 4 - Enable Instrument Application */
    SC_RtsEntryHeader_t hdr4;
    SAMPLE_NoArgs_cmd_t cmd4;
    /* 5 - Reset AP 35 - Leaving HI Region */
    SC_RtsEntryHeader_t hdr5;
    LC_ResetAPStats_t cmd5;
    /* 6 - Enable AP 35 - Leaving HI Region */
    SC_RtsEntryHeader_t hdr6;
    LC_SetAPState_t cmd6;
} SC_RtsStruct032_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct032_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable032_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct032_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable032_t SC_Rts032 = {    
.rts = {
        /* 1 - Manager Note: Doing Science, Over HI Region */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd1), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd1.U8 = SS_SCIENCE_OVER_HI,
        /* 2 - Increment Science Pass Counter */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd2), MGR_SCI_PASS_INC_CC, 0x00),
        /* 3 - Enable Instrument Switch on EPS*/
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd3), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd3.SwitchNumber = 0,
        .cmd3.State = 0xAA,
        /* 4 - Enable Instrument Application */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd4), SAMPLE_ENABLE_CC, 0x00),
        /* 5 - Reset AP 35 - Leaving HI Region */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd5), LC_RESET_AP_STATS_CC, 0x00),
        .cmd5.APNumber = 35,
        .cmd5.Padding = 0,
        /* 6 - Enable AP 35 - Leaving HI Region */
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd6), LC_SET_AP_STATE_CC, 0x00),
        .cmd6.APNumber = 35,
        .cmd6.NewAPState = LC_APSTATE_ACTIVE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts032, SC.RTS_TBL032, SC Example RTS_TBL032, sc_rts032.tbl)
