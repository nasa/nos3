 /*************************************************************************
 ** File:
 **   $Id: hs_test_utils.c 1.1 2016/06/24 14:31:55EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test utilities for the HS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_test_utils.c  $
 **   Revision 1.1 2016/06/24 14:31:55EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hs_test_utils.h"
#include "hs_app.h"

extern HS_AppData_t     HS_AppData;

/*
 * Function Definitions
 */

void HS_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&HS_AppData, 0, sizeof(HS_AppData_t));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end HS_Test_Setup */

void HS_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end HS_Test_TearDown */


/************************/
/*  End of File Comment */
/************************/
