/* File: md_unit_test3.c
**  $Id: md_unit_test3.c 1.5 2012/01/09 19:28:15EST aschoeni Exp  $
**
** Purpose: 
**   Test driver for unit testing of CFS Memory Dwell Application.
**   Test cases in which dwell table is restored from Critical Data Store.
**   Need to test a valid table and an invalid table.
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
extern CFE_TBL_TaskData_t CFE_TBL_TaskData;


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

/* Use to add simulated memory range */
void UTF_AddSimulatedMemory ( int argc, char *argv[])
{
    uint8 data1[4];
    uint32 data2;
    data1[1] = 0xAA;
    data1[2] = 0xBB;
    data1[3] = 0xCC;
    data1[4] = 0xDD;
    data2 = 0xabbadaba;
    UTF_add_sim_address (0x004610A0, 8, "MEMORY_BANK" );
    UTF_write_sim_address(0x004610A0, 4, &data1);
    UTF_write_sim_address(0x004610A4, 4, &data2);
}

void UTF_SCRIPT_DisplayTableRegistry ( int argc, char *argv[])
{
    UTF_CFE_TBL_DisplayTableRegistryContents ();
}

int32 CFE_TBL_Register_FunctionHook ( CFE_TBL_Handle_t *TblHandlePtr,
                        const char *Name,
                        uint32  Size,
                        uint16  TblOptionFlags,
                        CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr )
/* To be used before control is turned over to input command file. */
/* Used in concert with CFE_TBL_GetAddress_FunctionHook to set up test cases. */
{
   static uint32 Count = 0;
   CFE_TBL_Handle_t AllocatedHandle;
   Count++;
   
   /* Allocate a handle from Table Services */
   AllocatedHandle =  CFE_TBL_FindFreeHandle();
   
   /* Mark that handle as used.  Necessary for future accesses. */
   CFE_TBL_TaskData.Handles[AllocatedHandle].UsedFlag = TRUE; 
   
   /* Provide handle value to caller */
   *TblHandlePtr = (CFE_TBL_Handle_t) AllocatedHandle; 

   /* Assign and return return code. */
   if ((Count >= 1) && (Count <=4))
      return (CFE_TBL_INFO_RECOVERED_TBL);
   else if ((Count >= 5) && (Count <= 8))
      return (CFE_TBL_INFO_RECOVERED_TBL);
   else
      return (CFE_SUCCESS);
}

int32 CFE_TBL_GetAddress_FunctionHook ( void **TblPtr,
                          CFE_TBL_Handle_t TblHandle )
/* To be used before control is turned over to input command file. */
/* Used in concert with CFE_TBL_Register_FunctionHook to set up test cases. */
/* Count 1: invalid table */
/* Count 2: valid table and unexpected return code */
/* Count 3: valid table with enabled field == TRUE */
/* Count 4: valid table and nominal return code */
{
   static uint32 Count = 0;
   Count++;
   static MD_DwellTableLoad_t DwellTbl;
   static int16 Num1 = 5;
   int32 RetVal;
      
   UTF_put_text("CALL #%d to GetAddress_FunctionHook\n", Count);
          
   CFE_PSP_MemSet(&DwellTbl, 0, sizeof(MD_DwellTableLoad_t));


    /********************************/
    /* Set up Memory                */
    /********************************/
    
    /* Calling CFE_PSP_MemRangeSet makes calls to CFE_PSP_MemValidateRange for this */
    /* range return true. */
    RetVal = CFE_PSP_MemRangeSet(1, CFE_PSP_MEM_RAM, (uint32)&Num1,
                     0x00000002,  CFE_PSP_MEM_SIZE_BYTE,  CFE_PSP_MEM_ATTR_READ);
    if (RetVal != OS_SUCCESS)
    {
       UTF_put_text ("Error return %d from CFE_PSP_MemRangeSet\n", RetVal);
    }
    

/* Recover invalid table w/invalid value for enable flag */
/* for dwell tables #1 and #4                            */
   if (Count == 1)
/* Use for first dwell table */
   {
      /* Set TblPtr to point to an invalid table */
      DwellTbl.Enabled = 17;  /* invalid value */
      DwellTbl.Entry[0].Length = 2;
      DwellTbl.Entry[0].Delay = 1;
      DwellTbl.Entry[0].DwellAddress.Offset = (uint32)&Num1;
      strcpy (DwellTbl.Entry[0].DwellAddress.SymName, "");
      DwellTbl.Entry[1].Length = 0;
      DwellTbl.Entry[1].Delay = 0;
      DwellTbl.Entry[1].DwellAddress.Offset = 0;
   /*   strcpy (DwellTbl.Entry[1].DwellAddress.SymName, ""); */
      DwellTbl.Entry[1].DwellAddress.SymName[0]='\0';

      /* */
      *TblPtr = (void *)&DwellTbl;
      return (CFE_TBL_INFO_UPDATED);
   }

   else if  (Count == 3)
   {
      DwellTbl.Enabled = 1;  
      DwellTbl.Entry[0].Length = 2;
      DwellTbl.Entry[0].Delay = 1;
      DwellTbl.Entry[0].DwellAddress.Offset = (uint32)&Num1;
      strcpy (DwellTbl.Entry[0].DwellAddress.SymName, "");
      DwellTbl.Entry[1].Length = 0;
      DwellTbl.Entry[1].Delay = 0;
      DwellTbl.Entry[1].DwellAddress.Offset = 0;
   /*   strcpy (DwellTbl.Entry[1].DwellAddress.SymName, ""); */
      DwellTbl.Entry[1].DwellAddress.SymName[0]='\0';

      /* */
      *TblPtr = (void *)&DwellTbl;
      return (CFE_TBL_INFO_UPDATED);
   }
   else if (Count == 2)
   {

      *TblPtr = (void *)0;
      return (CFE_TBL_ERR_NO_ACCESS);
   }
/* Recover valid tables */
   else 
   /* this will cover 4th call on initialization, as well as any */
   /* for any updates. */
   {

      /* Set TblPtr to point to a valid table */
      DwellTbl.Enabled = MD_DWELL_STREAM_DISABLED;
      DwellTbl.Entry[0].Length = 2;
      DwellTbl.Entry[0].Delay = 1;
      DwellTbl.Entry[0].DwellAddress.Offset = (uint32)&Num1;
      strcpy (DwellTbl.Entry[0].DwellAddress.SymName, "");
      DwellTbl.Entry[1].Length = 0;
      DwellTbl.Entry[1].Delay = 0;
      DwellTbl.Entry[1].DwellAddress.Offset = 0;
      strcpy (DwellTbl.Entry[1].DwellAddress.SymName, "");

      /* */
      *TblPtr = (void *)&DwellTbl;
      return (CFE_TBL_INFO_UPDATED);
   }
}

int main(void)
{	
    int32 MemDwellAppId = 5;
    char MemDwellAppName[10];
	
    /********************************/
    /* Set up input file           */
    /********************************/
    UTF_add_input_file(CFS_MD_CMD_PIPE, "md_unit_test3.in");
    MD_AppData.CmdPipe = CFS_MD_CMD_PIPE;

    /********************************/
    /* Set up output file           */
    /********************************/
    UTF_set_output_filename("md_unit_test3.out");
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
    UTF_init_sim_time(1000.00);


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
    


/*    UTF_put_text("\n***Register local task with ES***\n"); */
/*    CFE_ES_RegisterApp(); */ /* Register local task with ES  */
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

 
   /**************************************************/
   /* Start Memory Dwell application                 */
   /**************************************************/

    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, (void *) &CFE_TBL_Register_FunctionHook);

    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, (void *) &CFE_TBL_GetAddress_FunctionHook); 

   /**************************************************/
   /* Start Memory Dwell application                 */
   /**************************************************/
        
   UTF_put_text("\n*** Start Memory Dwell Main Task ***\n");
   MD_AppMain();

   /**************************************************/
   /* Here we've reached the end of input file processing */
   /**************************************************/

    /********************************************************/
    /* Simulate pipe read error from Software Bus Services. */
    /********************************************************/
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RUNLOOP_PROC, TRUE );
;    UTF_ES_InitTaskRecords();
     UTF_ES_DumpAppRecords();
;    MemDwellAppId = UTF_ES_GetFreeTaskRecord();
    if (MemDwellAppId < 0)
    {
        UTF_put_text("\n***Error return from UTF_ES_GetFreeTaskRecord***\n");
        exit(0);
    }
;    strcpy(MemDwellAppName,"MD_APP");
    ;UTF_ES_AddAppRecord(MemDwellAppName,  (uint32)MemDwellAppId);	

    UTF_put_text("\n* * * * * * * * * * * * * * * * * * * * * * \n");
    
   /**************************************************/
   /* Test Table Services APIs                       */
   /**************************************************/
   UTF_CFE_TBL_DisplayTableRegistryContents();

   exit(0);
}
