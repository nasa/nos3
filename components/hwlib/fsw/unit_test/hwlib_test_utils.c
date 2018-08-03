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

#include "hwlib_test_utils.h"

#include <i2c_hooks.h>
#include <usart_hooks.h>

#include <cfe.h>

#include <utassert.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>
#include <ut_osapi_stubs.h>

/* TODO remove when fixed in l3c lib */
uint8 cadet_debug_mode = 0;

/* prototypes */
static int i2c_transaction(int handle, uint8_t addr, void* txbuf, uint8_t txlen,
                           void* rxbuf, uint8_t rxlen, uint16_t timeout);
static int usart_messages_waiting_hook(int handle);
static char usart_getc_hook(int handle);
static void usart_putstr_hook(int handle, char* buf, int len);

/* i2c data */
i2c_data_t i2c_data;
static uint16_t i2c_read_index = 0;

/* i2c hooks */
static i2c_hooks_t i2c_hooks = {
    .i2c_init_master_hook = NULL,
    .i2c_master_transaction_hook = i2c_transaction,
};

/* usart data */
usart_data_t usart_data;

/* usart hooks */
static usart_hooks_t usart_hooks = {
    .usart_init_hook = NULL,
    .usart_set_callback_hook = NULL,
    .usart_putc_hook = NULL,
    .usart_getc_hook = usart_getc_hook,
    .usart_putstr_hook = usart_putstr_hook,
    .usart_messages_waiting_hook = usart_messages_waiting_hook
};

void HWLib_Test_Setup(void)
{
    /* initialize services */
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();

    /* set i2c hooks */
    HWLib_Reset_I2C_Data();
    set_i2c_hooks(&i2c_hooks);

    /* set usart hooks */
    HWLib_Reset_USART_Data();
    set_usart_hooks(&usart_hooks);
}

void HWLib_Test_TearDown(void)
{
    set_i2c_hooks(NULL);
    set_usart_hooks(NULL);
}

void HWLib_Reset_I2C_Data(void)
{
    memset(&i2c_data, 0, sizeof(i2c_data_t));
    i2c_data.retcode = E_NO_ERR;
    i2c_read_index = 0;
}

void HWLib_Reset_USART_Data(void)
{
    memset(&usart_data, 0, sizeof(usart_data_t));
}

/* i2c transaction hook */
static int i2c_transaction(int handle, uint8_t addr, void* txbuf, uint8_t txlen,
                           void* rxbuf, uint8_t rxlen, uint16_t timeout)
{
    /* verify buffers */
    if(txlen > 0) UtAssert_True(txbuf != NULL, "i2c txbuf != NULL");
    if(rxlen > 0) UtAssert_True(rxbuf != NULL, "i2c rxbuf != NULL");

    /* save off i2c params */
    i2c_data.cmd_cnt += 1;
    i2c_data.handle = handle;
    i2c_data.address = addr;
    i2c_data.timeout = timeout;

    /* save tx data for testing */
    if(txlen > 0)
    {
        uint16_t avail = I2C_BUF_MAX - i2c_data.txlen;
        UtAssert_True(txlen <= avail, "i2c txbuf overflow");
        uint16_t len = (txlen > avail) ? avail : txlen;
        memcpy(i2c_data.txbuf + i2c_data.txlen, txbuf, len);
        i2c_data.txlen += len;
    }

    /* only process on no error */
    if(i2c_data.retcode == E_NO_ERR)
    {
        /* return rxbuf test data */
        if(rxlen > 0)
        {
            uint16_t avail = i2c_data.rxlen - i2c_read_index;
            UtAssert_True(rxlen <= avail, "i2c rxbuf underflow");
            uint16_t len = (rxlen > avail) ? avail : rxlen;
            memcpy(rxbuf, i2c_data.rxbuf + i2c_read_index, len);
            i2c_read_index += len;
        }
    }

    return i2c_data.retcode;
}

/* usart messages waiting hook */
static int usart_messages_waiting_hook(int handle)
{
    usart_data.handle = handle;
    return usart_data.rxlen;
}

/* usart getc hook */
static char usart_getc_hook(int handle)
{
    usart_data.handle = handle;
    char data = 0;

    /* data in buffer? */
    if(usart_data.rxlen > 0)
    {
        data = (char)*usart_data.rxbuf;
        usart_data.rxbuf += 1;
        usart_data.rxlen -= 1;
    }

    return data;
}

/* usart putstr hook */
static void usart_putstr_hook(int handle, char* buf, int len)
{
    /* verify buffer */
    if(len > 0) UtAssert_True(buf != NULL, "usart buf != NULL");

    /* save off usart params */
    usart_data.handle = handle;

    /* save tx data for testing */
    if(len > 0)
    {
        uint16_t avail = USART_BUF_MAX - usart_data.txlen;
        UtAssert_True(len <= avail, "usart tx buf overflow");
        if(len > avail) len = avail;
        memcpy(usart_data.txbuf + usart_data.txlen, (uint8_t*)buf, len);
        usart_data.txlen += len;
    }
}

