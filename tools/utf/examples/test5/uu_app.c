/*
** File: uu_app.c
**
** Subsystem: UU Table using application
**     ( based on cFE Application Template (QQ) Application)
**
** Author:
**
** Notes:
**
** $Log: uu_app.c  $
** Revision 1.1 2008/04/17 07:09:20GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test5/project.pj
** Revision 1.3 2007/06/05 08:24:50EDT wfmoleski 
** Changes to examples and utf source that reflect the cFE 4.1.0 ES subsystem.
** Revision 1.2 2006/08/01 08:50:27EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.1 2006/03/21 16:44:20EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test5/project.pj
** 2005/09/19 nschweiss Initial version based on tt_app.c.
** 2005/09/29 nschweiss Corrected use of TBL calls so that
** when GetAddress returns PENDING status, we call ReleaseAddress,
** Update, and GetAddress again so that we end up with current
** value for table pointer.
**
** 2005/10/05 nschweiss Commented the forever loop back in.
** 2005/10/11 nschweiss Corrected TT_GetData to capture return code
**  for ReleaseAddress call.
** 2005/10/11 nschweiss Renamed TT_GetData -> TT_AccessCurrentData.
** 2005/11/01 nschweiss Adapt to demonstrate presetting return codes.
** 2005/12/09 nschweiss Modified includes.
** 2005/12/20 nschweiss Added UU_DisplayApiValues.
*/


/*
** Required header files...
*/
#include "uu_app.h"

#include <string.h>

#include "cfe_msgids.h"
#include "cfe_mission_cfg.h"
#include "utf_types.h"

/*
** UU global data...
*/
UU_AppData_t UU_AppData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* UU_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UU_AppMain(void)
{
	int32 Status;
	int32 Status2;
	CFE_SB_PipeId_t pipeId;
	char pipeName[10];
	void *TblPointer;
	CFE_TBL_Handle_t TblHandle;
	

    /*
    ** Register application...
    */
    /* CFE_ES_RegisterApp(); */

    /*
    ** Perform application specific initialize 
    ** including access initial table values.
    */
    UU_AppInit();
    TblHandle = 5;
                         
     /*
    ** Main process loop (forever)...
    */
    while (TRUE)
    {
     	    
        /*
        ** Wait for the next Software Bus message...
        */
        Status = CFE_SB_RcvMsg(&UU_AppData.MsgPtr,
                                  UU_AppData.CmdPipe,
                                  4);
                                  
     /*   if (Status == CFE_SB_TIME_OUT) */
        if (1)
        {
        	   UTF_put_text("CFE_ES_ResetCFE returned %lx\n",CFE_ES_ResetCFE(5));
        	
        	   UTF_put_text("CFE_ES_RegisterApp returned %lx\n",CFE_ES_RegisterApp());
        	  /* This one caused a seg fault */
        	   /*UTF_put_text("CFE_ES_GetAppID returned %d\n",CFE_ES_GetAppID((uint32 *)0x3344beef)); */
       	   UTF_put_text("CFE_ES_GetAppID returned %lx\n",CFE_ES_GetAppID(0));
       	   
       	   Status2 = CFE_EVS_SendEvent(UU_INIT_INF_EID,
                      CFE_EVS_INFORMATION,
                     "UU: SendEvent called from UU task's forever loop");
           UTF_put_text("CFE_EVS_SendEvent returned %lx\n", Status2);
/*           UTF_put_text("CFE_FS_ReadHeader returned %lx\n", CFE_FS_ReadHeader(0,0)); */
       	   UTF_put_text("CFE_SB_CreatePipe returned %lx\n", CFE_SB_CreatePipe(&pipeId,1,pipeName));
       	   UTF_put_text("CFE_SB_RcvMsg returned %lx\n", Status);
       	   UTF_put_text(" CFE_TBL_GetAddress returned %lx\n", 
       	                  CFE_TBL_GetAddress( &TblPointer,TblHandle ));

        }
                
    }

} /* End of UU_AppMain() */

void UU_DisplayApiValues(void)
{
	int32 Status2;
	CFE_SB_PipeId_t pipeId;
	char pipeName[10];
	void *TblPointer;
	CFE_TBL_Handle_t TblHandle;
	
    /*
    ** Perform application specific initialize 
    ** including access initial table values.
    */
    UU_AppInit();
    TblHandle = 5;
	
	UTF_put_text("CFE_ES_ResetCFE returned %lx\n",CFE_ES_ResetCFE(5));
        	
    UTF_put_text("CFE_ES_RegisterApp returned %lx\n",CFE_ES_RegisterApp());
    /* This one caused a seg fault */
    /* UTF_put_text("CFE_ES_GetAppID returned %d\n",CFE_ES_GetAppID((uint32 *)0x3344beef)); */
    UTF_put_text("CFE_ES_GetAppID returned %lx\n",CFE_ES_GetAppID(0));
       	   
    Status2 = CFE_EVS_SendEvent(UU_INIT_INF_EID,
                      CFE_EVS_INFORMATION,
                     "UU: SendEvent called from UU_DisplayApiValues()");
    UTF_put_text("CFE_EVS_SendEvent returned %lx\n", Status2);
/*    UTF_put_text("CFE_FS_ReadHeader returned %lx\n", CFE_FS_ReadHeader(0,0)); */
    UTF_put_text("CFE_SB_CreatePipe returned %lx\n", CFE_SB_CreatePipe(&pipeId,1,pipeName));
    
    UTF_put_text("CFE_TBL_GetAddress returned %lx\n", 
       	                  CFE_TBL_GetAddress( &TblPointer,TblHandle ));
	
}

void UU_GetSBMessage(void)
{
	int32 Status;
	UU_AppInit();
	 
	Status = CFE_SB_RcvMsg(&UU_AppData.MsgPtr,
                                  UU_AppData.CmdPipe,
                                  4);
    
    UTF_put_text("CFE_SB_RcvMsg returned %lx\n", Status);
			
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* UU_AppInit() -- UU initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void UU_AppInit(void)
{
	/* void *TblPointer;
	//int32 Status; */
    /*
    ** Initialize app command execution counters...
    */
    UU_AppData.CmdCount = 0;
    UU_AppData.ErrCount = 0;
    UU_AppData.HkPacket.UpdateCount = 0;

    /*
    ** Initialize app configuration data...
    */
    strcpy(UU_AppData.PipeName, "UU_CMD_PIPE");

    UU_AppData.PipeDepth = 12;

    UU_AppData.LimitHK   = 2;
    UU_AppData.LimitCmd  = 4;

    /*
    ** Initialize event filter table...
    */
    UU_AppData.EventFilters[0].EventID = UU_INIT_INF_EID;
    UU_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    UU_AppData.EventFilters[1].EventID = UU_NOOP_INF_EID;
    UU_AppData.EventFilters[1].Mask    = CFE_EVS_NO_FILTER;
    UU_AppData.EventFilters[2].EventID = UU_RESET_INF_EID;
    UU_AppData.EventFilters[2].Mask    = CFE_EVS_NO_FILTER;
    UU_AppData.EventFilters[3].EventID = UU_MID_ERR_EID;
    UU_AppData.EventFilters[3].Mask    = CFE_EVS_NO_FILTER;
    UU_AppData.EventFilters[4].EventID = UU_CC1_ERR_EID;
    UU_AppData.EventFilters[4].Mask    = CFE_EVS_NO_FILTER;
    UU_AppData.EventFilters[5].EventID = UU_LEN_ERR_EID;
    UU_AppData.EventFilters[5].Mask    = CFE_EVS_NO_FILTER;

    /*
    ** Register event filter table...
    */
    CFE_EVS_Register(UU_AppData.EventFilters,
                     UU_EVT_COUNT,
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&UU_AppData.HkPacket,
                    CFE_UU_HK_TLM_MID,
                    sizeof(UU_HkPacket_t), TRUE);

    /*
    ** Create Software Bus message pipe...
    */
    CFE_SB_CreatePipe(&UU_AppData.CmdPipe,
                       UU_AppData.PipeDepth,
                       UU_AppData.PipeName);

    /*
    ** Subscribe to Housekeeping request commands...
    */
    CFE_SB_Subscribe(CFE_UU_SEND_HK_MID,UU_AppData.CmdPipe);

    /*
    ** Subscribe to UU ground command packets...
    */
    CFE_SB_Subscribe(CFE_UU_CMD_MID,UU_AppData.CmdPipe);
    
    /*
    ** Application startup event message...
    */
    CFE_EVS_SendEvent(UU_INIT_INF_EID,
                      CFE_EVS_INFORMATION,
                     "UU: Application Initialized");

    return;

} /* End of UU_AppInit() */

/************************/
/*  End of File Comment */
/************************/

