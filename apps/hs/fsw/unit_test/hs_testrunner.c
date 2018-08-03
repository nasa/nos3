 /*************************************************************************
 ** File:
 **   $Id: hs_testrunner.c 1.1 2016/06/24 14:31:55EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the HS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_testrunner.c  $
 **   Revision 1.1 2016/06/24 14:31:55EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "hs_app_test.h"
#include "hs_cmds_test.h"
#include "hs_custom_test.h"
#include "hs_monitors_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    HS_App_Test_AddTestCases();
    HS_Cmds_Test_AddTestCases();
    HS_Custom_Test_AddTestCases();
    HS_Monitors_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
