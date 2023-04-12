#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */

/*
** Component Includes
*/
#include "generic_radio_msgids.h"
#include "sample_msgids.h"

/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000

/*
** Command code defines
*/
#define SAMPLE_CONFIG_CC           0x0400
#define GENERIC_RADIO_PROXIMITY_CC 0x0300
#define SC_START_RTS_CC            0x0400

/*
** cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table005", "SC.RTS_TBL005", "SC RTS_TBL005",
    "sc_rts005.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};

/*
** RTS Table Data
*/
uint16 RTS_Table005[SC_RTS_BUFF_SIZE] =
{
/* cmd time,  <---------------------------- cmd pkt primary header ---------------------------->     <----- cmd pkt 2nd header ---->             <-- opt data ---> */
  1,          CFE_MAKE_BIG16(SAMPLE_CMD_MID),     CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(5),      CFE_MAKE_BIG16(SAMPLE_CONFIG_CC),           0x0000, 0x007B, // Sample Instrument Configuration 123
  1,          CFE_MAKE_BIG16(GENERIC_RADIO_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(13),   CFE_MAKE_BIG16(GENERIC_RADIO_PROXIMITY_CC), 0x0000, CFE_MAKE_BIG16(SC_CMD_MID),   CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(3), CFE_MAKE_BIG16(SC_START_RTS_CC), 0x0005, // Radio Proximity Run RTS5
};



/************************/
/*  End of File Comment */
/************************/
