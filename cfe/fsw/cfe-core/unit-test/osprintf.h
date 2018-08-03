/*
 * osprintf.h
 *
 *  Created on: May 20, 2013
 *      Author: Kevin McCluney
 *
 *
 *      The source code in this file is designated as Safety Critical.

    - Define OS_USE_EMBEDDED_PRINTF to use the embedded (replacement) printf
      functions; undefine to use the standard printf library functions
    - Define OSP_NO_FLOAT to exclude the code to handle floating point values
      in the variable parameter printf functions; undefine to include this code
    - Define OSP_ARINC653 and OSP_ARINC653_CFE if using embedded (replacement)
      printf functions and compiling on the GHS target using cFE modified for
      ARINC653
 */

#ifndef OSPRINTF_H_
#define OSPRINTF_H_

#ifdef OS_USE_EMBEDDED_PRINTF /* Use replacement printf functions */
/*
** Includes
*/
#include "common_types.h"

#ifndef OS_PRINTF
#define OS_PRINTF(m,n)
#endif

/*
** Macro definitions
*/
#define VA_LIST int*

#ifdef OSP_ARINC653 /* Compiling on GHS target */

#define OFFSET_CFE_ES_WRITETOSYSLOG 6
#define OFFSET_EVS_SENDEVENT 8
#define OFFSET_CFE_EVS_SENDEVENT 7
#define OFFSET_CFE_SENDEVENTWITHAPPID 9
#define OFFSET_CFE_EVS_SENDTIMEDEVENT 6
#define OFFSET_OS_PRINTF 2
#define OFFSET_OS_SPRINTF 3
#define OFFSET_OS_SNPRINTF 4

#define BREAK_CFE_ES_WRITETOSYSLOG 7
#define BREAK_EVS_SENDEVENT 5
#define BREAK_CFE_EVS_SENDEVENT 5
#define BREAK_CFE_SENDEVENTWITHAPPID 4
#define BREAK_CFE_EVS_SENDTIMEDEVENT 4
#define BREAK_OS_PRINTF 7
#define BREAK_OS_SPRINTF 6
#define BREAK_OS_SNPRINTF 5

#define SKIP_CFE_ES_WRITETOSYSLOG 90
#define SKIP_EVS_SENDEVENT 64
#define SKIP_CFE_EVS_SENDEVENT 64
#define SKIP_CFE_SENDEVENTWITHAPPID 66
#define SKIP_CFE_EVS_SENDTIMEDEVENT 66
#define SKIP_OS_PRINTF 20
#define SKIP_OS_SPRINTF 22
#define SKIP_OS_SNPRINTF 22

#define VA_START(ptr, string, offset, breakpt, skip) ptr = \
    ((VA_LIST) (char *) &string) + offset + 1; \
    if (OS_PrintfMutexId == 0xffffffff) OS_PrintfMutexId = \
    OS_MutSemCreate(&OS_PrintfMutexId, "OS_PrintfMutex", 0); \
    OS_MutSemTake(OS_PrintfMutexId); \
    OS_printf_break = breakpt; OS_printf_skip = skip

#define VA_END(ptr) OS_MutSemGive(OS_PrintfMutexId)

extern uint32 OS_PrintfMutexId;
extern int OS_printf_break;
extern int OS_printf_skip;

#else /* Compiling on other than GHS target (e.g., Linux) */

#define VA_START(ptr, string, offset, breakpt, skip) ptr = \
    ((VA_LIST) (char *) &string) + 1

#define VA_END(ptr)

#endif /* Target */

#define VSNPRINTF OS_vsnprintf
#define SNPRINTF OS_snprintf
#define SPRINTF OS_sprintf
#define PRINTF OS_printf

/*
** Function prototypes
*/
/*****************************************************************************/
/**
** \brief Print formatted output to stdout
**
** \par Description
**        Writes the C string pointed by format to the standard output
**        (stdout).  If format includes format specifiers (identified by a
**        percent sign, %, followed by a format code; e.g., d, s, c), the
**        variable list of arguments following format are formatted
**        accordingly and inserted in the output string, replacing the
**        respective format specifier.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
void OS_printf(const char *format, ...) OS_PRINTF(1,2);

/*****************************************************************************/
/**
** \brief Write formatted output to a character string
**
** \par Description
**        Creates a formatted string identical to that if printf is used, but
**        instead stores the resulting content in the buffer pointed by out.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_sprintf(char *out, const char *format, ...) OS_PRINTF(2,3);

/*****************************************************************************/
/**
** \brief Write formatted output to a sized buffer
**
** \par Description
**        Creates a formatted string identical to that if printf is used, but
**        instead stores the resulting content in the buffer pointed by out to
**        a maximum of max_len characters.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  max_len      Maximum allowed length of the output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_snprintf(char *out, unsigned max_len, const char *format, ...) OS_PRINTF(3,4);

/*****************************************************************************/
/**
** \brief Write formatted data from a variable argument list to a sized buffer
**
** \par Description
**        Creates a formatted string identical to that if printf is used, but
**        uses the elements in the variable argument list pointed to by varg
**        instead of additional function arguments and stores the resulting
**        content in the buffer pointed by out to a maximum of max_len
**        characters.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out     Pointer to character buffer to hold the  output string.
**
** \param[in]  max_len  Maximum allowed length of the output string.
**
** \param[in]  *format  Pointer to a printf-style format string.
**
** \param[in]  varg     Pointer to the beginning of the list of variables
**                      to output per the format string.
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int OS_vsnprintf(char *out_buffer, int max_len,
                 const char *format, VA_LIST varg);

#else  /* Use standard printf functions */

/*
** Includes
*/
#include <stdarg.h>

/*
** Macro definitions
*/
#define VA_LIST va_list
#define VA_START(ptr, string, offset, breakpt, skip) va_start(ptr, string)
#define VA_END va_end

#define VSNPRINTF vsnprintf
#define SNPRINTF snprintf
#define SPRINTF sprintf
#define PRINTF printf

#endif /* Replacement/standard printf */

#endif /* OSPRINTF_H_ */
