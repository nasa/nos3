/*******************************************************************************
** File:
**   nav_msg.h 
**
** Purpose: 
**   Navigation (NAV) App messages
**
** Notes:
**
*******************************************************************************/

#ifndef _nav_msg_h_
#define _nav_msg_h_


/*******************************************************************************
/ ** Command Codes 
********************************************************************************/
#define NAV_NOOP_CC           0		/* NOOP Command 								*/
#define NAV_REQ_DATA_CC		  1		/* Command code to request data of gps */
#define NAV_RST_COUNTERS_CC   2     /* Command code to reset all app counters 		*/

typedef struct
{
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
} NAV_NoArgsCmd_t;


/* This struct is used to serialize the pertinent gps data to send to NOS Engine and to cFE FSW
   todo - this is a temporary workaround until the NOS Engine serialization is exposed on the C interface
*/
typedef struct
{
    uint32_t weeks;
    uint32_t seconds_into_week;
    double fractions;
    double ECEF_X;
    double ECEF_Y;
    double ECEF_Z;
    double vel_x;
    double vel_y;
    double vel_z;
} GPSSerialiation;


/*******************************************************************************
/ ** Messages
********************************************************************************/
typedef struct
{
    uint8    header[CFE_SB_TLM_HDR_SIZE];
    uint8    cmd_error_count;
    uint8    cmd_count;
    
	/* todo - put gps hk variables here */
    GPSSerialiation gps_data;

} NAV_HkTlm_t;

#define NAV_HK_TLM_LENGTH sizeof(NAV_HkTlm_t)

#endif


