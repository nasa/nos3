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

#include <stdint.h>
#include <string.h>

/* osal */
#include <common_types.h>

/* libutil */
#include <util/error.h>

/* cam i2c params */
#define CAM_I2C_ADDRESS  0x30
#define CAM_I2C_HANDLE   2
#define CAM_I2C_BUF_MAX  128

/* test setup/teardown */
void CAM_Test_Setup(void);
void CAM_Test_TearDown(void);
void CAM_set_time(uint32 milliseconds);
void CAM_write_nvram(uint8 *buf, uint16 len);

/* i2c data */
typedef struct
{
    uint16_t txlen;
    uint8_t txbuf[CAM_I2C_BUF_MAX];
    uint16_t rxlen;
    const uint8_t* rxbuf;
    int retcode;
} i2c_data_t;

/* test i2c data */
extern i2c_data_t i2c_data;
