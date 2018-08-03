
void CI_AddTestCase(void);

/*
 * Filename: ci_testrunner.c
 *
 * Purpose: This file contains a unit test runner for the CI Application.
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
    CI_AddTestCase();
    return(UtTest_Run());
}

