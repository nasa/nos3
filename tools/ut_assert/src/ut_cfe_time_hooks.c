/*
**
** File: ut_cfe_time_hooks.c
**
** $Id: ut_cfe_time_hooks.c 1.1 2016/02/08 20:53:31EST rperera Exp  $
**
** Copyright 2012-2013 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Other Rights Reserved.  
**
** This software was created at NASA's Goddard Space Flight Center.
** This software is governed by the NASA Open Source Agreement and may be 
** used, distributed and modified only pursuant to the terms of that 
** agreement.
**
** Purpose: Unit test hooks for cFE Time routines
**
** $Log: ut_cfe_time_hooks.c  $
** Revision 1.1 2016/02/08 20:53:31EST rperera 
** Initial revision
** Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/src/project.pj
** Revision 1.3 2015/06/16 16:14:03EDT sstrege 
** Added copyright information
** Revision 1.2 2015/05/01 14:13:34EDT czogby 
** Added Ut_CFE_TIME_PrintHook
** Revision 1.1 2015/04/03 17:41:51EDT czogby 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "cfe_time_utils.h"

CFE_TIME_SysTime_t Ut_CFE_TIME_AddHook (CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds + Time2.Subseconds;

    /*
    ** Check for sub-seconds roll-over
    */
    if (Result.Subseconds < Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds + Time2.Seconds) + 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds + Time2.Seconds;
    }

    return(Result);
}

CFE_TIME_SysTime_t Ut_CFE_TIME_SubtractHook (CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t Result;

    Result.Subseconds = Time1.Subseconds - Time2.Subseconds;

    if (Result.Subseconds > Time1.Subseconds)
    {
        Result.Seconds = (Time1.Seconds - Time2.Seconds) - 1;
    }
    else
    {
        Result.Seconds = Time1.Seconds - Time2.Seconds;
    }

    return(Result);
}

uint32 Ut_CFE_TIME_Sub2MicroSecsHook (uint32 SubSeconds)
{
    uint32 MicroSeconds;
    
    /* 0xffffdf00 subseconds = 999999 microseconds, so anything greater 
     * than that we set to 999999 microseconds, so it doesn't get to
     * a million microseconds */
    
    if (SubSeconds > 0xffffdf00)
    {
            MicroSeconds = 999999;
    }
    else
    {
        /*
        **  Convert a 1/2^32 clock tick count to a microseconds count
        **
        **  Conversion factor is  ( ( 2 ** -32 ) / ( 10 ** -6 ) ).
        **
        **  Logic is as follows:
        **    x * ( ( 2 ** -32 ) / ( 10 ** -6 ) )
        **  = x * ( ( 10 ** 6  ) / (  2 ** 32 ) )
        **  = x * ( ( 5 ** 6 ) ( 2 ** 6 ) / ( 2 ** 26 ) ( 2 ** 6) )
        **  = x * ( ( 5 ** 6 ) / ( 2 ** 26 ) )
        **  = x * ( ( 5 ** 3 ) ( 5 ** 3 ) / ( 2 ** 7 ) ( 2 ** 7 ) (2 ** 12) )
        **
        **  C code equivalent:
        **  = ( ( ( ( ( x >> 7) * 125) >> 7) * 125) >> 12 )
        */   

        MicroSeconds = (((((SubSeconds >> 7) * 125) >> 7) * 125) >> 12);
    

        /* if the Subseconds % 0x4000000 != 0 then we will need to
         * add 1 to the result. the & is a faster way of doing the % */  
        if ((SubSeconds & 0x3ffffff) != 0)
        {
            MicroSeconds++;
        }
    
        /* In the Micro2SubSecs conversion, we added an extra anomaly
         * to get the subseconds to bump up against the end point,
         * 0xFFFFF000. This must be accounted for here. Since we bumped
         * at the half way mark, we must "unbump" at the same mark 
         */
        if (MicroSeconds > 500000)
        {
            MicroSeconds --;
        }
        
    } /* end else */
    
    return(MicroSeconds);
}

uint32 Ut_CFE_TIME_Micro2SubSecsHook (uint32 MicroSeconds)
{
    uint32 SubSeconds;

    /*
    ** Conversion amount must be less than one second
    */
    if (MicroSeconds > 999999)
    {
        SubSeconds = 0xFFFFFFFF;
    }
    else
    {
    /*
    **  Convert micro-seconds count to sub-seconds (1/2^32) count
    **
    **  Conversion factor is  ( ( 10 ** -6 ) / ( 2 ** -20 ).
    **
    **  Logic is as follows:
    **    x * ( ( 10 ** -6 ) / ( 2 ** -32 ) )
    **  = x * ( ( 2 ** 32 ) / ( 10 ** 6 ) )
    **  = x * ( ( ( 2 ** 26 ) ( 2 ** 6) ) / ( ( 5 ** 6 ) ( 2 ** 6 ) ) )
    **  = x * ( ( 2 ** 26 ) / ( 5 ** 6 ) )
    **  = x * ( ( ( 2 ** 11) ( 2 ** 3) (2 ** 12) ) / ( 5( 5 ** 5 ) ) )
    **  = x * ( ( ( ( ( 2 ** 11 ) / 5 ) * ( 2 ** 3 ) ) / ( 5 ** 5 ) ) * (2 ** 12) )
    **
    **  C code equivalent:
    **  = ( ( ( ( ( x << 11 ) / 5 ) << 3 ) / 3125 ) << 12 )
    **
    **  Conversion factor was reduced and factored accordingly
    **  to minimize precision loss and register overflow.
    */
        SubSeconds = ( ( ( ( MicroSeconds << 11 ) / 5 ) << 3 ) / 3125 ) << 12;

        /* To get the SubSeconds to "bump up" against 0xFFFFF000 when 
         * MicroSeconds = 9999999, we add in another anomaly to the 
         * conversion at the half-way point  (500000 us). This will bump
         * all of the subseconds up by 0x1000, so 999999 us == 0xFFFFF00,
         * 999998 == 0xFFFFE000, etc. This extra anomaly is accounted for
         * in the Sub2MicroSecs conversion as well.
         */
        
        if (SubSeconds > 0x80001000)
        {
           SubSeconds += 0x1000;
        }

    }

    return(SubSeconds);
}

void Ut_CFE_TIME_PrintHook(char *PrintBuffer, CFE_TIME_SysTime_t TimeToPrint)
{
    uint32 NumberOfYears;
    uint32 NumberOfDays;
    uint32 NumberOfHours;
    uint32 NumberOfMinutes;
    uint32 NumberOfSeconds;
    uint32 NumberOfMicros;
    uint32 DaysInThisYear;

    boolean StillCountingYears = TRUE;

    /*
    ** Convert the cFE time (offset from epoch) into calendar time...
    */
    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_TIME_EPOCH_MINUTE;
    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_TIME_EPOCH_SECOND;

    /*
    ** Adding the epoch "seconds" after computing the minutes avoids
    **    overflow problems when the input time value (seconds) is
    **    at, or near, 0xFFFFFFFF...
    */
    while (NumberOfSeconds >= 60)
    {
        NumberOfMinutes++;
        NumberOfSeconds -= 60;
    }

    /*
    ** Compute the years/days/hours/minutes...
    */
    NumberOfHours   = (NumberOfMinutes / 60) + CFE_TIME_EPOCH_HOUR;
    NumberOfMinutes = (NumberOfMinutes % 60);

    /*
    ** Unlike hours and minutes, epoch days are counted as Jan 1 = day 1...
    */
    NumberOfDays  = (NumberOfHours / 24) + (CFE_TIME_EPOCH_DAY - 1);
    NumberOfHours = (NumberOfHours % 24);

    NumberOfYears = CFE_TIME_EPOCH_YEAR;

    /*
    ** Convert total number of days into years and remainder days...
    */
    while (StillCountingYears)
    {
        /*
        ** Set number of days in this year (leap year?)...
        */
        DaysInThisYear = 365;

        if ((NumberOfYears % 4) == 0)
        {
            if ((NumberOfYears % 100) != 0)
            {
                DaysInThisYear = 366;
            }
            else if ((NumberOfYears % 400) == 0)
            {
                DaysInThisYear = 366;
            }
        }

        /*
        ** When we have less than a years worth of days, we're done...
        */
        if (NumberOfDays < DaysInThisYear)
        {
            StillCountingYears = FALSE;
        }
        else
        {
            /*
            ** Add a year and remove the number of days in that year...
            */
            NumberOfYears++;
            NumberOfDays -= DaysInThisYear;
        }
    }

    /*
    ** Unlike hours and minutes, days are displayed as Jan 1 = day 1...
    */
    NumberOfDays++;

    /*
    ** After computing microseconds, convert to 5 digits from 6 digits...
    */
    NumberOfMicros = CFE_TIME_Sub2MicroSecs(TimeToPrint.Subseconds) / 10;

    /*
    ** Build formatted output string (yyyy-ddd-hh:mm:ss.xxxxx)...
    */
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 1000); NumberOfYears = NumberOfYears % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 100);  NumberOfYears = NumberOfYears % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfYears / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfYears % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfDays / 100); NumberOfDays = NumberOfDays % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfDays / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfDays % 10);
    *PrintBuffer++ = '-';

    *PrintBuffer++ = '0' + (char) (NumberOfHours / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfHours % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfMinutes / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMinutes % 10);
    *PrintBuffer++ = ':';

    *PrintBuffer++ = '0' + (char) (NumberOfSeconds / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfSeconds % 10);
    *PrintBuffer++ = '.';

    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10000); NumberOfMicros = NumberOfMicros % 10000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 1000);  NumberOfMicros = NumberOfMicros % 1000;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 100);   NumberOfMicros = NumberOfMicros % 100;
    *PrintBuffer++ = '0' + (char) (NumberOfMicros / 10);
    *PrintBuffer++ = '0' + (char) (NumberOfMicros % 10);
    *PrintBuffer++ = '\0';

    return;
}

CFE_TIME_Compare_t  Ut_CFE_TIME_CompareHook(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    CFE_TIME_Compare_t Result;

    if (TimeA.Seconds > TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeA.Seconds - TimeB.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_A_GT_B;
        }
    }
    else if (TimeA.Seconds < TimeB.Seconds)
    {
        /*
        ** Assume rollover if difference is too large...
        */
        if ((TimeB.Seconds - TimeA.Seconds) > CFE_TIME_NEGATIVE)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else
        {
            Result = CFE_TIME_A_LT_B;
        }
    }
    else
    {
        /*
        ** Seconds are equal, check sub-seconds
        */
        if (TimeA.Subseconds > TimeB.Subseconds)
        {
            Result = CFE_TIME_A_GT_B;
        }
        else if (TimeA.Subseconds < TimeB.Subseconds)
        {
            Result = CFE_TIME_A_LT_B;
        }
        else
        {
            Result = CFE_TIME_EQUAL;
        }
    }

    return(Result);
}
