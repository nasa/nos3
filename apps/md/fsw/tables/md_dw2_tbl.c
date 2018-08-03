/************************************************************************
** File:
**   $Id: md_dw2_tbl.c 1.3 2015/03/01 17:17:23EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose:
**  The CFS Memory Dwell (MD) Example Dwell Table #2
**
** Notes:
**
** $Log: md_dw2_tbl.c  $
** Revision 1.3 2015/03/01 17:17:23EST sstrege 
** Added copyright information
** Revision 1.2 2012/07/20 16:57:56EDT aschoeni 
** Fixed compiler warnings
** Revision 1.1 2009/10/09 17:12:00EDT aschoeni 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/md/fsw/tables/project.pj
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_tbldefs.h"
#include "md_app.h"
#include "cfs_utils.h"
#include "cfe_tbl_filedef.h"
#include "md_platform_cfg.h"


MD_DwellTableLoad_t     MD_Default_Dwell2_Tbl =
{
/* Enabled State */ MD_DWELL_STREAM_DISABLED,
#if MD_SIGNATURE_OPTION == 1  
/* Signature     */ "Default Table 2",
#endif
/* Entry    Length    Delay    Offset           SymName     */
/*   1 */{{      0,       0,  {     0,               ""  }   },
/*   2 */ {      0,       0,  {     0,               ""  }   },
/*   3 */ {      0,       0,  {     0,               ""  }   },
/*   4 */ {      0,       0,  {     0,               ""  }   },
/*   5 */ {      0,       0,  {     0,               ""  }   },
/*   6 */ {      0,       0,  {     0,               ""  }   },
/*   7 */ {      0,       0,  {     0,               ""  }   },
/*   8 */ {      0,       0,  {     0,               ""  }   },
/*   9 */ {      0,       0,  {     0,               ""  }   },
/*  10 */ {      0,       0,  {     0,               ""  }   },
/*  11 */ {      0,       0,  {     0,               ""  }   },
/*  12 */ {      0,       0,  {     0,               ""  }   },
/*  13 */ {      0,       0,  {     0,               ""  }   },
/*  14 */ {      0,       0,  {     0,               ""  }   },
/*  15 */ {      0,       0,  {     0,               ""  }   },
/*  16 */ {      0,       0,  {     0,               ""  }   },
/*  17 */ {      0,       0,  {     0,               ""  }   },
/*  18 */ {      0,       0,  {     0,               ""  }   },
/*  19 */ {      0,       0,  {     0,               ""  }   },
/*  20 */ {      0,       0,  {     0,               ""  }   },
/*  21 */ {      0,       0,  {     0,               ""  }   },
/*  22 */ {      0,       0,  {     0,               ""  }   },
/*  23 */ {      0,       0,  {     0,               ""  }   },
/*  24 */ {      0,       0,  {     0,               ""  }   },
/*  25 */ {      0,       0,  {     0,               ""  }   },
}
};

CFE_TBL_FILEDEF(MD_Default_Dwell2_Tbl, MD.DWELL_TABLE2, MD Dwell Table 2,md_dw2_tbl.tbl)

/************************/
/*  End of File Comment */
/************************/

