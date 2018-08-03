/*
**
** File: utassert.c
**
** $Id: utassert.c 1.1 2016/02/08 20:53:31EST rperera Exp  $
**
** Copyright 2012-2013 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: This code implements a standard set of asserts for use in unit tests.
**
** $Log: utassert.c  $
** Revision 1.1 2016/02/08 20:53:31EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.3 2015/06/16 16:14:04EDT sstrege 
** Added copyright information
** Revision 1.2 2015/03/10 15:21:43EDT czogby 
** Add Missing Functionality to UT Assert Library
** --- Added comments ---  czogby [2015/03/31 18:36:49Z]
** Added a #include and a #ifdef
**
*/

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "uttools.h"
#include <stdlib.h>

/*
 * Local Data
 */

uint32      UtAssertPassCount = 0;
uint32      UtAssertFailCount = 0;

/*
 * Function Definitions
 */

uint32 UtAssert_GetPassCount(void)
{
    return(UtAssertPassCount);
}

uint32 UtAssert_GetFailCount(void)
{
    return(UtAssertFailCount);
}

boolean UtAssert(boolean Expression, const char *Description, const char *File, uint32 Line)
{
    if (Expression) {
        #ifdef UT_VERBOSE
        printf("PASS: %s\n", Description);
        #endif
        UtAssertPassCount++;
        return(TRUE);
    }
    else {
        printf("FAIL: %s, File: %s, Line: %lu\n", Description, File, Line);
        UtAssertFailCount++;
        #ifdef UT_STOP_ON_ASSERT_FAILURE
        printf("********** Unit Test Aborted **********\n");
        exit(1);
        #endif
        return(FALSE);
    }
}
