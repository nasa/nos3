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
**    fs_UT.h
**
** Purpose:
**    FS unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:23:28GMT-05:00 $
** $Revision: 1.1 $
*/
#ifndef _fs_UT_h_
#define _fs_UT_h_

/*
** Includes
*/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cfe.h"
#include "cfe_fs.h"
#include "cfe_fs_priv.h"
#include "common_types.h"
#include "ut_stubs.h"
#include "ut_fs_zipfile.h"

/* FS unit test functions */
/*****************************************************************************/
/**
** \brief Master function for the FS API test functions
**
** \par Description
**        This function calls the FS API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void Test_CFE_FS_API(void);

/*****************************************************************************/
/**
** \brief Test FS API initialize header function
**
** \par Description
**        This function tests the initialize header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Report, #CFE_FS_InitHeader
**
******************************************************************************/
void Test_CFE_FS_InitHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API read header function
**
** \par Description
**        This function tests the read header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_ReadHeader,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_ReadHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API write header function
**
** \par Description
**        This function tests the write header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_WriteHeader,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_WriteHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API set time stamp function
**
** \par Description
**        This function tests the set time stamp function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_SetTimestamp,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_SetTimestamp(void);

/*****************************************************************************/
/**
** \brief Test FS API byte swap cFE header function
**
** \par Description
**        This function tests the byte swap cFE header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_FS_ByteSwapCFEHeader, #UT_Report
**
******************************************************************************/
void Test_CFE_FS_ByteSwapCFEHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API byte swap uint32 function
**
** \par Description
**        This function tests the byte swap uint32 function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_FS_ByteSwapUint32, #UT_Report
**
******************************************************************************/
void Test_CFE_FS_ByteSwapUint32(void);

/*****************************************************************************/
/**
** \brief Test FS API is .gz file function
**
** \par Description
**        This function tests the "is .gz file" function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_IsGzFile
**
******************************************************************************/
void Test_CFE_FS_IsGzFile(void);

/*****************************************************************************/
/**
** \brief Test FS API write extract file name from path function
**
** \par Description
**        This function tests the "write extract file name from path" function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_ExtractFilenameFromPath
**
******************************************************************************/
void Test_CFE_FS_ExtractFileNameFromPath(void);

/*****************************************************************************/
/**
** \brief Tests for FS private functions (cfe_fs_priv.c)
**
** \par Description
**        This function tests the FS private functions (cfe_fs_priv.c).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_EarlyInit,
** \sa #CFE_FS_LockSharedData, #UT_SetRtnCode, #CFE_FS_UnlockSharedData
**
******************************************************************************/
void Test_CFE_FS_Private(void);

/*****************************************************************************/
/**
** \brief Tests for FS decompress functions (cfe_fs_decompress.c)
**
** \par Description
**        This function tests the FS decompress functions
**        (cfe_fs_decompress.c).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_Decompress,
** \sa #UT_SetReadBuffer, #FS_gz_inflate_fixed, #FS_gz_inflate_stored,
** \sa #FS_gz_fill_inbuf, #FS_gz_flush_window
**
******************************************************************************/
void Test_CFE_FS_Decompress(void);

/*****************************************************************************/
/**
** \brief Unit test specific call to process SB messages
**
** \par Description
**        This function serves as a pass-through for messages coming from the
**        CFE_SB_SendMsg() stub function.  By using a common pass-through
**        function name, the stub can be generic for all of the tests for the
**        various services (i.e., EVS, TBL, etc.).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.  
**
******************************************************************************/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

#endif /* _es_ut_h_ */
