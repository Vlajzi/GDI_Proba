
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

vector<CPoint> CGDIProbaView::OdrediPoligon(CPoint start, int size, int n)
{
	vector<CPoint> rez;

	int r = size / 2;


	CPoint centar = { start.x + r,start.y + r };

	for (int i = 0; i < n; i++)
	{
		long x = centar.x + r * cos(2.0 * M_PI * i / (double)n);
		long y = centar.y + r * sin(2.0 * M_PI * i / (double)n);
		rez.push_back({ x , y });
	}

	return rez;
}

void CGDIProbaView::PtRectCig(CDC* pDC, int with, int height, int thicknes, COLORREF boja, int count)
{
	pDC->Rectangle(0, 0, with, height);

	CPen cetka(PS_GEOMETRIC, thicknes, boja);

	count = 8;



	CPoint start;
	start.x = 0;
	start.y = -with/2;
	vector<CPoint> tacke = OdrediPoligon(start, with, count);
	//reverse(tacke.begin(), tacke.end());
	rotate(tacke.begin(), tacke.begin() + count/2, tacke.end());


	CPen* oldBrush = pDC->SelectObject(&cetka);

	for (int i = 0; i < height; i += with/4)
	{
		pDC->Polyline(&tacke[0], count/2+1);
		for (int j = 0; j < count / 2 + 1; j++)
		{
			tacke[j].y += with/4;
		}
	}

	pDC->SelectObject(oldBrush);
}


void CGDIProbaView::CreatePattern(CDC* pDC, int with, int height, int type,int thicnes, COLORREF boja)
{
	CBitmap map;
	map.CreateBitmap(with, height, 1, 32, NULL);

	CBitmap* oldBmp = pDC->SelectObject(&map);//ovo treba van

	CPoint centar = { with / 2,height / 2 };

	CPen olovka(PS_COSMETIC,thicnes,boja);

	CPen* oldPen = pDC->SelectObject(&olovka);

	
	pDC->Rectangle(0, 0, with, height);

	CBitmap bit;
	int tmp = thicnes * 5;
	bit.CreateBitmap(tmp, tmp, 1, 32, NULL);

	CDC* tmpDC = new CDC();
	tmpDC->CreateCompatibleDC(NULL);

	CBitmap* oldBmpS = tmpDC->SelectObject(&bit);

	tmpDC->Rectangle(-5, -5, tmp + 5, tmp + 5);



	CPen cetka(PS_GEOMETRIC, thicnes, boja);

	//centar = { with / 2,height / 2 };

	CPen* oldBrush = tmpDC->SelectObject(&cetka);

	tmpDC->Ellipse(0, 0, tmp, tmp);//

	int size = tmp * tmp;

	BITMAP test;
	
	
	WORD* matricaSrc = (WORD*)calloc(size, sizeof(WORD));
	WORD* matricaDst = (WORD*)calloc(height * with, sizeof(WORD));

	//BYTE* matricaSrc = (BYTE*)GlobalAlloc(GPTR, size);
	//BYTE* matricaDst = (BYTE*)GlobalAlloc(GPTR, with*height);
	bit.GetBitmapBits(size * 4, matricaSrc);

	for (int i = 0; i < with * height; i += size)
	{

		memcpy(&matricaDst[i], matricaSrc, size);
	}

	CBitmap nova;
	int b = nova.CreateBitmap(with, height, 1, 32, NULL);
	free(matricaSrc);
	b = nova.SetBitmapBits(with * height * 4, (BYTE*)matricaDst);

	tmpDC->SelectObject(&nova);

	pDC->BitBlt(0, 0, with, height, tmpDC, 0, 0, SRCCOPY);


	


	/*for (int r = with-thicnes; r > 0; r -= 2 * thicnes)
	{
		pDC->Ellipse(centar.x - r, centar.y - r, centar.x + r, centar.y + r);
	}*/

	//krugovi
	//PtRectCig(pDC, with, height, 30, boja, 4);

	pDC->SelectObject(oldPen);
	olovka.DeleteObject();
	tmpDC->SelectObject(&oldBmpS);
	tmpDC->SelectObject(oldBrush);///////s
	tmpDC->DeleteDC();
	delete tmpDC;
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

	//reasize!!!

	CDC* memA = new CDC();
	memA->CreateCompatibleDC(NULL);
	CDC* memB = new CDC();
	memB->CreateCompatibleDC(NULL);

	CBitmap* scAoldBmp = memA->SelectObject(&bmpImageS);
	CBitmap* scBoldBmp = memB->SelectObject(&bmpImage);

	memB->StretchBlt(0, 0, 400, 1300, memA, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

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


	CDC* testDC = new CDC();
	testDC->CreateCompatibleDC(NULL);

	CreatePattern(testDC, 800, 500, 0, 20, RGB(0, 255, 0));

	srcDC->BitBlt(0, 0, 800, 500, testDC, 0, 0, SRCAND);


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

	XFORM def;
	def.eDx = 0;
	def.eDy = 0;
	def.eM11 = 1;
	def.eM12 = 0;
	def.eM21 = 0;
	def.eM22 = 1;

	

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
	a.eDx = 0;
	a.eDy = 0;
	a.eM11 = 1;
	a.eM21 = 0;
	a.eM12 = 0;
	a.eM22 = 1;
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
	;

	int prevMode = SetGraphicsMode(memDC->m_hDC, GM_ADVANCED);
	memDC->SetWorldTransform(&def);
	for (int i = 0; i <= 2000; i = i + 100)
	{
		memDC->MoveTo(i, 0);
		memDC->LineTo(i, 2000);
		memDC->MoveTo(0, i);
		memDC->LineTo(2000, i);
	}
	XFORM staro;
	memDC->GetWorldTransform(&staro);

	SetWorldTransform(memDC->m_hDC, &a);
	//memDC->SetWorldTransform(&a);
	RemoveGreean(memDC, slika);
	
	XFORM Xform;
	Xform.eM11 = (FLOAT)1;
	Xform.eM12 = (FLOAT)0;
	Xform.eM21 = (FLOAT)0;
	Xform.eM22 = (FLOAT)1;
	Xform.eDx = (FLOAT)0.0;
	Xform.eDy = (FLOAT)0.0;

	SetWorldTransform(memDC->m_hDC, &Xform);
	//pDC->BitBlt(0,0,2000,2000,memDC,0,0,SRCCOPY);

	CDC* testDC = new CDC();
	testDC->CreateCompatibleDC(NULL);

	CreatePattern(testDC, 400, 1300, 0, 20, RGB(0, 255, 0));

	pDC->BitBlt(0, 0, 400, 1300, testDC, 0, 0, SRCAND);
	DWORD dw = GetLastError();

	
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
