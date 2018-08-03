/*
** File: utf_test17.c
**
** Purpose: This test makes calls to event service api.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2008/04/17 07:09:08GMT-05:00 $
** $Revision: 1.1 $
** $Log: utf_test17.c  $
** Revision 1.1 2008/04/17 07:09:08GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test17/project.pj
** Revision 1.2 2007/10/16 09:46:34EDT wfmoleski 
** Added the Event Type and ID to the print statement in the UTF version of CFE_EVS_SendEvent.
** Revision 1.1 2007/06/05 08:37:58EDT wfmoleski 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test17/project.pj
** Revision 1.6 2007/04/12 10:59:10EDT wfmoleski 
** Changes for 3159:1
** Revision 1.5 2006/08/01 08:35:33EDT wfmoleski 
** Changes made during the conversion to cFE 3.3.0
** Revision 1.4 2006/06/20 11:17:58EDT wfmoleski 
** Added tests to the source file to exercise the new DeletePipe functions.
** Revision 1.3 2006/04/10 15:47:39EDT sslegel 
** Updated code to test new hooks
**
*/

#include "cfe.h"
#include "ccsds.h"
#include "cfe_evs.h"
#include "cfe_time.h"
#include "utf_custom.h"
#include "utf_types.h"
#include "utf_cfe.h"
#include "utf_cfe_evs.h"
#include "utf_osfilesys.h"
#include "utf_osapi.h"

CFE_EVS_BinFilter_t  EventFilters[5];

int main(void)
{
	EventFilters[0].EventID = 0;
    EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
	EventFilters[1].EventID = 1;
    EventFilters[1].Mask    = CFE_EVS_NO_FILTER;
	EventFilters[2].EventID = 2;
    EventFilters[2].Mask    = CFE_EVS_NO_FILTER;
	EventFilters[3].EventID = 3;
    EventFilters[3].Mask    = CFE_EVS_NO_FILTER;
	EventFilters[4].EventID = 4;
    EventFilters[4].Mask    = CFE_EVS_NO_FILTER;

	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("utf_test17.ActualOutput");
    UTF_init_sim_time(1.5);

    /* EVS Register Test */
    UTF_put_text("EVS_Register default Return Code %x\n", CFE_EVS_Register(EventFilters, 5, CFE_EVS_BINARY_FILTER));
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_REGISTER_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_Register Return Code %x\n", CFE_EVS_Register(EventFilters, 5, CFE_EVS_BINARY_FILTER));
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_REGISTER_PROC);
    UTF_put_text("EVS_Register Return Code %x\n", CFE_EVS_Register(EventFilters, 5, CFE_EVS_BINARY_FILTER));
    UTF_put_text("EVS_Register with bad filter scheme Return Code %x\n", CFE_EVS_Register(EventFilters, 5, 2));

    /* EVS UnRegister Test */
    UTF_put_text("EVS_Unregister default Return Code %x\n", CFE_EVS_Unregister());
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_UNREGISTER_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_Unregister Return Code %x\n", CFE_EVS_Unregister());
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_UNREGISTER_PROC);
    UTF_put_text("EVS_Unregister default Return Code %x\n", CFE_EVS_Unregister());

	/* Register this app with EVS */
    CFE_EVS_Register(EventFilters, 5, CFE_EVS_BINARY_FILTER);

	/* Need to Register this APP before sending messages */
	UTF_ES_InitAppRecords();
	UTF_ES_AddAppRecord("Test17",0);	
    CFE_ES_RegisterApp();

    /* EVS Send Event Test */
    UTF_put_text("EVS_SendEvent default Return Code %x\n",
    	CFE_EVS_SendEvent(0,CFE_EVS_INFORMATION,"Test17: Info Event 0"));
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_SENDEVENT_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_SendEvent Return Code %x\n",
    	CFE_EVS_SendEvent(0,CFE_EVS_INFORMATION,"Test17: Info Event 0"));
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_SENDEVENT_PROC);
    UTF_put_text("EVS_SendEvent default Return Code %x\n", 
		CFE_EVS_SendEvent(0,CFE_EVS_INFORMATION,"Test17: Info Event 0"));
	CFE_EVS_SendEvent(1,CFE_EVS_ERROR,"Test17: Error Event 1");
	CFE_EVS_SendEvent(2,CFE_EVS_DEBUG,"Test17: Debug Event 2");
	CFE_EVS_SendEvent(3,CFE_EVS_CRITICAL,"Test17: Critical Event 3");
	CFE_EVS_SendEvent(4,5,"Test17: Invalid Event");

    /* EVS Send Event with AppId Test */
    UTF_put_text("EVS_SendEventWithAppID default Return Code %x\n",
    	CFE_EVS_SendEventWithAppID(1,CFE_EVS_ERROR,0,"Test17: Error Event 1"));
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_SENDEVENTWITHAPPID_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_SendEventWithAppID Return Code %x\n",
    	CFE_EVS_SendEventWithAppID(1,CFE_EVS_ERROR,0,"Test17: Error Event 1"));
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_SENDEVENTWITHAPPID_PROC);
    UTF_put_text("EVS_SendEventWithAppID default Return Code %x\n", 
		CFE_EVS_SendEventWithAppID(1,CFE_EVS_ERROR,0,"Test17: Error Event 1"));

    /* EVS Send Timed Event Test */
    CFE_TIME_SysTime_t myTime = CFE_TIME_GetTime();
    UTF_put_text("EVS_SendTimedEvent default Return Code %x\n",
    	CFE_EVS_SendTimedEvent(myTime,2,CFE_EVS_INFORMATION,"Test17: Info Event 2"));
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_SENDTIMEDEVENT_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_SendTimedEvent Return Code %x\n",
    	CFE_EVS_SendTimedEvent(myTime,2,CFE_EVS_INFORMATION,"Test17: Info Event 2"));
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_SENDTIMEDEVENT_PROC);
    UTF_put_text("EVS_SendTimedEvent default Return Code %x\n", 
		CFE_EVS_SendTimedEvent(myTime,2,CFE_EVS_INFORMATION,"Test17: Info Event 2"));

    /* EVS Reset Filter Test */
    UTF_put_text("EVS_ResetFilter default Return Code %x\n", CFE_EVS_ResetFilter(3));
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_RESETFILTER_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_ResetFilter Return Code %x\n", CFE_EVS_ResetFilter(3));
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_RESETFILTER_PROC);
    UTF_put_text("EVS_ResetFilter default Return Code %x\n", CFE_EVS_ResetFilter(3));

    /* EVS Reset All Filters Test */
    UTF_put_text("EVS_ResetAllFilters default Return Code %x\n", CFE_EVS_ResetAllFilters());
    UTF_CFE_EVS_Set_Api_Return_Code(CFE_EVS_RESETALLFILTERS_PROC, CFE_EVS_APP_NOT_REGISTERED);
    UTF_put_text("EVS_ResetAllFilters Return Code %x\n", CFE_EVS_ResetAllFilters());
    UTF_CFE_EVS_Use_Default_Api_Return_Code(CFE_EVS_RESETALLFILTERS_PROC);
    UTF_put_text("EVS_ResetAllFilters default Return Code %x\n", CFE_EVS_ResetAllFilters());

	exit(0);
}
