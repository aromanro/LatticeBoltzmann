#pragma once


// COptionsPropertySheet

class COptionsPropertySheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(COptionsPropertySheet)

public:
	COptionsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsPropertySheet();

protected:
	HICON hIcon;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


