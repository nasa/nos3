 /*************************************************************************
 ** File:
 **   $Id: example_testrunner.c 1.1 2016/06/14 16:47:23EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the UT-Assert example application
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: example_testrunner.c  $
 **   Revision 1.1 2016/06/14 16:47:23EDT czogby 
 **   Initial revision
 **   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/unit_test/project.pj
 *************************************************************************/

#include "uttest.h"
#include "example_app_test.h"
#include "example_cmds_test.h"

int main(void)
{   
    Example_App_Test_AddTestCases();
    Example_Cmds_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */

/************************/
/*  End of File Comment */
/************************/
