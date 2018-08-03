The cFE Software Bus Service (SB) unit tests were run in the following pc-linux environment:

Distributor ID: RedHatEnterpriseClient
Release:        5.11

Linux gs582w-cfelnx 2.6.18-407.el5PAE #1 SMP Fri Oct 16 12:08:56 EDT 2015 
i686 i686 i386 GNU/Linux i386

Unit tests built with gcc (GCC) 4.1.2 20080704 (Red Hat 4.1.2-55)

The expected SB unit test pass/fail test results are located in the ut_cfe_sb_log.txt file

The SB unit test results are expected to have the following coverage results for each of 
the SB source files:

gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/ccsds.c' 100.00%  21
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_api.c' 99.82%  542
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_buf.c' 100.00%  51
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_init.c' 100.00%  45
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_priv.c' 100.00%  142
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_task.c' 99.42%  346
gcov: '/home/sstrege/cFE650/cfe/fsw/cfe-core/src/sb/cfe_sb_util.c' 100.00%  117

==========================================================================

ccsds.c - 100.00% coverage

==========================================================================

cfe_sb_api.c - 99.82% coverage - (1 Line of Code NOT executed)

In CFE_SB_SendMsgFull:

       44: 1276:    for (i=0; i < RtgTblPtr -> Destinations; i++) {
        -: 1277:
        -: 1278:        /* The DestPtr should never be NULL in this loop, this is just extra
        -: 1279:           protection in case of the unforseen */
       22: 1280:        if(DestPtr == NULL){
    #####: 1281:          break;
        -: 1282:        }        
        
REASON: See comment provided on line 1278

==========================================================================

cfe_sb_buf.c - 100% coverage

==========================================================================

cfe_sb_init.c - 100% coverage

==========================================================================

cfe_sb_priv.c - 100% coverage

==========================================================================

cfe_sb_task.c - 99.42% coverage 

The first line of code that is not executed in Unit Testing is in the
CFE_SB_TaskMain function.  The code snippet is as follows:

        -:  137:    /* Main loop */
        5:  138:    while (Status == CFE_SUCCESS)
        -:  139:    {
        -:  140:        /* Increment the Main task Execution Counter */
        1:  141:        CFE_ES_IncrementTaskCounter();
        -:  142:
        1:  143:        CFE_ES_PerfLogExit(CFE_SB_MAIN_PERF_ID);
        -:  144:
        -:  145:        /* Pend on receipt of packet */
        1:  146:        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
        1:  147:                                CFE_SB.CmdPipe,
        -:  148:                                CFE_SB_PEND_FOREVER);
        -:  149:
        1:  150:        CFE_ES_PerfLogEntry(CFE_SB_MAIN_PERF_ID);
        -:  151:
        1:  152:        if(Status == CFE_SUCCESS)
        -:  153:        {
        -:  154:            /* Process cmd pipe msg */
    #####:  155:            CFE_SB_ProcessCmdPipePkt();
        -:  156:        }else{
        1:  157:            CFE_ES_WriteToSysLog("SB:Error reading cmd pipe,RC=0x%08X\n",Status);
        -:  158:        }/* end if */
        -:  159:
        -:  160:    }/* end while */
        -:  161:
        -:  162:    /* while loop exits only if CFE_SB_RcvMsg returns error */
        2:  163:    CFE_ES_ExitApp(CFE_ES_CORE_APP_RUNTIME_ERROR);
        -:  164:
        2:  165:}/* end CFE_SB_TaskMain */

REASON: Have not found a way to execute this line while allowing the unit test
        terminate normally. 

The next line of code cannot be executed is found in CFE_SB_AppInit:

        -:  249:    /* Be sure the number of events to register for filtering
        -:  250:    ** does not exceed CFE_EVS_MAX_EVENT_FILTERS */
       12:  251:    if(CFE_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
    #####:  252:      CfgFileEventsToFilter = CFE_EVS_MAX_EVENT_FILTERS;
        -:  253:    }/* end if */
        
REASON: Cannot be executed without altering EVS or SB config parameters.

==========================================================================

cfe_sb_util.c - 100% coverage

==========================================================================
