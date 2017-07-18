// ComputationPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "LatticeBoltzmann.h"
#include "ComputationPropPage.h"
#include "afxdialogex.h"

#include "Options.h"
#include "LatticeBoltzmann.h"


// CComputationPropPage dialog

IMPLEMENT_DYNAMIC(CComputationPropPage, CMFCPropertyPage)

CComputationPropPage::CComputationPropPage()
	: CMFCPropertyPage(IDD_PROPPAGE_COMPUTATION)
{
	numThreads = theApp.options.numThreads;
	numSteps = theApp.options.refreshSteps;
}

CComputationPropPage::~CComputationPropPage()
{
}

void CComputationPropPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, numThreads);
	DDX_Text(pDX, IDC_EDIT2, numSteps);
}


BEGIN_MESSAGE_MAP(CComputationPropPage, CMFCPropertyPage)
	ON_EN_CHANGE(IDC_EDIT1, &CComputationPropPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, &CComputationPropPage::OnEnChangeEdit)
END_MESSAGE_MAP()


// CComputationPropPage message handlers


BOOL CComputationPropPage::OnApply()
{
	UpdateData();
	ApplyValues();

	return CMFCPropertyPage::OnApply();
}




void CComputationPropPage::ApplyValues()
{
	theApp.options.numThreads = numThreads;
	theApp.options.refreshSteps = numSteps;

	theApp.options.Save();
}


void CComputationPropPage::OnEnChangeEdit()
{
	SetModified();
}


