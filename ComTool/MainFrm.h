// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CComToolView;

class CMainFrame : public CBCGPFrameWnd
{
	friend class CComToolView;

	DECLARE_DYNCREATE(CMainFrame)
	
protected: // create from serialization only
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnChangeRibbonCategory(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	
	//! Tx
	afx_msg void OnButtonTxData();
	afx_msg void OnUpdateButtonTxData(CCmdUI* pCmdUI);
	afx_msg void OnButtonSendData();
	afx_msg void OnUpdateButtonSendData(CCmdUI* pCmdUI);

	//! control
	afx_msg void OnButtonBoardTurn();
	afx_msg void OnButtonBoardPlay();
	afx_msg void OnUpdateButtonBoardPlay(CCmdUI* pCmdUI);

	//! StatusBar
	afx_msg void OnUpdateLabelTxCount(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelRxCount(CCmdUI* pCmdUI);
	afx_msg void OnButtonResetCount();
	afx_msg void OnUpdateLabelResend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgressResend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelResendTimeRate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelResendTime(CCmdUI* pCmdUI);
	afx_msg void OnSliderResendTime();
	afx_msg void OnUpdateSliderResendTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelResendTimeValue(CCmdUI* pCmdUI);
	
	DECLARE_MESSAGE_MAP()

protected:  // control bar embedded members
	void ShowOptions (int nPage);

	BOOL CreateRibbonBar ();
	BOOL CreateStatusBar ();

public:
	void SetStatusBarPaneText(UINT nIDC, LPCTSTR text, BOOL reDraw = TRUE);
	void SetStatusBarCtrlVisible(UINT nIDC, BOOL visible = TRUE, BOOL reDraw = TRUE);
	void SetStatusBarProgressValue(UINT nIDC, int value, BOOL reDraw = TRUE);
	void SetStatusBarProgressRange(UINT nIDC, int nMin, int nMax);
	void SetStatusBarSliderValue(UINT nIDC, int value, BOOL reDraw = TRUE);
	void SetStatusBarSliderRange(UINT nIDC, int nMin, int nMax);

	void SetView(CComToolView* value) { ASSERT(value != NULL); m_wndView = value; }
	CComToolView* GetView() { ASSERT(m_wndView != NULL); return m_wndView; }

protected:  // control bar embedded members
	CBCGPRibbonStatusBar	m_wndStatusBar;
	CBCGPRibbonBar			m_wndRibbonBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;

	UINT					m_nAppLook;

private:
	CComToolView*		m_wndView;		// 

	//! 
	BOOL					m_bBoardTurnOn;
	BOOL					m_bBoardRun;
	BOOL					m_enableButtonTxData;

	enum {
		TIMER_ID_SYS_TIME = 1,				// 系统时间显示定时器信息
		TIMER_ELAPSE_SYS_TIME = 1000,		// 1000ms/times

		TIMER_ID_RESEND_TIME_RATE = 2,		// 单次发送数据帧失败时，重发时间进度刷新
		TIMER_ELAPSE_RESEND_TIME_RATE = 50	// 50ms/times
	};
};
