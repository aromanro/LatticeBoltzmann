#pragma once

#include "NumberEdit.h"

// CSimulationPropPage dialog

class CSimulationPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CSimulationPropPage)

public:
	CSimulationPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_SIMULATION };
#endif

private:
	int boundaryConditions; // 0 - periodic, 1 - bounce back, 2 - slippery
	double tau;
	double accelX; // push force on the left side
	int useAccelX;

	CNumberEdit tauEdit;
	CNumberEdit accelXEdit;

	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnApply() override;
	BOOL OnInitDialog() override;
	void ApplyValues();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedCheck1();
};
