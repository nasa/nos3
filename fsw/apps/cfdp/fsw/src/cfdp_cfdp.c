#include "cfe.h"

#include "cfdp_app.h"
#include "cfdp_cfdp.h"
#include "cfdp_events.h"
#include "cfdp_msgids.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

void CFDP_CFDP_SendFile(const char *source, const char *destination)
{
	CFDP_CFDP_Transaction_t transaction;

	memset(&transaction, 0, sizeof(transaction));
	transaction.source = source;
	strncpy(CFDP_AppData.fileInfo.destination, destination, sizeof(CFDP_AppData.fileInfo.destination)-1);
	strncpy(CFDP_AppData.fileInfo.source, source, sizeof(CFDP_AppData.fileInfo.source)-1);
	CFDP_AppData.fileInfo.destination[sizeof(CFDP_AppData.fileInfo.destination)-1] = 0;
	CFDP_AppData.fileInfo.source[sizeof(CFDP_AppData.fileInfo.source)-1] = 0;
	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Preparing to send file: %s", CFDP_AppData.fileInfo.destination);
	transaction.CFDP_class = CFDP_CFDP_CLASS_1;
	transaction.chunk_size = CFDP_CHUNK_SIZE*16;

	CFDP_CFDP_StartSendFileTransaction(&transaction);
}

void CFDP_CFDP_GetFile(uint8_t pdu_type, const char *buffer, const char *destination, uint16_t length)
{
	if(pdu_type == 2)
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: EOF Reached");
		CFDP_CFDP_SendEOF();
		return;
	}
	CFDP_CFDP_Transaction_t transaction;

	memset(&transaction, 0, sizeof(transaction));
	transaction.destination = destination;
	transaction.chunk_size = length;

	CFDP_CFDP_StartGetTransaction(&transaction, buffer);
}

void CFDP_CFDP_StartSendFileTransaction(CFDP_CFDP_Transaction_t *transaction)
{
	int32_t status;
	osal_id_t fd;

	status = OS_OpenCreate(&fd, transaction->source, OS_FILE_FLAG_NONE, OS_READ_ONLY);
	if(status != OS_SUCCESS)
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to open source: %s with error: %d", transaction->source, (int)status);
		return;
	}

	size_t size;

	while(1)
	{
		size = OS_read(fd, CFDP_AppData.fileInfo.buffer, transaction->chunk_size);

		if(size < 0)
		{
			CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to read from source file. Read %ld bytes", size);
			OS_close(fd);
			return;
		}
		else
		{
			if(size == 0)
			{
				CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Reached EOF");
				break;
			}
			status = CFDP_CFDP_SendPDU();
			if(status != CFE_SUCCESS)
			{
				CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to send file data PDU");
				OS_close(fd);
				return;
			}

		}
		sleep(2);	
	}

	status = CFDP_CFDP_SendEOF();
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to send EOF");
	}
	else
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: File send complete");
	}

	OS_close(fd);
}

void CFDP_CFDP_StartGetTransaction(CFDP_CFDP_Transaction_t *transaction, const char* buffer)
{
	int32 status;
	osal_id_t fd;

	status = OS_OpenCreate(&fd, transaction->destination, OS_FILE_FLAG_CREATE , OS_READ_WRITE);
	if(status != OS_SUCCESS)
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to open destination: %s with error: %d", transaction->destination, status);
		return;
	}
	status = OS_lseek(fd, 0, OS_SEEK_END);
	if(status < OS_SUCCESS)
	{
		CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to seek end of file");
		OS_close(fd);
		return;
	}

	//CFDP_PrintMe(buffer);
	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: About to write %d bytes to file", transaction->chunk_size);
	for(int i=0; i < transaction->chunk_size; i+=2)
	{
		uint8_t byte = hexPairToByte(&buffer[i]);
		status = OS_write(fd, &byte, 1);
		if(status < OS_SUCCESS)
		{
			CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Failed to write to destination file: Error: %d", (int)status);
			OS_close(fd);
			return;
		}
	}
	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "CFDP: Wrote to file with status: %d", status);
	OS_close(fd);
}

int32_t CFDP_CFDP_SendRequest(const char *destination, const char *source)
{
	strncpy(CFDP_AppData.fileInfo.destination, destination, sizeof(CFDP_AppData.fileInfo.destination)-1);
	strncpy(CFDP_AppData.fileInfo.source, source, sizeof(CFDP_AppData.fileInfo.source)-1);
	CFDP_AppData.fileInfo.destination[sizeof(CFDP_AppData.fileInfo.destination)-1] = 0;
	CFDP_AppData.fileInfo.source[sizeof(CFDP_AppData.fileInfo.source)-1] = 0;
	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Preparing to receive file %s and save to %s", CFDP_AppData.fileInfo.destination, CFDP_AppData.fileInfo.source);

	int32 status;
	CFDP_AppData.fileInfo.pdu_type = 0;
	CFDP_AppData.fileInfo.direction = 2;

	CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo);
    status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo, true);
	if (status == CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Successfully sent telemetry packet");
        return CFE_SUCCESS;
    }
    else
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Error sending tlm packet: %d", status);
    	return status;
    }
}

int32_t CFDP_CFDP_SendEOF()
{
	int32 status;
	CFDP_AppData.fileInfo.pdu_type = 2;
	CFDP_AppData.fileInfo.direction = 1;

	CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo);
    status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo, true);
	if (status == CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Successfully sent telemetry packet");
        return CFE_SUCCESS;
    }
    else
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Error sending tlm packet: %d", status);
    	return status;
    }
}

int32 CFDP_CFDP_SendPDU()
{
	int32 status;
	CFDP_AppData.fileInfo.pdu_type = 0;
	CFDP_AppData.fileInfo.direction = 1;

	CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo);
    status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CFDP_AppData.fileInfo, true);
    if (status == CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Successfully sent telemetry packet for %s", CFDP_AppData.fileInfo.destination);
        return CFE_SUCCESS;
    }
    else
    {
    	CFE_EVS_SendEvent(CFDP_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "CFDP: Error sending tlm packet: %d", status);
    	return status;
    }

	
}

uint8_t hexPairToByte(const char *hex_pair)
{
	char buf[3] = {hex_pair[0], hex_pair[1], '\0'};
	return (uint8_t) strtol(buf, NULL, 16);
}
