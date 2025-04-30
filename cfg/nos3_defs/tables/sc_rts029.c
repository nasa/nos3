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
** ************************************************
** RTS 029 - Science Mode: Exited Science Mode
** ************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Manager Note: Exiting Science Mode */
    SC_RtsEntryHeader_t hdr1;
    MGR_U8_cmd_t cmd1;
    /* 2 - Disable AP 27 - Science, Low Power */
    SC_RtsEntryHeader_t hdr2;
    LC_SetAPState_t cmd2;
    /* 3 - Disbale AP 28 - Science, Recharged */
    SC_RtsEntryHeader_t hdr3;
    LC_SetAPState_t cmd3;
    /* 4 - Disable AP 29 - Return to Safe Mode */
    SC_RtsEntryHeader_t hdr4;
    LC_SetAPState_t cmd4;
    /* 5 - Disable AP 30 - Science over AK */
    SC_RtsEntryHeader_t hdr5;
    LC_SetAPState_t cmd5;
    /* 6 - Disable AP 31 - Science over CONUS */
    SC_RtsEntryHeader_t hdr6;
    LC_SetAPState_t cmd6;
    /* 7 - Disable AP 32 - Science over HI */
    SC_RtsEntryHeader_t hdr7;
    LC_SetAPState_t cmd7;
    /* 8 - Disable AP 33 - Pause Science, Left AK */
    SC_RtsEntryHeader_t hdr8;
    LC_SetAPState_t cmd8;
    /* 9 - Disable AP 34 - Pause Science, Left CONUS */
    SC_RtsEntryHeader_t hdr9;
    LC_SetAPState_t cmd9;
    /* 10 - Disable AP 35 - Pause Science, Left HI */  
    SC_RtsEntryHeader_t hdr10;
    LC_SetAPState_t cmd10;
    /* 11 - Disable Instrument Application */
    SC_RtsEntryHeader_t hdr11;
    SAMPLE_NoArgs_cmd_t cmd11;
    /* 12 - Disable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr12;
    GENERIC_EPS_Switch_cmd_t cmd12;
    /* 13 - Reset AP 26 - Go to Science Mode */
    SC_RtsEntryHeader_t hdr13;
    LC_ResetAPStats_t cmd13;
    /* 14 - Enable AP 26 - Go to Science Mode */
    SC_RtsEntryHeader_t hdr14;
    LC_SetAPState_t cmd14;
} SC_RtsStruct029_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct029_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable029_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct029_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable029_t SC_Rts029 = {    
.rts = {
        /* 1 - Manager Note: Exited Science Mode */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd1), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd1.U8 = SS_EXITED_SCIENCE_MODE,
        /* 2 - Disable AP 27 - Science, Low Power */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd2), LC_SET_AP_STATE_CC, 0x00),
        .cmd2.APNumber = 27,
        .cmd2.NewAPState = LC_APSTATE_DISABLED,
        /* 3 - Disable AP 28 - Science, Recharged */
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd3), LC_SET_AP_STATE_CC, 0x00),
        .cmd3.APNumber = 28,
        .cmd3.NewAPState = LC_APSTATE_DISABLED,
        /* 4 - Disable AP 29 - Return to Safe Mode */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd4), LC_SET_AP_STATE_CC, 0x00),
        .cmd4.APNumber = 29,
        .cmd4.NewAPState = LC_APSTATE_DISABLED,
        /* 5 - Disable AP 30 - Science over AK */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd5), LC_SET_AP_STATE_CC, 0x00),
        .cmd5.APNumber = 30,
        .cmd5.NewAPState = LC_APSTATE_DISABLED,
        /* 6 - Disable AP 31 - Science over CONUS */
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd6), LC_SET_AP_STATE_CC, 0x00),
        .cmd6.APNumber = 31,
        .cmd6.NewAPState = LC_APSTATE_DISABLED,
        /* 7 - Disable AP 32 - Science over HI */
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd7), LC_SET_AP_STATE_CC, 0x00),
        .cmd7.APNumber = 32,
        .cmd7.NewAPState = LC_APSTATE_DISABLED,
        /* 8 - Disable AP 33 - Pause Science, Left AK */
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd8), LC_SET_AP_STATE_CC, 0x00),
        .cmd8.APNumber = 33,
        .cmd8.NewAPState = LC_APSTATE_DISABLED,
        /* 9 - Disable AP 34 - Pause Science, Left CONUS */
        .hdr9.TimeTag = 1,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd9), LC_SET_AP_STATE_CC, 0x00),
        .cmd9.APNumber = 34,
        .cmd9.NewAPState = LC_APSTATE_DISABLED,
        /* 10 - Disable AP 35 - Pause Science, Left HI */  
        .hdr10.TimeTag = 1,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd10), LC_SET_AP_STATE_CC, 0x00),
        .cmd10.APNumber = 35,
        .cmd10.NewAPState = LC_APSTATE_DISABLED,
        /* 11 - Disable Instrument Application */
        .hdr11.TimeTag = 1,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd11), SAMPLE_DISABLE_CC, 0x00),
        /* 12 - Disable Instrument Switch on EPS*/
        .hdr12.TimeTag = 1,
        .cmd12.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd12), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd12.SwitchNumber = 0,
        .cmd12.State = 0x00,
        /* 13 - Reset AP 26 - Go to Science Mode */
        .hdr13.TimeTag = 1,
        .cmd13.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd13), LC_RESET_AP_STATS_CC, 0x00),
        .cmd13.APNumber = 26,
        .cmd13.Padding = 0,
        /* 14 - Enable AP 26 - Go to Science Mode */
        .hdr14.TimeTag = 1,
        .cmd14.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd14), LC_SET_AP_STATE_CC, 0x00),
        .cmd14.APNumber = 26,
        .cmd14.NewAPState = LC_APSTATE_ACTIVE,
    }
};
/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts029, SC.RTS_TBL029, SC Example RTS_TBL029, sc_rts029.tbl)
