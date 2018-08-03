/**********************************************************
** File: sbn_utils.c
**
** Author: ejtimmon
** Date: 01 Sep 2015
**
** Purpose:
**  This file contains SBN functions that should be 
**  accessible by the SBN application as well as SBN
**  interface modules.
**
**********************************************************/

#include <string.h>
#include <strings.h>
#include "sbn_lib_utils.h"

/**
 * Returns true (1) if the message type is a protocol message.
 * Returns true (1) if the message type is an announce, 
 * announce ack, heartbeat, or heartbeat ack message.
 * Returns false (0) if the message is an command ack/nack, 
 * subscribe, unsubscripe, app, or unknown message.
 */
int32 SBN_LIB_MsgTypeIsProto(uint32 MsgType) {
    int32 Result = 0;

    switch (MsgType) {
        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:
            Result = 1;
            break;
        case SBN_COMMAND_ACK_MSG:
        case SBN_COMMAND_NACK_MSG:
        case SBN_APP_MSG:
        case SBN_SUBSCRIBE_MSG:
        case SBN_UN_SUBSCRIBE_MSG:
        default:
            Result = 0;
    }

    return Result;
}

/**
 * Returns false (0) if the message type is a protocol message.
 * Returns false (0) if the message type is an announce, 
 * announce ack, heartbeat, heartbeat ack, or unknown message.
 * Returns true (1) if the message is an command ack/nack, 
 * subscribe, unsubscripe, or app message.
 */
int32 SBN_LIB_MsgTypeIsData(uint32 MsgType) {
    int32 Result = 0;

    switch (MsgType) {
        case SBN_COMMAND_ACK_MSG:
        case SBN_COMMAND_NACK_MSG:
        case SBN_APP_MSG:
        case SBN_SUBSCRIBE_MSG:
        case SBN_UN_SUBSCRIBE_MSG:
            Result = 1;
            break;
        case SBN_ANNOUNCE_MSG:
        case SBN_ANNOUNCE_ACK_MSG:
        case SBN_HEARTBEAT_MSG:
        case SBN_HEARTBEAT_ACK_MSG:
        default:
            Result = 0;
    }

    return Result;
}

char *SBN_LIB_GetMsgName(uint32 MsgType) {
    static char SBN_MsgName[OS_MAX_API_NAME];

    bzero(SBN_MsgName, OS_MAX_API_NAME);

    switch (MsgType)
    {

        case SBN_ANNOUNCE_MSG:
            strcpy(SBN_MsgName, "Announce");
            break;

        case SBN_ANNOUNCE_ACK_MSG:
            strcpy(SBN_MsgName, "Announce Ack");
            break;

        case SBN_HEARTBEAT_MSG:
            strcpy(SBN_MsgName, "Heartbeat");
            break;

        case SBN_HEARTBEAT_ACK_MSG:
            strcpy(SBN_MsgName, "Heartbeat Ack");
            break;

        case SBN_SUBSCRIBE_MSG:
            strcpy(SBN_MsgName, "Subscribe");
            break;

        case SBN_UN_SUBSCRIBE_MSG:
            strcpy(SBN_MsgName, "UnSubscribe");
            break;
        case SBN_COMMAND_ACK_MSG:
            strcpy(SBN_MsgName, "Command Ack");
            break;
        case SBN_COMMAND_NACK_MSG:
            strcpy(SBN_MsgName, "Command Nack");
            break;
        case SBN_APP_MSG:
            strcpy(SBN_MsgName, "App Msg");
            break;

        default:
            strcpy(SBN_MsgName, "Unknown");
            break;
    }/* end switch */

    SBN_MsgName[OS_MAX_API_NAME - 1] = '\0';

    return SBN_MsgName;

}/* SBN_GetMsgName */

char *SBN_LIB_StateNum2Str(uint32 StateNum) {
    static char SBN_StateName[OS_MAX_API_NAME];

    switch (StateNum)
    {
        case 0:
            strcpy(SBN_StateName, "SBN_ANNOUNCING");
            break;

        case 1:
            strcpy(SBN_StateName, "SBN_HEARTBEATING");
            break;

        default:
            strcpy(SBN_StateName, "SBN_UNKNOWN");
            break;

            return SBN_StateName;

    }/* end switch */

    return SBN_StateName;

}/* end SBN_StateNum2Str */

int32 SBN_EndianMemCpy(void *dest, void *src, uint32 n)
{
#ifdef LITTLE_ENDIAN
    uint32 i = 0;
    for(i = 0; i < n; i++)
    {
        ((uint8 *)dest)[i] = ((uint8 *)src)[n - i - 1];
    }/* end for */
    return CFE_PSP_SUCCESS;
#else /* !LITTLE_ENDIAN */
    return CFE_PSP_MemCpy(dest, src, n);
#endif /* LITTLE_ENDIAN */
}/* end SBN_EndianMemCpy */
