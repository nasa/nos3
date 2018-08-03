/************************************************************************
** File:
**   $Id: hs_emt.c 1.1 2015/11/12 14:29:06EST wmoleski Exp  $
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
**  The CFS Health and Safety (HS) Event Monitor Table Definition
**
** Notes:
**
** $Log: hs_emt.c  $
** Revision 1.1 2015/11/12 14:29:06EST wmoleski 
** Initial revision
** Member added to project /CFS-APPs-PROJECT/hs/fsw/tables/project.pj
** Revision 1.2 2015/05/04 11:00:02EDT lwalling 
** Change definitions for MAX_CRITICAL to MAX_MONITORED
** Revision 1.1 2015/05/01 14:54:32EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/tables/project.pj
** Revision 1.4 2015/04/27 14:33:47EDT lwalling 
** Add attribute unused to default table definitions
** Revision 1.3 2015/03/03 12:16:26EST sstrege 
** Added copyright information
** Revision 1.2 2011/08/15 15:43:08EDT aschoeni 
** Updated so application name is configurable
** Revision 1.1 2009/05/04 11:50:09EDT aschoeni 
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
    "HS_Default_EventMon_Tbl", HS_APP_NAME ".EventMon_Tbl", "HS EventMon Table",
    "hs_emt.tbl", (sizeof(HS_EMTEntry_t) * HS_MAX_MONITORED_EVENTS)
};



HS_EMTEntry_t      HS_Default_EventMon_Tbl[HS_MAX_MONITORED_EVENTS] =
{
/*          AppName                    NullTerm EventID        ActionType */
                                                
/*   0 */ { "CFE_ES",                  0,       10,            HS_EMT_ACT_NOACT },
/*   1 */ { "CFE_EVS",                 0,       10,            HS_EMT_ACT_NOACT },
/*   2 */ { "CFE_TIME",                0,       10,            HS_EMT_ACT_NOACT },
/*   3 */ { "CFE_TBL",                 0,       10,            HS_EMT_ACT_NOACT },
/*   4 */ { "CFE_SB",                  0,       10,            HS_EMT_ACT_NOACT },
/*   5 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*   6 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*   7 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*   8 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*   9 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  10 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  11 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  12 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  13 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  14 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
/*  15 */ { "",                        0,       10,            HS_EMT_ACT_NOACT },
                                 
};                               
                                 
/************************/       
/*  End of File Comment */       
/************************/       
