/************************************************************************
** File:
**   $Id: utf_test_sc.c 1.9 2015/03/02 12:59:09EST sstrege Exp  $
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
**   This is a test driver used to unit test the CFS Stored Command (SC)
**   Application.
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
**   $Log: utf_test_sc.c  $
**   Revision 1.9 2015/03/02 12:59:09EST sstrege 
**   Added copyright information
**   Revision 1.8 2010/09/28 10:25:16EDT lwalling 
**   Update header file list, add prototypes for some SC functions
**   Revision 1.7 2010/05/18 14:10:00EDT lwalling 
**   Change entry pointers to entry index, remove ats/rts pad words
**   Revision 1.6 2010/03/26 18:04:45EDT lwalling 
**   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
**   Revision 1.5 2010/03/11 10:20:54EST lwalling 
**   Remove include of obsolete header file - utf_osapiarch.h
**   Revision 1.4 2010/03/09 15:12:05EST lwalling 
**   Change CDS cfg definition from ifdef or ifndef to if true or if false
**   Revision 1.3 2009/03/03 07:53:38EST nyanchik 
**   Check in of ATS unit test tables
**   Revision 1.2 2009/02/19 10:07:22EST nyanchik 
**   Update SC To work with cFE 5.2 Config parameters
**   Revision 1.1 2009/01/26 14:20:15EST nyanchik 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sc/fsw/unit_test/project.pj
** 
*************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
        /* cFE headers         */

#include "utf_custom.h"        /* UTF headers         */
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osapi.h"
#include "utf_cfe.h"
/* #include "utf_osapiarch.h" */


#include "sc_app.h"
#include "sc_cmds.h"
#include "sc_state.h"
#include "sc_atsrq.h"
#include "sc_rtsrq.h"
#include "sc_utils.h"
#include "sc_loads.h"
#include "sc_msgids.h"

#include "utf_test_sc_ats.h"

#include <sys/fcntl.h>         /* System headers      */
#include <unistd.h>
#include <stdlib.h>

void SC_AppMain (void);
int32 SC_AppInit(void);
int32 SC_RegisterTablesNoCDS(void);
int32 SC_RegisterDumpOnlyTables(void);
int32 SC_GetTableAddresses(void);

int32 CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, uint32 MsgId, uint16 CommandCode, uint32 Parameter) { return(0); }

/************************************************************************
** Local function prototypes
*************************************************************************/

void    PrintHkPacket(void);
void    TimeHook(OS_time_t *time_struct);
void    UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[]);
void    SetTime(int argc,char *argv[]);
int32   CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) ;
int32   CFE_TBL_RegisterHookDumpOnlyTables( CFE_TBL_Handle_t *TblHandlePtr,                              const char *Name,                              uint32  Size,                              uint16  TblOptionFlags,                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr );

int32   CFE_TBL_RegisterHookRegisterNoCDS( CFE_TBL_Handle_t *TblHandlePtr,                              const char *Name,                              uint32  Size,                              uint16  TblOptionFlags,                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr );
/************************************************************************
** Macro Definitions
*************************************************************************/
#define MESSAGE_FORMAT_IS_CCSDS

#define SC_CMD_PIPE		  1

/************************************************************************
** CS global data
*************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Program main                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int main(void)
{
    
    char                        AppName[10];
    int32                       SC_AppId = 0;
    uint16                      RtsBuffer[SC_RTS_BUFF_SIZE];
    
    strcpy(AppName, "SC_APP");
    
    /*
    ** Set up to read in script
    */
    
    UTF_add_input_file(SC_CMD_PIPE, "utf_test_cmds_sc.in");
       
    UTF_add_volume("/", "cf",  FS_BASED, FALSE, FALSE, TRUE, "CF",  "/cf", 0);
    UTF_add_volume("/", "ram", FS_BASED, FALSE, FALSE, TRUE, "RAM", "/ram", 0);

    UTF_set_output_filename("utf_test_sc.out");
    
    
    UTF_add_special_command("LOAD_TABLE_FROM_GROUND", UTF_SCRIPT_LoadTableFromGround);
    UTF_add_special_command("SET_SB_RETURN_CODE",     UTF_SCRIPT_SB_Set_Api_Return_Code);
    UTF_add_special_command("SET_TIME",               SetTime);
    
    /*
     ** Initialize time data structures
     */
    UTF_init_sim_time(0.0);
    UTF_OSAPI_set_function_hook(OS_GETLOCALTIME_HOOK, TimeHook);    
    
     /*
     ** Register app SC with executive services.                         
     */
    UTF_ES_InitAppRecords();
    UTF_ES_AddAppRecord(AppName,SC_AppId);  
    
     /*
     ** Initialize table services data structures, though we
     ** don't use any tables for these tests
     */
    CFE_ES_CDS_EarlyInit();
    CFE_TBL_EarlyInit();

    
    SC_AppMain();

    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_RCVMSG_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);

    
    /* Now that most of the code is taken care of, we need to take care of the 
       miscelaneous error conditions */
    
    /********************* sc_state.c **********************/ 
     
     /* Copy the command to the end of the buffer, except for the last byte */
     /* The second command will run of the end of the buffer */    

    uint16                      RtsCommand [] = {
        0x0000, 0x18A9, 0xC000, 0x0005, 0x0400, 0x0000, 0x0000,
        0x0000, 0x18A9, 0xC000, 0x0005, 0x0400, 0x0001, 0x0000 };

     
     SC_OperData.RtsTblAddr[0] = RtsBuffer;
     SC_OperData.RtsCtrlBlckAddr -> RtsNumber = 1;
     SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
     
     bzero(RtsBuffer, sizeof(RtsBuffer));
         
     SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = 135;
     memcpy (&SC_OperData.RtsTblAddr[0][135], RtsCommand, sizeof(RtsCommand) -4); 
    
     SC_GetNextRtsCommand();

     /*********************************************************/  

    /* Have a command that who's message length is > SC_MAX_PACKET_SIZE */
    uint16 RtsCommand2 [] = {
        0x0000, 0x18A9, 0xC000, 0x00FF, 0x0400, 0x0000, 0x0000,
        0x0000, 0x18A9, 0xC000, 0x0005, 0x0400, 0x0001, 0x0000 };
          
     SC_OperData.RtsTblAddr[0] = RtsBuffer;
     SC_OperData.RtsCtrlBlckAddr -> RtsNumber = 1;
     SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
     
     bzero(RtsBuffer, sizeof(RtsBuffer));
         
     SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = 0;
     memcpy (SC_OperData.RtsTblAddr[0], RtsCommand2, sizeof(RtsCommand2) -4); 
    
     SC_GetNextRtsCommand();   
     /*********************************************************/  
     
     /********************* sc_rtsrq.c **********************/ 
     
     
     /* do a Start RTS command where the RTS has an invalid length */
     uint16 RtsCommand3 [] = {
        0x0001, 0x18A9, 0xC000, 0x00FF, 0x008F };
        
     SC_RtsCmd_t  StartCmd;
     
     CFE_SB_InitMsg(&StartCmd, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);    
     StartCmd.RtsId = 1; 
     
          
     SC_OperData.RtsTblAddr[0] = RtsBuffer;
     SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_LOADED;
     SC_OperData.RtsInfoTblAddr[0].DisabledFlag = FALSE;
     
     bzero(RtsBuffer, sizeof(RtsBuffer));
         
     SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = 0;
     memcpy (SC_OperData.RtsTblAddr[0], RtsCommand3, sizeof(RtsCommand3) ); 
    
    SC_StartRtsCmd ((CFE_SB_MsgPtr_t) &StartCmd);
     /********************* sc_loads.c **********************/ 

    
    /* Test for invalid conditions on SC_LoadAts. Normally, these 
       error conditions are caught in validation, but if something gets corrupted....
       These ATS buffers are defined in the header file, and are virtually identical to those
       in the table files that were used for validtion testing */

    SC_OperData.AtsTblAddr[0] = AtsBuffer1;
    SC_LoadAts(0);

    SC_OperData.AtsTblAddr[0] = AtsBuffer2;
    SC_LoadAts(0);


    SC_OperData.AtsTblAddr[0] = AtsBuffer3;
    SC_LoadAts(0);

    SC_OperData.AtsTblAddr[0] = AtsBuffer4;
    SC_LoadAts(0);

       
    /********************* sc_cmds.c **********************/ 
    
    
    /* ProcessRTP Command */
         /* do a Start RTS command where the RTS has an invalid length */
     uint16 RtsCommand4 [] = 
     {   0x0001, 0x18A9, 0xC000, 0x0001, 0x008F};
 
     SC_AppData.NextCmdTime[SC_RTP] = 0;
     SC_AppData.CurrentTime =0;
     SC_AppData.NextProcNumber = SC_RTP;
     SC_OperData.RtsCtrlBlckAddr -> RtsNumber = 1;     
     SC_OperData.RtsTblAddr[0] = RtsBuffer;
     SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
     SC_OperData.RtsInfoTblAddr[0].DisabledFlag = FALSE;
     
     bzero(RtsBuffer, sizeof(RtsBuffer));
         
     SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = 0;
     memcpy (SC_OperData.RtsTblAddr[0], RtsCommand4, sizeof(RtsCommand4) ); 
    
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SENDMSG_PROC, -1);
    SC_ProcessRtpCommand(); 
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SENDMSG_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    /*********************************************************/  

     /* Run the same command to get the checksum to fail */
     uint16 RtsCommand5 [] = 
     {   0x0001, 0x18A9, 0xC000, 0x0001, 0x00FF};
 
     SC_AppData.NextCmdTime[SC_RTP] = 0;
     SC_AppData.CurrentTime =0;
     SC_AppData.NextProcNumber = SC_RTP;
     SC_OperData.RtsCtrlBlckAddr -> RtsNumber = 1;     
     SC_OperData.RtsTblAddr[0] = RtsBuffer;
     SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
     SC_OperData.RtsInfoTblAddr[0].DisabledFlag = FALSE;
     
     bzero(RtsBuffer, sizeof(RtsBuffer));
         
     SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = 0;
     memcpy (SC_OperData.RtsTblAddr[0], RtsCommand5, sizeof(RtsCommand5) );   

    SC_ProcessRtpCommand(); 
    /*********************************************************/  

    /* Get Error conditions from SC_ProcessAtpCmd */    
    
    uint16 AtsCommand1[] = 
    /* inline switch command */
    { 0x0001, 0x0088, 0x0000, 0x18A9, 0xC000, 0x0001, 0x0887 };
    
    SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 0; 
    SC_AppData.AtsCmdIndexBuffer[0] [0] = 0;
    SC_OperData.AtsCmdStatusTblAddr[0][0] = SC_LOADED;
    SC_OperData.AtsTblAddr[0] = AtsCommand1;
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 0; /* new ATS not loaded */
    
    /* test the inline switch call failing */
    SC_ProcessAtpCmd();
    /*********************************************************/  

  
    /* The SB has trouble sending out the command */
    
        uint16 AtsCommand2[] = 
    { 0x0001, 0x0080, 0x0000, 0x18A9, 0xC000, 0x0001, 0x008F };
    
    SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 0; 
    SC_AppData.AtsCmdIndexBuffer[0] [0] = 0;
    SC_OperData.AtsCmdStatusTblAddr[0][0] = SC_LOADED;
    SC_OperData.AtsTblAddr[0] = AtsCommand2;
    
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SENDMSG_PROC, -1);
    SC_ProcessAtpCmd(); 
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SENDMSG_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    /*********************************************************/  

    
    /* The command number in the control block doesn't match what was in the ATS */
    
    uint16 AtsCommand3[] = 
    { 0x0006, 0x0080, 0x0000, 0x18A9, 0xC000, 0x0001, 0x008F };
    
    SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 0; 
    SC_AppData.AtsCmdIndexBuffer[0] [0] = 0;
    SC_OperData.AtsCmdStatusTblAddr[0][0] = SC_LOADED;
    SC_OperData.AtsTblAddr[0] = AtsCommand3;
    
    SC_ProcessAtpCmd(); 
    /*********************************************************/  
    /* The command number in the control block doesn't match what was in the ATS  for ATS B*/
   
    uint16 AtsCommand4[] = 
    { 0x000B, 0x0080, 0x0000, 0x18A9, 0xC000, 0x0001, 0x008F,
      0x000A, 0x0080, 0x0000, 0x18A9, 0xC000, 0x0001, 0x008F };
    
     SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = SC_ATSB;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 0; 
    SC_AppData.AtsCmdIndexBuffer[1] [0] = 0;
    SC_OperData.AtsCmdStatusTblAddr[1][0] = SC_LOADED;
    SC_OperData.AtsTblAddr[1] = AtsCommand4;
    SC_ProcessAtpCmd(); 

    /*********************************************************/  
    
    /* Proccess an ATP command with a bad checksum */   
    uint16 AtsCommand5[] = 
    { 0x0001, 0x0080, 0x0000, 0x18A9, 0xC000, 0x0001, 0x00FF };
    
     SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = SC_ATSA;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 0; 
    SC_AppData.AtsCmdIndexBuffer[0] [0] = 0;
    SC_OperData.AtsCmdStatusTblAddr[0][0] = SC_LOADED;
    SC_OperData.AtsTblAddr[0] = AtsCommand5;
        
        
    SC_ProcessAtpCmd(); 
    /*********************************************************/  
    
    /* Test an invalid message ID */
     uint16 RtsCommand6 [] = 
     {   0x0001, 0xFFFF, 0xC000, 0x0001, 0x00FF};
    
    SC_ProcessRequest( (CFE_SB_MsgPtr_t ) & RtsCommand6);
    
    /*********************************************************/  

    /* Test error conditions of SC_GetTableAddresses */
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, -1);
    SC_GetTableAddresses();
    UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    
    
    
    /********************* sc_atsrq.c **********************/ 
    
    /* Test with having an Idle Atp */
    SC_AppData.NextCmdTime[SC_ATP] = 1;
    SC_AppData.CurrentTime = 0;
    
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_IDLE;
    SC_ServiceSwitchPend();
    /*********************************************************/  

    /* Test where the ATS to switch has no command in it */
     SC_AppData.NextCmdTime[SC_ATP] = 1;
    SC_AppData.CurrentTime = 0;
    SC_OperData.AtsCtrlBlckAddr ->AtsNumber =  1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 0; 
    SC_OperData.AtsCtrlBlckAddr -> AtpState = SC_EXECUTING;
    SC_ServiceSwitchPend();
    
    /*********************************************************/  
    /* Test where the command for the inline switched Ats commands are skipped */
    SC_OperData.AtsCtrlBlckAddr ->AtsNumber =  2;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1; 
    
    SC_AppData.AtsTimeIndexBuffer[0][0] = 0;

    ((SC_AtsEntryHeader_t *) SC_OperData.AtsTblAddr[0]) ->TimeTag1 = 0xFFFF;
    ((SC_AtsEntryHeader_t *) SC_OperData.AtsTblAddr[0]) ->TimeTag2 = 0xFFFF;




    
    SC_InlineSwitch();
    
    
        /********************* sc_app.c **********************/ 
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, -1);
    SC_AppMain();
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE); 
    
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, -1);
    SC_AppInit();
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
                              
                             
    
     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, -1);    
     SC_AppInit();
     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);

     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, -1);
     SC_AppInit();
     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_GETADDRESS_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    
    #if (SC_SAVE_TO_CDS == FALSE)
     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, -1);    
     SC_RegisterTablesNoCDS();
     UTF_CFE_TBL_Set_Api_Return_Code(CFE_TBL_REGISTER_PROC, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    #endif
      UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK,
                             (void *)&CFE_SB_SubscribeHook);
     SC_AppInit();
     SC_AppInit();
     SC_AppInit();  
     
     UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK,
                              (void *)&CFE_TBL_RegisterHookDumpOnlyTables);
     SC_RegisterDumpOnlyTables();
     SC_RegisterDumpOnlyTables();
     SC_RegisterDumpOnlyTables();
     SC_RegisterDumpOnlyTables();
     
    #if (SC_SAVE_TO_CDS == FALSE)
     UTF_TBL_set_function_hook(CFE_TBL_REGISTER_HOOK,
                              (void *)&CFE_TBL_RegisterHookRegisterNoCDS);
     SC_RegisterTablesNoCDS();
    #endif
     return 0;
         
}/* end main*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Function to simulate time incrementing                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TimeHook(OS_time_t *time_struct)
{
   /* this also works: time_struct->seconds = time_struct->seconds + 1.0; */
   /* Increment time */
   UTF_set_sim_time (UTF_get_sim_time() + 1.0);
   /* Associate new time with passed in argument */
   
   *time_struct = UTF_double_to_hwtime (UTF_get_sim_time());
} /* end TimeHook */
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Load a table from the ground                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[])
{
	char Table_Name[38], File_Name[50];
    /*	UTF_put_text("Entered UTF_SCRIPT_LoadTableFromGround\n"); */
	if (argc != 3)
	{
        UTF_error("Error: Read %d args w/script cmd LOAD_TABLE_FROM_GROUND. Expected 2.\n",
                  argc -1 );
        UTF_exit();
	}
    
	strcpy(Table_Name,argv[1]);
	strcpy(File_Name,argv[2]);
    /*	UTF_put_text("Table_Name is %s\n",Table_Name);
    	UTF_put_text("File_Name is %s\n", File_Name); */ 
	UTF_TBL_LoadTableFromGround(Table_Name, File_Name);
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Set the current Time                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SetTime(int argc,char *argv[])
{
    uint32 Time;
        
	Time = atoi(argv[1]);    
    UTF_init_sim_time(Time);

	return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_SB_Subscribe that will return an error    */
/* on every call except the first one                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) 
{
    static uint32 Count = 0;
    
    if (Count == 0 || Count == 2 || Count == 5)
    {   
        Count++;
        return (CFE_SB_MAX_MSGS_MET);
    }
    else
    {
        Count++;
        return CFE_SUCCESS;
    }
}/* end CFE_SB_SubscribeHook */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Register that will return an error    */
/* on every call except the first one for Registering Dump-only    */
/* tables                                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_TBL_RegisterHookDumpOnlyTables( CFE_TBL_Handle_t *TblHandlePtr,                            const char *Name,                            uint32  Size,                            uint16  TblOptionFlags,                            CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr )
{
    static uint32 CountTBLDumpOnly = 0;
    
    if (CountTBLDumpOnly  == 1 || CountTBLDumpOnly  == 4 || CountTBLDumpOnly  == 8 || CountTBLDumpOnly  == 13)
    {   
        CountTBLDumpOnly ++;
        return (-1);
    }
    else
    {
        CountTBLDumpOnly ++;
        return CFE_SUCCESS;
    }
}/* end CFE_TBL_RegisterHookDumpOnlyTables */    
    
#if (SC_SAVE_TO_CDS == FALSE)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Hook function for CFE_TBL_Register that will return an error    */
/* on every call except the first one for Registering tables       */
/* without the CDS                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32   CFE_TBL_RegisterHookRegisterNoCDS( CFE_TBL_Handle_t *TblHandlePtr,                              const char *Name,                              uint32  Size,                              uint16  TblOptionFlags,                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr ) 
{
    static uint32 CountTBLRegisterCDS = 0;


        CountTBLRegisterCDS ++;
    if (CountTBLRegisterCDS >= SC_NUMBER_OF_RTS + SC_NUMBER_OF_ATS)
    {
        return (-1);
    }
    else
    {

        return CFE_SUCCESS;
    }
}/* end CFE_TBL_RegisterHookRegisterNoCDS */  
#endif  
    