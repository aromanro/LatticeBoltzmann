// DisplayPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "LatticeBoltzmann.h"
#include "DisplayPropPage.h"
#include "afxdialogex.h"

#include "Options.h"
#include "LatticeBoltzmann.h"

#include "MainFrm.h"
#include "LatticeBoltzmannDoc.h"


// CDisplayPropPage dialog

IMPLEMENT_DYNAMIC(CDisplayPropPage, CMFCPropertyPage)

CDisplayPropPage::CDisplayPropPage()
	: CMFCPropertyPage(IDD_PROPPAGE_DISPLAY)
{
	minSpeed = theApp.GetOptions().minSpeed;
	maxSpeed = theApp.GetOptions().maxSpeed;

	minDensity = theApp.GetOptions().minDensity;
	maxDensity = theApp.GetOptions().maxDensity;

	minVorticity = theApp.GetOptions().minVorticity;
	maxVorticity = theApp.GetOptions().maxVorticity;

	chartColors = static_cast<int>(theApp.GetOptions().chartColors);
	resultsType = theApp.GetOptions().resultsType;

	zoom = (theApp.GetOptions().zoom == 1 ? 0 : 1);
}

void CDisplayPropPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, obstaclesColorButton);

	DDX_Control(pDX, IDC_EDIT1, minDensityEdit);
	DDX_Control(pDX, IDC_EDIT3, maxDensityEdit);
	DDX_Control(pDX, IDC_EDIT4, minSpeedEdit);
	DDX_Control(pDX, IDC_EDIT5, maxSpeedEdit);
	DDX_Control(pDX, IDC_EDIT6, minVorticityEdit);
	DDX_Control(pDX, IDC_EDIT7, maxVorticityEdit);

	DDX_Text(pDX, IDC_EDIT1, minDensity);
	DDX_Text(pDX, IDC_EDIT3, maxDensity);
	DDX_Text(pDX, IDC_EDIT4, minSpeed);
	DDX_Text(pDX, IDC_EDIT5, maxSpeed);
	DDX_Text(pDX, IDC_EDIT6, minVorticity);
	DDX_Text(pDX, IDC_EDIT7, maxVorticity);

	DDX_Radio(pDX, IDC_RADIO1, resultsType);
	DDX_Radio(pDX, IDC_RADIO4, chartColors);

	DDX_Check(pDX, IDC_CHECK1, zoom);
}


BEGIN_MESSAGE_MAP(CDisplayPropPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_RADIO1, &CDisplayPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, &CDisplayPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO3, &CDisplayPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO4, &CDisplayPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO5, &CDisplayPropPage::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_CHECK1, &CDisplayPropPage::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, &CDisplayPropPage::OnBnClickedMfccolorbutton1)
	ON_EN_CHANGE(IDC_EDIT1, &CDisplayPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT3, &CDisplayPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT4, &CDisplayPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT5, &CDisplayPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT6, &CDisplayPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT7, &CDisplayPropPage::OnEnChangeEdit)
END_MESSAGE_MAP()


// CDisplayPropPage message handlers


BOOL CDisplayPropPage::OnApply()
{
	UpdateData();
	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


BOOL CDisplayPropPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	minSpeedEdit.allowNegative = false;
	maxSpeedEdit.allowNegative = false;

	minDensityEdit.allowNegative = false;
	maxDensityEdit.allowNegative = false;

	minVorticityEdit.allowNegative = true;
	maxVorticityEdit.allowNegative = true;

	obstaclesColorButton.EnableOtherButton(_T("More Colors..."));
	obstaclesColorButton.SetColor(theApp.GetOptions().obstaclesColor);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDisplayPropPage::ApplyValues()
{
	theApp.GetOptions().minSpeed = minSpeed;
	theApp.GetOptions().maxSpeed = maxSpeed;

	theApp.GetOptions().minDensity = minDensity;
	theApp.GetOptions().maxDensity = maxDensity;

	theApp.GetOptions().minVorticity = minVorticity;
	theApp.GetOptions().maxVorticity = maxVorticity;

	theApp.GetOptions().obstaclesColor = obstaclesColorButton.GetColor();

	theApp.GetOptions().chartColors = (Options::ChartColors)chartColors;
	theApp.GetOptions().resultsType = resultsType;

	theApp.GetOptions().zoom = (zoom == 1 ? 2 : 1);

	theApp.GetOptions().Save();

	CLatticeBoltzmannDoc* doc = dynamic_cast<CLatticeBoltzmannDoc*>(dynamic_cast<CMainFrame*>(theApp.m_pMainWnd)->GetActiveDocument());
	if (doc)
	{
		std::lock_guard<std::mutex> lock(doc->lattice.resMutex);
		doc->lattice.resultsType = (LatticeBoltzmann::Lattice::ResultsType)resultsType;
	}
}


void CDisplayPropPage::OnBnClickedRadio()
{
	SetModified();
}



void CDisplayPropPage::OnBnClickedCheck1()
{
	SetModified();
}


void CDisplayPropPage::OnBnClickedMfccolorbutton1()
{
	SetModified();
}


void CDisplayPropPage::OnEnChangeEdit()
{
	SetModified();
}


