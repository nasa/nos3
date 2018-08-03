 /*************************************************************************
 ** File:
 **   $Id: sc_utils.c 1.9 2015/03/02 12:59:04EST sstrege Exp  $
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
 **   This file contains the utilty functions for Stored Command
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_utils.c  $
 **   Revision 1.9 2015/03/02 12:59:04EST sstrege 
 **   Added copyright information
 **   Revision 1.8 2011/05/25 16:43:06EDT lwalling 
 **   Remove spurious non-printable character from function SC_ComputeAbsTime()
 **   Revision 1.7 2011/05/19 15:27:54EDT lwalling 
 **   Replace absolute time assignment with call to CFE_PSP_MemCpy()
 **   Revision 1.6 2011/05/16 17:20:24EDT lwalling 
 **   Add support for all endian types when extracting ATS time tags
 **   Revision 1.5 2011/04/25 17:02:26EDT lwalling 
 **   Re-wrote function SC_GetAtsEntryTime() without use of pointers that break strict-aliasing rules
 **   Revision 1.4 2010/09/28 10:32:30EDT lwalling 
 **   Update list of included header files
 **   Revision 1.3 2010/03/26 18:03:49EDT lwalling 
 **   Remove pad from ATS and RTS structures, change 32 bit ATS time to two 16 bit values
 **   Revision 1.2 2009/01/05 08:26:59EST nyanchik 
 **   Check in after code review changes
 *************************************************************************/


/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sc_utils.h"
#include "sc_events.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the Current time from CFE TIME                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SC_GetCurrentTime(void)
{
    CFE_TIME_SysTime_t TempTime;
    
    /* Get the system time of the correct time */
    
    if (SC_TIME_TO_USE == SC_USE_UTC)
    {
        TempTime = CFE_TIME_GetUTC();
    }
    else
    {
        if (SC_TIME_TO_USE == SC_USE_TAI)
        {
            TempTime = CFE_TIME_GetTAI();
        }
        else
        {
            /* Gets the cFE configured time */
            TempTime = CFE_TIME_GetTime();
        }            
    }

    /* We don't care about subseconds */
    SC_AppData.CurrentTime = TempTime.Seconds;
    
} /* end of SC_GetCurrentTime */


SC_AbsTimeTag_t SC_GetAtsEntryTime(SC_AtsEntryHeader_t *Entry)
{
    /*
    ** ATS Entry Header looks like this...
    **
    **    uint16 CmdNumber;
    **
    **    uint16 TimeTag1;
    **    uint16 TimeTag2;
    **
    **    uint8  CmdHeader[CFE_SB_CMD_HDR_SIZE];
    **
    ** The command packet data is variable length,
    **    only the command packet header is shown here.
    */
    SC_AbsTimeTag_t AbsTimeTag = 0;

    /* Store as bytes to avoid boundary, endian and strict-aliasing issues */
    CFE_PSP_MemCpy(&AbsTimeTag, &Entry->TimeTag1, sizeof(SC_AbsTimeTag_t));

    return(AbsTimeTag);

} /* End of SC_GetAtsEntryTime() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Compute Absolute time from relative time                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
SC_AbsTimeTag_t SC_ComputeAbsTime(uint16 RelTime)
{
    CFE_TIME_SysTime_t  AbsoluteTimeWSubs;
    CFE_TIME_SysTime_t  RelTimeWSubs;
    CFE_TIME_SysTime_t  ResultTimeWSubs;       
    /*
     ** get the current time
     */
    AbsoluteTimeWSubs.Seconds    = SC_AppData.CurrentTime;
    AbsoluteTimeWSubs.Subseconds = 0;
    
    RelTimeWSubs.Seconds    = RelTime;
    RelTimeWSubs.Subseconds = 0;    
    /*
     ** add the relative time the current time
     */
    ResultTimeWSubs = CFE_TIME_Add ( AbsoluteTimeWSubs, RelTimeWSubs);
    
    /* We don't need subseconds */
    return (ResultTimeWSubs.Seconds);
    
} /* end of SC_ComputeAbsTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Compare absolute times                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SC_CompareAbsTime(SC_AbsTimeTag_t AbsTime1,
                          SC_AbsTimeTag_t AbsTime2)
{    
    boolean Status;
    CFE_TIME_SysTime_t Time1WSubs;
    CFE_TIME_SysTime_t Time2WSubs; 
    CFE_TIME_Compare_t Result;   
    
    Time1WSubs.Seconds = AbsTime1;
    Time1WSubs.Subseconds = 0;

    Time2WSubs.Seconds = AbsTime2;
    Time2WSubs.Subseconds = 0;    
    
    Result = CFE_TIME_Compare( Time1WSubs, Time2WSubs);
    
   if ( Result == CFE_TIME_A_GT_B)
   {
        Status = TRUE;
   }
   else
   {
        Status = FALSE;
   }

    return Status;
    
}/* end of SC_CompareAbsTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SC Verify the length of the command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SC_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                           uint16          ExpectedLength)
{
    CFE_SB_MsgId_t MessageID;
    uint16  CommandCode;
    boolean Result = TRUE;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(msg);
    
    /* Verify the command packet length */
    if (ExpectedLength !=  ActualLength)
    {
        CommandCode = CFE_SB_GetCmdCode(msg);
        MessageID =  CFE_SB_GetMsgId(msg);
        
        CFE_EVS_SendEvent(SC_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);
        Result = FALSE;
        SC_AppData.CmdErrCtr++;
    }    
    return(Result);
} /* End of SC_VerifyCmdLength */

/************************/
/*  End of File Comment */
/************************/
