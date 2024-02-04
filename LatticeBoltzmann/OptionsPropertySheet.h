#pragma once


// COptionsPropertySheet

class COptionsPropertySheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(COptionsPropertySheet)

public:
	COptionsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	COptionsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	~COptionsPropertySheet() override;

private:
	HICON hIcon = 0;

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog() override;
};


