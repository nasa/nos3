/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

/************************************************************************
** File:
**   $Id: cam_platform_cfg.h  $
**
** Purpose:
**  Define CAM platform configuation parameters - application definitions
**
** Notes:
**
*************************************************************************/
#ifndef _CAM_PLATFORM_CFG_H_
#define _CAM_PLATFORM_CFG_H_

/*
** CAM Child Task Definitions
*/
#define CAM_CHILD_TASK_NAME              	"CAM_CHILD_TASK"
#define CAM_CHILD_TASK_STACK_SIZE       	2048
#define CAM_CHILD_TASK_PRIORITY          	205
#define CAM_RUN						 	    0
#define CAM_PAUSE						    1
#define CAM_STOP						    2
#define CAM_TIME                            3
#define CAM_LOW_VOLTAGE                     4

#define CAM_MUTEX_NAME                      "CAM_MUTEX"
#define CAM_SEM_NAME                        "CAM_SEM"

/****************************************************/
/* Sensor related definition 												*/
/****************************************************/
#define BMP 	0
#define JPEG	1

#define OV2640_160x120 		0	//160x120
#define OV2640_176x144 		1	//176x144
#define OV2640_320x240 		2	//320x240
#define OV2640_352x288 		3	//352x288
#define OV2640_640x480		4	//640x480
#define OV2640_800x600 		5	//800x600
#define OV2640_1024x768		6	//1024x768
#define OV2640_1280x1024	7	//1280x1024
#define OV2640_1600x1200	8	//1600x1200

/****************************************************/
/* ArduChip related definition 											*/
/****************************************************/
#define ARDUCHIP_MODE      		0x02  //Mode register

#endif 
