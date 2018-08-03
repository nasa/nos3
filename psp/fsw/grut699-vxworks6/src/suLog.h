/*-----------------------------------------------------------------------------
//
// Module Name: suLog.h
//
//      Copyright (c) 2004-2011, United States Government as represented by 
//      Administrator for The National Aeronautics and Space Administration. 
//      All Rights Reserved.
//
//      This is governed by the NASA Open Source Agreement and may be used,
//      distributed and modified only pursuant to the terms of that agreement.
//
// Description: header file defining the SUROM Log and C Logging routines
//
// Change History:
//
// Date      Pgm  Description
// --------  ---  ------------------------------------------------------
// 08/04/02  PMB  High Level Design
// 10/15/02  PMB  Code
// 10/06/04  ds   Simplify high/low log to a single log area (low).
//                Combined w/ suFncid.h and eliminated suLog.emc.
//                Log verion 1: Initial baseline for common SUROM
// 09/13/05  TDN  Add PCI log support
// 12/15/05  cal  Moved PCI log support to suLogCustom.h
// 01/20/06  cal  Update ERRORLOGEND to 0x00D8.
// 12/18/06  apc  Converted comments to C style
//                added bit defintions for reset types
//
// $Revision: 1.1 $
// $Date: 2014/08/29 16:01:42GMT-05:00 $
// $Source: suLog.h $
//-----------------------------------------------------------------------------
*/
#ifndef _suLog_h
#define _suLog_h

#if !defined(_EMC_ASSEMBLER) && !defined(_ASMLANGUAGE)
#include <vxWorks.h>
#endif

/*-----------------------------------------------------------------------------
//
// SUROM Log
//
// The SUROM Log contains data that SUROM uses to report status, decide execution
// paths, and record errors.
//
// The SUROM log is kept at 0x00 - 0xFF in RAM.
//
// The Log Format:
//
//        Offset      Data                  Description
//        ---------------------------------------------------------------------
//         0x0     Reset Cause          Bitmap indicating the reason for
//                                      SUROM execution.  Read directly
//                                      from the Power PCI BIST Core Status.
//
//         0x4     Soft Reset Flag      Indicator of whether SUROM should
//                                      perform a soft reset boot. A value of
//                                      'SOFT' (0x534F4654) indicates soft reset,
//                                      which preserves RAM (and scratch pad)
//                                      contents. This flag is always cleared
//                                      by SUROM, so the application must set
//                                      it if a soft boot is desired.
//
//         0x8     Version Info         Log version & ROM offset of SUROM
//                                      shared ROM data area. See defines.
//
//         0xC     SUROM Progress       Running Indicator of SUROM functions
//                                      completed. The 4 most recent values
//                                      are kep t(0xC is most recent, 0xF is
//                                      the oldest). Decodes defined below.
//
//         0x10    RAM Boot Flag        Indicator for the RAM Boot Address being
//                                      valid.  The value must be 'BOOT'
//                                      (0x424F4F54) for a valid Soft Reset
//                                      RAM Boot to occur, i.e., jumping to
//                                      the RAM Boot Addr directly. This flag
//                                      is always cleared by SUROM, so the
//                                      application must set it if a RAM boot
//                                      is desired.
//
//         0x14    RAM Boot Addr        Address to jump to start a program
//                                      already in RAM. SUROM will set this
//                                      address prior to jump to the loaded
//                                      application.
//
//         0x18    BIST Status          Individual BIST pass/fail/not run/NA
//                                      results. Seeded with ROM BIST Status.
//
//         0x1C    PCI Test Status      PCI device BIST and memory test status.
//         0x20    PCI Device Present   PCI device present status.
//         0x24    Reserved             Reserved for future use (0).
//         0x28    Reserved             Reserved for future use (0).
//
//         0x2C    Error Count          Count of errors during SUROM processing.
//                                      The first 22 errors will be logged (each
//                                      entry is 2 32-bit words long).
//
//         0x30    Start of Error Log   Error Log contains a maximum of 22 entries.
//                                      When full, no more entries will be recorded.
//
//         0xD8    Last Error Log       Last possible 2 word Error Log Entry
//
//         0xE0    EMC Command          Command for EMC Function (Change Clock)
//         0xE4    EMC Parameter 1      (see emcVec0.emc)
//         0xE8    EMC Parameter 2
//         0xEC    EMC Parameter 3
//         0xF0    EMC Parameter 4
//         0xF4    EMC Parameter 5
//         0xF8    EMC Command RC       Result of EMC Command
//
//         0xFC    EMC Work Area        (reserved)
//
//
//         0xE0    PPCI ABIST Low       Signature temporarily stored here
//         0xE4    PPCI ABIST Med       during SUROM bring-up/BIST. Only used
//         0xE8    PPCI ABIST High      by EMC between ABIST & LBIST.
//-----------------------------------------------------------------------------
*/
#define SUROM_LOG_VERSION     0x001   /* 3 nibbles: */
                                      /* 001 - Initial version for RAD750 SUROM 2.0 */

/*--------------------------------------------------------------------------
// This first set of defines is referenced by EMC code. The C definitions
// below are based off these constants.
//--------------------------------------------------------------------------
*/

/* Base Addresses of the SUROM Log (for upper half of GPRs) */
#define SULOG                 0x0000

/* SUROM log data fields (lower 16 bits of addr) */
#define RESETCAUSE            0x0000
#define SOFTFLAG              0x0004
#define VERSIONINFO           0x0008
#define SUROMPROGRESS         0x000C
#define BOOTRAMFLAG           0x0010
#define BOOTRAMADDR           0x0014
#define BISTSTATUS            0x0018
#define PCITESTSTATUS         0x001C
#define PCIDEVICEPRESENT      0x0020
#define ERRORLOGCNT           0x002C
#define ERRORLOG              0x0030
#define ERRORLOGEND           0x00D8
#define EMCCMDS               0x00E0
#define EMCPARM1              0x00E4
#define EMCPARM2              0x00E8
#define EMCPARM3              0x00EC
#define EMCPARM4              0x00F0
#define EMCPARM5              0x00F4
#define VEC0RC                0x00F8
#define VEC0SIG               0x00FC

#define TMPABISTLOW           0x00E0    /* Must be a cache-line */
#define TMPABISTMID           0x00E4
#define TMPABISTHIGH          0x00E8

/*--------------------------------------------------------------------------
// Now for the C definitions...
//--------------------------------------------------------------------------
*/

/* Base Addresses of the SUROM Log */
#define LOG_BASE              (SULOG << 16)

/* SUROM log */
#define resetCause            (* (volatile ULONG *) (LOG_BASE + RESETCAUSE   ))
#define softFlag              (* (volatile ULONG *) (LOG_BASE + SOFTFLAG     ))
#define versionInfo           (* (volatile ULONG *) (LOG_BASE + VERSIONINFO ))
#define suromProgress         (* (volatile ULONG *) (LOG_BASE + SUROMPROGRESS))
#define bootRamFlag           (* (volatile ULONG *) (LOG_BASE + BOOTRAMFLAG  ))
#define bootRamAddr           (* (volatile ULONG *) (LOG_BASE + BOOTRAMADDR  ))
#define suromBISTStatus       (* (volatile ULONG *) (LOG_BASE + BISTSTATUS   ))
#define pciTestStatus         (* (volatile ULONG *) (LOG_BASE + PCITESTSTATUS ))
#define pciDevicePresent      (* (volatile ULONG *) (LOG_BASE + PCIDEVICEPRESENT ))
#define errorLogCnt           (* (volatile ULONG *) (LOG_BASE + ERRORLOGCNT  ))
#define errorLog              (  (volatile ULONG *) (LOG_BASE + ERRORLOG     ))
#define errorLogEnd           (* (volatile ULONG *) (LOG_BASE + ERRORLOGEND  ))
#define emcCmds               (* (volatile ULONG *) (LOG_BASE + EMCCMDS      ))
#define emcParm1              (* (volatile ULONG *) (LOG_BASE + EMCPARM1     ))
#define emcParm2              (* (volatile ULONG *) (LOG_BASE + EMCPARM2     ))
#define emcParm3              (* (volatile ULONG *) (LOG_BASE + EMCPARM3     ))
#define emcParm4              (* (volatile ULONG *) (LOG_BASE + EMCPARM4     ))
#define emcParm5              (* (volatile ULONG *) (LOG_BASE + EMCPARM5     ))
#define emcCmdRc              (* (volatile ULONG *) (LOG_BASE + VEC0RC       ))
#define tmpABistLow           (* (volatile ULONG *) (LOG_BASE + TMPABISTLOW  ))
#define tmpABistMid           (* (volatile ULONG *) (LOG_BASE + TMPABISTMID  ))
#define tmpABistHigh          (* (volatile ULONG *) (LOG_BASE + TMPABISTHIGH ))

/* Error Log Size */
#define MAX_ERRORS            22

/* Size of SUROM Log Area  */
#define SUROM_LOG_SIZE        0x100


/* Defines for version info */
#define LOG_VERSION_MASK      0xFFF00000
#define LOG_VERSION_SHIFT     20
#define ROM_DATA_OFFS_MASK    0x000FFFFF


/* Flags used in SUROM Log and Boot Info */
#define SOFT                  0x534F4654 /* "SOFT" */
#define BOOT                  0x424F4F54 /* "BOOT" */

/* EMC Command IDs */
#define SETCLKCMD             0xFADE

/* EMC Return Codes */
#define BADCMD                0xBADC
#define GOODCMD               0

/* Reset Cause bit defintions */
#define PPCI_POWER_ON_RESET      0x1
#define PPCI_INTERNAL_HW_RESET   0x2
#define PPCI_OCB_SOFTWARE_RESET  0x4
#define PPCI_JTAG_SOFTWARE_RESET 0x8
/*--------------------------------------------------------------------------
// SUROM Progress
//
// The SUROM Progress indicator in the SUROM Log contains a running id of
// SUROM's paths through execution.  This is most useful in error conditions
// where SUROM has stopped and is waiting for help.
//
// The decodes (used by both C and assemblers) are below.
//--------------------------------------------------------------------------
*/
#define EMC_COMPLETE                    0x08
#define R750_OUT_OF_RESET               0x10
#define MEM_TEST_STARTED                0x1f
#define BANK_0_COMPLETE                 0x20
#define BANK_1_COMPLETE                 0x21
#define BANK_2_COMPLETE                 0x22
#define BANK_3_COMPLETE                 0x23
#define BANK_4_COMPLETE                 0x24
#define BANK_5_COMPLETE                 0x25
#define BANK_6_COMPLETE                 0x26
#define BANK_7_COMPLETE                 0x27
#define BOOT_DISCRETES_CHECKED          0x30
#define PCI_CONFIGURED                  0x40

#define SUMMIT_BIT_COMPLETE             0x41
#define SUMMIT_RAM_TEST_COMPLETE        0x42
#define SUMMIT_RAM_INITIALIZED          0x43

#define SPW_LBIST_COMPLETE              0x60
#define SPW_ABIST_COMPLETE              0x65

#define SPW_MEM_TEST_STARTED            0x79
#define SPW_BANK_0_COMPLETE             0x80
#define SPW_BANK_1_COMPLETE             0x81
#define SPW_BANK_2_COMPLETE             0x82
#define SPW_BANK_3_COMPLETE             0x83
#define SPW_BANK_4_COMPLETE             0x84
#define SPW_BANK_5_COMPLETE             0x85
#define SPW_BANK_6_COMPLETE             0x86
#define SPW_BANK_7_COMPLETE             0x87

#define EEPROM_BOOT                     0xB0
#define UART_BOOT                       0xB1
#define RAM_BOOT                        0xB2
#define SUROM_COMPLETE                  0xFF

/*--------------------------------------------------------------------------
// Self Test Status:
//
// PASSED         - Performed test and it passed.
// FAILED         - Performed test and it failed.
// NOT_RUN        - Initialized to NOT_RUN if test is normal for selected boot
//                  path.  May remain at NOT_RUN if SUROM was unable to perform
//                  the test.  For instance, if the device is not present.
// NOT_APPLICABLE - Initialized to NOT_APPLICABLE if test is not normal for the
//                  selected boot path.
//
// Note: the initial value for these fields are kept in ROM. Each field
// is initially either NOT_RUN or NOT_APPLICABLE. A value of NOT_APPLICABLE
// causes the test to be skipped (whether or not thattest is built into the
// SUROM). This provides a quick patch mechanism to disable tests in the lab.
//--------------------------------------------------------------------------
*/
#define ST_PASSED            0
#define ST_FAILED            1
#define ST_NOT_RUN           2
#define ST_NOT_APPLICABLE    3

#define ST_FIELD_MASK        3
#define ST_FIELD_WIDTH       2

/*--------------------------------------------------------------------------
// Self Test IDs (0..15 can fit in word w/ 2 bits per field)
//--------------------------------------------------------------------------
*/
#define ST_RAM_BANK0         0   /* All 8 banks in order */
#define ST_RAM_BANK1         1
#define ST_RAM_BANK2         2
#define ST_RAM_BANK3         3
#define ST_RAM_BANK4         4
#define ST_RAM_BANK5         5
#define ST_RAM_BANK6         6
#define ST_RAM_BANK7         7
#define ST_PPCI_BIST         8
#define ST_PPCI_ABIST        9   /* For enhanced PPCI */
#define ST_R750_BIST         10

#define BIST_STAT_MASK(item) (ST_FIELD_MASK << ((item) * ST_FIELD_WIDTH))
#define BIST_STAT_FIELD(item, status) ((status) << ((item) * ST_FIELD_WIDTH))

/*
// The following simple defines are used by EMC code to decide to skip tests.
// They are used as both masks and "skip test" values, so if any of the
// above related ST_* defines are changed, so must these.
*/
#define NO_PPCI_LBIST         0x00030000
#define NO_PPCI_ABIST         0x000c0000
#define NO_750_BIST           0x00300000
#define NO_RAM_TEST           0x0000ffff   /* All 8 banks disabled (N/A) */

/*
// Value left in EMC GPR8 to indicate we couldn't run 750 BIST because
// we could not master enable. Treat this case as a Not Run.
*/
#define NO_MASTER_EANBLE      0x12345678

/*--------------------------------------------------------------------------
// PCI Device Present Status:
//   0=Absent, 1=Present
//--------------------------------------------------------------------------
*/
#define ABSENT		0
#define PRESENT 	1

/*--------------------------------------------------------------------------
// SUROM Error Logs
//
//
// Each SUROM Error Log entry is 2 32-bit words.   Two word size was chosen because
// memory errors are reported by the Power PCI in 2-word reports.  Non-memory errors
// may not need 2 words to be reported, but for consistency and efficiency in
// maintaining the log 2 words are used.
//
// For each 2-word entry in the Error Log, the Error Log Count is incremented by 1.
//
// A single bit correctable or double bit detected memory error report will be
// exactly what is reported by the Power PCI. See the Programmer's Reference
// Manual for a detailed explanation of these reports.
//
// A memory error of greater than two bits may not be detected by the Power PCI but
// will be detected during memory test.  It will be reported as described below.
//
// A non-memory (or memory error not detected) error log will take the following form:
//
// Word 1 -> Error Code indicating the Error
// Word 2 -> Supporting data for the error.
//
// Error Conditions reported in this way are:
//
//
//   Error Condition                            Supporting Data
//   --------------------------------------     ----------------------------------
//   Power PCI Memory Error Counter > 4         (Error Counter - 4)
//   > 2 bit memory error detected              Address
//   Power PCI BIST Failed                      None (sig in SUROM Log)
//   RAD750 BIST Failed                         None (status in SUROM Log)
//   PCI Bus in Reset                           PCI Bus Reset Register
//   RAM Boot Address Invalid                   RAM Boot Address
//   EEPROM Not Out of Reset                    PID Input Reg
//   EEPROM Disabled                            PID Input Reg
//   EEPROM Boot Flag Invalid                   Boot Flag from header
//   EEPROM Program Offset Invalid              Offset from header
//   EEPROM Byte Count Invalid                  Byte Count from header
//   EEPROM Offset+ByteCount Invalid            Sum of offset + byte count
//   EEPROM Start Address Invalid               Start Address from header
//   EEPROM Start+ByteCount Invalid             Sum of start + byte count
//   EEPROM Jump Address Invalid                Jump Address from header
//   EEPROM Checksum Failed                     Calculated Checksum
//   UART Timeout                               None
//   Machine Check                              Address causing (SRR0)
//   Data Storage Interrupt                     Data Address Register (DAR)
//   Instruction Storage Interrupt              Address causing (SRR0)
//   External Interrupt                         PPCI Int Collection Reg
//   Program Interrupt                          Address causing (SRR0)
//
//--------------------------------------------------------------------------
*/

/*--------------------------------------------------------------------------
//
// SUROM Log Error Codes (for errors not reported by the Power PCI)
//
// (Defines used by both C and assemblers.)
//--------------------------------------------------------------------------
*/
#define MEM_ERROR_COUNT         0x001E
#define MULT_BIT_ERROR          0x002E
#define PPCI_BIST_FAILED        0x003E
#define PPCI_ABIST_FAILED       0x004E
#define R750_BIST_FAILED        0x005E
#define PCI_BUS_IN_RESET        0x006E

#define INV_RAM_ADDR            0x008E
#define EEPROM_DISABLED         0x009E
#define EEPROM_IN_RESET         0x00AE
#define INV_EEP_FLAG            0x00BE
#define INV_EEP_OFFS            0x00CE
#define INV_EEP_LEN             0x00DE
#define INV_EEP_OFFS_LEN        0x00EE
#define INV_EEP_START           0x00FE
#define INV_EEP_START_LEN       0x010E
#define INV_EEP_JUMP            0x011E
#define INV_EEP_SUM             0x012E
#define UART_TIMEOUT_ERR        0x013E
#define MC_INT                  0x020E
#define DSI_INT                 0x030E
#define ISI_INT                 0x040E
#define EI_INT                  0x050E
#define PROG_INT                0x070E

/*
** Failure codes with matching SUROM Progress code.  Tacked "0E" to progress code.
*/
#define SUMMIT_BIT_FAILED	      0x410E
#define SUMMIT_RAM_TEST_FAILED  0x420E
#define SUMMIT_RAM_INIT_FAILED  0x430E

#define SPW_LBIST_FAILED        0x600E
#define SPW_ABIST_FAILED        0x650E

/* Devices NOT present.  "D" for Device. */
#define SPIF_NOT_PRESENT 	      0xD00E
#define SPW_NOT_PRESENT 	      0xD01E


/*-----------------------------------------------------------------------------
// SUROM Log Macros
//
// These macros provide defines for updating SUROM Log.
// For fields whose data must be preserved across a reset, a DCflush must 
// follow the write to SUROM log.  For the common SUROM, softFlag, bootRamFlag, 
// and bootRamAddr are the fields whose data must be preserved.  Look in 
// r750.emc (if customized) for additional fields.
//-----------------------------------------------------------------------------
*/

#define setResetCause(cause)           \
  resetCause = cause;                  \
    DCflush((ULONG)&suromProgress);

#define setSoftFlag(flag)              \
  softFlag = flag;                     \
    DCflush((ULONG)&suromProgress);

#define setSuromProgress(prog)         \
  suromProgress = (suromProgress << 8) | (prog);            \
    DCflush((ULONG)&suromProgress);
    
#define setPciDevicePresent( value )   \
  pciDevicePresent = value;            \
    DCflush((ULONG)&pciDevicePresent);

#define getPciDevicePresent(device, present) \
  present = pciDevicePresent;  \
  present = present & (1 << device);  \
  present = present >> device; 
    
#define logSuromBISTStatus(item, status)    \
  suromBISTStatus = suromBISTStatus & ~BIST_STAT_MASK(item);  \
  suromBISTStatus = suromBISTStatus | BIST_STAT_FIELD(item, status);  \
    DCflush((ULONG)&suromBISTStatus);
    
#define logPciTestStatus(item, status)    \
  pciTestStatus = pciTestStatus & ~BIST_STAT_MASK(item);  \
  pciTestStatus = pciTestStatus | BIST_STAT_FIELD(item, status);  \
    DCflush((ULONG)&pciTestStatus);
    
#define testNotRun(item)    \
  ((suromBISTStatus & BIST_STAT_MASK(item)) == BIST_STAT_FIELD(item, ST_NOT_RUN))

/*-----------------------------------------------------------------------------
// SUROM Log and Error Management
//-----------------------------------------------------------------------------
*/
#if !defined(_EMC_ASSEMBLER) && !defined(_ASMLANGUAGE)

/* Log an error.  This writes both lower and upper error logs */
void logError(ULONG data1, ULONG data2);


/* Halt SUROM upon an error condition. */
void haltSUROM(ULONG errCode);

/* Finish SUROM and jump to app. */
void suromDone();

/* Initialize log. Returns error count left by EMC. */
ULONG initLog();

#endif

#endif
