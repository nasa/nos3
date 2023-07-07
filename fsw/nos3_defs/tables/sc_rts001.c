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

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    SC_RtsEntryHeader_t hdr1;
    DS_AppStateCmd_t cmd1;
    SC_RtsEntryHeader_t hdr2;
    TO_LAB_EnableOutputCmd_t cmd2;
    SC_RtsEntryHeader_t hdr3;
    SC_RtsGrpCmd_t cmd3;
    SC_RtsEntryHeader_t hdr4;
    SAMPLE_NoArgs_cmd_t cmd4;
    SC_RtsEntryHeader_t hdr5;
    SAMPLE_NoArgs_cmd_t cmd5;
    SC_RtsEntryHeader_t hdr6;
    LC_SetLCState_t cmd6;
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
SC_RtsTable001_t SC_Rts001 = 
{
    /* 1 - Enable DS */
    .rts.hdr1.TimeTag = 1,
    .rts.cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(DS_CMD_MID, SC_MEMBER_SIZE(cmd1), DS_SET_APP_STATE_CC, 0x00),
    .rts.cmd1.EnableState = 0x0001,
    .rts.cmd1.Padding = 0x0000,

    /* 2 - Enable Debug */
    .rts.hdr2.TimeTag = 1,
    .rts.cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(TO_LAB_CMD_MID, SC_MEMBER_SIZE(cmd2), SC_ENABLE_RTS_CC, 0x00),
    .rts.cmd2.Payload.dest_IP = "127.0.0.1:5013",

    /* 3 - Enable RTS 3-62 */
    .rts.hdr3.TimeTag = 1,
    .rts.cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd3), SC_ENABLE_RTS_GRP_CC, 0x00),
    .rts.cmd3.FirstRtsId = 3,
    .rts.cmd3.LastRtsId = 62,

    /* 4 - Sample NOOP */
    .rts.hdr4.TimeTag = 1,
    .rts.cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd4), SAMPLE_NOOP_CC, 0x00),

    /* 5 - Sample Enable */
    .rts.hdr5.TimeTag = 1,
    .rts.cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd5), SAMPLE_ENABLE_CC, 0x00),

    /* 6 - Enable LC */
    .rts.hdr6.TimeTag = 1,
    .rts.cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd6), LC_SET_LC_STATE_CC, 0x00),
    .rts.cmd6.NewLCState = LC_STATE_ACTIVE,
    .rts.cmd6.Padding = 0x0000,
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts001, SC.RTS_TBL001, SC Example RTS_TBL001, sc_rts001.tbl)
