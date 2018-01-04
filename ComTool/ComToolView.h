// ComToolView.h : interface of the CComToolView class
//


#pragma once

#include "OutputDlg.h"

class CMainFrame;

class CComToolView : public CView
{
protected: // create from serialization only
	CComToolView();
	DECLARE_DYNCREATE(CComToolView)

// Attributes
public:
	CComToolDoc* GetDocument() const;

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL DestroyWindow();

// Implementation
public:
	virtual ~CComToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnChangeRibbonCategory(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	CSkinDialog* const GetCurSelDlg() const 
	{ 
		ASSERT(m_pCurSelDlg != NULL); 
		ASSERT_VALID(m_pCurSelDlg); 
		return m_pCurSelDlg; 
	}

	COutputDlg* const GetOutputDlg()
	{
		ASSERT(m_pOutputDlg != NULL); 
		ASSERT_VALID(m_pOutputDlg); 
		return m_pOutputDlg; 
	}

private:
	CBCGPStaticLayout*	m_layout;			// 自动布局管理器
	CRect				m_rcWndCurSel;		// 视中当前活动对话框的位置和尺寸
	CSkinDialog*		m_pCurSelDlg;		// 视中当前活动对话框
	CRect				m_rcWndOutput;		// 信息输出对话框的位置和尺寸
	COutputDlg*			m_pOutputDlg;		// 信息输出对话框
	BOOL				m_bEnableOutput;	// 允许显示信息输出对话框？
	BOOL				m_bEnFlush;			// 数据刷新开关量
	BOOL				m_bEnFlushBn;		// 是否使能数据刷新开关
	BOOL				m_bEnQuitBn;		// 是否使能返回开关
	CMainFrame*			m_wndFrame;			// CMainFrame object
};

#ifndef _DEBUG  // debug version in ComToolView.cpp
inline CComToolDoc* CComToolView::GetDocument() const
   { return reinterpret_cast<CComToolDoc*>(m_pDocument); }
#endif

