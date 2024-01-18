#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_tbldefs.h"      /* defines SC table headers */
#include "sc_platform_cfg.h" /* defines table buffer size */
#include "sc_msgdefs.h"      /* defines SC command code values */
#include "sc_msgids.h"       /* defines SC packet msg ID's */
#include "sc_msg.h"          /* defines SC message structures */

/* Command Includes */
#include "generic_radio_msg.h"
#include "generic_radio_msgids.h"
#include "sample_msg.h"
#include "sample_msgids.h"

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    SC_RtsEntryHeader_t hdr1;
    SAMPLE_Config_cmd_t cmd1;
    SC_RtsEntryHeader_t hdr2;
    GENERIC_RADIO_Proximity_cmd_t cmd2;
} SC_RtsStruct005_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct005_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable005_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct005_t *)0)->member))

/* Used designated intializers to be verbose, modify as needed/desired */
SC_RtsTable005_t SC_Rts005 = {    
.rts = {
        /* 1 - Sample Configuration 123 */
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(SAMPLE_CMD_MID, SC_MEMBER_SIZE(cmd1), SAMPLE_CONFIG_CC, 0x00),
        .cmd1.DeviceCfg = CFE_MAKE_BIG32(123),

        /* 2 - Radio Proximity Run Rts5 */
        .hdr2.TimeTag = 1,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(GENERIC_RADIO_CMD_MID, SC_MEMBER_SIZE(cmd2), GENERIC_RADIO_PROXIMITY_CC, 0x00),
        .cmd2.SCID = 0,
        .cmd2.Payload = {0x18, 0xA9, 0xC0, 0x00, 0x00, 0x05, 0x04, 0x00, 0x05, 0x00, 0x00, 0x00},
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts005, SC.RTS_TBL005, SC Example RTS_TBL005, sc_rts005.tbl)
