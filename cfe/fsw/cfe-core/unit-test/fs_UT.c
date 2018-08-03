/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    fss_UT.c
**
** Purpose:
**    File Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:21:47GMT-05:00 $
** $Revision: 1.9 $
**
*/

/*
** Includes
*/
#include "fs_UT.h"
#include "cfe_fs_decompress.h"

/*
** External global variables
*/
extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t OSlseekRtn;
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t OSReadRtn;
extern UT_SetRtn_t OSReadRtn2;
extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t WriteSysLogRtn;

static CFE_FS_Decompress_State_t UT_FS_Decompress_State;


/*
** Functions
*/
void OS_Application_Startup(void)
{
    /* Initialize unit test */
    UT_Init("fs");
    UT_Text("cFE FS Unit Test Output File\n\n");

    /* Perform tests */
    UT_ADD_TEST(Test_CFE_FS_InitHeader);
    UT_ADD_TEST(Test_CFE_FS_ReadHeader);
    UT_ADD_TEST(Test_CFE_FS_WriteHeader);
    UT_ADD_TEST(Test_CFE_FS_SetTimestamp);
    UT_ADD_TEST(Test_CFE_FS_ByteSwapCFEHeader);
    UT_ADD_TEST(Test_CFE_FS_ByteSwapUint32);
    UT_ADD_TEST(Test_CFE_FS_IsGzFile);
    UT_ADD_TEST(Test_CFE_FS_ExtractFileNameFromPath);
    UT_ADD_TEST(Test_CFE_FS_Private);
    UT_ADD_TEST(Test_CFE_FS_Decompress);
}

/*
** Test FS API initialize header function
*/
void Test_CFE_FS_InitHeader(void)
{
    CFE_FS_Header_t Hdr;

    /* Test initializing the header */
    UT_InitData();
    CFE_FS_InitHeader(&Hdr, "description", 123);
    UT_Report(__FILE__, __LINE__,
    		  Hdr.SubType = 123,
              "CFE_FS_InitHeader",
              "Initialize header - successful");
}

/*
** Test FS API read header function
*/
void Test_CFE_FS_ReadHeader(void)
{
    int32 FileDes = 0;
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Read Header\n");
#endif

    /* Test reading the header with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ReadHeader(&Hdr, FileDes) != OS_LSEEK_FAIL,
              "CFE_FS_ReadHeader",
              "Header read lseek failed");

    /* Test successfully reading the header */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, OS_FS_SUCCESS, 1);
    UT_SetOSFail(OS_READ_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ReadHeader(&Hdr, FileDes) != sizeof(CFE_FS_Header_t),
              "CFE_FS_ReadHeader",
              "Header read - successful");
}

/*
** Test FS API write header function
*/
void Test_CFE_FS_WriteHeader(void)
{
    int32 FileDes = 0;
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Write Header\n");
#endif

    /* Test writing the header with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_WriteHeader(FileDes, &Hdr) != OS_LSEEK_FAIL,
              "CFE_FS_WriteHeader",
              "Header write lseek failed");

    /* Test successfully writing the header */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, OS_FS_SUCCESS, 1);
    UT_SetRtnCode(&FileWriteRtn, OS_FS_SUCCESS, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_WriteHeader(FileDes, &Hdr) == OS_FS_SUCCESS,
              "CFE_FS_WriteHeader",
              "Header write - successful");
}

/*
** Test FS API set time stamp function
*/
void Test_CFE_FS_SetTimestamp(void)
{
    int32 FileDes = 0;
    CFE_TIME_SysTime_t NewTimestamp = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Set Time Stamp\n");
#endif

    /* Test setting the time stamp with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_FS_ERROR,
              "CFE_FS_SetTimestamp",
              "Failed to lseek time fields");

    /* Test setting the time stamp with a seconds write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) != OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write seconds");

    /* Test setting the time stamp with a subeconds write failure */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, 0, 2);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write subseconds");

    /* Test successfully setting the time stamp */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_FS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Write time stamp - successful");
}

/*
** Test FS API byte swap cFE header function
*/
void Test_CFE_FS_ByteSwapCFEHeader(void)
{
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Byte Swap cFE Header\n");
#endif

    UT_InitData();
    Hdr.ContentType = 0x11223344;
    Hdr.SubType = 0x22334455;
    Hdr.Length = 0x33445566;
    Hdr.SpacecraftID = 0x44556677;
    Hdr.ProcessorID = 0x55667788;
    Hdr.ApplicationID = 0x66778899;
    Hdr.TimeSeconds = 0x778899aa;
    Hdr.TimeSubSeconds = 0x8899aabb;

    /* Test byte-swapping the header values */
    CFE_FS_ByteSwapCFEHeader(&Hdr);
    UT_Report(__FILE__, __LINE__,
              Hdr.ContentType == 0x44332211 && Hdr.SubType == 0x55443322 &&
              Hdr.Length == 0x66554433 && Hdr.SpacecraftID == 0x77665544 &&
              Hdr.ProcessorID == 0x88776655 &&
              Hdr.ApplicationID == 0x99887766 &&
              Hdr.TimeSeconds == 0xaa998877 &&
              Hdr.TimeSubSeconds == 0xbbaa9988,
              "CFE_FS_ByteSwapUint32",
              "Byte swap header - successful");
}

/*
** Test FS API byte swap uint32 function
*/
void Test_CFE_FS_ByteSwapUint32(void)
{
    uint32 test = 0x11223344;
    uint32 *testptr = &test;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Byte Swap uint32\n");
#endif

    /* Test byte-swapping a uint32 value */
    UT_InitData();
    CFE_FS_ByteSwapUint32(testptr);
    UT_Report(__FILE__, __LINE__,
              test == 0x44332211, "CFE_FS_ByteSwapUint32",
              "Byte swap - successful");
}

/*
** Test FS API is .gz file function
*/
void Test_CFE_FS_IsGzFile(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Is .gz File\n");
#endif

    /* Test if file name ends in .gz with the file name too short */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile("a") == FALSE,
              "CFE_FS_IsGzFile",
              "File name too short");

    /* Test if file name ',gz' extension is missing */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile("Normal_gz") == FALSE,
              "CFE_FS_IsGzFile",
              "File name missing .gz extension 1");

    /* Test if file name ends in .gz with no file name */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile(NULL) == FALSE,
              "CFE_FS_IsGzFile",
              "Null file name");

    /* Test a valid file name ending in .gz */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile("tar.gz") == TRUE,
              "CFE_FS_IsGzFile",
              ".gz file name check - successful");

    /* Test if file name ',gz' extension is missing */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile("Normal._z") == FALSE,
              "CFE_FS_IsGzFile",
              "File name missing .gz extension 2");

    /* Test if file name ',gz' extension is missing */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_IsGzFile("Normal.g_") == FALSE,
              "CFE_FS_IsGzFile",
              "File name missing .gz extension 3");
}

/*
** Test FS API write extract file name from path function
*/
void Test_CFE_FS_ExtractFileNameFromPath(void)
{
    char Original[OS_MAX_PATH_LEN];
    char FileName[OS_MAX_PATH_LEN];
    char LongFileName[OS_MAX_PATH_LEN + 7];
    int j;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Extract File Name from Path\n");
#endif

    /* Test extracting the file name from a path/file name that's
     * missing the path
     */
    UT_InitData();
    strncpy(Original, "/cf/appslibrary.gz", OS_MAX_PATH_LEN);
    Original[OS_MAX_PATH_LEN - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath("name", FileName) ==
                  CFE_FS_INVALID_PATH,
              "CFE_FS_ExtractFilenameFromPath",
              "Missing file path");

    /* Test extracting the file name from a path that's null */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(NULL, FileName) ==
                  CFE_FS_BAD_ARGUMENT,
              "CFE_FS_ExtractFilenameFromPath",
              "Null name");

    /* Test extracting the file name from a path/file name that's too long */
    UT_InitData();
    strcpy(LongFileName, "ut/");

    for (j = 0; j < OS_MAX_PATH_LEN; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(LongFileName, FileName) ==
                CFE_FS_FNAME_TOO_LONG,
              "CFE_FS_ExtractFilenameFromPath",
              "Path/file name too long");

    /* Test successfully extracting the file name from a path/file name */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(Original, FileName) ==
                CFE_SUCCESS,
              "CFE_FS_ExtractFilenameFromPath",
              "Extract path name - successful");

    /* Test extracting the file name from a file name that's null */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(Original, NULL) ==
                  CFE_FS_BAD_ARGUMENT,
              "CFE_FS_ExtractFilenameFromPath",
              "Null file name");
}

/*
** Tests for FS private functions (cfe_fs_priv.c)
*/
void Test_CFE_FS_Private(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Private\n");
#endif

    /* Test successful FS initialization */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_EarlyInit() == CFE_SUCCESS,
              "CFE_FS_EarlyInit",
              "FS initialization - successful");

    /* Test FS initialization with a mutex creation failure */
    UT_InitData();
    UT_SetOSFail(OS_MUTCREATE_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_EarlyInit() == -1,
              "CFE_FS_EarlyInit",
              "Mutex creation failure");

    /* Test successful locking of shared data */
    UT_InitData();
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == -1 && WriteSysLogRtn.count == 0,
              "CFE_FS_LockSharedData",
              "Lock shared data - successful");

    /* Test locking of shared data with a mutex take error */
    UT_InitData();
    UT_SetRtnCode(&MutSemTakeRtn, -1, 1);
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == FS_SYSLOG_OFFSET + 1 &&
              WriteSysLogRtn.count == 1,
              "CFE_FS_LockSharedData",
              "Shared data mutex take error");

    /* Test successful unlocking of shared data */
    UT_InitData();
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == -1 && WriteSysLogRtn.count == 0,
              "CFE_FS_UnlockSharedData",
              "Unlock shared data - successful");

    /* Test unlocking of shared data with a mutex give error */
    UT_InitData();
    UT_SetRtnCode(&MutSemGiveRtn, -1, 1);
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              WriteSysLogRtn.value == FS_SYSLOG_OFFSET + 2 &&
              WriteSysLogRtn.count == 1,
              "CFE_FS_UnlockSharedData",
              "SharedData mutex give error");

#ifdef UT_VERBOSE
    UT_Text("End Test Private\n\n");
#endif
}

/*
** Tests for FS decompress functions (cfe_fs_decompress.c)
*/
void Test_CFE_FS_Decompress(void)
{
    int NumBytes = 35400;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Decompress\n");
#endif

    /* Test file decompression with a file open failure */
    UT_InitData();
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_Decompress("Filename.gz", "Output") ==
                CFE_FS_GZIP_OPEN_INPUT,
              "CFE_FS_Decompress",
              "Open failed");

    /* Test file decompression with a file create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_Decompress_Reentrant(&UT_FS_Decompress_State,
                                          "Filename.gz", "Output") ==
                CFE_FS_GZIP_OPEN_OUTPUT,
              "CFE_FS_Decompress",
              "Create failed");

    /* Test successful file decompression using local buffered data
     * instead of real file I/O
     */
    UT_InitData();
    UT_SetReadBuffer(fs_gz_test, NumBytes);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_Decompress_Reentrant(&UT_FS_Decompress_State,
                                          "fs_test.gz", "Output") ==
                CFE_SUCCESS,
              "CFE_FS_Decompress",
              "Decompress file - successful");

    /* Test file decompression with a non-existent file */
    UT_InitData();
    UT_SetReadBuffer(&fs_gz_test[1], NumBytes - 1);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_Decompress_Reentrant(&UT_FS_Decompress_State,
                                          "fake", "Output") ==
                CFE_FS_GZIP_NON_ZIP_FILE,
              "CFE_FS_Decompress",
              "File does not exist");

    /* Test successful decompression of an inflated type 1 (fixed Huffman
     * codes) block
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              FS_gz_inflate_fixed_Reentrant(&UT_FS_Decompress_State) ==
                CFE_SUCCESS,
              "FS_gz_inflate_fixed",
              "Inflated type 1 decompression - successful");

    /* Test decompressing an inflated type 0 (stored) block */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              FS_gz_inflate_stored_Reentrant(&UT_FS_Decompress_State) !=
                CFE_SUCCESS,
              "FS_gz_inflate_stored",
              "Failed");

    /* Test filling the input buffer with a FS error  */
    UT_InitData();
    UT_SetRtnCode(&OSReadRtn, 4, 1);
    UT_SetRtnCode(&OSReadRtn2, OS_FS_ERROR, 1);
    UT_Report(__FILE__, __LINE__,
              FS_gz_fill_inbuf_Reentrant(&UT_FS_Decompress_State) == EOF,
              "FS_gz_fill_inbuf",
              "gzip read error");

    /* Test writing the output window and updating the CRC */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, -1, 1);
    UT_FS_Decompress_State.outcnt = 435;
    FS_gz_flush_window_Reentrant(&UT_FS_Decompress_State);
    UT_Report(__FILE__, __LINE__,
              UT_FS_Decompress_State.outcnt == 0 &&
              UT_FS_Decompress_State.Error == CFE_FS_GZIP_WRITE_ERROR,
              "FS_gz_flush_window",
              "Write output - successful");

#ifdef UT_VERBOSE
    UT_Text("End Test Decompress\n\n");
#endif
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
}
