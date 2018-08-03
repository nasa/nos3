/*
**
**  Filename: cfe_evs_msg.h
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASAs Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
*
**  $Id: cfe_evs_msg.h 1.6 2012/01/10 15:26:15GMT-05:00 jmdagost Exp  $
**
**  Title:    Event Services Message definition header file Header File
**
**  Purpose: 
**	           Unit specification for Event services command codes and data structures.
**
**  Design Notes: 
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  $Date: 2012/01/10 15:26:15GMT-05:00 $
**  $Revision: 1.6 $
**  $Log: cfe_evs_msg.h  $
**  Revision 1.6 2012/01/10 15:26:15GMT-05:00 jmdagost 
**  Changed command codes to start at zero for no-op (consistent with other apps).
**  Revision 1.5 2011/05/23 15:42:27EDT lwalling 
**  Change unused 32 bit MemPoolHandle to 8 bit LogEnabled and three 8 bit spares
**  Revision 1.4 2010/10/04 15:25:19EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.3 2009/07/31 19:52:03EDT aschoeni 
**  padded commands to be even number of bytes
**  Revision 1.2 2009/03/04 12:12:38EST rmcgraw 
**  DCR4715:1 Added bit information to event type cmds and port cmds
**  Revision 1.1 2008/04/17 08:05:21EDT ruperera 
**  Member moved from tlserver to tlserver3
**  Revision 1.10 2007/09/18 14:46:46EDT njyanchik 
**  Check in the telemetry comments
**  Revision 1.9 2007/09/18 14:02:18EDT njyanchik 
**  add command doxygen comments
**  Revision 1.8 2007/07/18 14:09:56EDT njyanchik 
**  moved 12 #defines to cfe_evs_msg.h  because they are needed for commands
**  Revision 1.7 2007/07/18 11:53:05EDT njyanchik 
**  The two structures specified in DCR 4407 were moved to cfe_evs_msg.h
**  Revision 1.6 2007/06/07 13:07:45EDT dlkobe 
**  Added Command and Telemetry doxygen comments to TIME, EVS and SB
**  Revision 1.5 2007/05/16 15:32:23EDT dlkobe 
**  Corrected circular data type dependency
**  Revision 1.4 2007/05/16 15:13:38EDT dlkobe 
**  Added MemPoolHandle members to HK packet
**  Revision 1.3 2007/04/27 12:31:05EDT njyanchik 
**  I changed EVS and TIME to not have a telemetry counter for SB pipe reads in  
**  the main loop. Instead, I have added a syslog message to state the problem
**  Revision 1.2 2007/04/11 10:59:55EDT njyanchik 
**  Remove the TaskDelay in the main loop for EVS. I also added a counter in 
**  telemtry that will notify the ground of an error in reading the command pipe.
**  Revision 1.1 2007/03/01 11:53:54EST njyanchik 
**  Initial revision
**  Member added to cfe project on tlserver
*/
#ifndef _cfe_evs_msg_
#define _cfe_evs_msg_

/********************************** Include Files  ************************************/
#include "common_types.h"    /* Basic data types */
#include "cfe_time.h"        /* Time library function definitions */
#include "osapi.h"
#include "cfe_sb.h"
#include "cfe_es.h"


/** \name Event Services Command Codes */
/** \{ */

/** \cfeevscmd Event Services No-Op
**
**  \par Description
**       This command performs no other function than to increment the 
**       command execution counter. The command may be used to verify 
**       general aliveness of the Event Services task.
**
**  \cfecmdmnemonic \EVS_NOOP
**
**  \par Command Structure
**       #CFE_TBL_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**         increment
**       - The #CFE_EVS_NOOP_EID informational event message will 
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Event 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS itself) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_EVS_NO_OPERATION_CC            0

/** \cfeevscmd Event Services Reset Counters
**
**  \par Description
**       This command resets the following counters within the Event 
**       Services housekeeping telemetry:
**       - Command Execution Counter (\EVS_CMDPC)
**       - Command Error Counter (\EVS_CMDEC)
**
**  \cfecmdmnemonic \EVS_RESETCTRS
**
**  \par Command Structure
**       #CFE_TBL_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**         increment
**       - The #CFE_EVS_RSTCNT_EID debug event message will be 
**         generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Event 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is 
**       possible for ground systems and on-board safing procedures 
**       to be designed such that they react to changes in the counter 
**       values that are reset by this command.
**
**  \sa  #CFE_EVS_RESET_APP_COUNTER_CC
*/
#define CFE_EVS_RESET_COUNTERS_CC          1

/** \cfeevscmd Enable Event Type
**
**  \par Description
**       This command enables the command specified Event Type allowing event
**       messages of this type to be sent through Event Service.  An Event Type
**       is defined to be a classification of an Event Message such as debug, 
**       informational, error and critical. This command is a global enable of a 
**       particular event type, it applies to all applications. 
**
**  \cfecmdmnemonic \EVS_ENAEVENTTYPE
**
**  \par Command Structure
**       #CFE_EVS_BitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be enabled (or unfiltered). 
**        A zero in a bit position means the filtering state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       
**        - \b \c \EVS_CMDPC - command execution counter will 
**         increment
**        - The generation of  #CFE_EVS_ENAEVTTYPE_EID debug message        
**
**  \par Error Conditions
**        This command may fail for the following reason(s):
**
**    	Invalid Event Type selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Enabling an event type is not particularly hazardous, as the result may 
**      be turning on necessary event messages and communication to the ground 
**      system.  However, inappropriately enabling an event type could result 
**      in flooding of the system.
**
**  \sa #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, 
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_EVENT_TYPE_CC       2

/** \cfeevscmd Disable Event Type
**
**  \par Description
**      This command disables the command specified Event Type preventing event
**      messages of this type to be sent through Event Service.   An Event Type
**      is defined to be a classification of an Event Message such as debug, 
**      informational, error and critical. This command is a global disable of a 
**      particular event type, it applies to all applications.
**
**  \cfecmdmnemonic \EVS_DISEVENTTYPE
**
**  \par Command Structure
**       #CFE_EVS_BitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be disabled (or filtered). 
**        A zero in a bit position means the filtering state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**      
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
         - The generation of #CFE_EVS_DISEVTTYPE_EID debug message 
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**
**       - Invalid Event Type selection 

**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Disabling an event type is not particularly hazardous, as the result 
**       may be shutting off unnecessary event messages and possible event 
**       flooding of the system.  However, inappropriately disabling an event 
**       type could result in a loss of critical information and missed 
**       behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, 
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_EVENT_TYPE_CC      3

/** \cfeevscmd Set Event Format Mode
**
**  \par Description
**  This command sets the event format mode to the command specified value.
**  The event format mode may be either short or long.  A short event format
**  detaches the Event Data from the event message and only includes the 
**  following information in the event packet: Processor ID, Application ID,
**  Event ID, and Event Type.  Refer to section 5.3.3.4 for a description of 
**  the Event Service event packet contents.  Event Data is defined to be data
**  describing an Event that is supplied to the cFE Event Service.  ASCII text
**  strings are used as the primary format for Event Data because heritage 
**  ground systems use string compares as the basis for their automated alert
**  systems.  Two systems, ANSR and SERS were looked at for interface 
**  definitions.  The short event format is used to accommodate experiences 
**  with limited telemetry bandwidth.  The long event format includes all event
**  information included within the short format along with the Event Data.
**
**  \cfecmdmnemonic \EVS_SETEVTFMT
**
**  \par Command Structure
**       #CFE_EVS_ModeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_SETEVTFMTMOD_EID debug message
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**  	Invalid SB message (command) length
**  	Invalid MODE selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**
**  \par Criticality
**      Setting the event format mode is not particularly hazardous, as the 
**      result may be saving necessary bandwidth.  However, inappropriately 
**      setting the event format mode could result in a loss of information
**      and missed behavior for the ground system
**  \sa 
*/
#define CFE_EVS_SET_EVENT_FORMAT_MODE_CC   4

/** \cfeevscmd Enable Application Event Type
**
**  \par Description
**      This command enables the command specified event type for the command
**      specified application, allowing the application to send event messages
**      of the command specified event type through Event Service.  An Event 
**      Type is defined to be a classification of an Event Message such as 
**      debug, informational, critical, and error.
**      Note: In order for this command to take effect, applications must be 
**      registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ENAAPPEVTTYPE
**
**  \par Command Structure
**       #CFE_EVS_AppNameBitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be enabled (or 
**          unfiltered) for the specified application. 
**        A zero in a bit position means the filtering state is unchanged for 
**          the specified application.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_ENAAPPEVTTYPE_EID debug event message
**          
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid Event Type Selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**
**  \par Criticality
**      Enabling an application¿s event type is not particularly hazardous, as 
**      the result may be turning on necessary event messages and 
**      communication to the ground system.  However, inappropriately enabling
**      an application's event type could result in flooding of the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, 
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_APP_EVENT_TYPE_CC   5

/** \cfeevscmd Disable Application Event Type
**
**  \par Description
**      This command disables the command specified event type for the command
**      specified application, preventing the application from sending event
**      messages of the command specified event type through Event Service.  
**      An Event Type is defined to be a classification of an Event Message such
**      as debug, informational, critical, and error.  Note: In order for this
**      command to take effect, applications must be registered for Event Service.      
**
**  \cfecmdmnemonic \EVS_DISAPPEVTTYPE
**
**  \par Command Structure
**       #CFE_EVS_AppNameBitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be disabled (or 
**          filtered) for the specified application. 
**        A zero in a bit position means the filtering state is unchanged for 
**          the specified application.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_DISAPPENTTYPE_EID debug event message      
**       - The clearing of the Active Flag in \link #CFE_EVS_AppDataFile_t::ActiveFlag The Active Flag in EVS App Data File \endlink
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid Event Type Selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Disabling an application's event type is not particularly hazardous, 
**      as the result may be shutting off unnecessary event messages and
**      possible event flooding of the system.  However, inappropriately 
**      disabling an application's event type could result in a loss of critical
**      information and missed behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, 
**       #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_APP_EVENT_TYPE_CC  6

/** \cfeevscmd Enable Event Services for an Application
**
**  \par Description
**      This command enables the command specified application to send events 
**      through the Event Service.  Note: In order for this command to take 
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ENAAPPEVGEN
**
**  \par Command Structure
**       #CFE_EVS_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_ENAAPPEVT_EID debug event message 
**       - The setting of the Active Flag in \link #CFE_EVS_AppDataFile_t::ActiveFlag The Active Flag in EVS App Data File \endlink
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Enabling an application¿s events is not particularly hazardous, 
**      as the result may be turning on necessary event messages and 
**      communication to the ground system.  However, inappropriately enabling 
**      an application's events could result in flooding of the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, 
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_APP_EVENTS_CC       7

/** \cfeevscmd Disable Event Services for an Application
**
**  \par Description
**      This command disables the command specified application from sending 
**      events through Event Service.  Note: In order for this command to take
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_DISAPPEVGEN
**
**  \par Command Structure
**       #CFE_EVS_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_DISAPPEVT_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Disabling an application's events is not particularly hazardous, as the 
**      result may be shutting off unnecessary event messages and possible event
**      flooding of the system.  However, inappropriately disabling an 
**      application's events could result in a loss of critical information and
**      missed behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, 
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_APP_EVENTS_CC      8

/** \cfeevscmd Reset Application Event Counters
**
**  \par Description
**      This command sets the command specified application's event counter to zero.
**      Note: In order for this command to take effect, applications must be registered
**      for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTAPPCTRS
**
**  \par Command Structure
**       #CFE_EVS_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_RSTEVTCNT_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      This command is not inherently dangerous.  However, it is possible for 
**      ground systems and on-board safing procedures to be designed such that
**      they react to changes in the counter value that is reset by this command.
**
**  \sa #CFE_EVS_RESET_COUNTERS_CC
*/
#define CFE_EVS_RESET_APP_COUNTER_CC       9

/** \cfeevscmd Set Application Event Filter
**
**  \par Description
**      This command sets the command specified application's event filter mask
**      to the command specified value for the command specified event.  Note: 
**      In order for this command to take effect, applications must be 
**      registered for Event Service.
**
**  \cfecmdmnemonic \EVS_SETBINFLTRMASK
**
**  \par Command Structure
**       #CFE_EVS_AppNameEventIDMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_SETFILTERMSK_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Setting an application¿s event filter mask is not particularly hazardous,
**      as the result may be shutting off unnecessary event messages and possible
**      event flooding of the system.  However, inappropriately setting an 
**      application's event filter mask could result in a loss of critical 
**      information and missed behavior for the ground system or flooding of the 
**      ground system.
**
**  \sa #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_SET_FILTER_CC              10

/** \cfeevscmd Enable Event Services Output Ports
**
**  \par Description
**      This command enables the command specified port to output event messages
**
**  \cfecmdmnemonic \EVS_ENAPORT
**
**  \par Command Structure
**       #CFE_EVS_BitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Port 1
**            Bit 1 - Port 2
**            Bit 2 - Port 3
**            Bit 3 - Port 4
**        A one in a bit position means the port will be enabled. 
**        A zero in a bit position means the port state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_ENAPORT_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Invalid PORT selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**          None.
**
**  \sa #CFE_EVS_DISABLE_PORTS_CC
*/
#define CFE_EVS_ENABLE_PORTS_CC            11

/** \cfeevscmd Disable Event Services Output Ports
**
**  \par Description
**      This command disables the specified port from outputting event messages.
**
**  \cfecmdmnemonic \EVS_DISPORT
**
**  \par Command Structure
**       #CFE_EVS_BitMaskCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Port 1
**            Bit 1 - Port 2
**            Bit 2 - Port 3
**            Bit 3 - Port 4
**        A one in a bit position means the port will be disabled. 
**        A zero in a bit position means the port state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_DISPORT_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Invalid PORT selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_ENABLE_PORTS_CC
*/
#define CFE_EVS_DISABLE_PORTS_CC           12

/** \cfeevscmd Reset an Event Filter for an Application
**
**  \par Description
**      This command resets the command specified application's event filter for
**      the command specified event ID.  Note: In order for this command to take 
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTBINFLTRCTR
**
**  \par Command Structure
**       #CFE_EVS_AppNameEventIDCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_RSTFILTER_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_RESET_FILTER_CC            13 

/** \cfeevscmd Reset All Event Filters for an Application
**
**  \par Description
**      This command resets all of the command specified applications event 
**      filters.  Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTALLFLTRS
**
**  \par Command Structure
**       #CFE_EVS_AppNameCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_RSTALLFILTER_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_RESET_ALL_FILTERS_CC       14

/** \cfeevscmd Add Application Event Filter
**
**  \par Description
**      This command adds the given filter for the given application identifier and event identifier.
**      Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ADDEVTFLTR
**
**  \par Command Structure
**       #CFE_EVS_AppNameEventIDMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_ADDFILTER_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_ADD_EVENT_FILTER_CC        15

/** \cfeevscmd Delete Application Event Filter
**
**  \par Description
**      This command removes the given filter for the given application identifier and event identifier.
**      Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_DELEVTFLTR
**
**  \par Command Structure
**       #CFE_EVS_AppNameEventIDMaskCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_DELFILTER_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Application selected is not registered to receive Event Service
**      - Application ID is out of range
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC
*/
#define CFE_EVS_DELETE_EVENT_FILTER_CC     16

/** \cfeevscmd Write Event Services Application Information to File
**
**  \par Description
**       This command writes all application data to a file for all applications that
**           have registered with the EVS.  The application data includes the Application ID,
**           Active Flag, Event Count, Event Types Active Flag, and Filter Data.
**
**  \cfecmdmnemonic \EVS_WRITEAPPDATA2FILE
**
**  \par Command Structure
**       #CFE_EVS_AppDataCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_WRDAT_EID debug event message 
**       - The generation of the file written to
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Writing a file is not particularly hazardous, but if proper file management is not
**       taken, then the file system can fill up if this command is used repeatedly.
**
**  \sa #CFE_EVS_FILE_WRITE_LOG_DATA_CC, #CFE_EVS_SET_LOG_MODE_CC
*/
#define CFE_EVS_FILE_WRITE_APP_DATA_CC     17

/** \cfeevscmd Write Event Log to File
**
**  \par Description
**      This command requests the Event Service to generate a file containing 
**      the contents of the local event log.  
**
**  \cfecmdmnemonic \EVS_WRITELOG2FILE
**
**  \par Command Structure
**       #CFE_EVS_LogFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_WRLOG_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Writing a file is not particularly hazardous, but if proper file management is not
**       taken, then the file system can fill up if this command is used repeatedly.
**
**  \sa #CFE_EVS_FILE_WRITE_APP_DATA_CC, #CFE_EVS_SET_LOG_MODE_CC, #CFE_EVS_CLEAR_LOG_CC
*/
#define CFE_EVS_FILE_WRITE_LOG_DATA_CC     18

/** \cfeevscmd Set Logging Mode
**
**  \par Description
**       This command sets the logging mode to the command specified value.
**
**  \cfecmdmnemonic \EVS_SETLOGMODE
**
**  \par Command Structure
**       #CFE_EVS_ModeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**       - The generation of #CFE_EVS_LOGMODE_EID debug event message 
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**      - Invalid MODE selected
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Setting the event logging mode is not particularly hazardous, as the
**       result may be saving valuable event data.  However, inappropriately
**       setting the log mode could result in a loss of critical information.  
**      Note: the event log is a back-up log to the on-board recorder.
**
**  \sa #CFE_EVS_FILE_WRITE_LOG_DATA_CC, #CFE_EVS_CLEAR_LOG_CC
*/
#define CFE_EVS_SET_LOG_MODE_CC            19

/** \cfeevscmd Clear Event Log
**
**  \par Description
**       This command clears the contents of the local event log.
**
**  \cfecmdmnemonic \EVS_CLRLOG
**
**  \par Command Structure
**       #CFE_TBL_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will 
**       increment
**
**  \par Error Conditions
**      This command may fail for the following reason(s):
**      - Invalid SB message (command) length
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Clearing the local event log is not particularly hazardous, as the 
**      result may be making available space to record valuable event data. 
**      However, inappropriately clearing the local event log could result
**      in a loss of critical information.  Note: the event log is a back-up
**      log to the on-board recorder.
**
**  \sa #CFE_EVS_FILE_WRITE_LOG_DATA_CC, #CFE_EVS_SET_LOG_MODE_CC
*/
#define CFE_EVS_CLEAR_LOG_CC               20
/** \} */


/* Event Message Format Modes */
#define CFE_EVS_SHORT_FORMAT            0
#define CFE_EVS_LONG_FORMAT             1

/* Event Type bit masks */
#define CFE_EVS_DEBUG_BIT               0x0001
#define CFE_EVS_INFORMATION_BIT         0x0002
#define CFE_EVS_ERROR_BIT               0x0004
#define CFE_EVS_CRITICAL_BIT            0x0008

/* Output Port bit masks */
#define CFE_EVS_PORT1_BIT               0x0001
#define CFE_EVS_PORT2_BIT               0x0002
#define CFE_EVS_PORT3_BIT               0x0004
#define CFE_EVS_PORT4_BIT               0x0008

/* EVS Log Modes */
#define CFE_EVS_LOG_OVERWRITE       0
#define CFE_EVS_LOG_DISCARD         1  

/******************  Structure Definitions *********************/

/**
** \brief Write Event Log to File Command
**
** For command details, see #CFE_EVS_FILE_WRITE_LOG_DATA_CC
**
**/
typedef struct {
    char LogFilename[OS_MAX_PATH_LEN];      /**< \brief Filename where log data is to be written */
} CFE_EVS_LogFileCmd_Payload_t;

typedef struct {
   uint8                        CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_LogFileCmd_Payload_t Payload;
} CFE_EVS_LogFileCmd_t;

/**
** \brief Write Event Services Application Information to File Command
**
** For command details, see #CFE_EVS_FILE_WRITE_APP_DATA_CC
**
**/
typedef struct {
   char AppDataFilename[OS_MAX_PATH_LEN];  /**< \brief Filename where applicaton data is to be written */
} CFE_EVS_AppDataCmd_Payload_t;

typedef struct {
   uint8                        CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_AppDataCmd_Payload_t Payload;
} CFE_EVS_AppDataCmd_t;

/**
** \brief Set Event Format Mode or Set Log Mode Commands
**
** For command details, see #CFE_EVS_SET_EVENT_FORMAT_MODE_CC and/or #CFE_EVS_SET_LOG_MODE_CC
**
**/
typedef struct {
   uint8                     Mode;                              /**< \brief Mode to use in the command*/
   uint8                     Spare;                             /**< \brief Pad to even byte*/
} CFE_EVS_ModeCmd_Payload_t;

typedef struct {
   uint8                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_ModeCmd_Payload_t Payload;
} CFE_EVS_ModeCmd_t;

/**
** \brief Enable/Disable Events or Ports Commands
**
** For command details, see #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC,
**                          #CFE_EVS_ENABLE_PORTS_CC and/or #CFE_EVS_DISABLE_PORTS_CC
**
**/
typedef struct {
   uint8                     BitMask;           /**< \brief BitMask to use in the command */
   uint8                     Spare;                             /**< \brief Pad to even byte*/
} CFE_EVS_BitMaskCmd_Payload_t;

typedef struct {
   uint8                        CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_BitMaskCmd_Payload_t Payload;
} CFE_EVS_BitMaskCmd_t;

/**
** \brief Enable/Disable Application Events or Reset One or All Filter Counters
**
** For command details, see #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC,
**                          #CFE_EVS_RESET_APP_COUNTER_CC and/or #CFE_EVS_RESET_ALL_FILTERS_CC
**
**/
typedef struct {
   char                     AppName[OS_MAX_API_NAME];          /**< \brief Application name to use in the command*/
} CFE_EVS_AppNameCmd_Payload_t;

typedef struct {
   uint8                        CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_AppNameCmd_Payload_t Payload;
} CFE_EVS_AppNameCmd_t;

/**
** \brief Reset an Event Filter for an Application
**
** For command details, see #CFE_EVS_RESET_FILTER_CC
**
**/
typedef struct {
   char                      AppName[OS_MAX_API_NAME];          /**< \brief Application name to use in the command*/
   uint16                    EventID;                           /**< \brief Event ID  to use in the command*/
} CFE_EVS_AppNameEventIDCmd_Payload_t;

typedef struct {
   uint8                                CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_AppNameEventIDCmd_Payload_t  Payload;
} CFE_EVS_AppNameEventIDCmd_t;

/**
** \brief Enable/Disable an Event Type for an Application
**
** For command details, see #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC and/or #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC
**
**/
typedef struct {
   char                      AppName[OS_MAX_API_NAME];          /**< \brief Application name to use in the command*/
   uint8                     BitMask;                           /**< \brief BitMask to use in the command*/
   uint8                     Spare;                             /**< \brief Pad to even byte*/
} CFE_EVS_AppNameBitMaskCmd_Payload_t;

typedef struct {
   uint8                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_AppNameBitMaskCmd_Payload_t Payload;
} CFE_EVS_AppNameBitMaskCmd_t;

/**
** \brief Set, Add or Delete an Event Filter for an Application
**
** For command details, see #CFE_EVS_SET_FILTER_CC, #CFE_EVS_ADD_EVENT_FILTER_CC 
**                      and/or #CFE_EVS_DELETE_EVENT_FILTER_CC
**
**/
typedef struct {
   char                      AppName[OS_MAX_API_NAME];          /**< \brief Application name to use in the command*/
   uint16                    EventID;                           /**< \brief Event ID  to use in the command*/
   uint16                    Mask;                              /**< \brief Mask to use in the command */
} CFE_EVS_AppNameEventIDMaskCmd_Payload_t;

typedef struct {
   uint8                                    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_EVS_AppNameEventIDMaskCmd_Payload_t  Payload;
} CFE_EVS_AppNameEventIDMaskCmd_t;

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
typedef struct {
   uint32               AppID;                  /**< \cfetlmmnemonic \EVS_APPID
                                                     \brief Numerical application identifier */
   uint16               AppMessageSentCounter;  /**< \cfetlmmnemonic \EVS_APPMSGSENTC
                                                     \brief Application message sent counter */
   uint8                AppEnableStatus;        /**< \cfetlmmnemonic \EVS_APPENASTAT
                                                     \brief Application event service enable status */
   uint8                Padding;                /**< \cfetlmmnemonic \EVS_SPARE2ALIGN3
                                                     \brief Padding for 32 bit boundary */

} CFE_EVS_AppTlmData_t;


/** 
**  \cfeevstlm Event Services Housekeeping Telemetry Packet
**/
typedef struct {
   uint8                 CommandCounter;                    /**< \cfetlmmnemonic \EVS_CMDPC
                                                                 \brief EVS Command Counter */
   uint8                 CommandErrCounter;                 /**< \cfetlmmnemonic \EVS_CMDEC
                                                                 \brief EVS Command Error Counter */
   uint8                 MessageFormatMode;                 /**< \cfetlmmnemonic \EVS_MSGFMTMODE
                                                                 \brief Event message format mode (short/long) */
   uint8                 MessageTruncCounter;               /**< \cfetlmmnemonic \EVS_MSGTRUNC
                                                                 \brief Event message truncation counter */
   
   uint8                 UnregisteredAppCounter;            /**< \cfetlmmnemonic \EVS_UNREGAPPC
                                                                 \brief Unregistered application message send counter */
   uint8                 OutputPort;                        /**< \cfetlmmnemonic \EVS_OUTPUTPORT
                                                                 \brief Output port mask */
   uint8                 LogFullFlag;                       /**< \cfetlmmnemonic \EVS_LOGFULL
                                                                 \brief Local event log full flag */
   uint8                 LogMode;                           /**< \cfetlmmnemonic \EVS_LOGMODE
                                                                 \brief Local event logging mode (overwrite/discard) */
   
   uint16                MessageSendCounter;                /**< \cfetlmmnemonic \EVS_MSGSENTC
                                                                 \brief Event message send counter */
   uint16                LogOverflowCounter;                /**< \cfetlmmnemonic \EVS_LOGOVERFLOWC
                                                                 \brief Local event log overflow counter */
   
   uint8                 LogEnabled;                        /**< \cfetlmmnemonic \EVS_LOGENABLED
                                                                 \brief Current event log enable/disable state */
   uint8                 Spare1;                            /**< \cfetlmmnemonic \EVS_HK_SPARE1
                                                                 \brief Padding for 32 bit boundary */
   uint8                 Spare2;                            /**< \cfetlmmnemonic \EVS_HK_SPARE2
                                                                 \brief Padding for 32 bit boundary */
   uint8                 Spare3;                            /**< \cfetlmmnemonic \EVS_HK_SPARE3
                                                                 \brief Padding for 32 bit boundary */
   
   CFE_EVS_AppTlmData_t  AppData[CFE_ES_MAX_APPLICATIONS];  /**< \cfetlmmnemonic \EVS_APP
                                                                 \brief Array of registered application table data */

} CFE_EVS_TlmPkt_Payload_t;

typedef struct {
   uint8                    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   CFE_EVS_TlmPkt_Payload_t Payload;
} CFE_EVS_TlmPkt_t;

/** Telemetry packet structures */

typedef struct {
   char    AppName[OS_MAX_API_NAME];  /**< \cfetlmmnemonic \EVS_APPNAME
                                           \brief Application name */
   uint16  EventID;                   /**< \cfetlmmnemonic \EVS_EVENTID
                                           \brief Numerical event identifier */
   uint16  EventType;                 /**< \cfetlmmnemonic \EVS_EVENTTYPE
                                           \brief Numerical event type identifier */
   uint32  SpacecraftID;              /**< \cfetlmmnemonic \EVS_SCID
                                           \brief Spacecraft identifier */
   uint32  ProcessorID;               /**< \cfetlmmnemonic \EVS_PROCESSORID
                                           \brief Numerical processor identifier */

} CFE_EVS_PacketID_t;


/** 
**  \cfeevstlm Event Message Telemetry Packet
**/
typedef struct {
   CFE_EVS_PacketID_t        PacketID;                              /**< \brief Event packet information */
   char                      Message[CFE_EVS_MAX_MESSAGE_LENGTH];   /**< \cfetlmmnemonic \EVS_EVENT
                                                                         \brief Event message string */
   uint8                     Spare1;                                /**< \cfetlmmnemonic \EVS_SPARE1
                                                                         \brief Structure padding */
   uint8                     Spare2;		                        /**< \cfetlmmnemonic \EVS_SPARE2
                                                                         \brief Structure padding */
} CFE_EVS_Packet_Payload_t;

typedef struct {
   uint8                    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   CFE_EVS_Packet_Payload_t Payload;
} CFE_EVS_Packet_t;


#define CFE_EVS_HK_TLM_LNGTH  sizeof(CFE_EVS_TlmPkt_t)


#endif



