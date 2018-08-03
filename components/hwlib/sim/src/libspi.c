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

#include "libspi.h"

/* psp */
#include <cfe_psp.h>

/* osal */
#include <osapi.h>

/* nos */
#include <Spi/Client/CInterface.h>

/* spi device handles */
static NE_SpiHandle *spi_device[NUM_SPI_DEVICES] = {0};

/* spi mutex */
static uint32 nos_spi_mutex = 0;

/* public prototypes */
void nos_init_spi_link(void);
void nos_destroy_spi_link(void);

/* private prototypes */
static NE_SpiHandle* nos_get_spi_device(spi_info_t device);

/* initialize nos engine spi link */
void nos_init_spi_link(void)
{
    /* create mutex */
    int32 result = OS_MutSemCreate(&nos_spi_mutex, "nos_spi", 0);

}

/* destroy nos engine spi link */
void nos_destroy_spi_link(void)
{
    OS_MutSemTake(nos_spi_mutex);

    /* clean up spi buses */
    int i;
    for(i = 0; i < NUM_SPI_DEVICES; i++)
    {
        NE_SpiHandle *dev = spi_device[i];
        if(dev) NE_spi_close(&dev);
    }
    
    OS_MutSemGive(nos_spi_mutex);

    /* destroy mutex */
    int32 result = OS_MutSemDelete(nos_spi_mutex);
}

/* nos spi init */
int32 spi_init_dev(spi_info_t device)
{
    int result = OS_ERROR;

    if(device.handle < NUM_SPI_DEVICES)
    {
        OS_MutSemTake(nos_spi_mutex);

        /* get spi device handle */
        NE_SpiHandle **dev = &spi_device[device.handle];
        if(*dev == NULL)
        {
            /* get nos spi connection params */
            const nos_connection_t *con = &nos_spi_connection[device.handle];

            /* try to initialize master */
            *dev = NE_spi_init_master3(hub, con->uri, con->bus);
            if(*dev)
            {
                result = OS_SUCCESS;
            }
            else
            {
                OS_printf("nos spi_init_master failed\n");
            }
        }
        else
        {
            result = OS_SUCCESS;
        }

        OS_MutSemGive(nos_spi_mutex);
    }

    return result;
}

/* get spi device */
static NE_SpiHandle* nos_get_spi_device(spi_info_t device)
{
    NE_SpiHandle *dev = NULL;
    if(device.handle < NUM_SPI_DEVICES)
    {
        dev = spi_device[device.handle];
        if(dev == NULL)
        {
            spi_init_dev(device);
            dev = spi_device[device.handle];
        }
    }
    return dev;
}

/* nos spi chip select */
int32 spi_select_chip(spi_info_t device)
{
    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        OS_MutSemTake(nos_spi_mutex);
        NE_spi_select_chip(dev, device.cs);
        OS_MutSemGive(nos_spi_mutex);
    }

    return OS_SUCCESS;
}

/* nos spi chip unselect */
int32 spi_unselect_chip(spi_info_t device)
{
    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        OS_MutSemTake(nos_spi_mutex);
        NE_spi_unselect_chip(dev);
        OS_MutSemGive(nos_spi_mutex);
    }

    return OS_SUCCESS;
}

/* nos spi write */
int32 spi_write(spi_info_t device, uint8 data[], const uint32 numBytes)
{
    int result = OS_ERROR;

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        OS_MutSemTake(nos_spi_mutex);
        if(NE_spi_write(dev, data, numBytes) == NE_SPI_SUCCESS)
        {
            result = OS_SUCCESS;
        }
        OS_MutSemGive(nos_spi_mutex);
    }

    return result;
}

/* nos spi read */
int32 spi_read(spi_info_t device, uint8 data[], const uint32 numBytes)
{
    int result = OS_ERROR;

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        OS_MutSemTake(nos_spi_mutex);
        if(NE_spi_read(dev, data, numBytes) == NE_SPI_SUCCESS)
        {
            result = OS_SUCCESS;
        }
        OS_MutSemGive(nos_spi_mutex);
    }

    return result;
}
