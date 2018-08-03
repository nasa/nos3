/* File: utf_test16.c
**  $Id: utf_test16.c 1.2 2008/07/30 12:05:07GMT-05:00 wfmoleski Exp  $
**
** Purpose: 
**   This test demonstrates a Table Sharing scenario.
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
**
** $Date: 2008/07/30 12:05:07GMT-05:00 $
** $Revision: 1.2 $
** $Log: utf_test16.c  $
** Revision 1.2 2008/07/30 12:05:07GMT-05:00 wfmoleski 
** Several Table Services modifications affected the UTF including a new API. The test16 example was 
** modified to execute this new API.
** Revision 1.1 2008/04/17 08:09:06EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test16/project.pj
** Revision 1.6 2008/04/07 09:39:46EDT wfmoleski 
** Added the capability for the specified hook functions. Added return code setting code for several
** OS functions that seemed appropriate.
** Revision 1.5 2007/06/05 08:14:57EDT wfmoleski 
** Updates to examples that test the CFE Table Services subsystem.
** Revision 1.4 2006/08/01 08:43:18EDT wfmoleski 
** Changes made to upgrade this example to cFE 3.3.0
** Revision 1.3 2006/06/22 13:56:41EDT wfmoleski 
** Changes to example to make it work with UTF and fsw changes for cFE 3.3
** Revision 1.1.1.2 2006/06/09 16:49:47EDT nsschweiss 
** Incorporated new    UTF_CFE_Init call.
** Revision 1.1.1.1 2006/06/08 15:25:12EDT nsschweiss 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test16/project.pj
 *
 * 
 *
  *
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

/* Global Variables used in the GetAddress Hook Function */
CFE_TBL_Handle_t TblHandle1, TblHandle2, TblHandle3, TblHandle4, TblHandle5;
uint32 TableHookData[10];

int32 GetLocalAppId(void)
{
	int32 returnval;
	uint32 AppId;
	char CurrentAppName[10];
	
	
	 returnval = CFE_ES_GetAppID(&AppId);
    
    if (returnval == CFE_SUCCESS)
    {
    	CFE_ES_GetAppName(CurrentAppName, AppId, sizeof(CurrentAppName));
    	/* UTF_put_text("Current App: is <%s>.\n", CurrentAppName); */
    	return (int32)AppId;   
    }
    else
    {
    	UTF_put_text("Error in utf_test16:GetLocalAppId.\n");
    	return -1;
    }
    
		
}

void SetLocalAppId(uint32 AppId)
{
	   char CurrentAppName[10];
	   int32 RetrievedAppId;
	
    UTF_CFE_ES_SetAppID(AppId);
    RetrievedAppId = GetLocalAppId();
    if (RetrievedAppId >= 0)
    {
    	CFE_ES_GetAppName(CurrentAppName, RetrievedAppId, sizeof(CurrentAppName));
    	UTF_put_text("\n-->Current App switched to Name/AppId <%s>/%d.\n", CurrentAppName, RetrievedAppId);    
    }
}

/* Function Hook Test routines */
int32 CFE_TBL_RegisterHook(CFE_TBL_Handle_t *TblHandlePtr,
                        const char *Name,
                        uint32  Size,
                        uint16  TblOptionFlags,
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TBL_ERR_INVALID_SIZE);
}

int32 CFE_TBL_LoadHook(CFE_TBL_Handle_t TblHandle,
					CFE_TBL_SrcEnum_t SrcType,
                    const void *SrcDataPtr)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TBL_ERR_DUMP_ONLY);
}

int32 CFE_TBL_ManageHook(CFE_TBL_Handle_t TblHandle)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TBL_INFO_VALIDATION_PENDING);
}

int32 CFE_TBL_GetAddressHook(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
/*    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TBL_ERR_UNREGISTERED); */

	if (TblHandle == TblHandle1)
	{
		UTF_put_text("Returning Table 1 handle.\n");
		*TblPtr = &TableHookData[0];
		return(CFE_SUCCESS);
	}
	else if (TblHandle == TblHandle2)
	{
		UTF_put_text("Returning Table 2 handle.\n");
		*TblPtr = &TableHookData[2];
		return(CFE_SUCCESS);
	}
	else if (TblHandle == TblHandle3)
	{
		UTF_put_text("Returning Table 3 handle.\n");
		*TblPtr = &TableHookData[4];
		return(CFE_SUCCESS);
	}
    else
        return(CFE_TBL_ERR_UNREGISTERED);
}

int32 CFE_TBL_GetAddressesHook(void **TblPtrs[], uint16 NumTables,
                               const CFE_TBL_Handle_t TblHandles[])
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TBL_ERR_UNREGISTERED);
}

int main(void)
{	
/*	CFE_TBL_Handle_t TblHandle1, TblHandle2, TblHandle3, TblHandle4, TblHandle5; */
	CFE_TBL_Handle_t CriticalTblHandle;
	CFE_TBL_Handle_t TblHandles[6];
	CFE_TBL_Info_t *TblInfoPtr;
	char getInfoTableName[60];
	char TableName1[50], TableName2[50], TableName3[50], TableName4[50], TableName5[50];
	char CriticalTableName[50];
	strcpy(TableName1, "MagSettings");
	strcpy(TableName2, "GyroSettings");
	strcpy(TableName3, "FilterTbl1");
	strcpy(TableName4, "FilterTbl2");
	strcpy(TableName5, "TableOfImportantNumbers");
	strcpy(CriticalTableName, "CriticalTable");
	int32 returnval;
	uint32 OwnerApp1Id = 5;
	uint32 OwnerApp2Id = 6;
    uint32 NonOwnerAppId = 15;
    char OwnerApp1Name[10], OwnerApp2Name[10];
    char NonOwnerAppName[10];
    char CurrentAppName[10];
    strcpy(OwnerApp1Name,"Owner1");
    strcpy(OwnerApp2Name,"Owner2");
    strcpy(NonOwnerAppName,"NN");
    int32 Load1 = 5735;
    int32 Load2 = 4868;
/*   uint32 AppId; */
    void *TblPtr;
    void **TblPtrs;
	
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test16.ActualOutput");
	#ifdef UTF_USE_STDOUT 
	UTF_put_text("Std output is being used. \n");
	#endif

    /**************************************************/
	/* Initialize Unit Test Framework                 */ 
    /**************************************************/
    /* Initialize the CDS */
    CFE_ES_CDS_EarlyInit();
    UTF_put_text("\n***Initialize UTF ***\n");
   	UTF_CFE_Init();
	
	/********************************/
	/* Initialize simulation time   */
	/********************************/
    UTF_init_sim_time(1.5);
    
    /**************************************************/
	/* Add tasks  to ES's list of tasks, and          */
	/* register local task with Executive Services.   */
	/**************************************************/
    UTF_ES_InitTaskRecords();
  	UTF_put_text("\n***Add tasks to ES list of tasks ***\n");
	UTF_ES_AddAppRecord(OwnerApp1Name,  OwnerApp1Id);	
	UTF_ES_AddAppRecord(OwnerApp2Name,  OwnerApp2Id);	
	UTF_ES_AddAppRecord(NonOwnerAppName,  NonOwnerAppId);	

    UTF_put_text("\n***Register local task with ES***\n");
/*    CFE_ES_RegisterApp(); */ /* Register local task with ES  */
        
	/**************************************************/
	/* Test Table Services APIs                       */
	/**************************************************/

    /* CFE_TBL_Register Test */
    UTF_put_text("\n***Test CFE_TBL_Register API***\n");
    
    UTF_put_text("\n* Test Case 1: Call TBL_Register with valid arguments.*\n");
    UTF_put_text("* Expect: UTF info msg that table has been registered.*\n");
    SetLocalAppId(OwnerApp1Id);
    returnval = CFE_TBL_Register( &TblHandle1,TableName1,    10,        0, 0  );  
    UTF_put_text("* Return value is %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();
    returnval = CFE_TBL_Register( &TblHandle2,TableName2,    10,        0, 0  );  
    UTF_put_text("* Return value is %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();

    SetLocalAppId(OwnerApp2Id);
    returnval = CFE_TBL_Register( &TblHandle3,TableName3,    10,        0, 0  );  
    UTF_put_text("* Return value is %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();
    returnval = CFE_TBL_Register( &TblHandle4,TableName4,    10,        0, 0  );  
    UTF_put_text("* Return value is %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();
    	
    
    UTF_put_text("\n* Test Case 2: Call TBL_Register with name argument which is too long.*\n");
    UTF_put_text("* Expect: SysLog error message.*\n");
    returnval = CFE_TBL_Register( &TblHandle5,TableName5,    10,        0, 0  ); 
    UTF_put_text("* Return value is %x.  Expected %x.*\n", returnval, CFE_TBL_ERR_INVALID_NAME);
    UTF_CFE_TBL_DisplayTableRegistryContents();
     
    UTF_put_text("\n* Test Case 3: Call TBL_Register for a Critical Table.*\n");
    returnval = CFE_TBL_Register(&CriticalTblHandle,CriticalTableName,10,CFE_TBL_OPT_CRITICAL,0);  
    UTF_put_text("* Return value is %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();
 	CFE_TBL_LoadWorkingBuffer(CriticalTblHandle,CFE_TBL_SRC_ADDRESS,&Load1);
    returnval = CFE_TBL_Manage(CriticalTblHandle);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected 0.*\n", returnval);

    /* CFE_TBL_Unregister Test */
    UTF_put_text("\n***Test CFE_TBL_Unregister API***\n");
    UTF_put_text("\n* Test Case 1: Call CFE_TBL_Unregister from non-owner app.*\n");
    
    returnval = CFE_TBL_Unregister (  TblHandle1 );
    UTF_put_text("* Return value is %x.  Expected %x.*\n", returnval, CFE_SUCCESS);
    UTF_CFE_TBL_DisplayTableRegistryContents();

    UTF_put_text("\n* Test Case 2: Call CFE_TBL_Unregister from owner app.*\n");

    SetLocalAppId(OwnerApp1Id);
    returnval = CFE_TBL_Unregister (  TblHandle1 );
    UTF_put_text("* Return value is %x.  Expected %x.*\n", returnval, CFE_SUCCESS);
    UTF_CFE_TBL_DisplayTableRegistryContents();

	UTF_put_text("\n****************************\n");
    UTF_put_text  ("***Shared table scenarios***\n");
	UTF_put_text  ("****************************\n");
    UTF_put_text("\n* Shared table scenario 1: App Owner1 registers table.  App NN shares it.*\n");
    UTF_put_text("* Expect: UTF info msg that table has been registered.*\n");
    /* Set AppId so Table Services will associate AppId with Register operation.*/
    SetLocalAppId(OwnerApp1Id);
    returnval = CFE_TBL_Register( &TblHandle1,TableName1,    1024,        0, 0  );  
    UTF_put_text("* CFE_TBL_Register returned %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();
 	CFE_TBL_LoadWorkingBuffer( TblHandle1,
                    CFE_TBL_SRC_ADDRESS,
                    &Load1 );
    returnval = CFE_TBL_Manage(TblHandle1);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected %x.*\n", returnval, CFE_TBL_INFO_UPDATED);
    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandle1);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.*\n", returnval);
    UTF_put_text("* Table now contains value %d.\n", *(int32 *)TblPtr);
    returnval = CFE_TBL_ReleaseAddress(TblHandle1);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
    
    /* Set AppId so Table Services will associate AppId with Share operation.*/
    SetLocalAppId(NonOwnerAppId);
    CFE_ES_GetAppName(CurrentAppName, (uint32)GetLocalAppId(), sizeof(CurrentAppName));
    UTF_put_text("* Initiate Shared access of table Owner1.MagSettings for App <%s>\n.", CurrentAppName);
    returnval = CFE_TBL_Share(  &TblHandles[5], "Owner1.MagSettings" );
    UTF_put_text("* CFE_TBL_Share returned %x.  Expected 0.*\n", returnval);
    UTF_CFE_TBL_DisplayTableRegistryContents();

    returnval = CFE_TBL_Manage(TblHandles[5]);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected %x.*\n", returnval, CFE_TBL_INFO_UPDATED);

    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandles[5]);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.*\n", returnval);
    UTF_put_text(" %x value indicates CFE_TBL_INFO_UPDATED\n",CFE_TBL_INFO_UPDATED);
    UTF_put_text("* Table now contains value %d.\n", *(int32 *)TblPtr);
    returnval = CFE_TBL_ReleaseAddress(TblHandles[5]);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
    
    
    UTF_put_text("\nNow NN accesses table and Owner1 accesses table before NN releases it.\n");
    returnval = CFE_TBL_Manage(TblHandles[5]);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected CFE_SUCCESS(%x).*\n", 
    					returnval, CFE_SUCCESS);

    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandles[5]);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.Expected CFE_SUCCESS(%x).*\n", 
    				returnval, CFE_SUCCESS);
    UTF_put_text("* Table now contains value %d.\n", *(int32 *)TblPtr);
    
    SetLocalAppId(OwnerApp1Id);
    returnval = CFE_TBL_Manage(TblHandle1);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected CFE_SUCCESS(%x).*\n", 
    					returnval, CFE_SUCCESS);
    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandle1);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.  Expected CFE_SUCCESS(%x)*\n", 
    					returnval, CFE_SUCCESS);
    UTF_put_text("* Table now contains value %d.\n", *(int32 *)TblPtr);

    UTF_put_text("* Both applications will now release table.. *\n");
    SetLocalAppId(NonOwnerAppId);
    returnval = CFE_TBL_ReleaseAddress(TblHandles[5]);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
 
    SetLocalAppId(OwnerApp1Id);
    returnval = CFE_TBL_ReleaseAddress(TblHandle1);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
    
    UTF_put_text("* Load even number to table. *\n");

    SetLocalAppId(OwnerApp1Id);
    CFE_TBL_LoadWorkingBuffer( TblHandle1,
                    CFE_TBL_SRC_ADDRESS,
                    &Load2 );

    returnval = CFE_TBL_Manage(TblHandle1);
    UTF_put_text("* CFE_TBL_Manage returned %x.  Expected CFE_SUCCESS(%x).*\n", 
    					returnval, CFE_SUCCESS);
    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandle1);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.  Expected CFE_SUCCESS(%x)*\n", 
    					returnval, CFE_SUCCESS);
    UTF_put_text("* Table now contains value %d.\n", *(int32 *)TblPtr);
    returnval = CFE_TBL_ReleaseAddress(TblHandle1);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
    
    UTF_put_text("\n* Owner of table unregisters. *\n");
    returnval = CFE_TBL_Unregister( TblHandle1);  
    UTF_put_text("* CFE_TBL_Unregister returned %x.*\n", returnval);    
    UTF_CFE_TBL_DisplayTableRegistryContents();
    
    UTF_put_text("\n* App with shared access attempts to access table. *\n");
    SetLocalAppId(NonOwnerAppId);
    returnval = CFE_TBL_Manage(TblHandles[5]);
    UTF_put_text("* CFE_TBL_Manage returned %x.*\n", returnval);

    returnval = CFE_TBL_GetAddress( &TblPtr, TblHandles[5]);
    UTF_put_text("* CFE_TBL_GetAddress returned %x.  Expect %x*\n", returnval, CFE_TBL_ERR_UNREGISTERED);
    returnval = CFE_TBL_ReleaseAddress(TblHandles[5]);
    UTF_put_text("* CFE_TBL_ReleaseAddress returned %x.  Expected 0.*\n", returnval);
    UTF_put_text("\n* App with shared access unregisters. *\n");
    returnval = CFE_TBL_Unregister( TblHandles[5]);  
    UTF_CFE_TBL_DisplayTableRegistryContents();

    UTF_put_text("\n***Test CFE_TBL_GetInfo API***\n");
    strcpy(getInfoTableName,OwnerApp2Name);
    strcat(getInfoTableName,".");
    strcat(getInfoTableName,TableName3);
    UTF_put_text("* Looking for table named '%s'.\n", getInfoTableName);
    TblInfoPtr = malloc(sizeof(CFE_TBL_Info_t));
	returnval = CFE_TBL_GetInfo(TblInfoPtr,getInfoTableName);
    UTF_put_text("* CFE_TBL_GetInfo returned %x.\n", returnval);
    if (returnval == CFE_TBL_ERR_INVALID_NAME)
	    UTF_put_text("* Table name was invalid.\n");
	else
	{
		/* Print the contents of TblInfoPtr */
		UTF_put_text("* Table info returned:\n");
		UTF_put_text("*     Size:              %d\n", TblInfoPtr->Size);
		UTF_put_text("*     NumUsers:          %d\n", TblInfoPtr->NumUsers);
		UTF_put_text("*     FileCreateSecs:    %d\n", TblInfoPtr->FileCreateTimeSecs);
		UTF_put_text("*     FileCreateSubSecs: %d\n", TblInfoPtr->FileCreateTimeSubSecs);
		UTF_put_text("*     LastUpdateSecs:    %d\n", TblInfoPtr->TimeOfLastUpdate.Seconds);
		UTF_put_text("*     LastUpdateSubSecs: %d\n", TblInfoPtr->TimeOfLastUpdate.Subseconds);
		UTF_put_text("*     LoadedOnce flag:   %d\n", TblInfoPtr->TableLoadedOnce);
		UTF_put_text("*     DumpOnly flag:     %d\n", TblInfoPtr->DumpOnly);
		UTF_put_text("*     Double Buffered:   %d\n", TblInfoPtr->DblBuffered);
		UTF_put_text("*     User Defined Addr: %d\n", TblInfoPtr->UserDefAddr);
		UTF_put_text("*     Critical flag:     %d\n", TblInfoPtr->Critical);
		UTF_put_text("*     Last File Loaded  '%s'\n", TblInfoPtr->LastFileLoaded);
	}

    UTF_put_text("\n***Test CFE_TBL_GetInfo API with a Critical Table ***\n");
    strcpy(getInfoTableName,OwnerApp2Name);
    strcat(getInfoTableName,".");
    strcat(getInfoTableName,CriticalTableName);
    UTF_put_text("* Looking for table named '%s'.\n", getInfoTableName);
    TblInfoPtr = malloc(sizeof(CFE_TBL_Info_t));
	returnval = CFE_TBL_GetInfo(TblInfoPtr,getInfoTableName);
    UTF_put_text("* CFE_TBL_GetInfo returned %x.\n", returnval);
    if (returnval == CFE_TBL_ERR_INVALID_NAME)
	    UTF_put_text("* Table name was invalid.\n");
	else
	{
		/* Print the contents of TblInfoPtr */
		UTF_put_text("* Table info returned:\n");
		UTF_put_text("*     Size:              %d\n", TblInfoPtr->Size);
		UTF_put_text("*     NumUsers:          %d\n", TblInfoPtr->NumUsers);
		UTF_put_text("*     FileCreateSecs:    %d\n", TblInfoPtr->FileCreateTimeSecs);
		UTF_put_text("*     FileCreateSubSecs: %d\n", TblInfoPtr->FileCreateTimeSubSecs);
		UTF_put_text("*     LastUpdateSecs:    %d\n", TblInfoPtr->TimeOfLastUpdate.Seconds);
		UTF_put_text("*     LastUpdateSubSecs: %d\n", TblInfoPtr->TimeOfLastUpdate.Subseconds);
		UTF_put_text("*     LoadedOnce flag:   %d\n", TblInfoPtr->TableLoadedOnce);
		UTF_put_text("*     DumpOnly flag:     %d\n", TblInfoPtr->DumpOnly);
		UTF_put_text("*     Double Buffered:   %d\n", TblInfoPtr->DblBuffered);
		UTF_put_text("*     User Defined Addr: %d\n", TblInfoPtr->UserDefAddr);
		UTF_put_text("*     Critical flag:     %d\n", TblInfoPtr->Critical);
		UTF_put_text("*     Last File Loaded  '%s'\n", TblInfoPtr->LastFileLoaded);
	}
	
    SetLocalAppId(OwnerApp2Id);
    UTF_put_text("\n***Test CFE_TBL_Modified API***\n");
    UTF_put_text("CFE_TBL_Modified called for '%s' returned %x\n", TableName4, CFE_TBL_Modified(TblHandle4));  
    UTF_CFE_TBL_DisplayTableRegistryContents();
    UTF_put_text("CFE_TBL_Modified called for '%s' returned %x\n", CriticalTableName, CFE_TBL_Modified(CriticalTblHandle));  
    UTF_CFE_TBL_DisplayTableRegistryContents();

    UTF_put_text("\n***Function Hook Tests***\n");
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, (void *)&CFE_TBL_RegisterHook);
    UTF_put_text("Register Hook Return Code %x\n", CFE_TBL_Register(&TblHandle1,TableName1,1024,0,0));  
    UTF_put_text("Register Hook Return Code %x\n", CFE_TBL_Register(&TblHandle1,TableName1,1024,0,0));  
    UTF_put_text("Register Hook Return Code %x\n", CFE_TBL_Register(&TblHandle1,TableName1,1024,0,0));  
    UTF_put_text("Register Hook Return Code %x\n", CFE_TBL_Register(&TblHandle1,TableName1,1024,0,0));  

    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, (void *)&CFE_TBL_LoadHook);
    UTF_put_text("Load Hook Return Code %x\n", CFE_TBL_Load(TblHandle1,CFE_TBL_SRC_ADDRESS,&Load2));  
    UTF_put_text("Load Hook Return Code %x\n", CFE_TBL_Load(TblHandle1,CFE_TBL_SRC_ADDRESS,&Load2));  
    UTF_put_text("Load Hook Return Code %x\n", CFE_TBL_Load(TblHandle1,CFE_TBL_SRC_ADDRESS,&Load2));  
    UTF_put_text("Load Hook Return Code %x\n", CFE_TBL_Load(TblHandle1,CFE_TBL_SRC_ADDRESS,&Load2));  

    UTF_TBL_set_function_hook(CFE_TBL_MANAGE_HOOK, (void *)&CFE_TBL_ManageHook);
    UTF_put_text("Manage Hook Return Code %x\n", CFE_TBL_Manage(TblHandle1));  
    UTF_put_text("Manage Hook Return Code %x\n", CFE_TBL_Manage(TblHandle1));  
    UTF_put_text("Manage Hook Return Code %x\n", CFE_TBL_Manage(TblHandle1));  
    UTF_put_text("Manage Hook Return Code %x\n", CFE_TBL_Manage(TblHandle1));  
	
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *)&CFE_TBL_GetAddressHook);
    UTF_put_text("GetAddress Hook Return Code %x\n", CFE_TBL_GetAddress( &TblPtr, TblHandle1));  
    UTF_put_text("Table Pointer returned = %d. Expected %d.\n", (int32 *)TblPtr,&TableHookData[0]);
    UTF_put_text("GetAddress Hook Return Code %x\n", CFE_TBL_GetAddress( &TblPtr, TblHandle2));  
    UTF_put_text("Table Pointer returned = %d. Expected %d.\n", (int32 *)TblPtr,&TableHookData[2]);
    UTF_put_text("GetAddress Hook Return Code %x\n", CFE_TBL_GetAddress( &TblPtr, TblHandle3));  
    UTF_put_text("Table Pointer returned = %d. Expected %d.\n", (int32 *)TblPtr,&TableHookData[4]);
    UTF_put_text("GetAddress Hook Return Code %x\n", CFE_TBL_GetAddress( &TblPtr, TblHandles[5]));

    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESSES_HOOK, (void *)&CFE_TBL_GetAddressesHook);
    UTF_put_text("GetAddresses Hook Return Code %x\n", CFE_TBL_GetAddresses(&TblPtrs,3, TblHandles));  
    UTF_put_text("GetAddresses Hook Return Code %x\n", CFE_TBL_GetAddresses(&TblPtrs,3,TblHandles));  
    UTF_put_text("GetAddresses Hook Return Code %x\n", CFE_TBL_GetAddresses(&TblPtrs,3,TblHandles));  
    UTF_put_text("GetAddresses Hook Return Code %x\n", CFE_TBL_GetAddresses(&TblPtrs,3,TblHandles));

 	exit(0);
}
