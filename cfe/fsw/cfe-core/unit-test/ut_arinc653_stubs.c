/*
**
**      Copyright (c) 2013, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
** $Id: ut_arinc653_stubs.c 1.1 2014/05/28 09:23:31GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for ARINC653 routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.1 $
** $Log: ut_arinc653_stubs.c  $
** Revision 1.1 2014/05/28 09:23:31GMT-05:00 wmoleski 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.00 2012/01/13 13:59:30CST rmcclune
** Baseline version
*/
/* Define CFE_ARINC653 when compiling using the ARINC653 version of the cFE */
#ifdef CFE_ARINC653

/*
** Includes
*/
#include "ut_stubs.h"
#include "ut_arinc653_stubs.h"
#include "common_types.h"

#ifndef CFE_LINUX
#include <TUTF.h>
#endif

/*
** Global variables
*/
int32 UT_LibInitRtn;

/*
** Functions
*/
/*
** Set the library initialization return code
*/
void UT_SetLibInitRtn(int32 LibInitRtn)
{
    UT_LibInitRtn = LibInitRtn;
}

/*
** Return the library initialization return code
*/
uint32 UT_LibInit(void)
{
    return UT_LibInitRtn;
}

/*
** CFS Library Initialization Routine
*/
int32 CFS_LibInit(void)
{
    return CFE_SUCCESS;
}

/*
** Initializes the SCH Library
*/
int32 SCH_LibInit(void)
{
    return CFE_SUCCESS;
}

/*
** Initialize the Scheduler CFS application
*/
int32 SCH_AppInit(void)
{
    return CFE_SUCCESS;
}

/*
** CFS Scheduler (SCH) Application Entry Point
*/
void SCH_AppMain(void)
{
}

/*
** Initialize the housekeeping application
*/
int32 HK_AppInit(void)
{
    return CFE_SUCCESS;
}

/*
** CFS Housekeeping (HK) application entry point
*/
void HK_AppMain(void)
{
}

/*
** CI initialization
*/
int32 CI_TaskInit(void)
{
    return CFE_SUCCESS;
}

/*
** Application entry point and main process loop
*/
void CI_AppMain(void)
{
}

/*
** TO initialization
*/
int32 TO_init(void)
{
    return CFE_SUCCESS;
}

/*
** Application entry point and main process loop
*/
void TO_AppMain(void)
{
}

/*
** Initialize all data used by SBN653 application
*/
int32 SBN653_InitApp(void)
{
    return CFE_SUCCESS;
}

/*
** SBN653 application's entry point and main loop
*/
void SBN653_AppMain(void)
{
}

#endif
