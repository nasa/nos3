/**********************************************************
** File: sbn_utils.h
**
** Author: ejtimmon
** Date: 01 Sep 2015
**
** Purpose:
**  This file contains SBN functions that should be 
**  accessible by the SBN application as well as SBN
**  interface modules.
**
**********************************************************/

#ifndef _sbn_lib_utils_h_
#define _sbn_lib_utils_h_

#include "cfe.h"
#include "sbn_lib_defs.h"

int32 SBN_LIB_MsgTypeIsProto(uint32 MsgType);
int32 SBN_LIB_MsgTypeIsData(uint32 MsgType);
char *SBN_LIB_GetMsgName(uint32 MsgType);
char *SBN_LIB_StateNum2Str(uint32 StateNum);

int32 SBN_EndianMemCpy(void *dest, void *src, uint32 n);

#endif /* _sbn_utils_h_ */
