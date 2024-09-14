#ifndef __VERSION_H__
#define __VERSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _T
#define _T(x)	x
#endif

#define _chSTR(x)		_T(#x)
#define chSTR(x)		_chSTR(x)

#define PACKAGE_VERSION "1.0.5"

#define VERSION_MJR		0
#define VERSION_MIN		5
#define VERSION_UPDATE	1
#define VERSION_BUILD	0
#ifdef _DEBUG
  #ifdef _UNICODE
    #define VERSION_SPECIAL_BUILD	_T(" - Unicode Debug")
  #else
    #define VERSION_SPECIAL_BUILD	_T(" - Debug")
  #endif
#else
  #ifdef _UNICODE
    #define VERSION_SPECIAL_BUILD	_T(" - Unicode")
  #else
    #define VERSION_SPECIAL_BUILD	_T("")
  #endif
#endif

#define	SZ_VERSION_NAME		chSTR(VERSION_MJR) _T(".") chSTR(VERSION_MIN) _T(".") chSTR(VERSION_UPDATE) VERSION_SPECIAL_BUILD
#define	SZ_PRODUCT_NAME		_T("eMule part file access for VideoLAN") _T(" ") _T(PACKAGE_VERSION)
#define	SZ_COMPANY_NAME		_T("http://emule-project.net")
#define	SZ_COPYRIGHT		_T("Copyright © 2010 ") SZ_COMPANY_NAME

#endif /* !__VERSION_H__ */
