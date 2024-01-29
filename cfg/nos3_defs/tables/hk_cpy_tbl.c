/************************************************************************
** File:
**   $Id: hk_cpy_tbl.c 1.2 2015/11/10 16:48:55EST lwalling Exp  $
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
**  The CFS Housekeeping (HK) Application Copy Table Definition
**
** Notes:
**
** $Log: hk_cpy_tbl.c  $
** Revision 1.2 2015/11/10 16:48:55EST lwalling 
** Restore data lost in MKS 2010 from MKS 2009
** Revision 1.1 2015/07/25 21:31:55EDT rperera 
** Initial revision
** Member added to project /CFS-APPs-PROJECT/hk/fsw/tables/project.pj
** Revision 1.7 2015/03/04 14:58:33EST sstrege 
** Added copyright information
** Revision 1.6 2012/08/15 18:50:58EDT aschoeni 
** fixed table compile warning
** Revision 1.5 2010/07/16 13:29:12EDT jmdagost 
** Fixed app name from "HK_APP" to "HK".
** Revision 1.4 2008/09/17 10:53:43EDT rjmcgraw 
** Member moved from hk_cpy_tbl.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj to hk_cpy_tbl.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/tables/project.pj.
** Revision 1.3 2008/09/17 09:53:43ACT rjmcgraw 
** DCR4325:1 Changed tbl name in hdr from HK_APP.HkCopyTable to HK_APP.CopyTable
** Revision 1.2 2008/09/11 11:29:20EDT rjmcgraw 
** DCR4041:1 Added hk_tbldefs.h to list of #includes
** Revision 1.1 2008/04/09 16:42:07EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj
**
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_msgids.h"
#include "hk_utils.h"
#include "hk_app.h"
#include "hk_msgids.h"
#include "hk_tbldefs.h"
#include "cfe_tbl_filedef.h"

hk_copy_table_entry_t      HK_CopyTable[HK_COPY_TABLE_ENTRIES] =
{
/*         inputMid        inputOffset     outputMid    outputOffset  numBytes*/

/*   0 */ { CFE_EVS_HK_TLM_MID,   12,	HK_COMBINED_PKT1_MID,      12,   4, },
/*   1 */ { CFE_TIME_HK_TLM_MID,  12,   HK_COMBINED_PKT1_MID,      16,   4, },
/*   2 */ { CFE_SB_HK_TLM_MID,    12,   HK_COMBINED_PKT1_MID,      20,   4, },
/*   3 */ { CFE_ES_HK_TLM_MID,    12,   HK_COMBINED_PKT1_MID,      24,   4, },
/*   4 */ { CFE_TBL_HK_TLM_MID,   12,   HK_COMBINED_PKT1_MID,      28,   4, },

/*   5 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*   6 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*   7 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*   8 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },

/*   9 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  10 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  11 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  12 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  13 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  14 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  15 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },                                                                                                        
/*  16 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  17 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  18 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  19 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  20 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  21 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  22 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  23 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  24 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  25 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },                                                                                                        
/*  26 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  27 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  28 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  29 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  30 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  31 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  32 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },                                                                                                        
/*  33 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  34 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  35 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  36 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  37 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  38 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  39 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  40 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  41 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  42 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  43 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  44 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  45 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  46 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  47 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  48 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  49 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  50 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  51 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  52 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  53 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  54 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  55 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  56 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  57 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  58 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  59 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  60 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  61 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  62 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  63 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  64 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  65 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  66 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  67 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  68 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  69 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  70 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  71 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  72 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  73 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  74 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  75 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  76 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  77 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  78 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  79 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  80 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  81 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  82 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  83 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  84 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  85 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  86 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  87 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  88 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  89 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  90 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  91 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  92 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  93 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  94 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  95 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  96 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  97 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  98 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/*  99 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 100 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 101 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 102 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 103 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 104 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 105 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 106 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 107 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 108 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 109 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 110 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 111 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 112 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 113 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 114 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 115 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 116 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 117 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 118 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 119 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 120 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 121 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 122 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 123 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 124 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 125 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 126 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },
/* 127 */ { HK_UNDEFINED_ENTRY,    0,   HK_UNDEFINED_ENTRY,       0,   0, },

};

/*
** Table file header
*/
CFE_TBL_FILEDEF(HK_CopyTable, HK.CopyTable, HK Copy Tbl, hk_cpy_tbl.tbl)

/************************/
/*  End of File Comment */
/************************/
