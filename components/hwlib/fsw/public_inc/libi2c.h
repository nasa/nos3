/* Copyright (C) 2009 - 2018 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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
#include <stdlib.h>

/* Used for OSAL return codes */
#include <osapi.h>

/**
 * I2C device modes
 */
#define I2C_MASTER 	0
#define I2C_SLAVE 	1

/* 
 * Initialize I2C handle as Master with bus speed
 *
 * @param Device Which I2C bus (if more than one exists)
 * @param speed Bus speed in kbps
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32 i2c_master_init(int device, uint16_t speed);

/**
 * Excecute a I2C master write and slave read in one transaction
 *
 * @param device Handle to the device
 * @param addr I2C address, not bit-shifted
 * @param txbuf pointer to tx data
 * @param txlen length of tx data
 * @param rxbuf pointer to rx data
 * @param rxlen length of rx data
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: OS_SUCCESS if a frame is received, or OS_ERROR if timed out or if handle is not a valid device
 */
int32 i2c_master_transaction(int device, uint8_t addr, void * txbuf, uint8_t txlen, void * rxbuf, uint8_t rxlen, uint16_t timeout);