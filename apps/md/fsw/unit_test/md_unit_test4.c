/* File: md_unit_test4.c
**  $Id: md_unit_test4.c 1.4 2012/01/09 19:28:14EST aschoeni Exp  $
**
** Purpose: 
**   Test driver #4 for unit testing of CFS Memory Dwell Application.
**   Includes test cases which load dwell tables.
**
** References:
**
** Assumptions and Notes:
* 
* Before running this test, subdirectory /tmp/ramdev0/ must exist.
*
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
extern CFE_TBL_TaskData_t CFE_TBL_TaskData;

#define CFS_MD_CMD_PIPE 1

unsigned short int msg_queue_id = 1;



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
    uint32 data1;
    uint32 data2;
    data1 = 0x1337d00d;
    data2 = 0x3475BEEF;

    UTF_add_sim_address (0x01234000, 16, "MEMORY_BANK" );
    CFE_PSP_MemRangeSet(0, CFE_PSP_MEM_RAM, 0x01234000,
                     16,  CFE_PSP_MEM_SIZE_BYTE,  CFE_PSP_MEM_ATTR_READ);

    UTF_write_sim_address(0x01234000, 4, &data1);
    UTF_write_sim_address(0x01234004, 4, &data2);
    UTF_write_sim_address(0x01234008, 4, &data1);
    UTF_write_sim_address(0x0123400C, 4, &data2);
}

void UTF_SCRIPT_DisplayTableRegistry ( int argc, char *argv[])
{
    UTF_CFE_TBL_DisplayTableRegistryContents ();
}

void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[])
{
    int debug = 1;
    int32 status;
    char Table_Name[30], File_Name[50];
    if (argc != 3)
    {
       UTF_error("Error: Read %d args w/script cmd LOAD_TABLE_FROM_GROUND. Expected 2.\n",
	   argc -1 );
       UTF_exit();
    }

   strcpy(Table_Name,argv[1]);
   strcpy(File_Name,argv[2]);
   if (debug) 
      UTF_put_text("UTF_SCRIPT_LoadTableFromGround called for Table_Name = '%s', File_Name = '%s'\n",
                   Table_Name, File_Name); 
   status = UTF_TBL_LoadTableFromGround(Table_Name, File_Name);
   if (debug) UTF_put_text("UTF_TBL_LoadTableFromGround returned %d", status);
   return;
}


int main(void)
{	
    int32 MemDwellAppId = 5;
    char MemDwellAppName[10];
	
    /********************************/
    /* Set up input file           */
    /********************************/
    UTF_add_input_file(CFS_MD_CMD_PIPE, "md_unit_test4.in");
    MD_AppData.CmdPipe = CFS_MD_CMD_PIPE;

    /********************************/
    /* Set up output file           */
    /********************************/
    UTF_set_output_filename("md_unit_test4.out");
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
    UTF_init_sim_time(1.5);

   /**************************************************/
   /*  Add volume to hold loadfiles                  */
   /**************************************************/
#if MD_SIGNATURE_OPTION == 1
    UTF_add_volume("/", "ram",  FS_BASED, FALSE, FALSE, TRUE, "RAM",  "/ram", 0);
#else
    UTF_add_volume("/", "ramnosig",  FS_BASED, FALSE, FALSE, TRUE, "RAM",  "/ram", 0);
#endif
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
    strcpy(MemDwellAppName,"MD");
    UTF_ES_AddAppRecord(MemDwellAppName,  MemDwellAppId);	

    UTF_ES_DumpAppRecords();
    /**************************************************/
    /* Add "Special" Commands                         */
    /**************************************************/
    UTF_add_special_command("SET_MEM_RANGE_FALSE",  UTF_SetMemRangeError); 
    UTF_add_special_command("SET_MEM_RANGE_VALID",  UTF_SetMemRangeValid); 
    UTF_add_special_command("LOAD_TABLE_FROM_GROUND", UTF_SCRIPT_LoadTableFromGround);
    UTF_add_special_command("DISPLAY_TABLE_REGISTRY", UTF_SCRIPT_DisplayTableRegistry);
    UTF_add_special_command("ADD_SIM_MEMORY",  UTF_AddSimulatedMemory);
    UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
    UTF_add_special_command("USE_DEFAULT_SB_RETURN_CODE", UTF_SCRIPT_SB_Use_Default_Api_Return_Code);
    UTF_add_special_command("SET_TBL_RETURN_CODE", UTF_SCRIPT_TBL_Set_Api_Return_Code);
    UTF_add_special_command("USE_DEFAULT_TBL_RETURN_CODE", UTF_SCRIPT_TBL_Use_Default_Api_Return_Code);

 

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
