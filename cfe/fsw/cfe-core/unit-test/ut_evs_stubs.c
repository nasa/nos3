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
** $Id: ut_evs_stubs.c 1.7 2014/05/28 09:21:49GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for Event Service routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.7 $
** $Log: ut_evs_stubs.c  $
** Revision 1.7 2014/05/28 09:21:49GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.6 2012/10/01 18:47:27EDT aschoeni 
** Removed relative paths in includes, this is now done by makefile
** Revision 1.5 2012/01/13 13:59:30EST acudmore 
** Added license text
** Revision 1.4 2011/11/30 15:45:29EST jmdagost 
** Replaced CFE_EVS_BIG_BUFFER_SIZE with 1024.
** Revision 1.3 2009/05/07 15:04:48EDT rmcgraw 
** DCRR 7366:1 Moved call to AddEventToHistory
** Revision 1.2 2009/04/23 09:27:56EDT rmcgraw 
** DCR7366:1 Comment out the fprintfs that indicate the function was called
** Revision 1.1 2008/04/17 08:05:45EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.7 2007/07/05 15:04:30EDT David Kobe (dlkobe) 
** Baseline following complete TBL unit testing
** Revision 1.6 2007/05/30 15:15:18EDT njyanchik 
** check in of ES Unit test
** Revision 1.5 2007/05/04 09:10:25EDT njyanchik 
** Check in of Time UT and related changes
** Revision 1.4 2007/05/01 13:28:23EDT njyanchik 
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.3 2007/01/17 09:26:08EST njyanchik 
** Check in of ES Unit Test changed files
** Revision 1.2 2006/04/25 10:17:04GMT-05:00 rjmcgraw 
** SendTimedEvent added
** Revision 1.1 2006/03/02 20:10:25GMT jjhageman 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_evs.h"
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_evs_task.h"
#include "ut_stubs.h"

/*
** Macro definitions
*/
#ifdef OSP_ARINC653
#define UT_OFFSET_CFE_EVS_SENDEVENT 4
#define UT_OFFSET_CFE_SENDEVENTWITHAPPID 5

#define UT_BREAK_CFE_EVS_SENDEVENT 5
#define UT_BREAK_CFE_SENDEVENTWITHAPPID 4

#define UT_SKIP_CFE_EVS_SENDEVENT 54
#define UT_SKIP_CFE_SENDEVENTWITHAPPID 56
#endif

/*
** External global variables
*/
extern char cMsg[];

extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t EVSCleanUpRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_EVS_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_EarlyInit(void)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE EVS function
**        CFE_EVS_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_EVS_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendEvent stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendEvent.  The user can adjust the response by setting
**        the values in the EVS_SendEventRtn structure prior to this function
**        being called.  If the value EVS_SendEventRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_SendEventRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendEvent(uint16 EventID,
                        uint16 EventType,
                        const char *Spec,
                        ...)
{
    int32   status = CFE_SUCCESS;
    boolean flag = FALSE;
#ifdef UT_VERBOSE
    char    BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH*2];
    va_list Ptr;

#ifdef OSP_ARINC653
    va_start(Ptr, Spec, UT_OFFSET_CFE_EVS_SENDEVENT,
             UT_BREAK_CFE_EVS_SENDEVENT, UT_SKIP_CFE_EVS_SENDEVENT);
#else
    va_start(Ptr, Spec);
#endif

    vsnprintf(BigBuf, CFE_EVS_MAX_MESSAGE_LENGTH, Spec, Ptr);
    va_end(Ptr);
#endif

    if (EVS_SendEventRtn.count > 0)
    {
        EVS_SendEventRtn.count--;

        if (EVS_SendEventRtn.count == 0)
        {
            status = EVS_SendEventRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        UT_AddEventToHistory(EventID);
        SendMsgEventIDRtn.value = EventID;
#ifdef UT_VERBOSE
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_EVS_SendEvent: %u, %u - %s",
                 EventID, EventType, BigBuf);
        UT_Text(cMsg);
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendTimedEvent stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendTimedEvent.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendTimedEvent(CFE_TIME_SysTime_t Time,
                             uint16 EventID,
                             uint16 EventType,
                             const char *Spec,
                             ...)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_Register stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_Register.  The user can adjust the response by setting
**        the values in the EVS_RegisterRtn structure prior to this function
**        being called.  If the value EVS_RegisterRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_RegisterRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_Register(void *Filters,
                       uint16 NumEventFilters,
                       uint16 FilterScheme)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (EVS_RegisterRtn.count > 0)
    {
        EVS_RegisterRtn.count--;
        if (EVS_RegisterRtn.count == 0)
        {
            status = EVS_RegisterRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_EVS_Register called: %ld", EVS_RegisterRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        UT_Text("  CFE_EVS_Register called");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_SendEventWithAppID stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_SendEventWithAppID.  The event ID, EventID, is added to the
**        unit test' event history.  The number of events can be retrieved by
**        the unit tests for comparison to expected values in order to detect
**        correct functionality.  The user can adjust the response by setting
**        the values in the EVS_SendEventRtn structure prior to this function
**        being called.  If the value EVS_SendEventRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVS_SendEventRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_SendEventWithAppID(uint16 EventID,
                                 uint16 EventType,
                                 uint32 AppID,
                                 const char *Spec,
                                 ...)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
    char    BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
    va_list Ptr;

#ifdef OSP_ARINC653
    va_start(Ptr, Spec, UT_OFFSET_CFE_SENDEVENTWITHAPPID,
             UT_BREAK_CFE_SENDEVENTWITHAPPID, UT_SKIP_CFE_SENDEVENTWITHAPPID);
#else
    va_start(Ptr, Spec);
#endif
    vsnprintf(BigBuf, CFE_EVS_MAX_MESSAGE_LENGTH, Spec, Ptr);
    va_end(Ptr);
#endif
    UT_AddEventToHistory(EventID);
  
    if (EVS_SendEventRtn.count > 0)
    {
        EVS_SendEventRtn.count--;

        if (EVS_SendEventRtn.count == 0)
        {
            status = EVS_SendEventRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_EVS_SendEvent from app %lu: %u, %u - %s",
                 AppID, EventID, EventType, BigBuf);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_EVS_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE EVS function
**        CFE_EVS_CleanUpApp.  The user can adjust the response by setting
**        the values in the EVSCleanUpRtn structure prior to this function
**        being called.  If the value EVSCleanUpRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value EVSCleanUpRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_EVS_CleanUpApp(uint32 AppId)
{
    int32 status = CFE_SUCCESS;

    if (EVSCleanUpRtn.count > 0)
    {
      EVSCleanUpRtn.count--;

      if (EVSCleanUpRtn.count == 0)
      {
          status = EVSCleanUpRtn.value;
      }
    }

    return status;
}
