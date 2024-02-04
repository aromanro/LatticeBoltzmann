// InletOutletPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "LatticeBoltzmann.h"
#include "InletOutletPropPage.h"
#include "afxdialogex.h"

#include "Options.h"
#include "LatticeBoltzmann.h"




// CInletOutletPropPage dialog

IMPLEMENT_DYNAMIC(CInletOutletPropPage, CMFCPropertyPage)

CInletOutletPropPage::CInletOutletPropPage()
	: CMFCPropertyPage(IDD_PROPPAGE_INLETOUTLET)
{
	inletOption = theApp.GetOptions().inletOption;
	outletOption = theApp.GetOptions().outletOption;
	inletDensity = theApp.GetOptions().inletDensity;
	outletDensity = theApp.GetOptions().outletDensity;
	inletSpeed = theApp.GetOptions().inletSpeed;
	outletSpeed = theApp.GetOptions().outletSpeed;
}

void CInletOutletPropPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO4, inletOption);
	DDX_Radio(pDX, IDC_RADIO6, outletOption);

	DDX_Text(pDX, IDC_EDIT1, inletDensity);
	DDX_Text(pDX, IDC_EDIT3, outletDensity);
	DDX_Text(pDX, IDC_EDIT2, inletSpeed);
	DDX_Text(pDX, IDC_EDIT4, outletSpeed);

	DDX_Control(pDX, IDC_EDIT1, inletDensityEdit);
	DDX_Control(pDX, IDC_EDIT3, outletDensityEdit);
	DDX_Control(pDX, IDC_EDIT2, inletSpeedEdit);
	DDX_Control(pDX, IDC_EDIT4, outletSpeedEdit);
}


BEGIN_MESSAGE_MAP(CInletOutletPropPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_RADIO4, &CInletOutletPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO5, &CInletOutletPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO6, &CInletOutletPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO7, &CInletOutletPropPage::OnBnClickedRadio)
	ON_EN_CHANGE(IDC_EDIT1, &CInletOutletPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, &CInletOutletPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT3, &CInletOutletPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT4, &CInletOutletPropPage::OnEnChangeEdit)
END_MESSAGE_MAP()


// CInletOutletPropPage message handlers


void CInletOutletPropPage::ApplyValues()
{
	theApp.GetOptions().inletOption = inletOption;
	theApp.GetOptions().outletOption = outletOption;
	theApp.GetOptions().inletDensity = inletDensity;
	theApp.GetOptions().outletDensity = outletDensity;

	// a safety check, this won't prevent entering unreasonable values
	if ((inletSpeed > 0 && outletSpeed < 0) || (inletSpeed < 0 && outletSpeed > 0))
		outletSpeed *= -1;

	theApp.GetOptions().inletSpeed = inletSpeed;
	theApp.GetOptions().outletSpeed = outletSpeed;

	theApp.GetOptions().Save();
}


BOOL CInletOutletPropPage::OnApply()
{
	UpdateData();
	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


BOOL CInletOutletPropPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	inletDensityEdit.allowNegative = false;
	inletSpeedEdit.allowNegative = true;
	outletDensityEdit.allowNegative = false;
	outletSpeedEdit.allowNegative = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CInletOutletPropPage::OnBnClickedRadio()
{
	SetModified();
}


void CInletOutletPropPage::OnEnChangeEdit()
{
	SetModified();
}


