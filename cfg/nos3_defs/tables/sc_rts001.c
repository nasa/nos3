#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

/* Command Includes */
#include "ds_msg.h"
#include "ds_msgdefs.h"
#include "ds_msgids.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "sample_msg.h"
#include "sample_msgids.h"
#include "to_cmds.h"
#include "to_lab_msgids.h"
#include "to_lab_msg.h"
#include "lc_msgids.h"
#include "lc_app.h"
#include "mgr_msgids.h"
#include "mgr_app.h"


/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Enable DS */
    SC_RtsEntryHeader_t hdr1;
    DS_AppStateCmd_t cmd1;
    /* 2 - Enable Debug */
    SC_RtsEntryHeader_t hdr2;
    TO_LAB_EnableOutputCmd_t cmd2;
    /* 3 - Enable RTS 3-64 */
    SC_RtsEntryHeader_t hdr3;
    SC_RtsGrpCmd_t cmd3;
    /* 4 - Enable LC */
    SC_RtsEntryHeader_t hdr4;
    LC_SetLCState_t cmd4;
    /* 5 - Start RTS 3 (Safe Mode) */
    SC_RtsEntryHeader_t hdr5;
    SC_RtsCmd_t cmd5;
    /* 6 - Enable Science Transition RTS 26 */
    SC_RtsEntryHeader_t hdr6;
    SC_RtsCmd_t cmd6;
    /* 7 - Reset Science Mode AP */
    SC_RtsEntryHeader_t hdr7;
    LC_ResetAPStats_t cmd7;
    /* 8 - Enable Science Mode AP */
    SC_RtsEntryHeader_t hdr8;
    LC_SetAPState_t cmd8;
    /* 9 - Enable Science_Reboot to Science Mode RTS 25 */
    SC_RtsEntryHeader_t hdr9;
    SC_RtsCmd_t cmd9;
    /* 10 - Reset Science_Reboot to Science AP */
    SC_RtsEntryHeader_t hdr10;
    LC_ResetAPStats_t cmd10;
    /* 11 - Enable Science_Reboot to Science AP */
    SC_RtsEntryHeader_t hdr11;
    LC_SetAPState_t cmd11;
    /* 12 - Update Science Status in MGR */
    SC_RtsEntryHeader_t hdr12;
    MGR_U8_cmd_t cmd12;
    /* 12 - Update Science Status in MGR */
    SC_RtsEntryHeader_t hdr13;
    SC_RtsCmd_t cmd13;
} SC_RtsStruct001_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct001_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable001_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct001_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable001_t SC_Rts001 = {
.rts = {
        /* 1 - Enable DS */
        .hdr1.TimeTag = 1,
        .cmd1.CommandHeader = CFE_MSG_CMD_HDR_INIT(DS_CMD_MID, SC_MEMBER_SIZE(cmd1), DS_SET_APP_STATE_CC, 0x00),
        .cmd1.Payload.EnableState = 0x0001,
        .cmd1.Payload.Padding = 0x0000,

        /* 2 - Enable Debug */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(TO_LAB_CMD_MID, SC_MEMBER_SIZE(cmd2), TO_LAB_OUTPUT_ENABLE_CC, 0x00),
        .cmd2.Payload.dest_IP = "active-gs",

        /* 3 - Enable RTS 3-64 */
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd3), SC_ENABLE_RTS_GRP_CC, 0x00),
        .cmd3.FirstRtsId = 3,
        .cmd3.LastRtsId = 64,

        /* 4 - Enable LC */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd4), LC_SET_LC_STATE_CC, 0x00),
        .cmd4.NewLCState = LC_STATE_ACTIVE,
        .cmd4.Padding = 0x0000,

        /* 5 - Start RTS 3 (Safe Mode) */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd5), SC_START_RTS_CC, 0x00),
        .cmd5.RtsId = 3,

        /* 6 - Enable Science Transition RTS: (26) */
        .hdr6.TimeTag = 0,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd6), SC_ENABLE_RTS_CC, 0x00),
        .cmd6.RtsId = 26,
        .cmd6.Padding = 0,

        /* 7 - Reset Science Mode AP */
        .hdr7.TimeTag = 1,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd7), LC_RESET_AP_STATS_CC, 0x00),
        .cmd7.APNumber = 26,
        .cmd7.Padding = 0,

        /* 8 - Enable Science Mode AP */
        .hdr8.TimeTag = 1,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd8), LC_SET_AP_STATE_CC, 0x00),
        .cmd8.APNumber = 26,
        .cmd8.NewAPState = LC_APSTATE_ACTIVE,

        /* 9 - Enable Science Reboot to Science RTS: (25) */
        .hdr9.TimeTag = 0,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd9), SC_ENABLE_RTS_CC, 0x00),
        .cmd9.RtsId = 25,
        .cmd9.Padding = 0,

        /* 10 - Reset Science_Reboot to Science Mode AP 25 */
        .hdr10.TimeTag = 1,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd10), LC_RESET_AP_STATS_CC, 0x00),
        .cmd10.APNumber = 25,
        .cmd10.Padding = 0,

        /* 11 - Enable Science_Reboot to Science Mode AP 25*/
        .hdr11.TimeTag = 1,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd11), LC_SET_AP_STATE_CC, 0x00),
        .cmd11.APNumber = 25,
        .cmd11.NewAPState = LC_APSTATE_ACTIVE,

        /* 12 - Set Science Status to Off to avoid Confusion with reloaded Science Status */
        .hdr12.TimeTag = 1,
        .cmd12.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd12), MGR_UPDATE_SCI_STATUS_CC, 0x00),
        .cmd12.U8 = SS_SCIENCE_OFF,
    #ifdef ENABLE_GROUND_OPERATIONS_EXERCISE
        /* 13 - Start RTS 37 */
        .hdr13.TimeTag = 1,
        .cmd13.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd13), SC_START_RTS_CC, 0x00),
        .cmd13.RtsId = 37,
    #endif
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts001, SC.RTS_TBL001, POR RTS001, sc_rts001.tbl)
