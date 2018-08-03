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
#include "hwlib_eps_test.h"
#include "hwlib_ants_test.h"
#include "hwlib_l3c_test.h"
#include "hwlib_imu_test.h"
#include "hwlib_fram_test.h"
#include "hwlib_gps_test.h"
#include "hwlib_spw_test.h"
#include "hwlib_sen_test.h"
#include "hwlib_cam_test.h"
#include "hwlib_csee_test.h"
#include <stf1_test.h>
#include <uttest.h>

STF1_TEST_RUNNER()
{
    /* add test cases */
    HWLib_Test_AddTestCases();
    HWLib_EPS_Test_AddTestCases();
    HWLib_ANTS_Test_AddTestCases();
    HWLib_L3C_Test_AddTestCases();
    HWLib_FRAM_Test_AddTestCases();
    HWLib_GPS_Test_AddTestCases();
    HWLib_IMU_Test_AddTestCases();
    HWLib_SPW_Test_AddTestCases();
    HWLib_SEN_Test_AddTestCases();
	HWLib_CAM_Test_AddTestCases();
    HWLib_CSEE_Test_AddTestCases();

    /* run tests */
    return(UtTest_Run());
}

