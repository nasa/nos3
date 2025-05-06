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
** RTS 025 - Enable Science Mode
** *****************************
*/

/* Custom table structure, modify as needed to add desired commands */
typedef struct
{
    /* 1 - Disable AP 25: Science_Reboot to Science */
    SC_RtsEntryHeader_t hdr1;
    LC_SetAPState_t cmd1;
    /* 2 - Set MGR from Science_Reboot to Science Mode */
    SC_RtsEntryHeader_t hdr2;
    MGR_U8_cmd_t cmd2;
} SC_RtsStruct025_t;

/* Define the union to size the table correctly */
typedef union
{
    SC_RtsStruct025_t rts;
    uint16            buf[SC_RTS_BUFF_SIZE];
} SC_RtsTable025_t;

/* Helper macro to get size of structure elements */
#define SC_MEMBER_SIZE(member) (sizeof(((SC_RtsStruct025_t *)0)->member))

/* Used designated initializers to be verbose, modify as needed/desired */
SC_RtsTable025_t SC_Rts025 = {    
.rts = {
        /* 1 - Disable AP 25 - Science_Reboot to Science Mode */  
        .hdr1.TimeTag = 1,
        .cmd1.CmdHeader = CFE_MSG_CMD_HDR_INIT(LC_CMD_MID, SC_MEMBER_SIZE(cmd1), LC_SET_AP_STATE_CC, 0x00),
        .cmd1.APNumber = 25,
        .cmd1.NewAPState = LC_APSTATE_DISABLED,
        /* 2 - Set MGR from Science_Reboot to Science Mode */
        .hdr2.TimeTag = 0,
        .cmd2.CmdHeader = CFE_MSG_CMD_HDR_INIT(MGR_CMD_MID, SC_MEMBER_SIZE(cmd2), MGR_SET_MODE_CC, 0x00),
        .cmd2.U8 = MGR_SCIENCE_MODE,
    }
};

/* Macro for table structure */
CFE_TBL_FILEDEF(SC_Rts025, SC.RTS_TBL025, SC Example RTS_TBL025, sc_rts025.tbl)
