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
#pragma once

class CFileDataIO;

class CPartFile
{
public:
	CPartFile();

	bool LoadTags(LPCTSTR pszFilePath);

	CString m_strFileName;
	ULONGLONG m_ullFileSize;
	CString m_strFileType;
	bool m_bStopped;
	CString m_strMediaArtist;
	CString m_strMediaAlbum;
	CString m_strMediaTitle;
	UINT m_nMediaLength;
	UINT m_nMediaBitrate;
	CString m_strMediaCodec;
	FILETIME m_ftCreate;
	FILETIME m_ftAccess;
	FILETIME m_ftLastWrite;
};