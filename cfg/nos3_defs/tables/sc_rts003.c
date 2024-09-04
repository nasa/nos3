#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

/* Command Includes */
#include "generic_css_msg.h"
#include "generic_css_msgids.h"
#include "generic_fss_msg.h"
#include "generic_fss_msgids.h"
#include "generic_imu_msg.h"
#include "generic_imu_msgids.h"
#include "generic_mag_msg.h"
#include "generic_mag_msgids.h"
#include "generic_torquer_msg.h"
#include "generic_torquer_msgids.h"
#include "novatel_oem615_msg.h"
#include "novatel_oem615_msgids.h"
#include "generic_adcs_msg.h"
#include "generic_adcs_msgids.h"
#include "generic_adcs_adac.h"

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Enable CSS */
    SC_RtsEntryHeader_t hdr1;
    GENERIC_CSS_NoArgs_cmd_t cmd1;
    /* 2 - Enable FSS */
    SC_RtsEntryHeader_t hdr2;
    GENERIC_FSS_NoArgs_cmd_t cmd2;
    /* 3 - Enable IMU */
    SC_RtsEntryHeader_t hdr3;
    GENERIC_IMU_NoArgs_cmd_t cmd3;
    /* 4 - Enable MAG */
    SC_RtsEntryHeader_t hdr4;
    GENERIC_MAG_NoArgs_cmd_t cmd4;
    /* 5 - Enable torquers */
    SC_RtsEntryHeader_t hdr5;
    GENERIC_TORQUER_NoArgs_cmd_t cmd5;
    /* 6 - Enable GPS */
    SC_RtsEntryHeader_t hdr6;
    NOVATEL_OEM615_NoArgs_cmd_t cmd6;
    /* 7 - Set ADCS to SUNSAFE_MODE */
    SC_RtsEntryHeader_t hdr7;
    Generic_ADCS_Mode_cmd_t cmd7;
} SC_RtsStruct003_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct003_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable003_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct003_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable003_t SC_Rts003 = {
.rts = {
        /* 1 - Enable CSS */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_CSS_CMD_MID, SC_MEMBER_SIZE(cmd1), GENERIC_CSS_ENABLE_CC, 0x00),

        /* 2 - Enable FSS */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_FSS_CMD_MID, SC_MEMBER_SIZE(cmd2), GENERIC_FSS_ENABLE_CC, 0x00),

        /* 3 - Enable IMU */
        .hdr3.TimeTag = 1,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_IMU_CMD_MID, SC_MEMBER_SIZE(cmd3), GENERIC_IMU_ENABLE_CC, 0x00),

        /* 4 - Enable MAG */
        .hdr4.TimeTag = 1,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_MAG_CMD_MID, SC_MEMBER_SIZE(cmd4), GENERIC_MAG_ENABLE_CC, 0x00),

        /* 5 - Enable torquers */
        .hdr5.TimeTag = 1,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_TORQUER_CMD_MID, SC_MEMBER_SIZE(cmd5), GENERIC_TORQUER_ENABLE_CC, 0x00),

        /* 6 - Enable GPS */
        .hdr6.TimeTag = 1,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(NOVATEL_OEM615_CMD_MID, SC_MEMBER_SIZE(cmd6), NOVATEL_OEM615_ENABLE_CC, 0x00),

        /* 7 - Set ADCS to SUNSAFE_MODE */
        .hdr7.TimeTag = 5,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_ADCS_CMD_MID, SC_MEMBER_SIZE(cmd7), GENERIC_ADCS_SET_MODE_CC, 0x00),
        .cmd7.Mode = SUNSAFE_MODE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts003, SC.RTS_TBL003, Safe Mode RTS003, sc_rts003.tbl)
