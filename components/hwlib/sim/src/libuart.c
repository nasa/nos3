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

#include "libuart.h"

/* psp */
#include <cfe_psp.h>

/* osal */
#include <osapi.h>

/* nos */
#include <Uart/Client/CInterface.h>

/* size of uart buffer */
#define USART_RX_BUF_SIZE    512

/* usart device handles */
static NE_Uart *usart_device[NUM_USARTS] = {0};

/* usart mutex */
static uint32 nos_usart_mutex = 0;

/* public prototypes */
void nos_init_usart_link(void);
void nos_destroy_usart_link(void);

/* private prototypes */
static NE_Uart* nos_get_usart_device(int handle);

/* initialize nos engine usart link */
void nos_init_usart_link(void)
{
    /* create mutex */
    int32 result = OS_MutSemCreate(&nos_usart_mutex, "nos_usart", 0);

}

/* destroy nos engine usart link */
void nos_destroy_usart_link(void)
{
    int i;

    OS_MutSemTake(nos_usart_mutex);

    /* clean up usart buses */
    for(i = 0; i <= NUM_USARTS; i++)
    {
        NE_Uart *dev = usart_device[i];
        if(dev) NE_uart_close(&dev);
    }
    
    OS_MutSemGive(nos_usart_mutex);

    /* destroy mutex */
    int32 result = OS_MutSemDelete(nos_usart_mutex);
}

/* init usart */
int32 uart_init_port(uart_info_t device)
{
    if(device.handle != -1)
    {
        OS_MutSemTake(nos_usart_mutex);

        /* get usart device handle */
        NE_Uart **dev = &usart_device[device.handle];
        if(*dev == NULL)
        {
            /* get nos usart connection params */
            const nos_connection_t *con = &nos_usart_connection[device.handle];

            /* try to initialize usart */
            *dev = NE_uart_open3(hub, "fsw", con->uri, con->bus, device.handle);
            if(*dev)
            {
                /* set default queue size */
                NE_uart_set_queue_size(*dev, USART_RX_BUF_SIZE);

                device.isOpen = PORT_OPEN;
            }
            else
            {
                OS_printf("nos uart_open failed\n");
            }
        }

        OS_MutSemGive(nos_usart_mutex);
    }
    return OS_SUCCESS;
}

/* get usart device */
static NE_Uart* nos_get_usart_device(int handle)
{
    NE_Uart *dev = NULL;
    if(handle < NUM_USARTS)
    {
        dev = usart_device[handle];
    }
    return dev;
}

/* usart write */
int32 uart_write_port(int32 handle, uint8 data[], const uint32 numBytes)
{
    NE_Uart *dev = nos_get_usart_device((int)handle);
    if(dev)
    {
        OS_MutSemTake(nos_usart_mutex);
        NE_uart_write(dev, (const uint8_t*)data, numBytes);
        OS_MutSemGive(nos_usart_mutex);
    }

    return OS_SUCCESS;
}

/* usart read */
int32 uart_read_port(int32 handle, uint8 data[], const uint32 numBytes)
{
    char c = 0;
    int  i;
    NE_Uart *dev = nos_get_usart_device((int)handle);
    if(dev)
    {
        OS_MutSemTake(nos_usart_mutex);
        for (i = 0; i < (int)numBytes; i++)
        {
            NE_uart_getc(dev, (uint8_t*)&c);
            data[i] = c;
        }
        NE_uart_getc(dev, (uint8_t*)&c);
        OS_MutSemGive(nos_usart_mutex);
    }
    return OS_SUCCESS;
}

/* usart number bytes available */
int32 uart_bytes_available(int32 handle)
{
    int bytes = 0;
    NE_Uart *dev = nos_get_usart_device((int)handle);
    if(dev)
    {
        OS_MutSemTake(nos_usart_mutex);
        bytes = (int)NE_uart_available(dev);
        OS_MutSemGive(nos_usart_mutex);
    }
    return bytes;
}

