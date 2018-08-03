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

typedef struct {
    char    *deviceString;      /* uart string descriptor of the port  */
	int32   handle;			    /* handle to the hardware device */
	uint8   cs;			        /* chip Select */
    uint32  baudrate;		    /* baudrate for the SPI */
	uint8   spi_mode;		    /* Which of the four SPI-modes to use when transmitting */
} spi_info_t;

/**
 * Initialize SPI device
 * @param device spi_info_t struct with all spi params
 */
int32 spi_init_dev(spi_info_t device);

/*
 * Write a number of bytes to of a given spi port
 * 
 * @param spi_info_t struct with all spi params
 * @param data array of the data to write
 * @param numBytes number of bytes to write the port
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32 spi_write(spi_info_t device, uint8 data[], const uint32 numBytes);

/*
 * Read a number of bytes off of a given spi port
 * 
 * @param spi_info_t struct with all spi params
 * @param data array to store the read data
 * @param numBytes number of bytes to read off the port
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32 spi_read(spi_info_t device, uint8 data[], const uint32 numBytes);

/*
 * For manual control of the CS line where needed
 * 
 *  @param chipSelect the number of the CS line
 *  @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32 spi_select_chip(spi_info_t device);

int32 spi_unselect_chip(spi_info_t device);