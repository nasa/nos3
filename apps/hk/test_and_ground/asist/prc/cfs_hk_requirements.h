/*******************************************************************************
*  cfs_hk_requirements.h
*   
*  Purpose:  This header file is used by the hk test procs.  It defines
*             all the requirements tested by all procs other then hk_gencmds
*             It also defines some contants used by the procs
*  Change History
*	Date		   Name		Description
*	06/5/08	 Barbie Medina	Original header file.
*
*******************************************************************************/

#ifndef _cfs_hk_requirements_
#define _cfs_hk_requirements_

global ut_req_array_size = 9
global ut_requirement [0 .. ut_req_array_size]

#define HK_2000        0
#define HK_2001        1
#define HK_20011       2
#define HK_20012       3
#define HK_20013       4
#define HK_20015       5 
#define HK_20016       6 
#define HK_20017       7 
#define HK_3000        8
#define HK_4000        9

#define NoChecks      0     
#define MissingYes    1
#define TstPktLen     2
#define TblUpdate     3
#define TstLenTstTbl  4

#define Pkt1           1
#define Pkt2           2
#define Pkt3           3
#define Pkt4           4
#define Pkt5           5
#define Pkt6           6

#endif
