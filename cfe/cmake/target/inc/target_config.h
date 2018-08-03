/*
 *  Copyright (c) 2004-2014, United States government as represented by the 
 *  administrator of the National Aeronautics Space Administration.  
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 *
 *  This is governed by the NASA Open Source Agreement and may be used, 
 *  distributed and modified only according to the terms of that agreement.
 */

/**
 * \file target_config.h
 *
 *  Created on: Dec 12, 2014
 *  Created by: joseph.p.hickey@nasa.gov
 *
 * Defines structures for the global system-wide configuration data.
 * These structures can be accessed at runtime and are an alternative to
 * setting tunable parameters with preprocessor macros.  By putting the
 * tunable parameter in here and accessing it at runtime, the object
 * code becomes more portable.
 */

#ifndef TARGET_CONFIG_H_
#define TARGET_CONFIG_H_

#include <common_types.h>
#include <cfe_psp_configdata.h>

/**
 * Prototype for the main system entry function implemented in CFE ES
 * The PSP should call this at start up.
 */
typedef void (*System_MainFunc_t)(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

/**
 * Prototype for 1Hz ISR function implemented in CFE TIME
 * The PSP should call this at 1Hz.
 */
typedef void (*System_1HzISRFunc_t)(void);


/**
 * Prototype for exception ISR function implemented in CFE ES
 * The PSP should call this when exceptions occur.
 */
typedef void (*System_ExceptionFunc_t)(uint32  HostTaskId, const char *ReasonString, const uint32 *ContextPointer, uint32 ContextSize);

/**
 * Abstract pointer to a module API
 * Contents are not defined at this level - the PSP/CFE implementation actually defines this.
 */
typedef const void CFE_StaticModuleApi_t;

/**
 * Combines a module API with a name to create the static module list.
 */
typedef const struct
{
   const char *Name;
   CFE_StaticModuleApi_t *Api;
} CFE_StaticModuleLoadEntry_t;

/**
 * Core Flight Executive configuration information.
 */
typedef const struct
{
   /**
    * 1Hz ISR entry point.  Called from PSP once per second on HW clock.
    */
   System_1HzISRFunc_t System1HzISR;

   /**
    * Main CFE entry point.  Called from PSP startup code.
    */
   System_MainFunc_t SystemMain;

   /**
    * Exception handler function. Called from PSP during exception handling.
    */
   System_ExceptionFunc_t SystemExceptionISR;

   /*
    * Sizes of memory segments required by the CFE based on the current config
    */
   uint32 CdsSize;                /***< CDS segment size */
   uint32 ResetAreaSize;          /***< Reset area segment size */
   uint32 UserReservedSize;       /***< User reserved area segment size */
   uint32 RamDiskSectorSize;      /***< RAM disk sector size */
   uint32 RamDiskTotalSectors;    /***< RAM disk number of sectors */

   /**
    * Default value for start up file
    */
   char *NonvolStartupFile;

} Target_CfeConfigData;

/**
 * Extern reference to cfe config struct.
 * Allows the actual instantiation to be done outside this module
 */
extern Target_CfeConfigData GLOBAL_CFE_CONFIGDATA;

/**
 * Build system information
 * This data will be auto-populated by the build scripts
 *
 * Note that with some PSP's (such as the linux ones) the values
 * for CPU name and number may be overridden, so the correct value
 * for these should be acquired through the PSP via the normal API.
 * The values in this structure are only the defaults.
 */
typedef const struct
{
    const char *MissionVersion; /**< Version string acquired from version control system at build time */
    const char *CfeVersion;     /**< Version string acquired from version control system at build time */
    const char *OsalVersion;    /**< Version string acquired from version control system at build time */
    const char *Config;  /**< Configuration used for build */
    const char *Date;    /**< Date and time of build */
    const char *User;    /**< User ID and build machine */

    /*
     * Default values for CPU ID and CPU Name
     */
    const char *Default_CpuName; /**< Compile-time value for CPU name */
    uint16 Default_CpuId;        /**< Compile-time value for CPU number */
    uint16 Default_SpacecraftId; /**< Compile-time value for Spacecraft ID (mission-wide) */

    Target_CfeConfigData *CfeConfig;   /**< CFE configuration sub-structure */
    Target_PspConfigData *PspConfig;   /**< PSP configuration sub-structure */
    CFE_StaticModuleLoadEntry_t *PspModuleList; /**< List of PSP modules (API structures) statically linked into the core EXE */
    CFE_StaticModuleLoadEntry_t *CfsModuleList; /**< List of CFE modules (API structures) statically linked into the core EXE */

} Target_ConfigData;


/**
 * Extern reference to global config struct.
 * This object is always instantiated within this module
 */
extern Target_ConfigData GLOBAL_CONFIGDATA;

#endif /* TARGET_CONFIG_H_ */
