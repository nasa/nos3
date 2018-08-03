/************************************************************************
** File:
**   $Id: hs_mat.c 1.1 2015/11/12 14:29:06EST wmoleski Exp  $
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
**  The CFS Health and Safety (HS) Message Actions Table Definition
**
** Notes:
**
** $Log: hs_mat.c  $
** Revision 1.1 2015/11/12 14:29:06EST wmoleski 
** Initial revision
** Member added to project /CFS-APPs-PROJECT/hs/fsw/tables/project.pj
** Revision 1.1 2015/05/01 14:54:32EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hs/fsw/tables/project.pj
** Revision 1.4 2015/04/27 14:33:47EDT lwalling 
** Add attribute unused to default table definitions
** Revision 1.3 2015/03/03 12:16:06EST sstrege 
** Added copyright information
** Revision 1.2 2011/08/15 15:42:45EDT aschoeni 
** Updated so application name is configurable
** Revision 1.1 2009/05/04 11:50:10EDT aschoeni 
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
    "HS_Default_MsgActs_Tbl", HS_APP_NAME ".MsgActs_Tbl", "HS MsgActs Table",
    "hs_mat.tbl", (sizeof(HS_MATEntry_t) * HS_MAX_MSG_ACT_TYPES)
};



HS_MATEntry_t      HS_Default_MsgActs_Tbl[HS_MAX_MSG_ACT_TYPES] =
{
/*          EnableState               Cooldown       Message */

/*   0 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   1 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   2 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   3 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   4 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   5 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   6 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
/*   7 */ { HS_MAT_STATE_DISABLED,    10,            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },

};

/************************/
/*  End of File Comment */
/************************/
