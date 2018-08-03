/**
 * \file cfe_psp_config.c
 *
 *  Created on: Dec 31, 2014
 *      Author: joseph.p.hickey@nasa.gov
 *
 *  Copyright (c) 2004-2011, United States Government as represented by 
 *  Administrator for The National Aeronautics and Space Administration. 
 *  All Rights Reserved.
 *
 *  This is governed by the NASA Open Source Agreement and may be used,
 *  distributed and modified only pursuant to the terms of that agreement.
 */

#include <cfe_psp.h>
#include <cfe_psp_configdata.h>


/**
 * Instantiation of the PSP/HW configuration data
 *
 * Because this is compiled within the context of the PSP code,
 * this can access the internal PSP macros directly.  External
 * code such as CFE core or apps would not be able to #include the
 * PSP cfe_psp_config.h or psp_version.h files
 */
Target_PspConfigData GLOBAL_PSP_CONFIGDATA =
{
      .PSP_WatchdogMin = CFE_PSP_WATCHDOG_MIN,
      .PSP_WatchdogMax = CFE_PSP_WATCHDOG_MAX,
      .PSP_MemTableSize = CFE_PSP_MEM_TABLE_SIZE,
      .PSP_MemoryTable = CFE_PSP_MemoryTable,

      .OS_VolumeTableSize = NUM_TABLE_ENTRIES,
      .OS_VolumeTable = OS_VolumeTable,
      .OS_CpuContextSize = CFE_PSP_CPU_CONTEXT_SIZE,

      .HW_NumEepromBanks = CFE_PSP_NUM_EEPROM_BANKS,

      .PSP_VersionInfo =
      {
            .MajorVersion = CFE_PSP_IMPL_MAJOR_VERSION,
            .MinorVersion = CFE_PSP_IMPL_MINOR_VERSION,
            .Revision = CFE_PSP_IMPL_REVISION,
            .MissionRev = CFE_PSP_IMPL_MISSION_REV
      }
};

