/******************************************************************************/
/** \file  ci_app.h
*
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Header file for CI Application
*
*   \par Limitations, Assumptions, External Events, and Notes:
*       - Application functions are defined in ci_app.c
*       - Utilities are defined in ci_utils.c
*       - Custom functions are defined in ci_custom.c
*       - SEND_HK is subscribed to the command pipe. The wakeup rate should
*         generally be set faster than the SEND_HK rate, otherwise HK packets
*         will be dropped. SEND_HK is required to get housekeeping data.
*       - The CI_WAKEUP_TIMEOUT value may be used instead of Scheduler 
*         table for app processing rate.
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/
    
#ifndef _CI_APP_H_
#define _CI_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
** Includes
*******************************************************************************/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "osconfig.h"

#include "ci_platform_cfg.h"
#include "ci_mission_cfg.h"
#include "ci_events.h"

/*******************************************************************************
** Macro Definitions
*******************************************************************************/
/** \name Version numbers */
/** \{ */
#define CI_MAJOR_VERSION     1  /**< Major Version Release */
#define CI_MINOR_VERSION     0  /**< Minor Version Release */
#define CI_REVISION          0  /**< Revision for bug fixes */
#define CI_MISSION_REV       0  /**< Revision for mission */
/** \} */

/** \name Return codes */
/** \{ */
#define CI_SUCCESS  0   /**< Success */
#define CI_ERROR   -1   /**< Error */
/** \} */


/** \name Default Macro definitions if not defined in ci_platform_cfg.h */
/** \{ */
#ifndef CI_WAKEUP_TIMEOUT
#define CI_WAKEUP_TIMEOUT  1000  /**< Timeout for App rate (ms) */
#endif

#ifndef CI_SCH_PIPE_DEPTH
#define CI_SCH_PIPE_DEPTH  10   /**< Scheduler Pipe Depth */
#endif
#ifndef CI_CMD_PIPE_DEPTH  
#define CI_CMD_PIPE_DEPTH  10   /**< Command Pipe Depth */
#endif

#ifndef CI_CUSTOM_TASK_STACK_PTR 
#define CI_CUSTOM_TASK_STACK_PTR NULL      /**< Custom task stack pointer */ 
#endif
#ifndef CI_CUSTOM_TASK_STACK_SIZE 
#define CI_CUSTOM_TASK_STACK_SIZE 0x4000   /**< Custom task stack size    */
#endif
#ifndef CI_CUSTOM_TASK_PRIO 
#define CI_CUSTOM_TASK_PRIO 118            /**< Custom task priority      */
#endif
/** \} */


/*******************************************************************************
** Structure definitions
*******************************************************************************/
/** /brief AppData Structure Defenition */
typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[CI_EVT_CNT];      /**< Event Filter Table. */

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId;                     /**< Schedule Pipe ID */
    uint16           usSchPipeDepth;                /**< Schedule Pipe depth */
    char             cSchPipeName[OS_MAX_API_NAME]; /**< Schedule Pipe name */

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;                     /**< Command Pipe ID */   
    uint16           usCmdPipeDepth;                /**< Command Pipe depth */
    char             cCmdPipeName[OS_MAX_API_NAME]; /**< Command Pipe name */ 
    
    /* Task-related */
    uint32  uiRunStatus;        /**< Application Run Status */
   
    /* Wakeup timeout - may be used to set CI rate without SCH app. */
    uint32  uiWakeupTimeout;    /**< CI Wakeup Timeout (ms) */

    /* Output data 
       Data structure defined in $MISSION/apps/inc/{MISSION}_ci_types.h */
    CI_OutData_t  OutData;  /**< Output Data Packet */

    /* Housekeeping telemetry (Sent on CI_SEND_HK command)
       Data structure defined in $MISSION/apps/inc/{MISSION}_ci_types.h */
    CI_HkTlm_t  HkTlm;      /**< Housekeeping Packet */

    /* Mutex to protect telemetry packets (outData, hkTlm). */
    uint32 ciMutex;         /**< CI AppData Mutex */

} CI_AppData_t;


/*******************************************************************************
** Application Function Declarations
*******************************************************************************/

/******************************************************************************/
/** \brief Main Entry Point for CI Application
*
*   \par Description/Algorithm
*       This function is the main entry point of the CI Application. It 
*       performs the following:
*       1. Registers the Application with cFE
*       2. Initializes the application through CI_AppInit
*       3. Loops over the CI_RcvMsg function to perform main function.
*       4. Exit application on kill signal or error
*
*   \par Assumptions, External Events, and Notes:
*       - The CI_MAIN_TASK_PERF_ID Entered in CI_RcvMsg
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns None
*
*   \see 
*       #CI_AppInit
*       #CI_RcvMsg
*       #CI_CleanupCallback
*******************************************************************************/
void  CI_AppMain(void);

/******************************************************************************/
/** \brief Initialize The Application
*
*   \par Description/Algorithm
*        High level initialization function.  Calls in order:
*        1. CI_InitEvent
*        2. CI_InitPipe
*        3. CI_InitData
*        4. CI_CustomInit
*        5. Installs the Cleanup Callback function.
*
*   \par Assumptions, External Events, and Notes:
*       - The CI_CustomInit is defined in ci_custom.c
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns
*   \retcode #CFE_SUCCESS \retdesc  \copydoc CFE_SUCCESS \endcode
*   \retcode #CI_ERROR \retdesc Initialization Error \endcode
*
*   \see 
*       #CI_InitEvent
*       #CI_InitPipe
*       #CI_InitData
*       #CI_CustomInit
*******************************************************************************/
int32  CI_AppInit(void);

/******************************************************************************/
/** \brief Initialize the Event Filter Table.
*
*   \par Description/Algorithm
*        1. Set the EventTbl EventIds based on ids defined in ci_events.h
*        2. Register the events with cFE Table services.
*
*   \par Assumptions, External Events, and Notes:
*        - All Events are intialized as unfiltered.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns
*   \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
*   \retstmt Any of the error codes from #CFE_EVS_Register    \endstmt
*   \endreturns
*
*   \see 
*       #CI_AppInit
*******************************************************************************/
int32  CI_InitEvent(void);

/******************************************************************************/
/** \brief Initialize the Pipes
*
*   \par Description/Algorithm
*        Initialize the Scheduler and Cmd Pipes. The Scheduler pipe is
*        subscribed to the WAKEUP, while the Command
*        pipe is subscribed to the CI_APP_CMD_MID and CI_SEND_HK_MID. 
*
*   \par Assumptions, External Events, and Notes:
*        - No Telemetry pipe is included in the CI application.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns
*   \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
*   \retstmt Any of the error codes from #CFE_SB_CreatePipe    \endstmt
*   \endreturns
*
*   \see 
*       #CI_AppInit
*******************************************************************************/
int32  CI_InitPipe(void);

/******************************************************************************/
/** \brief Initialize Data
*
*   \par Description/Algorithm
*        Initialize the Housekeeping Packet, the OutData Packet and the CI
*        Mutex, for memory protection of these packets.
*
*   \par Assumptions, External Events, and Notes:
*       - The CI_WAKEUP_TIMEOUT is copied to AppData here so that it may be
*         manipulated if desired in custom layer.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns
*   \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
*
*   \see 
*       #CI_AppInit
*******************************************************************************/
int32  CI_InitData(void);

/******************************************************************************/
/** \brief Receive Messages from Software Bus
*
*   \par Description/Algorithm
*       Pend on the SchPipe for CI_WAKEUP_MID. On wakeup, call
*       CI_ProcessNewCmds() and CI_SendOutData(). May also be scheduled at 
*       fixed rate through wakeup timeout.
*
*   \par Assumptions, External Events, and Notes:
*       - On timeout, process as if a wakeup message is received.
*       - Quit app on error status.
*       - SEND_HK is processed in CI_ProcessNewCmds.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] iBlocking blocking timeout (set through CI_WAKEUP_TIMEOUT)
*
*   \returns
*   \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
*   \retstmt Any of the error codes from #CFE_SB_RcvMsg \endstmt
*
*   \see 
*       #CI_AppMain
*       #CI_ProcessNewCmds
*       #CI_SendOutData
*******************************************************************************/
int32  CI_RcvMsg(int32 iBlocking);

/******************************************************************************/
/** \brief Process New Commands
*
*   \par Description/Algorithm
*       Loop over Cmd Pipe for any new messages. Call CI_ProcessNewAppCmds on
*       the receipt of the CI_APP_CMD_MID and CI_ReportHousekeeping on receipt
*       of CI_SEND_HK_MID. Error otherwise. 
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_WAKEUP_MID.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns None
*
*   \see 
*       #CI_RcvMsg
*       #CI_ProcessNewAppCmds
*       #CI_ReportHousekeeping
*******************************************************************************/
void  CI_ProcessNewCmds(void);

/******************************************************************************/
/** \brief Process New Application Commands
*
*   \par Description/Algorithm
*       Process the appropriate response based on the received command code.
*       Possible command codes include:
*       1. CI_NOOP_CC - No-operations. Increment cmd counter and return event.
*       2. CI_RESET_CC - Reset the housekeeping packet.
*       3. CI_ENABLE_TO_CC - Call the CI_CustomEnableTO function.
*       4. Other custom commands - Call CI_CustomAppCmds
*
*   \par Assumptions, External Events, and Notes:
*       - The CI_CustomEnableTO function is defined in ci_custom.c
*       - The command length of each command is verified
*       - All command message structures are defined in {MISSION}_ci_types.h.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns None
*
*   \see 
*       #CI_ProcessNewCmds
*       #CI_CustomEnableTO
*       #CI_CustomAppCmds
*******************************************************************************/
void  CI_ProcessNewAppCmds(CFE_SB_Msg_t*);

/******************************************************************************/
/** \brief Report Housekeeping Packet
*
*   \par Description/Algorithm
*       Send the housekeeping packet to the software bus.
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_SEND_HK_MID.
*       - The HK Packet is protected by ciMutex.
*       - The default Housekeeping packet is defined in ci_hktlm.h.
*       - Housekeeping packet may be extended in {MISSION}_ci_types.h.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns None
*
*   \see 
*       #CI_RcvMsg
*******************************************************************************/
void  CI_ReportHousekeeping(void);

/******************************************************************************/
/** \brief Send the OutData Packet
*
*   \par Description/Algorithm
*       Send the OutData packet to the software bus.
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_WAKEUP_MID.
*       - The OutData Packet is protected by ciMutex.
*       - The OutData packet is defined in {MISSION}_ci_types.h.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*
*   \returns None
*
*   \see 
*       #CI_RcvMsg
*******************************************************************************/
void  CI_SendOutData(void);

/******************************************************************************/
/** \brief Perform cleanup on shutdown
*
*   \par Call the custom cleanup function to close I/O channels, etc. 
*
*   \par Assumptions, External Events, and Notes:
*       - CI_CustomCleanup is defined in ci_custom.c
*       - This function gets called on CFE_ES_ExitApp from CI_AppMain.
*
*   \param None
*
*   \returns None
*
*   \see 
*       #CI_AppMain
*       #CI_AppInit
*       #CI_CustomCleanup
*******************************************************************************/
void  CI_CleanupCallback(void);


/*******************************************************************************
** Utility Function Declarations
*******************************************************************************/

/******************************************************************************/
/** \brief Increment a housekeeping packet counter
*
*   \par Description/Algorithm
*       Increments the specified counter. The Incrementation is protected by
*       the application mutex.
*
*   \par Assumptions, External Events, and Notes:
*       - This function should be used within the custom layer to increment any
*       counters, as it has memory access protection.
*       - This assumes that all counters are of type uint16.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] counter The pointer to the counter to increment.
*
*   \returns None
*
*   \see 
*       #CI_ProcessNewAppCmds
*       #CI_CustomAppCmds
*******************************************************************************/
void CI_IncrHkCounter(uint16 * counter);

/******************************************************************************/
/** \brief Verify the command length against expected length
*
*   \par Description/Algorithm
*       Get the message length through the SB API and compare it with the passed
*       in expected length.  If not equal, issue an error event, increment the
*       uiCmdErrCnt and return false.
*
*   \par Assumptions, External Events, and Notes:
*       - Call this function for all received commands to verify the length.
*       - Should also be used in the custom layer.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] pMsg Pointer to the CCSDS message.
*   \param[in] expectedLen Expected Command message length.
*
*   \returns True or False
*
*   \see 
*       #CI_ProcessNewAppCmds
*       #CI_CustomAppCmds
*       #CI_CustomGateCmds
*******************************************************************************/
boolean  CI_VerifyCmdLength(CFE_SB_Msg_t*, uint16);


/*******************************************************************************
** Required Custom Functions
*******************************************************************************/

/******************************************************************************/
/** \brief Custom Initialization
*
*   \par Description/Algorithm
*       This function is mainly responsible to initialize the transport
*       protocol(s) to use and to create the custom child task.  The entry 
*       function for the child task should be CI_CustomMain.
*
*   \par Assumptions, External Events, and Notes:
*       - Configuration macros defined in ci_platform_cfg.h should be used when
*       configuring the transport protocol(s).
*       - A local file descriptor should be defined for each transport protocols
*       - A local buffer should be allocated to store incomming messages.
*       - Any access to AppData (HK or OutData) should make use of the ciMutex
*       for memory protection.
*       - This function is executed by the application main task through
*       CI_AppInit 
*
*   \param None
*
*   \returns 
*   \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
*   \retcode #CI_ERROR \retdesc Initialization Error \endcode
*
*   \see 
*       #CI_AppInit
*       #CI_CustomMain
*       #CFE_ES_CreateChildTask
*******************************************************************************/
int32 CI_CustomInit(void);

/******************************************************************************/
/** \brief Custom Child Task Entry Point
*
*   \par Description/Algorithm
*       This function is responsible for receiving all commands over the choosen
*       transport protocol.  The general pattern to follow is as followed:
*       1. Check that the local file descriptor has been set correctly
*       2. Loop as long as no errors are present
*       3. Pend forever on uplink commands
*       4. Call any data link I/O services (if applicable) to form full packet
*       from frames.
*       5. Call any format conversion protocols (if applicable) to form SPP
*       (CCSDS) command packets.
*       6. Get the CCSDS Msg ID of the command message. 
*       7. Validate the Checksum of the message.
*       8. If the command is the CI_GATE_CMG_MID, call CI_CustomGateCmds 
*       9. Pass on any other commands to the Software bus
*       10. Continue loop (2)
*
*   \par Assumptions, External Events, and Notes:
*       - Configuration macros defined in ci_platform_cfg.h should be used when
*       configuring the transport protocol(s).
*       - Any access to AppData (HK or OutData) should make use of the ciMutex
*       for memory protection.
*       - This function is executed by the Custom Child Task.
*
*   \param None
*
*   \returns None 
*
*   \see 
*       #CI_CustomInit
*       #CI_CustomGateCmds
*       #CFE_SB_ValidateChecksum
*       #CFE_SB_GetMsgId
*       #CFE_SB_SendMsg
*******************************************************************************/
void  CI_CustomMain(void);

/******************************************************************************/
/** \brief Process of Gate commands by child task
*
*   \par Description/Algorithm
*       This function is responsible to process any custom gate commands. Make
*       use of same pattern as in CI_ProcessNewAppCmds, with a switch case on
*       the received command code of the message.
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_GATE_CMD_MID, called 
*       from the CI_CustomMain function.
*       - Any Gate commands should be executed immidiately.
*       - Any access to AppData (HK or OutData) should make use of the ciMutex
*       for memory protection.
*       - This function is executed by the Custom Child Task.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] cmdMsgPtr The pointer to the (CCSDS) Gate command message.
*
*   \returns None 
*
*   \see 
*       #CI_CustomMain
*       #CI_IncrHkCounter
*       #CI_VerifyCmdLength
*       #CFE_SB_GetCmdCode
*******************************************************************************/
void  CI_CustomGateCmds(CFE_SB_Msg_t *);

/******************************************************************************/
/** \brief Process of custom app commands by main task
*
*   \par Description/Algorithm
*       This function is responsible to process any custom app commands. Make
*       use of same pattern as in CI_ProcessNewAppCmds, with a switch case on
*       the received command code of the message. This function is called on 
*       any user defined non-generic commands from CI_ProcessNewAppCmds. Return
*       CI_ERROR if the command code is not recognized.
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_APP_CMD_MID, called 
*       from the CI_ProcessNewAppCmds function, on a custom command code.
*       - Any access to AppData (HK or OutData) should make use of the ciMutex
*       for memory protection.
*       - This function is executed by the application main task.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] pCmdMsg The pointer to the (CCSDS) app command message.
*
*   \returns
*   \retcode #CI_SUCCESS \retdesc Success \endcode
*   \retcode #CI_ERROR \retdesc Bad Command code \endcode
*
*   \see 
*       #CI_ProcessNewAppCmds
*       #CI_IncrHkCounter
*       #CI_VerifyCmdLength
*       #CFE_SB_GetCmdCode
*******************************************************************************/
int32 CI_CustomAppCmds(CFE_SB_Msg_t *pCmdMsg);

/******************************************************************************/
/** \brief Custom response to the Enable TO command
*
*   \par Description/Algorithm
*       This function performs the response to the CI_ENABLE_TO_CC command code
*       and is called from the CI_ProcessNewAppCmds function. It must construct
*       the appropriate enabling message for the TO application, according to
*       the transport protocol used by TO. Examples:
*       - For UDP: The input command can be piped through to TO with the
*       TO_APP_CMD_MID, provided that the message contains the destination IP.
*       - For RS422: A TO message should be generated with the file descriptor
*       used by CI if the serial port is to be used as a duplex serial port. 
*       In all cases, the new message should be generated as followed:
*       1. Set the Message ID to TO_APP_CMD_MID
*       2. Set the Command code to TO_ENABLE_OUTPUT_CC
*       3. Generate a new checksum 
*
*   \par Assumptions, External Events, and Notes:
*       - This function is called in response to the CI_APP_CMD_MID with 
*       command code CI_ENABLE_TO_CC, called from the CI_ProcessNewAppCmds 
*       function, on the CI_ENABLE_TO_CC command code.
*       - Any access to AppData (HK or OutData) should make use of the ciMutex
*       for memory protection.
*       - This function is executed by the application main task.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] pCmdMsg The pointer to the (CCSDS) Gate command message.
*
*   \returns None 
*
*   \see 
*       #CI_ProcessNewAppCmds
*       #CFE_SB_SetMsgId
*       #CFE_SB_SetCmdCode
*       #CFE_SB_GenerateChecksum
*       #CFE_SB_SendMsg
*******************************************************************************/
void  CI_CustomEnableTO(CFE_SB_Msg_t *pCmdMsg);

/******************************************************************************/
/** \brief Custom Cleanup 
*
*   \par Description/Algorithm
*       This function will close any transport protocols in response to 
*       the exiting the application.  
*
*   \par Assumptions, External Events, and Notes:
*       - The custom child task will be exited automatically on termination
*       of the application.  
*       - This function is called during the termination process of the 
*       main task, by the main task.
*
*   \param[in,out] g_CI_AppData CI Global Application Data
*   \param[in] cmdMsgPtr The pointer to the (CCSDS) Gate command message.
*
*   \returns None 
*
*   \see 
*       #CI_AppMain
*       #CI_CleanupCallback
*******************************************************************************/
void  CI_CustomCleanup(void);


#ifdef __cplusplus
}
#endif

#endif /* _CI_APP_H_ */

/*==============================================================================
** End of file ci_app.h
**============================================================================*/
