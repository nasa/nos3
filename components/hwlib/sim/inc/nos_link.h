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

#ifndef _NOS_ENGINE_LINK_
#define _NOS_ENGINE_LINK_

/* nos */
#include <Client/CInterface.h>

#define NUM_USARTS      5
#define NUM_I2C_DEVICES 3
#define NUM_SPI_DEVICES 2

#ifdef __cplusplus
extern "C" {
#endif

/* nos engine connection */
typedef struct {
    const char* uri;
    const char* bus;
} nos_connection_t;

/* nos usart connection table */
extern nos_connection_t nos_usart_connection[NUM_USARTS];

/* nos i2c connection table */
extern nos_connection_t nos_i2c_connection[NUM_I2C_DEVICES];

/* nos spi connection table */
extern nos_connection_t nos_spi_connection[NUM_SPI_DEVICES];

/* common transport hub */
extern NE_TransportHub *hub;

/* init/destroy nos engine link (called by nos psp) */
void nos_init_link(void);
void nos_destroy_link(void);

#ifdef __cplusplus
}
#endif

#endif

