/************************************************************************
** File:
**   $Id: cs_eepromtbl.c 1.3 2017/02/24 10:13:36EST mdeschu Exp  $
**
**   Copyright (c) 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**  The CFS Checksum (CS) Application Default EEPROM Table Definition
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cs_platform_cfg.h"
#include "cs_msgdefs.h"
#include "cs_tbldefs.h"
#include "cfe_tbl_filedef.h"

CS_Def_EepromMemory_Table_Entry_t      CS_EepromTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES] =
{
    /*         State           Filler   StartAddress     NumBytes */
    /*  0 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  1 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  2 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  3 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  4 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  5 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  6 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  7 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  8 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /*  9 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 10 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 11 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 12 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 13 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 14 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  },
    /* 15 */ { CS_STATE_EMPTY, 0x1234,  0x00000000,      0x00000000  }

};

/*
** Table file header
*/
CFE_TBL_FILEDEF(CS_EepromTable, CS.DefEepromTbl, CS EEPROM Tbl, cs_eepromtbl.tbl)

/************************/
/*  End of File Comment */
/************************/
