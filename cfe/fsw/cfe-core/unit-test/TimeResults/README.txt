The cFE Time Service (TIME) unit tests were run in the following pc-linux environment:

Distributor ID: RedHatEnterpriseClient
Release:        5.11

Linux gs582w-cfelnx 2.6.18-407.el5PAE #1 SMP Fri Oct 16 12:08:56 EDT 2015 
i686 i686 i386 GNU/Linux i386

Unit tests built with gcc (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)

The expected TIME unit test pass/fail test results are located in the ut_cfe_time_log.txt file

The TIME unit test results are expected to have the following coverage results for each of 
the TIME source files:

gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/time/cfe_time_api.c' 97.99%  199
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/time/cfe_time_task.c' 100.00%  257
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/time/cfe_time_tone.c' 100.00%  137
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/time/cfe_time_utils.c' 100.00%  275

==========================================================================

cfe_time_api.c - 97.99% coverage (4 lines not executed)

In CFE_TIME_CFE2FSSeconds:

        -:  694:uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE)
        3:  695:{
        -:  696:    /*
        -:  697:    ** Using a signed integer allows the factor to be negative...
        -:  698:    */
        3:  699:    int32 ConvertFactor = CFE_TIME_FS_FACTOR;
        -:  700:
        -:  701:    /*
        -:  702:    ** File system time = cFE time + conversion factor...
        -:  703:    */
        3:  704:    uint32 SecondsFS = SecondsCFE + (uint32) ConvertFactor;
        -:  705:
        -:  706:    /*
        -:  707:    ** Prevent file system time from going below zero...
        -:  708:    */
        3:  709:    if (ConvertFactor < 0)
        -:  710:    {
    #####:  711:        if (-ConvertFactor > SecondsCFE)
        -:  712:        {
    #####:  713:            SecondsFS = 0;
        -:  714:        }
        -:  715:    }
        -:  716:
        3:  717:    return(SecondsFS);
        -:  718:
        -:  719:} /* End of CFE_TIME_CFE2FSSeconds() */

REASON: These lines are contingent on a platform configuration setting and it is only set one way for the unit test.

In CFE_TIME_Print:

        -:  783:    /*
        -:  784:    ** Convert the cFE time (offset from epoch) into calendar time...
        -:  785:    */
       10:  786:    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_TIME_EPOCH_MINUTE;
       10:  787:    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_TIME_EPOCH_SECOND;
        -:  788:
        -:  789:    /*
        -:  790:    ** Adding the epoch "seconds" after computing the minutes avoids
        -:  791:    **    overflow problems when the input time value (seconds) is
        -:  792:    **    at, or near, 0xFFFFFFFF...
        -:  793:    */
       20:  794:    while (NumberOfSeconds >= 60)
        -:  795:    {
    #####:  796:        NumberOfMinutes++;
    #####:  797:        NumberOfSeconds -= 60;
        -:  798:    }

REASON: These lines are contingent on a platform configuration setting and it is only set one way for the unit test.

==========================================================================

cfe_time_task.c - 100.00 % coverage

==========================================================================

cfe_time_tone.c - 100.00% coverage

==========================================================================

cfe_time_utils.c - 100.00% coverage

==========================================================================
