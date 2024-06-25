/************************************************************************
** File:
**   $Id: newsim_platform_cfg.h  $
**
** Purpose:
**  Define newsim Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _NEWSIM_PLATFORM_CFG_H_
#define _NEWSIM_PLATFORM_CFG_H_

/*
** Default NEWSIM Configuration
*/
#ifndef NEWSIM_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define NEWSIM_CFG_STRING           "usart_27"
    #define NEWSIM_CFG_HANDLE           27 
    #define NEWSIM_CFG_BAUDRATE_HZ      115200
    #define NEWSIM_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define NEWSIM_CFG_DEBUG
#endif

#endif /* _NEWSIM_PLATFORM_CFG_H_ */
