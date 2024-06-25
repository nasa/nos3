#ifndef _NEWSIM_CHECKOUT_DEVICE_CFG_H_
#define _NEWSIM_CHECKOUT_DEVICE_CFG_H_

/*
** NEWSIM Checkout Configuration
*/
#define NEWSIM_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define NEWSIM_CFG_STRING           "/dev/usart_29"
#define NEWSIM_CFG_HANDLE           29 
#define NEWSIM_CFG_BAUDRATE_HZ      115200
#define NEWSIM_CFG_MS_TIMEOUT       250
#define NEWSIM_CFG_DEBUG

#endif /* _NEWSIM_CHECKOUT_DEVICE_CFG_H_ */
