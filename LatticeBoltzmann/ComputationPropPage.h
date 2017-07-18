#pragma once


// CComputationPropPage dialog

class CComputationPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CComputationPropPage)

public:
	CComputationPropPage();
	virtual ~CComputationPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_COMPUTATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	void ApplyValues();
	afx_msg void OnEnChangeEdit();
	int numThreads;
	int numSteps;
};
