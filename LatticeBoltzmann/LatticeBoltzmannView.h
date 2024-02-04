
// LatticeBoltzmannView.h : interface of the CLatticeBoltzmannView class
//

#pragma once

#include "MemoryBitmap.h"


class CLatticeBoltzmannView : public CView
{
protected: // create from serialization only
	CLatticeBoltzmannView() = default;
	DECLARE_DYNCREATE(CLatticeBoltzmannView)

public:
	CLatticeBoltzmannDoc* GetDocument() const;
	// Operations

private:
	// Attributes
	// Overrides
	void OnDraw(CDC* pDC) override;  // overridden to draw this view
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

// Implementation
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	UINT_PTR timer = 0;

	MemoryBitmap memoryBitmap;

// Generated message map functions
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LatticeBoltzmannView.cpp
inline CLatticeBoltzmannDoc* CLatticeBoltzmannView::GetDocument() const
   { return reinterpret_cast<CLatticeBoltzmannDoc*>(m_pDocument); }
#endif

