/************************************************************************
** File:
**   $Id: mm_msgdefs.h 1.9 2015/04/01 14:50:28EDT sstrege Exp  $
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
**   message constant definitions.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**   These Macro definitions have been put in this file (instead of 
**   mm_msg.h) so this file can be included directly into ASIST build 
**   test scripts. ASIST RDL files can accept C language #defines but 
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE! 
**   ADD THEM TO mm_msg.h IF NEEDED! 
**
**   $Log: mm_msgdefs.h  $
**   Revision 1.9 2015/04/01 14:50:28EDT sstrege 
**   Added criticality information to doxygen command descriptions
**   Revision 1.8 2015/03/30 17:33:45EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.7 2015/03/02 14:26:40EST sstrege 
**   Added copyright information
**   Revision 1.6 2010/12/08 14:29:38EST jmdagost 
**   Corrected typos in doxygen links
**   Revision 1.5 2010/11/29 08:46:52EST jmdagost 
**   Added support for EEPROM write-enable/disable commands
**   Revision 1.4 2010/11/24 17:06:29EST jmdagost 
**   Update ducumentation for Save Symbol Table To File command.
**   Revision 1.3 2009/04/18 15:29:36EDT dkobe 
**   Corrected doxygen comments
**   Revision 1.2 2008/09/06 15:00:40EDT dahardison 
**   Updated to support the symbol lookup ground command
**   Revision 1.1 2008/05/22 15:16:48EDT dahardison 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/mm/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _mm_msgdefs_
#define _mm_msgdefs_

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MM Data Sizes for Peeks and Pokes */ 
/** \{ */
#define MM_BYTE_BIT_WIDTH      8
#define MM_WORD_BIT_WIDTH     16
#define MM_DWORD_BIT_WIDTH    32
/** \} */

/**
** \name MM Memory Types */ 
/** \{ */
#define MM_NOMEMTYPE   0        /**< \brief Used to indicate that no memtype specified          */
#define MM_RAM         1        /**< \brief Normal RAM, no special access required              */
#define MM_EEPROM      2        /**< \brief EEPROM, requires special access for writes          */
#define MM_MEM8        3        /**< \brief Optional memory type that is only 8-bit read/write  */
#define MM_MEM16       4        /**< \brief Optional memory type that is only 16-bit read/write */
#define MM_MEM32       5        /**< \brief Optional memory type that is only 32-bit read/write */
/** \} */

/**
** \name Misc Initialization Values */ 
/** \{ */
#define MM_CLEAR_SYMNAME    '\0'    /**< \brief Used to clear out symbol name strings      */
#define MM_CLEAR_FNAME      '\0'    /**< \brief Used to clear out file name strings        */
#define MM_CLEAR_ADDR         0     /**< \brief Used to clear out memory address variables */
#define MM_CLEAR_PATTERN      0     /**< \brief Used to clear out fill and test patterns   */
/** \} */

/**
** \name HK MM Last Action Identifiers */ 
/** \{ */
#define MM_NOACTION            0     /**< \brief Used to clear out HK action variable       */
#define MM_PEEK                1     
#define MM_POKE                2     
#define MM_LOAD_FROM_FILE      3
#define MM_LOAD_WID            4
#define MM_DUMP_TO_FILE        5
#define MM_DUMP_INEVENT        6
#define MM_FILL                7
#define MM_SYM_LOOKUP          8
#define MM_SYMTBL_SAVE         9
#define MM_EEPROMWRITE_ENA     10
#define MM_EEPROMWRITE_DIS     11
#define MM_NOOP                12
#define MM_RESET               13
/** \} */

/** \mmcmd Noop 
**  
**  \par Description
**       Implements the Noop command that insures the MM task is alive
**
**  \mmcmdmnemonic \MM_NOOP
**
**  \par Command Structure
**       #MM_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC - command counter will increment
**       - The #MM_NOOP_INF_EID informational event message will be 
**         generated when the command is received
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #MM_RESET_CC
*/
#define MM_NOOP_CC                     0

/** \mmcmd Reset Counters
**  
**  \par Description
**       Resets the MM housekeeping counters
**
**  \mmcmdmnemonic \MM_RESETCTRS
**
**  \par Command Structure
**       #MM_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC - command counter will be cleared
**       - \b \c \MM_CMDEC - command error counter will be cleared
**       - The #MM_RESET_DBG_EID informational event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #MM_NOOP_CC
*/
#define MM_RESET_CC                    1    

/** \mmcmd Memory Peek
**  
**  \par Description
**       Reads 8,16, or 32 bits of data from any given input address
**
**  \mmcmdmnemonic \MM_PEEK
**
**  \par Command Structure
**       #MM_PeekCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_PEEK
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved destination 
**                                memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the byte size of the
**                                    peek operation (1, 2, or 4) 
**       - The #MM_PEEK_BYTE_INF_EID informational event message will
**         be generated with the peek data if the data size was 8 bits
**       - The #MM_PEEK_WORD_INF_EID informational event message will
**         be generated with the peek data if the data size was 16 bits
**       - The #MM_PEEK_DWORD_INF_EID informational event message will
**         be generated with the peek data if the data size was 32 bits
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - A symbol name was specified that can't be resolved
**       - The specified data size is invalid
**       - The specified memory type is invalid
**       - The address range fails validation check
**       - The address and data size are not properly aligned
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BITS_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i> DestSymAddress </i> and
**       <i> MemType </i> in the command.  It is possible to generate a machine check 
**       exception when accessing I/O memory addresses/registers and other types of memory.  
**       The user is cautioned to use extreme care. 
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
**  \sa #MM_POKE_CC
*/
#define MM_PEEK_CC                     2

/** \mmcmd Memory Poke
**  
**  \par Description
**       Writes 8, 16, or 32 bits of data to any memory address
**
**  \mmcmdmnemonic \MM_POKE
**
**  \par Command Structure
**       #MM_PokeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_POKE
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved source 
**                                memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the byte size of the
**                                    poke operation (1, 2, or 4) 
**       - The #MM_POKE_BYTE_INF_EID informational event message will
**         be generated if the data size was 8 bits
**       - The #MM_POKE_WORD_INF_EID informational event message will
**         be generated if the data size was 16 bits
**       - The #MM_POKE_DWORD_INF_EID informational event message will
**         be generated if the data size was 32 bits
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - A symbol name was specified that can't be resolved
**       - The specified data size is invalid
**       - The specified memory type is invalid
**       - The address range fails validation check
**       - The address and data size are not properly aligned
**       - An EEPROM write error occured
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BITS_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**       - Error specific event message #MM_OS_EEPROMWRITE8_ERR_EID
**       - Error specific event message #MM_OS_EEPROMWRITE16_ERR_EID
**       - Error specific event message #MM_OS_EEPROMWRITE32_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i>DestSymAddress</i>, 
**       <i>MemType</i>, and <i>Data</i> in the command.  It is highly recommended 
**       to verify the success or failure of the memory poke.  The poke may be verified 
**       by issuing a subsequent peek command and evaluating the returned value.  It is 
**       possible to destroy critical information with this command causing unknown 
**       consequences. In addition, it is possible to generate a machine check exception 
**       when accessing I/O memory addresses/registers and other types of memory. The user 
**       is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
**  \sa #MM_PEEK_CC
*/
#define MM_POKE_CC                     3

/** \mmcmd Memory Load With Interrupts Disabled
**  
**  \par Description
**       Reprogram processor memory with input data.  Loads up to 
**       #MM_MAX_UNINTERRUPTABLE_DATA data bytes into RAM with 
**       interrupts disabled 
**
**  \mmcmdmnemonic \MM_LOADWID
**
**  \par Command Structure
**       #MM_LoadMemWIDCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_LOAD_WID
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                destination memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the number of bytes
**                                    loaded
**       - The #MM_LOAD_WID_INF_EID information event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - A symbol name was specified that can't be resolved
**       - The computed CRC doesn't match the command message value
**       - The address range fails validation check
**       - Invalid data size specified in command message
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_LOAD_WID_CRC_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BYTES_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i>DestSymAddress</i>, 
**       <i>NumOfBytes</i>, and <i>DataArray</i> contents in the command.  It is 
**       highly recommended to verify the success or failure of the memory load.  The 
**       load may be verified by dumping memory and evaluating the dump contents.  It 
**       is possible to destroy critical information with this command causing unknown 
**       consequences. In addition, it is possible to generate a machine check exception 
**       when accessing I/O memory addresses/registers and other types of memory. The 
**       user is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
*/
#define MM_LOAD_MEM_WID_CC             4

/** \mmcmd Memory Load From File
**  
**  \par Description
**       Reprograms processor memory with the data contained within the given 
**       input file 
**
**  \mmcmdmnemonic \MM_LOADFILE
**
**  \par Command Structure
**       #MM_LoadMemFromFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_LOAD_FROM_FILE
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                destination memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the number of bytes
**                                    loaded
**       - \b \c \MM_FILENAME   - will be set to the load file name
**       - The #MM_LD_MEM_FILE_INF_EID informational event message will
**         be generated 
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Command specified filename is invalid
**       - #OS_open call fails
**       - #OS_close call fails
**       - #OS_read doesn't read the expected number of bytes
**       - The #CFS_ComputeCRCFromFile call fails
**       - The computed CRC doesn't match the load file value
**       - A symbol name was specified that can't be resolved
**       - #CFE_FS_ReadHeader call fails
**       - #OS_read call fails 
**       - The address range fails validation check
**       - The specified data size is invalid
**       - The address and data size are not properly aligned
**       - The specified memory type is invalid
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_CMD_FNAME_ERR_EID
**       - Error specific event message #MM_OS_OPEN_ERR_EID
**       - Error specific event message #MM_OS_CLOSE_ERR_EID
**       - Error specific event message #MM_OS_READ_EXP_ERR_EID
**       - Error specific event message #MM_CFS_COMPUTECRCFROMFILE_ERR_EID
**       - Error specific event message #MM_LOAD_FILE_CRC_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_FILE_LOAD_PARAMS_ERR_EID
**       - Error specific event message #MM_CFE_FS_READHDR_ERR_EID
**       - Error specific event message #MM_OS_READ_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BYTES_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the  contents of the load file 
**       in the command.  It is highly recommended to verify the success or failure of 
**       the memory load.  The load may be verified by dumping memory and evaluating the 
**       dump contents.  It is possible to destroy critical information with this command 
**       causing unknown consequences. In addition, it is possible to generate a machine 
**       check exception when accessing I/O memory addresses/registers and other types of 
**       memory. The user is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
**  \sa #MM_DUMP_MEM_TO_FILE_CC
*/
#define MM_LOAD_MEM_FROM_FILE_CC       5    

/** \mmcmd Memory Dump To File
**  
**  \par Description
**       Dumps the input number of bytes from processor memory 
**       to a file 
**
**  \mmcmdmnemonic \MM_DUMPFILE
**
**  \par Command Structure
**       #MM_DumpMemToFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_DUMP_TO_FILE
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                source memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the number of bytes
**                                    dumped
**       - \b \c \MM_FILENAME   - will be set to the dump file name
**       - The #MM_DMP_MEM_FILE_INF_EID informational event message will
**         be generated 
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Command specified filename is invalid
**       - A symbol name was specified that can't be resolved
**       - #OS_creat call fails
**       - #CFE_FS_WriteHeader call fails
**       - #OS_close call fails
**       - #OS_write doesn't write the expected number of bytes
**         or returns an error code
**       - The #CFS_ComputeCRCFromFile call fails
**       - The address range fails validation check
**       - The specified data size is invalid
**       - The address and data size are not properly aligned
**       - The specified memory type is invalid
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_CMD_FNAME_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_OS_CREAT_ERR_EID
**       - Error specific event message #MM_CFE_FS_WRITEHDR_ERR_EID
**       - Error specific event message #MM_OS_CLOSE_ERR_EID
**       - Error specific event message #MM_OS_WRITE_EXP_ERR_EID
**       - Error specific event message #MM_CFS_COMPUTECRCFROMFILE_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BYTES_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i>SrcSymAddress</i>,
**       <i>NumOfBytes</i>, and <i>MemType</i> in the command.  It is possible to 
**       generate a machine check exception when accessing I/O memory addresses/registers 
**       and other types of memory.  The user is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
**  \sa #MM_LOAD_MEM_FROM_FILE_CC
*/
#define MM_DUMP_MEM_TO_FILE_CC         6

/** \mmcmd Dump In Event Message
**  
**  \par Description
**       Dumps up to #MM_MAX_DUMP_INEVENT_BYTES of memory in an event message 
**
**  \mmcmdmnemonic \MM_DUMPEVT
**
**  \par Command Structure
**       #MM_DumpInEventCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_DUMP_INEVENT
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                source memory address
**       - \b \c \MM_BYTESPROCESSED - will be set to the number of bytes
**                                    dumped
**       - The #MM_DUMP_INEVENT_INF_EID informational event message will
**         be generated with the dump data
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - A symbol name was specified that can't be resolved
**       - The address range fails validation check
**       - The specified data size is invalid
**       - The address and data size are not properly aligned
**       - The specified memory type is invalid
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BYTES_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i>SrcSymAddress</i>,
**       <i>NumOfBytes</i>, and <i>MemType</i> in the command.  It is possible to 
**       generate a machine check exception when accessing I/O memory addresses/registers 
**       and other types of memory.  The user is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
*/
#define MM_DUMP_IN_EVENT_CC            7

/** \mmcmd Memory Fill 
**  
**  \par Description
**       Reprograms processor memory with the fill pattern contained 
**       within the command message 
**
**  \mmcmdmnemonic \MM_FILL
**
**  \par Command Structure
**       #MM_FillMemCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_FILL
**       - \b \c \MM_MEMTYPE    - will be set to the commanded memory type
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                destination memory address
**       - \b \c  MM_FILLPATTERN    - will be set to the fill pattern used
**       - \b \c \MM_BYTESPROCESSED - will be set to the number of bytes
**                                    filled
**       - The #MM_FILL_INF_EID informational event message will
**         be generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - A symbol name was specified that can't be resolved
**       - The address range fails validation check
**       - The specified data size is invalid
**       - The address and data size are not properly aligned
**       - The specified memory type is invalid
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**       - Error specific event message #MM_OS_MEMVALIDATE_ERR_EID
**       - Error specific event message #MM_DATA_SIZE_BYTES_ERR_EID
**       - Error specific event message #MM_ALIGN32_ERR_EID
**       - Error specific event message #MM_ALIGN16_ERR_EID
**       - Error specific event message #MM_MEMTYPE_ERR_EID
**
**  \par Criticality
**       It is the responsibility of the user to verify the <i>DestSymAddress</i>, 
**       and <i>NumOfBytes</i> in the command.  It is highly recommended to verify 
**       the success or failure of the memory fill.  The fill may be verified by 
**       dumping memory and evaluating the dump contents.  It is possible to destroy 
**       critical information with this command causing unknown consequences.  In 
**       addition, it is possible to generate a machine check exception when accessing 
**       I/O memory addresses/registers and other types of memory. The user 
**       is cautioned to use extreme care.
**
**       Note: Valid memory ranges are defined within a hardcoded structure contained in the 
**       PSP layer (CFE_PSP_MemoryTable) however, not every address within the defined ranges
**       may be valid.
**
*/
#define MM_FILL_MEM_CC                 8    

/** \mmcmd Symbol Table Lookup
**  
**  \par Description
**       Queries the system symbol table and reports the resolved address
**       in telemetry and an informational event message
**
**  \mmcmdmnemonic \MM_GETADDRFROMSYMBOL
**
**  \par Command Structure
**       #MM_LookupSymCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_SYM_LOOKUP
**       - \b \c \MM_ADDR       - will be set to the fully resolved 
**                                memory address
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - An empty string was specified as the symbol name
**       - A symbol name was specified that can't be resolved
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMNAME_NUL_ERR_EID
**       - Error specific event message #MM_SYMNAME_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #MM_SYMTBL_TO_FILE_CC
*/
#define MM_LOOKUP_SYM_CC               9

/** \mmcmd Save Symbol Table To File
**  
**  \par Description
**       Saves the system symbol table to a file that can be transfered
**       to the ground
**
**  \mmcmdmnemonic \MM_WRITESYMTBL2FILE
**
**  \par Command Structure
**       #MM_SymTblToFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_SYMTBL_SAVE
**       - \b \c \MM_FILENAME - will be set to the dump file name
**       - The #MM_SYMTBL_TO_FILE_INF_EID informational event message will
**         be generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - An empty string was specified as the dump filename
**       - The OSAL returns a status other than success to the command
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_SYMFILENAME_NUL_ERR_EID
**       - Error specific event message #MM_SYMTBL_TO_FILE_FAIL_ERR_EID
**
**  \par Note:
**       - Dump filenames #OS_MAX_PATH_LEN characters or longer are truncated
**
**  \par Criticality
**       None
**
**  \sa #MM_LOOKUP_SYM_CC
*/
#define MM_SYMTBL_TO_FILE_CC          10

/** \mmcmd EEPROM Write Enable
**  
**  \par Description
**       Enables writing to a specified EEPROM bank
**
**  \mmcmdmnemonic \MM_EEPROMWRITEENABLE
**
**  \par Command Structure
**       #MM_EepromWriteEnaCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_EEPROMWRITE_ENA
**       - The #MM_EEPROM_WRITE_ENA_INF_EID informational event message will
**         be generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Non-success return status from PSP write enable
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_EEPROM_WRITE_ENA_ERR_EID
**
**  \par Criticality
**       Extreme caution is advised in the use of this command. It is intended to be 
**       used only as a maintence tool for patching the default FSW image. This command 
**       will leave the EEPROM bank in a very vulnerable state. Once a patch has been 
**       completed the #MM_DISABLE_EEPROM_WRITE_CC command must be issued to protect the 
**       EEPROM bank from being inadvertently written.
**
**  \sa #MM_DISABLE_EEPROM_WRITE_CC
*/
#define MM_ENABLE_EEPROM_WRITE_CC          11

/** \mmcmd EEPROM Write Disable
**  
**  \par Description
**       Disables writing to a specified EEPROM bank
**
**  \mmcmdmnemonic \MM_EEPROMWRITEDISABLE
**
**  \par Command Structure
**       #MM_EepromWriteDisCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \MM_CMDPC      - command counter will increment
**       - \b \c \MM_LASTACTION - will be set to #MM_EEPROMWRITE_DIS
**       - The #MM_EEPROM_WRITE_DIS_INF_EID informational event message will
**         be generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Non-success return status from PSP write disable
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \MM_CMDEC - command error counter will increment
**       - Error specific event message #MM_LEN_ERR_EID
**       - Error specific event message #MM_EEPROM_WRITE_DIS_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #MM_ENABLE_EEPROM_WRITE_CC
*/
#define MM_DISABLE_EEPROM_WRITE_CC          12

#endif /* _mm_msgdefs_ */

/************************/
/*  End of File Comment */
/************************/
