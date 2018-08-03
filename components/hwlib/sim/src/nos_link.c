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
#include <stdlib.h>

/* osal */
#include <osapi.h>

/* nos usart connection table */
nos_connection_t nos_usart_connection[NUM_USARTS] = {
    {"tcp://127.0.0.1:12000", "usart_0"},
    {"tcp://127.0.0.1:12000", "usart_1"},
    {"tcp://127.0.0.1:12000", "usart_2"},
    {"tcp://127.0.0.1:12000", "usart_3"},
    {"tcp://127.0.0.1:12000", "usart_4"}
};

/* nos i2c connection table */
nos_connection_t nos_i2c_connection[NUM_I2C_DEVICES] = {
    {"tcp://127.0.0.1:12000", "i2c_0"},
    {"tcp://127.0.0.1:12000", "i2c_1"},
    {"tcp://127.0.0.1:12000", "i2c_2"}
};

/* nos spi connection table */
nos_connection_t nos_spi_connection[NUM_SPI_DEVICES] = {
    {"tcp://127.0.0.1:12000", "spi_0"},
    {"tcp://127.0.0.1:12000", "spi_1"}
};

/* common transport hub */
NE_TransportHub *hub = NULL;

/* internal hardware bus init/destroy */
extern void nos_init_usart_link(void);
extern void nos_destroy_usart_link(void);
extern void nos_init_i2c_link(void);
extern void nos_destroy_i2c_link(void);
extern void nos_init_spi_link(void);
extern void nos_destroy_spi_link(void);

/* initialize nos engine link */
void nos_init_link(void)
{
    OS_printf("initializing nos engine link...\n");

    /* create transport hub */
    hub = NE_create_transport_hub(0);

    /* initialize buses */
    nos_init_usart_link();
    nos_init_i2c_link();
    nos_init_spi_link();
}

/* destroy nos engine link */
void nos_destroy_link(void)
{
    OS_printf("destroying nos engine link...\n");

    /* destroy buses */
    nos_destroy_usart_link();
    nos_destroy_i2c_link();
    nos_destroy_spi_link();

    /* destroy transport hub */
    NE_destroy_transport_hub(&hub);
}

