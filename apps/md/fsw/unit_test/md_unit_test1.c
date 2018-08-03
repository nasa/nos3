/* File: md_unit_test1.c
**  $Id: md_unit_test1.c 1.7 2012/01/09 19:28:18EST aschoeni Exp  $
**
** Purpose: 
**   Test driver for unit testing of CFS Memory Dwell Application.
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

int32 LongWordValue = 45;

extern MD_AppData_t MD_AppData;
extern CFE_ES_AppRecord_t ES_AppTable[CFE_ES_MAX_APPLICATIONS];


unsigned short int msg_queue_id = 1;

#define CFS_MD_CMD_PIPE 1



    /********************************/
    /* Special Command Functions    */
    /********************************/

/* Use to set value of return for CFE_PSP_MemValidateRange calls */
void UTF_SetMemRangeError ( int argc, char *argv[])
{
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_ERROR);
}

/* Use to set value of return for CFE_PSP_MemValidateRange calls */
void UTF_SetMemRangeValid ( int argc, char *argv[])
{
    UTF_PSP_Set_Api_Return_Code(CFE_PSP_MEMVALIDATERANGE_PROC, OS_SUCCESS);
}

/* Display Enabled field value, from Table Services Buffer, for each table */
void UTF_ShowBufferEnabledFields ( int argc, char *argv[])
{
    int32 TblIndex;
    MD_DwellTableLoad_t *MD_LoadTablePtr = 0; 
    int32 EnabledValue;
    int32 GetAddressResult;

    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
    {
        GetAddressResult = CFE_TBL_GetAddress ( (void *) &MD_LoadTablePtr,  
                                            MD_AppData.MD_TableHandle[TblIndex]);
        EnabledValue = MD_LoadTablePtr->Enabled;
        UTF_put_text("Table #%d Enabled = %d\n", TblIndex + 1, EnabledValue);
        
        /* Unlock Table */
        CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TblIndex] );


    }
    
    return;
}

/* Display values from Table Services Buffer for specified table and entry */
void UTF_ShowBufferJamEntry ( int argc, char *argv[])
{

    uint16 TblNum;
    uint16 EntryNum;
    uint16 TblIndex;
    uint16 EntryIndex;

    int32 GetAddressResult;
    MD_DwellTableLoad_t *MD_LoadTablePtr = 0; 
    MD_TableLoadEntry_t *MD_EntryPtr = 0;

    /* Check for correct number of arguments */
    if (argc != 3)
    {
        UTF_error("UTF Error: argc = %d in UTF_ShowBufferJamEntry. Expected 3.\n", argc  );
        UTF_exit();
    }

    /* Extract argument values */
    TblNum = UTF_arg2uint(argv[1]);
    EntryNum  = UTF_arg2uint(argv[2]);
    TblIndex = TblNum - 1;
    EntryIndex = EntryNum - 1;


    GetAddressResult = CFE_TBL_GetAddress ( (void *) &MD_LoadTablePtr,  
                                            MD_AppData.MD_TableHandle[TblIndex]);
    MD_EntryPtr = &MD_LoadTablePtr->Entry[EntryIndex];
                                            
    UTF_put_text("Contents of Table #%d Entry #%d\n", TblNum, EntryNum);
    UTF_put_text("\tLength = %d\n", MD_EntryPtr->Length);
    UTF_put_text("\tDelay = %d\n", MD_EntryPtr->Delay);
    UTF_put_text("\tDwellAddress.Offset = 0x%08X\n", MD_EntryPtr->DwellAddress.Offset);
    UTF_put_text("\tDwellAddress.SymName = '%s'\n", MD_EntryPtr->DwellAddress.SymName);
        
    /* Unlock Table */
    CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TblIndex] );

    return;
}

/* Display signature from Table Services Buffer for specified table */
void UTF_ShowBufferSignature ( int argc, char *argv[])
{
#if MD_SIGNATURE_OPTION == 1  
    uint16 TblNum;
    uint16 TblIndex;

    int32 GetAddressResult;
    MD_DwellTableLoad_t *MD_LoadTablePtr = 0; 

    /* Check for correct number of arguments */
    if (argc != 2)
    {
        UTF_error("UTF Error: argc = %d in UTF_ShowBufferSignature. Expected 2.\n", argc  );
        UTF_exit();
    }

    /* Extract argument values */
    TblNum = UTF_arg2uint(argv[1]);
    TblIndex = TblNum - 1;


    GetAddressResult = CFE_TBL_GetAddress ( (void *) &MD_LoadTablePtr,  
                                            MD_AppData.MD_TableHandle[TblIndex]);
                                            
    UTF_put_text("Signature of Table #%d ='%s'\n", TblNum,MD_LoadTablePtr->Signature );
        
    /* Unlock Table */
    CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TblIndex] );
	
#else
    UTF_put_text("Signature option is disabled\n" );
#endif	

    return;
}

/* Use to add simulated memory range */
void UTF_AddSimulatedMemory ( int argc, char *argv[])
{
    int32 RetVal;
    uint8 data1[4];
    uint32 data2;
    data1[1] = 0xAA;
    data1[2] = 0xBB;
    data1[3] = 0xCC;
    data1[4] = 0xDD;
    data2 = 0xabbadaba;

    /* Calling UTF_read_sim_address makes UTF simulate successful reads */
    /* for this range. */
    UTF_add_sim_address (0x004610A0, 8, "MEMORY_BANK" );
    UTF_write_sim_address(0x004610A0, 4, &data1);
    UTF_write_sim_address(0x004610A4, 4, &data2);

    /* Calling CFE_PSP_MemRangeSet makes calls to CFE_PSP_MemValidateRange for this */
    /* range return true. */
    RetVal = CFE_PSP_MemRangeSet(0, CFE_PSP_MEM_RAM, 0x004610A0, 
                     0x00000008,  CFE_PSP_MEM_SIZE_BYTE,  CFE_PSP_MEM_ATTR_READ);
    if (RetVal != OS_SUCCESS)
    {
       UTF_put_text ("Error return %d from CFE_PSP_MemRangeSet\n", RetVal);
    }
    
    /* Calling CFE_PSP_MemRangeSet makes calls to CFE_PSP_MemValidateRange for this */
    /* range return true. */
    RetVal = CFE_PSP_MemRangeSet(1, CFE_PSP_MEM_RAM, 0x005610A0, 
                     0x00000008,  CFE_PSP_MEM_SIZE_BYTE,  CFE_PSP_MEM_ATTR_READ);
    if (RetVal != OS_SUCCESS)
    {
       UTF_put_text ("Error return %d from CFE_PSP_MemRangeSet\n", RetVal);
    }
    
    /* No call to UTF_add_sim_address for address 0x005610A0 will cause */
    /* reads to return FALSE */
    
}


void UTF_SCRIPT_DisplayTableRegistry ( int argc, char *argv[])
{
    UTF_CFE_TBL_DisplayTableRegistryContents ();
}


int32 CFE_SB_CreatePipeHook (CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, char *PipeName)
/* CreatePipe is called many times in this test procedure, and we need to force its */
/* behavior so that PipeId stays at same value rather than incrementing as is default behavior for UTF 'stub'.  */
{
   *PipeIdPtr = CFS_MD_CMD_PIPE;

   return (CFE_SUCCESS);
}

int32 CFE_SB_SubscribeHook1 (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
/* This routine will return an error the 2nd time it's called; */
/* it will return CFE_SUCCESS on all other calls. */
{
   static uint32 Count = 1;
   if (Count++ == 2)
      return (CFE_SB_MAX_MSGS_MET);
   else
      return (CFE_SUCCESS);
}


int32 CFE_SB_SubscribeHook2 (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
/* This routine will return an error the 3rd time it's called; */
/* it will return CFE_SUCCESS on all other calls. */
{
   static uint32 Count = 1;
   if (Count++ == 3)
      return (CFE_SB_MAX_MSGS_MET);
   else
      return (CFE_SUCCESS);
}


int main(void)
{	
    /* int32 MemDwellAppId = 5; */
    int32 MemDwellAppId = 0;
    char MemDwellAppName[10];
	
    /********************************/
    /* Set up output file           */
    /********************************/
    UTF_set_output_filename("md_unit_test1.out");
    #ifdef UTF_USE_STDOUT 
    UTF_put_text("Std output is being used. \n");
    #endif

    /********************************/
    /* Set up input file           */
    /********************************/
    UTF_add_input_file(CFS_MD_CMD_PIPE, "md_unit_test1.in");
    MD_AppData.CmdPipe = CFS_MD_CMD_PIPE;

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
    UTF_init_sim_time(1.5);

    /**************************************************/
    /* Exercise those errors which cause App to exit */
    /* without  executing the RcvMsg call.           */
    /**************************************************/

    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Run Early Exit Cases\n");
    
    /**************************************************/
    /* Simulate error return from Executive Services. */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #1: CFE_ES_RegisterApp returns error\n");
    UTF_put_text("Expected Output: 1) Specific SysLog error 2) App exit\n");
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERAPP_PROC, OS_ERR_INVALID_ID );
    MD_AppMain();
    UTF_CFE_ES_Use_Default_Api_Return_Code(CFE_ES_REGISTERAPP_PROC);

    /**************************************************/
    /* Simulate error return from Event Services.     */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #2: CFE_EVS_Register returns error\n");
    UTF_put_text("Expected Output: 1) Specific SysLog error 2) App exit\n");
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, CFE_EVS_UNKNOWN_FILTER );
    MD_AppMain();
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);

    /**************************************************/
    /* Simulate error return from CFE_SB_CreatePipe.     */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #3: CFE_SB_CreatePipe returns error\n");
    UTF_put_text("Expected Output: 1) Specific case SysLog error 2) SysLog that init failed 3) App exit\n");
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, CFE_SB_PIPE_CR_ERR );
    MD_AppMain();
    UTF_SB_set_function_hook(CFE_SB_CREATEPIPE_HOOK, (void *) &CFE_SB_CreatePipeHook); /* CreatePipe will return same value for pipe id each time */

    /**************************************************/
    /* Simulate error return from CFE_SB_Subscribe.     */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #4: CFE_SB_Subscribe returns error\n");
    UTF_put_text("Expected Output: 1) Specific case SysLog error 2) SysLog that init failed 3) App exit\n");
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC, CFE_SB_MAX_DESTS_MET );
    MD_AppMain();
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC);

    /**************************************************/
    /* Need first CFE_SB_Subscribe call to pass, and  */
    /* 2nd to fail.                                   */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #5: 1st CFE_SB_Subscribe is success; 2nd is error\n");
    UTF_put_text("Expected Output: 1) Specific case SysLog error 2) SysLog that init failed 3) App exit\n");
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *) &CFE_SB_SubscribeHook1);
    MD_AppMain();

    /**************************************************/
    /* Need first two CFE_SB_Subscribe calls to pass, */
    /* and 3rd to fail.                               */
    /**************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #6: first two CFE_SB_Subscribe calls are successful; 3rd is error.\n");
    UTF_put_text("Expected Output: 1) Specific case SysLog error 2) SysLog that init failed 3) App exit\n");
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *) &CFE_SB_SubscribeHook2);
    MD_AppMain();


    /****************************************************************************/
    /* Test case: CFE_TBL_ERR_INVALID_SIZE error return from CFE_TBL_Register.  */
    /****************************************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #7: CFE_TBL_ERR_INVALID_SIZE return from CFE_TBL_Register.\n");
    UTF_put_text("Expected Results: 1) Critical event: too large table couldn't be registered 2) SysLog Msg: task failed to initialize 3) App exit\n");
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, CFE_TBL_ERR_INVALID_SIZE);
    MD_AppMain();
    UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_REGISTER_PROC);

    /****************************************************************************/
    /* Test case: CFE_ES_ERR_BUFFER error return from CFE_TBL_Load.  */
    /****************************************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #8: CFE_ES_ERR_BUFFER return from CFE_TBL_Load.\n");
    UTF_put_text("Expected Results: 1) SysLog Msg:  'Error 0x_ received loading tbl#_'\n");
    UTF_put_text("                  2) SysLog Msg: 'MD:Application Init Failed,RC=________X'\n");
    UTF_put_text("                  3) App exit\n");
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_LOAD_PROC, CFE_ES_ERR_BUFFER);
    MD_AppMain();
    UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_LOAD_PROC);

    /**************************************************/
    /* Add tasks  to ES's list of tasks, and          */
    /* register local task with Executive Services.   */
    /**************************************************/
    UTF_put_text("\n***Add tasks to ES list of tasks ***\n");
    UTF_ES_InitTaskRecords();

    if (MemDwellAppId < 0)
    {
        UTF_put_text("\n***Error return from UTF_ES_GetFreeTaskRecord***\n");
        exit(0);
    }
    strcpy(MemDwellAppName,"MD_APP");
    UTF_ES_AddAppRecord(MemDwellAppName,  MemDwellAppId);	

    UTF_ES_DumpAppRecords();

    /*****************************************************************/
    /* Test case: General error return from CFE_TBL_Register.        */
    /*****************************************************************/
    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    UTF_put_text("Early Exit Case #9: General error return from CFE_TBL_Register.\n");
    UTF_put_text("Expected Results: 1) Critical event 2) SysLog Msg: task failed to initialize 3) App exit\n");
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, CFE_TBL_ERR_DUPLICATE_NOT_OWNED);
    MD_AppMain();
    UTF_CFE_TBL_Use_Default_Api_Return_Code(CFE_TBL_REGISTER_PROC);
    
    /**************************************************/
    /* Add "Special" Commands                         */
    /**************************************************/
    UTF_add_special_command("SET_MEM_RANGE_FALSE",  UTF_SetMemRangeError); 
    UTF_add_special_command("SET_MEM_RANGE_VALID",  UTF_SetMemRangeValid); 
    UTF_add_special_command("DISPLAY_TABLE_REGISTRY", UTF_SCRIPT_DisplayTableRegistry);
    UTF_add_special_command("ADD_SIM_MEMORY",  UTF_AddSimulatedMemory);
    UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
    UTF_add_special_command("USE_DEFAULT_SB_RETURN_CODE", UTF_SCRIPT_SB_Use_Default_Api_Return_Code);
    UTF_add_special_command("SET_TBL_RETURN_CODE", UTF_SCRIPT_TBL_Set_Api_Return_Code);
    UTF_add_special_command("USE_DEFAULT_TBL_RETURN_CODE", UTF_SCRIPT_TBL_Use_Default_Api_Return_Code);
    UTF_add_special_command("SHOW_BUFFER_ENABLED_FIELDS", UTF_ShowBufferEnabledFields);
    UTF_add_special_command("SHOW_BUFFER_JAM_ENTRY", UTF_ShowBufferJamEntry);
    UTF_add_special_command("SHOW_BUFFER_SIGNATURE", UTF_ShowBufferSignature);

 
   /**************************************************/
   /* Set up ES ES_AppTable so that UTF versions of  */
   /* CFE_ES_RunLoop, etc. can run correctly.        */
   /**************************************************/
   UTF_CFE_ES_SetAppID(MemDwellAppId);
   UTF_ES_InitAppRecords();
   strcpy(MemDwellAppName,"MD_APP");
   UTF_ES_AddAppRecord(MemDwellAppName,  MemDwellAppId);	
   UTF_ES_DumpAppRecords();

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
