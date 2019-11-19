
// GDI_ProbaView.cpp : implementation of the CGDIProbaView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GDI_Proba.h"
#endif

#include "GDI_ProbaDoc.h"
#include "GDI_ProbaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define _USE_MATH_DEFINES
#include <math.h>

// CGDIProbaView

IMPLEMENT_DYNCREATE(CGDIProbaView, CView)

BEGIN_MESSAGE_MAP(CGDIProbaView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGDIProbaView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CGDIProbaView construction/destruction

CGDIProbaView::CGDIProbaView() noexcept
{
	// TODO: add construction code here

}

CGDIProbaView::~CGDIProbaView()
{
}

BOOL CGDIProbaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGDIProbaView drawing

CBitmap* CGDIProbaView::RemoveGreean(CDC* pDC, HBITMAP slika)
{
	CBitmap bmpImageS;
	bmpImageS.m_hObject = slika;
	CBitmap bmpMask;
	BITMAP bm;
	bmpImageS.GetBitmap(&bm);
	CBitmap bmpImage;
	bmpImage.CreateBitmap(800, 500, 1, 32, NULL);

	//reasize

	CDC* memA = new CDC();
	memA->CreateCompatibleDC(NULL);
	CDC* memB = new CDC();
	memB->CreateCompatibleDC(NULL);

	CBitmap* scAoldBmp = memA->SelectObject(&bmpImageS);
	CBitmap* scBoldBmp = memB->SelectObject(&bmpImage);

	memB->StretchBlt(0, 0, 800, 500, memA, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	memA->SelectObject(scAoldBmp);
	memB->SelectObject(scBoldBmp);
	memA->DeleteDC();
	memB->DeleteDC();
	delete memA;
	delete memB;


	bmpImage.GetBitmap(&bm);
	bmpMask.CreateBitmap(bm.bmWidth,bm.bmHeight, 1, 1, NULL);

	

	CDC* srcDC = new CDC();
	srcDC->CreateCompatibleDC(NULL);
	CDC* dstDC = new CDC();
	dstDC->CreateCompatibleDC(NULL);


	CBitmap* oldSrcBmp = srcDC->SelectObject(&bmpImage);
	CBitmap* oldDstBmp = dstDC->SelectObject(&bmpMask);


	COLORREF clrTopLeft = srcDC->GetPixel(0, 0);
	COLORREF clrSaveBk = srcDC->SetBkColor(clrTopLeft);
	
	//srcDC->StretchBlt(0, 0, 400, 250, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	/*bm.bmWidth = 400;
	bm.bmHeight = 250;*/

	dstDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, srcDC, 0, 0, SRCCOPY);


	COLORREF clrSaveTextDst = srcDC->SetTextColor(RGB(255, 255, 255));
	srcDC->SetBkColor(RGB(0, 0, 0));

	srcDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, dstDC, 0, 0, SRCAND);

	dstDC->SetTextColor(clrSaveTextDst);
	srcDC->SetBkColor(clrSaveBk);
	srcDC->SelectObject(oldSrcBmp);
	dstDC->SelectObject(oldDstBmp);
	srcDC->DeleteDC();
	delete srcDC;
	dstDC->DeleteDC();
	delete dstDC;



	CDC* MemDC = new CDC();
	MemDC->CreateCompatibleDC(NULL);
	CBitmap* bmpOldT = MemDC->SelectObject(&bmpMask);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, MemDC, 0, 0, SRCAND);

	MemDC->SelectObject(&bmpImage);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, MemDC, 0, 0, SRCPAINT);


	MemDC->SelectObject(bmpOldT);
	MemDC->DeleteDC();
	delete MemDC;
	
	return nullptr;
}

void CGDIProbaView::OnDraw(CDC* pDC)
{
	CGDIProbaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;


	

	pDC->SetGraphicsMode(GM_ADVANCED);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(2000, 2000);
	CRect ekran;

	GetClientRect(&ekran);

	long ratio1;
	long ratio2;
	long startX;
	long startY;


	if (ekran.right < ekran.bottom)
	{
		ratio1 = ekran.right;
		ratio2 = ekran.bottom;
		startX = 0;
		startY = (ratio2 - ratio1) / 2;

	}
	else
	{
		ratio1 = ekran.bottom;
		ratio2 = ekran.right;
		startX = (ratio2 - ratio1) / 2;
		startY = 0;
	}

	pDC->SetViewportExt(ratio1, ratio1);//proba za odnose

	pDC->SetViewportOrg(startX, startY);


	HBITMAP slika = (HBITMAP)LoadImage(0, _T("Base.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);


	//ZONA
	XFORM a;
	a.eDx = 200;
	a.eDy = 200;
	a.eM11 = 1;
	a.eM11 = 0;
	a.eM11 = 0;
	a.eM11 = 1;
	XFORM old;

	
	CBitmap bmpImage;
	bmpImage.m_hObject = slika;
	CBitmap bmpMask;
	BITMAP bm;
	bmpImage.GetBitmap(&bm);
	
	 /*CDC* pDCPaint;
	 pDCPaint = NULL;
	 CMemDC MemDC(*pDCPaint, this);
	 CDC* memDC = &MemDC.GetDC();*/
    
	
	

	

	

	CBitmap proba;
	proba.CreateCompatibleBitmap(pDC, 2000, 2000);

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	

	CBitmap* memOld = memDC->SelectObject(&proba);

	memDC->Rectangle(0, 0, 2005, 2005);
	
	int prevMode = SetGraphicsMode(memDC->m_hDC, GM_ADVANCED);

	for (int i = 0; i <= 2000; i = i + 100)
	{
		memDC->MoveTo(i, 0);
		memDC->LineTo(i, 2000);
		memDC->MoveTo(0, i);
		memDC->LineTo(2000, i);
	}
	XFORM staro;
	memDC->GetWorldTransform(&staro);

	//SetWorldTransform(memDC->m_hDC, &a);
	
	RemoveGreean(memDC, slika);
	

	pDC->BitBlt(0,0,2000,2000,memDC,0,0,SRCCOPY);



	memDC->SetWorldTransform(&staro);
	memDC->SelectObject(memOld);
	memDC->DeleteDC();
	delete memDC;

	
	
}


// CGDIProbaView printing


void CGDIProbaView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGDIProbaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGDIProbaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGDIProbaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CGDIProbaView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGDIProbaView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGDIProbaView diagnostics

#ifdef _DEBUG
void CGDIProbaView::AssertValid() const
{
	CView::AssertValid();
}

void CGDIProbaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGDIProbaDoc* CGDIProbaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGDIProbaDoc)));
	return (CGDIProbaDoc*)m_pDocument;
}
#endif //_DEBUG


// CGDIProbaView message handlers
