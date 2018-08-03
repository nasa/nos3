 /*************************************************************************
 ** File:
 **   $Id: example_test_utils.h 1.1 2016/06/14 16:47:23EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes and global variables for the unit test utilities for the UT-Assert example application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: example_test_utils.h  $
 **   Revision 1.1 2016/06/14 16:47:23EDT czogby 
 **   Initial revision
 **   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "example_app.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include <time.h>

/*
 * Function Definitions
 */

void Example_Test_Setup(void);
void Example_Test_TearDown(void);

/************************/
/*  End of File Comment */
/************************/
