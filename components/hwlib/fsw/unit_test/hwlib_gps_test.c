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

#include "hwlib_gps_test.h"
#include "hwlib_test_utils.h"

#include <gps_lib.h>

#include <uttest.h>
#include <utassert.h>

#define GPS_HANDLE 1

/* test lib init */
static void HWLib_GPS_Test_LibInit(void)
{
    int32 status = GPS_LibInit();
    UtAssert_True(status == OS_SUCCESS, "gps lib init");
}

/* test lib close */
static void HWLib_GPS_Test_LibClose(void)
{
    int32 status = GPS_LibClose();
    UtAssert_True(status == OS_SUCCESS, "gps lib close");
}

/* test read available data */
//TODO:  Commented this out of tests below as it is broken due to GPS_ReadAvailableData Function removal or move?
static void HWLib_GPS_Test_ReadAvailable(void)
{
    /*uint8_t exp_data[] = {0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xfa, 0xce, 0xff, 0x10, 0xaa, 0x23};
    uint8_t data[16];
    usart_data.rxbuf = exp_data;
    usart_data.rxlen = sizeof(exp_data);
    
    int32 len;
    GPS_ReadAvailableData(data, &len);

    UtAssert_True(len == sizeof(exp_data), "gps num read");
    UtAssert_True(usart_data.handle == GPS_HANDLE, "gps handle");

    unsigned int i;
    for(i = 0; i < sizeof(exp_data); i++)
    {
        UtAssert_True(data[i] == exp_data[i], "gps read data");
    }*/
}

/* test read bytes */
static void HWLib_GPS_Test_ReadBytes(void)
{
    int i;
    int idx = 0;
    uint8_t exp_data[] = {0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xfa, 0xce, 0xff, 0x10, 0xaa, 0x23};
    uint8_t data[16];
    usart_data.rxbuf = exp_data;
    usart_data.rxlen = sizeof(exp_data);
    int num;

    /* read 0 bytes */
    num = GPS_ReadNumBytes(data, 0);
    UtAssert_True(num == 0, "gps num read");
    UtAssert_True(usart_data.handle == 0, "gps handle");

    /* read 1 byte */
    num = GPS_ReadNumBytes(data, 1);
    UtAssert_True(num == 1, "gps num read");
    UtAssert_True(usart_data.handle == GPS_HANDLE, "gps handle");
    UtAssert_True(data[0] == exp_data[idx], "gps read data");
    idx += 1;

    /* read 8 bytes */
    num = GPS_ReadNumBytes(data, 8);
    UtAssert_True(num == 8, "gps num read");
    UtAssert_True(usart_data.handle == GPS_HANDLE, "gps handle");
    for(i = 0; i < 8; i++)
    {
        UtAssert_True(data[i] == exp_data[idx + i], "gps read data");
    }
    idx += 8;

    /* read more than available */
    num = GPS_ReadNumBytes(data, 5);
    UtAssert_True(num == 5, "gps num read");
    UtAssert_True(usart_data.handle == GPS_HANDLE, "gps handle");
    for(i = 0; i < 3; i++)
    {
        UtAssert_True(data[i] == exp_data[idx + i], "gps read data");
    }
}

/* test write bytes */
static void HWLib_GPS_Test_WriteBytes(void)
{
    uint8_t exp_data[] = {0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xfa, 0xce, 0xff, 0x10, 0xaa, 0x23};

    GPS_WriteString((char*)exp_data, sizeof(exp_data));

    unsigned int i;
    for(i = 0; i < sizeof(exp_data); i++)
    {
        UtAssert_True(usart_data.txbuf[i] == exp_data[i], "gps write data");
    }
}

void HWLib_GPS_Test_AddTestCases(void)
{
    UtTest_Add(HWLib_GPS_Test_LibInit, HWLib_Test_Setup, HWLib_Test_TearDown,
               "hwlib gps: lib init");
    UtTest_Add(HWLib_GPS_Test_LibClose, HWLib_Test_Setup, HWLib_Test_TearDown,
               "hwlib gps: lib close");
   // UtTest_Add(HWLib_GPS_Test_ReadAvailable, HWLib_Test_Setup, HWLib_Test_TearDown,
   //            "hwlib gps: read available");
    UtTest_Add(HWLib_GPS_Test_ReadBytes, HWLib_Test_Setup, HWLib_Test_TearDown,
               "hwlib gps: read bytes");
    UtTest_Add(HWLib_GPS_Test_WriteBytes, HWLib_Test_Setup, HWLib_Test_TearDown,
               "hwlib gps: write bytes");
}

