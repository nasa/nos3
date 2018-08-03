
/*
 * Filename: utassert.c
 *
 *  Copyright © 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement. 
 *
 * Purpose: This file contains a standard set of asserts for use in unit tests.
 *
 */

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "uttools.h"

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

boolean UtAssert(boolean Expression, char *Description, char *File, uint32 Line)
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
        return(FALSE);
    }
}
