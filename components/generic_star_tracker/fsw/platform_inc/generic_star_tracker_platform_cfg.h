/************************************************************************
** File:
**   $Id: generic_star_tracker_platform_cfg.h  $
**
** Purpose:
**  Define generic_star_tracker Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_STAR_TRACKER_PLATFORM_CFG_H_
#define _GENERIC_STAR_TRACKER_PLATFORM_CFG_H_

/*
** Default GENERIC_STAR_TRACKER Configuration
*/
#ifndef GENERIC_STAR_TRACKER_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_STAR_TRACKER_CFG_STRING           "usart_29"
    #define GENERIC_STAR_TRACKER_CFG_HANDLE           29 
    #define GENERIC_STAR_TRACKER_CFG_BAUDRATE_HZ      115200
    #define GENERIC_STAR_TRACKER_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_STAR_TRACKER_CFG_DEBUG
#endif

#endif /* _GENERIC_STAR_TRACKER_PLATFORM_CFG_H_ */
