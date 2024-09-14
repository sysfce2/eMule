//this file is part of eMule
//Copyright (C)2002-2008 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// MSDN: Using the Windows Headers
// ===========================================================
//Windows Vista			_WIN32_WINNT>=0x0600	WINVER>=0x0600
//Windows Server 2003	_WIN32_WINNT>=0x0502    WINVER>=0x0502
//Windows XP			_WIN32_WINNT>=0x0501	WINVER>=0x0501
//Windows 2000			_WIN32_WINNT>=0x0500    WINVER>=0x0500
//Windows NT 4.0		_WIN32_WINNT>=0x0400	WINVER>=0x0400
//Windows Me			_WIN32_WINDOWS=0x0500	WINVER>=0x0500
//Windows 98			_WIN32_WINDOWS>=0x0410	WINVER>=0x0410
//Windows 95			_WIN32_WINDOWS>=0x0400	WINVER>=0x0400
//
//IE 7.0				_WIN32_IE>=0x0700 
//IE 6.0 SP2			_WIN32_IE>=0x0603 
//IE 6.0 SP1			_WIN32_IE>=0x0601 
//IE 6.0				_WIN32_IE>=0x0600 
//IE 5.5				_WIN32_IE>=0x0550 
//IE 5.01				_WIN32_IE>=0x0501 
//IE 5.0, 5.0a, 5.0b	_WIN32_IE>=0x0500 
//IE 4.01				_WIN32_IE>=0x0401 
//IE 4.0				_WIN32_IE>=0x0400 
//IE 3.0, 3.01, 3.02	_WIN32_IE>=0x0300 

#if defined(HAVE_VISTA_SDK)

#ifndef WINVER
#define WINVER 0x0502			// 0x0502 == Windows Server 2003, Windows XP (same as VS2005-MFC)
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT WINVER		// same as VS2005-MFC
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410	// 0x0410 == Windows 98
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0560		// 0x0560 == Internet Explorer 5.6 -> Comctl32.dll v5.8
#endif

#else//HAVE_VISTA_SDK

#ifndef WINVER
#define WINVER 0x0400			// 0x0400 == Windows 98 and Windows NT 4.0 (because of '_WIN32_WINDOWS=0x0410')
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400		// 0x0400 == Windows NT 4.0
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410	// 0x0410 == Windows 98
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0560		// 0x0560 == Internet Explorer 5.6 -> Comctl32.dll v5.8 (same as MFC internally used)
#endif

#endif//HAVE_VISTA_SDK

#define _ATL_FREE_THREADED					// at least one object has ThreadingModel = 'Both'
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// Makes certain CString constructors explicit, preventing any unintentional conversions
#define	_ATL_EX_CONVERSION_MACROS_ONLY		// Disable old ATL 3.0 string conversion macros
#ifdef _ATL_EX_CONVERSION_MACROS_ONLY
#define CharNextO CharNextW					// work around a bug in ATL headers
#endif//_ATL_EX_CONVERSION_MACROS_ONLY
#define _CONVERSION_DONT_USE_THREAD_LOCALE	// for consistency with C-RTL/MFC the ATL thread locale support has to get disabled
//#define _ATL_NO_COM_SUPPORT
#define _ATL_NO_PERF_SUPPORT
//#define _ATL_NO_COMMODULE
#define _ATL_NO_CONNECTION_POINTS
#define _ATL_NO_DOCHOSTUIHANDLER
#define _ATL_NO_HOSTING

#define _ATL_ALL_WARNINGS
#define _AFX_ALL_WARNINGS
// Disable some warnings which get fired with /W4 for Windows/MFC/ATL headers
#pragma warning(disable:4127) // conditional expression is constant

// Disable some warnings which are only generated when using "/Wall"
#pragma warning(disable:4061) // enumerate in switch of enum is not explicitly handled by a case label
#pragma warning(disable:4062) // enumerate in switch of enum is not handled
#pragma warning(disable:4191) // 'type cast' : unsafe conversion from <this> to <that>
#if _MSC_VER<1400
#pragma warning(disable:4217) // <func>: member template functions cannot be used for copy-assignment or copy-construction
#endif
#pragma warning(disable:4263) // <func> member function does not override any base class virtual member function
#pragma warning(disable:4264) // <func>: no override available for virtual member function from base <class>; function is hidden
#pragma warning(disable:4265) // <class>: class has virtual functions, but destructor is not virtual
#if _MSC_VER>=1400
#pragma warning(disable:4266) // no override available for virtual member function from base <class>; function is hidden
#endif
#if _MSC_VER<1400
#pragma warning(disable:4529) // forming a pointer-to-member requires explicit use of the address-of operator ('&') and a qualified name
#endif
#if _MSC_VER>=1400
#pragma warning(disable:4365) // conversion from 'int' to 'UINT', signed/unsigned mismatch
#endif
#pragma warning(disable:4548) // expression before comma has no effect; expected expression with side-effect
#pragma warning(disable:4555) // expression has no effect; expected expression with side-effect
#if _MSC_VER>=1400
#pragma warning(disable:4571) // Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#endif
#pragma warning(disable:4619) // #pragma warning : there is no warning number <n>
#pragma warning(disable:4625) // <class> : copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4626) // <class> : assignment operator could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable:4640) // construction of local static object is not thread-safe
#pragma warning(disable:4668) // <name>  is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable:4710) // function not inlined
#pragma warning(disable:4711) // function <func> selected for automatic inline expansion
#if _MSC_VER>=1400
#pragma warning(disable:4738) // storing 32-bit float result in memory, possible loss of performance
#endif
#pragma warning(disable:4820) // <n> bytes padding added after member <member>
#pragma warning(disable:4917) // a GUID can only be associated with a class, interface or namespace
#pragma warning(disable:4928) // illegal copy-initialization; more than one user-defined conversion has been implicitly applied

#if _MSC_VER>=1400
#pragma warning(disable:6211) // Leaking memory <name> due to an exception
#pragma warning(disable:6246) // Local declaration of <name> hides declaration of the same name in outer scope
#pragma warning(disable:6284) // Object passed as parameter <num> when string is required in call to <printf>
#pragma warning(disable:6387) // <argument> might be '0': this does not adhere to the specification for the function <name>
#pragma warning(disable:6309) // Argument <n> is null: this does not adhere to function specification of <func>
#pragma warning(disable:6255) // _alloca indicates failure by raising a stack overflow exception.
#endif

#if _MSC_VER>=1400

// _CRT_SECURE_NO_DEPRECATE - Disable all warnings for not using "_s" functions.
//
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

// _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES - Overloads all standard string functions (e.g. strcpy) with "_s" functions
// if, and only if, the size of the output buffer is known at compile time (so, if it is a static array). If there is
// a buffer overflow during runtime, it will throw an exception.
//
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

// _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT - This is a cool CRT feature but does not make sense for our code.
// With our existing code we could get exceptions which are though not justifiable because we explicitly
// terminate all our string buffers. This define could be enabled for debug builds though.
//
//#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
//#endif

#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES) || (_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES==0)
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#endif//!defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES) || (_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES==0)

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#endif//_MSC_VER>=1400

#ifdef _DEBUG
#define _ATL_DEBUG
#define _ATL_DEBUG_QI
#endif

#ifdef _DEBUG
//#define TRACE_OBJ TRACE
#define	TRACE_OBJ	__noop
#define TRACE_INTFN TRACE
//#define TRACE_INTFN __noop
#else
#define TRACE_OBJ	TRACE
#define TRACE_INTFN	TRACE
#endif

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions
#include <afxdisp.h>        // MFC IDispatch & ClassFactory support
#include <..\src\mfc\afximpl.h>
#include <atlbase.h>
#include <atlcom.h>
#include <shlguid.h>



// Enable warnings which were disabled for Windows/MFC/ATL headers
//#pragma warning(default:4505) // unreferenced local function has been removed
#pragma warning(default:4127) // conditional expression is constant
#if _MSC_VER<=1310
#pragma warning(default:4548) // expression before comma has no effect; expected expression with side-effect
#endif
#if _MSC_VER==1310
#pragma warning(default:4555) // expression has no effect; expected expression with side-effect
#endif

// when using warning level 4
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union (not worth to mess with, it's due to MIDL created code)
#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue
#if _MSC_VER>=1400
#pragma warning(disable:4127) // conditional expression is constant
#endif

#include "types.h"

#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
#define malloc(s)		  _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)	  _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)	  _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s)	  _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(p)			  _free_dbg(p, _NORMAL_BLOCK)
#define _msize(p)		  _msize_dbg(p, _NORMAL_BLOCK)
#endif

#define _TWINAPI(fname)	fname "W"

extern "C" int __cdecl __ascii_stricmp(const char * dst, const char * src);

inline BOOL afxIsWin95()
{
#if _MFC_VER>=0x0900
	return FALSE;
#else
	return afxData.bWin95;
#endif
}
