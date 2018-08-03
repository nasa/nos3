/******************************************************************************
** File:  cfe_psp_ssr.c
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  Alan Cudmore    | Initial version,
**
******************************************************************************/


/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "cacheLib.h"
#include "xbdBlkDev.h"
#include "xbdRamDisk.h"
#include <hrFsLib.h>
#include <xbdPartition.h>


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_mission_cfg.h"   /* for spacecraft ID */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"


     

#define  BLOCKSIZE         512
#define  DISKSIZE          (BLOCKSIZE * 2000)


/******************************************************************************
**  Function:  CFE_PSP_InitSSR
**
**  Purpose:
**    Initializes the Solid State Recorder device. For the grut699, this simply
**    creates a ram drive

**
**  Arguments:
**    DeviceName
**
**  Return:
**    (none)
*/

int32 CFE_PSP_InitSSR( uint32 bus, uint32 device, char * DeviceName )
{
   int32     ReturnCode = CFE_PSP_ERROR;
   device_t  xbd = NULLDEV;
   STATUS error = OK;

#ifdef USE_VXWORKS_ATA_DRIVER
   xbd = xbdRamDiskDevCreate (BLOCKSIZE, DISKSIZE, 0, DeviceName);
   if (xbd == NULLDEV)
   {
       ReturnCode = CFE_PSP_ERROR;
   }
   else
   {
      /* 
      ** Format the RAM disk for HRFS. Allow for upto a 1000 files/directories 
      ** and let HRFS determine the logical block size.
      */

      error = hrfsFormat(DeviceName, 0ll, 0, 1000);
      if (error != OK)
      {
         ReturnCode = CFE_PSP_ERROR;
      }
      else
      {
         ReturnCode = CFE_PSP_SUCCESS;
      }
   }
#endif

   return(ReturnCode);
   
}

