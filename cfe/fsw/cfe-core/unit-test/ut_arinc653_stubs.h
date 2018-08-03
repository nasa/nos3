/*
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
*/
#ifndef __UT_ARINC653_STUBS_H_
#define __UT_ARINC653_STUBS_H_

#ifdef CFE_ARINC653
/*****************************************************************************/
/**
** \brief Set the library initialization return code
**
** \par Description
**        Set the library initialization return code.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] LibInitRtn  Library init return code
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetLibInitRtn(int32 LibInitRtn);

/*****************************************************************************/
/**
** \brief Return the library initialization return code
**
** \par Description
**        Return the library initialization return code.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the library initialization return code.
**
******************************************************************************/
uint32 UT_LibInit(void);

#endif

#endif /* __UT_ARINC653_STUBS_H_ */
