/************************************************************************
** File:
**   $Id: hk_app.h 1.8 2015/03/04 14:58:30EST sstrege Exp  $
**
**  Copyright © 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**  The CFS Housekeeping (HK) Application header file
**
** Notes:
**
** $Log: hk_app.h  $
** Revision 1.8 2015/03/04 14:58:30EST sstrege 
** Added copyright information
** Revision 1.7 2009/12/03 17:00:17EST jmdagost 
** Added include of hk_msgdefs.h
** Revision 1.6 2009/04/18 13:02:36EDT dkobe 
** Corrected doxygen comments
** Revision 1.5 2008/09/11 11:41:35EDT rjmcgraw 
** DCR 4041:1 Added #include hk_platform_cfg.h
** Revision 1.4 2008/07/15 16:52:49EDT rjmcgraw 
** DCR4042:1 Removed old #define HK_REV_NUM
** Revision 1.3 2008/06/19 13:54:39EDT rjmcgraw 
** DCR3052:1 Table processing fix - changed version number from 0.1 to 0.2
** Revision 1.2 2008/05/02 12:18:13EDT rjmcgraw 
** DCR1647:1 Moved performance marker to hk_perfids.h
** Revision 1.1 2008/04/09 16:41:46EDT rjmcgraw 
** Initial revision
** Member added to CFS project
**
*************************************************************************/
#ifndef _hk_app_h_
#define _hk_app_h_


/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "hk_msgdefs.h"
#include "hk_msg.h"
#include "hk_utils.h"
#include "hk_platform_cfg.h"

/*************************************************************************
** Macro definitions
**************************************************************************/
#define HK_PIPE_NAME          "HK_CMD_PIPE" /**< \brief Application Pipe Name  */

#define HK_SUCCESS            (0)  /**< \brief HK return code for success */
#define HK_ERROR              (-1) /**< \brief HK return code for general error */
#define HK_BAD_MSG_LENGTH_RC  (-2) /**< \brief HK return code for unexpected cmd length */


/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief HK global data structure
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet...
    */
    HK_HkPacket_t 			HkPacket;/**< \brief HK Housekeeping Packet */

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_MsgPtr_t         MsgPtr;/**< \brief Pointer to msg received on software bus */

    CFE_SB_PipeId_t         CmdPipe;/**< \brief Pipe Id for HK command pipe */    
    uint8					CmdCounter;/**< \brief Number of valid commands received */
    uint8					ErrCounter;/**< \brief Number of invalid commands received */
    uint8					Spare;/**< \brief Spare byte for alignment */

    uint16					MissingDataCtr;/**< \brief Number of times missing data was detected */
    uint16					CombinedPacketsSent;/**< \brief Count of combined output msgs sent */    

    uint32                  MemPoolHandle;/**< \brief HK mempool handle for output pkts */
    uint32                  RunStatus;/**< \brief HK App run status */
        
    CFE_TBL_Handle_t        CopyTableHandle;/**< \brief Copy Table handle */
    CFE_TBL_Handle_t        RuntimeTableHandle;/**< \brief Run-time table handle */

    hk_copy_table_entry_t   *CopyTablePtr;/**< \brief Ptr to copy table entry */
    hk_runtime_tbl_entry_t  *RuntimeTablePtr;/**< \brief Ptr to run-time table entry */
        
    uint8                   MemPoolBuffer [HK_NUM_BYTES_IN_MEM_POOL];/**< \brief HK mempool buffer */

} HK_AppData_t;


/*************************************************************************
** Exported data
**************************************************************************/
extern HK_AppData_t             HK_AppData;/**< \brief HK Housekeeping Packet */

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS Housekeeping (HK) application entry point
**  
**  \par Description
**       Housekeeping application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
*************************************************************************/
void HK_AppMain(void);


/************************************************************************
** Prototypes for functions defined in hk_app.c
*************************************************************************/
/************************************************************************/
/** \brief Initialize the housekeeping application
**  
**  \par Description
**       Housekeeping application initialization routine. This 
**       function performs all the required startup steps to 
**       get the application registered with the cFE services so
**       it can begin to receive command messages. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register         \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
**  \endreturns
**
*************************************************************************/
int32 HK_AppInit (void);


/************************************************************************/
/** \brief Initialize the Copy Table and the Runtime Table
**  
**  \par Description
**       Registers the Copy table and Runtime table with cFE Table 
**       Services. Also processes the copy table. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \sa #HK_AppInit
**
*************************************************************************/
int32 HK_TableInit (void);


/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the command.
**       
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message 
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void HK_AppPipe (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Send Combined Housekeeping Message
**  
**  \par Description
**       Processes the command to send a combined housekeeping message
**
**  \par Assumptions, External Events, and Notes:
**       This command does not affect the command execution counter, but 
**       this command will increment the cmd error counter if an invalid cmd
**       length is detected.
**
**  \param [in]  MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message 
**
*************************************************************************/
void HK_SendCombinedHKCmd(CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Process housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This command does not affect the command execution counter, but 
**       this command will increment the cmd error counter if an invalid cmd
**       length is detected.
**
**  \param [in]  MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message 
**
*************************************************************************/
void HK_HousekeepingCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Process noop command
**  
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]  MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                            references the software bus message 
**
**  \sa #HK_NOOP_CC
**
*************************************************************************/
void HK_NoopCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Process reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the memory manager commmand error and command execution counters
**       to zero.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #HK_RESET_CC
**
*************************************************************************/
void HK_ResetCtrsCmd (CFE_SB_MsgPtr_t MessagePtr);


/************************************************************************/
/** \brief Reset housekeeping data
**  
**  \par Description
**       Function called in response to a Reset Counters Command. This
**       function will reset the HK housekeeping data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \sa #HK_RESET_CC
**
*************************************************************************/
void HK_ResetHkData (void);


/************************************************************************/
/** \brief  Verify length of HK commands
**  
**  \par Description
**       Function called when an HK command is received. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**  \param [in]   ExpectedLength   The expected lenght of the command
**
**  \returns
**  \retcode #HK_SUCCESS  if actual cmd length is same as expected
**  \retcode #HK_BAD_MSG_LENGTH_RC if actual cmd length is not as expected
**  \endreturns
**
**  \sa 
**
*************************************************************************/
int32 HK_VerifyCmdLength (CFE_SB_MsgPtr_t MessagePtr,uint32 ExpectedLength);



#endif /* _hk_app_ */

/************************/
/*  End of File Comment */
/************************/
