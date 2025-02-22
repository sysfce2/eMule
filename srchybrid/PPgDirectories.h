#pragma once
#include "DirectoryTreeCtrl.h"

class CPPgDirectories : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgDirectories)

	enum
	{
		IDD = IDD_PPG_DIRECTORIES
	};
	void ErrorBalloon(int iEdit, UINT uid);
public:
	CPPgDirectories();									// standard constructor

	void Localize();

protected:
	CDirectoryTreeCtrl m_ShareSelector;
	HICON m_icoBrowse;

	void LoadSettings();

	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSettingsChange()			{ SetModified(); }
	afx_msg void OnBnClickedSelincdir();
	afx_msg void OnBnClickedSeltempdir();
	afx_msg void OnBnClickedAddUNC();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO*);
	afx_msg void OnBnClickedSeltempdiradd();
	afx_msg void OnDestroy();
};