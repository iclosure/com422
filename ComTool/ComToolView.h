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
	CBCGPStaticLayout*	m_layout;			// �Զ����ֹ�����
	CRect				m_rcWndCurSel;		// ���е�ǰ��Ի����λ�úͳߴ�
	CSkinDialog*		m_pCurSelDlg;		// ���е�ǰ��Ի���
	CRect				m_rcWndOutput;		// ��Ϣ����Ի����λ�úͳߴ�
	COutputDlg*			m_pOutputDlg;		// ��Ϣ����Ի���
	BOOL				m_bEnableOutput;	// ������ʾ��Ϣ����Ի���
	BOOL				m_bEnFlush;			// ����ˢ�¿�����
	BOOL				m_bEnFlushBn;		// �Ƿ�ʹ������ˢ�¿���
	BOOL				m_bEnQuitBn;		// �Ƿ�ʹ�ܷ��ؿ���
	CMainFrame*			m_wndFrame;			// CMainFrame object
};

#ifndef _DEBUG  // debug version in ComToolView.cpp
inline CComToolDoc* CComToolView::GetDocument() const
   { return reinterpret_cast<CComToolDoc*>(m_pDocument); }
#endif

