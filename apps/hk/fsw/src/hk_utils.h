/************************************************************************
** File:
**   $Id: hk_utils.h 1.7 2015/03/04 14:58:28EST sstrege Exp  $
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
** $Log: hk_utils.h  $
** Revision 1.7 2015/03/04 14:58:28EST sstrege 
** Added copyright information
** Revision 1.6 2009/04/18 13:10:28EDT dkobe 
** Correct doxygen comment for return code
** Revision 1.5 2009/04/18 13:08:31EDT dkobe 
** Corrections to function prototype doxygen comments
** Revision 1.4 2009/04/18 13:02:36EDT dkobe 
** Corrected doxygen comments
** Revision 1.3 2008/09/11 10:26:37EDT rjmcgraw 
** DCR4040:1 Replaced #include hk_platform_cfg.h with hk_tbldefs.h and fixed tabs
** Revision 1.2 2008/06/19 13:26:17EDT rjmcgraw 
** DCR3052:1 Replaced HandleUpdateToCopyTable proto with CheckStatusOfTables proto
** Revision 1.1 2008/04/09 16:42:42EDT rjmcgraw 
** Initial revision
** Member added to CFS project
**
*************************************************************************/
#ifndef _hk_utils_h_
#define _hk_utils_h_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "hk_tbldefs.h"


/*************************************************************************
** Macro definitions
**************************************************************************/
#define HK_INPUTMID_SUBSCRIBED          0xFF  /**< \brief Input MsgId has been subscribed to */
#define HK_INPUTMID_NOT_SUBSCRIBED      0     /**< \brief Input MsgId is not subscribed */

#define HK_DATA_NOT_PRESENT             0     /**< \brief Input MsgId present in output msg */
#define HK_DATA_PRESENT                 1     /**< \brief Input MsgId not present */

#define HK_NO_MISSING_DATA              0     /**< \brief Output Msg has no missing data */
#define HK_MISSING_DATA_DETECTED        1     /**< \brief Output Msg has missing data */

#define HK_UNDEFINED_ENTRY              0     /**< \brief Undefined table field entry */



/************************************************************************
** Prototypes for functions defined in hk_utils.c
*************************************************************************/

/*****************************************************************************/
/**
** \brief Process incoming housekeeping data message
**
** \par Description
**        This routine examines each entry in the table and determines whether
**        its field comprises a legal entry.  If so, a portion of the input 
**        packet is copied to the appropriate combined output packet.
**
** \par Assumptions, External Events, and Notes:
**        Currently the combined telemetry packets are not initialized after 
**        they are sent so values will repeat if no housekeeping update is 
**        received.
**
** \param[in]  MessagePtr    A pointer to the input message. 
**
** \sa #HK_AppPipe
**
******************************************************************************/
void HK_ProcessIncomingHkData (CFE_SB_MsgPtr_t MessagePtr);


/*****************************************************************************/
/**
** \brief Validate Housekeeping Copy Table
**
** \par Description
**      This routine is called from CFE_TBL_Register.  It determines whether
**      the data contained in the new table is acceptable.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblPtr    A pointer to the new table data. 
**
** \returns
** \retstmt Zero indicates acceptable table, non-zero indicates unacceptable table \endcode
** \endreturns
**
** \sa #HK_TableInit
**
******************************************************************************/
int32 HK_ValidateHkCopyTable (void * TblPtr);


/*****************************************************************************/
/**
** \brief Process New Copy Table
**
** \par Description
**        Upon the arrival of a new HK Copy Table, this routine will
**        handle whatever is necessary to make this new data functional.       
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CpyTblPtr    A pointer to the first entry in the new copy table.
**
** \param[in]  RtTblPtr     A pointer to the first entry in the run-time table.
**
** \sa 
**
******************************************************************************/
void HK_ProcessNewCopyTable (hk_copy_table_entry_t * CpyTblPtr, 
                             hk_runtime_tbl_entry_t * RtTblPtr);

/*****************************************************************************/
/**
** \brief Tear Down Old Copy Table
**
** \par Description
**        This routine does what is necessary in order to remove the table data
**          from the system.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CpyTblPtr    A pointer to the first entry in the copy table.
**
** \param[in]  RtTblPtr     A pointer to the first entry in the run-time table. 
**
** \sa 
**
******************************************************************************/
void HK_TearDownOldCopyTable (hk_copy_table_entry_t * CpyTblPtr, 
                              hk_runtime_tbl_entry_t * RtTblPtr);


/*****************************************************************************/
/**
** \brief Send combined output message
**
** \par Description
**        This routine searches for the combined HK that contains the specified 
**        MID.  Once found, the packet is sent.  If not found, an event is 
**        generated. Also sets the data pieces for this output pkt
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  WhichMidToSend - the MsgId of the combined output message to send 
**
** \sa 
**
******************************************************************************/
void HK_SendCombinedHkPacket (CFE_SB_MsgId_t WhichMidToSend);


/*****************************************************************************/
/**
** \brief HK_CheckStatusOfTables
**
** \par Description
**        This is a high level routine that controls the actions taken by HK
**        when a copy table update is detected or a runtime table dump is 
**        pending 
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \sa 
**
******************************************************************************/
void HK_CheckStatusOfTables (void);


/*****************************************************************************/
/**
** \brief Check for Missing Data
**
** \par Description
**        This routine checks for missing data for the given output message.
**        It returns #HK_MISSING_DATA_DETECTED at the first piece of data that
**        is not present. The missing Input MsgId is sent back to the caller
**        through the given pointer named MissingInputMid.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  OutPktToCheck     MsgId of the combined output message to check
**
** \param[in]  MissingInputMid   A pointer to the caller provided MsgId variable
**
** \param[out] *MissingInputMid  The value of the missing input MsgId  
**
** \returns
** \retcode #HK_MISSING_DATA_DETECTED  \retdesc \copydoc HK_MISSING_DATA_DETECTED \endcode
** \retcode #HK_NO_MISSING_DATA        \retdesc \copydoc HK_NO_MISSING_DATA \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int32 HK_CheckForMissingData(CFE_SB_MsgId_t OutPktToCheck, 
                             CFE_SB_MsgId_t *MissingInputMid);


/*****************************************************************************/
/**
** \brief Set Data Present Flags to 'Not Present'
**
** \par Description
**        This routine will set the data present flags to data-not-present for 
**        given combined output message
**  
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  OutPkt    The MsgId whose data present flags will be set. 
**
** \sa 
**
******************************************************************************/
void HK_SetFlagsToNotPresent(CFE_SB_MsgId_t OutPkt);


#endif      /* _hk_utils_h_ */

/************************/
/*  End of File Comment */
/************************/
