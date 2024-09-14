//this file is part of eMule
//Copyright (C)2002 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "emule.h"
#include "Server.h"
#include "Opcodes.h"
#include "OtherFunctions.h"
#include "Packets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CServer::CServer(const ServerMet_Struct* in_data)
{
	port = in_data->port;
	ip = in_data->ip;
	_tcscpy(ipfull, ipstr(ip));
	files = 0;
	users = 0;
	preferences = 0;
	ping = 0;
	failedcount = 0; 
	lastpinged = 0;
	lastpingedtime = 0;
	staticservermember = 0;
	maxusers = 0;
	softfiles = 0;
	hardfiles = 0;
	lastdescpingedcout = 0;
	m_uTCPFlags = 0;
	m_uUDPFlags = 0;
	m_uDescReqChallenge = 0;
	m_uLowIDUsers = 0;
	challenge = 0;
}

CServer::CServer(uint16 in_port, LPCTSTR i_addr)
{
	USES_CONVERSION;
	port = in_port;
	if ((ip = inet_addr(T2CA(i_addr))) == INADDR_NONE && _tcscmp(i_addr, _T("255.255.255.255")) != 0){
		m_strDynIP = i_addr;
		ip = 0;
	}
	_tcscpy(ipfull, ipstr(ip));
	files = 0;
	users = 0;
	preferences = 0;
	ping = 0;
	failedcount = 0;
	lastpinged = 0;
	lastpingedtime = 0;
	staticservermember = 0;
	maxusers = 0;
	softfiles = 0;
	hardfiles = 0;
	lastdescpingedcout = 0;
	m_uTCPFlags = 0;
	m_uUDPFlags = 0;
	m_uDescReqChallenge = 0;
	m_uLowIDUsers = 0;
	challenge = 0;
}

CServer::CServer(const CServer* pOld)
{
	port = pOld->port;
	ip = pOld->ip; 
	staticservermember = pOld->IsStaticMember();
	_tcscpy(ipfull, pOld->ipfull);
	files = pOld->files;
	users = pOld->users;
	preferences = pOld->preferences;
	ping = pOld->ping;
	failedcount = pOld->failedcount;
	lastpinged = pOld->lastpinged;
	lastpingedtime = pOld->lastpingedtime;
	maxusers = pOld->maxusers;
	softfiles = pOld->softfiles;
	hardfiles = pOld->hardfiles;
	lastdescpingedcout = pOld->lastdescpingedcout;
	m_strDescription = pOld->m_strDescription;
	m_strName = pOld->m_strName;
	m_strDynIP = pOld->m_strDynIP;
	m_strVersion = pOld->m_strVersion;
	m_uTCPFlags = pOld->m_uTCPFlags;
	m_uUDPFlags = pOld->m_uUDPFlags;
	m_uDescReqChallenge = pOld->m_uDescReqChallenge;
	m_uLowIDUsers = pOld->m_uLowIDUsers;
	challenge = pOld->challenge;
}

CServer::~CServer()
{
}

bool CServer::AddTagFromFile(CFileDataIO* servermet)
{
	if (servermet == NULL)
		return false;
	CTag* tag = new CTag(servermet, false);
	switch(tag->GetNameID()){		
	case ST_SERVERNAME:
		ASSERT( tag->IsStr() );
		if (tag->IsStr()){
#ifdef _UNICODE
			if (m_strName.IsEmpty())
#endif
				m_strName = tag->GetStr();
		}
		break;
	case ST_DESCRIPTION:
		ASSERT( tag->IsStr() );
		if (tag->IsStr()){
#ifdef _UNICODE
			if (m_strDescription.IsEmpty())
#endif
				m_strDescription = tag->GetStr();
		}
		break;
	case ST_PING:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			ping = tag->GetInt();
		break;
	case ST_FAIL:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			failedcount = tag->GetInt();
		break;
	case ST_PREFERENCE:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			preferences = tag->GetInt();
		break;
	case ST_DYNIP:
		ASSERT( tag->IsStr() );
		if (tag->IsStr()){
#ifdef _UNICODE
			if (m_strDynIP.IsEmpty())
#endif
				m_strDynIP = tag->GetStr();
		}
		break;
	case ST_MAXUSERS:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			maxusers = tag->GetInt();
		break;
	case ST_SOFTFILES:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			softfiles = tag->GetInt();
		break;
	case ST_HARDFILES:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			hardfiles = tag->GetInt();
		break;
	case ST_LASTPING:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			lastpingedtime = tag->GetInt();
		break;
	case ST_VERSION:
		if (tag->IsStr()){
#ifdef _UNICODE
			if (m_strVersion.IsEmpty())
#endif
				m_strVersion = tag->GetStr();
		}
		else if (tag->IsInt())
			m_strVersion.Format(_T("%u.%u"), tag->GetInt() >> 16, tag->GetInt() & 0xFFFF);
		else
			ASSERT(0);
		break;
	case ST_UDPFLAGS:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			m_uUDPFlags = tag->GetInt();
		break;
	case ST_LOWIDUSERS:
		ASSERT( tag->IsInt() );
		if (tag->IsInt())
			m_uLowIDUsers = tag->GetInt();
		break;
	default:
		if (tag->GetNameID()){
			ASSERT( 0 );
		}
		else if (!CmpED2KTagName(tag->GetName(), "files")){
			ASSERT( tag->IsInt() );
			if (tag->IsInt())
				files = tag->GetInt();
		}
		else if (!CmpED2KTagName(tag->GetName(), "users")){
			ASSERT( tag->IsInt() );
			if (tag->IsInt())
				users = tag->GetInt();
		}
	}
	delete tag;
	return true;
}

void CServer::SetListName(LPCTSTR newname)
{
	m_strName = newname;
}

void CServer::SetDescription(LPCTSTR newname)
{
	m_strDescription = newname;
}

LPCTSTR CServer::GetAddress() const
{
	if (!m_strDynIP.IsEmpty())
		return m_strDynIP;
	else
		return ipfull;
}

void CServer::SetIP(uint32 newip)
{
	ip = newip;
	_tcscpy(ipfull, ipstr(ip));
}

void CServer::SetDynIP(LPCTSTR newdynip)
{
	m_strDynIP = newdynip;
}

void CServer::SetLastDescPingedCount(bool bReset)
{
	if (bReset)
		lastdescpingedcout = 0;
	else
		lastdescpingedcout++;
}
