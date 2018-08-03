/************************************************************************
** File:
**   $Id: lc_app.h 1.2 2015/03/04 16:09:50EST sstrege Exp  $
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
**   Unit specification for the Core Flight System (CFS) 
**   Limit Checker (LC) Application.  
**
** Notes:
**
**   $Log: lc_app.h  $
**   Revision 1.2 2015/03/04 16:09:50EST sstrege 
**   Added copyright information
**   Revision 1.1 2012/07/31 16:53:37EDT nschweis 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lcx/fsw/src/project.pj
**   Revision 1.6 2011/06/08 16:06:15EDT lwalling 
**   Removed WPsInUse and subscription vars, added hash table definitions and link data arrays
**   Revision 1.5 2011/03/10 14:12:57EST lwalling 
**   Cleanup use of debug events during task startup
**   Revision 1.4 2011/03/01 09:35:47EST lwalling 
**   Modified startup logic re use of CDS and critical tables
**   Revision 1.3 2008/12/10 10:58:35EST dahardis 
**   Modified header includes to include lc_tbl.h
**   instead of lc_tbldefs.h
**   Revision 1.2 2008/12/03 13:59:40EST dahardis 
**   Corrections from peer code review
**   Revision 1.1 2008/10/29 14:18:59EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/lc/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _lc_app_
#define _lc_app_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "lc_msg.h"
#include "lc_tbl.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name LC Table Name Strings */ 
/** \{ */
#define LC_WDT_TABLENAME    "LC_WDT"
#define LC_ADT_TABLENAME    "LC_ADT"
#define LC_WRT_TABLENAME    "LC_WRT"
#define LC_ART_TABLENAME    "LC_ART"
/** \} */

/**
** \name LC CDS Buffer Strings */ 
/** \{ */
#define LC_WRT_CDSNAME      "LC_CDS_WRT"
#define LC_ART_CDSNAME      "LC_CDS_ART"
#define LC_APPDATA_CDSNAME  "LC_CDS_AppData"
/** \} */

/**
** \name LC Command Pipe Parameters */ 
/** \{ */
#define LC_PIPE_NAME        "LC_CMD_PIPE"
/** \} */

/**
** \name Table and CDS Initialization Results */ 
/** \{ */
#define LC_CDS_ENABLED         0x00000001
#define LC_CDS_CREATED         0x00000002
#define LC_CDS_RESTORED        0x00000004
#define LC_CDS_UPDATED         0x00000008

#define LC_WRT_DEFAULT_DATA    0x00000010
#define LC_WRT_CDS_CREATED     0x00000020
#define LC_WRT_CDS_RESTORED    0x00000040
#define LC_WRT_TBL_CREATED     0x00000080

#define LC_ART_DEFAULT_DATA    0x00000100
#define LC_ART_CDS_CREATED     0x00000200
#define LC_ART_CDS_RESTORED    0x00000400
#define LC_ART_TBL_CREATED     0x00000800

#define LC_APP_DEFAULT_DATA    0x00001000
#define LC_APP_CDS_CREATED     0x00002000
#define LC_APP_CDS_RESTORED    0x00004000

#define LC_WDT_DEFAULT_TBL     0x00010000
#define LC_WDT_CRITICAL_TBL    0x00020000
#define LC_WDT_TBL_RESTORED    0x00040000
#define LC_WDT_NOT_CRITICAL    0x00080000

#define LC_ADT_DEFAULT_TBL     0x00100000
#define LC_ADT_CRITICAL_TBL    0x00200000
#define LC_ADT_TBL_RESTORED    0x00400000
#define LC_ADT_NOT_CRITICAL    0x00800000
/** \} */

/**  \name Hash table definitions - presumes MessageID as hash function input */
#define LC_HASH_TABLE_ENTRIES  256
#define LC_HASH_TABLE_MASK     0x00FF

/**  \brief Linked list of Watchpoints that reference the same MessageID */
typedef struct LC_WListTag
{
    struct LC_WListTag *Next;          /**< \brief Next linked list element */

    uint16              WatchIndex;    /**< \brief Watchpoint table index   */
    uint16              Spare;         /**< \brief Structure alignment pad  */

} LC_WatchPtList_t;

/**  \brief Linked list of MessageID's with same hash function result */
typedef struct LC_MListTag
{
    struct LC_MListTag *Next;          /**< \brief Next linked list element */

    CFE_SB_MsgId_t      MessageID;     /**< \brief MessageID for this link  */
    uint16              Spare;         /**< \brief Structure alignment pad  */

    LC_WatchPtList_t   *WatchPtList;   /**< \brief Watchpoint list for this MessageID */

} LC_MessageList_t;


/************************************************************************
** Type Definitions
*************************************************************************/
/*
** The following data is never saved to the CDS
** even when the app is configured to use it
*/
/** 
**  \brief LC Operational Data Structure
*/
typedef struct          
{ 
    CFE_SB_MsgPtr_t      MsgPtr;             /**< \brief Pointer to command message           */
    CFE_SB_PipeId_t      CmdPipe;            /**< \brief Command pipe ID                      */

    LC_WDTEntry_t        *WDTPtr;            /**< \brief Watchpoint  Definition Table Pointer */
    LC_ADTEntry_t        *ADTPtr;            /**< \brief Actionpoint Definition Table Pointer */
   
    LC_WRTEntry_t        *WRTPtr;            /**< \brief Watchpoint  Results Table Pointer    */
    LC_ARTEntry_t        *ARTPtr;            /**< \brief Actionpoint Results Table Pointer    */
   
    CFE_TBL_Handle_t     WDTHandle;          /**< \brief Watchpoint  Definition Table Handle  */
    CFE_TBL_Handle_t     ADTHandle;          /**< \brief Actionpoint Definition Table Handle  */
   
    CFE_TBL_Handle_t     WRTHandle;          /**< \brief Watchpoint  Results Table Handle     */
    CFE_TBL_Handle_t     ARTHandle;          /**< \brief Actionpoint Results Table Handle     */
   
    CFE_ES_CDSHandle_t   WRTDataCDSHandle;   /**< \brief Watchpoint  Results Table CDS Handle */
    CFE_ES_CDSHandle_t   ARTDataCDSHandle;   /**< \brief Actionpoint Results Table CDS Handle */
 
    CFE_ES_CDSHandle_t   AppDataCDSHandle;   /**< \brief Application Data CDS Handle          */
    
    uint16               WatchpointCount;    /**< \brief Count of in-use watchpoints defined
                                                         in the Watchpoint Definition Table   */
    
    uint16               MessageIDsCount;    /**< \brief Count of unique MessageIDs referenced
                                                         in the Watchpoint Definition Table   */
    
    LC_HkPacket_t        HkPacket;           /**< \brief Housekeeping telemetry packet        */
   
    uint32               TableResults;       /**< \brief Table and CDS initialization results */

    LC_MessageList_t    *HashTable[LC_HASH_TABLE_ENTRIES]; /** \brief Each entry in the hash
                                                                      table is a linked list
                                                                      of all the MessageID's
                                                                      that the hash function
                                                                      converts to each index  */

    LC_MessageList_t     MessageLinks[LC_MAX_WATCHPOINTS]; /** \brief Linked list elements    */
    LC_WatchPtList_t     WatchPtLinks[LC_MAX_WATCHPOINTS]; /** \brief Linked list elements    */

    boolean              HaveActiveCDS;      /**< \brief Critical Data Store in use flag      */

} LC_OperData_t;           

/*
** The following structure holds data that is saved and restored
** from the CDS when the app is configured to use it
*/
/** 
**  \brief LC Application Data Structure
*/
typedef struct          
{ 
    
    uint16         CmdCount;               /**< \brief Command Counter                        */        
    uint16         CmdErrCount;            /**< \brief Command Error Counter                  */ 

    uint32         APSampleCount;          /**< \brief Total count of Actionpoints sampled    */
    uint32         MonitoredMsgCount;      /**< \brief Total count of messages monitored      */
    uint32         RTSExecCount;           /**< \brief Total count of RTS sequences initiated */
    uint16         PassiveRTSExecCount;    /**< \brief Total count of RTS sequences not
                                                       initiated because the LC state is 
                                                       set to #LC_STATE_PASSIVE or the state
                                                       of the actionpoint that failed is set to
                                                       #LC_APSTATE_PASSIVE                    */
   
    uint16         CDSSavedOnExit;         /**< \brief Variable that tells us if we exited
                                                       clean or not                           */
    
    uint8          CurrentLCState;         /**< \brief Current LC application operating state */

} LC_AppData_t;           

/************************************************************************
** Exported Data
*************************************************************************/
extern LC_OperData_t    LC_OperData;
extern LC_AppData_t     LC_AppData;           

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS Limit Checker (LC) application entry point
**  
**  \par Description
**       Limit Checker application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
*************************************************************************/
void LC_AppMain(void);

#endif /* _lc_app_ */

/************************/
/*  End of File Comment */
/************************/
