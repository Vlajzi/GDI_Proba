
// GDI_ProbaView.h : interface of the CGDIProbaView class
//

#pragma once
#include<vector>

using namespace std;

class CGDIProbaView : public CView
{
protected: // create from serialization only
	CGDIProbaView() noexcept;
	DECLARE_DYNCREATE(CGDIProbaView)

// Attributes
public:
	CGDIProbaDoc* GetDocument() const;

// Operations
public:
	CBitmap* RemoveGreean(CDC* pDC,HBITMAP slika);
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void CreatePattern(CDC* pDC,int with,int height,int type,int thicnes,COLORREF boja);
	void PtRectCig(CDC* pDC, int with, int height, int thicknes, COLORREF boja, int count);
	vector<CPoint> OdrediPoligon(CPoint start, int size, int n);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGDIProbaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GDI_ProbaView.cpp
inline CGDIProbaDoc* CGDIProbaView::GetDocument() const
   { return reinterpret_cast<CGDIProbaDoc*>(m_pDocument); }
#endif

