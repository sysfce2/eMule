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
#include "stdafx.h"
#include "PartMetFile.h"
#include "PartFile.h"
#include "PartMetPropertyStorage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CLSID CLSID_PartMetFile = {0x5F081689,0xCE7D,0x43E7,{0x8B,0x11,0xDA,0xD9,0x9A,0x4A,0x96,0xD6}};

#ifdef _DEBUG
const GUID FMTID_Unk1 = {0x6D748DE2,0x8D38,0x4CC3,{0xAC,0x60,0xF0,0x09,0xB0,0x57,0xC5,0x57}};
#endif

CPartMetFile::CPartMetFile()
{
	TRACE_OBJ("%s\n", __FUNCTION__);
	m_pPartFile = NULL;
}

CPartMetFile::~CPartMetFile()
{
	TRACE_OBJ("%s\n", __FUNCTION__);
	delete m_pPartFile;
}

STDMETHODIMP CPartMetFile::GetClassID(CLSID *pClassID)
{
	TRACE_INTFN("%s\n", __FUNCTION__);
	*pClassID = CLSID_PartMetFile;
	return S_OK;
}

STDMETHODIMP CPartMetFile::IsDirty(void)
{
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetFile::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
	UNREFERENCED_PARAMETER(dwMode);
	TRACE_INTFN("%s: dwMode=%08x, FileName='%ls'\n", __FUNCTION__, dwMode, pszFileName);

	m_strFileName.Empty();
	delete m_pPartFile;
	m_pPartFile = NULL;

	HRESULT hr = E_FAIL;
	try {
		WCHAR wszExt[_MAX_EXT];
		_wsplitpath(pszFileName, NULL, NULL, NULL, wszExt);
		CString strFileName(pszFileName);
		if (wcsicmp(wszExt, L".part") == 0)
			strFileName += L".met";

		m_pPartFile = new CPartFile;
		if (m_pPartFile->LoadTags(strFileName)) {
			m_strFileName = strFileName;
			hr = S_OK;
		}
	}
	catch(CMemoryException* exc) {
		exc->Delete();
		hr = E_OUTOFMEMORY;
	}
	catch(CException* exc) {
		exc->Delete();
	}

	if (hr != S_OK) {
		m_strFileName.Empty();
		delete m_pPartFile;
		m_pPartFile = NULL;
	}

	return hr;
}

STDMETHODIMP CPartMetFile::Save(LPCOLESTR pszFileName, BOOL fRemember)
{
	UNREFERENCED_PARAMETER(pszFileName);
	UNREFERENCED_PARAMETER(fRemember);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetFile::SaveCompleted(LPCOLESTR pszFileName)
{
	UNREFERENCED_PARAMETER(pszFileName);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetFile::GetCurFile(LPOLESTR *ppszFileName)
{
	UNREFERENCED_PARAMETER(ppszFileName);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetFile::GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip)
{
	TRACE_INTFN("%s: dwFlags=%08x\n", __FUNCTION__, dwFlags);
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;

	HRESULT hrResult = E_OUTOFMEMORY;
	try {
		CString strTip;
		if (dwFlags & QITIPF_USENAME)
			strTip = m_pPartFile->m_strFileName;
		else
			strTip.AppendFormat(L"Title: %s", m_pPartFile->m_strFileName);

		LPMALLOC pIMalloc = 0;
		if (SHGetMalloc(&pIMalloc) == S_OK && pIMalloc != 0)
		{
			LPWSTR pwszTip = (LPWSTR)pIMalloc->Alloc(sizeof(WCHAR)*(strTip.GetLength() + 1));
			if (pwszTip != NULL)
			{
				wcscpy(pwszTip, strTip);
				*ppwszTip = pwszTip;
				hrResult = S_OK;
			}
			pIMalloc->Release();
		}
	}
	catch(CException* exc) {
		exc->Delete();
	}

	return hrResult;
}

STDMETHODIMP CPartMetFile::GetInfoFlags(DWORD *pdwFlags)
{
	UNREFERENCED_PARAMETER(pdwFlags);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
// IPropertySetStorage

STDMETHODIMP CPartMetFile::Create(REFFMTID rfmtid, const CLSID *pclsid, DWORD grfFlags, DWORD grfMode, IPropertyStorage **ppprstg)
{
	UNREFERENCED_PARAMETER(rfmtid);
	UNREFERENCED_PARAMETER(pclsid);
	UNREFERENCED_PARAMETER(grfFlags);
	UNREFERENCED_PARAMETER(grfMode);
	UNREFERENCED_PARAMETER(ppprstg);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

#ifdef _DEBUG
void DebugFMTID(const FMTID& rfmtid)
{
	if (rfmtid == FMTID_SummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_SummaryInformation\n");
	else if (rfmtid == FMTID_DocSummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_DocSummaryInformation\n");
	else if (rfmtid == FMTID_UserDefinedProperties)
		TRACE_INTFN("rfmtid=FMTID_UserDefinedProperties\n");
	else if (rfmtid == FMTID_DiscardableInformation)
		TRACE_INTFN("rfmtid=FMTID_DiscardableInformation\n");
	else if (rfmtid == FMTID_ImageSummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_ImageSummaryInformation\n");
	else if (rfmtid == FMTID_ImageProperties)
		TRACE_INTFN("rfmtid=FMTID_ImageProperties\n");
	else if (rfmtid == FMTID_AudioSummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_AudioSummaryInformation\n");
	else if (rfmtid == FMTID_VideoSummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_VideoSummaryInformation\n");
	else if (rfmtid == FMTID_MediaFileSummaryInformation)
		TRACE_INTFN("rfmtid=FMTID_MediaFileSummaryInformation\n");
	else if (rfmtid == FMTID_MUSIC)
		TRACE_INTFN("rfmtid=FMTID_MUSIC\n");
	else if (rfmtid == FMTID_DRM)
		TRACE_INTFN("rfmtid=FMTID_DRM\n");
	else if (rfmtid == FMTID_Unk1)
		TRACE_INTFN("rfmtid=***FMTID_Unk1\n");
	else
		TRACE_INTFN("rfmtid=*Unknown*\n");
}
#endif

STDMETHODIMP CPartMetFile::Open(REFFMTID rfmtid, DWORD grfMode, IPropertyStorage **ppprstg)
{
	TRACE_INTFN("\n%s: grfMode=%08x, ", __FUNCTION__, grfMode);
	DEBUG_ONLY( DebugFMTID(rfmtid) );
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;

	if ((grfMode & (STGM_WRITE | STGM_READWRITE)) != 0)
		return STG_E_INVALIDFUNCTION;

	if (rfmtid != FMTID_SummaryInformation &&
		rfmtid != FMTID_AudioSummaryInformation && 
		rfmtid != FMTID_VideoSummaryInformation &&
		rfmtid != FMTID_MUSIC) {
		//return STG_E_FILENOTFOUND;
		return E_FAIL; // Windows built-in handlers do return E_FAIL here!
	}

	CComObject<CPartMetPropertyStorage>* pPartMetPropertyStorage;
	if (FAILED(CComObject<CPartMetPropertyStorage>::CreateInstance(&pPartMetPropertyStorage)))
		return STG_E_INSUFFICIENTMEMORY;
	pPartMetPropertyStorage->m_fmtid = rfmtid;
	pPartMetPropertyStorage->m_pPartFile = m_pPartFile;
	return pPartMetPropertyStorage->QueryInterface(ppprstg);
}

STDMETHODIMP CPartMetFile::Delete(REFFMTID rfmtid)
{
	UNREFERENCED_PARAMETER(rfmtid);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

class ATL_NO_VTABLE CPartMetStatPropSetStg : 
	public CComObjectRoot,
	public IUnknown
{
public:
	CPartMetStatPropSetStg() {
		m_iProps = 0;
		m_props = NULL;
	}
	~CPartMetStatPropSetStg() {
		free(m_props);
	}

	BEGIN_COM_MAP(CPartMetStatPropSetStg)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	int m_iProps;
	STATPROPSETSTG* m_props;
};

STDMETHODIMP CPartMetFile::Enum(IEnumSTATPROPSETSTG **ppenum)
{
	TRACE_INTFN("%s\n", __FUNCTION__);
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;

	CComObject<CPartMetStatPropSetStg>* pPartMetStatPropSetStg;
	if (FAILED(CComObject<CPartMetStatPropSetStg>::CreateInstance(&pPartMetStatPropSetStg)))
		return STG_E_INSUFFICIENTMEMORY;

	CComPtr<IUnknown> pIUnkStatPropSetStg;
	VERIFY( SUCCEEDED(pPartMetStatPropSetStg->QueryInterface(&pIUnkStatPropSetStg)) );

	static const FMTID s_aFMTID[] = 
	{
		FMTID_SummaryInformation,
		FMTID_AudioSummaryInformation,
		FMTID_VideoSummaryInformation,
		FMTID_MUSIC
		//,
		//FMTID_DocSummaryInformation,
		//FMTID_UserDefinedProperties,	// NOT to enumerate
		//FMTID_DiscardableInformation,
		//FMTID_ImageSummaryInformation,
		//FMTID_ImageProperties,
		//FMTID_MediaFileSummaryInformation,
		//FMTID_DRM,
		//FMTID_Unk1
	};

	pPartMetStatPropSetStg->m_props = (STATPROPSETSTG*)malloc(sizeof(*pPartMetStatPropSetStg->m_props) * _countof(s_aFMTID));
	if (pPartMetStatPropSetStg->m_props == NULL)
		return STG_E_INSUFFICIENTMEMORY;
	for (int i = 0; i < _countof(s_aFMTID); i++)
	{
		// Windows built-in handlers set all fields (except 'fmtid') to 0.
		memset(&pPartMetStatPropSetStg->m_props[pPartMetStatPropSetStg->m_iProps], 0, sizeof(*pPartMetStatPropSetStg->m_props));
		pPartMetStatPropSetStg->m_props[pPartMetStatPropSetStg->m_iProps].fmtid = s_aFMTID[i];
		pPartMetStatPropSetStg->m_props[pPartMetStatPropSetStg->m_iProps].clsid = CLSID_NULL;
		pPartMetStatPropSetStg->m_props[pPartMetStatPropSetStg->m_iProps].grfFlags = PROPSETFLAG_DEFAULT;
		pPartMetStatPropSetStg->m_iProps++;
	}

	typedef CComEnum<IEnumSTATPROPSETSTG, &__uuidof(IEnumSTATPROPSETSTG), STATPROPSETSTG, _Copy<STATPROPSETSTG> > CMyEnum;
	CComObject<CMyEnum>* pMyEnum;
	if (FAILED(CComObject<CMyEnum>::CreateInstance(&pMyEnum)))
		return STG_E_INSUFFICIENTMEMORY;
	VERIFY( SUCCEEDED(pMyEnum->Init(pPartMetStatPropSetStg->m_props, pPartMetStatPropSetStg->m_props + pPartMetStatPropSetStg->m_iProps, pIUnkStatPropSetStg)) );
	return pMyEnum->QueryInterface(ppenum);
}
