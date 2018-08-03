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
#include <osapi.h>

/* libutil */
#include <util/error.h>

#define I2C_BUF_MAX   512
#define USART_BUF_MAX 64

/* test setup/teardown */
void HWLib_Test_Setup(void);
void HWLib_Test_TearDown(void);
void HWLib_Reset_I2C_Data(void);
void HWLib_Reset_USART_Data(void);

/* i2c data */
typedef struct
{
    uint8_t cmd_cnt;
    int handle;
    uint8_t address;
    uint16_t txlen;
    uint8_t txbuf[I2C_BUF_MAX];
    uint16_t rxlen;
    const uint8_t* rxbuf;
    uint16_t timeout;
    int retcode;
} i2c_data_t;

/* app i2c data */
extern i2c_data_t i2c_data;

/* usart data */
typedef struct
{
    int handle;
    uint16_t txlen;
    uint8_t txbuf[USART_BUF_MAX];
    uint16_t rxlen;
    const uint8_t* rxbuf;
} usart_data_t;

/* app usart data */
extern usart_data_t usart_data;

