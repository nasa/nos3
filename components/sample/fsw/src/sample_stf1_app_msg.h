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


/*******************************************************************************
** File:
**   sample_stf1_app_msg.h
**
** Purpose:
**  Define SAMPLE STF1 App Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _SAMPLE_STF1_APP_MSG_H_
#define _SAMPLE_STF1_APP_MSG_H_

/*
** SAMPLE App command codes
*/
#define SAMPLE_APP_NOOP_CC                 0
#define SAMPLE_APP_RESET_COUNTERS_CC       1
/* todo - add application dependent command codes here */



/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} SAMPLE_NoArgsCmd_t;


/*
** Type definition (SAMPLE STF1 App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              CommandErrorCount;
    uint8              CommandCount;
  
    /*
    ** todo - add app specific telemetry values to this struct
    */

} SAMPLE_Hk_tlm_t;

#define SAMPLE_STF1_APP_HK_TLM_LNGTH  sizeof ( SAMPLE_Hk_tlm_t )

#endif 
