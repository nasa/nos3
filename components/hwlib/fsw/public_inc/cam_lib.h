/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any dacames, including, but not limited to direct, indirect, special or consequential dacames,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

#ifndef _cam_lib_h_
#define _cam_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

/************************************************************************
** Debug Definitions
*************************************************************************/
//#define FILE_OUTPUT
//#define STF1_DEBUG

/************************************************************************
** Type Definitions
*************************************************************************/
#define CAM_I2C					2
#define CAM_SPEED				1000000
#define CAP_DONE_MASK      		0x08
#define CAM_TIMEOUT				100
#define CAM_DATA_SIZE           1010

// Select Hardware (only 1)
    //#define OV2640
    #define OV5640 
    //#define OV5642

// Hardware Specific Definitions
#ifdef OV2640
    #define CAM_ADDR			0x30
    #define CHIPID_HIGH         0x0A
    #define CHIPID_LOW          0x0B
    #define CAM_VID             0x26
    #define CAM_PID             0x42
    #define MAX_FIFO_SIZE       0x5FFFF // 384KByte
#endif
#ifdef OV5640
    #define CAM_ADDR	        0x3C
    #define CHIPID_HIGH         0x300A
    #define CHIPID_LOW          0x300B
    #define CAM_VID             0x56
    #define CAM_PID             0x40
    #define MAX_FIFO_SIZE       0x7FFFFF // 8MByte
#endif
#ifdef OV5642
    #define CAM_ADDR	        0x3C
    #define CHIPID_HIGH         0x300A
    #define CHIPID_LOW          0x300B
    #define CAM_VID             0x56
    #define CAM_PID             0x42
    #define MAX_FIFO_SIZE       0x7FFFFF // 8MByte
#endif

#define size_160x120			0
#define size_320x240			1
#define size_800x600			2
#define size_1600x1200			3
#define size_2592x1944          4

/*************************************************************************
** Structures
*************************************************************************/
struct sensor_reg {
    uint16_t reg;
    uint16_t val;
};

/*************************************************************************
** Exported Functions
*************************************************************************/
extern int32 CAM_LibInit(void);
extern int32 CAM_init_i2c(void);
extern int32 CAM_init_spi(void);
extern int32 CAM_config(void);
extern int32 CAM_jpeg_init(void);
extern int32 CAM_yuv422(void);
extern int32 CAM_jpeg(void);
extern int32 CAM_jpeg_320x240(void);
extern int32 CAM_setup(void);
extern int32 CAM_setSize(uint8_t size);
extern int32 CAM_capture_prep(void);
extern int32 CAM_capture(void);
extern int32 CAM_read_fifo_length(uint32* length);
extern int32 CAM_read_prep(char* buf, uint16* i);
extern int32 CAM_read(char* buf, uint16* i, uint8* status);

#endif 
