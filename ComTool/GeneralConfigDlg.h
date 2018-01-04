#pragma once

// GeneralConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneralConfigDlg dialog

class CGeneralConfigDlg : public CBCGPDialog
{
	DECLARE_DYNCREATE(CGeneralConfigDlg)

// Construction
public:
	CGeneralConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBackStagePageInfo)
	enum { IDD = IDD_GENERAL_CONFIG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackStagePageInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBackStagePageInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString	m_strDocName;
	CString	m_strPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

