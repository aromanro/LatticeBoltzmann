
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

public:
// Attributes
	LatticeBoltzmann::Lattice lattice;

	std::thread theThread;

// Operations
	void SetImageAndStartComputing(const CImage& image);
	void StopSimulation();
	CLatticeBoltzmannView* GetMainView();
	void OnCloseDocument() override;

// Implementation
	~CLatticeBoltzmannDoc() override;

// Generated message map functions
private:
	// Overrides
	BOOL OnNewDocument() override;
	void Serialize(CArchive& ar) override;
#ifdef SHARED_HANDLERS
	void InitializeSearchContent() override;
	void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif // SHARED_HANDLERS
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
