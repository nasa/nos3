/*
** $Id: cfe_time_msg.h 1.6 2011/11/30 15:10:45GMT-05:00 jmdagost Exp  $
**
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
*
** Purpose:  cFE Time Services (TIME) SB message definitions header file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: cfe_time_msg.h  $
** Revision 1.6 2011/11/30 15:10:45GMT-05:00 jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.5 2010/10/25 15:01:13EDT jmdagost 
** Corrected bad apostrophe in prologue.
** Revision 1.4 2010/10/04 15:25:01EDT jmdagost 
** Cleaned up copyright symbol.
** Revision 1.3 2009/06/08 16:13:08EDT lwalling 
** Change 1Hz adjust cmd arg from micro-seconds to sub-seconds
** Revision 1.2 2008/12/08 12:07:17EST dkobe 
** Updates to correct doxygen errors
** Revision 1.1 2008/04/17 08:05:26EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.12 2007/09/12 09:46:04EDT David Kobe (dlkobe) 
** Added doxygen comments to commands and some configuration parameters
** Revision 1.11 2007/06/07 13:07:49EDT dlkobe 
** Added Command and Telemetry doxygen comments to TIME, EVS and SB
** Revision 1.10 2007/05/23 11:22:01EDT dlkobe 
** Added doxygen formatting
** Revision 1.9 2007/04/27 12:31:07EDT njyanchik 
** I changed EVS and TIME to not have a telemetry counter for SB pipe reads in the main loop. 
** Instead, I have added a syslog message to state the problem
** Revision 1.8 2007/04/11 11:02:10EDT njyanchik 
** Remove the TaskDelay in the main loop for TIME. I also addeda counterin telemtry that will
** notify the ground of an error in reading the command pipe.
** Revision 1.7 2007/03/07 12:03:37EST njyanchik 
** There were errors in the previous change package, so I fixed them
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_msg_
#define _cfe_time_msg_

/*
** Required header files...
*/
#include "cfe.h"

/*************************************************************************/

/*
** Time task command packet command codes...
*/
/** \name Time Services Command Codes */
/** \{ */

/** \cfetimecmd Time No-Op
**
**  \par Description
**       This command performs no other function than to increment the 
**       command execution counter. The command may be used to verify 
**       general aliveness of the Time Services task.
**
**  \cfecmdmnemonic \TIME_NOOP
**
**  \par Command Structure
**       #CFE_TIME_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - The #CFE_TIME_NOOP_EID informational event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_TIME_NOOP_CC         0    /* no-op command */

/** \cfetimecmd Time Reset Counters
**
**  \par Description
**       This command resets the following counters within the Time
**       Services \link CFE_TIME_HkPacket_t Housekeeping Telemetry \endlink:
**       - Command Execution Counter (\TIME_CMDPC)
**       - Command Error Counter (\TIME_CMDEC)
**       This command also resets the following counters within the
**       Time Services \link CFE_TIME_DiagPacket_t Diagnostic Telemetry \endlink:
**       - Tone Signal Detected Software Bus Message Counter (\TIME_TSDETCNT)
**       - Time at the Tone Data Software Bus Message Counter (\TIME_TATTCNT)
**       - Tone Signal/Data Verify Counter (\TIME_VERIFYCNT)
**       - Tone Signal/Data Error Counter (\TIME_VERIFYER)
**       - Tone Signal Interrupt Counter (\TIME_TSISRCNT)
**       - Tone Signal Interrupt Error Counter (\TIME_TSISRERR)
**       - Tone Signal Task Counter (\TIME_TSTASKCNT)
**       - Local 1 Hz Interrupt Counter (\TIME_1HZISRCNT)
**       - Local 1 Hz Task Counter (\TIME_1HZTASKCNT)
**       - Reference Time Version Counter (\TIME_VERSIONCNT)
**
**  \cfecmdmnemonic \TIME_RESETCTRS
**
**  \par Command Structure
**       #CFE_TIME_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - The #CFE_TIME_RESET_EID informational event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time 
**       Services receives the command, the event is sent (although it 
**       may be filtered by EVS) and the counter is incremented 
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_TIME_RESET_CC        1    /* reset counters */

/** \cfetimecmd Request TIME Diagnostic Telemetry
**
**  \par Description
**       This command requests that the Time Service generate a message 
**       containing various data values not included in the normal Time 
**       Service housekeeping message.  The command requests only a single 
**       copy of the diagnostic message.  Refer to #CFE_TIME_DiagPacket_t for 
**       a description of the Time Service diagnostic message contents. 
**
**  \cfecmdmnemonic \TIME_REQUESTDIAG
**
**  \par Command Structure
**       #CFE_TIME_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - Sequence Counter for #CFE_TIME_DiagPacket_t will increment
**       - The #CFE_TIME_DIAG_EID debug event message will be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Time 
**       Services receives the command, the event and telemetry is sent 
**       (although one or both may be filtered by EVS and TO) and the 
**       counter is incremented unconditionally.
**
**  \par Criticality
**       None
**
**  \sa 
*/
#define CFE_TIME_DIAG_TLM_CC     2    /* request diagnostic hk telemetry */

/** \cfetimecmd Set Time Source
**
**  \par Description
**       This command selects the Time Service clock source.  Although the 
**       list of potential clock sources is mission specific and defined 
**       via configuration parameters, this command provides a common method 
**       for switching between the local processor clock and an external 
**       source for time data.<BR><BR>
**       When commanded to accept external time data (GPS, MET, spacecraft 
**       time, etc.), the Time Server will enable input via an API function 
**       specific to the configuration definitions for the particular source.  
**       When commanded to use internal time data, the Time Server will ignore 
**       the external data.  However, the Time Server will continue to use the 
**       API function as the trigger to generate a "time at the tone" command 
**       packet regardless of the internal/external command selection.<BR><BR>
**       Notes:
**       - Operating in FLYWHEEL mode is not considered a choice related 
**         to clock source, but rather an element of the clock state.  See below 
**         for a description of the #CFE_TIME_SET_STATE_CC command.
**       - This command is only valid when the #CFE_TIME_CFG_SOURCE configuration
**         parameter in the cfe_platform_cfg.h file has been set to TRUE.
**
**  \cfecmdmnemonic \TIME_SETSOURCE
**
**  \par Command Structure
**       #CFE_TIME_SourceCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_SOURCE - Diagnostic Telemetry point will indicate the
**         command specified value
**       - The #CFE_TIME_SOURCE_EID informational event message will be generated
**
**  \par Error Conditions
**       - Invalid Source selection 
**         (a value other than #CFE_TIME_USE_INTERN or #CFE_TIME_USE_EXTERN was specified)
**       - Time source selection not allowed on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (either #CFE_TIME_SOURCE_CFG_EID or #CFE_TIME_SOURCE_ERR_EID)
**
**  \par Criticality
**       Although clock source selection is important, this command is not critical.
**
**  \sa #CFE_TIME_SET_STATE_CC, #CFE_TIME_SET_SIGNAL_CC
*/
#define CFE_TIME_SET_SOURCE_CC   3    /* set clock source (int vs ext) */

/** \cfetimecmd Set Time State
**
**  \par Description
**       This command indirectly affects the Time Service on-board determination 
**       of clock state.  Clock state is a combination of factors, most significantly 
**       whether the spacecraft time has been accurately set, and whether Time Service 
**       is operating in FLYWHEEL mode.<BR><BR>
**       This command may be used to notify the Time Server that spacecraft time is 
**       now correct, or that time is no longer correct.  This information will be 
**       distributed to Time Clients, and in turn, to any interested sub-systems.<BR><BR> 
**       Also, this command may be used to force a Time Server or Time Client into 
**       FLYWHEEL mode.  Use of FLYWHEEL mode is mainly for debug purposes although 
**       in extreme circumstances, it may be of value to force Time Service not to rely 
**       on normal time updates.  Note that when commanded into FLYWHEEL mode, the Time 
**       Service will remain so until receipt of another "set state" command setting the 
**       state into a mode other than FLYWHEEL.<BR><BR>
**       Note also that setting the clock state to VALID or INVALID on a Time Client that 
**       is currently getting time updates from the Time Server will have very limited 
**       effect.  As soon as the Time Client receives the next time update, the VALID/INVALID 
**       selection will be set to that of the Time Server.  However, setting a Time Client 
**       to FLYWHEEL cannot be overridden by the Time Server since the Time Client will 
**       ignore time updates from the Time Server while in FLYWHEEL mode.
**
**  \cfecmdmnemonic \TIME_SETSTATE
**
**  \par Command Structure
**       #CFE_TIME_StateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STATEFLG - Housekeeping Telemetry point "may"l indicate the
**         command specified value (see above)
**       - The #CFE_TIME_STATE_EID informational event message will be generated
**
**  \par Error Conditions
**       - Invalid State selection 
**         (a value other than #CFE_TIME_INVALID, #CFE_TIME_VALID or #CFE_TIME_FLYWHEEL was specified)
**       - Time source selection not allowed on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (#CFE_TIME_STATE_ERR_EID)
**
**  \par Criticality
**       Setting Time Service into FLYWHEEL mode is not particularly hazardous, as 
**       the result may be that the calculation of spacecraft time is done using a 
**       less than optimal timer.  However, inappropriately setting the clock state 
**       to VALID (indicating that spacecraft time is accurate) may result in other 
**       sub-systems performing incorrect time based calculations.  The specific risk 
**       is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_SOURCE_CC, #CFE_TIME_SET_SIGNAL_CC
*/
#define CFE_TIME_SET_STATE_CC    4    /* set clock state */

/** \cfetimecmd Add Time to Tone Time Delay
**
**  \par Description
**       This command is used to factor out a known, predictable latency between the 
**       Time Server and a particular Time Client.  The correction is applied (added) 
**       to the current time calculation for Time Clients, so this command has no 
**       meaning for Time Servers.  Each Time Client can have a unique latency setting.  
**       The latency value is a positive number of seconds and microseconds that represent 
**       the deviation from the time maintained by the Time Server. 
**
**  \cfecmdmnemonic \TIME_ADDCLOCKLAT
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LATENTS - Housekeeping Telemetry point indicating command specified values
**       - \b \c \TIME_LATENTDIR - Diagnostic Telemetry point indicating commanded latency direction
**       - The #CFE_TIME_DELAY_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Client
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELAY_CFG_EID or #CFE_TIME_DELAY_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SUB_DELAY_CC
*/
#define CFE_TIME_ADD_DELAY_CC    5    /* add tone delay value */

/** \cfetimecmd Subtract Time from Tone Time Delay
**
**  \par Description
**       This command is used to factor out a known, predictable latency between the Time Server 
**       and a particular Time Client.  The correction is applied (subtracted) to the current time 
**       calculation for Time Clients, so this command has no meaning for Time Servers.  Each Time 
**       Client can have a unique latency setting.  The latency value is a positive number of seconds 
**       and microseconds that represent the deviation from the time maintained by the Time Server.<BR><BR>
**       Note that it is unimaginable that the seconds value will ever be anything but zero.  
**
**  \cfecmdmnemonic \TIME_SUBCLOCKLAT
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LATENTS - Housekeeping Telemetry point indicating command specified values
**       - \b \c \TIME_LATENTDIR - Diagnostic Telemetry point indicating commanded latency direction
**       - The #CFE_TIME_DELAY_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Client
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELAY_CFG_EID or #CFE_TIME_DELAY_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_DELAY_CC
*/
#define CFE_TIME_SUB_DELAY_CC    6    /* sub tone delay value */

/** \cfetimecmd Set Spacecraft Time
**
**  \par Description
**       This command sets the spacecraft clock to a new value, 
**       regardless of the current setting (time jam). The new time 
**       value represents the desired offset from the mission-defined 
**       time epoch and takes effect immediately upon execution of 
**       this command.  Time Service will calculate a new STCF value 
**       based on the current MET and the desired new time using one 
**       of the following: <BR><BR>
**       If Time Service is configured to compute current time as TAI<BR>
**       - <B> STCF = (new time) - (current MET) </B>
**       - <B> (current time) = (current MET) + STCF </B>
**       <BR><BR>If Time Service is configured to compute current time as UTC
**       - <B> STCF = ((new time) - (current MET)) + (Leap Seconds) </B>
**       - <B> (current time) = ((curent MET) + STCF) - (Leap Seconds) </B>
**
**  \cfecmdmnemonic \TIME_SETCLOCK
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating newly calculated STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating newly calculated STCF subseconds value
**       - The #CFE_TIME_TIME_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_TIME_CFG_EID or #CFE_TIME_TIME_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_STCF_CC, #CFE_TIME_SET_LEAPS_CC
*/
#define CFE_TIME_SET_TIME_CC     7    /* set time */

/** \cfetimecmd Set Mission Elapsed Time
**
**  \par Description
**       This command sets the Mission Elapsed Timer (MET) to the specified value.<BR><BR>
**       Note that the MET (as implemented for cFE Time Service) is a logical representation 
**       and not a physical timer.  Thus, setting the MET is not dependent on whether the 
**       hardware supports a MET register that can be written to.<BR><BR>
**       Note also that Time Service "assumes" that during normal operation, the MET is 
**       synchronized to the tone signal.  Therefore, unless operating in FLYWHEEL mode, 
**       the sub-seconds portion of the MET will be set to zero at the next tone signal interrupt.<BR><BR>  
**       The new MET takes effect immediately upon execution of this command.
**
**  \cfecmdmnemonic \TIME_SETCLOCKMET
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_METSECS - Housekeeping Telemetry point indicating new MET seconds value
**       - \b \c \TIME_METSUBSECS - Housekeeping Telemetry point indicating new MET subseconds value
**       - The #CFE_TIME_MET_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_MET_CFG_EID or #CFE_TIME_MET_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_STCF_CC, #CFE_TIME_SET_LEAPS_CC
*/
#define CFE_TIME_SET_MET_CC      8    /* set MET */

/** \cfetimecmd Set Spacecraft Time Correlation Factor
**
**  \par Description
**       This command sets the Spacecraft Time Correlation Factor (STCF) to the specified value.  
**       This command differs from the previously described SET CLOCK in the nature of the command 
**       argument.  This command sets the STCF value directly, rather than extracting the STCF 
**       from a value representing the total of MET, STCF and optionally, Leap Seconds.  The new 
**       STCF takes effect immediately upon execution of this command. 
**
**  \cfecmdmnemonic \TIME_SETCLOCKSTCF
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new  STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new  STCF subseconds value
**       - The #CFE_TIME_STCF_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_STCF_CFG_EID or #CFE_TIME_STCF_ERR_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_LEAPS_CC
*/
#define CFE_TIME_SET_STCF_CC     9    /* set STCF */

/** \cfetimecmd Set Leap Seconds
**
**  \par Description
**       This command sets the spacecraft Leap Seconds to the specified value.  
**       Leap Seconds may be positive or negative, and there is no limit to the 
**       value except, of course, the limit imposed by the 16 bit signed integer 
**       data type.  The new Leap Seconds value takes effect immediately upon 
**       execution of this command.
**
**  \cfecmdmnemonic \TIME_SETCLOCKLEAP
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_LEAPSECS - Housekeeping Telemetry point indicating new Leap seconds value
**       - The #CFE_TIME_LEAPS_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_LEAPS_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_SET_TIME_CC, #CFE_TIME_SET_MET_CC, #CFE_TIME_SET_STCF_CC
*/
#define CFE_TIME_SET_LEAPS_CC    10   /* set Leap Seconds */

/** \cfetimecmd Add Delta to Spacecraft Time Correlation Factor 
**
**  \par Description
**       This command adjusts the Spacecraft Time Correlation Factor (STCF) by 
**       adding the specified value.  The new STCF takes effect immediately upon 
**       execution of this command. 
**
**  \cfecmdmnemonic \TIME_ADDSTCFADJ
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_DELTA_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELTA_ERR_EID or #CFE_TIME_DELTA_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_1HZADJ_CC, #CFE_TIME_SUB_1HZADJ_CC
*/
#define CFE_TIME_ADD_ADJUST_CC   11   /* add one time STCF adjustment */

/** \cfetimecmd Subtract Delta from Spacecraft Time Correlation Factor 
**
**  \par Description
**       This command adjusts the Spacecraft Time Correlation Factor (STCF) by subtracting the specified 
**       value.  The new STCF takes effect immediately upon execution of this command. 
**
**  \cfecmdmnemonic \TIME_SUBSTCFADJ
**
**  \par Command Structure
**       #CFE_TIME_TimeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_DELTA_EID informational event message will be generated
**
**  \par Error Conditions
**       - An invalid number of microseconds was specified (must be less than 1 million)
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event messages will be issued (#CFE_TIME_DELTA_ERR_EID or #CFE_TIME_DELTA_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_ADD_1HZADJ_CC, #CFE_TIME_SUB_1HZADJ_CC
*/
#define CFE_TIME_SUB_ADJUST_CC   12   /* subtract one time STCF adjustment */

/** \cfetimecmd Add Delta to Spacecraft Time Correlation Factor each 1Hz
**
**  \par Description
**       This command has been updated to take actual sub-seconds (1/2^32 seconds)
**       rather than micro-seconds as an input argument.  This change occurred
**       after the determination was made that one micro-second is too large an
**       increment for a constant 1Hz adjustment.<BR><BR>
**       This command continuously adjusts the Spacecraft Time Correlation Factor (STCF) 
**       every second, by adding the specified value.  The adjustment to the STCF is 
**       applied in the Time Service local 1Hz interrupt handler.  As the local 1Hz 
**       interrupt is not synchronized to the tone signal, one cannot say when the 
**       adjustment will occur, other than once a second, at about the same time 
**       relative to the tone.<BR><BR>
**       There was some debate about whether the maximum 1Hz clock drift correction
**       factor would ever need to exceed some small fraction of a second.  But, the
**       decision was made to provide the capability to make 1Hz adjustments greater
**       than one second and leave it to the ground system to provide mission specific
**       limits.
**
**  \cfecmdmnemonic \TIME_ADD1HZSTCF
**
**  \par Command Structure
**       #CFE_TIME_1HzAdjCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_1HZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_1HZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_SUB_1HZADJ_CC
*/
#define CFE_TIME_ADD_1HZADJ_CC   13   /* add 1Hz STCF adjustment */

/** \cfetimecmd Subtract Delta from Spacecraft Time Correlation Factor each 1Hz 
**
**  \par Description
**       This command has been updated to take actual sub-seconds (1/2^32 seconds)
**       rather than micro-seconds as an input argument.  This change occurred
**       after the determination was made that one micro-second is too large an
**       increment for a constant 1Hz adjustment.<BR><BR>
**       This command continuously adjusts the Spacecraft Time Correlation Factor (STCF) 
**       every second, by subtracting the specified value.  The adjustment to the STCF
**       is applied in the Time Service local 1Hz interrupt handler.  As the local 1Hz 
**       interrupt is not synchronized to the tone signal, one cannot say when the 
**       adjustment will occur, other than once a second, at about the same time 
**       relative to the tone.<BR><BR>
**       There was some debate about whether the maximum 1Hz clock drift correction
**       factor would ever need to exceed some small fraction of a second.  But, the
**       decision was made to provide the capability to make 1Hz adjustments greater
**       than one second and leave it to the ground system to provide mission specific
**       limits.
**
**  \cfecmdmnemonic \TIME_SUB1HZSTCF
**
**  \par Command Structure
**       #CFE_TIME_1HzAdjCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will increment
**       - \b \c \TIME_STCFSECS - Housekeeping Telemetry point indicating new STCF seconds value
**       - \b \c \TIME_STCFSUBSECS - Housekeeping Telemetry point indicating new STCF subseconds value
**       - The #CFE_TIME_1HZ_EID informational event message will be generated
**
**  \par Error Conditions
**       - Platorm receiving the command is not a Time Server
**       <BR><BR>Evidence of Failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - command error counter will increment
**       - Error specific event message will be issued (#CFE_TIME_1HZ_CFG_EID)
**
**  \par Criticality
**       Inappropriately setting the clock may result in other sub-systems performing incorrect 
**       time based calculations.  The specific risk is dependent upon the behavior of those sub-systems.
**
**  \sa #CFE_TIME_ADD_ADJUST_CC, #CFE_TIME_SUB_ADJUST_CC, #CFE_TIME_ADD_1HZADJ_CC
*/
#define CFE_TIME_SUB_1HZADJ_CC   14   /* subtract 1Hz STCF adjustment */


/** \cfetimecmd Set Tone Signal Source
**
**  \par Description
**       This command selects the Time Service tone signal source.  Although the 
**       list of potential tone signal sources is mission specific, a common 
**       choice is the selection of primary or redundant tone signal.  The selection 
**       may be available to both the Time Server and Time Clients, depending on 
**       hardware configuration.<BR><BR>
**       Notes: 
**       - This command is only valid when the #CFE_TIME_CFG_SIGNAL configuration
**         parameter in the cfe_platform_cfg.h file has been set to TRUE.
**
**  \cfecmdmnemonic \TIME_SETSIGNAL
**
**  \par Command Structure
**       #CFE_TIME_SignalCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the 
**       following telemetry:
**       - \b \c \TIME_CMDPC - command execution counter will 
**         increment
**       - \b \c \TIME_SIGNAL - Diagnostic Telemetry point will indicate the
**         command specified value
**       - The #CFE_TIME_SIGNAL_EID informational event message will 
**         be generated
**
**  \par Error Conditions
**       - Invalid Signal selection 
**         (a value other than #CFE_TIME_TONE_PRI or #CFE_TIME_TONE_RED was specified)
**       - Multiple Tone Signal Sources not available on this platform
**       <BR><BR>Evidence of failure may be found in the following telemetry:
**       - \b \c \TIME_CMDEC - Command Error counter will increment
**       - Error specific event message (either #CFE_TIME_SIGNAL_CFG_EID or #CFE_TIME_SIGNAL_ERR_EID)
**
**  \par Criticality
**       Although tone signal source selection is important, this command is not critical
**
**  \sa #CFE_TIME_SET_STATE_CC, #CFE_TIME_SET_SOURCE_CC
*/
#define CFE_TIME_SET_SIGNAL_CC   15    /* set clock signal (pri vs red) */
/** \} */

/*
** Clock source command arguments...
*/
/** \name Clock Source Selection Parameters */
/** \{ */
#define CFE_TIME_USE_INTERN      1
#define CFE_TIME_USE_EXTERN      2
/** \} */

/*
** Clock signal command arguments...
*/
/** \name Clock Signal Selection Parameters */
/** \{ */
#define CFE_TIME_TONE_PRI        1
#define CFE_TIME_TONE_RED        2
/** \} */

/*
** STCF adjustment direction (for both "one time" and "1Hz" adjustments)...
*/
/** \name Spacecraft Time Correlation Factor Adjustment Parameters */
/** \{ */
#define CFE_TIME_ADD_ADJUST      1    /* add STCF adjustment value */
#define CFE_TIME_SUB_ADJUST      2    /* subtract STCF adjustment value */
/** \} */

/*
** Fly-wheel status values (are we currently fly-wheeling)...
*/
/** \name Fly-wheel status values */
/** \{ */
#define CFE_TIME_NO_FLY          0
#define CFE_TIME_IS_FLY          1
/** \} */

/*
** Clock "set" status values (has the clock been set to correct time)...
*/
/** \name Clock "set" status values */
/** \{ */
#define CFE_TIME_NOT_SET         0
#define CFE_TIME_WAS_SET         1
/** \} */

/*
** Clock "state flag" definitions (state data converted to flags)...
*/
/** \name Clock "state flag" values */
/** \{ */
#define CFE_TIME_FLAG_CLKSET     0x8000   /**< \brief The spacecraft time has been set */
#define CFE_TIME_FLAG_FLYING     0x4000   /**< \brief This instance of Time Services is flywheeling */
#define CFE_TIME_FLAG_SRCINT     0x2000   /**< \brief The clock source is set to "internal" */
#define CFE_TIME_FLAG_SIGPRI     0x1000   /**< \brief The clock signal is set to "primary" */
#define CFE_TIME_FLAG_SRVFLY     0x0800   /**< \brief The Time Server is in flywheel mode */
#define CFE_TIME_FLAG_CMDFLY     0x0400   /**< \brief This instance of Time Services was commanded into flywheel mode */
#define CFE_TIME_FLAG_ADDADJ     0x0200   /**< \brief One time STCF Adjustment is to be done in positive direction */
#define CFE_TIME_FLAG_ADD1HZ     0x0100   /**< \brief 1 Hz STCF Adjustment is to be done in a positive direction */
#define CFE_TIME_FLAG_ADDTCL     0x0080   /**< \brief Time Client Latency is applied in a positive direction */
#define CFE_TIME_FLAG_SERVER     0x0040   /**< \brief This instance of Time Services is a Time Server */
#define CFE_TIME_FLAG_GDTONE     0x0020   /**< \brief The tone received is good compared to the last tone received */
#define CFE_TIME_FLAG_UNUSED     0x001F   /**< \brief Reserved flags - should be zero */
/** \} */


/*************************************************************************/

/*
** Type definition (generic "no arguments" command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CFE_TIME_NoArgsCmd_t;


/*
** Type definition (leap seconds command)...
*/
typedef struct
{
    int16                 LeapSeconds;
} CFE_TIME_LeapsCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_LeapsCmd_Payload_t   Payload;
} CFE_TIME_LeapsCmd_t;


/*
** Type definition (clock state command)...
*/
typedef struct
{
    int16                 ClockState;                    /**< \brief #CFE_TIME_INVALID=Spacecraft time has not been accurately set,
                                                                     #CFE_TIME_VALID=Spacecraft clock has been accurately set,
                                                                     #CFE_TIME_FLYWHEEL=Force into FLYWHEEL mode   */
                                                         /**< Selects the current clock state */
} CFE_TIME_StateCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_StateCmd_Payload_t   Payload;
} CFE_TIME_StateCmd_t;


/*
** Type definition (time data source command)...
*/
typedef struct
{
    int16                 TimeSource;                    /**< \brief #CFE_TIME_USE_INTERN=Internal Source,
                                                                     #CFE_TIME_USE_EXTERN=External Source   */
                                                         /**< Selects either the "Internal" and "External" clock source */
} CFE_TIME_SourceCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_SourceCmd_Payload_t  Payload;
} CFE_TIME_SourceCmd_t;


/*
** Type definition (tone signal source command)...
*/
typedef struct
{
    int16                 ToneSource;                    /**< \brief #CFE_TIME_TONE_PRI=Primary Source,
                                                                     #CFE_TIME_TONE_RED=Redundant Source   */
                                                         /**< Selects either the "Primary" or "Redundant" tone signal source */
} CFE_TIME_SignalCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_SignalCmd_Payload_t  Payload;

} CFE_TIME_SignalCmd_t;


/*
** Type definition (generic "time argument" command)...
*/
typedef struct
{
    uint32                Seconds;
    uint32                MicroSeconds;
} CFE_TIME_TimeCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_TimeCmd_Payload_t    Payload;
} CFE_TIME_TimeCmd_t;


/*
** Type definition (1Hz STCF adjustment "time argument" command)...
*/
typedef struct
{
    uint32                Seconds;
    uint32                Subseconds;

} CFE_TIME_1HzAdjCmd_Payload_t;

typedef struct
{
  uint8                         CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_1HzAdjCmd_Payload_t  Payload;

} CFE_TIME_1HzAdjCmd_t;


/*
** Type definition (local 1Hz wake-up command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CFE_TIME_1HzCmd_t;


/*
** Type definition (time at the tone signal command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CFE_TIME_ToneSignalCmd_t;


/*
** Type definition ("fake" time at the tone signal command)...
*/
typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} CFE_TIME_FakeToneCmd_t;


/*
** Type definition (time at the tone data command)...
*/
typedef struct
{
    CFE_TIME_SysTime_t    AtToneMET;    /**< \brief MET at time of tone */
    CFE_TIME_SysTime_t    AtToneSTCF;   /**< \brief STCF at time of tone */
    int16                 AtToneLeaps;  /**< \brief Leap Seconds at time of tone */
    int16                 AtToneState;  /**< \brief Clock state at time of tone */
} CFE_TIME_ToneDataCmd_Payload_t;

typedef struct
{
  uint8                             CmdHeader[CFE_SB_CMD_HDR_SIZE];
  CFE_TIME_ToneDataCmd_Payload_t    Payload;
} CFE_TIME_ToneDataCmd_t;


/*************************************************************************/

/** 
**  \cfetimetlm Time Services Housekeeping Packet
**/
typedef struct
{
    /*
    ** Task command interface counters...
    */
    uint8                 CmdCounter;       /**< \cfetlmmnemonic \TIME_CMDPC
                                                 \brief Time Command Execution Counter */
    uint8                 ErrCounter;       /**< \cfetlmmnemonic \TIME_CMDEC
                                                 \brief Time Command Error Counter */

    /*
    ** Clock state flags and "as calculated" clock state...
    */
    uint16                ClockStateFlags;  /**< \cfetlmmnemonic \TIME_STATEFLG
                                                 \brief State Flags */
    int16                 ClockStateAPI;    /**< \cfetlmmnemonic \TIME_APISTATE
                                                 \brief API State */

    /*
    ** Leap Seconds...
    */
    int16                 LeapSeconds;      /**< \cfetlmmnemonic \TIME_LEAPSECS
                                                 \brief Current Leaps Seconds */

    /*
    ** Current MET and STCF time values...
    */
    uint32                SecondsMET;       /**< \cfetlmmnemonic \TIME_METSECS
                                                 \brief Current MET (seconds) */
    uint32                SubsecsMET;       /**< \cfetlmmnemonic \TIME_METSUBSECS
                                                 \brief Current MET (sub-seconds) */

    uint32                SecondsSTCF;      /**< \cfetlmmnemonic \TIME_STCFSECS
                                                 \brief Current STCF (seconds) */
    uint32                SubsecsSTCF;      /**< \cfetlmmnemonic \TIME_STCFSUBSECS
                                                 \brief Current STCF (sub-seconds) */

    /*
    ** 1Hz STCF adjustment values (server only)...
    */
    #if (CFE_TIME_CFG_SERVER == TRUE)
    uint32                Seconds1HzAdj;    /**< \cfetlmmnemonic \TIME_1HZADJSECS
                                                 \brief Current 1 Hz SCTF adjustment (seconds) */
    uint32                Subsecs1HzAdj;    /**< \cfetlmmnemonic \TIME_1HZADJSSECS
                                                 \brief Current 1 Hz SCTF adjustment (sub-seconds) */
    #endif

    /*
    ** Time at tone delay values (client only)...
    */
    #if (CFE_TIME_CFG_CLIENT == TRUE)
    uint32                SecondsDelay;     /**< \cfetlmmnemonic \TIME_1HZDLYSECS
                                                 \brief Current 1 Hz SCTF Delay (seconds) */
    uint32                SubsecsDelay;     /**< \cfetlmmnemonic \TIME_1HZDLYSSECS
                                                 \brief Current 1 Hz SCTF Delay (sub-seconds) */
    #endif

} CFE_TIME_HkPacket_Payload_t;

typedef struct
{
  uint8                         TlmHeader[CFE_SB_TLM_HDR_SIZE];
  CFE_TIME_HkPacket_Payload_t   Payload;
} CFE_TIME_HkPacket_t;


/*************************************************************************/

/** 
**  \cfetimetlm Time Services Diagnostics Packet
**/
typedef struct
{
    /*
     ** Data values used to compute time (in reference to "tone")...
     */
    CFE_TIME_SysTime_t    AtToneMET;      /**< \cfetlmmnemonic \TIME_TMETS
	                                             \brief MET at time of tone */
    CFE_TIME_SysTime_t    AtToneSTCF;     /**< \cfetlmmnemonic \TIME_STCFS
	                                             \brief STCF at time of tone */
    CFE_TIME_SysTime_t    AtToneDelay;    /**< \cfetlmmnemonic \TIME_LATENTS
	                                             \brief Adjustment for slow tone detection */
    CFE_TIME_SysTime_t    AtToneLatch;    /**< \cfetlmmnemonic \TIME_TVALIDS
	                                             \brief Local clock latched at time of tone */

    int16                 AtToneLeaps;    /**< \cfetlmmnemonic \TIME_LEAPS
	                                             \brief Leap Seconds at time of tone */
    int16                 ClockStateAPI;  /**< \cfetlmmnemonic \TIME_APISTATE
	                                             \brief Clock state as per API */

    /*
     ** Data values that reflect the time (right now)...
     */
    CFE_TIME_SysTime_t    TimeSinceTone;  /**< \cfetlmmnemonic \TIME_ELAPSEDS
	                                             \brief Time elapsed since the tone */
    CFE_TIME_SysTime_t    CurrentLatch;   /**< \cfetlmmnemonic \TIME_LOCALS
	                                             \brief Local clock latched just "now" */
    CFE_TIME_SysTime_t    CurrentMET;     /**< \cfetlmmnemonic \TIME_METS
	                                             \brief MET at this instant */
    CFE_TIME_SysTime_t    CurrentTAI;     /**< \cfetlmmnemonic \TIME_TAIS
	                                             \brief TAI at this instant */
    CFE_TIME_SysTime_t    CurrentUTC;     /**< \cfetlmmnemonic \TIME_UTCS
	                                             \brief UTC at this instant */

    /*
     ** Data values used to define the current clock state...
     */
    int16                 ClockSetState;  /**< \cfetlmmnemonic \TIME_VALID
	                                             \brief Time has been "set" */
    int16                 ClockFlyState;  /**< \cfetlmmnemonic \TIME_FLYWHEEL
	                                             \brief Current fly-wheel state */
    int16                 ClockSource;    /**< \cfetlmmnemonic \TIME_SOURCE
	                                             \brief Internal vs external, etc. */
    int16                 ClockSignal;    /**< \cfetlmmnemonic \TIME_SIGNAL
	                                             \brief Primary vs redundant, etc. */
    int16                 ServerFlyState; /**< \cfetlmmnemonic \TIME_SRVFLY
	                                             \brief Used by clients only */
    int16                 Forced2Fly;     /**< \cfetlmmnemonic \TIME_CMD2FLY
	                                             \brief Commanded into fly-wheel */

    /*
     ** Clock state flags...
     */
    uint16                ClockStateFlags;/**< \cfetlmmnemonic \TIME_STATEFLAGS
	                                             \brief Clock State Flags */

    /*
     ** STCF adjustment direction values...
     */
    int16                 OneTimeDirection; /**< \cfetlmmnemonic \TIME_ADJUSTDIR
	                                               \brief One time STCF adjustment direction (Add = 1,  Sub = 2) */
    int16                 OneHzDirection;   /**< \cfetlmmnemonic \TIME_1HZADJDIR
	                                               \brief 1Hz STCF adjustment direction */
    int16                 DelayDirection;   /**< \cfetlmmnemonic \TIME_LATENTDIR
	                                               \brief Client latency adjustment direction */

    /*
     ** STCF adjustment values...
     */
    CFE_TIME_SysTime_t    OneTimeAdjust;    /**< \cfetlmmnemonic \TIME_ADJUSTS
	                                               \brief Previous one-time STCF adjustment */
    CFE_TIME_SysTime_t    OneHzAdjust;      /**< \cfetlmmnemonic \TIME_1HZADJS
	                                               \brief Current 1Hz STCF adjustment */

    /*
     ** Most recent local clock latch values...
     */
    CFE_TIME_SysTime_t    ToneSignalLatch;  /**< \cfetlmmnemonic \TIME_TTS
	                                               \brief  Local Clock latched at most recent tone signal */
    CFE_TIME_SysTime_t    ToneDataLatch;    /**< \cfetlmmnemonic \TIME_TDS
	                                               \brief  Local Clock latched at arrival of tone data */

    /*
     ** Miscellaneous counters (subject to reset command)...
     */
    uint32                ToneMatchCount;   /**< \cfetlmmnemonic \TIME_VERIFYCNT
	                                               \brief  Tone signal / data verification count */
    uint32                ToneMatchErrors;  /**< \cfetlmmnemonic \TIME_VERIFYER
	                                               \brief  Tone signal / data verification error count */
    uint32                ToneSignalCount;  /**< \cfetlmmnemonic \TIME_TSDETCNT
	                                               \brief  Tone signal detected SB message count */
    uint32                ToneDataCount;    /**< \cfetlmmnemonic \TIME_TATTCNT
	                                               \brief  Time at the tone data SB message count */
    uint32                ToneIntCount;     /**< \cfetlmmnemonic \TIME_TSISRCNT
	                                               \brief  Tone signal ISR execution count */
    uint32                ToneIntErrors;    /**< \cfetlmmnemonic \TIME_TSISRERR
	                                               \brief  Tone signal ISR error count */
    uint32                ToneTaskCount;    /**< \cfetlmmnemonic \TIME_TSTASKCNT
	                                               \brief  Tone task execution count */
    uint32                VersionCount;     /**< \cfetlmmnemonic \TIME_VERSIONCNT
	                                               \brief  Count of mods to time at tone reference data (version) */
    uint32                LocalIntCount;    /**< \cfetlmmnemonic \TIME_1HZISRCNT
	                                               \brief  Local 1Hz ISR execution count */
    uint32                LocalTaskCount;   /**< \cfetlmmnemonic \TIME_1HZTASKCNT
	                                               \brief  Local 1Hz task execution count */

    /*
     ** Miscellaneous counters (not subject to reset command)...
     */
    uint32                VirtualMET;       /**< \cfetlmmnemonic \TIME_LOGICALMET
	                                               \brief  Software MET */

    /*
     ** Time window verification values (converted from micro-secs)...
     **
     ** Regardless of whether the tone follows the time packet, or vice
     **    versa, these values define the acceptable window of time for
     **    the second event to follow the first.  The minimum value may
     **    be as little as zero, and the maximum must be something less
     **    than a second.
     */
    uint32                MinElapsed;       /**< \cfetlmmnemonic \TIME_MINWINDOW
	                                               \brief Min tone signal / data pkt arrival window (Sub-seconds) */
    uint32                MaxElapsed;       /**< \cfetlmmnemonic \TIME_MAXWINDOW
	                                               \brief Max tone signal / data pkt arrival window (Sub-seconds) */

    /*
     ** Maximum local clock value (before roll-over)...
     */
    CFE_TIME_SysTime_t    MaxLocalClock;    /**< \cfetlmmnemonic \TIME_WRAPS
	                                               \brief Max local clock value before rollover */

    /*
     ** Tone signal tolerance limits...
     */
    uint32                ToneOverLimit;    /**< \cfetlmmnemonic \TIME_MAXSS
	                                               \brief Max between tone signal interrupts */
    uint32                ToneUnderLimit;   /**< \cfetlmmnemonic \TIME_MINSS
	                                               \brief Min between tone signal interrupts */

    /*
     ** Reset Area...
     */
    uint32                DataStoreStatus;  /**< \cfetlmmnemonic \TIME_ATASTSTAT
	                                               \brief Data Store status (preserved across processor reset) */
} CFE_TIME_DiagPacket_Payload_t;

typedef struct
{
  uint8                 		TlmHeader[CFE_SB_TLM_HDR_SIZE];
  CFE_TIME_DiagPacket_Payload_t Payload;
} CFE_TIME_DiagPacket_t;


/*************************************************************************/

#endif /* _cfe_time_msg_ */

/************************/
/*  End of File Comment */
/************************/
