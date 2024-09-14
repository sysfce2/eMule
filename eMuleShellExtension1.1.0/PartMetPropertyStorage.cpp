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
#include "StdAfx.h"
#include "propidl.h"
#include "PartMetPropertyStorage.h"
#include "PartFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
void DebugFMTID(const FMTID& rftmid);
#endif

 // FMTID_ImageSummaryInformation
#define PIDISI_FILETYPE      2  // VT_LPWSTR
#define PIDISI_CX            3  // VT_UI4
#define PIDISI_CY            4  // VT_UI4
#define PIDISI_RESOLUTIONX   5  // VT_UI4
#define PIDISI_RESOLUTIONY   6  // VT_UI4
#define PIDISI_BITDEPTH      7  // VT_UI4
#define PIDISI_COLORSPACE    8  // VT_LPWSTR
#define PIDISI_COMPRESSION   9  // VT_LPWSTR
#define PIDISI_TRANSPARENCY	10  // VT_UI4
#define PIDISI_GAMMAVALUE	11  // VT_UI4
#define PIDISI_FRAMECOUNT   12  // VT_UI4
#define PIDISI_DIMENSIONS   13  // VT_LPWSTR


#ifndef PIDVSI_TOTAL_BITRATE
#define	PIDVSI_TOTAL_BITRATE	43	// VT_UI4
#endif

CPartMetPropertyStorage::CPartMetPropertyStorage(void)
{
	TRACE_OBJ("%s\n", __FUNCTION__);
	m_pPartFile = 0;
}

CPartMetPropertyStorage::~CPartMetPropertyStorage(void)
{
	TRACE_OBJ("%s\n", __FUNCTION__);
}

//////////////////////////////////////////////////////////////////////////////
// IPropertyStorage

STDMETHODIMP CPartMetPropertyStorage::ReadMultiple(ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[])
{
	TRACE_INTFN("%s: cpspec=%u\n", __FUNCTION__, cpspec);
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;

	int iPropsFound = 0;
	for (ULONG i = 0; i < cpspec; i++)
	{
		rgpropvar[i].vt = VT_EMPTY;
		if (rgpspec[i].ulKind == PRSPEC_PROPID)
		{
			// First, check for codepage, this is same for all FMTIDs
			if (rgpspec[i].propid == PID_CODEPAGE) {
				TRACE_INTFN("  PID_CODEPAGE\n");
				rgpropvar[i].vt = VT_I2;
				rgpropvar[i].iVal = 1200;
				iPropsFound++;
			}
			else if (m_fmtid == FMTID_SummaryInformation)
			{
				if (rgpspec[i].propid == PIDSI_TITLE) {
					TRACE_INTFN("  PIDSI_TITLE\n");
					// prop:DocTitle
					// prop:System.Title
					if (!m_pPartFile->m_strFileName.IsEmpty()) {
						rgpropvar[i].vt = VT_LPWSTR;
						rgpropvar[i].bstrVal = AtlAllocTaskWideString(m_pPartFile->m_strFileName);
						iPropsFound++;
					}
				}
				else if (rgpspec[i].propid == PIDSI_CREATE_DTM) {
					TRACE_INTFN("  PIDSI_CREATE_DTM\n");
					if (m_pPartFile->m_ftCreate.dwLowDateTime || m_pPartFile->m_ftCreate.dwHighDateTime) {
						rgpropvar[i].vt = VT_FILETIME;
						rgpropvar[i].filetime = m_pPartFile->m_ftCreate;
						iPropsFound++;
					}
				}
			#ifdef _DEBUG
				else if (rgpspec[i].propid == PIDSI_AUTHOR)		TRACE_INTFN("  PIDSI_AUTHOR\n");
				else if (rgpspec[i].propid == PIDSI_SUBJECT)	TRACE_INTFN("  PIDSI_SUBJECT\n");
				else if (rgpspec[i].propid == PIDSI_PAGECOUNT)	TRACE_INTFN("  PIDSI_PAGECOUNT\n");
				else if (rgpspec[i].propid == PIDSI_COMMENTS)	TRACE_INTFN("  PIDSI_COMMENTS\n");
				else TRACE_INTFN("  %u (FMTID_SummaryInformation)\n", rgpspec[i].propid);
			#endif
			}
			else if (m_fmtid == FMTID_AudioSummaryInformation)
			{
				if (rgpspec[i].propid == PIDASI_TIMELENGTH) {
					TRACE_INTFN("  PIDASI_TIMELENGTH\n");
					// prop:Duration
					if (m_pPartFile->m_nMediaLength) {
						rgpropvar[i].vt = VT_UI8;
						rgpropvar[i].uhVal.QuadPart = m_pPartFile->m_nMediaLength * 10000000ull;
						iPropsFound++;
					}
				}
				else if (rgpspec[i].propid == PIDASI_AVG_DATA_RATE) {
					TRACE_INTFN("  PIDASI_AVG_DATA_RATE\n");
					// prop:Bitrate
					if (m_pPartFile->m_nMediaBitrate) {
						rgpropvar[i].vt = VT_UI4;
						rgpropvar[i].ulVal = m_pPartFile->m_nMediaBitrate * 1000;
						iPropsFound++;
					}
				}
			#ifdef _DEBUG
				else if (rgpspec[i].propid == PIDASI_SAMPLE_SIZE)	TRACE_INTFN("  PIDASI_SAMPLE_SIZE\n");
				else if (rgpspec[i].propid == PIDASI_SAMPLE_RATE)	TRACE_INTFN("  PIDASI_SAMPLE_RATE\n");
				else if (rgpspec[i].propid == PIDASI_CHANNEL_COUNT)	TRACE_INTFN("  PIDASI_CHANNEL_COUNT\n");
				else TRACE_INTFN("  %u (FMTID_AudioSummaryInformation)\n", rgpspec[i].propid);
			#endif
			}
			else if (m_fmtid == FMTID_VideoSummaryInformation)
			{
				if (rgpspec[i].propid == PIDVSI_TIMELENGTH) {
					TRACE_INTFN("  PIDVSI_TIMELENGTH\n");
					if (m_pPartFile->m_nMediaLength) {
						rgpropvar[i].vt = VT_UI8;
						rgpropvar[i].uhVal.QuadPart = m_pPartFile->m_nMediaLength * 10000000ull;
						iPropsFound++;
					}
				}
				else if (rgpspec[i].propid == PIDVSI_COMPRESSION) {
					TRACE_INTFN("  PIDVSI_COMPRESSION\n");
					if (!m_pPartFile->m_strMediaCodec.IsEmpty()) {
						rgpropvar[i].vt = VT_LPWSTR;
						rgpropvar[i].bstrVal = AtlAllocTaskWideString(m_pPartFile->m_strMediaCodec);
						iPropsFound++;
					}
				}
			#ifdef _DEBUG
				else TRACE_INTFN("  %u (FMTID_VideoSummaryInformation)\n", rgpspec[i].propid);
			#endif
			}
			else if (m_fmtid == FMTID_MUSIC)
			{
				if (rgpspec[i].propid == PIDSI_ARTIST) {
					TRACE_INTFN("  PIDSI_ARTIST\n");
					// prop:Artist
					if (!m_pPartFile->m_strMediaArtist.IsEmpty()) {
						rgpropvar[i].vt = VT_LPWSTR;
						rgpropvar[i].bstrVal = AtlAllocTaskWideString(m_pPartFile->m_strMediaArtist);
						iPropsFound++;
					}
				}
				else if (rgpspec[i].propid == PIDSI_SONGTITLE) {
					TRACE_INTFN("  PIDSI_SONGTITLE\n");
					if (!m_pPartFile->m_strMediaTitle.IsEmpty()) {
						rgpropvar[i].vt = VT_LPWSTR;
						rgpropvar[i].bstrVal = AtlAllocTaskWideString(m_pPartFile->m_strMediaTitle);
						iPropsFound++;
					}
				}
				else if (rgpspec[i].propid == PIDSI_ALBUM) {
					TRACE_INTFN("  PIDSI_ALBUM\n");
					// prop:Album
					if (!m_pPartFile->m_strMediaAlbum.IsEmpty()) {
						rgpropvar[i].vt = VT_LPWSTR;
						rgpropvar[i].bstrVal = AtlAllocTaskWideString(m_pPartFile->m_strMediaAlbum);
						iPropsFound++;
					}
				}
			#ifdef _DEBUG
				else if (rgpspec[i].propid == PIDSI_YEAR)		TRACE_INTFN("  PIDSI_YEAR\n");
				else if (rgpspec[i].propid == PIDSI_GENRE)		TRACE_INTFN("  PIDSI_GENRE\n");
				else if (rgpspec[i].propid == PIDSI_TRACK)		TRACE_INTFN("  PIDSI_TRACK\n");
				else TRACE_INTFN("  %u (FMTID_MUSIC)\n", rgpspec[i].propid);
			#endif
			}
			else
			{
				TRACE("***Unknown fmtid\n");
				DEBUG_ONLY( DebugFMTID(m_fmtid) );
			}
		}
		else
		{
			TRACE("%s: ***STG_E_INVALIDPARAMETER\n", __FUNCTION__);
			return STG_E_INVALIDPARAMETER;
		}
	}
	TRACE("%s: hr = %u\n", __FUNCTION__, iPropsFound ? S_OK : S_FALSE);
	return iPropsFound ? S_OK : S_FALSE;
}

STDMETHODIMP CPartMetPropertyStorage::WriteMultiple(ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], PROPID propidNameFirst)
{
	UNREFERENCED_PARAMETER(cpspec);
	UNREFERENCED_PARAMETER(rgpspec);
	UNREFERENCED_PARAMETER(rgpropvar);
	UNREFERENCED_PARAMETER(propidNameFirst);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::DeleteMultiple(ULONG cpspec, const PROPSPEC rgpspec[])
{
	UNREFERENCED_PARAMETER(cpspec);
	UNREFERENCED_PARAMETER(rgpspec);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::ReadPropertyNames(ULONG cpropid, const PROPID rgpropid[], LPOLESTR rglpwstrName[])
{
	UNREFERENCED_PARAMETER(cpropid);
	UNREFERENCED_PARAMETER(rgpropid);
	UNREFERENCED_PARAMETER(rglpwstrName);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::WritePropertyNames(ULONG cpropid, const PROPID rgpropid[], const LPOLESTR rglpwstrName[])
{
	UNREFERENCED_PARAMETER(cpropid);
	UNREFERENCED_PARAMETER(rgpropid);
	UNREFERENCED_PARAMETER(rglpwstrName);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::DeletePropertyNames(ULONG cpropid, const PROPID rgpropid[])
{
	UNREFERENCED_PARAMETER(cpropid);
	UNREFERENCED_PARAMETER(rgpropid);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::Commit(DWORD grfCommitFlags)
{
	UNREFERENCED_PARAMETER(grfCommitFlags);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::Revert(void)
{
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

class ATL_NO_VTABLE CPartMetStatPropStg : 
	public CComObjectRoot,
	public IUnknown
{
public:
	CPartMetStatPropStg() {
		m_iProps = 0;
		m_props = NULL;
	}
	~CPartMetStatPropStg() {
		free(m_props);
	}

	BEGIN_COM_MAP(CPartMetStatPropStg)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	int m_iProps;
	STATPROPSTG* m_props;
};

STDMETHODIMP CPartMetPropertyStorage::Enum(IEnumSTATPROPSTG **ppenum)
{
	TRACE_INTFN("%s\n", __FUNCTION__);
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;

	CComObject<CPartMetStatPropStg>* pPartMetStatPropStg;
	if (FAILED(CComObject<CPartMetStatPropStg>::CreateInstance(&pPartMetStatPropStg)))
		return STG_E_INSUFFICIENTMEMORY;

	CComPtr<IUnknown> pIUnkStatPropStg;
	VERIFY( SUCCEEDED(pPartMetStatPropStg->QueryInterface(&pIUnkStatPropStg)) );

	if (m_fmtid == FMTID_SummaryInformation)
	{
		pPartMetStatPropStg->m_props = (STATPROPSTG*)malloc(sizeof(*pPartMetStatPropStg->m_props) * 2);
		if (pPartMetStatPropStg->m_props == NULL)
			return STG_E_INSUFFICIENTMEMORY;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDSI_TITLE;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_LPWSTR;
		pPartMetStatPropStg->m_iProps++;

		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDSI_CREATE_DTM;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_FILETIME;
		pPartMetStatPropStg->m_iProps++;
	}
	else if (m_fmtid == FMTID_AudioSummaryInformation)
	{
		pPartMetStatPropStg->m_props = (STATPROPSTG*)malloc(sizeof(*pPartMetStatPropStg->m_props) * 2);
		if (pPartMetStatPropStg->m_props == NULL)
			return STG_E_INSUFFICIENTMEMORY;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDASI_TIMELENGTH;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_UI8;
		pPartMetStatPropStg->m_iProps++;

		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDASI_AVG_DATA_RATE;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_UI4;
		pPartMetStatPropStg->m_iProps++;
	}
	else if (m_fmtid == FMTID_VideoSummaryInformation)
	{
		pPartMetStatPropStg->m_props = (STATPROPSTG*)malloc(sizeof(*pPartMetStatPropStg->m_props) * 2);
		if (pPartMetStatPropStg->m_props == NULL)
			return STG_E_INSUFFICIENTMEMORY;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDVSI_COMPRESSION;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_LPWSTR;
		pPartMetStatPropStg->m_iProps++;

		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDVSI_TIMELENGTH;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_UI8;
		pPartMetStatPropStg->m_iProps++;
	}
	else if (m_fmtid == FMTID_MUSIC)
	{
		pPartMetStatPropStg->m_props = (STATPROPSTG*)malloc(sizeof(*pPartMetStatPropStg->m_props) * 3);
		if (pPartMetStatPropStg->m_props == NULL)
			return STG_E_INSUFFICIENTMEMORY;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDSI_ARTIST;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_LPWSTR;
		pPartMetStatPropStg->m_iProps++;

		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDSI_SONGTITLE;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_LPWSTR;
		pPartMetStatPropStg->m_iProps++;

		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].lpwstrName = NULL;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].propid = PIDSI_ALBUM;
		pPartMetStatPropStg->m_props[pPartMetStatPropStg->m_iProps].vt = VT_LPWSTR;
		pPartMetStatPropStg->m_iProps++;
	}
	else
	{
		TRACE("***Unknown fmtid\n");
		DEBUG_ONLY( DebugFMTID(m_fmtid) );
		return E_UNEXPECTED;
	}

	typedef CComEnum<IEnumSTATPROPSTG, &__uuidof(IEnumSTATPROPSTG), STATPROPSTG, _Copy<STATPROPSTG> > CMyEnum;
	CComObject<CMyEnum>* pMyEnum;
	if (FAILED(CComObject<CMyEnum>::CreateInstance(&pMyEnum)))
		return STG_E_INSUFFICIENTMEMORY;
	VERIFY( SUCCEEDED(pMyEnum->Init(pPartMetStatPropStg->m_props, pPartMetStatPropStg->m_props + pPartMetStatPropStg->m_iProps, pIUnkStatPropStg)) );
	return pMyEnum->QueryInterface(ppenum);
}

STDMETHODIMP CPartMetPropertyStorage::SetTimes(const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime)
{
	UNREFERENCED_PARAMETER(pctime);
	UNREFERENCED_PARAMETER(patime);
	UNREFERENCED_PARAMETER(pmtime);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::SetClass(REFCLSID clsid)
{
	UNREFERENCED_PARAMETER(clsid);
	TRACE_INTFN("%s: ***E_NOTIMPL\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CPartMetPropertyStorage::Stat(STATPROPSETSTG *pstatpsstg)
{
	TRACE_INTFN("%s\n", __FUNCTION__);
	if (m_pPartFile == NULL)
		return E_UNEXPECTED;
	
	// Windows built-in handlers set all fields (except 'fmtid') to 0.
	memset(pstatpsstg, 0, sizeof(*pstatpsstg));
	pstatpsstg->fmtid = m_fmtid;
	pstatpsstg->clsid = CLSID_NULL;
	pstatpsstg->grfFlags = PROPSETFLAG_DEFAULT;
	return S_OK;
}
