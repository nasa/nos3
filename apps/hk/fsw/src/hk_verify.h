/************************************************************************
** File:
**   $Id: hk_verify.h 1.5 2015/03/04 14:58:30EST sstrege Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**  Define the CFS Housekeeping (HK) Application compile-time checks
**
** Notes:
**
** $Log: hk_verify.h  $
** Revision 1.5 2015/03/04 14:58:30EST sstrege 
** Added copyright information
** Revision 1.4 2012/08/15 18:32:10EDT aschoeni 
** Added ability to discard incomplete combo packets
** Revision 1.3 2009/12/03 16:34:45EST jmdagost 
** Included cfe.h, hk_platfrom_cfg.h, and hk_app.h
** Added tests for pipe depth, copy table entries, and definition of mem pool.
** Revision 1.2 2008/09/11 10:20:50EDT rjmcgraw 
** DCR4040:1 Added newline at end of file
** Revision 1.1 2008/04/09 16:43:05EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/hk/fsw/src/project.pj
**
*************************************************************************/
#ifndef _hk_verify_h_
#define _hk_verify_h_

#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "hk_platform_cfg.h"

#include "hk_app.h"

#if HK_DISCARD_INCOMPLETE_COMBO  <  0
    #error HK_DISCARD_INCOMPLETE_COMBO cannot be less than 0!
#elif HK_DISCARD_INCOMPLETE_COMBO  >  1
    #error HK_DISCARD_INCOMPLETE_COMBO cannot be greater than 1!
#endif

#ifndef HK_PIPE_DEPTH
    #error HK_PIPE_DEPTH must be defined!
#elif (HK_PIPE_DEPTH  <  1)
    #error HK_PIPE_DEPTH cannot be less than 1!
#elif (HK_PIPE_DEPTH  >  CFE_SB_MAX_PIPE_DEPTH)
    #error HK_PIPE_DEPTH cannot be greater than CFE_SB_MAX_PIPE_DEPTH!
#endif

#ifndef HK_COPY_TABLE_ENTRIES
    #error HK_COPY_TABLE_ENTRIES must be defined!
#elif (HK_COPY_TABLE_ENTRIES  <  1)
    #error HK_COPY_TABLE_ENTRIES cannot be less than 1!
#elif (HK_COPY_TABLE_ENTRIES  >  8192)
    #error HK_COPY_TABLE_ENTRIES cannot be greater than 8192!
#endif

#ifndef HK_NUM_BYTES_IN_MEM_POOL
    #error HK_NUM_BYTES_IN_MEM_POOL must be defined!
#endif

#endif /* _hk_verify_h_ */

/************************/
/*  End of File Comment */
/************************/
