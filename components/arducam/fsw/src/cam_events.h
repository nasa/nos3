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

#ifndef _CAM_EVENTS_H_
#define _CAM_EVENTS_H_

/* define any custom app event IDs */
#define CAM_RESERVED_EID              0
#define CAM_STARTUP_INF_EID           1 
#define CAM_COMMAND_ERR_EID           2
#define CAM_COMMANDNOP_INF_EID        3 
#define CAM_COMMANDRST_INF_EID        4
#define CAM_INVALID_MSGID_ERR_EID     5 
#define CAM_LEN_ERR_EID               6 
#define CAM_PIPE_ERR_EID              7
#define CAM_MUTEX_ERR_EID             8
#define CAM_SEMAPHORE_ERR_EID         9
#define CAM_CHILD_REG_ERR_EID         10
#define CAM_INIT_CHILD_ERR_EID        11
#define CAM_INIT_ERR_EID              12
#define CAM_INIT_REG_ERR_EID          13
#define CAM_INIT_PIPE_ERR_EID         14
#define CAM_INIT_SUB_CMD_ERR_EID      15
#define CAM_INIT_SUB_HK_ERR_EID       16

/* Child Task IDs */
#define CAM_STOP_INF_EID			  20
#define CAM_PAUSE_INF_EID			  21
#define CAM_RUN_INF_EID			   	  22
#define CAM_TIMEOUT_INF_EID           23  
#define CAM_LOW_VOLTAGE_INT_EID       24
#define CAM_CHILD_STOP_INF_EID	   	  25
#define CAM_CHILD_PAUSE_INF_EID	   	  26
#define CAM_CHILD_RUN_INF_EID	   	  27
#define CAM_CHILD_INIT_EID			  28
#define CAM_CHILD_INIT_ERR_EID		  29
#define CAM_CHILD_EXP_EID			  30
#define CAM_CHILD_EXP_ERR_EID		  31

/* Full Experiments Completed */
#define CAM_EXP1_EID			      40
#define CAM_EXP2_EID			      41
#define CAM_EXP3_EID			      42
#define CAM_HW_CHECK_EID              43 

/* Errors */
#define CAM_INIT_SPI_ERR_EID          61
#define CAM_INIT_I2C_ERR_EID          62
#define CAM_CONFIG_ERR_EID            63
#define CAM_JPEG_INIT_ERR_EID         64
#define CAM_YUV422_ERR_EID            65
#define CAM_JPEG_ERR_EID              66
#define CAM_SETUP_ERR_EID             67
#define CAM_SET_SIZE_ERR_EID          68
#define CAM_CAPTURE_PREP_ERR_EID      69
#define CAM_CAPTURE_ERR_EID           70
#define CAM_READ_FIFO_LEN_ERR_EID     71
#define CAM_READ_PREP_ERR_EID         72
#define CAM_READ_ERR_EID              73
#define CAM_PUBLISH_ERR_EID           74
#define CAM_LOW_VOLTAGE_EID           75
#define CAM_TIME_EID                  76

#endif