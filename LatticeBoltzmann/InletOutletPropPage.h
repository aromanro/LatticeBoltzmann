#pragma once

#include "NumberEdit.h"


// CInletOutletPropPage dialog

class CInletOutletPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CInletOutletPropPage)

public:
	CInletOutletPropPage();
	virtual ~CInletOutletPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_INLETOUTLET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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
public:
	void ApplyValues();
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnEnChangeEdit();
};
