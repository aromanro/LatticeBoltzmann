#pragma once

#include "NumberEdit.h"

// CDisplayPropPage dialog

class CDisplayPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CDisplayPropPage)

public:
	CDisplayPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_DISPLAY };
#endif

private:
	BOOL OnApply() override;
	BOOL OnInitDialog() override;
	void ApplyValues();
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	afx_msg void OnBnClickedRadio();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedMfccolorbutton1();
	afx_msg void OnEnChangeEdit();

	double minSpeed;
	double maxSpeed;

	double minDensity;
	double maxDensity;

	double minVorticity;
	double maxVorticity;

	CNumberEdit minSpeedEdit;
	CNumberEdit maxSpeedEdit;

	CNumberEdit minDensityEdit;
	CNumberEdit maxDensityEdit;

	CNumberEdit minVorticityEdit;
	CNumberEdit maxVorticityEdit;

	CMFCColorButton obstaclesColorButton;

	int chartColors;
	int resultsType;

	int zoom;

	DECLARE_MESSAGE_MAP()
};
