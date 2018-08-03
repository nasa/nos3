/************************************************************************
** File:
**   $Id: lc_utest.c 1.2.1.2 2015/03/04 16:15:45EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   This is a test driver used to unit test the CFS Limit Checker (LC)
**   Application.
**
**   The CFS Limit Checker (LC) is a table driven application
**   that provides telemetry monitoring and autonomous response 
**   capabilities to Core Flight Executive (cFE) based systems. 
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
**   $Log: lc_utest.c  $
**   Revision 1.2.1.2 2015/03/04 16:15:45EST sstrege 
**   Added copyright information
**   Revision 1.2.1.1 2012/10/01 18:45:04EDT lwalling 
**   Apply revision 1.3 and 1.4 changes to branch
**   Revision 1.4 2012/10/01 13:30:46PDT lwalling 
**   Update LC unit test driver for LCX
**   Revision 1.3 2012/10/01 08:01:00PDT lwalling 
**   Updated LCX unit tests, modified old LC tests and added new LCX tests
**   Revision 1.2 2012/09/17 13:40:24PDT lwalling 
**   Removed references to Test_AcquirePointers(), modified sample AP cmd args
**   Revision 1.1 2012/07/31 13:53:43PDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/unit_test/project.pj
**   Revision 1.2 2009/12/28 14:19:06EST lwalling 
**   Update unit tests per latest version of CFE tools
**   Revision 1.1 2009/01/15 15:24:03EST dahardis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/unit_test/project.pj
** 
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "lc_app.h"            /* Application headers */
#include "lc_msgids.h"
#include "lc_cmds.h"
#include "lc_watch.h"
#include "lc_action.h"

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

/* 
** Defined locally in lc_app.c, but we need them here
*/
#define LC_CDS_SAVED            0xF0F0
#define LC_CDS_NOT_SAVED        0x0F0F

/************************************************************************
** LC global data external to this file
*************************************************************************/
extern  LC_AppData_t    LC_AppData;   /* App data that can be saved to CDS   */
extern  LC_OperData_t   LC_OperData;  /* Operational data that's never saved */

/* 
** Actionpoint and Watchpoint Definition Tables
** declared in lc_utest.adt.c and lc_utest.wdt.c 
*/
extern  LC_ADTEntry_t   LC_UnitTestADT[LC_MAX_ACTIONPOINTS];
extern  LC_WDTEntry_t   LC_UnitTestWDT[LC_MAX_WATCHPOINTS];

/*
** Non-public functions in LC that we need prototypes for so we
** can call them
*/
int32   LC_AppInit(void);
int32   LC_CreateDefinitionTables(void);
int32   LC_CreateResultTables(void);
uint8   LC_EvaluateRPN(uint16 APNumber);
void    LC_ExitApp(void);
uint8   LC_FloatCompare(uint16 WatchIndex, LC_MultiType_t WPMultiType, LC_MultiType_t CompareMultiType);
boolean LC_GetSizedWPData(uint16 WatchIndex, uint8  *WPDataPtr, uint32 *SizedDataPtr);
int32   LC_HousekeepingReq(CFE_SB_MsgPtr_t MessagePtr);
int32   LC_LoadDefaultTables(void);
int32   LC_ManageTables(void);
uint8   LC_OperatorCompare(uint16 WatchIndex, uint32 ProcessedWPData);
void    LC_SampleAPReq(CFE_SB_MsgPtr_t MessagePtr);
void    LC_SampleSingleAP(uint16 APNumber);
void    LC_SetAPPermOffCmd(CFE_SB_MsgPtr_t MessagePtr);
void    LC_SetAPStateCmd(CFE_SB_MsgPtr_t MessagePtr);
int32   LC_TableInit(void);

/************************************************************************
** Local data
*************************************************************************/

typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint32  Data[10];    

} LC_BigCmdPacket_t;

typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];

    uint32  Data1;    
    uint32  Data2;
    uint32  Data3;    

} LC_TestPacket1_t;

typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];

    uint32  Data1;    
    uint32  Data2;
    float   Data3;    

} LC_TestPacket2_t;

LC_TestPacket1_t    LC_TestPacket1;
LC_TestPacket2_t    LC_TestPacket2;

LC_BigCmdPacket_t LC_BigCmdPacket;
LC_NoArgsCmd_t    LC_NoArgsCmd;
LC_SetLCState_t   LC_SetLCState;
LC_SetAPState_t   LC_SetAPState;
LC_SetAPPermOff_t LC_SetAPPermOff;
LC_ResetAPStats_t LC_ResetAPStats;
LC_ResetWPStats_t LC_ResetWPStats;
LC_SampleAP_t     LC_SampleAP;

/*
** Actionpoint and Watchpoint Result Tables
*/
LC_ARTEntry_t    LC_UnitTestART[LC_MAX_ACTIONPOINTS];
LC_WRTEntry_t    LC_UnitTestWRT[LC_MAX_WATCHPOINTS];

/*
** Global variables used by function hooks
*/
uint32   CFE_SB_SubscribeCallCount     = 1;
uint32   CFE_SB_SubscribeFailCount     = 1;
uint32   CFE_TBL_RegisterCallCount     = 1;
uint32   CFE_TBL_RegisterFailCount     = 1;
uint32   CFE_TBL_LoadCallCount         = 1;
uint32   CFE_TBL_LoadFailCount         = 1;
uint32   CFE_TBL_GetAddressCallCount   = 1; 
uint32   CFE_TBL_GetAddressFailCount   = 1;
uint32   CFE_TBL_GetAddressUpdateCount = 1;

/************************************************************************
** Local function prototypes
*************************************************************************/
void  Test_LCInit        (void);
void  Test_LCInitFromCDS (void);
void  Test_LCExitApp     (void);
void  Test_LCInitNoCDS   (void);
void  Test_LCCmdPipe     (void);
void  Test_HKRequest     (void);
void  Test_ValidateWDT   (void);
void  Test_ValidateADT   (void);
void  Test_SetAPState    (void);
void  Test_SetAPOff      (void);
void  Test_SampleAP      (void);
void  Test_WPProcessing  (void);
void  Test_CmdHandlers   (void);
void  Test_Coverage      (void);
void  PrintHKPacket      (void);
void  PrintWRTEntry      (uint16 WatchIndex);
void  PrintARTEntry      (uint16 ActionIndex);

/*
** Prototypes for function hooks
*/
int32 CFE_SB_Subscribe_FailOnN (CFE_SB_MsgId_t  MsgId,
                                CFE_SB_PipeId_t PipeId);

int32 CFE_TBL_Register_FailOnN (CFE_TBL_Handle_t *TblHandlePtr,
                                const char *Name,
                                uint32  Size,
                                uint16  TblOptionFlags,
                                CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr);

int32 CFE_TBL_Register_Upd2 (CFE_TBL_Handle_t *TblHandlePtr,
                             const char *Name,
                             uint32  Size,
                             uint16  TblOptionFlags,
                             CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr);

int32 CFE_TBL_Load_FailOnN (CFE_TBL_Handle_t  TblHandle,
                            CFE_TBL_SrcEnum_t SrcType,
                            const void       *SrcDataPtr);

int32 CFE_TBL_GetAddress_FailOnN (void             **TblPtr,
                                  CFE_TBL_Handle_t TblHandle);

int32 CFE_TBL_GetAddress_UpdateOnN (void             **TblPtr,
                                    CFE_TBL_Handle_t TblHandle);

int32 CFE_TBL_GetAddress_Hook (void             **TblPtr,
                               CFE_TBL_Handle_t TblHandle);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* LC unit test program main                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
   /*
   ** Set up output file and HK packet handler           
   */
   UTF_set_output_filename("lc_utest.out");
   UTF_set_packet_handler(LC_HK_TLM_MID, (utf_packet_handler)PrintHKPacket);
    
   /*
   ** Initialize time data structures
   */
   UTF_init_sim_time(0.0);

   /*
   ** Initialize ES application data                         
   */
   UTF_ES_InitAppRecords();
   UTF_ES_AddAppRecord("LC",0);  
   CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
   
   /*
   ** Initialize CDS and table services data structures
   */
   CFE_ES_CDS_EarlyInit();
   CFE_TBL_EarlyInit();
   
   /*
   ** Run tests on LC initialization code
   */
   printf("*** LC UNIT TEST APPLICATION INIT TESTS START ***\n\n");
   
   Test_LCInit();
   
   printf("*** LC UNIT TEST APPLICATION INIT TESTS END   ***\n\n");
   
   /* 
   ** Test command processing through the software bus command pipe via
   ** the lc_utest.in command script
   */
   printf("*** LC UNIT TEST COMMAND PIPE TESTS START ***\n\n");
   
   Test_LCCmdPipe();
   
   printf("*** LC UNIT TEST COMMAND PIPE TESTS END   ***\n\n");
   
   /*
   ** Run tests that invoke LC code directly 
   */
   printf("*** LC UNIT TEST DRIVER TESTS START ***\n\n");

   LC_OperData.ARTPtr = LC_UnitTestART;
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

   printf("*** LC UNIT TEST DRIVER TESTS END   ***\n\n");
   
   return 0;
   
} /* end main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on LC initialization code                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LCInit (void)
{
    UTF_put_text("\n");
    UTF_put_text("************************************\n");
    UTF_put_text("* Application Initialization Tests *\n");
    UTF_put_text("************************************\n");
    UTF_put_text("\n");
    
    UTF_put_text("Test CFE_EVS_Register call error \n");
    UTF_put_text("---------------------------------\n");
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, 0xc2000003L);
    LC_AppMain();
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);
    UTF_put_text("\n");      
    
    UTF_put_text("Test CFE_SB_CreatePipe call error \n");
    UTF_put_text("---------------------------------\n");
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, 0xca000004L);
    LC_AppMain(); 
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_CREATEPIPE_PROC); 
    UTF_put_text("\n");      
 
    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, CFE_SB_Subscribe_FailOnN);   
    
    /*
    ** Test error return on first CFE_SB_Subscribe call
    */
    CFE_SB_SubscribeCallCount  = 1;   /* Always need to init this */
    CFE_SB_SubscribeFailCount  = 1;
    UTF_put_text("Test CFE_SB_Subscribe call 1 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Test error return on second CFE_SB_Subscribe call
    */
    CFE_SB_SubscribeCallCount  = 1;   /* Always need to init this */
    CFE_SB_SubscribeFailCount  = 2;
    UTF_put_text("Test CFE_SB_Subscribe call 2 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Test error return on third CFE_SB_Subscribe call
    */
    CFE_SB_SubscribeCallCount  = 1;   /* Always need to init this */
    CFE_SB_SubscribeFailCount  = 3;
    UTF_put_text("Test CFE_SB_Subscribe call 3 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Remove the custom function hook
    */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, NULL);

    /* 
    ** How we proceed will depend on if we are configured to
    ** restore data from CDS or not 
    */  
#ifdef LC_SAVE_TO_CDS
    Test_LCInitFromCDS();
    Test_LCExitApp();
#else    
    Test_LCInitNoCDS();
#endif     
    
} /* end Test_LCInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on LC initialization code that is conditionally      */
/* compiled in when LC is configured to attempt to restore         */
/* data from the CDS on application restarts                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LCInitFromCDS (void)
{
    UTF_put_text("\n");
    UTF_put_text("*********************************************\n");
    UTF_put_text("* Application Initialization With CDS Tests *\n");
    UTF_put_text("*********************************************\n");
    UTF_put_text("\n");

    /*
    ** Set all future calls to CFE_ES_CopyToCDS to return success
    ** We do this so the LC_ExitApp routine (that is included when
    ** we are using the CDS) will exit normally during all the
    ** subsequent tests that we run.
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_COPYTOCDS_PROC, CFE_SUCCESS);
    
    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, CFE_TBL_Register_FailOnN);   
  
    /*
    ** Test error return on first CFE_TBL_Register call
    ** Generates an error in the LC_InitFromCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 1;
    UTF_put_text("Test CFE_TBL_Register call 1 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
 
    /*
    ** Test error return on second CFE_TBL_Register call
    ** Generates an error in the LC_InitFromCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 2;
    UTF_put_text("Test CFE_TBL_Register call 2 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Test error return on third CFE_TBL_Register call
    ** Generates an error in the LC_InitFromCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 3;
    UTF_put_text("Test CFE_TBL_Register call 3 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Test error return on fourth CFE_TBL_Register call
    ** Generates an error in the LC_InitFromCDS function 
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 4;
    UTF_put_text("Test CFE_TBL_Register call 4 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Remove the custom function hook
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, NULL);

    /*
    ** Set any future CFE_TBL_Register calls to return success
    ** without actually registering the tables. This prevents
    ** the multiple calls to LC_AppMain below from blowing up
    ** the UTF table buffers
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, CFE_SUCCESS);
 
    /*
    ** Set calls to CFE_ES_RegisterCDS to return an error code
    ** NOTE: CFE_ES_RegisterCDS does NOT support function hooks,
    ** so we can only force an error on the first call. 
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCDS_PROC, CFE_ES_CDS_INVALID_NAME);
 
    /*
    ** Test error return on first CFE_ES_RegisterCDS call
    ** Generates an error in the LC_InitFromCDS function
    */
    UTF_put_text("Test CFE_ES_RegisterCDS call 1 error \n");
    UTF_put_text("-------------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
   
    /*
    ** Set any future CFE_ES_RegisterCDS calls to return success
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_REGISTERCDS_PROC, CFE_SUCCESS);
    
    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, CFE_TBL_GetAddress_FailOnN);   
 
    /*
    ** Test error return on first CFE_TBL_GetAddress call
    ** Generates an error in the LC_InitFromCDS function
    */
    CFE_TBL_GetAddressCallCount  = 1;   /* Always need to init this */
    CFE_TBL_GetAddressFailCount  = 1;
    UTF_put_text("Test CFE_TBL_GetAddress call 1 error \n");
    UTF_put_text("-------------------------------------\n");
    LC_AppMain(); 
    UTF_put_text("\n");
    
    /*
    ** Test error return on second CFE_TBL_GetAddress call
    ** Generates an error in the LC_InitFromCDS function 
    */
    CFE_TBL_GetAddressCallCount  = 1;   /* Always need to init this */
    CFE_TBL_GetAddressFailCount  = 2;
    UTF_put_text("Test CFE_TBL_GetAddress call 2 error \n");
    UTF_put_text("-------------------------------------\n");
    LC_AppMain(); 
    UTF_put_text("\n");
    
    /*
    ** Set a function hook for CFE_TBL_GetAddress that will return
    ** pointers to our local test data buffers 
    */
    UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, CFE_TBL_GetAddress_Hook);   
    
    /*
    ** Set CFE_TBL_ReleaseAddress to not do anything and
    ** always return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, CFE_SUCCESS);
    
    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, CFE_TBL_Load_FailOnN);   
 
    /*
    ** Test error return on first CFE_TBL_Load call
    ** Generates an error in the LC_LoadDefaults function
    */
    CFE_TBL_LoadCallCount  = 1;   /* Always need to init this */
    CFE_TBL_LoadFailCount  = 1;
    UTF_put_text("Test CFE_TBL_Load call 1 error \n");
    UTF_put_text("-------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Test error return on second CFE_TBL_Load call
    ** Generates an error in the LC_LoadDefaults function
    */
    CFE_TBL_LoadCallCount  = 1;   /* Always need to init this */
    CFE_TBL_LoadFailCount  = 2;
    UTF_put_text("Test CFE_TBL_Load call 2 error \n");
    UTF_put_text("-------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Remove the custom function hook
    */
    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, NULL);

    /*
    ** Set any future CFE_TBL_Load calls to return success
    ** (needed because we don't load table images from files
    **  for unit testing, we use memory buffers instead)
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_LOAD_PROC, CFE_SUCCESS);

    /*
    ** Setup a function hook for CFE_TBL_Register that will return
    ** CFE_TBL_INFO_RECOVERED_TBL on the first two times that it's
    ** called then return CFE_SUCCESS afterwards
    **
    ** This will make the LC code think the ADT and WDT were 
    ** restored from the CDS
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, CFE_TBL_Register_Upd2);      
    
    /*
    ** Set calls to CFE_ES_RestoreFromCDS to return an error code
    ** NOTE: CFE_ES_RestoreFromCDS does NOT support function hooks,
    ** so we can only force an error on the first call. 
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RESTOREFROMCDS_PROC, CFE_ES_CDS_INVALID);

    /*
    ** Set any future CFE_TBL_Manage calls to return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_MANAGE_PROC, CFE_SUCCESS);
   
    /*
    ** Set any call to CFE_TBL_GetStatus to return error
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETSTATUS_PROC, CFE_TBL_ERR_INVALID_HANDLE);
    
    /*
    ** Test error return on first CFE_ES_RestoreFromCDS call which will
    ** fall through to the LC_AcquirePointers where we need to force 
    ** an error return from CFE_TBL_GetStatus so control will return
    ** back to this function.
    */
    CFE_TBL_RegisterCallCount = 1;  /* Need to do this to get to CFE_ES_RestoreFromCDS call */
    UTF_put_text("Test CFE_ES_RestoreFromCDS call 1 error\n");
    UTF_put_text("AND  CFE_TBL_GetStatus call 1 error\n");  
    UTF_put_text("---------------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Set calls to CFE_ES_RestoreFromCDS to return success
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_RESTOREFROMCDS_PROC, CFE_SUCCESS);

    /*
    ** Rerun previous test 
    */
    CFE_TBL_RegisterCallCount = 1;  /* Need to do this to get to CFE_ES_RestoreFromCDS call */
    UTF_put_text("Test CFE_ES_RestoreFromCDS no error\n");
    UTF_put_text("AND  CFE_TBL_GetStatus call 1 error\n");  
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
  
    /*
    ** Set CDSSavedOnExit to execute alternate branch 
    */
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;
    CFE_TBL_RegisterCallCount = 1;  /* Need to do this to get to CFE_ES_RestoreFromCDS call */
    UTF_put_text("Test CFE_ES_RestoreFromCDS no error\n");
    UTF_put_text("AND  LC_AppData.CDSSavedOnExit = LC_CDS_SAVED\n");
    UTF_put_text("AND  CFE_TBL_GetStatus call 1 error\n");  
    UTF_put_text("---------------------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Set any future call to CFE_TBL_GetStatus to return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETSTATUS_PROC, CFE_SUCCESS);
    
} /* end Test_LCInitFromCDS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on LC exit code that is conditionally                */
/* compiled in when LC is configured to attempt to save            */
/* data to the CDS on application restarts                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifdef LC_SAVE_TO_CDS
void Test_LCExitApp (void)
{
    UTF_put_text("\n");
    UTF_put_text("***********************************\n");
    UTF_put_text("* Application Exit With CDS Tests *\n");
    UTF_put_text("***********************************\n");
    UTF_put_text("\n");

    /*
    ** In the current version of the UTF, CFE_ES_CopyToCDS
    ** doesn't support function hooks so the best we can
    ** do is test the first error condition in LC_ExitApp
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_COPYTOCDS_PROC, CFE_ES_CDS_ACCESS_ERROR);   
    
    UTF_put_text("Test CFE_ES_CopyToCDS call 1 error\n");
    UTF_put_text("----------------------------------\n");
    LC_ExitApp();    
    UTF_put_text("\n");
    
    /*
    ** Set all future calls to CFE_ES_CopyToCDS to return success
    ** so LC will exit normally during all the subsequent tests 
    ** that we run.
    */
    UTF_CFE_ES_Set_Api_Return_Code(CFE_ES_COPYTOCDS_PROC, CFE_SUCCESS);
    
} /* end Test_LCExitApp */
#endif /* end #ifdef LC_SAVE_TO_CDS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on LC initialization code that is conditionally      */
/* compiled in when LC is configured NOT to use the CDS on         */
/* application restarts                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LCInitNoCDS (void)
{
    UTF_put_text("\n");
    UTF_put_text("************************************************\n");
    UTF_put_text("* Application Initialization With NO CDS Tests *\n");
    UTF_put_text("************************************************\n");
    UTF_put_text("\n");

    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, CFE_TBL_Register_FailOnN);   
  
    /*
    ** Test error return on first CFE_TBL_Register call
    ** Generates an error in the LC_InitNoCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 1;
    UTF_put_text("Test CFE_TBL_Register call 1 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
 
    /*
    ** Test error return on second CFE_TBL_Register call
    ** Generates an error in the LC_InitNoCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 2;
    UTF_put_text("Test CFE_TBL_Register call 2 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Test error return on third CFE_TBL_Register call
    ** Generates an error in the LC_InitNoCDS function
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 3;
    UTF_put_text("Test CFE_TBL_Register call 3 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Test error return on fourth CFE_TBL_Register call
    ** Generates an error in the LC_InitNoCDS function 
    */
    CFE_TBL_RegisterCallCount  = 1;   /* Always need to init this */
    CFE_TBL_RegisterFailCount  = 4;
    UTF_put_text("Test CFE_TBL_Register call 4 error \n");
    UTF_put_text("-----------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Remove the custom function hook
    */
    UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, NULL);

    /*
    ** Set any future CFE_TBL_Register calls to return success
    ** without actually registering the tables. This prevents
    ** the multiple calls to LC_AppMain below from blowing up
    ** the UTF table buffers
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, CFE_SUCCESS);
    
    /*
    ** Set up a custom function hook for the following tests
    */
    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, CFE_TBL_Load_FailOnN);   
 
    /*
    ** Test error return on first CFE_TBL_Load call
    ** Generates an error in the LC_LoadDefaults function
    */
    CFE_TBL_LoadCallCount  = 1;   /* Always need to init this */
    CFE_TBL_LoadFailCount  = 1;
    UTF_put_text("Test CFE_TBL_Load call 1 error \n");
    UTF_put_text("-------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Test error return on second CFE_TBL_Load call
    ** Generates an error in the LC_LoadDefaults function
    */
    CFE_TBL_LoadCallCount  = 1;   /* Always need to init this */
    CFE_TBL_LoadFailCount  = 2;
    UTF_put_text("Test CFE_TBL_Load call 2 error \n");
    UTF_put_text("-------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");
    
    /*
    ** Remove the custom function hook
    */
    UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, NULL);

    /*
    ** Set any future CFE_TBL_Load calls to return success
    ** (needed because we don't load table images from files
    **  for unit testing, we use memory buffers instead)
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_LOAD_PROC, CFE_SUCCESS);

    /*
    ** Set any future CFE_TBL_Manage calls to return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_MANAGE_PROC, CFE_SUCCESS);
    
    /*
    ** Set any call to CFE_TBL_GetStatus to return error
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETSTATUS_PROC, CFE_TBL_ERR_INVALID_HANDLE);

    /*
    ** Test error return on first CFE_TBL_GetStatus call
    ** Generates an error in the beginning of the LC_AcquirePointers 
    ** function that's called at the end of the init sequence.
    **
    ** We test the rest of the LC_AcquirePointers code below.
    */
    UTF_put_text("Test CFE_TBL_GetStatus call 1 error \n");
    UTF_put_text("------------------------------------\n");
    LC_AppMain();    
    UTF_put_text("\n");

    /*
    ** Set any future call to CFE_TBL_GetStatus to return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETSTATUS_PROC, CFE_SUCCESS);
    
} /* end Test_LCInitNoCDS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs command pipe tests                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_LCCmdPipe (void)
{
    CFE_SB_PipeId_t  CmdPipe_ID;

    UTF_put_text("\n");
    UTF_put_text("*************************\n");
    UTF_put_text("* LC Command Pipe Tests *\n");
    UTF_put_text("*************************\n");
    UTF_put_text("\n");
    
    /*
    ** Re-Initialize ES application data otherwise our earlier
    ** tests will cause CFE_ES_RunLoop to not return TRUE and
    ** LC will exit right away
    */
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord("LC",0);  
    CFE_ES_RegisterApp();
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
    
    /*
    ** Set CFE_TBL_ReleaseAddress to not do anything and
    ** always return success
    */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, CFE_SUCCESS);
    
    /*
    ** Set up to read in script through command pipe
    ** The command pipe ID needs to match the value that will be
    ** returned by the next call to CFE_SB_CreatePipe. So we
    ** increment the last value
    */
    CmdPipe_ID = LC_OperData.CmdPipe + 1;
    UTF_add_input_file(CmdPipe_ID, "lc_utest.in");

    /*
    ** Add this hook so we can force a software bus read error
    ** in the lc_utest.in input file that will make the application 
    ** exit and return control to this function.
    */
    UTF_add_special_command("SET_SB_RETURN_CODE", UTF_SCRIPT_SB_Set_Api_Return_Code);
    
    /*
    ** Call app main
    */
    LC_AppMain();
    
} /* end Test_LCCmdPipe */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs housekeeping request tests                                 */
/* The primary purpose of these is to ensure that the Watchpoint   */
/* and Actionpoint results arrays in the housekeeping packet get   */
/* formatted correctly according to the values in the WP and AP    */
/* results tables                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_HKRequest (void)
{
    LC_NoArgsCmd_t  HKReqMsg;
    int32           Status;
    
    /* Setup the test message header */ 
    CFE_SB_InitMsg(&HKReqMsg, LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    
    UTF_put_text("\n");
    UTF_put_text("*********************************\n");
    UTF_put_text("* LC Housekeeping Request Tests *\n");
    UTF_put_text("*********************************\n");
    UTF_put_text("\n");
    
    /* Setup some dummy Watchpoint results 
    ** The numbers are chosen to execute all of the 
    ** case switch statements in LC_HousekeepingReq where the
    ** results arrays are constructed 
    */ 
    LC_OperData.WRTPtr [4].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr [8].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[12].WatchResult = LC_WATCH_ERROR;    
    LC_OperData.WRTPtr[16].WatchResult = LC_WATCH_STALE;    
    
    LC_OperData.WRTPtr [9].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[13].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[17].WatchResult = LC_WATCH_ERROR;    
    LC_OperData.WRTPtr[21].WatchResult = LC_WATCH_STALE;    

    LC_OperData.WRTPtr[14].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[18].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[22].WatchResult = LC_WATCH_ERROR;    
    LC_OperData.WRTPtr[26].WatchResult = LC_WATCH_STALE;    
    
    LC_OperData.WRTPtr[19].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[23].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[27].WatchResult = LC_WATCH_ERROR;    
    LC_OperData.WRTPtr[31].WatchResult = LC_WATCH_STALE;    

    /* Setup some dummy Actionpoint results */
    LC_OperData.ARTPtr [4].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr [6].CurrentState = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr [8].CurrentState = LC_APSTATE_DISABLED;
    LC_OperData.ARTPtr[10].CurrentState = LC_APSTATE_PERMOFF;    
    LC_OperData.ARTPtr[14].CurrentState = LC_ACTION_NOT_USED;
    
    LC_OperData.ARTPtr[13].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[15].CurrentState = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr[17].CurrentState = LC_APSTATE_DISABLED;
    LC_OperData.ARTPtr[19].CurrentState = LC_APSTATE_PERMOFF;    
    LC_OperData.ARTPtr[23].CurrentState = LC_ACTION_NOT_USED;
    
    LC_OperData.ARTPtr [22].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr [24].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ARTPtr [26].ActionResult = LC_ACTION_ERROR;
    LC_OperData.ARTPtr [30].ActionResult = LC_ACTION_STALE;

    LC_OperData.ARTPtr [29].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr [31].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ARTPtr [33].ActionResult = LC_ACTION_ERROR;
    LC_OperData.ARTPtr [37].ActionResult = LC_ACTION_STALE;
    
    /*
    ** Generate the packet which will cause the print routine
    ** to be invoked automatically
    */
    Status = LC_HousekeepingReq((CFE_SB_MsgPtr_t)&HKReqMsg);
    
    /*
    ** Reset our statistics
    */    
    LC_OperData.WRTPtr [4].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr [8].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[12].WatchResult = LC_WATCH_STALE;    
    
    LC_OperData.WRTPtr [9].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[13].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[17].WatchResult = LC_WATCH_STALE;    

    LC_OperData.WRTPtr[14].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[18].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[22].WatchResult = LC_WATCH_STALE;    
    
    LC_OperData.WRTPtr[19].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[23].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[27].WatchResult = LC_WATCH_STALE;    
    
    LC_OperData.ARTPtr [4].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr [6].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr [8].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr[10].CurrentState = LC_ACTION_NOT_USED;    
    
    LC_OperData.ARTPtr[13].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr[15].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr[17].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr[19].CurrentState = LC_ACTION_NOT_USED;    
    
    LC_OperData.ARTPtr [22].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr [24].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr [26].ActionResult = LC_ACTION_STALE;

    LC_OperData.ARTPtr [29].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr [31].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr [33].ActionResult = LC_ACTION_STALE;
    
} /* end Test_HKRequest */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs Watchpoint Definition Table (WDT) Validation Tests         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_ValidateWDT (void)
{
    int32 Status;
    
    UTF_put_text("\n");
    UTF_put_text("***************************\n");
    UTF_put_text("* LC WDT Validation Tests *\n");
    UTF_put_text("***************************\n");
    UTF_put_text("\n");
    
    UTF_put_text("Test WDT Validation with bad DataType\n");
    UTF_put_text("-------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType = 45;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");

    UTF_put_text("Test WDT Validation with bad OperatorID\n");
    UTF_put_text("---------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[20].OperatorID = 45;    
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");

    UTF_put_text("Test WDT Validation with bad MessageID\n");
    UTF_put_text("--------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[20].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[20].MessageID  = CFE_SB_HIGHEST_VALID_MSGID + 1;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");
     
    UTF_put_text("Test WDT Validation with floating point NAN comparison value\n");
    UTF_put_text("------------------------------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_FLOAT_LE;
    LC_OperData.WDTPtr[20].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[20].MessageID  = 1024;
    LC_OperData.WDTPtr[20].ComparisonValue.Unsigned32 = 0xFFFFFFFF;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");
 
    UTF_put_text("Test WDT Validation with floating point infinite comparison value\n");
    UTF_put_text("-----------------------------------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_FLOAT_LE;
    LC_OperData.WDTPtr[20].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[20].MessageID  = 1024;
    LC_OperData.WDTPtr[20].ComparisonValue.Unsigned32 = 0x7F800000;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");

    UTF_put_text("Test WDT Validation with good non-floating point entry\n");
    UTF_put_text("------------------------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[20].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[20].MessageID  = 1024;
    LC_OperData.WDTPtr[20].ComparisonValue.Unsigned16in32.Unsigned16 = 0xF0F0;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test WDT Validation with good floating point entry\n");
    UTF_put_text("--------------------------------------------------\n");
    LC_OperData.WDTPtr[20].DataType   = LC_DATA_FLOAT_LE;
    LC_OperData.WDTPtr[20].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[20].MessageID  = 1024;
    LC_OperData.WDTPtr[20].ComparisonValue.Float32 = 500.25;
    Status = LC_ValidateWDT(LC_OperData.WDTPtr);
    UTF_put_text("\n");
    
    /*
    ** Reset the watchpoint entry that we used
    */
    LC_OperData.WDTPtr[20].DataType                   = LC_WATCH_NOT_USED;
    LC_OperData.WDTPtr[20].OperatorID                 = LC_NO_OPER;
    LC_OperData.WDTPtr[20].MessageID                  = 0;
    LC_OperData.WDTPtr[20].WatchpointOffset           = 0;
    LC_OperData.WDTPtr[20].BitMask                    = LC_NO_BITMASK;
    LC_OperData.WDTPtr[20].CustomFuncArgument         = 0;
    LC_OperData.WDTPtr[20].ComparisonValue.Unsigned32 = 0;
    
} /* end Test_ValidateWDT */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs Actionpoint Definition Table (ADT) Validation Tests        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_ValidateADT (void)
{
    int32 Status;
    
    UTF_put_text("\n");
    UTF_put_text("***************************\n");
    UTF_put_text("* LC ADT Validation Tests *\n");
    UTF_put_text("***************************\n");
    UTF_put_text("\n");
 
    UTF_put_text("Test ADT Validation with bad DefaultState\n");
    UTF_put_text("-----------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState = 10;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with bad RTSId\n");
    UTF_put_text("----------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId        = LC_MAX_VALID_ADT_RTSID + 1;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with bad MaxFailsBeforeRTS\n");
    UTF_put_text("----------------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 0;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with bad EventType\n");
    UTF_put_text("--------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[30].EventType         = 10;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");

    UTF_put_text("Test ADT Validation with bad RPN expression\n");
    UTF_put_text("LC_RPN_NOT as the first symbol\n");
    UTF_put_text("-------------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[30].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[30].RPNEquation[0]    = LC_RPN_NOT;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with bad RPN expression\n");
    UTF_put_text("Missing watchpoint ID\n");
    UTF_put_text("-------------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[30].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[30].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[30].RPNEquation[1]    = LC_RPN_AND;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with bad RPN expression\n");
    UTF_put_text("Not a valid polish symbol or watchpoint ID\n");
    UTF_put_text("-------------------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[30].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[30].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[30].RPNEquation[1]    = 755;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");
    
    UTF_put_text("Test ADT Validation with good entry\n");
    UTF_put_text("-----------------------------------\n");
    LC_OperData.ADTPtr[30].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[30].RTSId             = 20;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[30].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[30].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[30].RPNEquation[1]    = 16;
    LC_OperData.ADTPtr[30].RPNEquation[2]    = LC_RPN_AND;
    LC_OperData.ADTPtr[30].RPNEquation[3]    = LC_RPN_EQUAL;
    Status = LC_ValidateADT(LC_OperData.ADTPtr);
    UTF_put_text("\n");

    /*
    ** Reset the actionpoint entry that we used
    */
    LC_OperData.ADTPtr[30].DefaultState      = LC_ACTION_NOT_USED;
    LC_OperData.ADTPtr[30].RTSId             = 0;
    LC_OperData.ADTPtr[30].MaxFailsBeforeRTS = 0;
    LC_OperData.ADTPtr[30].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[30].EventID           = 0;
    LC_OperData.ADTPtr[30].EventText[0]      = '\0';
    LC_OperData.ADTPtr[30].RPNEquation[0]    = 0;
    LC_OperData.ADTPtr[30].RPNEquation[1]    = LC_RPN_EQUAL;
    
} /* end Test_ValidateADT */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on code that handles the set actionpoint state       */
/* command                                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_SetAPState (void)
{
    LC_SetAPState_t  SetAPStateCmd;
    
    /* Setup the test message header */ 
    CFE_SB_InitMsg(&SetAPStateCmd, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    
    UTF_put_text("\n");
    UTF_put_text("**********************************\n");
    UTF_put_text("* LC Set Actionpoint State Tests *\n");
    UTF_put_text("**********************************\n");
    UTF_put_text("\n");
    
    UTF_put_text("Test set AP state command with invalid new state\n");
    UTF_put_text("------------------------------------------------\n");
    SetAPStateCmd.APNumber   = 100;
    SetAPStateCmd.NewAPState = 100;
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)&SetAPStateCmd);    
    UTF_put_text("\n");
   
    UTF_put_text("Test set AP state command with invalid AP number\n");
    UTF_put_text("------------------------------------------------\n");
    SetAPStateCmd.APNumber   = LC_MAX_ACTIONPOINTS + 1;
    SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)&SetAPStateCmd);    
    UTF_put_text("\n");
    
    UTF_put_text("Test set AP state command with invalid current AP state\n");
    UTF_put_text("-------------------------------------------------------\n");
    SetAPStateCmd.APNumber   = 100; /* Current state is LC_ACTION_NOT_USED */
    SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)&SetAPStateCmd);    
    UTF_put_text("\n");
   
    UTF_put_text("Test good set AP state command for a single AP\n");
    UTF_put_text("----------------------------------------------\n");
    LC_OperData.ARTPtr[100].CurrentState = LC_APSTATE_ACTIVE;
    SetAPStateCmd.APNumber   = 100;
    SetAPStateCmd.NewAPState = LC_APSTATE_PASSIVE;  
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)&SetAPStateCmd);    
    UTF_put_text("\n");

    UTF_put_text("Test good set AP state command for all APs\n");
    UTF_put_text("------------------------------------------\n");
    /* 
    ** Set all actionpoints ACTIVE, which will only set the one
    ** used previously since all others are unused
    */
    SetAPStateCmd.APNumber   = LC_ALL_ACTIONPOINTS;
    SetAPStateCmd.NewAPState = LC_APSTATE_ACTIVE;  
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)&SetAPStateCmd);    
    UTF_put_text("\n");
    
    /*
    ** Reset the actionpoint entry that we used
    */
    LC_OperData.ARTPtr[100].CurrentState = LC_ACTION_NOT_USED;
    
} /* end Test_SetAPState */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on code that handles the set actionpoint state       */
/* to permananetly off command                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_SetAPOff (void)
{
    LC_SetAPPermOff_t  SetAPOffCmd;
    
    /* Setup the test message header */ 
    CFE_SB_InitMsg(&SetAPOffCmd, LC_CMD_MID, sizeof(LC_SetAPPermOff_t), TRUE);
    
    UTF_put_text("\n");
    UTF_put_text("**********************************\n");
    UTF_put_text("* LC Set Actionpoint Off Tests *\n");
    UTF_put_text("**********************************\n");
    UTF_put_text("\n");
    
    UTF_put_text("Test set AP off command with invalid AP number\n");
    UTF_put_text("----------------------------------------------\n");
    SetAPOffCmd.APNumber   = LC_MAX_ACTIONPOINTS + 1;
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)&SetAPOffCmd);    
    UTF_put_text("\n");
    
    UTF_put_text("Test set AP off command with invalid current AP state\n");
    UTF_put_text("-----------------------------------------------------\n");
    SetAPOffCmd.APNumber   = 100; /* Current state is LC_ACTION_NOT_USED */
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)&SetAPOffCmd);    
    UTF_put_text("\n");
   
    UTF_put_text("Test good set AP off command\n");
    UTF_put_text("----------------------------\n");
    LC_OperData.ARTPtr[100].CurrentState = LC_APSTATE_DISABLED;
    SetAPOffCmd.APNumber   = 100;
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)&SetAPOffCmd);    
    UTF_put_text("\n");

    /*
    ** Reset the actionpoint entry that we used
    */
    LC_OperData.ARTPtr[100].CurrentState = LC_ACTION_NOT_USED;
    
} /* end Test_SetAPOff */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Runs tests on code that handles actionpoint sampling            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_SampleAP (void)
{
    LC_SampleAP_t  SampleAPMsg;
    
    /* Setup the test message header */ 
    CFE_SB_InitMsg(&SampleAPMsg, LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);
    
    UTF_put_text("\n");
    UTF_put_text("*******************************\n");
    UTF_put_text("* LC Sample Actionpoint Tests *\n");
    UTF_put_text("*******************************\n");
    UTF_put_text("\n");
    
    /*
    ** Set the LC application state to active
    */
    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;
    
    UTF_put_text("Test sample AP message with invalid AP number\n");
    UTF_put_text("---------------------------------------------\n");
    SampleAPMsg.StartIndex = LC_MAX_ACTIONPOINTS + 1;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
    
    UTF_put_text("Test sample single AP message with invalid current AP state\n");
    UTF_put_text("-----------------------------------------------------------\n");
    SampleAPMsg.StartIndex = 75; /* Current state is LC_ACTION_NOT_USED */
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");

    UTF_put_text("Test sample all APs message \n");
    UTF_put_text("----------------------------\n");
    /*
    ** This is just for code coverage since all APs are currently
    ** set to LC_ACTION_NOT_USED nothing will trigger
    */
    SampleAPMsg.StartIndex = LC_ALL_ACTIONPOINTS;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
 
    /* 
    ** Setup an actionpoint definition
    */ 
    LC_OperData.ADTPtr[75].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[75].RTSId             = 20;
    LC_OperData.ADTPtr[75].MaxFailsBeforeRTS = 5;
    LC_OperData.ADTPtr[75].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[75].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_EQUAL;

    /*
    ** Setup the AP results
    */
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr[75].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;

    /*
    ** Setup the watch result
    */
    LC_OperData.WRTPtr[4].WatchResult = LC_WATCH_STALE;
    
    UTF_put_text("Test sample single AP message with watch not measured\n");
    UTF_put_text("-----------------------------------------------------\n");
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");

    UTF_put_text("Test sample single AP message with watch error\n");
    UTF_put_text("----------------------------------------------\n");
    LC_OperData.WRTPtr[4].WatchResult = LC_WATCH_ERROR;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
    
    /*
    ** Setup the watch results for the next set of tests
    */
    LC_OperData.WRTPtr[4].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[5].WatchResult = LC_WATCH_FALSE;
    
    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Expression terminated with LC_RPN_AND\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_AND;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
    
    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Expression terminated with LC_RPN_OR\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_OR;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");

    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Expression terminated with LC_RPN_XOR\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_XOR;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
    
    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Expression begins with LC_RPN_NOT\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[0]    = LC_RPN_NOT;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");
  
    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Premature LC_RPN_EQUAL in expression\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[1]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[2]    = LC_RPN_EQUAL;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");

    UTF_put_text("Test sample single AP message with bad RPN expression\n");
    UTF_put_text("Expression runs beyond LC_MAX_RPN_EQU_SIZE\n");
    UTF_put_text("-----------------------------------------------------\n");
    LC_OperData.ADTPtr[75].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[1]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[2]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[3]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[4]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[5]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[6]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[7]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[8]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[9]    = 5;
    LC_OperData.ADTPtr[75].RPNEquation[10]   = 4;
    LC_OperData.ADTPtr[75].RPNEquation[11]   = 5;
    LC_OperData.ADTPtr[75].RPNEquation[12]   = 5;
    LC_OperData.ADTPtr[75].RPNEquation[13]   = 4;
    LC_OperData.ADTPtr[75].RPNEquation[14]   = 5;
    LC_OperData.ADTPtr[75].RPNEquation[15]   = 4;
    LC_OperData.ADTPtr[75].RPNEquation[16]   = 5;
    LC_OperData.ADTPtr[75].RPNEquation[17]   = 4;
    LC_OperData.ADTPtr[75].RPNEquation[18]   = 5;
    LC_OperData.ADTPtr[75].RPNEquation[19]   = 4;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);    
    UTF_put_text("\n");

    /* 
    ** Setup an actionpoint definition
    */ 
    LC_OperData.ADTPtr[75].DefaultState      = LC_APSTATE_ACTIVE;
    LC_OperData.ADTPtr[75].RTSId             = 20;
    LC_OperData.ADTPtr[75].MaxFailsBeforeRTS = 1;
    LC_OperData.ADTPtr[75].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[75].EventID           = 455;
    strcpy (LC_OperData.ADTPtr[75].EventText, "Test event text");
    LC_OperData.ADTPtr[75].RPNEquation[0]    = 4;
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_EQUAL;

    /*
    ** Setup the AP results
    */
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr[75].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;

    /*
    ** Setup the watch result
    */
    LC_OperData.WRTPtr[4].WatchResult = LC_WATCH_TRUE;
    
    UTF_put_text("Test sample single AP message, PASS to FAIL Transition\n");
    UTF_put_text("-----------------------------------------------------\n");
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);
    PrintARTEntry (75);
    UTF_put_text("\n");

    UTF_put_text("Test sample single AP message, PASS to FAIL Transition\n");
    UTF_put_text("While LC state = LC_STATE_PASSIVE\n");    
    UTF_put_text("-----------------------------------------------------\n");
    LC_AppData.CurrentLCState           = LC_STATE_PASSIVE;
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr[75].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);
    PrintARTEntry (75);
    UTF_put_text("\n");
    
    UTF_put_text("Test sample single AP message, PASS to FAIL Transition\n");
    UTF_put_text("While AP state = LC_APSTATE_PASSIVE\n");    
    UTF_put_text("-----------------------------------------------------\n");
    LC_AppData.CurrentLCState           = LC_STATE_ACTIVE;
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr[75].CurrentState = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);
    PrintARTEntry (75);
    UTF_put_text("\n");
    
    UTF_put_text("Test sample single AP message, FAIL to PASS Transition\n");
    UTF_put_text("------------------------------------------------------\n");
    LC_AppData.CurrentLCState           = LC_STATE_ACTIVE;
    LC_OperData.WRTPtr[4].WatchResult   = LC_WATCH_FALSE;
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ARTPtr[75].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;
    SampleAPMsg.StartIndex = 75;
    SampleAPMsg.EndIndex = SampleAPMsg.StartIndex;
    LC_SampleAPReq((CFE_SB_MsgPtr_t)&SampleAPMsg);
    PrintARTEntry (75);
    UTF_put_text("\n");
 
    /*
    ** Reset the data that we used
    */
    LC_AppData.CurrentLCState           = LC_STATE_ACTIVE;
    
    LC_OperData.WRTPtr[4].WatchResult   = LC_WATCH_STALE;
    LC_OperData.WRTPtr[5].WatchResult   = LC_WATCH_STALE;
    
    LC_OperData.ARTPtr[75].ActionResult = LC_ACTION_STALE;
    LC_OperData.ARTPtr[75].CurrentState = LC_ACTION_NOT_USED;
    LC_OperData.ARTPtr[75].FailToPassCount         = 0;
    LC_OperData.ARTPtr[75].PassToFailCount         = 0;
    LC_OperData.ARTPtr[75].ConsecutiveFailCount    = 0;
    LC_OperData.ARTPtr[75].CumulativeFailCount     = 0;
    LC_OperData.ARTPtr[75].CumulativeRTSExecCount  = 0;
    
    LC_OperData.ADTPtr[75].DefaultState      = LC_ACTION_NOT_USED;
    LC_OperData.ADTPtr[75].RTSId             = 0;
    LC_OperData.ADTPtr[75].MaxFailsBeforeRTS = 0;
    LC_OperData.ADTPtr[75].EventType         = CFE_EVS_INFORMATION;
    LC_OperData.ADTPtr[75].EventID           = 0;
    LC_OperData.ADTPtr[75].EventText[0]      ='\0';
    LC_OperData.ADTPtr[75].RPNEquation[0]    = 0;
    LC_OperData.ADTPtr[75].RPNEquation[1]    = LC_RPN_EQUAL;
    
} /* end Test_SampleAP */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run tests on code that handles watchpoint processing            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_WPProcessing (void)
{
   
   UTF_put_text("Test creation of WP hash table\n");
   UTF_put_text("--------------------------------------------------\n");
    LC_OperData.WDTPtr[11].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[11].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[11].MessageID  = 0x400;
    LC_OperData.WDTPtr[11].WatchpointOffset = 12;
    LC_OperData.WDTPtr[11].BitMask    = 0xFF;
    LC_OperData.WDTPtr[11].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[12].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[12].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[12].MessageID  = 0x401;
    LC_OperData.WDTPtr[12].WatchpointOffset = 12;
    LC_OperData.WDTPtr[12].BitMask    = 0xFF;
    LC_OperData.WDTPtr[12].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;
    LC_OperData.WDTPtr[13].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[13].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[13].MessageID  = 0x400;
    LC_OperData.WDTPtr[13].WatchpointOffset = 12;
    LC_OperData.WDTPtr[13].BitMask    = 0xFF;
    LC_OperData.WDTPtr[13].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[14].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[14].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[14].MessageID  = 0x401;
    LC_OperData.WDTPtr[14].WatchpointOffset = 12;
    LC_OperData.WDTPtr[14].BitMask    = 0xFF;
    LC_OperData.WDTPtr[14].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;
    LC_OperData.WDTPtr[15].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[15].OperatorID = LC_OPER_GE;  
    LC_OperData.WDTPtr[15].MessageID  = 0x400;
    LC_OperData.WDTPtr[15].WatchpointOffset = 12;
    LC_OperData.WDTPtr[15].BitMask    = 0xFF;
    LC_OperData.WDTPtr[15].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[16].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[16].OperatorID = LC_OPER_GT;  
    LC_OperData.WDTPtr[16].MessageID  = 0x400;
    LC_OperData.WDTPtr[16].WatchpointOffset = 12;
    LC_OperData.WDTPtr[16].BitMask    = 0xFF;
    LC_OperData.WDTPtr[16].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[17].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[17].OperatorID = LC_OPER_GT;  
    LC_OperData.WDTPtr[17].MessageID  = 0x401;
    LC_OperData.WDTPtr[17].WatchpointOffset = 12;
    LC_OperData.WDTPtr[17].BitMask    = 0xFF;
    LC_OperData.WDTPtr[17].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;
    LC_OperData.WDTPtr[18].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[18].OperatorID = LC_OPER_GE;  
    LC_OperData.WDTPtr[18].MessageID  = 0x401;
    LC_OperData.WDTPtr[18].WatchpointOffset = 12;
    LC_OperData.WDTPtr[18].BitMask    = 0xFF;
    LC_OperData.WDTPtr[18].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;

    LC_OperData.WDTPtr[21].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[21].OperatorID = LC_OPER_LE;  
    LC_OperData.WDTPtr[21].MessageID  = 0x400;
    LC_OperData.WDTPtr[21].WatchpointOffset = 12;
    LC_OperData.WDTPtr[21].BitMask    = 0xFF;
    LC_OperData.WDTPtr[21].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[22].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[22].OperatorID = LC_OPER_LT;  
    LC_OperData.WDTPtr[22].MessageID  = 0x400;
    LC_OperData.WDTPtr[22].WatchpointOffset = 12;
    LC_OperData.WDTPtr[22].BitMask    = 0xFF;
    LC_OperData.WDTPtr[22].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[23].DataType   = LC_DATA_BYTE;
    LC_OperData.WDTPtr[23].OperatorID = LC_OPER_LT;  
    LC_OperData.WDTPtr[23].MessageID  = 0x400;
    LC_OperData.WDTPtr[23].WatchpointOffset = 24;
    LC_OperData.WDTPtr[23].BitMask    = 0xFF;
    LC_OperData.WDTPtr[23].ComparisonValue.Signed8in32.Signed8 = 0x12;
    LC_OperData.WDTPtr[24].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[24].OperatorID = LC_OPER_LT;  
    LC_OperData.WDTPtr[24].MessageID  = 0x401;
    LC_OperData.WDTPtr[24].WatchpointOffset = 12;
    LC_OperData.WDTPtr[24].BitMask    = 0xFF;
    LC_OperData.WDTPtr[24].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;
    LC_OperData.WDTPtr[25].DataType   = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[25].OperatorID = LC_OPER_LE;  
    LC_OperData.WDTPtr[25].MessageID  = 0x401;
    LC_OperData.WDTPtr[25].WatchpointOffset = 12;
    LC_OperData.WDTPtr[25].BitMask    = 0xFF;
    LC_OperData.WDTPtr[25].ComparisonValue.Unsigned8in32.Unsigned8 = 0x12;

    LC_OperData.WDTPtr[31].DataType   = LC_DATA_WORD_LE;
    LC_OperData.WDTPtr[31].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[31].MessageID  = 0x400;
    LC_OperData.WDTPtr[31].WatchpointOffset = 12;
    LC_OperData.WDTPtr[31].BitMask    = 0xFFFF;
    LC_OperData.WDTPtr[31].ComparisonValue.Signed16in32.Signed16 = 0x1234;
    LC_OperData.WDTPtr[32].DataType   = LC_DATA_WORD_BE;
    LC_OperData.WDTPtr[32].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[32].MessageID  = 0x400;
    LC_OperData.WDTPtr[32].WatchpointOffset = 16;
    LC_OperData.WDTPtr[32].BitMask    = 0xFFFF;
    LC_OperData.WDTPtr[32].ComparisonValue.Unsigned16in32.Unsigned16 = 0x1234;
    LC_OperData.WDTPtr[33].DataType   = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[33].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[33].MessageID  = 0x401;
    LC_OperData.WDTPtr[33].WatchpointOffset = 12;
    LC_OperData.WDTPtr[33].BitMask    = 0xFFFF;
    LC_OperData.WDTPtr[33].ComparisonValue.Signed16in32.Signed16 = 0x1234;
    LC_OperData.WDTPtr[34].DataType   = LC_DATA_UWORD_BE;
    LC_OperData.WDTPtr[34].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[34].MessageID  = 0x401;
    LC_OperData.WDTPtr[34].WatchpointOffset = 16;
    LC_OperData.WDTPtr[34].BitMask    = 0xFFFF;
    LC_OperData.WDTPtr[34].ComparisonValue.Unsigned16in32.Unsigned16 = 0x1234;

    LC_OperData.WDTPtr[41].DataType   = LC_DATA_DWORD_LE;
    LC_OperData.WDTPtr[41].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[41].MessageID  = 0x500;
    LC_OperData.WDTPtr[41].WatchpointOffset = 12;
    LC_OperData.WDTPtr[41].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[41].ComparisonValue.Signed32 = 0x12345678;
    LC_OperData.WDTPtr[42].DataType   = LC_DATA_DWORD_BE;
    LC_OperData.WDTPtr[42].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[42].MessageID  = 0x500;
    LC_OperData.WDTPtr[42].WatchpointOffset = 16;
    LC_OperData.WDTPtr[42].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[42].ComparisonValue.Unsigned32 = 0x12345678;
    LC_OperData.WDTPtr[43].DataType   = LC_DATA_UDWORD_LE;
    LC_OperData.WDTPtr[43].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[43].MessageID  = 0x501;
    LC_OperData.WDTPtr[43].WatchpointOffset = 12;
    LC_OperData.WDTPtr[43].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[43].ComparisonValue.Signed32 = 0x12345678;
    LC_OperData.WDTPtr[44].DataType   = LC_DATA_UDWORD_BE;
    LC_OperData.WDTPtr[44].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[44].MessageID  = 0x501;
    LC_OperData.WDTPtr[44].WatchpointOffset = 16;
    LC_OperData.WDTPtr[44].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[44].ComparisonValue.Unsigned32 = 0x12345678;

    LC_OperData.WDTPtr[51].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[51].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[51].MessageID  = 0x500;
    LC_OperData.WDTPtr[51].WatchpointOffset = 12;
    LC_OperData.WDTPtr[51].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[51].ComparisonValue.Float32 = 1.5;
    LC_OperData.WDTPtr[52].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[52].OperatorID = LC_OPER_NE;  
    LC_OperData.WDTPtr[52].MessageID  = 0x500;
    LC_OperData.WDTPtr[52].WatchpointOffset = 16;
    LC_OperData.WDTPtr[52].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[52].ComparisonValue.Float32 = 1.5;
    LC_OperData.WDTPtr[53].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[53].OperatorID = LC_OPER_GT;  
    LC_OperData.WDTPtr[53].MessageID  = 0x500;
    LC_OperData.WDTPtr[53].WatchpointOffset = 12;
    LC_OperData.WDTPtr[53].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[53].ComparisonValue.Float32 = 1.5;
    LC_OperData.WDTPtr[54].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[54].OperatorID = LC_OPER_GE;  
    LC_OperData.WDTPtr[54].MessageID  = 0x500;
    LC_OperData.WDTPtr[54].WatchpointOffset = 16;
    LC_OperData.WDTPtr[54].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[54].ComparisonValue.Float32 = 1.5;
    LC_OperData.WDTPtr[55].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[55].OperatorID = LC_OPER_LT;  
    LC_OperData.WDTPtr[55].MessageID  = 0x500;
    LC_OperData.WDTPtr[55].WatchpointOffset = 12;
    LC_OperData.WDTPtr[55].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[55].ComparisonValue.Float32 = 1.5;
    LC_OperData.WDTPtr[56].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[56].OperatorID = LC_OPER_LE;  
    LC_OperData.WDTPtr[56].MessageID  = 0x500;
    LC_OperData.WDTPtr[56].WatchpointOffset = 16;
    LC_OperData.WDTPtr[56].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[56].ComparisonValue.Float32 = 1.5;

    LC_OperData.WDTPtr[61].DataType   = LC_DATA_DWORD_BE;
    LC_OperData.WDTPtr[61].OperatorID = LC_OPER_CUSTOM;  
    LC_OperData.WDTPtr[61].MessageID  = 0x600;
    LC_OperData.WDTPtr[61].WatchpointOffset = 12;
    LC_OperData.WDTPtr[61].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[61].ComparisonValue.Unsigned32 = 0x12345678;

    LC_OperData.WDTPtr[62].DataType   = 99;
    LC_OperData.WDTPtr[62].OperatorID = LC_OPER_EQ;  
    LC_OperData.WDTPtr[62].MessageID  = 0x600;
    LC_OperData.WDTPtr[62].WatchpointOffset = 12;
    LC_OperData.WDTPtr[62].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[62].ComparisonValue.Unsigned32 = 0x12345678;

    LC_OperData.WDTPtr[63].DataType   = LC_DATA_DWORD_BE;
    LC_OperData.WDTPtr[63].OperatorID = 99;  
    LC_OperData.WDTPtr[63].MessageID  = 0x600;
    LC_OperData.WDTPtr[63].WatchpointOffset = 12;
    LC_OperData.WDTPtr[63].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[63].ComparisonValue.Unsigned32 = 0x12345678;

    LC_OperData.WDTPtr[64].DataType   = LC_DATA_UDWORD_BE;
    LC_OperData.WDTPtr[64].OperatorID = 99;  
    LC_OperData.WDTPtr[64].MessageID  = 0x600;
    LC_OperData.WDTPtr[64].WatchpointOffset = 12;
    LC_OperData.WDTPtr[64].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[64].ComparisonValue.Unsigned32 = 0x12345678;

    LC_OperData.WDTPtr[65].DataType   = LC_DATA_FLOAT_BE;
    LC_OperData.WDTPtr[65].OperatorID = 99;  
    LC_OperData.WDTPtr[65].MessageID  = 0x600;
    LC_OperData.WDTPtr[65].WatchpointOffset = 12;
    LC_OperData.WDTPtr[65].BitMask    = 0xFFFFFFFF;
    LC_OperData.WDTPtr[65].ComparisonValue.Unsigned32 = 0x12345678;

   LC_CreateHashTable();
   LC_CreateHashTable();

   UTF_put_text("\n");

   UTF_put_text("Test process TLM packet\n");
   UTF_put_text("--------------------------------------------------\n");

   CFE_SB_InitMsg(&LC_TestPacket1, 0x400, sizeof(LC_TestPacket1_t), TRUE);
   LC_TestPacket1.Data1 = 0x12345678;
   LC_TestPacket1.Data2 = 0x12345678;
   LC_TestPacket1.Data3 = 1.5;
   LC_CheckMsgForWPs(0x400, (CFE_SB_MsgPtr_t) &LC_TestPacket1);

   CFE_SB_InitMsg(&LC_TestPacket1, 0x401, sizeof(LC_TestPacket1_t), TRUE);
   LC_TestPacket1.Data1 = 0x12345678;
   LC_TestPacket1.Data2 = 0x12345678;
   LC_TestPacket1.Data3 = 1.5;
   LC_CheckMsgForWPs(0x401, (CFE_SB_MsgPtr_t) &LC_TestPacket1);

   CFE_SB_InitMsg(&LC_TestPacket1, 0x500, sizeof(LC_TestPacket1_t), TRUE);
   LC_TestPacket1.Data1 = 0x12345678;
   LC_TestPacket1.Data2 = 0x12345678;
   LC_TestPacket1.Data3 = 1.5;
   LC_CheckMsgForWPs(0x500, (CFE_SB_MsgPtr_t) &LC_TestPacket1);

   CFE_SB_InitMsg(&LC_TestPacket2, 0x501, sizeof(LC_TestPacket2_t), TRUE);
   LC_TestPacket2.Data1 = 0x12345678;
   LC_TestPacket2.Data2 = 0x12345678;
   LC_TestPacket2.Data3 = 0x12345678;
   LC_CheckMsgForWPs(0x501, (CFE_SB_MsgPtr_t) &LC_TestPacket2);

   CFE_SB_InitMsg(&LC_TestPacket2, 0x600, sizeof(LC_TestPacket2_t), TRUE);
   LC_TestPacket2.Data1 = 0x12345678;
   LC_TestPacket2.Data2 = 0x12345678;
   LC_TestPacket2.Data3 = 0x12345678;
   LC_CheckMsgForWPs(0x600, (CFE_SB_MsgPtr_t) &LC_TestPacket2);
   UTF_put_text("\n");

   CFE_SB_InitMsg(&LC_TestPacket2, 0x600, sizeof(LC_TestPacket2_t), TRUE);
   LC_TestPacket2.Data1 = 0x12345678;
   LC_TestPacket2.Data2 = 0x12345678;
   LC_TestPacket2.Data3 = 0x12345678;
   LC_CheckMsgForWPs(0x700, (CFE_SB_MsgPtr_t) &LC_TestPacket2);
   UTF_put_text("\n");

   return;

} /* end Test_WPProcessing */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run tests on code that handles LC_CMD_MID commands              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_CmdHandlers (void)
{
    /* MID = not a LC command (process watchpoints) */
    CFE_SB_InitMsg(&LC_TestPacket1, 0x400, sizeof(LC_TestPacket1_t), TRUE);
    LC_TestPacket1.Data1 = 0x12345678;
    LC_TestPacket1.Data2 = 0x12345678;
    LC_TestPacket1.Data3 = 1.5;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_TestPacket1);

    /* MID = LC_SAMPLE_AP_MID, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_SAMPLE_AP_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* MID = LC_SAMPLE_AP_MID, invalid start index (too big) */
    CFE_SB_InitMsg(&LC_SampleAP, LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);
    LC_SampleAP.StartIndex = LC_MAX_ACTIONPOINTS;
    LC_SampleAP.EndIndex = LC_MAX_ACTIONPOINTS - 1;
    LC_SampleAP.UpdateAge = FALSE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SampleAP);

    /* MID = LC_SAMPLE_AP_MID, invalid end index (too big) */
    CFE_SB_InitMsg(&LC_SampleAP, LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);
    LC_SampleAP.StartIndex = LC_MAX_ACTIONPOINTS - 1;
    LC_SampleAP.EndIndex = LC_MAX_ACTIONPOINTS;
    LC_SampleAP.UpdateAge = FALSE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SampleAP);

    /* MID = LC_SAMPLE_AP_MID, success */
    CFE_SB_InitMsg(&LC_SampleAP, LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);
    LC_SampleAP.StartIndex = LC_MAX_ACTIONPOINTS - 1;
    LC_SampleAP.EndIndex = LC_MAX_ACTIONPOINTS - 1;
    LC_SampleAP.UpdateAge = TRUE;
    LC_OperData.WRTPtr[LC_MAX_WATCHPOINTS - 1].CountdownToStale = 1;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SampleAP);

    /* MID = LC_SEND_HK_MID, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_SEND_HK_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* MID = LC_SEND_HK_MID, success */
    CFE_SB_InitMsg(&LC_NoArgsCmd, LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    LC_OperData.HaveActiveCDS = TRUE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_NoArgsCmd);
    LC_OperData.HaveActiveCDS = FALSE;

    /* MID = LC_CMD_MID, CC = invalid */
    CFE_SB_InitMsg(&LC_NoArgsCmd, LC_CMD_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_NoArgsCmd, 99);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_NoArgsCmd);

    /* CC = LC_NOOP_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_NOOP_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_NOOP_CC, success */
    CFE_SB_InitMsg(&LC_NoArgsCmd, LC_CMD_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_NoArgsCmd, LC_NOOP_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_NoArgsCmd);

    /* CC = LC_RESET_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_RESET_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_RESET_CC, success */
    CFE_SB_InitMsg(&LC_NoArgsCmd, LC_CMD_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_NoArgsCmd, LC_RESET_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_NoArgsCmd);

    /* CC = LC_SET_LC_STATE_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_SET_LC_STATE_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_SET_LC_STATE_CC, invalid new LC state */
    CFE_SB_InitMsg(&LC_SetLCState, LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetLCState, LC_SET_LC_STATE_CC);
    LC_SetLCState.NewLCState = 99;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetLCState);

    /* CC = LC_SET_LC_STATE_CC, success (LC_STATE_PASSIVE) */
    CFE_SB_InitMsg(&LC_SetLCState, LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetLCState, LC_SET_LC_STATE_CC);
    LC_SetLCState.NewLCState = LC_STATE_PASSIVE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetLCState);

    /* CC = LC_SET_LC_STATE_CC, success (LC_STATE_DISABLED) */
    CFE_SB_InitMsg(&LC_SetLCState, LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetLCState, LC_SET_LC_STATE_CC);
    LC_SetLCState.NewLCState = LC_STATE_DISABLED;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetLCState);

    /* CC = LC_SET_LC_STATE_CC, success (LC_STATE_ACTIVE) */
    CFE_SB_InitMsg(&LC_SetLCState, LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetLCState, LC_SET_LC_STATE_CC);
    LC_SetLCState.NewLCState = LC_STATE_ACTIVE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetLCState);

    /* CC = LC_SET_AP_STATE_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_SET_AP_STATE_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_SET_AP_STATE_CC, invalid new AP state */
    CFE_SB_InitMsg(&LC_SetAPState, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPState, LC_SET_AP_STATE_CC);
    LC_SetAPState.APNumber = LC_MAX_ACTIONPOINTS - 1;
    LC_SetAPState.NewAPState = 99;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPState);

    /* CC = LC_SET_AP_STATE_CC, invalid AP (too big) */
    CFE_SB_InitMsg(&LC_SetAPState, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPState, LC_SET_AP_STATE_CC);
    LC_SetAPState.APNumber = LC_MAX_ACTIONPOINTS;
    LC_SetAPState.NewAPState = LC_APSTATE_ACTIVE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPState);

    /* CC = LC_SET_AP_STATE_CC, success (LC_APSTATE_PASSIVE) */
    CFE_SB_InitMsg(&LC_SetAPState, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPState, LC_SET_AP_STATE_CC);
    LC_SetAPState.APNumber = LC_MAX_ACTIONPOINTS - 1;
    LC_SetAPState.NewAPState = LC_APSTATE_PASSIVE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPState);

    /* CC = LC_SET_AP_STATE_CC, success (LC_APSTATE_DISABLED) */
    CFE_SB_InitMsg(&LC_SetAPState, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPState, LC_SET_AP_STATE_CC);
    LC_SetAPState.APNumber = LC_MAX_ACTIONPOINTS - 1;
    LC_SetAPState.NewAPState = LC_APSTATE_DISABLED;
    LC_OperData.ARTPtr[LC_MAX_ACTIONPOINTS - 1].CurrentState = LC_APSTATE_PERMOFF;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPState);
    LC_OperData.ARTPtr[LC_MAX_ACTIONPOINTS - 1].CurrentState = LC_APSTATE_DISABLED;

    /* CC = LC_SET_AP_STATE_CC, success (LC_APSTATE_ACTIVE) */
    CFE_SB_InitMsg(&LC_SetAPState, LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPState, LC_SET_AP_STATE_CC);
    LC_SetAPState.APNumber = LC_MAX_ACTIONPOINTS - 1;
    LC_SetAPState.NewAPState = LC_APSTATE_ACTIVE;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPState);

    /* CC = LC_SET_AP_PERMOFF_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_SET_AP_PERMOFF_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_SET_AP_PERMOFF_CC, invalid AP (too big) */
    CFE_SB_InitMsg(&LC_SetAPPermOff, LC_CMD_MID, sizeof(LC_SetAPPermOff_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_SetAPPermOff, LC_SET_AP_PERMOFF_CC);
    LC_SetAPPermOff.APNumber = LC_MAX_ACTIONPOINTS;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_SetAPPermOff);

    /* CC = LC_RESET_AP_STATS_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_RESET_AP_STATS_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_RESET_AP_STATS_CC, invalid AP (too big) */
    CFE_SB_InitMsg(&LC_ResetAPStats, LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetAPStats, LC_RESET_AP_STATS_CC);
    LC_ResetAPStats.APNumber = LC_MAX_ACTIONPOINTS;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetAPStats);

    /* CC = LC_RESET_AP_STATS_CC, success (all AP's) */
    CFE_SB_InitMsg(&LC_ResetAPStats, LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetAPStats, LC_RESET_AP_STATS_CC);
    LC_ResetAPStats.APNumber = LC_ALL_ACTIONPOINTS;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetAPStats);

    /* CC = LC_RESET_AP_STATS_CC, success (one AP) */
    CFE_SB_InitMsg(&LC_ResetAPStats, LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetAPStats, LC_RESET_AP_STATS_CC);
    LC_ResetAPStats.APNumber = LC_MAX_ACTIONPOINTS - 1;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetAPStats);

    /* CC = LC_RESET_WP_STATS_CC, invalid packet length */
    CFE_SB_InitMsg(&LC_BigCmdPacket, LC_CMD_MID, sizeof(LC_BigCmdPacket_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_BigCmdPacket, LC_RESET_WP_STATS_CC);
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_BigCmdPacket);

    /* CC = LC_RESET_WP_STATS_CC, invalid WP (too big) */
    CFE_SB_InitMsg(&LC_ResetWPStats, LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetWPStats, LC_RESET_WP_STATS_CC);
    LC_ResetWPStats.WPNumber = LC_MAX_WATCHPOINTS;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetWPStats);

    /* CC = LC_RESET_WP_STATS_CC, success (all AP's) */
    CFE_SB_InitMsg(&LC_ResetWPStats, LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetWPStats, LC_RESET_WP_STATS_CC);
    LC_ResetWPStats.WPNumber = LC_ALL_WATCHPOINTS;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetWPStats);

    /* CC = LC_RESET_WP_STATS_CC, success (one AP) */
    CFE_SB_InitMsg(&LC_ResetWPStats, LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &LC_ResetWPStats, LC_RESET_WP_STATS_CC);
    LC_ResetWPStats.WPNumber = LC_MAX_WATCHPOINTS - 1;
    LC_AppPipe((CFE_SB_MsgPtr_t) &LC_ResetWPStats);

    /* Invoke reset AP & WP results functions with caller not a command handler */
    LC_ResetResultsAP(0, 0, FALSE);
    LC_ResetResultsWP(0, 0, FALSE);

   UTF_put_text("\n");

   return;

} /* end Test_CmdHandlers */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Miscellaneous cleanup to complete coverage                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Test_Coverage (void)
{
   LC_MultiType_t WPMultiType;
   uint32 Test_Arg32;

   /* LC_SampleAPs() - Actionpoint isn't currently operational */
   LC_OperData.ARTPtr[0].CurrentState = LC_ACTION_NOT_USED;
   LC_SampleAPs(0, 0);
   LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_PERMOFF;
   LC_SampleAPs(0, 0);

   /* LC_SampleSingleAP() - Send only a limited number of Pass to Fail events */
   LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_ACTIVE;
   LC_OperData.ARTPtr[0].ActionResult = LC_ACTION_PASS;
   LC_OperData.ARTPtr[0].ConsecutiveFailCount = 0;
   LC_OperData.ARTPtr[0].CumulativeFailCount = 0;
   LC_OperData.ARTPtr[0].PassToFailCount = 0;
   LC_OperData.ADTPtr[0].MaxPassFailEvents = 1;
   LC_OperData.ADTPtr[0].MaxFailsBeforeRTS = 5;
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_SampleSingleAP(0);

   /* LC_SampleSingleAP() - Send only a limited number of AP is Passive events */
   LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_PASSIVE;
   LC_OperData.ARTPtr[0].ActionResult = LC_ACTION_PASS;
   LC_OperData.ARTPtr[0].ConsecutiveFailCount = 0;
   LC_OperData.ARTPtr[0].CumulativeFailCount = 0;
   LC_OperData.ARTPtr[0].PassToFailCount = 0;
   LC_OperData.ADTPtr[0].MaxPassFailEvents = 1;
   LC_OperData.ADTPtr[0].MaxFailsBeforeRTS = 1;
   LC_OperData.ARTPtr[0].PassiveAPCount = 0;
   LC_OperData.ADTPtr[0].MaxPassiveEvents = 1;
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_SampleSingleAP(0);

   /* LC_SampleSingleAP() - Send only a limited number of Fail to Pass events */
   LC_OperData.ARTPtr[0].CurrentState = LC_APSTATE_ACTIVE;
   LC_OperData.ARTPtr[0].ActionResult = LC_ACTION_FAIL;
   LC_OperData.ARTPtr[0].ConsecutiveFailCount = 0;
   LC_OperData.ARTPtr[0].CumulativeFailCount = 0;
   LC_OperData.ARTPtr[0].FailToPassCount = 0;
   LC_OperData.ADTPtr[0].MaxFailPassEvents = 1;
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;
   LC_SampleSingleAP(0);

   /* LC_EvaluateRPN() - LC_RPN_AND: if either operand = LC_WATCH_ERROR */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_AND: if either operand = LC_WATCH_STALE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_AND: else both operands = LC_WATCH_TRUE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_OR: if either operand = LC_WATCH_ERROR */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_OR: if either operand = LC_WATCH_STALE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_OR: else both operands = LC_WATCH_FALSE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_XOR: if either operand = LC_WATCH_ERROR */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_XOR: if either operand = LC_WATCH_STALE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;
   LC_EvaluateRPN(0);
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = 1;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
   LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
   LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_NOT: if operand = LC_WATCH_ERROR */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_NOT;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - LC_RPN_NOT: if operand = LC_WATCH_STALE */
   LC_OperData.ADTPtr[0].RPNEquation[0] = 0;
   LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_NOT;
   LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_EQUAL;
   LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
   LC_EvaluateRPN(0);

   /* LC_EvaluateRPN() - default: if RPNData >= LC_MAX_WATCHPOINTS */
   LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS;
   LC_EvaluateRPN(0);

   LC_OperData.ADTPtr = LC_UnitTestADT;
   LC_OperData.WDTPtr = LC_UnitTestWDT;

   /* LC_OperatorCompare() - default: WP has invalid data type */
   LC_OperData.WDTPtr[0].DataType = 99;
   LC_OperatorCompare(0, 0);
   LC_OperData.WDTPtr[0].DataType = LC_WATCH_NOT_USED;

   /* LC_FloatCompare() - WPMultiType is NAN (not a number) */
   WPMultiType.Unsigned32 = 0x7F800001;
   LC_FloatCompare(0, WPMultiType, WPMultiType);

   UTF_TBL_set_function_hook(CFE_TBL_MANAGE_HOOK, NULL);
   UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, CFE_SUCCESS);
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_MANAGE_PROC, CFE_SUCCESS);

   /* LC_ManageTables() - CFE_TBL_GetAddress = CFE_TBL_INFO_UPDATED */
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, CFE_TBL_INFO_UPDATED);
   LC_ManageTables();

   /* LC_ManageTables() - CFE_TBL_GetAddress = CFE_SUCCESS */
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, CFE_SUCCESS);
   LC_ManageTables();

   /* LC_ManageTables() - CFE_TBL_GetAddress (2nd call) = CFE_TBL_ERR_INVALID_HANDLE */
   UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, CFE_TBL_GetAddress_FailOnN);   
   CFE_TBL_GetAddressCallCount  = 0;
   CFE_TBL_GetAddressFailCount  = 1;
   LC_ManageTables();

   /* LC_HousekeepingReq() - LC_ManageTables = CFE_SUCCESS */
   UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, NULL);
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, CFE_SUCCESS);
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_RELEASEADDRESS_PROC, CFE_SUCCESS);
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_MANAGE_PROC, CFE_SUCCESS);
   CFE_SB_InitMsg(&LC_NoArgsCmd, LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);
   LC_HousekeepingReq((CFE_SB_MsgPtr_t) &LC_NoArgsCmd);

   /* LC_AddWatchpoint() - CFE_SB_Subscribe = CFE_SB_BAD_ARGUMENT */
   UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, NULL);   
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC, CFE_SB_BAD_ARGUMENT);
   /* called from LC_CreateHashTable() - see next test element */

   /* LC_CreateHashTable() - CFE_SB_Unsubscribe = CFE_SB_NO_SUBSCRIBERS */
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_UNSUBSCRIBE_PROC, CFE_SB_NO_SUBSCRIBERS);
   LC_CreateHashTable();

   /* LC_GetSizedWPData() - default: WP has invalid data type */
   LC_OperData.WDTPtr[0].DataType = 99;
   LC_GetSizedWPData(0, (uint8  *) &Test_Arg32, &Test_Arg32);


   UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, CFE_TBL_Load_FailOnN);   
   UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, CFE_TBL_GetAddress_FailOnN);   
   UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, CFE_TBL_Register_FailOnN);   

   /* LC_LoadDefaultTables() - CFE_TBL_Load (WDT) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_LoadCallCount = 1;
   CFE_TBL_LoadFailCount = 1;
   LC_LoadDefaultTables();

   /* LC_LoadDefaultTables() - CFE_TBL_GetAddress (WDT) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_LoadCallCount = 0;
   CFE_TBL_LoadFailCount = 1;
   CFE_TBL_GetAddressCallCount = 1;
   CFE_TBL_GetAddressFailCount = 1;
   LC_LoadDefaultTables();

   /* LC_LoadDefaultTables() - CFE_TBL_Load (ADT) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_LoadCallCount = 0;
   CFE_TBL_LoadFailCount = 1;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 1;
   LC_LoadDefaultTables();

   /* LC_LoadDefaultTables() - CFE_TBL_GetAddress (ADT) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_LoadCallCount = 0;
   CFE_TBL_LoadFailCount = 2;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 1;
   LC_LoadDefaultTables();

   /* LC_LoadDefaultTables() - CFE_TBL_GetAddress (ADT) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_LoadCallCount = 0;
   CFE_TBL_LoadFailCount = 2;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 2;
   LC_LoadDefaultTables();

   /* LC_CreateDefinitionTables() - CFE_TBL_Register (WDT) = CFE_TBL_ERR_INVALID_SIZE */
   LC_OperData.HaveActiveCDS = TRUE;
   CFE_TBL_RegisterCallCount = 1;
   CFE_TBL_RegisterFailCount = 1;
   LC_CreateDefinitionTables();

   /* LC_CreateDefinitionTables() - CFE_TBL_Register (WDT) = CFE_TBL_ERR_INVALID_SIZE */
   LC_OperData.HaveActiveCDS = FALSE;
   CFE_TBL_RegisterCallCount = 1;
   CFE_TBL_RegisterFailCount = 1;
   LC_CreateDefinitionTables();

   /* LC_CreateDefinitionTables() - CFE_TBL_GetAddress (WDT) = CFE_TBL_ERR_INVALID_HANDLE */
   LC_OperData.HaveActiveCDS = TRUE;
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 2;
   LC_CreateDefinitionTables();

   /* LC_CreateDefinitionTables() - CFE_TBL_GetAddress (WDT) = CFE_TBL_ERR_INVALID_HANDLE */
   LC_OperData.HaveActiveCDS = FALSE;
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 1;
   LC_CreateDefinitionTables();

   /* LC_CreateResultTables() - CFE_TBL_Register (ART) = CFE_TBL_ERR_INVALID_SIZE */
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 1;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 1;
   LC_CreateResultTables();

   /* LC_CreateResultTables() - CFE_TBL_GetAddress (ART) = CFE_TBL_ERR_INVALID_HANDLE */
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 2;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 1;
   LC_CreateResultTables();

   /* LC_CreateResultTables() - Result = CFE_SUCCESS */
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 2;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 2;
   LC_CreateResultTables();

   /* LC_TableInit() - LC_CreateDefinitionTables != CFE_SUCCESS */
   LC_OperData.HaveActiveCDS = FALSE;
   CFE_TBL_RegisterCallCount = 0;
   CFE_TBL_RegisterFailCount = 2;
   CFE_TBL_GetAddressCallCount = 0;
   CFE_TBL_GetAddressFailCount = 2;
   LC_TableInit();

   UTF_TBL_set_function_hook(CFE_TBL_LOAD_HOOK, NULL);   
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_LOAD_PROC, CFE_SUCCESS);
   UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK, NULL);   
   UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, CFE_SUCCESS);
   UTF_TBL_set_function_hook(CFE_TBL_GETADDRESS_HOOK, CFE_TBL_GetAddress_Hook);   

   UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, NULL);   
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC, CFE_SUCCESS);
   UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_UNSUBSCRIBE_PROC, CFE_SUCCESS);

   /* LC_TableInit() - CFE_TBL_GetAddress (ADT) = CFE_TBL_ERR_INVALID_HANDLE */
   LC_OperData.HaveActiveCDS = FALSE;
   LC_TableInit();

   /* LC_AppInit() - Result = CFE_SUCCESS */
   LC_AppInit();

   return;

} /* end Test_Coverage */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values in the LC housekeeping packet     */
/* data structure                                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintHKPacket (void)
{
   int i;
    
   /* Output the LC housekeeping data */
   UTF_put_text("\nLC HOUSEKEEPING DATA:\n");

   UTF_put_text("   Command Count            = %d\n", LC_OperData.HkPacket.CmdCount);
   UTF_put_text("   Command Error Count      = %d\n", LC_OperData.HkPacket.CmdErrCount);
   UTF_put_text("   Actionpoint Sample Count = %d\n", LC_OperData.HkPacket.APSampleCount);
   UTF_put_text("   Monitored Message Count  = %d\n", LC_OperData.HkPacket.MonitoredMsgCount);
   UTF_put_text("   RTS Exec Count           = %d\n", LC_OperData.HkPacket.RTSExecCount);
   UTF_put_text("   Passive RTS Exec Count   = %d\n", LC_OperData.HkPacket.PassiveRTSExecCount);
   UTF_put_text("   Watchpoints In Use       = %d\n", LC_OperData.HkPacket.WPsInUse);
   UTF_put_text("   Active Actionpoints      = %d\n", LC_OperData.HkPacket.ActiveAPs);

   /*
   ** Handle the current state enumerated type
   */
   switch(LC_OperData.HkPacket.CurrentLCState)
   {
      case LC_STATE_ACTIVE:
         UTF_put_text("   Current LC State         = LC_STATE_ACTIVE\n");
         break;

      case LC_STATE_PASSIVE:
         UTF_put_text("   Current LC State         = LC_STATE_PASSIVE\n");
         break;
        
      case LC_STATE_DISABLED:
         UTF_put_text("   Current LC State         = LC_STATE_DISABLED\n");
         break;
         
      default:
         UTF_put_text("   Current LC State         = %d\n", LC_OperData.HkPacket.CurrentLCState);
         break;
   }
   
   /*
   ** Handle the watchpoint results array
   */
   UTF_put_text("\nWatchpoint Results:\n");   
   for (i = 1; i <= LC_HKWR_NUM_BYTES; i++)
   {
       UTF_put_text("0x%02X ", LC_OperData.HkPacket.WPResults[i - 1]);
       
       if (i % 10 == 0)         /* Do 10 per line */
       {
           UTF_put_text("\n");
       }
   }
   UTF_put_text("\n");
   
   /*
   ** Handle the actionpoint results array
   */
   UTF_put_text("\nActionpoint Results:\n");   
   for (i = 1; i <= LC_HKAR_NUM_BYTES; i++)
   {
       UTF_put_text("0x%02X ", LC_OperData.HkPacket.APResults[i - 1]);
       
       if (i % 10 == 0)         /* Do 10 per line */
       {
           UTF_put_text("\n");
       }
   }
   UTF_put_text("\n");
   
} /* end PrintHKPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values for a single Watchpoint Results   */
/* Table (WRT) entry                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintWRTEntry (uint16 WatchIndex)
{
    
   UTF_put_text("\nLC WRT DATA FOR ENTRY #%d:\n", WatchIndex);

   switch(LC_OperData.WRTPtr[WatchIndex].WatchResult)
   {
      case LC_WATCH_STALE:
         UTF_put_text("   Watch Result                 = LC_WATCH_STALE\n");
         break;

      case LC_WATCH_FALSE:
         UTF_put_text("   Watch Result                 = LC_WATCH_FALSE\n");
         break;
        
      case LC_WATCH_TRUE:
         UTF_put_text("   Watch Result                 = LC_WATCH_TRUE\n");
         break;
         
      case LC_WATCH_ERROR:
         UTF_put_text("   Watch Result                 = LC_WATCH_ERROR\n");
         break;
      
      default:
         UTF_put_text("   Watch Result                 = %d\n", LC_OperData.WRTPtr[WatchIndex].WatchResult);
         break;
   }
  
   UTF_put_text("   Evaluation Count             = %d\n", LC_OperData.WRTPtr[WatchIndex].EvaluationCount);
   UTF_put_text("   False To True Count          = %d\n", LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount);
   UTF_put_text("   Consecutive True Count       = %d\n", LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount);
   UTF_put_text("   Cumulative True Count        = %d\n", LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount);
   UTF_put_text("\n");
   UTF_put_text("   Last False To True (Value)   = %d\n", LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value);
   UTF_put_text("   Last False To True (Secs)    = %d\n", LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds);
   UTF_put_text("   Last False To True (SubSecs) = %d\n", LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds);
   UTF_put_text("\n");
   UTF_put_text("   Last True To False (Value)   = %d\n", LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value);
   UTF_put_text("   Last True To False (Secs)    = %d\n", LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds);
   UTF_put_text("   Last True To False (SubSecs) = %d\n", LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds);
   UTF_put_text("\n");
   
} /* end PrintWRTEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Prints out the current values for a single Actionpoint Results  */
/* Table (ART) entry                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void PrintARTEntry (uint16 ActionIndex)
{
    
   UTF_put_text("\nLC ART DATA FOR ENTRY #%d:\n", ActionIndex);

   switch(LC_OperData.ARTPtr[ActionIndex].ActionResult)
   {
      case LC_ACTION_STALE:
         UTF_put_text("   Action Result             = LC_ACTION_STALE\n");
         break;

      case LC_ACTION_PASS:
         UTF_put_text("   Action Result             = LC_ACTION_PASS\n");
         break;
        
      case LC_ACTION_FAIL:
         UTF_put_text("   Action Result             = LC_ACTION_FAIL\n");
         break;
         
      case LC_ACTION_ERROR:
         UTF_put_text("   Action Result             = LC_ACTION_ERROR\n");
         break;
      
      default:
         UTF_put_text("   Action Result             = %d\n", LC_OperData.ARTPtr[ActionIndex].ActionResult);
         break;
   }
   
   switch(LC_OperData.ARTPtr[ActionIndex].CurrentState)
   {
      case LC_ACTION_NOT_USED:
         UTF_put_text("   Current State             = LC_ACTION_NOT_USED\n");
         break;

      case LC_APSTATE_ACTIVE:
         UTF_put_text("   Current State             = LC_APSTATE_ACTIVE\n");
         break;
        
      case LC_APSTATE_PASSIVE:
         UTF_put_text("   Current State             = LC_APSTATE_PASSIVE\n");
         break;
         
      case LC_APSTATE_DISABLED:
         UTF_put_text("   Current State             = LC_APSTATE_DISABLED\n");
         break;
      
      case LC_APSTATE_PERMOFF:
         UTF_put_text("   Current State             = LC_APSTATE_PERMOFF\n");
         break;
      
      default:
         UTF_put_text("   Current State             = %d\n", LC_OperData.ARTPtr[ActionIndex].CurrentState);
         break;
   }
   
   UTF_put_text("   Fail To Pass Count        = %d\n", LC_OperData.ARTPtr[ActionIndex].FailToPassCount);
   UTF_put_text("   Pass To Fail Count        = %d\n", LC_OperData.ARTPtr[ActionIndex].PassToFailCount);
   UTF_put_text("   Consecutive Fail Count    = %d\n", LC_OperData.ARTPtr[ActionIndex].ConsecutiveFailCount);
   UTF_put_text("   Cumulative Fail Count     = %d\n", LC_OperData.ARTPtr[ActionIndex].CumulativeFailCount);
   UTF_put_text("   Cumulative RTS Exec Count = %d\n", LC_OperData.ARTPtr[ActionIndex].CumulativeRTSExecCount);
   UTF_put_text("\n");
   
} /* end PrintARTEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_SB_Subscribe that will return an error    */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_SB_Subscribe_FailOnN (CFE_SB_MsgId_t  MsgId,
                                CFE_SB_PipeId_t PipeId)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_SB_Subscribe, we use two global variables that are set 
    ** by the calling routine to control when we return an error
    */
    if (CFE_SB_SubscribeCallCount == CFE_SB_SubscribeFailCount)
    {
        Status = CFE_SB_MAX_MSGS_MET;
    }
    else
    {
        /*
        ** Note: we can't call CFE_SB_Subscribe here or we'll recurse
        ** back into this routine infinitely.
        */
        Status = CFE_SUCCESS;
        CFE_SB_SubscribeCallCount++;
    }

    return(Status);    
   
} /* end CFE_SB_Subscribe_FailOnN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Register that will return an error    */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_Register_FailOnN (CFE_TBL_Handle_t *TblHandlePtr,
                                const char *Name,
                                uint32  Size,
                                uint16  TblOptionFlags,
                                CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_TBL_Register, we use two global variables that are set 
    ** by the calling routine to control when we return an error
    */
    if (CFE_TBL_RegisterCallCount == CFE_TBL_RegisterFailCount)
    {
        Status = CFE_TBL_ERR_INVALID_SIZE;
    }
    else
    {
        /*
        ** Note: we can't call CFE_TBL_Register here or we'll recurse
        ** back into this routine infinitely.
        */
        Status = CFE_SUCCESS;
        CFE_TBL_RegisterCallCount++;
    }

    return(Status);    
   
} /* end CFE_TBL_Register_FailOnN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Register that will return             */
/* CFE_TBL_INFO_RECOVERED_TBL on the first two times that it's     */
/* called then return CFE_SUCCESS after                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_Register_Upd2 (CFE_TBL_Handle_t *TblHandlePtr,
                             const char *Name,
                             uint32  Size,
                             uint16  TblOptionFlags,
                             CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_TBL_Register, we use a global variable that is set 
    ** by the calling routine to control when we return different
    ** status codes
    */
    if (CFE_TBL_RegisterCallCount <= 2)
    {
        Status = CFE_TBL_INFO_RECOVERED_TBL;
        CFE_TBL_RegisterCallCount++;
    }
    else
    {
        /*
        ** Note: we can't call CFE_TBL_Register here or we'll recurse
        ** back into this routine infinitely.
        */
        Status = CFE_SUCCESS;
        CFE_TBL_RegisterCallCount++;
    }

    return(Status);    
   
} /* end CFE_TBL_Register_Upd2 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Load that will return an error        */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_Load_FailOnN (CFE_TBL_Handle_t  TblHandle,
                            CFE_TBL_SrcEnum_t SrcType,
                            const void       *SrcDataPtr)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_TBL_Load, we use two global variables that are set 
    ** by the calling routine to control when we return an error
    */
    if (CFE_TBL_LoadCallCount == CFE_TBL_LoadFailCount)
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }
    else
    {
        /*
        ** Note: we can't call CFE_TBL_Load here or we'll recurse
        ** back into this routine infinitely.
        */
        Status = CFE_SUCCESS;
        CFE_TBL_LoadCallCount++;
    }

    return(Status);    
   
} /* end CFE_TBL_Load_FailOnN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_GetAddress that will return an error  */
/* when called a certain number of times (set by global variable)  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_GetAddress_FailOnN (void             **TblPtr,
                                  CFE_TBL_Handle_t TblHandle)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_TBL_GetAddress, we use two global variables that are set 
    ** by the calling routine to control when we return an error
    */
    if (CFE_TBL_GetAddressCallCount == CFE_TBL_GetAddressFailCount)
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }
    else
    {
        /*
        ** Update all the apps table pointers to our local test
        ** buffers just to be sure they're all set right
        */
        LC_OperData.WRTPtr = LC_UnitTestWRT;
        LC_OperData.ARTPtr = LC_UnitTestART;
        LC_OperData.WDTPtr = LC_UnitTestWDT;
        LC_OperData.ADTPtr = LC_UnitTestADT;  
            
        Status = CFE_SUCCESS;
        CFE_TBL_GetAddressCallCount++;
    }

    return(Status);    
   
} /* end CFE_TBL_GetAddress_FailOnN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_GetAddress that will return table     */
/* updated when called a certain number of times (set by global    */
/* variable)                                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_GetAddress_UpdateOnN (void             **TblPtr,
                                    CFE_TBL_Handle_t TblHandle)
{
    int32 Status;
    
    /* 
    ** Since this function hook's argument list has to match
    ** CFE_TBL_GetAddress, we use two global variables that are set 
    ** by the calling routine to control when we return an error
    */
    if (CFE_TBL_GetAddressCallCount == CFE_TBL_GetAddressUpdateCount)
    {
        /*
        ** Update all the apps table pointers to our local test
        ** buffers just to be sure they're all set right
        */
        LC_OperData.WRTPtr = LC_UnitTestWRT;
        LC_OperData.ARTPtr = LC_UnitTestART;
        LC_OperData.WDTPtr = LC_UnitTestWDT;
        LC_OperData.ADTPtr = LC_UnitTestADT;
        
        Status = CFE_TBL_INFO_UPDATED;
    }
    else
    {
        /*
        ** Update all the apps table pointers to our local test
        ** buffers just to be sure they're all set right
        */
        LC_OperData.WRTPtr = LC_UnitTestWRT;
        LC_OperData.ARTPtr = LC_UnitTestART;
        LC_OperData.WDTPtr = LC_UnitTestWDT;
        LC_OperData.ADTPtr = LC_UnitTestADT;
            
        Status = CFE_SUCCESS;
        CFE_TBL_GetAddressCallCount++;
    }

    return(Status);    
   
} /* end CFE_TBL_GetAddress_UpdateOnN */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_GetAddress that will set our table    */
/* pointers to our local test buffers                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_GetAddress_Hook (void             **TblPtr,
                               CFE_TBL_Handle_t TblHandle)
{
    /*
    ** Update all the apps table pointers to our local test
    ** buffers
    */
    LC_OperData.WRTPtr = LC_UnitTestWRT;
    LC_OperData.ARTPtr = LC_UnitTestART;
    LC_OperData.WDTPtr = LC_UnitTestWDT;
    LC_OperData.ADTPtr = LC_UnitTestADT;

    return(CFE_SUCCESS);    
   
} /* end CFE_TBL_GetAddress_Hook */

