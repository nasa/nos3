/*
** File: utf_test10.c
**
** Purpose: This test makes calls to software bus api.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2009/10/13 14:23:44GMT-05:00 $
** $Revision: 1.2 $
** $Log: utf_test10.c  $
** Revision 1.2 2009/10/13 14:23:44GMT-05:00 wmoleski 
** 
** Revision 1.1 2008/04/17 08:08:57EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test10/project.pj
** Revision 1.7 2007/08/27 09:56:41EDT wfmoleski 
** Modified UTF to match the implemetation of SB_GetLastSenderID.
** Revision 1.6 2007/04/12 10:59:10EDT wfmoleski 
** Changes for 3159:1
** Revision 1.5 2006/08/01 08:35:33EDT wfmoleski 
** Changes made during the conversion to cFE 3.3.0
** Revision 1.4 2006/06/20 11:17:58EDT wfmoleski 
** Added tests to the source file to exercise the new DeletePipe functions.
** Revision 1.3 2006/04/10 15:47:39EDT sslegel 
** Updated code to test new hooks
**
*/

#include "cfe.h"
#include "ccsds.h"
#include "cfe_sb.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe_sb.h"
#include "utf_osfilesys.h"
#include "utf_osapi.h"

typedef struct
{
    uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8                 CmdCount;
    uint8                 ErrCount;
} TestPacket_t;

int32 CFE_SB_SubscribeHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId) {

    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_SB_MAX_MSGS_MET);
}

int32 CFE_SB_SubscribeExHook(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim) {

    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_SB_MAX_MSGS_MET);
}

int32 CFE_SB_CreatePipeHook(CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, char *PipeName) {

    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_SB_PIPE_CR_ERR);
}

int32 CFE_SB_DeletePipeHook(CFE_SB_PipeId_t PipeId) {

    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_SB_BAD_ARGUMENT);
}

int main(void)
{

    CFE_SB_PipeId_t TempPipeId = 0;
    CFE_SB_MsgPtr_t TestMsg;
    TestPacket_t    TestPacket;
    CFE_SB_SenderId_t *lastSender;
    lastSender = malloc(sizeof(CFE_SB_SenderId_t));

	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test10.ActualOutput");
    UTF_init_sim_time(1.5);

    /* Create Pipe Test */
    UTF_put_text("Create Pipe Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_CREATEPIPE_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("Create Pipe Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_CREATEPIPE_PROC);
    UTF_put_text("Create Pipe Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));

    /* Test create pipe hook for returning good and bad return codes */
    UTF_SB_set_function_hook(CFE_SB_CREATEPIPE_HOOK, (void *)&CFE_SB_CreatePipeHook);
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));
    UTF_put_text("CreatePipe Hook Return Code %x\n", CFE_SB_CreatePipe(&TempPipeId, 1, "My Pipe"));

    /* Delete Pipe Test */
    UTF_put_text("Delete Pipe Return Code %x\n", CFE_SB_DeletePipe(0));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_DELETEPIPE_PROC, CFE_SB_BAD_ARGUMENT);
    UTF_put_text("Delete Pipe Return Code %x\n", CFE_SB_DeletePipe(1));
    
    UTF_SB_set_function_hook(CFE_SB_DELETEPIPE_HOOK, (void *)&CFE_SB_DeletePipeHook);
    UTF_put_text("DeletePipe Hook Return Code %x\n", CFE_SB_DeletePipe(0));
    UTF_put_text("DeletePipe Hook Return Code %x\n", CFE_SB_DeletePipe(1));
    UTF_put_text("DeletePipe Hook Return Code %x\n", CFE_SB_DeletePipe(2));
    UTF_put_text("DeletePipe Hook Return Code %x\n", CFE_SB_DeletePipe(3));
    
    /* SubscribeEx Test */
    CFE_ES_PerfLogEntry(1);
    UTF_put_text("SubscribeEx Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBEEX_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("SubscribeEx Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_SUBSCRIBEEX_PROC);
    UTF_put_text("SubscribeEx Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
	CFE_ES_PerfLogExit(1);
	
    /* Test subscribeex hook for returning good and bad return codes */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBEEX_HOOK, (void *)&CFE_SB_SubscribeExHook);
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));
    UTF_put_text("SubscribeEx Hook Return Code %x\n", CFE_SB_SubscribeEx(CFE_SB_SEND_HK_MID, TempPipeId, CFE_SB_Default_Qos, 10));

    /* Subscribe Test */
    UTF_put_text("Subscribe Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("Subscribe Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_SUBSCRIBE_PROC);
    UTF_put_text("Subscribe Return Code %x\n", CFE_SB_Subscribe(0, 0));

    /* Test subscribe hook for returning good and bad return codes */
    UTF_SB_set_function_hook(CFE_SB_SUBSCRIBE_HOOK, (void *)&CFE_SB_SubscribeHook);
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));
    UTF_put_text("Subscribe Hook Return Code %x\n", CFE_SB_Subscribe(0, 0));

	/* Set a new output file name */
	UTF_set_output_filename("utf_test10.ActualOutput2");

    /* Unsubscribe Test */
    UTF_put_text("Unsubscribe Return Code %x\n", CFE_SB_Unsubscribe(CFE_SB_SEND_HK_MID, TempPipeId));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_UNSUBSCRIBE_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("Unsubscribe Return Code %x\n", CFE_SB_Unsubscribe(CFE_SB_SEND_HK_MID, TempPipeId));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_UNSUBSCRIBE_PROC);
    UTF_put_text("Unsubscribe Return Code %x\n", CFE_SB_Unsubscribe(CFE_SB_SEND_HK_MID, TempPipeId));

    /* Send Msg Test */
    CFE_SB_InitMsg(&TestPacket, CFE_SB_HK_TLM_MID, sizeof(TestPacket_t), TRUE);
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *)&TestPacket);
    UTF_put_text("Send Message Return Code %x\n", CFE_SB_SendMsg((CFE_SB_Msg_t *)&TestPacket));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_SENDMSG_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("Send Message Return Code %x\n", CFE_SB_SendMsg((CFE_SB_Msg_t *)&TestPacket));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_SENDMSG_PROC);
    UTF_put_text("Send Message Return Code %x\n", CFE_SB_SendMsg((CFE_SB_Msg_t *)&TestPacket));

    /* Recv Msg Test */
    UTF_put_text("Recv Message Return Code %x\n", CFE_SB_RcvMsg(&TestMsg, TempPipeId, CFE_SB_PEND_FOREVER));
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_RCVMSG_PROC, CFE_SB_INTERNAL_ERR);
    UTF_put_text("Recv Message Return Code %x\n", CFE_SB_RcvMsg(&TestMsg, TempPipeId, CFE_SB_PEND_FOREVER));
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_RCVMSG_PROC);
    UTF_put_text("Recv Message Return Code %x\n", CFE_SB_RcvMsg(&TestMsg, TempPipeId, CFE_SB_PEND_FOREVER));

    /* Get Last Sender ID Test */
    UTF_put_text("Get Last Sender ID Return Code %x\n", CFE_SB_GetLastSenderId(&lastSender, TempPipeId));
	UTF_put_text("Last Sender returned ProcessorID %d and AppName %s\n",lastSender->ProcessorId, lastSender->AppName);
    UTF_CFE_SB_Set_Api_Return_Code(CFE_SB_GETLASTSENDERID_PROC, CFE_SB_BAD_ARGUMENT);
    UTF_put_text("Get Last Sender ID Return Code = %x; expected %x\n", CFE_SB_GetLastSenderId(&lastSender, TempPipeId), CFE_SB_BAD_ARGUMENT);
	UTF_put_text("Last Sender returned ProcessorID %d and AppName %s\n",lastSender->ProcessorId, lastSender->AppName);
    UTF_CFE_SB_Use_Default_Api_Return_Code(CFE_SB_GETLASTSENDERID_PROC);
    UTF_put_text("Get Last Sender ID Return Code %x\n", CFE_SB_GetLastSenderId(&lastSender, TempPipeId));
	UTF_put_text("Last Sender returned ProcessorID %d and AppName %s\n",lastSender->ProcessorId, lastSender->AppName);

    /* Zero Copy Get Pointer Test  - NOT IMPLEMENTED in the UTF */
    /* Zero Copy Release Pointer Test - NOT IMPLEMENTED in the UTF */

    /* Zero Copy Send Test */
    CFE_SB_ZeroCopyHandle_t zcHdl;
    CFE_SB_InitMsg(&TestPacket, CFE_SB_HK_TLM_MID, sizeof(TestPacket_t), TRUE);
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *)&TestPacket);
    UTF_put_text("Zero Copy Send Return Code %x\n", CFE_SB_ZeroCopySend((CFE_SB_Msg_t *)&TestPacket,zcHdl));

    /* Zero Copy Pass Test */
    UTF_put_text("Zero Copy Pass Return Code %x\n", CFE_SB_ZeroCopyPass((CFE_SB_Msg_t *)&TestPacket,zcHdl));

	exit(0);
}
