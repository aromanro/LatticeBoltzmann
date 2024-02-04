
// LatticeBoltzmann.h : main header file for the LatticeBoltzmann application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include "Options.h"

// CLatticeBoltzmannApp:
// See LatticeBoltzmann.cpp for the implementation of this class
//

class CLatticeBoltzmannApp : public CWinAppEx
{
public:
	CLatticeBoltzmannApp();

	Options& GetOptions() { return options; }
	UINT& GetAppLook() { return m_nAppLook; }
	BOOL GetHiColorIcons() const { return m_bHiColorIcons; }

// Overrides
private:
	BOOL InitInstance() override;

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	Options options;

	void PreLoadState() override;
	void LoadCustomState() override;
	void SaveCustomState() override;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLatticeBoltzmannApp theApp;
