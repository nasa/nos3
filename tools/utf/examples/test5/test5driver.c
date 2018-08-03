/* File: test5driver.c
 * 
 * This test driver is used to demonstrate the use of
 * UTF utility to set return codes.
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
 * 2005/10/26 nschweiss Original code.
 * 2005/12/20 nschweiss Revised to show procedure calls from the
 * test driver to modify API return codes .
 */ 

#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "cfe.h"
#include "cfe_tbl.h"
#include "cfe_tbl_task.h"
#include "cfe_mission_cfg.h" /* defines CCSDS msg format */
#include "ccsds.h"           /* defines CCSDS_PriHdr_t */
#include "cfe_sb.h"          /* defines CFE_SB_MsgPtr_t, etc. */
#include "cfe_msgids.h"		 /* defines UU message ids */
#include "utf_custom.h"      /* defines UTF_QUEUE_SIZE */
#include "utf_types.h"       /* defines UTF_queue_add() */
#include "cfe_time_utils.h"
#include "cfe_es_cds.h"		/* for CFE_ES_CDS_EarlyInit */
#include "utf_cfe_sb.h"          /* defines UTF_set_sb_receive_return_status */
#include "uu_app.h"
#include "string.h"
#include "cfe_tbl_internal.h"
#include "utf_cfe_es.h"
#include "utf_cfe_evs.h"
#include "utf_cfe_tbl.h"
#include "utf_osapi.h"

#define CFE_UU_INVALID_CC 55
#define CFE_UU_INVALID_MID 0x12
#define MESSAGE_FORMAT_IS_CCSDS
#define CFE_UU_CMD_PIPE		1
#define CFE_TBL_NUMBER_OF_TABLES 16

extern UU_AppData_t  UU_AppData;

void print_housekeeping (uint8 source, void *packet)
{

    UU_HkPacket_t *UU_HKStatusPkt =(UU_HkPacket_t *)packet;

    /* Output the UU housekeeping data */
    UTF_put_text("UU HOUSEKEEPING DATA:\n");

    UTF_put_text("command_counter                  = %d\n",   UU_HKStatusPkt->CmdCount);
    UTF_put_text("command_error_counter            = %d\n",   UU_HKStatusPkt->ErrCount);
    UTF_put_text("tbl update count 				= %d\n",   UU_AppData.HkPacket.UpdateCount);
    
}

int main(void)
{
	char AppName[10];
	strcpy(AppName, "UU");
	
	/********************************/
	/* Set up to read in script    */
	/********************************/

	UU_AppData.CmdPipe = CFE_UU_CMD_PIPE;  /* Hook for application code */						    
	
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("test5driver.ActualOutput");
	UTF_set_packet_handler(CFE_UU_HK_TLM_MID, print_housekeeping);
	
	
	/**************************************************/
	/* Initialize time data structures                */
	/**************************************************/
	UTF_init_sim_time(0.0);
	
	/**************************************************/
	/* Call utility to register task UU with          */
	/* 		Executive Services.                         */
	/**************************************************/
	UTF_ES_InitAppRecords();
	UTF_ES_AddAppRecord(AppName,0);	
    CFE_ES_RegisterApp();

	/**************************************************/
	/* Initialize data structures used by Table Services */
	/**************************************************/
    /* Initialize the CDS */
    CFE_ES_CDS_EarlyInit();	
	CFE_TBL_EarlyInit();

	/********************************/
	/* Get API values               */
	/********************************/
	UTF_put_text("\n**** Output from UU_DisplayApiValues: ****\n");
	UU_DisplayApiValues();

	/********************************/
	/* Set API return values        */
	/********************************/
	UTF_put_text("\n**** Set API return values: ****\n");	
	UTF_CFE_ES_Set_Api_Return_Code(0, 0xc4000002L);
	UTF_CFE_ES_Set_Api_Return_Code(1, 0xc4000001);
	UTF_CFE_ES_Set_Api_Return_Code(  2, 0xc400ffffL);
	UTF_CFE_EVS_Set_Api_Return_Code ( 2, 0xefefefef); 
    UTF_CFE_SB_Set_Api_Return_Code  (0, 0x357a357a);
    UTF_CFE_TBL_Set_Api_Return_Code (5, 0xbeadbabe);

	UTF_put_text("\n**** Output from UU_DisplayApiValues: ****\n");	
	UU_DisplayApiValues();

	/********************************/
	/* Revert to original API return values */
	/********************************/
	UTF_put_text("\n**** Revert to original API return values: ****\n");	
	UTF_CFE_ES_Use_Default_Api_Return_Code(0);
	UTF_CFE_ES_Use_Default_Api_Return_Code(1);
	UTF_CFE_ES_Use_Default_Api_Return_Code(2);
	UTF_CFE_EVS_Use_Default_Api_Return_Code(2);
/* 	UTF_CFE_FS_Use_Default_Api_Return_Code(0); */
	UTF_CFE_SB_Use_Default_Api_Return_Code(0);
	UTF_CFE_TBL_Use_Default_Api_Return_Code(5);
	
	UTF_put_text("\n**** Output from UU_DisplayApiValues: ****\n");	
	UU_DisplayApiValues();

		
	return 0;
}




