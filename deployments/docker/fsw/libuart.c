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

#include "libuart.h"

int32_t uart_init_port(uart_info_t* device)
{
  int32_t status = UART_SUCCESS;
  speed_t speed;

  // Set the access flag.  We default to O_RDWR if the specified access flag is
  // out of range
  int oflag = O_RDWR;

  if (device->access_option == uart_access_flag_RDONLY)
    oflag = O_RDONLY;
  else if (device->access_option == uart_access_flag_WRONLY)
    oflag = O_WRONLY;

  device->handle = open(device->deviceString, oflag);

  if (device->handle >= 0)
  {
    // Set open flag
    device->isOpen = PORT_OPEN;

    // Get current port options
    if(tcgetattr(device->handle, &device->options)<0)
    {
        status = OS_ERR_FILE;
        return status;
    }

    // Set baud rate
    switch(device->baud)
    {
      case 4800:
        speed=B4800;
        break;
      case 9600:
        speed=B9600;
        break;
      case 19200:
        speed=B19200;
        break;
      case 38400:
        speed=B38400;
        break;
      case 57600:
        speed=B57600;
        break;
      case 115200:
        speed=B115200;
        break;
      case 230400:
        speed=B230400;
        break;
      case 460800:
        speed=B460800;
        break;
      case 500000:
        speed=B500000;
        break;
      case 576000:
        speed=B576000;
        break;
      case 921600:
        speed=B921600;
        break;
      case 1000000:
        speed=B1000000;
        break;
      case 1152000:
        speed=B1152000;
        break;
      case 2000000:
        speed=B2000000;
        break;
      case 2500000:
        speed=B2500000;
        break;
      case 3000000:
        speed=B3000000;
        break;
      case 3500000:
        speed=B3500000;
        break;
      case 4000000:
        speed=B4000000;
        break;
      default:
        status = OS_ERR_FILE;
        return status;
    }
    if(cfsetispeed(&device->options,speed)<0)
    {
        status = OS_ERR_FILE;
        return status;
    }
    if(cfsetospeed(&device->options,speed)<0)
    {
        status = OS_ERR_FILE;
        return status;
    }

    // Raw byte mode - no strings and no CRLF
    device->options.c_iflag      = IGNBRK | INPCK;
    device->options.c_oflag      = 0;
    device->options.c_cflag      = CREAD | CS8 | CLOCAL;
    device->options.c_lflag      = NOFLSH;
    if(device->canonicalModeOn == 1)
    {
        device->options.c_lflag |= ICANON;
    }

    // Set the port to blocking read with timeout of 0.1 sec
    device->options.c_cc[VMIN] = 0;       // min of bytes to read
    device->options.c_cc[VTIME] = 1;      // intra-byte time to wait - tenths of sec
    fcntl(device->handle, F_SETFL, O_NONBLOCK);    // Don't have serial port block

    // TODO - any other options needed like hw control?
    tcflush(device->handle, TCIOFLUSH);

    // Set the options
    if(tcsetattr(device->handle, TCSANOW, &device->options)<0)
    {
        status = OS_ERR_FILE;
        return status;
    }
  }
  else
  {
      printf("Oh no!  Open \"%s\" failed and reported: %s \n", device->deviceString, strerror(device->handle));
      device->isOpen = PORT_CLOSED;
      status = OS_ERR_FILE;
  }

  return status;
}

int32_t uart_bytes_available(uart_info_t* device)
{
  int32_t bytes_available = 0;

  ioctl(device->handle, FIONREAD, &bytes_available);

  return bytes_available;
}

int32_t uart_flush(uart_info_t* device)
{
  tcflush(device->handle,TCIOFLUSH);

  return UART_SUCCESS;
}

int32_t uart_read_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes)
{
  int32_t status = UART_SUCCESS;

  if (data != NULL)
  {
    // TODO - this read blocks forever if no serial data on the port.
    //        it should be timing out - need to look into this ASAP
    status = read(device->handle, data, numBytes);
  }
  else
  {
    status = OS_ERR_FILE;
  }

  return status;
}

int32_t uart_write_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes)
{
  int32_t status = UART_SUCCESS;

  status = write(device->handle, data, numBytes);

  return status;
}

int32_t uart_close_port(uart_info_t* device)
{
  int32_t status = UART_SUCCESS;

  if (device->handle >= 0)
  {
    status = close(device->handle);
    if (0 == status) /* todo remove magic number */
    {
      status = UART_SUCCESS;
    }
    else
    {
      status = OS_ERR_FILE;
    }
  }
  else
  {
    status = OS_ERR_FILE;
  }
  return status;
}
