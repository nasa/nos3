/**
 * \file cfe_psp_config.h
 *
 *  Created on: Dec 31, 2014
 *      Author: joseph.p.hickey@nasa.gov
 *
 *      Copyright (c) 2004-2011, United States Government as represented by 
 *      Administrator for The National Aeronautics and Space Administration. 
 *      All Rights Reserved.
 *
 *      This is governed by the NASA Open Source Agreement and may be used,
 *      distributed and modified only pursuant to the terms of that agreement.
 */

#ifndef CFE_PSP_CONFIG_H_
#define CFE_PSP_CONFIG_H_

/* osapi.h is required for the definition of OS_VolumeInto_t */
#include <osapi.h>
/* cfe_psp.h is required for the definition of CFE_PSP_MemTable_t */
#include <cfe_psp.h>

/*
** PSP software version record
** (replaces PSP_*_VERSION macros)
*/
typedef const struct
{
   uint8 MajorVersion;
   uint8 MinorVersion;
   uint8 Revision;
   uint8 MissionRev;
} CFE_PSP_VersionInfo_t;



/**
 * PSP/Hardware configuration parameters
 * This structure should be instantiated by the PSP according
 * such that other modules do not need to directly include
 * the PSP configuration at compile time.
 */
typedef const struct
{
   uint32 PSP_WatchdogMin;                /**< PSP Minimum watchdog in milliseconds */
   uint32 PSP_WatchdogMax;                /**< PSP Maximum watchdog in milliseconds */
   uint32 PSP_MemTableSize;               /**< Size of PSP memory table */
   CFE_PSP_MemTable_t *PSP_MemoryTable;   /**< Pointer to PSP memory table (forward reference) */

   uint32 OS_VolumeTableSize;             /**< Size of OS volume table */
   OS_VolumeInfo_t *OS_VolumeTable;       /**< Pointer to OS volume table (forward reference) */

   /**
    * Processor Context type.
    * This is needed to determine the size of the context entry in the ER log.
    * It is a placeholder as the implementation to use it is not merged in yet.
    */
   uint32 OS_CpuContextSize;

   /**
    * Number of EEPROM banks on this platform
    */
   uint32 HW_NumEepromBanks;

   CFE_PSP_VersionInfo_t   PSP_VersionInfo;

} Target_PspConfigData;

/**
 * Extern reference to psp config struct.
 * Allows the actual instantiation to be done outside this module
 */
extern Target_PspConfigData GLOBAL_PSP_CONFIGDATA;

/**
 * Extern reference to the psp memory table
 * Allows the actual instantiation to be done outside this module
 */
extern CFE_PSP_MemTable_t CFE_PSP_MemoryTable[];

/**
 * Extern reference to the psp volume table
 * Allows the actual instantiation to be done outside this module
 */
extern OS_VolumeInfo_t OS_VolumeTable[];


#endif /* CFE_PSP_CONFIG_H_ */
