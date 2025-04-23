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
#include "lc_app.h"
#include "lc_msgids.h"
#include "mgr_msgids.h"
#include "mgr_app.h"

/* 
** ***************************************************
** RTS 028 - Science Mode: Recharged, Resuming Science
** ***************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Manager Note: Recharged, Resuming Science */
    SC_RtsEntryHeader_t hdr1;
    MGR_U8_cmd_t cmd1;
    /* 2 - Disable Science AP 28 - Science Recharged */
    SC_RtsEntryHeader_t hdr2;
    LC_SetAPState_t cmd2;
    /* 3 - Reset AP 27 - Science Low Power */
    SC_RtsEntryHeader_t hdr3;
    LC_ResetAPStats_t cmd3;
    /* 4 - Reset AP 29 - Go to Safe Mode */
    SC_RtsEntryHeader_t hdr4;
    LC_ResetAPStats_t cmd4;
    /* 5 - Reset AP 30 - Do Science Over AK */
    SC_RtsEntryHeader_t hdr5;
    LC_ResetAPStats_t cmd5;
    /* 6 - Reset AP 31 - Do Science over CONUS */
    SC_RtsEntryHeader_t hdr6;
    LC_ResetAPStats_t cmd6;
    /* 7 - Reset AP 32 - Do Science over HI */
    SC_RtsEntryHeader_t hdr7;
    LC_ResetAPStats_t cmd7;
    /* 8 - Enable AP 27 - Science Low Power */
    SC_RtsEntryHeader_t hdr8;
    LC_SetAPState_t cmd8;
    /* 9 - Enable AP 29 - Go to Safe Mode */
    SC_RtsEntryHeader_t hdr9;
    LC_SetAPState_t cmd9;
    /* 10 - Enable AP 30 - Do Science Over AK */  
    SC_RtsEntryHeader_t hdr10;
    LC_SetAPState_t cmd10;
    /* 11 - Enable AP 31 - Do Science over CONUS */
    SC_RtsEntryHeader_t hdr11;
    LC_SetAPState_t cmd11;
    /* 12 - Enable AP 32 - Do Science over HI */
    SC_RtsEntryHeader_t hdr12;
    LC_SetAPState_t cmd12;
} SC_RtsStruct028_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct028_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable028_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct028_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable028_t SC_Rts028 = {    
.rts = {
        /* 1 - Manager Note: Recharged, Resuming Science */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd1), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd1.U8 = SS_NO_SCIENCE_RECHARGED,
        /* 2 - Disable Science AP 28 - Science Recharged */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd2), LC_SET_AP_STATE_CC, 0x00),
        .cmd2.APNumber = 28,
        .cmd2.NewAPState = LC_APSTATE_DISABLED,
        /* 3-7 - Reset Science APs */
        /* 3 - Reset AP 27 - Science Low Power */
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd3), LC_RESET_AP_STATS_CC, 0x00),
        .cmd3.APNumber = 27,
        .cmd3.Padding = 0,
        /* 4 - Reset AP 29 - Go to Safe Mode */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd4), LC_RESET_AP_STATS_CC, 0x00),
        .cmd4.APNumber = 29,
        .cmd4.Padding = 0,
        /* 5 - Reset AP 30 - Do Science Over AK */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd5), LC_RESET_AP_STATS_CC, 0x00),
        .cmd5.APNumber = 30,
        .cmd5.Padding = 0,
        /* 6 - Reset AP 31 - Do Science over CONUS */
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd6), LC_RESET_AP_STATS_CC, 0x00),
        .cmd6.APNumber = 31,
        .cmd6.Padding = 0,
        /* 7 - Reset AP 32 - Do Science over HI */
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd7), LC_RESET_AP_STATS_CC, 0x00),
        .cmd7.APNumber = 32,
        .cmd7.Padding = 1,
        /* 8-12 - Enable Science APs */
        /* 8 - Enable AP 27 - Science Low Power */
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd8), LC_SET_AP_STATE_CC, 0x00),
        .cmd8.APNumber = 27,
        .cmd8.NewAPState = LC_APSTATE_ACTIVE,
        /* 9 - Enable AP 29 - Go to Safe Mode */
        .hdr9.TimeTag = 1,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd9), LC_SET_AP_STATE_CC, 0x00),
        .cmd9.APNumber = 29,
        .cmd9.NewAPState = LC_APSTATE_ACTIVE,
        /* 10 - Enable AP 30 - Do Science Over AK */
        .hdr10.TimeTag = 1,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd10), LC_SET_AP_STATE_CC, 0x00),
        .cmd10.APNumber = 30,
        .cmd10.NewAPState = LC_APSTATE_ACTIVE,
        /* 11 - Enable AP 31 - Do Science over CONUS */
        .hdr11.TimeTag = 1,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd11), LC_SET_AP_STATE_CC, 0x00),
        .cmd11.APNumber = 31,
        .cmd11.NewAPState = LC_APSTATE_ACTIVE,
        /* 12 - Enable AP 32 - Do Science over HI */
        .hdr12.TimeTag = 1,
        .cmd12.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd12), LC_SET_AP_STATE_CC, 0x00),
        .cmd12.APNumber = 32,
        .cmd12.NewAPState = LC_APSTATE_ACTIVE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts028, SC.RTS_TBL028, SC Example RTS_TBL028, sc_rts028.tbl)
