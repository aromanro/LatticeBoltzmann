#pragma once

#include "NumberEdit.h"

// CSimulationPropPage dialog

class CSimulationPropPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CSimulationPropPage)

public:
	CSimulationPropPage();
	virtual ~CSimulationPropPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_SIMULATION };
#endif

protected:
	int boundaryConditions; // 0 - periodic, 1 - bounce back, 2 - slippery
	double tau;
	double accelX; // push force on the left side
	int useAccelX;

	CNumberEdit tauEdit;
	CNumberEdit accelXEdit;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	void ApplyValues();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedCheck1();
};
