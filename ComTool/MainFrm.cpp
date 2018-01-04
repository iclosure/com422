// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ComTool.h"
#include "MainFrm.h"
#include "ComToolDoc.h"
#include "ComToolView.h"
#include "Version.h"
#include "GeneralConfigDlg.h"
#include "UserWinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(BCGM_ON_CHANGE_RIBBON_CATEGORY, &ThisClass::OnChangeRibbonCategory)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2012_DARK, &ThisClass::OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2012_DARK, &ThisClass::OnUpdateAppLook)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	// RibbonBar
	// -> Tx
	ON_COMMAND(IDC_BUTTON_TX_DATA, &ThisClass::OnButtonTxData)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_TX_DATA, &ThisClass::OnUpdateButtonTxData)
	ON_COMMAND(IDC_BUTTON_SEND, &ThisClass::OnButtonSendData)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SEND, &ThisClass::OnUpdateButtonSendData)
	// -> Rx

	// -> control
	ON_COMMAND(IDC_BUTTON_BOARD_TURN, &ThisClass::OnButtonBoardTurn)
	ON_COMMAND(IDC_BUTTON_BOARD_PLAY, &ThisClass::OnButtonBoardPlay)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_BOARD_PLAY, &ThisClass::OnUpdateButtonBoardPlay)
	//! StatusBar
	ON_UPDATE_COMMAND_UI(IDC_LABEL_TX_COUNT, &ThisClass::OnUpdateLabelTxCount)
	ON_UPDATE_COMMAND_UI(IDC_LABEL_RX_COUNT, &ThisClass::OnUpdateLabelRxCount)
	ON_COMMAND(IDC_BUTTON_RESET_COUNT, &ThisClass::OnButtonResetCount)
	ON_UPDATE_COMMAND_UI(IDC_LABEL_RESEND, &ThisClass::OnUpdateLabelResend)
	ON_UPDATE_COMMAND_UI(IDC_PROGRESS_RESEND, &ThisClass::OnUpdateProgressResend)
	ON_UPDATE_COMMAND_UI(IDC_LABEL_RESEND_TIME_RATE, &ThisClass::OnUpdateLabelResendTimeRate)
	ON_UPDATE_COMMAND_UI(IDC_LABEL_RESEND_TIME, &ThisClass::OnUpdateLabelResendTime)
	ON_COMMAND(IDC_SLIDER_RESEND_TIME, &ThisClass::OnSliderResendTime)
	ON_UPDATE_COMMAND_UI(IDC_SLIDER_RESEND_TIME, &ThisClass::OnUpdateSliderResendTime)
	ON_UPDATE_COMMAND_UI(IDC_LABEL_RESEND_TIME_VALUE, &ThisClass::OnUpdateLabelResendTimeValue)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2010_1);

	m_bBoardTurnOn = FALSE;
	m_bBoardRun = FALSE;
	m_enableButtonTxData = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		TRACE0("Failed to Create CMainFrame Object\n");
		return -1;
	}

	CString versin;
	versin.Format(_T("COM422 通信测试 V%d.%d.%d"), 
		TARGET_MAJOR_VERSION, TARGET_MINOR_VERSION, TARGET_PATCH_VERSION);
	SetTitle(versin);

	if (!CreateRibbonBar ())
	{
		TRACE0("Failed to create ribbon bar\n");
		return -1;      // fail to create
	}

	if (!CreateStatusBar ())
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);

	OnAppLook(m_nAppLook);

	//! Status bar
	SetStatusBarPaneText(IDC_LABEL_COMMU_STATUS, _T("断开"));

	SetTimer(TIMER_ID_SYS_TIME, TIMER_ELAPSE_SYS_TIME, NULL);	 // 启动系统时间显示的定时器

	// VISUAL_MANAGER
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::DestroyWindow()
{
	return CBCGPFrameWnd::DestroyWindow();
}

BOOL CMainFrame::CreateStatusBar ()
{
	if (!m_wndStatusBar.Create(this))
	{
		return FALSE;
	}

	if (!m_wndStatusBar.LoadFromXML (_T("IDR_BCGP_RIBBON_XML")))
	{
		return FALSE;
	}

	// USE_STATUS_DESIGNER

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}

#endif //_DEBUG


LRESULT CMainFrame::OnChangeRibbonCategory(WPARAM wParam, LPARAM lParam)
{
	//! 
	m_wndRibbonBar.SendMessage(BCGM_ON_CHANGE_RIBBON_CATEGORY, wParam, lParam);
	return 0;
}

// CMainFrame message handlers

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	BOOL bIsScenic = (m_nAppLook >= ID_VIEW_APPLOOK_2010_1 && m_nAppLook <= ID_VIEW_APPLOOK_2010_3) 
		|| m_nAppLook == ID_VIEW_APPLOOK_WIN7;

	m_wndRibbonBar.SetBackstageMode();
	m_wndRibbonBar.SetScenicLook(bIsScenic);
	if (m_wndRibbonBar.GetMainButton() != NULL)
	{
		m_wndRibbonBar.GetMainButton()->SetScenicText(m_nAppLook != ID_VIEW_APPLOOK_WIN7 ? _T("COM422") : NULL);
	}

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007:
	case ID_VIEW_APPLOOK_2007_1:
	case ID_VIEW_APPLOOK_2007_2:
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2007:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_2007_1:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_2007_2:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
			break;

		case ID_VIEW_APPLOOK_2007_3:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2010_1:
	case ID_VIEW_APPLOOK_2010_2:
	case ID_VIEW_APPLOOK_2010_3:
		// enable Office 2010 look:
		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2010_1:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Blue);
			break;

		case ID_VIEW_APPLOOK_2010_2:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Black);
			break;

		case ID_VIEW_APPLOOK_2010_3:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Silver);
			break;
		}
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));
		
		m_wndRibbonBar.SetScenicLook();
		m_wndRibbonBar.EnableMinimizeButton();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WIN7:
		// enable Windows look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerScenic));
		
		m_wndRibbonBar.SetScenicLook();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2008 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2010:
		// enable VS 2010 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2010));
		CBCGPDockManager::SetDockMode(BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2012_LIGHT:
	case ID_VIEW_APPLOOK_VS2012_DARK:
		// enable VS 2012 look:
		CBCGPVisualManagerVS2012::SetStyle(m_nAppLook == ID_VIEW_APPLOOK_VS2012_LIGHT ?
			CBCGPVisualManagerVS2012::VS2012_Light : CBCGPVisualManagerVS2012::VS2012_Dark);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2012));
		CBCGPDockManager::SetDockMode(BCGP_DT_SMART);
		break;
	}

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_ID_SYS_TIME:
		{
			SYSTEMTIME time;
			GetLocalTime(&time);
			CString timeStr;
			timeStr.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time.wYear, time.wMonth,
				time.wDay, time.wHour, time.wMinute, time.wSecond/*, time.wMilliseconds*/);
			SetStatusBarPaneText(IDC_LABEL_SYS_TIME, timeStr);
		}
		break;
	default:
		break;
	}

	__super::OnTimer(nIDEvent);
}

void CMainFrame::OnClose()
{
	//! 
	theApp.GetDataContainer().close();

	//! 清空计数和界面显示
	SendMessage(WM_COMMAND, IDC_BUTTON_RESET_COUNT);

	__super::OnClose();
}

void CMainFrame::OnButtonTxData()
{

}

void CMainFrame::OnUpdateButtonTxData(CCmdUI* pCmdUI)
{
	//pCmdUI->Enable(!theApp.GetDataContainer().isSendBusy());
}

void CMainFrame::OnButtonSendData()
{
	//! 
	CBCGPRibbonButton *pSendDataButton = DYNAMIC_DOWNCAST(CBCGPRibbonButton, 
		m_wndRibbonBar.FindByID(IDC_BUTTON_SEND));
	COMM_INFO& commInfo = theApp.GetComInfo();

	
}

void CMainFrame::OnUpdateButtonSendData(CCmdUI* pCmdUI)
{
	//pCmdUI->Enable(m_bBoardTurnOn && m_bBoardRun && !theApp.GetTargApiEx().isSendBusy());
}

// Turn
void CMainFrame::OnButtonBoardTurn()
{
	
}

// Start
void CMainFrame::OnButtonBoardPlay() 
{
	
}

void CMainFrame::OnUpdateButtonBoardPlay(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bBoardTurnOn);
}

//! StatusBar

void CMainFrame::OnUpdateLabelTxCount(CCmdUI* pCmdUI)
{
	//CString text;
	//text.Format(_T("%ld 个"), theApp.GetDataContainer().getCountTx());
	//pCmdUI->SetText(text);
}

void CMainFrame::OnUpdateLabelRxCount(CCmdUI* pCmdUI)
{
	//CString text;
	//text.Format(_T("%ld 个"), theApp.GetTargApiEx().getCountRx());
	//pCmdUI->SetText(text);
}

void CMainFrame::OnButtonResetCount()
{
	
}

void CMainFrame::OnUpdateLabelResend(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnUpdateProgressResend(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnUpdateLabelResendTimeRate(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnUpdateLabelResendTime(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnSliderResendTime()
{
}

void CMainFrame::OnUpdateSliderResendTime(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnUpdateLabelResendTimeValue(CCmdUI* pCmdUI)
{
}

BOOL CMainFrame::CreateRibbonBar ()
{
	m_wndRibbonBar.EnableCustomization();

	if (!m_wndRibbonBar.Create(this))
	{
		return FALSE;
	}

	if (!m_wndRibbonBar.LoadFromXML (_T("IDR_BCGP_RIBBON_XML")))
	{
		return FALSE;
	}

	CBCGPRibbonCategory* pBackstageCategory = m_wndRibbonBar.GetBackstageCategory ();

	if (pBackstageCategory != NULL)
	{
		CBCGPRibbonBackstageViewPanel* pBackstagePanel = DYNAMIC_DOWNCAST (CBCGPRibbonBackstageViewPanel, 
			pBackstageCategory->GetPanel (0));

		if (pBackstagePanel != NULL)
		{
			pBackstagePanel->AttachViewToItem (0, new CBCGPRibbonBackstageViewItemForm(CGeneralConfigDlg::IDD, 
				RUNTIME_CLASS(CGeneralConfigDlg)), FALSE);
		}
	}

	return TRUE;
}

// 设置状态栏中某一个Pane的文本
void CMainFrame::SetStatusBarPaneText(UINT nIDC, LPCTSTR text, BOOL reDraw/* = TRUE*/)
{
	ASSERT(text != NULL);
	CBCGPBaseRibbonElement* pElement = m_wndStatusBar.FindByID(nIDC);
	ASSERT(pElement != NULL);
	pElement->SetText(text);
	if ( reDraw )
	{
		m_wndStatusBar.RedrawWindow();
		m_wndStatusBar.RecalcLayout();
	}
}

// 设置状态栏中某一个控件有效
void CMainFrame::SetStatusBarCtrlVisible(UINT nIDC, BOOL visible/* = TRUE*/, BOOL reDraw/* = TRUE*/)
{
	CBCGPBaseRibbonElement* pElement = m_wndStatusBar.FindByID(nIDC);
	ASSERT(pElement != NULL);
	pElement->SetVisible(visible);
	if ( reDraw )
	{
		m_wndStatusBar.RedrawWindow();
		m_wndStatusBar.RecalcLayout();
	}
}

// 设置状态栏进度条控件值
void CMainFrame::SetStatusBarProgressValue(UINT nIDC, int value, BOOL reDraw/* = TRUE*/)
{
	CBCGPRibbonProgressBar* pProgress = DYNAMIC_DOWNCAST(CBCGPRibbonProgressBar, m_wndStatusBar.FindByID(nIDC));
	ASSERT(pProgress != NULL);
	ASSERT_VALID(pProgress);
	pProgress->SetPos(value);
	if (reDraw)
	{
		m_wndStatusBar.RedrawWindow();
		m_wndStatusBar.RecalcLayout();
	}
}

// 设置状态栏进度条控件值的范围
void CMainFrame::SetStatusBarProgressRange(UINT nIDC, int nMin, int nMax)
{
	CBCGPRibbonProgressBar* pProgress = DYNAMIC_DOWNCAST(CBCGPRibbonProgressBar, m_wndStatusBar.FindByID(nIDC));
	ASSERT(pProgress != NULL);
	ASSERT_VALID(pProgress);
	pProgress->SetRange(nMin, nMax);
}

// 设置状态栏Slider控件值
void CMainFrame::SetStatusBarSliderValue(UINT nIDC, int value, BOOL reDraw/* = TRUE*/)
{
	CBCGPRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CBCGPRibbonSlider, m_wndStatusBar.FindByID(nIDC));
	ASSERT(pSlider != NULL);
	ASSERT_VALID(pSlider);
	pSlider->SetPos(value);
	if (reDraw)
	{
		m_wndStatusBar.RedrawWindow();
		m_wndStatusBar.RecalcLayout();
	}
}

// 设置状态栏Slider控件值的范围
void CMainFrame::SetStatusBarSliderRange(UINT nIDC, int nMin, int nMax)
{
	CBCGPRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CBCGPRibbonSlider, m_wndStatusBar.FindByID(nIDC));
	ASSERT(pSlider != NULL);
	ASSERT_VALID(pSlider);
	pSlider->SetRange(nMin, nMax);
}
