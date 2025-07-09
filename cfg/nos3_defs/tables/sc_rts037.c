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

#include "generic_reaction_wheel_msgids.h"
#include "generic_reaction_wheel_msg.h"
#include "generic_reaction_wheel_events.h"
#include "generic_reaction_wheel_app.h"

#include "generic_css_msg.h"
#include "generic_css_msgids.h"
#include "generic_css_app.h"

#include "generic_fss_msg.h"
#include "generic_fss_msgids.h"
#include "generic_fss_app.h"

#include "generic_mag_msg.h"
#include "generic_mag_msgids.h"
#include "generic_mag_app.h"

#include "generic_star_tracker_msg.h"
#include "generic_star_tracker_msgids.h"
#include "generic_star_tracker_app.h"

// #include "cpu1_msgids.h"
// #include "default_cfe_es_fcncodes.h"


#define CFE_ES_CMD_MID 0x1806
#define CFE_ES_RESTART_CC 2

// /**
//  * \brief cFS command header
//  */
// typedef struct CFE_MSG_CommandHeader CFE_MSG_CommandHeader_t;

typedef struct CFE_ES_RestartCmd_Payload
{
    uint16 RestartType; /**< \brief #CFE_PSP_RST_TYPE_PROCESSOR=Processor Reset
                             or #CFE_PSP_RST_TYPE_POWERON=Power-On Reset        */
} CFE_ES_RestartCmd_Payload_t;

/**
 * \brief Restart cFE Command
 */
typedef struct CFE_ES_RestartCmd
{
    CFE_MSG_CommandHeader_t     CommandHeader; /**< \brief Command header */
    CFE_ES_RestartCmd_Payload_t Payload;       /**< \brief Command payload */
} CFE_ES_RestartCmd_t;

/* 
** ************************************************
** RTS 037 - Random Errors
** ************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Disable Instrument Application */
    SC_RtsEntryHeader_t hdr1;
    SAMPLE_NoArgs_cmd_t cmd1;
    /* 2 - Disable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr2;
    GENERIC_RW_Cmd_t cmd2;

    /* 3 - Disable RW 0*/
    SC_RtsEntryHeader_t hdr3;
    GENERIC_RW_Cmd_t cmd3;

    /* 4 - Enable Star Tracker Switch 7 on EPS*/
    SC_RtsEntryHeader_t hdr4;
    GENERIC_EPS_Switch_cmd_t cmd4;

    /* 5 - Disable Star Tracker Switch 7 on EPS*/
    SC_RtsEntryHeader_t hdr5;
    GENERIC_EPS_Switch_cmd_t cmd5;

    /* 6 - Enable RW 0*/
    SC_RtsEntryHeader_t hdr6;
    GENERIC_RW_Cmd_t cmd6;

    /* 7 - Disable Instrument Application*/
    SC_RtsEntryHeader_t hdr7;
    GENERIC_CSS_NoArgs_cmd_t cmd7;

    /* 8 - Disable Instrument Application*/
    SC_RtsEntryHeader_t hdr8;
    GENERIC_FSS_NoArgs_cmd_t cmd8;

    /* 9 - Disable Instrument Application*/
    SC_RtsEntryHeader_t hdr9;
    GENERIC_MAG_NoArgs_cmd_t cmd9;

    /* 10 - Disable Instrument Application*/
    SC_RtsEntryHeader_t hdr10;
    GENERIC_STAR_TRACKER_NoArgs_cmd_t cmd10;

    /* 11 - Start RTS 37*/
    SC_RtsEntryHeader_t hdr11;
    SC_RtsCmd_t cmd11;

} SC_RtsStruct037_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct037_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable037_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct037_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable037_t SC_Rts037 = {    
.rts = {
        /* 1 - Disable Instrument Application */
        .hdr1.TimeTag = 10,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd1), SAMPLE_DISABLE_CC, 0x00),
        
        /* 2 - RW noop command */
        .hdr2.TimeTag = 30,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_RW_APP_CMD_MID, SC_MEMBER_SIZE(cmd2), GENERIC_RW_ENABLE_CC, 0x00),
        .cmd3.wheel_number = 2,

        /* 3 - Set RW 0 to disable */
        .hdr3.TimeTag = 10,
        .cmd3.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_RW_APP_CMD_MID, SC_MEMBER_SIZE(cmd3), GENERIC_RW_DISABLE_CC, 0x00),
        .cmd3.wheel_number = 0,

        /* 4 - Enable Star Tracker Switch 7 on EPS*/
        .hdr4.TimeTag = 30,
        .cmd4.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd4), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd4.SwitchNumber = 7,
        .cmd4.State = 0xaa,

        /* 5 - Disable Star Tracker Switch 7 on EPS*/
        .hdr5.TimeTag = 360,
        .cmd5.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd5), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd5.SwitchNumber = 7,
        .cmd5.State = 0x00,

        /* 6 - Set RW 0 to enable */
        .hdr6.TimeTag = 10,
        .cmd6.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_RW_APP_CMD_MID, SC_MEMBER_SIZE(cmd6), GENERIC_RW_ENABLE_CC, 0x00),
        .cmd6.wheel_number = 0,

        /* 7 - Disable Instrument Application */
        .hdr7.TimeTag = 120,
        .cmd7.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_CSS_CMD_MID, SC_MEMBER_SIZE(cmd7), GENERIC_CSS_DISABLE_CC, 0x00),

        /* 8 - Disable Instrument Application */
        .hdr8.TimeTag = 5,
        .cmd8.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_FSS_CMD_MID, SC_MEMBER_SIZE(cmd8), GENERIC_FSS_DISABLE_CC, 0x00),

        /* 9 - Disable Instrument Application */
        .hdr9.TimeTag = 5,
        .cmd9.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_MAG_CMD_MID, SC_MEMBER_SIZE(cmd9), GENERIC_MAG_DISABLE_CC, 0x00),

        /* 10 - Disable Instrument Application */
        .hdr10.TimeTag = 5,
        .cmd10.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_STAR_TRACKER_CMD_MID, SC_MEMBER_SIZE(cmd10), GENERIC_STAR_TRACKER_DISABLE_CC, 0x00),

        /* 11 - Start RTS 37 (Random Errors) */
        .hdr11.TimeTag = 15,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(SC_CMD_MID, SC_MEMBER_SIZE(cmd11), SC_START_RTS_CC, 0x00),
        .cmd11.RtsId = 37,
        
    }
};
/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts037, SC.RTS_TBL037, SC Example RTS_TBL037, sc_rts037.tbl)
