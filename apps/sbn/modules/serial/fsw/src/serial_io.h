#ifndef _serial_io_h_
#define _serial_io_h_

#include "cfe.h"
#include "sbn_interfaces.h"
#include "serial_sbn_if_struct.h"

/* Function declarations */

int32 Serial_IoOpenPort(char *DevName, uint32 BaudRate, int32 *Fd);
int32 Serial_IoSetAttrs(int32 Fd, uint32 Baud);
int32 Serial_IoReadMsg(Serial_SBNHostData_t *host); 
int32 Serial_IoReadSyncBytes(int32 Fd);
uint32 Serial_IoReadMessageSize(int32 Fd);
int32 Serial_IoWriteMsg(int32 Fd, void *Msg, size_t MsgSize);

void Serial_IoReadTaskMain(void); 
int32 Serial_IoStartReadTask(Serial_SBNHostData_t *host);

#endif

