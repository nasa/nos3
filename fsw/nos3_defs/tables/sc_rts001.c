#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */

/*
** Component Includes
*/
#include "ds_msgids.h"
#include "lc_msgids.h"
#include "sample_app_msgids.h"
#include "to_lab_msgids.h"

/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000

/*
** Command code defines
*/
#define DS_SET_APP_STATE_CC   0x0200
#define LC_SET_LC_STATE_CC    0x0200
#define SAMPLE_APP_NOOP_CC    0x0000
#define TO_DEBUG_ENABLE_CC    0x0200

/*
** cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table001", "SC.RTS_TBL001", "SC RTS_TBL001",
    "sc_rts001.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};

/*
** RTS Table Data
*/
uint16 RTS_Table001[SC_RTS_BUFF_SIZE] =
{
/* cmd time,  <---------------------------- cmd pkt primary header ---------------------------->     <----- cmd pkt 2nd header ---->           <-- opt data ---> */
  1,          CFE_MAKE_BIG16(DS_CMD_MID),         CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(5),      CFE_MAKE_BIG16(DS_SET_APP_STATE_CC),      0x0001, 0x0000, // Enable DS
  1,          CFE_MAKE_BIG16(TO_LAB_CMD_MID),     CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(21),     CFE_MAKE_BIG16(TO_DEBUG_ENABLE_CC),       0x0031, 0x3237, 0x2E30, 0x2E30, 0x2E31, 0x0000, 0x0000, 0x0000, 0x0095, 0x1300, // Enable Debug, 127.0.0.1, 5013
  1,          CFE_MAKE_BIG16(SAMPLE_APP_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(1),      CFE_MAKE_BIG16(SAMPLE_APP_NOOP_CC),       // Sample Instrument NOOP
  5,          CFE_MAKE_BIG16(LC_CMD_MID),         CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(5),      CFE_MAKE_BIG16(LC_SET_LC_STATE_CC),       0x0001, 0x0000, // Enable LC
     
};

/************************/
/*  End of File Comment */
/************************/
