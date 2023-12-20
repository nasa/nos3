/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** CPU1 - NOS3
*/

#ifndef _cpu1_device_cfg_
#define _cpu1_device_cfg_

/*
** Note: These includes are required for HWLIB
*/
#include "cfe.h"
#include "osapi.h"

/* Note: NOS3 uart requires matching handle and bus number */

/*
** SAMPLE Configuration
*/
#define SAMPLE_CFG
#define SAMPLE_CFG_STRING           "usart_16"
#define SAMPLE_CFG_HANDLE           16 
#define SAMPLE_CFG_BAUDRATE_HZ      115200
#define SAMPLE_CFG_MS_TIMEOUT       50            /* Max 255 */
//#define SAMPLE_CFG_DEBUG

/*
** GENERIC_RADIO Configuration
*/
#define GENERIC_RADIO_CFG
#define GENERIC_RADIO_CFG_PROX_DATA_SIZE   64
#define GENERIC_RADIO_CFG_FSW_IP           "nos_fsw"
#define GENERIC_RADIO_CFG_DEVICE_IP        "radio_sim"
#define GENERIC_RADIO_CFG_DEVICE_DELAY_MS  250
#define GENERIC_RADIO_CFG_UDP_PROX_TO_FSW  7010
#define GENERIC_RADIO_CFG_UDP_FSW_TO_PROX  7011
#define GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO 5014
#define GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW 5015
//#define GENERIC_RADIO_CFG_DEBUG


#endif /* _cpu1_device_cfg_ */
