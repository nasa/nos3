/*
 * osprintf_priv.h
 *
 *  Created on: May 20, 2013
 *      Author: Kevin McCluney
 *
 *
 *
 *  The source code in this file is designated as Safety Critical.
 */

#ifndef OSPRINTF_PRIV_H_
#define OSPRINTF_PRIV_H_

/*
** Macro definitions
*/
#define PAD_RIGHT 1
#define PAD_ZERO  2
#define PAD_HEX   4

#define LEAD_NEG   1
#define LEAD_SIGN  2
#define LEAD_SPACE 4
#define LEAD_HEX   8

/* Maximum number of characters needed to display the longest
 * 32 bit integer value
 */
#define PRINT_BUF_LEN 11

/* Maximum number of characters allowed to display a floating point value */
#define FLOAT_BUF_LEN 40

/*
** Function prototypes
*/
/*****************************************************************************/
/**
** \brief Determine the length of a character string
**
** \par Description
**        Count the number of characters in a string until a null character
**        is reached.  The null terminating character is not included in
**        the count.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  strptr  Pointer to a character string.
**
** \returns
** \retstmt
**        Returns the number of characters in the string. \endcode
** \endreturns
**
******************************************************************************/
int OS_GetStringLen(const char *strptr);

/*****************************************************************************/
/**
** \brief Output a single character to the string buffer or console
**
** \par Description
**        Send a single character to either the output string buffer or to the
**        console (or whatever output device is referenced by the putchar()
**        function).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] str  Pointer to the output character string buffer.
**
** \param[in]  c    Single character for output to the buffer.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #putchar
**
******************************************************************************/
void OS_OutputSingleChar(char **str, int c);

/*****************************************************************************/
/**
** \brief Convert a floating point value to a character string representation
**
** \par Description
**        Converts a floating point value into integer equivalents for the
**        whole and fractional portions, then converts these integers into
**        a character string representing the original floating point value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out_bfr     Pointer to a character string buffer.
**
** \param[in]  dbl         Double precision floating point value to convert.
**
** \param[in]  dec_digits  Number of digits to display to the right of the
**                         decimal point.
**
** \param[in]  lead        Pointer to the leading character bit flag.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #OS_GetStringLen
**
******************************************************************************/
void OS_Double2String(char *out_bfr, double dbl,
                      unsigned dec_digits, int *lead);

/*****************************************************************************/
/**
** \brief Format a character string for output
**
** \par Description
**        Apply any special formatting (leading zeroes, +/- sign, spaces) to
**        a string, then output to the buffer or console.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out        Pointer to the output character string buffer.
**
** \param[in]  *string    Pointer to the string to format and then output.
**
** \param[in]  min_width  Minimum resulting string length.
**
** \param[in]  max_width  Maximum resulting string length.
**
** \param[in]  pad        Pointer to the leading character bit flag.
**
** \param[in]  lead       Leading character bit flag.
**
** \param[in]  s_flag     Non-zero if this function is called by the %s
**                        handler; zero otherwise.  Used to determine how the
**                        maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_GetStringLen, #OS_OutputSingleChar
**
******************************************************************************/
int OS_ParseString(char **out, const char *string,
                   int min_width, int max_width, int pad,
                   int lead, unsigned s_flag);

/*****************************************************************************/
/**
** \brief Convert an integer value into a character string
**
** \par Description
**        Converts an integer value into its equivalent character string
**        representation.  The resulting string is then passed to
**        OS_ParseString() for special formatting and output to  the
**        buffer or console.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out              Pointer to the output character string buffer.
**
** \param[in]  i                Integer to convert.
**
** \param[in]  base             = 10 for base 10 (decimal); = 16 for base 16
**                              (hexadecimal).
**
** \param[in]  sign             Non-zero for integer values; zero for unsigned
**                              integer values .
**
** \param[in]  space_pad_width  Format modifier minimum field width (e.g.,
**                              the 5 in %5.2d).
**
** \param[in]  zero_pad_width   Format modifier precision specifier (e.g.,
**                              the 2 in %5.2d).
**
** \param[in]  pad              Pointer to the leading character bit flag.
**
** \param[in]  lead             Leading character bit flag.
**
** \param[in]  letbase          Non-zero if this function is called by the %s
**                              handler; zero otherwise.  Used to determine how the
**                              maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_ParseString
**
******************************************************************************/
int OS_ParseInteger(char **out, int i, unsigned base, int sign,
                    int space_pad_width, int zero_pad_width,
                    int pad, int lead, int letbase);


#endif /* OSPRINTF_PRIV_H_ */
