/*
** File: tt_app.c
**
** Subsystem: TT Table using application
**     ( based on cFE Application Template (QQ) Application)
**
** Author:
**
** Notes:
**
** $Log: tt_app.c  $
** Revision 1.1 2008/04/17 07:09:10GMT-05:00 ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test2/project.pj
** Revision 1.3 2007/02/01 13:18:17EST wfmoleski 
** Needed to add the full path specification to all filenames to make these examples work with cFE 4.0
** Revision 1.2 2006/08/01 08:45:25EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.1 2006/03/21 16:44:13EST sslegel 
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/tools/utf/examples/test2/project.pj
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
** 2005/11/28 nschweiss Modified TT_AccessCurrentData() to use
** new Table Services 3.1 API.
** CFE_SB_SUCCESS -> CFE_SUCCESS.
*/


/*
** Required header files...
*/
#include "tt_app.h"

#include <string.h>

#include "cfe_msgids.h"
#include "utf_types.h"

/*
** TT global data...
*/
TT_AppData_t TT_AppData;

void TT_AccessCurrentData(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_AppMain() -- Application entry point and main process loop   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_AppMain(void)
{
	int32 Status;

    /*
    ** Register application...
    */
    CFE_ES_RegisterApp();

    /*
    ** Perform application specific initialize 
    ** including access initial table values.
    */
    TT_AppInit();

    	/* This step simulates the working buffer being */
	/* loaded in the background.                    */
/*	UTF_put_text("\n>>Loading new data.\n");
//	Status = CFE_TBL_LoadWorkingBuffer(TT_AppData.TblHandle,
 //                CFE_TBL_SRC_FILE,
 //                "/tt_table_values1.dat");
    //UTF_put_text("CFE_TBL_LoadWorkingBuffer returned %lx\n", Status); */
                         
     /*
    ** Main process loop (forever)...
    */
    while (TRUE)
    {
     	    
        /*
        ** Wait for the next Software Bus message...
        */
        Status = CFE_SB_RcvMsg(&TT_AppData.MsgPtr,
                                  TT_AppData.CmdPipe,
                                  4);

        if (Status == CFE_SUCCESS)
        {
            /*
            ** Process Software Bus message...
            */
            TT_AppPipe(TT_AppData.MsgPtr);
        }
        
        /* Access the latest Table Values */
        TT_AccessCurrentData();
        
    }

} /* End of TT_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_AppInit() -- TT initialization                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_AppInit(void)
{
	void *TblPointer;
	int32 Status;
    /*
    ** Initialize app command execution counters...
    */
    TT_AppData.CmdCount = 0;
    TT_AppData.ErrCount = 0;
    TT_AppData.HkPacket.UpdateCount = 0;

    /*
    ** Initialize app configuration data...
    */
    strcpy(TT_AppData.PipeName, "TT_CMD_PIPE");

    TT_AppData.PipeDepth = 12;

    TT_AppData.LimitHK   = 2;
    TT_AppData.LimitCmd  = 4;

    /*
    ** Initialize event filter table...
    */
    TT_AppData.EventFilters[0].EventID = TT_INIT_INF_EID;
    TT_AppData.EventFilters[0].Mask    = CFE_EVS_NO_FILTER;
    TT_AppData.EventFilters[1].EventID = TT_NOOP_INF_EID;
    TT_AppData.EventFilters[1].Mask    = CFE_EVS_NO_FILTER;
    TT_AppData.EventFilters[2].EventID = TT_RESET_INF_EID;
    TT_AppData.EventFilters[2].Mask    = CFE_EVS_NO_FILTER;
    TT_AppData.EventFilters[3].EventID = TT_MID_ERR_EID;
    TT_AppData.EventFilters[3].Mask    = CFE_EVS_NO_FILTER;
    TT_AppData.EventFilters[4].EventID = TT_CC1_ERR_EID;
    TT_AppData.EventFilters[4].Mask    = CFE_EVS_NO_FILTER;
    TT_AppData.EventFilters[5].EventID = TT_LEN_ERR_EID;
    TT_AppData.EventFilters[5].Mask    = CFE_EVS_NO_FILTER;

    /*
    ** Register event filter table...
    */
    CFE_EVS_Register(TT_AppData.EventFilters,
                     TT_EVT_COUNT,
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&TT_AppData.HkPacket,
                    CFE_TT_HK_TLM_MID,
                    sizeof(TT_HkPacket_t), TRUE);

    /*
    ** Create Software Bus message pipe...
    */
    CFE_SB_CreatePipe(&TT_AppData.CmdPipe,
                       TT_AppData.PipeDepth,
                       TT_AppData.PipeName);

    /*
    ** Subscribe to Housekeeping request commands...
    */
    CFE_SB_Subscribe(CFE_TT_SEND_HK_MID,TT_AppData.CmdPipe);

    /*
    ** Subscribe to TT ground command packets...
    */
    CFE_SB_Subscribe(CFE_TT_CMD_MID,TT_AppData.CmdPipe);
    
    /*
    ** Register & Initialize Table...
    */
    Status = CFE_TBL_Register(&TT_AppData.TblHandle,
                     "FourNumbers",
                     4, /* size of table in bytes*/
                     CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                     NULL);
    UTF_put_text("CFE_TBL_Register returned %lx\n", Status);
 
    Status = CFE_TBL_Load(TT_AppData.TblHandle,
                 CFE_TBL_SRC_FILE,
                 "/ram/tt_table_values0.dat");
    UTF_put_text("CFE_TBL_Load returned %lx\n", Status);
                 
    Status = CFE_TBL_GetAddress(&TblPointer,TT_AppData.TblHandle);
    UTF_put_text("CFE_TBL_GetAddress returned %lx\n", Status);
      
    UTF_put_text("The initial values are:%d %d %d %d\n",
     *(int8*)TblPointer,*(int8*)(TblPointer+1),
     *(int8*)(TblPointer+2),*(int8*)(TblPointer+3));
     
     
    Status = CFE_TBL_ReleaseAddress(TT_AppData.TblHandle);                
   UTF_put_text("CFE_TBL_ReleaseAddress returned %lx\n", Status);

    /*
    ** Application startup event message...
    */
    CFE_EVS_SendEvent(TT_INIT_INF_EID,
                      CFE_EVS_INFORMATION,
                     "TT: Application Initialized");

    return;

} /* End of TT_AppInit() */

void TT_AccessCurrentData(void)
{
	void *TblPointer;
	int32 Status;
	
	CFE_TBL_Manage(TT_AppData.TblHandle);
    Status = CFE_TBL_GetAddress(&TblPointer,TT_AppData.TblHandle);
       
    if (Status ==  CFE_TBL_INFO_UPDATED)    
    {      	      	
        TT_AppData.HkPacket.UpdateCount++; 
    }

	 UTF_put_text("The current table values are:%d %d %d %d\n",
     			*(int8*)TblPointer,*(int8*)(TblPointer+1),
     			*(int8*)(TblPointer+2),*(int8*)(TblPointer+3));
    	               
     Status = CFE_TBL_ReleaseAddress(TT_AppData.TblHandle);

	
} /*TT_AccessCurrentData*/



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_AppPipe() -- Process command pipe message                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_AppPipe(CFE_SB_MsgPtr_t msg)
{
    CFE_SB_MsgId_t MessageID;
    uint16 CommandCode;

    MessageID = CFE_SB_GetMsgId(msg);
    switch (MessageID)
    {
        /*
        ** Housekeeping telemetry request...
        */
        case CFE_TT_SEND_HK_MID:
            TT_HousekeepingCmd(msg);
            break;

        /*
        ** TT ground commands...
        */
        case CFE_TT_CMD_MID:

            CommandCode = CFE_SB_GetCmdCode(msg);
            switch (CommandCode)
            {
                case TT_NOOP_CC:
                    TT_NoopCmd(msg);
                    break;

                case TT_RESET_CC:
                    TT_ResetCmd(msg);
                    break;

                default:
                    CFE_EVS_SendEvent(TT_CC1_ERR_EID, CFE_EVS_ERROR,
                     "Invalid ground command code: ID = 0x%X, CC = %d",
                                      MessageID, CommandCode);
                    break;
            }
            break;

        default:

            CFE_EVS_SendEvent(TT_MID_ERR_EID, CFE_EVS_ERROR,
                             "Invalid command pipe message ID: 0x%X",
                              MessageID);
            break;
    }

    return;

} /* End of TT_AppPipe() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_HousekeepingCmd() -- On-board command (HK request)           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_HousekeepingCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(TT_NoArgsCmd_t);

   /*
    ** Verify command packet length...
    */
    if (TT_VerifyCmdLength(msg, ExpectedLength))
    {
        /*
        ** Get command execution counters...
        */
        TT_AppData.HkPacket.CmdCount = TT_AppData.CmdCount;
        TT_AppData.HkPacket.ErrCount = TT_AppData.ErrCount;

        /*
        ** Send housekeeping telemetry packet...
        */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TT_AppData.HkPacket);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &TT_AppData.HkPacket);

        /*
        ** This command does not affect the command execution counter...
        */
    }

    return;

} /* End of TT_HousekeepingCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_NoopCmd() -- TT ground command (NO-OP)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_NoopCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(TT_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (TT_VerifyCmdLength(msg, ExpectedLength))
    {
        TT_AppData.CmdCount++;

        CFE_EVS_SendEvent(TT_NOOP_INF_EID, CFE_EVS_INFORMATION,
                         "TT: No-op command");
    }

    return;

} /* End of TT_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_ResetCmd() -- TT ground command (reset counters)             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TT_ResetCmd(CFE_SB_MsgPtr_t msg)
{
    uint16 ExpectedLength = sizeof(TT_NoArgsCmd_t);

    /*
    ** Verify command packet length...
    */
    if (TT_VerifyCmdLength(msg, ExpectedLength))
    {
        TT_AppData.CmdCount = 0;
        TT_AppData.ErrCount = 0;

        CFE_EVS_SendEvent(TT_RESET_INF_EID, CFE_EVS_INFORMATION,
                         "TT: Reset Counters command");
    }

    return;

} /* End of TT_ResetCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TT_VerifyCmdLength() -- Verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean TT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{
    boolean result = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length...
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(TT_LEN_ERR_EID, CFE_EVS_ERROR,
           "TT: Invalid cmd pkt: ID = 0x%X,  CC = %d, Len = %d",
                          MessageID, CommandCode, ActualLength);
        result = FALSE;
        TT_AppData.ErrCount++;
    }

    return(result);

} /* End of TT_VerifyCmdLength() */


/************************/
/*  End of File Comment */
/************************/

