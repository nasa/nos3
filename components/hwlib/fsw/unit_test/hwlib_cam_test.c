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

#include "hwlib_cam_test.h"
#include "hwlib_test_utils.h"

#include <cam_lib.h>

#include <uttest.h>
#include <utassert.h>


#include <dev/gs_spi.h>

/* test lib init */
static void HWLib_CAM_Test_LIBINIT(void)
{
	int32 status = CAM_LibInit();
	UtAssert_True(status == OS_SUCCESS, "CAM Lib Init");
}

/* test cam init i2c */
static void HWLib_CAM_Test_I2CINIT(void)
{
	int32 status = CAM_init_i2c();
	UtAssert_True(status == OS_SUCCESS, "CAM I2C Init");
}

/* test cam init spi */
static void HWLib_CAM_Test_SPI(void)
{
	extern gs_spi_chip_t chip;
	int32 status = CAM_init_spi();
	
	UtAssert_True(status == OS_SUCCESS, "CAM SPI Init");  
	printf("STATUS: %d\n", status);
	UtAssert_True(chip.handle == 0, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.reg == 1, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.baudrate == 8000000, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.bits == 16, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.spck_delay == 0, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.trans_delay == 0, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.stay_act == 0, "CAM SPI Init: Chip->handle");
	UtAssert_True(chip.spi_mode == 0, "CAM SPI Init: Chip->handle");
}

/* test cam config */
static void HWLib_CAM_Test_CONFIG(void)
{
	int32 status = CAM_config();
	UtAssert_True(status == OS_SUCCESS, "CAM Config");
}

/* test cam setup */
static void HWLib_CAM_Test_SETUP(void)
{
	int32 status = CAM_setup();
	UtAssert_True(status == OS_SUCCESS, "CAM Setup");
}

/* test cam jpeg init */
static void HWLib_CAM_Test_JPEGINIT(void)
{
	int32 status = CAM_jpeg_init();
	UtAssert_True(status == OS_SUCCESS, "CAM JPEG Init");
}

/* test cam yuv 422 */
static void HWLib_CAM_Test_YUV422(void)
{
	int32 status = CAM_yuv422();
	UtAssert_True(status == OS_SUCCESS, "CAM YUV 422 ");
}

/* test cam jpeg */
static void HWLib_CAM_Test_JPEG(void)
{
	int32 status = CAM_jpeg();
	UtAssert_True(status == OS_SUCCESS, "CAM JPEG");
}

/* test cam jpeg 320x240 */
static void HWLib_CAM_Test_JPEG320x240(void)
{
	int32 status = CAM_jpeg_320x240();
	UtAssert_True(status == OS_SUCCESS, "CAM JPEG 320x240");
}

/* test cam set size */
static void HWLib_CAM_Test_SETSIZE(void) //May want to just break this out into separate individual tests
{
	int i = 0;
	int32 result;
	for (i = 0; i < 10; i ++)
	{
		printf("Testing size %d\n", i);
		result = CAM_setSize(i);
		UtAssert_True(result == OS_SUCCESS, "CAM SET SIZE");
	}
}

/* test cam capture prep */
static void HWLib_CAM_Test_CAPTUREPREP(void)
{
	int32 status = CAM_capture_prep();
	UtAssert_True(status == OS_SUCCESS, "CAM Capture Prep");
}

/* test cam capture */
static void HWLib_CAM_Test_CAPTURE(void) //This could probablyuse additional tests
{
	int32 status = CAM_capture();
	UtAssert_True(status == OS_SUCCESS, "CAM Capture");
}

/* test cam read prep */
static void HWLib_CAM_Test_READPREP(void) 
{
	CAM_init_spi();
	char buf[1024] = "";
	uint16 i = 0;

	int32 status = CAM_read_prep(buf, &i);
	UtAssert_True(status == OS_SUCCESS, "CAM Read Prep");
}

/* test cam read */
static void HWLib_CAM_Test_READ(void) 
{
	char buf[1024] = "";
	uint16 i = 0;
	uint8 result = 0;

	int32 status = CAM_read(buf, &i, &result);
	UtAssert_True(status == OS_SUCCESS, "CAM Read");
}

/* test cam i2c write regs */
//static void HWLib_CAM_Test_I2CWRITEREGS(void) 
//{
//	//This is currently only a nominal test (completely avoids the while loop)
//	struct sensor_reg reglist[10];
//	reglist[0].reg = 0xff;
//	reglist[0].val = 0xff;
//	int32 status = arducam_i2c_write_regs(reglist);
//	UtAssert_True(status == OS_SUCCESS, "Arducam I2C Write Regs");
//}

//static void HWLib_CAM_Test_I2CWRITEREGS_END(void) //End of Array
//{
//	struct sensor_reg reglist[10];
//	reglist[9].reg = 0xff;
//	reglist[9].val = 0xff;
//	int32 status = arducam_i2c_write_regs(reglist);
//	UtAssert_True(status == OS_SUCCESS, "Arducam I2C Write Regs");
//}

//static void HWLib_CAM_Test_I2CWRITEREGS_MIDDLE(void) //End of Array
//{
//	struct sensor_reg reglist[10];
//	reglist[5].reg = 0xff;
//	reglist[5].val = 0xff;
//	int32 status = arducam_i2c_write_regs(reglist);
//	UtAssert_True(status == OS_SUCCESS, "Arducam I2C Write Regs");
//}

void HWLib_CAM_Test_AddTestCases(void)
{
	UtTest_Add(HWLib_CAM_Test_LIBINIT, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Lib Init");
	UtTest_Add(HWLib_CAM_Test_I2CINIT, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: I2C Init");
	UtTest_Add(HWLib_CAM_Test_SPI, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: SPI Test");
	UtTest_Add(HWLib_CAM_Test_CONFIG, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Config Test");
	UtTest_Add(HWLib_CAM_Test_SETUP, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Setup Test");
	UtTest_Add(HWLib_CAM_Test_JPEGINIT, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: JPEG Init");
	UtTest_Add(HWLib_CAM_Test_YUV422, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: YUV 422 Test");
	UtTest_Add(HWLib_CAM_Test_JPEG, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: JPEG Test");
	UtTest_Add(HWLib_CAM_Test_JPEG320x240, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: JPEG 320x240 Test");
	UtTest_Add(HWLib_CAM_Test_SETSIZE, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Set Size Test");
	UtTest_Add(HWLib_CAM_Test_CAPTUREPREP, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Capture Prep Test");
	UtTest_Add(HWLib_CAM_Test_CAPTURE, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Capture Test");
	UtTest_Add(HWLib_CAM_Test_READPREP, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Read Prep Test");
	UtTest_Add(HWLib_CAM_Test_READ, HWLib_Test_Setup, HWLib_Test_TearDown, 
		"HWLib, CAM: Read Test");
//	UtTest_Add(HWLib_CAM_Test_I2CWRITEREGS, HWLib_Test_Setup, HWLib_Test_TearDown, 
//		"HWLib, CAM: I2C Write Regs Test");
//	UtTest_Add(HWLib_CAM_Test_I2CWRITEREGS_END, HWLib_Test_Setup, HWLib_Test_TearDown, 
//		"HWLib, CAM: I2C Write Regs Test END");
//	UtTest_Add(HWLib_CAM_Test_I2CWRITEREGS_MIDDLE, HWLib_Test_Setup, HWLib_Test_TearDown, 
//		"HWLib, CAM: I2C Write Regs Test MIDDLE");
}