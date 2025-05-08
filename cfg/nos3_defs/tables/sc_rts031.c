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
** RTS 031 - Science Mode: Doing Science, Over CONUS Region
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
    /* 3 - Enable Star Tracker Switch on EPS*/
    SC_RtsEntryHeader_t hdr3;
    GENERIC_EPS_Switch_cmd_t cmd3;
    /* 4 - Enable Star Tracker Application */
    SC_RtsEntryHeader_t hdr4;
    GENERIC_STAR_TRACKER_NoArgs_cmd_t cmd4;
    /* 5 - Set ADCS to INERTIAL_MODE */
    SC_RtsEntryHeader_t hdr5;
    Generic_ADCS_Mode_cmd_t cmd5;
    /* 6 - Set ADCS INERTIAL Quaternion to 0, 0, 0, 1 */
    SC_RtsEntryHeader_t hdr6;
    Generic_ADCS_Quat_cmd_t cmd6;
    /* 7 - Enable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr7;
    GENERIC_EPS_Switch_cmd_t cmd7;
    /* 8 - Enable Instrument Application */
    SC_RtsEntryHeader_t hdr8;
    SAMPLE_NoArgs_cmd_t cmd8;
    /* 9 - Reset AP 34 - Leaving CONUS Region */
    SC_RtsEntryHeader_t hdr9;
    LC_ResetAPStats_t cmd9;
    /* 10 - Enable AP 34 - Leaving CONUS Region */
    SC_RtsEntryHeader_t hdr10;
    LC_SetAPState_t cmd10;
} SC_RtsStruct031_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct031_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable031_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct031_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable031_t SC_Rts031 = {    
.rts = {
        /* 1 - Manager Note: Doing Science, Over CONUS Region */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd1), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd1.U8 = SS_SCIENCE_OVER_CONUS,
        /* 2 - Increment Science Pass Counter */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd2), MGR_SCI_PASS_INC_CC, 0x00),
        /* 3 - Enable Star Tracker Switch on EPS*/
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd3), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd3.SwitchNumber = 1,
        .cmd3.State = 0xAA,
        /* 4 - Enable Star Tracker Application */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_STAR_TRACKER_CMD_MID, SC_MEMBER_SIZE(cmd4), GENERIC_STAR_TRACKER_ENABLE_CC, 0x00),
        /* 5 - Set ADCS to INERTIAL_MODE */
        .hdr5.TimeTag = 5,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_ADCS_CMD_MID, SC_MEMBER_SIZE(cmd5), GENERIC_ADCS_SET_MODE_CC, 0x00),
        .cmd5.Mode = INERTIAL_MODE,
        /* 6 - Set ADCS Inertial Quaternion to 0, 0, 0, 1 */
        .hdr6.TimeTag = 5,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_ADCS_CMD_MID, SC_MEMBER_SIZE(cmd6), GENERIC_ADCS_INERTIAL_QUATERNION_CC, 0x00),
        .cmd6.qbn = {0.0f, 0.0f, 0.0f, 1.0f},
        /* 7 - Enable Instrument Switch on EPS*/
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd7), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd7.SwitchNumber = 0,
        .cmd7.State = 0xAA,
        /* 8 - Enable Instrument Application */
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd8), SAMPLE_ENABLE_CC, 0x00),
        /* 9 - Reset AP 34 - Leaving CONUS Region */
        .hdr9.TimeTag = 1,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd9), LC_RESET_AP_STATS_CC, 0x00),
        .cmd9.APNumber = 34,
        .cmd9.Padding = 0,
        /* 10 - Enable AP 34 - Leaving CONUS Region */
        .hdr10.TimeTag = 1,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd10), LC_SET_AP_STATE_CC, 0x00),
        .cmd10.APNumber = 34,
        .cmd10.NewAPState = LC_APSTATE_ACTIVE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts031, SC.RTS_TBL031, SC Example RTS_TBL031, sc_rts031.tbl)
