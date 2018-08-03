/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software (cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    ut_osprintf_stubs.h
**
** Purpose:
**    OS API unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2012/10/01 18:47:28EDT $
** $Revision: 1.13 $
**
*/
#ifndef _time_UT_h_
#define _time_UT_h_

#define UT_OSP_NO_MESSAGE                 0
#define UT_OSP_CORE_INIT                  11
#define UT_OSP_POR_MAX_PROC_RESETS        12
#define UT_OSP_CORE_RUNTIME               13
#define UT_OSP_CORE_APP_EXIT              14
#define UT_OSP_PROC_RESET_COMMANDED       15
#define UT_OSP_POR_COMMANDED              16
#define UT_OSP_CANNOT_CALL_APP_MAIN       17
#define UT_OSP_GET_APP_ID                 18
#define UT_OSP_MUTEX_TAKE                 19
#define UT_OSP_MUTEX_GIVE                 20
#define UT_OSP_CANNOT_RESTART_APP         21
#define UT_OSP_INSUFF_FREE_SPACE          22
#define UT_OSP_CREATE_VOLATILE            23
#define UT_OSP_INIT_VOLATILE              24
#define UT_OSP_MOUNT_VOLATILE             25
#define UT_OSP_REMOUNT_VOLATILE           26
#define UT_OSP_REFORMAT_VOLATILE          27
#define UT_OSP_REMOVE_VOLATILE            28
#define UT_OSP_UNMOUNT_VOLATILE           29
#define UT_OSP_DETERMINE_BLOCKS           30
#define UT_OSP_CORE_APP_CREATE            31
#define UT_OSP_RECORD_USED                32
#define UT_OSP_NO_FREE_APP_SLOTS          33
#define UT_OSP_EARLYINIT                  34
#define UT_OSP_FUNCTION_POINTER           35
#define UT_OSP_FILE_LINE_TOO_LONG         36
#define UT_OSP_STARTUP_READ               37
#define UT_OSP_CANNOT_OPEN_ES_APP_STARTUP 38
#define UT_OSP_ES_APP_STARTUP_OPEN        39
#define UT_OSP_COMMAND_PIPE               40
#define UT_OSP_APP_INIT                   41
#define UT_OSP_REGISTER_APP               42
#define UT_OSP_SHARED_LIBRARY_INIT        43
#define UT_OSP_DECOMPRESS_LIBRARY         44
#define UT_OSP_EXTRACT_FILENAME_CF        45
#define UT_OSP_EXTRACT_FILENAME_UT46      46
#define UT_OSP_LIB_PATH_FILE_TOO_LONG     47
#define UT_OSP_LOAD_SHARED_LIBRARY        48
#define UT_OSP_FIND_LIBRARY               49
#define UT_OSP_LIBRARY_SLOTS              50
#define UT_OSP_APP_CREATE                 51
#define UT_OSP_DECOMPRESS_APP             52
#define UT_OSP_TABLE_SLOT_IN_USE          53
#define UT_OSP_EXTRACT_FILENAME_UT        54
#define UT_OSP_EXTRACT_FILENAME_UT55      55
#define UT_OSP_CANNOT_FIND_SYMBOL         56
#define UT_OSP_EXTRACT_FILENAME_UT57      57
#define UT_OSP_APP_PATH_FILE_TOO_LONG     58
#define UT_OSP_INVALID_LOG_MODE           59
#define UT_OSP_SYSTEM_LOG_FULL            60
#define UT_OSP_POR_HW_SPECIAL             61
#define UT_OSP_POR_OTHER                  62
#define UT_OSP_POR_MAX_HW_SPECIAL         63
#define UT_OSP_PROC_RESET_MAX_HW_SPECIAL  64
#define UT_OSP_STARTUP_SYNC_FAIL          65
#define UT_OSP_NO_FREE_CORE_APP_SLOTS     66

#endif

