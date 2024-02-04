#pragma once

#include "NumberEdit.h"


// CInletOutletPropPage dialog

class CInletOutletPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CInletOutletPropPage)

public:
	CInletOutletPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_INLETOUTLET };
#endif

private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	void ApplyValues();
	BOOL OnApply() override;
	BOOL OnInitDialog() override;

	afx_msg void OnBnClickedRadio();
	afx_msg void OnEnChangeEdit();

	int inletOption; // 0 - use inlet density, 1 - use inlet speed
	int outletOption;

	double inletDensity;
	double inletSpeed;

	double outletDensity;
	double outletSpeed;

	CNumberEdit inletDensityEdit;
	CNumberEdit inletSpeedEdit;
	CNumberEdit outletDensityEdit;
	CNumberEdit outletSpeedEdit;

	DECLARE_MESSAGE_MAP()
};
