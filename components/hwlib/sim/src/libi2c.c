/* Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include "nos_link.h"
#include <stdint.h>
#include <stdlib.h>

#include "libi2c.h"

/* psp */
#include <cfe_psp.h>

/* osal */
#include <osapi.h>

/* nos */
#include <I2C/Client/CInterface.h>

/* i2c device handles */
static NE_I2CHandle *i2c_device[NUM_I2C_DEVICES] = {0};

/* i2c mutex */
static uint32 nos_i2c_mutex = 0;

/* public prototypes */
void nos_init_i2c_link(void);
void nos_destroy_i2c_link(void);

/* initialize nos engine i2c link */
void nos_init_i2c_link(void)
{
    /* create mutex */
    int32 result = OS_MutSemCreate(&nos_i2c_mutex, "nos_i2c", 0);

}

/* destroy nos engine i2c link */
void nos_destroy_i2c_link(void)
{
    OS_MutSemTake(nos_i2c_mutex);

    /* clean up i2c buses */
    int i;
    for(i = 0; i < NUM_I2C_DEVICES; i++)
    {
        NE_I2CHandle *dev = i2c_device[i];
        if(dev) NE_i2c_close(&dev);
    }
    
    OS_MutSemGive(nos_i2c_mutex);

    /* destroy mutex */
    int32 result = OS_MutSemDelete(nos_i2c_mutex);
}

/* nos i2c transaction */
int32 i2c_master_transaction(int handle, uint8_t addr, void * txbuf, uint8_t txlen,
                               void * rxbuf, uint8_t rxlen, uint16_t timeout)
{
    int result = OS_ERROR;

    if(handle < NUM_I2C_DEVICES)
    {
        OS_MutSemTake(nos_i2c_mutex);

        /* get i2c device handle */
        NE_I2CHandle **dev = &i2c_device[handle];
        if(*dev == NULL)
        {
            /* get nos i2c connection params */
            const nos_connection_t *con = &nos_i2c_connection[handle];

            /* try to initialize master */
            *dev = NE_i2c_init_master3(hub, 10, con->uri, con->bus);
            if(*dev == NULL)
            {
                OS_printf("nos i2c_init_master failed\n");
            }
        }

        /* i2c transaction */
        if(*dev)
        {
            if(NE_i2c_transaction(*dev, addr, txbuf, txlen, rxbuf, rxlen) == NE_I2C_SUCCESS)
            {
                result = OS_SUCCESS;
            }
        }

        OS_MutSemGive(nos_i2c_mutex);
    }

    return result;
}

