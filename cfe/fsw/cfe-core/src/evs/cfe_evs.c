/*
**
**  File Name: cfe_evs.c
**  $Id: cfe_evs.c 1.8 2014/05/30 13:26:02GMT-05:00 lwalling Exp  $
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
**  Title: Event Services API library
**
**  Purpose: This module defines the library functions of the
**           Event Services API
**
**  $Date: 2014/05/30 13:26:02GMT-05:00 $
**  $Revision: 1.8 $
**  $Log: cfe_evs.c  $
**  Revision 1.8 2014/05/30 13:26:02GMT-05:00 lwalling 
**  Added CFE_EVS_EVT_NOT_REGISTERED error condition to CFE_EVS_ResetFilter()
**  Revision 1.7 2012/09/28 19:14:22EDT aschoeni 
**  Removed bad check on EventID limit when resetting filter
**  Revision 1.6 2012/01/13 12:00:56EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.5 2011/04/05 16:33:57EDT lwalling 
**  Optimize EVS use of string functions, plus other performance improvements
**  Revision 1.4 2010/10/26 17:01:42EDT jmdagost 
**  Made CFE_EVS_ResetFilter() argument a signed int and added test for positive.
**  Revision 1.3 2010/10/25 15:01:44EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.2 2010/10/04 15:27:15EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.1 2008/04/17 08:05:12EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/evs/project.pj
**  Revision 1.35 2007/08/07 12:52:41EDT David Kobe (dlkobe) 
**  Modified CFE_ES_GetPoolBuf API's first parameter to be of type uint32**
**  Revision 1.34 2007/07/18 11:59:00EDT dlkobe 
**  Replaced memory address reference with CFE_EVS_GlobalData.EVS_TableHdl.
**  Revision 1.33 2007/03/21 14:35:21EST njyanchik 
**  I didn't previously fix this DCR for these two other Send Event APIs.
**  Revision 1.32 2007/02/06 14:42:37EST njyanchik 
**  this CP checks in the changes to fix the broken message truncation counter
**  Revision 1.31 2006/07/31 12:38:49GMT-05:00 njyanchik 
**  LDRA complained about passing an uninitialized variable into a function (even
**  when the function itself initializes the variable).
**  
**  In a structure definition, there was a uint16 AppID variable. It should be uint32.
**  Revision 1.30 2006/06/09 16:27:25EDT rjmcgraw 
**  Swapped parameters for memory pool API's
**  Revision 1.29 2006/06/09 19:28:15GMT kkaudra 
**  Changed size of BigBuf, removed static declaration from BigBuf, replaced vsprintf with vsnprintf
**  Revision 1.28 2006/06/08 14:14:54EDT njyanchik 
**  I added the appropriate legal headers to all of the evs files
**
*/

/* Include Files */
#include "cfe_evs.h"         /* EVS library function definitions */
#include "cfe_evs_task.h"    /* EVS internal definitions */
#include "cfe_evs_utils.h"   /* EVS utility function definitions */
#include "common_types.h"    /* Basic data types */
#include "cfe_es.h"          /* Executive Service definitions */
#include "cfe_error.h"       /* cFE error code definitions */

#include <stdarg.h>
#include <string.h>

/* External Data */

/* Local Function Prototypes */


/* Function Definitions */

/*
**             Function Prologue
**
** Function Name:      CFE_EVS_Register
**
** Purpose:  This routine registers an application for receiving event services.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_Register (void *Filters, uint16 NumEventFilters, uint16 FilterScheme)
{
   uint16 FilterLimit;
   uint16 i;
   int32  Status = CFE_SUCCESS;
   uint32 AppID = CFE_EVS_UNDEF_APPID;
   CFE_EVS_BinFilter_t   *AppFilters;
   EVS_AppData_t         *AppDataPtr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      /* Cleanup if already registered */
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == TRUE)
      {
         CFE_PSP_MemSet(&CFE_EVS_GlobalData.AppData[AppID], 0, sizeof(EVS_AppData_t));
      }

      /* Verify filter arguments */
      if (FilterScheme != CFE_EVS_BINARY_FILTER)
      {
         Status = CFE_EVS_UNKNOWN_FILTER;
      }
      else if ((NumEventFilters != 0) && (Filters == NULL))
      {
         Status = CFE_ES_ERR_BUFFER;
      }
      else
      {
         /* Initialize application event data */
         AppDataPtr = &CFE_EVS_GlobalData.AppData[AppID];
         AppDataPtr->RegisterFlag = TRUE;
         AppDataPtr->ActiveFlag   = TRUE;
         AppDataPtr->EventCount   = 0;
         AppDataPtr->EventTypesActiveFlag = CFE_EVS_DEFAULT_TYPE_FLAG;

         /* Set limit for number of provided filters */
         if (NumEventFilters < CFE_EVS_MAX_EVENT_FILTERS)
         {
            FilterLimit = NumEventFilters;
         }
         else
         {
            FilterLimit = CFE_EVS_MAX_EVENT_FILTERS;
         }

         if (Filters != NULL)
         {
            AppFilters = (CFE_EVS_BinFilter_t *) Filters;

            /* Copy provided filters */
            for (i = 0; i < FilterLimit; i++)
            {
               AppDataPtr->BinFilters[i].EventID = AppFilters[i].EventID;
               AppDataPtr->BinFilters[i].RateLimit = AppFilters[i].RateLimit;
               AppDataPtr->BinFilters[i].BurstLimit = AppFilters[i].BurstLimit;
               AppDataPtr->BinFilters[i].burstCount = AppFilters[i].BurstLimit;

               // If there is a rate limit make sure we have a burst of at least
               // one or the algorithm doesn't work right
               if (AppDataPtr->BinFilters[i].RateLimit && AppDataPtr->BinFilters[i].BurstLimit == 0)
               {
                   AppDataPtr->BinFilters[i].BurstLimit = 1;
                   AppDataPtr->BinFilters[i].burstCount = 1;
               }
            }
         }

         /* Initialize remainder of filters as unused */
         for (i = FilterLimit; i < CFE_EVS_MAX_EVENT_FILTERS; i++)
         {
            AppDataPtr->BinFilters[i].EventID = CFE_EVS_FREE_SLOT;
            AppDataPtr->BinFilters[i].Mask    = 0;
            AppDataPtr->BinFilters[i].Count   = 0;
         }
      }
   }

   return(Status);

} /* End CFE_EVS_Register */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_Unregister
**
** Purpose:  This routine unregisters an application for receiving event services.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_Unregister(void)
{
   int32 Status = CFE_SUCCESS;
   uint32 AppID = CFE_EVS_UNDEF_APPID;

   /* Query and verify the caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      /* Cleanup if already registered */
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == TRUE)
      {
         CFE_PSP_MemSet(&CFE_EVS_GlobalData.AppData[AppID], 0, sizeof(EVS_AppData_t));
      }
   }

   return(Status);

} /* End CFE_EVS_Unregister */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SendEvent
**
** Purpose:  This routine sends an event message, with message ID containing the current
**           processor/box and task ID as well as the eventID.
**
** Assumptions and Notes:
*/
int32 CFE_EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   CFE_EVS_Packet_t   EVS_Packet;
   int32              ExpandedLength;
   int32              Status = CFE_SUCCESS;
   uint32             AppID = CFE_EVS_UNDEF_APPID;
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == FALSE)
      {
         /* Handler for events from apps not registered with EVS */
         Status = EVS_NotRegistered(AppID);
      }
      else if (EVS_IsFiltered(AppID, EventID, EventType) == FALSE)
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
         EVS_SendPacket(AppID, Time, &EVS_Packet);
      }
   }

   return(Status);

} /* End CFE_EVS_SendEvent */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SendEventWithAppID
**
** Purpose:  This routine sends an event message, with message ID containing the current
**           processor/box and task ID as well as the eventID. Also forces the user to
**           supply an AppID to be sent with the message.
**
** Assumptions and Notes:
*/
int32 CFE_EVS_SendEventWithAppID (uint16 EventID, uint16 EventType, uint32 AppID, const char *Spec, ... )
{
   CFE_EVS_Packet_t   EVS_Packet;
   int32              ExpandedLength;
   int32              Status = CFE_SUCCESS;
   CFE_TIME_SysTime_t Time;
   va_list            Ptr;

   if (AppID >= CFE_ES_MAX_APPLICATIONS)
   {
      Status = CFE_EVS_APP_ILLEGAL_APP_ID;
   }
   else if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == FALSE)
   {
      /* Handler for events from apps not registered with EVS */
      Status = EVS_NotRegistered(AppID);
   }
   else if (EVS_IsFiltered(AppID, EventID, EventType) == FALSE)
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
      EVS_SendPacket(AppID, Time, &EVS_Packet);
   }

   return Status;

} /* End CFE_EVS_SendEventWithAppID */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SendTimedEvent
**
** Purpose:  This routine sends a timed event message, with message ID containing the
**           current processor/box and task ID as well as the eventID.
**
** Assumptions and Notes:
*/
int32 CFE_EVS_SendTimedEvent (CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, const char *Spec, ... )
{
   CFE_EVS_Packet_t   EVS_Packet;
   int32              ExpandedLength;
   int32              Status = CFE_SUCCESS;
   uint32             AppID = CFE_EVS_UNDEF_APPID;
   va_list            Ptr;

   /* Query and verify the caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == FALSE)
      {
         /* Handler for events from apps not registered with EVS */
         Status = EVS_NotRegistered(AppID);
      }
      else if (EVS_IsFiltered(AppID, EventID, EventType) == FALSE)
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

         /* Send the event packet */
         EVS_SendPacket(AppID, Time, &EVS_Packet);
      }
   }

   return(Status);

} /* End CFE_EVS_SendTimedEvent */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetFilter
**
** Purpose:  This routine clears the filter counter for the calling application's given event_id.
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetFilter (int16 EventID)
{
   int32            Status = CFE_SUCCESS;
   EVS_BinFilter_t *FilterPtr = NULL;
   uint32           AppID = CFE_EVS_UNDEF_APPID;

   /* Query and verify the caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == FALSE)
      {
         Status = CFE_EVS_APP_NOT_REGISTERED;
      }
      else
      {
         FilterPtr = EVS_FindEventID(EventID, CFE_EVS_GlobalData.AppData[AppID].BinFilters);

         if (FilterPtr != NULL)
         {
            FilterPtr->Count = 0;
         }
         else
         {
            Status = CFE_EVS_EVT_NOT_REGISTERED;
         }
      }
   }

   return(Status);

} /* End CFE_EVS_ResetFilter */


/*
**             Function Prologue
**
** Function Name:      CFE_EVS_ResetAllFilters
**
** Purpose:  This routine clears all filter counters for the calling application
**
** Assumptions and Notes:
**
*/
int32 CFE_EVS_ResetAllFilters ( void )
{
   int32    Status = CFE_SUCCESS;
   uint32   AppID = CFE_EVS_UNDEF_APPID;
   uint32   i;

   /* Get and verify caller's AppID */
   Status = EVS_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
      if (CFE_EVS_GlobalData.AppData[AppID].RegisterFlag == FALSE)
      {
         Status = CFE_EVS_APP_NOT_REGISTERED;
      }
      else
      {
         for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS; i++)
         {
            CFE_EVS_GlobalData.AppData[AppID].BinFilters[i].Count = 0;
         }
      }
   }

   return(Status);

} /* End CFE_EVS_ResetAllFilters */


/* End CFE_EVS.C */
