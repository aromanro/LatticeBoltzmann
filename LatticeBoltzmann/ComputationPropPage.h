#pragma once


// CComputationPropPage dialog

class CComputationPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CComputationPropPage)

public:
	CComputationPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_COMPUTATION };
#endif

private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnApply() override;
	void ApplyValues() const;
	afx_msg void OnEnChangeEdit();
	int numThreads;
	int numSteps;
};
