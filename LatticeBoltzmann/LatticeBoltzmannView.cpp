
// LatticeBoltzmannView.cpp : implementation of the CLatticeBoltzmannView class
//

#include "stdafx.h"

#include "LatticeBoltzmann.h"

#include "LatticeBoltzmannDoc.h"
#include "LatticeBoltzmannView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLatticeBoltzmannView

IMPLEMENT_DYNCREATE(CLatticeBoltzmannView, CView)

BEGIN_MESSAGE_MAP(CLatticeBoltzmannView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLatticeBoltzmannView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CLatticeBoltzmannView construction/destruction

CLatticeBoltzmannView::CLatticeBoltzmannView()
	: timer(NULL)
{
	// TODO: add construction code here

}

CLatticeBoltzmannView::~CLatticeBoltzmannView()
{
}

BOOL CLatticeBoltzmannView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLatticeBoltzmannView drawing

void CLatticeBoltzmannView::OnDraw(CDC* pDC)
{
	CLatticeBoltzmannDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(rect);

	int width = 0;
	int height = 0;

	{
		std::lock_guard<std::mutex> lock(pDoc->lattice.resMutex);

		memoryBitmap.SetMatrix(pDoc->lattice.results, static_cast<int>(pDoc->lattice.resultsType), pDoc->lattice.latticeObstacles);

		width = static_cast<int>(pDoc->lattice.results.cols());
		height = static_cast<int>(pDoc->lattice.results.rows());
	}

	width *= theApp.options.zoom;
	height *= theApp.options.zoom;

	CRect rct;
	rct.top = rect.top + rect.Height() / 2 - height / 2;
	rct.left = rect.left + rect.Width() / 2 - width / 2;
	rct.right = rct.left + width;
	rct.bottom = rct.top + height;

	memoryBitmap.Draw(pDC, rct);

	CBrush whiteBrush(RGB(255,255,255));
	CRect paintRect;

	paintRect.top = rect.top;
	paintRect.bottom = rct.top;
	paintRect.left = rect.left;
	paintRect.right = rect.right;
	pDC->FillRect(paintRect, &whiteBrush);

	paintRect.top = rct.bottom;
	paintRect.bottom = rect.bottom;
	pDC->FillRect(paintRect, &whiteBrush);

	paintRect.right = rct.left;
	paintRect.top = rct.top;
	paintRect.bottom = rct.bottom;
	pDC->FillRect(paintRect, &whiteBrush);

	paintRect.left = rct.right;
	paintRect.right = rect.right;
	pDC->FillRect(paintRect, &whiteBrush);
}


// CLatticeBoltzmannView printing


void CLatticeBoltzmannView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLatticeBoltzmannView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLatticeBoltzmannView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLatticeBoltzmannView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CLatticeBoltzmannView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLatticeBoltzmannView::OnContextMenu(CWnd* /* pWnd */, CPoint /*point*/)
{
#ifndef SHARED_HANDLERS
#endif
}


// CLatticeBoltzmannView diagnostics

#ifdef _DEBUG
void CLatticeBoltzmannView::AssertValid() const
{
	CView::AssertValid();
}

void CLatticeBoltzmannView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLatticeBoltzmannDoc* CLatticeBoltzmannView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLatticeBoltzmannDoc)));
	return dynamic_cast<CLatticeBoltzmannDoc*>(m_pDocument);
}
#endif //_DEBUG


// CLatticeBoltzmannView message handlers


int CLatticeBoltzmannView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	timer = SetTimer(1, 66, NULL);

	return 0;
}


void CLatticeBoltzmannView::OnDestroy()
{
	KillTimer(timer);

	CView::OnDestroy();	
}



void CLatticeBoltzmannView::OnTimer(UINT_PTR nIDEvent)
{
	CView::OnTimer(nIDEvent);

	Invalidate();
}


BOOL CLatticeBoltzmannView::OnEraseBkgnd(CDC* pDC)
{
	if (pDC->IsPrinting())
		return CView::OnEraseBkgnd(pDC);

	return TRUE;
}
