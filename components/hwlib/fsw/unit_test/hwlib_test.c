/*
  Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

  This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
  limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
  for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
  any warranty that the software will be error free.

  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
  arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
  contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
  documentation or services provided hereunder

  ITC Team
  NASA IV&V
  ivv-itc@lists.nasa.gov
*/

#include "hwlib_test.h"
#include "hwlib_test_utils.h"

#include <cfe.h>

#include <uttest.h>
#include <utassert.h>

extern int32 hwlib_Init(void);

/* test lib init */
static void HWLib_Test_LibInit(void)
{
    int32 status = hwlib_Init();
    UtAssert_True(status == OS_SUCCESS, "hwlib init");
}

void HWLib_Test_AddTestCases(void)
{
    UtTest_Add(HWLib_Test_LibInit, HWLib_Test_Setup, HWLib_Test_TearDown,
               "hwlib: lib init");
}

