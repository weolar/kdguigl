/*
 * wsprintf functions
 *
 * Copyright 1996 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * NOTE:
 * This code is duplicated in user32. If you change something here make sure
 * to change it in user32 too.
 */



#ifdef _MSC_VER

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "windef.h"
#include "winbase.h"
#define NO_SHLWAPI_REG
#include "shlwapi.h"

#ifndef WANG_BIN4_5
#define WANG_BIN4_5 

//#include "wine/debug.h"

//WINE_DEFAULT_DEBUG_CHANNEL(string);

#define WPRINTF_LEFTALIGN   0x0001  /* Align output on the left ('-' prefix) */
#define WPRINTF_PREFIX_HEX  0x0002  /* Prefix hex with 0x ('#' prefix) */
#define WPRINTF_ZEROPAD     0x0004  /* Pad with zeros ('0' prefix) */
#define WPRINTF_LONG        0x0008  /* Long arg ('l' prefix) */
#define WPRINTF_SHORT       0x0010  /* Short arg ('h' prefix) */
#define WPRINTF_UPPER_HEX   0x0020  /* Upper-case hex ('X' specifier) */
#define WPRINTF_WIDE        0x0040  /* Wide arg ('w' prefix) */

#ifndef GWLP_USERDATA
#define GWLP_USERDATA       (-21)
#endif
#define   AC_SRC_ALPHA      0x01 

typedef enum
{
    WPR_UNKNOWN,
    WPR_CHAR,
    WPR_WCHAR,
    WPR_STRING,
    WPR_WSTRING,
    WPR_SIGNED,
    WPR_UNSIGNED,
    WPR_HEXA
} WPRINTF_TYPE;

typedef struct
{
    UINT         flags;
    UINT         width;
    UINT         precision;
    WPRINTF_TYPE   type;
} WPRINTF_FORMAT;

typedef union {
    WCHAR   wchar_view;
    CHAR    char_view;
    LPCSTR  lpcstr_view;
    LPCWSTR lpcwstr_view;
    INT     int_view;
} WPRINTF_DATA;

/***********************************************************************
 *           WPRINTF_ParseFormatA
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 */
static INT WPRINTF_ParseFormatA( LPCSTR format, WPRINTF_FORMAT *res );

/***********************************************************************
 *           WPRINTF_ParseFormatW
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 */
static INT WPRINTF_ParseFormatW( LPCWSTR format, WPRINTF_FORMAT *res );


/***********************************************************************
 *           WPRINTF_GetLen
 */
static UINT WPRINTF_GetLen( WPRINTF_FORMAT *format, WPRINTF_DATA *arg,
                              LPSTR number, UINT maxlen );

/***********************************************************************
 *           wvnsprintfA   (SHLWAPI.@)
 *
 * Print formatted output to a string, up to a maximum number of chars.
 *
 * PARAMS
 * buffer [O] Destination for output string
 * maxlen [I] Maximum number of characters to write
 * spec   [I] Format string
 *
 * RETURNS
 *  Success: The number of characters written.
 *  Failure: -1.
 */
//extern "C" INT WINAPI wvnsprintfA( LPSTR buffer, INT maxlen, LPCSTR spec, va_list args );

/***********************************************************************
 *           wvnsprintfW   (SHLWAPI.@)
 *
 * See wvnsprintfA.
 */
//extern "C" INT WINAPI wvnsprintfW( LPWSTR buffer, INT maxlen, LPCWSTR spec, va_list args );


/*************************************************************************
 *           wnsprintfA   (SHLWAPI.@)
 *
 * Print formatted output to a string, up to a maximum number of chars.
 *
 * PARAMS
 * lpOut      [O] Destination for output string
 * cchLimitIn [I] Maximum number of characters to write
 * lpFmt      [I] Format string
 *
 * RETURNS
 *  Success: The number of characters written.
 *  Failure: -1.
 */
//extern "C" int WINAPIV wnsprintfA(LPSTR lpOut, int cchLimitIn, LPCSTR lpFmt, ...);

/*************************************************************************
 *           wnsprintfW   (SHLWAPI.@)
 *
 * See wnsprintfA.
 */
//extern "C" int WINAPIV wnsprintfW(LPWSTR lpOut, int cchLimitIn, LPCWSTR lpFmt, ...);

#endif

#else

inline bool IsDebuggerPresent ()
{
	return true;
}

inline void DebugBreak()
{
	*(int*)0 = 0;
}

#endif // _MSC_VER

inline void KDASSERT(BOOL b)
{
	if (!b && IsDebuggerPresent())
		DebugBreak();
}