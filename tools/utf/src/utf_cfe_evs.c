/*
**  File Name: utf_cfe_evs.c
**  $Id: utf_cfe_evs.c 1.8 2012/01/13 12:51:56GMT-05:00 acudmore Exp  $
**
**  Title: Event Services API library 
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
**  Purpose: This module defines the library functions of the
**           Event Services API
**
**  $Date: 2012/01/13 12:51:56GMT-05:00 $
**  $Revision: 1.8 $
**  $Log: utf_cfe_evs.c  $
**  Revision 1.8 2012/01/13 12:51:56GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.7 2010/10/26 17:01:07EDT jmdagost 
**  Made CFE_EVS_ResetFilter() argument a signed int.
**  Revision 1.6 2010/10/25 15:06:44EDT jmdagost 
**  Corrected bad apostrophe in prologue.
**  Revision 1.5 2010/10/04 14:57:10EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.4 2009/10/13 14:55:25EDT wmoleski 
**  Changes required to be compatible with cFE 6.0.0
**  Revision 1.3 2008/09/04 07:16:27EDT wfmoleski 
**  A misleading SB event was being sent from the CFE_EVS_SendEventWithAppId function causing
**  telemtry output to be printed to the STDOUT device. This code was removed.
**  Revision 1.2 2008/08/19 14:20:33EDT wfmoleski 
**  Added UTF_put_text commands ti the SendEventWithAppID function so that the events are actually
**  readable.
**  Revision 1.1 2008/04/17 08:09:37EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/src/project.pj
**  Revision 1.6 2007/10/16 09:46:34EDT wfmoleski 
**  Added the Event Type and ID to the print statement in the UTF version of CFE_EVS_SendEvent.
**  Revision 1.5 2007/05/25 11:06:34EDT wfmoleski 
**  Modified to mimic cFE 4.1.0 fsw changes. These were minor but necessary changes.
**  Revision 1.4 2006/11/09 08:05:30EST wfmoleski 
**  Modified the UTF version to fix the same problem
**  Revision 1.3 2006/06/22 13:53:33EDT wfmoleski 
**  Most of the changes to these files was to add the copyright comments. There were other changes needed
**  to bring the UTF up to cFE 3.3
**  Revision 1.2 2006/05/08 15:26:28EDT nsschweiss 
**  Updated out-of-date return codes.  
**  Removed '//' comment indicators.
**  Added some additional logic paths from cfe_evs.c.
**  Revision 1.1 2006/03/21 16:44:27EST sslegel 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/src/project.pj
**  Revision 1.2 2005/12/01 18:21:56EST NSSCHWEISS 
**  utf 2.3.0.
**  Revision 1.1 2005/09/26 11:30:12EDT lswalling 
**  Initial revision
**  Member added to project d:/mksdata/MKS-LRO-REPOSITORY/utf/src_utf/project.pj
**
**  2005/11/29 nschweiss Added CFE_EVS_SendEventWithAppID.
**  Added EVS_SendEventInternal.
**  2005/11/02 nschweiss Remove CFE_EVS_Initialization since it is not
**             in new cfe_evs.h.
**   Made necessary changes for capability to set return codes.
* ** 2005/09/15 nschweiss Remove '#define ix86' statement.
**  2005/08/10 nschweiss Made changes to make code consistent with revised cfe_evs.h.
**  2005/08/08 nschweiss Changes consistent with cFE 2.0.
**  2005/08/08 nschweiss Change UTF_error calls in SendEvent to UTF_put_text.
**  2005/07/18 nschweiss Modify SendEvent to call UTF_put_text.
**  2005/07/01 nschweiss Modifying for CFE UTF.
**
**  Revision 1.1 2005/06/09 10:57:48EDT rperera 
**  Initial revision
**  Member added to project d:/mksdata/MKS-CFE-REPOSITORY/cfe-core/evs/project.pj
**  Revision 1.19  2005/05/03 15:10:38  sstrege
**  Added code to disable interrupts when writing to the event log
**
**  Revision 1.18  2005/05/02 18:06:21  sstrege
**  Changed AppID to AppName in EVS Telemetry, Packet, and Application Data File structures
**
**  Revision 1.17  2005/04/29 15:49:49  sstrege
**  Removed EVS parameter for MAX Applications and added ES parameter for MAX Applications
**
**  Revision 1.16  2005/04/22 21:35:52  sstrege
**  Added printf statements to output events from all ports
**
**  Revision 1.15  2005/04/21 16:43:36  sstrege
**  Removed EVS: from events
**
**
**
*/

/* Include Files */
#include "cfe.h"
#include "cfe_evs.h"         /* EVS library function definitions */
#include "osconfig.h"         /* Includes OS_MAX_API_NAME */
#include "cfe_evs_task.h"    /* EVS internal definitions */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "common_types.h"    /* Basic data types */
#include "utf_osapi.h"           /* OS API interface definitions */
#include "cfe_sb.h"          /* Software Bus library function definitions */
#include "cfe_es.h"
#include "utf_custom.h"      /* Defines UTF_QUEUE_SIZE */
#include "utf_types.h"
#include "utf_cfe.h"


/* External Data */


/* Local Data */
uint32	UTF_AppID_Registered = FALSE;
uint32  UTF_Long_Events_Enabled = TRUE;

int32 cfe_evs_api_return_value[NUM_OF_CFE_EVS_API_PROCS]=
 {
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
  UTF_CFE_USE_DEFAULT_RETURN_CODE, UTF_CFE_USE_DEFAULT_RETURN_CODE,
 };




/* Local Function Prototypes */
int32 EVS_SendEventInternal(uint32 AppID, CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, char *EventString);

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
** Original procedure returns CFE_EVS_ERR_NONE or CFE_EVS_ERR.
*/
int32 CFE_EVS_Register (void *Filters, uint16 NumEventFilters, uint16 FilterScheme)
{
	int32                  Status = CFE_SUCCESS;
	
	/* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_REGISTER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_REGISTER_PROC];
   }
	

   UTF_AppID_Registered = TRUE;
   
   if(FilterScheme != CFE_EVS_BINARY_FILTER)
   {
         Status = CFE_EVS_UNKNOWN_FILTER;            		
   }
   
   return Status;

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
int32 CFE_EVS_Unregister ( void )
{
	
		/* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_UNREGISTER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_UNREGISTER_PROC];
   }
	

      /* Make sure application is registered */
	  if(UTF_AppID_Registered == TRUE)
	  {
         /* Set registered flag to false */
		 UTF_AppID_Registered = FALSE;

	  }
     return CFE_SUCCESS;


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
	int32               Status = CFE_SUCCESS;
   char                BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
   char                GTBigBuf[CFE_EVS_MAX_MESSAGE_LENGTH + 2];
   va_list             Ptr;
   CFE_EVS_Packet_t    EVS_Packet;
   uint32              AppID = 0xFFFFFFFF;

   /* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_SENDEVENT_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_SENDEVENT_PROC];
   }

   /* Get application identifier */
   Status = CFE_ES_GetAppID(&AppID);

   if (Status == CFE_SUCCESS)
   {
	  if(UTF_AppID_Registered == FALSE)
      {
		char warning_text[120];
		strcpy (warning_text,"UTF WARNING: CFE_EVS_SendEvent called by application which is not ");
	 	strcat(warning_text,"registered for event services.\n");
        /* if the application is not registered for event services do not perform event service */
		UTF_put_text(warning_text);
      }

	  /* Obtain task and system information */
      /*EVS_Packet.PacketID.SpacecraftID = OS_BSPGetSpacecraftId();*/
      CFE_ES_GetAppName(EVS_Packet.PacketID.AppName, AppID, OS_MAX_API_NAME);
      /* EVS_Packet.PacketID.ProcessorID  = OS_BSPGetProcessorId();*/
      EVS_Packet.PacketID.EventID      = EventID;
      EVS_Packet.PacketID.EventType    = EventType;

      /* Copy message to event packet if long format is enabled */
      if (UTF_Long_Events_Enabled == TRUE)
      {
         va_start(Ptr, Spec);
         vsnprintf(GTBigBuf, CFE_EVS_MAX_MESSAGE_LENGTH +2, Spec, Ptr);
         va_end(Ptr);

         if(strlen(GTBigBuf) > CFE_EVS_MAX_MESSAGE_LENGTH)
		 {
			UTF_put_text ("Max Event Message Size Exceeded. Permitted %d; Actual %d\n",
			CFE_EVS_MAX_MESSAGE_LENGTH, strlen(GTBigBuf));
		 }

		 strncpy(BigBuf,GTBigBuf,CFE_EVS_MAX_MESSAGE_LENGTH);
		 if (EventType == CFE_EVS_DEBUG)
			 UTF_put_text("DEBUG EVENT ID=%d %s\n",EventID, BigBuf);
		 else if (EventType == CFE_EVS_INFORMATION)
			 UTF_put_text("INFO EVENT ID=%d %s\n",EventID, BigBuf);
		 else if (EventType == CFE_EVS_ERROR)
			 UTF_put_text("ERROR EVENT ID=%d %s\n",EventID, BigBuf);
		 else if (EventType == CFE_EVS_CRITICAL)
			 UTF_put_text("CRITICAL EVENT ID=%d %s\n",EventID, BigBuf);
		 else
			 UTF_put_text("Invalid Event Type %d ID=%d %s\n",EventType,EventID, BigBuf);
      }
      else
      {
         /* Send an empty message if short format is enabled */
         strcpy(EVS_Packet.Message, "\0");
      }

      return CFE_SUCCESS;
   }
   
   return Status;

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
int32 CFE_EVS_SendEventWithAppID (uint16 EventID, uint16 EventType,uint32 AppID, const char *Spec, ... )
{
   int32               Status = CFE_SUCCESS;
   char                BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
   char                GTBigBuf[CFE_EVS_MAX_MESSAGE_LENGTH + 2];
   va_list             Ptr;

   /* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_SENDEVENTWITHAPPID_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_SENDEVENTWITHAPPID_PROC];
   }

   /* Assume the given AppID is legit */   
   if (Status == CFE_SUCCESS)
   {
      /* Copy message to event packet */
      va_start(Ptr, Spec);
      vsnprintf(GTBigBuf, CFE_EVS_MAX_MESSAGE_LENGTH +2, Spec, Ptr);
      va_end(Ptr);

      if (strlen(GTBigBuf) > CFE_EVS_MAX_MESSAGE_LENGTH)
	  {
		UTF_put_text ("Max Event Message Size Exceeded. Permitted %d; Actual %d\n",
		CFE_EVS_MAX_MESSAGE_LENGTH, strlen(GTBigBuf));
	  }

	  strncpy(BigBuf,GTBigBuf,CFE_EVS_MAX_MESSAGE_LENGTH);
	  if (EventType == CFE_EVS_DEBUG)
		UTF_put_text("DEBUG EVENT ID=%d %s\n",EventID, BigBuf);
	  else if (EventType == CFE_EVS_INFORMATION)
		UTF_put_text("INFO EVENT ID=%d %s\n",EventID, BigBuf);
	  else if (EventType == CFE_EVS_ERROR)
		UTF_put_text("ERROR EVENT ID=%d %s\n",EventID, BigBuf);
	  else if (EventType == CFE_EVS_CRITICAL)
		UTF_put_text("CRITICAL EVENT ID=%d %s\n",EventID, BigBuf);
	  else
		UTF_put_text("Invalid Event Type %d ID=%d %s\n",EventType,EventID, BigBuf);
   }

   return Status;

} /* End CFE_EVS_SendEventWithAppID */

/*
**             Function Prologue
**
** Function Name:      EVS_SendEventInternal
** from: cfe_evs_utils.c in cFE 3.1, originally named EVS_SendEvent
**
** Purpose:  This routine sends an EVS event message out the software bus and all
**           enabled output ports if the calling application has been determined to
**           be registered and the event message is unfiltered
**
** Assumptions and Notes:
*/
int32 EVS_SendEventInternal(uint32 AppID, CFE_TIME_SysTime_t Time, uint16 EventID, uint16 EventType, char *EventString)
{
   int32              Status = CFE_SUCCESS;
   CFE_EVS_Packet_t   EVS_Packet;
   boolean            IsFiltered = FALSE;

   if(Status == CFE_SUCCESS)
   {
      if (!(IsFiltered))
      {
         /* Initialize CCSDS event packet */
         CFE_SB_InitMsg(&(EVS_Packet), CFE_EVS_EVENT_MSG_MID, sizeof(CFE_EVS_Packet_t), FALSE);

         /* Set the packet timestamp */
         CFE_SB_SetMsgTime((CFE_SB_Msg_t *) &EVS_Packet, Time);

         /* Initialize event message string */
         EVS_Packet.Message[0] = '\0';

         /* Obtain task and system information */
     /*    EVS_Packet.PacketID.SpacecraftID = OS_BSPGetSpacecraftId(); */
         CFE_ES_GetAppName(EVS_Packet.PacketID.AppName, AppID, OS_MAX_API_NAME);
      /*   EVS_Packet.PacketID.ProcessorID  = OS_BSPGetProcessorId(); */
         EVS_Packet.PacketID.EventID      = EventID;
         EVS_Packet.PacketID.EventType    = EventType;

         /* Copy message string to event packet message buffer */
         strncpy(EVS_Packet.Message, EventString, CFE_EVS_MAX_MESSAGE_LENGTH);

         /* Ensure that the packet is always terminated by a null character */
         EVS_Packet.Message[CFE_EVS_MAX_MESSAGE_LENGTH-1] = '\0';

         /* send event out software bus */
         Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) &EVS_Packet);
      }
   }

   return Status;

} /* End EVS_SendEventInternal */



/*
**             Function Prologue
**
** Function Name:      CFE_EVS_SendTimedEvent
**
** Purpose:  This routine sends a timed event message, with message ID containing the 
**           current processor/box and task ID as well as the eventID.
**
** Assumptions and Notes:
**   Original routine returns CFE_EVS_ERR or CFE_EVS_ERR_NONE.
*/
int32 CFE_EVS_SendTimedEvent (CFE_TIME_SysTime_t Time, uint16 EventID, 
                              uint16 EventType, const char *Spec, ... )
{  
   char               BigBuf[CFE_EVS_MAX_MESSAGE_LENGTH];
   char               GTBigBuf[CFE_EVS_MAX_MESSAGE_LENGTH + 2];
   va_list            Ptr;
   CFE_EVS_Packet_t   EVS_Packet;

	/* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_SENDTIMEDEVENT_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_SENDTIMEDEVENT_PROC];
   }

   /* Initialize CCSDS event packet */
   CFE_SB_InitMsg(&(EVS_Packet), CFE_EVS_EVENT_MSG_MID, sizeof(CFE_EVS_Packet_t), FALSE);

   /* Set the packet timestamp */
   CFE_SB_SetMsgTime((CFE_SB_Msg_t *) &EVS_Packet, Time);   
  
   EVS_Packet.Message[0] = '\0';

   /* Copy message to event packet if long format is enabled */
   va_start(Ptr, Spec);
   vsnprintf(GTBigBuf, CFE_EVS_MAX_MESSAGE_LENGTH +2, Spec, Ptr);
   va_end(Ptr);

   if (strlen(GTBigBuf) > CFE_EVS_MAX_MESSAGE_LENGTH)
   {
	 UTF_put_text ("Max Event Message Size Exceeded. Permitted %d; Actual %d\n",
			CFE_EVS_MAX_MESSAGE_LENGTH, strlen(GTBigBuf));
   }

   strncpy(BigBuf,GTBigBuf,CFE_EVS_MAX_MESSAGE_LENGTH);
   UTF_put_text("TIMED EVENT: %s\n", BigBuf);

  return CFE_SUCCESS;

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
   /* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_RESETFILTER_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_RESETFILTER_PROC];
   }

   return CFE_SUCCESS;   

 
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
   int32                  Status = CFE_SUCCESS;
   
   	/* Handle Preset Return Code */
   if (cfe_evs_api_return_value[CFE_EVS_RESETALLFILTERS_PROC] !=  UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
      return cfe_evs_api_return_value[CFE_EVS_RESETALLFILTERS_PROC];
   }
   

   return Status;

} /* End CFE_EVS_ResetAllFilters */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_EVS_Set_Api_Return_Code() --                                     */
/*  Set a return code for an api to a specified value.                     */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 UTF_CFE_EVS_Set_Api_Return_Code(int32 ApiIndex, uint32 ReturnCode)
{
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_EVS_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_EVS_Set_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }
   /* If index is valid, assign return value and report action */
   cfe_evs_api_return_value[ApiIndex] = ReturnCode;

   if (ReturnCode == UTF_CFE_USE_DEFAULT_RETURN_CODE)
   {
   	  UTF_put_text("UTF Info: Return code for CFE EVS Api #%d returned to default\n",
   	               ApiIndex);
   }
   else
   {
   	  UTF_put_text("UTF Info: Return value for CFE EVS Api #%d set to %lx\n", 
   	                ApiIndex, ReturnCode);
   }
   return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_CFE_EVS_Use_Default_Api_Return_Code() --                             */
/* Use to return to default return code for an api.                        */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 UTF_CFE_EVS_Use_Default_Api_Return_Code(int32 ApiIndex)
{
	int32 Success;
	
   /* Check that ApiIndex is in the valid range */
   if ( (ApiIndex >= NUM_OF_CFE_EVS_API_PROCS ) || (ApiIndex < 0 ) )  
   {
      UTF_put_text("UTF Error: Invalid ApiIndex %d passed to UTF_CFE_EVS_Use_Default_Api_Return_Code\n",
      ApiIndex);
      return -1;
   }

   /* If index is valid, assign return value and report action */
   Success = UTF_CFE_EVS_Set_Api_Return_Code(ApiIndex, UTF_CFE_USE_DEFAULT_RETURN_CODE);
   
   return Success;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_EVS_Set_Api_Return_Code() --                                  */
/*  Script interface function for UTF_CFE_EVS_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_EVS_Set_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;
    int32 Code;
	/* Check for correct number of arguments */
	if (argc != 3)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_EVS_Set_Api_Return_Code. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
    Code  = UTF_arg2uint(argv[2]);
    /* Call function to set API return code */
	UTF_CFE_EVS_Set_Api_Return_Code(Index, Code);
	return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                         */
/* UTF_SCRIPT_EVS_Use_Default_Api_Return_Code() --                          */
/*  Script interface function for UTF_CFE_EVS_Set_Api_Return_Code.          */
/*                                                                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UTF_SCRIPT_EVS_Use_Default_Api_Return_Code(int argc,char *argv[])
{
    int32 Index;

	/* Check for correct number of arguments */
	if (argc != 2)
	{
	   UTF_error("UTF Error: Read %d args w/script cmd UTF_SCRIPT_EVS_Set_Api_Return_Code. Expected 1.\n",
	   argc -1 );
	   UTF_exit();
	}
 
    /* Extract argument values */
    Index = UTF_arg2dbl(argv[1]);
 
    /* Call function to set API return code */
	UTF_CFE_EVS_Set_Api_Return_Code(Index, UTF_CFE_USE_DEFAULT_RETURN_CODE);
	return;
}



/* End UTF_CFE_EVS.C */
