/*
** File: utf_test14.c
**
** Purpose: This test makes ES api calls.
**
** References:
**
** Assumptions and Notes:
*** Output can be directed either to screen or to file.
 * To direct output to screen,
 *    comment in '#define UTF_USE_STDOUT' statement in the
 *    utf_custom.h file.
 *
 * To direct output to file,
 *    comment out '#define UTF_USE_STDOUT' statement in
 *    utf_custom.h file.
* 
** $Date: 2012/02/28 10:33:00GMT-05:00 $
** $Revision: 1.6 $
** $Log: utf_test14.c  $
** Revision 1.6 2012/02/28 10:33:00GMT-05:00 wmoleski 
** Added function hooks and Return Code handling and updated the examples to test these changes.
** Revision 1.5 2009/10/13 15:23:32EDT wmoleski 
** 
** Revision 1.4 2008/08/19 14:14:40EDT wfmoleski 
** When an invalid AppID was used and retrieved in the RunLoop, a statement tried to set an array
** variable in the AppID's slot. Since the AppID was beyond the array bounds, the process crashed
** with a stack dump.
** Revision 1.3 2008/08/19 14:08:48EDT wfmoleski 
** Execution Counters added to the UTF and test example
** Revision 1.2 2008/07/18 09:22:56EDT wfmoleski 
** The CFE_ES_GetAppInfo function along with an internal function to set this info and the test that calls 
** these functions were implemted in the UTF.
** Revision 1.1 2008/04/17 08:09:02EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test14/project.pj
** Revision 1.9 2008/02/26 13:30:10EST wfmoleski 
** Added the OS_TaskDelete function hook capability to utf_osapi and the example test 14. I also changed
** the OS_FPUExFPUMask function in the UTF to OS_FPUExSetMask.
** Revision 1.8 2007/06/07 11:52:22EDT wfmoleski 
** Changes due to a fsw problem with Critical Data Store deletion.
** Revision 1.7 2007/06/05 08:24:49EDT wfmoleski 
** Changes to examples and utf source that reflect the cFE 4.1.0 ES subsystem.
** Revision 1.6 2006/11/14 15:46:34EST wfmoleski 
** Modifications for Restart implementation
** Revision 1.5 2006/11/09 08:00:43EST wfmoleski 
** Example updates for the new CDS implementation
** Revision 1.4 2006/08/01 08:41:07EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.3 2006/06/22 13:55:19EDT wfmoleski 
** Mods to the examples to make them work with the new UTF code. Makefile changes included adding
** required object files.
** Revision 1.2 2006/06/06 16:01:41EDT nsschweiss 
** 
**
*/

#include "utf_cfe.h"
#include "cfe_es_cds.h"

/* Define a structure that holds the CDS for this app */
typedef struct
{
	uint32	TestData1;
	char	TestData2[24];
	uint32	TestData3;
} test14_CDSType_t;

typedef struct
{
	CFE_ES_CDSHandle_t	myCDSHandle;
	test14_CDSType_t	myCDSData;
} test14_TaskData_t;

uint32	resetSubType;

/* Task Delete Handler */
int32 delete_hook(uint32 taskID);

/* Function Hook for CreateChildTask */
int32 CFE_ES_CreateChildTaskHook(uint32 *TaskIdPtr, const char *TaskName, CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr, const uint32 *StackPtr, uint32 StackSize, uint32 Priority, uint32 Flags)
{
  UTF_put_text("CreateChildTask Function Hook called for '%s'\n",TaskName);
  return CFE_SUCCESS;
}

/* Function Hook for OS_BinSemCreate */
int32 OS_BinSemCreateHook(uint32 *sem_id, const char *sem_name, uint32 initial_val, uint32 options)
{
  UTF_put_text("OS_BinSemCreate Function Hook called for '%s'\n",sem_name);
  return OS_SUCCESS;
}

int main(void)
{
	uint32 AppId;
	char AppName[128];
	int32 ReturnVal;
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test14.ActualOutput");
    UTF_init_sim_time(1.5);

    /* ES Reset CFE Test */
    UTF_put_text("\n***Test CFE_ES_ResetCFE API***\n");
    UTF_put_text("\nTest Case #1: Call CFE_ES_ResetCFE(CFE_ES_PROCESSOR_RESET)\n"); 
    UTF_put_text("CFE_ES_ResetCFE returned %x. Expected %x\n", CFE_ES_ResetCFE(CFE_ES_PROCESSOR_RESET),CFE_SUCCESS);
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_ResetCFE to 0x3abc3abc\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RESETCFE_PROC, 0x3abc3abc);
    UTF_put_text("CFE_ES_ResetCFE returned %x. Expected 0x3abc3abc\n", CFE_ES_ResetCFE(0));
    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_ResetCFE to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_RESETCFE_PROC);
    UTF_put_text("CFE_ES_ResetCFE returned %x. Expected %x\n", CFE_ES_ResetCFE(CFE_ES_POWERON_RESET), CFE_SUCCESS);

    /* ES Get Reset Type Test */
    UTF_put_text("\n***Test CFE_ES_GetResetType API***\n");
    UTF_put_text("\nTest Case #1: Call CFE_ES_GetResetType(NULL)\n"); 
    UTF_put_text("CFE_ES_GetResetType returned %x. Expected %x\n", CFE_ES_GetResetType(NULL),CFE_ES_POWERON_RESET);
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_GetResetType to 0x3abc3abc\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_GETRESETTYPE_PROC, 0x3abc3abc);
    UTF_put_text("CFE_ES_GetResetType(NULL) returned %x. Expected 0x3abc3abc\n", CFE_ES_GetResetType(NULL));
    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_GetResetType to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_GETRESETTYPE_PROC);
    CFE_ES_ResetCFE(CFE_ES_PROCESSOR_RESET);  
    UTF_put_text("CFE_ES_GetResetType(NULL) returned %x. Expected %x\n", CFE_ES_GetResetType(NULL), CFE_ES_PROCESSOR_RESET);

    UTF_put_text("\nTest Case #4: Test the SubType argument return of CFE_ES_GetResetType\n"); 
    UTF_CFE_ES_SetResetSubtype(CFE_ES_POWER_CYCLE);
    ReturnVal = CFE_ES_GetResetType(&resetSubType);
    UTF_put_text("CFE_ES_GetResetType() returned type = %x & subType = %x. Expected %x & %x\n", ReturnVal, resetSubType, CFE_ES_PROCESSOR_RESET, CFE_ES_POWER_CYCLE);

    UTF_put_text("\nTest Case #5: Set the SubType to an invalid value\n"); 
    UTF_CFE_ES_SetResetSubtype(45);
    ReturnVal = CFE_ES_GetResetType(&resetSubType);
    UTF_put_text("CFE_ES_GetResetType() returned type = %x & subType = %x. Expected %x & 0\n",
    	ReturnVal, resetSubType, CFE_ES_PROCESSOR_RESET);

    UTF_put_text("\nTest Case #6: Test the SubType argument return of CFE_ES_GetResetType\n"); 
    UTF_CFE_ES_SetResetSubtype(CFE_ES_EXCEPTION);
    ReturnVal = CFE_ES_GetResetType(&resetSubType);
    UTF_put_text("CFE_ES_GetResetType() returned type = %x & subType = %x. Expected %x & %x\n",
    	ReturnVal, resetSubType, CFE_ES_PROCESSOR_RESET, CFE_ES_EXCEPTION);
	
    /* ES Register App Test */
    UTF_put_text("\n***Test CFE_ES_RegisterApp API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_RegisterApp()\n"); 
    UTF_put_text("CFE_ES_RegisterApp returned %x. Expected %x.\n", CFE_ES_RegisterApp(),CFE_SUCCESS);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_RegisterApp to 0x652a652a\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERAPP_PROC, 0x652a652a);
    UTF_put_text("CFE_ES_RegisterApp returned %x. Expected 0x652a652a.\n", CFE_ES_RegisterApp());

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_Register to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERAPP_PROC);
    UTF_put_text("CFE_ES_RegisterApp returned %x. Expected %x\n", CFE_ES_RegisterApp(), CFE_SUCCESS);

    /* ES Get AppID By Name Test */
    UTF_put_text("\n***Test CFE_ES_GetAppIDByName API***\n");

    UTF_put_text("\nSet up step #1: Call UTF_ES_InitAppRecords()\n"); 
    UTF_ES_InitAppRecords();
        
    UTF_put_text("\nSet up step #2: Call UTF_ES_AddAppRecord\n"); 
    UTF_ES_AddAppRecord("",11); 
    UTF_ES_AddAppRecord("Application#1",11);
    UTF_ES_AddAppRecord("Application#2",12);
    UTF_ES_AddAppRecord("Application#3",13);

    UTF_put_text("\nTest Case #1: Call CFE_ES_GetAppIDByName()\n"); 
    CFE_ES_GetAppIDByName(&AppId, "Application#1");
    UTF_put_text("CFE_ES_GetAppIDByName returned apid=%d. Expected %d.\n", AppId,11); 
    CFE_ES_GetAppIDByName(&AppId, "Application#2");
    UTF_put_text("CFE_ES_GetAppIDByName returned apid=%d. Expected %d.\n", AppId,12);
    CFE_ES_GetAppIDByName(&AppId, "Application#3");
    UTF_put_text("CFE_ES_GetAppIDByName returned apid=%d. Expected %d.\n", AppId,13);

    /* ES Get AppID Test */
    UTF_put_text("\n***Test CFE_ES_GetAppID API***\n");
    UTF_put_text("\nSet up step #1: Call UTF_CFE_ES_SetAppID(11)\n");
	UTF_CFE_ES_SetAppID(11);
	
    UTF_put_text("\nTest Case #1: Call CFE_ES_GetAppID(0)\n"); 
    UTF_put_text("CFE_ES_GetAppID returned apid=%x. Expected %x.\n", CFE_ES_GetAppID(0),CFE_ES_ERR_BUFFER);
    
    UTF_put_text("\nTest Case #2: Call CFE_ES_GetAppID.\n"); 
    CFE_ES_GetAppID(&AppId);
    UTF_put_text("CFE_ES_GetAppID returned apid=%d. Expected 11.\n", AppId); 
    
    /* CFE_ES_GetAppName Test */
    UTF_put_text("\n***Test CFE_ES_GetAppName API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_GetAppName() for assigned apids.\n"); 
    ReturnVal = CFE_ES_GetAppName(AppName, 11, 128);
    UTF_put_text("CFE_ES_GetAppName(11) returned %x. AppName arg returned <%s>. Expected 0 and <Application#1>.\n", ReturnVal,AppName);
    ReturnVal = CFE_ES_GetAppName(AppName, 12, 128);
    UTF_put_text("CFE_ES_GetAppName(12) returned %x. AppName arg returned <%s>. Expected 0 and <Application#2>.\n", ReturnVal,AppName);
    ReturnVal = CFE_ES_GetAppName(AppName, 13, 128);
    UTF_put_text("CFE_ES_GetAppName(13) returned %x. AppName arg returned <%s>. Expected 0 and <Application#3>.\n", ReturnVal,AppName);

    UTF_put_text("\nTest Case #2: Call CFE_ES_GetAppName(5) for unassigned apid.\n"); 
    ReturnVal = CFE_ES_GetAppName(AppName, 5, 128);
    UTF_put_text("CFE_ES_GetAppName(5) returned %x. AppName arg returned <%s>.\n", ReturnVal,AppName);
 
    UTF_put_text("\n***Dump the Registered App Table***\n");
	UTF_ES_DumpAppRecords();
	
    /* CFE_ES_RestartApp Test */
    UTF_put_text("\n***Test CFE_ES_RestartApp API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_RestartApp(11).\n"); 
    ReturnVal = CFE_ES_RestartApp(11);
    UTF_put_text("CFE_ES_RestartApp(11) returned %x. Expected 0.\n", ReturnVal);
    
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_RestartApp to 0x586e586e\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RESTARTAPP_PROC, 0x586e586e);
    ReturnVal = CFE_ES_RestartApp(11);
    UTF_put_text("CFE_ES_RestartApp(11) returned %x. Expected 0x586e586e.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_RestartApp to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_RESTARTAPP_PROC);
    ReturnVal = CFE_ES_RestartApp(11);
    UTF_put_text("CFE_ES_RestartApp(11) returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #4: Call CFE_ES_RestartApp(7).\n"); 
    ReturnVal = CFE_ES_RestartApp(7);
    UTF_put_text("CFE_ES_RestartApp(7) returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_APPID);

    /* CFE_ES_ReloadApp Test */
    UTF_put_text("\n***Test CFE_ES_ReloadApp API***\n");
    
    UTF_put_text("\nTest Case #1: Call CFE_ES_ReloadApp(11,'app11.o').\n"); 
    ReturnVal = CFE_ES_ReloadApp(11,"app11.o");
    UTF_put_text("CFE_ES_ReloadApp(11,'app11.o') returned %x. Expected 0.\n", ReturnVal);
    
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_ReloadApp to 0x586e586e\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RELOADAPP_PROC, 0x586e586e);
    ReturnVal = CFE_ES_ReloadApp(11,"app11.o");
    UTF_put_text("CFE_ES_ReloadApp(11,'app11.o') returned %x. Expected 0x586e586e.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_ReloadApp to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_RELOADAPP_PROC);
    ReturnVal = CFE_ES_ReloadApp(11,"app11.o");
    UTF_put_text("CFE_ES_ReloadApp(11,'app11.o') returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #4: Call CFE_ES_ReloadApp(22,'thisAppDoesntExist.o').\n"); 
    ReturnVal = CFE_ES_ReloadApp(22,"thisAppDoesntExist.o");
    UTF_put_text("CFE_ES_ReloadApp(22,'thisAppDoesntExist.o') returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_APPID);

    /* CFE_ES_DeleteApp Test */
    UTF_put_text("\n***Test CFE_ES_DeleteApp API***\n");
    
    UTF_put_text("\nTest Case #1: Call CFE_ES_DeleteApp(11).\n"); 
    ReturnVal = CFE_ES_DeleteApp(11);
    UTF_put_text("CFE_ES_DeleteApp(11) returned %x. Expected 0.\n", ReturnVal);
    
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_DeleteApp to 0x586e586e\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_DELETEAPP_PROC, 0x586e586e);
    ReturnVal = CFE_ES_DeleteApp(11);
    UTF_put_text("CFE_ES_DeleteApp(11) returned %x. Expected 0x586e586e.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_DeleteApp to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_DELETEAPP_PROC);
    UTF_ES_AddAppRecord("Application#1",11);
    ReturnVal = CFE_ES_DeleteApp(11);
    UTF_put_text("CFE_ES_DeleteApp(11) returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\n***Dump the Registered App Table***\n");
    UTF_put_text("\n***AppID 11 should not appear***\n");
	UTF_ES_DumpAppRecords();

    UTF_ES_AddAppRecord("Application#1",11);

    UTF_put_text("\nTest Case #4: Call CFE_ES_DeleteApp(22).\n"); 
    ReturnVal = CFE_ES_DeleteApp(22);
    UTF_put_text("CFE_ES_DeleteApp(22) returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_APPID);

    /* CFE_ES_CreateChildTask Test */
    UTF_put_text("\n***Test CFE_ES_CreateChildTask API***\n");
    uint32 childTask1Id, childTask2Id, childHookId=0;

    UTF_put_text("\nTest Case #1: Call CFE_ES_CreateChildTask()\n"); 
    ReturnVal = CFE_ES_CreateChildTask(&childTask1Id,  "ImportantTask1",  0, 0, 1024, 3, 0);
    UTF_put_text("CFE_ES_CreateChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    UTF_put_text("ChildTask %d created.\n",childTask1Id);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_CreateChildTask to 0x586e586e\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_CREATECHILDTASK_PROC, 0x586e586e);
    ReturnVal = CFE_ES_CreateChildTask(&childTask1Id,  "ImportantTask",  0, 0, 1024, 3, 0);
    UTF_put_text("CFE_ES_CreateChildTask returned %x. Expected 0x586e586e.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_CreateChildTask to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_CREATECHILDTASK_PROC);
    ReturnVal = CFE_ES_CreateChildTask(&childTask2Id,  "ImportantTask2",  0, 0, 1024, 3, 0);
    UTF_put_text("CFE_ES_CreateChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    UTF_put_text("ChildTask %d created.\n",childTask2Id);

    UTF_put_text("\nTest Case #4: CFE_ES_CreateChildTask Function Hook test\n");
    UTF_ES_API_set_function_hook(CFE_ES_CREATECHILDTASK_HOOK, (void *)&CFE_ES_CreateChildTaskHook);
    UTF_put_text("CreateChildTask Hook Return Code %x\n", CFE_ES_CreateChildTask(&childHookId,"HookTask",0,0,1024,3,0));
    /* Set the function hook pointer back to NULL */
    UTF_ES_API_set_function_hook(CFE_ES_CREATECHILDTASK_HOOK,NULL);
    
    /* CFE_ES_RegisterChildTask Test */
    UTF_put_text("\n***Test CFE_ES_RegisterChildTask API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_RegisterChildTask()\n"); 
    ReturnVal = CFE_ES_RegisterChildTask();
    UTF_put_text("CFE_ES_RegisterChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_RegisterChildTask to 0x847a847a\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCHILDTASK_PROC, 0x847a847a);
    ReturnVal = CFE_ES_RegisterChildTask();
    UTF_put_text("CFE_ES_RegisterChildTask returned %x. Expected 0x847a847a.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_RegisterChildTask to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERCHILDTASK_PROC);
    ReturnVal = CFE_ES_RegisterChildTask();
    UTF_put_text("CFE_ES_RegisterChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    
    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_ES_DumpTaskRecords();
    
    /* CFE_ES_DeleteChildTask Test */
    UTF_put_text("\n***Test CFE_ES_DeleteChildTask API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_DeleteChildTask()\n"); 
    ReturnVal = CFE_ES_DeleteChildTask(childTask1Id);
    UTF_put_text("CFE_ES_DeleteChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_DeleteChildTask to 0x146c146c\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_DELETECHILDTASK_PROC, 0x146c146c);
    ReturnVal = CFE_ES_DeleteChildTask(childTask1Id);
    UTF_put_text("CFE_ES_DeleteChildTask returned %x. Expected 0x146c146c.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_DeleteChildTask to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_DELETECHILDTASK_PROC);
    ReturnVal = CFE_ES_DeleteChildTask(45);
    UTF_put_text("CFE_ES_DeleteChildTask returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_TASKID);

    UTF_put_text("\nTest Case #4: Attempt to delete a child task that is a main task.\n");
	uint32 mainTaskId=0;
	mainTaskId = UTF_ES_GetTaskIDByAppID(12);
    ReturnVal = CFE_ES_DeleteChildTask(mainTaskId);
    UTF_put_text("CFE_ES_DeleteChildTask(%d) returned %x. Expected %x.\n",mainTaskId,ReturnVal,CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK);
    
    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_put_text("\n***Child TaskId %d should not appear***\n",childTask1Id);
    UTF_ES_DumpTaskRecords();
    
    /* CFE_ES_ExitChildTask Test */
    UTF_put_text("\n***Test CFE_ES_ExitChildTask API***\n");
	UTF_ES_SetTaskID(childTask2Id);
	CFE_ES_ExitChildTask();

    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_put_text("\n***Child TaskId %d should not appear***\n",childTask2Id);
    UTF_ES_DumpTaskRecords();

    /* OS_TaskDelete function hook Test */
    UTF_put_text("\n***Testing OS_TaskDelete function hook***\n");
    UTF_put_text("\nTest Case #1: Attempt to install a NULL pointer as the handler.\n");
    ReturnVal = OS_TaskInstallDeleteHandler(NULL);
    UTF_put_text("OS_TaskInstallDeleteHandler(NULL) returned %x. Expected %x.\n",ReturnVal,OS_INVALID_POINTER);

    UTF_put_text("\nTest Case #2: Install a valid Handler.\n");
    ReturnVal = OS_TaskInstallDeleteHandler(delete_hook);
    UTF_put_text("OS_TaskInstallDeleteHandler() returned %x. Expected %x.\n",ReturnVal,OS_SUCCESS);

    UTF_put_text("\nTest Case #3: Verify the TaskDelete function hook is called.\n");
    ReturnVal = CFE_ES_CreateChildTask(&childTask1Id,"DeleteHookTask1",0,0,1024,3,0);
    UTF_put_text("CFE_ES_CreateChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    UTF_put_text("ChildTask %d created.\n",childTask1Id);
    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_put_text("\n***Child TaskId %d should not appear***\n",childTask1Id);
    UTF_ES_DumpTaskRecords();
	ReturnVal = CFE_ES_DeleteChildTask(childTask1Id);
    UTF_put_text("CFE_ES_DeleteChildTask returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_put_text("\n***Child TaskId %d should not appear***\n",childTask1Id);
    UTF_ES_DumpTaskRecords();
	
    /* OS_BinSemCreate function hook & return code tests */
    uint32 semId=0;
    UTF_put_text("\nTest Case #1: OS_BinSemCreate Function Hook test\n");
    UTF_OSAPI_set_function_hook(OS_BINSEMCREATE_HOOK, (void *)&OS_BinSemCreateHook);
    UTF_put_text("OS_BinSemCreate Hook Return Code %x\n", OS_BinSemCreate(&semId,"BinarySemName",0,0));
    /* Set the function hook pointer back to NULL */
    UTF_OSAPI_set_function_hook(OS_BINSEMCREATE_HOOK,NULL);
    
    UTF_put_text("\nTest Case #2: OS_BinSemCreate Return Code test\n");
    UTF_CFE_OSAPI_Set_Api_Return_Code(CFE_OSAPI_BINSEMCREATE_PROC, 0x12ab34cd);
    UTF_put_text("OS_BinSemCreate Return Code %x\n", OS_BinSemCreate(&semId,"ReturnSemName",0,0));
    UTF_CFE_OSAPI_Use_Default_Api_Return_Code(CFE_OSAPI_BINSEMCREATE_PROC);

    /* CFE_ES_WriteToSysLog Test */
   	char VeryLongString[400];
    UTF_put_text("\n***Test CFE_ES_WriteToSysLog API***\n");
    
    UTF_put_text("\nTest Case #1: Call CFE_ES_WriteToSysLog() with short string.\n"); 
    ReturnVal = CFE_ES_WriteToSysLog("This is a short string for the Sys Log.\n");
    UTF_put_text("CFE_ES_WriteToSysLog returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    
    UTF_put_text("\nTest Case #2: Call CFE_ES_WriteToSysLog() with long string.\n"); 
    strcpy(VeryLongString, "Long strings ");
    strcat(VeryLongString, "will have to be truncated because only 128 characters can be accommodated ");
    strcat(VeryLongString, "including the 'SysLog:' prefix that is prepended in UTF. ");
    strcat(VeryLongString, "Extra characters will be lost.\n");
    ReturnVal = CFE_ES_WriteToSysLog(VeryLongString);
    UTF_put_text("CFE_ES_WriteToSysLog returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
 
    UTF_put_text("\nTest Case #3: Call CFE_ES_WriteToSysLog() with another short string.\n"); 
    ReturnVal = CFE_ES_WriteToSysLog("So long, it's been good to know ya.\n");
    UTF_put_text("CFE_ES_WriteToSysLog returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #4: Set return for CFE_ES_WriteToSysLog to CFE_ES_ERR_SYS_LOG_FULL\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_WRITETOSYSLOG_PROC, CFE_ES_ERR_SYS_LOG_FULL);
    ReturnVal = CFE_ES_WriteToSysLog("Wimoweh");
    UTF_put_text("CFE_ES_WriteToSysLog returned %x. Expected %x.\n", ReturnVal, CFE_ES_ERR_SYS_LOG_FULL);
    
    UTF_put_text("\nTest Case 5: Revert return for CFE_ES_WriteToSysLog to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_WRITETOSYSLOG_PROC);
    ReturnVal = CFE_ES_WriteToSysLog("Big Jake--a bargain!\n");
    UTF_put_text("CFE_ES_WriteToSysLog returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);

    /* CFE_ES_RegisterDriver Test */
    UTF_put_text("\n***Test CFE_ES_RegisterDriver API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_RegisterDriver()\n"); 
    ReturnVal = CFE_ES_RegisterDriver(0,0);
    UTF_put_text("CFE_ES_RegisterDriver returned %x. Expected %x.\n", ReturnVal,CFE_ES_NOT_IMPLEMENTED);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_RegisterDriver to 0x83ab83ab\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERDRIVER_PROC, 0x83ab83ab);
    ReturnVal = CFE_ES_RegisterDriver(0,0);
    UTF_put_text("CFE_ES_RegisterDriver returned %x. Expected 0x83ab83ab.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_RegisterDriver to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERDRIVER_PROC);
    ReturnVal = CFE_ES_RegisterDriver(0,0);
    UTF_put_text("CFE_ES_RegisterDriver returned %x. Expected %x.\n", ReturnVal,CFE_ES_NOT_IMPLEMENTED);
    
    /* CFE_ES_UnloadDriver Test */
    UTF_put_text("\n***Test CFE_ES_UnloadDriver API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_UnloadDriver()\n"); 
    ReturnVal = CFE_ES_UnloadDriver(5);
    UTF_put_text("CFE_ES_UnloadDriver returned %x. Expected %x.\n", ReturnVal,CFE_ES_NOT_IMPLEMENTED);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_UnloadDriver to 0x83ab83ab\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_UNLOADDRIVER_PROC, 0x83ab83ab);
    ReturnVal = CFE_ES_UnloadDriver(5);
    UTF_put_text("CFE_ES_UnloadDriver returned %x. Expected 0x83ab83ab.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_UnloadDriver to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_UNLOADDRIVER_PROC);
    ReturnVal = CFE_ES_UnloadDriver(5);
    UTF_put_text("CFE_ES_UnloadDriver returned %x. Expected %x.\n", ReturnVal,CFE_ES_NOT_IMPLEMENTED);
    
    /* CFE_ES_CalculateCRC Test */
    UTF_put_text("\n***Test CFE_ES_CalculateCRC API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_CalculateCRC(%d)\n",CFE_ES_DEFAULT_CRC);
/*    CFE_ES_CDSBlockDesc.CRC = CFE_ES_CalculateCRC(DataToWrite, CFE_ES_CDSBlockDesc.SizeUsed, 0, CFE_ES_DEFAULT_CRC); */
    ReturnVal = CFE_ES_CalculateCRC(AppName,64,0,CFE_ES_DEFAULT_CRC);
    UTF_put_text("CFE_ES_CalculateCRC returned %x. Expected non-zero.\n", ReturnVal);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_CalculateCRC to 0x7ef97ef9\n"); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_CALCULATECRC_PROC, 0x7ef97ef9);
    ReturnVal = CFE_ES_CalculateCRC(0,64,0,CFE_ES_DEFAULT_CRC);
    UTF_put_text("CFE_ES_CalculateCRC returned %x. Expected 0x7ef97ef9.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_CalculateCRC to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_CALCULATECRC_PROC);
    ReturnVal = CFE_ES_CalculateCRC(0,64,0,CFE_ES_CRC_32);
    UTF_put_text("CFE_ES_CalculateCRC returned %x. Expected 0.\n", ReturnVal);
    
    UTF_put_text("\nTest Case #4: Call CFE_ES_CalculateCRC(%d)\n",CFE_ES_CRC_8); 
    ReturnVal = CFE_ES_CalculateCRC(0,64,0,CFE_ES_CRC_8);
    UTF_put_text("CFE_ES_CalculateCRC returned %x. Expected 0.\n", ReturnVal);

    UTF_put_text("\nTest Case #5: Call CFE_ES_CalculateCRC(%d)\n",CFE_ES_CRC_16); 
    ReturnVal = CFE_ES_CalculateCRC(AppName,64,0,CFE_ES_CRC_16);
    UTF_put_text("CFE_ES_CalculateCRC returned %x. Expected non-zero.\n", ReturnVal);

    /* Test the Critical Data Store (CDS) functions */
    UTF_ES_AddAppRecord("UTF Test14",5); 
	UTF_CFE_ES_SetAppID(5);
    
    /* Initialize the CDS */
    CFE_ES_CDS_EarlyInit();
    
    /* Declare CDS data variables */
    test14_TaskData_t test14Data, test14RestoredData;
    
    UTF_put_text("\n***Test CFE_ES_RegisterCDS API***\n");

    UTF_put_text("\nTest Case #1: Call CFE_ES_RegisterCDS()\n");
    ReturnVal = CFE_ES_RegisterCDS(&test14Data.myCDSHandle,
    							sizeof(test14_TaskData_t),"Test14Data");
    UTF_put_text("CFE_ES_RegisterCDS returned %x.\n", ReturnVal);

    UTF_put_text("\nTest Case #2: Set return for CFE_ES_RegisterCDS to %x\n",CFE_ES_CDS_INVALID_NAME); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCDS_PROC, CFE_ES_CDS_INVALID_NAME);
    ReturnVal = CFE_ES_RegisterCDS(&test14Data.myCDSHandle,
    							sizeof(test14_TaskData_t),"Test14Data");
    UTF_put_text("CFE_ES_RegisterCDS returned %x. Expected %x.\n", ReturnVal,CFE_ES_CDS_INVALID_NAME);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_RegisterCDS to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERCDS_PROC);
	/* Calling this a second time should return the CFE_ES_CDS_ALREADY_EXISTS code */
    ReturnVal = CFE_ES_RegisterCDS(&test14Data.myCDSHandle,
    							sizeof(test14_TaskData_t),"Test14Data");
    UTF_put_text("CFE_ES_RegisterCDS returned %x. Expected %x.\n", ReturnVal, CFE_ES_CDS_ALREADY_EXISTS);

    UTF_put_text("\n***Test CFE_ES_CopyToCDS & CFE_ES_RestoreFromCDS API***\n");
	test14Data.myCDSData.TestData1 = 128;
	strcpy(test14Data.myCDSData.TestData2, "Testing CDS strings");
	test14Data.myCDSData.TestData3 = 45;
	
    UTF_put_text("\nTest Case #1: Call CFE_ES_CopyToCDS()\n");
    ReturnVal = CFE_ES_CopyToCDS(test14Data.myCDSHandle,&test14Data);
    UTF_put_text("CFE_ES_CopyToCDS returned %x.\n", ReturnVal);
	if (ReturnVal == OS_SUCCESS)
	{
	    UTF_put_text("Copied data of <%d>; <%s>; <%d> to CDS\n",
	    	test14Data.myCDSData.TestData1,
	    	test14Data.myCDSData.TestData2,
	    	test14Data.myCDSData.TestData3);

	    UTF_put_text("\nCall CFE_ES_RestoreFromCDS()\n");
    	ReturnVal = CFE_ES_RestoreFromCDS(&test14RestoredData,test14Data.myCDSHandle);
    	UTF_put_text("CFE_ES_RestoreFromCDS returned %x.\n", ReturnVal);
		if (ReturnVal == OS_SUCCESS)
		{
	    	UTF_put_text("Restored data = <%d>; <%s>; <%d> to CDS\n",
	    		test14RestoredData.myCDSData.TestData1,
	    		test14RestoredData.myCDSData.TestData2,
	    		test14RestoredData.myCDSData.TestData3);
		}
	}
	
    UTF_put_text("\nTest Case #2: Set return for CFE_ES_CopyToCDS to %x\n", CFE_ES_ERR_MEM_HANDLE); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_COPYTOCDS_PROC, CFE_ES_ERR_MEM_HANDLE);
    ReturnVal = CFE_ES_CopyToCDS(test14Data.myCDSHandle,&test14Data);
    UTF_put_text("CFE_ES_CopyToCDS returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_MEM_HANDLE);

    UTF_put_text("\nSet return for CFE_ES_RestoreFromCDS to %x\n", CFE_ES_CDS_INVALID); 
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCDS_PROC, CFE_ES_CDS_INVALID);
    ReturnVal = CFE_ES_RestoreFromCDS(&test14RestoredData,test14Data.myCDSHandle);
    UTF_put_text("CFE_ES_RestoreFromCDS returned %x. Expected %x.\n", ReturnVal,CFE_ES_CDS_INVALID);

    UTF_put_text("\nTest Case #3: Revert return for CFE_ES_CopyToCDS & CFE_ES_RestoreFromCDS to default\n"); 
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_COPYTOCDS_PROC);
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERCDS_PROC);

	test14Data.myCDSData.TestData1 = 45;
	strcpy(test14Data.myCDSData.TestData2, "CDS string Test #2");
	test14Data.myCDSData.TestData3 = 128;
    ReturnVal = CFE_ES_CopyToCDS(test14Data.myCDSHandle,&test14Data);
    UTF_put_text("CFE_ES_CopyToCDS returned %x.\n", ReturnVal);
	if (ReturnVal == OS_SUCCESS)
	{
	    UTF_put_text("Copied data of <%d>; <%s>; <%d> to CDS\n",
	    	test14Data.myCDSData.TestData1,
	    	test14Data.myCDSData.TestData2,
	    	test14Data.myCDSData.TestData3);

	    UTF_put_text("\nCall CFE_ES_RestoreFromCDS()\n");
    	ReturnVal = CFE_ES_RestoreFromCDS(&test14RestoredData,test14Data.myCDSHandle);
    	UTF_put_text("CFE_ES_RestoreFromCDS returned %x.\n", ReturnVal);
		if (ReturnVal == OS_SUCCESS)
		{
	    	UTF_put_text("Restored data = <%d>; <%s>; <%d> to CDS\n",
	    		test14RestoredData.myCDSData.TestData1,
	    		test14RestoredData.myCDSData.TestData2,
	    		test14RestoredData.myCDSData.TestData3);
		}
	}

    UTF_put_text("\n***Testing CFE_ES_DeleteCDS***\n");
    char CDSName[CFE_ES_CDS_MAX_FULL_NAME_LEN] = {""};
    /* Call FormCDSName */
	CFE_ES_FormCDSName(CDSName, "Test14Data", 5);

    UTF_put_text("\nTest Case #1: Call CFE_ES_DeleteCDS for an active Application\n"); 
    ReturnVal = CFE_ES_DeleteCDS(CDSName,0);
    UTF_put_text("CFE_ES_DeleteCDS(%s) returned %x. Expected %x.\n",CDSName,ReturnVal,CFE_ES_CDS_OWNER_ACTIVE_ERR);

    UTF_put_text("\nTest Case #2: Call CFE_ES_DeleteCDS with the flag indicating a Critical Table CDS\n"); 
    ReturnVal = CFE_ES_DeleteCDS(CDSName,1);
    UTF_put_text("CFE_ES_DeleteCDS(%s) returned %x. Expected %x.\n",CDSName,ReturnVal,CFE_ES_CDS_WRONG_TYPE_ERR);

    UTF_put_text("\nTest Case #3: Delete the Application and call CFE_ES_DeleteCDS\n"); 
    ReturnVal = CFE_ES_DeleteApp(5);
    UTF_put_text("CFE_ES_DeleteApp(5) returned %x. Expected %x.\n", ReturnVal,CFE_SUCCESS);
    ReturnVal = CFE_ES_DeleteCDS(CDSName,0);
    UTF_put_text("CFE_ES_DeleteCDS(%s) returned %x. Expected %x.\n",CDSName,ReturnVal,CFE_SUCCESS);

    UTF_put_text("\nTest Case #1: Call CFE_ES_DeleteCDS again to verify the CDS is deleted.\n"); 
    ReturnVal = CFE_ES_DeleteCDS(CDSName,0);
    UTF_put_text("CFE_ES_DeleteCDS(%s) returned %x. Expected %x.\n",CDSName,ReturnVal,CFE_ES_CDS_NOT_FOUND_ERR);

    UTF_put_text("\n***Dumping the Task Record Table***\n");
    UTF_ES_DumpTaskRecords();

    UTF_put_text("\n***Test CFE_ES_GetTaskInfo API***\n");
	CFE_ES_TaskInfo_t myTaskInfo;
	
	ReturnVal = UTF_ES_GetTaskIDByAppID(11);
	if (ReturnVal != -1)
	{
		uint32 taskID = ReturnVal;
	    UTF_put_text("\nTest Case #1: Call CFE_ES_GetTaskInfo(%d)\n", taskID); 
		ReturnVal = CFE_ES_GetTaskInfo(&myTaskInfo,taskID);
    	UTF_put_text("CFE_ES_GetTaskInfo(%d) returned %x. Expected %x.\n",taskID,ReturnVal,CFE_SUCCESS);
		if (ReturnVal == CFE_SUCCESS)
		{
			UTF_put_text("AppID    = %d\n",myTaskInfo.AppId);
		   	UTF_put_text("TaskName = %s\n",myTaskInfo.TaskName);
	   		UTF_put_text("AppName  = %s\n",myTaskInfo.AppName);
		}
	}
	
	UTF_put_text("\nTest Case #2: Call CFE_ES_GetTaskInfo(45)\n"); 
	ReturnVal = CFE_ES_GetTaskInfo(&myTaskInfo,45);
    UTF_put_text("CFE_ES_GetTaskInfo(45) returned %x. Expected %x.\n", ReturnVal,CFE_ES_ERR_TASKID);
	
    UTF_put_text("\n***Dump the Registered App Table***\n");
	UTF_ES_DumpAppRecords();

    UTF_put_text("\n***Test CFE_ES_GetAppInfo API***\n");
    CFE_ES_AppInfo_t myAppInfo;
    
	UTF_put_text("\nTest Case #1: Call CFE_ES_GetAppInfo(12)\n"); 
    ReturnVal = CFE_ES_GetAppInfo(&myAppInfo,12);
    UTF_put_text("CFE_ES_GetAppInfo(12) returned %x. Expected %x.\n",ReturnVal,CFE_SUCCESS);
	if (ReturnVal == CFE_SUCCESS)
	{
		UTF_put_text("AppID        = %d\n",myAppInfo.AppId);
	   	UTF_put_text("Type         = %d\n",myAppInfo.Type);
	  	UTF_put_text("AppName      = '%s'\n",myAppInfo.Name);
	  	UTF_put_text("EntryPoint   = '%s'\n",myAppInfo.EntryPoint);
	  	UTF_put_text("FileName     = '%s'\n",myAppInfo.FileName);
	   	UTF_put_text("StackSize    = %d\n",myAppInfo.StackSize);
	   	UTF_put_text("StartAddress = %d\n",myAppInfo.StartAddress);
	   	UTF_put_text("ExceptnActn  = %d\n",myAppInfo.ExceptionAction);
	   	UTF_put_text("Priority     = %d\n",myAppInfo.Priority);
	   	UTF_put_text("MainTaskId   = %d\n",myAppInfo.MainTaskId);
	   	UTF_put_text("MainTaskName = '%s'\n",myAppInfo.MainTaskName);
	   	UTF_put_text("NumChildTasks= %d\n",myAppInfo.NumOfChildTasks);
	   	UTF_put_text("ModuleId     = %d\n",myAppInfo.ModuleId);
	   	UTF_put_text("AddrsValid   = %d\n",myAppInfo.AddressesAreValid);
	   	UTF_put_text("CodeAddress  = %d\n",myAppInfo.CodeAddress);
	   	UTF_put_text("CodeSize     = %d\n",myAppInfo.CodeSize);
	   	UTF_put_text("DataAddress  = %d\n",myAppInfo.DataAddress);
	   	UTF_put_text("DataSize     = %d\n",myAppInfo.DataSize);
	   	UTF_put_text("BSSAddress   = %d\n",myAppInfo.BSSAddress);
	   	UTF_put_text("BSSSize      = %d\n",myAppInfo.BSSSize);
	}

	UTF_put_text("\nTest Case #2: Call CFE_ES_GetAppInfo(25)\n"); 
    ReturnVal = CFE_ES_GetAppInfo(&myAppInfo,25);
    UTF_put_text("CFE_ES_GetAppInfo(25) returned %x. Expected %x.\n",ReturnVal,CFE_ES_ERR_APPID);

	UTF_put_text("\nTest Case #3: Call CFE_ES_GetAppInfo(45)\n"); 
    ReturnVal = CFE_ES_GetAppInfo(&myAppInfo,45);
    UTF_put_text("CFE_ES_GetAppInfo(45) returned %x. Expected %x.\n",ReturnVal,CFE_ES_ERR_APPID);

	UTF_put_text("\nTest Case #4: Change some AppInfo using the UTF_SetDefaultAppInfo utility\n"); 
	myAppInfo.AddressesAreValid = TRUE;
	myAppInfo.CodeAddress = 123456000;
	myAppInfo.CodeSize = 512;
	myAppInfo.DataAddress = 123456512;
	myAppInfo.DataSize = 256;
	myAppInfo.BSSAddress = 123457000;
	myAppInfo.BSSSize = 1024;
	ReturnVal = UTF_CFE_ES_SetDefaultAppInfo(myAppInfo);
	
	UTF_put_text("Call CFE_ES_GetAppInfo(12)\n"); 
    ReturnVal = CFE_ES_GetAppInfo(&myAppInfo,12);
    UTF_put_text("CFE_ES_GetAppInfo(12) returned %x. Expected %x.\n",ReturnVal,CFE_SUCCESS);
	if (ReturnVal == CFE_SUCCESS)
	{
		UTF_put_text("AppID        = %d\n",myAppInfo.AppId);
	   	UTF_put_text("Type         = %d\n",myAppInfo.Type);
	  	UTF_put_text("AppName      = '%s'\n",myAppInfo.Name);
	  	UTF_put_text("EntryPoint   = '%s'\n",myAppInfo.EntryPoint);
	  	UTF_put_text("FileName     = '%s'\n",myAppInfo.FileName);
	   	UTF_put_text("StackSize    = %d\n",myAppInfo.StackSize);
	   	UTF_put_text("StartAddress = %d\n",myAppInfo.StartAddress);
	   	UTF_put_text("ExceptnActn  = %d\n",myAppInfo.ExceptionAction);
	   	UTF_put_text("Priority     = %d\n",myAppInfo.Priority);
	   	UTF_put_text("MainTaskId   = %d\n",myAppInfo.MainTaskId);
	   	UTF_put_text("MainTaskName = '%s'\n",myAppInfo.MainTaskName);
	   	UTF_put_text("NumChildTasks= %d\n",myAppInfo.NumOfChildTasks);
	   	UTF_put_text("ModuleId     = %d\n",myAppInfo.ModuleId);
	   	UTF_put_text("AddrsValid   = %d\n",myAppInfo.AddressesAreValid);
	   	UTF_put_text("CodeAddress  = %d\n",myAppInfo.CodeAddress);
	   	UTF_put_text("CodeSize     = %d\n",myAppInfo.CodeSize);
	   	UTF_put_text("DataAddress  = %d\n",myAppInfo.DataAddress);
	   	UTF_put_text("DataSize     = %d\n",myAppInfo.DataSize);
	   	UTF_put_text("BSSAddress   = %d\n",myAppInfo.BSSAddress);
	   	UTF_put_text("BSSSize      = %d\n",myAppInfo.BSSSize);
	}

    UTF_put_text("\n***Test CFE_ES_RunLoop API***\n");
	uint32 exitStatus=0;
	UTF_CFE_ES_SetAppID(12);

    UTF_put_text("\nTest Case #1: Call CFE_ES_RunLoop(CFE_ES_APP_RUN)\n"); 
	exitStatus = CFE_ES_APP_RUN;
    ReturnVal = CFE_ES_RunLoop(&exitStatus);
    UTF_put_text("CFE_ES_RunLoop(CFE_ES_APP_RUN) returned %x. Expected 1.\n", ReturnVal);

    UTF_put_text("\nTest Case #2: Call CFE_ES_RunLoop(CFE_ES_APP_EXIT)\n"); 
	exitStatus = CFE_ES_APP_EXIT;
    ReturnVal = CFE_ES_RunLoop(&exitStatus);
    UTF_put_text("CFE_ES_RunLoop(CFE_ES_APP_EXIT) returned %x. Expected 0.\n", ReturnVal);

    UTF_put_text("\nTest Case #3: Call CFE_ES_RunLoop(CFE_ES_APP_ERROR)\n"); 
	exitStatus = CFE_ES_APP_ERROR;
    ReturnVal = CFE_ES_RunLoop(&exitStatus);
    UTF_put_text("CFE_ES_RunLoop(CFE_ES_APP_ERROR) returned %x. Expected 0.\n", ReturnVal);    

    UTF_put_text("\nTest Case #4: Call CFE_ES_RunLoop(45)\n"); 
	exitStatus = 45;
    ReturnVal = CFE_ES_RunLoop(&exitStatus);
    UTF_put_text("CFE_ES_RunLoop(45) returned %x. Expected 0.\n", ReturnVal);    

    UTF_put_text("\nTest Case #5: Call CFE_ES_RunLoop with an invalid AppID\n");
	/* Set the return code for the GetAppID proc to CFE_ES_ERR_APPID */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_GETAPPID_PROC, CFE_ES_ERR_APPID);

 	exitStatus = CFE_ES_APP_RUN;
	UTF_CFE_ES_SetAppID(30);
    ReturnVal = CFE_ES_RunLoop(&exitStatus);
    UTF_put_text("CFE_ES_RunLoop() returned %x. Expected 0.\n", ReturnVal);    
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_GETAPPID_PROC);

    UTF_put_text("\n***Test CFE_ES_ExitApp API***\n");
    CFE_ES_ExitApp(0);
 	exit(0);
}

int32 delete_hook(uint32 taskID)
{
	UTF_put_text("OS_TaskDelete Function Hook called for id %d\n",taskID);
	return OS_SUCCESS;
}
