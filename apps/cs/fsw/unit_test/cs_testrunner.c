 /*************************************************************************
 ** File:
 **   $Id: cs_testrunner.c 1.2 2017/02/16 15:33:08EST mdeschu Exp  $
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains the unit test runner for the CS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "cs_app_test.h"
#include "cs_app_cmds_test.h"
#include "cs_cmds_test.h"
#include "cs_compute_test.h"
#include "cs_eeprom_cmds_test.h"
#include "cs_memory_cmds_test.h"
#include "cs_table_cmds_test.h"
#include "cs_table_processing_test.h"
#include "cs_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    CS_App_Test_AddTestCases();
    CS_App_Cmds_Test_AddTestCases();
    CS_Cmds_Test_AddTestCases();
    CS_Compute_Test_AddTestCases();
    CS_Eeprom_Cmds_Test_AddTestCases();
    CS_Memory_Cmds_Test_AddTestCases();
    CS_Table_Cmds_Test_AddTestCases();
    CS_Table_Processing_Test_AddTestCases();
    CS_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
