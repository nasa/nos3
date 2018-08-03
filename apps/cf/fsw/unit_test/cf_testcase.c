/*
 * Filename: cf_testcase.c
 *
 *  Copyright � 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement. 
 *
 * Purpose: This file contains a unit test case for the cf application 
 * 
 */

/*
 * Includes
 */


#include "cfe.h"
#include "utassert.h"
#include "uttest.h"
#include "utlist.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_msgids.h"

#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"
#include "ut_cfe_es_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "cfdp_provides.h"/* for cfdp_cycle_each_transaction */

#include "cf_playback.h"
#include "cf_callbacks.h"


void CF_Setup(void);
void CF_Teardown(void);

/* prevents the following compiler error when misc.h is #included */
/* ../src/PRI/misc.h:70: error: expected ‘)’ before ‘*’ token     */
void misc__set_trans_seq_num (u_int_4 value);


extern cf_config_table_t                CF_ConfigTable;
extern Ut_CFE_FS_ReturnCodeTable_t      Ut_CFE_FS_ReturnCodeTable[UT_CFE_FS_MAX_INDEX];
extern Ut_OSFILEAPI_HookTable_t         Ut_OSFILEAPI_HookTable;
extern Ut_CFE_ES_HookTable_t            Ut_CFE_ES_HookTable;
extern Ut_OSAPI_HookTable_t             Ut_OSAPI_HookTable;


typedef struct
{
  uint8         CHdr[CFE_SB_CMD_HDR_SIZE];//CCSDS Header
  CF_PDU_Hdr_t  PHdr;//PDU Hdr
  uint8         Data[200];
}CF_PDU_Msg_t;



/*******************************************************************************
**
**  CF Custom Hook Functions needed for unit test
**
*******************************************************************************/

uint32 CFE_ES_GetPoolBufHookCallCnt;
int32 CFE_ES_GetPoolBufHook(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size);
int32 CFE_ES_GetPoolBufHook(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size)
{
    uint32  Offset;
    uint8   *BytePtr;
    
    Offset = (CFE_ES_GetPoolBufHookCallCnt * sizeof (CF_QueueEntry_t));
    
    BytePtr = (uint8 *)&CF_AppData.Mem.Partition;
  
    BytePtr += Offset;
    
    *BufPtr = (uint32 *)BytePtr;

    CFE_ES_GetPoolBufHookCallCnt++;

    return Size;
}


int32 OS_statHook(const char *path, os_fstat_t *filestats);
int32 OS_statHook(const char *path, os_fstat_t *filestats)
{

    filestats->st_size = 123;
    
    return OS_FS_SUCCESS;
}

int32 OS_FDGetInfoHook (int32 filedes, OS_FDTableEntry *fd_prop);
int32 OS_FDGetInfoHook (int32 filedes, OS_FDTableEntry *fd_prop)
{
    
    strcpy(fd_prop->Path,"/cf/testfile.txt");

    return OS_FS_SUCCESS;
}


static uint32 ReaddirHookCallCnt;
os_dirent_t   ReaddirHookDirEntry;

os_dirent_t *  OS_readdirHook (os_dirp_t directory);
os_dirent_t *  OS_readdirHook (os_dirp_t directory)
{
       
    ReaddirHookCallCnt++;
    
    if(ReaddirHookCallCnt == 1){
    
      strcpy(ReaddirHookDirEntry.d_name,".");
    
    }else if(ReaddirHookCallCnt == 2){
    
      strcpy(ReaddirHookDirEntry.d_name,"..");
    
    }else if(ReaddirHookCallCnt == 3){
      
      strcpy(ReaddirHookDirEntry.d_name,"filename1.txt");
      ReaddirHookDirEntry.d_type = DT_DIR;
    
    }else if(ReaddirHookCallCnt == 4){
      
      strcpy(ReaddirHookDirEntry.d_name,"ThisFilenameIsTooLongItExceeds64ThisFilenameIsTooLongItIs65charas");
      ReaddirHookDirEntry.d_type = 5;
    
    }else if(ReaddirHookCallCnt == 5){
      
      strcpy(ReaddirHookDirEntry.d_name, "ThisFilenameIsTooLongWhenThePathIsAttachedToIt.ItIs63Characters");
                                          
    }else if(ReaddirHookCallCnt == 6){
      
      strcpy(ReaddirHookDirEntry.d_name,"testfile.txt");
    
    }else if(ReaddirHookCallCnt == 7){
      
      strcpy(ReaddirHookDirEntry.d_name,"filename5.txt");
    
    }else{
      return NULL;
    }
    
    return &ReaddirHookDirEntry;
}


/*******************************************************************************
**
**  CF Test Utilities needed for unit test
**
*******************************************************************************/

int32 CF_TstUtil_VerifyListOrder(char *OrderGiven);
int32 CF_TstUtil_VerifyListOrder(char *OrderGiven)
{
    CF_QueueEntry_t   *PtrToEntry;
    char              Buf[64];
    uint32            i=0;

    PtrToEntry = CF_AppData.Chan[0].PbQ[CF_PB_PENDINGQ].HeadPtr;            
    while(PtrToEntry != NULL)
    {                
        sprintf(&Buf[i],"%d",(int)PtrToEntry->TransNum);
        PtrToEntry = PtrToEntry->Next;
        i++;
    }
    
    if(strncmp(OrderGiven,Buf,64)==0)
        return CF_SUCCESS;
    else{
      printf("VerfiyList is comparing given %s with %s\n",OrderGiven,Buf);
      return CF_ERROR;
    }
}



void CF_TstUtil_CreateOnePendingQueueEntry(void);
void CF_TstUtil_CreateOnePendingQueueEntry(void)
{
  CF_PlaybackFileCmd_t      PbFileCmdMsg;

    /* reset CF globals etc */
  CF_AppInit();  
  
  /* reset the transactions seq number used by the engine */
  misc__set_trans_seq_num(1);
  
  /* Execute a playback file command so that one queue entry is added to the pending queue */
  CFE_SB_InitMsg(&PbFileCmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&PbFileCmdMsg, CF_PLAYBACK_FILE_CC);
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 0;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "2.25");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* execute the playback file cmd to get a queue entry on the pending queue */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  
}/* CF_TstUtil_CreateOnePendingQueueEntry */


void CF_TstUtil_CreateOnePbActiveQueueEntry(void);
void CF_TstUtil_CreateOnePbActiveQueueEntry(void)
{
  
  CF_TstUtil_CreateOnePendingQueueEntry();
  
  /* Force OS_stat to return a valid size and success */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &OS_statHook);
  
  CF_StartNextFile(0);
  
}/* CF_TstUtil_CreateOnePbActiveQueueEntry */


void CF_TstUtil_CreateOnePbHistoryQueueEntry(void);
void CF_TstUtil_CreateOnePbHistoryQueueEntry(void)
{

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ABANDON_CC);
  strcpy(CmdMsg.Trans,"All");

  CF_TstUtil_CreateOnePbActiveQueueEntry();
  
  cfdp_cycle_each_transaction();
  
  /* Send Abandon Cmd */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
    
  cfdp_cycle_each_transaction();
  
  cfdp_cycle_each_transaction();

  
}/* CF_TstUtil_CreateOnePbHistoryQueueEntry */



void CF_TstUtil_CreateOneUpActiveQueueEntry(void);
void CF_TstUtil_CreateOneUpActiveQueueEntry(void)
{
  INDICATION_TYPE IndType = IND_MACHINE_ALLOCATED;
  TRANS_STATUS TransInfo;
  
    /* reset CF globals etc */
  CF_AppInit();  
  
  TransInfo.role =  CLASS_1_RECEIVER;
  TransInfo.trans.number = 500;
  TransInfo.trans.source_id.value[0] = 0;
  TransInfo.trans.source_id.value[1] = 23;
  
  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  CF_Indication (IndType,TransInfo);
  
}/* CF_TstUtil_CreateOneUpActiveQueueEntry */


void CF_TstUtil_CreateOneUpHistoryQueueEntry(void);
void CF_TstUtil_CreateOneUpHistoryQueueEntry(void)
{
  INDICATION_TYPE IndType = IND_MACHINE_DEALLOCATED;
  TRANS_STATUS TransInfo;
  
  CF_TstUtil_CreateOneUpActiveQueueEntry();
  
  TransInfo.role =  CLASS_1_RECEIVER;
  TransInfo.trans.number = 500;
  TransInfo.trans.source_id.value[0] = 0;
  TransInfo.trans.source_id.value[1] = 23;
  TransInfo.final_status = FINAL_STATUS_SUCCESSFUL;  
  strcpy(TransInfo.md.dest_file_name,"/ram/uploadedfile.txt");
  
  CF_Indication (IndType,TransInfo);
  
}/* CF_TstUtil_CreateOneUpHistoryQueueEntry */


void CF_ResetEngine(void);
void CF_ResetEngine(void){

  CF_CARSCmd_t    CmdMsg;
  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ABANDON_CC);
  strcpy(CmdMsg.Trans,"All");

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  cfdp_cycle_each_transaction();
  
  misc__set_trans_seq_num (1);
  
}




/*******************************************************************************
**
**  CF AppInit Tests
**
*******************************************************************************/



void Test_CF_AppInit_EVSRegFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 
  UtPrintf("\n");/* needed because sys log msg has no EOL */
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_NoEventSent("Verifying No Event was sent");
    
}/* end Test_CF_AppInit_EVSRegFail */


void Test_CF_AppInit_CrPipeFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_CR_PIPE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_CrPipeFail */


void Test_CF_AppInit_Sub1Fail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_SUB_REQ_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_Sub1Fail */



void Test_CF_AppInit_Sub2Fail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_SUB_CMD_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_Sub2Fail */



void Test_CF_AppInit_Sub3Fail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_SUB_WAKE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_Sub3Fail */


void Test_CF_AppInit_Sub4Fail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 4);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_SUB_PDUS_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_Sub4Fail */


void Test_CF_AppInit_TblRegFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_CFGTBL_REG_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_TblRegFail */



void Test_CF_AppInit_TblLoadFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_CFGTBL_LD_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_TblLoadFail */


void Test_CF_AppInit_TblManageFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_MANAGE_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_CFGTBL_MNG_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_TblManageFail */



void Test_CF_AppInit_TblGetAdrFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_CFGTBL_GADR_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppInit_TblGetAdrFail */


void Test_CF_AppInit_PoolCreateExFail(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = -1;
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_POOLCREATEEX_INDEX, -1, 1);
 
  /* Execute Test */
  ActRtn = CF_AppInit(); 

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==0,"Event Count = 0");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_NoEventSent("Verifying No Event was sent");
    
}/* end Test_CF_AppInit_PoolCreateExFail */


void Test_CF_AppInit_SendEventFail(void){

  int32 ExpRtn,ActRtn,NumEventsExp;

  /* Setup Inputs */
  NumEventsExp = 8;/* cfdp debug events for set mib calls */
  ExpRtn = -1;
  Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_SENDEVENT_INDEX, -1, (NumEventsExp + 1));
 
  /* Execute Test */
  ActRtn = CF_AppInit();
  
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==8,"Event Count = 8");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
    
}/* end Test_CF_AppInit_SendEventFail */


void Test_CF_AppInit_NoErrors(void){

  int32 ExpRtn,ActRtn;

  /* Setup Inputs */
  ExpRtn = CFE_SUCCESS;
 
  /* Execute Test */
  ActRtn = CF_AppInit();

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==9,"Event Count = 9");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
    
}/* end Test_CF_AppInit_NoErrors */



/*******************************************************************************
**
**  CF Table Validation Tests
**
*******************************************************************************/

void Test_CF_TblValFlightEntityIdFail(void){

  int32 ExpRtn,ActRtn;
  char  ValidFlightEntityId[CF_MAX_CFG_VALUE_CHARS];

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  /* store the valid flight entity ID to be restored later */
  strncpy(ValidFlightEntityId,CF_AppData.Tbl->FlightEntityId,CF_MAX_CFG_VALUE_CHARS);
  /* Corrupt table param */  
  strncpy(CF_AppData.Tbl->FlightEntityId,"1234567890", CF_MAX_CFG_VALUE_CHARS);
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_TBL_VAL_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  
  /* restore tbl param */
  strncpy(CF_AppData.Tbl->FlightEntityId,ValidFlightEntityId,CF_MAX_CFG_VALUE_CHARS);
  
}/* end Test_CF_TblValFlightEntityIdFail */


void Test_CF_TblValIncomingMsgIdFail(void){

  int32 ExpRtn,ActRtn;
  CFE_SB_MsgId_t ValidIncomingPDUMsgId;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidIncomingPDUMsgId = CF_AppData.Tbl->InCh[0].IncomingPDUMsgId;
  /* Corrupt table param */  
  CF_AppData.Tbl->InCh[0].IncomingPDUMsgId = 0xFFFF;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->InCh[0].IncomingPDUMsgId = ValidIncomingPDUMsgId;
  
}/* end Test_CF_TblValIncomingMsgIdFail */


void Test_CF_TblValOutgoingFileChunkFail(void){

  int32 ExpRtn,ActRtn;
  char  ValidOutgoingFileChunkSize[CF_MAX_CFG_VALUE_CHARS];

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  /* store the valid flight entity ID to be restored later */
  strncpy(ValidOutgoingFileChunkSize,CF_AppData.Tbl->OutgoingFileChunkSize,CF_MAX_CFG_VALUE_CHARS);
  /* Corrupt table param */  
  strncpy(CF_AppData.Tbl->OutgoingFileChunkSize,"1234567890", CF_MAX_CFG_VALUE_CHARS);
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  strncpy(CF_AppData.Tbl->OutgoingFileChunkSize,ValidOutgoingFileChunkSize,CF_MAX_CFG_VALUE_CHARS);
  
}/* end Test_CF_TblValOutgoingFileChunkFail */



void Test_CF_TblValChanInUseFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidChanInUse;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidChanInUse = CF_AppData.Tbl->OuCh[0].EntryInUse;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].EntryInUse = 3;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR4_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].EntryInUse = ValidChanInUse;
  
}/* end Test_CF_TblValChanInUseFail */



void Test_CF_TblValDequeEnableFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidDequeueEnable;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidDequeueEnable = CF_AppData.Tbl->OuCh[0].DequeueEnable;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].DequeueEnable = 2;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR5_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].DequeueEnable = ValidDequeueEnable;
  
}/* end Test_CF_TblValDequeEnableFail */


void Test_CF_TblValOutgoingMsgIdFail(void){

  int32 ExpRtn,ActRtn;
  CFE_SB_MsgId_t ValidOutgoingPDUMsgId;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidOutgoingPDUMsgId = CF_AppData.Tbl->OuCh[0].OutgoingPduMsgId;
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].OutgoingPduMsgId = 0xFFFF;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR7_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].OutgoingPduMsgId = ValidOutgoingPDUMsgId;
  
}/* end Test_CF_TblValOutgoingMsgIdFail */



void Test_CF_TblValPollDirInUseFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidPollDirInUse;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidPollDirInUse = CF_AppData.Tbl->OuCh[0].PollDir[0].EntryInUse;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].PollDir[0].EntryInUse = 3;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR8_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].PollDir[0].EntryInUse = ValidPollDirInUse;
  
}/* end Test_CF_TblValPollDirInUseFail */




void Test_CF_TblValPollEnableFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidPollEnable;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidPollEnable = CF_AppData.Tbl->OuCh[0].PollDir[0].EnableState;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].PollDir[0].EnableState = 2;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR9_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].PollDir[0].EnableState = ValidPollEnable;
  
}/* end Test_CF_TblValPollEnableFail */


void Test_CF_TblValPollClassFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidPollClass;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidPollClass = CF_AppData.Tbl->OuCh[0].PollDir[0].Class;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].PollDir[0].Class = 0;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR10_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].PollDir[0].Class = ValidPollClass;
  
}/* end Test_CF_TblValPollClassFail */



void Test_CF_TblValPollPreserveFail(void){

  int32 ExpRtn,ActRtn;
  uint32 ValidPollPreserve;

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  
  /* store the valid parameter to be restored later */
  ValidPollPreserve = CF_AppData.Tbl->OuCh[0].PollDir[0].Preserve;
  
  /* Corrupt table param */  
  CF_AppData.Tbl->OuCh[0].PollDir[0].Preserve = 3;
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR11_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  CF_AppData.Tbl->OuCh[0].PollDir[0].Preserve = ValidPollPreserve;
  
}/* end Test_CF_TblValPollPreserveFail */



void Test_CF_TblValPollSrcPathFail(void){

  int32 ExpRtn,ActRtn;
  char  ValidSrcPath[OS_MAX_PATH_LEN];

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  /* store the valid flight entity ID to be restored later */
  strncpy(ValidSrcPath,CF_AppData.Tbl->OuCh[0].PollDir[0].SrcPath,OS_MAX_PATH_LEN);
  /* Corrupt table param */  
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].SrcPath,"/NoSlashAtEnd", OS_MAX_PATH_LEN);
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR12_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].SrcPath,ValidSrcPath,OS_MAX_PATH_LEN);
  
}/* end Test_CF_TblValPollSrcPathFail */


void Test_CF_TblValPollDstPathFail(void){

  int32 ExpRtn,ActRtn;
  char  ValidDstPath[OS_MAX_PATH_LEN];

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  /* store the valid flight entity ID to be restored later */
  strncpy(ValidDstPath,CF_AppData.Tbl->OuCh[0].PollDir[0].DstPath,OS_MAX_PATH_LEN);
  /* Corrupt table param */  
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].DstPath,"/No Spaces Allowed", OS_MAX_PATH_LEN);
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR13_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].DstPath,ValidDstPath,OS_MAX_PATH_LEN);
  
}/* end Test_CF_TblValPollDstPathFail */



void Test_CF_TblValPeerEntityIdFail(void){

  int32 ExpRtn,ActRtn;
  char  ValidPeerEntityId[CF_MAX_CFG_VALUE_CHARS];

  /* Setup Inputs */
  ExpRtn = CF_ERROR;
  CF_AppInit();
  /* store the valid flight entity ID to be restored later */
  strncpy(ValidPeerEntityId,CF_AppData.Tbl->OuCh[0].PollDir[0].PeerEntityId,CF_MAX_CFG_VALUE_CHARS);
  /* Corrupt table param */  
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].PeerEntityId,"12", CF_MAX_CFG_VALUE_CHARS);
  
  /* Execute Test */
  ActRtn = CF_ValidateCFConfigTable (CF_AppData.Tbl);
  
  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_TBL_VAL_ERR6_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_TBL_VAL_ERR14_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  /* restore tbl param */
  strncpy(CF_AppData.Tbl->OuCh[0].PollDir[0].PeerEntityId,ValidPeerEntityId,CF_MAX_CFG_VALUE_CHARS);
  
}/* end Test_CF_TblValPeerEntityIdFail */


/*******************************************************************************
**
**  CF AppMain Tests
**
*******************************************************************************/


void Test_CF_AppMain_InitErrors(void){

  /* Setup Inputs */
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
 
  /* Execute Test */
  CF_AppMain();

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFGTBL_GADR_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppMain_InitErrors */


void Test_CF_AppMain_SemGetIdByNameFail(void){

  /* Setup Inputs */
  Ut_OSAPI_SetReturnCode(UT_OSAPI_COUNTSEMGETIDBYNAME_INDEX, -1, 1);
  //The line below is needed to stop entry into while loop
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
 
  /* Execute Test */
  CF_AppMain();

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFGTBL_GADR_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_HANDSHAKE_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_AppMain_SemGetIdByNameFail */



void Test_CF_AppMain_RcvMsgErr(void){

  /* Setup Inputs */
  Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);
 
  /* Execute Test */
  CF_AppMain();

  /* Verify Outputs */
  UtAssert_EventSent(CF_RCV_MSG_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
    
}/* end Test_CF_AppMain_RcvMsgErr */


/* TO_AppMain_Test02 - Successful (Process Command) */
void Test_CF_AppMain_RcvMsgOkOnFirst(void)
{
    CF_NoArgsCmd_t    NoopCmd;
    int32           CmdPipe;

    /* Setup Inputs */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    CmdPipe = Ut_CFE_SB_CreatePipe("CF_CMD_PIPE");
    CFE_SB_InitMsg(&NoopCmd, CF_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&NoopCmd, CF_NOOP_CC);
    Ut_CFE_SB_AddMsgToPipe(&NoopCmd, CmdPipe);

    /* Execute Test */
    CF_AppMain();

    /* Verify Outputs */
    UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
    UtAssert_EventSent(CF_NOOP_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
    UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
    

}/* end Test_CF_AppMain_RcvMsgOkOnFirst */



/*******************************************************************************
**
**  CF Command Tests
**
*******************************************************************************/


void Test_CF_HousekeepingCmd(void){

  CF_NoArgsCmd_t  HousekeepingCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&HousekeepingCmdMsg, CF_SEND_HK_MID, sizeof(CF_NoArgsCmd_t), TRUE);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&HousekeepingCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&HousekeepingCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==0,"Event Count = 0");
  UtAssert_PacketSent(CF_HK_TLM_MID, "Housekeeping Packet Sent");

}/* end Test_CF_HousekeepingCmd */


void Test_CF_HkCmdInvLen(void){

  CF_NoArgsCmd_t  HousekeepingCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&HousekeepingCmdMsg, CF_SEND_HK_MID, 88, FALSE);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&HousekeepingCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&HousekeepingCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_HkCmdInvLen */


void Test_CF_HkCmdTblUpdated(void){

  CF_NoArgsCmd_t  HousekeepingCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&HousekeepingCmdMsg, CF_SEND_HK_MID, sizeof(CF_NoArgsCmd_t), TRUE);
  //The line below is needed to get coverage in CF_CheckForTblRequests
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_UPDATE_PENDING, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&HousekeepingCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&HousekeepingCmdMsg);

  /* Verify Outputs */  
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_TBL_LD_ATTEMPT_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_HkCmdTblUpdated */


void Test_CF_HkCmdValPending(void){

  CF_NoArgsCmd_t  HousekeepingCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&HousekeepingCmdMsg, CF_SEND_HK_MID, sizeof(CF_NoArgsCmd_t), TRUE);
    //The line below is needed to get coverage in CF_CheckForTblRequests
  Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_VALIDATION_PENDING, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&HousekeepingCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&HousekeepingCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==0,"Event Count = 0");

}/* end Test_CF_HkCmdValPending */


void Test_CF_NoopCmd(void){

  CF_NoArgsCmd_t  NoopCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&NoopCmdMsg, CF_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&NoopCmdMsg, CF_NOOP_CC); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&NoopCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&NoopCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NOOP_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_NoopCmd */


void Test_CF_NoopCmdInvLen(void){

  CF_NoArgsCmd_t  NoopCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&NoopCmdMsg, CF_CMD_MID, 1, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&NoopCmdMsg, CF_NOOP_CC); 
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&NoopCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&NoopCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_NoopCmdInvLen */


void Test_CF_WakeupCmd(void){

  CF_NoArgsCmd_t  WakeupCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&WakeupCmdMsg, CF_WAKE_UP_REQ_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&WakeupCmdMsg;  
  // need to call at least 4 times to get coverage of 
  // checking polling directories and pending queues
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  //fixme - add test to verify wakeup ctr increments

}/* end Test_CF_WakeupCmd */


void Test_CF_WakeupCmdPollingEnabled(void){

  CF_NoArgsCmd_t  WakeupCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&WakeupCmdMsg, CF_WAKE_UP_REQ_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);

  /* force polling to be enabled to get more coverage in playback.c */
  CF_AppData.Tbl->OuCh[0].PollDir[0].EnableState = CF_ENABLED;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&WakeupCmdMsg;  
  // need to call at least 4 times to get coverage of 
  // checking polling directories and pending queues
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_OPEN_DIR_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  //fixme - add test to verify wakeup ctr increments
  
  CF_AppData.Tbl->OuCh[0].PollDir[0].EnableState = CF_DISABLED;

}/* end Test_CF_WakeupCmdPollingEnabled */


void Test_CF_WakeupCmdInvLen(void){

  CF_NoArgsCmd_t  WakeupCmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&WakeupCmdMsg, CF_WAKE_UP_REQ_CMD_MID, 5, TRUE);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&WakeupCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&WakeupCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WakeupCmdInvLen */


void Test_CF_InPDUNoErrCmd(void){

  /* Incoming PDU tests cannot get a no errors condition because the engine states
size must be >=524 and CF states size must be less than 512. Must be some pdu 
header field wrongly set. Still get 100% coverage on CF_SendPDUToEngine */ 
  
  CF_PDU_Msg_t    IncomingPduMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&IncomingPduMsg, CF_INCOMING_PDU_MID, sizeof(CF_PDU_Msg_t), TRUE);
  IncomingPduMsg.PHdr.Octet1 = 0x04;//file directive,toward rcvr,class1,crc not present
  IncomingPduMsg.PHdr.PDataLen = 0xC8;//pdu data field size
  IncomingPduMsg.PHdr.Octet4 = 0x13;// hex 1 - length entityID is 2, hex 3 - length xact seq is 4
  IncomingPduMsg.PHdr.SrcEntityId = 0x0017;//0.23
  IncomingPduMsg.PHdr.TransSeqNum = 1;
  IncomingPduMsg.PHdr.DstEntityId = 0x0018;//0.24

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&IncomingPduMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&IncomingPduMsg);


  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_PDU_RCV_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InPDUNoErrCmd */



void Test_CF_InPDUTlmPktCmd(void){

  CF_PDU_Msg_t    IncomingPduMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&IncomingPduMsg, 0x08FD, sizeof(CF_PDU_Msg_t), TRUE);
  IncomingPduMsg.PHdr.Octet1 = 0x04;//file directive,toward rcvr,class1,crc not present
  IncomingPduMsg.PHdr.PDataLen = 0xC8;//pdu data field size
  IncomingPduMsg.PHdr.Octet4 = 0x13;// hex 1 - length entityID is 2, hex 3 - length xact seq is 4
  IncomingPduMsg.PHdr.SrcEntityId = 0x0017;//0.23
  IncomingPduMsg.PHdr.TransSeqNum = 1;
  IncomingPduMsg.PHdr.DstEntityId = 0x0018;//0.24

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&IncomingPduMsg;   
  CF_SendPDUToEngine((CFE_SB_MsgPtr_t)&IncomingPduMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_PDU_RCV_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InPDUTlmPktCmd */



void Test_CF_InPDUHdrSizeErrCmd(void){

  int32           ExpAppInitRtn,ActAppInitRtn;
  CF_PDU_Msg_t    IncomingPduMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&IncomingPduMsg, CF_INCOMING_PDU_MID, sizeof(CF_PDU_Msg_t), TRUE);
  IncomingPduMsg.PHdr.Octet1 = 0x04;//file directive,toward rcvr,class1,crc not present
  IncomingPduMsg.PHdr.PDataLen = 0x00;//pdu data field
  IncomingPduMsg.PHdr.Octet4 = 0x24;// hex 2 - length entityID is 3, hex 4 - length xact seq is 5
  IncomingPduMsg.PHdr.SrcEntityId = 0x0017;//0.23
  IncomingPduMsg.PHdr.TransSeqNum = 1;
  IncomingPduMsg.PHdr.DstEntityId = 0x0018;//0.24

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit(); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&IncomingPduMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&IncomingPduMsg);


  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PDU_RCV_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InPDUHdrSizeErrCmd */




void Test_CF_InPDUTooBigCmd(void){

  int32           ExpAppInitRtn,ActAppInitRtn;
  CF_PDU_Msg_t    IncomingPduMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&IncomingPduMsg, CF_INCOMING_PDU_MID, sizeof(CF_PDU_Msg_t), TRUE);
  IncomingPduMsg.PHdr.Octet1 = 0x04;//file directive,toward rcvr,class1,crc not present
  IncomingPduMsg.PHdr.PDataLen = 0xFA0;//pdu data field - 4000 bytes
  IncomingPduMsg.PHdr.Octet4 = 0x13;//1 - length entityID is 2, 3 - length xact seq is 4
  IncomingPduMsg.PHdr.SrcEntityId = 0x0017;//0.23
  IncomingPduMsg.PHdr.TransSeqNum = 1;
  IncomingPduMsg.PHdr.DstEntityId = 0x0018;//0.24

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit(); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&IncomingPduMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&IncomingPduMsg);


  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PDU_RCV_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InPDUTooBigCmd */



void Test_CF_InPDUTooSmallCmd(void){

  int32           ExpAppInitRtn,ActAppInitRtn;
  CF_PDU_Msg_t    IncomingPduMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&IncomingPduMsg, CF_INCOMING_PDU_MID, sizeof(CF_PDU_Msg_t), TRUE);
  IncomingPduMsg.PHdr.Octet1 = 0x04;//file directive,toward rcvr,class1,crc not present
  IncomingPduMsg.PHdr.PDataLen = 0x00;//pdu data field
  IncomingPduMsg.PHdr.Octet4 = 0x13;//1 - length entityID is 2, 3 - length xact seq is 4
  IncomingPduMsg.PHdr.SrcEntityId = 0x0017;//0.23
  IncomingPduMsg.PHdr.TransSeqNum = 1;
  IncomingPduMsg.PHdr.DstEntityId = 0x0018;//0.24

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit(); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&IncomingPduMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&IncomingPduMsg);


  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_PDU_RCV_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  
  
}/* end Test_CF_InPDUTooSmallCmd */


void Test_CF_RstCtrsCmd(void){

  CF_ResetCtrsCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_ResetCtrsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_RESET_CC);
  CmdMsg.Value = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_RESET_CMD_EID, CFE_EVS_DEBUG, "", "DEBUG Event Sent");

}/* end Test_CF_RstCtrsCmd */



void Test_CF_RstCtrsCmdInvLen(void){

  CF_ResetCtrsCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 88, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_RESET_CC);
  CmdMsg.Value = 0;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_RstCtrsCmdInvLen */


void Test_CF_FreezeCmd(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_FREEZE_CC); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_FREEZE_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_FreezeCmd */


void Test_CF_FreezeCmdInvLen(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 3, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_FREEZE_CC); 
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_FreezeCmdInvLen */


void Test_CF_ThawCmd(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_THAW_CC); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_THAW_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_ThawCmd */


void Test_CF_ThawCmdInvLen(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 10, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_THAW_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_ThawCmdInvLen */


void Test_CF_SuspendTransIdCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"0.24_56");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_SuspendTransIdCmd*/


void Test_CF_SuspendTransIdCmdInvLen(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 6, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"0.24_56");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SuspendTransIdCmdInvLen*/


void Test_CF_SuspendFilenameCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"/cf/file.txt");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CARS_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_SuspendFilenameCmd */


void Test_CF_SuspendInvFilenameCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"/cf/    file.txt");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_SuspendInvFilenameCmd */


void Test_CF_SuspendCmdInvLen(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 55, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"/cf/file.txt");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_SuspendCmdInvLen */


void Test_CF_SuspendUntermStrgCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);  
  CFE_PSP_MemSet  (CmdMsg.Trans,0xFF, OS_MAX_PATH_LEN);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_SuspendUntermStrgCmd */


void Test_CF_SuspendAllCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SUSPEND_CC);
  strcpy(CmdMsg.Trans,"All");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CARS_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_SuspendAllCmd*/


void Test_CF_ResumeCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_RESUME_CC);
  strcpy(CmdMsg.Trans,"0.24_56");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_ResumeCmd */


void Test_CF_ResumeAllCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_RESUME_CC);
  strcpy(CmdMsg.Trans,"All");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CARS_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_ResumeAllCmd */



void Test_CF_CancelCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_CANCEL_CC);
  strcpy(CmdMsg.Trans,"0.24_56");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_CancelCmd */


void Test_CF_CancelAllCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_CANCEL_CC);
  strcpy(CmdMsg.Trans,"All");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CARS_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_CancelAllCmd */



void Test_CF_AbandonCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ABANDON_CC);
  strcpy(CmdMsg.Trans,"0.24_56");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_AbandonCmd */


void Test_CF_AbandonAllCmd(void){

  CF_CARSCmd_t    CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ABANDON_CC);
  strcpy(CmdMsg.Trans,"All");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CARS_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");

}/* end Test_CF_AbandonAllCmd */



void Test_CF_SetMibParamCmd(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"save_incomplete_files");
  strcpy(CmdMsg.Value,"yes");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibParamCmd */


void Test_CF_SetMibParamCmdInvLen(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 88, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"save_incomplete_files");
  strcpy(CmdMsg.Value,"yes");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetMibParamCmdInvLen */


void Test_CF_SetMibCmdUntermParam(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  CFE_PSP_MemSet  (CmdMsg.Param,0xFF, CF_MAX_CFG_PARAM_CHARS);
  strcpy(CmdMsg.Value,"yes");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent\n");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetMibCmdUntermParam */


void Test_CF_SetMibCmdUntermValue(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"save_incomplete_files");
  CFE_PSP_MemSet(CmdMsg.Value,0xFF, CF_MAX_CFG_VALUE_CHARS);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetMibCmdUntermValue */



void Test_CF_SetMibCmdFileChunkOverLimit(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"OUTGOING_FILE_CHUNK_SIZE");
  strcpy(CmdMsg.Value,"10000");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SET_MIB_CMD_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent\n");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetMibCmdFileChunkOverLimit */


void Test_CF_SetMibCmdMyIdInvalid(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"MY_ID");
  strcpy(CmdMsg.Value,"10000");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SET_MIB_CMD_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent\n");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetMibCmdMyIdInvalid */


void Test_CF_SetMibCmdAckLimit(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"ACK_LIMIT");
  strcpy(CmdMsg.Value,"2");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibCmdAckLimit */


void Test_CF_SetMibCmdAckTimeout(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"ACK_TIMEOUT");
  strcpy(CmdMsg.Value,"25");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibCmdAckTimeout */


void Test_CF_SetMibCmdInactTimeout(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"INACTIVITY_TIMEOUT");
  strcpy(CmdMsg.Value,"200");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibCmdInactTimeout */


void Test_CF_SetMibCmdNakLimit(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"NAK_LIMIT");
  strcpy(CmdMsg.Value,"4");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibCmdNakLimit */


void Test_CF_SetMibCmdNakTimeout(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"NAK_TIMEOUT");
  strcpy(CmdMsg.Value,"15");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibCmdNakTimeout */


void Test_CF_SetMibFileChunkSize(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"OUTGOING_FILE_CHUNK_SIZE");
  strcpy(CmdMsg.Value,"250");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibFileChunkSize */


void Test_CF_SetMibMyId(void){

  CF_SetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_MIB_PARAM_CC);
  
  strcpy(CmdMsg.Param,"MY_ID");
  strcpy(CmdMsg.Value,"25.29");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==2,"Event Count = 2");
  UtAssert_EventSent(CF_CFDP_ENGINE_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_SET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_SetMibMyId */


void Test_CF_GetMibParamCmd(void){

  CF_GetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_GetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_GET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"save_incomplete_files");  

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_GET_MIB_CMD_EID, CFE_EVS_INFORMATION, "", "Info Event Sent\n");

}/* end Test_CF_GetMibParamCmd */


void Test_CF_GetMibParamCmdInvLen(void){

  CF_GetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 0, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_GET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"save_incomplete_files");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_GetMibParamCmdInvLen */


void Test_CF_GetMibCmdUntermParam(void){

  CF_GetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_GetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_GET_MIB_PARAM_CC);
  CFE_PSP_MemSet  (CmdMsg.Param,0xFF, CF_MAX_CFG_PARAM_CHARS);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent\n");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_GetMibCmdUntermParam */


void Test_CF_GetMibParamCmdInvParam(void){

  CF_GetMibParam_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_GetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_GET_MIB_PARAM_CC);
  strcpy(CmdMsg.Param,"save_the_bay");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CFDP_ENGINE_WARN_EID, CFE_EVS_INFORMATION, "", "Information Event Sent\n");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_GetMibParamCmdInvParam */



void Test_CF_SendCfgParamsCmd(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_CFG_PARAMS_CC); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_CFG_CMD_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_SendCfgParamsCmd */


void Test_CF_SendCfgParamsCmdInvLen(void){

  CF_NoArgsCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 20, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_CFG_PARAMS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendCfgParamsCmdInvLen */



void Test_CF_WriteQueueCmdCreatErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_QUE_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_WriteQueueCmdCreatErr */


void Test_CF_WriteQueueCmdInvLen(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 270, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WriteQueueCmdInvLen */


void Test_CF_WriteQueueUpQValueErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WR_CMD_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WriteQueueUpQValueErr */


void Test_CF_WriteQueueUpDefFilename(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 1;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);  
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_Q_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteQueueUpDefFilename */


void Test_CF_WriteQueueUpCustomFilename(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 2;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"/ram/filename.dat");
  
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_Q_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteQueueUpCustomFilename */


void Test_CF_WriteQueueOutQValueErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 4;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WR_CMD_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WriteQueueOutQValueErr */


void Test_CF_WriteQueueOutQTypeErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 3;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WR_CMD_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WriteQueueOutQTypeErr */


void Test_CF_WriteQueueOutChanErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 16;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  CF_AppData.Hk.ErrCounter = 0;

  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WR_CMD_ERR4_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_WriteQueueOutChanErr */


void Test_CF_WriteQueueOutDefFilename(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"");
  
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_Q_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteQueueOutDefFilename */


void Test_CF_WriteQueueOneEntry(void){

  CF_WriteQueueCmd_t      WrQCmdMsg;
  CF_PlaybackFileCmd_t    PbFileCmdMsg;

  /* reset CF globals etc */
  CF_AppInit();

  /* Create one queue entry */
  /* Execute a playback file command so that one queue entry is added to the pending queue */
  CFE_SB_InitMsg(&PbFileCmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&PbFileCmdMsg, CF_PLAYBACK_FILE_CC);
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 0;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "255.255");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  /* end ... Create one queue entry */

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  
  /* force the CFE_FS_WriteHdr function to return a valid byte count (96) */
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute a Write Queue Command now that we have one queue entry */
  CFE_SB_InitMsg(&WrQCmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&WrQCmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  WrQCmdMsg.Type = 2;/*(up=1/down=2)*/
  WrQCmdMsg.Chan = 0;
  WrQCmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(WrQCmdMsg.Filename,"");
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&WrQCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&WrQCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_SND_Q_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_PLAYBACK_FILE_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");  

}/* end Test_CF_WriteQueueOneEntry */



void Test_CF_WriteQueueOutCustomFilename(void){

  CF_WriteQueueCmd_t  CmdMsg;

  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"/ram/filename.dat");
  
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SND_Q_INFO_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteQueueOutCustomFilename */


void Test_CF_WriteQueueWriteHdrErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"/ram/filename.dat");
  
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_FILEWRITE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_WriteQueueWriteHdrErr */



void Test_CF_WriteQueueEntryWriteErr(void){

  CF_WriteQueueCmd_t      WrQCmdMsg;
  CF_PlaybackFileCmd_t    PbFileCmdMsg;

  /* reset CF globals etc */
  CF_AppInit();

  /* Create one queue entry */
  /* Execute a playback file command so that one queue entry is added to the pending queue */
  CFE_SB_InitMsg(&PbFileCmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&PbFileCmdMsg, CF_PLAYBACK_FILE_CC);
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 0;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "255.255");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  /* end ... Create one queue entry */

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  
  /* force the CFE_FS_WriteHdr function to return a valid byte count (96) */
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);
  
  /* force an error when writing the entry to the file */
  /* Coincidentally, entries are the same size as cfe file hdr (96), this rtns 12 */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, 12, 1);

  /* Execute a Write Queue Command now that we have one queue entry */
  CFE_SB_InitMsg(&WrQCmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&WrQCmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  WrQCmdMsg.Type = 2;/*(up=1/down=2)*/
  WrQCmdMsg.Chan = 0;
  WrQCmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(WrQCmdMsg.Filename,"");
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&WrQCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&WrQCmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_PLAYBACK_FILE_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");  
  UtAssert_EventSent(CF_FILEWRITE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_WriteQueueEntryWriteErr */





void Test_CF_WriteQueueInvFilenameErr(void){

  CF_WriteQueueCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WRITE_QUEUE_INFO_CC); 

  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */
  strcpy(CmdMsg.Filename,"/ram/fil ename.dat");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_WriteQueueInvFilenameErr */



void Test_CF_WriteActTransDefaultFilename(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* create one playback chan 0, active queue entry */
  CF_TstUtil_CreateOnePbActiveQueueEntry(); 

  /* create one uplink active queue entry */
  CF_AddFileToUpQueue(CF_UP_ACTIVEQ, (CF_QueueEntry_t *)&CF_AppData.Mem.Partition);

  /* build cmd to write all active entries to a file */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC); 
  CmdMsg.Type = 0;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"");

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  
  /* force the CFE_FS_WriteHdr function to return a valid byte count (96) */
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_PLAYBACK_FILE_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_WRACT_TRANS_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteActTransDefaultFilename */



void Test_CF_WriteActTransCustFilename(void){

  CF_WriteActiveTransCmd_t  CmdMsg;
  
  CF_TstUtil_CreateOnePbActiveQueueEntry();

  /* build cmd to write all active entries to a file */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC); 
  CmdMsg.Type = 2;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf/ActiveTransactions.dat");

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  
  /* force the CFE_FS_WriteHdr function to return a valid byte count (96) */
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_PLAYBACK_FILE_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_WRACT_TRANS_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_WriteActTransCustFilename */


void Test_CF_WriteActTransCmdInvLen(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 1024, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_WriteActTransCmdInvLen */


void Test_CF_WriteActTransCmdInvFilename(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  CmdMsg.Type = 2;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf Transactions.dat");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_WriteActTransCmdInvFilename */




void Test_CF_WriteActTransCreatFail(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  CmdMsg.Type = 2;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf/Transactions.dat");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WRACT_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_WriteActTransCreatFail */



void Test_CF_WriteActTransWrHdrFail(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  CmdMsg.Type = 2;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf/Transactions.dat");

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_FILEWRITE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_WriteActTransWrHdrFail */


void Test_CF_WriteActTransInvWhichQs(void){

  CF_WriteActiveTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Note: The code named the 'Type' cmd param - WhichQueues, hence the test name */
  CmdMsg.Type = 3;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf/Transactions.dat");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_WRACT_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_WriteActTransInvWhichQs */



void Test_CF_WriteActTransEntryWriteErr(void){

  CF_WriteActiveTransCmd_t  CmdMsg;
  
  CF_TstUtil_CreateOnePbActiveQueueEntry();

  /* build cmd to write all active entries to a file */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_WriteActiveTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_WR_ACTIVE_TRANS_CC); 
  CmdMsg.Type = 2;/*(all=0/up=1/down=2)*/
  strcpy(CmdMsg.Filename,"/cf/ActiveTransactions.dat");

  /* force the file create to return a valid file descriptor (5) */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 5, 1);
  
  /* force the CFE_FS_WriteHdr function to return a valid byte count (96) */
  Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, 96, 1);
  
  /* force an error when writing the entry to the file */
  /* Coincidentally, entries are the same size as cfe file hdr (96), this rtns 12 */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, 12, 1);
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_PLAYBACK_FILE_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_EventSent(CF_FILEWRITE_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_WriteActTransEntryWriteErr */


void Test_CF_QuickStatusFilenameCmd(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"/ram/file4.dat");

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_QUICK_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_QuickStatusFilenameCmd */



void Test_CF_InvCmdCodeCmd(void){

  CF_SetMibParam_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, 55); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CC_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InvCmdCodeCmd */


void Test_CF_InvMsgIdCmd(void){

  CF_SetMibParam_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, 0x20F, sizeof(CF_SetMibParam_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_FREEZE_CC); 

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_MID_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");

}/* end Test_CF_InvMsgIdCmd */






/*******************************************************************************
**
**      Transaction Diagnostic command 
**
*******************************************************************************/


void Test_CF_SendTransDiagCmdSuccess(void){

  CF_SendTransCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SendTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC);
  strcpy(CmdMsg.Trans,"/cf/testfile.txt");
  CF_TstUtil_CreateOnePbActiveQueueEntry();
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_SND_TRANS_CMD_EID,CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter==2,"CF_AppData.Hk.CmdCounter = 2");  

}/* end Test_CF_SendTransDiagCmdSuccess */


void Test_CF_SendTransDiagFileNotFound(void){

  CF_SendTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SendTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC);   
  strcpy(CmdMsg.Trans,"/gnd/file.txt");
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_TRANS_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendTransDiagFileNotFound */


void Test_CF_SendTransDiagTransNotFound(void){

  CF_SendTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SendTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC); 
  strcpy(CmdMsg.Trans,"2.35_5");
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_SND_TRANS_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendTransDiagTransNotFound */


void Test_CF_SendTransDiagCmdInvLen(void){

  CF_SendTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 3, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC);   
  strcpy(CmdMsg.Trans,"/gnd/file.txt");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendTransDiagCmdInvLen */


void Test_CF_SendTransDiagUntermString(void){

  CF_SendTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SendTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC); 
  CFE_PSP_MemSet  (CmdMsg.Trans,0xFF, OS_MAX_PATH_LEN);
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendTransDiagUntermString */


void Test_CF_SendTransDiagInvFilename(void){

  CF_SendTransCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SendTransCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SEND_TRANS_DIAG_DATA_CC);   
  strcpy(CmdMsg.Trans,"/This string has spaces");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SendTransDiagInvFilename */



void Test_CF_SetPollParamCmd(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM1_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_SetPollParamCmd */


void Test_CF_SetPollParamCmdInvLen(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 4, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC);
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamCmdInvLen */


void Test_CF_SetPollParamInvChan(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 130;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvChan */


void Test_CF_SetPollParamInvDir(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 8;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1) */
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvDir */


void Test_CF_SetPollParamInvClass(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 0;/* 1=class 1, 2= class 2 */
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvClass */


void Test_CF_SetPollParamInvPreserve(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 2;/* 0=delete, 1=keep */
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR4_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvPreserve */


void Test_CF_SetPollParamInvSrc(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf /");/* no spaces */
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR5_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvSrc */


void Test_CF_SetPollParamInvDst(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234.200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gn d/path");/* space in string */
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR6_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvDst */



void Test_CF_SetPollParamInvId(void){

  CF_SetPollParamCmd_t  CmdMsg;

  /* Setup Inputs */
  CF_AppInit();  
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_SetPollParamCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_SET_POLL_PARAM_CC); 
  
  CmdMsg.Chan = 0;
  CmdMsg.Dir = 1;
  CmdMsg.Class = 1;
  CmdMsg.Priority = 1;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId,"234200");
  strcpy(CmdMsg.SrcPath,"/cf/");
  strcpy(CmdMsg.DstPath,"/gnd/path/");
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_SET_POLL_PARAM_ERR7_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_SetPollParamInvId */


void Test_CF_DeleteQueueNodeCmdInvLen(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 100, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.1_209");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DeleteQueueNodeCmdInvLen */


void Test_CF_DeleteQueueNodeTransUnterm(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  CFE_PSP_MemSet(CmdMsg.Trans,0xFF, OS_MAX_PATH_LEN);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DeleteQueueNodeTransUnterm */


void Test_CF_DeleteQueueNodeInvFilename(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"/Filename with space");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_DeleteQueueNodeInvFilename */



void Test_CF_DeleteQueueNodeFileNotFound(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"/FileDoesNotExist.txt");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DEQ_NODE_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_DeleteQueueNodeFileNotFound */


void Test_CF_DeleteQueueNodeIdNotFound(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.1_209");
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DEQ_NODE_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_DeleteQueueNodeIdNotFound */



void Test_CF_DeleteQueueNodeUpActive(void){

  CF_DequeueNodeCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.23_500");
  CF_AppData.Hk.ErrCounter = 0;
  
  CF_TstUtil_CreateOneUpActiveQueueEntry();

  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);
  
  /* This dequeue command will produce the warning */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  /* This cmd will dequeue without warning because its the second identical cmd */
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  OS_printf("CF_AppData.Hk.ErrCounter = %d\n",CF_AppData.Hk.ErrCounter);
   OS_printf("CF_AppData.Hk.CmdCounter = %d\n",CF_AppData.Hk.CmdCounter);
  
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_DEQ_NODE_ERR2_EID, CFE_EVS_CRITICAL, "", "Critical Event Sent");
  UtAssert_EventSent(CF_DEQ_NODE1_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 2, "CF_AppData.Hk.CmdCounter = 2");
  
  CF_ResetEngine();

}/* end Test_CF_DeleteQueueNodeUpActive */


void Test_CF_DeleteQueueNodeUpHist(void){

  CF_DequeueNodeCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.23_500");
  CF_AppData.Hk.ErrCounter = 0;
  
  CF_TstUtil_CreateOneUpHistoryQueueEntry();

  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_IN_TRANS_OK_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_DEQ_NODE1_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
  
  CF_ResetEngine();

}/* end Test_CF_DeleteQueueNodeUpHist */


void Test_CF_DeleteQueueNodePbPend(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"/cf/testfile.txt");
  CF_TstUtil_CreateOnePendingQueueEntry();
  
  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);  
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_DEQ_NODE2_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 2, "CF_AppData.Hk.CmdCounter = 2");

}/* end Test_CF_DeleteQueueNodePbPend */


void Test_CF_DeleteQueueNodePbActive(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.24_1");
  CF_AppData.Hk.ErrCounter = 0;
  CF_TstUtil_CreateOnePbActiveQueueEntry();
    
  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  /* This dequeue cmd will produce the warning */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  /* This cmd will dequeue without warning because its the second identical cmd */
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==15,"Event Count = 15");
  UtAssert_EventSent(CF_DEQ_NODE_ERR3_EID, CFE_EVS_CRITICAL, "", "Critical Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 3, "CF_AppData.Hk.CmdCounter = 3");
    
  CF_ResetEngine();

}/* end Test_CF_DeleteQueueNodePbActive */


void Test_CF_DeleteQueueNodePbHist(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.24_1");
  CF_AppData.Hk.ErrCounter = 0;

  CF_TstUtil_CreateOnePbHistoryQueueEntry();

  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
    
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==25,"Event Count = 25");
  UtAssert_EventSent(CF_OUT_TRANS_FAILED_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 3, "CF_AppData.Hk.CmdCounter = 3");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
  
  CF_ResetEngine();

}/* end Test_CF_DeleteQueueNodePbHist */


void Test_CF_DeleteQueueNodePutFail(void){

  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"0.24_1");
  CF_AppData.Hk.ErrCounter = 0;

  CF_TstUtil_CreateOnePbHistoryQueueEntry();

  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
    
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==26,"Event Count = 26");
  UtAssert_EventSent(CF_OUT_TRANS_FAILED_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_EventSent(CF_MEM_DEALLOC_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 3, "CF_AppData.Hk.CmdCounter = 3");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
  
  CF_ResetEngine();

}/* end Test_CF_DeleteQueueNodePutFail */


void Test_CF_DeleteQueueNodeInvType(void){

  CF_QueueEntry_t     *Ptr;
  CF_DequeueNodeCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_DequeueNodeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DELETE_QUEUE_NODE_CC);
  strcpy(CmdMsg.Trans,"/cf/testfile.txt");
  CF_AppData.Hk.ErrCounter = 0;
  Ptr = (CF_QueueEntry_t *)CF_AppData.Mem.Partition;  
  CF_TstUtil_CreateOnePendingQueueEntry();
  Ptr->NodeType = 55;
  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
    
  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_DEQ_NODE_ERR4_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");  

}/* end Test_CF_DeleteQueueNodeInvType */



void Test_CF_PurgeQueueCmdInvLen(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 28000, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeQueueCmdInvLen */


void Test_CF_PurgeUplinkActive(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 1;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeUplinkActive */


void Test_CF_PurgeUpHistory(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 2;/* 0=pending,1=active,2=history */
  
  CF_TstUtil_CreateOneUpHistoryQueueEntry();
  
  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_PURGEQ1_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  
  CF_ResetEngine();
  
}/* end Test_CF_PurgeUpHistory */


void Test_CF_PurgeInvUpQ(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 1;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 3;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeInvUpQ */


void Test_CF_PurgeOutActive(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 1;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR3_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeOutActive */



void Test_CF_PurgeOutPend(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */

  CF_TstUtil_CreateOnePendingQueueEntry();
  
  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_EventSent(CF_PURGEQ2_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 2, "CF_AppData.Hk.CmdCounter = 2");
  
}/* end Test_CF_PurgeOutPend */


void Test_CF_PurgeOutHist(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 2;/* 0=pending,1=active,2=history */
    
  CF_TstUtil_CreateOnePbHistoryQueueEntry();

  /* Have put pool return positive number(indicating success) instead of default zero */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, 16, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==25,"Event Count = 25");
  UtAssert_EventSent(CF_PURGEQ2_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 3, "CF_AppData.Hk.CmdCounter = 3");
  
  CF_ResetEngine();
  
  
}/* end Test_CF_PurgeOutHist */



void Test_CF_PurgeInvOutQ(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 3;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR4_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeInvOutQ */


void Test_CF_PurgeInvOutChan(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 2;/*(up=1/down=2)*/
  CmdMsg.Chan = 47;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR5_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeInvOutChan */


void Test_CF_PurgeInvType(void){

  CF_PurgeQueueCmd_t  CmdMsg;
  
  CF_AppInit();
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PurgeQueueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PURGE_QUEUE_CC); 
  CmdMsg.Type = 3;/*(up=1/down=2)*/
  CmdMsg.Chan = 0;
  CmdMsg.Queue = 0;/* 0=pending,1=active,2=history */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */  
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_PURGEQ_ERR6_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PurgeInvType */


void Test_CF_EnableDequeueCmd(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisDequeueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DEQUEUE_CC);
  CmdMsg.Chan =0;
  CF_AppData.Hk.CmdCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_ENA_DQ_CMD_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");

}/* end Test_CF_EnableDequeueCmd */


void Test_CF_EnableDequeueCmdInvLen(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 97, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DEQUEUE_CC);
  CmdMsg.Chan =0;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_EnableDequeueCmdInvLen */


void Test_CF_EnableDequeueInvChan(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisDequeueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DEQUEUE_CC);
  CmdMsg.Chan =49;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DQ_CMD_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_EnableDequeueInvChan */


void Test_CF_DisableDequeueCmd(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisDequeueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DEQUEUE_CC);
  CmdMsg.Chan = 0;
  CF_AppData.Hk.CmdCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DIS_DQ_CMD_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  
}/* end Test_CF_DisableDequeueCmd */


void Test_CF_DisableDequeueCmdInvLen(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 34, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DEQUEUE_CC);
  CmdMsg.Chan = 0;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DisableDequeueCmdInvLen */


void Test_CF_DisableDequeueInvChan(void){

  CF_EnDisDequeueCmd_t  CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisDequeueCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DEQUEUE_CC);
  CmdMsg.Chan = 46;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DQ_CMD_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DisableDequeueInvChan */


void Test_CF_EnableDirPollingCmd(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.CmdCounter = 0;
  

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_ENA_POLL_CMD2_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  
}/* end Test_CF_EnableDirPollingCmd */


void Test_CF_EnableDirPollingCmdInvLen(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 60, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_EnableDirPollingCmdInvLen */



void Test_CF_EnablePollingInvChan(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =3;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_ENA_POLL_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_EnablePollingInvChan */


void Test_CF_EnablePollingInvDir(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =32;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_ENA_POLL_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_EnablePollingInvDir */


void Test_CF_EnablePollingAll(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_ENABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0xFF;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.CmdCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_ENA_POLL_CMD1_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  
}/* end Test_CF_EnablePollingAll */


void Test_CF_DisableDirPollingCmd(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DIR_POLLING_CC);
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */  

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DIS_POLL_CMD2_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  
}/* end Test_CF_DisableDirPollingCmd */


void Test_CF_DisableDirPollingCmdInvLen(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 30000, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DIR_POLLING_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DisableDirPollingCmdInvLen */


void Test_CF_DisablePollingInvChan(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =3;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DIS_POLL_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DisablePollingInvChan */


void Test_CF_DisablePollingInvDir(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =32;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DIS_POLL_ERR2_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_DisablePollingInvDir */


void Test_CF_DisablePollingAll(void){

  CF_EnDisPollCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_EnDisPollCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_DISABLE_DIR_POLLING_CC); 
  CmdMsg.Chan =0;/* 0 to (CF_MAX_PLAYBACK_CHANNELS - 1) */
  CmdMsg.Dir =0xFF;/* 0 to (CF_MAX_POLLING_DIRS_PER_CHAN - 1), or 0xFF for en/dis all */

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_DIS_POLL_CMD1_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");
  
}/* end Test_CF_DisablePollingAll */


void Test_CF_KickStartCmd(void){

  CF_KickstartCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_KickstartCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_KICKSTART_CC);
  CmdMsg.Chan = 1;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_KICKSTART_CMD_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_KickStartCmd */


void Test_CF_KickStartCmdInvLen(void){

  CF_KickstartCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 650, FALSE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_KICKSTART_CC);
  CmdMsg.Chan = 1;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_KickStartCmdInvLen */


void Test_CF_KickStartCmdInvChan(void){

  CF_KickstartCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_KickstartCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_KICKSTART_CC);
  CmdMsg.Chan = 5;
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_KICKSTART_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_KickStartCmdInvChan */


void Test_CF_QuickStatusTransCmd(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"0.24_6");
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_QUICK_ERR1_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_QuickStatusTransCmd */


void Test_CF_QuickStatusActiveTrans(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"0.24_1");
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  CF_TstUtil_CreateOnePbActiveQueueEntry();
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_QUICK_CMD_EID,CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 2, "CF_AppData.Hk.CmdCounter = 2");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
    
  CF_ResetEngine();

}/* end Test_CF_QuickStatusActiveTrans */


void Test_CF_QuickStatusActiveName(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"/cf/testfile.txt");
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  CF_TstUtil_CreateOnePbActiveQueueEntry();
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==14,"Event Count = 14");
  UtAssert_EventSent(CF_QUICK_CMD_EID,CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 2, "CF_AppData.Hk.CmdCounter = 2");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
    
  CF_ResetEngine();

}/* end Test_CF_QuickStatusActiveName */


void Test_CF_QuickStatusActiveSuspended(void){

  CF_QuickStatCmd_t   CmdMsg;
  CF_CARSCmd_t        SuspendCmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"/cf/testfile.txt");
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  CF_TstUtil_CreateOnePbActiveQueueEntry();
  
  /* Suspend the active transaction */
  CFE_SB_InitMsg(&SuspendCmdMsg, CF_CMD_MID, sizeof(CF_CARSCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&SuspendCmdMsg, CF_SUSPEND_CC);
  strcpy(SuspendCmdMsg.Trans,"/cf/testfile.txt"); 
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&SuspendCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&SuspendCmdMsg);  
    
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==17,"Event Count = 17");
  UtAssert_EventSent(CF_QUICK_CMD_EID,CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 3, "CF_AppData.Hk.CmdCounter = 3");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 0, "CF_AppData.Hk.ErrCounter = 0");
    
  CF_ResetEngine();

}/* end Test_CF_QuickStatusActiveSuspended */


void Test_CF_QuickStatusCmdInvLen(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 0, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  printf("CF_AppData.Hk.ErrCounter = %d\n",CF_AppData.Hk.ErrCounter);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.HkPacket.ErrCounter = 1");

}/* end Test_CF_QuickStatusCmdInvLen */


void Test_CF_QuickStatusUntermString(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  CFE_PSP_MemSet  (CmdMsg.Trans,0xFF, OS_MAX_PATH_LEN);
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_NO_TERM_ERR_EID,CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_QuickStatusUntermString */


void Test_CF_QuickStatusInvFilename(void){

  CF_QuickStatCmd_t  CmdMsg;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_QuickStatCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_QUICKSTATUS_CC);
  strcpy(CmdMsg.Trans,"/invalid filename");
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_INV_FILENAME_EID,CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_QuickStatusInvFilename */



void Test_CF_PbFileNoMem(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_QDIR_NOMEM1_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");
  
}/* end Test_CF_PbFileNoMem */


void Test_CF_PbFileCmdInvLen(void){

  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 16, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileCmdInvLen */


void Test_CF_PbFileCmdParamErr(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 3;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR1_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileCmdParamErr */


void Test_CF_PbFileChanNotInUse(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 1;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR2_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileChanNotInUse */


void Test_CF_PbFileInvSrcFilename(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/ testfile.txt");/* no spaces */
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileInvSrcFilename */



void Test_CF_PbFileInvDstFilename(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  CFE_PSP_MemSet  (CmdMsg.DstFilename,0xFF, OS_MAX_PATH_LEN);/* dest filename not terminated */

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_INV_FILENAME_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileInvDstFilename */


void Test_CF_PbFilePendQFull(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();
  
  /* set the pending queue depth to the max */
  CF_AppData.Chan[0].PbQ[0].EntryCnt = CF_AppData.Tbl->OuCh[1].PendingQDepth;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR3_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFilePendQFull */


void Test_CF_PbFileInvPeerId(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "2555.255");/* 255 changed to 2555 */
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR6_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");


}/* end Test_CF_PbFileInvPeerId */


void Test_CF_PbFileFileOpen(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();
  
  /* Force OS_FDGetInfo to return 'file is open' and success */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &OS_FDGetInfoHook);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, NULL);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR4_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileFileOpen */


void Test_CF_PbFileFileOnQ(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackFileCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_FILE_CC);
  CmdMsg.Class = 1;
  CmdMsg.Channel = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcFilename, "/cf/testfile.txt");
  strcpy(CmdMsg.DstFilename, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);
  
  /* send cmd again to invoke the error */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
    printf("Qdepth=%u, CmdCtr=%d, ErrCtr=%d\n",(unsigned int)Ut_CFE_EVS_GetEventQueueDepth(),
          CF_AppData.Hk.CmdCounter,CF_AppData.Hk.ErrCounter);
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_FILE_ERR5_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 1, "CF_AppData.Hk.CmdCounter = 1");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbFileFileOnQ */


void Test_CF_PbDirCmd(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/");
  strcpy(CmdMsg.DstPath, "gndpath/");
  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PLAYBACK_DIR_EID, CFE_EVS_DEBUG, "", "Debug Event Sent");

}/* end Test_CF_PbDirCmd */


void Test_CF_PbDirCmdOpenErr(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/");
  strcpy(CmdMsg.DstPath, "gndpath/");
  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_OPEN_DIR_ERR_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");

}/* end Test_CF_PbDirCmdOpenErr */



void Test_CF_PbDirCmdInvLen(void){

  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, 7, TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CF_AppData.Hk.ErrCounter = 0;

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_EventSent(CF_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirCmdInvLen */


void Test_CF_PbDirCmdParamErr(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 3;/* invalid class */
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/");
  strcpy(CmdMsg.DstPath, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_DIR_ERR1_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirCmdParamErr */


void Test_CF_PbDirChanNotInUse(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 1;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/");
  strcpy(CmdMsg.DstPath, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_DIR_ERR2_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirChanNotInUse */


void Test_CF_PbDirInvSrcPath(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/ testfile.txt");/* no spaces, no slash at end */
  strcpy(CmdMsg.DstPath, "gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_DIR_ERR3_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirInvSrcPath */



void Test_CF_PbDirInvDstPath(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "255.255");
  strcpy(CmdMsg.SrcPath, "/cf/");
  CFE_PSP_MemSet  (CmdMsg.DstPath,0xFF, OS_MAX_PATH_LEN);/* dest filename not terminated */

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_DIR_ERR4_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirInvDstPath */


void Test_CF_PbDirInvPeerId(void){

  int32               ExpAppInitRtn,ActAppInitRtn;
  CF_PlaybackDirCmd_t   CmdMsg;
  
  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_PlaybackDirCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_PLAYBACK_DIR_CC);
  CmdMsg.Class = 1;
  CmdMsg.Chan = 0;
  CmdMsg.Priority = 0;
  CmdMsg.Preserve = 0;
  strcpy(CmdMsg.PeerEntityId, "2555.255");/* 255 changed to 2555 */
  strcpy(CmdMsg.SrcPath, "/cf/");
  strcpy(CmdMsg.DstPath, "/gndpath/");

  ExpAppInitRtn = CFE_SUCCESS;
  ActAppInitRtn = CF_AppInit();

  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_True(ActAppInitRtn == ExpAppInitRtn, "ActAppInitRtn = ExpAppInitRtn");
  UtAssert_EventSent(CF_INIT_EID, CFE_EVS_INFORMATION, "", "Info Event Sent");
  UtAssert_EventSent(CF_PB_DIR_ERR5_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_PbDirInvPeerId */


void Test_CF_QDirFilesQFull(void){

  CF_QueueDirFiles_t  Qdf;
  uint32              ActRtn,ExpRtn;
  
  /* Setup Inputs */
  Qdf.Chan = 0;
  Qdf.Class = 1;
  Qdf.Priority = 1;
  Qdf.Preserve = 1;
  Qdf.CmdOrPoll = 2;
  strcpy(Qdf.PeerEntityId,"0.23");
  strcpy(Qdf.SrcPath,"/cf/");
  strcpy(Qdf.DstPath,"/gnd/");
  
  /* makes the pending queue appear full */
  CF_AppData.Chan[0].PbQ[0].EntryCnt = CF_AppData.Tbl->OuCh[0].PendingQDepth;
    
    /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);

  /* 
  ** Force OS_readdir to first return a 'dot filename,then a Sub Dir, 
  ** then the Queue Full Check will fail due to line above */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &OS_readdirHook);
  
  /* Execute Test */ 
  ExpRtn = CF_ERROR;
  ActRtn = CF_QueueDirectoryFiles(&Qdf);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==1,"Event Count = 1");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_QDIR_PQFUL_EID, CFE_EVS_ERROR, "", "ERROR Event Sent");

  ReaddirHookCallCnt = 0;
  CF_AppData.Chan[0].PbQ[0].EntryCnt = 0;

}/* end Test_CF_QDirFilesQFull */



void Test_CF_QDirFilesNoMem(void){

  CF_QueueDirFiles_t  Qdf;
  uint32              ActRtn,ExpRtn;
  
  /* Setup Inputs */
  Qdf.Chan = 0;
  Qdf.Class = 1;
  Qdf.Priority = 1;
  Qdf.Preserve = 1;
  Qdf.CmdOrPoll = 2;
  strcpy(Qdf.PeerEntityId,"0.23");
  strcpy(Qdf.SrcPath,"/cf/");
  strcpy(Qdf.DstPath,"/gnd/");
 
  CF_AppInit();
  
  /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);
  
  /* make the mem allocation for the new queue node fail */
  Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETPOOLBUF_INDEX, -1, 1);

  /* 
  ** Force OS_readdir to first return a 'dot filename,then a Sub Dir, 
  ** then the Queue Full Check will fail due to line above */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &OS_readdirHook);
  
  /* Execute Test */ 
  ExpRtn = CF_ERROR;
  ActRtn = CF_QueueDirectoryFiles(&Qdf);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==13,"Event Count = 13");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_QDIR_INV_NAME1_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_INV_NAME2_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_NOMEM2_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  
  ReaddirHookCallCnt = 0;

}/* end Test_CF_QDirFilesNoMem */


void Test_CF_QDirFilesFileOnQ(void){

  CF_QueueDirFiles_t  Qdf;
  uint32              ActRtn,ExpRtn;
  
  /* Setup Inputs */
  Qdf.Chan = 0;
  Qdf.Class = 1;
  Qdf.Priority = 1;
  Qdf.Preserve = 1;
  Qdf.CmdOrPoll = 2;
  strcpy(Qdf.PeerEntityId,"0.23");
  strcpy(Qdf.SrcPath,"/cf/");
  strcpy(Qdf.DstPath,"/gnd/");
        
  /* This call puts on the pending queue, the same filename listed in readdirHook */
  CF_TstUtil_CreateOnePendingQueueEntry();
  
  /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);

  /* 
  ** Force OS_readdir to first return a 'dot filename,then a Sub Dir, 
  ** then the Queue Full Check will fail due to line above */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &OS_readdirHook);
  
  /* Execute Test */ 
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_QueueDirectoryFiles(&Qdf);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==13,"Event Count = 13");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_QDIR_INV_NAME1_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_INV_NAME2_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_ACTIVEFILE_EID,CFE_EVS_DEBUG, "", "Debug Event Sent");
  
  ReaddirHookCallCnt = 0;
  
}/* end Test_CF_QDirFilesFileOnQ */


void Test_CF_QDirFilesFileOpen(void){

  CF_QueueDirFiles_t  Qdf;
  uint32              ActRtn,ExpRtn;
  
  /* Setup Inputs */
  Qdf.Chan = 0;
  Qdf.Class = 1;
  Qdf.Priority = 1;
  Qdf.Preserve = 1;
  Qdf.CmdOrPoll = 2;
  strcpy(Qdf.PeerEntityId,"0.23");
  strcpy(Qdf.SrcPath,"/cf/");
  strcpy(Qdf.DstPath,"/gnd/");
 
  CF_AppInit();
      
  /* Force OS_FDGetInfo to return 'file is open' and success */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &OS_FDGetInfoHook);
  
  /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);
  
  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* 
  ** Force OS_readdir to first return a 'dot filename,then a Sub Dir, 
  ** then the Queue Full Check will fail due to line above */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &OS_readdirHook);
  
  /* Execute Test */ 
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_QueueDirectoryFiles(&Qdf);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==12,"Event Count = 12");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_QDIR_INV_NAME1_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_INV_NAME2_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_OPENFILE_EID,CFE_EVS_INFORMATION, "", "Info Event Sent");

  ReaddirHookCallCnt = 0;
  Ut_OSFILEAPI_HookTable.OS_FDGetInfo = NULL;

}/* end Test_CF_QDirFilesFileOpen */



void Test_CF_QDirFilesAllGood(void){

  CF_QueueDirFiles_t  Qdf;
  uint32              ActRtn,ExpRtn;
  
  /* Setup Inputs */
  Qdf.Chan = 0;
  Qdf.Class = 1;
  Qdf.Priority = 1;
  Qdf.Preserve = 1;
  Qdf.CmdOrPoll = CF_POLLDIRECTORY;
  strcpy(Qdf.PeerEntityId,"0.23");
  strcpy(Qdf.SrcPath,"/cf/");
  strcpy(Qdf.DstPath,"/gnd/");
  
  CF_AppInit();

  /* Force OS_opendir to return success, instead of default NULL */
  Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 5, 1);
   
  /* Force OS_readdir to first return a 'dot filename,then a Sub Dir, 
  ** then the Queue Full Check will fail due to line above */
  Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &OS_readdirHook);
  
  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);
  
  /* Execute Test */ 
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_QueueDirectoryFiles(&Qdf);

  /* Verify Outputs */
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==11,"Event Count = 11");
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_EventSent(CF_QDIR_INV_NAME1_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  UtAssert_EventSent(CF_QDIR_INV_NAME2_EID,CFE_EVS_ERROR, "", "ERROR Event Sent");
  
  ReaddirHookCallCnt = 0;
  
}/* end Test_CF_QDirFilesAllGood */


void Test_CF_PbQueueRemoveFirst(void){

  CF_QueueEntry_t   Node1, Node2, Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node2);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);

  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntBefore = 3");
  
  /* Execute Test */
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromPbQueue(Chan, Queue, &Node1);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("23")==CF_SUCCESS,"ListOrder 2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntAfter = 2");
  
}/* end Test_CF_PbQueueRemoveFirst */


void Test_CF_PbQueueRemoveMiddle(void){

  CF_QueueEntry_t   Node1, Node2, Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node2);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);

  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntBefore = 3");
  
  /* Execute Test */
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromPbQueue(Chan, Queue, &Node2);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntAfter = 2");
  
}/* end Test_CF_PbQueueRemoveMiddle */

void Test_CF_PbQueueRemoveLast(void){

  CF_QueueEntry_t   Node1, Node2, Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node2);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);

  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntBefore = 3");
  
  /* Execute Test */
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromPbQueue(Chan, Queue, &Node3);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("12")==CF_SUCCESS,"ListOrder 1,2");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntAfter = 2");
  
}/* end Test_CF_PbQueueRemoveLast */


void Test_CF_PbQueueRemoveNull(void){

  CF_QueueEntry_t   Node1, Node2, Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node2);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);

  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntBefore = 3");
  
  /* Execute Test */
  ExpRtn = CF_ERROR;
  ActRtn = CF_RemoveFileFromPbQueue(Chan, Queue, NULL);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntAfter = 3");
  
}/* end Test_CF_PbQueueRemoveNull */



void Test_CF_PbQueueInsertInvChan(void){

  CF_QueueEntry_t   Node1,Node2,Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);
  
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntBefore = 2");
  
  /* Execute Test */
  ExpRtn = CF_ERROR;
  ActRtn = CF_InsertPbNode(Chan + 5,Queue,&Node2, &Node3);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntAfter = 2");

}/* end Test_CF_PbQueueInsertInvChan */



void Test_CF_PbQueueInsertInvQ(void){

  CF_QueueEntry_t   Node1,Node2,Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);
  
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntBefore = 2");
  
  /* Execute Test */
  ExpRtn = CF_ERROR;
  ActRtn = CF_InsertPbNode(Chan,Queue + 7,&Node2, &Node3);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntAfter = 2");

}/* end Test_CF_PbQueueInsertInvQ */


void Test_CF_PbQueueInsertGood(void){

  CF_QueueEntry_t   Node1,Node2,Node3;
  uint32            Chan = 0;
  uint32            Queue = 0;
  uint32            ActRtn,ExpRtn;
    
  /* Setup Inputs */
  CF_AppInit();

  Node1.Prev = NULL;
  Node1.Next = NULL;
  Node1.TransNum = 1;

  Node2.Prev = NULL;
  Node2.Next = NULL;
  Node2.TransNum = 2;  

  Node3.Prev = NULL;
  Node3.Next = NULL;
  Node3.TransNum = 3;
  
  CF_AddFileToPbQueue(Chan, Queue, &Node3);
  CF_AddFileToPbQueue(Chan, Queue, &Node1);
  
  UtAssert_True(CF_TstUtil_VerifyListOrder("13")==CF_SUCCESS,"ListOrder 1,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==2,"EntryCntBefore = 2");
  
  /* Execute Test */
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_InsertPbNode(Chan,Queue,&Node2, &Node3);

  /* Verify Outputs */
  UtAssert_True(ActRtn == ExpRtn, "ActRtn = ExpRtn");
  UtAssert_True(CF_TstUtil_VerifyListOrder("123")==CF_SUCCESS,"ListOrder 1,2,3");
  UtAssert_True(CF_AppData.Chan[Chan].PbQ[Queue].EntryCnt==3,"EntryCntAfter = 3");
  
}/* end Test_CF_PbQueueInsertGood */



void Test_CF_PbQueueFrontGood(void){

  /* queue one low priority file, then a high priority file.
  This should cause CF_InsertPbNodeAtFront to be executed */

  CF_PlaybackFileCmd_t      PbFileCmdMsg;

    /* reset CF globals etc */
  CF_AppInit();  
  
  /* reset the transactions seq number used by the engine */
  misc__set_trans_seq_num(1);
  
  CFE_SB_InitMsg(&PbFileCmdMsg, CF_CMD_MID, sizeof(CF_PlaybackFileCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&PbFileCmdMsg, CF_PLAYBACK_FILE_CC);

  /* low priority file */
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 100;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "2.25");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/lowestpriority.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* execute the playback file cmd to get a queue entry on the pending queue */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  
  
    /* next low priority file */
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 70;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "2.25");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/lowerpriority.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* execute the playback file cmd to get a queue entry on the pending queue */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  
  
  /* next low priority file */
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 50;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "2.25");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/lowpriority.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* execute the playback file cmd to get a queue entry on the pending queue */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);
  
  
    
  /* high priority file */
  PbFileCmdMsg.Class = 1;
  PbFileCmdMsg.Channel = 0;
  PbFileCmdMsg.Priority = 5;
  PbFileCmdMsg.Preserve = 0;
  strcpy(PbFileCmdMsg.PeerEntityId, "1.25");
  strcpy(PbFileCmdMsg.SrcFilename, "/cf/hipriority.txt");
  strcpy(PbFileCmdMsg.DstFilename, "gndpath/");

  /* force the GetPoolBuf call for the queue entry to return something valid */
  Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &CFE_ES_GetPoolBufHook);

  /* execute the playback file cmd to get a queue entry on the pending queue */
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&PbFileCmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&PbFileCmdMsg);


  /* verify */
  UtAssert_True(CF_AppData.Chan[0].PbQ[0].EntryCnt==4,"EntryCntBefore = 4");
  
}/* end Test_CF_PbQueueFrontGood */


void Test_CF_PbQueueFrontInvChan(void)
{
  int32           ActRtn,ExpRtn;
  uint8           Chan = 10;
  uint32          Queue = 0;
  CF_QueueEntry_t *NodeToInsert;
  
  CF_AppInit();
    
  NodeToInsert = (CF_QueueEntry_t *)&CF_AppData.Mem.Partition;
  ExpRtn = CF_ERROR;
  ActRtn = CF_InsertPbNodeAtFront(Chan,Queue,NodeToInsert);
  
    /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_PbQueueFrontInvChan */


void Test_CF_AddUpQueueInvNewNode(void)
{
  int32           ActRtn,ExpRtn;
  uint32          Queue = 0;
  
  CF_AppInit();
    
  ExpRtn = CF_ERROR;
  ActRtn = CF_AddFileToUpQueue(Queue, NULL);
  
  /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_AddUpQueueInvNewNode */


void Test_CF_AddUpQueueSecondNode(void)
{
  int32           ActRtn,ExpRtn;
  uint32          Queue = 0;
  CF_QueueEntry_t FirstNodeAdded;
  CF_QueueEntry_t *FirstNodePtr = &FirstNodeAdded;
  
  CF_AppInit();
    
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_AddFileToUpQueue(Queue, FirstNodePtr);
  
  CF_AddFileToUpQueue(Queue, FirstNodePtr);
  
  /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_AddUpQueueSecondNode */


    
void Test_CF_RemoveFirstUpNode(void)
{
  int32           ActRtn,ExpRtn;
  uint32          Queue = 0;
  CF_QueueEntry_t FirstNodeAdded;
  CF_QueueEntry_t SecondNodeAdded;
  CF_QueueEntry_t ThirdNodeAdded;
  CF_QueueEntry_t *FirstNodePtr = &FirstNodeAdded;
  CF_QueueEntry_t *SecondNodePtr = &SecondNodeAdded;
  CF_QueueEntry_t *ThirdNodePtr = &ThirdNodeAdded;  
  
  CF_AppInit();
    
  ExpRtn = CF_SUCCESS;
  CF_AddFileToUpQueue(Queue, FirstNodePtr);
  CF_AddFileToUpQueue(Queue, SecondNodePtr);
  CF_AddFileToUpQueue(Queue, ThirdNodePtr);
    
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromUpQueue(Queue, FirstNodePtr);
  
  /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_RemoveFirstUpNode */


void Test_CF_RemoveMiddleUpNode(void)
{
  int32           ActRtn,ExpRtn;
  uint32          Queue = 0;
  CF_QueueEntry_t FirstNodeAdded;
  CF_QueueEntry_t SecondNodeAdded;
  CF_QueueEntry_t ThirdNodeAdded;
  CF_QueueEntry_t *FirstNodePtr = &FirstNodeAdded;
  CF_QueueEntry_t *SecondNodePtr = &SecondNodeAdded;
  CF_QueueEntry_t *ThirdNodePtr = &ThirdNodeAdded;  
  
  CF_AppInit();
    
  ExpRtn = CF_SUCCESS;
  CF_AddFileToUpQueue(Queue, FirstNodePtr);
  CF_AddFileToUpQueue(Queue, SecondNodePtr);
  CF_AddFileToUpQueue(Queue, ThirdNodePtr);
    
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromUpQueue(Queue, SecondNodePtr);
  
  /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_RemoveMiddleUpNode */


void Test_CF_RemoveLastUpNode(void)
{
  int32           ActRtn,ExpRtn;
  uint32          Queue = 0;
  CF_QueueEntry_t FirstNodeAdded;
  CF_QueueEntry_t SecondNodeAdded;
  CF_QueueEntry_t ThirdNodeAdded;
  CF_QueueEntry_t *FirstNodePtr = &FirstNodeAdded;
  CF_QueueEntry_t *SecondNodePtr = &SecondNodeAdded;
  CF_QueueEntry_t *ThirdNodePtr = &ThirdNodeAdded;  
    
  CF_AppInit();
    
  ExpRtn = CF_SUCCESS;
  CF_AddFileToUpQueue(Queue, FirstNodePtr);
  CF_AddFileToUpQueue(Queue, SecondNodePtr);
  CF_AddFileToUpQueue(Queue, ThirdNodePtr);
    
  ExpRtn = CF_SUCCESS;
  ActRtn = CF_RemoveFileFromUpQueue(Queue, ThirdNodePtr);
  
  /* verify */
  UtAssert_True(ActRtn==ExpRtn,"ActRtn==ExpRtn");
   
}/* end Test_CF_RemoveLastUpNode */



void Test_CF_GiveSemInvParamCmd(void){

  CF_GiveTakeCmd_t  CmdMsg;
  int32             CreatRtn;
  uint32            SemId;
  uint32            InitialValue = 5;
  uint32            SemOptions = 0;

  /* Setup Inputs */
  CFE_SB_InitMsg(&CmdMsg, CF_CMD_MID, sizeof(CF_GiveTakeCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdMsg, CF_GIVETAKE_CC);
  CmdMsg.Chan = 0;
  CmdMsg.GiveOrTakeSemaphore = CF_GIVE_SEMAPHORE;/* 0 */
  CF_AppData.Hk.CmdCounter = 0;
  CF_AppData.Hk.ErrCounter = 0;
  
  CreatRtn = OS_CountSemCreate (&SemId, "Test_Semaphore", InitialValue, SemOptions);
  
  CF_AppInit();
  
  /* Execute Test */  
  CF_AppData.MsgPtr = (CFE_SB_MsgPtr_t)&CmdMsg;  
  CF_AppPipe((CFE_SB_MsgPtr_t)&CmdMsg);

  /* Verify Outputs */
  UtAssert_True(CreatRtn==OS_SUCCESS,"Create = SUCCESS");
  UtAssert_True(Ut_CFE_EVS_GetEventQueueDepth()==10,"Event Count = 10");
  UtAssert_EventSent(CF_GIVETAKE_ERR1_EID,CFE_EVS_ERROR, "", "Error Event Sent");
  UtAssert_True(CF_AppData.Hk.CmdCounter == 0, "CF_AppData.Hk.CmdCounter = 0");
  UtAssert_True(CF_AppData.Hk.ErrCounter == 1, "CF_AppData.Hk.ErrCounter = 1");

}/* end Test_CF_GiveSemaphoreCmd */





void CF_Setup(void)
{  

    CFE_ES_GetPoolBufHookCallCnt = 0;
    Ut_OSAPI_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_CFE_EVS_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_TBL_AddTable(CF_CONFIG_TABLE_FILENAME, &CF_ConfigTable);

}

void CF_TearDown(void)
{

}

/* CF_AddTestCase is last in the file so I don't have to declare prototypes for each test method */
void CF_AddTestCase(void)
{

    UtTest_Add(Test_CF_AppInit_EVSRegFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_EVSRegFail");
    UtTest_Add(Test_CF_AppInit_CrPipeFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_CrPipeFail");
    UtTest_Add(Test_CF_AppInit_Sub1Fail, CF_Setup, CF_TearDown, "Test_CF_AppInit_Sub1Fail");
    UtTest_Add(Test_CF_AppInit_Sub2Fail, CF_Setup, CF_TearDown, "Test_CF_AppInit_Sub2Fail");
    UtTest_Add(Test_CF_AppInit_Sub3Fail, CF_Setup, CF_TearDown, "Test_CF_AppInit_Sub3Fail");
    UtTest_Add(Test_CF_AppInit_Sub4Fail, CF_Setup, CF_TearDown, "Test_CF_AppInit_Sub3Fail");
    UtTest_Add(Test_CF_AppInit_TblRegFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_TblRegFail");
    UtTest_Add(Test_CF_AppInit_TblLoadFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_TblLoadFail");
    UtTest_Add(Test_CF_AppInit_TblManageFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_TblManageFail");
    UtTest_Add(Test_CF_AppInit_TblGetAdrFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_TblGetAdrFail");
    UtTest_Add(Test_CF_AppInit_PoolCreateExFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_PoolCreateExFail");
    UtTest_Add(Test_CF_AppInit_SendEventFail, CF_Setup, CF_TearDown, "Test_CF_AppInit_SendEventFail");
    UtTest_Add(Test_CF_AppInit_NoErrors, CF_Setup, CF_TearDown, "Test_CF_AppInit_NoErrors");

    UtTest_Add(Test_CF_TblValFlightEntityIdFail, CF_Setup, CF_TearDown, "Test_CF_TblValFlightEntityIdFail");
    UtTest_Add(Test_CF_TblValIncomingMsgIdFail, CF_Setup, CF_TearDown, "Test_CF_TblValIncomingMsgIdFail");
    UtTest_Add(Test_CF_TblValOutgoingFileChunkFail, CF_Setup, CF_TearDown, "Test_CF_TblValOutgoingFileChunkFail");
    UtTest_Add(Test_CF_TblValChanInUseFail, CF_Setup, CF_TearDown, "Test_CF_TblValChanInUseFail");
    UtTest_Add(Test_CF_TblValDequeEnableFail, CF_Setup, CF_TearDown, "Test_CF_TblValDequeEnableFail");
    UtTest_Add(Test_CF_TblValOutgoingMsgIdFail, CF_Setup, CF_TearDown, "Test_CF_TblValOutgoingMsgIdFail");
    UtTest_Add(Test_CF_TblValPollDirInUseFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollDirInUseFail");
    UtTest_Add(Test_CF_TblValPollEnableFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollEnableFail");
    UtTest_Add(Test_CF_TblValPollClassFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollClassFail");
    UtTest_Add(Test_CF_TblValPollPreserveFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollPreserveFail");
    UtTest_Add(Test_CF_TblValPollSrcPathFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollSrcPathFail");
    UtTest_Add(Test_CF_TblValPollDstPathFail, CF_Setup, CF_TearDown, "Test_CF_TblValPollDstPathFail");
    UtTest_Add(Test_CF_TblValPeerEntityIdFail, CF_Setup, CF_TearDown, "Test_CF_TblValPeerEntityIdFail");

    UtTest_Add(Test_CF_AppMain_InitErrors, CF_Setup, CF_TearDown, "Test_CF_AppMain_AppInitErrors");
    UtTest_Add(Test_CF_AppMain_SemGetIdByNameFail, CF_Setup, CF_TearDown, "Test_CF_AppMain_SemGetIdByNameFail");

    UtTest_Add(Test_CF_AppMain_RcvMsgErr, CF_Setup, CF_TearDown, "Test_CF_AppMain_RcvMsgErr");
    UtTest_Add(Test_CF_AppMain_RcvMsgOkOnFirst, CF_Setup, CF_TearDown, "Test_CF_AppMain_RcvMsgOkOnFirst");

    UtTest_Add(Test_CF_HousekeepingCmd, CF_Setup, CF_TearDown, "Test_CF_HousekeepingCmd");
    UtTest_Add(Test_CF_HkCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_HkCmdInvLen");

    UtTest_Add(Test_CF_HkCmdTblUpdated, CF_Setup, CF_TearDown, "Test_CF_HkCmdTblUpdated");
    UtTest_Add(Test_CF_HkCmdValPending, CF_Setup, CF_TearDown, "Test_CF_HkCmdValPending");      
    UtTest_Add(Test_CF_NoopCmd, CF_Setup, CF_TearDown, "Test_CF_NoopCmd");
    UtTest_Add(Test_CF_NoopCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_NoopCmdInvLen");
    
    UtTest_Add(Test_CF_WakeupCmd, CF_Setup, CF_TearDown, "Test_CF_WakeupCmd");
    UtTest_Add(Test_CF_WakeupCmdPollingEnabled, CF_Setup, CF_TearDown, "Test_CF_WakeupCmdPollingEnabled");    
    UtTest_Add(Test_CF_WakeupCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_WakeupCmdInvLen");

    UtTest_Add(Test_CF_InPDUNoErrCmd, CF_Setup, CF_TearDown, "Test_CF_InPDUNoErrCmd");
    UtTest_Add(Test_CF_InPDUTlmPktCmd, CF_Setup, CF_TearDown, "Test_CF_InPDUTlmPktCmd");        
    UtTest_Add(Test_CF_InPDUHdrSizeErrCmd, CF_Setup, CF_TearDown, "Test_CF_InPDUHdrSizeErrCmd");
    UtTest_Add(Test_CF_InPDUTooBigCmd, CF_Setup, CF_TearDown, "Test_CF_InPDUTooBigCmd");
    UtTest_Add(Test_CF_InPDUTooSmallCmd, CF_Setup, CF_TearDown, "Test_CF_InPDUTooSmallCmd");    

    UtTest_Add(Test_CF_RstCtrsCmd, CF_Setup, CF_TearDown, "Test_CF_RstCtrsCmd");
    UtTest_Add(Test_CF_RstCtrsCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_RstCtrsCmdInvLen");    
    
    UtTest_Add(Test_CF_FreezeCmd, CF_Setup, CF_TearDown, "Test_CF_FreezeCmd");
    UtTest_Add(Test_CF_FreezeCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_FreezeCmdInvLen");
    
    UtTest_Add(Test_CF_ThawCmd, CF_Setup, CF_TearDown, "Test_CF_ThawCmd");
    UtTest_Add(Test_CF_ThawCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_ThawCmdInvLen");
    
    UtTest_Add(Test_CF_SuspendTransIdCmd, CF_Setup, CF_TearDown, "Test_CF_SuspendTransIdCmd");
    UtTest_Add(Test_CF_SuspendFilenameCmd, CF_Setup, CF_TearDown, "Test_CF_SuspendFilenameCmd");
    UtTest_Add(Test_CF_SuspendInvFilenameCmd, CF_Setup, CF_TearDown, "Test_CF_SuspendInvFilenameCmd");
    UtTest_Add(Test_CF_SuspendCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_SuspendCmdInvLen");
    UtTest_Add(Test_CF_SuspendUntermStrgCmd, CF_Setup, CF_TearDown, "Test_CF_SuspendUntermStrgCmd");
    UtTest_Add(Test_CF_SuspendAllCmd, CF_Setup, CF_TearDown, "Test_CF_SuspendAllCmd");        
    UtTest_Add(Test_CF_ResumeCmd, CF_Setup, CF_TearDown, "Test_CF_ResumeCmd");
    UtTest_Add(Test_CF_ResumeAllCmd, CF_Setup, CF_TearDown, "Test_CF_ResumeAllCmd");
    UtTest_Add(Test_CF_CancelCmd, CF_Setup, CF_TearDown, "Test_CF_CancelCmd");
    UtTest_Add(Test_CF_CancelAllCmd, CF_Setup, CF_TearDown, "Test_CF_CancelAllCmd");
    UtTest_Add(Test_CF_AbandonCmd, CF_Setup, CF_TearDown, "Test_CF_AbandonCmd");
    UtTest_Add(Test_CF_AbandonAllCmd, CF_Setup, CF_TearDown, "Test_CF_AbandonAllCmd");    

    UtTest_Add(Test_CF_SetMibParamCmd, CF_Setup, CF_TearDown, "Test_CF_SetMibParamCmd");
    UtTest_Add(Test_CF_SetMibParamCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_SetMibParamCmdInvLen");    
    UtTest_Add(Test_CF_SetMibCmdUntermParam, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdUntermParam");
    UtTest_Add(Test_CF_SetMibCmdUntermValue, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdUntermValue");
    UtTest_Add(Test_CF_SetMibCmdFileChunkOverLimit, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdFileChunkOverLimit");
    UtTest_Add(Test_CF_SetMibCmdMyIdInvalid, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdMyIdInvalid");
    UtTest_Add(Test_CF_SetMibCmdAckLimit, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdAckLimit");
    UtTest_Add(Test_CF_SetMibCmdAckTimeout, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdAckTimeout");
    UtTest_Add(Test_CF_SetMibCmdInactTimeout, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdInactTimeout");
    UtTest_Add(Test_CF_SetMibCmdNakLimit, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdNakLimit");
    UtTest_Add(Test_CF_SetMibCmdNakTimeout, CF_Setup, CF_TearDown, "Test_CF_SetMibCmdNakTimeout");
    UtTest_Add(Test_CF_SetMibFileChunkSize, CF_Setup, CF_TearDown, "Test_CF_SetMibFileChunkSize");
    UtTest_Add(Test_CF_SetMibMyId, CF_Setup, CF_TearDown, "Test_CF_SetMibMyId");

    UtTest_Add(Test_CF_GetMibParamCmd, CF_Setup, CF_TearDown, "Test_CF_GetMibParamCmd");
    UtTest_Add(Test_CF_GetMibParamCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_GetMibParamCmdInvLen");
    UtTest_Add(Test_CF_GetMibCmdUntermParam, CF_Setup, CF_TearDown, "Test_CF_GetMibCmdUntermParam");
    UtTest_Add(Test_CF_GetMibParamCmdInvParam, CF_Setup, CF_TearDown, "Test_CF_GetMibParamCmdInvParam");
        
    UtTest_Add(Test_CF_SendCfgParamsCmd, CF_Setup, CF_TearDown, "Test_CF_SendCfgParamsCmd");
    UtTest_Add(Test_CF_SendCfgParamsCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_SendCfgParamsCmdInvLen");
       
    UtTest_Add(Test_CF_WriteQueueCmdCreatErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueCmdCreatErr");
    UtTest_Add(Test_CF_WriteQueueCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_WriteQueueCmdInvLen");
    UtTest_Add(Test_CF_WriteQueueUpQValueErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueUpQValueErr");
    UtTest_Add(Test_CF_WriteQueueUpDefFilename, CF_Setup, CF_TearDown, "Test_CF_WriteQueueUpDefFilename");
    UtTest_Add(Test_CF_WriteQueueUpCustomFilename, CF_Setup, CF_TearDown, "Test_CF_WriteQueueUpCustomFilename");
    UtTest_Add(Test_CF_WriteQueueOutQValueErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOutQValueErr");
    UtTest_Add(Test_CF_WriteQueueOutQTypeErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOutQTypeErr");
    UtTest_Add(Test_CF_WriteQueueOutChanErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOutChanErr");
    UtTest_Add(Test_CF_WriteQueueOutDefFilename, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOutDefFilename");
    UtTest_Add(Test_CF_WriteQueueOneEntry, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOneEntry");
    UtTest_Add(Test_CF_WriteQueueOutCustomFilename, CF_Setup, CF_TearDown, "Test_CF_WriteQueueOutCustomFilename");
    UtTest_Add(Test_CF_WriteQueueWriteHdrErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueWriteHdrErr");
    UtTest_Add(Test_CF_WriteQueueEntryWriteErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueEntryWriteErr");
    UtTest_Add(Test_CF_WriteQueueInvFilenameErr, CF_Setup, CF_TearDown, "Test_CF_WriteQueueInvFilenameErr");
    
    UtTest_Add(Test_CF_WriteActTransDefaultFilename, CF_Setup, CF_TearDown, "Test_CF_WriteActTransDefaultFilename");
    UtTest_Add(Test_CF_WriteActTransCustFilename, CF_Setup, CF_TearDown, "Test_CF_WriteActTransCustFilename");
    UtTest_Add(Test_CF_WriteActTransCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_WriteActTransCmdInvLen");
    UtTest_Add(Test_CF_WriteActTransCmdInvFilename, CF_Setup, CF_TearDown, "Test_CF_WriteActTransCmdInvFilename");
    UtTest_Add(Test_CF_WriteActTransCreatFail, CF_Setup, CF_TearDown, "Test_CF_WriteActTransCreatFail");
    UtTest_Add(Test_CF_WriteActTransWrHdrFail, CF_Setup, CF_TearDown, "Test_CF_WriteActTransWrHdrFail");
    UtTest_Add(Test_CF_WriteActTransInvWhichQs, CF_Setup, CF_TearDown, "Test_CF_WriteActTransInvWhichQs");
    UtTest_Add(Test_CF_WriteActTransEntryWriteErr, CF_Setup, CF_TearDown, "Test_CF_WriteActTransEntryWriteErr");
    
    UtTest_Add(Test_CF_InvCmdCodeCmd, CF_Setup, CF_TearDown, "Test_CF_InvCmdCodeCmd");
    UtTest_Add(Test_CF_InvMsgIdCmd, CF_Setup, CF_TearDown, "Test_CF_InvMsgIdCmd");

    UtTest_Add(Test_CF_SendTransDiagCmdSuccess, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagCmdSuccess");
    UtTest_Add(Test_CF_SendTransDiagFileNotFound, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagFileNotFound");
    UtTest_Add(Test_CF_SendTransDiagTransNotFound, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagTransNotFound");
    UtTest_Add(Test_CF_SendTransDiagCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagCmdInvLen");
    UtTest_Add(Test_CF_SendTransDiagUntermString, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagUntermString");
    UtTest_Add(Test_CF_SendTransDiagInvFilename, CF_Setup, CF_TearDown, "Test_CF_SendTransDiagInvFilename");
    
    UtTest_Add(Test_CF_SetPollParamCmd, CF_Setup, CF_TearDown, "Test_CF_SetPollParamCmd");
    UtTest_Add(Test_CF_SetPollParamCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_SetPollParamCmdInvLen");
    UtTest_Add(Test_CF_SetPollParamInvChan, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvChan");
    UtTest_Add(Test_CF_SetPollParamInvDir, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvDir");
    UtTest_Add(Test_CF_SetPollParamInvClass, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvClass");
    UtTest_Add(Test_CF_SetPollParamInvPreserve, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvPreserve");
    UtTest_Add(Test_CF_SetPollParamInvSrc, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvSrc");
    UtTest_Add(Test_CF_SetPollParamInvDst, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvDst");
    UtTest_Add(Test_CF_SetPollParamInvId, CF_Setup, CF_TearDown, "Test_CF_SetPollParamInvId");
    
    UtTest_Add(Test_CF_DeleteQueueNodeCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeCmdInvLen");
    UtTest_Add(Test_CF_DeleteQueueNodeTransUnterm, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeTransUnterm");
    UtTest_Add(Test_CF_DeleteQueueNodeInvFilename, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeInvFilename");
    UtTest_Add(Test_CF_DeleteQueueNodeFileNotFound, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeFileNotFound");
    UtTest_Add(Test_CF_DeleteQueueNodeIdNotFound, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeIdNotFound");
    UtTest_Add(Test_CF_DeleteQueueNodeUpActive, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeUpActive");
    UtTest_Add(Test_CF_DeleteQueueNodeUpHist, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeUpHist");
    UtTest_Add(Test_CF_DeleteQueueNodePbPend, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodePbPend");
    UtTest_Add(Test_CF_DeleteQueueNodePbActive, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodePbActive");
    UtTest_Add(Test_CF_DeleteQueueNodePbHist, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodePbHist");
    UtTest_Add(Test_CF_DeleteQueueNodePutFail, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodePutFail");
    UtTest_Add(Test_CF_DeleteQueueNodeInvType, CF_Setup, CF_TearDown, "Test_CF_DeleteQueueNodeInvType");
    
    UtTest_Add(Test_CF_PurgeQueueCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_PurgeQueueCmdInvLen");
    UtTest_Add(Test_CF_PurgeUplinkActive, CF_Setup, CF_TearDown, "Test_CF_PurgeUplinkActive");
    UtTest_Add(Test_CF_PurgeUpHistory, CF_Setup, CF_TearDown, "Test_CF_PurgeUpHistory");
    UtTest_Add(Test_CF_PurgeInvUpQ, CF_Setup, CF_TearDown, "Test_CF_PurgeInvUpQ");
    UtTest_Add(Test_CF_PurgeOutActive, CF_Setup, CF_TearDown, "Test_CF_PurgeOutActive");
    UtTest_Add(Test_CF_PurgeOutPend, CF_Setup, CF_TearDown, "Test_CF_PurgeOutPend");
    UtTest_Add(Test_CF_PurgeOutHist, CF_Setup, CF_TearDown, "Test_CF_PurgeOutHist");
    UtTest_Add(Test_CF_PurgeInvOutQ, CF_Setup, CF_TearDown, "Test_CF_PurgeInvOutQ");
    UtTest_Add(Test_CF_PurgeInvOutChan, CF_Setup, CF_TearDown, "Test_CF_PurgeInvOutChan");
    UtTest_Add(Test_CF_PurgeInvType, CF_Setup, CF_TearDown, "Test_CF_PurgeInvType"); 
    
    UtTest_Add(Test_CF_EnableDequeueCmd, CF_Setup, CF_TearDown, "Test_CF_EnableDequeueCmd");
    UtTest_Add(Test_CF_EnableDequeueCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_EnableDequeueCmdInvLen");
    UtTest_Add(Test_CF_EnableDequeueInvChan, CF_Setup, CF_TearDown, "Test_CF_EnableDequeueInvChan");

    UtTest_Add(Test_CF_DisableDequeueCmd, CF_Setup, CF_TearDown, "Test_CF_DisableDequeueCmd");
    UtTest_Add(Test_CF_DisableDequeueCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_DisableDequeueCmdInvLen");
    UtTest_Add(Test_CF_DisableDequeueInvChan, CF_Setup, CF_TearDown, "Test_CF_DisableDequeueInvChan");
    
    UtTest_Add(Test_CF_EnableDirPollingCmd, CF_Setup, CF_TearDown, "Test_CF_EnableDirPollingCmd");
    UtTest_Add(Test_CF_EnableDirPollingCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_EnableDirPollingCmdInvLen");
    UtTest_Add(Test_CF_EnablePollingInvChan, CF_Setup, CF_TearDown, "Test_CF_EnablePollingInvChan");
    UtTest_Add(Test_CF_EnablePollingInvDir, CF_Setup, CF_TearDown, "Test_CF_EnablePollingInvDir");
    UtTest_Add(Test_CF_EnablePollingAll, CF_Setup, CF_TearDown, "Test_CF_EnablePollingAll");
    
    UtTest_Add(Test_CF_DisableDirPollingCmd, CF_Setup, CF_TearDown, "Test_CF_DisableDirPollingCmd");
    UtTest_Add(Test_CF_DisableDirPollingCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_DisableDirPollingCmdInvLen");    
    UtTest_Add(Test_CF_DisablePollingInvChan, CF_Setup, CF_TearDown, "Test_CF_DisablePollingInvChan");
    UtTest_Add(Test_CF_DisablePollingInvDir, CF_Setup, CF_TearDown, "Test_CF_DisablePollingInvDir");
    UtTest_Add(Test_CF_DisablePollingAll, CF_Setup, CF_TearDown, "Test_CF_DisablePollingAll");
    
    UtTest_Add(Test_CF_KickStartCmd, CF_Setup, CF_TearDown, "Test_CF_KickStartCmd");
    UtTest_Add(Test_CF_KickStartCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_KickStartCmdInvLen");
    UtTest_Add(Test_CF_KickStartCmdInvChan, CF_Setup, CF_TearDown, "Test_CF_KickStartCmdInvChan");
    
    UtTest_Add(Test_CF_QuickStatusFilenameCmd, CF_Setup, CF_TearDown, "Test_CF_QuickStatusFilenameCmd");    
    UtTest_Add(Test_CF_QuickStatusTransCmd, CF_Setup, CF_TearDown, "Test_CF_QuickStatusTransCmd");
    UtTest_Add(Test_CF_QuickStatusActiveTrans, CF_Setup, CF_TearDown, "Test_CF_QuickStatusActiveTrans");
    UtTest_Add(Test_CF_QuickStatusActiveName, CF_Setup, CF_TearDown, "Test_CF_QuickStatusActiveName"); 
    UtTest_Add(Test_CF_QuickStatusActiveSuspended, CF_Setup, CF_TearDown, "Test_CF_QuickStatusActiveSuspended"); 
    UtTest_Add(Test_CF_QuickStatusCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_QuickStatusCmdInvLen");
    UtTest_Add(Test_CF_QuickStatusUntermString, CF_Setup, CF_TearDown, "Test_CF_QuickStatusUntermString");
    UtTest_Add(Test_CF_QuickStatusInvFilename, CF_Setup, CF_TearDown, "Test_CF_QuickStatusInvFilename");

    UtTest_Add(Test_CF_PbFileNoMem, CF_Setup, CF_TearDown, "Test_CF_PbFileNoMem");
    UtTest_Add(Test_CF_PbFileCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_PbFileCmdInvLen");
    UtTest_Add(Test_CF_PbFileCmdParamErr, CF_Setup, CF_TearDown, "Test_CF_PbFileCmdParamErr");
    UtTest_Add(Test_CF_PbFileChanNotInUse, CF_Setup, CF_TearDown, "Test_CF_PbFileChanNotInUse");
    UtTest_Add(Test_CF_PbFileInvSrcFilename, CF_Setup, CF_TearDown, "Test_CF_PbFileInvSrcFilename");
    UtTest_Add(Test_CF_PbFileInvDstFilename, CF_Setup, CF_TearDown, "Test_CF_PbFileInvDstFilename");
    UtTest_Add(Test_CF_PbFilePendQFull, CF_Setup, CF_TearDown, "Test_CF_PbFilePendQFull");
    UtTest_Add(Test_CF_PbFileInvPeerId, CF_Setup, CF_TearDown, "Test_CF_PbFileInvPeerId");
    UtTest_Add(Test_CF_PbFileFileOpen, CF_Setup, CF_TearDown, "Test_CF_PbFileFileOpen");
    UtTest_Add(Test_CF_PbFileFileOnQ, CF_Setup, CF_TearDown, "Test_CF_PbFileFileOnQ");
    
    UtTest_Add(Test_CF_PbDirCmd, CF_Setup, CF_TearDown, "Test_CF_PbDirCmd");
    UtTest_Add(Test_CF_PbDirCmdOpenErr, CF_Setup, CF_TearDown, "Test_CF_PbDirCmdOpenErr");
    UtTest_Add(Test_CF_PbDirCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_PbDirCmdInvLen");
    UtTest_Add(Test_CF_PbDirCmdParamErr, CF_Setup, CF_TearDown, "Test_CF_PbDirCmdParamErr");
    UtTest_Add(Test_CF_PbDirChanNotInUse, CF_Setup, CF_TearDown, "Test_CF_PbDirChanNotInUse");
    UtTest_Add(Test_CF_PbDirInvSrcPath, CF_Setup, CF_TearDown, "Test_CF_PbDirInvSrcPath");
    UtTest_Add(Test_CF_PbDirInvDstPath, CF_Setup, CF_TearDown, "Test_CF_PbDirInvDstPath"); 
    UtTest_Add(Test_CF_PbDirInvPeerId, CF_Setup, CF_TearDown, "Test_CF_PbDirInvPeerId"); 

    UtTest_Add(Test_CF_QDirFilesQFull, CF_Setup, CF_TearDown, "Test_CF_QDirFilesQFull");
    UtTest_Add(Test_CF_QDirFilesNoMem, CF_Setup, CF_TearDown, "Test_CF_QDirFilesNoMem");    
    UtTest_Add(Test_CF_QDirFilesFileOnQ, CF_Setup, CF_TearDown, "Test_CF_QDirFilesFileOnQ");
    UtTest_Add(Test_CF_QDirFilesFileOpen, CF_Setup, CF_TearDown, "Test_CF_QDirFilesFileOpen");
    UtTest_Add(Test_CF_QDirFilesAllGood, CF_Setup, CF_TearDown, "Test_CF_QDirFilesAllGood");

    UtTest_Add(Test_CF_PbQueueRemoveFirst, CF_Setup, CF_TearDown, "Test_CF_PbQueueRemoveFirst");
    UtTest_Add(Test_CF_PbQueueRemoveMiddle, CF_Setup, CF_TearDown, "Test_CF_PbQueueRemoveMiddle");
    UtTest_Add(Test_CF_PbQueueRemoveLast, CF_Setup, CF_TearDown, "Test_CF_PbQueueRemoveLast");
    UtTest_Add(Test_CF_PbQueueRemoveNull, CF_Setup, CF_TearDown, "Test_CF_PbQueueRemoveNull");
    
    UtTest_Add(Test_CF_PbQueueInsertInvChan, CF_Setup, CF_TearDown, "Test_CF_PbQueueInsertInvChan");
    UtTest_Add(Test_CF_PbQueueInsertInvQ, CF_Setup, CF_TearDown, "Test_CF_PbQueueInsertInvQ");
    UtTest_Add(Test_CF_PbQueueInsertGood, CF_Setup, CF_TearDown, "Test_CF_PbQueueInsertGood");
    
    UtTest_Add(Test_CF_PbQueueFrontGood, CF_Setup, CF_TearDown, "Test_CF_PbQueueFrontGood");
    UtTest_Add(Test_CF_PbQueueFrontInvChan, CF_Setup, CF_TearDown, "Test_CF_PbQueueFrontInvChan");
    
    //int32 CF_AddFileToUpQueue(uint32 Queue, CF_QueueEntry_t *NewNode)
    UtTest_Add(Test_CF_AddUpQueueInvNewNode, CF_Setup, CF_TearDown, "Test_CF_AddUpQueueInvNewNode");
    UtTest_Add(Test_CF_AddUpQueueSecondNode, CF_Setup, CF_TearDown, "Test_CF_AddUpQueueSecondNode");
    
    UtTest_Add(Test_CF_RemoveFirstUpNode, CF_Setup, CF_TearDown, "Test_CF_RemoveFirstUpNode");
    UtTest_Add(Test_CF_RemoveMiddleUpNode, CF_Setup, CF_TearDown, "Test_CF_RemoveMiddleUpNode");
    UtTest_Add(Test_CF_RemoveLastUpNode, CF_Setup, CF_TearDown, "Test_CF_RemoveLastUpNode");
  
    UtTest_Add(Test_CF_GiveSemInvParamCmd, CF_Setup, CF_TearDown, "Test_CF_GiveSemaphoreCmd");
#if 0
    UtTest_Add(Test_CF_TakeSemaphoreCmd, CF_Setup, CF_TearDown, "Test_CF_TakeSemaphoreCmd");
    UtTest_Add(Test_CF_GiveSemaphoreCmd, CF_Setup, CF_TearDown, "Test_CF_GiveSemInvParamCmd");
    UtTest_Add(Test_CF_GiveTakeCmdInvLen, CF_Setup, CF_TearDown, "Test_CF_GiveTakeCmdInvLen");
    UtTest_Add(Test_CF_GiveTakeCmdInvChan, CF_Setup, CF_TearDown, "Test_CF_GiveTakeCmdInvChan");
    UtTest_Add(Test_CF_GiveTakeCmdSemErr, CF_Setup, CF_TearDown, "Test_CF_GiveTakeCmdSemErr");
#endif


        
}


  

