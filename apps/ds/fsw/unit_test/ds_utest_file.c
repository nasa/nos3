/************************************************************************
**
** $Id: ds_utest_file.c 1.4.1.1 2015/02/28 17:13:46EST sstrege Exp  $
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
**   Unit test for CFS Data Storage (DS) application source file "ds_file.c"
**
**   To direct text output to screen, 
**      enable '#define UTF_USE_STDOUT' in utf_custom.h
**
**   To direct text output to file, 
**      disable '#define UTF_USE_STDOUT' in utf_custom.h
** 
** $Log: ds_utest_file.c  $
** Revision 1.4.1.1 2015/02/28 17:13:46EST sstrege 
** Added copyright information
** Revision 1.4 2009/12/07 13:40:38EST lwalling 
** Update DS unit tests, add unit test results files to MKS
** Revision 1.3 2009/08/27 16:32:27EDT lwalling 
** Updates from source code review
** Revision 1.2 2009/08/13 10:01:27EDT lwalling 
** Updates to unit test source files
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

#define GOOD_FILTER_ENTRY  (DS_PACKETS_IN_FILTER_TABLE - 1)
#define GOOD_PARMS_ENTRY   (DS_FILTERS_PER_PACKET - 1)
#define GOOD_FILE_ENTRY    (DS_DEST_FILE_CNT - 1)
#define BAD_FILE_ENTRY     (DS_DEST_FILE_CNT - 3)

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
** Local function prototypes
*************************************************************************/


/************************************************************************
** Local data
*************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Unit tests for source file ds_file.c                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Test_file(void)
{
    uint16 Counter16;
    uint32 Counter32;
    int i;

    DS_HkPacket_t HkPacket;
    char DataBuffer[128];
    char Sequence[32];

    uint32 TestCount = 0;
    uint32 FailCount = 0;

    /*
    ** Initialize local buffers and test packet...
    */
    memset(Sequence, 0, sizeof(Sequence));
    memset(DataBuffer, 0, sizeof(DataBuffer));
    CFE_SB_InitMsg(&HkPacket, DS_HK_TLM_MID, sizeof(DS_HkPacket_t), TRUE);

    /*
    ** Initialize packet filter and destination file tables...
    */
    memset(&UT_FilterTbl, 0, sizeof(UT_FilterTbl));
    memset(&UT_DestFileTbl, 0, sizeof(UT_DestFileTbl));

    /*
    ** Initialize file status data...
    */
    for (i = 0; i < DS_DEST_FILE_CNT; i++)
    {
        memset(&DS_AppData.FileStatus[i], 0, sizeof(DS_AppFileStatus_t));
        DS_AppData.FileStatus[i].FileHandle = DS_CLOSED_FILE_HANDLE;
    }

    /*
    ** Create one good entry in the packet filter table...
    */
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].MessageID = DS_HK_TLM_MID;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].FileTableIndex = GOOD_FILE_ENTRY;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].FilterType = DS_BY_COUNT;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_N = 1;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_X = 2;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_O = 0;    

    /*
    ** Create one good entry in the destination file table...
    */
    strcpy(UT_DestFileTbl.File[GOOD_FILE_ENTRY].Pathname, "/tt/");
    strcpy(UT_DestFileTbl.File[GOOD_FILE_ENTRY].Basename, "b_");
    strcpy(UT_DestFileTbl.File[GOOD_FILE_ENTRY].Extension, ".x");
    UT_DestFileTbl.File[GOOD_FILE_ENTRY].FileNameType = DS_BY_COUNT;
    UT_DestFileTbl.File[GOOD_FILE_ENTRY].MaxFileSize = 5000;
    UT_DestFileTbl.File[GOOD_FILE_ENTRY].MaxFileAge = 5000;
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileCount = DS_MAX_SEQUENCE_COUNT;
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileAge = 100;
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState = DS_ENABLED;

    DS_AppData.FilterTblPtr = &UT_FilterTbl;
    DS_AppData.DestFileTblPtr = &UT_DestFileTbl;
 
    /*
    ** An open data storage file is required for this group of unit tests...
    **
    ** Also - this is test (2) for function DS_FileCreateDest() (see below)
    */
    DS_FileCreateDest(GOOD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState == DS_DISABLED)
    {
        UTF_put_text("DS_FileCreateDest() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileStorePacket()...
    **
    **   (1)  MessageID = not found
    **   (2)  MessageID = found, file = disabled
    **   (3)  MessageID = found, file = enabled, packet = filtered
    **   (4)  MessageID = found, file = enabled, packet = not filtered
    */

    /* (1) MessageID = not found */
    Counter32 = DS_AppData.IgnoredPktCounter;
    DS_FileStorePacket(DS_SEND_HK_MID, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (Counter32 == DS_AppData.IgnoredPktCounter)
    {
        UTF_put_text("DS_FileStorePacket() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) MessageID = found, file = disabled */
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState = DS_DISABLED;
    Counter32 = DS_AppData.FilteredPktCounter;
    DS_FileStorePacket(DS_HK_TLM_MID, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (Counter32 == DS_AppData.FilteredPktCounter)
    {
        UTF_put_text("DS_FileStorePacket() -- test failed (2)\n");
        FailCount++;
    }
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState = DS_ENABLED;

    /* (3) MessageID = found, file = enabled, packet = filtered */
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_N = 1;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_X = 5;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_O = 4;    
    Counter32 = DS_AppData.FilteredPktCounter;
    DS_FileStorePacket(DS_HK_TLM_MID, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (Counter32 == DS_AppData.FilteredPktCounter)
    {
        UTF_put_text("DS_FileStorePacket() -- test failed (3)\n");
        FailCount++;
    }
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_N = 1;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_X = 2;    
    UT_FilterTbl.Packet[GOOD_FILTER_ENTRY].Filter[GOOD_PARMS_ENTRY].Algorithm_O = 0;    

    /* (4) MessageID = found, file = enabled, packet = not filtered */
    Counter32 = DS_AppData.PassedPktCounter;
    DS_FileStorePacket(DS_HK_TLM_MID, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (Counter32 == DS_AppData.PassedPktCounter)
    {
        UTF_put_text("DS_FileStorePacket() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileSetupWrite()...
    **
    **   (1)  file = closed, create = error
    **   (2)  file = open, size = re-open, create = ok
    **   (3)  file = open, size = ok
    */

    /* (1) file = closed, create = error */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, "/tt/");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, "basename_");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Extension, ".ext");
    UT_DestFileTbl.File[BAD_FILE_ENTRY].FileNameType = DS_BY_COUNT;
    UT_DestFileTbl.File[BAD_FILE_ENTRY].EnableState = DS_BY_COUNT;
    UT_DestFileTbl.File[BAD_FILE_ENTRY].MaxFileSize = 5000;
    UT_DestFileTbl.File[BAD_FILE_ENTRY].MaxFileAge = 5000;
    UT_DestFileTbl.File[BAD_FILE_ENTRY].SequenceCount = 5000;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = DS_CLOSED_FILE_HANDLE;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState = DS_ENABLED;
    DS_FileSetupWrite(BAD_FILE_ENTRY, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState != DS_DISABLED)
    {
        UTF_put_text("DS_FileSetupWrite() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) file = open, size = re-open, create = ok */
    DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileSize =
       UT_DestFileTbl.File[GOOD_FILE_ENTRY].MaxFileSize - (sizeof(DS_HkPacket_t) / 2);
    DS_FileSetupWrite(GOOD_FILE_ENTRY, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState == DS_DISABLED)
    {
        UTF_put_text("DS_FileSetupWrite() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) file = open, size = ok */
    DS_FileSetupWrite(GOOD_FILE_ENTRY, (CFE_SB_MsgPtr_t) &HkPacket);
    TestCount++;
    if (DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState == DS_DISABLED)
    {
        UTF_put_text("DS_FileSetupWrite() -- test failed (3)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileWriteData()...
    **
    **   (1)  call to OS_write() = error
    **   (2)  call to OS_write() = ok
    */

    /* (1) call to OS_write() = error */
    memset(DataBuffer, 'D', sizeof(DataBuffer));
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileSize = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState = DS_ENABLED;
    strcpy(DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName, "testname.ext");
    DS_FileWriteData(BAD_FILE_ENTRY, DataBuffer, sizeof(DataBuffer));
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState != DS_DISABLED)
    {
        UTF_put_text("DS_FileWriteData() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) call to OS_write() = ok */
    memset(DataBuffer, 'D', sizeof(DataBuffer));
    DS_FileWriteData(GOOD_FILE_ENTRY, DataBuffer, sizeof(DataBuffer));
    TestCount++;
    if (DS_AppData.FileStatus[GOOD_FILE_ENTRY].FileState == DS_DISABLED)
    {
        UTF_put_text("DS_FileWriteData() -- test failed (2)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileWriteHeader()...
    **
    **   (1)  call to CFE_FS_WriteHeader() = error
    **   (2)  call to CFE_FS_WriteHeader() = ok, call to OS_write() = error
    **   (3)  call to CFE_FS_WriteHeader() = ok, call to OS_write() = ok
    **
    **  Test (2) cannot be done at the present time
    **  Test (3) is part of setup at top of page - in DS_FileCreateDest()
    */

    /* (1) call to CFE_FS_WriteHeader() = error */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileSize = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState = DS_ENABLED;
    strcpy(DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName, "testname.ext");
    DS_FileWriteHeader(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState != DS_DISABLED)
    {
        UTF_put_text("CFE_FS_WriteHeader() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileWriteError()...
    **
    **   (1)  function has only 1 execution path
    */

    /* (1) function has only 1 execution path */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileSize = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState = DS_ENABLED;
    strcpy(DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName, "testname.ext");
    DS_FileWriteError(BAD_FILE_ENTRY, 456, 789);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState != DS_DISABLED)
    {
        UTF_put_text("DS_FileWriteError() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileCreateDest()...
    **
    **   (1)  filename portion of qualified filename is too long
    **   (2)  filename portion of qualified filename is ok
    **
    **  Test (2) is performed during setup at top of page
    */

    /* (1) filename portion of qualified filename is too long */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, "/tt/");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, "basename_");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Extension, ".ext");
    UT_DestFileTbl.File[BAD_FILE_ENTRY].FileNameType = DS_BY_COUNT;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileCount = 123;
    DS_FileCreateDest(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName[0] != DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateDest() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileCreateName()...
    **
    **   (1)  add path separator to pathname, pathname is too long
    **   (2)  pathname plus basename plus sequence is too long
    **   (3)  pathname plus basename plus sequence, add dot, plus extension is too long
    **   (4)  pathname plus basename plus sequence, have dot, plus extension is ok
    */

    /* (1) add path separator to pathname, pathname plus basename is too long */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, 'P', 40);
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, 'B', 40);
    DS_FileCreateName(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName[0] != DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateName() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) pathname plus basename plus sequence is too long */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));

    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, 'P', 30);
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, 'B', 30);
    UT_DestFileTbl.File[BAD_FILE_ENTRY].FileNameType = DS_BY_TIME;
    DS_FileCreateName(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName[0] != DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateName() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) pathname plus basename plus sequence, add dot, plus extension is too long */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, 'P', 24);
    UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname[24] = DS_PATH_SEPARATOR;
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, 'B', 25);
    memset(UT_DestFileTbl.File[BAD_FILE_ENTRY].Extension, 'E', 3);
    UT_DestFileTbl.File[BAD_FILE_ENTRY].FileNameType = DS_BY_TIME;
    DS_FileCreateName(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName[0] != DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateName() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) pathname plus basename plus sequence, have dot, plus extension is ok */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Pathname, "disk/path/");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Basename, "base_");
    strcpy(UT_DestFileTbl.File[BAD_FILE_ENTRY].Extension, ".ext");
    UT_DestFileTbl.File[BAD_FILE_ENTRY].FileNameType = DS_BY_TIME;
    DS_FileCreateName(BAD_FILE_ENTRY);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName[0] == DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateName() -- test failed (4)\n");
        FailCount++;
    }
    else
    {
        UTF_put_text("DS_FileCreateName() -- %s (4)\n",
        DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName);
    }


    /*
    ** Tests for function DS_FileCreateSequence()...
    **
    **   (1)  sequence type = DS_UNUSED (invalid)
    **   (2)  sequence type = DS_BY_TIME (valid)
    **   (3)  sequence type = DS_BY_COUNT (valid)
    */

    /* (1) sequence derived from current time */
    DS_FileCreateSequence(Sequence, DS_UNUSED, 12345);
    TestCount++;
    if (Sequence[0] != DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateSequence() -- test failed (1)\n");
        FailCount++;
    }

    /* (2) sequence derived from current time */
    DS_FileCreateSequence(Sequence, DS_BY_TIME, 12345);
    TestCount++;
    if (Sequence[0] == DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateSequence() -- test failed (2)\n");
        FailCount++;
    }
    else
    {
        UTF_put_text("DS_FileCreateSequence() -- %s (2)\n", Sequence);
    }

    /* (3) sequence derived from packet sequence count */
    DS_FileCreateSequence(Sequence, DS_BY_COUNT, 12345);
    TestCount++;
    if (Sequence[0] == DS_STRING_TERMINATOR)
    {
        UTF_put_text("DS_FileCreateSequence() -- test failed (3)\n");
        FailCount++;
    }
    else
    {
        UTF_put_text("DS_FileCreateSequence() -- %s (3)\n", Sequence);
    }


    /*
    ** Tests for function DS_FileUpdateHeader()...
    **
    **   (1)  call to OS_lseek() = error
    **   (2)  call to OS_lseek() = ok, call to OS_write() = error
    **   (3)  call to OS_lseek() = ok, call to OS_write() = ok
    **
    **  Test (2) cannot be done at the present time
    **  Test (3) is part of cleanup at bottom of page - before DS_FileCloseDest()
    */

    /* (1) call to OS_lseek() = error */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileSize = 123;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileState = DS_ENABLED;
    strcpy(DS_AppData.FileStatus[BAD_FILE_ENTRY].FileName, "testname.ext");
    Counter16 = DS_AppData.FileUpdateErrCounter;
    DS_FileUpdateHeader(BAD_FILE_ENTRY);
    TestCount++;
    if (Counter16 == DS_AppData.FileUpdateErrCounter)
    {
        UTF_put_text("DS_FileUpdateHeader() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileCloseDest()...
    **
    **   (1)  function has only 1 execution path
    */

    /* (1) function has only 1 execution path */
    memset(&DS_AppData.FileStatus[BAD_FILE_ENTRY], 0, sizeof(DS_AppFileStatus_t));
    memset(&UT_DestFileTbl.File[BAD_FILE_ENTRY], 0, sizeof(DS_DestFileEntry_t));
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 2000;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 99;
    DS_FileCloseDest(BAD_FILE_ENTRY);
    TestCount++;
    if ((DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge == 2000) ||
        (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle == 99))
    {
        UTF_put_text("DS_FileCloseDest() -- test failed (1)\n");
        FailCount++;
    }


    /*
    ** Tests for function DS_FileTestAge()...
    **
    **   (1)  table not loaded (function does nothing)
    **   (2)  table loaded, file not open (function does nothing)
    **   (3)  table loaded, file open, file not too old (function does nothing)
    **   (4)  table loaded, file open, file too old (function closes file)
    */

    /* (1) file table not loaded (function does nothing) */
    DS_AppData.DestFileTblPtr = (DS_DestFileTable_t *) NULL;
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 4000;
    DS_FileTestAge(5);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge != 4000)
    {
        UTF_put_text("DS_FileTestAge() -- test failed (1)\n");
        FailCount++;
    }
    DS_AppData.DestFileTblPtr = &UT_DestFileTbl;

    /* (2) table loaded, file not open (function does nothing) */
    DS_FileTestAge(5);
    TestCount++;
    if (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge != 4000)
    {
        UTF_put_text("DS_FileTestAge() -- test failed (2)\n");
        FailCount++;
    }

    /* (3) table loaded, file open, file not too old (function does nothing) */
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle = 99;
    UT_DestFileTbl.File[BAD_FILE_ENTRY].MaxFileAge = 5000;
    DS_FileTestAge(5);
    TestCount++;
    if ((DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge != 4005) ||
        (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle != 99))
    {
        UTF_put_text("DS_FileTestAge() -- test failed (3)\n");
        FailCount++;
    }

    /* (4) table loaded, file open, file too old (function closes file) */
    DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge = 4996;
    DS_FileTestAge(5);
    TestCount++;
    if ((DS_AppData.FileStatus[BAD_FILE_ENTRY].FileAge != 0) ||
        (DS_AppData.FileStatus[BAD_FILE_ENTRY].FileHandle != DS_CLOSED_FILE_HANDLE))
    {
        UTF_put_text("DS_FileTestAge() -- test failed (4)\n");
        FailCount++;
    }


    /*
    ** Close data storage file opened for this group of unit tests...
    */
    DS_FileUpdateHeader(GOOD_FILE_ENTRY);
    DS_FileCloseDest(GOOD_FILE_ENTRY);


    DS_AppData.FilterTblPtr = DS_FilterTblPtr;
    DS_AppData.DestFileTblPtr = DS_DestFileTblPtr;


    /*
    ** Summary for this group of unit tests...
    */
    UTF_put_text("ds_file.c -- test count = %d, test errors = %d\n", TestCount, FailCount);

    UT_TotalTestCount += TestCount;
    UT_TotalFailCount += FailCount;

    return;

} /* End of Test_file() */
