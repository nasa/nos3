/*
** File: utf_test12.c
**
** Purpose: This test makes calls to OS time api.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2008/09/08 08:57:08GMT-05:00 $
** $Revision: 1.4 $
** $Log: utf_test12.c  $
** Revision 1.4 2008/09/08 08:57:08GMT-05:00 wfmoleski 
** Tests for the CFE_TIME function hooks and UTF return code capabilities were added.
** Revision 1.3 2008/09/04 15:19:15EDT wfmoleski 
** Source, output and makefile updates for the OS_Timer API tests.
** Revision 1.2 2008/08/20 15:08:09EDT wfmoleski 
** Added the include of utf_cfe_time.h to utf_cfe.h and added the utf_cfe_time_api.o specification to each
** example's makefile
** Revision 1.1 2008/04/17 08:09:01EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test12/project.pj
** Revision 1.3 2006/08/01 08:38:59EDT wfmoleski 
** Changes made to upgrade this example to cFE 3.3.0
** Revision 1.2 2006/06/20 11:28:11EDT wfmoleski 
** Added calls that tested limit changes made to the fsw code.
** Revision 1.1 2006/04/10 16:32:00EDT sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test12/project.pj
**
*/

#include "cfe.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_osapi.h"
#include "utf_cfe_time.h"
#include "utf_cfe_time_utils.h"
#include "utf_ostimer.h"

void timerCallback (uint32 timerID)
{
	UTF_put_text("UTF Timer Callback called - ID = %d\n",timerID);
}

/* Function hooks */
uint32 CFE_TIME_GetMETsecondsHook(void)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(Count);
}

uint32 CFE_TIME_GetMETsubsecondsHook(void)
{
    static uint32 Count = 0;
    return(Count++);
}

int32 CFE_TIME_RegisterSynchCallbackHook(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TIME_TOO_MANY_SYNCH_CALLBACKS);
}

int32 CFE_TIME_UnregisterSynchCallbackHook(CFE_TIME_SynchCallbackPtr_t CallbackFuncPtr)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(CFE_SUCCESS);
    else
        return(CFE_TIME_CALLBACK_NOT_REGISTERED);
}

int main(void)
{
    /* most of the functions tested here are using the actual flight code, they
       are tested to make sure the simulated time stuff works */
    CFE_TIME_SysTime_t  system_time;
    CFE_TIME_SysTime_t  time1;
    CFE_TIME_SysTime_t  time2;
    CFE_TIME_SysTime_t  spacecraft_time;
    char                print_buffer[128];
    
	/* Set up output file */
	UTF_set_output_filename("utf_test12.ActualOutput");
    
    /* the simulated time is set as a floating point number based on seconds "." fraction 
       of a second.  So setting a time of 1.0625 = 1 second and 62500 microseconds, or 1 
       second and 1 subsecond (16 subseconds per second so 1/16 = .0625).  for this test
       we are setting the initial time to 1 and a half seconds or 1 second 500000us */
    UTF_put_text("Set the initial time to 1 and a half seconds or 1 second 500000us\n");
    UTF_init_sim_time(1.5);

    /* read back the current MET which should be 1 second and ? subseconds */
    UTF_put_text("CFE_TIME_GetMETseconds Return Value %x\n", CFE_TIME_GetMETseconds());
    UTF_put_text("CFE_TIME_GetMETsubsecs Return Value %x\n", CFE_TIME_GetMETsubsecs());
    system_time = CFE_TIME_GetMET();
    UTF_put_text("CFE_TIME_GetMET Return Value, Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);
    CFE_TIME_Print((char *)&print_buffer, system_time);
    UTF_put_text("CFE_TIME_Print Return Value %s\n", print_buffer);
    
    /* test STCF */
    system_time = CFE_TIME_GetSTCF();
    UTF_put_text("CFE_TIME_GetSTCF Default Return Value, (should be 0, 0), Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

    UTF_put_text("Set STCF to 2 seconds and 3 subseconds\n");
    system_time.Seconds = 2;
    system_time.Subseconds = 0x30000000;    
    UTF_set_stcf(system_time);
    system_time = CFE_TIME_GetSTCF();
    UTF_put_text("CFE_TIME_GetSTCF New Return Value, (should be 2, 3), Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);
   
    /* test leap seconds */ 
    UTF_put_text("CFE_TIME_GetLeapSeconds Default Return Value, (should be 0), %x\n", CFE_TIME_GetLeapSeconds());

    UTF_put_text("Set Leap Seconds to 1\n");
    UTF_set_leap_seconds(1);
    UTF_put_text("CFE_TIME_GetLeapSeconds New Return Value, (should be 1), %x\n", CFE_TIME_GetLeapSeconds());

    /* test time formats */
    system_time = CFE_TIME_GetTime();
    UTF_put_text("CFE_TIME_GetTime Return Value, (should be same as TAI), Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

    system_time = CFE_TIME_GetTAI();
    UTF_put_text("CFE_TIME_GetTAI Return Value, TAI = MET + STCF, Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

    system_time = CFE_TIME_GetUTC();
    UTF_put_text("CFE_TIME_GetUTC Return Value, UTC = MET + STCF - Leap Seconds, Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

    /* test clock state function */
    UTF_put_text("CFE_TIME_GetClockState Return Value, (should be CFE_TIME_VALID = 0), %x\n", CFE_TIME_GetClockState());
    
    time1.Seconds = 3;
    time1.Subseconds = 4;
    time2.Seconds = 1;
    time2.Subseconds = 2;
    
    /* test time add */
    system_time = CFE_TIME_Add(time1, time2);
    UTF_put_text("CFE_TIME_Add Return Value, (should be 4, 6), Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);
    
    /* test time subtract */
    system_time = CFE_TIME_Subtract(time1, time2);
    UTF_put_text("CFE_TIME_Subtract Return Value, (should be 2, 2), Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

    /* test time compare */
    UTF_put_text("CFE_TIME_Compare Return Value, (should be CFE_TIME_A_GT_B = 1), %x\n", CFE_TIME_Compare(time1, time2));
    
    /* test subseconds to microseconds */
    UTF_put_text("CFE_TIME_Sub2MicroSecs Return Value, (should be 62500), %d\n", CFE_TIME_Sub2MicroSecs(0x10000000));

    /* test microseconds to subseconds */
    UTF_put_text("CFE_TIME_Micro2SubSecs Return Value, (should be 0x10000000), %x\n", CFE_TIME_Micro2SubSecs(62500));

    /* test cFE seconds to File System seconds */
    UTF_put_text("CFE_TIME_CFE2FSSeconds Return Value, (should be time + CFE_TIME_FS_FACTOR = 789004802), %d\n", CFE_TIME_CFE2FSSeconds(2));

    /* test File System seconds to cFE seconds */
    UTF_put_text("CFE_TIME_FS2CFESeconds Return Value, (should be time - CFE_TIME_FS_FACTOR = 2) %x\n", CFE_TIME_FS2CFESeconds(789004802));
    
    /* test call external tone function */
    UTF_put_text("CFE_TIME_ExternalTone - does not do anything\n");
    CFE_TIME_ExternalTone(); 

	/********************************************************************
	 *  These calls are a result of the cFE 3.3 changes to cfe_time_api.c
	 ********************************************************************/
    /* test subseconds to microseconds MAX limit */
    UTF_put_text("CFE_TIME_Sub2MicroSecs(0xFFFFDF00) Return Value, (should be 999998), %d\n", CFE_TIME_Sub2MicroSecs(0xFFFFDF00));

    /* test microseconds to subseconds MAX limit */
    UTF_put_text("CFE_TIME_Micro2SubSecs(999999) Return Value, (should be 0xFFFFF000), %x\n", CFE_TIME_Micro2SubSecs(999999));
    
    /* test subseconds to microseconds MAX Limit + 1 */
    UTF_put_text("CFE_TIME_Sub2MicroSecs(0xFFFFE000) Return Value, (should be 999999), %d\n", CFE_TIME_Sub2MicroSecs(0xFFFFE000));

    /* test microseconds to subseconds Max limit + 1 */
    UTF_put_text("CFE_TIME_Micro2SubSecs(1000000) Return Value, (should be 0xFFFFFFFF), %x\n", CFE_TIME_Micro2SubSecs(1000000));

	/* test new MET2SCTime function */
    system_time = CFE_TIME_GetMET();
    UTF_put_text("CFE_TIME_GetMET Return Value, Seconds %x, Subseconds %x\n", system_time.Seconds, system_time.Subseconds);

	spacecraft_time = CFE_TIME_MET2SCTime(system_time);
    UTF_put_text("CFE_TIME_MET2SCTime Return Value, Seconds %x, Subseconds %x\n", spacecraft_time.Seconds, spacecraft_time.Subseconds);

	/* cFE 5.2.0 Tests */
	/* Test the GetClockInfo API */
    UTF_put_text("CFE_TIME_GetClockInfo() Return Value = %x\n", CFE_TIME_GetClockInfo());
	UTF_CFE_TIME_Set_Api_Return_Code(CFE_TIME_GETCLOCKINFO_PROC,0xFF);
    UTF_put_text("CFE_TIME_GetClockInfo() Return Value = %x\n", CFE_TIME_GetClockInfo());
	UTF_CFE_TIME_Use_Default_Api_Return_Code(CFE_TIME_GETCLOCKINFO_PROC);
    UTF_put_text("CFE_TIME_GetClockInfo() Return Value = %x\n", CFE_TIME_GetClockInfo());

	/* Test the RegisterSynchCallback API */
    UTF_put_text("CFE_TIME_RegisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_RegisterSynchCallback((void *)timerCallback));
	UTF_CFE_TIME_Set_Api_Return_Code(CFE_TIME_REGISTERSYNCHCALLBACK_PROC,0xEE);
    UTF_put_text("CFE_TIME_RegisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_RegisterSynchCallback((void *)timerCallback));
	UTF_CFE_TIME_Use_Default_Api_Return_Code(CFE_TIME_REGISTERSYNCHCALLBACK_PROC);
    UTF_put_text("CFE_TIME_RegisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_RegisterSynchCallback((void *)timerCallback));

	/* Test the UnregisterSynchCallback API */
    UTF_put_text("CFE_TIME_UnregisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_UnregisterSynchCallback((void *)timerCallback));
	UTF_CFE_TIME_Set_Api_Return_Code(CFE_TIME_UNREGISTERSYNCHCALLBACK_PROC,0xDD);
    UTF_put_text("CFE_TIME_UnregisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_UnregisterSynchCallback((void *)timerCallback));
	UTF_CFE_TIME_Use_Default_Api_Return_Code(CFE_TIME_UNREGISTERSYNCHCALLBACK_PROC);
    UTF_put_text("CFE_TIME_UnregisterSynchCallback() Return Value = %x\n",
    		CFE_TIME_RegisterSynchCallback((void *)timerCallback));

	/* OS_Timer API tests */
	UTF_put_text("\nOS_Timer API Tests\n");
	int32 retStatus;
	uint32 timer_id1, timer_id2, retTimer_id, clock_accuracy;
	OS_timer_prop_t timerInfo;
	
	UTF_OSTIMER_Set_ClockAccuracy(20);
	OS_TimerAPIInit();
	
	/* Test the OS_TimerCreate API */
	retStatus = OS_TimerCreate(&timer_id1,"Walts Timer 1",&clock_accuracy, timerCallback);
    UTF_put_text("OS_TimerCreate() Return Value = %x\n", retStatus);
    UTF_put_text("ID = %d; accuracy = %d\n", timer_id1, clock_accuracy);
    UTF_put_text("OS_TimerGetInfo() Return Value = %x\n",OS_TimerGetInfo(timer_id1,&timerInfo));
    UTF_put_text("Info Values returned:\n");
    UTF_put_text("    Creator  = %d\n",timerInfo.creator);
    UTF_put_text("    Name     = %s\n",timerInfo.name);
    UTF_put_text("    Start    = %d\n",timerInfo.start_time);
    UTF_put_text("    Interval = %d\n",timerInfo.interval_time);
    UTF_put_text("    Accuracy = %d\n",timerInfo.accuracy);
	UTF_OSTIMER_Set_Api_Return_Code(OS_TIMER_CREATE_PROC,0xFF);
	retStatus = OS_TimerCreate(&timer_id1,"Walts Timer 1",&clock_accuracy, timerCallback);
    UTF_put_text("OS_TimerCreate() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Use_Default_Api_Return_Code(OS_TIMER_CREATE_PROC);
	retStatus = OS_TimerCreate(&timer_id2,"Walts Timer 2",&clock_accuracy, timerCallback);
    UTF_put_text("OS_TimerCreate() Return Value = %x\n", retStatus);
    UTF_put_text("ID = %d; accuracy = %d\n", timer_id2, clock_accuracy);
	
	/* Test the OS_TimerSet API */
	retStatus = OS_TimerSet(timer_id1, 10, 1);
    UTF_put_text("OS_TimerSet() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Set_Api_Return_Code(OS_TIMER_SET_PROC,0xEA);
	retStatus = OS_TimerSet(timer_id1, 10, 1);
    UTF_put_text("OS_TimerSet() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Use_Default_Api_Return_Code(OS_TIMER_SET_PROC);
	retStatus = OS_TimerSet(timer_id2, 10, 1);
    UTF_put_text("OS_TimerSet() Return Value = %x\n", retStatus);

	/* Test the OS_TimerDelete API */
	retStatus = OS_TimerDelete(timer_id1);
    UTF_put_text("OS_TimerDelete() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Set_Api_Return_Code(OS_TIMER_DELETE_PROC,0xFFEA);
	retStatus = OS_TimerDelete(timer_id1);
    UTF_put_text("OS_TimerDelete() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Use_Default_Api_Return_Code(OS_TIMER_DELETE_PROC);
	retStatus = OS_TimerDelete(timer_id1);
    UTF_put_text("OS_TimerDelete() Return Value = %x; Expected %x\n", retStatus,OS_ERR_INVALID_ID);

	/* Test the OS_TimerGetIdByName API */
	retStatus = OS_TimerGetIdByName(&retTimer_id, "Walts Timer 1");
    UTF_put_text("OS_TimerGetIdByName(Walts Timer 1) Return Value = %x; Expected %x\n", retStatus,OS_ERR_NAME_NOT_FOUND);
	UTF_OSTIMER_Set_Api_Return_Code(OS_TIMER_GETIDBYNAME_PROC,0xEAFF);
	retStatus = OS_TimerGetIdByName(&retTimer_id, "Walts Timer 1");
    UTF_put_text("OS_TimerGetIdByName() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Use_Default_Api_Return_Code(OS_TIMER_GETIDBYNAME_PROC);
	retStatus = OS_TimerGetIdByName(&retTimer_id, "Walts Timer 2");
    UTF_put_text("OS_TimerGetIdByName(Walts Timer 2) Return Value = %x\n", retStatus);
    UTF_put_text("Returned ID = %d\n", retTimer_id);

	/* Test the OS_TimerGetInfo API */
	retStatus = OS_TimerGetInfo(timer_id2,&timerInfo);
    UTF_put_text("OS_TimerGetInfo() Return Value = %x\n", retStatus);
    UTF_put_text("Info Values returned:\n");
    UTF_put_text("    Creator  = %d\n",timerInfo.creator);
    UTF_put_text("    Name     = %s\n",timerInfo.name);
    UTF_put_text("    Start    = %d\n",timerInfo.start_time);
    UTF_put_text("    Interval = %d\n",timerInfo.interval_time);
    UTF_put_text("    Accuracy = %d\n",timerInfo.accuracy);
	UTF_OSTIMER_Set_Api_Return_Code(OS_TIMER_GETINFO_PROC,0xFFFF);
	retStatus = OS_TimerGetInfo(timer_id2,&timerInfo);
    UTF_put_text("OS_TimerGetInfo() Return Value = %x\n", retStatus);
	UTF_OSTIMER_Use_Default_Api_Return_Code(OS_TIMER_GETINFO_PROC);
	retStatus = OS_TimerGetInfo(timer_id2,&timerInfo);
    UTF_put_text("OS_TimerGetInfo() Return Value = %x\n", retStatus);
    UTF_put_text("Info Values returned:\n");
    UTF_put_text("    Creator  = %d\n",timerInfo.creator);
    UTF_put_text("    Name     = %s\n",timerInfo.name);
    UTF_put_text("    Start    = %d\n",timerInfo.start_time);
    UTF_put_text("    Interval = %d\n",timerInfo.interval_time);
    UTF_put_text("    Accuracy = %d\n",timerInfo.accuracy);
    
    /* Test function hook capability */
    UTF_TIME_set_function_hook(CFE_TIME_GETMETSECONDS_HOOK, (void *)&CFE_TIME_GetMETsecondsHook);
    UTF_put_text("GetMETseconds Hook Return Code %x\n", CFE_TIME_GetMETseconds());  
    UTF_put_text("GetMETseconds Hook Return Code %x\n", CFE_TIME_GetMETseconds());  
    UTF_put_text("GetMETseconds Hook Return Code %x\n", CFE_TIME_GetMETseconds());  
    UTF_put_text("GetMETseconds Hook Return Code %x\n", CFE_TIME_GetMETseconds());  

    UTF_TIME_set_function_hook(CFE_TIME_GETMETSUBSECS_HOOK, (void *)&CFE_TIME_GetMETsubsecondsHook);
    UTF_put_text("GetMETsubseconds Hook Return Code %x\n", CFE_TIME_GetMETsubsecs());  
    UTF_put_text("GetMETsubseconds Hook Return Code %x\n", CFE_TIME_GetMETsubsecs());  
    UTF_put_text("GetMETsubseconds Hook Return Code %x\n", CFE_TIME_GetMETsubsecs());  
    UTF_put_text("GetMETsubseconds Hook Return Code %x\n", CFE_TIME_GetMETsubsecs());  

    UTF_TIME_set_function_hook(CFE_TIME_REGISTERSYNCHCALLBACK_HOOK, (void *)&CFE_TIME_RegisterSynchCallbackHook);
    UTF_put_text("RegisterSyncCallback Hook Return Code %x\n", CFE_TIME_RegisterSynchCallback((void *)timerCallback));  
    UTF_put_text("RegisterSyncCallback Hook Return Code %x\n", CFE_TIME_RegisterSynchCallback((void *)timerCallback));  
    UTF_put_text("RegisterSyncCallback Hook Return Code %x\n", CFE_TIME_RegisterSynchCallback((void *)timerCallback));  
    UTF_put_text("RegisterSyncCallback Hook Return Code %x\n", CFE_TIME_RegisterSynchCallback((void *)timerCallback));  

    UTF_TIME_set_function_hook(CFE_TIME_UNREGISTERSYNCHCALLBACK_HOOK, (void *)&CFE_TIME_UnregisterSynchCallbackHook);
    UTF_put_text("UnregisterSyncCallback Hook Return Code %x\n", CFE_TIME_UnregisterSynchCallback((void *)timerCallback));  
    UTF_put_text("UnregisterSyncCallback Hook Return Code %x\n", CFE_TIME_UnregisterSynchCallback((void *)timerCallback));  
    UTF_put_text("UnregisterSyncCallback Hook Return Code %x\n", CFE_TIME_UnregisterSynchCallback((void *)timerCallback));  
    UTF_put_text("UnregisterSyncCallback Hook Return Code %x\n", CFE_TIME_UnregisterSynchCallback((void *)timerCallback));  

	exit(0);
}
