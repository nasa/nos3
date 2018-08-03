/* File: hk_tst_drv.c
 *
 * Copyright © 2007-2014 United States Government as represented by the 
 * Administrator of the National Aeronautics and Space Administration. 
 * All Other Rights Reserved.  
 *
 * This software was created at NASA's Goddard Space Flight Center.
 * This software is governed by the NASA Open Source Agreement and may be 
 * used, distributed and modified only pursuant to the terms of that 
 * agreement. 
 * 
 * This test driver is used to invoke HK application code and prepare the 
 * HK_AppMain function to receive messages that will be sent from the 
 * script file sbsim.in
 * 
 * $Log: utf_test_hk.c  $
 * Revision 1.1.1.5 2015/03/04 14:58:29EST sstrege 
 * Added copyright information
 * Revision 1.1.1.4 2011/09/19 17:24:26EDT jmdagost 
 * Updated unit test and results files.
 * Revision 1.1.1.3 2010/05/11 16:20:01EDT jmdagost 
 * Major revisions to unit test and associated files, plus results.
 * Revision 1.1.1.2 2008/10/17 15:47:22EDT rjmcgraw 
 * Initial revision
 * Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/project.pj
 * Revision 1.1 2008/10/17 15:29:41EDT rjmcgraw 
 * Initial revision
 * Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/unit_test/project.pj
 * Revision 1.1 2008/04/09 16:43:33EDT rjmcgraw 
 * Initial revision
 * Member added to project .../CFS-REPOSITORY/hk/fsw/unit_test/project.pj
 *
 */ 

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "hk_app.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_cfe.h"


/************************************************************************
** HK global data
*************************************************************************/
extern  HK_AppData_t HK_AppData;

#define HK_CMD_PIPE		  1


/************************************************************************
** Function prototypes
*************************************************************************/
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);
int32 CFE_ES_PoolCreateHook(CFE_ES_MemHandle_t  *HandlePtr,
                        uint8               *MemPtr,
                        uint32               Size );
int32 CFE_TBL_RegisterHook( CFE_TBL_Handle_t *TblHandlePtr,
                        const char *Name,
                        uint32  Size,
                        uint16  TblOptionFlags,
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr );
int32 CFE_TBL_ManageHook( CFE_TBL_Handle_t TblHandle );
int32 CFE_TBL_GetAddressHook( void **TblPtr,CFE_TBL_Handle_t TblHandle );


/************************************************************************
** Script version for LoadTableFromGround 
*************************************************************************/
void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[])
{
	int debug = 1;
	char Table_Name[30], File_Name[50];
	if (debug) UTF_put_text("Entered UTF_SCRIPT_LoadTableFromGround\n");
	if (argc != 3)
	{
	   UTF_error("<utf_test_hk:UTF_SCRIPT_LoadTableFromGround> Error: Read %d args w/script cmd LOAD_TABLE_FROM_GROUND. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}

	strcpy(Table_Name,argv[1]);
	strcpy(File_Name,argv[2]);
    UTF_put_text("<utf_test_hk:UTF_SCRIPT_LoadTableFromGround> Table_Name is %s\n",Table_Name);
    UTF_put_text("<utf_test_hk:UTF_SCRIPT_LoadTableFromGround> File_Name is %s\n",File_Name);
    UTF_put_text("--------------------------------------------------------------------------------\n");
	if (debug) UTF_put_text("Table_Name is %s\n",Table_Name); 
	if (debug) UTF_put_text("File_Name is %s\n", File_Name); 
	UTF_TBL_LoadTableFromGround(Table_Name, File_Name);
	return;
}


/************************************************************************
** HK Unit Test Main Routine
*************************************************************************/
int main(void)
{

	/*******************************************************************/
	/* Add special commands to simplify the input script file sbsim.in */		
	/*******************************************************************/	
	UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
	UTF_add_special_command("LOAD_TABLE_FROM_GROUND", UTF_SCRIPT_LoadTableFromGround);
	UTF_add_special_command("SET_TBL_RETURN_CODE",UTF_SCRIPT_TBL_Set_Api_Return_Code);
	UTF_add_special_command("SET_ES_RETURN_CODE",UTF_SCRIPT_ES_Set_Api_Return_Code);
	UTF_add_special_command("SET_DEFAULT_TBL_RETURN_CODE",UTF_SCRIPT_TBL_Use_Default_Api_Return_Code);
	UTF_add_special_command("SET_DEFAULT_SB_RETURN_CODE",UTF_SCRIPT_SB_Use_Default_Api_Return_Code);

	
	/**************************************************/
	/* Call utility to register task with             */
	/* 		Executive Services.                       */
	/**************************************************/
	UTF_ES_InitAppRecords();
	UTF_ES_AddAppRecord("HK",0);    
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);


	/**************************************************/
	/* Initialize Table Services data structures      */
	/**************************************************/
    CFE_ES_CDS_EarlyInit();/* seg fault when not here?*/
    CFE_TBL_EarlyInit();
    

	/***************************************************/
	/* Add a simulated on-board /cf directory.		   */
	/* This directory is an actual folder on the local */ 
	/* file system located in the unit_test directory  */
	/***************************************************/    
    UTF_add_volume("/", "cf", FS_BASED, FALSE, FALSE, TRUE, "CF", "/cf", 0);
         	
    
    /************************************************************************
    ** Start the Unit Test
    *************************************************************************/
    UTF_put_text("***************************************\n");
    UTF_put_text("* <utf_hk_test> HK Init Failure Tests *\n");
    UTF_put_text("***************************************\n");
    UTF_put_text("\n");

    /*
    ** Test path that terminates HK due to failure during EVS registration 
    */
    UTF_put_text("<utf_hk_test> Test Init with EVS Reg Failure \n");
    UTF_put_text("-------------------------------------------- \n");   
    UTF_CFE_EVS_Set_Api_Return_Code(0,-1);    
    HK_AppMain();
    UTF_CFE_EVS_Use_Default_Api_Return_Code(0);
    UTF_put_text("\n");
    
    /*
    ** Test path that terminates HK due to failure during CFE_SB_CreatePipe 
    */
    UTF_put_text("<utf_hk_test> Test Init with Create Pipe Failure \n");
    UTF_put_text("------------------------------------------------ \n");
    UTF_CFE_SB_Set_Api_Return_Code(0,-1);    
    HK_AppMain();
    UTF_CFE_SB_Use_Default_Api_Return_Code(0);
    UTF_put_text("\n");
    
    /* Use custom hook function to get failures on each of the subscribe calls */
    /* We'll be leaving this hook function in place throughout the test because*/
    /* later in the test we need to fail a subscribe call again 			   */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *)&CFE_SB_SubscribeHook);
    
    /*
    ** Test path that terminates HK due to failure during subscription for
    ** cmd to send out pkt. 
    */
    UTF_put_text("<utf_hk_test> Test Init with Subscription1 Failure \n");
    UTF_put_text("-------------------------------------------------- \n");
    HK_AppMain();
    UTF_put_text("\n");
    
    /*
    ** Test path that terminates HK due to failure during subscription for
    ** HK Request pkt. 
    */
    UTF_put_text("<utf_hk_test> Test Init with Subscription2 Failure \n");
    UTF_put_text("-------------------------------------------------- \n");
    HK_AppMain();
    UTF_put_text("\n");
    
    /*
    ** Test path that terminates HK due to failure during subscription for
    ** hk ground commands 
    */
    UTF_put_text("<utf_hk_test> Test Init with Subscription3 Failure \n");
    UTF_put_text("-------------------------------------------------- \n");
    HK_AppMain();
    UTF_put_text("\n");
    
    /* Use hook function so that we can get a failure on the pool create call */
    UTF_ES_set_function_hook(CFE_ES_POOLCREATE_HOOK, (void *)&CFE_ES_PoolCreateHook);
    
    /*
    ** Test path that terminates HK due to failure during mem pool create
    */
    UTF_put_text("<utf_hk_test> Test Init with Memory Pool Create Failure \n");
    UTF_put_text("------------------------------------------------------- \n");
    HK_AppMain();
    UTF_put_text("\n");    
    
    /* go back to using the utf provided stub function for pool create */
    UTF_ES_set_function_hook(CFE_ES_POOLCREATE_HOOK, (void *)NULL);   
    
    
    /*
    ** Use hook function because second call needs to fail for the runtime 
    ** table registration. Could have used 'set return code' for first call
    ** (Cpy Tbl Reg), then hook for second call (runtime tbl reg), but wanted
    ** to be consistent
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, (void *)&CFE_TBL_RegisterHook);   
    
    /*
    ** Test path that terminates HK due to failure during copy table register 
    */
    UTF_put_text("<utf_hk_test> Test Init with Cpy Tbl Reg Failure \n");
    UTF_put_text("------------------------------------------------ \n");    
    HK_AppMain();
    UTF_put_text("\n");

    /*
    ** Test path that terminates HK due to failure during copy table register 
    */
    UTF_put_text("<utf_hk_test> Test Init with Runtime Tbl Reg Failure \n");
    UTF_put_text("---------------------------------------------------- \n");    
    HK_AppMain();
    UTF_put_text("\n");
        
    /*
    ** Done using hook function, set back to original utf stub 
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, (void *)NULL);
    
    /*
    ** Test path that terminates HK due to failure during copy table load 
    */
    UTF_put_text("<utf_hk_test> Test Init with Cpy Tbl Load Failure \n");
    UTF_put_text("------------------------------------------------- \n");
    UTF_CFE_TBL_Set_Api_Return_Code(3,-1);    
    HK_AppMain();
    UTF_CFE_TBL_Use_Default_Api_Return_Code(3);
    UTF_put_text("\n");
       
 
    /*
    ** Use hook function CFE_TBL_ManageHook 
    */
    UTF_TBL_set_function_hook(CFE_TBL_MANAGE_HOOK, (void *)&CFE_TBL_ManageHook);
       
    /*
    ** Test path that terminates HK due to failure during cpy table manage 
    */
    UTF_put_text("<utf_hk_test> Test Init with Cpy Tbl Manage Failure \n");
    UTF_put_text("--------------------------------------------------- \n"); 
    HK_AppMain();
    UTF_put_text("\n");
    

    /*
    ** Test path that terminates HK due to failure during runtime table manage
    */
    UTF_put_text("<utf_hk_test> Test Init with Runtime Tbl Manage Failure \n");
    UTF_put_text("------------------------------------------------------- \n");   
    HK_AppMain();
    UTF_put_text("\n");
    
    /*
    ** Done using hook function, set back to original utf stub 
    */
    UTF_TBL_set_function_hook(CFE_TBL_MANAGE_HOOK, (void *)NULL);
    
    
    /*
    ** Use hook function CFE_TBL_GetAddressHook 
    */
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *)&CFE_TBL_GetAddressHook);

    /*
    ** Test path that terminates HK due to failure during cpy table get adr 
    */
    UTF_put_text("<utf_hk_test> Test Init with Cpy Tbl GetAdr Failure \n");
    UTF_put_text("--------------------------------------------------- \n");   
    HK_AppMain();
    UTF_put_text("\n");
    
    /*
    ** Test path that terminates HK due to failure during runtim table get adr 
    */
    UTF_put_text("<utf_hk_test> Test Init with Runtime Tbl GetAdr Failure \n");
    UTF_put_text("------------------------------------------------------- \n");
    HK_AppMain();
    UTF_put_text("\n");
        
    /*
    ** Done using hook function, set back to original utf stub 
    */
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *)NULL); 
       
    
    /*
    ** Test path that terminates HK due to failure while sending init event 
    */
    UTF_put_text("<utf_hk_test> Test Init with SendEvent Failure \n");
    UTF_put_text("---------------------------------------------- \n");
    UTF_CFE_EVS_Set_Api_Return_Code(2,-1);    
    HK_AppMain();
    UTF_CFE_EVS_Use_Default_Api_Return_Code(2);
    UTF_put_text("\n");   

    /*
    ** Test no errors path of HK main
    ** For some??? reason, you need to call these 2 ES functions again. 
    ** Otherwise, HK_AppMain fails before it gets into the while loop
    */
    UTF_put_text("<utf_hk_test> Test HK_AppMain with No Failures \n");
    UTF_put_text("---------------------------------------------- \n");
    UTF_ES_InitAppRecords();
	UTF_ES_AddAppRecord("HK",0);
    HK_AppMain();
    UTF_put_text("\n");
    
    return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_SB_Subscribe that will return an error    */
/* on some calls                              					   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) 
{
    static uint32 Count = 0;

    Count++;
    
    if ((Count == 1)||/* used to fail the first subscibe call in HK_AppInit */
    	(Count == 3)||/* used to fail the second subscibe call in HK_AppInit */
    	(Count == 6)||/* used to fail the third subscibe call in HK_AppInit */
    	(Count == 58))/* used to fail subscibe call in HK_ProcessNewCopyTable */
    {
    	UTF_put_text("<utf_hk_test:CFE_SB_SubscribeHook> Subscribe returning error Cnt = %d\n",Count);
        UTF_put_text("---------------------------------------------------------------------\n");
    	return(CFE_SB_MAX_MSGS_MET);
    }else{
    	UTF_put_text("<utf_hk_test:CFE_SB_SubscribeHook> Subscribe returning ok Cnt = %d\n",Count);
        UTF_put_text("------------------------------------------------------------------\n");
        return(CFE_SUCCESS);
    }
       
}/* end CFE_SB_SubscribeHook */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_ES_PoolCreate that will return an error    */
/* on some calls                              					   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_PoolCreateHook(CFE_ES_MemHandle_t  *HandlePtr,
                        uint8               *MemPtr,
                        uint32               Size )
{
    static uint32 Count = 0;

    Count++;
    
    if ((Count == 1))/* used to fail PoolCreate on first call */
    {
    	UTF_put_text("<utf_hk_test:CFE_ES_PoolCreateHook> PoolCreate returning error Cnt = %d\n",Count);
        UTF_put_text("-----------------------------------------------------------------------\n");
    	return(CFE_ES_BAD_ARGUMENT);
    }else{
    	UTF_put_text("<utf_hk_test:CFE_ES_PoolCreateHook> PoolCreate returning ok Cnt = %d\n",Count);
        UTF_put_text("--------------------------------------------------------------------\n");
       return(CFE_SUCCESS);
    }
       
}/* end CFE_ES_PoolCreateHook */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Register that will return an error    */
/* on some calls                              					   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_RegisterHook( CFE_TBL_Handle_t *TblHandlePtr,
                        const char *Name,
                        uint32  Size,
                        uint16  TblOptionFlags,
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr ) 
{
    static uint32 Count = 0;

    Count++;
    
    if ((Count == 1)||/* used to fail the first register call in HK_TableInit */
    	(Count == 3))/* used to fail the second register call in HK_TableInit */
    {
    	UTF_put_text("<utf_hk_test:CFE_TBL_RegisterHook> Register returning error Cnt = %d\n",Count);
        UTF_put_text("--------------------------------------------------------------------\n");
    	return(CFE_TBL_ERR_INVALID_SIZE);
    }else{
    	UTF_put_text("<utf_hk_test:CFE_TBL_RegisterHook> Register returning ok Cnt = %d\n",Count);
        UTF_put_text("-----------------------------------------------------------------\n");
       return(CFE_SUCCESS);
    }
       

}/* end CFE_TBL_RegisterHook */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Manage that will return an error    */
/* on some calls                              					   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_ManageHook( CFE_TBL_Handle_t TblHandle )
{
    static uint32 Count = 0;

    Count++;
    
    if ((Count == 2)||/* used to fail the first manage call in HK_TableInit */
    	(Count == 5))/* used to fail the second manage call in HK_TableInit */
    {
    	UTF_put_text("<utf_hk_test:CFE_TBL_ManageHook> Manage returning error Cnt = %d\n",Count);
        UTF_put_text("----------------------------------------------------------------\n");
    	return(CFE_TBL_ERR_UNREGISTERED);
    }else{
    	UTF_put_text("<utf_hk_test:CFE_TBL_ManageHook> Manage returning ok Cnt = %d\n",Count);
        UTF_put_text("-------------------------------------------------------------\n");
       return(CFE_SUCCESS);
    }
       
}/* end CFE_TBL_ManageHook */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_GetAddress that will return an error  */
/* on some calls                              					   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_GetAddressHook( void **TblPtr,
                          CFE_TBL_Handle_t TblHandle )
{
    static uint32 Count = 0;

    Count++;
    
    if ((Count == 1)||/* used to fail the first GetAddress call HK_TableInit */
    	(Count == 3))/* used to fail the second GetAddress call HK_TableInit */
    {
    	UTF_put_text("<utf_hk_test:CFE_TBL_GetAddressHook> GetAddress returning Unregistered Cnt = %d\n",Count);
        UTF_put_text("-------------------------------------------------------------------------------\n");
    	return(CFE_TBL_ERR_UNREGISTERED);
    }else if(Count == 2){
    	UTF_put_text("<utf_hk_test:CFE_TBL_GetAddressHook> GetAddress returning InfoUpdated Cnt = %d\n",Count);
        UTF_put_text("------------------------------------------------------------------------------\n");
    	return CFE_TBL_INFO_UPDATED;
    }else{
    	UTF_put_text("<utf_hk_test:CFE_TBL_GetAddressHook> GetAddress returning ok Cnt = %d\n",Count);
        UTF_put_text("---------------------------------------------------------------------\n");
       return(CFE_SUCCESS);
    }
       
}/* end CFE_TBL_GetAddressHook */


