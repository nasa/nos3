/************************************************************************
** File:
**   $Id: mm_events.h 1.13 2015/04/06 15:41:22EDT lwalling Exp  $
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
**   Specification for the CFS Memory Manger event identifers.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_events.h  $
**   Revision 1.13 2015/04/06 15:41:22EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.12 2015/03/02 14:26:33EST sstrege 
**   Added copyright information
**   Revision 1.11 2011/12/05 15:16:32EST jmdagost 
**   Added zero-bytes-read event message for memory loads from file.
**   Revision 1.10 2010/12/08 14:38:29EST jmdagost 
**   Added "bad filename" event to symbol table dump cmd, updated subsequent event numbers.
**   Revision 1.9 2010/11/29 08:47:12EST jmdagost 
**   Added support for EEPROM write-enable/disable commands
**   Revision 1.8 2010/11/24 17:07:24EST jmdagost 
**   Added event messages for Write Symbol Table To File command
**   Revision 1.7 2009/06/12 14:37:32EDT rmcgraw 
**   DCR82191:1 Changed OS_Mem function calls to CFE_PSP_Mem
**   Revision 1.6 2008/09/06 15:33:30EDT dahardison 
**   Modified doxygen comment blocks for new init and noop
**   event strings with version information
**   Revision 1.5 2008/09/06 15:01:09EDT dahardison 
**   Updated to support the symbol lookup ground command
**   Revision 1.4 2008/09/05 12:34:20EDT dahardison 
**   Added an event message for a housekeeping request with a bad message length
**   Revision 1.3 2008/05/19 15:23:05EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_events_
#define _mm_events_

/** \brief <tt> 'MM Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'MM Initialized. Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when the CFS Memory Manager has
**  completed initialization.
**
**  The \c Version fields contain the #MM_MAJOR_VERSION,
**  #MM_MINOR_VERSION, #MM_REVISION, and #MM_MISSION_REV
**  version identifiers. 
*/
#define MM_INIT_INF_EID                       1    

/** \brief <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
**  \event <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a NOOP command has been received.
**
**  The \c Version fields contain the #MM_MAJOR_VERSION,
**  #MM_MINOR_VERSION, #MM_REVISION, and #MM_MISSION_REV
**  version identifiers. 
*/
#define MM_NOOP_INF_EID                       2

/** \brief <tt> 'Reset counters command received' </tt>
**  \event <tt> 'Reset counters command received' </tt>
**  
**  \par Type: DEBUG
**
**  \par Cause:
**
**  This event message is issued when a reset counters command has
**  been received.    
*/
#define MM_RESET_DBG_EID                      3

/** \brief <tt> 'Load Memory WID Command: Wrote \%d bytes to address: 0x\%08X' </tt>
**  \event <tt> 'Load Memory WID Command: Wrote \%d bytes to address: 0x\%08X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a memory load with interrupts disabled
**  command has been executed.     
**
**  The \c bytes field identifies how many bytes were written, the \c address
**  field shows the fully resolved destination address of the load. 
*/
#define MM_LOAD_WID_INF_EID                   4

/** \brief <tt> 'Load Memory From File Command: Loaded \%d bytes to address 0x\%08X from file '\%s'' </tt>
**  \event <tt> 'Load Memory From File Command: Loaded \%d bytes to address 0x\%08X from file '\%s'' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a load memory from file command has
**  been executed.
**
**  The \c bytes field identifies how many bytes were written, the \c address
**  field shows the fully resolved destination address of the load, the \c file
**  field identifies the name of the file used for the load. 
*/
#define MM_LD_MEM_FILE_INF_EID                5

/** \brief <tt> 'Fill Memory Command: Filled \%d bytes at address: 0x\%08X with pattern: 0x\%08X' </tt>
**  \event <tt> 'Fill Memory Command: Filled \%d bytes at address: 0x\%08X with pattern: 0x\%08X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a fill memory command has been executed.
**
**  The \c bytes field identifies how many bytes were written, the \c address
**  field shows the fully resolved destination address of the fill, the \c pattern
**  field identifies the fill pattern used. 
*/
#define MM_FILL_INF_EID                       6

/** \brief <tt> 'Peek Command: Addr = 0x\%08X Size = 8 bits Data = 0x\%02X' </tt>
**  \event <tt> 'Peek Command: Addr = 0x\%08X Size = 8 bits Data = 0x\%02X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when an 8 bit memory peek command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the source 
**  memory location and the \c Data field contains the data read.
*/
#define MM_PEEK_BYTE_INF_EID                  7

/** \brief <tt> 'Peek Command: Addr = 0x\%08X Size = 16 bits Data = 0x\%04X' </tt>
**  \event <tt> 'Peek Command: Addr = 0x\%08X Size = 16 bits Data = 0x\%04X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a 16 bit memory peek command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the source 
**  memory location and the \c Data field contains the data read.
*/
#define MM_PEEK_WORD_INF_EID                  8

/** \brief <tt> 'Peek Command: Addr = 0x\%08X Size = 32 bits Data = 0x\%08X' </tt>
**  \event <tt> 'Peek Command: Addr = 0x\%08X Size = 32 bits Data = 0x\%08X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a 32 bit memory peek command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the source 
**  memory location and the \c Data field contains the data read.
*/
#define MM_PEEK_DWORD_INF_EID                 9

/** \brief <tt> 'Poke Command: Addr = 0x\%08X Size = 8 bits Data = 0x\%02X' </tt>
**  \event <tt> 'Poke Command: Addr = 0x\%08X Size = 8 bits Data = 0x\%02X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when an 8 bit memory poke command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the destination 
**  memory location and the \c Data field contains the data written.
*/
#define MM_POKE_BYTE_INF_EID                 10

/** \brief <tt> 'Poke Command: Addr = 0x\%08X Size = 16 bits Data = 0x\%04X' </tt>
**  \event <tt> 'Poke Command: Addr = 0x\%08X Size = 16 bits Data = 0x\%04X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when an 16 bit memory poke command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the destination 
**  memory location and the \c Data field contains the data written.
*/
#define MM_POKE_WORD_INF_EID                 11

/** \brief <tt> 'Poke Command: Addr = 0x\%08X Size = 32 bits Data = 0x\%08X' </tt>
**  \event <tt> 'Poke Command: Addr = 0x\%08X Size = 32 bits Data = 0x\%08X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when an 32 bit memory poke command has been
**  executed.
**
**  The \c Addr field shows the fully resolved address of the destination 
**  memory location and the \c Data field contains the data written.
*/
#define MM_POKE_DWORD_INF_EID                12

/** \brief <tt> 'Dump Memory To File Command: Dumped \%d bytes from address 0x\%08X to file '\%s'' </tt>
**  \event <tt> 'Dump Memory To File Command: Dumped \%d bytes from address 0x\%08X to file '\%s'' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a dump memory to file command has
**  been executed. 
**
**  The \c bytes field identifies how many bytes were read, the \c address
**  field shows the fully resolved source address of the dump, the \c file
**  field identifies the name of the file used for the dump. 
*/
#define MM_DMP_MEM_FILE_INF_EID              13

/** \brief <tt> 'Memory Dump: 0x\%02X from address: 0x\%08lX' </tt>
**  \event <tt> 'Memory Dump: 0x\%02X from address: 0x\%08lX' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued in response to a dump memory in event message
**  command.
**
**  The \c 0x\%02X field is a single byte of data and will be repeated according
**  to the requested number of dump bytes, the \c address field shows the fully 
**  resolved source address of the dump.
*/
#define MM_DUMP_INEVENT_INF_EID              14

/** \brief <tt> 'SB Pipe Read Error, App will exit. RC = 0x\%08X' </tt>
**  \event <tt> 'SB Pipe Read Error, App will exit. RC = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to #CFE_SB_RcvMsg fails.    
**
**  The \c RC field is the return code from the #CFE_SB_RcvMsg function
**  call that generated the error.
*/
#define MM_PIPE_ERR_EID                      15

/** \brief <tt> 'Invalid command pipe message ID: 0x\%X' </tt>
**  \event <tt> 'Invalid command pipe message ID: 0x\%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a software bus message is received
**  with an invalid message ID.
**
**  The \c message \c ID field contains the message ID that generated 
**  the error.
*/
#define MM_MID_ERR_EID                       16

/** \brief <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt>
**  \event <tt> 'Invalid ground command code: ID = 0x\%X, CC = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a software bus message is received
**  with an invalid command code.    
**
**  The \c ID field contains the message ID, the \c CC field contains
**  the command code that generated the error.
*/
#define MM_CC1_ERR_EID                       17

/** \brief <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when command message is received with a message
**  length that doesn't match the expected value.   
**
**  The \c ID field contains the message ID, the \c CC field contains the 
**  command code, the \c Len field is the actual length returned by the
**  CFE_SB_GetTotalMsgLength call, and the \c Expected field is the expected
**  length for messages with that command code.
*/
#define MM_LEN_ERR_EID                       18

/** \brief <tt> 'Invalid memory type specified: MemType = \%d' </tt>
**  \event <tt> 'Invalid memory type specified: MemType = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a command is received with an
**  unrecognized or unsupported memory type specified.
**
**  The \c MemType field is the invalid memory type specifier that was
**  received in the command message. 
*/
#define MM_MEMTYPE_ERR_EID                   19

/** \brief <tt> 'Symbolic address can't be resolved: Name = '\%s'' </tt>
**  \event <tt> 'Symbolic address can't be resolved: Name = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a symbol name string can't be
**  resolved by the OSAPI.
**
**  The \c Name field is the symbol name string that generated
**  the error. 
*/
#define MM_SYMNAME_ERR_EID                   20

/** \brief <tt> 'Data size in bytes invalid or exceeds limits: Data Size = \%d' </tt>
**  \event <tt> 'Data size in bytes invalid or exceeds limits: Data Size = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a command or load file specifies a number
**  of bytes that is either zero or exceeds the limits specified by the  
**  MM configuration parameters
**
**  The \c Data \c Size field is the data size in bytes that generated the
**  error. 
*/
#define MM_DATA_SIZE_BYTES_ERR_EID           21

/** \brief <tt> 'Data size in bits invalid: Data Size = \%d' </tt>
**  \event <tt> 'Data size in bits invalid: Data Size = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a command specified bit width 
**  for a peek or poke operation is either undefined or not valid for the
**  specified memory type.  
**
**  The \c Data \c Size field is the data size in bits that generated the
**  error. 
*/
#define MM_DATA_SIZE_BITS_ERR_EID            22

/** \brief <tt> 'Data and address not 32 bit aligned: Addr = 0x\%08X Size = \%d' </tt>
**  \event <tt> 'Data and address not 32 bit aligned: Addr = 0x\%08X Size = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when command execution requires 32 bit wide
**  memory access and the data size and address specified are not both 32 bit 
**  aligned.
**
**  The \c Addr field is the address and the \c Size field is the 
**  specified size in bytes, that failed the alignment check.
*/
#define MM_ALIGN32_ERR_EID                   23

/** \brief <tt> 'Data and address not 16 bit aligned: Addr = 0x\%08X Size = \%d' </tt>
**  \event <tt> 'Data and address not 16 bit aligned: Addr = 0x\%08X Size = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when command execution requires 16 bit wide
**  memory access and the data size and address specified are not both 16 bit 
**  aligned.
**
**  The \c Addr field is the address and the \c Size field is the 
**  specified size in bytes, that failed the alignment check.
*/
#define MM_ALIGN16_ERR_EID                   24

/** \brief <tt> 'CFE_PSP_MemValidate error received: RC = 0x\%08X Addr = 0x\%08X Size = \%d MemType = \%d' </tt>
**  \event <tt> 'CFE_PSP_MemValidate error received: RC = 0x\%08X Addr = 0x\%08X Size = \%d MemType = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #CFE_PSP_MemValidateRange routine that
**  is used to check address parameters fails.
**
**  The \c RC field is the return code from the #CFE_PSP_MemValidateRange call,
**  the \c Addr field is the address, \c Size field is the specified size in bytes,
**  and the \c MemType field is the memory type of the address range that failed validation.
*/
#define MM_OS_MEMVALIDATE_ERR_EID            25

/** \brief <tt> 'Load file CRC failure: Expected = 0x\%X Calculated = 0x\%X File = '\%s'' </tt>
**  \event <tt> 'Load file CRC failure: Expected = 0x\%X Calculated = 0x\%X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a CRC computation on the data in 
**  a load file does not return the expected result that is specified in
**  the load file header. 
**
**  The \c Expected field is the expected result, the 
**  \c Calculated field is the computed value, and \c File is the 
**  name of the file where the mismatch was detected
*/
#define MM_LOAD_FILE_CRC_ERR_EID             26

/** \brief <tt> 'Interrupts Disabled Load CRC failure: Expected = 0x\%X Calculated = 0x\%X' </tt>
**  \event <tt> 'Interrupts Disabled Load CRC failure: Expected = 0x\%X Calculated = 0x\%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a CRC computation on the data in 
**  a load with interrupts disabled command message does not return the 
**  expected result that is specified in the command message header. 
**
**  The \c Expected field is the expected result and the 
**  \c Calculated field is the computed value.
*/
#define MM_LOAD_WID_CRC_ERR_EID              27

/** \brief <tt> 'OS_EepromWrite8 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  \event <tt> 'OS_EepromWrite8 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_EepromWrite8 function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code from the #OS_EepromWrite8 call, 
**  the \c Addr field is the address that the write was attempted to.
*/
#define MM_OS_EEPROMWRITE8_ERR_EID           28

/** \brief <tt> 'OS_EepromWrite16 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  \event <tt> 'OS_EepromWrite16 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_EepromWrite16 function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code from the #OS_EepromWrite16 call, 
**  the \c Addr field is the address that the write was attempted to.
*/
#define MM_OS_EEPROMWRITE16_ERR_EID          29

/** \brief <tt> 'OS_EepromWrite32 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  \event <tt> 'OS_EepromWrite32 error received: RC = 0x\%08X Addr = 0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_EepromWrite32 function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code from the #OS_EepromWrite32 call, 
**  the \c Addr field is the address that the write was attempted to.
*/
#define MM_OS_EEPROMWRITE32_ERR_EID          30

/** \brief <tt> 'OS_creat error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'OS_creat error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_creat function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code and \c File is the filename 
**  from the #OS_creat call that generated the error. 
*/
#define MM_OS_CREAT_ERR_EID                  31

/** \brief <tt> 'OS_open error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'OS_open error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_open function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code and \c File is the filename 
**  from the #OS_open call that generated the error. 
*/
#define MM_OS_OPEN_ERR_EID                   32

/** \brief <tt> 'OS_close error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'OS_close error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_close function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code and \c File is the filename 
**  from the #OS_close call that generated the error. 
*/
#define MM_OS_CLOSE_ERR_EID                  33

/** \brief <tt> 'OS_read error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'OS_read error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_read function
**  returns a negative error code.   
**
**  The \c RC field is the return code and \c File is the filename 
**  from the #OS_read call that generated the error. 
*/
#define MM_OS_READ_ERR_EID                   34

/** \brief <tt> 'OS_read error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  \event <tt> 'OS_read error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_read function
**  returns some value other than the expected number of bytes read.   
**
**  The \c RC field is the return code, the \c Expected field is the
**  expected return value and the \c File is the filename from the 
**  #OS_read call that generated the error. 
*/
#define MM_OS_READ_EXP_ERR_EID               35

/** \brief <tt> 'OS_write error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  \event <tt> 'OS_write error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_read function
**  returns some value other than the expected number of bytes written.   
**
**  The \c RC field is the return code, the \c Expected field is the
**  expected return value and the \c File is the filename from the 
**  #OS_write call that generated the error. 
*/
#define MM_OS_WRITE_EXP_ERR_EID              36

/** \brief <tt> 'OS_stat error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'OS_stat error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_stat function
**  returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code and \c File is the filename
**  from the #OS_stat call that generated the error. 
*/
#define MM_OS_STAT_ERR_EID                   37

/** \brief <tt> 'CFS_ComputeCRCFromFile error received: RC = 0x\%08X File = '\%s'' </tt>
**  \event <tt> 'CFS_ComputeCRCFromFile error received: RC = 0x\%08X File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #CFS_ComputeCRCFromFile 
**  function returns some value other than #OS_SUCCESS.   
**
**  The \c RC field is the return code and \c File is the filename
**  from the #CFS_ComputeCRCFromFile call that generated the error. 
*/
#define MM_CFS_COMPUTECRCFROMFILE_ERR_EID    38

/** \brief <tt> 'Command specified filename invalid: Name = '\%s'' </tt>
**  \event <tt> 'Command specified filename invalid: Name = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a filename included in a command
**  message fails a check for prohibited characters
**
**  The \c Name field holds the filename string that generated
**  the error. 
*/
#define MM_CMD_FNAME_ERR_EID                 39

/** \brief <tt> 'Load file size error: Reported by OS = \%d Expected = \%d File = '\%s'' </tt>
**  \event <tt> 'Load file size error: Reported by OS = \%d Expected = \%d File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a load memory from file command
**  is processed and the size of the load file in bytes (as reported by the
**  filesystem) doesn't match what would be expected based upon the load byte 
**  count specified in the file header. 
**
**  The \c Reported \c by \c OS field holds size of the file in bytes as
**  reported by the operating system, the \c Expected field holds the 
**  expected byte count that is the sum of number of load bytes specified
**  in the file header and the size of the file header itself. The 
**  \c Name field holds the name of the load file that generated
**  the error. 
**  
*/
#define MM_LD_FILE_SIZE_ERR_EID              40

/** \brief <tt> 'Load file failed parameters check: File = '\%s'' </tt>
**  \event <tt> 'Load file failed parameters check: File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a load file fails one of a series of
**  parameter checks on the Destination Address, Memory Type, and Byte Size
**  specified in the load file header. Another error event will be issued
**  with the specific error, this is a supplemental message that echos the
**  name of the file that failed.
**
**  The \c File field holds the name of the file that failed the
**  parameter checks. 
*/
#define MM_FILE_LOAD_PARAMS_ERR_EID          41

/** \brief <tt> 'CFE_FS_ReadHeader error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  \event <tt> 'CFE_FS_ReadHeader error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #CFE_FS_ReadHeader function
**  returns some value other than the expected number of bytes read.   
**
**  The \c RC field is the return code, the \c Expected field is the
**  expected return value and the \c File is the filename from the 
**  #CFE_FS_ReadHeader call that generated the error. 
*/
#define MM_CFE_FS_READHDR_ERR_EID            42

/** \brief <tt> 'CFE_FS_WriteHeader error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  \event <tt> 'CFE_FS_WriteHeader error received: RC = 0x\%08X Expected = \%d File = '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #CFE_FS_WriteHeader function
**  returns some value other than the expected number of bytes written.   
**
**  The \c RC field is the return code, the \c Expected field is the
**  expected return value and the \c File is the filename from the 
**  #CFE_FS_WriteHeader call that generated the error. 
*/
#define MM_CFE_FS_WRITEHDR_ERR_EID           43

/** \brief <tt> 'Invalid HK request msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  \event <tt> 'Invalid HK request msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when housekeeping request message is received 
**  with a message length that doesn't match the expected value.   
**
**  The \c ID field contains the message ID, the \c CC field contains the 
**  command code, the \c Len field is the actual length returned by the
**  CFE_SB_GetTotalMsgLength call, and the \c Expected field is the expected
**  length.
*/
#define MM_HKREQ_LEN_ERR_EID                 44

/** \brief <tt> 'Symbol Lookup Command: Name = '\%s' Addr = 0x\%08X' </tt>
**  \event <tt> 'Symbol Lookup Command: Name = '\%s' Addr = 0x\%08X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when an symbol lookup command has been
**  successfully executed.
**
**  The \c Name field holds the symbol name string, the \c Addr field shows 
**  the fully resolved address
*/
#define MM_SYM_LOOKUP_INF_EID                45

/** \brief <tt> 'NUL (empty) string specified as symbol name' </tt>
**  \event <tt> 'NUL (empty) string specified as symbol name' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a NUL string has been specified
**  as the symbol name in a lookup symbol command
*/
#define MM_SYMNAME_NUL_ERR_EID               46

/** \brief <tt> 'Symbol Table Dump to File Started: Name = '\%s'' </tt>
**  \event <tt> 'Symbol Table Dump to File Started: Name = '\%s'' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a dump symbol table fo file command has been
**  successfully executed.
**
**  The \c Name field holds the requested dump file name string
*/
#define MM_SYMTBL_TO_FILE_INF_EID                47

/** \brief <tt> 'NUL (empty) string specified as symbol dump file name' </tt>
**  \event <tt> 'NUL (empty) string specified as symbol dump file name' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a NUL string has been specified
**  as the dump file name in a dump symbol table to file command
*/
#define MM_SYMFILENAME_NUL_ERR_EID           48

/** \brief <tt> 'Error dumping symbol table, OS_Status= 0x%X, File='%s'' </tt>
**  \event <tt> 'Error dumping symbol table, OS Status= 0x%X, File='%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a NUL string has been specified
**  as the dump file name in a dump symbol table to file command
**
**  The \c OS_Status field holds the return code from the call to #OS_SymbolTableDump
**  The \c File field holds the requested dump file name string
*/
#define MM_SYMTBL_TO_FILE_FAIL_ERR_EID           49

/** \brief <tt> 'Illegal characters in target filename, File='%s'' </tt>
**  \event <tt> 'Illegal characters in target filename, File='%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when an illegal character has been found
**  in the specified dump file name of a dump symbol table to file command
**
**  The \c File field holds the requested dump file name string
*/
#define MM_SYMTBL_TO_FILE_INVALID_ERR_EID        50

/** \brief <tt> 'EEPROM bank %d write enabled, cFE_Status= 0x%X' </tt>
**  \event <tt> 'EEPROM bank %d write enabled, cFE_Status= 0x%X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a request to enable writing to a specified
**  EEPROM bank results in a success status from the PSP.
**
**  The \c bank field identifies the requested EEPROM bank to be write-enabled
**  The \c CFE_Status field holds the return code from the call to #CFE_PSP_EepromWriteEnable
*/
#define MM_EEPROM_WRITE_ENA_INF_EID           51

/** \brief <tt> 'Error requesting EEPROM bank %d write enable, cFE_Status= 0x%X' </tt>
**  \event <tt> 'Error requesting EEPROM bank %d write enable, cFE_Status= 0x%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a request to enable writing to a specified
**  EEPROM bank results in an error status from the PSP.
**
**  The \c bank field identifies the requested EEPROM bank to be write-enabled
**  The \c CFE_Status field holds the return code from the call to #CFE_PSP_EepromWriteEnable
*/
#define MM_EEPROM_WRITE_ENA_ERR_EID           52

/** \brief <tt> 'EEPROM bank %d write disabled, cFE_Status= 0x%X' </tt>
**  \event <tt> 'EEPROM bank %d write disabled, cFE_Status= 0x%X' </tt>
**  
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a request to disable writing to a specified
**  EEPROM bank results in a success status from the PSP.
**
**  The \c bank field identifies the requested EEPROM bank to be write-disabled
**  The \c CFE_Status field holds the return code from the call to #CFE_PSP_EepromWriteDisable
*/
#define MM_EEPROM_WRITE_DIS_INF_EID           53

/** \brief <tt> 'Error requesting EEPROM bank %d write disable, cFE_Status= 0x%X' </tt>
**  \event <tt> 'Error requesting EEPROM bank %d write disable, cFE_Status= 0x%X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a request to disable writing to a specified
**  EEPROM bank results in an error status from the PSP.
**
**  The \c bank field identifies the requested EEPROM bank to be write-disabled
**  The \c CFE_Status field holds the return code from the call to #CFE_PSP_EepromWriteDisable
*/
#define MM_EEPROM_WRITE_DIS_ERR_EID           54

/** \brief <tt> 'Zero bytes read by OS_read of file '\%s'' </tt>
**  \event <tt> 'Zero bytes read by OS_read of file '\%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to the #OS_read function
**  returns zero total bytes read.   
**
**  The \c File is the filename that #OS_Read attempted to read.  
*/
#define MM_OS_ZERO_READ_ERR_EID                55

/** \brief <tt> 'PSP read memory error: RC=0x\%08X, Src=0x\%08X, Tgt=0x\%08X, Type='%s'' </tt>
**  \event <tt> 'PSP read memory error: RC=0x\%08X, Src=0x\%08X, Tgt=0x\%08X, Type='%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to one of the CFE_PSP_MemRead functions
**  (#CFE_PSP_MemRead8, #CFE_PSP_MemRead16, #CFE_PSP_MemRead32) returns something
**  other than CFE_PSP_SUCCESS.
**
**  The \c RC field is the function return code and \c Src is the read location and
**  \c Tgt is the storage location from the function call that generated the error.
**  The \c Type field will indicate MEM8, MEM16 or MEM32.
*/
#define MM_PSP_READ_ERR_EID                    56

/** \brief <tt> 'PSP write memory error: RC=0x\%08X, Address=0x\%08X, MemType='%s'' </tt>
**  \event <tt> 'PSP write memory error: RC=0x\%08X, Address=0x\%08X, MemType='%s'' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to one of the CFE_PSP_MemWrite functions
**  (#CFE_PSP_MemWrite8, #CFE_PSP_MemWrite16, #CFE_PSP_MemWrite32) returns something
**  other than CFE_PSP_SUCCESS.
**
**  The \c RC field is the function return code and \c Address is the write location
**  from the function call that generated the error. The \c MemType
**  field will indicate MEM8, MEM16 or MEM32.
*/
#define MM_PSP_WRITE_ERR_EID                   57

/** \brief <tt> 'PSP copy memory error: RC=0x\%08X, Src=0x\%08X, Tgt=0x\%08X, Size=0x\%08X' </tt>
**  \event <tt> 'PSP copy memory error: RC=0x\%08X, Src=0x\%08X, Tgt=0x\%08X, Size=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to #CFE_PSP_MemCpy returns something
**  other than CFE_PSP_SUCCESS.
**
**  The \c RC field is the function return code and \c Src and \c Tgt are the source
**  and target for the copy. The \c Size field is the length of the copy.
*/
#define MM_PSP_COPY_ERR_EID                    58

/** \brief <tt> 'PSP set memory error: RC=0x\%08X, Tgt=0x\%08X, Size=0x\%08X' </tt>
**  \event <tt> 'PSP set memory error: RC=0x\%08X, Tgt=0x\%08X, Size=0x\%08X' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when a call to #CFE_PSP_MemSet returns something
**  other than CFE_PSP_SUCCESS.
**
**  The \c RC field is the function return code and \c Tgt is the target for the
**  memory set. The \c Size field is the length of the memory set.
*/
#define MM_PSP_SET_ERR_EID                     59

#endif /* _mm_events_ */

/************************/
/*  End of File Comment */
/************************/
