/*
** $Id: cfe_time.h 1.6 2014/08/17 17:23:30GMT-05:00 sstrege Exp  $
**
** Purpose:  cFE Time Services (TIME) library API header file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
** $Log: cfe_time.h  $
** Revision 1.6 2014/08/17 17:23:30GMT-05:00 sstrege 
** Fixed doxygen compiler warnings.
** Revision 1.5 2011/11/30 15:10:46EST jmdagost 
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.4 2008/12/08 12:07:21EST dkobe 
** Updates to correct doxygen errors
** Revision 1.3 2008/08/07 09:17:43EDT dkobe 
** Doxygen comments corrected for CFE_TIME_RegisterSynchCallback 
** and CFE_TIME_UnregisterSynchCallback
** Revision 1.2 2008/08/06 22:42:28EDT dkobe 
** Added CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:25EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/inc/project.pj
** Revision 1.9 2007/05/23 11:22:00EDT David Kobe (dlkobe) 
** Added doxygen formatting
** Revision 1.8 2007/03/07 12:03:35EST njyanchik 
** There were errors in the previous change package, so I fixed them
** Revision 1.7 2007/03/07 10:43:45EST njyanchik 
** I made a new API called CFE_TIME_GetClockInfo that is functionally equivilant to CFE_TIME_GetStateFlags. The reason I made a new function is because this function
** will get changed in a future DCR (3191).
** Revision 1.5 2006/11/21 15:58:50EST rjmcgraw 
** Added #define CFE_TIME_PRINTED_STRING_SIZE 24
** Revision 1.4 2006/11/09 15:25:04EST wfmoleski 
** Modified the function prototypes for ExternalMET, ExternalGPS and ExternalTime from int32 to void.
** Revision 1.3 2006/05/04 09:29:00EDT njyanchik 
** Added MET to default spacecraft time conversion
** API is CFE_TIME_SysTime_t CFE_TIME_MET2SCTime (CFE_TIME_SysTime_t METTime)
** Revision 1.2 2005/07/21 15:28:21EDT lswalling 
** Add Time print API function
** Revision 1.1 2005/06/09 10:57:51EDT rperera 
** Initial revision
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_
#define _cfe_time_


/*
** Required header files...
*/
#include "common_types.h"


/*
** Defines
*/
#define CFE_TIME_PRINTED_STRING_SIZE      24 /**< \brief Required size of buffer to be passed into #CFE_TIME_Print (includes null terminator) */

/*
** Type definition (system time)...
*/
/** 
**  \brief Data structure used to hold system time values
**
**  \par Description
**       The #CFE_TIME_SysTime_t data structure is used to hold time
**       values.  Time is referred to as the elapsed time (in seconds
**       and subseconds) since a specified epoch time.  The subseconds
**       field contains the number of 2^(-32) second intervals that have
**       elapsed since the epoch.
**
*/
typedef struct
{
  uint32  Seconds;            /**< \brief Number of seconds since epoch */
  uint32  Subseconds;         /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;

/*
** Type definition (clock state)...
*/
/** 
**  \brief Enumerated types identifying the quality of the current time
**
**  \par Description
**       The #CFE_TIME_ClockState_t enumerations identify the three recognized states of the current time.
**       If the clock has never been successfully synchronized with the primary onboard clock source, the
**       time is conisdered to be #CFE_TIME_INVALID.  If the time is currently synchronized (i.e. - the
**       primary synchronization mechanism has not been dropped for any significant amount of time), then
**       the current time is considered to be #CFE_TIME_VALID.  If the time had, at some point in the past,
**       been synchronized, but the synchronization with the primary onboard clock has since been lost, then
**       the time is considered to be #CFE_TIME_FLYWHEEL.  Since different clocks drift at different rates
**       from one another, the accuracy of the time while in #CFE_TIME_FLYWHEEL is dependent upon the time
**       spent in that state.
*/
typedef enum  
{
  CFE_TIME_INVALID  = -1,     /**< The spacecraft time has not been set since the last clock 
                              **   reset.  Times returned by clock routines have no relationship
                              **   to any ground-based time reference.
                              */
  CFE_TIME_VALID    =  0,     /**< The spacecraft time has been set at least once since the last 
                              **   clock reset, and it is synchronized with the primary on-board 
                              **   time base.  Times returned by clock routines can be trusted.
                              */
  CFE_TIME_FLYWHEEL =  1      /**< The spacecraft time has been set at least once since the last 
                              **   clock reset, but it is not currently synchronized with the 
                              **   primary on-board time base.  Times returned by clock routines 
                              **   are a "best guess" based on a non-optimal oscillator. 
                              */
} CFE_TIME_ClockState_t;

/*
** Type definition (time comparison results)...
*/
/** 
**  \brief Enumerated types identifying the relative relationships of two times
**
**  \par Description
**       Since time fields contain numbers that are relative to an epoch time, then it is possible for a time value
**       to be "negative".  This can lead to some confusion about what relationship exists between two time values.
**       To resolve this confusion, the cFE provides the API #CFE_TIME_Compare which returns these enumerated values.
*/
typedef enum  
{
  CFE_TIME_A_LT_B  = -1,      /**< \brief The first specified time is considered to be before the second specified time */
  CFE_TIME_EQUAL   =  0,      /**< \brief The two specified times are considered to be equal */
  CFE_TIME_A_GT_B  =  1       /**< \brief The first specified time is considered to be after the second specified time */
} CFE_TIME_Compare_t;

/*
** Type definition (time portion the ES Reset data structure)
*/
/**
**  \brief Time related variables that are maintained through a Processor Reset
**
**  \par Description
**       The #CFE_TIME_ResetVars_t data structure contains those variables that are maintained
**       in an area of memory that is not cleared during a Processor Reset.  This allows the
**       cFE Time Service to maintain time to the best of its ability after a Processor Reset.
*/
typedef struct
{
  uint32 Signature;                     /**< \brief Data validation signature used to verify data structure contents*/
  int16  LeapSeconds;                   /**< \brief Leap seconds value */
  uint16 ClockSignal;                   /**< \brief Current clock signal selection */
  CFE_TIME_SysTime_t CurrentMET;        /**< \brief Current Mission Elapsed Time (MET) */
  CFE_TIME_SysTime_t CurrentSTCF;       /**< \brief Current Spacecraft Time Correlation Factor (STCF) */
  CFE_TIME_SysTime_t CurrentDelay;      /**< \brief Current time client delay value */

} CFE_TIME_ResetVars_t;

/**
**   \brief Time Synchronization Callback Function Ptr Type
**
**   \par Description
**        Applications that wish to get direct notification of the receipt of the cFE Time Synchronization signal
**        (typically a 1 Hz signal), must register a callback function with the following prototype via the
**        #CFE_TIME_RegisterSynchCallback API.
*/
typedef int32 (*CFE_TIME_SynchCallbackPtr_t)(void);

/*
** Function prototypes (get time)...
*/
/*****************************************************************************/
/**
** \brief Get the current spacecraft time
**
** \par Description
**        This routine returns the current spacecraft time.  The time returned 
**        is either TAI (no leap seconds) or UTC (including leap seconds).  This choice
**        is made in the mission configuration file by defining either #CFE_TIME_CFG_DEFAULT_TAI
**        or #CFE_TIME_CFG_DEFAULT_UTC as TRUE at compile time.  To maintain re-usability 
**        across missions, most applications should be using this function 
**        (or #CFE_TIME_GetTime) rather than the specific routines for getting UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc  The current spacecraft time   \endcode
** \endreturns
**
** \sa #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET, 
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_GetTime(void);   /* returns default time (see default time selection above) */

/*****************************************************************************/
/**
** \brief Get the current TAI time
**
** \par Description
**        This routine returns the current TAI time to the caller.  TAI is an 
**        international time standard that does not include leap seconds.  
**        This routine should only be used in situations where TAI is absolutely 
**        required.  Applications that call #CFE_TIME_GetTAI() may not be portable 
**        to all missions.  Maintenance of correct TAI in flight is not guaranteed 
**        under all mission operations scenarios.  To maintain re-usability across 
**        missions, most applications should be using #CFE_TIME_GetTime(), rather 
**        than the specific routines for getting UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          -# The "TAI" time returned is referenced to the mission-defined time epoch, 
**             which may or may not be the same as the standard TAI epoch.
**          -# Even though TAI does not include leap seconds, the time returned by this 
**             function can still jump forward or backward without warning when the 
**             spacecraft clock is set or adjusted by operators.  Applications using 
**             this function must be able to handle these time discontinuities gracefully.
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc  The current TAI time   \endcode
** \endreturns
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetUTC, #CFE_TIME_GetMET, 
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_GetTAI(void);    /* returns time computed as TAI (MET + STCF) */

/*****************************************************************************/
/**
** \brief Get the current UTC time
**
** \par Description
**        This routine returns the current UTC time to the caller.  This routine 
**        should only be used in situations where UTC is absolutely required.  
**        Applications that call #CFE_TIME_GetUTC() may not be portable to all 
**        missions.  Maintenance of correct UTC in flight is not guaranteed under 
**        all mission operations scenarios.  If UTC is maintained in flight, it will 
**        jump backwards occasionally due to leap second adjustments.  To maintain 
**        re-usability across missions, most applications should be using 
**        #CFE_TIME_GetTime(), rather than the specific routines for getting 
**        UTC/TAI directly.
**
** \par Assumptions, External Events, and Notes:
**          Note: The "UTC" time returned is referenced to the mission-defined time epoch, 
**                which may or may not be the same as the standard UTC epoch.
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc The current UTC time   \endcode            
** \endreturns
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetMET, 
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs
**
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_GetUTC(void);    /* returns time computed as UTC (MET + STCF - Leap Seconds) */

/*****************************************************************************/
/**
** \brief Convert specified MET into Spacecraft Time
**
** \par Description
**        This function returns Spacecraft Time given MET.  Note that Spacecraft 
**        Time is returned as either UTC or TAI depeneding on whether the mission
**        configuration parameter #CFE_TIME_CFG_DEFAULT_UTC or #CFE_TIME_CFG_DEFAULT_TAI
**        was set to TRUE at compile time.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] METTime      The MET to be converted.
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc  Spacecraft Time (UTC or TAI) corresponding to the specified MET   \endcode
** \endreturns
**
** \sa #CFE_TIME_GetMET, #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs,
**     #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs, #CFE_TIME_CFE2FSSeconds, #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
CFE_TIME_SysTime_t CFE_TIME_MET2SCTime (CFE_TIME_SysTime_t METTime); /* returns given MET time as the default spacecraft time */

/*
** Function prototypes (get parts of time)...
*/

/*****************************************************************************/
/**
** \brief Get the current value of the spacecraft time correction factor (STCF).
**
** \par Description
**        This routine returns the current value of the spacecraft time correction 
**        factor.  This is the delta time between the MET and the TAI time.  
**        Applications cannot set or adjust the STCF; that can only be done 
**        through ground commands.  However, science applications may want to 
**        include the STCF in their data products to aid in time correlation 
**        during downstream science data processing. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc  The current STCF    \endcode
** \endreturns
**
** \sa #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**               
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_GetSTCF(void);   /* returns STCF (does not include leap seconds calculation) */

/*****************************************************************************/
/**
** \brief Get the current value of the Mission Elapsed Time (MET).
**
** \par Description
**        This routine returns the current mission-elapsed time (MET).  MET is 
**        usually derived from a hardware-based clock that is not adjusted 
**        during normal operations.  Callers of this routine should not assume 
**        that the MET return value has any specific relationship to any 
**        ground-based time standard. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_TIME_SysTime_t  \retdesc  The current MET    \endcode
** \endreturns
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, 
**     #CFE_TIME_GetMETseconds, #CFE_TIME_GetMETsubsecs, #CFE_TIME_MET2SCTime
**
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_GetMET(void);    /* returns MET (both seconds and sub-seconds) */


/*****************************************************************************/
/**
** \brief Get the current seconds count of the mission-elapsed time.
**
** \par Description
**        This routine is the same as #CFE_TIME_GetMET(), except that it 
**        returns only the integer seconds portion of the MET time. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retstmt The current MET seconds   \endstmt             
** \endreturns
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET, 
**     #CFE_TIME_GetMETsubsecs, #CFE_TIME_MET2SCTime
**
******************************************************************************/
uint32  CFE_TIME_GetMETseconds(void);         /* returns MET "seconds" portion of current time */

/*****************************************************************************/
/**
** \brief Get the current sub-seconds count of the mission-elapsed time.
**
** \par Description
**        This routine is the same as #CFE_TIME_GetMET(), except that it 
**        returns only the integer sub-seconds portion of the MET time.
**        Each count is equal to 2^(-32) seconds. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retstmt The current MET sub-seconds  \endstmt              
** \endreturns
**
** \sa #CFE_TIME_GetTime, #CFE_TIME_GetTAI, #CFE_TIME_GetUTC, #CFE_TIME_GetMET, 
**     #CFE_TIME_GetMETseconds, #CFE_TIME_MET2SCTime
**
******************************************************************************/
uint32  CFE_TIME_GetMETsubsecs(void);         /* returns MET "sub-seconds" portion of current time */

/*****************************************************************************/
/**
** \brief Get the current value of the leap seconds counter.
**
** \par Description
**        This routine returns the current value of the leap seconds counter.  
**        This is the delta seconds between international atomic time (TAI) 
**        and universal coordinated time (UTC).  Applications cannot set or 
**        adjust the leap seconds; that can only be done through ground commands.  
**        However, science applications may want to include the leap seconds 
**        counter in their data products to aid in time correlation during 
**        downstream science data processing.  Note that some mission operations 
**        teams do not maintain the leap seconds count, preferring to adjust the 
**        STCF instead.  Users of this function should check with their mission 
**        ops team to see how they are planning to handle leap seconds. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retstmt The current leap seconds.   \endstmt
** \endreturns
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetClockState, #CFE_TIME_GetClockInfo
**               
******************************************************************************/
int16   CFE_TIME_GetLeapSeconds(void);        /* returns Leap Seconds portion of current time */

/*
** Function prototypes (get time status)...
*/

/*****************************************************************************/
/**
** \brief Get the current state of the spacecraft clock.
**
** \par Description
**        This routine returns the spacecraft clock state.  Applications that 
**        are highly dependent on valid time may want to call this routine 
**        before taking actions based on the times returned by the various 
**        clock routines 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_TIME_ClockState_t \retdesc The current spacecraft clock state    \endcode
** \endreturns
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockInfo
**               
******************************************************************************/
CFE_TIME_ClockState_t  CFE_TIME_GetClockState(void);   /* returns current spacecraft clock state */

/*****************************************************************************/
/**
** \brief Provides information about the spacecraft clock.
**
** \par Description
**        This routine returns information on the spacecraft clock in a bit mask. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retstmt Spacecraft clock information.  To extract the information from the
**         returned value, the following masks can be used as in the following: <br>
**         <tt> if ((ReturnValue & CFE_TIME_FLAG_xxxxxx) == CFE_TIME_FLAG_xxxxxx)</tt> then 
**            the following definition of the \c CFE_TIME_FLAG_xxxxxx is true. <br>  \endstmt
** \retcode  #CFE_TIME_FLAG_CLKSET \retdesc \copydoc CFE_TIME_FLAG_CLKSET \endcode
** \retcode  #CFE_TIME_FLAG_FLYING \retdesc \copydoc CFE_TIME_FLAG_FLYING \endcode
** \retcode  #CFE_TIME_FLAG_SRCINT \retdesc \copydoc CFE_TIME_FLAG_SRCINT \endcode
** \retcode  #CFE_TIME_FLAG_SIGPRI \retdesc \copydoc CFE_TIME_FLAG_SIGPRI \endcode
** \retcode  #CFE_TIME_FLAG_SRVFLY \retdesc \copydoc CFE_TIME_FLAG_SRVFLY \endcode
** \retcode  #CFE_TIME_FLAG_CMDFLY \retdesc \copydoc CFE_TIME_FLAG_CMDFLY \endcode
** \retcode  #CFE_TIME_FLAG_ADDADJ \retdesc \copydoc CFE_TIME_FLAG_ADDADJ \endcode
** \retcode  #CFE_TIME_FLAG_ADD1HZ \retdesc \copydoc CFE_TIME_FLAG_ADD1HZ \endcode
** \retcode  #CFE_TIME_FLAG_ADDTCL \retdesc \copydoc CFE_TIME_FLAG_ADDTCL \endcode
** \retcode  #CFE_TIME_FLAG_SERVER \retdesc \copydoc CFE_TIME_FLAG_SERVER \endcode
** \retcode  #CFE_TIME_FLAG_GDTONE \retdesc \copydoc CFE_TIME_FLAG_GDTONE \endcode
** \retcode  #CFE_TIME_FLAG_UNUSED \retdesc \copydoc CFE_TIME_FLAG_UNUSED \endcode    
** \endreturns
**
** \sa #CFE_TIME_GetSTCF, #CFE_TIME_GetLeapSeconds, #CFE_TIME_GetClockState
**               
******************************************************************************/
uint16 CFE_TIME_GetClockInfo(void); /* returns clock information */


/*
** Function prototypes (add/subtract/compare time)...
*/

/*****************************************************************************/
/**
** \brief Adds two time values
**
** \par Description
**        This routine adds the two specified times and returns the result.  
**        Normally, at least one of the input times should be a value representing 
**        a delta time.  Adding two absolute times together will not cause an error, 
**        but the result will probably be meaningless. 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Time1   The first time to be added.
**
** \param[in] Time2   The second time to be added.
**
** \returns
** \retstmt The sum of the two times, in the #CFE_TIME_SysTime_t format described above.  
**         If the sum is greater than the maximum value that can be stored in a 
**         #CFE_TIME_SysTime_t, the result will roll over (this is not considered an error).  \endstmt
** \endreturns
**
** \sa #CFE_TIME_Subtract, #CFE_TIME_Compare
**                
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_Add(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);       /* (Time1 + Time2) */

/*****************************************************************************/
/**
** \brief Subtracts two time values
**
** \par Description
**        This routine subtracts time2 from time1 and returns the result.  The 
**        time values can represent either absolute or delta times, but not all 
**        combinations make sense.
**           -  AbsTime - AbsTime = DeltaTime
**           -  AbsTime - DeltaTime = AbsTime
**           -  DeltaTime - DeltaTime = DeltaTime
**           -  DeltaTime - AbsTime = garbage
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] Time1   The base time.
**
** \param[in] Time2   The time to be subtracted from the base time.
**
** \returns
** \retstmt The result of subtracting the two times, in the #CFE_TIME_SysTime_t 
**         format.  If the subtraction results in an underflow, the result will 
**         roll over (this is not considered an error).    \endstmt            
** \endreturns
**
** \sa #CFE_TIME_Add, #CFE_TIME_Compare
**                
******************************************************************************/
CFE_TIME_SysTime_t  CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2);  /* (Time1 - Time2) */

/*****************************************************************************/
/**
** \brief Compares two time values
**
** \par Description
**        This routine compares two time values to see which is "greater".  It 
**        is important that applications use this function rather than trying 
**        to directly compare the component pieces of times.  This function will 
**        handle roll-over cases seamlessly, which may not be intuitively obvious.
**        The cFE's internal representation of time "rolls over" when the 32 bit 
**        seconds count reaches 0xFFFFFFFF.  Also, subtracting a delta time from 
**        an absolute time close to the epoch could result in "roll under".  The 
**        strange cases that result from these situations can be handled by defining 
**        the comparison function for times as follows:
**        Plot the two times on the circumference of a circle where 0 is at the 
**        top and 0x80000000 is at the bottom.  If the shortest arc from time A 
**        to time B runs clockwise around the circle, then time A is less than 
**        time B.  If the shortest arc from A to B runs counter-clockwise, then 
**        time A is greater than time B.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TimeA   The first time to compare.
**
** \param[in] TimeB   The second time to compare.
**
** \returns
** \retstmt The result of comparing the two times, one of: \endstmt
** \retcode #CFE_TIME_EQUAL  \retdesc \copydoc CFE_TIME_EQUAL  \endcode
** \retcode #CFE_TIME_A_GT_B \retdesc \copydoc CFE_TIME_A_GT_B \endcode
** \retcode #CFE_TIME_A_LT_B \retdesc \copydoc CFE_TIME_A_LT_B \endcode             
** \endreturns
**
** \sa #CFE_TIME_Add, #CFE_TIME_Subtract
**                
******************************************************************************/
CFE_TIME_Compare_t  CFE_TIME_Compare(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB);   /* (TimeA <=> TimeB) */

/*
** Function prototypes (convert sub-seconds)...
*/

/*****************************************************************************/
/**
** \brief Converts a sub-seconds count to an equivalent number of microseconds
**
** \par Description
**        This routine converts from a sub-seconds count 
**        (each tick is 1 / 2^32 seconds) to microseconds (each tick is 1e-06 seconds).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] SubSeconds   The sub-seconds count to convert.
**
** \returns
** \retstmt The equivalent number of microseconds.    \endstmt            
** \endreturns
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Micro2SubSecs, 
**     #CFE_TIME_CFE2FSSeconds, #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
uint32  CFE_TIME_Sub2MicroSecs(uint32 SubSeconds);     /* convert sub-seconds (1/2^32) to micro-seconds (1/1000000) */

/*****************************************************************************/
/**
** \brief Converts a number of microseconds to an equivalent sub-seconds count.
**
** \par Description
**        This routine converts from microseconds (each tick is 1e-06 seconds) 
**        to a subseconds count (each tick is 1 / 2^32 seconds).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] MicroSeconds   The sub-seconds count to convert.
**
** \returns
** \retstmt The equivalent number of subseconds.  If the number of microseconds 
**         passed in is greater than one second, (i.e. > 999,999), the return 
**         value is equal to \c 0xffffffff.     \endstmt           
** \endreturns
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Sub2MicroSecs, 
**     #CFE_TIME_CFE2FSSeconds, #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
uint32  CFE_TIME_Micro2SubSecs(uint32 MicroSeconds);   /* convert micro-seconds (1/1000000) to sub-seconds (1/2^32) */

/*
** Function prototypes (convert cFE and file system time formats)...
*/

/*****************************************************************************/
/**
** \brief Converts cFE seconds into the File System's seconds
**
** \par Description
**        File systems use specific time epochs for their time tagging of files.
**        Since spacecraft systems rarely use an epoch that matches a particular
**        file system, this function provides a mechanism to translate a given
**        spacecraft time (in seconds) to the file system's time.  The conversion
**        is controlled by the configuration parameter #CFE_TIME_FS_FACTOR which
**        is set equal to the number of seconds between the spacecraft's epoch and
**        the file system's epoch.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] SecondsCFE   The spacecraft time, in seconds, to be converted.
**
** \returns
** \retstmt The equivalent time, in seconds, for the file system.    \endstmt            
** \endreturns
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs, 
**     #CFE_TIME_FS2CFESeconds
**
******************************************************************************/
uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE);

/*****************************************************************************/
/**
** \brief Converts a file system's seconds into cFE seconds
**
** \par Description
**        File systems use specific time epochs for their time tagging of files.
**        Since spacecraft systems rarely use an epoch that matches a particular
**        file system, this function provides a mechanism to translate a file
**        system time (in seconds) into the spacecraft time (in seconds).  The 
**        conversion is controlled by the configuration parameter #CFE_TIME_FS_FACTOR 
**        which is set equal to the number of seconds between the spacecraft's epoch and
**        the file system's epoch.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] SecondsFS   The file system time, in seconds, to be converted.
**
** \returns
** \retstmt The equivalent time, in seconds, for the spacecraft.    \endstmt
** \endreturns
**
** \sa #CFE_TIME_MET2SCTime, #CFE_TIME_Sub2MicroSecs, #CFE_TIME_Micro2SubSecs, 
**     #CFE_TIME_CFE2FSSeconds
**
******************************************************************************/
uint32 CFE_TIME_FS2CFESeconds(uint32 SecondsFS);

/*
** Function prototypes (convert time to string)...
*/

/*****************************************************************************/
/**
** \brief Print a time value as a string
**
** \par Description
**        This routine prints the specified time to the specified string buffer 
**        in the following format: <br> <br>
**           \c yyyy-ddd-hh:mm:ss.xxxxx\\0 <br> <br>
**        where:
**           - \c yyyy = year
**           - \c ddd = Julian day of the year
**           - \c hh = hour of the day (0 to 23)
**           - \c mm = minute (0 to 59)
**           - \c ss = second (0 to 59)
**           - \c xxxxx = subsecond formatted as a decimal fraction (1/4 second = 0.25000)
**           - \c \\0 = trailing null
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  PrintBuffer   Pointer to a character array of at least 
**                           #CFE_TIME_PRINTED_STRING_SIZE characters in length
**
** \param[in]  TimeToPrint   The time to print into the character array.
**
** \param[out] *PrintBuffer  The time as a character string as described above.
**
** \sa
**                
******************************************************************************/
void CFE_TIME_Print(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint);

/*
** Function prototypes (1hz tone signal processor)...
*/

/*****************************************************************************/
/**
** \brief Provides the 1 Hz signal from an external source
**
** \par Description
**        This routine provides a method for cFE TIME software to be notified 
**        of the occurance of the 1Hz tone signal without knowledge of the 
**        specific hardware design.  Regardless of the source of the tone, 
**        this routine should be called as soon as possible after detection 
**        to allow cFE TIME software the opportunity to latch the local clock 
**        as close as possible to the instant of the tone.
**
** \par Assumptions, External Events, and Notes:
**          - This routine may be called directly from within the context of an
**            interrupt handler.
**
** \sa #CFE_TIME_ExternalMET, #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**                
******************************************************************************/
void  CFE_TIME_ExternalTone(void); /* OK to call from ISR */

/*
** Function prototypes (external time source)...
**
** If a Time Server has been configured to accept external time
**    data, then one of the following functions will be enabled.
**
** If the Time Server has also been commanded to use the external
**    time data (as opposed to getting time data from a local MET)
**    then the Time Server will use the external data in computing
**    time.
**
** However, regardless whether the external time data is accepted
**    by the Time Server, these functions also act as the signal to
**    create and distribute the "time at the tone" command packet.
*/


/*****************************************************************************/
/**
** \brief Provides the Mission Elapsed Time from an external source
**
** \par Description
**        This routine provides a method to provide cFE TIME with MET acquired 
**        from an external source.  There is a presumption that this function 
**        will be called at the appropriate time (relative to the tone) such 
**        that this call may be used by cFE TIME as the signal to generate the 
**        "time at the tone" data command.  The "time at the tone" data command 
**        must arrive within the configuration parameter specified window for 
**        tone signal and data packet verification. <br> <br>
**        The MET value at the tone "should" have zero subseconds.  Although the 
**        interface accepts non-zero values for sub-seconds, it may be harmful 
**        to other applications that expect zero subseconds at the moment of the 
**        tone.  Any decision to use non-zero subseconds should be carefully considered.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration 
**            parameters are set to TRUE.  The first is #CFE_TIME_CFG_SERVER which defines 
**            this instantiation of cFE TIME as a time server (not a client).  The 
**            second required configuration parameter is #CFE_TIME_CFG_SOURCE which 
**            enables time source selection commands to the cFE TIME task, and further 
**            enables configuration definitions for the selected type of external time 
**            data.  The third configuration parameter required for this routine is 
**            #CFE_TIME_CFG_SRC_MET, which indicates that the external time data consists 
**            of MET.
**
** \param[in]  NewMET   The MET value at the next (or previous) 1 Hz tone signal.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalGPS, #CFE_TIME_ExternalTime
**                
******************************************************************************/
void CFE_TIME_ExternalMET(CFE_TIME_SysTime_t NewMET);


/*****************************************************************************/
/**
** \brief Provide the time from an external source that has data common to GPS receivers.
**
** \par Description
**        This routine provides a method to provide cFE TIME with current time 
**        data acquired from an external source.  There is a presumption that 
**        this function will be called at the appropriate time (relative to the 
**        tone) such that this call may be used by cFE TIME as the signal to 
**        generate the "time at the tone" data command.  The "time at the tone" 
**        data command must arrive within the configuration parameter specified 
**        window for tone signal and data packet verification. <br> <br>
**        Internally, cFE TIME will calculate a new STCF as the difference between 
**        this new time value and the spacecraft MET value at the tone.  This allows 
**        cFE TIME to always calculate time as the sum of MET and STCF.  The value 
**        of STCF will change only as much as the drift factor between spacecraft 
**        MET and the external time source.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration 
**            parameters are set to TRUE.  The first is #CFE_TIME_CFG_SERVER which defines this 
**            instantiation of cFE TIME as a time server (not a client).  The second 
**            required configuration parameter is #CFE_TIME_CFG_SOURCE which enables 
**            time source selection commands to the cFE TIME task, and further enables 
**            configuration definitions for the selected type of external time data.  
**            The third configuration parameter required for this routine is 
**            #CFE_TIME_CFG_SRC_GPS, which indicates that the external time data consists 
**            of a time value relative to a known epoch, plus a leap seconds value.
**
** \param[in]  NewTime   The MET value at the next (or previous) 1 Hz tone signal.
**
** \param[in]  NewLeaps The Leap Seconds value used to calculate time as UTC.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalMET, #CFE_TIME_ExternalTime
**                
******************************************************************************/
void CFE_TIME_ExternalGPS(CFE_TIME_SysTime_t NewTime, int16 NewLeaps);



/*****************************************************************************/
/**
** \brief Provide the time from an external source that measures time relative to a known epoch.
**
** \par Description
**        This routine provides a method to provide cFE TIME with current time 
**        data acquired from an external source.  There is a presumption that 
**        this function will be called at the appropriate time (relative to the 
**        tone) such that this call may be used by cFE TIME as the signal to 
**        generate the "time at the tone" data command.  The "time at the tone" 
**        data command must arrive within the configuration specified window for 
**        tone signal and data packet verification. <br> <br>
**        Internally, cFE TIME will calculate a new STCF as the difference between 
**        this new time value and the spacecraft MET value at the tone.  This allows 
**        cFE TIME to always calculate time as the sum of MET and STCF.  The value 
**        of STCF will change only as much as the drift factor between spacecraft 
**        MET and the external time source.
**
** \par Assumptions, External Events, and Notes:
**          - This routine is included in the API only when 3 specific configuration 
**            parameters are set to TRUE.  The first is #CFE_TIME_CFG_SERVER which defines this 
**            instanciation of cFE TIME as a time server (not a client).  The second 
**            required configuration parameter is #CFE_TIME_CFG_SOURCE which enables 
**            time source selection commands to the cFE TIME task, and further enables 
**            configuration definitions for the selected type of external time data.  
**            The third configuration parameter required for this routine is 
**            #CFE_TIME_CFG_SRC_TIME, which indicates that the external time data consists 
**            of a time value relative to a known epoch.
**
** \param[in]  NewTime   The MET value at the next (or previous) 1 Hz tone signal.
**
** \sa #CFE_TIME_ExternalTone, #CFE_TIME_ExternalMET, #CFE_TIME_ExternalGPS
**                
******************************************************************************/
void CFE_TIME_ExternalTime(CFE_TIME_SysTime_t NewTime);

/*****************************************************************************/
/**
** \brief Registers a callback function that is called whenever time synchronization occurs
**
** \par Description
**        This routine passes a callback function pointer for an Application that wishes to
**        be notified whenever a legitimate time synchronization signal (typically a 1 Hz)
**        is received.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS  \endcode
** \retcode #CFE_TIME_TOO_MANY_SYNCH_CALLBACKS \retdesc \copydoc CFE_TIME_TOO_MANY_SYNCH_CALLBACKS \endcode
** \retcode #CFE_ES_ERR_APPID  \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \endreturns
**
** \sa #CFE_TIME_UnregisterSynchCallback
**
******************************************************************************/
int32  CFE_TIME_RegisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);   


/*****************************************************************************/
/**
** \brief Unregisters a callback function that is called whenever time synchronization occurs
**
** \par Description
**        This routine removes the specified callback function pointer from the list
**        of Callback functions that are called whenever a time synchronization (typically
**        the 1Hz signal) is received.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \returns
** \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS  \endcode
** \retcode #CFE_TIME_CALLBACK_NOT_REGISTERED \retdesc \copydoc CFE_TIME_CALLBACK_NOT_REGISTERED \endcode
** \retcode #CFE_ES_ERR_APPID  \retdesc \copydoc CFE_ES_ERR_APPID   \endcode
** \endreturns
**
** \sa #CFE_TIME_RegisterSynchCallback
**
******************************************************************************/
int32  CFE_TIME_UnregisterSynchCallback(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr);   


/*****************************************************************************/
/**
** \brief This function should be called from the system PSP layer once per second
**
** \par Description
**        Drives the time processing logic from the system PSP layer.  This must be called
**        once per second based on a hardware interrupt or OS kernel signal.
**
** \par Assumptions, External Events, and Notes:
**        This will update the global data structures accordingly, incrementing each
**        by the 1Hz amount.
**
**
******************************************************************************/
void CFE_TIME_Local1HzISR(void);


#endif /* _cfe_time_ */

/************************/
/*  End of File Comment */
/************************/
