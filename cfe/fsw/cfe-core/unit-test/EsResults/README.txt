The ES Services unit test results are expected to have the
following results for each of the ES services source files:

ut_cfe_es PASSED 364 tests.
ut_cfe_es FAILED 10 tests.

FAILED [cFE.ES.01.001] CFE_ES_Main - Normal startup - Power on - Power cycle
FAILED [cFE.ES.01.002] CFE_ES_Main - Normal startup - Power on - Special command
FAILED [cFE.ES.01.003] CFE_ES_Main - Normal startup - Power on - other
FAILED [cFE.ES.02.001] CFE_ES_Main - Mutex create failure
FAILED [cFE.ES.02.002] CFE_ES_Main - Semaphore create failure
FAILED [cFE.ES.02.016] CFE_ES_CreateObjects - Record used error
FAILED [cFE.ES.02.017] CFE_ES_CreateObjects - Error returned when calling function
FAILED [cFE.ES.02.018] CFE_ES_CreateObjects - Error creating core application
FAILED [cFE.ES.02.019] CFE_ES_CreateObjects - No free application slots available
FAILED [cFE.ES.02.020] CFE_ES_CreateObjects - Bad function pointer


The CFE_ES_WaitForStartupSync API function was updated in the cFE 6.4.2 release to use a 
simpler polling loop (with delay) rather than a semaphore.  The unit tests were not updated 
to accommodate  this change resulting in the 10 failures reported above.  A DCR/Trac Ticket 
has been submitted and these errors will be corrected in a subsequent release.

gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_api.c' 98.99%  495
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_apps.c' 100.00%  439
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_cds.c' 100.00%  194
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_cds_mempool.c' 100.00%  213
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_erlog.c' 100.00%  27
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_esmempool.c' 100.00%  204
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_perf.c' 100.00%  154
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_shell.c' 100.00%  136
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_start.c' 100.00%  216
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/es/cfe_es_task.c' 99.35%  617

==========================================================================

cfe_es_api.c - 98.99% coverage (5 Lines of Code NOT executed)

In CFE_ES_ExitApp:

        -:  504:       else /* It is an external App */
        -:  505:       {
        -:  506:
    #####:  507:          CFE_ES_WriteToSysLog ("Application %s called CFE_ES_ExitApp\n",
        -:  508:                                 CFE_ES_Global.AppTable[AppID].StartParams.Name);
    #####:  509:          CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
        -:  510:
        -:  511:
        -:  512:          /*
        -:  513:          ** Unlock the ES Shared data before suspending the app
        -:  514:          */
    #####:  515:          CFE_ES_UnlockSharedData(__func__,__LINE__);
        -:  516:
        -:  517:          /*
        -:  518:          ** Suspend the Application until ES kills it.
        -:  519:          ** It might be better to have a way of suspending the app in the OS
        -:  520:          */
        -:  521:          while(1)
        -:  522:          {
    #####:  523:             OS_TaskDelay(500);
    #####:  524:          }
        -:  525:
        -:  526:       } /* end if */

REASON: Unit testing this branch is not possible because of the need for the
        infinite loop.

==========================================================================
cfe_es_apps.c - 100% coverage

==========================================================================
cfe_es_cds.c - 100% coverage

==========================================================================
cfe_es_cds_mempool.c - 100% coverage

==========================================================================
cfe_es_erlog.c - 100% coverage

==========================================================================
cfe_esmempool.c - 100% coverage

==========================================================================
cfe_es_perf.c - 100% coverage

==========================================================================
cfe_es_shell.c - 100% coverage

==========================================================================
cfe_es_start.c - 100% coverage

==========================================================================

cfe_es_task.c - 99.35%

In CFE_ES_StartAppCmd:

        5:  849:       else if (cmd->AppEntryPoint == NULL)
        -:  850:       {
    #####:  851:          CFE_ES_TaskData.ErrCounter++;
    #####:  852:
CFE_EVS_SendEvent(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID, CFE_EVS_ERROR,
        -:  853:                           "CFE_ES_StartAppCmd: App Entry
Point is NULL.");
        -:  854:       }
        5:  855:       else if (cmd->Application == NULL)
        -:  856:       {
    #####:  857:          CFE_ES_TaskData.ErrCounter++;
    #####:  858:
CFE_EVS_SendEvent(CFE_ES_START_NULL_APP_NAME_ERR_EID, CFE_EVS_ERROR,
        -:  859:                           "CFE_ES_StartAppCmd: App Name is
NULL.");
        -:  860:       }

REASON: The structure used in the unit tests containing the command cannot have
        a null value, so these two checks against null cannot be satisfied.
        A future build will fix the command parameter validation to allow 
        100% on this file.

==========================================================================
