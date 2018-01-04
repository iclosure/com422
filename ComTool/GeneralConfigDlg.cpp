// CGeneralConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ComTool.h"
#include "GeneralConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralConfigDlg dialog

IMPLEMENT_DYNCREATE(CGeneralConfigDlg, CBCGPDialog)

CGeneralConfigDlg::CGeneralConfigDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CGeneralConfigDlg::IDD, pParent)
{
}

void CGeneralConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGeneralConfigDlg, CBCGPDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackStagePageInfo message handlers

BOOL CGeneralConfigDlg::OnInitDialog() 
{
	CBCGPDialog::OnInitDialog();

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	
	CFrameWnd* pFrame = pMainFrame->GetActiveFrame();
	if (pFrame != NULL)
	{
		CDocument* pDoc = pFrame->GetActiveDocument();
		if (pDoc != NULL)
		{
			m_strDocName = pDoc->GetTitle();
			m_strPath = pDoc->GetPathName();
		}
	}

	((CComboBox*)GetDlgItem(IDC_COMBO_LANGUAGE))->SetCurSel(0);

	SetBackgroundColor(RGB(200, 200, 200));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralConfigDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectPath;

	GetDlgItem(IDC_PATH_LABEL)->GetWindowRect(rectPath);
	ScreenToClient(rectPath);

	CRect rectInfo;

	GetDlgItem(IDC_INFO_LABEL)->GetWindowRect(rectInfo);
	ScreenToClient(rectInfo);

	CFont* pOldFont = dc.SelectObject(&globalData.fontCaption);
	dc.SetTextColor(globalData.clrBtnText);
	dc.SetBkMode(TRANSPARENT);
	
	CString strInfo = _T("COM422 通信测试工具");
	dc.DrawText(strInfo, rectInfo, DT_SINGLELINE | DT_END_ELLIPSIS);

	int nTextHeight = dc.DrawText(m_strDocName, rectPath, DT_SINGLELINE | DT_END_ELLIPSIS);

	rectPath.top += nTextHeight + 5;

	dc.SelectObject(&globalData.fontRegular);

	dc.DrawText(m_strPath, rectPath, DT_SINGLELINE | DT_END_ELLIPSIS);

	dc.SelectObject(pOldFont);
}
