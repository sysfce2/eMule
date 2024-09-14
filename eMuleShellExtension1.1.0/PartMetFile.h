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
#include "resource.h"       // main symbols

class CPartFile;
extern const CLSID CLSID_PartMetFile;

class ATL_NO_VTABLE CPartMetFile : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CPartMetFile, &CLSID_PartMetFile>,
	public IPersistFile,
	public IQueryInfo,
	public IPropertySetStorage
{
public:
	CPartMetFile();
	~CPartMetFile();

	DECLARE_REGISTRY_RESOURCEID(IDR_PARTMETFILE)

	BEGIN_COM_MAP(CPartMetFile)
		COM_INTERFACE_ENTRY(IPersistFile)
		COM_INTERFACE_ENTRY_IID(IID_IQueryInfo, IQueryInfo)
		COM_INTERFACE_ENTRY(IPropertySetStorage)
	END_COM_MAP()

	// IPersist interface
    STDMETHOD(GetClassID)(CLSID *pClassID);

	// IPersistFile interface
    STDMETHOD(IsDirty)(void);
    STDMETHOD(Load)(LPCOLESTR pszFileName, DWORD dwMode);
	STDMETHOD(Save)(LPCOLESTR pszFileName, BOOL fRemember);
	STDMETHOD(SaveCompleted)(LPCOLESTR pszFileName);
	STDMETHOD(GetCurFile)(LPOLESTR *ppszFileName);

	// IQueryInfo
    STDMETHOD(GetInfoTip)(DWORD dwFlags, WCHAR **ppwszTip);
    STDMETHOD(GetInfoFlags)(DWORD *pdwFlags);

	// IPropertySetStorage
    STDMETHOD(Create)(REFFMTID rfmtid, const CLSID *pclsid, DWORD grfFlags, DWORD grfMode, IPropertyStorage **ppprstg);
    STDMETHOD(Open)(REFFMTID rfmtid, DWORD grfMode, IPropertyStorage **ppprstg);
    STDMETHOD(Delete)(REFFMTID rfmtid);
    STDMETHOD(Enum)(IEnumSTATPROPSETSTG **ppenum);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() {
		return S_OK;
	}

	void FinalRelease() {
	}

private:
	CString m_strFileName;
	CPartFile* m_pPartFile;
};

OBJECT_ENTRY_AUTO(CLSID_PartMetFile, CPartMetFile)
