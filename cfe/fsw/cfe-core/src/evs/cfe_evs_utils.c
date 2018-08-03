/*
**
**  File Name: cfe_evs_utils.c
**  $Id: cfe_evs_utils.c 1.12 2014/08/22 16:53:24GMT-05:00 lwalling Exp  $
**
**
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
**
**  Title: Event Services Utility functions
**
**  Purpose: This module defines the utility functions of the
**           Event Services Task and API
**
**  $Date: 2014/08/22 16:53:24GMT-05:00 $
**  $Revision: 1.12 $
**  $Log: cfe_evs_utils.c  $
**  Revision 1.12 2014/08/22 16:53:24GMT-05:00 lwalling 
**  Change signed loop counters to unsigned
**  Revision 1.11 2012/01/13 12:00:55EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.10 2011/05/23 15:47:47EDT lwalling 
**  Added missing newline terminator to text in call to CFE_ES_WriteToSysLog()
**  Revision 1.9 2011/04/05 16:33:46EDT lwalling 
**  Optimize EVS use of string functions, plus other performance improvements
**  Revision 1.8 2011/01/18 14:22:48EST lwalling 
**  Add missing message writes to system log
**  Revision 1.7 2010/10/25 15:01:40EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.6 2010/10/04 15:27:13EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.5 2009/07/28 17:16:57EDT aschoeni 
**  Added event message when filtering limit is reached.
**  Revision 1.4 2009/06/10 09:13:23EDT acudmore 
**  Converted OS_Mem* and OS_BSP* calls to CFE_PSP_*
**  Revision 1.3 2009/04/03 10:03:58EDT sjudy 
**  Put the '\n' chars after the '%s' in the functions OutputPortX like the way is was in version 1.1.  
**  It was decided that this was a messy thing to do and the CFE community was unhappy with it.
**  Revision 1.2 2009/03/26 16:12:37EDT sjudy 
**  In each of  EVS_OutputPortx I changed "%s\n" to "\n%s" to put the linefeed before the output
**  message.
**  Revision 1.1 2008/04/17 08:05:14EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
**  Revision 1.30 2007/09/13 09:46:11EDT rjmcgraw 
**  DCR4861 Added sem give prior to SendMsg call, then sem take after the call
**  Revision 1.29 2007/08/07 12:52:44EDT David Kobe (dlkobe) 
**  Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
**  Revision 1.28 2007/07/18 11:59:00EDT dlkobe 
**  Replaced memory address reference with CFE_EVS_GlobalData.EVS_TableHdl.
**  Revision 1.27 2007/05/07 13:44:45EDT njyanchik 
**  I'm removing things that shouldn't have been checked in
**  Revision 1.26 2007/05/07 10:42:05EDT njyanchik 
**  I added a sending of an Event message from EVS on the first time an EVS-unregistered
**  application tries to send an event message. A system log message is also written
**  Revision 1.25 2007/03/14 10:55:15EST njyanchik 
**  This is a check in of EVS's cleanup function for apps
**  Revision 1.24 2007/02/06 14:23:21EST njyanchik 
**  This change package fixes the issue where is a message was sent from an app
**  not registered with EVS, then the UnregisteredApp counter never incremented
**  Revision 1.23 2006/06/09 15:28:20GMT-05:00 rjmcgraw 
**  Swapped parameters for memory pool API's
**  Revision 1.22 2006/06/08 18:14:55GMT njyanchik 
**  I added the appropriate legal headers to all of the evs files
**  Revision 1.21 2006/04/25 16:14:36EDT kkaudra 
**  Added unregistered app evt msg 
**  Revision 1.20 2005/11/29 11:18:25EST kkaudra
**  Filters are now added at the next available slot instead of at the end of the buffer
**
*/

/* Include Files */
#include "cfe_evs.h"         /* EVS library function definitions */
#include "cfe_evs_log.h"     /* EVS local event log definitions */
#include "cfe_evs_task.h"    /* EVS internal definitions */
#include "cfe_evs_utils.h"   /* EVS utility function definitions */

#include <stdio.h>
#include <string.h>

#include "cfe_error.h"       /* cFE error code definitions */
#include "common_types.h"    /* Basic data types */
#include "osapi.h"           /* OS API interface definitions */
#include "cfe_psp.h"          /* cFE PSP glue functions */
#include "cfe_sb.h"          /* Software Bus library function definitions */
#include "cfe_es.h"

/* Local Function Prototypes */
void EVS_SendViaPorts (CFE_EVS_Packet_t *EVS_PktPtr);
void EVS_OutputPort1 (char *Message);
void EVS_OutputPort2 (char *Message);
void EVS_OutputPort3 (char *Message);
void EVS_OutputPort4 (char *Message);

/* Function Definitions */


/*
**             Function Prologue
**
** Function Name:      EVS_GetAppID
**
** Purpose:  This routine gets and validates the caller's AppID
**
** Assumptions and Notes:
**
*/
int32 EVS_GetAppID (uint32 *AppIdPtr)
{
   int32 Status = CFE_SUCCESS;

   /* Get the caller's AppID */
   Status = CFE_ES_GetAppID(AppIdPtr);

   if (Status == CFE_SUCCESS)
   {
      /* Verify that AppID is in range */
      if (*AppIdPtr >= CFE_ES_MAX_APPLICATIONS)
      {
         Status = CFE_EVS_APP_ILLEGAL_APP_ID;
      }
   }

   return(Status);

} /* End EVS_GetAppID */


/*
**             Function Prologue
**
** Function Name:      EVS_GetApplicationInfo
**
** Purpose:  This routine returns the application ID and
**           status specifying the validity of the ID
**
** Assumptions and Notes:
**
*/
int32 EVS_GetApplicationInfo (uint32 *pAppID, const char *pAppName)
{
   int32 Status = CFE_SUCCESS;

   if ((pAppName == (char *) NULL) || (pAppID == (uint32 *) NULL))
   {
      Status = CFE_ES_ERR_BUFFER;
   }
   else
   {
      Status = CFE_ES_GetAppIDByName(pAppID, pAppName);

      if (Status == CFE_SUCCESS)
      {
         if (*pAppID >= CFE_ES_MAX_APPLICATIONS)
         {
            Status = CFE_EVS_APP_ILLEGAL_APP_ID;
         }
         else if (CFE_EVS_GlobalData.AppData[*pAppID].RegisterFlag == FALSE)
         {
            Status = CFE_EVS_APP_NOT_REGISTERED;
         }
      }
   }

   return Status;

} /* End EVS_GetApplicationInfo */


/*
**             Function Prologue
**
** Function Name:      EVS_NotRegistered
**
** Purpose:  This routine sends one "not registered" event per application
**
** Assumptions and Notes:
**
*/
int32 EVS_NotRegistered (uint32 AppID)
{
   char AppName[OS_MAX_API_NAME];

   /* Send only one "not registered" event per application */
   if (CFE_EVS_GlobalData.AppData[AppID].EventCount == 0) 
   { 
      /* Increment count of "not registered" applications */
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.UnregisteredAppCounter++;

      /* Indicate that "not registered" event has been sent for this app */
      CFE_EVS_GlobalData.AppData[AppID].EventCount++;

      /* Get the name of the "not registered" app */
      CFE_ES_GetAppName(AppName, AppID, OS_MAX_API_NAME);

      /* Send the "not registered" event */
      EVS_SendEvent(CFE_EVS_ERR_UNREGISTERED_EVS_APP, CFE_EVS_ERROR,
                   "App %s not registered with Event Services. Unable to send event.", AppName);

      /* Write the "not registered" info to the system log */
      CFE_ES_WriteToSysLog("App %s not registered with Event Services. Unable to send event.\n", AppName);
   }

   return(CFE_EVS_APP_NOT_REGISTERED);

} /* End EVS_NotRegistered */


/*
**             Function Prologue
**
** Function Name:      EVS_IsFiltered
**
** Purpose:  This routine returns true if the given event identifier and event type
**           is filtered for the given application identifier.  Otherwise a value of
**           false is returned.
**
** Assumptions and Notes:
**
*/
boolean EVS_IsFiltered (uint32 AppID, uint16 EventID, uint16 EventType)
{
   EVS_BinFilter_t *FilterPtr;
   EVS_AppData_t   *AppDataPtr;
   boolean          Filtered = FALSE;
   char             AppName[OS_MAX_API_NAME];


   /* Caller has verified that AppID is good and has registered with EVS */
   AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];

   if (AppDataPtr->ActiveFlag == FALSE)
   {
      /* All events are disabled for this application */
      Filtered = TRUE;
   }
   else switch (EventType)
   {
      case CFE_EVS_DEBUG:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_DEBUG_BIT) == 0)
         {
            /* Debug events are disabled for this application */
            Filtered = TRUE;
         }
         break;

      case CFE_EVS_INFORMATION:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_INFORMATION_BIT) == 0)
         {
            /* Informational events are disabled for this application */
            Filtered = TRUE;
         }
         break;

      case CFE_EVS_ERROR:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_ERROR_BIT) == 0)
         {
            /* Error events are disabled for this application */
            Filtered = TRUE;
         }
         break;

      case CFE_EVS_CRITICAL:

         if ((AppDataPtr->EventTypesActiveFlag & CFE_EVS_CRITICAL_BIT) == 0)
         {
            /* Critical events are disabled for this application */
            Filtered = TRUE;
         }
         break;

      default:

         /* Invalid Event Type */
         Filtered = TRUE;
         break;
   }


   /* Is this type of event enabled for this application? */
   if (Filtered == FALSE) 
   {
      FilterPtr = EVS_FindEventID(EventID, AppDataPtr->BinFilters);


      /* Does this event ID have an event filter table entry? */
      if (FilterPtr != NULL && FilterPtr->RateLimit)
      {
         uint32 currentTicks;
         uint32 ticksPerMessage = FilterPtr->RateLimit;
         OS_time_t t;

         // Get the current OS tick
         OS_GetLocalTime(&t);
         currentTicks = (t.seconds * 1000) + (t.microsecs / 1000);

         if (currentTicks < (FilterPtr->lastReceived + ticksPerMessage)) 
         {
            // if Burst limit exceeded, ignore the message.
            // One must be subtracted from the burst count to represent the
            // first message that was sent through successfully without
            // triggering the rate limit.
            if ((FilterPtr->burstCount - 1) == 0) 
            {
                FilterPtr->throttleCount++;
                Filtered = TRUE;
                CFE_ES_GetAppName(AppName, AppID, OS_MAX_API_NAME);
            }

            //Within the burst limit.
            else 
            {
                FilterPtr->burstCount--;
            }

        //Within the rate limit.
        } 
        else 
        {
            // Restore the burst limit (if needed).
            if (FilterPtr->burstCount < FilterPtr->BurstLimit) {
                FilterPtr->burstCount += ((currentTicks - FilterPtr->lastReceived) / ticksPerMessage);
                if (FilterPtr->burstCount > FilterPtr->BurstLimit) {
                    FilterPtr->burstCount = FilterPtr->BurstLimit;
                }
            }
        }

        FilterPtr->lastReceived = currentTicks;

      }
   }

   return(Filtered);

} /* End EVS_IsFiltered */


/*
**             Function Prologue
**
** Function Name:      EVS_FindEventID
**
** Purpose:  This routine searches and returns an index to the given Event ID with the
**           given application filter array.
**
** Assumptions and Notes:
**
*/
EVS_BinFilter_t *EVS_FindEventID (int16 EventID, EVS_BinFilter_t *FilterArray)
{
   uint32 i;

   for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS; i++)
   {
      if (FilterArray[i].EventID == EventID)
      {
         return(&FilterArray[i]);
      }
   }

   return((EVS_BinFilter_t *) NULL);

} /* End EVS_FindEventID */


/*
**             Function Prologue
**
** Function Name:      EVS_EnableTypes
**
** Purpose:  This routine enables event types selected in BitMask
**
** Assumptions and Notes:
**
*/
void EVS_EnableTypes (uint8 BitMask, uint32 AppID)
{
   uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

   /* Enable selected event type bits from bitmask */
   CFE_EVS_GlobalData.AppData[AppID].EventTypesActiveFlag |= (BitMask & EventTypeBits);

} /* End EVS_EnableTypes */


/*
**             Function Prologue
**
** Function Name:      EVS_DisableTypes
**
** Purpose:  This routine disables event types selected in BitMask
**
** Assumptions and Notes:
**
*/
void EVS_DisableTypes (uint8 BitMask, uint32 AppID)
{
   uint8 EventTypeBits = (CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT);

   /* Disable selected event type bits from bitmask */
   CFE_EVS_GlobalData.AppData[AppID].EventTypesActiveFlag &= ~(BitMask & EventTypeBits);

} /* End EVS_DisableTypes */


/*
**             Function Prologue
**
** Function Name:      EVS_SendPacket
**
** Purpose:  This routine sends an EVS event message out the software bus and all
**           enabled output ports if the calling application has been determined to
**           be registered and the event message is unfiltered
**
** Assumptions and Notes:
*/
void EVS_SendPacket (uint32 AppID, CFE_TIME_SysTime_t Time, CFE_EVS_Packet_t *EVS_PktPtr)
{

   /* Obtain task and system information */
   CFE_ES_GetAppName((char *)EVS_PktPtr->Payload.PacketID.AppName, AppID,
           sizeof(EVS_PktPtr->Payload.PacketID.AppName));
   EVS_PktPtr->Payload.PacketID.SpacecraftID = CFE_PSP_GetSpacecraftId();
   EVS_PktPtr->Payload.PacketID.ProcessorID  = CFE_PSP_GetProcessorId();

   /* Set the packet timestamp */
   CFE_SB_SetMsgTime((CFE_SB_Msg_t *) EVS_PktPtr, Time);

   /* Write event to the event log */
   EVS_AddLog(EVS_PktPtr);

/* (LSW) Is the intent to write the event text to the log but not the SB msg ??? */

   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageFormatMode == CFE_EVS_SHORT_FORMAT)
   {
      /* Send an empty message if short format is enabled */
      EVS_PktPtr->Payload.Message[0] = '\0';

/* (LSW) This is pointless -- why bother to send a buffer with an empty string ??? */

   }

   /* Send event via SoftwareBus */
   CFE_SB_SendMsg((CFE_SB_Msg_t *) EVS_PktPtr);

   /* Send event via selected ports */
   EVS_SendViaPorts(EVS_PktPtr);

   /* Increment message send counters (prevent rollover) */
   if (CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter < CFE_EVS_MAX_EVENT_SEND_COUNT)
   {
      CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageSendCounter++;
   }

   if (CFE_EVS_GlobalData.AppData[AppID].EventCount < CFE_EVS_MAX_EVENT_SEND_COUNT)
   {
      CFE_EVS_GlobalData.AppData[AppID].EventCount++;
   }

   return;

} /* End EVS_SendPacket */


/*
**             Function Prologue
**
** Function Name:      EVS_SendViaPorts
**
** Purpose:  This routine sends a string event message out all enabled
**           output ports
**
** Assumptions and Notes:
*/
void EVS_SendViaPorts (CFE_EVS_Packet_t *EVS_PktPtr)
{
   char PortMessage[CFE_EVS_MAX_PORT_MSG_LENGTH];

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT1_BIT) >> 0) == TRUE)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port1 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #1 */
      EVS_OutputPort1(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT2_BIT) >> 1) == TRUE)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port2 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #2 */
      EVS_OutputPort2(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT3_BIT) >> 2) == TRUE)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port3 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #3 */
      EVS_OutputPort3(PortMessage);
   }

   if (((CFE_EVS_GlobalData.EVS_TlmPkt.Payload.OutputPort & CFE_EVS_PORT4_BIT) >> 3) == TRUE)
   {
      /* Copy event message to string format */
      snprintf(PortMessage, CFE_EVS_MAX_PORT_MSG_LENGTH, "EVS Port4 %u/%u/%s %u: %s", (unsigned int) EVS_PktPtr->Payload.PacketID.SpacecraftID,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.ProcessorID,
                                                        EVS_PktPtr->Payload.PacketID.AppName,
                                                        (unsigned int) EVS_PktPtr->Payload.PacketID.EventID,
                                                        EVS_PktPtr->Payload.Message);
      /* Send string event out port #4 */
      EVS_OutputPort4(PortMessage);
   }

} /* End SendViaPorts */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort1
**
** Purpose:  This routine sends the input message string out
**           EVS port 1
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort1 (char *Message)
{
    OS_printf("%s\n", Message);

} /* End ES_OutputPort1 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort2
**
** Purpose:  This routine sends the input message string out
**           EVS port 2
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort2 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort2 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort3
**
** Purpose:  This routine sends the input message string out
**           EVS port 3
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort3 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort3 */


/*
**             Function Prologue
**
** Function Name:      EVS_OutputPort4
**
** Purpose:  This routine sends the input message string out
**           EVS port 4
**
** Assumptions and Notes:
**
*/
void EVS_OutputPort4 (char *Message)
{
   OS_printf("%s\n", Message);

} /* End ES_OutputPort4 */


/*
**             Function Prologue
**
** Function Name:      EVS_SendEvent
**
** Purpose:  This routine allows EVS to send events without having to verify
**           that the caller has a valid AppID and has registered with EVS.
**           This routine also does not need to acquire the mutex semaphore,
**           which can be time consuming on some platforms.
**
** Assumptions and Notes:
*/
int32 EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   int32              ExpandedLength;
   CFE_EVS_Packet_t   EVS_Packet;
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;

   /*
    * Must check that EVS_AppID is valid, which can happen if this is called
    * by some other thread before CFE_EVS_TaskInit() runs
    */
   /* Unlikely, but possible that an EVS event filter was added by command */
   if (CFE_EVS_GlobalData.EVS_AppID < CFE_ES_MAX_APPLICATIONS &&
           EVS_IsFiltered(CFE_EVS_GlobalData.EVS_AppID, EventID, EventType) == FALSE)
   {
      /* Initialize EVS event packet */
      CFE_SB_InitMsg(&EVS_Packet, CFE_EVS_EVENT_MSG_MID, sizeof(CFE_EVS_Packet_t), TRUE);
      EVS_Packet.Payload.PacketID.EventID   = EventID;
      EVS_Packet.Payload.PacketID.EventType = EventType;

      /* vsnprintf() returns the total expanded length of the formatted string */
      /* vsnprintf() copies and zero terminates portion that fits in the buffer */
      va_start(Ptr, Spec);
      ExpandedLength = vsnprintf((char *)EVS_Packet.Payload.Message, sizeof(EVS_Packet.Payload.Message), Spec, Ptr);
      va_end(Ptr);

      /* Were any characters truncated in the buffer? */
      if (ExpandedLength >= sizeof(EVS_Packet.Payload.Message))
      {
         /* Mark character before zero terminator to indicate truncation */
         EVS_Packet.Payload.Message[sizeof(EVS_Packet.Payload.Message) - 2] = CFE_EVS_MSG_TRUNCATED;
         CFE_EVS_GlobalData.EVS_TlmPkt.Payload.MessageTruncCounter++;
      }

      /* Get current spacecraft time */
      Time = CFE_TIME_GetTime();

      /* Send the event packet */
      EVS_SendPacket(CFE_EVS_GlobalData.EVS_AppID, Time, &EVS_Packet);
   }

   return(CFE_SUCCESS);

} /* End EVS_SendEvent */


/* End cfe_evs_utils */
