/************************************************************************
** File:
**   $Id: ci_lab_msgids.h 1.2 2010/09/20 12:27:18GMT-05:00 wmoleski Exp  $
**
** Purpose: 
**  Define CI Lab Message IDs
**
** Notes:
**
** $Log: ci_lab_msgids.h  $
** Revision 1.2 2010/09/20 12:27:18GMT-05:00 wmoleski 
** Modified the CI_LAB, SCH_LAB and TO_LAB applications to use unique message IDs and Pipe Names. The "_LAB"
** was added to all definitions so that a mission can use these "Lab" apps as well as their own mission apps together.
** Revision 1.1 2008/04/30 13:07:18EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ci_lab/fsw/platform_inc/project.pj
**
*************************************************************************/
#ifndef _ci_lab_msgids_h_
#define _ci_lab_msgids_h_


#define CI_LAB_CMD_MID            	0x1884
#define CI_LAB_SEND_HK_MID        	0x1885

#define CI_LAB_HK_TLM_MID		0x0884

#endif /* _ci_lab_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
