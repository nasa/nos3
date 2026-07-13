/************************************************************************
** File:
**   $Id: cfdp_platform_cfg.h  $
**
** Purpose:
**  Define sample Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _CFDP_PLATFORM_CFG_H_
#define _CFDP_PLATFORM_CFG_H_

/*
** Default CFDP Configuration
*/
#ifndef CFDP_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define CFDP_CFG_STRING           "usart_29"
    #define CFDP_CFG_HANDLE           29 
    #define CFDP_CFG_BAUDRATE_HZ      115200
    #define CFDP_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define CFDP_CFG_DEBUG
#endif

#endif /* _CFDP_PLATFORM_CFG_H_ */
