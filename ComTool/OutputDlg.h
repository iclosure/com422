#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

// COutputDlg dialog

class COutputDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(COutputDlg)

public:
	COutputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutputDlg();

// Dialog Data
	enum { IDD = IDD_OUTPUT };

	enum CFM_INDEX
	{
		CFM_INDEX_START_LINE = __LINE__ + 1,
		CFM_INDEX_PATH = 0,
		CFM_INDEX_CMD,
		CFM_INDEX_ANS,
		CFM_INDEX_TIMEOUT,
		CFM_INDEX_ERROR,
		CFM_INDEX_COUNT = __LINE__ - CFM_INDEX_START_LINE
	};

	enum CHANNEL_INDEX {
		CHANNEL_INDEX_START_LINE = __LINE__ + 1,
		CHANNEL_INDEX_TX = 0,
		CHANNEL_INDEX_RX,
		CHANNEL_INDEX_COUNT = __LINE__ - CHANNEL_INDEX_START_LINE
	};

	enum { 
		TIMER_TX_ID = 1,
		TIMER_TX_PERIOD = 1,

		TIMER_RX_ID = 2,
		TIMER_RX_PERIOD = 1,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboRxDataFormat();
	afx_msg void OnBnClickedBnRxClearCache();

	afx_msg LRESULT OnUserVScrollEnd(WPARAM wParam, LPARAM lParam);		// WM_USER_VSCROLLEND
	afx_msg LRESULT	OnUserSaveTxt(WPARAM wParam, LPARAM lParam);		// WM_USER_SAVE_TXT
	afx_msg LRESULT	OnUserCleanTxt(WPARAM wParam, LPARAM lParam);		// WM_USER_CLEAN_TXT
	afx_msg LRESULT	OnUserCleanReport(WPARAM wParam, LPARAM lParam);	// WM_USER_CLEAN_REPORT
	afx_msg LRESULT	OnUserStartRecv(WPARAM wParam, LPARAM lParam);		// WM_USER_START_RECV
	afx_msg LRESULT	OnUserDispAns(WPARAM wParam, LPARAM lParam);		// WM_USER_DISP_ANS
	afx_msg LRESULT	OnUserTimeout(WPARAM wParam, LPARAM lParam);		// WM_USER_TIMEOUT
	afx_msg LRESULT	OnUserNoTimeout(WPARAM wParam, LPARAM lParam);		// WM_USER_NOTIMEOUT
	afx_msg LRESULT	OnUserAnsError(WPARAM wParam, LPARAM lParam);		// WM_USER_ANSERROR
	
public:
	void TextOut(UINT index, const char* text, CFM_INDEX cfm_index);
	void TextOut(UINT index, std::string text, CFM_INDEX cfm_index);
	void TextOut(UINT index, CString text, CFM_INDEX cfm_index);
	void LogOut(UINT index, CString text, CFM_INDEX cfm_index = CFM_INDEX_ANS);

	void DisableReportTabResizeCol(UINT index);

private:
	VOID InitializeGridCtrl();

private:
	CHARFORMAT2		m_cfms[CFM_INDEX_COUNT];			// 
	CListCtrlEx		m_listCtrl[CHANNEL_INDEX_COUNT]; 
	CRichEditCtrl	m_rEditLog[CHANNEL_INDEX_COUNT];
};
