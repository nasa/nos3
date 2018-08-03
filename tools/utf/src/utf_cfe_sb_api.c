/*
** File: utf_cfe_sb_api.c
** $Id: utf_cfe_sb_api.c 1.5 2012/01/13 12:51:59GMT-05:00 acudmore Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**  
**
** Purpose: This module defines the UTF versions of functions for the SB APIs.  The real
**          functions are contained in the file cfe_sb_api.c.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2012/01/13 12:51:59GMT-05:00 $
** $Revision: 1.5 $
** $Log: utf_cfe_sb_api.c  $
** Revision 1.5 2012/01/13 12:51:59GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.4 2010/10/25 15:06:34EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.3 2010/10/04 14:57:27EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.2 2009/10/13 14:55:23EDT wmoleski 
** Changes required to be compatible with cFE 6.0.0
** Revision 1.1 2008/04/17 08:09:37EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.6 2007/08/27 09:56:41EDT wfmoleski 
** Modified UTF to match the implemetation of SB_GetLastSenderID.
** Revision 1.5 2006/09/05 10:13:38EDT wfmoleski 
** Added a special return code UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE that is processed by the UTF's
** version of CFE_SB_RcvMsg. If this return code is set and an enpty pipe is encountered, the test process will exit.
** Revision 1.4 2006/06/21 15:37:51EDT wfmoleski 
** Added copyright prologs to utf_sb files
** Revision 1.3 2006/06/20 10:52:59EDT wfmoleski 
** Added the implementation of the CFE_DeletePipe function. Also, the capability to define a hook function
** for DeletePipe is implemented.
** Revision 1.2 2006/03/24 12:38:31EST sslegel 
** Removed unused routines
** Added hooks for subscribeex and createpipe
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
** Revision 1.2 2005/12/01 18:46:21EST NSSCHWEISS
** utf 2.3.0.
** Revision 1.1 2005/09/26 11:30:12EDT lswalling
** Initial revision
** Member added to project d:/mksdata/MKS-LRO-REPOSITORY/utf/src_utf/project.pj
** 2005/11/07 nschweiss Modified to allow return codes to be set for each API.
** 2005/09/15 nschweiss Updated CreatePipe with definition supplied by Scott Walling.
** 2005/08/10 nschweiss Swapped order of arguments in RcvMsg() to be consistent with revised api.
** 2005/08/08 nschweiss Added include of new cfe_error.h file.
** 2005/07/29 nschweiss Undo mod of 7/18 which added a UTF_exit.
**     This worked fine for previous case, but I believe it would
**     cause trouble when multiple input files are used.
** 2005/07/19 nschweiss Revise CFE_SB_SendMsg.
**     printf changed to UTF_put_text.
** 2005/07/18 nschweiss Revise to call UTF_exit & close
**     all files when there are no more input messages.
** 2005/07/13 nschweiss Revise CFE_SB_RcvMsg for cFE UTF.
** Revision 1.1 2005/06/09 10:57:53EDT rperera
*/

/*
** Include Files
*/

#include "common_types.h"
#include "ccsds.h"
#include "cfe_sb.h"
#include "cfe_sb_priv.h"
#include "osapi.h"
#include "cfe_error.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe_sb.h"

/*
** Global Variables
*/

/* Default Quality Of Service Data Structure */
CFE_SB_Qos_t            CFE_SB_Default_Qos = { CFE_SB_QOS_LOW_PRIORITY, CFE_SB_QOS_LOW_RELIABILITY };
/* Declare a sender structure for the GetLastSenderID call */
CFE_SB_SenderId_t utf_lastSender;

/* Function Hook Stuff */
typedef struct
{
    int32 (*CFE_SB_Subscribe)(CFE_SB_MsgId_t, CFE_SB_PipeId_t);
    int32 (*CFE_SB_SubscribeEx)(CFE_SB_MsgId_t, CFE_SB_PipeId_t, CFE_SB_Qos_t, uint16);
    int32 (*CFE_SB_CreatePipe)(CFE_SB_PipeId_t *, uint16, char *);
    int32 (*CFE_SB_DeletePipe)(CFE_SB_PipeId_t);
} UTF_SB_HookTable_t;

UTF_SB_HookTable_t UTF_SB_HookTable = {
    NULL,
    NULL,
    NULL,
    NULL
};

/* Return Code Stuff */
int32 cfe_sb_api_return_value[NUM_OF_CFE_SB_API_PROCS]=
{
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE
};

/******************************************************************************
**  Function: UTF_SB_set_function_hook()
**
**  Purpose:
**    Install a user defined hook function for a SB function call.
*/
void UTF_SB_set_function_hook(int Index, void *FunPtr)
{
    if      (Index == CFE_SB_SUBSCRIBE_HOOK)   { UTF_SB_HookTable.CFE_SB_Subscribe = FunPtr; }
    else if (Index == CFE_SB_SUBSCRIBEEX_HOOK) { UTF_SB_HookTable.CFE_SB_SubscribeEx = FunPtr; }
    else if (Index == CFE_SB_CREATEPIPE_HOOK)  { UTF_SB_HookTable.CFE_SB_CreatePipe = FunPtr; }
    else if (Index == CFE_SB_DELETEPIPE_HOOK)  { UTF_SB_HookTable.CFE_SB_DeletePipe = FunPtr; }
    else                                       { UTF_error("Invalid SB Hook Index In Set Hook Call %d", Index); }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_SB_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_SB_Set_Api_Return_Code(int32 ApiIndex, int32 ReturnCode)
{
    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_SB_API_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_SB_Set_Api_Return_Code\n", ApiIndex);
        return -1;
    }
    /* If index is valid, assign return value and report action */
    cfe_sb_api_return_value[ApiIndex] = ReturnCode;

    if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        UTF_put_text("UTF Info: Return code for CFE SB Api #%d returned to default\n", ApiIndex);
    }
    else
    {
        UTF_put_text("UTF Info: Return value for CFE SB Api #%d set to %lx\n", ApiIndex, ReturnCode);
    }
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_SB_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_SB_Use_Default_Api_Return_Code(int32 ApiIndex)
{
    int32 Success;

    /* Check that ApiIndex is in the valid range */
    if ( (ApiIndex >= NUM_OF_CFE_SB_API_PROCS ) || (ApiIndex < 0 ) )
    {
        UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_SB_Use_Default_Api_Return_Code\n", ApiIndex);
        return -1;
    }

    /* If index is valid, assign return value and report action */
    Success = UTF_CFE_SB_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);

    return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_SB_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_SB_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_SB_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;

    /* Check for correct number of arguments */
    if (argc != 3)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_SB_Set_Api_Return_Code. Expected 2.\n", argc -1 );
        UTF_exit();
    }

    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);

    /* Call function to set API return code */
    UTF_CFE_SB_Set_Api_Return_Code(Index, Code);
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_SB_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_SB_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_SB_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

    /* Check for correct number of arguments */
    if (argc != 2)
    {
        UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_SB_Set_Api_Return_Code. Expected 1.\n", argc -1 );
        UTF_exit();
    }

    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);

    /* Call function to set API return code */
    UTF_CFE_SB_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
    return;
}

/******************************************************************************
** Name:    CFE_SB_CreatePipe
**
** Purpose: API to create a pipe for receiving messages
**
** Assumptions, External Events, and Notes:
**
**          Note: Zero is a valid PipeId
**
** Date Written:
**          04/25/2005
**
** Inputs:
**          PipeIdPtr - Ptr to users empty PipeId variable, to be filled by
**                      this function.
**          Depth     - The depth of the pipe, synonymous to the max number
**                      of messages the pipe can hold at any time.
**          PipeName  - The name of the pipe displayed in event messages
**
** Outputs:
**          PipeId    - The handle of the pipe to be used when receiving
**                      messages.
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, char *PipeName)
{

    static CFE_SB_PipeId_t PipeID = 1;

    if (UTF_SB_HookTable.CFE_SB_CreatePipe)
        return(UTF_SB_HookTable.CFE_SB_CreatePipe(PipeIdPtr, Depth, PipeName));

    /* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_CREATEPIPE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_CREATEPIPE_PROC];
    }

    *PipeIdPtr = PipeID++;

    return (CFE_SUCCESS);

}/* end CFE_SB_CreatePipe */

/******************************************************************************
** Name:    CFE_SB_DeletePipe
**
** Purpose: API to delete a pipe
**
** Assumptions, External Events, and Notes:
**
**          Note: Zero is a valid PipeId
**
** Date Written:
**          06/7/2006
**
** Inputs:
**          PipeId - The ID of the Pipe to delete.
**
** Outputs:
**			None.
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    if (UTF_SB_HookTable.CFE_SB_DeletePipe)
        return(UTF_SB_HookTable.CFE_SB_DeletePipe(PipeId));

    /* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_DELETEPIPE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_DELETEPIPE_PROC];
    }

    return (CFE_SUCCESS);
}

/******************************************************************************
** Name:    CFE_SB_SubscribeEx
**
** Purpose: API to globally subscribe to a message when QOS and MsgLim defaults
**          are insufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**          Note: MsgLim default: 4, see Input Arguments below for more detail
**          Note: Qos pertains to off-board routing only.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          Quality - Quality of Service (Qos) - priority and reliability
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SubscribeEx(CFE_SB_MsgId_t   MsgId,
                          CFE_SB_PipeId_t  PipeId,
                          CFE_SB_Qos_t     Quality,
                          uint16           MsgLim){

    if (UTF_SB_HookTable.CFE_SB_SubscribeEx)
        return(UTF_SB_HookTable.CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim));

	/* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_SUBSCRIBEEX_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_SUBSCRIBEEX_PROC];
    }

    return (CFE_SUCCESS);

}/* end CFE_SB_SubscribeEx */

/******************************************************************************
** Name:    CFE_SB_Subscribe
**
** Purpose: API to locally subscribe to a message when QOS and MsgLim defaults
**          are sufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**                MsgLim default: 4 - meaning at most 4 messages of given
**                                    MsgId allowed on the given PipeId.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t  MsgId,
                       CFE_SB_PipeId_t PipeId)
{

    if (UTF_SB_HookTable.CFE_SB_Subscribe)
        return(UTF_SB_HookTable.CFE_SB_Subscribe(MsgId, PipeId));

	/* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_SUBSCRIBE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_SUBSCRIBE_PROC];
    }

    return (CFE_SUCCESS);

}/* end CFE_SB_Subscribe */

/******************************************************************************
** Name:    CFE_SB_Unsubscribe
**
** Purpose: API used to unsubscribe to a message.
**
** Assumptions, External Events, and Notes:
**          Note:
**
** Routines Called:
**          CFE_SB_UnsubscribeFull
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{

    (void) MsgId;          /* refer to function argument to avoid compiler warning */
    (void) PipeId;         /* refer to function argument to avoid compiler warning */

    /* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_UNSUBSCRIBE_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_UNSUBSCRIBE_PROC];
    }

    return (CFE_SUCCESS);

}/* end CFE_SB_Unsubscribe */

/******************************************************************************
** Name:    CFE_SB_SendMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function increments and tracks the source sequence
**                counter for all messages.
**
** Routines Called:
**          UTF_put_packet
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SendMsg(CFE_SB_Msg_t    *MsgPtr)
{

    /* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_SENDMSG_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_SENDMSG_PROC];
    }

	UTF_put_packet(UTF_SOURCE_SEND, (CCSDS_PriHdr_t *)MsgPtr);

    return CFE_SUCCESS;

}/* end CFE_SB_SendMsg */

/******************************************************************************
** Name:    CFE_SB_PassMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function does NOT increment and track the source
**                sequence counter for telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_PassMsg(CFE_SB_Msg_t    *MsgPtr)
{

    /* just sent the packet */
    UTF_put_packet(UTF_SOURCE_SEND, (CCSDS_PriHdr_t *)MsgPtr);

    return CFE_SUCCESS;

}/* end CFE_SB_PassMsg */

/******************************************************************************
** Name:    CFE_SB_RcvMsg
**
** Purpose: Simulates the API used to receive a message from the software bus.
**
** Assumptions, External Events, and Notes:
**          None
**
** Routines Called:
**          UTF_get_buffer
**          UTF_get_data
**          UTF_put_packet
**
** Date Written:
**          07/25/2005
**
** Input Arguments:
**          PipeId
**          BufPtr
**          timeOut - CFE_SB_PEND, CFE_SB_POLL or millisecond timeout
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_RcvMsg(CFE_SB_MsgPtr_t  *BufPtr,
                     CFE_SB_PipeId_t  PipeId,
                     int32            TimeOut)
{
    boolean     status;
    static void *temp = NULL;

    /* Grab a buffer from the buffer_table the first time CFE_SB_RcvMsg */
	/* is called.  This buffer is reused for the remaining calls.       */
    if (temp == NULL)
	{
        temp = (void *)UTF_get_buffer();
		if (temp == NULL)
			UTF_error("UTF_get_buffer returned null\n");
    }

    /* Assign the caller's pointer to point to our allocated buffer */
    *BufPtr = temp;

    status = UTF_get_data(PipeId, *BufPtr, UTF_BUFFER_SIZE, UTF_AS_WORD);
    if (status == TRUE) /* Message was received */
    {
        /* UTF_put_packet, in utf_sb.c, displays packet */
        UTF_put_packet (UTF_SOURCE_RECEIVE, (CCSDS_PriHdr_t *)*BufPtr);

	    /* Handle Preset & Special Return Codes */
        if ((cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC] != UTF_CFE_USE_DEFAULT_RETURN_CODE) &&
        	(cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC] != UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE))
   	    {
            return cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC];
        }

        return(CFE_SUCCESS);
    }
    else /* no data available */
    {
    	/* Arrive here when empty pipe is indicated in test script*/

	    /* Handle Preset & Special Return Codes */
        if ((cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC] != UTF_CFE_USE_DEFAULT_RETURN_CODE) &&
        	(cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC] != UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE))
   	    {
            return cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC];
        }

		/*  Handle default behavior for empty pipe indication */
        switch (TimeOut)
        {
        case CFE_SB_PEND_FOREVER:
		    /* Handle Special Return Code */
		    /* This code must be explicitly set by the caller.
		     * This will exit the test process */
    	    if (cfe_sb_api_return_value[CFE_SB_RCVMSG_PROC] == UTF_CFE_SB_RCVMSG_EXIT_RETURN_CODE)
    	    {
    	    	UTF_exit();
    	    }
	        return CFE_SB_NO_MESSAGE;
            break;

        case CFE_SB_POLL:
            return CFE_SB_NO_MESSAGE;
            break;

        default:
            return CFE_SB_TIME_OUT;
            break;
        }
    }
} /* end CFE_SB_RcvMsg */

/******************************************************************************
** Name:    CFE_SB_GetLastSenderId
**
** Purpose: API used for receiving sender Information of the last message received on
**          the given pipe.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          06/13/2007
**
** Input Arguments:
**          Ptr -     caller declares a ptr of type CFE_SB_SenderId_t then gives the
**                    address of that pointer.
**          PipeId -  the Pipe Id that the message was received on.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
uint32 CFE_SB_GetLastSenderId(CFE_SB_SenderId_t **Ptr,CFE_SB_PipeId_t  PipeId)
{

    /* Set the Sender pointer return argument */
    *Ptr = &utf_lastSender;
    utf_lastSender.ProcessorId = 1;
    strncpy((char *)utf_lastSender.AppName,"CFE_SB",OS_MAX_API_NAME);
    
    /* Handle Preset Return Code */
    if (cfe_sb_api_return_value[CFE_SB_GETLASTSENDERID_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
    {
        return cfe_sb_api_return_value[CFE_SB_GETLASTSENDERID_PROC];
    }

    return(CFE_SUCCESS);
}

/******************************************************************************
** Name:    CFE_SB_ZeroCopyGetPtr
**    ** Purpose: API used for for getting a pointer to a buffer (for zero copy mode
**          only)
**        
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:**          04/25/2005
**  
** Input Arguments:
**          MsgSize
**
** Output Arguments:
**          BufferHandle
**  
** Return Values:
**          Pointer to an empty buffer in SB Memory space, or NULL if the buffer
**          could not be allocated.
**
******************************************************************************/
CFE_SB_Msg_t  *CFE_SB_ZeroCopyGetPtr(uint16 MsgSize, CFE_SB_ZeroCopyHandle_t *BufferHandle)
{

    (void) MsgSize;          /* refer to function argument to avoid compiler warning */

    return(NULL); /* CFE_SB_NOT_IMPLEMENTED */
}

/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleasePtr
**
** Purpose: API used for releasing a pointer to a buffer (for zero copy mode
**          only) This function is typically not needed for zero copy transfers.
**          This function is needed only when a ptr is received via
**          CFE_SB_ZeroCopyGetPtr, but never used in a send.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          Ptr2Release
**          BufferHandle
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleasePtr(CFE_SB_Msg_t  *Ptr2Release, CFE_SB_ZeroCopyHandle_t BufferHandle)
{

    (void) Ptr2Release;          /* refer to function argument to avoid compiler warning */

    return(CFE_SUCCESS);
}

/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleaseDesc
**
** Purpose: API used for releasing a zero copy descriptor (for zero copy mode
**          only).
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          Ptr2Release
**          BufferHandle
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Msg_t  *Ptr2Release,
                                 CFE_SB_ZeroCopyHandle_t  BufferHandle)
{

    return CFE_SUCCESS;
}

/******************************************************************************
** Name:    CFE_SB_ZeroCopySend
**
** Purpose: API for sending messages in zero copy mode (with telemetry source
**          sequence count incrementing)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopySend(CFE_SB_Msg_t *MsgPtr, CFE_SB_ZeroCopyHandle_t BufferHandle)
{

    /* just sent the packet like SB_SendMsg does */
    UTF_put_packet(UTF_SOURCE_SEND, (CCSDS_PriHdr_t *)MsgPtr);

    return(CFE_SUCCESS);
}

/******************************************************************************
** Name:    CFE_SB_ZeroCopyPass
**
** Purpose: API for sending messages in zero copy mode (telemetry source
**          sequence count is preserved)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyPass(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    /* just sent the packet like SB_SendMsg does */
    UTF_put_packet(UTF_SOURCE_SEND, (CCSDS_PriHdr_t *)MsgPtr);

    return(CFE_SUCCESS);

}/* end CFE_SB_ZeroCopyPass */

/**************End of File ***************************************************/

