The cFE Table Service (TBL) unit tests were run in the following pc-linux environment:

Distributor ID: RedHatEnterpriseClient
Release:        5.11

Linux gs582w-cfelnx 2.6.18-407.el5PAE #1 SMP Fri Oct 16 12:08:56 EDT 2015 
i686 i686 i386 GNU/Linux i386

Unit tests built with gcc (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)

The expected TBL unit test pass/fail test results are located in the ut_cfe_tbl_log.txt file

The TBL unit test results are expected to have the following coverage results for each of 
the TBL source files:

gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/tbl/cfe_tbl_api.c' 99.20%  499
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/tbl/cfe_tbl_internal.c' 100.00%  438
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task_cmds.c' 99.76%  419
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task.c' 100.00%  93

==========================================================================

cfe_tbl_api.c - 99.01% coverage (4 Lines of Code NOT executed)

In CFE_TBL_Register:

        4:  400:                            Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, TRUE);
        -:  401:
        4:  402:                            if (Status != CFE_SUCCESS)
        -:  403:                            {
        -:  404:                                /* Unable to get a working buffer - this error is not really */
        -:  405:                                /* possible at this point during table registration.  But we */
        -:  406:                                /* do need to handle the error case because if the function */
        -:  407:                                /* call did fail, WorkingBufferPtr would be a NULL pointer. */
    #####:  408:                                CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
    #####:  409:                                CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to get work buffer for '%s.%s' (ErrCode=0x%08X)\n",
        -:  410:                                                     AppName, Name, (unsigned int)Status);
        -:  411:                            }
        
REASON: See comment on lines 404-407        

In CFE_TBL_Update:

       5: 1028:    if (Status != CFE_TBL_ERR_BAD_APP_ID)
        -: 1029:    {
        -: 1030:        /* Translate AppID of caller into App Name */
        4: 1031:        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
        -: 1032:    }
        -: 1033:    
        -: 1034:    /* On Error conditions, notify ground of screw up */
        5: 1035:    if (Status < 0)
        -: 1036:    {
        2: 1037:        if (RegRecPtr != NULL)
        -: 1038:        {
    #####: 1039:            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID,
        -: 1040:                                       CFE_EVS_ERROR,
        -: 1041:                                       CFE_TBL_TaskData.TableTaskAppId,
        -: 1042:                                       "%s Failed to Update '%s', Status=0x%08X",
        -: 1043:                                       AppName, RegRecPtr->Name, (unsigned int)Status);
        -: 1044:        }

REASON: This event message is never issued in unit testing because in order for it to
occur, the function CFE_TBL_UpdateInternal would be required to return an error
code.  At the current time, the CFE_TBL_UpdateInternal function only returns
either CFE_SUCCESS or informational status. The event message is being
maintained in the event the CFE_TBL_UpdateInternal function is modified to
return an error.

In CFE_TBL_Manage:

        -: 1403:            /* Validate the specified Table */
       10: 1404:            Status = CFE_TBL_Validate(TblHandle);
        -: 1405:
       10: 1406:            if (Status != CFE_SUCCESS)
        -: 1407:            {
        -: 1408:                /* If an error occurred during Validate, then do not perform any more managing */
    #####: 1409:                FinishedManaging = TRUE;
        -: 1410:            }

REASON: The FinishedManaging variable can currently never be set because the only error
conditions that CFE_TBL_Validate can return are already captured by the
validation performed in the CFE_TBL_GetStatus function.

==========================================================================

cfe_tbl_internal.c - 100% coverage

==========================================================================

cfe_tbl_task_cmds.c - 99.76% coverage (1 Line of Code NOT executed)

In CFE_TBL_LoadCmd:

        2:  538:                        else if (Status == CFE_TBL_ERR_NO_BUFFER_AVAIL)
        -:  539:                        {
        2:  540:                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID,
        -:  541:                                              CFE_EVS_ERROR,
        -:  542:                                              "No working buffers available for table '%s'",
        -:  543:                                              TblFileHeader.TableName);
        -:  544:                        }
        -:  545:                        else
        -:  546:                        {
    #####:  547:                            CFE_EVS_SendEvent(CFE_TBL_INTERNAL_ERROR_ERR_EID,
        -:  548:                                              CFE_EVS_ERROR,
        -:  549:                                              "Internal Error (Status=0x%08X)",
        -:  550:                                              (unsigned int)Status);
        -:  551:                        }

REASON: This event message is only issued when an unexpected error is detected in
underlying function calls.  In the current implementation, there isn't any
possibility of an unexpected error to occur.

==========================================================================

cfe_tbl_task.c - 100% coverage

==========================================================================
