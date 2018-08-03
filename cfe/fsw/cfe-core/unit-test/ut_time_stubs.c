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
** $Id: ut_time_stubs.c 1.5 2014/05/28 09:21:47GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for Time routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.5 $
** $Log: ut_time_stubs.c  $
** Revision 1.5 2014/05/28 09:21:47GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.4 2012/10/01 18:47:26EDT aschoeni 
** Removed relative paths in includes, this is now done by makefile
** Revision 1.3 2012/01/13 13:59:32EST acudmore 
** Added license text
** Revision 1.2 2008/08/15 11:26:55EDT njyanchik 
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:48BST ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.3 2007/05/01 13:28:14EDT njyanchik 
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.2 2007/01/17 09:26:07EST njyanchik 
** Check in of ES Unit Test changed files
** Revision 1.1 2006/03/02 15:10:27GMT-05:00 jjhageman 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_time.h"
#include "cfe_time_utils.h"
#include "ut_stubs.h"

extern UT_SetRtn_t TIMECleanUpRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_TIME_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_EarlyInit.  It always returns OS_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_TIME_EarlyInit(void)
{
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_TIME_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE TIME function
**        CFE_TIME_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TIME_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_TIME_Print stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_Print.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    snprintf(PrintBuffer,
            CFE_TIME_PRINTED_STRING_SIZE,
             "UT %lu.%lu -",
             (unsigned long)TimeToPrint.Seconds,
             (unsigned long)TimeToPrint.Subseconds);
}

/*****************************************************************************/
/**
** \brief CFE_TIME_GetTime stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_GetTime.  It increments the time structure values each time
**        it's called to provide a non-static time value for the unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the time structure.
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_GetTime(void)
{
    static CFE_TIME_SysTime_t time;

    time.Seconds++;
    time.Subseconds++;
    return time;
}

/*****************************************************************************/
/**
** \brief CFE_TIME_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TIME function
**        CFE_TIME_CleanUpApp.  The user can adjust the response by setting
**        the values in the TIMECleanUpRtn structure prior to this function
**        being called.  If the value TIMECleanUpRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value TIMECleanUpRtn.value.
**        A positive value, indicating success, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TIME_CleanUpApp(uint32 AppId)
{
    int32 status = CFE_SUCCESS;

    if (TIMECleanUpRtn.count > 0)
    {
        TIMECleanUpRtn.count--;

        if (TIMECleanUpRtn.count == 0)
        {
            status = TIMECleanUpRtn.value;
        }
    }

    return status;
}
