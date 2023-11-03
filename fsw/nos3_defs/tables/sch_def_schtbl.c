/*
** $Id: sch_def_schtbl.c 1.3 2017/06/21 15:29:50EDT mdeschu Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: Scheduler (SCH) default schedule table data
**
** Author: 
**
** Notes:
**
*/

/*************************************************************************
**
** Include section
**
**************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "sch_platform_cfg.h"
#include "sch_msgdefs.h"
#include "sch_tbldefs.h"

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/

/*
** Schedule Table "group" definitions
*/
#define SCH_GROUP_NONE         (0)

/* Define highest level multi-groups */
#define SCH_GROUP_CDH         (0x000001)                        /* All C&DH Messages        */
#define SCH_GROUP_GNC         (0x000002)                        /* All GNC  Messages        */

/* Define sub multi-groups           */
#define SCH_GROUP_CFS_HK      (  (0x000010) | SCH_GROUP_CDH)    /* CFS HK Messages          */
#define SCH_GROUP_CFE_HK      (  (0x000020) | SCH_GROUP_CDH)    /* cFE HK Messages          */
#define SCH_GROUP_GNC_HK      (  (0x000040) | SCH_GROUP_GNC)    /* GNC HK Messages          */

#define SCH_GROUP_

/* Define groups for messages that appear multiple times in Schedule */
#define SCH_GROUP_MD_WAKEUP   ((0x01000000) | SCH_GROUP_CDH)    /* MD Wakeup (aka Group #1) */


/*************************************************************************
**
** Type definitions
**
**************************************************************************/

/*
** (none)
*/

/*************************************************************************
**
** Exported data
**
**************************************************************************/

/*
** Default schedule table data
*/
SCH_ScheduleEntry_t SCH_DefaultScheduleTable[SCH_TABLE_ENTRIES] =
{

/*
** Structure definition...
**
**    uint8    EnableState  -- SCH_UNUSED, SCH_ENABLED, SCH_DISABLED
**    uint8    Type         -- 0 or SCH_ACTIVITY_SEND_MSG
**    uint16   Frequency    -- how many seconds between Activity execution
**    uint16   Remainder    -- seconds offset to perform Activity
**    uint16   MessageIndex -- Message Index into Message Definition table
**    uint32   GroupData    -- Group and Multi-Group membership definitions
*/

  /* slot #0 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  
  /* slot #1 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 26,  SCH_GROUP_CFS_HK },  /* CF HK Request */                                    
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},  
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
    
  /* slot #2 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #3 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  3,  2, SCH_GROUP_CFE_HK },   /* EVS HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #4 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  1,  6, SCH_GROUP_CFS_HK }, */  /* CS HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #5  - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 40, SCH_GROUP_NONE },   /* CAM HK Request */                                      
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 96, SCH_GROUP_NONE },   /* ADCS HK Request */       
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #6 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #7 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #8 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #9 */
  {  SCH_ENABLED, SCH_ACTIVITY_SEND_MSG,  4,  2,  7, SCH_GROUP_CFS_HK },  /* DS HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #10 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #11 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #12 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #13 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  3,  3, SCH_GROUP_CFE_HK },   /* SB HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #14 */
  {  SCH_ENABLED, SCH_ACTIVITY_SEND_MSG,  4,  1,  8, SCH_GROUP_CFS_HK },  /* FM HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #15 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 45, SCH_GROUP_NONE },   /* RW HK Request */         
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #16 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 20, SCH_GROUP_NONE }, */  /* CS Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #17 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #18 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #19 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  2,  9, SCH_GROUP_CFS_HK }, */  /* HK HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #20 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #21 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #22 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #23 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  3,  4, SCH_GROUP_CFE_HK },   /* TIME HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #24 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  1, 10, SCH_GROUP_CFS_HK }, */  /* HS HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #25 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  0, 50, SCH_GROUP_CFE_HK },   /* GPS HK Request */         
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_CFE_HK },  /* GPS Data Request   */                                              
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #26 */
  {  SCH_ENABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 21, SCH_GROUP_NONE },  /* SC Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #27 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #28 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #29 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  2, 11, SCH_GROUP_CFS_HK },  /* LC HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #30 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #31 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #32 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #33 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  3,  5, SCH_GROUP_CFE_HK },   /* TBL HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #34 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  1, 12, SCH_GROUP_CFS_HK }, */  /* MD HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #35 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  0, 55, SCH_GROUP_CFE_HK },   /* Sample HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 56, SCH_GROUP_CFE_HK },   /* Sample Data Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #36 */
  {  SCH_ENABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 23, SCH_GROUP_CFS_HK },  /* DS Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #37 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #38 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #39 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  2, 13, SCH_GROUP_CFS_HK }, */  /* MM HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #40 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #41 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #42 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #43 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  3,  1, SCH_GROUP_CFE_HK },   /* ES HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #44 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  1, 14, SCH_GROUP_CFS_HK },  /* SC HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #45 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  1, 60, SCH_GROUP_CFS_HK },  /* FSS HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_CFS_HK },  /* FSS Data Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #46 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #47 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #48 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #49 */
/*{  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  2, 15, SCH_GROUP_CFS_HK }, */   /* SCH HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #50 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 25,  SCH_GROUP_NONE },  /* CF Wakeup */                                         

  /* slot #51 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 22, SCH_GROUP_NONE }, */  /* LC Sample Action Points */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #52 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #53 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #54 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #55 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 65, SCH_GROUP_CFE_HK },   /* EPS HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #56 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  1, 30, SCH_GROUP_NONE },  /* CI HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  2, 31, SCH_GROUP_NONE },  /* TO HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #57 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #58 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #59 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #60 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #61 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #62 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #63 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #64 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #65 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  4,  1, 70, SCH_GROUP_NONE },  /* CSS HK Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #66 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #67 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #68 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #69 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #70 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 62, SCH_GROUP_NONE },  /* Torquer HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #71 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #72 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 
 
  /* slot #73 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #74 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
 
  /* slot #75 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  1, 75, SCH_GROUP_NONE },  /* IMU HK Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  2, 80, SCH_GROUP_NONE },  /* MAG HK Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #76 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #77 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #78 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #79 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #80 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #81 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #82 - ADCS */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #83 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #84 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #85 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  5,  4, 85, SCH_GROUP_NONE },  /* Radio HK Request */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #86 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #87 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #88 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #89 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #90 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #91 */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  0, 16, SCH_GROUP_CFS_HK }, */  /* HK Send Combined HK '1' */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  1, 17, SCH_GROUP_CFS_HK }, */  /* HK Send Combined HK '2' */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #92 - ADCS */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  2, 18, SCH_GROUP_CFS_HK }, */  /* HK Send Combined HK '3' */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  4,  3, 19, SCH_GROUP_CFS_HK }, */  /* HK Send Combined HK '4' */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 51, SCH_GROUP_NONE },  /* GPS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 61, SCH_GROUP_NONE },  /* FSS Data Request */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 71, SCH_GROUP_NONE },  /* CSS Data Request */                                          
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 76, SCH_GROUP_NONE },  /* IMU Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 81, SCH_GROUP_NONE },  /* MAG Data Request */ 

  /* slot #93 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,101, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0,100, SCH_GROUP_NONE },  /* ST Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #94 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #95 - Component HK */
/*{  SCH_DISABLED, SCH_ACTIVITY_SEND_MSG,  1,  0, 24, SCH_GROUP_MD_WAKEUP }, */  /* MD Wakeup */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                                   
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #96 */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #97 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 90, SCH_GROUP_NONE },  /* ADCS ADAC Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 91, SCH_GROUP_NONE },  /* ADCS DI Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 92, SCH_GROUP_NONE },  /* ADCS AD Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 93, SCH_GROUP_NONE },  /* ADCS GNC Data Request */ 
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 94, SCH_GROUP_NONE },  /* ADCS AC Data Request */ 

  /* slot #98 */
  {  SCH_ENABLED,  SCH_ACTIVITY_SEND_MSG,  1,  0, 95, SCH_GROUP_NONE },  /* ADCS DO Data Request */ 
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        

  /* slot #99 - Left Empty to allow Scheduler to Easily Resynchronize with 1 Hz */
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  {  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  //{  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE},                                        
  //{  SCH_UNUSED,   0,      0,  0, 0,  SCH_GROUP_NONE}                                       
};

/*
** Table file header
*/
CFE_TBL_FILEDEF(SCH_DefaultScheduleTable, SCH.SCHED_DEF, SCH schedule table, sch_def_schtbl.tbl)

/*************************************************************************
**
** File data
**
**************************************************************************/

/*
** (none)
*/

/*************************************************************************
**
** Local function prototypes
**
**************************************************************************/

/*
** (none)
*/

/************************/
/*  End of File Comment */
/************************/

