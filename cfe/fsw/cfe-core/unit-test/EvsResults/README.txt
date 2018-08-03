The cFE Event Service (EVS) unit tests were run in the following pc-linux environment:

Distributor ID: RedHatEnterpriseClient
Release:        5.11

Linux gs582w-cfelnx 2.6.18-407.el5PAE #1 SMP Fri Oct 16 12:08:56 EDT 2015 
i686 i686 i386 GNU/Linux i386

Unit tests built with gcc (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)

The expected EVS unit test pass/fail test results are located in the ut_cfe_evs_log.txt file

The EVS unit test results are expected to have the following coverage results for each of 
the EVS source files:

gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/evs/cfe_evs.c' 100.00%  119
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/evs/cfe_evs_log.c' 100.00%  63
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/evs/cfe_evs_task.c' 99.61%  517
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/evs/cfe_evs_utils.c' 100.00%  123


==========================================================================

cfe_evs.c - 100.00 % coverage

==========================================================================

cfe_evs_log.c - 100.00% coverage

==========================================================================
cfe_evs_task.c - 99.61% % coverage (2 Lines of Code NOT executed)

The lines not covered are in CFE_EVS_TaskInit:

        -:  385:   /* Register EVS task for event services */
        5:  386:   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
        5:  387:   if (Status != CFE_SUCCESS)
        -:  388:   {
    #####:  389:      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
    #####:  390:      return Status;
        -:  391:   }

REASON: None - A DCR was entered to get full coverage of this file.

==========================================================================
cfe_evs_utils.c - 100.00% coverage

==========================================================================
