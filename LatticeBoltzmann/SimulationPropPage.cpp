// SimulationPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "LatticeBoltzmann.h"
#include "SimulationPropPage.h"
#include "afxdialogex.h"


#include "Options.h"
#include "LatticeBoltzmann.h"

// CSimulationPropPage dialog

IMPLEMENT_DYNAMIC(CSimulationPropPage, CMFCPropertyPage)

CSimulationPropPage::CSimulationPropPage()
	: CMFCPropertyPage(IDD_PROPPAGE_SIMULATION)
{
	boundaryConditions = theApp.options.boundaryConditions;
	tau = theApp.options.tau;
	accelX = theApp.options.accelX;
	useAccelX = theApp.options.useAccelX;
}

CSimulationPropPage::~CSimulationPropPage()
{
}

void CSimulationPropPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, tauEdit);
	DDX_Control(pDX, IDC_EDIT8, accelXEdit);

	DDX_Radio(pDX, IDC_RADIO1, boundaryConditions);


	DDX_Text(pDX, IDC_EDIT1, tau);
	DDX_Text(pDX, IDC_EDIT8, accelX);

	DDX_Check(pDX, IDC_CHECK1, useAccelX);
}


BEGIN_MESSAGE_MAP(CSimulationPropPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_RADIO1, &CSimulationPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, &CSimulationPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO3, &CSimulationPropPage::OnBnClickedRadio)
	ON_EN_CHANGE(IDC_EDIT1, &CSimulationPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT8, &CSimulationPropPage::OnEnChangeEdit)
	ON_BN_CLICKED(IDC_CHECK1, &CSimulationPropPage::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CSimulationPropPage message handlers


BOOL CSimulationPropPage::OnApply()
{
	UpdateData();
	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


BOOL CSimulationPropPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	tauEdit.allowNegative = false;
	accelXEdit.allowNegative = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSimulationPropPage::ApplyValues()
{
	theApp.options.boundaryConditions = boundaryConditions;
	theApp.options.tau = tau;
	theApp.options.accelX = accelX;
	theApp.options.useAccelX = useAccelX;

	theApp.options.Save();
}


void CSimulationPropPage::OnBnClickedRadio()
{
	SetModified();
}


void CSimulationPropPage::OnEnChangeEdit()
{
	SetModified();
}




void CSimulationPropPage::OnBnClickedCheck1()
{
	SetModified();
}
