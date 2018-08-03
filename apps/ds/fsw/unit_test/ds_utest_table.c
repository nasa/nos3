/************************************************************************
**
** $Id: ds_utest_table.c 1.7.1.1 2015/02/28 17:13:38EST sstrege Exp  $
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
** Notes: 
**
**   Unit test for CFS Data Storage (DS) application source file "ds_table.c"
**
**   To direct text output to screen, 
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file, 
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
** 
** $Log: ds_utest_table.c  $
** Revision 1.7.1.1 2015/02/28 17:13:38EST sstrege 
** Added copyright information
** Revision 1.7 2009/12/07 13:40:38EST lwalling 
** Update DS unit tests, add unit test results files to MKS
** Revision 1.6 2009/09/01 15:22:09EDT lwalling 
** Reference new CFS Library function
** Revision 1.5 2009/08/31 17:51:36EDT lwalling 
** Convert calls from DS_TableVerifyString() to CFS_VerifyString() with descriptive arg names
** Revision 1.4 2009/08/27 16:32:30EDT lwalling 
** Updates from source code review
** Revision 1.3 2009/08/13 10:01:27EDT lwalling 
** Updates to unit test source files
** Revision 1.2 2009/08/07 16:25:01EDT lwalling 
** Update cmd tests, create table tests, modify makefile
** Revision 1.1 2009/05/26 13:37:49EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/unit_test/project.pj
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

#include "cfs_utils.h"

#include "ds_perfids.h"
#include "ds_msgids.h"
#include "ds_platform_cfg.h"

#include "ds_platform_cfg.h"
#include "ds_verify.h"

#include "ds_appdefs.h"

#include "ds_msg.h"
#include "ds_app.h"
#include "ds_cmds.h"
#include "ds_file.h"
#include "ds_table.h"
#include "ds_events.h"
#include "ds_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

/************************************************************************
** Global data external to this file
*************************************************************************/

extern  DS_AppData_t DS_AppData;   /* DS app global data */

extern  uint32 UT_TotalTestCount;  /* Unit test global data */
extern  uint32 UT_TotalFailCount;

extern  DS_NoopCmd_t        *UT_NoopCmd;
extern  DS_ResetCmd_t       *UT_ResetCmd;
extern  DS_FilterFileCmd_t  *UT_FilterFileCmd;
extern  DS_FilterTypeCmd_t  *UT_FilterTypeCmd;
extern  DS_FilterParmsCmd_t *UT_FilterParmsCmd;
extern  DS_DestTypeCmd_t    *UT_DestTypeCmd;
extern  DS_DestStateCmd_t   *UT_DestStateCmd;
extern  DS_DestPathCmd_t    *UT_DestPathCmd;
extern  DS_DestBaseCmd_t    *UT_DestBaseCmd;
extern  DS_DestExtCmd_t     *UT_DestExtCmd;
extern  DS_DestSizeCmd_t    *UT_DestSizeCmd;
extern  DS_DestAgeCmd_t     *UT_DestAgeCmd;
extern  DS_DestCountCmd_t   *UT_DestCountCmd;
extern  DS_CloseFileCmd_t   *UT_CloseFileCmd;

extern  DS_FilterTable_t     UT_FilterTbl;  
extern  DS_DestFileTable_t   UT_DestFileTbl;

extern  DS_FilterTable_t    *DS_FilterTblPtr;
extern  DS_DestFileTable_t  *DS_DestFileTblPtr;

/************************************************************************
** Local functions
*************************************************************************/

int32 RegisterHook = 0;

int32 CFE_TBL_RegisterHook(CFE_TBL_Handle_t *TblHandlePtr, const char *Name,
                           uint32  Size, uint16  TblOptionFlags,
                           CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    RegisterHook++;

    if ((RegisterHook == 1) || (RegisterHook == 3))
        return (-1);
    else
        return (CFE_SUCCESS);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file ds_table.c                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void Test_table(void)
{
    int32 FilterTableIndex;
    uint32  uint32Result;
    boolean BooleanResult;
    char StringBuffer[4];

    uint32 TestCount = 0;
    uint32 FailCount = 0;


    /*
    ** Tests for function DS_TableUpdateCDS()...
    **
    **   (1)  Table data ptr = NULL, get address = success
    */

    /* (1) invalid table descriptor text */
	UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_COPYTOCDS_PROC, -1);
    DS_AppData.DataStoreHandle = 1;
    DS_TableUpdateCDS();
    TestCount++;
    if (DS_AppData.DataStoreHandle != 0)
    {
        UTF_put_text("DS_TableUpdateCDS() -- test failed (1)\n");
        FailCount++;
    }
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_COPYTOCDS_PROC);


    /*
    ** Initialize packet filter table contents for these tests...
    **
    **   Table entry 0 = not in use (valid)
    **   Table entry 1 = in use (valid)
    **   Table entry 2 = in use (invalid)
    **   Last entry = in use (valid)
    **   Other entries = not in use (valid)
    */
    memset(&UT_FilterTbl, DS_UNUSED, sizeof(UT_FilterTbl));
    UT_FilterTbl.Packet[1].MessageID = DS_CMD_MID;    
    UT_FilterTbl.Packet[2].MessageID = DS_SEND_HK_MID;    
    UT_FilterTbl.Packet[2].Filter[0].FileTableIndex = DS_DEST_FILE_CNT;    
    UT_FilterTbl.Packet[DS_PACKETS_IN_FILTER_TABLE - 1].MessageID = DS_HK_TLM_MID;    

    /*
    ** Initialize destination file table contents for these tests...
    **
    **   Table entry 0 = not in use (valid)
    **   Table entry 1 = in use (valid)
    **   Table entry 2 = in use (invalid)
    **   Other entries = not in use (valid)
    */
    memset(&UT_DestFileTbl, DS_UNUSED, sizeof(UT_DestFileTbl));
    strcpy(UT_DestFileTbl.File[1].Pathname, "ok");
    strcpy(UT_DestFileTbl.File[1].Basename, "ok");
    strcpy(UT_DestFileTbl.File[1].Extension, "ok");
    UT_DestFileTbl.File[1].FileNameType = DS_BY_COUNT;
    UT_DestFileTbl.File[1].EnableState = DS_ENABLED;
    UT_DestFileTbl.File[1].MaxFileSize = DS_FILE_MIN_SIZE_LIMIT;
    UT_DestFileTbl.File[1].MaxFileAge = DS_FILE_MIN_AGE_LIMIT;
    UT_DestFileTbl.File[1].SequenceCount = DS_MAX_SEQUENCE_COUNT;
    strcpy(UT_DestFileTbl.File[2].Pathname, "ok");


    /*
    ** Tests for function DS_TableInit()...
    **
    **   (1)  failure to register Destination File Table
    **   (2)  failure to register Packet Filter Table
    **   (3)  failure to load Destination File Table
    **   (4)  failure to load Packet Filter Table
    **
    **   then calls DS_TableManageDestFile()
    **   then calls DS_TableManageFilter()
    */

    /* Turn on dummy proc that will fake the fail - pass - fail */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, (void *)&CFE_TBL_RegisterHook);
    /* (1) CFE_TBL_Register (file table) will fail */
    DS_TableInit();
    /* (2) CFE_TBL_Register (file table) will succeed, (filter table) will fail */
    DS_TableInit();
    /* Turn off dummy proc that faked the fail - pass - fail */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, NULL);

    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) &UT_DestFileTbl;
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) &UT_FilterTbl;
    UTF_CFE_TBL_Set_Api_Return_Code (CFE_TBL_REGISTER_PROC, CFE_TBL_INFO_RECOVERED_TBL);
    DS_TableInit();

/*
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) &UT_DestFileTbl;
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) &UT_FilterTbl;
    UTF_CFE_TBL_Set_Api_Return_Code (CFE_TBL_GETSTATUS_PROC, CFE_TBL_INFO_DUMP_PENDING);
    DS_TableInit();
	UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_GETSTATUS_PROC);
*/

    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) &UT_DestFileTbl;
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) &UT_FilterTbl;
    UTF_CFE_TBL_Set_Api_Return_Code (CFE_TBL_GETSTATUS_PROC, CFE_TBL_INFO_VALIDATION_PENDING);
    DS_TableInit();
	UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_GETSTATUS_PROC);

/*
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) &UT_DestFileTbl;
    DS_AppData.FilterTblPtr = (DS_FilterTable_t *) &UT_FilterTbl;
    UTF_CFE_TBL_Set_Api_Return_Code (CFE_TBL_GETSTATUS_PROC, CFE_TBL_INFO_UPDATE_PENDING);
    DS_TableInit();
	UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_GETSTATUS_PROC);
*/
	UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_REGISTER_PROC);


    /*
    ** Tests for function DS_TableManageDestFile()...
    **
    **   (1)  Table data ptr = NULL, get address = success
    **   (2)  Table data ptr = ok, get address = update
    **   (3)  Table data ptr = ok, get address = other
    */


    /*
    ** Tests for function DS_TableManageFilter()...
    **
    **   (1)  Table data ptr = NULL, get address = success
    **   (2)  Table data ptr = NULL, get address = update
    **   (3)  Table data ptr = NULL, get address = other
    **   (4)  Table data ptr = ok, get status = validate
    **   (5)  Table data ptr = ok, get status = update, get address = success
    **   (6)  Table data ptr = ok, get status = update, get address = update
    **   (7)  Table data ptr = ok, get status = update, get address = other
    **   (8)  Table data ptr = ok, get status = other
    */


    /*
    ** Tests for function DS_TableVerifyDestFile()...
    **
    **   (1)  invalid table descriptor text
    **   (2)  mix of good, bad and unused table entries
    */

    /* (1) invalid table descriptor text */
    memset(&UT_DestFileTbl.Descriptor, 'A', DS_DESCRIPTOR_BUFSIZE);
    uint32Result = DS_TableVerifyDestFile(&UT_DestFileTbl);
    TestCount++;
    if (uint32Result != DS_TABLE_VERIFY_ERR)
    {
        UTF_put_text("DS_TableVerifyDestFile() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) mix of good, bad and unused table entries */
    memset(&UT_DestFileTbl.Descriptor, DS_UNUSED, DS_DESCRIPTOR_BUFSIZE);
    uint32Result = DS_TableVerifyDestFile(&UT_DestFileTbl);
    TestCount++;
    if (uint32Result != DS_TABLE_VERIFY_ERR)
    {
        UTF_put_text("DS_TableVerifyDestFile() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyDestFileEntry()...
    **
    **   (1)  destination file pathname = invalid
    **   (2)  destination file basename = invalid
    **   (3)  destination file extension = invalid
    **   (4)  destination file filename type = invalid
    **   (5)  destination file enable state = invalid
    **   (6)  destination file max size = invalid
    **   (7)  destination file max age = invalid
    **   (8)  destination file sequence count = invalid
    **   (9)  destination file table entry = valid
    */

    /* (1) destination file pathname = invalid */
    memset(UT_DestFileTbl.File[5].Pathname, 'A', DS_PATHNAME_BUFSIZE);
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) destination file basename = invalid */
    strcpy(UT_DestFileTbl.File[5].Pathname, "ok");
    memset(UT_DestFileTbl.File[5].Basename, 'A', DS_BASENAME_BUFSIZE);
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) destination file extension = invalid */
    strcpy(UT_DestFileTbl.File[5].Basename, "ok");
    memset(UT_DestFileTbl.File[5].Extension, 'A', DS_EXTENSION_BUFSIZE);
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) destination file filename type = invalid */
    strcpy(UT_DestFileTbl.File[5].Extension, "ok");
    UT_DestFileTbl.File[5].FileNameType = DS_UNUSED;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) destination file enable state = invalid */
    UT_DestFileTbl.File[5].FileNameType = DS_BY_COUNT;
    UT_DestFileTbl.File[5].EnableState = 99;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) destination file max size = invalid */
    UT_DestFileTbl.File[5].EnableState = DS_ENABLED;
    UT_DestFileTbl.File[5].MaxFileSize = DS_FILE_MIN_SIZE_LIMIT - 1;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) destination file max age = invalid */
    UT_DestFileTbl.File[5].MaxFileSize = DS_FILE_MIN_SIZE_LIMIT;
    UT_DestFileTbl.File[5].MaxFileAge = DS_FILE_MIN_AGE_LIMIT - 1;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (7)\n");
        FailCount++;
    }

    /* (8) destination file sequence count = invalid */
    UT_DestFileTbl.File[5].MaxFileAge = DS_FILE_MIN_AGE_LIMIT;
    UT_DestFileTbl.File[5].SequenceCount = DS_MAX_SEQUENCE_COUNT + 1;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (8)\n");
        FailCount++;
    }

    /* (9) destination file table entry = valid */
    UT_DestFileTbl.File[5].SequenceCount = DS_MAX_SEQUENCE_COUNT;
    BooleanResult = DS_TableVerifyDestFileEntry(&UT_DestFileTbl.File[5], 5, 1);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyDestFileEntry() -- test failed (9)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyFilter()...
    **
    **   (1)  invalid table descriptor text
    **   (2)  mix of good, bad and unused table entries
    */

    /* (1) invalid table descriptor text */
    memset(&UT_FilterTbl.Descriptor, 'A', DS_DESCRIPTOR_BUFSIZE);
    uint32Result = DS_TableVerifyFilter(&UT_FilterTbl);
    TestCount++;
    if (uint32Result != DS_TABLE_VERIFY_ERR)
    {
        UTF_put_text("DS_TableVerifyFilter() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) mix of good, bad and unused table entries */
    memset(&UT_FilterTbl.Descriptor, DS_UNUSED, DS_DESCRIPTOR_BUFSIZE);
    uint32Result = DS_TableVerifyFilter(&UT_FilterTbl);
    TestCount++;
    if (uint32Result != DS_TABLE_VERIFY_ERR)
    {
        UTF_put_text("DS_TableVerifyFilter() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyFilterEntry()...
    **
    **   (1)  packet entry MID = unused
    **   (2)  packet entry in use - filter entry = unused
    **   (3)  packet entry in use - filter entry - file table index = invalid
    **   (4)  packet entry in use - filter entry - filter type = invalid
    **   (5)  packet entry in use - filter entry - filter parms = invalid
    **   (6)  packet entry in use - filter entry = valid
    */

    /* (1) packet entry MID = unused */
    memset(&UT_FilterTbl.Packet[10], DS_UNUSED, sizeof(DS_PacketEntry_t));
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) packet entry in use - filter entry = unused */
    UT_FilterTbl.Packet[10].MessageID = DS_CMD_MID;    
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) packet entry in use - filter entry - file table index = invalid */
    UT_FilterTbl.Packet[10].Filter[0].FileTableIndex = DS_DEST_FILE_CNT;    
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) packet entry in use - filter entry - filter type = invalid */
    UT_FilterTbl.Packet[10].Filter[0].FileTableIndex = DS_DEST_FILE_CNT - 1;
    UT_FilterTbl.Packet[10].Filter[0].FilterType = DS_UNUSED;
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) packet entry in use - filter entry - filter parms = invalid */
    UT_FilterTbl.Packet[10].Filter[0].FilterType = DS_BY_COUNT;
    UT_FilterTbl.Packet[10].Filter[0].Algorithm_N = 1;
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) packet entry in use - filter entry = valid */
    UT_FilterTbl.Packet[10].Filter[0].Algorithm_N = 0;
    BooleanResult = DS_TableVerifyFilterEntry(&UT_FilterTbl.Packet[10], 10, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyFilterEntry() -- test failed (6)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableEntryUnused()...
    **
    **   (1)  table entry unused
    **   (2)  table entry in use
    */

    /* (1) table entry unused */
    BooleanResult = DS_TableEntryUnused(&UT_FilterTbl.Packet[0], sizeof(DS_PacketEntry_t));
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableEntryUnused() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) table entry in use */
    BooleanResult = DS_TableEntryUnused(&UT_FilterTbl.Packet[1], sizeof(DS_PacketEntry_t));
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableEntryUnused() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyFileIndex()...
    **
    **   (1)  File Table Index < DS_DEST_FILE_CNT (valid)
    **   (2)  File Table Index = DS_DEST_FILE_CNT (invalid)
    **   (3)  File Table Index > DS_DEST_FILE_CNT (invalid)
    */

    /* (1) File Table Index < DS_DEST_FILE_CNT (valid) */
    BooleanResult = DS_TableVerifyFileIndex(DS_DEST_FILE_CNT - 1);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyFileIndex() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) File Table Index = DS_DEST_FILE_CNT (invalid) */
    BooleanResult = DS_TableVerifyFileIndex(DS_DEST_FILE_CNT);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyFileIndex() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) File Table Index > DS_DEST_FILE_CNT (invalid) */
    BooleanResult = DS_TableVerifyFileIndex(DS_DEST_FILE_CNT + 1);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyFileIndex() -- test failed (3)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyParms()...
    **
    **   N = pass this many
    **   X = out of this many
    **   O = starting at this offset
    **
    **   (1)  N is less than X, O is valid
    **   (2)  N is equal to X, O is valid
    **   (3)  N is greater than X, O is valid
    **   (4)  N is valid, O is less than X
    **   (5)  N is valid, O is equal to X
    **   (6)  N is valid, O is greater than X
    **   (7)  N = 0, X = 0, O = 0 (this is valid, it means filter all)
    */

    /* (1) N is less than X, O is valid */
    BooleanResult = DS_TableVerifyParms(1, 2, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) N is equal to X, O is valid */
    BooleanResult = DS_TableVerifyParms(2, 2, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) N is greater than X, O is valid */
    BooleanResult = DS_TableVerifyParms(3, 2, 0);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) N is valid, O is less than X */
    BooleanResult = DS_TableVerifyParms(1, 2, 1);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) N is valid, O is equal to X */
    BooleanResult = DS_TableVerifyParms(1, 2, 2);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (5)\n");
        FailCount++;
    }

    /* (6) N is valid, O is greater than X */
    BooleanResult = DS_TableVerifyParms(1, 2, 3);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (6)\n");
        FailCount++;
    }

    /* (7) N = 0, X = 0, O = 0 (this is valid, it means pass none) */
    BooleanResult = DS_TableVerifyParms(0, 0, 0);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyParms() -- test failed (7)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyType()...
    **
    **   (1)  valid type value (count)
    **   (2)  valid type value (time)
    **   (3)  invalid type value (99)
    */

    /* (1) valid type value (count) */
    BooleanResult = DS_TableVerifyType(DS_BY_COUNT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyType() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) valid type value (time) */
    BooleanResult = DS_TableVerifyType(DS_BY_TIME);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyType() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid type value */
    BooleanResult = DS_TableVerifyType(99);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyType() -- test failed (3)\n");
        FailCount++;
    }


    /*
    ** Tests for function CFS_VerifyString()...
    **
    **   (1)  empty string, string not required
    **   (2)  empty string, string required
    **   (3)  string without terminator
    **   (4)  string with invalid filename chars, char test required
    **   (5)  string with invalid filename chars, char test not required
    **   (6)  string with valid filename chars, char test required
    */

    /* create empty string */
    StringBuffer[0] = DS_STRING_TERMINATOR;
    StringBuffer[1] = DS_STRING_TERMINATOR;
    StringBuffer[2] = DS_STRING_TERMINATOR;
    StringBuffer[3] = DS_STRING_TERMINATOR;

    /* (1) empty string, not required */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_OPTIONAL, DS_DESCRIPTIVE_TEXT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) empty string, required */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_REQUIRED, DS_DESCRIPTIVE_TEXT);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (2)\n");
        FailCount++;
    }

    /* create string without terminator */
    StringBuffer[0] = 'A';
    StringBuffer[1] = 'B';
    StringBuffer[2] = 'C';
    StringBuffer[3] = 'D';

    /* (3) string without terminator */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_OPTIONAL, DS_DESCRIPTIVE_TEXT);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (3)\n");
        FailCount++;
    }

    /* create string with invalid filename characters */
    StringBuffer[0] = 'A';
    StringBuffer[1] = '$';
    StringBuffer[2] = '*';
    StringBuffer[3] = DS_STRING_TERMINATOR;

    /* (4) string with invalid filename chars, char test required */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_OPTIONAL, DS_FILENAME_TEXT);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (4)\n");
        FailCount++;
    }

    /* (5) string with invalid filename chars, char test not required */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_OPTIONAL, DS_DESCRIPTIVE_TEXT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (5)\n");
        FailCount++;
    }

    /* create string with valid filename characters */
    StringBuffer[0] = 'A';
    StringBuffer[1] = '/';
    StringBuffer[2] = 'B';
    StringBuffer[3] = DS_STRING_TERMINATOR;

    /* (6) string with valid filename chars, char test required */
    BooleanResult = CFS_VerifyString(StringBuffer, 4, DS_STRING_OPTIONAL, DS_FILENAME_TEXT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("CFS_VerifyString() -- test failed (6)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyState()...
    **
    **   (1)  valid state value (enabled)
    **   (2)  valid state value (disabled)
    **   (3)  invalid state value (99)
    */

    /* (1) valid state value (enabled) */
    BooleanResult = DS_TableVerifyState(DS_ENABLED);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyState() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) valid state value (disabled) */
    BooleanResult = DS_TableVerifyState(DS_DISABLED);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyState() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) invalid state value */
    BooleanResult = DS_TableVerifyState(99);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyState() -- test failed (3)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifySize()...
    **
    **   (1)  valid file size limit
    **   (2)  invalid file size limit
    */

    /* (1) valid file size limit */
    BooleanResult = DS_TableVerifySize(DS_FILE_MIN_SIZE_LIMIT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifySize() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid file size limit */
    BooleanResult = DS_TableVerifySize(DS_FILE_MIN_SIZE_LIMIT - 1);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifySize() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyAge()...
    **
    **   (1)  valid file age limit
    **   (2)  invalid file age limit
    */

    /* (1) valid file age limit */
    BooleanResult = DS_TableVerifyAge(DS_FILE_MIN_AGE_LIMIT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyAge() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid file age limit */
    BooleanResult = DS_TableVerifyAge(DS_FILE_MIN_AGE_LIMIT - 1);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyAge() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableVerifyCount()...
    **
    **   (1)  valid sequence count
    **   (2)  invalid sequence count
    */

    /* (1) valid sequence count */
    BooleanResult = DS_TableVerifyCount(DS_MAX_SEQUENCE_COUNT);
    TestCount++;
    if (BooleanResult == FALSE)
    {
        UTF_put_text("DS_TableVerifyCount() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) invalid sequence count */
    BooleanResult = DS_TableVerifyCount(DS_MAX_SEQUENCE_COUNT + 1);
    TestCount++;
    if (BooleanResult == TRUE)
    {
        UTF_put_text("DS_TableVerifyCount() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_TableSubscribe()...
    **
    **   (1)  this function has no failure case
    */

    /* (1) this function has no failure case */
    DS_TableSubscribe();
    TestCount++;


    /*
    ** Tests for function DS_TableUnsubscribe()...
    **
    **   (1)  this function has no failure case
    */

    /* (1) this function has no failure case */
    DS_TableUnsubscribe();
    TestCount++;


    /*
    ** Tests for function DS_TableFindMsgID()...
    **
    **   (1)  find message ID
    **       (known to be entry (DS_PACKETS_IN_FILTER_TABLE - 1))
    */

    /* (1) find message ID  */
    FilterTableIndex = DS_TableFindMsgID(DS_HK_TLM_MID);
    TestCount++;
    if (FilterTableIndex != (DS_PACKETS_IN_FILTER_TABLE - 1))
    {
        UTF_put_text("DS_TableFindMsgID() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("ds_table.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_table() */








