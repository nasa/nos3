/************************************************************************
**
** $Id: fm_utest_tbl.c 1.2 2009/12/02 14:31:19EST lwalling Exp  $
**
** Notes:
**
**   Unit test for CFS File Manager (FM) application source file "fm_tbl.c"
**
**   To direct text output to screen,
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file,
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
**
** $Log: fm_utest_tbl.c  $
** Revision 1.2 2009/12/02 14:31:19EST lwalling 
** Update FM unit tests to match UTF changes
** Revision 1.1 2009/11/20 16:12:10EST lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/fm/fsw/unit_test/project.pj
**
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"
#include "utf_cfe.h"

#include "fm_defs.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_tbl.h"
#include "fm_app.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_child.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  FM_FreeSpaceTable_t FM_FreeSpaceTable;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file fm_child.c                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_tbl(void)
{
    int32  iResult;

    uint32 TestCount = 0;
    uint32 FailCount = 0;


    /*
    ** Tests for function FM_TableInit()...
    **
    **   (1)  Valid call to register free space table
    */

    /* (1) Valid call to register free space table */
    FM_GlobalData.FreeSpaceTableHandle = 0;
    iResult = FM_TableInit();
    TestCount++;
    if (iResult != CFE_SUCCESS)
    {
        UTF_put_text("FM_TableInit() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function FM_ValidateTable()...
    **
    **   (1)  Invalid table entry state (999)
    **   (2)  Invalid free space table (zero length disk name)
    **   (3)  Invalid free space table (disk name too long)
    **   (4)  Invalid free space table (invalid name chars)
    **   (5)  Valid free space table (default table)
    */

    /* (1) Invalid table entry state (999) */
    CFE_PSP_MemSet(&FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1], 0, sizeof(FM_TableEntry_t));
    FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].State = 999;
    iResult = FM_ValidateTable(&FM_FreeSpaceTable);
    TestCount++;
    if (iResult != FM_TABLE_VALIDATION_ERR)
    {
        UTF_put_text("FM_ValidateTable() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) Invalid free space table (zero length disk name) */
    CFE_PSP_MemSet(&FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1], 0, sizeof(FM_TableEntry_t));
    FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].State = FM_TABLE_ENTRY_DISABLED;
    iResult = FM_ValidateTable(&FM_FreeSpaceTable);
    TestCount++;
    if (iResult != FM_TABLE_VALIDATION_ERR)
    {
        UTF_put_text("FM_ValidateTable() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) Invalid free space table (disk name too long) */
    CFE_PSP_MemSet(&FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1], 0, sizeof(FM_TableEntry_t));
    FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].State = FM_TABLE_ENTRY_DISABLED;
    CFE_PSP_MemSet(FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].Name, 'a', OS_MAX_PATH_LEN);
    iResult = FM_ValidateTable(&FM_FreeSpaceTable);
    TestCount++;
    if (iResult != FM_TABLE_VALIDATION_ERR)
    {
        UTF_put_text("FM_ValidateTable() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) Invalid free space table (invalid name chars) */
    CFE_PSP_MemSet(&FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1], 0, sizeof(FM_TableEntry_t));
    FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].State = FM_TABLE_ENTRY_DISABLED;
    strcpy(FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1].Name, "~!@#$%^&*()_+");
    iResult = FM_ValidateTable(&FM_FreeSpaceTable);
    TestCount++;
    if (iResult != FM_TABLE_VALIDATION_ERR)
    {
        UTF_put_text("FM_ValidateTable() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) Valid free space table (default table) */
    CFE_PSP_MemSet(&FM_FreeSpaceTable.FileSys[FM_TABLE_ENTRY_COUNT-1], 0, sizeof(FM_TableEntry_t));
    iResult = FM_ValidateTable(&FM_FreeSpaceTable);
    TestCount++;
    if (iResult != CFE_SUCCESS)
    {
        UTF_put_text("FM_ValidateTable() -- test failed (5)\n");
        FailCount++;
    }


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("fm_tbl.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_tbl() */








