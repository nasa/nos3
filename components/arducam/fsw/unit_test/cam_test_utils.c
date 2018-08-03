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

#include "cam_test_utils.h"

#include <cam_app.h>

#include <i2c_hooks.h>

#include <utassert.h>
#include <ut_cfe_es_stubs.h>
#include <ut_cfe_sb_stubs.h>
#include <ut_osapi_stubs.h>

#ifdef __linux__
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

/* prototypes */
static int i2c_transaction(int handle, uint8_t addr, void* txbuf, uint8_t txlen,
                           void* rxbuf, uint8_t rxlen, uint16_t timeout);

/* i2c data */
i2c_data_t i2c_data;
static uint8_t i2c_read_index = 0;

/* i2c hooks */
static i2c_hooks_t i2c_hooks = {
    .i2c_init_master_hook = NULL,
    .i2c_master_transaction_hook = i2c_transaction,
};

void CAM_Test_Setup(void)
{
    /* initialize services */
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();

    /* initialize app data */
    CFE_PSP_MemSet(&CAM_AppData, 0, sizeof(CAM_AppData_t));

    /* set i2c hooks */
    memset(&i2c_data, 0, sizeof(i2c_data_t));
    i2c_read_index = 0;
    i2c_data.retcode = E_NO_ERR;
    set_i2c_hooks(&i2c_hooks);
}

void CAM_Test_TearDown(void)
{
    set_i2c_hooks(NULL);
}


/* i2c transaction hook */
static int i2c_transaction(int handle, uint8_t addr, void* txbuf, uint8_t txlen,
                           void* rxbuf, uint8_t rxlen, uint16_t timeout)
{
    /* verify buffers */
    if(txlen > 0) UtAssert_True(txbuf != NULL, "i2c txbuf != NULL");
    if(rxlen > 0) UtAssert_True(rxbuf != NULL, "i2c rxbuf != NULL");

    /* verify basic cam i2c params */
    UtAssert_True(handle == CAM_I2C_HANDLE, "cam i2c handle");
    UtAssert_True(addr == CAM_I2C_ADDRESS, "cam i2c address");

    /* save tx data for testing */
    if(txlen > 0)
    {
        uint16_t avail = CAM_I2C_BUF_MAX - i2c_data.txlen;
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
            //UtAssert_True(rxlen <= avail, "i2c rxbuf underflow");
            uint16_t len = (rxlen > avail) ? avail : rxlen;
            memcpy(rxbuf, i2c_data.rxbuf + i2c_read_index, len);
            i2c_read_index += len;
        }
    }

    return i2c_data.retcode;
}
