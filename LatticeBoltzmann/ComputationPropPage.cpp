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
	numThreads = theApp.GetOptions().numThreads;
	numSteps = theApp.GetOptions().refreshSteps;
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




void CComputationPropPage::ApplyValues() const
{
	theApp.GetOptions().numThreads = numThreads;
	theApp.GetOptions().refreshSteps = numSteps;

	theApp.GetOptions().Save();
}


void CComputationPropPage::OnEnChangeEdit()
{
	SetModified();
}


