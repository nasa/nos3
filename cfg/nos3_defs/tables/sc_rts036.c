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
** RTS 036 - Sample Device Fail in Science Mode
** ************************************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 11 - Disable Instrument Application */
    SC_RtsEntryHeader_t hdr11;
    SAMPLE_NoArgs_cmd_t cmd11;
    /* 12 - Disable Instrument Switch on EPS*/
    SC_RtsEntryHeader_t hdr12;
    GENERIC_EPS_Switch_cmd_t cmd12;
    // 13
    SC_RtsEntryHeader_t hdr13;
    CFE_ES_RestartCmd_t cmd13;
} SC_RtsStruct036_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct036_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable036_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct036_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable036_t SC_Rts036 = {    
.rts = {
        /* 11 - Disable Instrument Application */
        .hdr11.TimeTag = 1,
        .cmd11.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd11), SAMPLE_DISABLE_CC, 0x00),
        /* 12 - Disable Instrument Switch on EPS*/
        .hdr12.TimeTag = 1,
        .cmd12.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_EPS_CMD_MID, SC_MEMBER_SIZE(cmd12), GENERIC_EPS_SWITCH_CC, 0x00),
        .cmd12.SwitchNumber = 0,
        .cmd12.State = 0x00,
        /* 13 - Restart CFS*/
        .hdr13.TimeTag = 1,
        .cmd13.CommandHeader = CFE_MSG_CMD_HDR_INIT(CFE_ES_CMD_MID, SC_MEMBER_SIZE(cmd13), CFE_ES_RESTART_CC, 0x00),
        .cmd13.Payload.RestartType = 1,
        
    }
};
/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts036, SC.RTS_TBL036, SC Example RTS_TBL036, sc_rts036.tbl)
