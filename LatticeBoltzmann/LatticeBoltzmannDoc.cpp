
// LatticeBoltzmannDoc.cpp : implementation of the CLatticeBoltzmannDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LatticeBoltzmann.h"
#endif

#include "LatticeBoltzmannDoc.h"
#include "LatticeBoltzmannView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLatticeBoltzmannDoc

IMPLEMENT_DYNCREATE(CLatticeBoltzmannDoc, CDocument)

BEGIN_MESSAGE_MAP(CLatticeBoltzmannDoc, CDocument)
END_MESSAGE_MAP()


// CLatticeBoltzmannDoc construction/destruction

CLatticeBoltzmannDoc::CLatticeBoltzmannDoc()
{
	// TODO: add one-time construction code here
}

CLatticeBoltzmannDoc::~CLatticeBoltzmannDoc()
{
	StopSimulation();
}

BOOL CLatticeBoltzmannDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CLatticeBoltzmannDoc serialization

void CLatticeBoltzmannDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CLatticeBoltzmannDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CLatticeBoltzmannDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CLatticeBoltzmannDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLatticeBoltzmannDoc diagnostics

#ifdef _DEBUG
void CLatticeBoltzmannDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLatticeBoltzmannDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLatticeBoltzmannDoc commands


void CLatticeBoltzmannDoc::SetImageAndStartComputing(const CImage& image)
{
	StopSimulation();

	lattice.latticeObstacles = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, LatticeBoltzmann::Lattice::DataOrder>(image.GetHeight(), image.GetWidth());

	const COLORREF freeColor = image.GetPixel(0, 0);

	for (int j = 0; j < image.GetWidth(); ++j)
		for (int i = 0; i < image.GetHeight(); ++i)
		{
			if (freeColor != image.GetPixel(j, i))
				lattice.latticeObstacles(i, j) = true;
			else 
				lattice.latticeObstacles(i, j) = false;
		}

	// set the other parameters from options
	lattice.numThreads = std::min<unsigned int>(theApp.options.numThreads, static_cast<unsigned int>(image.GetWidth()/4)); // at least 4 columns / thread, no matter what
	lattice.refreshSteps = theApp.options.refreshSteps;

	lattice.resultsType = static_cast<LatticeBoltzmann::Lattice::ResultsType>(theApp.options.resultsType);
	lattice.boundaryConditions = static_cast<LatticeBoltzmann::Lattice::BoundaryConditions>(theApp.options.boundaryConditions);
	lattice.accelX = theApp.options.accelX;

	lattice.useAccelX = theApp.options.useAccelX;
	lattice.inletOption = theApp.options.inletOption;
	lattice.outletOption = theApp.options.outletOption;
	lattice.inletDensity = theApp.options.inletDensity;
	lattice.outletDensity = theApp.options.outletDensity;
	lattice.inletSpeed = theApp.options.inletSpeed;
	lattice.outletSpeed = theApp.options.outletSpeed;

	lattice.tau = theApp.options.tau;

	lattice.simulate = true;
	theThread = std::thread(&LatticeBoltzmann::Lattice::Simulate, &lattice);
}


void CLatticeBoltzmannDoc::StopSimulation()
{
	lattice.simulate = false;

	if (theThread.joinable()) theThread.join();
}


CLatticeBoltzmannView* CLatticeBoltzmannDoc::GetMainView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CLatticeBoltzmannView)))
			return dynamic_cast<CLatticeBoltzmannView*>(pView);
	}

	return NULL;
}


void CLatticeBoltzmannDoc::OnCloseDocument()
{
	StopSimulation();

	CDocument::OnCloseDocument();
}
