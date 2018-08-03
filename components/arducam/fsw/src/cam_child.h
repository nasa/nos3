/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

#ifndef _cam_child_h_
#define _cam_child_h_

#include "cfe.h"
#include "cam_app.h"
#include "cam_perfids.h"
#include "cam_msgids.h"
#include "cam_msg.h"
#include "cam_events.h"
#include "cam_version.h"
#include "cam_child.h"
#include "cam_platform_cfg.h"
#include "cam_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CAM child task global function prototypes                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 CAM_ChildInit(void);
void  CAM_ChildTask(void);
int32 CAM_publish(void);
int32 CAM_eoe_publish(int32);
int32 CAM_state(void);
int32 CAM_fifo(uint16*, uint8*);
int32 CAM_exp(void); 

#endif /* _cam_child_h_ */

/************************/
/*  End of File Comment */
/************************/

