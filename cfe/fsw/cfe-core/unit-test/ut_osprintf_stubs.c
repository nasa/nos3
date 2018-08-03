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
** $Id: ut_osapi_stubs.c 1.11 2014/09/05 11:42:11GMT-05:00 acudmore Exp  $
**
** Purpose:
** Unit test stubs for Operating System routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.11 $
** $Log: ut_osapi_stubs.c  $
** Revision 1.11 2014/09/05 11:42:11GMT-05:00 acudmore
** Updated text for ES unit test ( OS_printf text for Syslog entries )
** Revision 1.10 2014/05/28 09:21:52GMT-05:00 wmoleski
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.9 2012/01/13 13:59:31EST acudmore
** Added license text
** Revision 1.8 2010/10/25 18:22:41EDT jmdagost
** Updated OS_TimerGetInfo() to return a success status.
** Revision 1.7 2010/09/09 15:06:07EDT jmdagost
** Update argument definition for OS_TaskCreate().
** Added dummy functions for OS_TimerGetInfo() and OS_TimerDelete() that currently only return an error.
** Revision 1.6 2009/08/04 13:34:38EDT aschoeni
** Added heap information to ES telemetry
** Revision 1.5 2009/06/25 12:46:02EDT rmcgraw
** DCR8290:5 cFE API changes to unit test files
** Revision 1.4 2009/04/27 10:38:32EDT rmcgraw
** DCR7366:1 Commented out debug messages to prevent flooding output file
** Revision 1.3 2009/04/01 16:12:03EDT rmcgraw
** DCR7366:1 Removed the printout of OS_TaskGetId called - it was flooding the output file
** Revision 1.2 2008/08/15 11:26:56EDT njyanchik
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:46BST ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.27 2007/09/26 15:00:47EDT njyanchik
** This is the first update of the ES unit tests for 5.0
** Revision 1.26 2007/07/05 15:04:31EDT David Kobe (dlkobe)
** Baseline following complete TBL unit testing
** Revision 1.25 2007/06/04 13:31:57EDT njyanchik
** Update of the TBL unit test
** Revision 1.24 2007/05/30 15:15:23EDT njyanchik
** check in of ES Unit test
** Revision 1.23 2007/05/30 08:56:41EDT njyanchik
** Update
** Revision 1.22 2007/05/25 13:13:43EDT njyanchik
** update of ES unit test
** Revision 1.21 2007/05/25 09:17:57EDT njyanchik
** I added the rmfs call to the OSAL and updated the unit test stubs to match
** Revision 1.20 2007/05/24 15:48:10EDT njyanchik
** Update for ES Unit Test
** Revision 1.19 2007/05/23 10:54:59EDT njyanchik
** Update again to prevent loss of work
** Revision 1.18 2007/05/16 11:14:37EDT njyanchik
** Update ES's unit test driver to match code for build 4.1
** Revision 1.17 2007/05/04 09:10:33EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.15 2007/05/01 13:28:25EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.14 2007/04/04 09:28:47EDT njyanchik
** This CP updates the unit test for the changes made to the OS (OS_IntLock/OS_IntUnlock) under
** this DCR
** Revision 1.13 2007/03/19 10:49:48EST njyanchik
** the UT needs to be updated because the API for OS_IntEnableAll changed
** Revision 1.12 2007/03/02 15:18:06EST njyanchik
** Jonathan provided me with a newer tbl unit test to check in
** Revision 1.11 2007/01/17 09:26:09GMT-05:00 njyanchik
** Check in of ES Unit Test changed files
** Revision 1.10 2006/11/28 13:01:02GMT-05:00 rjmcgraw
** Changed OS_write stub to return bytes written
** Revision 1.9 2006/11/08 09:55:19EST rjmcgraw
** Added OS_open
** Revision 1.8 2006/07/28 07:33:03EDT njyanchik
** These changes change the osapi functions OS_slose, OS_write, and OS_creat to
** int32 parameters instead of uint32.
** Revision 1.7 2006/07/26 08:08:27EDT rjmcgraw
** Changed name MutSemGivRtn to MutSemGiveRtn
** Revision 1.6 2006/06/08 19:13:59GMT rjmcgraw
** Added support for function UT_SetRtnCode
** Revision 1.5 2006/05/22 19:22:22GMT jjhageman
** Addition of OS_QueueDelete
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_error.h"
#include "osapi.h"
#include "common_types.h"
#include "ut_stubs.h"
#include "osprintf_priv.h"
#include "ut_osprintf_stubs.h"


#ifdef OS_USE_EMBEDDED_PRINTF
#define UT_OFFSET_OS_PRINTF 2
#define UT_OFFSET_OS_SPRINTF 3
#define UT_OFFSET_OS_SNPRINTF 4

#define UT_BREAK_OS_PRINTF 7
#define UT_BREAK_OS_SPRINTF 6
#define UT_BREAK_OS_SNPRINTF 5

#define UT_SKIP_OS_PRINTF 94
#define UT_SKIP_OS_SPRINTF 22
#define UT_SKIP_OS_SNPRINTF 22

static int gMaxOutputLen = -1;
static int gCurrentOutputLen = 0;

#ifdef OSP_ARINC653
uint32 OS_PrintfMutexId = 0xffffffff;
int OS_printf_break = -1;
int OS_printf_skip = 0;
#endif
#endif

/*
** Global variables
*/

extern UT_SetRtn_t OSPrintRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief OS_printf stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_printf.  The user can adjust the response by setting the value of
**        OSPrintRtn.count prior to calling the function.  If count is not
**        negative then the value OSPrintRtn.value is incremented by a fixed
**        amount and OSPrintRtn.count is incremented.  If count is negative the
**        print string is compared to known responses and if a match is found
**        the value OSPrintRtn.value is incremented by a specified constant
**        value.  OSPrintRtn.count is decremented to keep track of the number
**        of messages logged.  The unit test code compares the OSPrintRtn value
**        and count variables against expected totals to ensure the proper
**        response of other functions being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_printf(const char *string, ...)
{
    char    tmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE * 2];
    char    *start;
    char    dash = '-';
    va_list ptr;

#ifdef OSP_ARINC653
    va_start(ptr, string, UT_OFFSET_OS_PRINTF, UT_BREAK_OS_PRINTF,
             UT_SKIP_OS_PRINTF);
#else
    va_start(ptr, string);
#endif

    vsnprintf(tmpString, CFE_ES_MAX_SYSLOG_MSG_SIZE * 2, string, ptr);
    va_end(ptr);

#ifdef UT_VERBOSE
    UT_Text(tmpString);
#endif

    if (OSPrintRtn.count >= 0)
    {
        OSPrintRtn.value += OS_PRINT_INCR;
        OSPrintRtn.count++;
    }
    else
    {
        OSPrintRtn.count--;

        if ((start = strchr(tmpString, dash)))
        {
            start += 2; /* Skip dash and space */

            if (UT_strcmp(start, "CFE_ES_ExitApp: CORE Application CFE_ES Had "
                          "an Init Error.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CORE_INIT;
            }
            else if (UT_strcmp(start, "POWER ON RESET due to max proc resets "
                               "(Commanded).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_POR_MAX_PROC_RESETS;
            }
            else if (UT_strcmp(start, "CFE_ES_ExitApp: CORE Application CFE_ES"
                               " Had a Runtime Error.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CORE_RUNTIME;
            }
            else if (UT_strcmp(start, "CFE_ES_ExitApp, Cannot Exit CORE "
                               "Application CFE_ES\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CORE_APP_EXIT;
            }
            else if (UT_strcmp(start, "PROCESSOR RESET called from "
                               "CFE_ES_ResetCFE (Commanded).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_PROC_RESET_COMMANDED;
            }
            else if (UT_strcmp(start, "POWERON RESET called from "
                               "CFE_ES_ResetCFE (Commanded).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_POR_COMMANDED;
            }

            else if (UT_strcmp(start, "CFE_ES_ExitChildTask Error: Cannot "
                               "Call from a cFE App Main Task. ID = ~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CANNOT_CALL_APP_MAIN;
            }
            else if (UT_strcmp(start, "CFE_ES_ExitChildTask Error Calling "
                               "CFE_ES_GetAppID. Task ID = ~, RC = "
                               "0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_GET_APP_ID;
            }

            else if (UT_strcmp(start, "ES SharedData Mutex Take Err Stat=0x"
                               "~,App=1,Func=TestAPI,Line=12345\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_MUTEX_TAKE;
            }
            else if (UT_strcmp(start, "ES SharedData Mutex Give Err Stat=0x"
                               "~,App=1,Func=TestAPI,Line=98765\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_MUTEX_GIVE;
            }

            else if (UT_strcmp(start, "CFE_ES_RestartApp: Cannot Restart "
                               "Application appName, It is not "
                               "running.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CANNOT_RESTART_APP;
            }

            else if (UT_strcmp(start, "ES Startup: Insufficent Free Space on "
                               "Volatile Disk, Reformatting.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_INSUFF_FREE_SPACE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Creating Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CREATE_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Initializing Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_INIT_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Mounting Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_MOUNT_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Re-Mounting Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_REMOUNT_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Re-Formating Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_REFORMAT_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Removing Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_REMOVE_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Un-Mounting Volatile"
                               "(RAM) Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_UNMOUNT_VOLATILE;
            }
            else if (UT_strcmp(start, "ES Startup: Error Determining Blocks "
                               "Free on Volume. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_DETERMINE_BLOCKS;
            }

            else if (UT_strcmp(start, "ES Startup: OS_TaskCreate error "
                               "creating core App: CFE_TBL: EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CORE_APP_CREATE;
            }
            else if (UT_strcmp(start, "ES Startup: CFE_ES_Global.TaskTable "
                               "record used error for App: CFE_EVS, "
                               "continuing.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_RECORD_USED;
            }
            else if (UT_strcmp(start, "ES Startup: Error, No free application "
                               "slots available for CORE App!\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_NO_FREE_CORE_APP_SLOTS;
            }
            else if (UT_strcmp(start, "ES Startup: No free application "
                               "slots available\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_NO_FREE_APP_SLOTS;
            }
            else if (UT_strcmp(start, "ES Startup: Error returned when "
                               "calling function: CFE_TBL_EarlyInit: EC = "
                               "0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_EARLYINIT;
            }
            else if (UT_strcmp(start, "ES Startup: bad function pointer ( "
                               "table entry = 1).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_FUNCTION_POINTER;
            }

            else if (UT_strcmp(start, "ES Startup: ES Startup File Line is "
                               "too long: 137 bytes.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_FILE_LINE_TOO_LONG;
            }
            else if (UT_strcmp(start, "ES Startup: Error Reading Startup "
                               "file. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_STARTUP_READ;
            }
            else if (UT_strcmp(start, "ES Startup: Error, Can't Open ES App "
                               "Startup file: " CFE_ES_NONVOL_STARTUP_FILE " "
                               "EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CANNOT_OPEN_ES_APP_STARTUP;
            }
            else if (UT_strcmp(start, "ES Startup: Opened ES App Startup "
                               "file: " CFE_ES_VOLATILE_STARTUP_FILE "\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_ES_APP_STARTUP_OPEN;
            }

            else if (UT_strcmp(start, "ES:Error reading cmd pipe,RC="
                               "0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_COMMAND_PIPE;
            }
            else if (UT_strcmp(start, "ES:Application Init Failed,RC="
                               "0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_APP_INIT;
            }

            else if (UT_strcmp(start, "ES:Call to CFE_ES_RegisterApp Failed, "
                               "RC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_REGISTER_APP;
            }

            else if (UT_strcmp(start, "ES Startup: Load Shared Library Init "
                               "Error.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_SHARED_LIBRARY_INIT;
            }
            else if (UT_strcmp(start, "ES Startup: Unable to decompress "
                               "library file: /cf/apps/tst_lib.bundle."
                               "gz\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_DECOMPRESS_LIBRARY;
            }
            else if (UT_strcmp(start, "ES Startup: Unable to extract filename "
                               "from path: /cf/apps/tst_lib.bundle."
                               "gz.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_EXTRACT_FILENAME_CF;
            }
            else if (strncmp(start, "ES Startup: Unable to extract filename "
                             "from path: ut46/", 55) == 0)
            {
                OSPrintRtn.value += UT_OSP_EXTRACT_FILENAME_UT46;
            }
            else if (UT_strcmp(start, "ES Startup: Library path plus file "
                               "name length (~) exceeds max allowed "
                               "(~)\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_LIB_PATH_FILE_TOO_LONG;
            }
            else if (UT_strcmp(start, "ES Startup: Could not load cFE Shared "
                               "Library\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_LOAD_SHARED_LIBRARY;
            }
            else if (UT_strcmp(start, "ES Startup: Could not find Library "
                               "Init symbol:TST_LIB_Init. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_FIND_LIBRARY;
            }
            else if (UT_strcmp(start, "ES Startup: No free library slots "
                               "available\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_LIBRARY_SLOTS;
            }

            else if (UT_strcmp(start, "ES Startup: AppCreate Error: "
                               "TaskCreate AppName Failed. EC = 0x~!\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_APP_CREATE;
            }
            else if (UT_strcmp(start, "ES Startup: Unable to decompress "
                               "Application File: ut/filename.gz\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_DECOMPRESS_APP;
            }
            else if (UT_strcmp(start, "ES Startup: Error: ES_TaskTable slot "
                               "in use at task creation!\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_TABLE_SLOT_IN_USE;
            }
            else if (UT_strcmp(start, "ES Startup: Unable to extract filename "
                               "from path: ut/filename.gz.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_EXTRACT_FILENAME_UT;
            }
            else if (UT_strcmp(start, "ES Startup: Could not load cFE "
                               "application file:ut/filename.x. "
                               "EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_EXTRACT_FILENAME_UT55;
            }
            else if (UT_strcmp(start, "ES Startup: Could not find symbol:"
                               "EntryPoint. EC = 0x~\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_CANNOT_FIND_SYMBOL;
            }
            else if (strncmp(start, "ES Startup: Unable to extract filename "
                             "from path: ut57/", 55) == 0)
            {
                OSPrintRtn.value += UT_OSP_EXTRACT_FILENAME_UT57;
            }
            else if (UT_strcmp(start, "ES Startup: Application path plus file "
                               "name length (~) exceeds max allowed "
                               "(~)\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_APP_PATH_FILE_TOO_LONG;
            }
            else if (UT_strcmp(start, "ES Startup: Startup Sync failed - "
            		           "Applications may not have all started\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_STARTUP_SYNC_FAIL;
            }
            else if (UT_strcmp(start, "POWER ON RESET due to HW Special Cmd "
            		           "(Hw Spec Cmd).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_POR_HW_SPECIAL;
            }
            else if (UT_strcmp(start, "POWER ON RESET due to other cause "
            		           "(See Subtype).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_POR_OTHER;
            }
            else if (UT_strcmp(start, "POWER ON RESET due to max proc resets "
            		           "(HW Spec Cmd).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_POR_MAX_HW_SPECIAL;
            }
            else if (UT_strcmp(start, "PROCESSOR RESET due to Hardware Special "
            		           "Command (HW Spec Cmd).\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_PROC_RESET_MAX_HW_SPECIAL;
            }
       }
        else
        {
            if (UT_strcmp(tmpString, "Warning: Invalid System Log mode, log "
                          "entry discarded.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_INVALID_LOG_MODE;
            }
            else if (UT_strcmp(tmpString, "Warning: System Log full, log entry "
                               "discarded.\n") == 0)
            {
                OSPrintRtn.value += UT_OSP_SYSTEM_LOG_FULL;
            }
        }
    }
}

#ifdef OS_USE_EMBEDDED_PRINTF
/*****************************************************************************/
/**
** \brief OS_GetStringLen stub function
**
** \par Description
**        This function is identical to the OS API function OS_GetStringLen.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  strptr  Pointer to a character string.
**
** \returns
** \retstmt
**        Returns the number of characters in the string. \endcode
** \endreturns
**
******************************************************************************/
int OS_GetStringLen(const char *strptr)
{
    int len = 0;

    /* Find string length */
    for (; *strptr; strptr++)
    {
        len++;
    }

    return len;
}

/*****************************************************************************/
/**
** \brief OS_OutputSingleChar stub function
**
** \par Description
**        This function is identical to the OS API function
**        OS_OutputSingleChar.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] str  Pointer to the output character string buffer.
**
** \param[in]  c    Single character for output to the buffer.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #putchar
**
******************************************************************************/
void OS_OutputSingleChar(char **str, int c)
{
    if (!(gMaxOutputLen >= 0 && gCurrentOutputLen >= gMaxOutputLen))
    {
        if (str)
        {
            /* Output to the character string buffer */
            **str = (char) c;
            (*str)++;
        }
        else
        {
            /* Output to the console */
            /* ~~~ Insert putchar() replacement here ~~~ */
#ifdef OSP_ARINC653_CFE
            TUTF_putchar();
#else
            putchar(c);
#endif
            /* ~~~ Insert putchar() replacement here ~~~ */
        }

        gCurrentOutputLen++;
    }
}

/*****************************************************************************/
/**
** \brief OS_Double2String stub function
**
** \par Description
**        This function is identical to the OS API function OS_Double2String.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out_bfr     Pointer to a character string buffer.
**
** \param[in]  dbl         Double precision floating point value to convert.
**
** \param[in]  dec_digits  Number of digits to display to the right of the
**                         decimal point.
**
** \param[in]  lead        Pointer to the leading character bit flag.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #OS_GetStringLen
**
******************************************************************************/
#ifndef OSP_NO_FLOAT
void OS_Double2String(char *out_bfr, double dbl,
                      unsigned dec_digits, int *lead)
{
    unsigned mult = 1;
    unsigned idx;
    unsigned wholeNum;
    unsigned decimalNum;
    char temp_bfr[FLOAT_BUF_LEN + 1];

    static const double round_nums[8] =
    {
        0.5,
        0.05,
        0.005,
        0.0005,
        0.00005,
        0.000005,
        0.0000005,
        0.00000005
    };

    /* Extract negative info */
    if (dbl < 0.0)
    {
        dbl = -dbl;
        *lead |= LEAD_NEG;
    }

    /* Handling rounding by adding .5LSB to the floating-point data */
    if (dec_digits < 8)
    {
        dbl += round_nums[dec_digits];
    }

    /* Construct fractional multiplier for specified number of digits */
    for (idx = 0; idx < dec_digits; idx++)
    {
        mult *= 10;
    }

    wholeNum = (unsigned) dbl;
    decimalNum = (unsigned) ((dbl - wholeNum) * mult);

    /* Convert integer portion */
    idx = 0;

    while (wholeNum != 0 && idx < FLOAT_BUF_LEN)
    {
        temp_bfr[idx] = '0' + (wholeNum % 10);
        idx++;
        wholeNum /= 10;
    }

    if (idx == 0)
    {
        *out_bfr = '0';
        out_bfr++;
    }
    else
    {
        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    if (dec_digits > 0)
    {
        *out_bfr = '.';
        out_bfr++;

        /* Convert fractional portion */
        idx = 0;

        while (decimalNum != 0)
        {
            temp_bfr[idx] = '0' + (decimalNum % 10);
            idx++;
            decimalNum /= 10;
        }

        /* Pad the decimal portion with 0s as necessary */
        while (idx < dec_digits)
        {
            temp_bfr[idx] = '0';
            idx++;
        }

        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    *out_bfr = 0;
}
#endif

/*****************************************************************************/
/**
** \brief OS_ParseString stub function
**
** \par Description
**        This function is identical to the OS API function OS_ParseString.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out        Pointer to the output character string buffer.
**
** \param[in]  *string    Pointer to the string to format and then output.
**
** \param[in]  min_width  Minimum resulting string length.
**
** \param[in]  max_width  Maximum resulting string length.
**
** \param[in]  pad        Pointer to the leading character bit flag.
**
** \param[in]  lead       Leading character bit flag.
**
** \param[in]  s_flag     Non-zero if this function is called by the %s
**                        handler; zero otherwise.  Used to determine how the
**                        maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_GetStringLen, #OS_OutputSingleChar
**
******************************************************************************/
int OS_ParseString(char **out, const char *string,
                   int min_width, int max_width, int pad,
                   int lead, unsigned s_flag)
{
    register int pc = 0, padchar = ' ';
    int len = 0;

    len = OS_GetStringLen(string);

    /* Set length to # if specified by %.#s format */
    if (s_flag && max_width && len > max_width)
    {
        len = max_width;
    }

    if (min_width > 0)
    {
        if (len >= min_width)
        {
            min_width = 0;
        }
        else
        {
            min_width -= len;
        }

        if ((pad & PAD_ZERO) && !s_flag)
        {
            padchar = '0';
        }
    }

    if (max_width > min_width)
    {
        max_width -= min_width;

        if (len >= max_width)
        {
            max_width = 0;
        }
        else
        {
            max_width -= len;
        }

        if (lead & LEAD_HEX)
        {
            if (max_width >= 2)
            {
                max_width -= 2;
            }
            else
            {
                max_width = 0;
            }
        }
        else if ((lead & LEAD_NEG) || (lead & LEAD_SIGN))
        {
            if (max_width >= 1)
            {
                max_width--;
            }
            else
            {
                max_width = 0;
            }
        }

        for (; max_width > 0; max_width--)
        {
            OS_OutputSingleChar(out, ' ');
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == '0')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }
    else if (lead & LEAD_NEG)
    {
        OS_OutputSingleChar(out, '-');
        pc++;
    }
    else if (lead & LEAD_SIGN)
    {
        OS_OutputSingleChar(out, '+');
        pc++;
    }
    else if (lead & LEAD_SPACE)
    {
        OS_OutputSingleChar(out, ' ');
        pc++;
    }

    if (!(pad & PAD_RIGHT))
    {
        for (; min_width > 0; min_width--)
        {
            OS_OutputSingleChar(out, padchar);
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == ' ')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }

    for (; len; string++)
    {
        OS_OutputSingleChar(out, *string);
        pc++;
        len--;
    }

    for (; min_width > 0; min_width--)
    {
        OS_OutputSingleChar(out, padchar);
        pc++;
    }

    return pc;
}

/*****************************************************************************/
/**
** \brief OS_ParseInteger stub function
**
** \par Description
**        This function is identical to the OS API function OS_ParseInteger.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out              Pointer to the output character string buffer.
**
** \param[in]  i                Integer to convert.
**
** \param[in]  base             = 10 for base 10 (decimal); = 16 for base 16
**                              (hexadecimal).
**
** \param[in]  sign             Non-zero for integer values; zero for unsigned
**                              integer values .
**
** \param[in]  space_pad_width  Format modifier minimum field width (e.g.,
**                              the 5 in %5.2d).
**
** \param[in]  zero_pad_width   Format modifier precision specifier (e.g.,
**                              the 2 in %5.2d).
**
** \param[in]  pad              Pointer to the leading character bit flag.
**
** \param[in]  lead             Leading character bit flag.
**
** \param[in]  letbase          Non-zero if this function is called by the %s
**                              handler; zero otherwise.  Used to determine how the
**                              maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_ParseString
**
******************************************************************************/
int OS_ParseInteger(char **out, int i, unsigned base, int sign,
                    int space_pad_width, int zero_pad_width,
                    int pad, int lead, int letbase)
{
    char print_buf[PRINT_BUF_LEN + 1];
    char *s;
    int t, pc = 0;
    unsigned u = (unsigned) i;
    int rtn;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        rtn = OS_ParseString(out, print_buf, zero_pad_width,
                             space_pad_width, pad, lead, 0);
    }
    else
    {
        if (sign && base == 10 && i < 0)
        {
            u = (unsigned) -i;
            lead |= LEAD_NEG;
        }

        /* Make sure print_buf is NULL-terminated */
        s = print_buf + PRINT_BUF_LEN - 1;
        *s = '\0';

        while (u)
        {
            t = u % base;

            if (t >= 10)
            {
                t += letbase - '0' - 10;
            }

            s--;
            *s = (char) t + '0';
            u /= base;
        }

        rtn = pc + OS_ParseString(out, s, zero_pad_width,
                                  space_pad_width, pad, lead, 0);
    }

    return rtn;
}

/*****************************************************************************/
/**
** \brief OS_vsnprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_vsnprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out     Pointer to character buffer to hold the  output string.
**
** \param[in]  max_len  Maximum allowed length of the output string.
**
** \param[in]  *format  Pointer to a printf-style format string.
**
** \param[in]  varg     Pointer to the beginning of the list of variables
**                      to output per the format string.
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int OS_vsnprintf(char *out_buffer, int max_len,
                 const char *format, VA_LIST varg)
{
    unsigned post_decimal;
    int width, pad, lead;
    unsigned dec_width;
    int pc = 0;
    char scr[2];
    char *s;
    char **out;

#ifndef OSP_NO_FLOAT
    unsigned dec_width_set;
    double *dblptr;
    double dbl;
    char float_bfr[FLOAT_BUF_LEN + 1];
#endif

#ifdef OSP_ARINC653
    unsigned parm_count = 0;
#endif

    if (out_buffer)
    {
        out = &out_buffer;
    }
    else
    {
        out = 0;
    }

    gMaxOutputLen = max_len;
    gCurrentOutputLen = 0;

    /* Step through and parse the format string */
    for (; *format != '\0'; format++)
    {
        /* Process the format code following the format specifier (%) */
        if (*format == '%')
        {
            format++;
            width = 0;
            dec_width = 0;
#ifndef OSP_NO_FLOAT
            dec_width_set = 0;
#endif
            pad = 0;
            lead = 0;
            post_decimal = 0;

            /* Look for format modifiers */
            while (*format == '-' ||
                   *format == '+' ||
                   *format == ' ' ||
                   *format == '0')
            {
                if (*format == '-')
                {
                    /* Left-justify output */
                    format++;
                    pad |= PAD_RIGHT;
                }
                else if (*format == '+')
                {
                    /* Prepend the plus sign (+) to the output if the value is
                     * positive
                     */
                    format++;
                    lead |= LEAD_SIGN;
                }
                else if (*format == ' ')
                {
                    /* Prepend a space in place of the plus sign (+) if the
                     * value is positive
                     */
                    format++;
                    lead |= LEAD_SPACE;
                }
                else if (*format == '0')
                {
                    /* Pad numerical output with leading zeroes */
                    format++;
                    pad |= PAD_ZERO;
                }
            }

            /* Look for field width and precision specifier (e.g., ###.###) */
            while (1)
            {
                if (*format == '.')
                {
                    /* Decimal point indicates any following numbers are the
                     * precision specifier
                     */
                    if (post_decimal)
                    {
                        /* Already found one decimal point, any others indicate
                         * a format string error; back up pointer so the output
                         * will show all of the erroneous modifier
                         */
                        while (*(format -1) != '%')
                        {
                            format--;
                        }

                        break;
                    }

                    post_decimal = 1;
                    format++;
                }
                else if (*format >= '0' &&  *format <= '9')
                {
                    if (post_decimal)
                    {
                        /* After the decimal is the precision specifier; add
                         * number to the total, accounting for the number of
                         * digits
                         */
                        dec_width *= 10;
                        dec_width += (unsigned) (unsigned char)
                                         (*format - '0');
#ifndef OSP_NO_FLOAT
                        dec_width_set = 1;
#endif
		    }
                    else
                    {
                        /* Prior to the decimal is the field width; add number
                         * to the total, accounting for the number of digits
                         */
                        width *= 10;
                        width += *format - '0';
                    }

                    format++;
                }
                else
                {
                    /* End of field width and precision specifier reached;
                     * exit 'while' loop
                     */
                    break;
                }
            }

            /* Check for long format modifier; if found then skip since all
             * are treated as long regardless
             */
            if (*format == 'l')
            {
                format++;
            }

            switch (*format)
            {
                case 's': /* Character string */
                    s = (char *) *varg;
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    pc += OS_ParseString(out, s, width, dec_width, pad, 0, 1);
                    break;

                case 'd': /* (Long) integer */
                case 'i':
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;

                        if ((int) *varg < 0 || lead)
                        {
                            dec_width--;
                        }
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 1, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    break;

                case 'u': /* (Long) unsigned integer */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 0, width,
                                          dec_width, pad, 0, 'a');
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    break;

                case 'x': /* Hexadecimal (lower case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    break;

                case 'X': /* Hexadecimal (upper case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'A');
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    break;

                case 'p': /* Address (hexadecimal with '0x' prepended) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO) && width >= 2)
                    {
                        dec_width = width - 2;
                    }

                    lead = LEAD_HEX;
                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    break;

                case 'c': /* Single character */
                    scr[0] = (char) *varg;
                    varg++;
#ifdef OSP_ARINC653
                    parm_count++;
#endif
                    scr[1] = '\0';
                    pc += OS_ParseString(out, scr, width, 0, pad, 0, 0);
                    break;

#ifndef OSP_NO_FLOAT
                case 'f': /* (Long) float */
                    dblptr = (double *) varg;
                    dbl = *dblptr;
                    dblptr++;
                    varg = (VA_LIST) dblptr;
#ifdef OSP_ARINC653
                    parm_count++;
#endif

                    if (!dec_width_set)
                    {
                        dec_width = 6;
                    }

                    OS_Double2String(float_bfr, dbl, dec_width, &lead);

                    if (width && !(pad & PAD_RIGHT) && !(pad & PAD_ZERO))
                    {
                        dec_width = 0;
                    }
                    else
                    {
                        dec_width = width - (lead != 0);
                        width = 0;
                    }

                    pc += OS_ParseString(out, float_bfr, dec_width,
                                         width, pad, lead, 0);
                    break;
#endif

                case '%': /* Output percent character (%) */
                    OS_OutputSingleChar(out, *format);
                    pc++;
                    break;

                case '\0':
                    /* Premature end of format string; back up one so the
                     * 'for' loop will terminate parsing gracefully
                     */
                    format--;
                    break;

                default:
                    /* Invalid format code; output the format specifier
                     * and code
                     */
                    OS_OutputSingleChar(out, '%');
                    OS_OutputSingleChar(out, *format);
                    pc += 2;
                    break;
            }

#ifdef OSP_ARINC653
            /* If at the end of the first contiguous list of values */
            if (parm_count == OS_printf_break)
            {
                /* Adjust pointer to skip to the second contiguous list */
                parm_count++;
                varg += OS_printf_skip;
            }
#endif
        }
        /* Normal output character (not a format specifier); send to output */
        else
        {
            OS_OutputSingleChar(out, *format);
            pc++;
        }
    }

    if (out)
    {
        **out = '\0';
    }

    return pc;
}

/*****************************************************************************/
/**
** \brief OS_sprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_sprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_sprintf(char *out, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, UT_OFFSET_OS_SPRINTF, UT_BREAK_OS_SPRINTF,
             UT_SKIP_OS_SPRINTF);
    length = OS_vsnprintf(out, -1, format, varg);
    VA_END(varg);
    return(length);
}

/*****************************************************************************/
/**
** \brief OS_snprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_snprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  max_len      Maximum allowed length of the output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_snprintf(char *out, unsigned max_len, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, UT_OFFSET_OS_SNPRINTF, UT_BREAK_OS_SNPRINTF,
             UT_SKIP_OS_SNPRINTF);
    length = OS_vsnprintf(out, (int) max_len - 1, format, varg);
    VA_END(varg);
    return(length);
}
#endif
