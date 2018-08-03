/************************************************************************
** File:
**   $Id: ut_mm_test_loadwid.c 1.2 2015/03/02 14:26:53EST sstrege Exp  $
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
**   with interrupts disabled
**
**   $Log: ut_mm_test_loadwid.c  $
**   Revision 1.2 2015/03/02 14:26:53EST sstrege 
**   Added copyright information
**   Revision 1.1 2011/11/30 16:07:15EST jmdagost 
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

/*
** Use this address when you want CFE_PSP_MemValidateRange to fail since 
** it's not registered using the CFE_PSP_MemRangeSet calls below
*/
#define SIM_BAD_MEM_ADDR      0xD0000000  

/************************************************************************
** MM test data set
*************************************************************************/
extern uint8   MM_TestDataSet[1000];

/************************************************************************
** Local function prototypes
*************************************************************************/
void    Test_LoadWID(void);

extern void    PrintLocalHKVars(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Test load with interrupts disabled                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LoadWID(void)
{
   MM_LoadMemWIDCmd_t  CmdMsg;
   uint32              LocalCrc;
   
   /* Setup the test message header */ 
   CFE_SB_InitMsg(&CmdMsg, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
   
   /*
   ** Setup the load data array in the command message with the
   ** maximum amount of data allowed. 
   */
   memcpy(CmdMsg.DataArray, MM_TestDataSet, MM_MAX_UNINTERRUPTABLE_DATA);
   
   /* Compute the CRC value */
   LocalCrc = CFE_ES_CalculateCRC(MM_TestDataSet, MM_MAX_UNINTERRUPTABLE_DATA,
                                                       0, CFE_ES_DEFAULT_CRC);
  
   UTF_put_text("**********************************************\n");
   UTF_put_text("* Memory Load With Interrupts Disabled Tests *\n");
   UTF_put_text("**********************************************\n");
   UTF_put_text("\n");

   /*
   ** Test bad symbol name 
   */
   UTF_put_text("Test Load WID With Bad Symbol Name \n");
   UTF_put_text("-----------------------------------\n");
   strcpy(CmdMsg.DestSymAddress.SymName, "BadSymName");
   CmdMsg.DestSymAddress.Offset = 0;
   CmdMsg.NumOfBytes            = MM_MAX_UNINTERRUPTABLE_DATA;
   CmdMsg.Crc                   = LocalCrc;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test bad address 
   */
   UTF_put_text("Test Load WID With Bad Address \n");
   UTF_put_text("-------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_BAD_MEM_ADDR;
   CmdMsg.NumOfBytes                = MM_MAX_UNINTERRUPTABLE_DATA;
   CmdMsg.Crc                       = LocalCrc;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test bad CRC value 
   */
   UTF_put_text("Test Load WID With Bad CRC value \n");
   UTF_put_text("---------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes                = MM_MAX_UNINTERRUPTABLE_DATA;
   CmdMsg.Crc                       = LocalCrc ^ 0xF0F0F0F0;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test data size that exceeds configuration limits
   */
   UTF_put_text("Test Load WID That Exceeds Configuration Limits \n");
   UTF_put_text("------------------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes                = 0xFF;
   CmdMsg.Crc                       = LocalCrc;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      

   /*
   ** Test zero byte load
   */
   UTF_put_text("Test Load WID With Zero Data Specified \n");
   UTF_put_text("---------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes                = 0;
   CmdMsg.Crc                       = 0;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
   /*
   ** Test valid load of maximum size
   */
   UTF_put_text("Test Valid Load WID of Maximum Size \n");
   UTF_put_text("------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes                = MM_MAX_UNINTERRUPTABLE_DATA;
   CmdMsg.Crc                       = LocalCrc;
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
  
   /*
   ** Test valid load less than maximum size
   */
   UTF_put_text("Test Valid Load WID Less Than Maximum Size \n");
   UTF_put_text("-------------------------------------------\n");
   CmdMsg.DestSymAddress.SymName[0] = '\0';
   CmdMsg.DestSymAddress.Offset     = SIM_RAM_MEM_ADDR;
   CmdMsg.NumOfBytes                = 100;
   CmdMsg.Crc = CFE_ES_CalculateCRC(MM_TestDataSet, 100, 0, 
                                       CFE_ES_DEFAULT_CRC);
   MM_LoadMemWIDCmd((CFE_SB_MsgPtr_t)&CmdMsg);
   PrintLocalHKVars();   
   UTF_put_text("\n");      
   
} /* end Test_LoadWID */

/**** end file: ut_mm_test_loadwid.c ****/
