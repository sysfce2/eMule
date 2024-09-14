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

class CPartFile;

class ATL_NO_VTABLE CPartMetPropertyStorage :
	public CComObjectRoot,
	public IPropertyStorage
{
public:
	CPartMetPropertyStorage();
	~CPartMetPropertyStorage();

	BEGIN_COM_MAP(CPartMetPropertyStorage)
		COM_INTERFACE_ENTRY(IPropertyStorage)
	END_COM_MAP()

	// IPropertyStorage
    STDMETHOD(ReadMultiple)(ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[]);
    STDMETHOD(WriteMultiple)(ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], PROPID propidNameFirst);
    STDMETHOD(DeleteMultiple)(ULONG cpspec, const PROPSPEC rgpspec[]);
    STDMETHOD(ReadPropertyNames)(ULONG cpropid, const PROPID rgpropid[], LPOLESTR rglpwstrName[]);
    STDMETHOD(WritePropertyNames)(ULONG cpropid, const PROPID rgpropid[], const LPOLESTR rglpwstrName[]);
    STDMETHOD(DeletePropertyNames)(ULONG cpropid, const PROPID rgpropid[]);
    STDMETHOD(Commit)(DWORD grfCommitFlags);
    STDMETHOD(Revert)(void);
    STDMETHOD(Enum)(IEnumSTATPROPSTG **ppenum);
    STDMETHOD(SetTimes)(const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime);
    STDMETHOD(SetClass)(REFCLSID clsid);
    STDMETHOD(Stat)(STATPROPSETSTG *pstatpsstg);

	FMTID m_fmtid;
	CPartFile* m_pPartFile;
};
