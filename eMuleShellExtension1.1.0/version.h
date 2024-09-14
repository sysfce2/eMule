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
#ifndef __VERSION_H__
#define __VERSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define _chSTR(x)		#x
#define chSTR(x)		_chSTR(x)

#define VERSION_MJR		1
#define VERSION_MIN		1
#define VERSION_UPDATE	0
#define VERSION_BUILD	0
#ifdef _DEBUG
  #define VERSION_SPECIAL_BUILD " - Debug"
#else
  #define VERSION_SPECIAL_BUILD
#endif

#define	SZ_VERSION_NAME		chSTR(VERSION_MJR) "." chSTR(VERSION_MIN) "." chSTR(VERSION_UPDATE) VERSION_SPECIAL_BUILD
#define	SZ_PRODUCT_NAME		"eMule Shell Extension"
#define	SZ_COMPANY_NAME		"http://emule-project.net"
#define	SZ_COPYRIGHT		"Copyright © 2006,2008 " SZ_COMPANY_NAME

#endif /* !__VERSION_H__ */
