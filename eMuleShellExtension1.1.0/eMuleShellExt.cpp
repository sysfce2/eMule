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
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CeMuleShellExtModule

class CeMuleShellExtModule
	: public CAtlDllModuleT<CeMuleShellExtModule>
{
public:
};
CeMuleShellExtModule _AtlModule;


//////////////////////////////////////////////////////////////////////////////
// CeMuleShellExtApp

class CeMuleShellExtApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};
CeMuleShellExtApp theApp;

BEGIN_MESSAGE_MAP(CeMuleShellExtApp, CWinApp)
END_MESSAGE_MAP()

BOOL CeMuleShellExtApp::InitInstance()
{
    return CWinApp::InitInstance();
}

int CeMuleShellExtApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow() == S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer(FALSE/*bRegTypeLib*/);

	// This call notifies Windows that new file association information is available. If there are 
	// any images in your image format already present on the computer, the thumbnail cache will
	// contain default thumbnails for them because there was no decoder available that could
	// extract the thumbnails when the images were first acquired. When you notify Windows that a
	// new file association is available, the thumbnail cache will discard any empty thumbnails and
	// extract the actual thumbnails from the files that can now be decoded.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer(FALSE/*bUnRegTypeLib*/);

	// This call notifies Windows that new file association information is available. If there are 
	// any images in your image format already present on the computer, the thumbnail cache will
	// contain default thumbnails for them because there was no decoder available that could
	// extract the thumbnails when the images were first acquired. When you notify Windows that a
	// new file association is available, the thumbnail cache will discard any empty thumbnails and
	// extract the actual thumbnails from the files that can now be decoded.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return hr;
}
