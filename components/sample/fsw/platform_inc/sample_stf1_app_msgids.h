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


/************************************************************************
** File:
**   $Id: sample_stf1_app_msgids.h  $
**
** Purpose:
**  Define Sample STF App Message IDs
**
** Notes:
**
*************************************************************************/
#ifndef _SAMPLE_STF1_APP_MSGIDS_H_
#define _SAMPLE_STF1_APP_MSGIDS_H_

/* 
** refer to the ITC confluence page or spreadsheet when choosing app
**  message IDs.  science application message ids are defined as follows:
**       CSEE    = Commands: 0x1850 - 0x185F     Telemetry: 0x0850 - 0x085F
**       Physics = Commands: 0x1840 - 0x184F     Telemetry: 0x0840 - 0x084F
**       IMU     = Commands: 0x18D0 - 0x18DF     Telemetry: 0x08D0 - 0x08DF
**       GPS     = Commands: 0x18E0 - 0x18EF     Telemetry: 0x08E0 - 0x08EF
*/

/* 
** Commands - these can be either plain ol' messages or "ground commands" that
**            have associated command codes (cmdCodes)  
*/
#define SAMPLE_STF1_APP_CMD_MID         0x1830      /* todo change this for your app */ 

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define SAMPLE_STF1_APP_SEND_HK_MID     0x1831      /* todo change this for your app */

/* 
** telemetry message IDs - these messages are meant for publishing messages
** containing telemetry from the application needs sent to other apps or
** to the ground
*/
#define SAMPLE_STF1_APP_HK_TLM_MID      0x0830      /* todo change this for your app */


#endif
