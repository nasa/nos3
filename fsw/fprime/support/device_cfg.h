#ifndef _SAMPLE_CHECKOUT_DEVICE_CFG_H_
#define _SAMPLE_CHECKOUT_DEVICE_CFG_H_

/*
** SAMPLE Checkout Configuration
*/
#define SAMPLE_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define SAMPLE_CFG_STRING           "/dev/usart_29"
#define SAMPLE_CFG_HANDLE           29 
#define SAMPLE_CFG_BAUDRATE_HZ      115200
#define SAMPLE_CFG_MS_TIMEOUT       250
#define SAMPLE_CFG_DEBUG

#endif /* _SAMPLE_CHECKOUT_DEVICE_CFG_H_ */
