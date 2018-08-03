/************************************************************************
** File:
**   $Id: ut_mm_test_loadfromfile.c 1.3 2015/03/02 14:27:08EST sstrege Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   This is a test driver subroutine to test CFS Memory Manager (MM) load
**   from file
**
**   $Log: ut_mm_test_loadfromfile.c  $
**   Revision 1.3 2015/03/02 14:27:08EST sstrege 
**   Added copyright information
**   Revision 1.2 2011/12/05 15:17:58EST jmdagost 
**   Updates for zero-bytes read on file load.
**   Revision 1.1 2011/11/30 16:07:12EST jmdagost 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/unit_test/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "mm_app.h"            /* Application headers */
#include "mm_filedefs.h"
#include "mm_load.h"
#include "mm_dump.h"
#include "mm_msgids.h"

#include "cfe_es_cds.h"        /* cFE headers         */

#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_cfe.h"
#include "utf_osloader.h"
#include "utf_osfileapi.h"

#include <sys/fcntl.h>         /* System headers      */
#include <unistd.h>
#include <stdlib.h>

/************************************************************************
** Macro Definitions
*************************************************************************/
#define SIM_RAM_MEM_ADDR      0xB0000000  /* UTF Simulated RAM Memory Address */   

#define SIM_EEPROM_MEM_ADDR   0xC0000000  /* UTF Simulated EEPROM Memory Address */   

/*
** Use this address when you want CFE_PSP_MemValidateRange to fail since 
** it's not registered using the CFE_PSP_MemRangeSet calls below
*/
#define SIM_BAD_MEM_ADDR      0xD0000000  

/************************************************************************
** MM test data set
*************************************************************************/
extern uint8   MM_TestDataSet[1000];

/* Test data set CRC, this gets calculated in the main routine */
extern uint32   MM_TestDataSetCRC;

/************************************************************************
** Global variables used by function hooks
*************************************************************************/
extern uint32   OS_ReadHookCallCount;
extern uint32   OS_ReadHookFailCount;

/************************************************************************
** Local function prototypes
*************************************************************************/
void    Test_LoadFromFile(void);

extern boolean CreateTruncCFEHdrLoadFile(char FileName[]); 

extern boolean CreateTruncMMHdrLoadFile(char                     FileName[], 
                                 MM_LoadDumpFileHeader_t *MMFileHdr);

extern boolean CreateLoadFile(char                     FileName[], 
                       MM_LoadDumpFileHeader_t *MMFileHdr,
                       int32                    DataSetBytes);

extern boolean CreateBigLoadFile(char                     FileName[], 
                          MM_LoadDumpFileHeader_t *MMFileHdr,
                          int32                    NumDataSets);

extern void    PrintLocalHKVars(void);

/*
** Prototypes for function hooks
*/
extern int32   OS_ReadFailonN_Hook   (uint32 filedes, void* buffer, uint32 nbytes);
extern int32   OS_ReadZero_Hook      (uint32 filedes, void* buffer, uint32 nbytes);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test memory load from file processing                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LoadFromFile(void)
{
   MM_LoadMemFromFileCmd_t  CmdMsg;
   MM_LoadDumpFileHeader_t  FileHdr;
   boolean RetStatus;
   
   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
   
   UTF_put_text("*******************************\n");
   UTF_put_text("* Memory Load From File Tests *\n");
   UTF_put_text("*******************************\n");
   UTF_put_text("\n");

   /*
   ** Test no load filename specified
   */ 
   UTF_put_text("Test NUL Filename \n");
   UTF_put_text("------------------\n");
   CmdMsg.FileName[0] = '\0';
   MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);   
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test invalid load filename
   */ 
   UTF_put_text("Test Invalid Filename \n");
   UTF_put_text("----------------------\n");
   strcpy(CmdMsg.FileName, "/ram/Bad*(Filename).load");
   MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);   
   PrintLocalHKVars();   
   UTF_put_text("\n");      
      
   /*
   ** Test nonexistent load file
   */ 
   UTF_put_text("Test Nonexistent File \n");
   UTF_put_text("----------------------\n");
   strcpy(CmdMsg.FileName, "/ram/not_there.load");
   MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);   
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test load file with short cFE primary file header
   */ 
   RetStatus = CreateTruncCFEHdrLoadFile("ram/test1.load");
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Truncated cFE File Header \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test1.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test load file with short MM secondary file header
   */ 
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateTruncMMHdrLoadFile("ram/test2.load", &FileHdr);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Truncated MM File Header \n");
      UTF_put_text("------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test2.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test load file with too little load data
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test3.load", &FileHdr, (sizeof(MM_TestDataSet) / 2));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File With Too Little Data \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test3.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test load file with too much load data
   */ 
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = (sizeof(MM_TestDataSet) / 2);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test4.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File With Too Much Data \n");
      UTF_put_text("-----------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test4.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load file with bad CRC
   */ 
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC ^ 0xF0F0F0F0;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test5.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File With Bad CRC \n");
      UTF_put_text("-----------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test5.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test load file with bad symbol name 
   */
   strcpy(FileHdr.SymAddress.SymName, "BadSymName");
   FileHdr.SymAddress.Offset = 0;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test6.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File With Bad Symbol Name \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test6.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test RAM load that exceeds configuration limits 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateBigLoadFile("ram/test7.load", &FileHdr, 2048);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File That Exceeds RAM Configuration Limits \n");
      UTF_put_text("------------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test7.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
  
   /*
   ** Test EEPROM load that exceeds configuration limits 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   FileHdr.MemType           = MM_EEPROM;

   RetStatus = CreateBigLoadFile("ram/test8.load", &FileHdr, 512);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File That Exceeds EEPROM Configuration Limits \n");
      UTF_put_text("---------------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test8.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM32 load that exceeds configuration limits 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_MEM32;

   RetStatus = CreateBigLoadFile("ram/test9.load", &FileHdr, 2048);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File That Exceeds MEM32 Configuration Limits \n");
      UTF_put_text("--------------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test9.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM16 load that exceeds configuration limits 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_MEM16;

   RetStatus = CreateBigLoadFile("ram/test10.load", &FileHdr, 2048);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File That Exceeds MEM16 Configuration Limits \n");
      UTF_put_text("--------------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test10.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test MEM8 load that exceeds configuration limits 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_MEM8;

   RetStatus = CreateBigLoadFile("ram/test11.load", &FileHdr, 2048);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File That Exceeds MEM8 Configuration Limits \n");
      UTF_put_text("-------------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test11.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM32 load with misaligned address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM32;
   
   RetStatus = CreateLoadFile("ram/test12.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM32 File With Misaligned Address \n");
      UTF_put_text("----------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test12.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test MEM32 load with misaligned data size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_MEM32;
   FileHdr.NumOfBytes        = 503;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 503, 0,
                                                      CFE_ES_DEFAULT_CRC);
   
   RetStatus = CreateLoadFile("ram/test13.load", &FileHdr, 503);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM32 File With Misaligned Data Size \n");
      UTF_put_text("------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test13.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM16 load with misaligned address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR + 3;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM16;

   RetStatus = CreateLoadFile("ram/test14.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM16 File With Misaligned Address \n");
      UTF_put_text("----------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test14.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test MEM16 load with misaligned data size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.MemType           = MM_MEM16;
   FileHdr.NumOfBytes        = 503;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 503, 0,
                                                      CFE_ES_DEFAULT_CRC);

   RetStatus = CreateLoadFile("ram/test15.load", &FileHdr, 503);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM16 File With Misaligned Data Size \n");
      UTF_put_text("------------------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test15.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load file with an undefined memory type 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = 0xFF;

   RetStatus = CreateLoadFile("ram/test16.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test File With Invalid Memory Type \n");
      UTF_put_text("-----------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test16.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test RAM load with bad address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_BAD_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test17.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test RAM File With Bad Address \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test17.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test EEPROM load with bad address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_BAD_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_EEPROM;

   RetStatus = CreateLoadFile("ram/test18.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test EEPROM File With Bad Address \n");
      UTF_put_text("----------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test18.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM32 load with bad address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_BAD_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM32;

   RetStatus = CreateLoadFile("ram/test19.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM32 File With Bad Address \n");
      UTF_put_text("---------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test19.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test MEM16 load with bad address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_BAD_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM16;

   RetStatus = CreateLoadFile("ram/test20.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM16 File With Bad Address \n");
      UTF_put_text("---------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test20.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
 
   /*
   ** Test MEM8 load with bad address 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_BAD_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM8;

   RetStatus = CreateLoadFile("ram/test21.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test MEM8 File With Bad Address \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test21.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /************************************************************************
   ** The following tests force return codes from OSAL file I/O routines
   ** to increase code coverage. 
   *************************************************************************/
   /*
   ** Test RAM load with OS_close error
   ** Forces error in MM_LoadMemFromFileCmd routine
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test22.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_OSFILEAPI_Set_Api_Return_Code(OS_CLOSE_PROC, OS_FS_ERROR);
      UTF_put_text("Test RAM Load With OS_close error \n");
      UTF_put_text("----------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test22.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");
      UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CLOSE_PROC);
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Use same load file and test RAM load with OS_stat error
   ** Forces error in MM_VerifyLoadFileSize routine
   */
   UTF_OSFILEAPI_Set_Api_Return_Code(OS_STAT_PROC, OS_FS_ERROR);
   UTF_put_text("Test RAM Load With OS_stat error \n");
   UTF_put_text("---------------------------------\n");
   MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");
   UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_STAT_PROC);
   
   /*
   ** Use same load file and test RAM load with OS_read error
   ** on the 3rd call.
   ** Forces error return from CFS_ComputeCRCFromFile routine
   */
   OS_ReadHookCallCount = 1;   /* Always need to init this */
   OS_ReadHookFailCount = 3;
   UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadFailonN_Hook);
   UTF_put_text("Test RAM Load With OS_read on call 3 error \n");
   UTF_put_text("-------------------------------------------\n");
   MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");
   UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    
    /*
     ** Use same load file and test RAM load with OS_read error
     ** on the 9th call.
     ** Forces error return from MM_LoadMemFromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadFailonN_Hook);
    UTF_put_text("Test RAM Load With OS_read on call 9 error \n");
    UTF_put_text("-------------------------------------------\n");
    MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
    PrintLocalHKVars();   
    UTF_put_text("\n");
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    
    /*
     ** Use same load file and test MEM8 load with OS_read error
     ** on the 9th call.
     ** Forces error return from MM_LoadMem8FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM8;
    RetStatus = CreateLoadFile("ram/test22-8.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
        UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadFailonN_Hook);
        strcpy(CmdMsg.FileName, "/ram/test22-8.load");
        UTF_put_text("Test MEM8 Load With OS_read on call 9 error \n");
        UTF_put_text("-------------------------------------------\n");
        MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
        PrintLocalHKVars();   
        UTF_put_text("\n");
        UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Use same load file and test MEM16 load with OS_read error
     ** on the 9th call.
     ** Forces error return from MM_LoadMem16FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM16;
    RetStatus = CreateLoadFile("ram/test22-16.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadFailonN_Hook);
       UTF_put_text("Test MEM16 Load With OS_read on call 9 error \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-16.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Use same load file and test MEM32 load with OS_read error
     ** on the 9th call.
     ** Forces error return from MM_LoadMem32FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM32;
    RetStatus = CreateLoadFile("ram/test22-32.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadFailonN_Hook);
       UTF_put_text("Test MEM32 Load With OS_read on call 9 error \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-32.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Test RAM load with zero bytes read from OS_read
     ** Forces error return from MM_LoadMemFromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_RAM;
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadZero_Hook);
    RetStatus = CreateLoadFile("ram/test22-0.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_put_text("Test RAM Load With OS_read of zero bytes \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-0.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Test MEM32 load with zero bytes read from OS_read
     ** Forces error return from MM_LoadMem32FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM32;
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadZero_Hook);
    RetStatus = CreateLoadFile("ram/test22-32-0.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_put_text("Test MEM32 Load With OS_read of zero bytes \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-32-0.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Test MEM16 load with zero bytes read from OS_read
     ** Forces error return from MM_LoadMem16FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM16;
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadZero_Hook);
    RetStatus = CreateLoadFile("ram/test22-16-0.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_put_text("Test MEM16 Load With OS_read of zero bytes \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-16-0.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
    /*
     ** Test MEM8 load with zero bytes read from OS_read
     ** Forces error return from MM_LoadMem8FromFile routine
     */
    OS_ReadHookCallCount = 1;   /* Always need to init this */
    OS_ReadHookFailCount = 9;
    FileHdr.MemType      = MM_MEM8;
    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, OS_ReadZero_Hook);
    RetStatus = CreateLoadFile("ram/test22-8-0.load", &FileHdr, sizeof(MM_TestDataSet));
    if (RetStatus == TRUE)
    {
       UTF_put_text("Test MEM8 Load With OS_read of zero bytes \n");
       UTF_put_text("-------------------------------------------\n");
       strcpy(CmdMsg.FileName, "/ram/test22-8-0.load");
       MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
       PrintLocalHKVars();   
       UTF_put_text("\n");
       UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, NULL);
    }
    else /* abort these tests on any system error */
        return;
    
   /************************************************************************
   ** End OSAL file I/O error return tests
   *************************************************************************/
   
   /*
   ** Test load to RAM larger than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test23.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Large Valid Load To RAM \n");
      UTF_put_text("-----------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test23.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to EEPROM larger than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_EEPROM;

   RetStatus = CreateLoadFile("ram/test24.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Large Valid Load To EEPROM \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test24.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM32 larger than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM32;

   RetStatus = CreateLoadFile("ram/test25.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Large Valid Load To MEM32 \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test25.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM16 larger than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM16;

   RetStatus = CreateLoadFile("ram/test26.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Large Valid Load To MEM16 \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test26.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM8 larger than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = sizeof(MM_TestDataSet);
   FileHdr.Crc               = MM_TestDataSetCRC;
   FileHdr.MemType           = MM_MEM8;

   RetStatus = CreateLoadFile("ram/test27.load", &FileHdr, sizeof(MM_TestDataSet));
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Large Valid Load To MEM8 \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test27.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
   /*
   ** Test load to RAM smaller than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = 100;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0,
                                                      CFE_ES_DEFAULT_CRC);
   FileHdr.MemType           = MM_RAM;

   RetStatus = CreateLoadFile("ram/test28.load", &FileHdr, 100);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Small Valid Load To RAM \n");
      UTF_put_text("-----------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test28.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to EEPROM smaller than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_EEPROM_MEM_ADDR;
   FileHdr.NumOfBytes        = 100;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0,
                                                      CFE_ES_DEFAULT_CRC);
   FileHdr.MemType           = MM_EEPROM;

   RetStatus = CreateLoadFile("ram/test29.load", &FileHdr, 100);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Small Valid Load To EEPROM \n");
      UTF_put_text("--------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test29.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM32 smaller than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = 100;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0,
                                                      CFE_ES_DEFAULT_CRC);
   FileHdr.MemType           = MM_MEM32;

   RetStatus = CreateLoadFile("ram/test30.load", &FileHdr, 100);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Small Valid Load To MEM32 \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test30.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM16 smaller than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = 100;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0,
                                                      CFE_ES_DEFAULT_CRC);
   FileHdr.MemType           = MM_MEM16;

   RetStatus = CreateLoadFile("ram/test31.load", &FileHdr, 100);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Small Valid Load To MEM16 \n");
      UTF_put_text("-------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test31.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;

   /*
   ** Test load to MEM8 smaller than a segment size 
   */
   FileHdr.SymAddress.SymName[0] = '\0';
   FileHdr.SymAddress.Offset = SIM_RAM_MEM_ADDR;
   FileHdr.NumOfBytes        = 100;
   FileHdr.Crc               = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0,
                                                      CFE_ES_DEFAULT_CRC);
   FileHdr.MemType           = MM_MEM8;

   RetStatus = CreateLoadFile("ram/test32.load", &FileHdr, 100);
   if(RetStatus == TRUE)
   {
      UTF_put_text("Test Small Valid Load To MEM8 \n");
      UTF_put_text("------------------------------\n");
      strcpy(CmdMsg.FileName, "/ram/test32.load");
      MM_LoadMemFromFileCmd((CFE_SB_MsgPtr_t)&CmdMsg);
      PrintLocalHKVars();   
      UTF_put_text("\n");      
   }
   else /* abort these tests on any system error */
      return;
   
} /* end Test_LoadFromFile */

/**** end file: ut_mm_test_loadfromfile.c ****/
