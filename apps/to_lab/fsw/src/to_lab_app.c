/************************************************************************
** File: to_lab_app.c
**
** Purpose: 
**  his file contains the source code for the TO lab application
**
** Notes:
**
** $Log: to_lab_app.c  $
** Revision 1.8 2014/07/17 12:51:03GMT-05:00 acudmore 
** renamed to_sub_table.h to to_lab_sub_table.h
** Revision 1.7 2014/07/16 14:43:21GMT-05:00 acudmore 
** Updated TO_LAB , relocating subscription table in header file
** Revision 1.6 2010/09/20 13:28:05EDT wmoleski 
** Modified the CI_LAB, SCH_LAB and TO_LAB applications to use unique message IDs and Pipe Names. The "_LAB"
** was added to all definitions so that a mission can use these "Lab" apps as well as their own mission apps together.
** Revision 1.5 2008/09/22 14:01:35EDT apcudmore 
** Removed reference to FS Housekeeping packet, since FS thread does not exist.
** Revision 1.4 2008/09/22 13:57:46EDT apcudmore 
** Added RunLoop call to TO_LAB app.
** Revision 1.3 2008/09/19 15:31:45EDT rjmcgraw 
** DCR4337:1 Added #include version.h and display version after initialization is complete
** Revision 1.2 2008/04/30 15:31:40EDT rjmcgraw 
** DCR1718:2 Fixed compiler errors
** Revision 1.1 2008/04/30 14:43:50EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/src/project.pj
**
*************************************************************************/

#include "to_lab_app.h"
#include "to_lab_msg.h"
#include "to_lab_events.h"
#include "to_lab_msgids.h"
#include "to_lab_perfids.h"
#include "to_lab_version.h"


/*
** Global Data Section
*/
to_hk_tlm_t       to_hk_status;
CFE_SB_PipeId_t   TO_Tlm_pipe;
CFE_SB_PipeId_t   TO_Cmd_pipe;

/*
** Local Data Section
*/
static int                TLMsockid;
static to_data_types_fmt  data_types_pkt;
static boolean            downlink_on;
static char               tlm_dest_IP[17];
static boolean            suppress_sendto;

/*
** Include the TO subscription table
**  This header is in the platform include directory
**  and can be changed for default TO subscriptions in 
**  each CPU.
*/
#include "to_lab_sub_table.h"

/*
** Event Filter Table
*/
static CFE_EVS_BinFilter_t  CFE_TO_EVS_Filters[] =
          { /* Event ID    mask */
            {TO_INIT_INF_EID,           0x0000},
            {TO_CRCMDPIPE_ERR_EID,      0x0000},
            {TO_SUBSCRIBE_ERR_EID,      0x0000},
            {TO_TLMOUTSOCKET_ERR_EID,   0x0000},
            {TO_TLMOUTSTOP_ERR_EID,     0x0000},
            {TO_MSGID_ERR_EID,          0x0000},
            {TO_FNCODE_ERR_EID,         0x0000},
            {TO_NOOP_INF_EID,           0x0000}
          };

/*
** Prototypes Section
*/
static void TO_openTLM(void);
static void TO_init(void);
static void TO_process_commands(void);
static void TO_exec_local_command(CFE_SB_Msg_t  *MsgPtr);
static void TO_reset_status(void);
static void TO_output_data_types_packet(void);
static void TO_output_status(void);
static void TO_AddPkt(TO_ADD_PKT_t * cmd);
static void TO_RemovePkt(TO_REMOVE_PKT_t * cmd);
static void TO_RemoveAllPkt(void);
static void TO_forward_telemetry(void);
static void TO_StartSending( TO_OUTPUT_ENABLE_PKT_t * pCmd );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                   */
/* TO_Lab_AppMain() -- Application entry point and main process loop */
/*                                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFS_MODULE_DECLARE_APP(to_lab, 50, 8192);

void to_lab_Main(void)
{
   uint32                  RunStatus = CFE_ES_APP_RUN;
   
   CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

   TO_init();

   /*
   ** TO RunLoop
   */
   while(CFE_ES_RunLoop(&RunStatus) == TRUE)
   {
        CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

        OS_TaskDelay(TO_TASK_MSEC);    /*2 Hz*/

        CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

        TO_forward_telemetry();

        TO_process_commands();
    }

   CFE_ES_ExitApp(RunStatus);

} /* End of TO_Lab_AppMain() */

/*
** TO delete callback function.
** This function will be called in the event that the TO app is killed. 
** It will close the network socket for TO
*/
void TO_delete_callback(void)
{
    OS_printf("TO delete callback -- Closing TO Network socket.\n");
    if ( downlink_on == TRUE )
    {
        close(TLMsockid);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_init() -- TO initialization                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_init(void)
{
    int32            status;
    char             PipeName[16];
    uint16           PipeDepth;
    uint16           i;
    char             ToTlmPipeName[16];
    uint16           ToTlmPipeDepth;

    CFE_ES_RegisterApp();
    downlink_on = FALSE;
    PipeDepth = 8;
    strcpy(PipeName,  "TO_LAB_CMD_PIPE");
    ToTlmPipeDepth = 64;
    strcpy(ToTlmPipeName,  "TO_LAB_TLM_PIPE");

    /*
    ** Register event filter table...
    */
    CFE_EVS_Register(CFE_TO_EVS_Filters,
                     sizeof(CFE_TO_EVS_Filters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);
    /*
    ** Initialize housekeeping packet (clear user data area)...
    */
    CFE_SB_InitMsg(&to_hk_status,
                    TO_LAB_HK_TLM_MID,
                    sizeof(to_hk_status), TRUE);

    /* Subscribe to my commands */
    status = CFE_SB_CreatePipe(&TO_Cmd_pipe, PipeDepth, PipeName);
    if (status == CFE_SUCCESS)
    {
       CFE_SB_Subscribe(TO_LAB_CMD_MID,     TO_Cmd_pipe);
       CFE_SB_Subscribe(TO_LAB_SEND_HK_MID, TO_Cmd_pipe);
    }
    else
       CFE_EVS_SendEvent(TO_CRCMDPIPE_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't create cmd pipe status %i",__LINE__,(int)status);

    /* Create TO TLM pipe */
    status = CFE_SB_CreatePipe(&TO_Tlm_pipe, ToTlmPipeDepth, ToTlmPipeName);
    if (status != CFE_SUCCESS)
    {
       CFE_EVS_SendEvent(TO_TLMPIPE_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't create Tlm pipe status %i",__LINE__,(int)status);
    }

    /* Subscriptions for TLM pipe*/
    for (i=0; (i < (sizeof(TO_SubTable)/sizeof(TO_subsciption_t))); i++)
    {
       if(TO_SubTable[i].Stream != TO_UNUSED )
          status = CFE_SB_SubscribeEx(TO_SubTable[i].Stream,
                                      TO_Tlm_pipe,
                                      TO_SubTable[i].Flags,
                                      TO_SubTable[i].BufLimit);

       if (status != CFE_SUCCESS)
           CFE_EVS_SendEvent(TO_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,"L%d TO Can't subscribe to stream 0x%x status %i", __LINE__,
                             TO_SubTable[i].Stream,(int)status);
    }
    
    /*
    ** Install the delete handler
    */
    OS_TaskInstallDeleteHandler(&TO_delete_callback);

    CFE_EVS_SendEvent (TO_INIT_INF_EID, CFE_EVS_INFORMATION,
               "TO Lab Initialized. Version %d.%d.%d.%d Awaiting enable command.",
                TO_LAB_MAJOR_VERSION,
                TO_LAB_MINOR_VERSION, 
                TO_LAB_REVISION, 
                TO_LAB_MISSION_REV);

    // Enable Telemetry by deault
    TO_StartSending( (TO_OUTPUT_ENABLE_PKT_t *) "\x18\x84\xC0\x00\x00\x12\x00\x06\x31\x32\x37\x2E\x30\x2E\x30\x2E\x31");
    downlink_on = TRUE;
    
} /* End of TO_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_StartSending() -- TLM output enabled                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_StartSending( TO_OUTPUT_ENABLE_PKT_t * pCmd )
{
    (void) CFE_SB_MessageStringGet(tlm_dest_IP, pCmd->dest_IP, "",
                                   sizeof (tlm_dest_IP),
                                   sizeof (pCmd->dest_IP));
    suppress_sendto = FALSE;
    CFE_EVS_SendEvent(TO_TLMOUTENA_INF_EID,CFE_EVS_INFORMATION,"TO telemetry output enabled for IP %s", tlm_dest_IP);

    if(downlink_on == FALSE) /* Then turn it on, otherwise we will just switch destination addresses*/
    {
       TO_openTLM();
       downlink_on = TRUE;
    }
} /* End of TO_StartSending() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_process_commands() -- Process command pipe message           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_process_commands(void)
{
    CFE_SB_Msg_t    *MsgPtr;
    CFE_SB_MsgId_t  MsgId;

    while(1)
    {
       switch (CFE_SB_RcvMsg(&MsgPtr, TO_Cmd_pipe, CFE_SB_POLL))
       {
          case CFE_SUCCESS:

               MsgId = CFE_SB_GetMsgId(MsgPtr);
               /* For SB return statuses that imply a message: process it. */
               switch (MsgId)
               {
                  case TO_LAB_CMD_MID:
                       TO_exec_local_command(MsgPtr);
                       break;

                  case TO_LAB_SEND_HK_MID:
                       TO_output_status();
                       break;

                  default:
                       CFE_EVS_SendEvent(TO_MSGID_ERR_EID,CFE_EVS_ERROR, "L%d TO: Invalid Msg ID Rcvd 0x%x",__LINE__,MsgId);
                       break;
               }
               break;
            default:
               /* Exit command processing loop if no message received. */
               return;
       }
    }
} /* End of TO_process_commands() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  TO_exec_local_command() -- Process local message               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_exec_local_command(CFE_SB_MsgPtr_t cmd)
{
    uint16 CommandCode;
    boolean valid = TRUE;
    CommandCode = CFE_SB_GetCmdCode(cmd);

    switch (CommandCode)
    {
       case TO_NOP_CC:
            CFE_EVS_SendEvent(TO_NOOP_INF_EID,CFE_EVS_INFORMATION, "No-op command");
            break;

       case TO_RESET_STATUS_CC:
            TO_reset_status();
            --to_hk_status.command_counter;
            break;

       case TO_SEND_DATA_TYPES_CC:
            TO_output_data_types_packet();
            break;

       case TO_ADD_PKT_CC:
            TO_AddPkt((TO_ADD_PKT_t *)cmd);
            break;

       case TO_REMOVE_PKT_CC:
            TO_RemovePkt( (TO_REMOVE_PKT_t *)cmd);
            break;

       case TO_REMOVE_ALL_PKT_CC:
            TO_RemoveAllPkt();
            break;

       case TO_OUTPUT_ENABLE_CC:
            TO_StartSending( (TO_OUTPUT_ENABLE_PKT_t *)cmd );
            downlink_on = TRUE;
            break;

       default:
            CFE_EVS_SendEvent(TO_FNCODE_ERR_EID,CFE_EVS_ERROR, "L%d TO: Invalid Function Code Rcvd In Ground Command 0x%x",__LINE__,
                              CommandCode);
            valid = FALSE;
    }

    if (valid)
       ++to_hk_status.command_counter;
    else
       ++to_hk_status.command_error_counter;
} /* End of TO_exec_local_command() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_reset_status() -- Reset counters                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_reset_status(void)
{
    to_hk_status.command_error_counter = 0;
    to_hk_status.command_counter = 0;
} /* End of TO_reset_status() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_output_data_types_packet()  -- Output data types             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_output_data_types_packet(void)
{
    int16             i;
    char             string_variable[10] = "ABCDEFGHIJ";

    /* initialize data types packet */
    CFE_SB_InitMsg(&data_types_pkt,
                   TO_LAB_DATA_TYPES_MID,
                   sizeof(data_types_pkt), TRUE);

    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &data_types_pkt);

    /* initialize the packet data */
    data_types_pkt.synch = 0x6969;
#if 0
    data_types_pkt.bit1 = 1;
    data_types_pkt.bit2 = 0;
    data_types_pkt.bit34 = 2;
    data_types_pkt.bit56 = 3;
    data_types_pkt.bit78 = 1;
    data_types_pkt.nibble1 = 0xA;
    data_types_pkt.nibble2 = 0x4;
#endif
    data_types_pkt.bl1 = FALSE;
    data_types_pkt.bl2 = TRUE;
    data_types_pkt.b1 = 16;
    data_types_pkt.b2 = 127;
    data_types_pkt.b3 = 0x7F;
    data_types_pkt.b4 = 0x45;
    data_types_pkt.w1 = 0x2468;
    data_types_pkt.w2 = 0x7FFF;
    data_types_pkt.dw1 = 0x12345678;
    data_types_pkt.dw2 = 0x87654321;
    data_types_pkt.f1 = 90.01;
    data_types_pkt.f2 = .0000045;
    data_types_pkt.df1 = 99.9;
    data_types_pkt.df2 = .4444;

    for (i=0; i < 10; i++) data_types_pkt.str[i] = string_variable[i];

    CFE_SB_SendMsg((CFE_SB_Msg_t *)&data_types_pkt);
} /* End of TO_output_data_types_packet() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_output_status() -- HK status                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_output_status(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &to_hk_status);
    CFE_SB_SendMsg((CFE_SB_Msg_t *)&to_hk_status);
} /* End of TO_output_status() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_openTLM() -- Open TLM                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_openTLM(void)
{
    if ( (TLMsockid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
       CFE_EVS_SendEvent(TO_TLMOUTSOCKET_ERR_EID,CFE_EVS_ERROR, "L%d, TO TLM socket errno: %d",__LINE__, errno);

/*---------------- Add static arp entries ----------------*/

} /* End of TO_open_TLM() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_AddPkt() -- Add packets                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_AddPkt( TO_ADD_PKT_t * pCmd)
{
    int32  status;

    status = CFE_SB_SubscribeEx(pCmd->Stream,
                                TO_Tlm_pipe,
                                pCmd->Flags,
                                pCmd->BufLimit);

    if(status != CFE_SUCCESS)
       CFE_EVS_SendEvent(TO_ADDPKT_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't subscribe 0x%x status %i",__LINE__,
                         pCmd->Stream, (int)status);
    else
       CFE_EVS_SendEvent(TO_ADDPKT_INF_EID,CFE_EVS_INFORMATION, "L%d TO AddPkt 0x%x, QoS %d.%d, limit %d",__LINE__,
                         pCmd->Stream,
                         pCmd->Flags.Priority,
                         pCmd->Flags.Reliability,
                         pCmd->BufLimit);
} /* End of TO_AddPkt() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_RemovePkt() -- Remove Packet                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_RemovePkt(TO_REMOVE_PKT_t * pCmd)
{
    int32  status;

    status = CFE_SB_Unsubscribe(pCmd->Stream, TO_Tlm_pipe);
    if(status != CFE_SUCCESS)
       CFE_EVS_SendEvent(TO_REMOVEPKT_ERR_EID,CFE_EVS_ERROR,"L%d TO Can't Unsubscribe to Stream 0x%x on pipe %d, status %i",__LINE__,
                         pCmd->Stream, TO_Tlm_pipe, (int)status);
    else
       CFE_EVS_SendEvent(TO_REMOVEPKT_INF_EID,CFE_EVS_INFORMATION,"L%d TO RemovePkt 0x%x",__LINE__, pCmd->Stream);
} /* End of TO_RemovePkt() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_RemoveAllPkt() --  Remove All Packets                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_RemoveAllPkt(void)
{
    int32  status;
    int i;

    for (i=0; (i < (sizeof(TO_SubTable)/sizeof(TO_subsciption_t))); i++)
    {
       if (TO_SubTable[i].Stream != TO_UNUSED )
       {
          status = CFE_SB_Unsubscribe(TO_SubTable[i].Stream, TO_Tlm_pipe);

          if(status != CFE_SUCCESS)
             CFE_EVS_SendEvent(TO_REMOVEALLPTKS_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't Unsubscribe to stream 0x%x status %i", __LINE__,
                               TO_SubTable[i].Stream, (int)status);
       }
    }

    /* remove commands as well */
    status = CFE_SB_Unsubscribe(TO_LAB_CMD_MID, TO_Cmd_pipe);
    if(status != CFE_SUCCESS)
       CFE_EVS_SendEvent(TO_REMOVECMDTO_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't Unsubscribe to cmd stream 0x%x status %i", __LINE__,
                         TO_LAB_CMD_MID, (int)status);

    status = CFE_SB_Unsubscribe(TO_LAB_SEND_HK_MID, TO_Cmd_pipe);
    if (status != CFE_SUCCESS)
       CFE_EVS_SendEvent(TO_REMOVEHKTO_ERR_EID,CFE_EVS_ERROR, "L%d TO Can't Unsubscribe to cmd stream 0x%x status %i", __LINE__,
                         TO_LAB_CMD_MID, (int)status);

    CFE_EVS_SendEvent(TO_REMOVEALLPKTS_INF_EID,CFE_EVS_INFORMATION, "L%d TO Unsubscribed to all Commands and Telemetry", __LINE__);
} /* End of TO_RemoveAllPkt() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO_forward_telemetry() -- Forward telemetry                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_forward_telemetry(void)
{
    static struct sockaddr_in s_addr;
    int                       status;
    int32                     CFE_SB_status;
    uint16                    size;
    CFE_SB_Msg_t              *PktPtr;

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(tlm_dest_IP);
    s_addr.sin_port        = htons(cfgTLM_PORT);
    status = 0;

    do
    {
       CFE_SB_status = CFE_SB_RcvMsg(&PktPtr, TO_Tlm_pipe, CFE_SB_POLL);

       if ( (CFE_SB_status == CFE_SUCCESS) && (suppress_sendto == FALSE) )
       {
          size = CFE_SB_GetTotalMsgLength(PktPtr);
          
          if(downlink_on == TRUE)
          {
             CFE_ES_PerfLogEntry(TO_SOCKET_SEND_PERF_ID);

             status = sendto(TLMsockid, (char *)PktPtr, size, 0,
                                        (struct sockaddr *) &s_addr,
                                         sizeof(s_addr) );
                                         
             CFE_ES_PerfLogExit(TO_SOCKET_SEND_PERF_ID);                             
          }
          if (status < 0)
          {
             CFE_EVS_SendEvent(TO_TLMOUTSTOP_ERR_EID,CFE_EVS_ERROR,
                               "L%d TO sendto errno %d. Tlm output supressed\n", __LINE__, errno);
             suppress_sendto = TRUE;
          }
       }
    /* If CFE_SB_status != CFE_SUCCESS, then no packet was received from CFE_SB_RcvMsg() */
    }while(CFE_SB_status == CFE_SUCCESS);
} /* End of TO_forward_telemetry() */

/************************/
/*  End of File Comment */
/************************/
