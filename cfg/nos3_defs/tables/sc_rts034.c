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
#include "generic_adcs_msg.h"
#include "generic_adcs_msgids.h"
#include "generic_adcs_adac.h"
#include "generic_star_tracker_app.h"
#include "generic_star_tracker_msgids.h"
#include "generic_star_tracker_msg.h"
#include "sample_app.h"
#include "lc_app.h"
#include "lc_msgids.h"
#include "mgr_msgids.h"
#include "mgr_app.h"

/* 
** ********************************************************
** RTS 034 - Science Mode: Idle Science, Left CONUS Region
** ********************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Manager Note: Stop Science, Left CONUS Region */
    SC_RtsEntryHeader_t hdr1;
    MGR_U8_cmd_t cmd1;
    /* 2 - Disable Instrument Application */
    SC_RtsEntryHeader_t hdr2;
    SAMPLE_NoArgs_cmd_t cmd2;
    /* 3 - Disable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr3;
    GENERIC_EPS_Switch_cmd_t cmd3;
    /* 4 - Set ADCS to SUNSAFE_MODE */
    SC_RtsEntryHeader_t hdr4;
    Generic_ADCS_Mode_cmd_t cmd4;
    /* 5 - Disable Star Tracker Application */
    SC_RtsEntryHeader_t hdr5;
    GENERIC_STAR_TRACKER_NoArgs_cmd_t cmd5;
    /* 6 - Disable Star Tracker Switch on EPS*/
    SC_RtsEntryHeader_t hdr6;
    GENERIC_EPS_Switch_cmd_t cmd6;
    /* 7 - Reset AP 31 - Do Science, Entering CONUS Region */
    SC_RtsEntryHeader_t hdr7;
    LC_ResetAPStats_t cmd7;
    /* 8 - Enable AP 31 - Do Science, Entering CONUS Region */
    SC_RtsEntryHeader_t hdr8;
    LC_SetAPState_t cmd8;
} SC_RtsStruct034_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct034_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable034_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct034_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable034_t SC_Rts034 = {    
.rts = {
        /* 1 - Manager Note: Idle Science, Left CONUS Region */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd1), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd1.U8 = SS_NO_SCIENCE_LEFT_CONUS,
        /* 2 - Disable Instrument Application */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd2), SAMPLE_DISABLE_CC, 0x00),
        /* 3 - Disable Instrument Switch on EPS*/
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd3), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd3.SwitchNumber = 0,
        .cmd3.State = 0x00,
        /* 4 - Set ADCS to SUNSAFE_MODE */
        .hdr4.TimeTag = 5,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_ADCS_CMD_MID, SC_MEMBER_SIZE(cmd4), GENERIC_ADCS_SET_MODE_CC, 0x00),
        .cmd4.Mode = SUNSAFE_MODE,
        /* 5 - Disable Star Tracker Application */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_STAR_TRACKER_CMD_MID, SC_MEMBER_SIZE(cmd5), GENERIC_STAR_TRACKER_DISABLE_CC, 0x00),
        /* 6 - Disable Star Tracker Switch on EPS*/
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd6), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd6.SwitchNumber = 1,
        .cmd6.State = 0x00,
        /* 7 - Reset AP 31 - Do Science, Entering CONUS Region */
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd7), LC_RESET_AP_STATS_CC, 0x00),
        .cmd7.APNumber = 31,
        .cmd7.Padding = 0,
        /* 8 - Enable AP 31 - Do Science, Entering CONUS Region */
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd8), LC_SET_AP_STATE_CC, 0x00),
        .cmd8.APNumber = 31,
        .cmd8.NewAPState = LC_APSTATE_ACTIVE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts034, SC.RTS_TBL034, SC Example RTS_TBL034, sc_rts034.tbl)
