/************************************************************************
** File:
**   $Id: nav_utest.c 1.4 
**
** Purpose: 
**   This is a test driver used to unit test the STF-1 Navigation Application.
**
** 
**   Output can be directed either to screen or to file:
**   To direct output to screen, 
**      comment in '#define UTF_USE_STDOUT' statement in the
**      utf_custom.h file.
**
**   To direct output to file, 
**      comment out '#define UTF_USE_STDOUT' statement in 
**      utf_custom.h file.
** 
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "nav_app.h"            /* Application headers */
#include "nav_perfids.h"
#include "nav_msgids.h"
#include "nav_events.h"
#include "nav_msg.h"
#include "nav_version.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"
#include "utf_cfe.h"

#include <stdlib.h>            /* System headers      */

/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS


/************************************************************************
** NAV global data external to this file
*************************************************************************/
extern int32 CmdNoopCounter;
extern int32 CmdReqDataCounter;
extern int32 CmdInvalidCounter;


/************************************************************************
** Non-public functions in NAV that we need prototypes for so we
** can call them
************************************************************************/
void NAV_AppInit(void);
void NAV_ResetCounters(void);



/************************************************************************
** Local data
*************************************************************************/





/************************************************************************
** Global variables used by function hooks
*************************************************************************/



/************************************************************************
** Local function prototypes
*************************************************************************/
void Test_NAVInit        (void);
void Test_MCCmdPipe      (void);



/*
** Prototypes for function hooks
*/
int32 CFE_SB_Subscribe_FailOnN (CFE_SB_MsgId_t  MsgId,
                                CFE_SB_PipeId_t PipeId);



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* NAV unit test program main                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
   /*
   ** Set up output file and HK packet handler           
   */
   UTF_set_output_filename("nav_utest.out");
    
   /*
   ** Initialize time data structures
   */
   UTF_init_sim_time(0.0);

   /*
   ** Initialize ES application data                         
   */
   UTF_ES_InitAppRecords();
   UTF_ES_AddAppRecord("NAV",0);  
   CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
     
    printf("*** NAV UNIT TEST COMMAND PIPE TESTS START ***\n\n");
   
   Test_NAVCmdPipe();
   
   printf("*** NAV UNIT TEST COMMAND PIPE TESTS END   ***\n\n");
   
   /*
   ** Run tests that invoke NAV code directly 
   */
   printf("*** NAV UNIT TEST DRIVER TESTS START ***\n\n");

   /*LC_OperData.ARTPtr = LC_UnitTestART;
   LC_OperData.WRTPtr = LC_UnitTestWRT;
   Test_HKRequest();

   LC_OperData.ADTPtr = LC_UnitTestADT;
   LC_OperData.WDTPtr = LC_UnitTestWDT;

   Test_ValidateWDT();
   Test_ValidateADT();

   Test_SetAPState();
   Test_SetAPOff();
   Test_SampleAP();
   Test_WPProcessing();
   Test_CmdHandlers();
   Test_Coverage();
   */
   
   printf("*** NAV UNIT TEST DRIVER TESTS END   ***\n\n");
   
   return 0;
   
} /* end main */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs command pipe tests                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_NAVCmdPipe (void)
{
    CFE_SB_PipeId_t  CmdPipe_ID;

    UTF_put_text("\n");
    UTF_put_text("*************************\n");
    UTF_put_text("* NAV Command Pipe Tests *\n");
    UTF_put_text("*************************\n");
    UTF_put_text("\n");
    
    /*
    ** Re-Initialize ES application data otherwise our earlier
    ** tests will cause CFE_ES_RunLoop to not return TRUE and
    ** NAV will exit right away
    */
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord("NAV",0);  
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /*
    ** Set up to read in script through command pipe
    ** The command pipe ID needs to match the value that will be
    ** returned by the next call to CFE_SB_CreatePipe.  This is the first
	** pipe created so in our case, CmdPipe_ID = 1
    */
	CmdPipe_ID = 1;
    UTF_add_input_file(CmdPipe_ID, "nav_utest.in");

    /*
    ** Add this hook so we can force a software bus read error
    ** in the nav_utest.in input file that will make the application 
    ** exit and return control to this function.
    */
    UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
    	
	printf("here1!\n"); 
    /*
    ** Call app main
    */
    NAV_AppMain();
	
	printf("here2!\n");
	
	/** Verify that 5 NOOP command were sent.  Refer to the msg_utest.in file for
	 ** a listing of all commands
	 */
	 if(CmdNoopCounter == 5)
	 {
		 printf("PASSED!\n");
		 UTF_put_text("CmdNoopCounter: PASSED\n");
	 }
	 else
	 {
		printf("FAILED!\n");
		UTF_put_text("CmdNoopCounter: FAILED\n");
	 }
	 
	 
		 
    
} /* end Test_NAVCmdPipe */








