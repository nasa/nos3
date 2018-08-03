/************************************************************************
** File:
**
** Purpose: 
**  Define TO Lab Message IDs
**
** Notes:
**
** $Log: to_lab_msgids.h  $
** Revision 1.2 2010/09/20 12:28:05GMT-05:00 wmoleski 
** Modified the CI_LAB, SCH_LAB and TO_LAB applications to use unique message IDs and Pipe Names. The "_LAB"
** was added to all definitions so that a mission can use these "Lab" apps as well as their own mission apps together.
** Revision 1.1 2008/04/30 14:42:40EDT rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/platform_inc/project.pj
**
*************************************************************************/
#ifndef _to_lab_msgids_h_
#define _to_lab_msgids_h_


#define TO_LAB_CMD_MID            	0x1880
#define TO_LAB_SEND_HK_MID        	0x1881

#define TO_LAB_HK_TLM_MID		0x0880
#define TO_LAB_DATA_TYPES_MID	0x0881

#endif /* _to_lab_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
