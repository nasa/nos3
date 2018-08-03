/* File: md_unit_test2.c
**  $Id: md_unit_test2.c 1.2 2012/01/09 19:28:15EST aschoeni Exp  $
**
** Purpose: 
**   Test driver for unit testing of CFS Memory Dwell Application.
**   Tests error return from CFE_SB_RcvMsg.
**
** References:
**
** Assumptions and Notes:
* Output can be directed either to screen or to file.
 * To direct output to screen,
 *    comment in '#define UTF_USE_STDOUT' statement in the
 *    utf_custom.h file.
 *
 * To direct output to file,
 *    comment out '#define UTF_USE_STDOUT' statement in
 *    utf_custom.h file.
 */

#include <stdlib.h>  /* for malloc */

#include "cfe.h"
#include "ccsds.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osfilesys.h"
#include "utf_osapi.h"
#include "utf_cfe_es.h"
#include "cfe_es_cds.h"	/* for CFE_ES_CDS_EarlyInit */
#include "cfe_tbl_internal.h"  /* for CFE_TBL_EarlyInit */
#include "utf_cfe.h"
#include "md_app.h"
#include "md_msgids.h"
#include "utf_cfe_psp.h"
#include "md_tbldefs.h"


extern MD_AppData_t MD_AppData;

#define CFS_MD_CMD_PIPE 1

    /********************************/
    /* Special Command Functions    */
    /********************************/
void UTF_SCRIPT_SetPipeReadError(int argc,char *argv[])
/* Will cause RcvMsg routine to return Pipe Read Error. */
{
    int32 Status;
    UTF_put_text("Entered UTF_SCRIPT_SetPipeReadError\n");
    Status = UTF_CFE_SB_Set_Api_Return_Code((int32)CFE_SB_RCVMSG_PROC, (uint32)CFE_SB_PIPE_RD_ERR); 
    if (Status == CFE_SUCCESS)
       UTF_put_text("RcvMsg set to return error\n");
    else
       UTF_put_text("ERROR: unable to set RcvMsg to return error\n");
  
}

int main(void)
{	
    int32 MemDwellAppId = 5;
    char MemDwellAppName[10];
	
    /********************************/
    /* Set up input file           */
    /********************************/
    UTF_add_input_file(CFS_MD_CMD_PIPE, "md_unit_test2.in");
    MD_AppData.CmdPipe = CFS_MD_CMD_PIPE;

    /********************************/
    /* Set up output file           */
    /********************************/
    UTF_set_output_filename("md_unit_test2.out");
    #ifdef UTF_USE_STDOUT 
    UTF_put_text("Std output is being used. \n");
    #endif

    /**************************************************/
    /* Initialize Unit Test Framework                 */ 
    /**************************************************/
    /* Initialize the CDS */
    UTF_put_text("\n***Initialize UTF ***\n");
    CFE_ES_CDS_EarlyInit();
    UTF_CFE_Init();
	
    /********************************/
    /* Initialize simulation time   */
    /********************************/
    UTF_init_sim_time(10000);

    /**************************************************/
    /* Exercise those errors which cause App to exit */
    /* without  executing the RcvMsg call.           */
    /**************************************************/

    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    /**************************************************/
    /* Add tasks  to ES's list of tasks, and          */
    /* register local task with Executive Services.   */
    /**************************************************/
    UTF_put_text("\n***Add tasks to ES list of tasks ***\n");
    UTF_ES_InitTaskRecords();

    MemDwellAppId = UTF_ES_GetFreeTaskRecord();
    if (MemDwellAppId < 0)
    {
        UTF_put_text("\n***Error return from UTF_ES_GetFreeTaskRecord***\n");
        exit(0);
    }
    strcpy(MemDwellAppName,"MD_APP");
    UTF_ES_AddAppRecord(MemDwellAppName,  MemDwellAppId);	

    UTF_ES_DumpAppRecords();
    
    /*****************************************************************/
    /* Test case: Error return from CFE_SB_RcvMsg.                   */
    /*****************************************************************/
/*    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n"); 
    UTF_put_text("Test case:  Error return from CFE_SB_RcvMsg.\n"); 
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_RCVMSG_PROC, CFE_SB_PIPE_RD_ERR); 
    MD_AppMain(); 
    UTF_put_text("\nControl returned to test driver \n"); 
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_RCVMSG_PROC);  */


    /**************************************************/
    /* Add "Special" Commands                         */
    /**************************************************/
    UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
    UTF_add_special_command("USE_DEFAULT_SB_RETURN_CODE", UTF_SCRIPT_SB_Use_Default_Api_Return_Code);
    UTF_add_special_command("SET_PIPE_READ_ERROR",  UTF_SCRIPT_SetPipeReadError);
 

   /**************************************************/
   /* Start Memory Dwell application                 */
   /**************************************************/
        
   UTF_put_text("\n*** Start Memory Dwell Main Task ***\n"); 
  MD_AppMain(); 

   /**************************************************/
   /* Here we've reached the end of input file processing */
   /**************************************************/

   /**************************************************/
   /* Test Table Services APIs                       */
   /**************************************************/
   UTF_CFE_TBL_DisplayTableRegistryContents();

   exit(0);
}
