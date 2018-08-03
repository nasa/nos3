/*
** $Id: fm_freespace.c 1.8 2015/02/28 17:50:38EST sstrege Exp  $
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
** Title: File Manager (FM) File System Free Space Table Data
**
** Purpose: Default table contents
**
** Author: S. Walling, Microtel-LLC
**
** Notes:
**
** $Log: fm_freespace.c  $
** Revision 1.8 2015/02/28 17:50:38EST sstrege 
** Added copyright information
** Revision 1.7 2014/12/12 14:19:09EST lwalling 
** Add __attribute__((__used__)) to CFE_TBL_FileDef
** Revision 1.6 2009/11/13 16:17:17EST lwalling 
** Remove obsolete field TableID, add new UNUSED state
** Revision 1.5 2009/10/30 15:56:44EDT lwalling 
** Add missing free space table entries, modify table entry state definitions
** Revision 1.4 2009/10/30 14:02:24EDT lwalling 
** Remove trailing white space from all lines
** Revision 1.3 2009/10/30 10:49:26EDT lwalling
** Add include fm_msg.h to get table structure definitions
** Revision 1.2 2009/10/28 16:40:58EDT lwalling
** Complete effort to replace the use of phrase device table with file system free space table
** Revision 1.1 2009/10/09 17:27:47EDT lwalling
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/tables/project.pj
*/

/*************************************************************************
**
** Include section
**
**************************************************************************/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "fm_platform_cfg.h"
#include "fm_tbl.h"
#include "fm_msg.h"

/*
** FM file system free space table header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "FM_FreeSpaceTable", FM_APP_NAME "." FM_TABLE_CFE_NAME,
    FM_TABLE_DEF_DESC, FM_TABLE_FILENAME, sizeof(FM_FreeSpaceTable_t)
};

/*
** FM file system free space table data
**
** -- table entries must be enabled or disabled or unused
**
** -- enabled table entries may be disabled by command
** -- disabled table entries may be enabled by command
** -- unused table entries may not be modified by command
**
** -- enabled or disabled entries must have a valid file system name
**
** -- the file system name for unused entries is ignored
*/
FM_FreeSpaceTable_t FM_FreeSpaceTable =
{
  {
    {                                   /* - 0 - */
        FM_TABLE_ENTRY_ENABLED,         /* Entry state (enabled, disabled, unused) */
        "/ram",                         /* File system name (logical mount point) */
    },
    {                                   /* - 1 - */
        FM_TABLE_ENTRY_DISABLED,        /* Entry state (enabled, disabled, unused) */
        "/boot",                        /* File system name (logical mount point) */
    },
    {                                   /* - 2 - */
        FM_TABLE_ENTRY_DISABLED,        /* Entry state (enabled, disabled, unused) */
        "/alt",                         /* File system name (logical mount point) */
    },
    {                                   /* - 3 - */
        FM_TABLE_ENTRY_UNUSED,          /* Entry state (enabled, disabled, unused) */
        "",                             /* File system name (logical mount point) */
    },
    {                                   /* - 4 - */
        FM_TABLE_ENTRY_UNUSED,          /* Entry state (enabled, disabled, unused) */
        "",                             /* File system name (logical mount point) */
    },
    {                                   /* - 5 - */
        FM_TABLE_ENTRY_UNUSED,          /* Entry state (enabled, disabled, unused) */
        "",                             /* File system name (logical mount point) */
    },
    {                                   /* - 6 - */
        FM_TABLE_ENTRY_UNUSED,          /* Entry state (enabled, disabled, unused) */
        "",                             /* File system name (logical mount point) */
    },
    {                                   /* - 7 - */
        FM_TABLE_ENTRY_UNUSED,          /* Entry state (enabled, disabled, unused) */
        "",                             /* File system name (logical mount point) */
    },
  },
};

/************************/
/*  End of File Comment */
/************************/
