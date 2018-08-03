/************************************************************************
** File:
**   $Id: mm_app.h 1.8 2015/04/06 15:41:02EDT lwalling Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   Unit specification for the Core Flight System (CFS) 
**   Memory Manger (MM) Application.  
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_app.h  $
**   Revision 1.8 2015/04/06 15:41:02EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.7 2015/03/30 17:33:56EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.6 2015/03/20 14:16:53EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.5 2015/03/02 14:26:41EST sstrege 
**   Added copyright information
**   Revision 1.4 2008/09/05 14:24:51EDT dahardison 
**   Updated declaration of local HK variables
**   Revision 1.3 2008/05/19 15:22:56EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_app_
#define _mm_app_

/************************************************************************
** Includes
*************************************************************************/
#include "mm_msg.h"
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MM Command Pipe Parameters */ 
/** \{ */
#define MM_CMD_PIPE_DEPTH   12
#define MM_HK_LIMIT          2
#define MM_CMD_LIMIT         4    
/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief MM global data structure
*/
typedef struct          
{ 
   MM_HkPacket_t        HkPacket;        /**< \brief Housekeeping telemetry packet  */
   
   CFE_SB_MsgPtr_t      MsgPtr;          /**< \brief Pointer to command message     */
   CFE_SB_PipeId_t      CmdPipe;         /**< \brief Command pipe ID                */
   
   uint32               RunStatus;       /**< \brief Application run status         */
   
   char                 PipeName[16];    /**< \brief Command pipe name              */
   uint16               PipeDepth;       /**< \brief Command pipe message depth     */
   
   uint8                LimitHK;         /**< \brief Houskeeping messages limit     */
   uint8                LimitCmd;        /**< \brief Command messages limit         */

   uint8                CmdCounter;      /**< \brief MM Application Command Counter       */
   uint8                ErrCounter;      /**< \brief MM Application Command Error Counter */
   uint8                LastAction;      /**< \brief Last command action executed         */
   uint8                MemType;         /**< \brief Memory type for last command         */
   uint32               Address;         /**< \brief Fully resolved address used for last 
                                                     command                              */
   uint32               DataValue;       /**< \brief Last command data value -- may be 
                                                     fill pattern or peek/poke value      */    
   uint32               BytesProcessed;  /**< \brief Bytes processed for last command     */
   
   char                 FileName[OS_MAX_PATH_LEN];   /**< \brief Name of the data file 
                                                                 used for last command, 
                                                                 where applicable         */
   
   uint32         LoadBuffer[MM_MAX_LOAD_DATA_SEG / 4];  /**< \brief Load file i/o buffer */
   uint32         DumpBuffer[MM_MAX_DUMP_DATA_SEG / 4];  /**< \brief Dump file i/o buffer */
   uint32         FillBuffer[MM_MAX_FILL_DATA_SEG / 4];  /**< \brief Fill memory buffer   */
   
} MM_AppData_t;           

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS Memory Manager (MM) application entry point
**  
**  \par Description
**       Memory Manager application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
*************************************************************************/
void MM_AppMain(void);

#endif /* _mm_app_ */

/************************/
/*  End of File Comment */
/************************/
