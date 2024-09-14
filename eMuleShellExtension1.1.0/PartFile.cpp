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
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "opcodes.h"
#include "PartFile.h"
#include "SafeFile.h"
#include "Packets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CPartFile::CPartFile()
{
	(void)m_strFileName;
	//m_ullFileSize = 0;
	//(void)m_strFileType;
	//m_bStopped = false;
	(void)m_strMediaArtist;
	(void)m_strMediaAlbum;
	(void)m_strMediaTitle;
	m_nMediaLength = 0;
	m_nMediaBitrate = 0;
	(void)m_strMediaCodec;
	m_ftCreate.dwLowDateTime = 0;
	m_ftCreate.dwHighDateTime = 0;
	m_ftAccess.dwLowDateTime = 0;
	m_ftAccess.dwHighDateTime = 0;
	m_ftLastWrite.dwLowDateTime = 0;
	m_ftLastWrite.dwHighDateTime = 0;
}

bool CPartFile::LoadTags(LPCTSTR pszFilePath)
{
	CSafeBufferedFile metFile;
	if (!metFile.Open(pszFilePath, CFile::modeRead|CFile::osSequentialScan|CFile::typeBinary|CFile::shareDenyWrite))
		return false;
	setvbuf(metFile.m_pStream, NULL, _IOFBF, 16384);
	VERIFY( GetFileTime(metFile.m_hFile, &m_ftCreate, &m_ftAccess, &m_ftLastWrite) );

	try{
		uint8 version = metFile.ReadUInt8();
		if (version != PARTFILE_VERSION && version != PARTFILE_SPLITTEDVERSION && version != PARTFILE_VERSION_LARGEFILE){
			metFile.Close();
			return false;
		}
		
		//LoadDateFromFile(&metFile);
		(void)metFile.ReadUInt32();
		
		//LoadHashsetFromFile(&metFile);
		metFile.Seek(16, CFile::current);
		UINT parts = metFile.ReadUInt16();
		metFile.Seek(16*parts, CFile::current);

		UINT tagcount = metFile.ReadUInt32();
		for (UINT j = 0; j < tagcount; j++)
		{
			CTag* newtag = new CTag(&metFile, false);
			switch (newtag->GetNameID())
			{
				case FT_FILENAME:
					if (!newtag->IsStr()) {
						delete newtag;
						return false;
					}
					m_strFileName = newtag->GetStr();
					break;
				//case FT_FILESIZE:
				//	if (newtag->IsInt64(true))
				//		m_ullFileSize = newtag->GetInt64();
				//	break;
				//case FT_FILETYPE:
				//	if (newtag->IsStr())
				//		m_strFileType = newtag->GetStr();
				//	break;
				//case FT_STATUS:
				//	if (newtag->IsInt())
				//		m_bStopped = newtag->GetInt()!=0;
				//	break;
				case FT_MEDIA_ARTIST:
					if (newtag->IsStr())
						m_strMediaArtist = newtag->GetStr();
					break;
				case FT_MEDIA_ALBUM:
					if (newtag->IsStr())
						m_strMediaAlbum = newtag->GetStr();
					break;
				case FT_MEDIA_TITLE:
					if (newtag->IsStr())
						m_strMediaTitle = newtag->GetStr();
					break;
				case FT_MEDIA_LENGTH:
					if (newtag->IsInt())
						m_nMediaLength = newtag->GetInt();
					break;
				case FT_MEDIA_BITRATE:
					if (newtag->IsInt())
						m_nMediaBitrate = newtag->GetInt();
					break;
				case FT_MEDIA_CODEC:
					if (newtag->IsStr())
						m_strMediaCodec = newtag->GetStr();
					break;
			}
			delete newtag;
		}
		metFile.Close();
	}
	catch(CException* error){
		error->Delete();
		metFile.Abort();
		return false;
	}
	return true;
}
