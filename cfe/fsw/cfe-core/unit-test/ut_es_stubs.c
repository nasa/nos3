/*
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
** $Id: ut_es_stubs.c 1.13 2014/09/23 13:30:51GMT-05:00 lwalling Exp  $
**
** Purpose:
** Unit test stubs for Executive Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.13 $
** $Log: ut_es_stubs.c  $
** Revision 1.13 2014/09/23 13:30:51GMT-05:00 lwalling 
** Modify CFE_ES_GetTaskInfo() to also return an error
** Revision 1.12 2014/05/28 10:21:50EDT wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.11 2012/01/13 13:59:30EST acudmore
** Added license text
** Revision 1.10 2010/11/03 15:15:09EDT jmdagost
** Added cfe.h include file.
** Revision 1.9 2010/10/20 12:39:20EDT jmdagost
** Added ES memory pool creation with no semaphore option.
** Revision 1.8 2009/07/29 19:22:21EDT aschoeni
** Added GetPoolBufInfo and GetPoolInfoRtn
** Revision 1.7 2009/05/07 15:02:28EDT rmcgraw
** DCR7366:1 Modified ES_GetTaskInfo
** Revision 1.6 2009/05/06 09:53:02EDT rmcgraw
** DCR7366:1 Changed PutPool to return positive integer for success
** Revision 1.5 2009/04/23 09:27:16EDT rmcgraw
** DCR7366:1 Commented out the fprintfs that indicate the function was called
** Revision 1.4 2009/04/01 16:09:08EDT rmcgraw
** DCR7366:1 Added Stub support for PoolCreateEx and removed alternating task names
** Revision 1.3 2008/08/15 11:27:04EDT njyanchik
** Check in of ES Unit Test
** Revision 1.2 2008/08/07 03:43:52BST dkobe
** Updated Unit Tests for CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:44EDT ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.18 2007/09/24 08:01:56EDT njyanchik
** Update of EVS unit test
** Revision 1.17 2007/09/21 15:40:18EDT David Kobe (dlkobe)
** Modified pointer type definitions to eliminate Linux gcc compile warnings
** Revision 1.15 2007/07/08 22:30:12EDT dlkobe
** Enhanced simulation of CDS
** Revision 1.14 2007/07/05 15:04:30EDT dlkobe
** Baseline following complete TBL unit testing
** Revision 1.13 2007/05/10 15:14:25EDT njyanchik
** Another update of Jonathans UT
** Revision 1.12 2007/05/04 09:12:06EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.10 2007/05/01 13:28:23EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.9 2007/03/19 10:12:50EST njyanchik
** fix a small api change in ES
** Revision 1.8 2007/02/26 14:22:13EST njyanchik
** initial check in files for TBL unit test, plus updates to current files
** Revision 1.7 2006/11/02 13:53:57EST njyanchik
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.6 2006/07/26 07:05:23GMT-05:00 rjmcgraw
** Added CreatePoolRtn code
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "cfe_time_utils.h"
#include "cfe_es.h"
#include "ut_stubs.h"

#ifdef CFE_LINUX
#include "stdlib.h"
#endif

#ifdef OSP_ARINC653
#define UT_OFFSET_CFE_ES_WRITETOSYSLOG 2

#define UT_BREAK_CFE_ES_WRITETOSYSLOG 7

#define UT_SKIP_CFE_ES_WRITETOSYSLOG 58
#endif

/*
** External global variables
*/
extern char UT_appname[80];
extern char cMsg[];

extern uint32 UT_PutPool_Fail;
extern uint32 UT_AppID;
extern uint32 UT_SignatureCDS;
extern uint32 UT_StatusCDS;

extern UT_SetRtn_t CreatePoolRtn;
extern UT_SetRtn_t PoolCreateExRtn;
extern UT_SetRtn_t PutPoolRtn;
extern UT_SetRtn_t GetPoolRtn;
extern UT_SetRtn_t GetPoolInfoRtn;
extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t ES_CreateChildRtn;
extern UT_SetRtn_t ES_DeleteCDSRtn;
extern UT_SetRtn_t ES_RegisterCDSRtn;
extern UT_SetRtn_t ES_CopyToCDSRtn;
extern UT_SetRtn_t ES_RestoreFromCDSRtn;
extern UT_SetRtn_t GetAppIDRtn;
extern UT_SetRtn_t ES_ExitAppRtn;
extern UT_SetRtn_t WriteSysLogRtn;
extern UT_SetRtn_t GetResetTypeRtn;
extern UT_SetRtn_t ES_GetTaskInfoRtn;

extern uint8        UT_CDS[UT_CDS_SIZE];
extern UT_CDS_Map_t UT_CDS_Map;
extern uint32       poolBufIndex;

/*
** Global variables
*/
char poolBuffer[65536];

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_ES_CreateChildTask stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_CreateChildTask.  The user can adjust the response by setting
**        the values in the ES_CreateChildRtn structure prior to this function
**        being called.  If the value ES_CreateChildRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        ES_CreateChildRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_CreateChildTask(uint32 *TaskIdPtr,
                             const char *TaskName,
                             CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr,
                             uint32 *StackPtr,
                             uint32 StackSize,
                             uint32 Priority,
                             uint32 Flags)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (ES_CreateChildRtn.count > 0)
    {
        ES_CreateChildRtn.count--;

        if (ES_CreateChildRtn.count == 0)
        {
            status = ES_CreateChildRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_ES_CreateChildTask called: %ld",
                     ES_CreateChildRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_ES_CreateChildTask called: %s", TaskName);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppID stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppID.  The user can adjust the response by setting
**        the values in the GetAppIDRtn structure prior to this function
**        being called.  If the value GetAppIDRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetAppIDRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppID(uint32 *pAppID)
{
    int32 status = CFE_SUCCESS;

    *pAppID = UT_AppID;

    if (GetAppIDRtn.count > 0)
    {
        GetAppIDRtn.count--;

        if (GetAppIDRtn.count == 0)
        {
            status = GetAppIDRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppIDByName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppIDByName.  The user can adjust the response by setting
**        the value of UT_appname prior to this function being called, then
**        choosing specific values for the application name (pAppName) used
**        when calling this function.  The application ID returned is
**        dependent on the application name provided.  If application name
**        doesn't match the expected values the function returns an error
**        code.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either CFE_ES_ERR_APPNAME or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppIDByName(uint32 *pAppID, const char *pAppName)
{
    int32 status = CFE_SUCCESS;

    if (strcmp(pAppName, UT_appname) == 0)
    {
        *pAppID = UT_AppID;
    }
    else if (strcmp(pAppName, "unregistered_app") == 0)
    {
        *pAppID = CFE_ES_MAX_APPLICATIONS - 1;
    }
    else if (strcmp(pAppName, "illegal_id") == 0)
    {
        *pAppID = CFE_ES_MAX_APPLICATIONS + 1;
    }
    else
    {
        status = CFE_ES_ERR_APPNAME;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppName.  The user must set the value of UT_appname prior
**        to this function being called.  The function uses UT_appname for the
**        retrieved application name and returns CFE_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppName(char *pAppName, uint32 AppID, uint32 BufferLength)
{
    strncpy(pAppName, UT_appname, BufferLength);
    pAppName[BufferLength - 1] = '\0';
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterApp.  The user can adjust the response by setting
**        the values in the ES_RegisterRtn structure prior to this function
**        being called.  If the value ES_RegisterRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_RegisterRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterApp(void)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (ES_RegisterRtn.count > 0)
    {
        ES_RegisterRtn.count--;

        if (ES_RegisterRtn.count == 0)
        {
            status = ES_RegisterRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_ES_RegisterApp called: %ld", ES_RegisterRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        UT_Text("  CFE_ES_RegisterApp called: SUCCESS");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterChildTask stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterChildTask.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterChildTask(void)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_ES_WriteToSysLog stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_WriteToSysLog.  It always returns CFE_SUCCESS when called.
**        The log message is compared to known CFE function responses and if
**        a match is found the value WriteSysLogRtn.value is set to a
**        specified constant value.  WriteSysLogRtn.count is incremented to
**        keep track of the number of messages logged.  The unit test code
**        compares the WriteSysLogRtn value and count variables against
**        expected totals to ensure the proper response of other functions
**        being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_WriteToSysLog(const char *pSpecString, ...)
{
    char    tmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    va_list ap;

#ifdef OSP_ARINC653
    va_start(ap, pSpecString, UT_OFFSET_CFE_ES_WRITETOSYSLOG,
             UT_BREAK_CFE_ES_WRITETOSYSLOG, UT_SKIP_CFE_ES_WRITETOSYSLOG);
#else
    va_start(ap, pSpecString);
#endif

    vsnprintf(tmpString, CFE_ES_MAX_SYSLOG_MSG_SIZE, pSpecString, ap);
    va_end(ap);

    /* EVS SysLog messages */
    if (UT_strcmp(tmpString, "EVS call to CFE_PSP_GetResetArea failed, "
               "RC=0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 1;
    }
    else if (UT_strcmp(tmpString, "Unexpected size from CFE_PSP_GetResetArea: "
                    "expected = 0x~, actual = 0x00000000\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 2;
    }
    else if (UT_strcmp(tmpString, "EVS call to OS_MutSemCreate failed, "
                    "RC=0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 3;
    }
    else if (UT_strcmp(tmpString, "Event Log cleared following power-on "
                    "reset\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 4;
    }
    else if (UT_strcmp(tmpString, "Event Log cleared, n=~, c=~, f=~, m=~, "
                    "o=~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 5;
    }
    else if (UT_strcmp(tmpString, "Event Log restored, n=2, c=2, f=0, m=1, "
                    "o=0\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 6;
    }
    else if (UT_strcmp(tmpString, "EVS:Application Init Failed,RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 7;
    }
    else if (UT_strcmp(tmpString, "EVS:Error reading cmd pipe,RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 8;
    }
    else if (UT_strcmp(tmpString, "EVS:Call to CFE_ES_RegisterApp Failed:"
                    "RC=0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 9;
    }
    else if (UT_strcmp(tmpString, "EVS:Call to CFE_ES_GetAppID Failed:RC="
                    "0x~\n") == 0 )
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 10;
    }
    else if (UT_strcmp(tmpString, "EVS:Call to CFE_SB_CreatePipe Failed:RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 12;
    }
    else if (UT_strcmp(tmpString, "EVS:Subscribing to Cmds Failed:RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 13;
    }
    else if (UT_strcmp(tmpString, "EVS:Subscribing to HK Request Failed:"
                    "RC=0x~\n") == 0)
    {
        WriteSysLogRtn.value = EVS_SYSLOG_OFFSET + 14;
    }

    /* FS SysLog messages */
    else if (UT_strcmp(tmpString, "FS SharedData Mutex Take Err Stat=0x~,"
                    "App=0,Function=FunctionName\n") == 0)
    {
        WriteSysLogRtn.value = FS_SYSLOG_OFFSET + 1;
    }
    else if (UT_strcmp(tmpString, "FS SharedData Mutex Give Err Stat=0x~,"
                    "App=0,Function=FunctionName\n") == 0)
    {
        WriteSysLogRtn.value = FS_SYSLOG_OFFSET + 2;
    }

    else if (UT_strcmp(tmpString, "TIME:Error reading cmd pipe,RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = TIME_SYSLOG_OFFSET + 1;
    }
    else if (UT_strcmp(tmpString, "TIME:Application Init Failed,RC="
                    "0x~\n") == 0)
    {
        WriteSysLogRtn.value = TIME_SYSLOG_OFFSET + 2;
    }
    else if (strcmp(tmpString, "TIME:1Hz OS_TimerAdd failed:RC=0xFFFFFFFF\n") == 0 ||
             strcmp(tmpString, "TIME:1Hz OS_TimerAdd failed:RC=0xffffffff\n") == 0)
    {
        WriteSysLogRtn.value = TIME_SYSLOG_OFFSET + 3;
    }
    else if (strcmp(tmpString, "TIME:1Hz OS_TimerSet failed:RC=0xFFFFFFFF\n") == 0 ||
             strcmp(tmpString, "TIME:1Hz OS_TimerSet failed:RC=0xffffffff\n") == 0)
    {
        WriteSysLogRtn.value = TIME_SYSLOG_OFFSET + 4;
    }

    /* Unrecognized Syslog message */
    else
    {
        WriteSysLogRtn.value = 0;
    }

    ++WriteSysLogRtn.count;
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH, "  %s", tmpString);
    UT_Text(cMsg);
#endif
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetPoolBuf stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetPoolBuf.  The user can adjust the response by setting
**        the values in the GetPoolRtn structure prior to this function
**        being called.  If the value GetPoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetPoolRtn.value.
**        Otherwise the buffer size is computed based on the Size value and
**        returned.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined value, or the buffer size based on Size
**        value (0xffffffff if Size exceeds maximum value allowed).
**
******************************************************************************/
int32 CFE_ES_GetPoolBuf(uint32 **BufPtr,
                        CFE_ES_MemHandle_t HandlePtr,
                        uint32 Size)
{
    int32   Block = 0x8000000;
    boolean flag = FALSE;

    if (GetPoolRtn.count > 0)
    {
        GetPoolRtn.count--;

        if (GetPoolRtn.count == 0)
        {
            *BufPtr = NULL;
            Block = GetPoolRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (Size > CFE_SB_MAX_SB_MSG_SIZE)
        {
            Block = 0xffffffff;
        }
        else
        {
            /* Round up the requested size to the next highest multiple of 2
             * for the block size, similar to CFE_ES_GetBlockSize, in order to
             * account for memory alignment requirements
             */
            while (!(Block >> 1 & Size))
            {
                Block >>= 1;
            }

            *BufPtr = (uint32 *) &poolBuffer[poolBufIndex];
            poolBufIndex += Block;
        }
    }

    return Block;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreate stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreate.  The user can adjust the response by setting
**        the values in the CreatePoolRtn structure prior to this function
**        being called.  If the value CreatePoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value CreatePoolRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreate(cpuaddr *HandlePtr, uint8 *MemPtr, uint32 Size)
{
    int32 status = OS_SUCCESS;

#ifdef UT_VERBOSE
    UT_Text("  CFE_ES_PoolCreate called");
#endif

    if (CreatePoolRtn.count > 0)
    {
        CreatePoolRtn.count--;

        if (CreatePoolRtn.count == 0)
        {
            status = CreatePoolRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreateNoSem stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreateNoSem.  It always returns OS_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *HandlePtr,
                             uint8 *MemPtr,
                             uint32 Size)
{
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreateEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreateEx.  The user can adjust the response by setting
**        the values in the PoolCreateExRtn structure prior to this function
**        being called.  If the value PoolCreateExRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value PoolCreateExRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreateEx(cpuaddr *HandlePtr,
                          uint8 *MemPtr,
                          uint32 Size,
                          uint32 NumBlockSizes,
                          uint32 *BlockSizes,
                          uint16 UseMutex)
{
    int32 status = CFE_SUCCESS;

#ifdef UT_VERBOSE
    UT_Text("  CFE_ES_PoolCreateEx called");
#endif

    if (PoolCreateExRtn.count > 0)
    {
        PoolCreateExRtn.count--;

        if (PoolCreateExRtn.count == 0)
        {
            status = PoolCreateExRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PutPoolBuf stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PutPoolBuf.  The user can adjust the response by setting
**        the values in the PutPoolRtn structure prior to this function
**        being called.  If the value PutPoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value PutPoolRtn.value.
**        Otherwise the return value is positive, indicating success, if the
**        user sets UT_PutPool_Fail equal to zero prior to calling this
**        function; if UT_PutPool_Fail is non-zero then -1 is returned,
**        indicating failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, 16, or -1.
**
******************************************************************************/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    int32   status;
    boolean flag = FALSE;

    if (PutPoolRtn.count > 0)
    {
        PutPoolRtn.count--;

        if (PutPoolRtn.count == 0)
        {
            status = PutPoolRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_PutPool_Fail == 0)
        {
            status = 16; /* Return a positive integer to indicate success */
        }
        else
        {
            status = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetPoolBufInfo stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetPoolBufInfo.  The user can adjust the response by setting
**        the values in the GetPoolInfoRtn structure prior to this function
**        being called.  If the value GetPoolInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetPoolInfoRtn.value.
**        A positive value, indicating success, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or 16.
**
******************************************************************************/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    int32   status;
    boolean flag = FALSE;

    if (GetPoolInfoRtn.count > 0)
    {
        GetPoolInfoRtn.count--;

        if (GetPoolInfoRtn.count == 0)
        {
            status = GetPoolInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        status = 16; /* Return a positive integer to indicate success */
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PerfLogAdd stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_PerfLogAdd.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  CFE_ES_PerfLogAdd called: EntryExit = %lu", EntryExit);
    UT_Text(cMsg);
#endif
}

/*****************************************************************************/
/**
** \brief CFE_ES_CalculateCRC stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_CalculateCRC.  It is set to return a fixed CRC value for the
**        unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 332424.
**
******************************************************************************/
uint32 CFE_ES_CalculateCRC(const void *DataPtr,
                           uint32 DataLength,
                           uint32 InputCRC,
                           uint32 TypeCRC)
{
    return 332424;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetTaskInfo stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetTaskInfo.  The user can adjust the response by setting
**        the values in the ES_GetTaskInfoRtn structure prior to this function
**        being called.  If the value ES_GetTaskInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_GetTaskInfoRtn.value.
**        CFE_SUCCESS is returned otherwise.  The user can adjust the
**        application name and task name by setting the value of UT_appname
**        prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{

    static uint32 cnt;
    int32   status = CFE_SUCCESS;
    boolean flag = FALSE;

    if (ES_GetTaskInfoRtn.count > 0)
    {
        ES_GetTaskInfoRtn.count--;

        if (ES_GetTaskInfoRtn.count == 0)
        {
            status = ES_GetTaskInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        TaskInfo->AppId = 3; /* Fake ID number */
        strncpy((char *) &TaskInfo->AppName, UT_appname, OS_MAX_API_NAME);
        TaskInfo->AppName[OS_MAX_API_NAME - 1] = '\0';
        strncpy((char *) &TaskInfo->TaskName, UT_appname, OS_MAX_API_NAME);
        TaskInfo->TaskName[OS_MAX_API_NAME - 1] = '\0';

        if (cnt % 2)
        {
            strcpy((char *) &TaskInfo->TaskName, "");
        }

        cnt++;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_ExitApp stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_ExitApp.  The value passed to the function, ExitStatus, is
**        added to the variable ES_ExitAppRtn.value and the variable
**        ES_ExitAppRtn.count is incremented each time this function is called.
**        The unit tests compare these values to expected results to verify
**        proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_ExitApp(uint32 ExitStatus)
{
    ES_ExitAppRtn.value += ExitStatus;
    ES_ExitAppRtn.count++;
}

/*****************************************************************************/
/**
** \brief CFE_ES_CopyToCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_CopyToCDS.  The user can adjust the response by setting
**        the values in the ES_CopyToCDSRtn structure prior to this function
**        being called.  If the value ES_CopyToCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_CopyToCDSRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy)
{
    int32   i = 0;
    int32   status = CFE_SUCCESS;
    boolean flag = FALSE;

	if (ES_CopyToCDSRtn.count > 0)
	{
		ES_CopyToCDSRtn.count--;

		if (ES_CopyToCDSRtn.count == 0)
		{
		    status = ES_CopyToCDSRtn.value;
		    flag = TRUE;
		}
	}

	if (flag == FALSE)
	{
        while (UT_CDS_Map.Handles[i] != Handle)
        {
            i++;
        }

        memcpy(&UT_CDS[Handle], DataToCopy, (UT_CDS_Map.Handles[i+1] -
               UT_CDS_Map.Handles[i]));
	}

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RestoreFromCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RestoreFromCDS.  The user can adjust the response by setting
**        the values in the ES_RestoreFromCDSRtn structure prior to this
**        function being called.  If the value ES_RestoreFromCDSRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        ES_RestoreFromCDSRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    int32   i = 0;
    int32   status = CFE_SUCCESS;
    boolean flag = FALSE;

	if (ES_RestoreFromCDSRtn.count > 0)
	{
		ES_RestoreFromCDSRtn.count--;

		if (ES_RestoreFromCDSRtn.count == 0)
		{
		    status = ES_RestoreFromCDSRtn.value;
            flag = TRUE;
		}
	}

	if (flag == FALSE)
	{
        while (UT_CDS_Map.Handles[i] != Handle)
        {
            i++;
        }

        memcpy(RestoreToMemory, &UT_CDS[Handle], (UT_CDS_Map.Handles[i+1] -
               UT_CDS_Map.Handles[i]));
	}

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterCDSEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterCDSEx.  The user can adjust the response by setting
**        the values in the ES_RegisterCDSRtn structure prior to this function
**        being called.  If the value ES_RegisterCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        ES_RegisterCDSRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr,
                           int32 BlockSize,
                           const char *Name,
                           boolean CriticalTbl)
{
    int32 status = CFE_SUCCESS;

    *HandlePtr = UT_CDS_Map.Handles[UT_CDS_Map.NextHandle];
    UT_CDS_Map.Handles[UT_CDS_Map.NextHandle+1] =
        UT_CDS_Map.Handles[UT_CDS_Map.NextHandle] + BlockSize;
    UT_CDS_Map.NextHandle++;

    if (UT_CDS_Map.NextHandle >= UT_MAX_NUM_CDS)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_ES_RegisterCDSEx called: number of CDSs exceeds UT "
                 "maximum of %d", UT_MAX_NUM_CDS);
        UT_Text(cMsg);

#ifdef CFE_ARINC653
#ifdef CFE_LINUX
        exit(1);
#else
/* TODO: Insert substitute for exit() */
#endif
#else
        exit(1);
#endif
    }

    if (UT_CDS_Map.Handles[UT_CDS_Map.NextHandle] >= UT_CDS_SIZE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_ES_RegisterCDSEx called: size of CDSs (%lu) exceeds "
                 "UT maximum of %d", (unsigned long)UT_CDS_Map.Handles[UT_CDS_Map.NextHandle],
                 (int)UT_CDS_SIZE);
        UT_Text(cMsg);

#ifdef CFE_ARINC653
#ifdef CFE_LINUX
        exit(1);
#else
/* TODO: Insert substitute for exit() */
#endif
#else
        exit(1);
#endif
    }

	if (ES_RegisterCDSRtn.count > 0)
	{
		ES_RegisterCDSRtn.count--;

		if (ES_RegisterCDSRtn.count == 0)
		{
		    status = ES_RegisterCDSRtn.value;
		}
	}

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_DeleteCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_DeleteCDS.  The user can adjust the response by setting
**        the values in the ES_DeleteCDSRtn structure prior to this function
**        being called.  If the value ES_DeleteCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_DeleteCDSRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_DeleteCDS(const char *CDSName, boolean CalledByTblServices)
{
    int32 status = CFE_SUCCESS;

    if (ES_DeleteCDSRtn.count > 0)
	{
		ES_DeleteCDSRtn.count--;

		if (ES_DeleteCDSRtn.count == 0)
		{
		    status = ES_DeleteCDSRtn.value;
		}
	}

	return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetResetType stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_GetResetType.  It returns the user-defined value,
**        GetResetTypeRtn.value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status flag.
**
******************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    return GetResetTypeRtn.value;
}

/*****************************************************************************/
/**
** \brief CFE_ES_IncrementTaskCounter stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_IncrementTaskCounter.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_IncrementTaskCounter(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_ES_WaitForStartupSync stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_WaitForStartupSync.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_WaitForStartupSync(uint32 Timeout)
{
}
