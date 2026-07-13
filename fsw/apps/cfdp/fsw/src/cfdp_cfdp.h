#ifndef CFDP_CFDP_H
#define CFDP_CFDP_H

#include "cfe.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CFDP_CHUNK_SIZE 256

typedef enum {
	CFDP_CFDP_CLASS_1 = 0,
	CFDP_CFDP_CLASS_2 = 1
} CFDP_CFDP_Class_t;

typedef struct
{
	const char *source;
	const char *destination;
	CFDP_CFDP_Class_t CFDP_class;
	uint32 chunk_size;	
	char buffer[CFDP_CHUNK_SIZE];
} CFDP_CFDP_Transaction_t;

void CFDP_CFDP_SendFile(const char *source, const char *destination);
void CFDP_CFDP_GetFile(uint8_t pdu_type, const char *buffer, const char *destination, uint16_t length);
void CFDP_CFDP_StartSendFileTransaction(CFDP_CFDP_Transaction_t *transaction);
void CFDP_CFDP_StartGetTransaction(CFDP_CFDP_Transaction_t *transaction, const char* buffer);
int32 CFDP_CFDP_SendPDU(void);
int32_t CFDP_CFDP_SendEOF(void);
int32_t CFDP_CFDP_IsEOFPDU(CFE_SB_Buffer_t *buffer);
int32_t CFDP_CFDP_SendRequest(const char *destination, const char *source);
uint8_t hexPairToByte(const char *hex_pair);

#endif