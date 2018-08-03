
void CF_AddTestCase(void);

/*
 * Filename: cf_testrunner.c
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
 * Purpose: This file contains a unit test runner for the CF Application.
 *
 */

/*
 * Includes
 */

#include "uttest.h"

/*
 * Function Definitions
 */

int main(void)
{

    /* Call AddTestSuite or AddTestCase functions here */
    CF_AddTestCase();
    return(UtTest_Run());
}

