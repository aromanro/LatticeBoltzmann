
// LatticeBoltzmannDoc.h : interface of the CLatticeBoltzmannDoc class
//


#pragma once

#include "Lattice.h"
#include <thread>


class CLatticeBoltzmannView;

class CLatticeBoltzmannDoc : public CDocument
{
protected: // create from serialization only
	CLatticeBoltzmannDoc();
	DECLARE_DYNCREATE(CLatticeBoltzmannDoc)

// Attributes
public:
	LatticeBoltzmann::Lattice lattice;

	std::thread theThread;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CLatticeBoltzmannDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void SetImageAndStartComputing(CImage& image);
	void StopSimulation();
	CLatticeBoltzmannView* GetMainView();
	virtual void OnCloseDocument();
};
