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
#include "sample_app.h"
#include "lc_msgids.h"
#include "lc_app.h"
#include "mgr_msgids.h"
#include "mgr_app.h"

/* 
** *****************************
** RTS 026 - Enable Science Mode
** *****************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - UNUSED */
    // SC_RtsEntryHeader_t hdr1;
    // SC_RtsCmd_t cmd1;
    /* 2 - Enable Science RTSs: Safe Mode (29), AK (30), CONUS (31), HI (32) */
    SC_RtsEntryHeader_t hdr2;
    SC_RtsGrpCmd_t cmd2;
    /* 3 - Reset Science AP 27 - Low Power */
    SC_RtsEntryHeader_t hdr3;
    LC_ResetAPStats_t cmd3;
    /* 4 - Reset Science AP 29 - Go to Safe Mode */
    SC_RtsEntryHeader_t hdr4;
    LC_ResetAPStats_t cmd4;
    /* 5 - Reset Science AP 30 - Do Science AK */
    SC_RtsEntryHeader_t hdr5;
    LC_ResetAPStats_t cmd5;
    /* 6 - Reset Science AP 31 - Do Science CONUS */
    SC_RtsEntryHeader_t hdr6;
    LC_ResetAPStats_t cmd6;
    /* 7 - Reset Science AP 32 - Do Science HI */
    SC_RtsEntryHeader_t hdr7;
    LC_ResetAPStats_t cmd7;
    /* 8 - Enable Science AP 27 - Low Power */
    SC_RtsEntryHeader_t hdr8;
    LC_SetAPState_t cmd8;
    /* 9 - Enable Science AP 29 - Go to Safe Mode */
    SC_RtsEntryHeader_t hdr9;
    LC_SetAPState_t cmd9;
    /* 10 - Enable Science AP 30 - Do Science AK */
    SC_RtsEntryHeader_t hdr10;
    LC_SetAPState_t cmd10;
    /* 11 - Enable Science AP 31 - Do Science CONUS */
    SC_RtsEntryHeader_t hdr11;
    LC_SetAPState_t cmd11;
    /* 12 - Enable Science AP 32 - Do Science HI */
    SC_RtsEntryHeader_t hdr12;
    LC_SetAPState_t cmd12;
    /* 13 - Update Science Status in MGR */
    SC_RtsEntryHeader_t hdr13;
    MGR_U8_cmd_t cmd13;
} SC_RtsStruct026_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct026_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable026_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct026_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable026_t SC_Rts026 = {    
.rts = {
        /* 1 - UNUSED */
        // .hdr1.TimeTag = 0,
        // .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd1), SC_ENABLE_RTS_CC, 0x00),
        // .cmd1.RtsId = 27,
        // .cmd1.Padding = 0,
        /* 2 - Enable Science RTSs: Low Power (27), Recharged (28), Safe Mode (29), AK (30), CONUS (31), HI (32) */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd2), SC_ENABLE_RTSGRP_CC, 0x00),
        .cmd2.FirstRtsId = 27,
        .cmd2.LastRtsId = 32,
        /* 3-7 - Reset Science APs */
        // AP27 - Low Power
        // AP29 - Go to Safe Mode
        // AP30 - Do Science AK
        // AP31 - Do Science CONUS
        // AP32 - Do Science HI
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd3), LC_RESET_AP_STATS_CC, 0x00),
        .cmd3.APNumber = 27,
        .cmd3.Padding = 0,
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd4), LC_RESET_AP_STATS_CC, 0x00),
        .cmd4.APNumber = 29,
        .cmd4.Padding = 0,
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd5), LC_RESET_AP_STATS_CC, 0x00),
        .cmd5.APNumber = 30,
        .cmd5.Padding = 0,
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd6), LC_RESET_AP_STATS_CC, 0x00),
        .cmd6.APNumber = 31,
        .cmd6.Padding = 0,
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd7), LC_RESET_AP_STATS_CC, 0x00),
        .cmd7.APNumber = 32,
        .cmd7.Padding = 1,
        /* 8-12 - Enable Science APs */
        // AP27 - Low Power
        // AP29 - Go to Safe Mode
        // AP30 - Do Science AK
        // AP31 - Do Science CONUS
        // AP32 - Do Science HI
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd8), LC_SET_AP_STATE_CC, 0x00),
        .cmd8.APNumber = 27,
        .cmd8.NewAPState = LC_APSTATE_ACTIVE,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd9), LC_SET_AP_STATE_CC, 0x00),
        .cmd9.APNumber = 29,
        .cmd9.NewAPState = LC_APSTATE_ACTIVE,
        .hdr10.TimeTag = 0,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd10), LC_SET_AP_STATE_CC, 0x00),
        .cmd10.APNumber = 30,
        .cmd10.NewAPState = LC_APSTATE_ACTIVE,
        .hdr11.TimeTag = 0,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd11), LC_SET_AP_STATE_CC, 0x00),
        .cmd11.APNumber = 31,
        .cmd11.NewAPState = LC_APSTATE_ACTIVE,
        .hdr12.TimeTag = 0,
        .cmd12.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd12), LC_SET_AP_STATE_CC, 0x00),
        .cmd12.APNumber = 32,
        .cmd12.NewAPState = LC_APSTATE_ACTIVE,
        /* 13 - Manager Note: Science Initialized */
        .hdr13.TimeTag = 1,
        .cmd13.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd13), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd13.U8 = SS_SCIENCE_INITIALIZED,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts026, SC.RTS_TBL026, SC Example RTS_TBL026, sc_rts026.tbl)
