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
**
** File:
** $Id: ut_stubs.h 1.10 2014/07/10 09:30:59GMT-05:00 rmcgraw Exp  $
**
** Purpose:
** Unit specification for Unit Test Stubs
**
** Notes:
** These routines contain a minimum amount of functionality required for
** unit testing full path coverage
**
** $Date: 2014/07/10 09:30:59GMT-05:00 $
** $Revision: 1.10 $
** $Log: ut_stubs.h  $
** Revision 1.10 2014/07/10 09:30:59GMT-05:00 rmcgraw
** DCR9772:2 Fix unit tests that broke when main files were checked in
** Revision 1.9 2014/05/28 10:21:52EDT wmoleski
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.8 2012/10/01 18:47:29EDT aschoeni
** Removed relative paths in includes, this is now done by makefile
** Revision 1.7 2012/01/13 13:59:32EST acudmore
** Added license text
** Revision 1.6 2010/10/20 12:40:44EDT jmdagost
** Added UseMutex element to Pool_t structure.
** Revision 1.5 2009/05/06 09:57:03EDT rmcgraw
** DCR7366:1 Increased event history from 10 to 64
** Revision 1.4 2009/04/28 16:25:15EDT rmcgraw
** DCR7366:1 Added utility UT_CheckForOpenSockets
** Revision 1.3 2009/04/01 16:15:16EDT rmcgraw
** DCR7366:1 Added 'getActual...' and DisplayPkt utilities
** Revision 1.2 2008/08/15 11:26:53EDT njyanchik
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:47BST ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.20 2007/09/21 19:02:57EDT David Kobe (dlkobe)
** Updated table unit tests to stop segmentation fault
** Revision 1.19 2007/07/08 22:31:27EDT dlkobe
** Enhanced CDS Simulation
** Revision 1.18 2007/07/05 15:04:31EDT dlkobe
** Baseline following complete TBL unit testing
** Revision 1.17 2007/05/25 09:17:56EDT njyanchik
** I added the rmfs call to the OSAL and updated the unit test stubs to match
** Revision 1.16 2007/05/24 15:48:09EDT njyanchik
** Update for ES Unit Test
** Revision 1.15 2007/05/23 10:54:59EDT njyanchik
** Update again to prevent loss of work
** Revision 1.14 2007/05/22 13:03:57EDT njyanchik
** Updated changes to not lose my work
** Revision 1.13 2007/05/16 11:14:36EDT njyanchik
** Update ES's unit test driver to match code for build 4.1
** Revision 1.10 2007/05/01 13:28:24EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.9 2007/03/02 15:18:04EST njyanchik
** Jonathan provided me with a newer tbl unit test to check in
** Revision 1.8 2007/01/18 13:16:10GMT-05:00 njyanchik
** I fixed an errant "." (dot)
** Revision 1.7 2007/01/17 10:04:20GMT-05:00 njyanchik
** This CP Checks in cleaned-up versions of the files checked in for the ES unit test
** Revision 1.6 2007/01/17 09:26:09GMT-05:00 njyanchik
** Check in of ES Unit Test changed files
** Revision 1.5 2006/11/02 13:53:57GMT-05:00 njyanchik
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.4 2006/11/01 12:46:54GMT-05:00 njyanchik
** Changed the Unit test to reflect the changes from removing the CDS functionality from TIME
** Revision 1.3 2006/06/08 14:11:20GMT-05:00 rjmcgraw
** Added support for function UT_SetRtnCode
** Revision 1.2 2006/05/17 13:56:45GMT jjhageman
** Added UT_Queue for use by the osapi stubs (OS_Queue create, get, set)
** Revision 1.1 2006/03/02 15:10:26EST jjhageman
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/
#ifndef _UT_STUBS_H_
#define _UT_STUBS_H_

/* Compiler flags:
   - Define UT_SHOW_PASS to display PASSED (and FAILED) test results;
     undefine to show only FAILED test results
   - Define UT_VERBOSE to display stub routine messages in unit test
     log files
   - Define SOCKET_QUEUE to use Linux socket queue functions; undefine
     to use an embedded queue replacement
   - Define CFE_LINUX if compiling under Linux; undefine when compiling
     on the GHS target
   - Define OS_USE_EMBEDDED_PRINTF to use the embedded (replacement) printf
     functions; undefine to use the standard printf library functions
*/

/*
** Includes
*/
#ifdef CFE_LINUX
#include <stdio.h>
#endif

#include "cfe.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "common_types.h"
#include "cfe_evs_task.h"
#include "cfe_es_global.h"
#include "cfe_es_cds.h"
#include "cfe_esmempool.h"
#include "cfe_time_utils.h"

#include "utassert.h"
#include "uttest.h"
#include "uttools.h"
#include "utbsp.h"
#include "utstubs.h"

/*
 * Use the stub implementations provided by the OSAL --
 * This also means that the UT_SetRtn_t type must match the OSAL defined one
 *  (which in turn functionally matches the original UT_SetRtn_t type)
 */

/*
 * Use the SetRtn structure from UT assert stubs
 */
typedef UT_Compat_SetRtn_t UT_SetRtn_t;


/*
** Macro definitions
*/
#define OS_NO_FAIL           0x0000
#define OS_CREAT_FAIL        0x0001
#define OS_WRITE_FAIL        0x0002
#define OS_CLOSE_FAIL        0x0004
#define OS_MKFS_FAIL         0x0008
#define OS_INITFS_FAIL       0x0010
#define OS_MOUNT_FAIL        0x0020
#define OS_TASKCREATE_FAIL   0x0040
#define OS_SEMCREATE_FAIL    0x0080
#define OS_MUTCREATE_FAIL    0x0100
#define OS_OPEN_FAIL         0x0200
#define OS_TASKDELAY_FAIL    0x0400
#define OS_TASKREGISTER_FAIL 0x0800
#define OS_READ_FAIL         0x1000
#define OS_LSEEK_FAIL        0x2000
#define OS_TASKDELETE_FAIL   0x4000
#define OS_RMFS_FAIL         0x8000

#define BSP_WRITECDS_FAIL      0x0001
#define BSP_READCDS_FAIL       0x0002
#define BSP_GETCDSSIZE_FAIL    0x0004
#define BSP_GETVOLDISKMEM_FAIL 0x0008

#define OS_PRINT_INCR 100

#define EVS_SYSLOG_OFFSET  100
#define FS_SYSLOG_OFFSET   200
#define SB_SYSLOG_OFFSET   300
#define TBL_SYSLOG_OFFSET  400
#define TIME_SYSLOG_OFFSET 500

#define UT_EVENT_HISTORY_SIZE 64
#define UT_CDS_SIZE           20000
#define UT_MAX_NUM_CDS        20
#define UT_MAX_MESSAGE_LENGTH 300

#define UT_LITTLE_ENDIAN 1
#define UT_BIG_ENDIAN    2

/* CDS type declarations */
#define CFE_ES_CDS_CHECK_PATTERN   0x5a5a
#define CFE_ES_CDS_BLOCK_USED      0xaaaa
#define CFE_ES_CDS_BLOCK_UNUSED    0xdddd
#define CFE_ES_CDS_NUM_BLOCK_SIZES 17

/* MIN_BLOCK_SIZE must be < 16 bytes */
#define CFE_ES_CDS_MIN_BLOCK_SIZE 8

/* Macro to add a test to the UT assert list */
#define UT_ADD_TEST(Func)       UtTest_Add(Func, NULL, NULL, #Func)

/* Macro that returns TRUE if the character provided is a number (including
** hexadecimal)
*/
#define isHex(chr) (((chr) >= '0' && (chr) <= '9') || \
                    ((chr) >= 'a' && (chr) <= 'f') || \
                    ((chr) >= 'A' && (chr) <= 'F'))


typedef struct
{
    uint32             NextHandle;
    CFE_ES_CDSHandle_t Handles[UT_MAX_NUM_CDS];
} UT_CDS_Map_t;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief Initialize unit test
**
** \par Description
**        Set up logging file, initialize variables, and perform endian check.
**        This function must be called prior to executing any of the cFE unit
**        tests.
**
** \par Assumptions, External Events, and Notes:
**        A log file is written only if CFE_LINUX is defined.
**
** \param[in] subsys  Pointer to subsystem ID character string
**
** \returns
**        This function does not return a value.
**
** \sa #UT_InitData, #UT_EndianCheck
**
******************************************************************************/
void UT_Init(char *subsys);

/*****************************************************************************/
/**
** \brief Initialize unit test variables
**
** \par Description
**        This initialization function should be called prior to each unit test
**        in order to set the variables to known states.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_ClearEventHistory, #UT_ResetPoolBufferIndex, #UT_SetOSFail,
** \sa #UT_SetBSPFail, #UT_SetRtnCode, #UT_ResetCDS
**
******************************************************************************/
void UT_InitData(void);

/*****************************************************************************/
/**
** \brief Reset pool buffer index
**
** \par Description
**        Reset pool buffer index to zero.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
/* Reset pool buffer array index */
void UT_ResetPoolBufferIndex(void);

/*****************************************************************************/
/**
** \brief Output text string
**
** \par Description
**        Output a text string to the log file or other target.
**
** \par Assumptions, External Events, and Notes:
**        If CFE_LINUX is not defined then a suitable target function for the
**        text output must be designated.
**
** \param[in] text  Pointer to character string to output
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
/* Output text string to log file */
void UT_Text(char *text);

/*****************************************************************************/
/**
** \brief Output single test's pass/fail status
**
** \par Description
**        Out the results and description for a single test, and update the
**        pass/fail counters.
**
** \par Assumptions, External Events, and Notes:
**
** \param[in] test      Equals 0 if the test failed; non-zero if the
**                      test passed
**
** \param[in] fun_name  Character string pointer to the name of the function
**                      being tested
**
** \param[in] info      Character string pointer to the description of the test
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void UT_Report(const char *file, uint32 line, boolean test, char *fun_name,
		       char *info);

/*****************************************************************************/
/**
** \brief Test pass/fail summary
**
** \par Description
**        Output total number of tests passed and failed.
**
** \par Assumptions, External Events, and Notes:
**        If CFE_LINUX is defined then the output is also sent to the console
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void UT_ReportFailures(void);

/*****************************************************************************/
/**
** \brief Send a message via the software bus
**
** \par Description
**        Does the required work to send a software bus message.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] msg_ptr  Pointer to the buffer that contains the software
**                     bus message
**
** \param[in] id       Message ID to put into the message header
**
** \param[in] code     Command code to include in the message
**
** \returns
**        This function does not return a value.
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetCmdCode, #CFE_SB_SendMsg
**
******************************************************************************/
void UT_SendMsg(CFE_SB_MsgPtr_t msg_ptr, CFE_SB_MsgId_t id, uint16 code);

/*****************************************************************************/
/**
** \brief Set the application ID returned by unit test stubs
**
** \par Description
**        Set the application ID value returned by the unit test CFS stub
**        functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] AppID_in  Application ID
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetAppID(int32 AppID_in);

/*****************************************************************************/
/**
** \brief Set the PutPool fail flag
**
** \par Description
**        Set the response returned by the CFE_ES_PutPoolBuf stub function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] fail  Failure condition flag
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_SetPutPoolFail(uint32 fail);

/*****************************************************************************/
/**
** \brief Generic function used to tell stub functions to return 'rtnVal' when
**        call 'cnt' = 0
**
** \par Description
**        In order to dictate the behavior of a call to a cFE function, the
**        stub functions can be set to respond with a specified  return value
**        after a specified number of calls to the stub function.
**
** \param[in] varPtr  Pointer to return code structure
**
** \param[in] rtnVal  Failure condition code or return value
**
** \param[in] cnt     Counter
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetRtnCode(UT_SetRtn_t *varPtr, int32 rtnVal, int32 cnt);

/*****************************************************************************/
/**
** \brief Set the BinSem fail flag
**
** \par Description
**        Set the response returned by the OS_BinSemGive stub function.  Also
**        used as a counter in the OS_BinSemTake stub function before returning
**        a fail condition.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] fail  Failure condition flag
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetBinSemFail(uint32 fail);

/*****************************************************************************/
/**
** \brief Set the OS fail flag
**
** \par Description
**        Set the response returned by the OS create, write, etc. stub
**        functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] fail  Failure condition flag
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetOSFail(uint32 fail);

/*****************************************************************************/
/**
** \brief Set the return code of CFE_PSP_GetResetArea and the data validation
**        signature
**
** \par Description
**        Set the return code of CFE_PSP_GetResetArea and the data validation
**        signature.
**
** \param[in] status       CFE_PSP_GetResetArea return code
**
** \param[in] Signature    Data validation signature used to verify data
**                         structure contents
**
** \param[in] ClockSignal  Clock signal type
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetStatusBSPResetArea(int32 status, uint32 Signature,
                              uint32 ClockSignal);

/*****************************************************************************/
/**
** \brief Set the contents of the buffer to read
**
** \par Description
**        Set the contents of the buffer to read; e.g., the startup script.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Buff      Pointer to information to place into the buffer
**
** \param[in] NumBytes  Length in bytes of the buffer contents
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetReadBuffer(void *Buff, int NumBytes);

/*****************************************************************************/
/**
** \brief Set the contents of the header to read
**
** \par Description
**        Set the contents of the header to read.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Hdr       Pointer to information to place into the header
**
** \param[in] NumBytes  Length in bytes of the header contents
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetReadHeader(void *Hdr, int NumBytes);

/*****************************************************************************/
/**
** \brief Set the dummy function return code
**
** \par Description
**        Used to set the return code when ES is starting tasks by pointers;
**        the dummy function is started instead of actually tasks.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Return  Dummy function return code
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetDummyFuncRtn(int Return);

/*****************************************************************************/
/**
** \brief Set the size of the ES reset area
**
** \par Description
**        Set the size of the ES reset area that will be returned by calls
**        to the CFE_PSP_GetResetArea stub function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Size  Reset area size
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_SetSizeofESResetArea(int32 Size);

/*****************************************************************************/
/**
** \brief Set the failure mode for various BSP stub functions
**
** \par Description
**        Set the failure mode for the CFE_PSP_WriteToCDS, CFE_PSP_ReadFromCDS,
**        CFE_PSP_GetCDSSize, and CFE_PSP_GetVolatileDiskMem stub functions in
**        order to induce a specific response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Fail  BSP failure mode code
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetBSPFail(uint32 Fail);

/*****************************************************************************/
/**
** \brief Set the CDS size returned by the BSP
**
** \par Description
**        Set the CDS size returned by the BSP.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Size  CDS size
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetCDSSize(int32 Size);

/*****************************************************************************/
/**
** \brief Set the SB total message length
**
** \par Description
**        Set the SB total message length to be returned by calls to the
**        CFE_SB_GetTotalMsgLength stub function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Size  SB message length
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetSBTotalMsgLen(int Size);

/*****************************************************************************/
/**
** \brief Set the CDS valid end flag
**
** \par Description
**        Set the CDS valid end flag used when the CFE_PSP_ReadFromCDS stub
**        function is called.  If set to TRUE then a valid end string is
**        used; otherwise an invalid one is used.  Only takes effect if the
**        UT_BSPCheckValidity flag is also set to TRUE (set via the
**        UT_SetCDSBSPCheckValidity function).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Truth  Set to TRUE to use a valid end string
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetCDSReadGoodEnd(boolean Truth);

/*****************************************************************************/
/**
** \brief Set the BSP check validity flag
**
** \par Description
**        Set the BSP check validity flag used in CFE_PSP_ReadFromCDS.
**        If TRUE, allows the CFE_PSP_ReadFromCDS to set the data to read.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Truth  Set to TRUE to use a valid end string
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetCDSBSPCheckValidity(boolean Truth);

/*****************************************************************************/
/**
** \brief Set the BSP CDS rebuild flag
**
** \par Description
**        Set the BSP CDS rebuild flag used in CFE_PSP_ReadFromCDS.
**        If TRUE, sets the CFE_PSP_ReadFromCDS data to read to a set pattern.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] Truth  Set to TRUE to set the data to read to a preset pattern
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetCDSRebuild(boolean Truth);

/*****************************************************************************/
/**
** \brief Set BSP time
**
** \par Description
**        Set the time to be returned by calls to the CFE_PSP_GetTime stub
**        function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] seconds    Time, seconds
**
** \param[in] microsecs  Time, microseconds
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetBSP_Time(uint32 seconds, uint32 microsecs);

/*****************************************************************************/
/**
** \brief Clear the event history
**
** \par Description
**        Clear the event history so that one test's event messages won't be
          confused with a subsequent test (some tests depend on an accurate
          count of the number of messages generated for that test).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_ClearEventHistory(void);

/*****************************************************************************/
/**
** \brief Add an event ID to the event history
**
** \par Description
**        Add an event ID to the event history when the CFE_EVS_SendEvent or
**        CFE_EVS_SendEventWithAppID stub functions are called.  Some tests
**        depend on detecting specific messages generated by that test.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] EventID  Event ID to add
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_AddEventToHistory(uint16 EventID);

/*****************************************************************************/
/**
** \brief Search the event history for a specified event ID
**
** \par Description
**        Search the event history for a specified event ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] EventIDToSearchFor  Event ID to search for in the event history
**
** \returns
**        TRUE if the event ID to search for is found in the event history;
**        FALSE if the event ID is not found.
**
******************************************************************************/
boolean UT_EventIsInHistory(uint16 EventIDToSearchFor);

/*****************************************************************************/
/**
** \brief Return number of events issued
**
** \par Description
**        Return number of events issued since the last UT_ClearEventHistory
**        call.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Number of events issued.
**
******************************************************************************/
uint16 UT_GetNumEventsSent(void);

/*****************************************************************************/
/**
** \brief Initialize the CDS
**
** \par Description
**         Initialize the CDS values to all zeroes in preparation for the
**         next test.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_ResetCDS(void);

/*****************************************************************************/
/**
** \brief Perform a test to determine endianess
**
** \par Description
**        Perform a test to determine endianess and set global flag
**        accordingly.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa
**
******************************************************************************/
void UT_EndianCheck(void);

/*****************************************************************************/
/**
** \brief Display the contents of a packet
**
** \par Description
**         Display the contents of a packet.  The contents is displayed as a
**         series of bytes in hexadecimal.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] ptr   Pointer to packet to display
**
** \param[in] size  Size of packet in bytes
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_DisplayPkt(CFE_SB_MsgPtr_t ptr, uint32 size);

/*****************************************************************************/
/**
** \brief Return the actual packet length
**
** \par Description
**        Return the actual packet length field from the specified packet.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] MsgPtr  Message packet
**
** \returns
**        Returns a pointer to the SB packet length.
**
******************************************************************************/
/*uint16 TODO*/int16 UT_GetActualPktLenField(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/**
** \brief Return the actual command code
**
** \par Description
**        Return the actual command code from the specified packet.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] MsgPtr  Message packet
**
** \returns
**        Returns the actual SB packet command code.
**
******************************************************************************/
/*uint16 TODO*/uint8 UT_GetActualCmdCodeField(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/**
** \brief Report and close any sockets found open
**
** \par Description
**        Determine if any sockets are open; if so, close them.  If UT_VERBOSE
**        is defined then output the socket status to the test log file.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_CheckForOpenSockets(void);

/*****************************************************************************/
/**
** \brief Compare two strings for equality
**
** \par Description
**        Determines if the two input strings are identical.
**
** \par Assumptions, External Events, and Notes:
**        The tilde (~) is used as a wildcard in the second string and is
**        considered to match the associated numeric character(s) in the first
**        string (includes hexadecimal values).
**
** \returns
**        Returns FALSE (0) if the strings do not match; otherwise returns
**        TRUE (1).
**
******************************************************************************/
uint8 UT_strcmp(char *str1, char *str2);

#endif /* __UT_STUBS_H_ */
