/************************************************************************
** File:
**   $Id: hs_amt.c 1.1 2015/11/12 14:29:05EST wmoleski Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose:
**  The CFS Health and Safety (HS) Applications Monitor Table Definition
**
** Notes:
**
** $Log: hs_amt.c  $
** Revision 1.1 2015/11/12 14:29:05EST wmoleski 
** Initial revision
** Member added to project /CFS-APPs-PROJECT/hs/fsw/tables/project.pj
** Revision 1.2 2015/05/04 11:00:08EDT lwalling 
** Change definitions for MAX_CRITICAL to MAX_MONITORED
** Revision 1.1 2015/05/01 14:54:31EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/tables/project.pj
** Revision 1.4 2015/04/27 14:33:46EDT lwalling 
** Add attribute unused to default table definitions
** Revision 1.3 2015/03/03 12:16:12EST sstrege 
** Added copyright information
** Revision 1.2 2011/08/15 15:43:17EDT aschoeni 
** Updated so application name is configurable
** Revision 1.1 2009/05/04 11:50:07EDT aschoeni 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/project.pj
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "hs_tbl.h"
#include "hs_tbldefs.h"
#include "cfe_tbl_filedef.h"


static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "HS_Default_AppMon_Tbl", HS_APP_NAME ".AppMon_Tbl", "HS AppMon Table",
    "hs_amt.tbl", (sizeof(HS_AMTEntry_t) * HS_MAX_MONITORED_APPS)
};



HS_AMTEntry_t      HS_Default_AppMon_Tbl[HS_MAX_MONITORED_APPS] =
{
/*          AppName                    NullTerm CycleCount     ActionType */
                                                
/*   0 */ { "CFE_ES",                  0,       10,            HS_AMT_ACT_NOACT },
/*   1 */ { "CFE_EVS",                 0,       10,            HS_AMT_ACT_NOACT },
/*   2 */ { "CFE_TIME",                0,       10,            HS_AMT_ACT_NOACT },
/*   3 */ { "CFE_TBL",                 0,       10,            HS_AMT_ACT_NOACT },
/*   4 */ { "CFE_SB",                  0,       10,            HS_AMT_ACT_NOACT },
/*   5 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*   6 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*   7 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*   8 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*   9 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  10 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  11 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  12 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  13 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  14 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  15 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  16 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  17 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  18 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  19 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  20 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  21 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  22 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  23 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  24 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  25 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  26 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  27 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  28 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  29 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  30 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },
/*  31 */ { "",                        0,       10,            HS_AMT_ACT_NOACT },

};

/************************/
/*  End of File Comment */
/************************/
