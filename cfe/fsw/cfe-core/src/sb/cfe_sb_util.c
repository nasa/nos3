/******************************************************************************
** File: cfe_sb_util.c
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
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message header fields.
**
** Author:   R.McGraw/SSI
**
** $Log: cfe_sb_util.c  $
** Revision 1.8 2014/07/10 09:24:08GMT-05:00 rmcgraw 
** DCR9772:1 Changes from C. Monaco & W.M Reid from APL for endianess neutrality
** Revision 1.7 2014/06/06 15:18:37EDT rmcgraw
** DCR21559 - Changed CFE_SB_MsgHdrSize to use macro instead of writing msgid directly
** Revision 1.6 2012/09/27 17:16:04EDT aschoeni
** Fixed 32_32_M_20 get function to shift by 12 instead of masking
** Revision 1.5 2012/01/13 12:15:13EST acudmore
** Changed license text to reflect open source
** Revision 1.4 2011/02/03 15:25:37EST lwalling
** Modified Get/Set time functions to support CFE_SB_PACKET_TIME_FORMAT selection
** Revision 1.3 2010/10/25 13:54:10EDT aschoeni
** Removed unused value from SetUserDataLength
** Revision 1.2 2010/10/04 15:21:13EDT jmdagost
** Cleaned up copyright symbol.
** Revision 1.1 2008/04/17 08:05:32EDT ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/sb/project.pj
** Revision 1.10 2006/10/16 14:31:00EDT rjmcgraw
** Minor changes to comply with MISRA standard
** Revision 1.9 2006/06/12 19:26:06GMT rjmcgraw
** Added legal statement
** Revision 1.8 2006/06/05 15:43:50EDT rjmcgraw
** Comment changes in reference to DCR398
** Revision 1.7 2006/04/28 18:35:14GMT rjmcgraw
** Corrected problems with checksum utils when no sec hdr present
** Revision 1.6 2006/04/28 18:02:20GMT rjmcgraw
** Corrected problems with Set/GetCmdCode for cmd pkts wo sec hdr
** Revision 1.5 2006/04/27 18:45:10GMT rjmcgraw
** Corrected problems with Set/GetMsgTime when no sec hdr is present in tlm pkts
**
******************************************************************************/


/*
** Include Files
*/

#include "cfe_sb.h"
#include "ccsds.h"
#include "osapi.h"
#include "cfe_error.h"



/******************************************************************************
**  Function:  CFE_SB_InitMsg()
**
**  Purpose:
**    Initialize the header fields of a message
**
**  Arguments:
**    MsgPtr  - Pointer to the header of a message.
**    MsgId   - MsgId to use for the message.
**    Length  - Length of the message in bytes.
**    Clear   - Indicates whether to clear the entire message:
**                TRUE = fill sequence count and packet data with zeros
**                FALSE = leave sequence count and packet data unchanged
**  Return:
**    (none)
*/
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16         Length,
                    boolean        Clear )
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_InitPkt ((CCSDS_PriHdr_t *)MsgPtr,(uint16)MsgId,Length,Clear);

#endif
} /* end CFE_SB_InitMsg */



/******************************************************************************
**  Function:  CFE_SB_MsgHdrSize()
**
**  Purpose:
**    Get the size of a message header.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**     Size of Message Header.
*/
uint16 CFE_SB_MsgHdrSize(CFE_SB_MsgId_t MsgId)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    uint16 size;
    CCSDS_PriHdr_t  CCSDSPriHdr;

    CCSDS_WR_SID(CCSDSPriHdr,MsgId);

    /* if secondary hdr is not present... */
    if(CCSDS_RD_SHDR(CCSDSPriHdr) == 0){

        size = sizeof(CCSDS_PriHdr_t);

    }else if(CCSDS_RD_TYPE(CCSDSPriHdr) == CCSDS_CMD){

        size = CFE_SB_CMD_HDR_SIZE;

    }else{

        size = CFE_SB_TLM_HDR_SIZE;
    }

    return size;

#endif
}/* end CFE_SB_MsgHdrSize */


/******************************************************************************
**  Function:  CFE_SB_GetUserData()
**
**  Purpose:
**    Get a pointer to the user data portion of a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Pointer to the first byte after the headers
*/
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS
    uint8           *BytePtr;
    CFE_SB_MsgId_t  MsgId;
    uint16          HdrSize;

    BytePtr = (uint8 *)MsgPtr;
    MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
    HdrSize = CFE_SB_MsgHdrSize(MsgId);

    return (BytePtr + HdrSize);
#endif
}/* end CFE_SB_GetUserData */


/******************************************************************************
**  Function:  CFE_SB_GetMsgId()
**
**  Purpose:
**    Get the message ID of a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    The Message Id in the message.
*/
CFE_SB_MsgId_t CFE_SB_GetMsgId(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    return CCSDS_RD_SID(MsgPtr->Hdr);

#endif
}/* end CFE_SB_GetMsgId */


/******************************************************************************
**  Function:  CFE_SB_SetMsgId()
**
**  Purpose:
**    Set the message Id of a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**    MsgId  - Message Id to be written
**
**  Return:
**    (none)
*/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_WR_SID(MsgPtr->Hdr,MsgId);

#endif
}/* end CFE_SB_SetMsgId */


/******************************************************************************
**  Function:  CFE_SB_GetUserDataLength()
**
**  Purpose:
**    Get the length of the user data of a message (total size - hdrs).
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Size of the message minus the headers
*/
uint16 CFE_SB_GetUserDataLength(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS
    uint16 TotalMsgSize;
    uint16 HdrSize;

    CFE_SB_MsgId_t MsgId;
    MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);
    HdrSize = CFE_SB_MsgHdrSize(MsgId);

    return (TotalMsgSize - HdrSize);
#endif
}/* end CFE_SB_GetUserDataLength */


/******************************************************************************
**  Function:  CFE_SB_SetUserDataLength()
**
**  Purpose:
**    Set the length field in the hdr, given the user data length.
**
**  Arguments:
**    MsgPtr     - Pointer to a CFE_SB_Msg_t
**    DataLength - Length of the user data
**
**  Return:
**    (none)
*/
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr,uint16 DataLength)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    uint32 TotalMsgSize, HdrSize;

    CFE_SB_MsgId_t MsgId;
    MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
    HdrSize = CFE_SB_MsgHdrSize(MsgId);
    TotalMsgSize = HdrSize + DataLength;
    CCSDS_WR_LEN(MsgPtr->Hdr,TotalMsgSize);

#endif
}/* end CFE_SB_SetUserDataLength */


/******************************************************************************
**  Function:  CFE_SB_GetTotalMsgLength()
**
**  Purpose:
**    Get the total length of the message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Total Length of the message
*/
uint16 CFE_SB_GetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    return CCSDS_RD_LEN(MsgPtr->Hdr);

#endif
}/* end CFE_SB_GetTotalMsgLength */


/******************************************************************************
**  Function:  CFE_SB_SetTotalMsgLength()
**
**  Purpose:
**    Set the length field, given the total length of the message.
**
**  Arguments:
**    MsgPtr      - Pointer to a CFE_SB_Msg_t
**    TotalLength - Total Length of the message
**
**  Return:
**    (none)
*/
void CFE_SB_SetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr,uint16 TotalLength)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_WR_LEN(MsgPtr->Hdr,TotalLength);

#endif
}/* end CFE_SB_SetTotalMsgLength */


/******************************************************************************
**  Function:  CFE_SB_GetMsgTime()
**
**  Purpose:
**    Get the time field from a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Time field from message or
**    Time value of zero for msgs that do not have a Time field in header
*/
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_TIME_SysTime_t TimeFromMsg;
    uint32 LocalSecs32 = 0;
    uint32 LocalSubs32 = 0;

    #ifdef MESSAGE_FORMAT_IS_CCSDS

    #if (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_16_SUBS)
    uint16 LocalSubs16;
    #endif

    CFE_SB_TlmHdr_t *TlmHdrPtr;

    /* if msg type is a command or msg has no secondary hdr, time = 0 */
    if ((CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_CMD) && (CCSDS_RD_SHDR(MsgPtr->Hdr) != 0)) {

        /* copy time data to/from packets to eliminate alignment issues */
        TlmHdrPtr = (CFE_SB_TlmHdr_t *)MsgPtr;

        #if (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_16_SUBS)

        CFE_PSP_MemCpy(&LocalSecs32, &TlmHdrPtr->Sec.Time[0], 4);
        CFE_PSP_MemCpy(&LocalSubs16, &TlmHdrPtr->Sec.Time[4], 2);
        /* convert packet data into CFE_TIME_SysTime_t format */
        LocalSubs32 = ((uint32) LocalSubs16) << 16;

        #elif (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_32_SUBS)

        CFE_PSP_MemCpy(&LocalSecs32, &TlmHdrPtr->Sec.Time[0], 4);
        CFE_PSP_MemCpy(&LocalSubs32, &TlmHdrPtr->Sec.Time[4], 4);
        /* no conversion necessary -- packet format = CFE_TIME_SysTime_t format */

        #elif (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_32_M_20)

        CFE_PSP_MemCpy(&LocalSecs32, &TlmHdrPtr->Sec.Time[0], 4);
        CFE_PSP_MemCpy(&LocalSubs32, &TlmHdrPtr->Sec.Time[4], 4);
        /* convert packet data into CFE_TIME_SysTime_t format */
        LocalSubs32 = CFE_TIME_Micro2SubSecs((LocalSubs32 >> 12));

        #endif
    }

    #endif

    /* return the packet time converted to CFE_TIME_SysTime_t format */
    TimeFromMsg.Seconds    = LocalSecs32;
    TimeFromMsg.Subseconds = LocalSubs32;

    return TimeFromMsg;

}/* end CFE_SB_GetMsgTime */


/******************************************************************************
**  Function:  CFE_SB_SetMsgTime()
**
**  Purpose:
**    Set the time field from a message.
**
**  Arguments:
**    MsgPtr  - Pointer to a CFE_SB_Msg_t
**    NewTime - Time to write in message
**
**  Return:
**    CFE_SUCCESS for no errors
**    CFE_SB_WRONG_MSG_TYPE if msg does not have a header field for time.
*/
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    int32 Result = CFE_SB_WRONG_MSG_TYPE;

    #ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_TlmHdr_t *TlmHdrPtr;

    /* declare format specific vars */
    #if (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_16_SUBS)
    uint16 LocalSubs16;
    #elif (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_32_M_20)
    uint32 LocalSubs32;
    #endif

    /* cannot set time if msg type is a command or msg has no secondary hdr */
    if ((CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_CMD) && (CCSDS_RD_SHDR(MsgPtr->Hdr) != 0)) {

        /* copy time data to/from packets to eliminate alignment issues */
        TlmHdrPtr = (CFE_SB_TlmHdr_t *) MsgPtr;

        #if (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_16_SUBS)

        /* convert time from CFE_TIME_SysTime_t format to packet format */
        LocalSubs16 = (uint16) (NewTime.Subseconds >> 16);
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[4], &LocalSubs16, 2);
        Result = CFE_SUCCESS;

        #elif (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_32_SUBS)

        /* no conversion necessary -- packet format = CFE_TIME_SysTime_t format */
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[4], &NewTime.Subseconds, 4);
        Result = CFE_SUCCESS;

        #elif (CFE_SB_PACKET_TIME_FORMAT == CFE_SB_TIME_32_32_M_20)

        /* convert time from CFE_TIME_SysTime_t format to packet format */
        LocalSubs32 = CFE_TIME_Sub2MicroSecs(NewTime.Subseconds) << 12;
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        CFE_PSP_MemCpy(&TlmHdrPtr->Sec.Time[4], &LocalSubs32, 4);
        Result = CFE_SUCCESS;

        #endif
    }

    #endif

    return Result;

}/* end CFE_SB_SetMsgTime */


/******************************************************************************
**  Function:  CFE_SB_TimeStampMsg()
**
**  Purpose:
**    Set the time field to the current time.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    (none)
*/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_SetMsgTime(MsgPtr,CFE_TIME_GetTime());

}/* end CFE_SB_TimeStampMsg */


/******************************************************************************
**  Function:  CFE_SB_GetCmdCode()
**
**  Purpose:
**    Get the opcode field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    CmdCode from the message (CCSDS Function Code)
*/
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr, return 0 */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_FC(CmdHdrPtr->Sec);

#endif
}/* end CFE_SB_GetCmdCode */


/******************************************************************************
**  Function:  CFE_SB_SetCmdCode()
**
**  Purpose:
**    Set the opcode field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**    CmdCode - Command code for the message (CCSDS Function Code)
**
**  Return:
**    (none)
*/
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                      uint16 CmdCode)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return CFE_SB_WRONG_MSG_TYPE;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    CCSDS_WR_FC(CmdHdrPtr->Sec,CmdCode);

    return CFE_SUCCESS;

#endif

}/* end CFE_SB_SetCmdCode */


/******************************************************************************
**  Function:  CFE_SB_GetChecksum()
**
**  Purpose:
**    Get the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**
*/
uint16 CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_CHECKSUM(CmdHdrPtr->Sec);

#endif
}/* end CFE_SB_GetChecksum */


/******************************************************************************
**  Function:  CFE_SB_GenerateChecksum()
**
**  Purpose:
**    Calculate and Set the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Note: If any header fields are changed after this call, the checksum will
**        no longer be valid.
**        Also, the packet length field dictates the number of iterations
**        used in the checksum algorithm and therefore must be properly set
**        before calling this function.
**
**  Return:
**    (none)
*/
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_CmdPkt_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return;
    }/* end if */

    CmdPktPtr = (CCSDS_CmdPkt_t *)MsgPtr;

    CCSDS_LoadCheckSum(CmdPktPtr);

#endif
}/* end CFE_SB_GenerateChecksum */


/******************************************************************************
**  Function:  CFE_SB_ValidateChecksum()
**
**  Purpose:
**    Validate the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    TRUE if checksum of packet is valid; FALSE if not.
*/
boolean CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_CmdPkt_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return FALSE;
    }/* end if */

    CmdPktPtr = (CCSDS_CmdPkt_t *)MsgPtr;

    return CCSDS_ValidCheckSum (CmdPktPtr);

#endif
}/* end CFE_SB_ValidateChecksum */


/******************************************************************************
**  Function:  CFE_SB_MessageStringGet()
**
**  See function prototype for full description
**
**  UNIT TESTING NOTE:
**    During unit testing, the "stub" version of this function must
**    also copy the string or all functions that depend on this will break.
**
**    Therefore this function is essentially duplicated in the UT code
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 Result;

    /*
     * Error in caller if DestMaxSize == 0.
     * Cannot terminate the string, since there is no place for the NUL
     * In this case, do nothing
     */
    if (DestMaxSize == 0)
    {
        Result = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        Result = 0;

        /*
         * Check if should use the default, which is if
         * the source string has zero length (first char is NUL).
         */
        if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
        {
            SourceStringPtr = DefaultString;
            SourceMaxSize = DestMaxSize;
        }

        /* Reserve 1 character for the required NUL */
        --DestMaxSize;

        while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
        {
            *DestStringPtr = *SourceStringPtr;
            ++DestStringPtr;
            ++SourceStringPtr;
            --SourceMaxSize;
            --DestMaxSize;

            ++Result;
        }

        /* Put the NUL in the last character */
        *DestStringPtr = 0;
    }

    return Result;
}


/******************************************************************************
**  Function:  CFE_SB_MessageStringSet()
**
**  See function prototype for full description
**
**  UNIT TESTING NOTE:
**    During unit testing, the "stub" version of this function must
**    also copy the string or all functions that depend on this will break.
**
**    Therefore this function is essentially duplicated in the UT code
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 Result;

    Result = 0;

    while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
    {
        *DestStringPtr = *SourceStringPtr;
        ++DestStringPtr;
        ++SourceStringPtr;
        ++Result;
        --DestMaxSize;
        --SourceMaxSize;
    }

    /*
     * Pad the remaining space with NUL chars,
     * but this should NOT be included in the final size
     */
    while (DestMaxSize > 0)
    {
        /* Put the NUL in the last character */
        *DestStringPtr = 0;
        ++DestStringPtr;
        --DestMaxSize;
    }

    return Result;
}
