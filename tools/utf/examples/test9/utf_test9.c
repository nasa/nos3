/* File: utf_test9.c
 * 
 * This test driver is used to demonstrate that changes
 * made for Change Package 373:1 have been performed correctly.
 *
 * Output can be directed either to screen or to file.
 * To direct output to screen, 
 *    comment in '#define UTF_USE_STDOUT' statement in the
 *    utf_custom.h file.
 *
 * To direct output to file, comment out '#define UTF_USE_STDOUT' 
 * statement in utf_custom.h file.
 *
 * Changes
 * 2006/03/03 nschweiss Original code.
 */ 

#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
/* This needs to precede include cfe.h so that */
/* CFE_SB_TLM_HDR_SIZE will be defined.        */

#include "cfe.h"
#include "utf_custom.h"      /* defines UTF_QUEUE_SIZE */
#include "utf_types.h"       /* defines UTF_queue_add()*/
#include "string.h"


int main(void)
{
	/*****************************************************/
	/* Need to show that CFE_ES_MemHandle_t is available */
	/*****************************************************/
	CFE_ES_MemHandle_t MyMemHandle;
	
	/*****************************************************/
	/* Need to show that CFE_EVS_Packet_t is available */
	/*****************************************************/
	CFE_EVS_Packet_t MyEVSPacket;
	uint8 *MemPtr;
	int32 retval;
	uint32 HandlePtr;
	
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test9.ActualOutput");
	
	MyMemHandle = 5;
	strcpy(MyEVSPacket.Message, "Very Important EVS Packet Message");
	
	UTF_put_text("MyMemHandle = %d\n", MyMemHandle);
	UTF_put_text("MyEVSPacket.Message = <%s>\n",MyEVSPacket.Message);
	
	/*****************************************************/
	/* Need to show that CFE_ES_PoolCreate is available */
	/*****************************************************/
	MemPtr = (uint8 *)malloc(2000);

	retval = CFE_ES_PoolCreate(&HandlePtr, MemPtr, 1000);

	UTF_put_text("Call to CFE_ES_PoolCreate returned %d\n", retval);
	

		
	return 0;
}




