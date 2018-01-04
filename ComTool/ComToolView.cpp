// ComToolView.cpp : implementation of the CComToolView class
//

#include "stdafx.h"
#include "ComTool.h"
#include "MainFrm.h"
#include "ComToolDoc.h"
#include "ComToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CComToolView

IMPLEMENT_DYNCREATE(CComToolView, CView)

BEGIN_MESSAGE_MAP(CComToolView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_ON_CHANGE_RIBBON_CATEGORY, &ThisClass::OnChangeRibbonCategory)
END_MESSAGE_MAP()

// CComToolView construction/destruction

CComToolView::CComToolView()
{
	m_layout		= NULL;
	m_pCurSelDlg	= NULL;
	m_pOutputDlg	= NULL;

	m_bEnFlushBn	= FALSE;		// 默认不使能刷新按钮
	m_bEnQuitBn		= FALSE;		// 默认不使能返回按钮
	m_bEnFlush		= FALSE;		// 默认禁止刷新数据
}

CComToolView::~CComToolView()
{
	if (m_layout != NULL)
	{
		delete m_layout; m_layout = NULL;
	}

	if (m_pCurSelDlg != NULL)
	{
		delete m_pCurSelDlg; m_pCurSelDlg = NULL;
	}

	if (m_pOutputDlg != NULL)
	{
		delete m_pOutputDlg; m_pOutputDlg = NULL;
	}
}

BOOL CComToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CComToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GetDocument()->SetTitle(_T("ComTool"));

	// 
	if (m_pOutputDlg == NULL)
	{
		m_pOutputDlg = new COutputDlg;
		m_pOutputDlg->Create(COutputDlg::IDD, this);

		CRect rcView; GetClientRect(rcView);
		m_pOutputDlg->MoveWindow(&rcView);
		m_pOutputDlg->ShowWindow(SW_SHOW);					// 显示
	}

	if (m_layout == NULL) {
		m_layout = new CBCGPStaticLayout;
		m_layout->Create(this);
		m_layout->AddAnchor(m_pOutputDlg->m_hWnd, LAYOUT_MOVE_NONE, LAYOUT_SIZE_BOTH);
	}

	((CMainFrame*)AfxGetMainWnd())->SetView(this);					// 初始化CMainFrm中的m_wndView
	m_wndFrame = reinterpret_cast<CMainFrame*> (AfxGetMainWnd());	// 初始化CMBITView中的m_wndFrame
}

// CComToolView drawing

void CComToolView::OnDraw(CDC* /*pDC*/)
{
	CComToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

BOOL CComToolView::DestroyWindow()
{
	if (m_pCurSelDlg != NULL)
	{
		m_pCurSelDlg->DestroyWindow();
	}

	if (m_pOutputDlg != NULL)
	{
		m_pOutputDlg->DestroyWindow();
	}

	return __super::DestroyWindow();
}


// CComToolView diagnostics

#ifdef _DEBUG
void CComToolView::AssertValid() const
{
	CView::AssertValid();
}

void CComToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CComToolDoc* CComToolView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CComToolDoc)));
	return (CComToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CComToolView message handlers


BOOL CComToolView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CComToolView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_layout != NULL)
	{
		if (m_pCurSelDlg != NULL)
		{
			m_pCurSelDlg->SetRedraw(FALSE);
		}

		if (m_pOutputDlg != NULL && m_bEnableOutput)
		{
			m_pOutputDlg->SetRedraw(FALSE);
		}

 		m_layout->AdjustLayout();

		if (m_pCurSelDlg != NULL)
		{
			m_pCurSelDlg->SetRedraw(TRUE);
			m_pCurSelDlg->RedrawWindow();
		}

		if (m_pOutputDlg != NULL && m_bEnableOutput)
		{
			m_pOutputDlg->SetRedraw(TRUE);
			m_pOutputDlg->RedrawWindow();
		}
	}
}

// WM_CHANGE_RIBBON_CATEGORY
LRESULT CComToolView::OnChangeRibbonCategory(WPARAM wParam, LPARAM lParam)
{
	CBCGPRibbonCategory* pCategory = reinterpret_cast<CBCGPRibbonBar*>(lParam)->GetActiveCategory();
// 	switch(pCategory->GetContextID())
// 	{
// 	default:break;
// 	}

	return 0;
}
