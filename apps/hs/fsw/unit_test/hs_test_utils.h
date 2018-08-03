 /*************************************************************************
 ** File:
 **   $Id: hs_test_utils.h 1.1 2016/06/24 14:31:55EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes and global variables for the unit test utilities for the HS application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hs_test_utils.h  $
 **   Revision 1.1 2016/06/24 14:31:55EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hs/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hs_app.h"
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

void HS_Test_Setup(void);
void HS_Test_TearDown(void);


/************************/
/*  End of File Comment */
/************************/
