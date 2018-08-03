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
#include "ut_osprintf_stubs.h"

#ifdef SOCKET_QUEUE
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

/*
** Macro definitions
*/
#define OS_BASE_PORT 43000
#define MAX_PRIORITY 255

typedef struct
{
    char    name[OS_MAX_API_NAME];
    int     id;
    boolean free;
    char    data[100];
    int     size;
} UT_Queue_t;

/*
** External global variables
*/
extern char   cMsg[];

UT_Queue_t UT_Queue[OS_MAX_QUEUES];

/*
 * The following are now instantiated as part of the OSAL UT stub library,
 * in the same source file that actually implements the stub, rather than
 * instantiating it here.
 */

char    UT_ReadBuf[100000];
int32   UT_ReadBufOffset;
int     UT_lognum;
int     UT_DummyFuncRtn;
uint32  UT_BinSemFail = 0;
uint32  UT_OS_Fail = OS_NO_FAIL;

UT_SetRtn_t BinSemDelRtn;
UT_SetRtn_t BlocksFreeRtn;
UT_SetRtn_t CountSemDelRtn;
UT_SetRtn_t FileWriteRtn;
UT_SetRtn_t HeapGetInfoRtn;
UT_SetRtn_t ModuleInfoRtn;
UT_SetRtn_t ModuleLoadRtn;
UT_SetRtn_t ModuleUnloadRtn;
UT_SetRtn_t MutSemCreateRtn;
UT_SetRtn_t MutSemDelRtn;
UT_SetRtn_t MutSemGiveRtn;
UT_SetRtn_t MutSemTakeRtn;
UT_SetRtn_t OS_BinSemCreateRtn;
UT_SetRtn_t OSBinSemFlushRtn;
UT_SetRtn_t OSBinSemTimedWaitRtn;
UT_SetRtn_t OSCloseRtn;
UT_SetRtn_t OSlseekRtn;
UT_SetRtn_t OSReadRtn;
UT_SetRtn_t OSReadRtn2;
UT_SetRtn_t OSTaskExitRtn;
UT_SetRtn_t OSTimerGetInfoRtn;
UT_SetRtn_t QueueCreateRtn;
UT_SetRtn_t QueueDelRtn;
UT_SetRtn_t QueueGetRtn;
UT_SetRtn_t QueuePutRtn;
UT_SetRtn_t SymbolLookupRtn;
UT_SetRtn_t UnmountRtn;

/*
** Functions
*/
/**
 * Initialization function
 */
int32 OS_API_Init (void)
{
   uint32 i;

   /* Reset the UT_Queue structure */
   memset(UT_Queue, 0, sizeof(UT_Queue));

   for(i=0; i < OS_MAX_QUEUES; ++i)
   {
      UT_Queue[i].free = TRUE;
   }

   /* Reset all the existing-style deferred retcode structs */
   UT_OS_Fail = OS_NO_FAIL;
   UT_SetRtnCode(&QueuePutRtn, 0, 0);
   UT_SetRtnCode(&MutSemGiveRtn, 0, 0);
   UT_SetRtnCode(&MutSemTakeRtn, 0, 0);
   UT_SetRtnCode(&QueueCreateRtn, 0, 0);
   UT_SetRtnCode(&QueueDelRtn, 0, 0);
   UT_SetRtnCode(&QueueGetRtn, 0, 0);
   UT_SetRtnCode(&FileWriteRtn, 0, 0);
   UT_SetRtnCode(&OSReadRtn, 0, 0);
   UT_SetRtnCode(&OSReadRtn2, 0, 0);
   UT_SetRtnCode(&OS_BinSemCreateRtn, 0, 0);
   UT_SetRtnCode(&MutSemCreateRtn, 0, 0);
   UT_SetRtnCode(&OSlseekRtn, 0, 0);
   UT_SetRtnCode(&CountSemDelRtn, 0, 0);
   UT_SetRtnCode(&MutSemDelRtn, 0, 0);
   UT_SetRtnCode(&BinSemDelRtn, 0, 0);
   UT_SetRtnCode(&BlocksFreeRtn, 0, 0);
   UT_SetRtnCode(&UnmountRtn, 0, 0);
   UT_SetRtnCode(&ModuleLoadRtn, 0, 0);
   UT_SetRtnCode(&ModuleUnloadRtn, 0, 0);
   UT_SetRtnCode(&ModuleInfoRtn, 0, 0);
   UT_SetRtnCode(&SymbolLookupRtn, 0, 0);
   UT_SetRtnCode(&HeapGetInfoRtn, 0, 0);
   UT_SetRtnCode(&OSTaskExitRtn, 0, 0);
   UT_SetRtnCode(&OSBinSemTimedWaitRtn, -1, 0);
   UT_SetRtnCode(&OSBinSemFlushRtn, -1, 0);
   UT_SetRtnCode(&OSCloseRtn, 0, 0);
   UT_SetRtnCode(&OSTimerGetInfoRtn, 0, 0);

   return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_BinSemTake stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemTake.  The user can adjust the response by setting
**        the value of UT_BinSemFail, causing it to return a failure
**        indication (-1) on the first or second call to the function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either -1 or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemTake(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemTake called: %lu", sem_id);
    UT_Text(cMsg);
#endif

    /* Enable a failure return on 1st or 2nd second call.  Used to call
     * CFE_TIME_Local1HzTask and get coverage
     */
    if (UT_BinSemFail == 2)
    {
        UT_BinSemFail = 1;
    }
    else
    {
        if (UT_BinSemFail == 1)
        {
            UT_BinSemFail = 0;
            status = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_close stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_close.  The user determines the response by setting the value of
**        UT_OS_Fail prior to this function being called.  The values in the
**        structure OSCloseRtn are updated and subsequently used by the unit
**        tests to determine if the proper response was made to the calling
**        conditions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_ERROR or OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_close(int32 filedes)
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_CLOSE_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_close called: %ld (FAILURE)", filedes);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_close called: %ld (SUCCESS)", filedes);
        UT_Text(cMsg);
    }
#endif

    OSCloseRtn.value = status;
    OSCloseRtn.count++;
    return status;
}

/*****************************************************************************/
/**
** \brief OS_creat stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_creat.  The user determines the response by setting the value of
**        UT_OS_Fail prior to this function being called.  UT_lognum is
**        incremented each time this function is called; if UT_OS_Fail has not
**        been set to OS_CREAT_FAIL then UT_lognum is returned.  Otherwise
**        OS_FS_ERROR is returned.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_ERROR or the value of UT_lognum.
**
******************************************************************************/
/* Open local log file, increment reporting number on each call */
int32 OS_creat(const char *path, int32 access)
{
    int32 status;

    UT_lognum++;

    if (UT_OS_Fail & OS_CREAT_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_creat called: %d; call to open %s (FAILURE)",
                 UT_lognum, path);
        UT_Text(cMsg);
#endif
    }
    else
    {
        status = UT_lognum;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_creat called: %d; call to open %s (SUCCESS)",
                 UT_lognum, path);
        UT_Text(cMsg);
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_IntLock stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_IntLock.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_IntLock(void)
{
#if 0
#ifndef _ix86_
#error "IntDisableAll unit test stub may not work for your OS"
#endif
#endif

    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_IntUnlock stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_IntUnlock.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_IntUnlock(int32 IntLevel)
{
#if 0
#ifndef _ix86_
#error "IntEnableAll unit test stub may not work for your OS"
#endif
#endif

    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_MutSemGive stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemGive.  The user can adjust the response by setting
**        the values in the MutSemGiveRtn structure prior to this function
**        being called.  If the value MutSemGiveRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemGiveRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemGive(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemGiveRtn.count > 0)
    {
        MutSemGiveRtn.count--;

        if (MutSemGiveRtn.count == 0)
        {
            status = MutSemGiveRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemTake stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemTake.  The user can adjust the response by setting
**        the values in the MutSemTakeRtn structure prior to this function
**        being called.  If the value MutSemTakeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemTakeRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemTake(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemTakeRtn.count > 0)
    {
        MutSemTakeRtn.count--;

        if (MutSemTakeRtn.count == 0)
        {
            status = MutSemTakeRtn.value;
        }
    }

    return status;
}

#ifdef SOCKET_QUEUE
/*****************************************************************************/
/**
** \brief OS_QueueCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueCreate.  The user can adjust the response by setting
**        the values in the QueueCreateRtn structure prior to this function
**        being called.  If the value QueueCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueCreateRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        creating the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to the real call, doesn't care about creator or do any
**           mutex locking\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER,
**        OS_ERR_NAME_TOO_LONG, OS_ERR_NO_FREE_IDS, OS_ERR_NAME_TAKEN,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueCreate(uint32 *queue_id, const char *queue_name,
                     uint32 queue_depth, uint32 data_size, uint32 flags)
{
    int                tmpSkt;
    int                returnStat;
    struct sockaddr_in servaddr;
    int                i;
    uint32             possible_qid;
    int32              status = OS_SUCCESS;
    boolean            flag = FALSE;

    if (QueueCreateRtn.count > 0)
    {
        QueueCreateRtn.count--;

        if (QueueCreateRtn.count == 0)
        {
            status = QueueCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (queue_id == NULL || queue_name == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        /* Don't want to allow names too long; if truncated, two names might
         * be the same
         */
        else if (strlen(queue_name) >= OS_MAX_API_NAME)
        {
            status = OS_ERR_NAME_TOO_LONG;
        }
        else
        {
            /* Look for open queue */
            for (possible_qid = 0; possible_qid < OS_MAX_QUEUES;
                 possible_qid++)
            {
                if (UT_Queue[possible_qid].free == TRUE)
                {
                    break;
                }
            }

            if (possible_qid >= OS_MAX_QUEUES ||
                UT_Queue[possible_qid].free != TRUE)
            {
                status = OS_ERR_NO_FREE_IDS;
            }
            else
            {
                /* See if name is already taken */
                for (i = 0; i < OS_MAX_QUEUES; i++)
                {
                    if (strcmp((char*) queue_name, UT_Queue[i].name) == 0)
                    {
                        status = OS_ERR_NAME_TAKEN;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    tmpSkt = socket(AF_INET, SOCK_DGRAM, 0);
                    memset(&servaddr, 0, sizeof(servaddr));
                    servaddr.sin_family = AF_INET;
                    servaddr.sin_port = htons(OS_BASE_PORT + possible_qid);
                    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

                    /* Bind the input socket to a pipe; port numbers are
                     * OS_BASE_PORT + queue_id
                     */
                    returnStat = bind(tmpSkt, (struct sockaddr *) &servaddr,
                                      sizeof(servaddr));

                    if (returnStat == -1)
                    {
                        status = OS_ERROR;
#ifdef UT_VERBOSE
                        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                                 "  Bind failed on OS_QueueCreate, errno = %d",
                                 errno);
                        UT_Text(cMsg);
#endif
                    }
                    else
                    {
                        /* Store socket handle */
                        *queue_id = possible_qid;
                        UT_Queue[*queue_id].id = tmpSkt;
                        UT_Queue[*queue_id].free = FALSE;
                        strncpy(UT_Queue[*queue_id].name, (char*) queue_name,
                                OS_MAX_API_NAME);
                        UT_Queue[*queue_id].name[OS_MAX_API_NAME - 1] = '\0';
                    }
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueDelete.  The user can adjust the response by setting
**        the values in the QueueDelRtn structure prior to this function
**        being called.  If the value QueueDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueDelRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        deleting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to real code without mutex locking\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueDelete(uint32 queue_id)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueDelRtn.count > 0)
    {
        QueueDelRtn.count--;

        if (QueueDelRtn.count == 0)
        {
            status = QueueDelRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check to see if the queue_id given is valid */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        /* Try to delete the queue */
        else if (close(UT_Queue[queue_id].id) != 0)
        {
            status = OS_ERROR;
        }
        /* Now that the queue is deleted, remove its "presence"
         * in OS_message_q_table and OS_message_q_name_table
         */
        else
        {
            UT_Queue[queue_id].free = TRUE;
            strcpy(UT_Queue[queue_id].name, "");
            UT_Queue[queue_id].id = 0;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueGet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGet.  The user can adjust the response by setting
**        the values in the QueueGetRtn structure prior to this function
**        being called.  If the value QueueGetRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueGetRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        getting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Works similar to real function; note that pend on empty queue
**           doesn't block\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_EMPTY, OS_QUEUE_INVALID_SIZE,
**        OS_QUEUE_TIMEOUT, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGet(uint32 queue_id, void *data, uint32 size,
                  uint32 *size_copied, int32 timeout)
{
    int     sizeCopied;
    int     cntlFlags;
    int     timeloop;
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueGetRtn.count > 0)
    {
        QueueGetRtn.count--;

        if (QueueGetRtn.count == 0)
        {
            status = QueueGetRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
            flag = TRUE;
        }
        else
        {
            if ((data == NULL) || (size_copied == NULL))
            {
                status = OS_INVALID_POINTER;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            /* Read the socket for data */
            if (timeout == OS_PEND)
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags|O_NONBLOCK);
                sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                      NULL, NULL);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);

                if (sizeCopied == -1 && errno == EWOULDBLOCK)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
#ifdef UT_VERBOSE
                    UT_Text("WARNING: OS_QueueGet called as pend on an "
                            "empty queue;");
                    UT_Text(" normally would block but in UT environment "
                            "isn't valid");
#endif
                }
                else if (sizeCopied != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
            }
            else if (timeout == OS_CHECK)
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags|O_NONBLOCK);
                sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                      NULL, NULL);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);

                if (sizeCopied == -1 && errno == EWOULDBLOCK)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
                }
                else if (sizeCopied != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }

            }
            else /* Timeout */
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags | O_NONBLOCK);

                /* This "timeout" will check the socket for data every 100
                 * milliseconds up until the timeout value. Although this works
                 * fine for a desktop environment, it should be written more
                 * efficiently for a flight system.  The proper way will be to
                 * use select or poll with a timeout
                 */
                for (timeloop = timeout; timeloop > 0;
                     timeloop = timeloop - 100)
                {
                    sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                          NULL, NULL);

                    if (sizeCopied == size)
                    {
                        *size_copied = sizeCopied;
                        fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                        status = OS_SUCCESS;
                        flag = TRUE;
                        break;
                    }
                    else if (sizeCopied == -1 && errno == EWOULDBLOCK)
                    {
                        /* Sleep for 100 milliseconds */
                        usleep(100 * 1000);
                    }
                    else
                    {
                        *size_copied = 0;
                        fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                        status = OS_QUEUE_INVALID_SIZE;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                    status = OS_QUEUE_TIMEOUT;
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueuePut stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueuePut.  The user can adjust the response by setting
**        the values in the QueuePutRtn structure prior to this function
**        being called.  If the value QueuePutRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueuePutRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        putting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Same as real function\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_FULL, or OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
#else
int32 OS_QueuePut(uint32 queue_id, const void *data, uint32 size, uint32 flags)
#endif
{
    struct     sockaddr_in serva;
    static int socketFlags = 0;
    int        bytesSent = 0;
    int        tempSkt = 0;
    int32      status = OS_SUCCESS;
    boolean    flag = FALSE;

    if (QueuePutRtn.count > 0)
    {
        QueuePutRtn.count--;

        if (QueuePutRtn.count == 0)
        {
            status = QueuePutRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        else if (data == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        else
        {
            /* Specify the IP address and port number of destination */
            memset(&serva, 0, sizeof(serva));
            serva.sin_family = AF_INET;
            serva.sin_port = htons(OS_BASE_PORT + queue_id);
            serva.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            /* Open a temporary socket to transfer the packet to MR */
            tempSkt = socket(AF_INET, SOCK_DGRAM, 0);

            /* Send the packet to the message router task (MR) */
            bytesSent = sendto(tempSkt, (char *) data, size, socketFlags,
                               (struct sockaddr *) &serva, sizeof(serva));

            if (bytesSent != size)
            {
               status = OS_QUEUE_FULL;
            }
            else
            {
                /* Close socket */
                close(tempSkt);
            }
        }
    }

    return status;
}
#else /* Message queue without using sockets */
/*****************************************************************************/
/**
** \brief OS_QueueCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueCreate.  The user can adjust the response by setting
**        the values in the QueueCreateRtn structure prior to this function
**        being called.  If the value QueueCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueCreateRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        creating the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to the real call, doesn't care about creator or do any
**           mutex locking\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER,
**        OS_ERR_NAME_TOO_LONG, OS_ERR_NO_FREE_IDS, OS_ERR_NAME_TAKEN,
**        or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueCreate(uint32 *queue_id,
                     const char *queue_name,
                     uint32 queue_depth,
                     uint32 data_size,
                     uint32 flags)
{
    int     i;
    uint32  possible_qid;
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueCreateRtn.count > 0)
    {
        QueueCreateRtn.count--;

        if (QueueCreateRtn.count == 0)
        {
            status = QueueCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (queue_id == NULL || queue_name == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        /* Don't want to allow names too long; if truncated, two names might
         * be the same
         */
        else if (strlen(queue_name) >= OS_MAX_API_NAME)
        {
            status = OS_ERR_NAME_TOO_LONG;
        }
        else
        {
            /* Look for open queue */
            for (possible_qid = 0; possible_qid < OS_MAX_QUEUES;
                 possible_qid++)
            {
                if (UT_Queue[possible_qid].free == TRUE)
                {
                    break;
                }
            }

            if (possible_qid >= OS_MAX_QUEUES ||
                UT_Queue[possible_qid].free != TRUE)
            {
                status = OS_ERR_NO_FREE_IDS;
            }
            else
            {
                /* See if name is already taken */
                for (i = 0; i < OS_MAX_QUEUES; i++)
                {
                    if (strcmp((char*) queue_name, UT_Queue[i].name) == 0)
                    {
                        status = OS_ERR_NAME_TAKEN;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    *queue_id = possible_qid;
                    UT_Queue[*queue_id].id = possible_qid;
                    UT_Queue[*queue_id].free = FALSE;
                    strncpy(UT_Queue[*queue_id].name, (char*) queue_name,
                            OS_MAX_API_NAME);
                    UT_Queue[*queue_id].name[OS_MAX_API_NAME - 1] = '\0';
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueDelete.  The user can adjust the response by setting
**        the values in the QueueDelRtn structure prior to this function
**        being called.  If the value QueueDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueDelRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        deleting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to real code without mutex locking\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueDelete(uint32 queue_id)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueDelRtn.count > 0)
    {
        QueueDelRtn.count--;

        if (QueueDelRtn.count == 0)
        {
            status = QueueDelRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check to see if the queue_id given is valid */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        /* Now that the queue is deleted, remove its "presence"
         * in OS_message_q_table and OS_message_q_name_table
         */
        else
        {
            UT_Queue[queue_id].free = TRUE;
            strcpy(UT_Queue[queue_id].name, "");
            UT_Queue[queue_id].id = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueGet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGet.  The user can adjust the response by setting
**        the values in the QueueGetRtn structure prior to this function
**        being called.  If the value QueueGetRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueGetRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        getting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Works similar to real function; note that pend on empty queue
**           doesn't block\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_EMPTY, OS_QUEUE_INVALID_SIZE,
**        or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGet(uint32 queue_id,
                  void *data,
                  uint32 size,
                  uint32 *size_copied,
                  int32 timeout)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueGetRtn.count > 0)
    {
        QueueGetRtn.count--;

        if (QueueGetRtn.count == 0)
        {
            status = QueueGetRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
            flag = TRUE;
        }
        else
        {
            if ((data == NULL) || (size_copied == NULL))
            {
                status = OS_INVALID_POINTER;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            if (timeout == OS_PEND)
            {
                memcpy(data, UT_Queue[queue_id].data, size);

                if (UT_Queue[queue_id].size == 0)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
#ifdef UT_VERBOSE
                    UT_Text("WARNING: OS_QueueGet called as pend on an "
                            "empty queue;");
                    UT_Text(" normally would block but in UT environment "
                            "isn't valid");
#endif
                }
                else if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    UT_Queue[queue_id].size = 0;
                }
            }
            else if (timeout == OS_CHECK)
            {
                memcpy(data, UT_Queue[queue_id].data, size);

                if (UT_Queue[queue_id].size == 0)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
                }
                else if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    UT_Queue[queue_id].size = 0;
                }
            }
            else /* Timeout */
            {
                if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                     status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    *size_copied = UT_Queue[queue_id].size;
                    memcpy(data, UT_Queue[queue_id].data, size);
                    UT_Queue[queue_id].size = 0;
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueuePut stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueuePut.  The user can adjust the response by setting
**        the values in the QueuePutRtn structure prior to this function
**        being called.  If the value QueuePutRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueuePutRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        putting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Same as real function\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_FULL, or OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
#else
int32 OS_QueuePut(uint32 queue_id, const void *data, uint32 size, uint32 flags)
#endif
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueuePutRtn.count > 0)
    {
        QueuePutRtn.count--;

        if (QueuePutRtn.count == 0)
        {
            status = QueuePutRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        else if (data == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        else
        {
            UT_Queue[queue_id].size = size;
            memcpy(UT_Queue[queue_id].data, data, size);
        }
    }

    return status;
}
#endif /* End SOCKET_QUEUE */

/*****************************************************************************/
/**
** \brief OS_remove stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_remove.  It always returns 0.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 0.
**
******************************************************************************/
int32 OS_remove(const char *path)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH, "  OS_remove called: %s", path);
    UT_Text(cMsg);
#endif
    return 0;
}

/*****************************************************************************/
/**
** \brief OS_write stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_write.  The user can adjust the response by setting the values
**        in the FileWriteRtn structure prior to this function being called.
**        If the value FileWriteRtn.count is greater than zero then the counter
**        is decremented; if it then equals zero the return value is set to the
**        user-defined value FileWriteRtn.value.  Alternately, the user can
**        cause the function to return a failure result, OS_FS_ERROR, by
**        setting the value of UT_OS_Fail to OS_WRITE_FAIL prior to this
**        function being called.  The input value, nbytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        Only outputs SUCCESS or FAILURE since packet structure isn't known
**
** \returns
**        Returns either a user-defined status flag, OS_FS_ERROR, or the value
**        of the input variable, nbytes.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_write(int32 filedes, void *buffer, uint32 nbytes)
#else
int32 OS_write(int32 filedes, const void *buffer, uint32 nbytes)
#endif
{
    int32   status;
    boolean flag = FALSE;

    if (FileWriteRtn.count > 0)
    {
        FileWriteRtn.count--;

        if (FileWriteRtn.count == 0)
        {
            status = FileWriteRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_WRITE_FAIL)
        {
            status = OS_FS_ERROR;
#ifdef UT_VERBOSE
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_write called: %lu (FAILURE)", filedes);
            UT_Text(cMsg);
#endif
        }
        else
        {
            status = nbytes;
#ifdef UT_VERBOSE
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_write called: %lu (SUCCESS)", filedes);
            UT_Text(cMsg);
#endif
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemFlush stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemFlush.  The variable OSBinSemFlushRtn.value is set to the
**        value passed to the function, reset_type, and the variable
**        OSBinSemFlushRtn.count is incremented each time this function is
**        called.  The unit tests compare these values to expected results to
**        verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemFlush(uint32 sem_id)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemFlush called: %lu", sem_id);
    UT_Text(cMsg);
#endif
    OSBinSemFlushRtn.value = sem_id;
    OSBinSemFlushRtn.count++;
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_TaskRegister stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskRegister.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskRegister(void)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKREGISTER_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskRegister called (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskRegister called (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskCreate.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskCreate(uint32 *task_id, const char *task_name,
                    osal_task_entry function_pointer,
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
                    const uint32 *stack_pointer,
#else
                    uint32 *stack_pointer,
#endif
                    uint32 stack_size, uint32 priority,
                    uint32 flags)
{
    int32 status = OS_SUCCESS;

    *task_id = 1;

    if (UT_OS_Fail & OS_TASKCREATE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_TaskCreate called: %s%lu (FAILURE)",
                 task_name, *task_id);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_TaskCreate called: %s%lu (SUCCESS)",
                 task_name, *task_id);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskGetId stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_TaskGetId.  It always returns 1.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 1.
**
******************************************************************************/
uint32 OS_TaskGetId(void)
{
#ifdef UT_VERBOSE
    UT_Text("  OS_TaskGetId called");
#endif
    return 1;
}

/*****************************************************************************/
/**
** \brief OS_TaskGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskGetInfo.  The variable TaskGetInfoRtn.value is set to the
**        value passed to the function, reset_type, and the variable
**        TaskGetInfoRtn.count is incremented each time this function is
**        called.  The unit tests compare these values to expected results to
**        verify proper system response.  If TaskGetInfoRtn.count is set to
**        zero the if the input structure, task_prop, is null, it returns
**        OS_INVALID_POINTER.  Otherwise it sets the task structure variables
**        to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_INVALID_POINTER or OS_SUCCESS.
**
******************************************************************************/
int32 OS_TaskGetInfo(uint32 task_id, OS_task_prop_t *task_prop)
{
    int32 status = OS_SUCCESS;
    boolean flag = FALSE;

    if (TaskGetInfoRtn.count > 0)
    {
        TaskGetInfoRtn.count--;

        if (TaskGetInfoRtn.count == 0)
        {
            status = TaskGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (task_prop == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        else
        {
            task_prop->creator = 0;
            task_prop->OStask_id = OS_MAX_TASKS - 1;
            task_prop->stack_size = 100;
            task_prop->priority = 150;
            strncpy(task_prop->name, "UnitTest", OS_MAX_API_NAME);
            task_prop->name[OS_MAX_API_NAME] = '\0';
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_mkfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_mkfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_mkfs(char *address, char *devname, char * volname, uint32 blocksize,
#else
int32 OS_mkfs(char *address, const char *devname, const char * volname, uint32 blocksize,
#endif
              uint32 numblocks)
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_MKFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mkfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mkfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_rmfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_rmfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_rmfs(char *devname)
#else
int32 OS_rmfs(const char *devname)
#endif
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_RMFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_rmfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_rmfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_mount stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_mount.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_mount(const char *devname, char* mountpoint)
#else
int32 OS_mount(const char *devname, const char* mountpoint)
#endif
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_MOUNT_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mount called: devname=%s, mountpoint=%s (FAILURE)",
                 devname, mountpoint);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mount called: devname=%s, mountpoint=%s (SUCCESS)",
                 devname, mountpoint);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_initfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_initfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_initfs(char *address, char *devname, char *volname,
#else
int32 OS_initfs(char *address, const char *devname, const char *volname,
#endif
                uint32 blocksize, uint32 numblocks)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_INITFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_initfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_initfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ShellOutputToFile stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_ShellOutputToFile.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_ShellOutputToFile(char* Cmd, int32 OS_fd)
#else
int32 OS_ShellOutputToFile(const char* Cmd, int32 OS_fd)
#endif
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
            "  OS_ShellOutput to file called: %s", Cmd);
    UT_Text(cMsg);
#endif
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_read stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_read.  The user can adjust the response by setting the values
**        in the OSReadRtn structure prior to this function being called.  If
**        the value OSReadRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value OSReadRtn.value.  Alternately, the user can adjust
**        the response by setting the values in the OSReadRtn2 structure prior
**        to this function being called.  If the value OSReadRtn2.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        OSReadRtn2.value.  Finally, the user can cause the function to return
**        a failure result, nbytes - 1, by setting the value of UT_OS_Fail to
**        OS_READ_FAIL prior to this function being called.  The input value,
**        nbytes, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either one of two user-defined status flags, the value
**        of the input variable, nbytes, - 1 (failure case), or the value of
**        the input variable, nbytes (success case).
**
******************************************************************************/
int32 OS_read(int32 filedes, void *buffer, uint32 nbytes)
{
    int32   status;
    boolean flag = FALSE;

    if (OSReadRtn.count > 0)
    {
        OSReadRtn.count--;

        if (OSReadRtn.count == 0)
        {
            status = OSReadRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (OSReadRtn2.count > 0)
        {
            OSReadRtn2.count--;

            if (OSReadRtn2.count == 0)
            {
                status = OSReadRtn2.value;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            if (UT_OS_Fail & OS_READ_FAIL)
            {
                status = nbytes - 1;
            }
            else
            {
                memcpy(buffer, &UT_ReadBuf[UT_ReadBufOffset], nbytes);
                UT_ReadBufOffset += nbytes;
                status = nbytes;
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_lseek stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_lseek.  The user can adjust the response by setting the values
**        in the OSlseekRtn structure prior to this function being called.  If
**        the value OSlseekRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value OSlseekRtn.value.  Alternately, the user can cause
**        the function to return a failure result, OS_FS_ERROR, by setting the
**        value of UT_OS_Fail to OS_LSEEK_FAIL prior to this function being
**        called.  The input value, offset, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_FS_ERROR, or the value
**        of the input variable, offset.
**
******************************************************************************/
int32 OS_lseek(int32 filedes, int32 offset, uint32 whence)
{
    int32   status;
    boolean flag = FALSE;

    status = offset;

    if (OSlseekRtn.count > 0)
    {
        OSlseekRtn.count--;

        if (OSlseekRtn.count == 0)
        {
            status = OSlseekRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_LSEEK_FAIL)
        {
            status = OS_FS_ERROR;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemCreate.  The user can adjust the response by setting the
**        values in the OS_BinSemCreateRtn structure prior to this function
**        being called.  If the value OS_BinSemCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        OS_BinSemCreateRtn.value.  Alternately, the user can cause the
**        function to return a failure result, OS_ERROR, by setting the value
**        of UT_OS_Fail to OS_SEMCREATE_FAIL prior to this function being
**        called.  OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemCreate(uint32 *sem_id, const char *sem_name,
                      uint32 sem_initial_value, uint32 options)
{
    int32      status = OS_SUCCESS;
    static int sem_ctr = 0;
#ifdef UT_VERBOSE
    boolean    flag = FALSE;
#endif

    *sem_id = sem_ctr;
    sem_ctr++;

    if (UT_OS_Fail & OS_SEMCREATE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_BinSemCreate called: %d (FAILURE)", sem_ctr);
        UT_Text(cMsg);
#endif
    }
    else
    {
        if (OS_BinSemCreateRtn.count > 0)
        {
            OS_BinSemCreateRtn.count--;

            if (OS_BinSemCreateRtn.count == 0)
            {
                status = OS_BinSemCreateRtn.value;
#ifdef UT_VERBOSE
                flag = TRUE;
#endif
            }
        }

#ifdef UT_VERBOSE
        if (flag == FALSE)
        {
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_BinSemCreate called: %d (SUCCESS)", sem_ctr);
            UT_Text(cMsg);
        }
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemCreate.  The user can adjust the response by setting the
**        values in the MutSemCreateRtn structure prior to this function being
**        called.  If the value MutSemCreateRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value MutSemCreateRtn.value.
**        Alternately, the user can cause the function to return a failure
**        result, OS_ERROR, by setting the value of UT_OS_Fail to
**        OS_MUTCREATE_FAIL prior to this function being called.  OS_SUCCESS
**        is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemCreate(uint32 *sem_id, const char *sem_name, uint32 options)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (MutSemCreateRtn.count > 0)
    {
        MutSemCreateRtn.count--;

        if (MutSemCreateRtn.count == 0)
        {
            status = MutSemCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_MUTCREATE_FAIL)
        {
            status = OS_ERROR;
#ifdef UT_VERBOSE
            UT_Text("  OS_MutSemCreate called: (FAILURE)");
#endif
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_open stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_open.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_ERROR or a value that is incremented by 1 each
**        time the function is called.
**
******************************************************************************/
int32 OS_open(const char *path, int32 access, uint32 mode)
{
    int32      status;
    static int NumOpen = 1;

    if (UT_OS_Fail & OS_OPEN_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_open called: (FAILURE)");
#endif
    }
    else
    {
        status = NumOpen;
        NumOpen++;
#ifdef UT_VERBOSE
        UT_Text("  OS_open called: (SUCCESS)");
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskDelay stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskDelay.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskDelay(uint32 millisecond)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKDELAY_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskDelay called: (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskDelay called: (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemGive stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemGive.  The user can adjust the response by setting the value
**        of UT_BinSemFail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the user-defined value UT_BinSemFail.
**
******************************************************************************/
int32 OS_BinSemGive(uint32 sem_id)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemGive called: %lu", sem_id);
    UT_Text(cMsg);
#endif
    return UT_BinSemFail;
}

/*****************************************************************************/
/**
** \brief OS_BinSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemGetInfo.  The user can adjust the response by setting the
**        values in the BinSemGetInfoRtn structure prior to this function being
**        called.  If the value BinSemGetInfoRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value BinSemGetInfoRtn.value.
**        Otherwise it sets the binary semaphore structure variables to fixed
**        values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemGetInfo(uint32 sem_id, OS_bin_sem_prop_t *bin_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (BinSemGetInfoRtn.count > 0)
    {
        BinSemGetInfoRtn.count--;

        if (BinSemGetInfoRtn.count == 0)
        {
            status = BinSemGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        bin_prop->creator =  0;
        strncpy(bin_prop->name, "Name", OS_MAX_API_NAME + 1);
        bin_prop->name[OS_MAX_API_NAME] = '\0';
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemGetInfo.  The user can adjust the response by setting the
**        values in the MutSemGetInfoRtn structure prior to this function being
**        called.  If the value MutSemGetInfoRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value MutSemGetInfoRtn.value.
**        Otherwise it sets the mutex semaphore structure variables to fixed
**        values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemGetInfo(uint32 sem_id, OS_mut_sem_prop_t *mut_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (MutSemGetInfoRtn.count > 0)
    {
        MutSemGetInfoRtn.count--;

        if (MutSemGetInfoRtn.count == 0)
        {
            status = MutSemGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        strncpy(mut_prop->name, "Name", OS_MAX_API_NAME + 1);
        mut_prop->name[OS_MAX_API_NAME] = '\0';
        mut_prop->creator =  1;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_CountSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_CountSemGetInfo.  The user can adjust the response by setting the
**        values in the CountSemGetInfoRtn structure prior to this function
**        being called.  If the value CountSemGetInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        CountSemGetInfoRtn.value.  Otherwise it sets the counting semaphore
**        structure variables to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_CountSemGetInfo(uint32 sem_id, OS_count_sem_prop_t *count_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (CountSemGetInfoRtn.count > 0)
    {
        CountSemGetInfoRtn.count--;

        if (CountSemGetInfoRtn.count == 0)
        {
            status = CountSemGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        count_prop->creator =  0;
        strncpy(count_prop->name, "Name", OS_MAX_API_NAME + 1);
        count_prop->name[OS_MAX_API_NAME] = '\0';
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGetInfo.  The user can adjust the response by setting the
**        values in the QueueGetInfoRtn structure prior to this function being
**        called.  If the value QueueGetInfoRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value QueueGetInfoRtn.value.
**        Otherwise it sets the queue structure variables to fixed
**        values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGetInfo(uint32 sem_id, OS_queue_prop_t *queue_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueGetInfoRtn.count > 0)
    {
        QueueGetInfoRtn.count--;

        if (QueueGetInfoRtn.count == 0)
        {
            status = QueueGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        queue_prop->creator =  0;
        strncpy(queue_prop->name, "Name", OS_MAX_API_NAME + 1);
        queue_prop->name[OS_MAX_API_NAME] = '\0';
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemDelete.  The user can adjust the response by setting
**        the values in the MutSemDelRtn structure prior to this function
**        being called.  If the value MutSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemDelRtn.count > 0)
    {
        MutSemDelRtn.count--;

        if (MutSemDelRtn.count == 0)
        {
            status = MutSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemDelete.  The user can adjust the response by setting
**        the values in the BinSemDelRtn structure prior to this function
**        being called.  If the value BinSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BinSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (BinSemDelRtn.count > 0)
    {
        BinSemDelRtn.count--;

        if (BinSemDelRtn.count == 0)
        {
            status = BinSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_CountSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_CountSemDelete.  The user can adjust the response by setting
**        the values in the CountSemDelRtn structure prior to this function
**        being called.  If the value CountSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value CountSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_CountSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (CountSemDelRtn.count > 0)
    {
        CountSemDelRtn.count--;

        if (CountSemDelRtn.count == 0)
        {
            status = CountSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_FDGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_FDGetInfo.  The user can adjust the response by setting the
**        values in the FDGetInfoRtn structure prior to this function being
**        called.  If the value FDGetInfoRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value FDGetInfoRtn.value.
**        Otherwise it sets the table entry structure variables to fixed
**        values and returns OS_FS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_FDGetInfo(int32 filedes, OS_FDTableEntry *fd_prop)
{
    OS_FDTableEntry fd_prop2;
    int32           status = OS_SUCCESS;
    boolean         flag = FALSE;

    if (FDGetInfoRtn.count > 0)
    {
        FDGetInfoRtn.count--;

        if (FDGetInfoRtn.count == 0)
        {
            status = FDGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        fd_prop2.User = 0;
        strncpy(fd_prop2.Path, "PATH", OS_MAX_PATH_LEN);
        fd_prop2.Path[OS_MAX_PATH_LEN - 1] = '\0';
        fd_prop2.IsValid = TRUE;
        *fd_prop = fd_prop2;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskDelete.  The user can adjust the response by
**        setting the value of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskDelete(uint32 task_id)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKDELETE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskDelete called (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskDelete called (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_fsBlocksFree stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_fsBlocksFree.  The user can adjust the response by setting
**        the values in the BlocksFreeRtn structure prior to this function
**        being called.  If the value BlocksFreeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BlocksFreeRtn.value.
**        100 is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or 100.
**
******************************************************************************/
int32 OS_fsBlocksFree(const char *name)
{
    int32 status = 100;

    if (BlocksFreeRtn.count > 0)
    {
        BlocksFreeRtn.count--;

        if (BlocksFreeRtn.count == 0)
        {
            status = BlocksFreeRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_unmount stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_unmount.  The user can adjust the response by setting
**        the values in the UnmountRtn structure prior to this function
**        being called.  If the value UnmountRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value UnmountRtn.value.
**        OS_FS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_unmount(const char *mountpoint)
{
    int32 status = OS_FS_SUCCESS;

    if (UnmountRtn.count > 0)
    {
        UnmountRtn.count--;

        if (UnmountRtn.count == 0)
        {
            status = UnmountRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskExit stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskExit.  The variable OSTaskExitRtn.value is set to 1 and the
**        variable OSTaskExitRtn.count is incremented each time this function
**        is called.  The unit tests compare these values to expected results
**        to verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_TaskExit()
{
#ifdef UT_VERBOSE
    UT_Text("  OS_TaskExit called");
#endif
    OSTaskExitRtn.value = 1;
    OSTaskExitRtn.count++;
}

/*****************************************************************************/
/**
** \brief dummy_function stub function
**
** \par Description
**        This function is used by the OS API function, OS_SymbolLookup, which
**        requires a valid function for which to report the address.  The user
**        defines the function's return value in the variable UT_DummyFuncRtn.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status value, UT_DummyFuncRtn.
**
******************************************************************************/
int32 dummy_function(void)
{
#ifdef UT_VERBOSE
    UT_Text("  dummy function called");
#endif
    return UT_DummyFuncRtn;
}

/*****************************************************************************/
/**
** \brief OS_SymbolLookup stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_SymbolLookup.  The user can adjust the response by setting
**        the values in the SymbolLookupRtn structure prior to this function
**        being called.  If the value SymbolLookupRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SymbolLookupRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_SymbolLookup(uint32 *symbol_address, char *symbol_name)
#else
int32 OS_SymbolLookup(cpuaddr *symbol_address, const char *symbol_name)
#endif
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (SymbolLookupRtn.count > 0)
    {
        SymbolLookupRtn.count--;

        if (SymbolLookupRtn.count == 0)
        {
            status = SymbolLookupRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        *symbol_address = (uint32) &dummy_function;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleLoad stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleLoad.  The user can adjust the response by setting
**        the values in the ModuleLoadRtn structure prior to this function
**        being called.  If the value ModuleLoadRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleLoadRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_ModuleLoad(uint32 *module_id, char *module_name, char *filename)
#else
int32 OS_ModuleLoad(uint32 *module_id, const char *module_name, const char *filename)
#endif
{
    int32 status = OS_SUCCESS;

    if (ModuleLoadRtn.count > 0)
    {
        ModuleLoadRtn.count--;

        if (ModuleLoadRtn.count == 0)
        {
            status = ModuleLoadRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleUnload stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleUnload.  The user can adjust the response by setting
**        the values in the ModuleUnloadRtn structure prior to this function
**        being called.  If the value ModuleUnloadRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleUnloadRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_ModuleUnload(uint32 module_id)
{
    int32 status = OS_SUCCESS;

    if (ModuleUnloadRtn.count > 0)
    {
        ModuleUnloadRtn.count--;

        if (ModuleUnloadRtn.count == 0)
        {
            status = ModuleUnloadRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleInfo.  The user can adjust the response by setting
**        the values in the ModuleInfoRtn structure prior to this function
**        being called.  If the value ModuleInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleInfoRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
#if !defined(OSAL_API_VERSION) || (OSAL_API_VERSION < 40192)
int32 OS_ModuleInfo(uint32 module_id, OS_module_record_t *module_info)
#else
int32 OS_ModuleInfo(uint32 module_id, OS_module_prop_t *module_info)
#endif
{
    int32 status = OS_SUCCESS;

    if (ModuleInfoRtn.count > 0)
    {
        ModuleInfoRtn.count--;

        if (ModuleInfoRtn.count == 0)
        {
            status = ModuleInfoRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemTimedWait stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemTimedWait.  The variable OSBinSemTimedWaitRtn.value is set
**        to the value passed to the function, reset_type, and the variable
**        OSBinSemTimedWaitRtn.count is incremented each time this function is
**        called.  The unit tests compare these values to expected results to
**        verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemTimedWait(uint32 sem_id, uint32 msecs)
{
    OSBinSemTimedWaitRtn.value = sem_id;
    OSBinSemTimedWaitRtn.count++;
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_HeapGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_HeapGetInfo.  If the input structure, heap_prop, is null then
**        OS_INVALID_POINTER is returned.  If not null, the user can adjust
**        the response by setting the values in the HeapGetInfoRtn structure
**        prior to this function being called.  If the value
**        HeapGetInfoRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value HeapGetInfoRtn.value. OS_SUCCESS is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER, or
**        OS_SUCCESS.
**
******************************************************************************/
int32 OS_HeapGetInfo(OS_heap_prop_t *heap_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (heap_prop == NULL)
    {
        status = OS_INVALID_POINTER;
    }
    else
    {
        if (HeapGetInfoRtn.count > 0)
        {
            HeapGetInfoRtn.count--;

            if (HeapGetInfoRtn.count == 0)
            {
                status = HeapGetInfoRtn.value;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            /* Return some random data */
            heap_prop->free_bytes = (uint32) 12345;
            heap_prop->free_blocks = (uint32) 6789;
            heap_prop->largest_free_block = (uint32) 100;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TimerGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TimerGetInfo.  The user can adjust the response by setting
**        the values in the TimerGetInfoRtn2 structure prior to this function
**        being called.  If the value TimerGetInfoRtn2.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value TimerGetInfoRtn2.value.
**        Otherwise the user can adjust the response by setting the values in
**        the OSTimerGetInfoRtn structure prior to this function being called.
**        If the value OSTimerGetInfoRtn.count is greater than zero then the
**        counter is decremented and the timer creator value is set to the
**        user-defined value OSTimerGetInfoRtn.value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_TimerGetInfo(uint32 timer_id, OS_timer_prop_t *timer_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (OSTimerGetInfoRtn.count > 0)
    {
        OSTimerGetInfoRtn.count--;

        if (OSTimerGetInfoRtn.count == 0)
        {
            status = OSTimerGetInfoRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (OSTimerGetInfoRtn.count > 0)
        {
            timer_prop->creator = OSTimerGetInfoRtn.value;
            OSTimerGetInfoRtn.count--;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TimerDelete stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_TimerDelete.  The user can adjust the response by setting
**        the values in the TimerDeleteRtn structure prior to this function
**        being called.  If the value TimerDeleteRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value TimerDeleteRtn.value.
**        Otherwise it always returns OS_ERR_INVALID_ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_ERR_INVALID_ID.
**
******************************************************************************/
int32 OS_TimerDelete(uint32 timer_id)
{
    int32   status = OS_ERR_INVALID_ID;

    if (TimerDeleteRtn.count > 0)
    {
        TimerDeleteRtn.count--;

        if (TimerDeleteRtn.count == 0)
        {
            status = TimerDeleteRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_IntAttachHandler stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_IntAttachHandler.  It always returns OS_ERR_NOT_IMPLEMENTED.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_ERR_NOT_IMPLEMENTED.
**
******************************************************************************/
int32 OS_IntAttachHandler(uint32 InterruptNumber,
                          osal_task_entry InterruptHandler,
                          int32 parameter)
{
    return OS_ERR_NOT_IMPLEMENTED;
}

/*****************************************************************************/
/**
** \brief OS_SelectTone stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_SelectTone.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_SelectTone(int16 Signal)
{
}

/*
** Report and close any sockets found open
*/
void UT_CheckForOpenSockets(void)
{
    int i;
    int InUse = 0;

    for (i = 0; i < OS_MAX_QUEUES; i++)
    {
        if (UT_Queue[i].free == FALSE)
        {
            InUse++;
#ifdef UT_VERBOSE
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  UT_Queue[%d].%s left open. ID=%d",
                     i, UT_Queue[i].name, UT_Queue[i].id);
            UT_Text(cMsg);
#ifdef SOCKET_QUEUE
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "   Closing socket ID %d, close returned %d",
                     UT_Queue[i].id, close(UT_Queue[i].id));
            UT_Text(cMsg);
#endif
#endif

            /* Clean up same as OS_QueueDelete stub */
            UT_Queue[i].free = TRUE;
            strcpy(UT_Queue[i].name, "");
            UT_Queue[i].id = 0;
        }
    }

#ifdef UT_VERBOSE
    if (InUse > 0)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH, "%d socket(s) open", InUse);
        UT_Text(cMsg);
    }
#endif
}

