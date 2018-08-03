/* Copyright (C) 2009 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

/*************************************************************************
** Includes
*************************************************************************/
#include "nav_lib.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "osapi.h"
#include "libuart.h"


/*************************************************************************
** Macro Definitions
*************************************************************************/
#define NAV_USART_FD        1


/*************************************************************************
** Global Data
*************************************************************************/
static int GPSPoweredOn = 0;    // currently not used

char deviceName[] = "/dev/tty1";
uart_info_t NAV_UART = {.deviceString = &deviceName[1], .handle = 1, .isOpen = PORT_CLOSED, .baud = 115200};

/*************************************************************************
** External Global Memory
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
void nav_data_callback(uint8_t * buf, int len, void *pxTaskWoken);



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* GPS Library Initialization Routine                              */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 NAV_LibInit(void)
{
    int32 status;
    OS_printf("NAV_LibInit(): Initializing the GPS\n");
    status = uart_init_port(NAV_UART);

    return status;
}


/*
** Called by any cFS app that wants GPS data.
*/
void NAV_ReadAvailableData(uint8 DataBuffer[], int32 *DataLen)
{
    int32 i = 0;

    /* TODO does this need to be sent periodically? */
    //char gps_cmd[] = "log bestxyza";
    //usart_putstr(NAV_UART, gps_cmd, strlen(gps_cmd));

    /* check how many bytes are waiting on the uart */
    *DataLen = uart_bytes_available(NAV_UART.handle);
    /* OS_printf("GPS_ReadAvailableData(): gps messages waiting = %d\n", *DataLen); */

    /* declare an out buffer to hold that data */
    if (*DataLen > 0)
    {
        /* grab the bytes */
        uart_read_port(NAV_UART.handle, DataBuffer, *DataLen);
    }
    else
    {
        /* OS_printf("GPS_ReadAvailableData(): gps uart data len is 0\n"); */
    }
}





