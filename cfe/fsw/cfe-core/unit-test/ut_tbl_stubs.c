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
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "common_types.h"
#include "ut_stubs.h"

/*
** External global variables
*/
extern UT_SetRtn_t TBLEarlyInitRtn;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_TBL_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_EarlyInit.  The user can adjust the response by setting
**        the values in the TBLEarlyInitRtn structure prior to this function
**        being called.  If the value TBLEarlyInitRtn.count is non-zero then
**        the return value is set to -1.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either -1 or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_EarlyInit(void)
{
    int32 status = CFE_SUCCESS;

    if (TBLEarlyInitRtn.count != 0)
    {
        status = -1;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_TBL_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE TBL function
**        CFE_TBL_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TBL_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_TBL_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_CleanUpApp.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_CleanUpApp(uint32 AppId)
{
    return CFE_SUCCESS;
}
