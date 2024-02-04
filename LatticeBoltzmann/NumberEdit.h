#pragma once


// CNumberEdit

class CNumberEdit : public CEdit
{
	DECLARE_DYNAMIC(CNumberEdit)
	DECLARE_MESSAGE_MAP()

private:
	CString m_GoodValue;
	DWORD m_GoodSel = 0;
	bool m_Cancelling = false;

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnUpdate();
	afx_msg	LRESULT OnPaste(WPARAM wParam, LPARAM lParam);

public:
	bool allowNegative = true;
};


