/************************************************************************
** File:
**   $Id: mm_msg.h 1.9 2015/03/20 14:16:54EDT lwalling Exp  $
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
**   Specification for the CFS Memory Manager command and telemetry 
**   message data types.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**   Constant and enumerated types related to these message structures
**   are defined in mm_msgdefs.h. They are kept separate to allow easy 
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in mm_msgdefs.h for more 
**   info).
**
**   $Log: mm_msg.h  $
**   Revision 1.9 2015/03/20 14:16:54EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.8 2015/03/02 14:26:35EST sstrege 
**   Added copyright information
**   Revision 1.7 2010/11/29 08:46:50EST jmdagost 
**   Added support for EEPROM write-enable/disable commands
**   Revision 1.6 2010/11/24 17:04:37EST jmdagost 
**   Add filename argument to symbol table dump command structure
**   Revision 1.5 2008/09/06 15:00:50EDT dahardison 
**   Updated to support the symbol lookup ground command
**   Revision 1.4 2008/05/22 15:08:41EDT dahardison 
**   Changed inclusion of cfs_lib.h to cfs_utils.h
**   Moved #defines to mm_msgdefs.h so they can be included
**   in ASIST RDL files.
**   Revision 1.3 2008/05/19 15:23:33EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_msg_
#define _mm_msg_

/************************************************************************
** Includes
*************************************************************************/
#include "mm_platform_cfg.h"
#include "cfs_utils.h"
#include "cfe.h"
#include "mm_msgdefs.h"

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief No Arguments Command
**  For command details see #MM_NOOP_CC, #MM_RESET_CC
*/
typedef struct
{
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

} MM_NoArgsCmd_t;

/** 
**  \brief Memory Peek Command
**  For command details see #MM_PEEK_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint8             DataSize;	      /**< \brief Size of the data to be read     */
    uint8             MemType;         /**< \brief Memory type to peek data from   */
    uint8             Padding[2];      /**< \brief Structure padding               */
    CFS_SymAddr_t     SrcSymAddress;   /**< \brief Symbolic source peek address    */

} MM_PeekCmd_t;

/** 
**  \brief Memory Poke Command
**  For command details see #MM_POKE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint8             DataSize;        /**< \brief Size of the data to be written     */
    uint8             MemType;         /**< \brief Memory type to poke data to        */
    uint8             Padding[2];      /**< \brief Structure padding                  */
    uint32            Data;            /**< \brief Data to be written                 */
    CFS_SymAddr_t     DestSymAddress;  /**< \brief Symbolic destination poke address  */

} MM_PokeCmd_t;

/** 
**  \brief Memory Load With Interrupts Disabled Command
**  For command details see #MM_LOAD_MEM_WID_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint8             NumOfBytes;                        /**< \brief Number of bytes to be loaded       */
    uint8             Padding[3];                        /**< \brief Structure padding                  */
    uint32            Crc;                               /**< \brief Data check value                   */
    CFS_SymAddr_t     DestSymAddress;                    /**< \brief Symbolic destination load address  */
    uint8             DataArray[MM_MAX_UNINTERRUPTABLE_DATA];   /**< \brief Data to be loaded           */

} MM_LoadMemWIDCmd_t;

/** 
**  \brief Dump Memory In Event Message Command
**  For command details see #MM_DUMP_IN_EVENT_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint8             MemType;          /**< \brief Memory dump type             */
    uint8             NumOfBytes;       /**< \brief Number of bytes to be dumped */
    uint16            Padding;          /**< \brief Structure padding            */
    CFS_SymAddr_t     SrcSymAddress;    /**< \brief Symbolic source address      */
    
} MM_DumpInEventCmd_t;

/** 
**  \brief Memory Load From File Command
**  For command details see #MM_LOAD_MEM_FROM_FILE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    char              FileName[OS_MAX_PATH_LEN];          /**< \brief Name of memory load file */

} MM_LoadMemFromFileCmd_t;

/** 
**  \brief Memory Dump To File Command
**  For command details see #MM_DUMP_MEM_TO_FILE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];

    uint8             MemType;                           /**< \brief Memory dump type */
    uint8             Padding[3];                        /**< \brief Structure padding */
    uint32            NumOfBytes;                        /**< \brief Number of bytes to be dumped */
    CFS_SymAddr_t     SrcSymAddress;                     /**< \brief Symbol plus optional offset  */
    char              FileName[OS_MAX_PATH_LEN];         /**< \brief Name of memory dump file */

} MM_DumpMemToFileCmd_t;

/** 
**  \brief Memory Fill Command
**  For command details see #MM_FILL_MEM_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8             MemType;                           /**< \brief Memory type                  */
    uint8             Padding[3];                        /**< \brief Structure padding            */
    uint32            NumOfBytes;                        /**< \brief Number of bytes to fill      */
    uint32            FillPattern;                       /**< \brief Fill pattern to use          */
    CFS_SymAddr_t     DestSymAddress;                    /**< \brief Symbol plus optional offset  */

} MM_FillMemCmd_t;

/** 
**  \brief Symbol Table Lookup Command
**  For command details see #MM_LOOKUP_SYM_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char              SymName[OS_MAX_SYM_LEN];           /**< \brief Symbol name string           */

} MM_LookupSymCmd_t;

/** 
**  \brief Save Symbol Table To File Command
**  For command details see #MM_SYMTBL_TO_FILE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char              FileName[OS_MAX_PATH_LEN];         /**< \brief Name of symbol dump file */

} MM_SymTblToFileCmd_t;

/** 
**  \brief EEPROM Write Enable Command
**  For command details see #MM_ENABLE_EEPROM_WRITE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32            Bank;                              /**< \brief EEPROM bank number to write-enable */

} MM_EepromWriteEnaCmd_t;

/** 
**  \brief EEPROM Write Disable Command
**  For command details see #MM_DISABLE_EEPROM_WRITE_CC
*/
typedef struct {
    uint8             CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32            Bank;                              /**< \brief EEPROM bank number to write-disable */

} MM_EepromWriteDisCmd_t;

/** 
**  \mmtlm Housekeeping Packet Structure
*/
typedef struct
{
    uint8             TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< \brief cFE SB Tlm Msg Hdr */
    
    uint8             CmdCounter;                      /**< \mmtlmmnemonic \MM_CMDPC
                                                            \brief MM Application Command Counter */
    uint8             ErrCounter;		                /**< \mmtlmmnemonic \MM_CMDEC
                                                            \brief MM Application Command Error Counter */
    uint8             LastAction;                      /**< \mmtlmmnemonic \MM_LASTACTION
                                                            \brief Last command action executed */
    uint8             MemType;                         /**< \mmtlmmnemonic \MM_MEMTYPE
                                                            \brief Memory type for last command */
    uint32            Address;                         /**< \mmtlmmnemonic \MM_ADDR
                                                            \brief Fully resolved address used for last command */
    uint32            DataValue;                       /**< \mmtlmmnemonic \MM_DATAVALUE
                                                            \brief Last command data value -- may be 
                                                             fill pattern or peek/poke value      */    
    uint32            BytesProcessed;                  /**< \mmtlmmnemonic \MM_BYTESPROCESSED
                                                            \brief Bytes processed for last command */
    char              FileName[OS_MAX_PATH_LEN];       /**< \mmtlmmnemonic \MM_FILENAME
                                                            \brief Name of the data file used for last command, where applicable */
} MM_HkPacket_t;

#endif /* _mm_msg_ */

/************************/
/*  End of File Comment */
/************************/

