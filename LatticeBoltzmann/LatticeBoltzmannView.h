
// LatticeBoltzmannView.h : interface of the CLatticeBoltzmannView class
//

#pragma once

#include "MemoryBitmap.h"


class CLatticeBoltzmannView : public CView
{
protected: // create from serialization only
	CLatticeBoltzmannView();
	DECLARE_DYNCREATE(CLatticeBoltzmannView)

// Attributes
public:
	CLatticeBoltzmannDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLatticeBoltzmannView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	UINT_PTR timer;

	MemoryBitmap memoryBitmap;

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in LatticeBoltzmannView.cpp
inline CLatticeBoltzmannDoc* CLatticeBoltzmannView::GetDocument() const
   { return reinterpret_cast<CLatticeBoltzmannDoc*>(m_pDocument); }
#endif

