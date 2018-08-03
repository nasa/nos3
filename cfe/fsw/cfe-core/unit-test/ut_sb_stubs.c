/*
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** File:
** $Id: ut_sb_stubs.c 1.4 2014/05/28 09:21:45GMT-05:00 wmoleski Exp  $
**
** Purpose:
** Unit test stubs for Software Bus routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.4 $
** $Log: ut_sb_stubs.c  $
** Revision 1.4 2014/05/28 09:21:45GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.3 2012/10/01 18:46:54EDT aschoeni 
** Removed relative paths in includes, this is now done by makefile
** Revision 1.2 2012/01/13 13:59:31EST acudmore 
** Added license text
** Revision 1.1 2008/04/17 08:05:46EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.9 2007/05/30 15:15:22EDT njyanchik 
** check in of ES Unit test
** Revision 1.8 2007/05/21 08:17:37EDT njyanchik 
** A check in of ES's Unit test to save my work
** Revision 1.7 2007/05/16 11:14:37EDT njyanchik 
** Update ES's unit test driver to match code for build 4.1
** Revision 1.6 2007/05/10 15:14:25EDT njyanchik 
** Another update of Jonathans UT
** Revision 1.5 2007/05/07 13:45:37EDT njyanchik 
** EVS's Unit test drivers have been updated
** Revision 1.4 2007/05/04 09:10:24EDT njyanchik 
** Check in of Time UT and related changes
** Revision 1.3 2007/05/01 13:28:14EDT njyanchik 
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.2 2007/01/17 09:26:07EST njyanchik 
** Check in of ES Unit Test changed files
** Revision 1.1 2006/03/02 15:10:26GMT-05:00 jjhageman 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_sb.h"
#include "common_types.h"
#include "cfe_evs_task.h"
#include "ut_stubs.h"

/*
** External global variables
*/
extern uint16 UT_RcvMsgCode;
extern int    UT_SB_TotalMsgLen;
extern char   cMsg[];

extern CFE_SB_MsgId_t       UT_RcvMsgId;

extern UT_SetRtn_t SB_SubscribeRtn;
extern UT_SetRtn_t SB_SubscribeExRtn;
extern UT_SetRtn_t SB_SubscribeLocalRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SBSendMsgRtn;
extern UT_SetRtn_t SBCleanUpRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t SetMsgIdRtn;

/*
** Global variables
*/
CFE_SB_Qos_t CFE_SB_Default_Qos;
CFE_SB_Msg_t UT_message;
/*
**  Functions prototypes
*/
extern void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr);

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_SB_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_EarlyInit(void)
{
    return CFE_SUCCESS;
}
    
/*****************************************************************************/
/**
** \brief CFE_SB_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TaskMain(void)
{
}

/*****************************************************************************/
/**
** \brief CFE_SB_CreatePipe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_CreatePipe.  The user can adjust the response by setting
**        the values in the SB_CreatePipeRtn structure prior to this function
**        being called.  If the value SB_CreatePipeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_CreatePipeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth,
                        const char *PipeName)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (SB_CreatePipeRtn.count > 0)
    {
        SB_CreatePipeRtn.count--;

        if (SB_CreatePipeRtn.count == 0)
        {
#ifdef UT_VERBOSE
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_SB_CreatePipe called: %ld",
                     SB_CreatePipeRtn.value);
            UT_Text(cMsg);
            flag = TRUE;
#endif
            status = SB_CreatePipeRtn.value;
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        UT_Text("  CFE_SB_CreatePipe called");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetCmdCode stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetCmdCode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either the function code from command secondary header or
**        CFE_SUCCESS.
**
******************************************************************************/
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
    uint16          status = CFE_SUCCESS;
    CFE_SB_CmdHdr_t *CmdHdrPtr;

    /* If msg type isn't telemetry */
    if (CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_TLM)
    {
        /* Cast the input pointer to a Cmd Msg pointer */
        CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;
        status = CCSDS_RD_FC(CmdHdrPtr->Sec);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetMsgId stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetMsgId.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the entire stream ID from the primary header.
**
******************************************************************************/
CFE_SB_MsgId_t CFE_SB_GetMsgId(CFE_SB_MsgPtr_t MsgPtr)
{
    return CCSDS_RD_SID(MsgPtr->Hdr);
}

/*****************************************************************************/
/**
** \brief CFE_SB_InitMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_InitMsg.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_InitMsg(void *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16 Length,
                    boolean Clear)
{
#ifdef UT_VERBOSE
    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  CFE_SB_InitMsg called: initialized msg %d", MsgId);
    UT_Text(cMsg);
#endif
    CFE_SB_SetMsgId(MsgPtr, MsgId);
}

/*****************************************************************************/
/**
** \brief CFE_SB_RcvMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_RcvMsg.  It returns one successful command, then fails on a
**        subsequent call.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS on the first call, then -1 on the second.
**
******************************************************************************/
int32 CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *BufPtr,
                    CFE_SB_PipeId_t PipeId,
                    int32 TimeOut)
{
    int32      status = CFE_SUCCESS;
    static int NumRuns = 0;

    if (NumRuns == 0)
    {
        CFE_SB_SetMsgId(&UT_message, UT_RcvMsgId);
        CFE_SB_SetCmdCode(&UT_message, UT_RcvMsgCode);
        *BufPtr = &UT_message;
        NumRuns++;
    }
    else
    {
        if (NumRuns == 1)
        {
            status = CFE_SB_TIME_OUT;
            NumRuns++;
        }
        else
        {
            if (NumRuns == 2)
            {
                status = -1;
                NumRuns = 0;
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SendMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SendMsg.  The user can adjust the response by setting
**        the values in the SBSendMsgRtn structure prior to this function
**        being called.  If the value SBSendMsgRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SBSendMsgRtn.value.
**        CFE_SUCCESS is returned otherwise.  Only EVS and TIME messages are
**        handled directly by this function; other messages are passed to the
**        unit test function, UT_ProcessSBMsg, for any further action.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
/* Only doing subset of total messages;
** NOTE: Currently does EVS, TIME
*/
int32 CFE_SB_SendMsg(CFE_SB_Msg_t *MsgPtr)
{
    int32            status = CFE_SUCCESS;
    boolean          flag = FALSE;
    CFE_EVS_Packet_t *packet;
#ifdef UT_VERBOSE
    int              i;
    CFE_EVS_TlmPkt_t *tlmpkt;
#endif

    if (SBSendMsgRtn.count > 0)
    {
        SBSendMsgRtn.count--;

        if (SBSendMsgRtn.count == 0)
        {
            status = SBSendMsgRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Process message per type */
        switch (CFE_SB_GetMsgId(MsgPtr))
        {
            case CFE_EVS_EVENT_MSG_MID:

                packet = (CFE_EVS_Packet_t *) MsgPtr;
                SendMsgEventIDRtn.value = packet->Payload.PacketID.EventID;
#ifdef UT_VERBOSE
                snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
"  CFE_SB_SendMsg called: type CFE_EVS_EVENT_MSG_MID\n  %lu/%lu/%s %u:%s",
                        packet->Payload.PacketID.SpacecraftID,
                        packet->Payload.PacketID.ProcessorID,
                        packet->Payload.PacketID.AppName,
                        packet->Payload.PacketID.EventID,
                        packet->Payload.Message);
                UT_Text(cMsg);
#endif

                if (*packet->Payload.Message != '\0')
                {
                    SendMsgEventIDRtn.count++;
                }

                break;

            case CFE_EVS_HK_TLM_MID:

                SendMsgEventIDRtn.value = CFE_EVS_HK_TLM_MID;
#ifdef UT_VERBOSE
                tlmpkt = (CFE_EVS_TlmPkt_t *) MsgPtr;
                snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                         "  CFE_SB_SendMsg: type CFE_EVS_HK_TLM_MID\n"
                           "   CommandCounter = %d\n"
                           "   CommandErrCounter = %d\n"
                           "   MessageFormatMode = %d\n"
                           "   MessageSendCounter = %d\n"
                           "   MessageTruncCounter = %d\n"
                           "   UnregisteredAppCounter = %d\n"
                           "   OutputPort = %d\n   LogFullFlag = %d\n"
                           "   LogMode = %d\n   LogOverflowCounter = %d",
                         tlmpkt->Payload.CommandCounter, tlmpkt->Payload.CommandErrCounter,
                         tlmpkt->Payload.MessageFormatMode, tlmpkt->Payload.MessageSendCounter,
                         tlmpkt->Payload.MessageTruncCounter,
                         tlmpkt->Payload.UnregisteredAppCounter, tlmpkt->Payload.OutputPort,
                         tlmpkt->Payload.LogFullFlag, tlmpkt->Payload.LogMode,
                         tlmpkt->Payload.LogOverflowCounter);
                UT_Text(cMsg);

                for (i = 0; i < CFE_ES_MAX_APPLICATIONS; i++)
                {
                    snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                             "   AppID = %lu\n    AppEnableStatus = %d\n    "
                               "AppMessageSentCounter = %d",
                             tlmpkt->Payload.AppData[i].AppID,
                             tlmpkt->Payload.AppData[i].AppEnableStatus,
                             tlmpkt->Payload.AppData[i].AppMessageSentCounter);
                    UT_Text(cMsg);
               }
#endif
                break;

            case CFE_TIME_DIAG_TLM_MID:

                SendMsgEventIDRtn.value = CFE_TIME_DIAG_TLM_MID;
#ifdef UT_VERBOSE
                snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                         "Incomplete TIME command: %d",
                         CFE_SB_GetMsgId(MsgPtr));
                UT_Text(cMsg);
#endif
                break;

            case CFE_TIME_HK_TLM_MID:

                SendMsgEventIDRtn.value = CFE_TIME_HK_TLM_MID;
#ifdef UT_VERBOSE
                snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                         "Incomplete TIME housekeeping command: %d",
                         CFE_SB_GetMsgId(MsgPtr));
                UT_Text(cMsg);
#endif
                break;

            default:
                /* Process the message with subsystem specific call */
                UT_ProcessSBMsg(MsgPtr);
                break;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SetCmdCode stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetCmdCode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either CFE_SB_WRONG_MSG_TYPE or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr, uint16 CmdCode)
{
    int32           status = CFE_SUCCESS;
    CFE_SB_CmdHdr_t *CmdHdrPtr;

    /* If msg type is telemetry, ignore the request */
    if (CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)
    {
        status = CFE_SB_WRONG_MSG_TYPE;
    }
    else
    {
        /* Cast the input pointer to a Cmd Msg pointer */
        CmdHdrPtr = (CFE_SB_CmdHdr_t *) MsgPtr;
        CCSDS_WR_FC(CmdHdrPtr->Sec,CmdCode);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SetMsgId stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetMsgId.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr, CFE_SB_MsgId_t MsgId)
{
    SetMsgIdRtn.value = MsgId;
    SetMsgIdRtn.count++;
    CCSDS_WR_SID(MsgPtr->Hdr, MsgId);
}


/*****************************************************************************/
/**
** \brief CFE_SB_SetMsgTime stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetMsgTime.  It always returns CFE_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t time)
{
#ifdef UT_VERBOSE
    UT_Text("  CFE_SB_SetMsgTime called");
#endif
    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeEx.  The user can adjust the response by setting
**        the values in the SB_SubscribeExRtn structure prior to this function
**        being called.  If the value SB_SubscribeExRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        SB_SubscribeExRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                         CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (SB_SubscribeExRtn.count > 0)
    {
        SB_SubscribeExRtn.count--;

        if (SB_SubscribeExRtn.count == 0)
        {
            status = SB_SubscribeExRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_SB_SubscribeEx called: %ld",
                     SB_SubscribeExRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_SB_SubscribeEx called: subscribed to msg %d", MsgId);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_Subscribe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_Subscribe.  The user can adjust the response by setting
**        the values in the SB_SubscribeRtn structure prior to this function
**        being called.  If the value SB_SubscribeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_SubscribeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (SB_SubscribeRtn.count > 0)
    {
        SB_SubscribeRtn.count--;

        if (SB_SubscribeRtn.count == 0)
        {
            status = SB_SubscribeRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_SB_Subscribe called: %ld", SB_SubscribeRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_SB_Subscribe called: subscribed to msg %d", MsgId);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeLocal stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeLocal.  The user can adjust the response by setting
**        the values in the SB_SubscribeLocalRtn structure prior to this
**        function being called.  If the value SB_SubscribeLocalRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        SB_SubscribeLocalRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId,
                            CFE_SB_PipeId_t PipeId,
                            uint16 MsgLim)
{
    int32   status = CFE_SUCCESS;
#ifdef UT_VERBOSE
    boolean flag = FALSE;
#endif

    if (SB_SubscribeLocalRtn.count > 0)
    {
        SB_SubscribeLocalRtn.count--;

        if (SB_SubscribeLocalRtn.count == 0)
        {
            status = SB_SubscribeLocalRtn.value;
#ifdef UT_VERBOSE
            flag = TRUE;
            snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  CFE_SB_Subscribe called: %ld",
                     SB_SubscribeLocalRtn.value);
            UT_Text(cMsg);
#endif
        }
    }

#ifdef UT_VERBOSE
    if (flag == FALSE)
    {
        snprintf(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  CFE_SB_SubscribeLocal called: subscribed to msg %d",
                 MsgId);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TimeStampMsg stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TimeStampMsg.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef UT_VERBOSE
    UT_Text("  CFE_SB_TimeStampMsg called");
#endif
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetTotalMsgLength stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_GetTotalMsgLength.  It returns the user-defined value,
**        UT_SB_TotalMsgLen.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status value, UT_SB_TotalMsgLen.
**
******************************************************************************/
uint16 CFE_SB_GetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr)
{
    return UT_SB_TotalMsgLen;
}

/*****************************************************************************/
/**
** \brief CFE_SB_CleanUpApp stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_CleanUpApp.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
int32 CFE_SB_CleanUpApp(uint32 AppId)
{
    int32 status = CFE_SUCCESS;

    if (SBCleanUpRtn.count > 0)
    {
      SBCleanUpRtn.count--;

      if (SBCleanUpRtn.count == 0)
      {
          status = SBCleanUpRtn.value;
      }
    }

    return status;
}

/******************************************************************************
**  Function:  CFE_SB_MessageStringGet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    if (DestMaxSize == 0)
    {
        return CFE_SB_BAD_ARGUMENT;
    }

    /*
     * Check if should use the default, which is if
     * the source string has zero length (first char is NUL).
     */
    if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
    {
        SourceStringPtr = DefaultString;
        SourceMaxSize = DestMaxSize;
    }

    /* For the UT implementation, just call strncpy() */
    strncpy(DestStringPtr, SourceStringPtr, DestMaxSize - 1);
    DestStringPtr[DestMaxSize - 1] = 0;

    return strlen(DestStringPtr);
}


/******************************************************************************
**  Function:  CFE_SB_MessageStringSet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    if (DestMaxSize == 0)
    {
        return CFE_SB_BAD_ARGUMENT;
    }

    /* For the UT implementation, just call strncpy() */
    strncpy(DestStringPtr, SourceStringPtr, DestMaxSize);
    if (DestStringPtr[DestMaxSize - 1] != 0)
    {
        return DestMaxSize;
    }

    return strlen(DestStringPtr);
}
