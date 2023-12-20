/*
** Filename: fm_freespace.c 
**
** NASA Docket No. GSC-18,475-1, identified as “Core Flight Software System (CFS)
** File Manager Application Version 2.5.3
**
** Copyright © 2020 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
**  
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
*
** Title: File Manager (FM) File System Free Space Table Data
**
** Purpose: Default table contents
**
** Notes:
**
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
        "/data",                        /* File system name (logical mount point) */
    },
    {                                   /* - 1 - */
        FM_TABLE_ENTRY_ENABLED,         /* Entry state (enabled, disabled, unused) */
        "/ram",                         /* File system name (logical mount point) */
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
